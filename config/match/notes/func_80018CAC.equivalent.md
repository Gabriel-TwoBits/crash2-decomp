func_80018CAC walks the child list of a node and calls func_80018AA8(child, cb, arg) on each one.
The list head is the field at 0x4 when the node kind (field 0) is 2, otherwise the field at 0x4C.
The next pointer is read before the call, and the function returns -0xFF.

Attempt 1 (equivalent.c) agrees on 500 of 500 trials on seeds 0 and 1.
Many of those trials end in the same exception or at the instruction limit, so they were not compared fully.
Nothing disagrees.
