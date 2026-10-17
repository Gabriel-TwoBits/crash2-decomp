#!/usr/bin/env python3
"""Takes the files the build needs out of the player's own disc image into orig/USA/, and checks each one against the retail
release's SHA-1 (docs/TARGET.md).

    python tools/extract_disc.py "path/to/Crash Bandicoot 2 - Cortex Strikes Back (USA).cue"

The image is a .cue with its .bin (Redump's MODE2/2352 dump), or a plain 2048-byte-sector .iso.
"""
import argparse
import hashlib
import re
import struct
import sys
from collections import namedtuple
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
OUT = HERE / "orig" / "USA"
# The retail release's files (SCUS-94154), by their names on the disc
FILES = {
    "SCUS_941.54": "aabdade44a4ca863f71224daef51c45f34fb1bf8",
    "SYSTEM.CNF": "1c83a4e02b90bffa66580555ae884b0566b203cc",
}
# A track's mode: (bytes per sector in the image, offset of the 2048 bytes of user data in each)
SECTOR_LAYOUTS = {
    "MODE2/2352": (2352, 24),
    "MODE1/2352": (2352, 16),
    "MODE1/2048": (2048, 0),
}
ISO_LAYOUT = SECTOR_LAYOUTS["MODE1/2048"]
USER_DATA = 2048
# ISO 9660: the primary volume descriptor's sector, its identifier, and where the root directory's record sits in it
PRIMARY_VOLUME_SECTOR = 16
VOLUME_IDENTIFIER = b"CD001"
ROOT_RECORD_OFFSET = 156
# A directory record's fields: its length, its extent's sector and size (little-endian halves), its name's length and name
RECORD_EXTENT = 2
RECORD_SIZE = 10
RECORD_NAME_LENGTH = 32
RECORD_NAME = 33
# A file's directory record: its extent's first sector and size, and where the record sits (sector, offset in its user data)
Record = namedtuple("Record", "first size sector offset")


def first_track(cue):
    """(the .bin's path, its sector layout) from a .cue's first FILE and TRACK"""
    text = cue.read_text(errors="replace")
    file_match = re.search(r'FILE\s+"([^"]+)"', text)
    track_match = re.search(r"TRACK\s+\d+\s+(\S+)", text)
    if file_match is None or track_match is None:
        sys.exit(f"{cue}: no FILE or TRACK line")
    mode = track_match.group(1).upper()
    if mode not in SECTOR_LAYOUTS:
        sys.exit(f"{cue}: the first track is {mode}, not one of {', '.join(SECTOR_LAYOUTS)}")
    return cue.parent / file_match.group(1), SECTOR_LAYOUTS[mode]


class Disc:
    def __init__(self, path, layout):
        self.file = open(path, "rb")
        self.sector_size, self.data_offset = layout

    def sector(self, number):
        self.file.seek(number * self.sector_size + self.data_offset)
        data = self.file.read(USER_DATA)
        if len(data) != USER_DATA:
            sys.exit(f"sector {number} is past the image's end")
        return data

    def extent(self, first, size):
        sectors = (size + USER_DATA - 1) // USER_DATA
        return b"".join(self.sector(first + index) for index in range(sectors))[:size]


def root_records(disc):
    """{name without its version (";1"): Record} of the root directory's files"""
    volume = disc.sector(PRIMARY_VOLUME_SECTOR)
    if volume[1:1 + len(VOLUME_IDENTIFIER)] != VOLUME_IDENTIFIER:
        sys.exit("no ISO 9660 volume: not this game's disc image, or another sector layout")
    root = volume[ROOT_RECORD_OFFSET:]
    directory_sector = struct.unpack_from("<I", root, RECORD_EXTENT)[0]
    directory = disc.extent(directory_sector, struct.unpack_from("<I", root, RECORD_SIZE)[0])
    found = {}
    position = 0
    while position < len(directory):
        length = directory[position]
        if length == 0:
            # Records don't cross sectors: the rest of this one is padding
            position = (position // USER_DATA + 1) * USER_DATA
            continue
        name_length = directory[position + RECORD_NAME_LENGTH]
        name = directory[position + RECORD_NAME:position + RECORD_NAME + name_length].decode("ascii", "replace")
        found[name.split(";")[0]] = Record(struct.unpack_from("<I", directory, position + RECORD_EXTENT)[0],
                                           struct.unpack_from("<I", directory, position + RECORD_SIZE)[0],
                                           directory_sector + position // USER_DATA, position % USER_DATA)
        position += length
    return found


def root_files(disc):
    """{name without its version (";1"): (first sector, size)} of the root directory's files"""
    return {name: (record.first, record.size) for name, record in root_records(disc).items()}


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("image", help="the disc image's .cue (or a .iso)")
    image = Path(parser.parse_args().image)
    if not image.exists():
        sys.exit(f"{image} doesn't exist")
    if image.suffix.lower() == ".cue":
        path, layout = first_track(image)
    else:
        path, layout = image, ISO_LAYOUT
    disc = Disc(path, layout)
    files = root_files(disc)

    OUT.mkdir(parents=True, exist_ok=True)
    for name, expected in FILES.items():
        if name not in files:
            sys.exit(f"the disc has no {name}: not Crash Bandicoot 2 (USA, SCUS-94154)")
        data = disc.extent(*files[name])
        actual = hashlib.sha1(data).hexdigest()
        if actual != expected:
            sys.exit(f"{name}'s SHA-1 is {actual}, not the retail release's {expected} (docs/TARGET.md)")
        (OUT / name).write_bytes(data)
        print(f"{OUT / name}: {len(data)} bytes, SHA-1 matches")


if __name__ == "__main__":
    main()
