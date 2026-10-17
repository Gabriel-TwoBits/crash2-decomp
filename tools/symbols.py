"""The split's symbols, from splat's table (dump_symbols in config/SCUS_941.54.yaml, written each time configure.py
splits): each name's address and kind. Names come from config/symbol_addrs.txt or are splat's own, func_XXXXXXXX and
D_XXXXXXXX by address; an address name stays the stable key of the matching loop's records (docs/WORKFLOW.md, "Naming").
"""
import csv
import re
import sys
from functools import lru_cache
from pathlib import Path
from typing import NamedTuple

HERE = Path(__file__).resolve().parents[1]
SPLAT_SYMBOLS = HERE / ".splat" / "splat_symbols.csv"
COLUMNS = ("vram_start", "name", "type")
HEXADECIMAL = 16
# The image's addresses: .text's start (config/SCUS_941.54.yaml) to the end of .bss
IMAGE_START = 0x80010000
IMAGE_END = 0x8006F1F0
# PsyQ's libraries start here (config/SCUS_941.54.yaml): a function from there is the SDK's, not the game's
SDK_START = 0x80046884
FUNCTION_TYPE = "func"
# Branch labels, jump tables with their labels, entries into the handwritten code: inside functions
LABEL_TYPES = ("label", "jtbl", "jtbl_label", "alabel")
GAME_FUNCTION = "game function"
SDK_FUNCTION = "SDK function"
DATA = "data"
LABEL = "label"
# splat's names for what has none: a function, data
ADDRESS_NAME = re.compile(r"\b(func|D)_([0-9A-F]{8})\b")
FUNCTION_PREFIX = "func"
DATA_PREFIX = "D"


class Symbol(NamedTuple):
    name: str
    address: int
    kind: str


def kind_of_row(address, symbol_type):
    """A symbol's kind from splat's type and its address"""
    if symbol_type == FUNCTION_TYPE and address >= SDK_START:
        return SDK_FUNCTION
    elif symbol_type == FUNCTION_TYPE:
        return GAME_FUNCTION
    elif symbol_type in LABEL_TYPES:
        return LABEL
    else:
        return DATA


@lru_cache(maxsize=None)
def table():
    """{name: Symbol} of every symbol splat knows"""
    if not SPLAT_SYMBOLS.exists():
        sys.exit(f"{SPLAT_SYMBOLS} is missing: configure.py splits the executable")
    symbols = {}
    with SPLAT_SYMBOLS.open(newline="") as listing:
        reader = csv.DictReader(listing)
        missing = [column for column in COLUMNS if column not in (reader.fieldnames or [])]
        if missing:
            sys.exit(f"{SPLAT_SYMBOLS} has no {', '.join(missing)} column")
        for row in reader:
            address = int(row["vram_start"], HEXADECIMAL)
            symbols[row["name"]] = Symbol(row["name"], address, kind_of_row(address, row["type"]))
    if not symbols:
        sys.exit(f"{SPLAT_SYMBOLS} lists no symbol")
    return symbols


@lru_cache(maxsize=None)
def by_address():
    """{address: [Symbol]} of every symbol splat knows"""
    found = {}
    for symbol in table().values():
        found.setdefault(symbol.address, []).append(symbol)
    return found


def functions_by_address():
    """{address: names} of the functions splat knows, the game's and the SDK's, in C or in assembly"""
    functions = {}
    for symbol in table().values():
        if symbol.kind in (GAME_FUNCTION, SDK_FUNCTION):
            functions.setdefault(symbol.address, []).append(symbol.name)
        else:
            continue
    if not functions:
        sys.exit(f"{SPLAT_SYMBOLS} lists no function")
    return functions


def address_of(name):
    """A name's address: the table's, else the one an address name spells; None for an unknown name"""
    if name in table():
        return table()[name].address
    spelled = ADDRESS_NAME.fullmatch(name)
    if spelled is not None:
        return int(spelled.group(2), HEXADECIMAL)
    else:
        return None


def kind_of(name):
    """A name's kind: the table's, else the one an address name spells (a func_ name by its address); None for an
    unknown name"""
    if name in table():
        return table()[name].kind
    spelled = ADDRESS_NAME.fullmatch(name)
    if spelled is None:
        return None
    elif spelled.group(1) == DATA_PREFIX:
        return DATA
    elif int(spelled.group(2), HEXADECIMAL) >= SDK_START:
        return SDK_FUNCTION
    else:
        return GAME_FUNCTION


def address_name(name):
    """The address name of a function or data symbol, by either of its names: the matching loop's records are keyed by it"""
    address = address_of(name)
    if address is None:
        sys.exit(f"{name} is no symbol of the split")
    elif kind_of(name) in (GAME_FUNCTION, SDK_FUNCTION):
        return f"{FUNCTION_PREFIX}_{address:08X}"
    else:
        return f"{DATA_PREFIX}_{address:08X}"


def named_at(spelled):
    """The names of the symbols at an address name's address, of its kind (function or data)"""
    data = spelled.group(1) == DATA_PREFIX
    return [symbol.name for symbol in by_address().get(int(spelled.group(2), HEXADECIMAL), [])
            if symbol.kind != LABEL and (symbol.kind == DATA) == data]


def current_name(name):
    """A symbol's name now, from either of its names (an address name it had before it was named); exits on neither"""
    if name in table():
        return name
    spelled = ADDRESS_NAME.fullmatch(name)
    found = [] if spelled is None else named_at(spelled)
    if len(found) == 1:
        return found[0]
    else:
        sys.exit(f"{name} is no symbol of the split ({', '.join(found) or 'nothing'} at its address)")


def with_current_names(text):
    """C with each address name of a symbol named since replaced by its name: notes written before a rename still land"""
    def renamed(spelled):
        found = [] if spelled.group(0) in table() else named_at(spelled)
        # One symbol at the address, of the same kind: its name; otherwise the text stays as it is
        return found[0] if len(found) == 1 else spelled.group(0)
    return ADDRESS_NAME.sub(renamed, text)
