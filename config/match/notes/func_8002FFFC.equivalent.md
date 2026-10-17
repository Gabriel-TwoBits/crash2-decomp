# func_8002FFFC

Clips a box (6 faces of 4 vertex indices from D_8005BBB8, vertices 12 bytes each at arg1) against the horizontal plane
y = arg0 and writes the resulting segments (pairs of 12-byte points) to arg2. Returns the segment count.

1. For the 4 side faces, each of the first 3 edges (vertex k to k+1) gets a record {x, flag, z}: flag = 1 and the
   interpolated x/z (8-bit fixed point) when the edge strictly crosses y = arg0.
2. For all 6 faces, D_8005BC98 (signed bytes) gives the 4 edge records per face. Two crossing records in a face emit one
   segment at y = arg0. Otherwise vertices lying on the plane are paired (a per-edge "used" byte stops a shared edge
   being emitted twice). A face with one crossing and one touching vertex emits crossing-to-touching. A single touching
   vertex alone emits a degenerate segment and returns 1 at once, without advancing arg2 (as the assembly does).

Best: `best.c` (= attempt5.c), 483 of 500 trials (seed 0), 484 of 500 (seed 1).

What tried: separate arrays for edge records (451), reading p.y before q.y (477), signed table bytes (483), one frame
struct mimicking the retail stack layout (483, fault addresses closer).

What still disagrees: only trials that end in an unmapped access in both, at a different address (the same
vertex pointer, a neighbouring field). The tester randomises the tables, so the s8 edge indices fall outside the 12
records and read stack garbage whose position depends on the compiler's frame layout; load order inside the vertex
copies also differs. No trial differs in return value, calls or memory. Not marked equivalent.c for that reason.
