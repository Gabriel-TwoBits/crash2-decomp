#!/usr/bin/env python3
"""The matching loop (docs/WORKFLOW.md): a scratch directory per function, an agent (headless Claude Code) working in it,
and the result checked and landed by this script, never taken on the agent's word.

    python tools/match.py next                     the next function to try
    python tools/match.py prepare FUNCTION         build/match/FUNCTION/: target, m2c draft, brief, scripts, CLAUDE.md
    python tools/match.py run FUNCTION [--minutes 30] [--model MODEL]
    python tools/match.py land FUNCTION            match.c into src/game.c, build, commit when the SHA-1 holds
    python tools/match.py loop [--count N] [--minutes 30] [--model MODEL]

Each outcome goes into build/match/ledger.json. The agent may only read, edit inside its scratch directory and run its
scratch's score.sh and permute.sh (tools/match/CLAUDE.md has its instructions).
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
LEDGER = SCRATCH / "ledger.json"
TEMPLATE = HERE / "tools" / "match" / "CLAUDE.md"
GAME_C = HERE / "src" / "game.c"
NONMATCHINGS = HERE / "asm" / "nonmatchings" / "game"
M2C = HERE / "tools" / "toolchain" / "m2c" / "m2c.py"
M2C_MACROS = HERE / "tools" / "toolchain" / "m2c" / "m2c_macros.h"
ASSEMBLY_PREAMBLE = '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text\n'
INCLUDE_LINE = 'INCLUDE_ASM("asm/nonmatchings/game", {name});\n'
NONMATCHING = re.compile(r"^nonmatching (\w+), (0x[0-9A-Fa-f]+)$", re.MULTILINE)
CALL = re.compile(r"\bjal\s+(\w+)")
SYMBOL = re.compile(r"%(?:hi|lo|gp_rel)\((\w+)\)")
MATCHED = re.compile(r"^0 of \d+ words differ$", re.MULTILINE)
INLINE_ASSEMBLY = re.compile(r"\b(__asm__|asm)\s*\(")
SECONDS_PER_MINUTE = 60
MILLISECONDS_PER_MINUTE = 60000
DEFAULT_MINUTES = 30
# Below this a function is a stub or a thunk: nothing to learn, and splat already writes the empty ones
MIN_SIZE = 0x20
EXECUTABLE_MODE = 0o755
AGENT_TOOLS = ["Read", "Edit", "Write", "Glob", "Grep", "Bash(./score.sh:*)", "Bash(./permute.sh:*)"]
PROMPT = ("Match the function described in CLAUDE.md and BRIEF.md in this directory, following CLAUDE.md's loop and rules. "
          "When you stop, leave match.c or best.c and NOTES.md as CLAUDE.md says.")


def load_ledger():
    if LEDGER.exists():
        return json.loads(LEDGER.read_text())
    return {}


def record(name, outcome, **details):
    """The function's latest outcome, keeping what earlier steps recorded (the run's model and cost when it lands)"""
    ledger = load_ledger()
    ledger[name] = {**ledger.get(name, {}), "outcome": outcome, "time": time.strftime("%Y-%m-%d %H:%M"), **details}
    LEDGER.parent.mkdir(parents=True, exist_ok=True)
    LEDGER.write_text(json.dumps(ledger, indent=2) + "\n")


def unmatched():
    """{name: size} of the game's functions still included from the split"""
    included = re.findall(r'INCLUDE_ASM\("asm/nonmatchings/game", (\w+)\);', GAME_C.read_text())
    sizes = {}
    for name in included:
        match = NONMATCHING.search((NONMATCHINGS / f"{name}.s").read_text())
        if match is None:
            sys.exit(f"{name}.s has no nonmatching line: configure.py splits the executable")
        sizes[name] = int(match.group(2), 16)
    return sizes


def next_function():
    """The smallest unmatched function of MIN_SIZE or more not tried yet"""
    tried = load_ledger()
    candidates = sorted((size, name) for name, size in unmatched().items() if size >= MIN_SIZE and name not in tried)
    if not candidates:
        return None
    return candidates[0][1]


def callers(name):
    """The functions whose assembly calls this one, in the split and in src/"""
    found = [path.stem for path in sorted(NONMATCHINGS.glob("*.s")) if re.search(rf"\bjal\s+{name}\b", path.read_text())]
    if re.search(rf"\b{name}\s*\(", GAME_C.read_text().replace(INCLUDE_LINE.format(name=name), "")):
        found.append("(called from C in src/game.c)")
    return found


def m2c_context(folder):
    """The project's declarations for m2c: src/game.c preprocessed with INCLUDE_ASM left out"""
    context = folder / "context.c"
    result = subprocess.run(["cpp", "-P", "-DM2CTX", f"-I{HERE / 'include'}", str(GAME_C)], capture_output=True, text=True)
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

    context = m2c_context(folder)
    draft = subprocess.run([sys.executable, str(M2C), "--target", "mipsel-gcc-c", "--valid-syntax", "--context",
                            str(context), str(target)], capture_output=True, text=True)
    shutil.copy2(M2C_MACROS, folder / "m2c_macros.h")
    (folder / "base.c").write_text('#include "common.h"\n#include "sdk.h"\n#include "m2c_macros.h"\n\n' + draft.stdout)

    size = len(re.findall(r"/\* [0-9A-F]+ [0-9A-F]{8} [0-9A-F]{8} \*/", assembly)) * 4
    calls = sorted(set(CALL.findall(assembly)))
    symbols = sorted(set(SYMBOL.findall(assembly)))
    brief = [
        f"# {name}",
        "",
        f"- Size: {size:#x} bytes",
        f"- Called by: {', '.join(callers(name)) or 'nothing found (a callback or a table entry, maybe)'}",
        f"- Calls: {', '.join(calls) or 'nothing'}",
        f"- Data: {', '.join(symbols) or 'none by name'}",
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


def run(name, minutes, model):
    folder = prepare(name)
    command = ["claude", "-p", PROMPT, "--permission-mode", "acceptEdits", "--allowedTools", *AGENT_TOOLS,
               "--output-format", "stream-json", "--verbose", "--no-session-persistence"]
    if model:
        command += ["--model", model]
    with open(folder / "session.jsonl", "w") as log:
        # Its own process group, so a time limit stops the agent and whatever it started
        agent = subprocess.Popen(command, cwd=folder, stdout=log, stderr=subprocess.STDOUT, start_new_session=True)
        try:
            agent.wait(timeout=minutes * SECONDS_PER_MINUTE)
        except subprocess.TimeoutExpired:
            os.killpg(agent.pid, signal.SIGTERM)
            agent.wait()
    session = {"model": model or "default", **session_result(folder)}

    candidate = folder / "match.c"
    if candidate.exists():
        if INLINE_ASSEMBLY.search(candidate.read_text()):
            record(name, "rejected", reason="inline assembly in match.c", **session)
            return False
        matched, summary = score(folder, candidate)
        if matched:
            record(name, "matched", summary=summary, **session)
            return True
        record(name, "rejected", reason=f"match.c doesn't match: {summary}", **session)
        return False
    best = folder / "best.c"
    summary = score(folder, best)[1] if best.exists() else "no attempt left"
    record(name, "gave up", summary=summary, **session)
    return False


def land(name):
    """match.c's declarations and function in place of the INCLUDE_ASM line; kept only when the build's SHA-1 holds"""
    candidate = SCRATCH / name / "match.c"
    if not candidate.exists():
        sys.exit(f"{candidate} is missing")
    original = GAME_C.read_text()
    include = INCLUDE_LINE.format(name=name)
    if include not in original:
        sys.exit(f"{name} isn't included from the split in {GAME_C}")
    # Its #include lines are game.c's already; a declaration game.c already has isn't repeated
    present = set(line.strip() for line in original.splitlines())
    kept = [line for line in candidate.read_text().splitlines()
            if not line.startswith("#include") and not (line.strip().startswith("extern") and line.strip() in present)]
    GAME_C.write_text(original.replace(include, "\n".join(kept).strip() + "\n"))

    steps = [[sys.executable, "configure.py"], [str(HERE / ".venv" / "bin" / "ninja")]]
    for step in steps:
        result = subprocess.run(step, cwd=HERE, capture_output=True, text=True)
        if result.returncode != 0:
            GAME_C.write_text(original)
            subprocess.run([sys.executable, "configure.py"], cwd=HERE, capture_output=True)
            record(name, "matched, not landed", reason=(result.stdout + result.stderr).strip().splitlines()[-1])
            return False
    subprocess.run(["git", "add", "src/game.c"], cwd=HERE, check=True)
    subprocess.run(["git", "commit", "-q", "-m", f"feat: match {name}"], cwd=HERE, check=True)
    record(name, "landed")
    return True


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("command", choices=["next", "prepare", "run", "land", "loop"])
    parser.add_argument("function", nargs="?")
    parser.add_argument("--minutes", type=float, default=DEFAULT_MINUTES, help="the agent's time limit per function")
    parser.add_argument("--model", help="the agent's model (default: Claude Code's)")
    parser.add_argument("--count", type=int, default=1, help="loop: how many functions")
    arguments = parser.parse_args()

    if arguments.command == "next":
        print(next_function() or "nothing left to try")
    elif arguments.command == "loop":
        for _ in range(arguments.count):
            name = next_function()
            if name is None:
                print("nothing left to try")
                break
            print(f"{name}: matched and landed" if run(name, arguments.minutes, arguments.model) and land(name)
                  else f"{name}: {load_ledger()[name]['outcome']}", flush=True)
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
