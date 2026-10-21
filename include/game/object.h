#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "common.h"
#include "game/nsf.h"

/* The game object: what the GOOL interpreter's operands (Gool_ResolveRef, Gool_OpSetAudio) and the object code read.
   Each field has the width and signedness the code reads it with. The size isn't known: the code reads up to 0x16C
   (func_8001D63C), and Obj_InitPool sets up a pool of 0x60 entries 0x23C bytes apart, linked through 0x44, 0x48 and
   0x4C as objects are.

   0x40 to 0xBB are also read as words by index, the interpreter's registers: ((s32 *)obj->links)[index], the index
   reaching past the end of the structure (& 0x1FF in Gool_OpSetAudio). Field names follow CrashEdit's GOOL object
   fields (ObjectFields, index (offset - 0x40) / 4) where the code shows the same use. */

/* GoolObject.links' indexes, CrashEdit's names */
enum GoolLink {
    GOOL_LINK_SELF = 0, /* Obj_Init stores the object itself */
    GOOL_LINK_PARENT = 1, /* Obj_SetParent's parent, whose child list holds the object */
    GOOL_LINK_SIBLING = 2, /* the next object of the parent's child list (Obj_SetParent, Obj_Kill) */
    GOOL_LINK_CHILD = 3, /* the first object of the child list (Obj_SetParent, Obj_Kill) */
    GOOL_LINK_CREATOR = 4, /* the object whose SPWN opcode spawned it (Gool_OpSpawn) */
    GOOL_LINK_PLAYER = 5, /* g_PlayerSlot, stored by Obj_Init */
    GOOL_LINK_COLLIDER = 6, /* func_8001A710 links two colliding objects to each other through it */
    GOOL_LINK_INTERRUPTER = 7, /* the sender of the last event, stored by Gool_SendEvent */
    GOOL_LINK_COUNT = 8
};

typedef struct GoolObject {
    /* 0x00 */ s32 unk_0;
    /* 0x04 */ s32 unk_4;
    /* 0x08 */ void *unk_8;
    /* The type's GOOL entry (type 11), from the level's table (Obj_Init): its items read as CrashEdit's GOOL layout,
       the code (1), the event to state map (3), the states (4) and the animations (5) */
    /* 0x0C */ NsfEntry *exe;
    /* 0x10 */ void *unk_10;
    /* The GOOL entry the state's GOOL index names in unk_10's pool, whose code and pool the state may use
       (Gool_ChangeState; CrashEdit's external GOOL entries) */
    /* 0x14 */ void *state_exe;
    /* The zone (type 7) the object belongs to: the one Obj_SpawnEntity spawned it from, its parent's (Obj_Init),
       g_CurrentZone for the player (Cam_MoveAlongPath); Zone_KillObjects kills by it. An error code in func_8002BF58 */
    /* 0x18 */ NsfEntry *zone;
    /* 0x1C */ s32 state; /* the state's index, Gool_ChangeState */
    /* 0x20 */ s16 unk_20[0x10]; /* halfwords, the block passed by address (obj + 0x20) */
    /* Indexed by 3 bits (Gool_ResolveRef), GoolLink */
    /* 0x40 */ struct GoolObject *links[GOOL_LINK_COUNT];
    /* Six vectors: 0x60 and 0x90 read as x, y, z (func_8001CEDC), 0x6C and 0x78 passed by address (func_8001AA80),
       0x84, 0x90 and 0x9C read as a matrix's rows (Gfx_QueueObject) */
    /* 0x60 */ s32 pos[3]; /* a path point (Obj_SpawnEntity) or the level's start (the player's) */
    /* 0x6C */ s32 rot[3]; /* CrashEdit's rotx, roty, rotz: passed with scale to the model's drawer */
    /* 0x78 */ s32 scale[3]; /* 0x1000 each without a parent (Obj_Init); x negated to flip (Gool_OpPlayAnim) */
    /* 0x84 */ s32 vel[3]; /* CrashEdit's velx, vely, velz */
    /* 0x90 */ s32 target_rot[3]; /* CrashEdit's trotx, troty, trotz: the yaw turned towards (ratan2) */
    /* 0x9C */ s32 unk_9C[3];
    /* 0xA8 */ s32 status_a; /* CrashEdit's statusa */
    /* 0xAC */ s32 status_b; /* CrashEdit's statusb */
    /* 0xB0 */ s32 block_flags; /* the state's block flags (Gool_ChangeState), masking the states it may enter */
    /* 0xB4 */ s32 subtype; /* Obj_Init's, which picks the first state */
    /* 0xB8 */ s32 entity_id; /* the entity's id << 8 (Obj_SpawnEntity), g_EntityFlags' index */
    /* 0xBC */ s32 *sp; /* the stack the operands push and pop, $s6 in Gool_Interpret */
    /* 0xC0 */ s32 pc; /* the next instruction's address, $s5 in Gool_Interpret */
    /* 0xC4 */ u8 *fp; /* the frame Gool_PushFrame opens, $s7 in Gool_Interpret */
    /* 0xC8 */ s32 trans_pc; /* the state's trans hook (Gool_ChangeState), run each frame by Obj_Update */
    /* 0xCC */ s32 event_pc; /* the state's event hook (Gool_ChangeState), run by Gool_SendEvent */
    /* 0xD0 */ s32 unk_D0;
    /* 0xD4 */ s32 misc; /* CrashEdit's misc: Gool_OpChunk's and Save_RunCommand's results */
    /* 0xD8 */ s32 unk_D8;
    /* 0xDC */ u32 frame_time; /* the tick of the last frame Obj_Update ran */
    /* 0xE0 */ u32 state_time; /* the tick Gool_ChangeState entered the state at */
    /* 0xE4 */ s32 unk_E4;
    /* 0xE8 */ u8 *anim; /* into the GOOL entry's animations, item 5 (Gool_OpPlayAnim) */
    /* 0xEC */ u32 anim_frame; /* the frame << 8 (Gool_OpPlayAnim, Gool_OpPlayFrame) */
    /* 0xF0 */ void *entity; /* the zone's entity item (Obj_SpawnEntity), whose properties EFLR and EFLV read */
    /* 0xF4 */ s32 unk_F4;
    /* 0xF8 */ s32 path_length; /* the entity's path points << 8 (Obj_SpawnEntity) */
    /* 0xFC */ s32 unk_FC;
    /* 0x100 */ s32 state_flags; /* the state's flags (Gool_ChangeState) */
    /* 0x104 */ s32 unk_104;
    /* 0x108 */ s32 display_mode; /* CrashEdit's displaymode: the mode func_8001BFDC switches on (spin, timed waits) */
    /* 0x10C */ u32 unk_10C; /* the tick the display mode started, which func_8001BFDC times its waits from */
    /* 0x110 */ s32 unk_110;
    /* 0x114 */ s32 unk_114;
    /* 0x118 */ s32 unk_118;
    /* 0x11C */ s32 event_received; /* CrashEdit's eventreceived: the event an object accepted */
    /* 0x120 */ u8 unk_120; /* 0x120 to 0x123 also read as a u32 (Gfx_QueueObject) and cleared as one (Obj_Init) */
    /* 0x121 */ u8 unk_121;
    /* 0x122 */ u8 unk_122;
    /* 0x123 */ u8 unk_123[1];
    /* 0x124 */ s32 unk_124;
    /* 0x128 */ s32 unk_128;
    /* 0x12C */ s32 voice; /* CrashEdit's voice: the handle Snd_PlaySound returns */
    /* 0x130 */ u8 unk_130[4];
    /* 0x134 */ s32 unk_134;
    /* 0x138 */ s32 unk_138;
    /* 0x13C */ s32 unk_13C;
    /* 0x140 */ u8 unk_140[0x28];
    /* 0x168 */ s32 unk_168;
    /* 0x16C */ s32 unk_16C;
} GoolObject;

#endif /* GAME_OBJECT_H */
