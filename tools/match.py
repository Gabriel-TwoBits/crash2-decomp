#!/usr/bin/env python3
"""The matching loop (docs/WORKFLOW.md): a scratch directory per function, an agent (headless Claude Code) working in it,
and the result checked and landed by this script, never taken on the agent's word.

    python tools/match.py next                     the next function to try
    python tools/match.py prepare FUNCTION         build/match/FUNCTION/: target, m2c draft, brief, scripts, CLAUDE.md
    python tools/match.py run FUNCTION [--minutes 30] [--model MODEL]
    python tools/match.py land FUNCTION            match.c into its file in src/game/, build, commit when the SHA-1 holds
    python tools/match.py loop [--count N] [--minutes 30] [--model MODEL] [--retry-model MODEL]
    python tools/match.py report                   outcomes, models, costs, near misses

Each outcome goes into config/match/ledger.json, and each run's notes and C into config/match/notes/ (in git: what was
learned about a function outlives its scratch directory). config/match/skip.txt lists functions not to try, with why.
The agent may only read, edit inside its scratch directory and run its scratch's score.sh and permute.sh
(tools/match/CLAUDE.md has its instructions).
"""
import argparse
import json
import os
import re
import shutil
import signal
import subprocess
import sys
import time
from pathlib import Path

from probe_compiler import retail_assembly
from psx_cc import AS, ASFLAGS, HERE

SCRATCH = HERE / "build" / "match"
RECORDS = HERE / "config" / "match"
LEDGER = RECORDS / "ledger.json"
NOTES = RECORDS / "notes"
SKIP = RECORDS / "skip.txt"
TEMPLATE = HERE / "tools" / "match" / "CLAUDE.md"
SOURCES = HERE / "src" / "game"
GAME_H = HERE / "include" / "game.h"
NONMATCHINGS = HERE / "asm" / "nonmatchings" / "game"
# The game's assembly part, one file of many functions (docs/TARGET.md, "What the code holds")
GAME_ASSEMBLY = HERE / "asm" / "game"
GLABEL = re.compile(r"^glabel (\w+)$", re.MULTILINE)
M2C = HERE / "tools" / "toolchain" / "m2c" / "m2c.py"
M2C_MACROS = HERE / "tools" / "toolchain" / "m2c" / "m2c_macros.h"
ASSEMBLY_PREAMBLE = '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text\n'
# An unmatched function in its C file: INCLUDE_ASM("asm/nonmatchings/game/<file>", <function>);
INCLUDED = re.compile(r'^INCLUDE_ASM\("asm/nonmatchings/game/\w+", (\w+)\);\n', re.MULTILINE)
NONMATCHING = re.compile(r"^nonmatching (\w+), (0x[0-9A-Fa-f]+)$", re.MULTILINE)
CALL = re.compile(r"\bjal\s+(\w+)")
SYMBOL = re.compile(r"%(?:hi|lo|gp_rel)\((\w+)")
MATCHED = re.compile(r"^0 of \d+ words differ$", re.MULTILINE)
INLINE_ASSEMBLY = re.compile(r"\b(__asm__|asm)\s*\(")
HANDWRITTEN = "/* Handwritten function */"
DIFFERING = re.compile(r"^(\d+) of (\d+) words differ$", re.MULTILINE)
M2C_MACRO = re.compile(r"\bM2C_\w+")
# A top-level definition of a split data symbol (not an extern declaration), with its initialiser if it has one
DATA_DEFINITION = re.compile(r"^(?!extern\b)(?!typedef\b)[A-Za-z_][\w \t\*]*\b(D_[0-9A-F]{8})\b[^(;=]*(=[^;]*)?;\n?",
                             re.MULTILINE)
# A one-line declaration of a game function (not its definition): return type, name, parameters
PROTOTYPE = re.compile(r"^(?:extern[ \t]+)?(?P<declaration>[A-Za-z_][\w \t\*]*?\b(?P<name>func_[0-9A-F]{8})[ \t]*\([^;{\n]*\))"
                       r"[ \t]*;[ \t]*\n?", re.MULTILINE)
IDENTIFIER = re.compile(r"[A-Za-z_]\w*")
# A game function's definition in C: return type, name, parameters, then its body
DEFINITION = re.compile(r"^(?P<declaration>[A-Za-z_][\w \t\*]*?\b(?P<name>func_[0-9A-F]{8})[ \t]*\([^;{]*\))\s*\{",
                        re.MULTILINE)
# An empty parameter list declares nothing about the arguments: the compiler checks no call against it
UNCHECKED_PARAMETERS = re.compile(r"\(\s*\)$")
# PsyQ's libraries start here (config/SCUS_941.54.yaml): a func_ name from there is the SDK's, not the game's
SDK_START = 0x80046884
FUNCTION_ADDRESS_START = len("func_")
HEXADECIMAL = 16
# What a prototype in include/game.h may be written with: one using a structure of its file stays in the file
BASIC_TYPES = {"void", "char", "short", "int", "long", "unsigned", "signed", "const",
               "s8", "u8", "s16", "u16", "s32", "u32", "s64", "u64"}
ERROR_LINES = 12
# A near miss for the report: at most this share of the words differ
NEAR_MISS = 0.1
SECONDS_PER_MINUTE = 60
MILLISECONDS_PER_MINUTE = 60000
DEFAULT_MINUTES = 30
# Below this a function is a stub or a thunk: nothing to learn, and splat already writes the empty ones
MIN_SIZE = 0x20
EXECUTABLE_MODE = 0o755
AGENT_TOOLS = ["Read", "Edit", "Write", "Glob", "Grep", "Bash(./score.sh:*)", "Bash(./permute.sh:*)"]
PROMPT = ("Match the function described in CLAUDE.md and BRIEF.md in this directory, following CLAUDE.md's loop and rules. "
          "You have {minutes:g} minutes; past them the session is stopped, so keep best.c current. "
          "When you stop, leave match.c or best.c and NOTES.md as CLAUDE.md says.")


def load_ledger():
    if LEDGER.exists():
        return json.loads(LEDGER.read_text())
    return {}


def portable(text):
    """Text with this checkout's absolute path taken out: the records are in git, and paths of a machine mean nothing there"""
    return text.replace(f"{HERE}/", "").replace(str(HERE), ".")


def record(name, outcome, **details):
    """The function's latest outcome, keeping what earlier steps recorded (the run's model and cost when it lands) but not
    an earlier outcome's reason; each agent run's own details also go into its list of runs, so a retry with another
    model doesn't hide the first"""
    ledger = load_ledger()
    details = {key: portable(value) if isinstance(value, str) else value for key, value in details.items()}
    earlier = {key: value for key, value in ledger.get(name, {}).items() if key != "reason"}
    entry = {**earlier, "outcome": outcome, "time": time.strftime("%Y-%m-%d %H:%M"), **details}
    if "model" in details:
        entry["runs"] = entry.get("runs", []) + [{"outcome": outcome, **details}]
    ledger[name] = entry
    LEDGER.parent.mkdir(parents=True, exist_ok=True)
    LEDGER.write_text(json.dumps(ledger, indent=2) + "\n")


def sources():
    """The game's C files, in address order"""
    return sorted(SOURCES.glob("*.c"))


def source_of(name):
    """(C file, INCLUDE_ASM line) of a function still included from the split"""
    for source in sources():
        for match in INCLUDED.finditer(source.read_text()):
            if match.group(1) == name:
                return source, match.group(0)
    sys.exit(f"{name} isn't included from the split in {SOURCES}")


def assembly_of(name):
    """The function's file in the split, asm/nonmatchings/game/<C file>/<function>.s"""
    found = sorted(NONMATCHINGS.glob(f"*/{name}.s"))
    if len(found) != 1:
        sys.exit(f"{len(found)} files for {name} in {NONMATCHINGS}: configure.py splits the executable")
    return found[0]


def unmatched():
    """{name: size} of the game's functions still included from the split"""
    included = [name for source in sources() for name in INCLUDED.findall(source.read_text())]
    sizes = {}
    for name in included:
        assembly = assembly_of(name).read_text()
        if f"glabel {name}\n" not in assembly:
            # Data between two functions (a counter in .text): nothing to match
            continue
        match = NONMATCHING.search(assembly)
        if match is None:
            sys.exit(f"{name}.s has no nonmatching line: configure.py splits the executable")
        sizes[name] = int(match.group(2), 16)
    return sizes


def handwritten(name):
    """splat marks a function its disassembler finds not compiled (GTE code, register calling conventions)"""
    return HANDWRITTEN in assembly_of(name).read_text()


def skipped():
    """The names in config/match/skip.txt: one function per line, then why"""
    if not SKIP.exists():
        return set()
    return {line.split()[0] for line in SKIP.read_text().splitlines() if line.strip() and not line.startswith("#")}


def next_function():
    """The smallest unmatched function of MIN_SIZE or more not tried yet, handwritten and skipped ones left out"""
    tried = load_ledger()
    skip = skipped()
    candidates = sorted((size, name) for name, size in unmatched().items()
                        if size >= MIN_SIZE and name not in tried and name not in skip and not handwritten(name))
    if not candidates:
        return None
    return candidates[0][1]


def callers(name):
    """The functions whose assembly calls this one, in the split and in src/"""
    call = re.compile(rf"\bjal\s+{name}\b")
    found = [path.stem for path in sorted(NONMATCHINGS.glob("*/*.s")) if call.search(path.read_text())]
    for path in sorted(GAME_ASSEMBLY.glob("*.s")):
        # Each function of the file runs from its glabel to the next one
        pieces = GLABEL.split(path.read_text())[1:]
        found += [function for function, body in zip(pieces[::2], pieces[1::2]) if call.search(body)]
    for source in sources():
        text = INCLUDED.sub("", source.read_text())
        if re.search(rf"\b{name}\s*\(", text):
            found.append(f"(called from C in {source.relative_to(HERE)})")
    return found


def m2c_context(folder, source):
    """The declarations the function's C file has, for m2c: the file preprocessed with INCLUDE_ASM left out"""
    context = folder / "context.c"
    result = subprocess.run(["cpp", "-P", "-DM2CTX", f"-I{HERE / 'include'}", str(source)], capture_output=True, text=True)
    if result.returncode != 0:
        sys.exit(result.stderr)
    context.write_text(result.stdout)
    return context


def prepare(name):
    folder = SCRATCH / name
    if folder.exists():
        shutil.rmtree(folder)
    folder.mkdir(parents=True)
    assembly = retail_assembly(name)
    target = folder / "target.s"
    target.write_text(ASSEMBLY_PREAMBLE + assembly)
    subprocess.run([AS, *ASFLAGS, "-o", str(folder / "target.o"), str(target)], check=True, cwd=HERE)

    source = source_of(name)[0].relative_to(HERE)
    context = m2c_context(folder, HERE / source)
    draft = subprocess.run([sys.executable, str(M2C), "--target", "mipsel-gcc-c", "--valid-syntax", "--context",
                            str(context), str(target)], capture_output=True, text=True)
    shutil.copy2(M2C_MACROS, folder / "m2c_macros.h")
    (folder / "base.c").write_text('#include "common.h"\n#include "sdk.h"\n#include "game.h"\n#include "m2c_macros.h"\n\n'
                                   + draft.stdout)

    size = len(re.findall(r"/\* [0-9A-F]+ [0-9A-F]{8} [0-9A-F]{8} \*/", assembly)) * 4
    calls = sorted(set(CALL.findall(assembly)))
    symbols = sorted(set(SYMBOL.findall(assembly)))
    # The prototypes include/game.h and its file give its callees: match.c has to agree with them
    declared = [line.strip() for line in context.read_text().splitlines()
                if any(re.search(rf"\b{callee}\s*\(", line) for callee in calls) and line.rstrip().endswith(";")]
    # A callee defined in C has the signature of its definition, wherever it is
    declared += [" ".join(definition.group("declaration").split()) + ";" for path in sources()
                 for definition in DEFINITION.finditer(path.read_text()) if definition.group("name") in calls]
    declared = list(dict.fromkeys(declared))
    brief = [
        f"# {name}",
        "",
        f"- Size: {size:#x} bytes",
        f"- File: {source}",
        f"- Called by: {', '.join(callers(name)) or 'nothing found (a callback or a table entry, maybe)'}",
        f"- Calls: {', '.join(calls) or 'nothing'}",
        f"- Data: {', '.join(symbols) or 'none by name'}",
        "",
        f"## Prototypes include/game.h, {source} and the C definitions already have for its callees",
        "",
        "```c",
        "\n".join(declared) or "(none)",
        "```",
        "",
        "## m2c's draft (base.c)",
        "",
        "```c",
        draft.stdout.strip() or f"(m2c failed: {draft.stderr.strip()})",
        "```",
        "",
        "## The target (target.s)",
        "",
        "```",
        assembly.strip(),
        "```",
        "",
    ]
    (folder / "BRIEF.md").write_text("\n".join(brief))
    (folder / "CLAUDE.md").write_text(TEMPLATE.read_text().replace("$FUNCTION", name))

    python = sys.executable
    scripts = {
        "score.sh": f'#!/bin/sh\nexec "{python}" "{HERE / "tools" / "probe_compiler.py"}" "$1" {name} --diff 2.7.2\n',
        "permute.sh": f'#!/bin/sh\nexec "{python}" "{HERE / "tools" / "permute.py"}" "$1" {name} --minutes "${{2:-5}}"\n',
    }
    for script, text in scripts.items():
        (folder / script).write_text(text)
        (folder / script).chmod(EXECUTABLE_MODE)
    return folder


def score(folder, source):
    """The probe's verdict on a scratch file: (matched, its last line)"""
    result = subprocess.run([str(folder / "score.sh"), str(source)], capture_output=True, text=True)
    lines = result.stdout.strip().splitlines()
    summary = lines[-1] if lines else result.stderr.strip()
    return MATCHED.search(result.stdout) is not None, summary


def session_result(folder):
    """The agent session's own summary from its log: cost, turns, duration (an empty dict if it never finished)"""
    found = {}
    for line in (folder / "session.jsonl").read_text().splitlines():
        try:
            event = json.loads(line)
        except json.JSONDecodeError:
            continue
        if event.get("type") == "result":
            found = {"cost_usd": event.get("total_cost_usd"), "turns": event.get("num_turns"),
                     "minutes": round(event.get("duration_ms", 0) / MILLISECONDS_PER_MINUTE, 1)}
    return found


def best_attempt(folder):
    """(file, its probe summary) of the scratch's C file with the fewest differing words, kept even if the agent was stopped"""
    best = (None, "no attempt compiled")
    fewest = None
    for source in sorted(folder.glob("*.c")):
        if source.name == "context.c":
            continue
        summary = score(folder, source)[1]
        counted = DIFFERING.search(summary)
        if counted is not None and (fewest is None or int(counted.group(1)) < fewest):
            fewest = int(counted.group(1))
            best = (source.name, summary)
    return best


def keep_notes(name, folder, source):
    """The run's NOTES.md and its C (the match or the best attempt) into config/match/notes/"""
    NOTES.mkdir(parents=True, exist_ok=True)
    if (folder / "NOTES.md").exists():
        (NOTES / f"{name}.md").write_text(portable((folder / "NOTES.md").read_text()))
    if source is not None and (folder / source).exists():
        (NOTES / f"{name}.c").write_text(portable((folder / source).read_text()))


def run(name, minutes, model):
    folder = prepare(name)
    command = ["claude", "-p", PROMPT.format(minutes=minutes), "--permission-mode", "acceptEdits", "--allowedTools", *AGENT_TOOLS,
               "--output-format", "stream-json", "--verbose", "--no-session-persistence"]
    if model:
        command += ["--model", model]
    timed_out = False
    started = time.monotonic()
    with open(folder / "session.jsonl", "w") as log:
        # Its own process group, so a time limit stops the agent and whatever it started
        agent = subprocess.Popen(command, cwd=folder, stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            agent.wait(timeout=minutes * SECONDS_PER_MINUTE)
        except subprocess.TimeoutExpired:
            os.killpg(agent.pid, signal.SIGTERM)
            agent.wait()
            timed_out = True
    wall = round((time.monotonic() - started) / SECONDS_PER_MINUTE, 1)
    session = {"model": model or "default", "timed_out": timed_out, "wall_minutes": wall, **session_result(folder)}

    candidate = folder / "match.c"
    if candidate.exists():
        keep_notes(name, folder, "match.c")
        if INLINE_ASSEMBLY.search(candidate.read_text()):
            record(name, "rejected", reason="inline assembly in match.c", **session)
            return False
        matched, summary = score(folder, candidate)
        if matched:
            record(name, "matched", summary=summary, **session)
            return True
        record(name, "rejected", reason=f"match.c doesn't match: {summary}", **session)
        return False
    best, summary = best_attempt(folder)
    keep_notes(name, folder, best)
    record(name, "gave up", summary=summary, best=best, **session)
    return False


def declarator(definition):
    """A data definition's type and name, its initialiser left out: `s32 *D_8005F264`"""
    return " ".join(definition.group(0).split("=")[0].rstrip("; \n").split())


def with_prototypes(header, added):
    """include/game.h with these prototypes added to its list, which stays in address order"""
    prototypes = list(PROTOTYPE.finditer(header))
    if not prototypes:
        sys.exit(f"{GAME_H} has no list of prototypes to add to")
    start, end = prototypes[0].start(), prototypes[-1].end()
    if PROTOTYPE.sub("", header[start:end]).strip():
        sys.exit(f"{GAME_H}: something other than prototypes inside its list; add {added} by hand")
    lines = sorted([prototype.group("declaration") + ";" for prototype in prototypes] + added,
                   key=lambda line: PROTOTYPE.match(line).group("name"))
    return header[:start] + "\n".join(lines) + "\n" + header[end:]


def land(name):
    """match.c's declarations and function in place of the INCLUDE_ASM line; kept only when the build's SHA-1 holds"""
    candidate = SCRATCH / name / "match.c"
    if not candidate.exists():
        candidate = NOTES / f"{name}.c"
    if not candidate.exists():
        sys.exit(f"no match.c for {name}")
    text = candidate.read_text()
    macro = M2C_MACRO.search(text)
    if macro is not None:
        record(name, "matched, not landed", reason=f"uses m2c's {macro.group(0)}: real types needed")
        return False
    source, include = source_of(name)
    original = source.read_text()
    # The original compiler addresses a variable through $gp only in the file that defines it (docs/COMPILER.md). A
    # definition the file already has goes; an uninitialised one stays, a common the split's symbol resolves; an
    # initialised one the file doesn't have is data of another file, which can't be read through $gp from this one
    defined = {match.group(1): declarator(match) for match in DATA_DEFINITION.finditer(original)}
    for definition in DATA_DEFINITION.finditer(text):
        symbol, initialiser = definition.group(1), definition.group(2)
        if symbol in defined and defined[symbol] != declarator(definition):
            record(name, "matched, not landed", reason=f"declares {symbol} as `{declarator(definition)}`, "
                                                        f"{source.relative_to(HERE)} as `{defined[symbol]}`")
            return False
        elif symbol in defined:
            text = text.replace(definition.group(0), "")
        elif initialiser is not None:
            record(name, "matched, blocked", reason=f"defines {symbol}, which {source.relative_to(HERE)} doesn't")
            return False
        else:
            # A common: the split's symbol is the definition
            continue
    # One prototype per game function, in include/game.h: one it has goes, a new one of basic types moves there
    header = GAME_H.read_text()
    declared = {prototype.group("name") for prototype in PROTOTYPE.finditer(header)}
    added = []
    for prototype in PROTOTYPE.finditer(text):
        callee = prototype.group("name")
        if callee == name:
            # The function's own forward declaration stays with it
            continue
        elif callee in declared:
            text = text.replace(prototype.group(0), "")
        elif int(callee[FUNCTION_ADDRESS_START:], HEXADECIMAL) >= SDK_START:
            # An SDK function not identified yet: include/sdk.h's once it is
            continue
        elif UNCHECKED_PARAMETERS.search(prototype.group("declaration")):
            record(name, "matched, not landed", reason=f"`{prototype.group('declaration')}` has no parameter list")
            return False
        elif set(IDENTIFIER.findall(prototype.group("declaration"))) - {callee} <= BASIC_TYPES:
            added.append(prototype.group("declaration") + ";")
            text = text.replace(prototype.group(0), "")
        else:
            # A structure of its file in the prototype: it stays in the file
            continue
    updated_header = with_prototypes(header, added) if added else header
    # Its #include lines are the file's already; a declaration the file has above it isn't repeated
    present = set(line.strip() for line in original[:original.index(include)].splitlines())
    kept = [line for line in text.splitlines()
            if not line.startswith("#include") and not (line.strip().startswith("extern") and line.strip() in present)]
    source.write_text(original.replace(include, "\n".join(kept).strip() + "\n"))
    GAME_H.write_text(updated_header)

    steps = [[sys.executable, "configure.py"], [str(HERE / ".venv" / "bin" / "ninja")]]
    for step in steps:
        result = subprocess.run(step, cwd=HERE, capture_output=True, text=True)
        if result.returncode != 0:
            source.write_text(original)
            GAME_H.write_text(header)
            subprocess.run([sys.executable, "configure.py"], cwd=HERE, capture_output=True)
            error = "\n".join((result.stdout + result.stderr).strip().splitlines()[-ERROR_LINES:])
            record(name, "matched, not landed", reason=error)
            return False
    record(name, "landed")
    subprocess.run(["git", "add", str(source), str(GAME_H), str(RECORDS)], cwd=HERE, check=True)
    commit(f"feat: match {name}")
    return True


def commit(message):
    """A commit of what is staged, refused if any of it holds this checkout's absolute path"""
    staged = subprocess.run(["git", "diff", "--cached"], cwd=HERE, capture_output=True, text=True, check=True).stdout
    if str(HERE) in staged:
        sys.exit(f"the staged changes hold {HERE}: not committed (git diff --cached)")
    subprocess.run(["git", "commit", "-q", "-m", message], cwd=HERE, check=True)


def commit_records():
    """The ledger and notes of runs that didn't land, committed together"""
    subprocess.run(["git", "add", str(RECORDS)], cwd=HERE, check=True)
    staged = subprocess.run(["git", "diff", "--cached", "--quiet"], cwd=HERE)
    if staged.returncode != 0:
        commit("chore: record the matching runs")


def report():
    ledger = load_ledger()
    outcomes = {}
    models = {}
    near = []
    for name, entry in sorted(ledger.items()):
        outcomes[entry["outcome"]] = outcomes.get(entry["outcome"], 0) + 1
        for attempt in entry.get("runs", []):
            stats = models.setdefault(attempt["model"], {"runs": 0, "matched": 0, "cost_usd": 0.0})
            stats["runs"] += 1
            stats["matched"] += attempt["outcome"] == "matched"
            stats["cost_usd"] += attempt.get("cost_usd") or 0
        counted = DIFFERING.search(entry.get("summary", ""))
        if entry["outcome"] == "gave up" and counted is not None:
            if int(counted.group(1)) <= NEAR_MISS * int(counted.group(2)):
                near.append(f"{name} ({entry['summary']})")
        elif entry["outcome"] in ("matched, blocked", "matched, not landed"):
            # The reason's first compiler error says more than ninja's closing line
            lines = entry.get("reason", "").splitlines()
            errors = [line for line in lines if re.search(r"\.c:\d+", line)]
            if errors:
                reason = errors[0]
            elif lines:
                reason = lines[0]
            else:
                reason = ""
            near.append(f"{name} ({entry['outcome']}: {reason})")
        else:
            continue
    print("outcomes: " + ", ".join(f"{outcome} {count}" for outcome, count in sorted(outcomes.items())))
    for model, stats in sorted(models.items()):
        print(f"{model}: {stats['matched']} matched of {stats['runs']} runs, ${stats['cost_usd']:.2f}")
    print("near misses and matches waiting to land:")
    for line in near:
        print(f"  {line}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("command", choices=["next", "prepare", "run", "land", "loop", "report"])
    parser.add_argument("function", nargs="?")
    parser.add_argument("--minutes", type=float, default=DEFAULT_MINUTES, help="the agent's time limit per function")
    parser.add_argument("--model", help="the agent's model (default: Claude Code's)")
    parser.add_argument("--count", type=int, default=1, help="loop: how many functions")
    parser.add_argument("--retry-model", help="loop: a second model for what the first doesn't match")
    arguments = parser.parse_args()

    if arguments.command == "next":
        print(next_function() or "nothing left to try")
    elif arguments.command == "report":
        report()
    elif arguments.command == "loop":
        for _ in range(arguments.count):
            name = next_function()
            if name is None:
                print("nothing left to try")
                break
            matched = run(name, arguments.minutes, arguments.model)
            if not matched and arguments.retry_model:
                matched = run(name, arguments.minutes, arguments.retry_model)
            landed = matched and land(name)
            print(f"{name}: {'landed' if landed else load_ledger()[name]['outcome']}", flush=True)
        commit_records()
    elif arguments.function is None:
        sys.exit(f"{arguments.command} needs a FUNCTION")
    elif arguments.command == "prepare":
        print(prepare(arguments.function))
    elif arguments.command == "run":
        matched = run(arguments.function, arguments.minutes, arguments.model)
        print(f"{arguments.function}: {load_ledger()[arguments.function]}")
        sys.exit(0 if matched else 1)
    elif arguments.command == "land":
        sys.exit(0 if land(arguments.function) else 1)
    else:
        sys.exit(f"unknown command {arguments.command}")


if __name__ == "__main__":
    main()
