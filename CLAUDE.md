# CLAUDE.md

## Starting a session

Where the project stands and what comes next: docs/ROADMAP.md (its "Next" list, in order). What the loop has done:
`.venv/bin/python tools/match.py report`, and `git log --oneline`. Build first (`.venv/bin/python configure.py`, then
`.venv/bin/ninja`) to see the check pass before changing anything. A session takes one item of the "Next" list, or runs
the loop (docs/WORKFLOW.md, "Running the loop unattended"), and updates docs/ROADMAP.md with what it did.

A matching decompilation of Crash Bandicoot 2 (PlayStation, USA, SCUS-94154): C that compiles to the retail executable byte
for byte. Read before working: README.md, docs/ROADMAP.md (where the project is), docs/WORKFLOW.md (how a function gets
matched), docs/COMPILER.md (what is proven about the toolchain), docs/TARGET.md (the executable's facts).

## Hard rules

- **Never commit a path of this machine** (an absolute path, a home folder): not in code, docs, notes or the ledger.
  Run `git grep -nF "$PWD"` from the root before every push; `tools/match.py` refuses to commit staged changes that
  hold one.
- **Never commit game data.** Not `orig/`, not `asm/` or `assets/` (splat's output is the retail code and data), not NSF/NSD
  files, not excerpts of the executable's bytes or disassembly pasted into docs, commits or issues beyond a few instructions
  needed to explain a finding. A file's variables defined in C with their retail values are code, not game data;
  the game's content transcribed into arrays is (CONTRIBUTING.md). Run `git status` before every commit and look at what
  is staged.
- **Matched means proven.** A function is matched only when `tools/probe_compiler.py` reports no differing word for it and
  the build's SHA-1 check passes with it in `src/`, both run in this session. Never report a match from reading the code,
  from "equivalent" logic, or from a partial diff.
- **Equivalent means tested.** C that behaves like a function without matching it lives only under `#ifdef NON_MATCHING`,
  its `INCLUDE_ASM` under `#else` (docs/WORKFLOW.md, "The equivalent track"), and the default build stays green. Such a
  function is called equivalent, never matched, and only once `tools/equiv.py` (to be written) agrees on every trial.
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
committed. `.venv/bin/ninja nonmatching` builds the equivalent track's executable, unchecked (docs/WORKFLOW.md).

## Working on functions

- `tools/match.py` runs the per-function loop with a headless agent (docs/WORKFLOW.md); a run costs the user's Claude
  usage, so batches of more than a few functions need their go-ahead.
- Probe a draft with `tools/probe_compiler.py FILE.c FUNCTION` (every compiler and assembler version) or `--diff 2.7.2`
  (one side by side); `tools/permute.py FILE.c FUNCTION --minutes N` runs decomp-permuter on it. Drafts live in
  `build/` (ignored), never in `src/` until they match.

- Follow docs/WORKFLOW.md's loop: m2c draft, refine against the diff, decomp-permuter on near misses, the probe and the build's SHA-1 to check.
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
