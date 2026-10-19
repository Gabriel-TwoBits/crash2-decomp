#!/usr/bin/env python3
"""Plays the game from the player's disc in an emulator (PCSX-Redux, headless) with the retail executable and with the
equivalent track's, the same pad presses at the same frames for both, and compares what each displays frame by frame.

    python tools/run_game.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue" [--against retail]
                             [--baseline EXE] [--schedule {new-game,demo}] [--real-time]

By default frames are game frames, on the game clock (tools/run_game.lua): the game's time is pinned to its frames, each
lasting two vsyncs for the game whatever the code's speed, so faster or slower equivalent C plays the same game. The
schedule boots the disc, starts a new game from the title's menu, skips the intro, waits out Cortex's speech, walks into
Turtle Woods from the first warp room and runs, jumps and spins there: 3,450 game frames, about a minute and a half to
run on the emulator's interpreter, both runs at once. `--schedule demo` presses nothing instead: left at the title's
menu, the game plays its attract demo, Turtle Woods then a river level, 4,800 game frames. `--real-time` keeps the
game's own clock and counts vsyncs instead: 8,000 of them (2 minutes 13 of the game), about 15 seconds on the emulator's
recompiler, where a difference may be only timing. The equivalent executable (build/nonmatching/SCUS_941.54.exe, `ninja
nonmatching`) goes into a copy of the image first (tools/patch_disc.py). `--against retail` runs the retail image twice
instead: the emulator is deterministic when both runs agree on every frame. `--baseline EXE` runs that executable in
place of retail: the same equivalent build with and without a change, when the change can't be tested alone
(docs/WORKFLOW.md); on the game clock its ELF must sit beside it (EXE with the suffix .elf), for the clock's symbols.

Each run is checked for health: it reaches the last frame (a run without a new vsync for 20 seconds is stopped: the game
crashed or hung), its program counter stays in RAM or the BIOS, it takes no exception but interrupts and system calls,
and its display isn't frozen at the end. The two are then compared by a hash of the displayed image at every frame; at
each checkpoint both images are written side by side into build/run_game/.
Exit status 0 when both runs are healthy and display the same image at every frame, 1 otherwise.

Needs tools/setup_toolchain.py's PCSX-Redux; its own BIOS (OpenBIOS) boots the disc.
"""
import argparse
import os
import struct
import subprocess
import sys
import time
import zlib
from collections import namedtuple
from pathlib import Path

from patch_disc import DEFAULT_EXE, DEFAULT_OUT, patch

HERE = Path(__file__).resolve().parents[1]
EMULATOR = HERE / "tools" / "toolchain" / "pcsx-redux" / "usr" / "bin" / "pcsx-redux"
SCRIPT = HERE / "tools" / "run_game.lua"
OUT = HERE / "build" / "run_game"
# The image --baseline patches its executable into, beside the equivalent build's
BASELINE_OUT = DEFAULT_OUT.with_name("baseline.cue")
# Headless (no window, logs on stdout), safe mode (no settings file read), emulation started at once, OpenBIOS
EMULATOR_FLAGS = ["-cli", "-testmode", "-run"]
# The game clock's breakpoints only fire on the interpreter, with the debugger on
GAME_CLOCK_FLAGS = ["-interpreter", "-debugger"]
DISPLAY_VARIABLES = ("DISPLAY", "WAYLAND_DISPLAY")
# A run stalls when its trace doesn't grow for 20 seconds (the script writes it a few times a second); it then gets 10
# seconds to quit
POLL_SECONDS = 1
STALL_SECONDS = 20
STOP_SECONDS = 10

# On the game clock, frames are counted in game frames from power-on (Gfx_EndFrame's calls), each lasting 34 of
# g_TimerTicks for the game: two vsyncs, the length most of a real-time retail run's frames take
GAME_FRAME_TICKS = 34
GAME_CLOCK_SYMBOLS = ("Gfx_EndFrame", "Sys_TimerHandler", "g_TimerTicks")
GAME_TAP_FRAMES = 3
# A schedule on the game clock: presses (first game frame, game frames held, button), checkpoints (game frame, name),
# the last game frame, and the vsync by which a run still short of it counts as stuck (about 1.4 times retail's)
GameSchedule = namedtuple("GameSchedule", "presses checkpoints last vsync_limit")
GAME_SCHEDULES = {
    # Retail reaches game frame 3,450 by vsync 8,695
    "new-game": GameSchedule(
        presses=[
            (1050, GAME_TAP_FRAMES, "START"),  # New Game, from the title's menu
            (1350, GAME_TAP_FRAMES, "CROSS"),  # during the intro
            (2750, 100, "UP"),  # into Turtle Woods' portal, once Cortex's speech is over
            (2950, 150, "UP"),
            (3000, GAME_TAP_FRAMES, "CROSS"),
            (3050, GAME_TAP_FRAMES, "SQUARE"),
            (3100, 100, "RIGHT"),
            (3150, GAME_TAP_FRAMES, "CROSS"),
        ],
        checkpoints=[
            (300, "intro"),
            (500, "naughty_dog"),
            (1000, "title"),
            (1800, "cortex_speech"),
            (2750, "warp_room"),
            (2900, "level_entry"),
            (3100, "turtle_woods"),
            (3440, "turtle_woods_end"),
        ],
        last=3450,
        vsync_limit=12000,
    ),
    # Nothing pressed: the title's menu left alone plays the attract demo, Turtle Woods then a river level; retail
    # reaches game frame 4,800 by vsync 14,100
    "demo": GameSchedule(
        presses=[],
        checkpoints=[
            (1000, "title"),
            (2000, "demo_turtle_woods"),
            (2300, "demo_turtle_woods_end"),
            (2800, "title_again"),
            (3900, "demo_river"),
            (4300, "demo_river_middle"),
            (4600, "demo_river_end"),
            (4790, "title_last"),
        ],
        last=4800,
        vsync_limit=20000,
    ),
}
# The executables' symbols, for the game clock: the default build's ELF holds retail's bytes
RETAIL_ELF = HERE / "build" / "SCUS_941.54.elf"
NONMATCHING_ELF = DEFAULT_EXE.with_suffix(".elf")
NM = "mipsel-linux-gnu-nm"
NM_FIELDS = ("address", "type", "name")
HEXADECIMAL = 16

# In real time, frames are counted in vsyncs (60 a second) from power-on; the timings are the retail run's
LAST_FRAME = 8000
TAP_FRAMES = 6
PRESSES = [
    (3100, TAP_FRAMES, "START"),  # the title's menu
    (3400, TAP_FRAMES, "START"),  # New Game
    (3700, TAP_FRAMES, "CROSS"),  # skips the intro
    (6200, 200, "UP"),  # into Turtle Woods' portal, once Cortex's speech is over
    (7000, 300, "UP"),
    (7100, TAP_FRAMES, "CROSS"),
    (7200, TAP_FRAMES, "SQUARE"),
    (7300, 200, "RIGHT"),
    (7400, TAP_FRAMES, "CROSS"),
]
CHECKPOINTS = [
    (1500, "intro"),
    (2000, "naughty_dog"),
    (3000, "title"),
    (4500, "cortex_speech"),
    (6200, "warp_room"),
    (6800, "level_entry"),
    (7350, "turtle_woods"),
    (7950, "turtle_woods_end"),
]

# A trace line and a game frame's line, as tools/run_game.lua writes them
TRACE_FIELDS = ("frame", "image hash", "program counter", "exception code")
GAME_FRAME_FIELDS = ("game frame", "vsync", "image hash")
# Where a healthy program counter is: main RAM through its three segments (2 MB), and the BIOS ROM (512 KB)
PC_RANGES = [
    (0x00000000, 0x00200000),
    (0x80000000, 0x80200000),
    (0xA0000000, 0xA0200000),
    (0xBFC00000, 0xBFC80000),
]
# The exceptions a healthy run takes: interrupts and system calls (Cause's ExcCode)
EXPECTED_EXCEPTIONS = {0: "interrupt", 8: "syscall"}
# A display unchanged over its last 5 seconds counts as frozen
FROZEN_FRAMES = 300

# The emulator's screenshots: 15-bit BGR, or 24-bit RGB
BPP_15 = 0
BPP_24 = 1
BYTES_15 = 2
BYTES_24 = 3
CHANNEL_MASK_15 = 0x1F
GREEN_SHIFT_15 = 5
BLUE_SHIFT_15 = 10
CHANNEL_SCALE_15 = 3
# PNG: signature, 8-bit RGB, no interlace, and the "none" filter on every row
PNG_SIGNATURE = b"\x89PNG\r\n\x1a\n"
PNG_BIT_DEPTH = 8
PNG_COLOUR_RGB = 2
PNG_FILTER_NONE = b"\x00"
RGB_BYTES = 3
PAIR_GAP = 8
PERCENT = 100


def clock_symbols(elf):
    """The game clock's symbols' addresses in an executable's ELF, in GAME_CLOCK_SYMBOLS' order"""
    if not elf.exists():
        sys.exit(f"{elf} is missing: the game clock reads its symbols from it (or run with --real-time)")
    listing = subprocess.run([NM, str(elf)], capture_output=True, text=True, check=True).stdout
    found = {}
    for line in listing.splitlines():
        fields = line.split()
        if len(fields) == len(NM_FIELDS):
            found[fields[2]] = int(fields[0], HEXADECIMAL)
        else:
            # An undefined symbol, listed without an address
            continue
    missing = [name for name in GAME_CLOCK_SYMBOLS if name not in found]
    if missing:
        sys.exit(f"{elf} has no {', '.join(missing)}")
    return [found[name] for name in GAME_CLOCK_SYMBOLS]


def schedule_text(elf, schedule):
    """The schedule for tools/run_game.lua: on the game clock of the executable elf (schedule, a GameSchedule), or in
    real time when elf is None"""
    if elf is None:
        lines = [f"press {first} {first + frames - 1} {button}" for first, frames, button in PRESSES]
        lines += [f"shot {frame}" for frame, _ in CHECKPOINTS]
        lines.append(f"end {LAST_FRAME}")
    else:
        addresses = " ".join(f"{address:x}" for address in clock_symbols(elf))
        lines = [f"clock game {addresses} {GAME_FRAME_TICKS}", f"limit {schedule.vsync_limit}"]
        lines += [f"press {first} {first + frames - 1} {button}" for first, frames, button in schedule.presses]
        lines += [f"shot {frame}" for frame, _ in schedule.checkpoints]
        lines.append(f"end {schedule.last}")
    return "".join(f"{line}\n" for line in lines)


def start(name, cue, elf, schedule):
    """Starts the emulator on cue, writing into OUT/name, on elf's game clock and schedule (a GameSchedule) or in real
    time when elf is None; returns (process, folder)"""
    folder = OUT / name
    folder.mkdir(parents=True, exist_ok=True)
    for stale in [*folder.glob("shot_*.raw"), folder / "trace.txt", folder / "game_frames.txt", folder / "memcard1.mcd",
                  folder / "memcard2.mcd"]:
        stale.unlink(missing_ok=True)
    (folder / "schedule.txt").write_text(schedule_text(elf, schedule))
    environment = {**os.environ, "RUN_GAME_SCHEDULE": str(folder / "schedule.txt"), "RUN_GAME_OUT": str(folder)}
    # Without a display the emulator can't open a window, whatever its flags
    for variable in DISPLAY_VARIABLES:
        environment.pop(variable, None)
    flags = EMULATOR_FLAGS if elf is None else EMULATOR_FLAGS + GAME_CLOCK_FLAGS
    command = [str(EMULATOR), *flags, "-portable", str(folder), "-iso", str(cue), "-dofile", str(SCRIPT)]
    with open(folder / "emulator.log", "w") as log:
        process = subprocess.Popen(command, cwd=folder, stdout=log, stderr=subprocess.STDOUT, env=environment)
    return process, folder


def wait(process, folder):
    """The emulator's exit status, or None once it stalls: a crashed game stops the vsyncs, and with them the script"""
    trace = folder / "trace.txt"
    last_size = -1
    last_change = time.monotonic()
    while True:
        try:
            return process.wait(timeout=POLL_SECONDS)
        except subprocess.TimeoutExpired:
            pass
        size = trace.stat().st_size if trace.exists() else 0
        now = time.monotonic()
        if size != last_size:
            last_size = size
            last_change = now
        elif now - last_change > STALL_SECONDS:
            # SIGTERM lets the emulator quit cleanly, the script's buffered frames written
            process.terminate()
            try:
                process.wait(timeout=STOP_SECONDS)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
            return None
        else:
            # Still within the time a frame may take
            pass


def read_trace(folder):
    """[(hash, program counter, exception code)] by frame from 1"""
    path = folder / "trace.txt"
    if not path.exists():
        return []
    frames = []
    for line in path.read_text().splitlines():
        words = line.split()
        if len(words) != len(TRACE_FIELDS) or int(words[0]) != len(frames) + 1:
            sys.exit(f"{path}: unexpected line {line!r}")
        frames.append((words[1], int(words[2], 16), int(words[3])))
    return frames


def read_game_frames(folder):
    """[(hash, vsync)] by game frame from 0"""
    path = folder / "game_frames.txt"
    if not path.exists():
        return []
    frames = []
    for line in path.read_text().splitlines():
        words = line.split()
        if len(words) != len(GAME_FRAME_FIELDS) or int(words[0]) != len(frames):
            sys.exit(f"{path}: unexpected line {line!r}")
        frames.append((words[2], int(words[1])))
    return frames


def health(name, status, frames, game_frames, last_game_frame):
    """The run's problems, as lines of text (none when healthy); game_frames and last_game_frame are None in real
    time"""
    problems = []
    if status is None:
        problems.append(f"no frame for {STALL_SECONDS} seconds: the game crashed or hung (see its emulator.log)")
    elif status != 0:
        problems.append(f"the emulator exited with status {status}")
    else:
        # The emulator quit when the script asked it to
        pass
    if game_frames is None and len(frames) < LAST_FRAME:
        problems.append(f"stopped at frame {len(frames)} of {LAST_FRAME}")
    elif game_frames is not None and len(game_frames) <= last_game_frame:
        problems.append(f"stopped after {len(game_frames)} of {last_game_frame + 1} game frames, at vsync "
                        f"{len(frames)}")
    else:
        # It reached its last frame
        pass
    for number, (_, pc, _) in enumerate(frames, 1):
        if not any(low <= pc < high for low, high in PC_RANGES):
            problems.append(f"frame {number}: program counter {pc:08x} outside RAM and the BIOS")
            break
    for number, (_, _, code) in enumerate(frames, 1):
        if code not in EXPECTED_EXCEPTIONS:
            problems.append(f"frame {number}: exception code {code}")
            break
    if len(frames) >= FROZEN_FRAMES and len({image for image, _, _ in frames[-FROZEN_FRAMES:]}) == 1:
        problems.append(f"the display is frozen over the last {FROZEN_FRAMES} frames")
    return [f"{name}: {problem}" for problem in problems]


def read_shot(folder, frame):
    """(width, height, RGB rows) of the checkpoint's screenshot, or None when the run didn't reach it"""
    found = list(folder.glob(f"shot_{frame}_*.raw"))
    if len(found) != 1:
        return None
    size, bpp = found[0].stem.split("_")[2:]
    width, height = (int(value) for value in size.split("x"))
    data = found[0].read_bytes()
    rows = []
    if int(bpp) == BPP_15:
        if len(data) != width * height * BYTES_15:
            sys.exit(f"{found[0]}: {len(data)} bytes for {width}x{height} at 15 bits")
        for y in range(height):
            row = bytearray()
            for (value,) in struct.iter_unpack("<H", data[y * width * BYTES_15:(y + 1) * width * BYTES_15]):
                row += bytes(((value >> shift) & CHANNEL_MASK_15) << CHANNEL_SCALE_15
                             for shift in (0, GREEN_SHIFT_15, BLUE_SHIFT_15))
            rows.append(bytes(row))
    elif int(bpp) == BPP_24:
        if len(data) != width * height * BYTES_24:
            sys.exit(f"{found[0]}: {len(data)} bytes for {width}x{height} at 24 bits")
        rows = [data[y * width * BYTES_24:(y + 1) * width * BYTES_24] for y in range(height)]
    else:
        sys.exit(f"{found[0]}: unknown pixel format {bpp}")
    return width, height, rows


def write_png(path, width, height, rows):
    def chunk(kind, payload):
        return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload))

    header = struct.pack(">IIBBBBB", width, height, PNG_BIT_DEPTH, PNG_COLOUR_RGB, 0, 0, 0)
    pixels = zlib.compress(b"".join(PNG_FILTER_NONE + row for row in rows))
    path.write_bytes(PNG_SIGNATURE + chunk(b"IHDR", header) + chunk(b"IDAT", pixels) + chunk(b"IEND", b""))


def compare_shots(left, right, path):
    """Writes both images side by side into path; returns the share of pixels that differ, in percent"""
    def line(shot, y):
        width, height, rows = shot
        if y < height:
            return rows[y]
        return bytes(width * RGB_BYTES)

    height = max(left[1], right[1])
    gap = bytes(PAIR_GAP * RGB_BYTES)
    write_png(path, left[0] + PAIR_GAP + right[0], height, [line(left, y) + gap + line(right, y) for y in range(height)])
    if left[:2] != right[:2]:
        return PERCENT
    differing = sum(1 for left_row, right_row in zip(left[2], right[2])
                    for x in range(0, len(left_row), RGB_BYTES)
                    if left_row[x:x + RGB_BYTES] != right_row[x:x + RGB_BYTES])
    return PERCENT * differing / (left[0] * left[1])


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("image", type=Path, help="the disc image's .cue (a MODE2/2352 dump)")
    parser.add_argument("--against", choices=("nonmatching", "retail"), default="nonmatching",
                        help="what the retail run is compared with (default: %(default)s)")
    parser.add_argument("--baseline", type=Path,
                        help="an executable run in place of retail (another build of the equivalent track)")
    parser.add_argument("--schedule", choices=tuple(GAME_SCHEDULES), default="new-game",
                        help="what the game clock's runs play (default: %(default)s)")
    parser.add_argument("--real-time", action="store_true",
                        help="keep the game's own clock and count vsyncs, not game frames (new-game's own schedule)")
    args = parser.parse_args()
    if not args.image.exists():
        sys.exit(f"{args.image} doesn't exist")
    if args.baseline is not None and not args.baseline.exists():
        sys.exit(f"{args.baseline} doesn't exist")
    if not EMULATOR.exists():
        sys.exit(f"{EMULATOR} is missing: tools/setup_toolchain.py fetches it")

    def elf(path):
        """The ELF whose game clock a run uses, or None in real time"""
        if args.real_time:
            return None
        else:
            return path

    if args.against == "nonmatching":
        if not DEFAULT_EXE.exists():
            sys.exit(f"{DEFAULT_EXE} is missing: build it first (ninja nonmatching)")
        patch(args.image, DEFAULT_EXE.read_bytes(), DEFAULT_OUT)
        other = ("nonmatching", DEFAULT_OUT, elf(NONMATCHING_ELF))
    elif args.against == "retail":
        other = ("retail_again", args.image, elf(RETAIL_ELF))
    else:
        sys.exit(f"unknown --against {args.against}")

    if args.baseline is None:
        first = ("retail", args.image, elf(RETAIL_ELF))
    else:
        first = ("baseline", BASELINE_OUT, elf(args.baseline.with_suffix(".elf")))
        patch(args.image, args.baseline.read_bytes(), BASELINE_OUT)
    for _, _, path in (first, other):
        if path is not None:
            clock_symbols(path)
        else:
            # In real time: no symbols needed
            continue
    if args.real_time and args.schedule != "new-game":
        sys.exit("--real-time has only new-game's schedule")
    elif args.real_time:
        schedule = None
        unit, last, checkpoints = "frame", LAST_FRAME, CHECKPOINTS
    else:
        schedule = GAME_SCHEDULES[args.schedule]
        unit, last, checkpoints = "game frame", schedule.last, schedule.checkpoints
    print(f"running {first[0]} and {other[0]} to {unit} {last}", flush=True)
    runs = [start(*first, schedule), start(*other, schedule)]
    names = [first[0], other[0]]
    statuses = [wait(*run) for run in runs]
    traces = [read_trace(folder) for _, folder in runs]
    game_frames = [None, None] if args.real_time else [read_game_frames(folder) for _, folder in runs]

    problems = []
    for name, status, frames, played in zip(names, statuses, traces, game_frames):
        problems += health(name, status, frames, played, None if args.real_time else schedule.last)
    for problem in problems:
        print(problem)

    if args.real_time:
        images = [[image for image, _, _ in trace] for trace in traces]
        first_number = 1
    else:
        images = [[image for image, _ in played] for played in game_frames]
        first_number = 0
    common = min(len(sequence) for sequence in images)
    differing = [index + first_number for index in range(common) if images[0][index] != images[1][index]]
    if differing:
        print(f"the displays differ from {unit} {differing[0]} on, in {len(differing)} of {common} {unit}s")
    else:
        print(f"the displays agree on all {common} {unit}s")
    if not args.real_time and common > 0:
        vsyncs = [played[common - 1][1] for played in game_frames]
        print(f"  {unit} {common - 1} shown at vsync {vsyncs[0]} by {names[0]}, at {vsyncs[1]} by {names[1]}")
    for frame, checkpoint in checkpoints:
        shots = [read_shot(folder, frame) for _, folder in runs]
        if None in shots:
            print(f"  {checkpoint} ({unit} {frame}): not reached by both")
            continue
        path = OUT / f"{checkpoint}.png"
        share = compare_shots(shots[0], shots[1], path)
        print(f"  {checkpoint} ({unit} {frame}): {share:.1f}% of pixels differ, {path.relative_to(HERE)}")
    sys.exit(1 if problems or differing else 0)


if __name__ == "__main__":
    main()
