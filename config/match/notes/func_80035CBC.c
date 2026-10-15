#include "common.h"
#include "sdk.h"
#include "game.h"

extern char D_8005F284[];
int firstfile(char *, int);
int nextfile(int);
char *strcat(char *, char *);
char *strcpy(char *, char *);

int func_80035CBC(char *arg0, int arg1) {
    char buf[128];
    int n = 0;
    int p = arg1;

    strcpy(buf, arg0);
    strcat(buf, D_8005F284);
    if (p == firstfile(buf, p)) {
        n++;
        while (1) {
            p += 0x28;
            if (n >= 16) break;
            n++;
            if (p != nextfile(p)) { n--; break; }
        }
        return n;
    }
    return 0;
}
