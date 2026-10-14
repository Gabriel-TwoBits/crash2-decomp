# Contributing

## Setup

Linux (x86-64) with Python 3.12 or later and git. You need your own copy of the game (docs/TARGET.md): a disc image of the
North American release, SCUS-94154.

1. The assembler and linker for little-endian MIPS (Debian and Ubuntu):

   ```sh
   sudo apt install binutils-mipsel-linux-gnu
   ```

2. The Python environment:

   ```sh
   python3 -m venv .venv
   .venv/bin/python -m pip install -r requirements.txt
   ```

3. The toolchain (the PSX GCC versions, objdiff, maspsx, m2c, decomp-permuter) into `tools/toolchain/`, each checked:

   ```sh
   .venv/bin/python tools/setup_toolchain.py
   ```

4. The game's executable, out of your disc image and checked against the retail SHA-1:

   ```sh
   .venv/bin/python tools/extract_disc.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue"
   ```

5. Split it and build. `configure.py` runs splat and writes `build.ninja`; ninja assembles and links
   `build/SCUS_941.54.exe` and checks its SHA-1 against the retail one:

   ```sh
   .venv/bin/python configure.py
   .venv/bin/ninja
   ```

## Rules

- **No game data in the repository**: not the disc image, not the executable, not what splat makes of it (`asm/`,
  `assets/`), not the NSF files. `.gitignore` keeps them out; check `git status` before a commit anyway.
- **Data the code defines is code**: a matched file defines its variables in C with their retail values (counters, flags,
  pointers, short strings, small tables such as a sine table), as any matching decompilation does; the bytes can't match
  without them. What stays out, in C as anywhere else, is the game's content: graphics, sound, models, levels. Transcribing a
  block of it into an array is committing game data.
- **Matched means matched**: `tools/probe_compiler.py` reports no differing word and the build's SHA-1 check passes
  (docs/WORKFLOW.md).
- **Commits**: one Conventional Commit subject line in English (`feat: match Gool_Interpret`), no body. One function or one
  coherent change per commit.
- **Docs follow findings**: a fact learned about the compiler goes into docs/COMPILER.md, about the game into the docs, with
  the evidence, not only into a commit message.

## Licence

The code in this repository is under the GNU General Public License, version 3 or (at your option) any later version
(`LICENSE`). It covers the C, the tools and the docs written here, not the game: Crash Bandicoot 2 and everything on its disc
belong to their owners.
