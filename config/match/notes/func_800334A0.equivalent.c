#include "common.h"
#include "sdk.h"
#include "game.h"

#define MIX_SCALE_MUL 0x3FFF00
#define MIX_SCALE_SHIFT 16
#define MIX_VOL_MAX 0x3FFF
#define MIX_MVOL_SHIFT 7
#define MIX_FADE_IN_STEP 0x667
#define MIX_FADE_OUT_STEP (-0x222)
#define MIX_SEQ_MIN_MODE 2
#define MIX_SEQ_PLAYING 3
#define MIX_CMD_FADE_OUT 2
#define MIX_CMD_FADE_IN 3
#define MIX_VOICES 0x18
#define MIX_PITCH_DIV 4096
#define MIX_NO_RESOURCE 0x6396347F

#define EF_STOP 1
#define EF_STOP_IDLE 2
#define EF_ACTIVE 8
#define EF_STARTED 0x10
#define EF_VOL_RAMP 0x40
#define EF_PITCH_RAMP 0x80
#define EF_FOLLOW_OBJ 0x200
#define EF_SRC_SHIFT 12
#define EF_SRC_MASK 7
#define EF_SRC_BITS 0x7000
#define EF_SRC_QUEUED 0x2000
#define EF_BUSY 0x8000
#define SRC_QUEUE 1
#define SRC_QUEUED 2
#define SRC_RESOURCE 3

#define HDL_TAG_MASK 0xC0000002
#define HDL_ADDR_SHIFT 4
#define HDL_ADDR_PRE_SHIFT 2
#define RES_STATE_NEW 1
#define RES_STATE_USED 0x14
#define RES_STATE_USED_END 2
#define RES_STATE_WAIT 0xA
#define OBJ_TYPE_OFF 0x120
#define OBJ_TYPE_FIRST 6
#define OBJ_TYPE_COUNT 2
#define OBJ_POS_OFF 0x60

#define CMD_VOICE 0
#define CMD_MASK_IDX 1
#define CMD_ADDR_IDX 7
#define CMD_VOL_L 4
#define CMD_VOL_R 5
#define CMD_PITCH 10
#define CMD_WORDS 8
#define CMD_M_ADDR 0x80
#define CMD_M_VOL 3
#define CMD_M_PITCH 0x10

typedef struct Unk800334A0 {
    s32 unk_0;
    s32 flags;
    s32 unk_8;
    void *obj;
    u8 handle[4];
    s32 tag;
    u32 *res;
    u8 repeat;
    u8 unk_1D;
    u16 vol;
    u16 timer;
    u16 pitch;
    s32 vec[3];
    s32 prev[3];
    s32 unk_3C;
    s32 volCount;
    s32 volStep;
    s32 pitchCount;
    s32 pitchStep;
} Unk800334A0;

typedef struct Unk800334A0Res {
    s32 unk_0;
    s16 state;
    s16 unk_6;
    u8 unk_8[7];
    u8 refs;
    s32 unk_10;
    s32 unk_14;
    s32 unk_18;
    u32 addr;
} Unk800334A0Res;

typedef struct Unk80033DE0 {
    s16 seq;
    s16 pad2;
    s32 state;
    s32 pad8;
    s32 padC;
    s16 volL;
    s16 volR;
} Unk80033DE0;

extern void SsSetMVol(s32, s32);
extern void func_8005182C(u8 *);
extern void func_800518B8(s32 *);
extern s32 D_8005F18C;
extern u8 D_8005FC6C[];
extern void *D_800608CC;
extern s32 D_80067FB0;
extern s32 D_8006CD2C;
extern s32 D_8005F304;
extern s32 D_8006CBFC;
extern s32 D_8006CBF8;
extern s32 D_8005F25C;
extern s32 D_8005F258;
extern s16 D_8005F2F8;
extern s16 D_8005F2FC;
extern s16 D_8005F300;
extern s32 D_8005F308;
extern s32 D_8005F39C;
extern s16 D_8005FCE4;
extern Ent D_8005F4EC[];

void func_800334A0(void) {
    s32 cmd[CMD_WORDS + 8];
    s32 cmd2[CMD_WORDS + 8];
    s32 vec[3];
    s32 i;
    s32 busy;
    s32 mask;
    s32 flags;
    s32 voiceBit;
    s32 vol;
    u32 addr;
    u8 *status;
    u8 *p;
    Unk800334A0 *e;
    Unk800334A0Res *res;
    void *q;

    busy = 0;
    if (D_8006CBF8 != D_8005F25C) {
        D_8005F25C = D_8006CBF8;
        func_800325D0((D_8006CBF8 * MIX_SCALE_MUL) >> MIX_SCALE_SHIFT);
    }
    if (D_8006CBFC != D_8005F258) {
        D_8005F258 = D_8006CBFC;
        func_80033C84((D_8006CBFC * MIX_SCALE_MUL) >> MIX_SCALE_SHIFT);
    }
    if (D_800608CC != 0) {
        func_80034254(*(s32 *)((u8 *)D_800608CC + 0x10) + 0x2A4);
    }
    if (D_8005F300 != 0) {
        if (D_8005F300 < 0 && D_8005F2FC < -D_8005F300) {
            D_8005F2FC = 0;
            D_8005F300 = 0;
        }
        if (D_8005F300 > 0 && (MIX_VOL_MAX - D_8005F2FC) < D_8005F300) {
            D_8005F2FC = MIX_VOL_MAX;
            D_8005F300 = 0;
        }
        D_8005F2FC = (u16)D_8005F2FC + (u16)D_8005F300;
        vol = ((s32)((u32)(u16)D_8005F2FC << 16)) >> (16 + MIX_MVOL_SHIFT);
        SsSetMVol(vol, vol);
    }
    if (D_8005F18C == 1) {
        D_8005F300 = MIX_FADE_OUT_STEP;
        if (D_8005F308 != 0 && D_8005F2F8 >= MIX_SEQ_MIN_MODE && D_8005F39C == MIX_SEQ_PLAYING) {
            func_800344B0((Unk80033DE0 *)&D_8005FCE4, MIX_CMD_FADE_OUT, 0);
        }
    }
    if (D_8005F18C == -1) {
        D_8005F300 = MIX_FADE_IN_STEP;
        if (D_8005F308 != 0 && D_8005F2F8 >= MIX_SEQ_MIN_MODE && D_8005F39C == MIX_SEQ_PLAYING) {
            func_800344B0((Unk80033DE0 *)&D_8005FCE4, MIX_CMD_FADE_IN, 0);
        }
    }
    func_8005182C(D_8005FC6C);
    i = D_8005F304;
    if (i >= MIX_VOICES) {
        return;
    }
    e = (Unk800334A0 *)&D_8005F4EC[i];
    status = &D_8005FC6C[i];
    for (; i < MIX_VOICES; i++, e++, status++) {
        if (!(e->flags & EF_ACTIVE)) {
            continue;
        }
        voiceBit = 1 << i;
retry:
        switch ((e->flags >> EF_SRC_SHIFT) & EF_SRC_MASK) {
        case SRC_RESOURCE:
            addr = ((Unk800334A0Res *)e->res)->addr;
            goto start_voice;
        case SRC_QUEUE:
            if (func_800143C0(e->handle, 0) == 0) {
                break;
            }
            vol = (s32)func_80014260(e->handle, 1, 0);
            if ((vol & HDL_TAG_MASK) == HDL_TAG_MASK) {
                addr = ((u32)vol << HDL_ADDR_PRE_SHIFT) >> HDL_ADDR_SHIFT;
                goto start_voice;
            }
            e->flags = (e->flags & ~EF_SRC_BITS) | EF_SRC_QUEUED;
            /* fall through */
        case SRC_QUEUED:
            q = func_80014260(e->handle, 1, 0);
            res = (Unk800334A0Res *)func_8001557C(((s32 *)q)[1]);
            if (res != 0) {
                if (res->state == RES_STATE_NEW) {
                    res->state = RES_STATE_USED;
                } else if ((u32)((res->state - RES_STATE_USED) & 0xFFFF) >= RES_STATE_USED_END) {
                    break;
                } else {
                    /* already in use */
                }
                addr = res->addr;
                res->refs = res->refs + 1;
                e->res = (u32 *)res;
                func_80013E10(func_80014148((u32)q));
                goto start_voice;
            } else {
                Unk800334A0Res *r2 = (Unk800334A0Res *)func_80014148((u32)q);
                if ((u32)((u16)r2->state - RES_STATE_USED) < RES_STATE_USED_END && r2->unk_6 == 1) {
                    r2->state = RES_STATE_WAIT;
                    r2->unk_18 = ((s32 *)q)[1];
                    D_80067FB0 = D_80067FB0 + 1;
                }
            }
            break;
        default:
            break;
        }
        goto after;
start_voice:
        cmd[CMD_ADDR_IDX] = addr;
        cmd[CMD_MASK_IDX] = CMD_M_ADDR;
        cmd[CMD_VOICE] = voiceBit;
        func_800518B8(cmd);
        flags = e->flags & ~EF_SRC_BITS;
        e->flags = flags & ~EF_BUSY;
        if (!(flags & EF_STARTED) || e->timer == 0) {
            e->flags = (flags & ~EF_BUSY) | EF_STARTED;
            e->timer = 1;
        }
after:
        flags = e->flags;
        if (flags & EF_BUSY) {
            continue;
        }
        if (flags & EF_STARTED) {
            e->timer = e->timer - 1;
            if (e->timer != 0) {
                continue;
            }
            e->flags = e->flags & ~EF_STARTED;
            func_80051078(1, voiceBit);
            *status = 1;
        }
        if (*status == MIX_SEQ_PLAYING) {
            if (e->repeat >= 2) {
                e->repeat = e->repeat - 1;
                func_80051078(1, voiceBit);
                *status = 1;
            } else if (e->tag != MIX_NO_RESOURCE) {
                if (e->res != 0) {
                    func_80013E10(e->res);
                }
                addr = func_80032A98((s32 *)&e->flags, (s32 *)&e->tag);
                e->tag = MIX_NO_RESOURCE;
                if (addr == 0) {
                    goto retry;
                }
                cmd2[CMD_ADDR_IDX] = addr;
                cmd2[CMD_MASK_IDX] = CMD_M_ADDR;
                cmd2[CMD_VOICE] = voiceBit;
                func_800518B8(cmd2);
                func_80051078(1, voiceBit);
                *status = 1;
                goto after;
            } else {
                func_80033448(e);
                goto tail;
            }
        }
        mask = 0;
        if (e->flags & EF_VOL_RAMP) {
            e->vol = e->vol + e->volStep;
            e->volCount = e->volCount - 1;
            if (e->volCount > 0) {
                busy = 1;
            } else {
                e->flags = e->flags & ~EF_VOL_RAMP;
            }
            mask = mask | CMD_M_VOL;
            if ((u32)(*((u8 *)e->obj + OBJ_TYPE_OFF) - OBJ_TYPE_FIRST) < OBJ_TYPE_COUNT) {
                vol = (s16)e->vol;
                if (vol >= MIX_VOL_MAX) {
                    vol = MIX_VOL_MAX;
                } else if (vol < 0) {
                    vol = 0;
                } else {
                    /* in range */
                }
                ((s16 *)cmd)[CMD_VOL_R] = vol;
                ((s16 *)cmd)[CMD_VOL_L] = vol;
            } else {
                p = (u8 *)func_8003264C((s32 *)e->vec, (s16)e->vol);
                ((s16 *)cmd)[CMD_VOL_R] = p[2] | (p[3] << 8);
                ((s16 *)cmd)[CMD_VOL_L] = p[0] | (p[1] << 8);
            }
        }
        if (e->flags & EF_PITCH_RAMP) {
            e->pitch = e->pitch + e->pitchStep;
            e->pitchCount = e->pitchCount - 1;
            if (e->pitchCount > 0) {
                busy = 1;
            } else {
                e->flags = e->flags & ~EF_PITCH_RAMP;
            }
            mask = mask | CMD_M_PITCH;
            ((s16 *)cmd)[CMD_PITCH] = (s32)(e->pitch * D_8006CD2C) / MIX_PITCH_DIV;
        }
        if ((e->flags & EF_FOLLOW_OBJ) && e->obj != 0) {
            e->prev[0] = ((s32 *)e->obj)[OBJ_POS_OFF / 4];
            e->prev[1] = ((s32 *)e->obj)[OBJ_POS_OFF / 4 + 1];
            e->prev[2] = ((s32 *)e->obj)[OBJ_POS_OFF / 4 + 2];
            func_8001EC2C(&((s32 *)e->obj)[OBJ_POS_OFF / 4], vec, 1);
            e->vec[0] = vec[0];
            e->vec[1] = vec[1];
            e->vec[2] = vec[2];
            mask = mask | CMD_M_VOL;
            p = (u8 *)func_8003264C((s32 *)e->vec, (s16)e->vol);
            ((s16 *)cmd)[CMD_VOL_R] = p[2] | (p[3] << 8);
            ((s16 *)cmd)[CMD_VOL_L] = p[0] | (p[1] << 8);
        }
        cmd[CMD_MASK_IDX] = mask;
        if (mask != 0) {
            cmd[CMD_VOICE] = voiceBit;
            func_800518B8(cmd);
        }
tail:
        if ((e->flags & EF_STOP) || (busy == 0 && (e->flags & EF_STOP_IDLE))) {
            func_80033448(e);
        }
    }
}
