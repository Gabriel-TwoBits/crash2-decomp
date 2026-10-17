# func_8002FFFC notes

Clips 6 quads (vertex index table `D_8005BBB8`, 4 indices each, vertices of 12 bytes `{x, y, z}` at `arg1`) against the
plane `y == arg0` and writes line segments (two vertices each) to `arg2`; returns the segment count.

- Pass 1: for the first 4 quads, each of the 3 edges (corner i to i+1) gets a 12-byte slot `{x, hit, z}` on the stack;
  when the plane strictly lies between the edge's y values, `hit = 1` and x, z are interpolated in 8.8 fixed point.
- Pass 2: for each of the 6 quads, the 4 slot indices come from `D_8005BC98`. Two hit slots give a segment at y = `arg0`
  and end the quad. Corners with `y == arg0` pair up: the second one writes a segment from the first (skipped when the
  previous corner is also on the plane and its slot was already used). After the 4 corners, a single hit slot plus a
  corner on the plane gives a segment (the corner is dereferenced even when null, as the retail code does); a lone corner
  on the plane writes it twice and returns 1 at once.

Result: `best.c` (`earlier.c` with the previous-corner index kept in a variable) agrees on 462 of 500 trials, the same as
`earlier.c`. All disagreements are the same kind: both versions take an access fault but at different addresses (trial 5:
retail at 0x8020031c, candidate at 0x80200324, the x of the next vertex). The previous run's four attempts and this one
(indexing `(i - 1) & 3` through a variable) did not change it. The generated assembly could not be inspected (compiler
and probe commands were not runnable in this session), so the cause is unconfirmed; probably the order of two loads the
compiler schedules differently. Every branch of the assembly has its counterpart in the C, so `equivalent.c` is not written:
not every trial agrees.
