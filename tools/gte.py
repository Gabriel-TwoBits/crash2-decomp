"""A model of the PlayStation's GTE (coprocessor 2) for tools/equiv.py: its registers as the CPU moves them, and the
commands the game's C models (include/gte.h). Written from psx-spx's GTE chapter (docs/REFERENCES.md); include/gte.h
models the same commands in C, and equiv.py judges the C against this one."""

WORD_BITS = 32
WORD_MASK = 0xFFFFFFFF
HALF_MASK = 0xFFFF
HALF_SIGN = 0x8000
HALF_BITS = 16
REGISTERS = 32

# Data registers
VXY0, VZ0 = 0, 1
IR0, IR1, IR2, IR3 = 8, 9, 10, 11
SXY0, SXY1, SXY2, SXYP = 12, 13, 14, 15
SZ0, SZ1, SZ2, SZ3 = 16, 17, 18, 19
MAC0, MAC1, MAC2, MAC3 = 24, 25, 26, 27
IRGB, ORGB, LZCS, LZCR = 28, 29, 30, 31
# Held as 16 bits, read back sign-extended (the vectors' Z, IR0-IR3) or zero-extended (OTZ, SZ0-SZ3)
SIGNED_DATA = {1, 3, 5, 8, 9, 10, 11}
UNSIGNED_DATA = {7, 16, 17, 18, 19}

# Control registers: the rotation matrix's nine halves in five words (RT11 | RT12 << 16, ..., RT33), the translation
RT = 0
TRX, TRY, TRZ = 5, 6, 7
OFX, OFY, H, DQA, DQB = 24, 25, 26, 27, 28
FLAG = 31
# Held as 16 bits, read back sign-extended: RT33, L33, LB3, H (used unsigned), DQA, ZSF3, ZSF4
SIGNED_CONTROL = {4, 12, 20, 26, 27, 29, 30}

# FLAG's bits: MAC1-3 past 43 bits either way, IR1-3 and SZ3 saturated, the division overflowing, MAC0 past 31 bits
# either way, SX2, SY2 and IR0 saturated. Bit 31 is the OR of the error bits; bits 0-11 read zero
FLAG_MAC_POSITIVE = (1 << 30, 1 << 29, 1 << 28)
FLAG_MAC_NEGATIVE = (1 << 27, 1 << 26, 1 << 25)
FLAG_IR = (1 << 24, 1 << 23, 1 << 22)
FLAG_SZ3 = 1 << 18
FLAG_DIVIDE = 1 << 17
FLAG_MAC0_POSITIVE = 1 << 16
FLAG_MAC0_NEGATIVE = 1 << 15
FLAG_SX2 = 1 << 14
FLAG_SY2 = 1 << 13
FLAG_IR0 = 1 << 12
FLAG_ERROR = 1 << 31
# Bits 30-23 and 18-13
FLAG_ERROR_BITS = 0x7F87E000
FLAG_WRITABLE = 0x7FFFF000

# A command's fields: the function, sf (shift MAC1-3 right by 12) and lm (IR1-3 saturated at 0 rather than -0x8000)
FUNCTION_MASK = 0x3F
SF_SHIFT = 19
LM_SHIFT = 10
RTPS = 0x01
RTPT = 0x30
COMMANDS = {RTPS: "rtps", RTPT: "rtpt"}

FRACTION_BITS = 12
# MAC1-3 accumulate in 44 bits, checked after each addition
MAC_BITS = 44
MAC_LIMIT = 1 << (MAC_BITS - 1)
MAC0_LIMIT = 1 << 31
IR_MIN, IR_MAX = -0x8000, 0x7FFF
SZ_MAX = 0xFFFF
SCREEN_MIN, SCREEN_MAX = -0x400, 0x3FF
IR0_MAX = 0x1000
# The perspective division: H / SZ3 as unsigned 1.16, by Newton-Raphson from a 257-entry seed table
DIVIDE_MAX = 0x1FFFF
DIVIDE_SEED_COUNT = 0x101
DIVIDE_SEED_BIAS = 0x101
DIVIDE_SEED_BASE = 0x7FC0
DIVIDE_SEED_SHIFT = 7
DIVIDE_NORMAL_BITS = 16
DIVIDE_TWO = 0x2000080
DIVIDE_ROUND = 0x80
DIVIDE_STEP_SHIFT = 8
DIVIDE_FINAL_ROUND = 0x8000
SEED_NUMERATOR = 0x40000
SEED_OFFSET = 0x100
# IRGB and ORGB: three 5-bit colour components, IR1-IR3 / 0x80 each
COLOUR_BITS = 5
COLOUR_MAX = 0x1F
COLOUR_SCALE_SHIFT = 7
# A vector's and the rotation matrix's dimension; RTPT's vectors
DIMENSION = 3
RTPT_VECTORS = 3


def signed16(value):
    value &= HALF_MASK
    return value - (1 << HALF_BITS) if value & HALF_SIGN else value


def signed32(value):
    value &= WORD_MASK
    return value - (1 << WORD_BITS) if value & (1 << (WORD_BITS - 1)) else value


def clamp(value, low, high):
    return max(low, min(high, value))


def divide_seed(index):
    return max(0, (SEED_NUMERATOR // (index + SEED_OFFSET) + 1) // 2 - DIVIDE_SEED_BIAS)


DIVIDE_SEEDS = [divide_seed(index) for index in range(DIVIDE_SEED_COUNT)]


class Gte:
    """The GTE's 32 data and 32 control registers, each held as the hardware keeps it"""

    def __init__(self, words):
        self.data = [0] * REGISTERS
        self.control = [0] * REGISTERS
        for number in range(REGISTERS):
            self.write_data(number, words[number])
            self.write_control(number, words[REGISTERS + number])

    def read_data(self, number):
        if number == SXYP:
            return self.data[SXY2]
        elif number in (IRGB, ORGB):
            # Both read the colour IR1-IR3 make
            colour = 0
            for index, register in enumerate((IR1, IR2, IR3)):
                component = clamp(signed16(self.data[register]) >> COLOUR_SCALE_SHIFT, 0, COLOUR_MAX)
                colour |= component << (index * COLOUR_BITS)
            return colour
        elif number == LZCR:
            # The leading bits of LZCS equal to its sign bit
            value = self.data[LZCS]
            if value & (1 << (WORD_BITS - 1)):
                value = ~value & WORD_MASK
            else:
                pass
            return WORD_BITS - value.bit_length()
        else:
            return self.data[number]

    def write_data(self, number, value):
        value &= WORD_MASK
        if number in SIGNED_DATA:
            self.data[number] = signed16(value) & WORD_MASK
        elif number in UNSIGNED_DATA:
            self.data[number] = value & HALF_MASK
        elif number == SXYP:
            self.data[SXY0], self.data[SXY1], self.data[SXY2] = self.data[SXY1], self.data[SXY2], value
        elif number == IRGB:
            for index, register in enumerate((IR1, IR2, IR3)):
                self.data[register] = ((value >> (index * COLOUR_BITS)) & COLOUR_MAX) << COLOUR_SCALE_SHIFT
        elif number in (ORGB, LZCR):
            # Read-only
            pass
        else:
            self.data[number] = value

    def read_control(self, number):
        if number == FLAG:
            flag = self.control[FLAG]
            return flag | FLAG_ERROR if flag & FLAG_ERROR_BITS else flag
        else:
            return self.control[number]

    def write_control(self, number, value):
        value &= WORD_MASK
        if number in SIGNED_CONTROL:
            self.control[number] = signed16(value) & WORD_MASK
        elif number == FLAG:
            self.control[FLAG] = value & FLAG_WRITABLE
        else:
            self.control[number] = value

    def command(self, word):
        """Runs a command; False when it isn't one modelled"""
        function = word & FUNCTION_MASK
        sf = (word >> SF_SHIFT) & 1
        lm = (word >> LM_SHIFT) & 1
        if function == RTPS:
            self.control[FLAG] = 0
            self.perspective(0, sf, lm, True)
        elif function == RTPT:
            self.control[FLAG] = 0
            for vector in range(RTPT_VECTORS):
                self.perspective(vector, sf, lm, vector == RTPT_VECTORS - 1)
        else:
            return False
        return True

    def vector(self, index):
        xy = self.data[VXY0 + 2 * index]
        return signed16(xy), signed16(xy >> HALF_BITS), signed16(self.data[VZ0 + 2 * index])

    def rotation(self, row, column):
        """RT(row+1)(column+1): halves of the five words, low half first"""
        index = row * DIMENSION + column
        word = self.control[RT + index // 2]
        return signed16(word >> HALF_BITS) if index % 2 else signed16(word)

    def mac_checked(self, index, value):
        """A step of MAC1-3's accumulation: flagged past 43 bits either way, kept to 44 bits"""
        if value >= MAC_LIMIT:
            self.control[FLAG] |= FLAG_MAC_POSITIVE[index]
        elif value < -MAC_LIMIT:
            self.control[FLAG] |= FLAG_MAC_NEGATIVE[index]
        else:
            pass
        value &= (1 << MAC_BITS) - 1
        return value - (1 << MAC_BITS) if value & MAC_LIMIT else value

    def mac0_checked(self, value):
        if value >= MAC0_LIMIT:
            self.control[FLAG] |= FLAG_MAC0_POSITIVE
        elif value < -MAC0_LIMIT:
            self.control[FLAG] |= FLAG_MAC0_NEGATIVE
        else:
            pass
        return value

    def saturated(self, value, low, high, flag):
        if value < low or value > high:
            self.control[FLAG] |= flag
            return clamp(value, low, high)
        else:
            return value

    def divide(self, h, sz3):
        """H / SZ3 as the GTE's division reckons it, flagged when H >= 2 * SZ3"""
        if h >= sz3 * 2:
            self.control[FLAG] |= FLAG_DIVIDE
            return DIVIDE_MAX
        else:
            shift = DIVIDE_NORMAL_BITS - sz3.bit_length()
            numerator = h << shift
            divisor = sz3 << shift
            seed = DIVIDE_SEEDS[(divisor - DIVIDE_SEED_BASE) >> DIVIDE_SEED_SHIFT] + DIVIDE_SEED_BIAS
            divisor = (DIVIDE_TWO - divisor * seed) >> DIVIDE_STEP_SHIFT
            divisor = (DIVIDE_ROUND + divisor * seed) >> DIVIDE_STEP_SHIFT
            return min(DIVIDE_MAX, (numerator * divisor + DIVIDE_FINAL_ROUND) >> DIVIDE_NORMAL_BITS)

    def perspective(self, index, sf, lm, last):
        """RTPS on vector index: rotated and translated into MAC1-3 and IR1-3, its Z pushed on the SZ FIFO, its screen
        position on the SXY FIFO; the depth cue into MAC0 and IR0 for the last vector only"""
        vector = self.vector(index)
        translation = (TRX, TRY, TRZ)
        ir_low = 0 if lm else IR_MIN
        sums = []
        for row in range(DIMENSION):
            value = signed32(self.control[translation[row]]) << FRACTION_BITS
            for column in range(DIMENSION):
                value = self.mac_checked(row, value + self.rotation(row, column) * vector[column])
            sums.append(value)
            mac = value >> (sf * FRACTION_BITS)
            self.data[MAC1 + row] = mac & WORD_MASK
            if row < DIMENSION - 1:
                self.data[IR1 + row] = self.saturated(signed32(mac), ir_low, IR_MAX, FLAG_IR[row]) & WORD_MASK
            else:
                # IR3 is saturated from MAC3, but flagged from the sum shifted by 12 whatever sf is
                self.data[IR3] = clamp(signed32(mac), ir_low, IR_MAX) & WORD_MASK
                if not ir_low <= value >> FRACTION_BITS <= IR_MAX:
                    self.control[FLAG] |= FLAG_IR[2]
                else:
                    pass
        sz3 = self.saturated(sums[2] >> FRACTION_BITS, 0, SZ_MAX, FLAG_SZ3)
        self.data[SZ0], self.data[SZ1], self.data[SZ2] = self.data[SZ1], self.data[SZ2], self.data[SZ3]
        self.data[SZ3] = sz3
        quotient = self.divide(self.control[H] & HALF_MASK, sz3)
        screen = []
        for register, offset, flag in ((IR1, OFX, FLAG_SX2), (IR2, OFY, FLAG_SY2)):
            value = self.mac0_checked(quotient * signed16(self.data[register]) + signed32(self.control[offset]))
            self.data[MAC0] = value & WORD_MASK
            screen.append(self.saturated(value >> HALF_BITS, SCREEN_MIN, SCREEN_MAX, flag))
        self.data[SXY0], self.data[SXY1] = self.data[SXY1], self.data[SXY2]
        self.data[SXY2] = (screen[0] & HALF_MASK) | ((screen[1] & HALF_MASK) << HALF_BITS)
        if last:
            value = self.mac0_checked(quotient * signed16(self.control[DQA]) + signed32(self.control[DQB]))
            self.data[MAC0] = value & WORD_MASK
            self.data[IR0] = self.saturated(value >> FRACTION_BITS, 0, IR0_MAX, FLAG_IR0) & WORD_MASK
        else:
            pass

    def registers(self):
        """The 64 registers as the CPU reads them: data, then control"""
        return tuple(self.read_data(number) for number in range(REGISTERS)) + \
            tuple(self.read_control(number) for number in range(REGISTERS))
