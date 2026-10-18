#!/usr/bin/env python3
"""Compiles a C file the way PsyQ did: the PSX GCC's cpp and cc1 (tools/toolchain/gcc-<version>-psx), maspsx standing in for
ASPSX, then GNU as. The one compile path the probes, decomp-permuter and the build share.

    python tools/psx_cc.py SOURCE.c -o OUT.o [--gcc 2.7.2] [--aspsx 2.34] [--cflags "-O2 -G8 -mcpu=r8000"] [-D NAME]
"""
import argparse
import re
import sys
import tempfile
from pathlib import Path
import subprocess

HERE = Path(__file__).resolve().parents[1]
TOOLCHAIN = HERE / "tools" / "toolchain"
MASPSX = TOOLCHAIN / "maspsx" / "maspsx.py"
AS = "mipsel-linux-gnu-as"
ASFLAGS = ["-EL", "-march=r3000", "-mtune=r3000", "-no-pad-sections", "-O1", "-G0", f"-I{HERE / 'include'}"]
CPP_DEFINES = ["-undef", "-D__GNUC__=2", "-Dmips", "-D__mips__", "-D__mips", "-D_LANGUAGE_C", "-DLANGUAGE_C"]
# Proven so far (docs/COMPILER.md): an ASPSX of 2.30 to 2.49; GCC 2.7.2 at -O2 -G8, scheduled as for -mcpu=r8000
DEFAULT_GCC = "2.7.2"
DEFAULT_ASPSX = "2.34"
DEFAULT_CFLAGS = "-O2 -G8 -mcpu=r8000"
# maspsx addresses through $gp what the file defines in .sdata/.sbss (or as a common) and nothing else, as ASPSX did
# (docs/COMPILER.md); commons stay commons, so a tentative definition resolves to the split's symbol
MASPSX_FLAGS = ["--expand-div", "-G8", "--use-comm-section"]
# A C file's data ends on a word: each file's data starts on one in the original layout, and the linker script's
# SUBALIGN(2) would pack the next file's against a section of odd size
DATA_END = "".join(f"\t.section {section}\n\t.align 2\n" for section in (".rodata", ".data", ".sdata"))
# maspsx writes ASPSX's `li $r,0` as `ori $r,0`, which GNU as reads as `ori $r,$r,0`; ASPSX 2.34 assembles it as
# `ori $r,$zero,0` (maspsx's own fixture, aspsx/fixtures/expand_li.yaml; docs/COMPILER.md, "Loading zero")
MASPSX_LOAD_ZERO = re.compile(r"^(\s*ori\s+)(\$\w+),0$", re.MULTILINE)
# A compiler's or assembler's message on a line of its input (`FILE:LINE: ...`) that isn't a warning
SOURCE_ERROR = re.compile(r":\d+: (?!warning: )")


def compiler(version):
    folder = TOOLCHAIN / f"gcc-{version}-psx"
    if not (folder / "cc1").exists():
        sys.exit(f"no GCC {version} in {TOOLCHAIN}: tools/setup_toolchain.py")
    return folder


def cpp_command(gcc, source, output, defines=()):
    """The preprocessor's command line, source into output with the macros named in defines set; run from the root"""
    return [str(gcc / "cpp"), *CPP_DEFINES, *(f"-D{name}" for name in defines), f"-I{HERE / 'include'}", str(source),
            str(output)]


def compile_c(gcc, aspsx, source, cflags, obj, defines=()):
    """Builds obj from source with that GCC folder and ASPSX version, the macros named in defines set: None, or the
    failing step's first error on a line of its input, else its last line"""
    with tempfile.TemporaryDirectory() as folder:
        work = Path(folder)
        preprocessed = work / "source.i"
        assembly = work / "source.s"
        rewritten = work / "source.maspsx.s"
        # (command, file fed to its standard input, file its standard output goes to)
        steps = [
            (cpp_command(gcc, source, preprocessed, defines), None, None),
            ([str(gcc / "cc1"), "-quiet", *cflags.split(), str(preprocessed), "-o", str(assembly)], None, None),
            ([sys.executable, str(MASPSX), f"--aspsx-version={aspsx}", *MASPSX_FLAGS], assembly, rewritten),
            ([AS, *ASFLAGS, "-o", str(obj), str(rewritten)], None, None),
        ]
        for command, stdin, stdout in steps:
            # From the repository's root: INCLUDE_ASM's paths (asm/nonmatchings/...) are relative to it
            if stdin is None:
                result = subprocess.run(command, capture_output=True, text=True, stdin=subprocess.DEVNULL, cwd=HERE)
            else:
                with open(stdin) as feed:
                    result = subprocess.run(command, capture_output=True, text=True, stdin=feed, cwd=HERE)
            if result.returncode != 0:
                message = result.stderr.strip().splitlines()
                # The first error says more than what follows it (a warning, `In function ...`, a later error it caused)
                errors = [line for line in message if SOURCE_ERROR.search(line)]
                if errors:
                    return errors[0]
                elif message:
                    return message[-1]
                else:
                    return f"{Path(command[0]).name} failed"
            if stdout == rewritten:
                rewritten.write_text(MASPSX_LOAD_ZERO.sub(r"\1\2,$zero,0", result.stdout) + DATA_END)
            elif stdout is not None:
                stdout.write_text(result.stdout)
            else:
                # The step wrote its own output file
                pass
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("source")
    parser.add_argument("-o", dest="output", required=True)
    parser.add_argument("--gcc", default=DEFAULT_GCC)
    parser.add_argument("--aspsx", default=DEFAULT_ASPSX)
    parser.add_argument("--cflags", default=DEFAULT_CFLAGS)
    parser.add_argument("-D", dest="defines", action="append", default=[], metavar="NAME",
                        help="a macro to define, NON_MATCHING for the equivalent build")
    arguments = parser.parse_args()
    error = compile_c(compiler(arguments.gcc), arguments.aspsx, Path(arguments.source).resolve(), arguments.cflags,
                      Path(arguments.output).resolve(), arguments.defines)
    if error is not None:
        sys.exit(error)


if __name__ == "__main__":
    main()
