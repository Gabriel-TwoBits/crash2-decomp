# func_80035CBC

Builds the search pattern `path` + "*" in a 0x80-byte stack buffer, calls `firstfile(pattern, entries)`, and returns 0 if it
does not return `entries`. Otherwise it walks up to 16 entries of 0x28 bytes with `nextfile`, returning how many
consecutive calls returned the entry passed in (the first match counts as 1, capped at 16).

Attempt 1 (written from the assembly, m2c's goto loop rewritten as a while loop): 500 of 500 trials agree on seeds 0 and 1.
Nothing disagrees. The buffer size is a guess: the frame only fixes it at 0x80 bytes or less (0x10 to 0x90).
