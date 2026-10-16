#include "common.h"
#include "sdk.h"
#include "game.h"

struct Unk8005F288 {
    s32 a;
    s16 b;
};
struct Unk8003690C {
    s32 f0;
    s32 f4;
    s32 f8;
    s32 fC;
    s16 f10;
    s16 pad;
    s32 f14;
    s32 f18;
    s32 f1C;
    s32 f20;
    s32 f24;
    s32 f28;
    s32 f2C;
    s32 pad2[8];
    s32 f50;
    s32 f54;
    s32 f58;
    s32 f5C;
};
struct Unk8005F288 D_8005F288 = {0x59544D45, 0x5000};
void *memset(void *s, s32 c, s32 n);

void func_8003690C(void *p)
{
  struct Unk8003690C *o = p;
  s32 u;
  memset(o, 0, 0x2A4);
  o->f0 = 8;
  do { o->f4 = 1; u = D_8005F288.b; o->fC = D_8005F288.a; o->f10 = u; } while (0);
  o->f20 = 0x63;
  o->f2C = 5;
  o->f58 = 0xFF;
  o->f5C = 0xFF;
}
