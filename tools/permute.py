#!/usr/bin/env python3
"""Runs decomp-permuter on one function: build/permuter/<FUNCTION>/ gets the C (base.c, preprocessed), the retail function
assembled from the split (target.o), a compile.sh calling tools/psx_cc.py, and settings.toml. The permuter writes each better
candidate there as output-<score>-<n>/. The permuter's score leaves out nops, which matter on the R3000 (the ones ASPSX puts
after mflo/mfhi), so every candidate is scored again with tools/probe_compiler.py's count, and that count is what is
reported.

    python tools/permute.py FILE.c FUNCTION [--gcc 2.7.2] [--aspsx 2.34] [--cflags "-O2 -G8 -mcpu=r8000"] [-j 4] [--minutes 10]
"""
import argparse
import os
import shlex
import shutil
import signal
import subprocess
import sys
from pathlib import Path

from probe_compiler import compiled_words, retail_assembly, retail_words, score
from psx_cc import AS, ASFLAGS, DEFAULT_ASPSX, DEFAULT_CFLAGS, DEFAULT_GCC, HERE, compile_c, compiler

PERMUTER = HERE / "tools" / "toolchain" / "decomp-permuter" / "permuter.py"
OUT = HERE / "build" / "permuter"
ASSEMBLY_PREAMBLE = '.include "macro.inc"\n.set noat\n.set noreorder\n.section .text\n'
OBJDUMP_COMMAND = "mipsel-linux-gnu-objdump -drz -m mips:3000"
SECONDS_PER_MINUTE = 60
DEFAULT_MINUTES = 10
DEFAULT_THREADS = 4
BEST_SHOWN = 3
# rwxr-xr-x: compile.sh runs
EXECUTABLE_MODE = 0o755


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("source")
    parser.add_argument("function")
    parser.add_argument("--gcc", default=DEFAULT_GCC)
    parser.add_argument("--aspsx", default=DEFAULT_ASPSX)
    parser.add_argument("--cflags", default=DEFAULT_CFLAGS)
    parser.add_argument("-j", dest="threads", type=int, default=DEFAULT_THREADS)
    parser.add_argument("--minutes", type=float, default=DEFAULT_MINUTES, help="stop after this long")
    arguments = parser.parse_args()
    source = Path(arguments.source).resolve()

    folder = OUT / arguments.function
    if folder.exists():
        shutil.rmtree(folder)
    folder.mkdir(parents=True)
    # The permuter parses base.c with pycparser: the includes resolved here, the macros kept to what the code uses
    preprocessed = subprocess.run(["cpp", "-P", "-nostdinc", f"-I{HERE / 'include'}", f"-I{source.parent}", str(source)],
                                  capture_output=True, text=True)
    if preprocessed.returncode != 0:
        sys.exit(preprocessed.stderr)
    (folder / "base.c").write_text(preprocessed.stdout)
    target = folder / "target.s"
    target.write_text(ASSEMBLY_PREAMBLE + retail_assembly(arguments.function))
    subprocess.run([AS, *ASFLAGS, "-o", str(folder / "target.o"), str(target)], check=True)
    compile_script = folder / "compile.sh"
    compile_script.write_text(
        "#!/bin/sh\n"
        f"exec {shlex.quote(sys.executable)} {shlex.quote(str(HERE / 'tools' / 'psx_cc.py'))} --gcc {arguments.gcc} "
        f"--aspsx {arguments.aspsx} --cflags {shlex.quote(arguments.cflags)} \"$@\"\n")
    compile_script.chmod(EXECUTABLE_MODE)
    (folder / "settings.toml").write_text(
        f'func_name = "{arguments.function}"\ncompiler_type = "gcc"\nobjdump_command = "{OBJDUMP_COMMAND}"\n')

    # Its own process group: the permuter's workers are its children, and they stop with the group
    permuter = subprocess.Popen([sys.executable, str(PERMUTER), str(folder), "-j", str(arguments.threads), "--stop-on-zero",
                                 "--best-only", "--quiet"], start_new_session=True)
    try:
        permuter.wait(timeout=arguments.minutes * SECONDS_PER_MINUTE)
    except subprocess.TimeoutExpired:
        os.killpg(permuter.pid, signal.SIGTERM)
        permuter.wait()
        print(f"stopped after {arguments.minutes} minutes")
    # Each candidate's real count, the probe's: the permuter's own score ignores nops
    target_words = retail_words(arguments.function)
    gcc = compiler(arguments.gcc)
    counted = []
    # The base too: the permuter stops at once when it already scores it 0
    candidates = [folder / "base.c"] + [output / "source.c" for output in sorted(folder.glob("output-*"))]
    for candidate in candidates:
        obj = candidate.with_suffix(".probe.o")
        error = compile_c(gcc, arguments.aspsx, candidate, arguments.cflags, obj)
        if error is not None:
            continue
        words, masks = compiled_words(obj, arguments.function)
        counted.append((score(target_words, words, masks), candidate))
    if not counted:
        print("nothing compiled")
        return
    for differing, source in sorted(counted)[:BEST_SHOWN]:
        print(f"{differing} of {len(target_words)} words differ: {source}")

if __name__ == "__main__":
    main()
