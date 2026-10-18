#!/usr/bin/env python3
"""Plays the game from the player's disc in an emulator (PCSX-Redux, headless) with the retail executable and with the
equivalent track's, the same pad presses at the same frames for both, and compares what each displays frame by frame.

    python tools/run_game.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue" [--against retail]
                             [--baseline EXE]

The schedule boots the disc, opens the title's menu, starts a new game, skips the intro, waits out Cortex's speech in
the first warp room, walks into Turtle Woods and runs, jumps and spins there: 8,000 frames (2 minutes 13 of the game),
about 15 seconds to run, both runs at once and the emulator unthrottled. The equivalent executable (build/nonmatching/SCUS_941.54.exe, `ninja nonmatching`) goes into a
copy of the image first (tools/patch_disc.py). `--against retail` runs the retail image twice instead: the emulator is
deterministic when both runs agree on every frame. `--baseline EXE` runs that executable in place of retail: the same
equivalent build with and without a change, when the change can't be tested alone (docs/WORKFLOW.md).

Each run is checked for health: it reaches the last frame (a run without a new frame for 20 seconds is stopped: the game
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
DISPLAY_VARIABLES = ("DISPLAY", "WAYLAND_DISPLAY")
# A run stalls when its trace doesn't grow for 20 seconds (the script writes it a few times a second); it then gets 10
# seconds to quit
POLL_SECONDS = 1
STALL_SECONDS = 20
STOP_SECONDS = 10

# Frames are counted in vsyncs (60 a second) from power-on; the timings are the retail run's
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

# A trace line, as tools/run_game.lua writes it
TRACE_FIELDS = ("frame", "image hash", "program counter", "exception code")
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


def schedule_text():
    lines = [f"press {first} {first + frames - 1} {button}" for first, frames, button in PRESSES]
    lines += [f"shot {frame}" for frame, _ in CHECKPOINTS]
    lines.append(f"end {LAST_FRAME}")
    return "".join(f"{line}\n" for line in lines)


def start(name, cue):
    """Starts the emulator on cue, writing into OUT/name; returns (process, folder)"""
    folder = OUT / name
    folder.mkdir(parents=True, exist_ok=True)
    for stale in [*folder.glob("shot_*.raw"), folder / "trace.txt", folder / "memcard1.mcd", folder / "memcard2.mcd"]:
        stale.unlink(missing_ok=True)
    schedule = folder / "schedule.txt"
    schedule.write_text(schedule_text())
    environment = {**os.environ, "RUN_GAME_SCHEDULE": str(schedule), "RUN_GAME_OUT": str(folder)}
    # Without a display the emulator can't open a window, whatever its flags
    for variable in DISPLAY_VARIABLES:
        environment.pop(variable, None)
    command = [str(EMULATOR), *EMULATOR_FLAGS, "-portable", str(folder), "-iso", str(cue), "-dofile", str(SCRIPT)]
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


def health(name, status, frames):
    """The run's problems, as lines of text (none when healthy)"""
    problems = []
    if status is None:
        problems.append(f"no frame for {STALL_SECONDS} seconds: the game crashed or hung (see its emulator.log)")
    elif status != 0:
        problems.append(f"the emulator exited with status {status}")
    else:
        # The emulator quit when the script asked it to
        pass
    if len(frames) < LAST_FRAME:
        problems.append(f"stopped at frame {len(frames)} of {LAST_FRAME}")
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
    args = parser.parse_args()
    if not args.image.exists():
        sys.exit(f"{args.image} doesn't exist")
    if args.baseline is not None and not args.baseline.exists():
        sys.exit(f"{args.baseline} doesn't exist")
    if not EMULATOR.exists():
        sys.exit(f"{EMULATOR} is missing: tools/setup_toolchain.py fetches it")
    if args.against == "nonmatching":
        if not DEFAULT_EXE.exists():
            sys.exit(f"{DEFAULT_EXE} is missing: build it first (ninja nonmatching)")
        patch(args.image, DEFAULT_EXE.read_bytes(), DEFAULT_OUT)
        other = ("nonmatching", DEFAULT_OUT)
    elif args.against == "retail":
        other = ("retail_again", args.image)
    else:
        sys.exit(f"unknown --against {args.against}")

    if args.baseline is None:
        first = ("retail", args.image)
    else:
        first = ("baseline", BASELINE_OUT)
        patch(args.image, args.baseline.read_bytes(), BASELINE_OUT)
    print(f"running {first[0]} and {other[0]} to frame {LAST_FRAME}", flush=True)
    runs = [start(*first), start(*other)]
    names = [first[0], other[0]]
    statuses = [wait(*run) for run in runs]
    traces = [read_trace(folder) for _, folder in runs]

    problems = []
    for name, status, frames in zip(names, statuses, traces):
        problems += health(name, status, frames)
    for problem in problems:
        print(problem)

    common = min(len(trace) for trace in traces)
    differing = [number for number in range(1, common + 1) if traces[0][number - 1][0] != traces[1][number - 1][0]]
    if differing:
        print(f"the displays differ from frame {differing[0]} on, in {len(differing)} of {common} frames")
    else:
        print(f"the displays agree on all {common} frames")
    for frame, checkpoint in CHECKPOINTS:
        shots = [read_shot(folder, frame) for _, folder in runs]
        if None in shots:
            print(f"  {checkpoint} (frame {frame}): not reached by both")
            continue
        path = OUT / f"{checkpoint}.png"
        share = compare_shots(shots[0], shots[1], path)
        print(f"  {checkpoint} (frame {frame}): {share:.1f}% of pixels differ, {path.relative_to(HERE)}")
    sys.exit(1 if problems or differing else 0)


if __name__ == "__main__":
    main()
