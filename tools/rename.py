#!/usr/bin/env python3
"""Names game functions and data (docs/WORKFLOW.md, "Naming"): each name with its evidence in config/symbol_addrs.txt,
from which splat writes it into the split, and every whole-word use in src/ and include/ renamed, INCLUDE_ASM and
INCLUDE_RODATA lines included. Kept only when both builds hold: the retail SHA-1, and the equivalent executable byte for
byte what it was (a rename changes no code); otherwise every file goes back as it was.

    python tools/rename.py OLD NEW --evidence "why NEW is right"
    python tools/rename.py --table FILE.tsv          one rename per line: old, new, evidence (tab-separated, # comments)
    python tools/rename.py ... --allow-equivalent-change
                                                     keeps a batch that changes the equivalent executable, and lists the
                                                     functions whose code changed (equiv.py and run_game.py then judge them)

Nothing is committed: the batch is reviewed, then committed as `refactor: name ...`.
"""
import argparse
import csv
import hashlib
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

import symbols
from match import build
from probe_compiler import compiled_words
from progress import NM, SIZED_SYMBOL
from symbols import DATA, GAME_FUNCTION, HERE, IMAGE_END

SYMBOL_ADDRS = HERE / "config" / "symbol_addrs.txt"
# A symbol's line: name, address, then splat's attributes after `//` (type:, size:, ...)
SYMBOL_LINE = re.compile(r"^(?P<name>\w+) = (?P<address>0x[0-9A-Fa-f]+);(?P<rest>.*)$")
NAMES_HEADING = ('// Names, each with its evidence on the line above it (docs/WORKFLOW.md, "Naming"). A rename replaces',
                 "// the symbol's line, never adds a second name at its address")
FUNCTION_ATTRIBUTES = " // type:func"
# The C that uses the names
SOURCE_PATTERNS = (("src", "*.c"), ("src", "*.h"), ("include", "*.h"))
# A game function is Subsystem_PascalCase, the entry point PsyQ's startup code calls is main; a variable g_PascalCase.
# No PsyQ name has one of these prefixes followed by an underscore (theirs are Cd, Pad, Ss, _card_ and the like)
FUNCTION_PREFIXES = ("Nsf", "Gool", "Obj", "Zone", "Gfx", "Snd", "Input", "Save", "Heap", "Math", "Rand")
FUNCTION_NAME = re.compile(rf"(?:{'|'.join(FUNCTION_PREFIXES)})_[A-Z][A-Za-z0-9]*")
ENTRY_POINT = "main"
DATA_NAME = re.compile(r"g_[A-Z][A-Za-z0-9]*")
# Symbols the tools reach by name: configure.py's end of .bss (BSS_END_SYMBOL), which nothing defines
PROTECTED = {f"D_{IMAGE_END:08X}"}
COLUMNS = ("old", "new", "evidence")
COMMENT = "#"
NINJA = HERE / ".venv" / "bin" / "ninja"
CONFIGURE = [sys.executable, "configure.py"]
BUILD_STEPS = [CONFIGURE, [str(NINJA)], [str(NINJA), "nonmatching"]]
NONMATCHING_EXE = HERE / "build" / "nonmatching" / "SCUS_941.54.exe"
NONMATCHING_OBJECTS = HERE / "build" / "nonmatching" / "src"
SPLIT = HERE / "asm"


def read_table(path):
    """[(old, new, evidence)] of a rename table"""
    if not path.is_file():
        sys.exit(f"{path} is missing")
    renames = []
    with path.open(newline="") as table:
        for number, row in enumerate(csv.reader(table, delimiter="\t"), start=1):
            if not row or not "".join(row).strip() or row[0].startswith(COMMENT):
                continue
            elif len(row) != len(COLUMNS):
                sys.exit(f"{path}:{number}: {len(row)} columns, not {len(COLUMNS)} ({', '.join(COLUMNS)})")
            else:
                renames.append(tuple(field.strip() for field in row))
    if not renames:
        sys.exit(f"{path} has no rename")
    return renames


def source_files():
    """The C files and headers whose text uses the names"""
    return sorted(path for folder, pattern in SOURCE_PATTERNS for path in (HERE / folder).rglob(pattern))


def whole_word(names):
    return re.compile(r"\b(" + "|".join(re.escape(name) for name in names) + r")\b")


def refusals(renames):
    """Why each rename can't be made, as lines; empty when all can"""
    found = []
    olds = [old for old, _, _ in renames]
    news = [new for _, new, _ in renames]
    texts = [path.read_text() for path in source_files()] + [SYMBOL_ADDRS.read_text()]
    used = {match.group(1) for text in texts for match in whole_word(news).finditer(text)}
    for old, new, evidence in renames:
        symbol = symbols.table().get(old)
        if olds.count(old) > 1 or news.count(new) > 1:
            found.append(f"{old} -> {new}: the batch names it twice")
        elif symbol is None:
            spelled = symbols.ADDRESS_NAME.fullmatch(old)
            named = [] if spelled is None else symbols.named_at(spelled)
            found.append(f"{old}: no symbol of the split has this name" + (f" (its name now: {named[0]})" if named else ""))
        elif old in PROTECTED:
            found.append(f"{old}: protected, the tools reach it by this name")
        elif symbol.kind not in (GAME_FUNCTION, DATA):
            found.append(f"{old}: not a game function or data ({symbol.kind})")
        elif symbol.kind == GAME_FUNCTION and not (FUNCTION_NAME.fullmatch(new) or new == ENTRY_POINT):
            found.append(f"{new}: a game function's name is Subsystem_PascalCase, the subsystem one of "
                         f"{', '.join(FUNCTION_PREFIXES)} (or {ENTRY_POINT})")
        elif symbol.kind == DATA and not DATA_NAME.fullmatch(new):
            found.append(f"{new}: a variable's name is g_PascalCase")
        elif new in symbols.table() or new in used:
            found.append(f"{new}: already used (a symbol of the split, or a word of src/, include/ or "
                         f"{SYMBOL_ADDRS.relative_to(HERE)})")
        elif not evidence or "\n" in evidence:
            found.append(f"{old}: its evidence is one line, not empty")
        elif str(HERE) in evidence or str(Path.home()) in evidence:
            found.append(f"{old}: its evidence holds a path of this machine")
        else:
            # This one can be made
            continue
    return found


def renamed_symbol_addrs(text, renames):
    """config/symbol_addrs.txt with each symbol's line renamed, its attributes kept, its evidence above it; a symbol
    splat named by itself gets a line at the end"""
    lines = text.splitlines()
    appended = []
    for old, new, evidence in renames:
        address = symbols.table()[old].address
        at_address = [index for index, line in enumerate(lines)
                      if (found := SYMBOL_LINE.match(line)) is not None and int(found.group("address"), 0) == address]
        own = [index for index in at_address if SYMBOL_LINE.match(lines[index]).group("name") == old]
        if len(at_address) > 1 or (at_address and not own):
            sys.exit(f"{SYMBOL_ADDRS.relative_to(HERE)}: {len(at_address)} lines at {address:#010x}, not one naming {old}")
        elif own:
            line = SYMBOL_LINE.match(lines[own[0]])
            lines[own[0]] = f"{new} = {line.group('address')};{line.group('rest')}"
            if symbols.ADDRESS_NAME.fullmatch(old) is None and lines[own[0] - 1].startswith("//"):
                # A name given before: its evidence, on the line above, gives way
                lines[own[0] - 1] = f"// {evidence}"
            else:
                lines.insert(own[0], f"// {evidence}")
        else:
            attributes = FUNCTION_ATTRIBUTES if symbols.table()[old].kind == GAME_FUNCTION else ""
            appended += ["", f"// {evidence}", f"{new} = 0x{address:08X};{attributes}"]
    if appended and NAMES_HEADING[0] not in lines:
        appended = ["", *NAMES_HEADING] + appended
    return "\n".join(lines + appended) + "\n"


def sha1(path):
    return hashlib.sha1(path.read_bytes()).hexdigest()


def masked(object_file, function):
    """A function's words in an object, relocated fields zeroed, and where they are"""
    words, masks = compiled_words(object_file, function)
    return [word & ~masks.get(index, 0) for index, word in enumerate(words)], sorted(masks)


def changed_functions(before, renames):
    """The functions whose code differs between the equivalent build's objects saved in before and the current ones,
    relocated fields masked. TRADEOFF: masking hides a change of the symbol a relocation names; a reference that moves
    to another symbol shows only through equiv.py"""
    new_names = {old: new for old, new, _ in renames}
    changed = []
    for saved in sorted(before.rglob("*.o")):
        current = NONMATCHING_OBJECTS / saved.relative_to(before)
        if not current.exists():
            changed.append(f"{current.relative_to(HERE)} (no longer built)")
            continue
        listings = [subprocess.run([NM, "-S", "--defined-only", str(path)], capture_output=True, text=True,
                                   check=True).stdout for path in (saved, current)]
        old_functions = {new_names.get(name, name): name for _, name in SIZED_SYMBOL.findall(listings[0])}
        new_functions = {name for _, name in SIZED_SYMBOL.findall(listings[1])}
        for name in sorted(set(old_functions) | new_functions):
            if name not in old_functions or name not in new_functions:
                changed.append(name)
            elif masked(saved, old_functions[name]) != masked(current, name):
                changed.append(name)
            else:
                # The same code
                continue
    return changed


def remove_split_files(names):
    """splat's files of these symbols, which it leaves behind once the symbol has another name"""
    for name in names:
        for path in SPLIT.rglob(f"{name}.s"):
            path.unlink()


def rename(renames, allow_equivalent_change):
    """Applies the renames and keeps them when both builds hold; exits with why otherwise, every file as it was"""
    error = build(BUILD_STEPS)
    if error is not None:
        sys.exit(f"the tree doesn't build before the rename:\n{error}")
    reasons = refusals(renames)
    if reasons:
        sys.exit("not renamed:\n" + "\n".join(reasons))
    before_sha1 = sha1(NONMATCHING_EXE)
    saved = Path(tempfile.mkdtemp(prefix="rename-"))
    shutil.copytree(NONMATCHING_OBJECTS, saved / "src")

    pattern = whole_word([old for old, _, _ in renames])
    new_names = {old: new for old, new, _ in renames}
    originals = {path: path.read_text() for path in [SYMBOL_ADDRS, *source_files()]}
    updates = {path: pattern.sub(lambda found: new_names[found.group(1)], text) for path, text in originals.items()
               if path != SYMBOL_ADDRS}
    updates[SYMBOL_ADDRS] = renamed_symbol_addrs(originals[SYMBOL_ADDRS], renames)
    originals = {path: text for path, text in originals.items() if updates[path] != text}
    try:
        for path in originals:
            path.write_text(updates[path])
        error = build(BUILD_STEPS)
        symbols.table.cache_clear()
        symbols.by_address.cache_clear()
        missing = [f"{old} -> {new}" for old, new, _ in renames if new not in symbols.table() or old in symbols.table()]
        changed = []
        if error is not None:
            # A build failed: its cause is the reason
            pass
        elif missing:
            error = f"splat's table doesn't have the new names: {', '.join(missing)}"
        elif sha1(NONMATCHING_EXE) == before_sha1:
            # A pure rename: the same code
            pass
        elif allow_equivalent_change:
            changed = changed_functions(saved / "src", renames)
        else:
            error = ("the equivalent executable changed (--allow-equivalent-change keeps it), in: "
                     + (", ".join(changed_functions(saved / "src", renames)) or "no function's code, relocations masked"))
    except BaseException:
        restore(originals, renames)
        raise
    finally:
        shutil.rmtree(saved)
    if error is not None:
        restore(originals, renames)
        sys.exit(f"not renamed, every file as it was:\n{error}")
    remove_split_files(list(new_names))
    print(f"renamed {len(renames)}: {len(originals) - 1} files of src/ and include/ and "
          f"{SYMBOL_ADDRS.relative_to(HERE)}; the retail SHA-1 holds")
    if changed:
        print("the equivalent executable changed, in: " + ", ".join(changed))
        print("judge them with tools/equiv.py (2 seeds of 2,000 trials) and tools/run_game.py")
    else:
        print(f"the equivalent executable is byte for byte what it was ({before_sha1})")


def restore(originals, renames):
    """Every file as it was, and the split again from it"""
    for path, text in originals.items():
        path.write_text(text)
    subprocess.run(CONFIGURE, cwd=HERE, capture_output=True)
    remove_split_files([new for _, new, _ in renames])


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("old", nargs="?", help="the symbol's name now")
    parser.add_argument("new", nargs="?", help="its name")
    parser.add_argument("--evidence", help="why the name is right (docs/WORKFLOW.md, \"Naming\")")
    parser.add_argument("--table", type=Path, help="a tab-separated file of renames: old, new, evidence")
    parser.add_argument("--allow-equivalent-change", action="store_true",
                        help="keep a batch that changes the equivalent executable")
    arguments = parser.parse_args()
    if arguments.table is not None and (arguments.old or arguments.new or arguments.evidence):
        sys.exit("either OLD NEW --evidence or --table, not both")
    elif arguments.table is not None:
        renames = read_table(arguments.table)
    elif arguments.old and arguments.new and arguments.evidence:
        renames = [(arguments.old, arguments.new, arguments.evidence.strip())]
    else:
        sys.exit("usage: rename.py OLD NEW --evidence TEXT, or rename.py --table FILE.tsv")
    rename(renames, arguments.allow_equivalent_change)


if __name__ == "__main__":
    main()
