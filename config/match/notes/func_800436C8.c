#include "common.h"

/* Approximation in plain C: the original takes $v1 and the caller's frame implicitly and returns through $t8. */
void func_800436C8(int *obj, int **frame) {
    int *src = frame[10];
    obj[0x118 / 4] = src[0];
    obj[0x11C / 4] = src[1];
}
