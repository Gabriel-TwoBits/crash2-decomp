#include "common.h"
#include "sdk.h"
#include "game.h"

typedef struct { char b[4]; } V4;
typedef struct { char p[0x1C]; s32 f1c; char p2[4]; V4 f24; V4 f28; V4 f2c; } S;
void func_80020998(S *arg0, s32 arg1);

#ifdef NON_MATCHING
#define FLAG_ANY_RELOAD 0x7
#define FLAG_OBJECT 0x1
#define FLAG_TARGET 0x2
#define FLAG_POSITION 0x4
#define FLAG_SEGMENT 0x8
#define FLAG_FIELD_48 0x10

#define ARG_KEEP_SCREEN 0x2
#define ARG_FORCE 0x8
#define ARG_SKIP_PREPASS 0x10

#define OBJ_DATA 0x10
#define DATA_COUNT 0x190
#define DATA_ENTRIES 0x194
#define DATA_FLAGS 0x1B4
#define DATA_FLAG_ACTIVE 0x10
#define DATA_SUB 0x280
#define DATA_FLAG_OFF 0x27C
#define ENTRY_STRIDE 4

#define SEGMENT_SHIFT 8
#define SCREEN_MODE_PENDING 0x600
#define SAVE_MAGIC 0x6396347F
#define NO_VALUE 0xFFFF /* stored unsigned when the lookup finds nothing */

#define FIELD(p, T, off) (*(T *)((u8 *)(p) + (off)))


extern void *g_CurrentZone;
extern s32 D_800608D0;
extern s32 g_CameraPath;
extern s32 g_CameraProgress;
extern s32 g_CameraProgressMax;
extern s32 g_CameraNode;
extern s32 g_CameraNodeCount;
extern s32 g_CameraPathKind;
extern s32 D_800608FC;
extern s32 D_80060900;
extern s32 D_80060904;
extern s32 D_80060884;
extern s32 D_80060888;
extern s32 g_CameraX[];
extern s32 g_CameraAngleX[];
extern s32 D_80060918[];
extern s32 g_Player;
extern s32 D_8005F390;
extern s32 D_8005F398;
extern s32 D_8005F3D0;
extern s32 D_8006CBB4;
extern s32 D_8006CC84;
extern s32 D_8006CD7C;
extern s32 D_8006CB48;
extern void *D_8006759C;
extern s32 D_80067FC8;

void Cam_MoveAlongPath(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    s32 flags;
    s32 keepScreen;
    s32 target;
    s32 maxPos;
    s32 oldField48;
    s32 field48;
    s32 count;
    s32 seg;
    s32 i;
    s32 steps;
    s32 idx;
    s32 dist;
    s32 tmp;
    s32 res;
    s32 *list;
    s32 *p;
    s32 obj;
    s32 data;
    s32 sub;
    s32 geom;
    s32 shortVal;
    void *entry;

    flags = -((arg3 & ARG_FORCE) != 0) & FLAG_ANY_RELOAD;
    keepScreen = 1;
    target = arg2;
    if (arg1 != 0 && arg0 != 0) {
        if (arg0 != (s32)g_CurrentZone) {
            flags |= FLAG_OBJECT;
            if (g_CurrentZone != 0) {
                keepScreen = arg3 & ARG_KEEP_SCREEN;
                if (!(arg3 & ARG_SKIP_PREPASS)) {
                    data = FIELD(g_CurrentZone, s32, OBJ_DATA);
                    D_800608D0 = arg0;
                    for (i = 0; i < FIELD(data, s32, DATA_COUNT); i++) {
                        if (FIELD(data, s32, DATA_FLAGS + i * ENTRY_STRIDE) & DATA_FLAG_ACTIVE) {
                            sub = data + DATA_ENTRIES + i * ENTRY_STRIDE;
                            if (Nsf_CloseEntry((void *)sub, 0) != 0) {
                                entry = Nsf_OpenEntry((void *)sub, 1, 0);
                                if (Zone_IsLinked(arg0, (s32 *)entry) == 0) {
                                    func_8002001C((char *)entry);
                                }
                            }
                        }
                    }
                }
            }
        }
        if (arg1 != g_CameraPath || (arg3 & ARG_FORCE)) {
            flags |= FLAG_TARGET;
            count = Ent_GetPropertyRowLength(arg1, ENT_PROP_POSITIONS, 0, 2);
            maxPos = (count - 1) << SEGMENT_SHIFT;
            field48 = Ent_GetPropertyValue(arg1, 0x29, 0, 0, 0, 2, 0);
        } else {
            count = g_CameraNodeCount;
            maxPos = g_CameraProgressMax;
            field48 = g_CameraPathKind;
        }
        oldField48 = g_CameraPathKind;
        if (field48 != oldField48) {
            flags |= FLAG_FIELD_48;
        }
        if (target < 0) {
            target = 0;
        } else if (maxPos < target) {
            target = maxPos;
        }
        if (target != g_CameraProgress) {
            flags |= FLAG_POSITION;
        }
        seg = target >> SEGMENT_SHIFT;
        if (seg != g_CameraNode) {
            flags |= FLAG_SEGMENT;
        }
        D_80060918[0] = g_CameraAngleX[0];
        D_80060918[1] = g_CameraAngleX[1];
        D_80060918[2] = g_CameraAngleX[2];

        if (*FIELD(arg0, s32 *, OBJ_DATA) != 0 && (flags & FLAG_ANY_RELOAD)) {
            list = (s32 *)Nsf_OpenEntry(Ent_GetProperty(arg1, ENT_PROP_SLST, 0, 0, 0, 2, 0, 0, 0), 1, 1);
            steps = seg - g_CameraNode;
            dist = steps;
            if (steps < 0) {
                dist = -steps;
            }
            idx = seg;
            if ((flags & FLAG_TARGET) || seg < dist || (count - (seg + 1)) < dist) {
                ((void (*)(s32))Zone_QueueKillAll)(g_CameraNode);
                func_80014420();
                tmp = D_8005F398;
                D_8005F398 = D_8005F3D0;
                D_8005F3D0 = tmp;
                if (seg < (count >> 1)) {
                    idx = 0;
                    D_8005F390 = (s32)func_80029800((u16 *)list[0 + OBJ_DATA / 4], D_8005F390, (u16 *)tmp, 1);
                    Zone_ApplyLoadLists(arg1, 0, 2);
                    Zone_QueueDrawLists(arg1, 0, 2);
                    steps = seg - idx;
                } else {
                    idx = count - 1;
                    D_8005F390 = (s32)func_80029800((u16 *)list[count + OBJ_DATA / 4], D_8005F390, (u16 *)tmp, 0);
                    Zone_ApplyLoadLists(arg1, idx << SEGMENT_SHIFT, 1);
                    Zone_QueueDrawLists(arg1, idx << SEGMENT_SHIFT, 1);
                    steps = seg - idx;
                }
            } else {
                idx = g_CameraNode;
            }
            p = list + idx;
            while (steps != 0) {
                tmp = D_8005F398;
                D_8005F398 = D_8005F3D0;
                D_8005F3D0 = tmp;
                if (steps > 0) {
                    p++;
                    idx++;
                    steps--;
                    D_8005F390 = (s32)func_80029800((u16 *)p[OBJ_DATA / 4], D_8005F390, (u16 *)tmp, 1);
                    Zone_ApplyLoadLists(arg1, idx << SEGMENT_SHIFT, 2);
                    Zone_QueueDrawLists(arg1, idx << SEGMENT_SHIFT, 2);
                } else {
                    D_8005F390 = (s32)func_80029800((u16 *)p[OBJ_DATA / 4], D_8005F390, (u16 *)tmp, 0);
                    p--;
                    idx--;
                    steps++;
                    Zone_ApplyLoadLists(arg1, idx << SEGMENT_SHIFT, 1);
                    Zone_QueueDrawLists(arg1, idx << SEGMENT_SHIFT, 1);
                }
            }
            Nsf_CloseEntry((void *)Ent_GetProperty(arg1, ENT_PROP_SLST, 0, 0, 0, 2, 0, 0, 0), 1);
            Zone_RunEntityQueue(FIELD(arg1, void *, 4), 0x13B);
        }

        if (flags & FLAG_ANY_RELOAD) {
            if (flags & FLAG_OBJECT) {
                data = FIELD(arg0, s32, OBJ_DATA);
                if (D_8006CBB4 == SCREEN_MODE_PENDING && !(arg3 & ARG_KEEP_SCREEN)) {
                    keepScreen = 0;
                }
                g_CurrentZone = (void *)arg0;
                g_CameraProgress = target;
                g_CameraPath = arg1;
                g_CameraNode = seg;
                g_CameraNodeCount = count;
                g_CameraProgressMax = maxPos;
                g_CameraPathKind = field48;
                func_80020998((void *)(data + DATA_SUB), arg3);
                if (keepScreen != 0) {
                    func_80012DC0();
                }
                for (i = 0; i < FIELD(data, s32, DATA_COUNT); i++) {
                    sub = data + DATA_ENTRIES + i * ENTRY_STRIDE;
                    if ((FIELD(data, s32, DATA_FLAGS + i * ENTRY_STRIDE) & DATA_FLAG_ACTIVE) && Nsf_CloseEntry((void *)sub, 0) != 0) {
                        entry = Nsf_OpenEntry((void *)sub, 1, 0);
                        if (!(FIELD(FIELD(entry, s32, OBJ_DATA), s32, DATA_FLAG_OFF) & 1)) {
                            func_8001FFCC(entry);
                        }
                    }
                }
            } else {
                g_CurrentZone = (void *)arg0;
                g_CameraProgress = target;
                g_CameraPath = arg1;
                g_CameraNode = seg;
                g_CameraNodeCount = count;
                g_CameraProgressMax = maxPos;
                g_CameraPathKind = field48;
            }
            if (flags & FLAG_FIELD_48) {
                ((void (*)(s32))func_800221DC)(oldField48);
            }
            func_80022224(D_800608FC);
            ((void (*)(s32 *, s32, s32 *, s32 *, s32))Cam_GetPathPoint)((s32 *)g_CameraPath, g_CameraProgress, (s32 *)((u8 *)&D_800608FC - 0xE4), (s32 *)((u8 *)&D_800608FC + 0x28), 1);
            D_80060888 = Ent_GetPropertyValue(g_CameraPath, 0x1AA, g_CameraProgress, 0, 0, 3, 0);
            shortVal = NO_VALUE;
            if (flags & (FLAG_OBJECT | FLAG_TARGET)) {
                res = Ent_GetProperty(g_CameraPath, 0x176, 0, 0, 0, 1, 0, 0, 0);
                if (res != 0) {
                    shortVal = *(s16 *)res;
                }
                D_80060900 = shortVal;
                shortVal = NO_VALUE;
                res = Ent_GetProperty(g_CameraPath, 0x176, g_CameraProgressMax, 0, 0, 1, 0, 0, 0);
                if (res != 0) {
                    shortVal = *(s16 *)res;
                }
                D_80060904 = shortVal;
            }
            Zone_RunEntityQueue(g_CurrentZone, -1);
        }
        /* The camera's 9 words from g_CameraX take the ones Cam_GetPathPoint wrote 0x24 bytes on */
        for (i = 0; i < 9; i++) {
            g_CameraX[i] = g_CameraX[i + 9];
        }
        geom = ((s32 (*)(void))func_80023D7C)();
        if (D_80060884 != geom) {
            D_80060884 = geom;
            SetGeomScreen(geom);
        }
    }
    D_80067FC8 = 0;
    if (g_Player == 0 && FIELD(D_8006759C, s32, OBJ_DATA) != SAVE_MAGIC) {
        Obj_Spawn(&D_8006CB48, 0, 0, 0, 0, 1);
        data = (s32)D_8006759C + D_8006CD7C * 0x18;
        FIELD(g_Player, s32, 0x60) = FIELD(data, s32, 0x1E8);
        FIELD(g_Player, s32, 0x64) = FIELD(data, s32, 0x1EC);
        FIELD(g_Player, s32, 0x68) = FIELD(data, s32, 0x1F0);
        FIELD(g_Player, void *, 0x18) = g_CurrentZone;
        D_8006CC84 = -1;
        Sys_SaveCheckpoint((GoolObject *)g_Player, (u8 *)&g_CurrentZone + 0x8C, 1, 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800200A8", Cam_MoveAlongPath);
#endif

#ifdef NON_MATCHING
extern s32 g_TimerTicks;
extern V4 D_80060948;
extern V4 D_8006094C;
extern V4 D_80060950;
extern s32 D_80060954;
extern s32 D_8006CBE8;

void func_80020998(S *arg0, s32 arg1) {
    D_80060954 = g_TimerTicks;
    D_80060948 = D_8006094C;
    D_80060950 = arg0->f2c;
    D_8006CBE8 = arg0->f1c;
    if ((arg1 & 4) == 0) {
        func_80034238(&arg0->f24, arg1 & 4, &D_80060950, arg0);
    }
}
#else
INCLUDE_ASM("asm/nonmatchings/game/800200A8", func_80020998);
#endif
