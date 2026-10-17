# func_8002FFFC notes

Clips 6 quads (vertex index table `D_8005BBB8`, 4 indices each, 12-byte vertices `{x, y, z}` at `arg1`) against the
plane `y == arg0` and writes line segments (two vertices each) to `arg2`; returns the segment count.

- Pass 1: for the first 4 quads, each of the 3 edges gets a stack slot `{x, hit, z}`; when the plane lies strictly
  between the edge's y values, `hit = 1` and x, z are interpolated in 8.8 fixed point.
- Pass 2: for each of the 6 quads, slot indices come from `D_8005BC98`. Two hit slots give a segment at `y = arg0`.
  Two corners on the plane give a segment from the first to the second; the segment is skipped if the previous corner
  is also on the plane and that corner's slot was already used. After the 4 corners, one hit slot plus a corner on the
  plane gives a segment (the corner is read even when it is null, as retail does). A single corner on the plane is
  written twice and the function returns 1 at once.

Fix over the earlier run (462/500): retail zeroes 12 bytes of "used" flags at `sp+0xA0`, indexed by slot (0..11);
`earlier.c` declared only 3, so `used[flag[k]]` wrote past the array into other locals. Pass 1 also reads `prev->y`
fresh on every edge, as retail does.

Result: `equivalent.c` agrees on 500/500 (seed 0), 500/500 (seed 1), 2000/2000 (seed 1) and 2000/2000 (seed 2).
Most trials end in the same access fault on random vertex pointers, so they compare calls but not memory.
