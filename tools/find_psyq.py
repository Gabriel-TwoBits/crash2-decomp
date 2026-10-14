#!/usr/bin/env python3
"""Looks for PsyQ's library objects in the executable by their byte signatures (lab313ru/psx_psyq_signatures, fetched by
tools/setup_toolchain.py) and reports, for each SDK version, how many of its objects are there; then, for one version, where
each found object sits and the functions it holds.

    python tools/find_psyq.py                  every version's score
    python tools/find_psyq.py --version 400    that version's objects, by address
    python tools/find_psyq.py --version 400 --symbols       its functions as splat symbol_addrs.txt lines
    python tools/find_psyq.py --version 400 --subsegments   its objects (and the gaps between them) as splat subsegments

A signature's `??` bytes are the ones relocations fill in. An object found at more than one place is reported as ambiguous
and left out of --symbols and --subsegments, as is one lying inside another (a short signature matching part of a longer
object). Two libraries with the same object (LIBC and LIBC2) give it once, under the first.
"""
import argparse
import json
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
EXECUTABLE = HERE / "orig" / "USA" / "SCUS_941.54"
SIGNATURES = HERE / "tools" / "toolchain" / "psx_psyq_signatures"
HEADER_SIZE = 0x800
# The header's fields: where the loaded block goes, and its size
LOAD_ADDRESS_FIELD = 0x18
LOAD_SIZE_FIELD = 0x1C
INSTRUCTION_SIZE = 4
WILDCARD = "??"
# The signatures also carry their objects' local branch targets and section-relative labels
LOCAL_LABEL = re.compile(r"^(loc|text)_[0-9A-F]+$")


def pattern(signature):
    """A regex for a signature's bytes, any byte where it has ??"""
    parts = []
    for token in signature.split():
        if token == WILDCARD:
            parts.append(b".")
        else:
            parts.append(re.escape(bytes([int(token, 16)])))
    return re.compile(b"".join(parts), re.DOTALL)


def load_code():
    if not EXECUTABLE.exists():
        sys.exit(f"{EXECUTABLE} is missing: tools/extract_disc.py")
    data = EXECUTABLE.read_bytes()
    address, size = struct.unpack_from("<II", data, LOAD_ADDRESS_FIELD)
    return address, data[HEADER_SIZE:HEADER_SIZE + size]


def find_version(code, folder):
    """[(library, object, [addresses], labels)] of the version's objects that are in the code"""
    found = []
    for library in sorted(folder.glob("*.json")):
        for entry in json.loads(library.read_text()):
            signature = entry.get("sig")
            if not signature:
                continue
            offsets = [match.start() for match in pattern(signature).finditer(code)
                       if match.start() % INSTRUCTION_SIZE == 0]
            if offsets:
                found.append((library.stem, entry["name"], offsets, entry.get("labels", []),
                              len(signature.split())))
    return found


def placed(found):
    """The objects found at one place, by address, none inside another: [(address offset, end offset, library, object, labels)]"""
    single = sorted((item[2][0], item[2][0] + item[4], item[0], item[1], item[3]) for item in found if len(item[2]) == 1)
    kept = []
    for start, end, library, name, labels in single:
        if kept and start < kept[-1][1]:
            continue
        kept.append((start, end, library, name, labels))
    return kept


def subsegment_name(library, name):
    """sdk/<library>/<object>, lower case, without the extensions"""
    return f"sdk/{library.split('.')[0].lower()}/{name.split('.')[0].lower()}"


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--version", help="one SDK version's folder name (260 ... 470)")
    parser.add_argument("--symbols", action="store_true", help="print splat symbol_addrs.txt lines instead")
    parser.add_argument("--subsegments", action="store_true", help="print splat subsegments instead")
    arguments = parser.parse_args()
    if not SIGNATURES.exists():
        sys.exit(f"{SIGNATURES} is missing: tools/setup_toolchain.py")
    base, code = load_code()

    if arguments.version is None:
        for folder in sorted(path for path in SIGNATURES.iterdir() if path.is_dir() and path.name.isdigit()):
            found = find_version(code, folder)
            unique = [item for item in found if len(item[2]) == 1]
            covered = sum(item[4] for item in unique)
            print(f"{folder.name}: {len(found)} objects found ({len(unique)} at one place), {covered} bytes")
        return

    folder = SIGNATURES / arguments.version
    if not folder.is_dir():
        sys.exit(f"no signatures for version {arguments.version} in {SIGNATURES}")
    found = find_version(code, folder)
    if arguments.symbols:
        named = set()
        for start, _, _, name, labels in placed(found):
            for label in labels:
                if LOCAL_LABEL.match(label["name"]):
                    continue
                # A static function another object also has (memclr in each of libetc's INTR objects) gets its object's name
                if label["name"] in named:
                    symbol = f"{label['name']}_{name.split('.')[0].lower()}"
                else:
                    symbol = label["name"]
                named.add(symbol)
                print(f"{symbol} = 0x{base + start + label['offset']:08X}; // type:func")
    elif arguments.subsegments:
        position = None
        for start, end, library, name, _ in placed(found):
            if position is not None and start > position:
                print(f"      - [0x{HEADER_SIZE + position:X}, asm, sdk/unknown_{base + position:08X}]")
            print(f"      - [0x{HEADER_SIZE + start:X}, asm, {subsegment_name(library, name)}]")
            position = end
    else:
        for library, name, offsets, labels, size in sorted(found, key=lambda item: item[2][0]):
            if len(offsets) == 1:
                names = ", ".join(label["name"] for label in labels)
                print(f"{base + offsets[0]:#010X} {base + offsets[0] + size:#010X} {library}:{name} {names}")
            else:
                places = ", ".join(f"{base + offset:#010X}" for offset in offsets)
                print(f"ambiguous {library}:{name} at {places}")

if __name__ == "__main__":
    main()
