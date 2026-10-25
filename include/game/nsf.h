#ifndef GAME_NSF_H
#define GAME_NSF_H

#include "common.h"

/* A level streams from its .NSF, 64 KB pages of entries, indexed by its .NSD (file names: Nsf_FormatFileName) */

/* Nsf_RelocatePage refuses a page without it */
#define NSF_PAGE_MAGIC 0x1234
/* A page LoadImage sends whole to VRAM (Nsf_PageIn), which Nsf_RelocatePage leaves as it is */
#define NSF_PAGE_TYPE_TEXTURE 1
/* A page's size as a shift: Nsf_OpenLevel allocates its page buffers count << 16 bytes, one each 1 << 16 apart */
#define NSF_PAGE_SHIFT 16
/* A page's CD sectors as a shift: Nsf_ReadPages reads 32 sectors of 2,048 bytes per page */
#define NSF_PAGE_SECTOR_SHIFT 5

/* The NSD's hash table: Nsf_LookupEid scans from bucket (EID >> 15) & 0xFF */
#define NSD_BUCKET_COUNT 0x100
#define NSD_BUCKET_SHIFT 15
#define NSD_BUCKET_MASK 0xFF
/* Where Nsd's records start, for the C that reads them at a raw offset */
#define NSD_RECORDS_OFFSET 0x520

/* An NSD record's first word holds an entry's address when these bits are clear (Nsf_EntryType) */
#define NSD_REF_TAG_MASK 3

/* The page slots Nsf_OpenLevel allocates a buffer for, at most, and the ones it leaves without */
#define NSF_LEVEL_PAGE_SLOTS 0x15
#define NSF_LEVEL_UNK_SLOTS 0x25

/* An entry's halfword at 2, where a page has its type: func_8001232C tells the two apart by it */
#define NSF_ENTRY_MARK 0x100

/* An entry, as Nsf_RelocatePage leaves it */
typedef struct NsfEntry {
    /* 0x00 */ u16 unk_0;
    /* 0x02 */ u16 mark; /* NSF_ENTRY_MARK */
    /* 0x04 */ u32 eid; /* the key Nsf_PageIn and Nsf_PageOut look the NSD's records up by */
    /* 0x08 */ s32 type; /* what Nsf_EntryType returns for a loaded entry, Nsf_EidType's numbering otherwise */
    /* 0x0C */ s32 item_count; /* Nsf_RelocatePage's bound over items */
    /* item_count + 1 offsets from the entry, pointers once Nsf_RelocatePage ran; an item ends where the next starts
       (func_800123BC) */
    /* 0x10 */ u8 *items[0];
} NsfEntry;

/* A page, as Nsf_ReadPages reads it into a page slot's buffer */
typedef struct NsfPage {
    /* 0x00 */ u16 magic; /* NSF_PAGE_MAGIC */
    /* 0x02 */ u16 type; /* NSF_PAGE_TYPE_TEXTURE, or the 0, 3, 4 and 5 Nsf_PageIn switches on */
    /* 0x04 */ u32 unk_4; /* a texture page's EID: Nsf_PageIn passes it to Nsf_LookupEid */
    /* 0x08 */ s32 entry_count; /* Nsf_RelocatePage's bound over entries */
    /* 0x0C */ u32 unk_C;
    /* entry_count + 1 offsets from the page, pointers once Nsf_RelocatePage ran (the last one too) */
    /* 0x10 */ NsfEntry *entries[0];
} NsfPage;

/* One record per entry of the level, from the NSD's 0x520 */
typedef struct NsdRecord {
    /* The loaded entry when its two low bits are clear (Nsf_EntryType); else odd, the page reference Nsf_PageOut
       stores back, or a VRAM handle with bit 1 set (Nsf_PageIn) */
    /* 0x00 */ NsfEntry *entry;
    /* 0x04 */ u32 eid; /* Nsf_EntryType decodes it with Nsf_EidType */
} NsdRecord;

/* The .NSD, as Cd_ReadFile reads it for Nsf_OpenLevel; the records end its known part */
typedef struct Nsd {
    /* Record indexes on the disc, which Nsf_OpenLevel turns into pointers into records */
    /* 0x000 */ NsdRecord *buckets[NSD_BUCKET_COUNT];
    /* The words of the table Nsf_OpenLevel allocates for the pages, which a page reference indexes shifted right by
       one; Nsf_ReadPages reads 32 sectors from that index's */
    /* 0x400 */ s32 page_count;
    /* 0x404 */ s32 record_count; /* Nsf_CountEntriesOfType's bound over records */
    /* 0x408 */ s32 unk_408[4];
    /* 0x418 */ s32 unk_418; /* Nsf_ReadPages adds it and the page's 32 sectors to g_NsfStartSector */
    /* 0x41C */ s32 unk_41C; /* only the pages below it may be read at unk_420's sectors (func_800130A0) */
    /* By page: a sector offset in bits 6 and up (Nsf_ReadPages), a value in bits 0 to 5 (func_800130A0) */
    /* 0x420 */ u32 unk_420[0x40];
    /* 0x520 */ NsdRecord records[0];
} Nsd;

/* The 0x2C-byte slot a page is read into and tracked by: NsfLevel's, and the 10 at 0x80062BB4 and the 16 after them
   (func_80013AE8, Nsf_PageIn) */
typedef struct NsfPageSlot {
    /* 0x00 */ NsfPage *page; /* Nsf_OpenLevel's buffer, read by Nsf_ReadPages and relocated by func_800130A0 */
    /* 1 free (Nsf_FreePageSlot), 2 once func_80013BD8 takes it, 20 and 21 once read (func_80013FD8 waits for them) */
    /* 0x04 */ s16 state;
    /* 0x06 */ s16 unk_6;
    /* 0x08 */ s16 unk_8;
    /* 0x0A */ s16 unk_A;
    /* 0x0C */ s16 unk_C; /* the slot's index (Nsf_OpenLevel) */
    /* The part of ref_count taken with flag 4 (the load lists', Nsf_ApplyEntryList); Nsf_DropLoadListRefs drops it whole */
    /* 0x0E */ s8 unk_E;
    /* 0x0F */ s8 ref_count; /* Nsf_OpenEntry adds to it, Nsf_DropPageRef frees the slot at zero */
    /* 0x10 */ u16 *unk_10;
    /* 0x14 */ s32 unk_14; /* a page reference: Nsf_ReadPages reads the page unk_14 >> 1 */
    /* 0x18 */ s32 unk_18;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ NsdRecord *unk_20; /* what Nsf_LookupEid returns for a texture page's EID (Nsf_PageIn) */
    /* 0x24 */ struct NsfPageSlot *unk_24;
    /* 0x28 */ s32 unk_28;
} NsfPageSlot;

/* The level's paging state, which Nsf_OpenLevel fills and Nsf_CloseLevel frees; at 0x80067588, its fields also read
   as globals by their own addresses (D_80067590, D_800675A0, g_NsfStartSector, ...) */
typedef struct NsfLevel {
    /* 0x00 */ s32 is_open; /* Nsf_OpenLevel does nothing while it is set, Nsf_CloseLevel nothing unless it is 1 */
    /* The number Nsf_FileName formats into the file names, g_LevelFiles' index, and the EID character Input_PickDemo
       names entries with (g_EidAlphabet[level]) */
    /* 0x04 */ s32 level;
    /* 0x08 */ NsdRecord **buckets; /* nsd->buckets, which Nsf_LookupEid scans */
    /* 0x0C */ NsdRecord *records; /* nsd->records */
    /* 0x10 */ s32 *unk_10; /* nsd->unk_408 */
    /* 0x14 */ void *unk_14; /* nsd->records + nsd->record_count, what follows the records */
    /* 0x18 */ Nsd *nsd;
    /* 0x1C */ s32 unk_1C;
    /* 0x20 */ s32 nsf_start_sector; /* g_NsfStartSector */
    /* 0x24 */ s32 unk_24;
    /* 0x28 */ s32 page_slot_count; /* how many page buffers Nsf_OpenLevel could allocate */
    /* 0x2C */ NsfPageSlot page_slots[NSF_LEVEL_PAGE_SLOTS];
    /* 0x3C8 */ s32 unk_3C8; /* NSF_LEVEL_UNK_SLOTS */
    /* 0x3CC */ NsfPageSlot unk_3CC[NSF_LEVEL_UNK_SLOTS]; /* slots without a buffer (Nsf_OpenLevel) */
    /* 0xA28 */ s32 unk_A28;
    /* 0xA2C */ NsfPageSlot *unk_A2C; /* the slot Nsf_ReadPages is reading into (Nsf_UpdatePageSlot) */
    /* 0xA30 */ s32 unk_A30;
    /* 0xA34 */ NsfPageSlot *unk_A34;
    /* 0xA38 */ s32 unk_A38;
    /* 0xA3C */ s32 *unk_A3C; /* nsd->page_count words, -0x12 until a page's slot is stored in them */
    /* 0xA40 */ s32 unk_A40;
} NsfLevel;

#endif /* GAME_NSF_H */
