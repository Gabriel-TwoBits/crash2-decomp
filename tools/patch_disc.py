#!/usr/bin/env python3
"""Writes a copy of the player's disc image with another executable in place of SCUS_941.54, to run a build in an
emulator (tools/run_game.py): by default the equivalent track's, build/nonmatching/SCUS_941.54.exe.

    python tools/patch_disc.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue" [--exe EXE] [--out CUE]

The image is the .cue of a raw dump (MODE2/2352). The executable takes the file's sectors, which it may not outgrow, and
its size goes into the file's directory record. Each sector rewritten gets its EDC and ECC computed again, once the same
routine has given the original sector's own.
"""
import argparse
import re
import shutil
import struct
import sys
from pathlib import Path

from extract_disc import SECTOR_LAYOUTS, USER_DATA, Disc, first_track, root_records

HERE = Path(__file__).resolve().parents[1]
EXECUTABLE = "SCUS_941.54"
DEFAULT_EXE = HERE / "build" / "nonmatching" / f"{EXECUTABLE}.exe"
DEFAULT_OUT = HERE / "build" / "disc" / "nonmatching.cue"
RAW_LAYOUT = SECTOR_LAYOUTS["MODE2/2352"]
SECTOR_SIZE = RAW_LAYOUT[0]

# A Mode 2 Form 1 sector: sync, header (address and mode), subheader (twice), user data, EDC, then the ECC's P and Q parity
SYNC = b"\x00" + b"\xff" * 10 + b"\x00"
HEADER = 12
HEADER_SIZE = 4
MODE = 15
MODE_2 = 2
SUBHEADER = 16
SUBMODE = 18
SUBMODE_FORM_2 = 0x20
DATA = 24
EDC = DATA + USER_DATA
EDC_SIZE = 4
ECC_P = EDC + EDC_SIZE
ECC_Q = 2248
# The ECC's two passes over the sector from its header: (major count, minor count, major multiplier, minor increment)
ECC_P_SHAPE = (86, 24, 2, 86)
ECC_Q_SHAPE = (52, 43, 86, 88)
# The EDC is a reflected CRC-32 of this polynomial; the ECC works in GF(2^8) over x^8 + x^4 + x^3 + x^2 + 1
EDC_POLYNOMIAL = 0xD8018001
GF_POLYNOMIAL = 0x11D
BYTE_HIGH_BIT = 0x80
BYTE_MASK = 0xFF
BITS_PER_BYTE = 8
BYTE_VALUES = 256

# A directory record keeps its extent's size twice: little-endian at 10, big-endian at 14
RECORD_SIZE_LITTLE = 10
RECORD_SIZE_BIG = 14


def edc_table():
    table = []
    for value in range(BYTE_VALUES):
        for _ in range(BITS_PER_BYTE):
            if value & 1:
                value = (value >> 1) ^ EDC_POLYNOMIAL
            else:
                value >>= 1
        table.append(value)
    return table


def ecc_tables():
    """(forward, backward): multiplication by x in GF(2^8), and the inverse of multiplying by x + 1"""
    forward = [0] * BYTE_VALUES
    backward = [0] * BYTE_VALUES
    for value in range(BYTE_VALUES):
        doubled = value << 1
        if value & BYTE_HIGH_BIT:
            doubled ^= GF_POLYNOMIAL
        forward[value] = doubled & BYTE_MASK
        backward[value ^ (doubled & BYTE_MASK)] = value
    return forward, backward


EDC_TABLE = edc_table()
ECC_FORWARD, ECC_BACKWARD = ecc_tables()


def edc(data):
    value = 0
    for byte in data:
        value = (value >> BITS_PER_BYTE) ^ EDC_TABLE[(value ^ byte) & BYTE_MASK]
    return value


def ecc_pass(source, shape):
    """One parity pass (P or Q) over source, the sector from its header on"""
    major_count, minor_count, major_multiplier, minor_increment = shape
    size = major_count * minor_count
    parity = bytearray(2 * major_count)
    for major in range(major_count):
        index = (major >> 1) * major_multiplier + (major & 1)
        ecc_a = 0
        ecc_b = 0
        for _ in range(minor_count):
            byte = source[index]
            index += minor_increment
            if index >= size:
                index -= size
            ecc_a = ECC_FORWARD[ecc_a ^ byte]
            ecc_b ^= byte
        ecc_a = ECC_BACKWARD[ECC_FORWARD[ecc_a] ^ ecc_b]
        parity[major] = ecc_a
        parity[major + major_count] = ecc_a ^ ecc_b
    return parity


def seal(sector):
    """sector (a bytearray) with its EDC and ECC computed from its subheader and data"""
    struct.pack_into("<I", sector, EDC, edc(sector[SUBHEADER:EDC]))
    # Mode 2's ECC counts the header as zeros
    source = bytearray(sector[HEADER:ECC_P])
    source[:HEADER_SIZE] = bytes(HEADER_SIZE)
    source += ecc_pass(source, ECC_P_SHAPE)
    sector[ECC_P:ECC_Q] = source[ECC_P - HEADER:]
    sector[ECC_Q:] = ecc_pass(source, ECC_Q_SHAPE)
    return sector


def read_form_1(image, number):
    """The raw sector number, checked to be Mode 2 Form 1 with the EDC and ECC this tool computes"""
    image.seek(number * SECTOR_SIZE)
    sector = bytearray(image.read(SECTOR_SIZE))
    if len(sector) != SECTOR_SIZE:
        sys.exit(f"sector {number} is past the image's end")
    if sector[:len(SYNC)] != SYNC or sector[MODE] != MODE_2:
        sys.exit(f"sector {number} isn't a Mode 2 sector")
    if sector[SUBMODE] & SUBMODE_FORM_2:
        sys.exit(f"sector {number} is Form 2, not Form 1")
    if seal(bytearray(sector)) != sector:
        sys.exit(f"sector {number}'s EDC or ECC differs from the one computed here: the image is damaged, or this tool is")
    return sector


def write_cue(cue, out_cue, out_bin):
    """out_cue: cue's tracks with its first file replaced by out_bin, any other file by its absolute path"""
    replaced = False

    def file_line(match):
        nonlocal replaced
        if not replaced:
            replaced = True
            return f'FILE "{out_bin.name}"'
        return f'FILE "{(cue.parent / match.group(1)).resolve()}"'

    out_cue.write_text(re.sub(r'FILE\s+"([^"]+)"', file_line, cue.read_text(errors="replace")))


def patch(cue, exe, out_cue):
    """Writes out_cue and its .bin: the image of cue with exe (bytes) as SCUS_941.54"""
    track, layout = first_track(cue)
    if layout != RAW_LAYOUT:
        sys.exit(f"{cue}: the first track isn't MODE2/2352, the raw dump this tool rewrites")
    disc = Disc(track, layout)
    try:
        records = root_records(disc)
    finally:
        disc.file.close()
    if EXECUTABLE not in records:
        sys.exit(f"the disc has no {EXECUTABLE}: not Crash Bandicoot 2 (USA, SCUS-94154)")
    record = records[EXECUTABLE]
    sectors = (record.size + USER_DATA - 1) // USER_DATA
    if len(exe) > sectors * USER_DATA:
        sys.exit(f"the executable's {len(exe)} bytes outgrow {EXECUTABLE}'s {sectors} sectors ({sectors * USER_DATA} "
                 "bytes) on the disc")

    out_cue.parent.mkdir(parents=True, exist_ok=True)
    out_bin = out_cue.with_suffix(".bin")
    shutil.copyfile(track, out_bin)
    padded = exe.ljust(sectors * USER_DATA, b"\x00")
    with open(out_bin, "r+b") as image:
        for index in range(sectors):
            sector = read_form_1(image, record.first + index)
            sector[DATA:EDC] = padded[index * USER_DATA:(index + 1) * USER_DATA]
            image.seek((record.first + index) * SECTOR_SIZE)
            image.write(seal(sector))
        sector = read_form_1(image, record.sector)
        struct.pack_into("<I", sector, DATA + record.offset + RECORD_SIZE_LITTLE, len(exe))
        struct.pack_into(">I", sector, DATA + record.offset + RECORD_SIZE_BIG, len(exe))
        image.seek(record.sector * SECTOR_SIZE)
        image.write(seal(sector))
    write_cue(cue, out_cue, out_bin)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("image", type=Path, help="the disc image's .cue")
    parser.add_argument("--exe", type=Path, default=DEFAULT_EXE, help="the executable to put in (default: %(default)s)")
    parser.add_argument("--out", type=Path, default=DEFAULT_OUT, help="the .cue to write (default: %(default)s)")
    args = parser.parse_args()
    if not args.image.exists():
        sys.exit(f"{args.image} doesn't exist")
    if not args.exe.exists():
        sys.exit(f"{args.exe} doesn't exist: build it first (ninja nonmatching)")
    patch(args.image, args.exe.read_bytes(), args.out)
    print(f"{args.out}: {args.exe} as {EXECUTABLE}")


if __name__ == "__main__":
    main()
