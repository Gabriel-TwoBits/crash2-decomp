# References

## Crash Bandicoot 2

- [CrashEdit](https://github.com/cbhacks/CrashEdit): the level editor for Crash 1, 2 and 3. Its code is the most complete
  description of the NSF/NSD formats, the entry types and GOOL.
- [c2c](https://github.com/ughman/c2c): Crash 2's code machine-translated from MIPS into C, run with an emulator for the
  hardware (2015 to 2020, unlicensed). Useful for a function's behaviour, not as source.
- [Crash2Recomp](https://github.com/Zumbo06/Crash2Recomp): a static recompilation of the same release (SCUS-94154), playable.
  Its notes on the executable can confirm what a function does.
- [Hidden Palace](https://hiddenpalace.org/News/Crash_Landing): prototypes of the game (June and September 1997). Their
  executables are earlier builds of the same code: a function that differs between them and retail shows what changed.
- [TCRF](https://tcrf.net/Proto:Crash_Bandicoot_2:_Cortex_Strikes_Back): what the prototypes contain.

## The same engine

- [Crash Bandicoot 1's decomp](https://github.com/khasinski/crash-bandicoot-decomp) (SCUS-94900): the same engine a year
  earlier, with its compiler findings (gcc 2.5.7 and 2.6.3, `-O2 -G8`, maspsx `--aspsx-version=2.34`). No licence is stated,
  so its code isn't copied here; its findings and method are facts to verify for this game.

## Tools and method

- [splat](https://github.com/ethteck/splat), [spimdisasm](https://github.com/Decompollaborate/spimdisasm),
  [maspsx](https://github.com/mkst/maspsx), [objdiff](https://github.com/encounter/objdiff),
  [m2c](https://github.com/matt-kempster/m2c), [decomp-permuter](https://github.com/simonlindholm/decomp-permuter),
  [decompals/old-gcc](https://github.com/decompals/old-gcc) (the PSX GCC builds), [decomp.me](https://decomp.me).
- [Can LLMs really do matching decompilation?](https://gambiconf.substack.com/p/can-llms-really-do-matching-decompilation):
  an agent with m2c, objdiff and decomp-permuter on 60 functions of two games, and what worked.
- [Retro Reversing: PS1 games with debug symbols](https://www.retroreversing.com/ps1-debug-symbols).

## The hardware

- [psx-spx](https://psx-spx.consoledev.net/): the PlayStation's hardware, documented from tests. Its GTE chapter (the
  registers, RTPS/RTPT and the division) is what `tools/gte.py` and `include/gte.h` model.
