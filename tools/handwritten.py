"""The handwritten code (docs/TARGET.md) and the equivalent C that stands for part of it in the equivalent build only
(docs/WORKFLOW.md, "The handwritten code"): which of its functions the C file replaces, for configure.py and
tools/progress.py."""
import re
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
HANDWRITTEN = Path("asm") / "game" / "80039D2C.s"
HANDWRITTEN_C = Path("src") / "game" / "80039D2C.c"
# A function's line in the split's handwritten code (g_TimerTicks, a word among them, has no size)
FUNCTION_LINE = re.compile(r"^nonmatching (\w+), 0x[0-9A-F]+$", re.MULTILINE)
# A definition at the top level, its parameters on one line
C_FUNCTION = re.compile(r"^(static[ \t]+)?[A-Za-z_][\w \t\*]*?\b(\w+)[ \t]*\([^;{\n]*\)\s*\{", re.MULTILINE)


def functions():
    """The handwritten code's functions"""
    return set(FUNCTION_LINE.findall((HERE / HANDWRITTEN).read_text()))


def replaced():
    """The handwritten functions the C file replaces: its functions but its static ones"""
    if not (HERE / HANDWRITTEN_C).exists():
        return set()
    else:
        text = (HERE / HANDWRITTEN_C).read_text()
        return {definition.group(2) for definition in C_FUNCTION.finditer(text) if definition.group(1) is None}
