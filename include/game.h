#ifndef GAME_H
#define GAME_H

#include "common.h"

/* The game's functions that C calls, declared once so every file calls them alike. A parameter whose structure is known
   only in one file is a void pointer here until the structure is shared */

/* A structure func_80032F18 reads (0x100) and func_80038D08 decodes operands into */
struct Obj;

s32 func_8001148C(s32);
void func_800116C0(s32);
s32 func_80012010(s32);
void func_80012ED0(s32);
void func_80013F28(void *);
void *func_80014260(void *, s32, s32);
s32 func_800143C0(void *, s32);
void func_800155E0(void *, s32);
void func_800163B4(void *);
void func_80016F44(s32, s32, s32);
void func_80018DD0(void *, void *, void *);
void func_80019BBC(s32, s32);
void func_80019E28(void *);
void func_8002001C(char *);
void func_80023154(void);
void func_800241A8(void);
void func_80029538(s32, s32, s32);
s32 func_800322C0(s32, s32, s32, s32);
void func_80032588(void);
s32 func_80032F18(s32, s32, s32 *, struct Obj *);
void func_80034394(void);
s32 func_80035AB8();
s32 func_80035BB8();
s32 func_80036084(s32);
s32 func_800360CC(s32);
s32 func_80036150();
s32 func_80036304(void);
void func_80036324(s32);
s32 *func_80036D0C(struct Obj *, s32);
void func_8003CBE4(void);

#endif /* GAME_H */
