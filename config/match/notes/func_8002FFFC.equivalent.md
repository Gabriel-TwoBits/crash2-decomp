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

Result: `best.c` (same as `attempt2.c`) agrees on 462 of 500 trials (seeds 0 and 1: 462, 463). All the disagreements
are the same kind: both versions take an access fault but at different addresses (retail at a vertex's y, the candidate
at the x of the next vertex, 12 bytes on). Four attempts (carrying the previous y, struct copies field by field, explicit
x/z load order in the interpolation) did not change it; the generated assembly could not be inspected in this session. The
logic of every branch of the assembly has its counterpart in the C; the remaining difference is probably the order of
two loads the compiler schedules differently.
