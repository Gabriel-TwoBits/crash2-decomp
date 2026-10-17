#include "common.h"
#include "sdk.h"
#include "game.h"

#define MAX_DIR_ENTRIES 16
#define DIR_ENTRY_SIZE 0x28

extern char D_8005F284[];
s32 firstfile(char *, void *);
s32 nextfile(void *);
char *strcat(char *, const char *);

s32 func_80035CBC(char *path, void *entries) {
    char pattern[0x80];
    char *entry;
    s32 count;

    entry = entries;
    strcpy(pattern, path);
    strcat(pattern, D_8005F284);
    if ((void *)firstfile(pattern, entry) != entry) {
        return 0;
    }
    count = 1;
    while (1) {
        entry += DIR_ENTRY_SIZE;
        if (count >= MAX_DIR_ENTRIES) {
            break;
        }
        count++;
        if ((void *)nextfile(entry) != entry) {
            count--;
            break;
        }
    }
    return count;
}
