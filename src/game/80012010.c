#include "common.h"
#include "sdk.h"
#include "game.h"

INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010000);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010010);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010020);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010038);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010044);
INCLUDE_RODATA("asm/nonmatchings/game/80012010", D_80010054);

extern u8 g_EidAlphabet[];

s32 Nsf_EidType(s32 arg0) {
    switch (g_EidAlphabet[((u32)arg0 >> 1) & 0x3F]) {
    case 'V': return 1;
    case 'S': return 4;
    case 'G': return 2;
    case 'W': return 3;
    case 'T': return 5;
    case 'L': return 6;
    case 'Z': return 7;
    case 'C': return 0xB;
    case 'A': return 0xC;
    case 'O': return 0x14;
    case 'M': return 0xD;
    case 'I': return 0x10;
    case 'N': return 0xE;
    case 'P': return 0x11;
    case 'U': return 0x12;
    case 'B': return 0x13;
    case 'D': return 0x15;
    case 'X': return 0xF;
    default: return 0;
    }
}

u8 Nsf_LevelEidChar(s32 arg0) {
    return g_EidAlphabet[arg0];
}

extern u8 g_EidAlphabet[];

s32 func_80012100(s32 arg0) {
    s32 i;
    u8 c = arg0;

    for (i = 0; i < 0x40; i++) {
        if (g_EidAlphabet[i] == c) {
            return i;
        }
    }
    if (c == 0x2D) {
        return 0x63;
    }
    return -0xE;
}


void Nsf_FileName(s32 a0, s32 a1) {
    Nsf_FormatFileName(a0, a1, 0, 0);
}

extern char D_80010000[];
extern char D_80010010[];
extern char D_80010020[];
extern char D_80010038[];
extern char D_80010044[];
extern char D_80010054[];
extern s32 g_UseCd;
extern char D_8005F414[];


char *Nsf_FormatFileName(s32 a0, u32 a1, s32 a2, s32 a3) {
    switch (a0) {
    case 0:
        if (g_UseCd != 0) {
            sprintf(D_8005F414, D_80010000, a1 >> 4, a1);
        } else {
            sprintf(D_8005F414, D_80010010, D_80010020, D_80010038, a1);
        }
        break;
    case 1:
        if (g_UseCd != 0) {
            sprintf(D_8005F414, D_80010044, a1 >> 4, a1);
        } else {
            sprintf(D_8005F414, D_80010054, D_80010020, D_80010038, a1);
        }
        break;
    default:
        return (char *)-14;
    }
    return D_8005F414;
}

s32 Nsf_EntryType(NsdRecord *record) {
    NsfEntry *entry;
    s32 r;

    entry = record->entry;
    if ((u32)entry & NSD_REF_TAG_MASK) {
        r = Nsf_EidType(record->eid);
    } else {
        r = entry->type;
    }
    return r;
}

#ifdef NON_MATCHING
s32 Nsf_CountEntriesOfType(NsfLevel *level, s32 type) {
    s32 i;
    s32 matches;
    s32 offset;

    matches = 0;
    offset = NSD_RECORDS_OFFSET;
    for (i = 0; i < level->nsd->record_count; i++) {
        if (Nsf_EntryType((NsdRecord *)((u8 *)level->nsd + offset)) == type) {
            matches++;
        }
        offset += sizeof(NsdRecord);
    }
    return matches;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80012010", Nsf_CountEntriesOfType);
#endif

#ifdef NON_MATCHING
s32 func_8001232C(void *arg0) {
    NsfPage *page;
    NsfEntry *entry;
    s32 v;
    s32 t;

    page = arg0;
    t = page->type;
    if (t < 6) {
        if (t < 3 && t != 0) {
            if (t == NSF_PAGE_TYPE_TEXTURE) {
                return 1 << NSF_PAGE_SHIFT;
            }
            return -0xE;
        }
        v = (s32)page->entries[page->entry_count];
    } else {
        if (t != NSF_ENTRY_MARK) {
            return -0xE;
        }
        entry = arg0;
        v = (s32)entry->items[entry->item_count];
    }
    if (v < 0) {
        return v - (s32)page;
    }
    return v;
}
#else
INCLUDE_ASM("asm/nonmatchings/game/80012010", func_8001232C);
#endif

s32 Nsf_ItemSize(NsfEntry *entry, s32 index) {
    s32 r;

    if (entry->item_count == 0) {
        r = 0;
    } else {
        r = entry->items[index + 1] - entry->items[index];
    }
    return r;
}
