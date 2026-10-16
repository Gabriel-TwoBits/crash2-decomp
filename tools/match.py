#!/usr/bin/env python3
"""The matching loop (docs/WORKFLOW.md): a scratch directory per function, an agent (headless Claude Code) working in it,
and the result checked and landed by this script, never taken on the agent's word.

    python tools/match.py next                     the next function to try
    python tools/match.py prepare FUNCTION         build/match/FUNCTION/: target, m2c draft, brief, scripts, CLAUDE.md
    python tools/match.py run FUNCTION [--minutes 30] [--model MODEL]
    python tools/match.py land FUNCTION            match.c into its file in src/game/, build, commit when the SHA-1 holds
    python tools/match.py loop [--count N] [--minutes 30] [--model MODEL] [--retry-model MODEL]
    python tools/match.py report                   outcomes, models, costs, near misses

With --equivalent, next, prepare, run, land and loop work on the equivalent track (docs/WORKFLOW.md): the agent writes
behaviour-equivalent C (tools/match/EQUIVALENT.md), judged by tools/equiv.py, and land puts it into a NON_MATCHING block
around the function's INCLUDE_ASM line, kept only when both builds hold and equiv.py agrees on the landed file.

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

from equiv import DEFAULT_TRIALS, EXIT_UNSUPPORTED, GTE_OPCODES, OPCODE_SHIFT
from probe_compiler import retail_assembly, retail_words
from progress import EQUIVALENT
from psx_cc import AS, ASFLAGS, DEFAULT_CFLAGS, HERE

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
# A structure's definition, typedef'd or tagged, with one level of nested braces at most
TYPE_DEFINITION = re.compile(r"^(?:typedef[ \t]+)?struct[ \t]*(?P<tag>\w*)[ \t]*\{[^{}]*(?:\{[^{}]*\}[^{}]*)*\}[ \t]*"
                             r"(?P<alias>\w*)[ \t]*;[ \t]*\n?", re.MULTILINE)
# A game function's definition in C: return type, name, parameters, then its body
DEFINITION = re.compile(r"^(?P<declaration>[A-Za-z_][\w \t\*]*?\b(?P<name>func_[0-9A-F]{8})[ \t]*\([^;{]*\))\s*\{",
                        re.MULTILINE)
POINTER_SPACING = re.compile(r"\s*\*\s*")
# An empty parameter list declares nothing about the arguments: the compiler checks no call against it
UNCHECKED_PARAMETERS = re.compile(r"\(\s*\)$")
# PsyQ's libraries start here (config/SCUS_941.54.yaml): a func_ name from there is the SDK's, not the game's
SDK_START = 0x80046884
FUNCTION_ADDRESS_START = len("func_")
HEXADECIMAL = 16
# What a prototype in include/game.h may be written with: one using a structure of its file stays in the file
BASIC_TYPES = {"void", "char", "short", "int", "long", "unsigned", "signed", "const",
               "s8", "u8", "s16", "u16", "s32", "u32", "s64", "u64"}
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
# The outcomes after which an equivalent run may take a function: the matching loop is done with it, or its notes' C
# didn't land as equivalent (a run fixes what the builds refused). A match that didn't land ("matched, not landed") is
# fixed by hand instead
GIVEN_UP = ("gave up", "rejected", EQUIVALENT_REFUSED)
# equiv.py's lines: its last, the trials agreeing without being compared, and the first disagreement
AGREEING = re.compile(r"^(\d+) of (\d+) trials agree$", re.MULTILINE)
UNCOMPARED = re.compile(r"^(\d+) trials agree by both (?:reaching the limit|taking the same exception)", re.MULTILINE)
DISAGREEMENT = re.compile(r"^trial \d+ disagrees: ")
UNSUPPORTED = "unsupported"
# What an agent's run is checked with: equiv.py's defaults, as score.sh runs it
RUN_SEED = 0
# The landing check: more trials and another seed than the agent's score.sh
LANDING_TRIALS = 2000
LANDING_SEED = 1
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
    candidates = sorted((name not in ledger, size, name) for name, size in unmatched().items()
                        if size >= MIN_SIZE and name not in skip and name not in equivalent and not handwritten(name)
                        and (name not in ledger or given_up_elsewhere(ledger[name])))
    if not candidates:
        return None
    return candidates[0][2]


def ran_equivalent(entry):
    """Whether an equivalent run has had the function"""
    return any(attempt.get("mode") == EQUIVALENT_MODE for attempt in entry.get("runs", []))


def next_equivalent():
    """The next function for an equivalent run: the smallest one the matching loop gave up on for good, still plain
    INCLUDE_ASM, not skipped, without an equivalent run before, and without GTE instructions"""
    ledger = load_ledger()
    skip = skipped()
    equivalent = equivalents()
    candidates = sorted((size, name) for name, size in unmatched().items()
                        if name in ledger and ledger[name]["outcome"] in GIVEN_UP and not given_up_elsewhere(ledger[name])
                        and not ran_equivalent(ledger[name]) and name not in skip and name not in equivalent)
    # The GTE check reads the split, so only as far as the first candidate without
    for _, name in candidates:
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
    """The function's scratch directory, for a matching run or, with equivalent, an equivalent one"""
    folder = SCRATCH / name
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
    # A run that gave it up before left its notes and best C: the next one starts from them. An equivalent run starts
    # from an earlier equivalent run's, else from the matching run's
    earlier = f"{name}{EQUIVALENT_NOTES}" if equivalent else name
    if equivalent and not (NOTES / f"{earlier}.c").exists():
        earlier = name
    if (NOTES / f"{earlier}.md").exists() and (NOTES / f"{earlier}.c").exists():
        shutil.copy2(NOTES / f"{earlier}.md", folder / "EARLIER.md")
        shutil.copy2(NOTES / f"{earlier}.c", folder / "earlier.c")
        if not equivalent:
            told = "An earlier run gave it up, under other compiler flags: `EARLIER.md` has its notes, `earlier.c` its best C."
        elif earlier != name:
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
        # Arguments after the file go to equiv.py: --seed N, --trials N
        scripts = {
            "score.sh": (f'#!/bin/sh\nsource="$1"\nshift\n"{python}" "{EQUIV}" "$source" {name} "$@"\nstatus=$?\n'
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
    function's name by default)"""
    stem = stem or name
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
        # The first disagreement's trial and its first difference, then the count
        start = next((index for index, line in enumerate(lines) if DISAGREEMENT.match(line)), None)
        shown = [] if start is None else [line.strip() for line in lines[start:start + 2]]
        return False, 0, "\n".join(shown + [agreeing.group(0)])


def run_equivalent(name, minutes, model):
    """An agent run on the equivalent track; True when its equivalent.c agrees on every trial of equiv.py's default run"""
    folder = prepare(name, equivalent=True)
    session = {"mode": EQUIVALENT_MODE,
               **agent_session(folder, EQUIVALENT_PROMPT.format(minutes=minutes), EQUIVALENT_TOOLS, minutes, model)}
    stem = f"{name}{EQUIVALENT_NOTES}"
    candidate = folder / "equivalent.c"
    if candidate.exists():
        keep_notes(name, folder, candidate.name, stem)
        reason = unfit(candidate.read_text())
        if reason is not None:
            record(name, "equivalent, rejected", reason=f"equivalent.c: {reason}", **session)
            return False
        agrees, _, summary = equivalence(candidate, name, DEFAULT_TRIALS, RUN_SEED)
        if agrees:
            record(name, "equivalent, agrees", summary=summary, **session)
            return True
        record(name, "equivalent, rejected", reason=f"equivalent.c disagrees: {summary}", **session)
        return False
    best = "best.c" if (folder / "best.c").exists() else None
    keep_notes(name, folder, best, stem)
    summary = "no attempt kept" if best is None else equivalence(folder / best, name, DEFAULT_TRIALS, RUN_SEED)[2]
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


def basic(prototype, name):
    """Whether a prototype uses only basic types, so include/game.h can carry it"""
    return prototype is not None and set(IDENTIFIER.findall(prototype)) - {name} <= BASIC_TYPES


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
    include = INCLUDED.search(current).group(0)
    original = source.read_text().replace(current, include, 1)
    insertion = original.index(include)
    # Above the function, the file as it is; below it, the file without the definitions the candidate moves up
    above, below = original[:insertion], original[insertion:]
    # Other functions' equivalent blocks: what they define is in the nonmatching build only
    blocks = [range(match.start(), match.end()) for match in EQUIVALENT.finditer(original)]

    def moved_up(definition, what):
        """below without a definition the candidate moves up to the function. Into a NON_MATCHING block, only out of
        another one: from anywhere else the default build would lose it"""
        if equivalent and not any(definition.start() in block for block in blocks):
            raise NotLanded(f"{relative} defines {what} further down: moved into the NON_MATCHING block, it would leave "
                            f"the default build")
        return below.replace(definition.group(0), "", 1)

    # The original compiler addresses a variable through $gp only in the file that defines it (docs/COMPILER.md). A
    # definition the file already has goes, or moves up to this function when the file has it further down; an
    # uninitialised one stays, a common the split's symbol resolves; an initialised one the file doesn't have is data of
    # another file, which can't be read through $gp from this one
    defined = {match.group(1): match for match in DATA_DEFINITION.finditer(original)}
    for definition in DATA_DEFINITION.finditer(text):
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
    # A structure the file defines: the candidate's copy of it goes, or moves up to this function when the file has it
    # further down, and a different one can't land
    types = {type_name(match): match for match in TYPE_DEFINITION.finditer(original)}
    for definition in TYPE_DEFINITION.finditer(text):
        kind = type_name(definition)
        if kind not in types:
            # A structure of its own: it lands with the function
            continue
        elif " ".join(types[kind].group(0).split()) != " ".join(definition.group(0).split()):
            raise NotLanded(f"defines {kind} otherwise than {relative}")
        elif types[kind].start() < insertion:
            text = text.replace(definition.group(0), "")
        else:
            below = moved_up(types[kind], kind)
    # One prototype per game function, in include/game.h: one it has goes, a new one of basic types moves there
    header = GAME_H.read_text()
    declared = {prototype.group("name"): prototype.group("declaration") for prototype in PROTOTYPE.finditer(header)}
    added = []
    for prototype in PROTOTYPE.finditer(text):
        callee = prototype.group("name")
        if callee == name:
            # The function's own forward declaration stays with it
            continue
        elif callee in declared and signature_of(prototype.group("declaration")) != signature_of(declared[callee]):
            # The candidate was judged with its own prototype; include/game.h's would compile the call otherwise
            raise NotLanded(f"declares `{prototype.group('declaration')}`, include/game.h `{declared[callee]}`")
        elif callee in declared:
            text = text.replace(prototype.group(0), "")
        elif int(callee[FUNCTION_ADDRESS_START:], HEXADECIMAL) >= SDK_START:
            # An SDK function not identified yet: include/sdk.h's once it is
            continue
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
    if equivalent:
        body = f"#ifdef NON_MATCHING\n{body}#else\n{include}#endif\n"
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
    if equivalent:
        candidates = [SCRATCH / name / "equivalent.c", NOTES / f"{name}{EQUIVALENT_NOTES}.c", NOTES / f"{name}.c"]
        refused = EQUIVALENT_REFUSED
    else:
        candidates = [SCRATCH / name / "match.c", NOTES / f"{name}.c"]
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
        agrees, compared, summary = equivalence(source, name, LANDING_TRIALS, LANDING_SEED)
        if not agrees:
            error = summary
        elif compared < MIN_COMPARED_SHARE * LANDING_TRIALS:
            error = f"{summary}: fewer than {MIN_COMPARED_SHARE:.0%} of the trials compared"
        else:
            # Every trial agrees, enough of them compared
            pass
    if error is not None:
        source.write_text(original)
        GAME_H.write_text(header)
        subprocess.run([sys.executable, "configure.py"], cwd=HERE, capture_output=True)
        record(name, refused, reason=error)
        return False
    if equivalent:
        record(name, EQUIVALENT_MODE, summary=summary)
        message = f"feat: add equivalent C for {name}"
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
                key, success = f"{attempt['model']}, {EQUIVALENT_MODE}", "equivalent, agrees"
            else:
                key, success = attempt["model"], "matched"
            stats = models.setdefault(key, {"runs": 0, "succeeded": 0, "cost_usd": 0.0, "success": success})
            stats["runs"] += 1
            stats["succeeded"] += attempt["outcome"] == success
            stats["cost_usd"] += attempt.get("cost_usd") or 0
        if near_miss(entry):
            near.append(f"{name} ({entry['summary']})")
        elif entry["outcome"] in ("matched, blocked", "matched, not landed", EQUIVALENT_REFUSED):
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
        print(f"{model}: {stats['succeeded']} {stats['success']} of {stats['runs']} runs, ${stats['cost_usd']:.2f}")
    print("near misses and C waiting to land:")
    for line in near:
        print(f"  {line}")


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("command", choices=["next", "prepare", "run", "land", "loop", "report"])
    parser.add_argument("function", nargs="?")
    parser.add_argument("--minutes", type=float, default=DEFAULT_MINUTES, help="the agent's time limit per function")
    parser.add_argument("--model", help="the agent's model (default: Claude Code's)")
    parser.add_argument("--count", type=int, default=1, help="loop: how many functions")
    parser.add_argument("--retry-model", help="loop: a second model for near misses of the first")
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
            print(f"{name}: {load_ledger()[name]['outcome']}", flush=True)
        commit_records()
    elif arguments.command == "loop":
        for _ in range(arguments.count):
            name = next_function()
            if name is None:
                print("nothing left to try")
                break
            matched = run(name, arguments.minutes, arguments.model)
            # The second model only where the first came close: elsewhere it rarely matched, at several times the cost
            if not matched and arguments.retry_model and near_miss(load_ledger()[name]):
                matched = run(name, arguments.minutes, arguments.retry_model)
            landed = matched and land(name)
            print(f"{name}: {'landed' if landed else load_ledger()[name]['outcome']}", flush=True)
        commit_records()
    elif arguments.function is None:
        sys.exit(f"{arguments.command} needs a FUNCTION")
    elif arguments.command == "prepare":
        print(prepare(arguments.function, arguments.equivalent))
    elif arguments.command == "run" and arguments.equivalent:
        agrees = run_equivalent(arguments.function, arguments.minutes, arguments.model)
        print(f"{arguments.function}: {load_ledger()[arguments.function]}")
        sys.exit(0 if agrees else 1)
    elif arguments.command == "run":
        matched = run(arguments.function, arguments.minutes, arguments.model)
        print(f"{arguments.function}: {load_ledger()[arguments.function]}")
        sys.exit(0 if matched else 1)
    elif arguments.command == "land":
        sys.exit(0 if land(arguments.function, arguments.equivalent) else 1)
    else:
        sys.exit(f"unknown command {arguments.command}")


if __name__ == "__main__":
    main()
