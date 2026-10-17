#include "common.h"
#include "sdk.h"
#include "game.h"

#define FLAG_SPU_IDLE_MASK 0x8010
#define FLAG_ACTIVE 8
#define FLAG_FADE 0x40
#define FLAG_REVERB 0x400
#define FLAG_PITCH_CLAMP 0x200
#define FLAG_VOICE_MASK 0xFFC0FFFF
#define VOICE_MASK 0x3F
#define VOICE_SHIFT 16
#define CTRL_REQUIRED 0x1000
#define OBJ_FLAGS_REQUIRED 0x100000
#define VOL_MAX 0x3FFF
#define CMD_MASK 0xFF93
#define BAD_RESULT (-0xE)
#define TEMPLATE_FLAGS_KEEP 0x7000
#define TEMPLATE_FLAGS_SET 0x8000

extern s32 D_8005F2E0;
extern s16 D_8005F2E8;
extern s32 D_8005FC84;
extern s32 D_8006CB94;
extern s32 D_8006CD2C;
void SpuSetReverbVoice(s32, s32);
void func_800518B8(s32 *);
s32 func_800328D4(void);

#define OBJ_W(o, off) (*(s32 *)((u8 *)(o) + (off)))
#define ENT_W(e, off) (*(s32 *)((u8 *)(e) + (off)))
#define ENT_H(e, off) (*(s16 *)((u8 *)(e) + (off)))

s32 func_80032B7C(struct Obj *arg0, s32 arg1, s32 arg2) {
    s32 cmd[0x14];
    s32 pos[3];
    Ent *e;
    s32 *src;
    s32 *dst;
    s32 voice;
    u32 res;
    s32 v;
    s16 vol;
    s32 i;

    if (D_8006CBF8 == 0) {
        return 0;
    }
    if (!(D_8006CB94 & CTRL_REQUIRED) && !(OBJ_W(arg0, 0x100) & OBJ_FLAGS_REQUIRED)) {
        return 0;
    }
    voice = func_800328D4();
    if (voice <= 0) {
        return -1;
    }
    e = &D_8005F4EC[voice];
    if (arg1 & 1) {
        res = arg1 & ~1;
    } else {
        res = func_80032A98(&D_8005FC84, (s32 *)arg1);
        if (res == BAD_RESULT) {
            return -1;
        }
        if (D_8005FC84 & TEMPLATE_FLAGS_KEEP) {
            D_8005FC84 |= TEMPLATE_FLAGS_SET;
        }
    }
    src = &D_8005FC84;
    dst = (s32 *)((u8 *)e + 4);
    for (i = 0; i < 4; i++) {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        src += 4;
        dst += 4;
    }
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    ENT_H(e, 0x1E) = (arg2 * D_8005F2E8) >> 14;
    if (e->flags & FLAG_FADE) {
        ENT_W(e, 0x44) = (ENT_H(e, 0x3C) - ENT_H(e, 0x1E)) / ENT_W(e, 0x40);
    }
    e->key = arg0;
    if (arg0 != 0) {
        ENT_W(e, 0x30) = OBJ_W(arg0, 0x60);
        ENT_W(e, 0x34) = OBJ_W(arg0, 0x64);
        ENT_W(e, 0x38) = OBJ_W(arg0, 0x68);
        func_8001EC2C((s32 *)((u8 *)arg0 + 0x60), pos, 1);
        ENT_W(e, 0x24) = pos[0];
        ENT_W(e, 0x28) = pos[1];
        ENT_W(e, 0x2C) = pos[2];
    }
    func_800323AC();
    e->flags = (e->flags & FLAG_VOICE_MASK) | ((voice & VOICE_MASK) << VOICE_SHIFT);
    cmd[1] = CMD_MASK;
    cmd[0] = 1 << voice;
    if ((u32)(*((u8 *)e->key + 0x120) - 6) < 2U) {
        vol = ENT_H(e, 0x1E);
        if (vol >= VOL_MAX) {
            vol = VOL_MAX;
        } else if (vol < 0) {
            vol = 0;
        } else {
            /* in range */
        }
        ((s16 *)cmd)[5] = vol;
        ((s16 *)cmd)[4] = vol;
        e->flags &= ~FLAG_PITCH_CLAMP;
    } else {
        u8 *p = (u8 *)func_8003264C((s32 *)((u8 *)e + 0x24), ENT_H(e, 0x1E));
        /* the table entry may be unaligned: read it bytewise */
        ((s16 *)cmd)[5] = p[2] | (p[3] << 8);
        ((s16 *)cmd)[4] = p[0] | (p[1] << 8);
    }
    ((s16 *)cmd)[10] = ((s32)(*(u16 *)((u8 *)e + 0x22)) * D_8006CD2C) / 4096;
    cmd[9] = 1;
    cmd[10] = 1;
    cmd[11] = 3;
    cmd[7] = res;
    ((s16 *)cmd)[24] = 0;
    ((s16 *)cmd)[25] = 0;
    ((s16 *)cmd)[26] = 0;
    ((s16 *)cmd)[27] = 0;
    ((s16 *)cmd)[28] = 0xF;
    func_800518B8(cmd);
    SpuSetReverbVoice((e->flags >> 10) & 1, cmd[0]);
    if (!(e->flags & FLAG_SPU_IDLE_MASK)) {
        func_80051078(1, cmd[0]);
    }
    v = D_8005F2E0 + 1;
    e->a = v;
    D_8005F2E0 = v;
    e->flags |= FLAG_ACTIVE;
    return e->a;
}
