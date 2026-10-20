#!/usr/bin/env python3
"""Writes a copy of the player's disc image with another executable in place of SCUS_941.54, to run a build in an
emulator (tools/run_game.py): by default the equivalent track's, build/nonmatching/SCUS_941.54.exe.

    python tools/patch_disc.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue" [--exe EXE] [--out CUE]

The image is the .cue of a raw dump (MODE2/2352). The executable takes the file's sectors, or, when it outgrows them,
new sectors after the image's last (the retail file's left as they were, unreferenced), and its place and size go into
the file's directory record: the BIOS finds it by name (SYSTEM.CNF), and no other file moves. Each sector written gets
its EDC and ECC computed again, once the same routine has given the original sector's own.
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
ECC_P_SIZE = 172
ECC_Q = ECC_P + ECC_P_SIZE
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

# A directory record keeps its extent's first sector and size twice each: little-endian at 2 and 10, big-endian at 6
# and 14
RECORD_EXTENT_LITTLE = 2
RECORD_EXTENT_BIG = 6
RECORD_SIZE_LITTLE = 10
RECORD_SIZE_BIG = 14
# The primary volume descriptor's sector, and its volume space size (the volume's sectors), little- then big-endian
PRIMARY_VOLUME_SECTOR = 16
VOLUME_SPACE_LITTLE = 80
VOLUME_SPACE_BIG = 84
# A sector's header holds its absolute address: minutes, seconds and frames in BCD, 2 seconds (the pregap) past its
# number in the track
ADDRESS_SIZE = 3
PREGAP_SECTORS = 150
SECTORS_PER_SECOND = 75
SECONDS_PER_MINUTE = 60
BCD_BASE = 10
BCD_SHIFT = 4


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


def address(number):
    """The header's address of a track's sector"""
    seconds, frames = divmod(number + PREGAP_SECTORS, SECTORS_PER_SECOND)
    minutes, seconds = divmod(seconds, SECONDS_PER_MINUTE)
    return bytes(((value // BCD_BASE) << BCD_SHIFT) | (value % BCD_BASE) for value in (minutes, seconds, frames))


def single_track(cue):
    """Whether the .cue's first file holds one track only: sectors appended to it then stay in the data track"""
    text = cue.read_text(errors="replace")
    files = re.split(r'^\s*FILE\s', text, flags=re.MULTILINE)
    return len(files) > 1 and len(re.findall(r"^\s*TRACK\s", files[1], flags=re.MULTILINE)) == 1


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
    needed = (len(exe) + USER_DATA - 1) // USER_DATA
    if needed > sectors and not single_track(cue):
        sys.exit(f"the executable's {len(exe)} bytes outgrow {EXECUTABLE}'s {sectors} sectors, and {cue}'s first file "
                 "holds more than its data track to append sectors to")

    out_cue.parent.mkdir(parents=True, exist_ok=True)
    out_bin = out_cue.with_suffix(".bin")
    shutil.copyfile(track, out_bin)
    with open(out_bin, "r+b") as image:
        first_sector = read_form_1(image, record.first)
        if first_sector[HEADER:HEADER + ADDRESS_SIZE] != address(record.first):
            sys.exit(f"sector {record.first}'s header holds another address than its place in the image")
        if needed <= sectors:
            first, count = record.first, sectors
        else:
            image.seek(0, 2)
            if image.tell() % SECTOR_SIZE:
                sys.exit(f"{track} isn't whole sectors")
            first, count = image.tell() // SECTOR_SIZE, needed
        # The new extent's sectors take the old one's subheaders: a middle sector's, and the last's (end of the file)
        middle = read_form_1(image, record.first + sectors // 2)
        last = read_form_1(image, record.first + sectors - 1)
        padded = exe.ljust(count * USER_DATA, b"\x00")
        for index in range(count):
            if first == record.first:
                sector = read_form_1(image, first + index)
            elif index == count - 1:
                sector = bytearray(last)
            else:
                sector = bytearray(middle)
            sector[HEADER:HEADER + ADDRESS_SIZE] = address(first + index)
            sector[DATA:EDC] = padded[index * USER_DATA:(index + 1) * USER_DATA]
            image.seek((first + index) * SECTOR_SIZE)
            image.write(seal(sector))
        sector = read_form_1(image, record.sector)
        for (little, big), value in (((RECORD_EXTENT_LITTLE, RECORD_EXTENT_BIG), first),
                                     ((RECORD_SIZE_LITTLE, RECORD_SIZE_BIG), len(exe))):
            struct.pack_into("<I", sector, DATA + record.offset + little, value)
            struct.pack_into(">I", sector, DATA + record.offset + big, value)
        image.seek(record.sector * SECTOR_SIZE)
        image.write(seal(sector))
        volume = read_form_1(image, PRIMARY_VOLUME_SECTOR)
        if struct.unpack_from("<I", volume, DATA + VOLUME_SPACE_LITTLE)[0] < first + count:
            struct.pack_into("<I", volume, DATA + VOLUME_SPACE_LITTLE, first + count)
            struct.pack_into(">I", volume, DATA + VOLUME_SPACE_BIG, first + count)
            image.seek(PRIMARY_VOLUME_SECTOR * SECTOR_SIZE)
            image.write(seal(volume))
        else:
            # The volume already spans the executable's sectors
            pass
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
