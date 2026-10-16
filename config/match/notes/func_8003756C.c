#include "common.h"
#include "sdk.h"
#include "game.h"

inline static s32 *Pop(u8 *o, s32 reg)
{
  if ((reg & 0xE00) == 0xE00)
  {
    if (reg == 0xE1F)
    {
      s32 *p = (*((s32 **) (o + 0xBC))) - 1;
      *((s32 **) (o + 0xBC)) = p;
      return p;
    }
    return (s32 *) (o + (((reg & 0x1FF) * 4) + 0x40));
  }
  return func_80036D0C((struct Obj *) o, reg);
}

inline static s32 *Push(u8 *o, s32 reg)
{
  s32 *p;
  if ((reg & 0xE00) == 0xE00)
  {
    if (reg == 0xE1F)
    {
      p = *((s32 **) (o + 0xBC));
      *((s32 **) (o + 0xBC)) = p + 1;
      return p;
    }
    return (s32 *) (o + (((reg & 0x1FF) * 4) + 0x40));
  }
  return func_80036D0C((struct Obj *) o, reg);
}

void func_8003756C(u8 *o, u32 arg1)
{
  int new_var;
  s32 *dst;
  s32 src;
  src = *Pop(o, (arg1 >> 12) & 0xFFF);
  dst = Push(o, arg1 & 0xFFF);
  if (dst != 0)
  {
    do { new_var = (*((s32 *) (o + 0xF0))) != 0; } while (0);
    if (new_var)
    {
      *dst = func_800321D4(*((s32 *) (o + 0xF0)), ((unsigned int) ((u32) src)) >> 8, 0, 0, 0, 2, 0);
      return;
    }
    *dst = 0;
  }
}
