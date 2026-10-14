#ifndef COMMON_H
#define COMMON_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;

/* The split's functions (asm/nonmatchings/) use macro.inc's labels */
#define INCLUDE_ASM_USE_MACRO_INC 1
#include "include_asm.h"

#endif /* COMMON_H */
