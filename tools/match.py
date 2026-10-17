#!/usr/bin/env python3
"""The matching loop (docs/WORKFLOW.md): a scratch directory per function, an agent (headless Claude Code) working in it,
and the result checked and landed by this script, never taken on the agent's word.

    python tools/match.py next                     the next function to try
    python tools/match.py prepare FUNCTION         build/match/FUNCTION/: target, m2c draft, brief, scripts, CLAUDE.md
    python tools/match.py run FUNCTION [--minutes 30] [--model MODEL]
    python tools/match.py land FUNCTION            match.c into its file in src/game/, build, commit when the SHA-1 holds
    python tools/match.py loop [--count N] [--minutes 30] [--model MODEL] [--retry-model MODEL]
    python tools/match.py report                   outcomes, models, costs, near misses
    python tools/match.py score FUNCTION FILE.c [--trials 500] [--seed 0]
                                                   an equivalent draft as it would land, judged by equiv.py (score.sh)

With --equivalent, next, prepare, run, land and loop work on the equivalent track (docs/WORKFLOW.md): the agent writes
behaviour-equivalent C (tools/match/EQUIVALENT.md), judged by tools/equiv.py, and land puts it into a NON_MATCHING block
around the function's INCLUDE_ASM line, kept only when both builds hold and equiv.py agrees on the landed file.

Each outcome goes into config/match/ledger.json, and each run's notes and C into config/match/notes/ (in git: what was
learned about a function outlives its scratch directory). config/match/skip.txt lists functions not to try, with why.
These and build/match/ are keyed by the function's address name (func_XXXXXXXX), which a rename (tools/rename.py) leaves;
FUNCTION is either that or its name now.
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

from equiv import DEFAULT_TRIALS, EXIT_DISAGREE, EXIT_UNSUPPORTED, GTE_OPCODES, NONMATCHING_MACRO, OPCODE_SHIFT
from probe_compiler import retail_assembly, retail_words
from progress import EQUIVALENT
from psx_cc import AS, ASFLAGS, DEFAULT_ASPSX, DEFAULT_CFLAGS, DEFAULT_GCC, HERE, compile_c, compiler
from symbols import DATA, GAME_FUNCTION, address_name, address_of, current_name, kind_of, with_current_names

SCRATCH = HERE / "build" / "match"
RECORDS = HERE / "config" / "match"
LEDGER = RECORDS / "ledger.json"
NOTES = RECORDS / "notes"
SKIP = RECORDS / "skip.txt"
TEMPLATE = HERE / "tools" / "match" / "CLAUDE.md"
EQUIVALENT_TEMPLATE = HERE / "tools" / "match" / "EQUIVALENT.md"
EQUIV = HERE / "tools" / "equiv.py"
SOURCES = HERE / "src" / "game"
GAME_H = HERE / "include" / "game.h"
# The game's shared types, by subsystem: types, enums and constants only, included by include/game.h
SHARED_HEADERS = HERE / "include" / "game"
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
# A top-level definition of a variable (not an extern declaration), with its initialiser if it has one: a split data
# symbol's when the symbol table says so (data_definitions)
DATA_DEFINITION = re.compile(r"^(?!extern\b)(?!typedef\b)[A-Za-z_][\w \t\*]*\b(\w+)\b[^(;=\n]*(=[^;]*)?;\n?", re.MULTILINE)
# A one-line declaration of a function (not its definition): return type, name, parameters; a game function's when the
# symbol table says so (prototypes)
PROTOTYPE = re.compile(r"^(?:extern[ \t]+)?(?P<declaration>[A-Za-z_][\w \t\*]*?\b(?P<name>\w+)[ \t]*\([^;{\n]*\))"
                       r"[ \t]*;[ \t]*\n?", re.MULTILINE)
IDENTIFIER = re.compile(r"[A-Za-z_]\w*")
# A structure's definition, typedef'd or tagged, with one level of nested braces at most
TYPE_DEFINITION = re.compile(r"^(?:typedef[ \t]+)?struct[ \t]*(?P<tag>\w*)[ \t]*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}[ \t]*"
                             r"(?P<alias>\w*)[ \t]*;[ \t]*\n?", re.MULTILINE)
# A function's definition in C: return type, name, parameters, then its body
DEFINITION = re.compile(r"^(?P<declaration>[A-Za-z_][\w \t\*]*?\b(?P<name>\w+)[ \t]*\([^;{]*\))\s*\{", re.MULTILINE)
POINTER_SPACING = re.compile(r"\s*\*\s*")
# An empty parameter list declares nothing about the arguments: the compiler checks no call against it
UNCHECKED_PARAMETERS = re.compile(r"\(\s*\)$")
# What a prototype in include/game.h may be written with, besides the shared headers' types: one using a structure of
# its file stays in the file
BASIC_TYPES = {"void", "char", "short", "int", "long", "unsigned", "signed", "const",
               "s8", "u8", "s16", "u16", "s32", "u32", "s64", "u64"}
TAG_KEYWORDS = {"struct", "union", "enum"}
# What the shared headers name besides structures: enums, and typedefs of another type
ENUM_DEFINITION = re.compile(r"^(?:typedef[ \t]+)?enum[ \t]*(?P<tag>\w*)[ \t]*\{[^{}]*\}[ \t]*(?P<alias>\w*)[ \t]*;",
                             re.MULTILINE)
PLAIN_TYPEDEF = re.compile(r"^typedef[^;{}]*\b(\w+)[ \t]*;", re.MULTILINE)
# A parameter's name: the last word of its declaration when a type or a `*` comes before it and it isn't a type itself
PARAMETER_NAME = re.compile(r"(?<=[\s*])(?!(?:" + "|".join(sorted(BASIC_TYPES)) + r")\b)[A-Za-z_]\w*\s*$")
ERROR_LINES = 12
# A line naming a cause: the compiler's (`FILE.c:LINE: ...`, or a header's), the linker's, a failed step or SHA-1 check
# (C locale)
COMPILER_ERROR = re.compile(r"\.[ch]:\d+:|-ld: |FAILED")
# A near miss for the report: at most this share of the words differ
NEAR_MISS = 0.1
SECONDS_PER_MINUTE = 60
MILLISECONDS_PER_MINUTE = 60000
# Epoch second the agent's time runs out, read by score.sh while the session lasts
DEADLINE = "deadline"
DEFAULT_MINUTES = 30
# Below this a function is a stub or a thunk: nothing to learn, and splat already writes the empty ones
MIN_SIZE = 0x20
EXECUTABLE_MODE = 0o755
AGENT_TOOLS = ["Read", "Edit", "Write", "Glob", "Grep", "Bash(./score.sh:*)", "Bash(./permute.sh:*)"]
PROMPT = ("Match the function described in CLAUDE.md and BRIEF.md in this directory, following CLAUDE.md's loop and rules. "
          "You have {minutes:g} minutes; past them the session is stopped, so keep best.c current. "
          "score.sh ends with the minutes left. "
          "When you stop, leave match.c or best.c and NOTES.md as CLAUDE.md says.")
EQUIVALENT_TOOLS = ["Read", "Edit", "Write", "Glob", "Grep", "Bash(./score.sh:*)"]
EQUIVALENT_PROMPT = ("Write behaviour-equivalent C for the function described in CLAUDE.md and BRIEF.md in this directory, "
                     "following CLAUDE.md's loop and rules. "
                     "You have {minutes:g} minutes; past them the session is stopped, so keep best.c current. "
                     "score.sh ends with the minutes left. "
                     "When you stop, leave equivalent.c or best.c and NOTES.md as CLAUDE.md says.")
# The mode an equivalent run's records carry, and the outcome of a function landed as equivalent C
EQUIVALENT_MODE = "equivalent"
# Notes of an equivalent run sit beside the matching runs' as <function>.equivalent.md and .c
EQUIVALENT_NOTES = ".equivalent"
# Equivalent C that didn't land, from a run or the notes
EQUIVALENT_REFUSED = "equivalent, not landed"
# An equivalent draft as it would land: a scratch copy of its file and of include/game.h, in its scratch directory
PLACED = "placed"
# How an equivalent run's C was judged, in its record: as it would land (older runs judged the draft alone)
SCORED_IN_PLACE = "in place"
# The outcomes after which an equivalent run may take a function: the matching loop is done with it, or its notes' C
# didn't land as equivalent (a run fixes what the builds refused). A match that didn't land ("matched, not landed") is
# fixed by hand instead
GIVEN_UP = ("gave up", "rejected", EQUIVALENT_REFUSED)
# From this size the matching loop matched about one function in seven to ten, at US$7-10 a match (docs/ROADMAP.md): one
# not tried yet goes to the equivalent track without a matching run
EQUIVALENT_FIRST_SIZE = 0x240
# equiv.py's lines: its last, the trials agreeing without being compared, and the first disagreement
AGREEING = re.compile(r"^(\d+) of (\d+) trials agree$", re.MULTILINE)
UNCOMPARED = re.compile(r"^(\d+) trials agree by both (?:reaching the limit|taking the same exception)", re.MULTILINE)
DISAGREEMENT = re.compile(r"^trial \d+ disagrees: ")
# A disagreement's lines kept as a reason: the trial with its arguments, then its first difference
DISAGREEMENT_LINES = 2
UNSUPPORTED = "unsupported"
# What an agent's run is checked with: equiv.py's defaults, as score.sh runs it
RUN_SEED = 0
# The landing check: more trials and other seeds than the agent's score.sh. One seed of 2,000 let through two functions
# a third seed caught (a call's argument narrowed, two stores through possibly aliased pointers swapped)
LANDING_TRIALS = 2000
LANDING_SEEDS = (1, 2)
# TRADEOFF: a heuristic floor against vacuous agreement, when nearly every trial agrees only by both runs reaching the
# limit or taking the same exception; a function whose paths random inputs rarely reach needs inputs built for it
MIN_COMPARED_SHARE = 0.05


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
    model doesn't hide the first. Keyed by the function's address name, which a rename leaves"""
    ledger = load_ledger()
    details = {key: portable(value) if isinstance(value, str) else value for key, value in details.items()}
    earlier = {key: value for key, value in ledger.get(address_name(name), {}).items() if key != "reason"}
    entry = {**earlier, "outcome": outcome, "time": time.strftime("%Y-%m-%d %H:%M"), **details}
    if "model" in details:
        entry["runs"] = entry.get("runs", []) + [{"outcome": outcome, **details}]
    ledger[address_name(name)] = entry
    LEDGER.parent.mkdir(parents=True, exist_ok=True)
    LEDGER.write_text(json.dumps(ledger, indent=2) + "\n")


def sources():
    """The game's C files, in address order"""
    return sorted(SOURCES.glob("*.c"))


def source_of(name):
    """(C file, INCLUDE_ASM line) of a function still included from the split; for an equivalent one, its whole
    NON_MATCHING block, which the matched function replaces"""
    for source in sources():
        text = source.read_text()
        for match in EQUIVALENT.finditer(text):
            if match.group(1) == name:
                return source, match.group(0)
        for match in INCLUDED.finditer(text):
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


def equivalents():
    """The functions with an equivalent block in their file: still assembly in the build, their C under NON_MATCHING"""
    return {name for source in sources() for name in EQUIVALENT.findall(source.read_text())}


def uses_gte(name):
    """Whether the function's code has a GTE instruction, which tools/equiv.py can't run"""
    return any(word >> OPCODE_SHIFT in GTE_OPCODES for word in retail_words(name))


def handwritten(name):
    """splat marks a function its disassembler finds not compiled (GTE code, register calling conventions)"""
    return HANDWRITTEN in assembly_of(name).read_text()


def skipped():
    """The names in config/match/skip.txt: one function per line, then why"""
    if not SKIP.exists():
        return set()
    return {line.split()[0] for line in SKIP.read_text().splitlines() if line.strip() and not line.startswith("#")}


def given_up_elsewhere(entry):
    """Whether a function was given up only under other compiler flags than the build's: worth another run"""
    return entry["outcome"] == "gave up" and all(run.get("cflags") != DEFAULT_CFLAGS for run in entry.get("runs", []))


def next_function():
    """The next function to try, handwritten, skipped and equivalent ones left out: one given up under other compiler
    flags first, then the smallest unmatched one of MIN_SIZE or more not tried yet"""
    ledger = load_ledger()
    skip = skipped()
    # An equivalent function went to that track because matching it cost too much: a match is left to `run FUNCTION`
    equivalent = equivalents()
    # The records and skip.txt by address name; the address breaks a tie
    keyed = [(address_name(name), name, size) for name, size in unmatched().items()]
    candidates = sorted((key not in ledger, size, key, name) for key, name, size in keyed
                        if size >= MIN_SIZE and key not in skip and name not in equivalent and not handwritten(name)
                        and (key not in ledger or given_up_elsewhere(ledger[key])))
    if not candidates:
        return None
    return candidates[0][3]


def settled_equivalent(entry):
    """Whether an equivalent run has had the function for good. A run that judged its draft alone couldn't see what its
    file refuses at landing, so a function such runs left refused at landing is tried again; a run that judged it in
    place could"""
    runs = [attempt for attempt in entry.get("runs", []) if attempt.get("mode") == EQUIVALENT_MODE]
    if not runs:
        return False
    elif entry["outcome"] == EQUIVALENT_REFUSED:
        return any(attempt.get("scored") == SCORED_IN_PLACE for attempt in runs)
    else:
        return True


def next_equivalent():
    """The next function for an equivalent run: the smallest one the matching loop gave up on for good, or of
    EQUIVALENT_FIRST_SIZE or more and not tried yet; still plain INCLUDE_ASM, not skipped, not settled by an equivalent
    run, and without GTE instructions"""
    ledger = load_ledger()
    skip = skipped()
    equivalent = equivalents()

    def due(key, name, size):
        if key not in ledger:
            return size >= EQUIVALENT_FIRST_SIZE and not handwritten(name)
        else:
            return (ledger[key]["outcome"] in GIVEN_UP and not given_up_elsewhere(ledger[key])
                    and not settled_equivalent(ledger[key]))

    # The records and skip.txt by address name; the address breaks a tie
    keyed = [(address_name(name), name, size) for name, size in unmatched().items()]
    candidates = sorted((size, key, name) for key, name, size in keyed
                        if due(key, name, size) and key not in skip and name not in equivalent)
    # The GTE check reads the split, so only as far as the first candidate without
    for _, _, name in candidates:
        if not uses_gte(name):
            return name
        else:
            continue
    return None


def callers(name):
    """The functions whose assembly calls this one, in the split and in src/"""
    call = re.compile(rf"\bjal\s+{name}\b")
    found = [path.stem for path in sorted(NONMATCHINGS.glob("*/*.s")) if call.search(path.read_text())]
    for path in sorted(GAME_ASSEMBLY.glob("*.s")):
        # Each function of the file runs from its glabel to the next one
        pieces = GLABEL.split(path.read_text())[1:]
        found += [function for function, body in zip(pieces[::2], pieces[1::2]) if call.search(body)]
    for source in sources():
        # An equivalent function's C isn't in the build
        text = INCLUDED.sub("", EQUIVALENT.sub("", source.read_text()))
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


def prepare(name, equivalent=False):
    """The function's scratch directory, by its address name, for a matching run or, with equivalent, an equivalent one"""
    key = address_name(name)
    folder = SCRATCH / key
    if folder.exists():
        shutil.rmtree(folder)
    folder.mkdir(parents=True)
    assembly = retail_assembly(name)
    target = folder / "target.s"
    target.write_text(ASSEMBLY_PREAMBLE + assembly)
    subprocess.run([AS, *ASFLAGS, "-o", str(folder / "target.o"), str(target)], check=True, cwd=HERE)

    # The function's whole file also holds its jump tables, from which m2c rebuilds a switch
    m2c_input = folder / "m2c_input.s"
    m2c_input.write_text(ASSEMBLY_PREAMBLE + assembly_of(name).read_text())

    source = source_of(name)[0].relative_to(HERE)
    context = m2c_context(folder, HERE / source)
    draft = subprocess.run([sys.executable, str(M2C), "--target", "mipsel-gcc-c", "--valid-syntax", "--context",
                            str(context), str(m2c_input)], capture_output=True, text=True)
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
    # What the file and include/game.h declare of the function and its data, at the top level (a body's lines are
    # indented): its file compiles the C with them
    own = [line for line in context.read_text().splitlines()
           if line == line.lstrip() and line.rstrip().endswith(";")
           and any(re.search(rf"\b{symbol}\b", line) for symbol in [name, *symbols])]
    brief = [
        f"# {name}" if name == key else f"# {name} ({key})",
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
        f"## Declarations include/game.h and {source} already have for it and its data",
        "",
        "```c",
        "\n".join(dict.fromkeys(own)) or "(none)",
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
    # A run that gave it up before left its notes and best C: the next one starts from them. An equivalent run starts
    # from an earlier equivalent run's, else from the matching run's
    earlier = f"{key}{EQUIVALENT_NOTES}" if equivalent else key
    if equivalent and not (NOTES / f"{earlier}.c").exists():
        earlier = key
    if (NOTES / f"{earlier}.md").exists() and (NOTES / f"{earlier}.c").exists():
        shutil.copy2(NOTES / f"{earlier}.md", folder / "EARLIER.md")
        # The C by the symbols' names now, though it was written before some were named
        (folder / "earlier.c").write_text(with_current_names((NOTES / f"{earlier}.c").read_text()))
        if not equivalent:
            told = "An earlier run gave it up, under other compiler flags: `EARLIER.md` has its notes, `earlier.c` its best C."
        elif earlier != key:
            told = "An earlier equivalent run: `EARLIER.md` has its notes, `earlier.c` its best C."
        else:
            told = ("The matching loop gave it up: `EARLIER.md` has its notes, `earlier.c` its closest C, which compiles "
                    "to other bytes and may behave otherwise.")
        brief += ["## An earlier run", "", told, ""]
    (folder / "BRIEF.md").write_text("\n".join(brief))
    template = EQUIVALENT_TEMPLATE if equivalent else TEMPLATE
    (folder / "CLAUDE.md").write_text(template.read_text().replace("$FUNCTION", name))

    python = sys.executable
    minutes_left = (f'deadline="$(dirname "$0")/{DEADLINE}"\n'
                    'if [ -f "$deadline" ]; then\n'
                    f'    echo "$(( ($(cat "$deadline") - $(date +%s)) / {SECONDS_PER_MINUTE} )) minutes left" >&2\n'
                    'fi\n'
                    'exit $status\n')
    if equivalent:
        # The draft as it would land (score); arguments after the file go to equiv.py: --seed N, --trials N
        scripts = {
            "score.sh": (f'#!/bin/sh\n"{python}" "{HERE / "tools" / "match.py"}" score {name} "$@"\nstatus=$?\n'
                         + minutes_left),
        }
    else:
        scripts = {
            "score.sh": (f'#!/bin/sh\n"{python}" "{HERE / "tools" / "probe_compiler.py"}" "$1" {name} --diff 2.7.2\n'
                         'status=$?\n' + minutes_left),
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


def keep_notes(name, folder, source, stem=None):
    """The run's NOTES.md and its C (the match or the best attempt) into config/match/notes/, as <stem>.md and .c (the
    function's address name by default)"""
    stem = stem or address_name(name)
    NOTES.mkdir(parents=True, exist_ok=True)
    if (folder / "NOTES.md").exists():
        (NOTES / f"{stem}.md").write_text(portable((folder / "NOTES.md").read_text()))
    if source is not None and (folder / source).exists():
        (NOTES / f"{stem}.c").write_text(portable((folder / source).read_text()))


def agent_session(folder, prompt, tools, minutes, model):
    """Headless Claude Code in the scratch directory until it stops or its time runs out: what the ledger keeps of it"""
    command = ["claude", "-p", prompt, "--permission-mode", "acceptEdits", "--allowedTools", *tools,
               "--output-format", "stream-json", "--verbose", "--no-session-persistence"]
    if model:
        command += ["--model", model]
    timed_out = False
    started = time.monotonic()
    (folder / DEADLINE).write_text(str(int(time.time() + minutes * SECONDS_PER_MINUTE)))
    with open(folder / "session.jsonl", "w") as log:
        # Its own process group, so a time limit stops the agent and whatever it started
        agent = subprocess.Popen(command, cwd=folder, stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            agent.wait(timeout=minutes * SECONDS_PER_MINUTE)
        except subprocess.TimeoutExpired:
            os.killpg(agent.pid, signal.SIGTERM)
            agent.wait()
            timed_out = True
    (folder / DEADLINE).unlink()
    wall = round((time.monotonic() - started) / SECONDS_PER_MINUTE, 1)
    return {"model": model or "default", "cflags": DEFAULT_CFLAGS, "timed_out": timed_out, "wall_minutes": wall,
            **session_result(folder)}


def run(name, minutes, model):
    folder = prepare(name)
    session = agent_session(folder, PROMPT.format(minutes=minutes), AGENT_TOOLS, minutes, model)

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


def unfit(text):
    """Why a candidate's C can't go into a file as it is, or None"""
    macro = M2C_MACRO.search(text)
    if macro is not None:
        return f"uses m2c's {macro.group(0)}: real types needed"
    elif INLINE_ASSEMBLY.search(text):
        return "inline assembly"
    elif INCLUDED.search(text):
        return "an INCLUDE_ASM line"
    else:
        return None


def equivalence(source, name, trials, seed):
    """tools/equiv.py's verdict on a C file: (whether every trial agrees, the trials compared, its summary or why not).
    Compared are the agreeing trials but those agreeing by both runs reaching the limit or taking the same exception"""
    result = subprocess.run([sys.executable, str(EQUIV), str(source), name, "--trials", str(trials), "--seed", str(seed)],
                            cwd=HERE, capture_output=True, text=True)
    lines = (result.stdout + result.stderr).strip().splitlines()
    agreeing = AGREEING.search(result.stdout)
    if result.returncode == EXIT_UNSUPPORTED:
        return False, 0, next((line for line in lines if line.startswith(UNSUPPORTED)), UNSUPPORTED)
    elif agreeing is None:
        # It stopped before the trials: the C doesn't compile, or names what the retail executable doesn't have
        return False, 0, "\n".join(lines[-ERROR_LINES:]) or f"equiv.py exited with status {result.returncode}"
    elif result.returncode == 0 and agreeing.group(1) == agreeing.group(2):
        compared = int(agreeing.group(2)) - sum(int(count) for count in UNCOMPARED.findall(result.stdout))
        return True, compared, f"{agreeing.group(0)}, {compared} compared"
    else:
        # The first disagreement, then the count
        start = next((index for index, line in enumerate(lines) if DISAGREEMENT.match(line)), None)
        shown = [] if start is None else [line.strip() for line in lines[start:start + DISAGREEMENT_LINES]]
        return False, 0, "\n".join(shown + [agreeing.group(0)])


def placed_copy(name, text):
    """An equivalent draft as it would land, src/ and include/ left alone: placement() into a scratch copy of its file
    beside a scratch include/game.h, which a quoted `#include "game.h"` finds first. The other files calling a function
    whose prototype the landing changes are compiled there too. The copy's path; raises NotLanded"""
    reason = unfit(text)
    if reason is not None:
        raise NotLanded(reason)
    source, updated, updated_header = placement(name, text, True)
    folder = SCRATCH / address_name(name) / PLACED
    if folder.exists():
        shutil.rmtree(folder)
    folder.mkdir(parents=True)
    (folder / GAME_H.name).write_text(updated_header)
    placed = folder / source.name
    placed.write_text(updated)
    before = {prototype.group("name"): prototype.group("declaration") for prototype in game_prototypes(GAME_H.read_text())}
    changed = [prototype.group("name") for prototype in game_prototypes(updated_header)
               if before.get(prototype.group("name")) != prototype.group("declaration")]
    for other in sources():
        content = other.read_text()
        if other == source or not any(re.search(rf"\b{callee}\b", content) for callee in changed):
            continue
        copy = folder / other.name
        copy.write_text(content)
        # With NON_MATCHING: the nonmatching build compiles every declaration the default one does, and more
        error = compile_c(compiler(DEFAULT_GCC), DEFAULT_ASPSX, copy, DEFAULT_CFLAGS, copy.with_suffix(".o"),
                          (NONMATCHING_MACRO,))
        if error is not None:
            raise NotLanded(error)
    return placed


def judged_in_place(name, draft, trials, seed):
    """equiv.py's verdict on an equivalent draft as it would land: (whether every trial agrees, its summary or why not)"""
    try:
        placed = placed_copy(name, draft.read_text())
    except NotLanded as refusal:
        return False, f"not landable as it is: {refusal.reason}"
    agrees, _, summary = equivalence(placed, name, trials, seed)
    return agrees, summary


def score_in_place(name, draft, trials, seed):
    """score.sh in equivalent mode: equiv.py's whole output on the draft as it would land, or why it can't; its exit
    status"""
    if not draft.exists():
        sys.exit(f"{draft} is missing")
    try:
        placed = placed_copy(name, draft.read_text())
    except NotLanded as refusal:
        print(f"not landable as it is: {refusal.reason}")
        return EXIT_DISAGREE
    command = [sys.executable, str(EQUIV), str(placed), name, "--trials", str(trials), "--seed", str(seed)]
    return subprocess.run(command, cwd=HERE).returncode


def run_equivalent(name, minutes, model):
    """An agent run on the equivalent track; True when its equivalent.c agrees on every trial of equiv.py's default run"""
    folder = prepare(name, equivalent=True)
    session = {"mode": EQUIVALENT_MODE, "scored": SCORED_IN_PLACE,
               **agent_session(folder, EQUIVALENT_PROMPT.format(minutes=minutes), EQUIVALENT_TOOLS, minutes, model)}
    stem = f"{address_name(name)}{EQUIVALENT_NOTES}"
    candidate = folder / "equivalent.c"
    if candidate.exists():
        keep_notes(name, folder, candidate.name, stem)
        reason = unfit(candidate.read_text())
        if reason is not None:
            record(name, "equivalent, rejected", reason=f"equivalent.c: {reason}", **session)
            return False
        agrees, summary = judged_in_place(name, candidate, DEFAULT_TRIALS, RUN_SEED)
        if agrees:
            record(name, "equivalent, agrees", summary=summary, **session)
            return True
        record(name, "equivalent, rejected", reason=f"equivalent.c disagrees: {summary}", **session)
        return False
    best = "best.c" if (folder / "best.c").exists() else None
    keep_notes(name, folder, best, stem)
    summary = "no attempt kept" if best is None else judged_in_place(name, folder / best, DEFAULT_TRIALS, RUN_SEED)[1]
    record(name, "equivalent, gave up", summary=summary, best=best, **session)
    return False


def declarator(definition):
    """A data definition's type and name, its initialiser left out: `s32 *D_8005F264`"""
    return " ".join(definition.group(0).split("=")[0].rstrip("; \n").split())


def type_name(definition):
    """A structure definition's name: its typedef's, else its tag"""
    return definition.group("alias") or definition.group("tag")


def signature_of(declaration):
    """A declaration's return type, name and parameter types, parameter names left out and spacing made uniform
    (`s32 *func_X(u8 *, s32)`); None when a parameter is a function pointer, which this doesn't parse"""
    head, _, rest = POINTER_SPACING.sub(" *", declaration).partition("(")
    parameters = rest[:rest.rindex(")")]
    if "(" in parameters:
        return None
    elif parameters.strip() in ("", "void"):
        return f"{' '.join(head.split())}(void)"
    else:
        types = [" ".join(PARAMETER_NAME.sub("", parameter).split()) for parameter in parameters.split(",")]
        return f"{' '.join(head.split())}({', '.join(types)})"


def prototype_of(declaration):
    """A definition's declarator as a prototype; None when a parameter is a function pointer"""
    signature = signature_of(declaration)
    return None if signature is None else signature + ";"


def game_prototypes(text):
    """The one-line prototypes of the game's functions in C text"""
    return [match for match in PROTOTYPE.finditer(text) if kind_of(match.group("name")) == GAME_FUNCTION]


def data_definitions(text):
    """The top-level definitions of the split's data symbols in C text"""
    return [match for match in DATA_DEFINITION.finditer(text) if kind_of(match.group(1)) == DATA]


def shared_structures():
    """{name: (definition, header)} of the structures the shared headers define, by typedef name and by tag"""
    found = {}
    for header in sorted(SHARED_HEADERS.glob("*.h")):
        for definition in TYPE_DEFINITION.finditer(header.read_text()):
            for name in (definition.group("tag"), definition.group("alias")):
                if name:
                    found[name] = (definition.group(0), header)
                else:
                    # An untagged structure, or one without a typedef name
                    continue
    return found


def shared_type_names():
    """The names the shared headers give types: structures, enums, typedefs of another type"""
    names = set(shared_structures())
    for header in sorted(SHARED_HEADERS.glob("*.h")):
        text = header.read_text()
        names |= {name for enum in ENUM_DEFINITION.finditer(text) for name in enum.groups() if name}
        names |= set(PLAIN_TYPEDEF.findall(text))
    return names


def basic(prototype, name):
    """Whether a prototype uses only basic types and the shared headers' types, so include/game.h can carry it"""
    allowed = BASIC_TYPES | TAG_KEYWORDS | shared_type_names()
    return prototype is not None and set(IDENTIFIER.findall(prototype)) - {name} <= allowed


def with_prototypes(header, added):
    """include/game.h with these prototypes added to its list, which stays in address order"""
    prototypes = game_prototypes(header)
    if not prototypes:
        sys.exit(f"{GAME_H} has no list of prototypes to add to")
    start, end = prototypes[0].start(), prototypes[-1].end()
    listed = header[start:end]
    for prototype in prototypes:
        listed = listed.replace(prototype.group(0), "", 1)
    if listed.strip():
        sys.exit(f"{GAME_H}: something other than the game's prototypes inside its list; add {added} by hand")
    lines = sorted([prototype.group("declaration") + ";" for prototype in prototypes] + added,
                   key=lambda line: address_of(PROTOTYPE.match(line).group("name")))
    return header[:start] + "\n".join(lines) + "\n" + header[end:]


class NotLanded(Exception):
    """Why a candidate can't go into its file as it is; blocked when it defines data the file doesn't"""

    def __init__(self, reason, blocked=False):
        super().__init__(reason)
        self.reason = reason
        self.blocked = blocked


def placement(name, text, equivalent):
    """(C file, its new text, include/game.h's new text) with the candidate's declarations and function in place of the
    function's INCLUDE_ASM line, or with equivalent in a NON_MATCHING block around that line. An equivalent block the
    function already has goes whole, with what it defined. Raises NotLanded when the candidate can't go in as it is"""
    source, current = source_of(name)
    relative = source.relative_to(HERE)
    # C written before a symbol it uses was named calls it by its address name
    text = with_current_names(text)
    include = INCLUDED.search(current).group(0)
    original = source.read_text().replace(current, include, 1)
    insertion = original.index(include)
    # Above the function, the file as it is; below it, the file without the definitions the candidate moves up
    above, below = original[:insertion], original[insertion:]
    # Other functions' equivalent blocks: what they define is in the nonmatching build only
    blocks = [range(match.start(), match.end()) for match in EQUIVALENT.finditer(original)]

    def in_block(definition):
        """Whether a definition is inside another function's equivalent block"""
        return any(definition.start() in block for block in blocks)

    def moved_up(definition, what):
        """below without a definition the candidate moves up to the function. Into a NON_MATCHING block, only out of
        another one: from anywhere else the default build would lose it"""
        if equivalent and not in_block(definition):
            raise NotLanded(f"{relative} defines {what} further down: moved into the NON_MATCHING block, it would leave "
                            f"the default build")
        return below.replace(definition.group(0), "", 1)

    # The original compiler addresses a variable through $gp only in the file that defines it (docs/COMPILER.md). A
    # definition the file already has goes, or moves up to this function when the file has it further down; an
    # uninitialised one stays, a common the split's symbol resolves; an initialised one the file doesn't have is data of
    # another file, which can't be read through $gp from this one
    defined = {match.group(1): match for match in data_definitions(original)}
    for definition in data_definitions(text):
        symbol, initialiser = definition.group(1), definition.group(2)
        if symbol in defined and declarator(defined[symbol]) != declarator(definition):
            raise NotLanded(f"declares {symbol} as `{declarator(definition)}`, {relative} as "
                            f"`{declarator(defined[symbol])}`")
        elif symbol in defined and defined[symbol].start() < insertion:
            text = text.replace(definition.group(0), "")
        elif symbol in defined:
            below = moved_up(defined[symbol], symbol)
        elif initialiser is not None:
            raise NotLanded(f"defines {symbol}, which {relative} doesn't", blocked=True)
        else:
            # A common: the split's symbol is the definition
            continue
    # A structure the file or a shared header (include/game/) defines: the candidate's copy of it goes, or moves up to this
    # function when the file has it further down, and a different one can't land
    types = {type_name(match): match for match in TYPE_DEFINITION.finditer(original)}
    shared = shared_structures()
    # Structures the file defines further down that equivalent C needs: they go above its block, outside it
    hoisted = []
    for definition in TYPE_DEFINITION.finditer(text):
        kind = type_name(definition)
        shared_name = next((name for name in (definition.group("tag"), definition.group("alias")) if name in shared), None)
        if shared_name is not None and " ".join(shared[shared_name][0].split()) == " ".join(definition.group(0).split()):
            # include/game.h includes the shared headers: the file has it already
            text = text.replace(definition.group(0), "")
        elif shared_name is not None:
            raise NotLanded(f"defines {shared_name} otherwise than {shared[shared_name][1].relative_to(HERE)}")
        elif kind not in types:
            # A structure of its own: it lands with the function
            continue
        elif " ".join(types[kind].group(0).split()) != " ".join(definition.group(0).split()):
            raise NotLanded(f"defines {kind} otherwise than {relative}")
        elif types[kind].start() < insertion:
            text = text.replace(definition.group(0), "")
        elif equivalent and not in_block(types[kind]):
            # Both builds keep seeing it there; a type emits no code, so the default build's bytes stay. The blank line
            # after it goes with it
            moved = types[kind].group(0)
            below = below.replace(moved + "\n" if moved + "\n" in below else moved, "", 1)
            hoisted.append(types[kind].group(0).rstrip("\n") + "\n")
            text = text.replace(definition.group(0), "")
        else:
            below = moved_up(types[kind], kind)
    # One prototype per game function, in include/game.h: one it has goes, a new one of basic types moves there
    header = GAME_H.read_text()
    declared = {prototype.group("name"): prototype.group("declaration") for prototype in game_prototypes(header)}
    added = []
    for prototype in PROTOTYPE.finditer(text):
        callee = prototype.group("name")
        if callee == name:
            # The function's own forward declaration stays with it
            continue
        elif kind_of(callee) != GAME_FUNCTION:
            # An SDK function (include/sdk.h's once it is identified), or none the split knows: it stays in the file
            continue
        elif callee in declared and signature_of(prototype.group("declaration")) != signature_of(declared[callee]):
            # The candidate was judged with its own prototype; include/game.h's would compile the call otherwise
            raise NotLanded(f"declares `{prototype.group('declaration')}`, include/game.h `{declared[callee]}`")
        elif callee in declared:
            text = text.replace(prototype.group(0), "")
        elif UNCHECKED_PARAMETERS.search(prototype.group("declaration")):
            raise NotLanded(f"`{prototype.group('declaration')}` has no parameter list")
        elif basic(prototype_of(prototype.group("declaration")), callee):
            added.append(prototype_of(prototype.group("declaration")))
            text = text.replace(prototype.group(0), "")
        else:
            # A structure of its file in the prototype: it stays in the file
            continue
    # The definition is the truth about the function's signature: prototypes of it its callers' landings wrote give way to
    # the definition's, in include/game.h, when its types are basic
    own = next((match for match in DEFINITION.finditer(text) if match.group("name") == name), None)
    signature = None if own is None else prototype_of(own.group("declaration"))
    listed = header
    if basic(signature, name):
        for prototype in [match for match in PROTOTYPE.finditer(header) if match.group("name") == name]:
            listed = listed.replace(prototype.group(0), "", 1)
        for prototype in [match for match in PROTOTYPE.finditer(original) if match.group("name") == name]:
            above = above.replace(prototype.group(0), "", 1)
            below = below.replace(prototype.group(0), "", 1)
        for prototype in [match for match in PROTOTYPE.finditer(text) if match.group("name") == name]:
            text = text.replace(prototype.group(0), "", 1)
        added.append(signature)
    else:
        # A structure in its signature, or none parsed: its prototypes stay, and the build says whether they agree
        pass
    updated_header = with_prototypes(listed, added) if added else header
    # Its #include lines are the file's already; a declaration the file has above it isn't repeated
    present = set(above.splitlines())
    # A top-level line ending in `;` is a declaration
    kept = [line for line in text.splitlines()
            if not line.startswith("#include") and not (line == line.lstrip() and line.endswith(";") and line in present)]
    body = "\n".join(kept).strip() + "\n"
    if equivalent and hoisted:
        body = "".join(hoisted) + f"\n#ifdef NON_MATCHING\n{body}#else\n{include}#endif\n"
    elif equivalent:
        body = f"#ifdef NON_MATCHING\n{body}#else\n{include}#endif\n"
    else:
        # A match takes the INCLUDE_ASM line's place as it is
        pass
    updated = above + below.replace(include, body, 1)
    if equivalent and name not in EQUIVALENT.findall(updated):
        raise NotLanded("a conditional of its own (#if, #else, #endif), which the NON_MATCHING block can't hold")
    return source, updated, updated_header


def build(steps):
    """Runs each step from the root: None when all pass, else the failing step's cause"""
    for step in steps:
        result = subprocess.run(step, cwd=HERE, capture_output=True, text=True, env={**os.environ, "LC_ALL": "C"})
        if result.returncode != 0:
            lines = (result.stdout + result.stderr).strip().splitlines()
            # The compiler's and the linker's own lines first: ninja's progress lines bury them
            causes = [line for line in lines if COMPILER_ERROR.search(line)]
            return "\n".join((causes or lines[-ERROR_LINES:])[:ERROR_LINES])
        else:
            continue
    return None


def land(name, equivalent=False):
    """The candidate's declarations and function in place of the INCLUDE_ASM line, committed only when it holds. A match
    (match.c) holds when the build's SHA-1 does; equivalent C (equivalent.c, else the notes' C) goes into a NON_MATCHING
    block and holds when the SHA-1 does, the nonmatching build builds and tools/equiv.py agrees on the landed file"""
    key = address_name(name)
    if equivalent:
        candidates = [SCRATCH / key / "equivalent.c", NOTES / f"{key}{EQUIVALENT_NOTES}.c", NOTES / f"{key}.c"]
        replacing = name in equivalents()
        # A function already equivalent stays so when other C for it doesn't land: its block is still there
        refused = EQUIVALENT_MODE if replacing else EQUIVALENT_REFUSED
    else:
        candidates = [SCRATCH / key / "match.c", NOTES / f"{key}.c"]
        refused = "matched, not landed"
    candidate = next((path for path in candidates if path.exists()), None)
    if candidate is None:
        sys.exit(f"no {candidates[0].name} for {name}")
    reason = unfit(candidate.read_text())
    if reason is not None:
        record(name, refused, reason=reason)
        return False
    try:
        source, updated, updated_header = placement(name, candidate.read_text(), equivalent)
    except NotLanded as refusal:
        record(name, "matched, blocked" if refusal.blocked and not equivalent else refused, reason=refusal.reason)
        return False
    original = source.read_text()
    header = GAME_H.read_text()
    if updated == original and updated_header == header:
        # The file has this C already: nothing to build, record or commit
        print(f"{name}: {source.relative_to(HERE)} has this C already")
        return True
    source.write_text(updated)
    GAME_H.write_text(updated_header)

    ninja = str(HERE / ".venv" / "bin" / "ninja")
    steps = [[sys.executable, "configure.py"], [ninja]]
    if equivalent:
        steps.append([ninja, "nonmatching"])
    error = build(steps)
    summary = None
    if error is None and equivalent:
        # The landed file, not the candidate: its own definitions and include/game.h's prototypes compile the function
        for seed in LANDING_SEEDS:
            agrees, compared, summary = equivalence(source, name, LANDING_TRIALS, seed)
            if not agrees:
                error = summary
                break
            elif compared < MIN_COMPARED_SHARE * LANDING_TRIALS:
                error = f"{summary}: fewer than {MIN_COMPARED_SHARE:.0%} of the trials compared"
                break
            else:
                # Every trial of this seed agrees, enough of them compared
                continue
    if error is not None:
        source.write_text(original)
        GAME_H.write_text(header)
        subprocess.run([sys.executable, "configure.py"], cwd=HERE, capture_output=True)
        record(name, refused, reason=error)
        return False
    if equivalent:
        record(name, EQUIVALENT_MODE, summary=summary)
        message = f"feat: improve the equivalent C of {name}" if replacing else f"feat: add equivalent C for {name}"
    else:
        record(name, "landed")
        message = f"feat: match {name}"
    subprocess.run(["git", "add", str(source), str(GAME_H), str(RECORDS)], cwd=HERE, check=True)
    commit(message)
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


def near_miss(entry):
    """Whether the run that gave the function up came within NEAR_MISS of a match"""
    counted = DIFFERING.search(entry.get("summary", ""))
    return (entry["outcome"] == "gave up" and counted is not None
            and int(counted.group(1)) <= NEAR_MISS * int(counted.group(2)))


def report():
    ledger = load_ledger()
    outcomes = {}
    models = {}
    near = []
    for name, entry in sorted(ledger.items()):
        outcomes[entry["outcome"]] = outcomes.get(entry["outcome"], 0) + 1
        for attempt in entry.get("runs", []):
            # Matching and equivalent runs apart: an equivalent run succeeds when its C agrees
            if attempt.get("mode") == EQUIVALENT_MODE:
                key, success, verb = f"{attempt['model']}, {EQUIVALENT_MODE}", "equivalent, agrees", "agreed"
            else:
                key, success, verb = attempt["model"], "matched", "matched"
            stats = models.setdefault(key, {"runs": 0, "succeeded": 0, "cost_usd": 0.0, "verb": verb})
            stats["runs"] += 1
            stats["succeeded"] += attempt["outcome"] == success
            stats["cost_usd"] += attempt.get("cost_usd") or 0
        if near_miss(entry):
            near.append(f"{name} ({entry['summary']})")
        elif entry["outcome"] in ("matched, blocked", "matched, not landed", EQUIVALENT_REFUSED):
            # The reason's first compiler error says more than ninja's closing line
            lines = entry.get("reason", "").splitlines()
            errors = [line for line in lines if re.search(r"\.[ch]:\d+", line)]
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
        print(f"{model}: {stats['succeeded']} {stats['verb']} of {stats['runs']} runs, ${stats['cost_usd']:.2f}")
    print("near misses and C waiting to land:")
    for line in near:
        print(f"  {line}")


def entry_of(name):
    """A function's record in the ledger, by either of its names"""
    return load_ledger()[address_name(name)]


def game_function(argument):
    """A game function's name now, from either of its names (its address name or its name now); None for another kind of
    symbol"""
    name = current_name(argument)
    return name if kind_of(name) == GAME_FUNCTION else None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("command", choices=["next", "prepare", "run", "land", "loop", "report", "score"])
    parser.add_argument("function", nargs="?")
    parser.add_argument("draft", nargs="?", help="score: the equivalent draft to judge")
    parser.add_argument("--minutes", type=float, default=DEFAULT_MINUTES, help="the agent's time limit per function")
    parser.add_argument("--model", help="the agent's model (default: Claude Code's)")
    parser.add_argument("--count", type=int, default=1, help="loop: how many functions")
    parser.add_argument("--retry-model", help="loop: a second model for near misses of the first")
    parser.add_argument("--trials", type=int, default=DEFAULT_TRIALS, help="score: equiv.py's trials")
    parser.add_argument("--seed", type=int, default=RUN_SEED, help="score: equiv.py's seed")
    parser.add_argument("--equivalent", action="store_true",
                        help="next, prepare, run, land, loop: the equivalent track (behaviour-equivalent C under NON_MATCHING)")
    arguments = parser.parse_args()

    if arguments.command == "next" and arguments.equivalent:
        print(next_equivalent() or "nothing left to try")
    elif arguments.command == "next":
        print(next_function() or "nothing left to try")
    elif arguments.command == "report":
        report()
    elif arguments.command == "loop" and arguments.equivalent:
        if arguments.retry_model:
            sys.exit("--retry-model retries near misses of matching runs: not with --equivalent")
        for _ in range(arguments.count):
            name = next_equivalent()
            if name is None:
                print("nothing left to try")
                break
            if run_equivalent(name, arguments.minutes, arguments.model):
                land(name, equivalent=True)
            else:
                # Its C doesn't agree, or there is none: the ledger has why
                pass
            print(f"{name}: {entry_of(name)['outcome']}", flush=True)
        commit_records()
    elif arguments.command == "loop":
        for _ in range(arguments.count):
            name = next_function()
            if name is None:
                print("nothing left to try")
                break
            matched = run(name, arguments.minutes, arguments.model)
            # The second model only where the first came close: elsewhere it rarely matched, at several times the cost
            if not matched and arguments.retry_model and near_miss(entry_of(name)):
                matched = run(name, arguments.minutes, arguments.retry_model)
            landed = matched and land(name)
            print(f"{name}: {'landed' if landed else entry_of(name)['outcome']}", flush=True)
        commit_records()
    elif arguments.function is None:
        sys.exit(f"{arguments.command} needs a FUNCTION")
    elif (function := game_function(arguments.function)) is None:
        sys.exit(f"{arguments.function} is no game function of the split")
    elif arguments.command == "prepare":
        print(prepare(function, arguments.equivalent))
    elif arguments.command == "score" and arguments.draft is None:
        sys.exit("score needs a FUNCTION and a FILE.c")
    elif arguments.command == "score":
        sys.exit(score_in_place(function, Path(arguments.draft).resolve(), arguments.trials, arguments.seed))
    elif arguments.command == "run" and arguments.equivalent:
        agrees = run_equivalent(function, arguments.minutes, arguments.model)
        print(f"{function}: {entry_of(function)}")
        sys.exit(0 if agrees else 1)
    elif arguments.command == "run":
        matched = run(function, arguments.minutes, arguments.model)
        print(f"{function}: {entry_of(function)}")
        sys.exit(0 if matched else 1)
    elif arguments.command == "land":
        sys.exit(0 if land(function, arguments.equivalent) else 1)
    else:
        sys.exit(f"unknown command {arguments.command}")


if __name__ == "__main__":
    main()
