# CLAUDE.md

A matching decompilation of Crash Bandicoot 2 (PlayStation, USA, SCUS-94154): C that compiles to the retail executable byte
for byte. Read before working: README.md, docs/ROADMAP.md (where the project is), docs/WORKFLOW.md (how a function gets
matched), docs/COMPILER.md (what is proven about the toolchain), docs/TARGET.md (the executable's facts).

## Hard rules

- **Never commit game data.** Not `orig/`, not `asm/` or `assets/` (splat's output is the retail code and data), not NSF/NSD
  files, not excerpts of the executable's bytes or disassembly pasted into docs, commits or issues beyond a few instructions
  needed to explain a finding. Run `git status` before every commit and look at what is staged.
- **Matched means proven.** A function is matched only when objdiff reports 100% for it and the whole executable's SHA-1 is
  still the retail one, both run in this session. Never report a match from reading the code, from "equivalent" logic, or from
  a partial diff.
- **Never bend the target.** Don't edit the assembly, the expected objects or the linker script to make C match, and don't
  link a retail object in place of a compiled one.
- **Don't copy code from projects without a compatible licence.** c2c, Crash2Recomp, CrashEdit and the Crash Bandicoot 1
  decomp (docs/REFERENCES.md) are references for understanding behaviour and formats; write this project's C from the
  assembly. This repository is GPL-3.0-or-later.
- **Compiler facts need proof.** A claim about the compiler version, flags or assembler goes into docs/COMPILER.md with the
  function that proves it and what was tried, never as an assumption.

## Building

`.venv/bin/python configure.py` (splits with splat, writes `build.ninja`), then `.venv/bin/ninja`, which builds
`build/SCUS_941.54.exe` and fails unless its SHA-1 is the retail one. Run it after every change; a red check is never
committed.

## Working on functions

- Follow docs/WORKFLOW.md's loop: m2c draft, refine against the diff, decomp-permuter on near misses, objdiff to check.
- Most matches come in the first few attempts. After about three attempts with no progress in the diff, stop, leave the
  function as assembly, and note what was tried (the closest score and what differed) rather than looping.
- Matching C keeps the original's shape even where it breaks the usual style (a missing `else`, an odd temporary): output
  bytes come first. Style applies where it costs nothing in the output: named constants (`#define`, `enum`), meaningful
  names once a purpose is clear (docs/WORKFLOW.md, "Naming"), succinct comments.
- Names follow understanding: keep splat's `func_XXXXXXXX` / `D_XXXXXXXX` until the purpose is clear from the code and its
  callers. A wrong name costs more than an address.

## Tools and scripts

- Python tools live in `tools/`, run with `.venv/bin/python`. They follow the usual rules fully: exhaustive branching,
  named constants, no nested ternaries, defensive checks on files and downloads (pinned versions, SHA-256).
- Downloaded tools go in `tools/toolchain/` (ignored), fetched by `tools/setup_toolchain.py` at pinned versions. A new tool
  is added there with its version and hash, not installed by hand.

## Docs

Docs record findings with their evidence, in English, impersonal and succinct. When a milestone in docs/ROADMAP.md is done
or its plan changes, update it in the same change.
