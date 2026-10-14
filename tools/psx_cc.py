#!/usr/bin/env python3
"""Compiles a C file the way PsyQ did: the PSX GCC's cpp and cc1 (tools/toolchain/gcc-<version>-psx), maspsx standing in for
ASPSX, then GNU as. The one compile path the probes, decomp-permuter and the build share.

    python tools/psx_cc.py SOURCE.c -o OUT.o [--gcc 2.7.2] [--aspsx 2.34] [--cflags "-O2 -G8"]
"""
import argparse
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
# Proven so far (docs/COMPILER.md): an ASPSX of 2.30 to 2.49; GCC 2.6.0 to 2.7.2 at -O2 -G8
DEFAULT_GCC = "2.7.2"
DEFAULT_ASPSX = "2.34"
DEFAULT_CFLAGS = "-O2 -G8"


def compiler(version):
    folder = TOOLCHAIN / f"gcc-{version}-psx"
    if not (folder / "cc1").exists():
        sys.exit(f"no GCC {version} in {TOOLCHAIN}: tools/setup_toolchain.py")
    return folder


def compile_c(gcc, aspsx, source, cflags, obj):
    """Builds obj from source with that GCC folder and ASPSX version: None, or the error's last line"""
    with tempfile.TemporaryDirectory() as folder:
        work = Path(folder)
        preprocessed = work / "source.i"
        assembly = work / "source.s"
        rewritten = work / "source.maspsx.s"
        # (command, file fed to its standard input, file its standard output goes to)
        steps = [
            ([str(gcc / "cpp"), *CPP_DEFINES, f"-I{HERE / 'include'}", str(source), str(preprocessed)], None, None),
            ([str(gcc / "cc1"), "-quiet", *cflags.split(), str(preprocessed), "-o", str(assembly)], None, None),
            ([sys.executable, str(MASPSX), f"--aspsx-version={aspsx}", "--expand-div"], assembly, rewritten),
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
                return message[-1] if message else f"{Path(command[0]).name} failed"
            if stdout is not None:
                stdout.write_text(result.stdout)
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("source")
    parser.add_argument("-o", dest="output", required=True)
    parser.add_argument("--gcc", default=DEFAULT_GCC)
    parser.add_argument("--aspsx", default=DEFAULT_ASPSX)
    parser.add_argument("--cflags", default=DEFAULT_CFLAGS)
    arguments = parser.parse_args()
    error = compile_c(compiler(arguments.gcc), arguments.aspsx, Path(arguments.source).resolve(), arguments.cflags,
                      Path(arguments.output).resolve())
    if error is not None:
        sys.exit(error)


if __name__ == "__main__":
    main()
