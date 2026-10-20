"""The handwritten code (docs/TARGET.md) and the equivalent C that stands for part of it in the equivalent build only
(docs/WORKFLOW.md, "The handwritten code"): which of its functions the C file replaces, for configure.py and
tools/progress.py."""
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
HANDWRITTEN = Path("asm") / "game" / "80039D2C.s"
HANDWRITTEN_C = Path("src") / "game" / "80039D2C.c"
# A function's line in the split's handwritten code (g_TimerTicks, a word among them, has no size)
FUNCTION_LINE = re.compile(r"^nonmatching (\w+), 0x[0-9A-F]+$", re.MULTILINE)
# A definition at the top level, its parameters on one line
C_FUNCTION = re.compile(r"^(static[ \t]+)?[A-Za-z_][\w \t\*]*?\b(\w+)[ \t]*\([^;{\n]*\)\s*\{", re.MULTILINE)
# Handwritten routines only a function of the file reaches, which go with it: HANDWRITTEN_PARTS(function, part, ...)
PARTS = re.compile(r"^HANDWRITTEN_PARTS\((\w+)((?:,[ \t]*\w+)+)\)$", re.MULTILINE)


def functions():
    """The handwritten code's functions"""
    return set(FUNCTION_LINE.findall((HERE / HANDWRITTEN).read_text()))


def parts(text):
    """{function: its parts} of a C file's HANDWRITTEN_PARTS lines"""
    found = {}
    for line in PARTS.finditer(text):
        # group(2) starts with the comma after the function
        found.setdefault(line.group(1), set()).update(part.strip() for part in line.group(2).split(",")[1:])
    return found


def replaced():
    """The handwritten functions the C file replaces: its functions but its static ones, and the parts it names"""
    if not (HERE / HANDWRITTEN_C).exists():
        return set()
    else:
        text = (HERE / HANDWRITTEN_C).read_text()
    defined = {definition.group(2) for definition in C_FUNCTION.finditer(text) if definition.group(1) is None}
    named = parts(text)
    undefined = sorted(set(named) - defined)
    if undefined:
        sys.exit(f"{HANDWRITTEN_C}: HANDWRITTEN_PARTS of {', '.join(undefined)}, which the file doesn't define")
    else:
        return defined.union(*named.values())
