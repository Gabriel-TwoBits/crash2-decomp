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

/* An equivalent function's contract with its callers, for tools/equiv.py: argument ARGUMENT (0 for $a0) is between LOW
 * and HIGH, signed, in every call the game makes. Nothing in the build; its evidence goes on the line above it */
#define EQUIV_RANGE(function, argument, low, high)
/* Pointer argument ARGUMENT is never NULL in a call the game makes */
#define EQUIV_NONNULL(function, argument)
/* The buffers at pointer arguments FIRST and SECOND, argument COUNT's value times UNIT bytes each, never overlap */
#define EQUIV_DISJOINT(function, first, second, count, unit)
/* Pointer argument ARGUMENT is a buffer of BYTES of its own: no other argument so declared overlaps it */
#define EQUIV_BUFFER(function, argument, bytes)
/* The halfword OFFSET bytes past pointer argument ARGUMENT's address is between LOW and HIGH, unsigned */
#define EQUIV_HALF(function, argument, offset, low, high)
/* In an equivalent function's body: CONDITION holds in every call the game makes, its evidence beside it. Nothing in
 * the build; tools/equiv.py leaves a trial where it fails uncompared */
#ifdef EQUIV_TRIAL
void Equiv_Reject(void);
#define EQUIV_ASSUME(condition) \
    do { \
        if (!(condition)) { \
            Equiv_Reject(); \
        } \
    } while (0)
#else
#define EQUIV_ASSUME(condition)
#endif
/* The BYTES from ADDRESS are the function's own scratch, a save area every other user writes before reading: what it
 * leaves there doesn't count */
#define EQUIV_SCRATCH(function, address, bytes)
/* The retail code runs into these handwritten routines as its own (a branch, not a call), which other callers keep in
 * the build */
#define EQUIV_RUNS(function, routines...)
/* Argument ARGUMENT is a function every caller passes that takes COUNT arguments: a call through it compares those, and
 * leaves the registers named after COUNT as they were (the callback never writes them) */
#define EQUIV_CALLBACK(function, argument, count, kept...)
/* The words OFFSET bytes past pointer argument ARGUMENT's address are, in every call, the words of one such line: a
 * number, a #define of one, or a symbol's address */
#define EQUIV_WORDS(function, argument, offset, words...)
/* A call to CALLEE leaves the registers named as they were (its code never writes them) */
#define EQUIV_KEEPS(function, callee, kept...)
/* The word at ADDRESS points to TABLE, a symbol of the data, which holds its retail words, in every call */
#define EQUIV_TABLE(function, address, table)

/* The split's functions (asm/nonmatchings/) use macro.inc's labels */
#define INCLUDE_ASM_USE_MACRO_INC 1
#include "include_asm.h"

#endif /* COMMON_H */
