# sym
A symbol table library. Strings are interned and turned into symbols
(uint16_t). These symbols can be compared via numeric equality, and
converted back into strings.

## Implementation
sym uses an open-addressed hash-table with linear probing and lazy
deletion.  It is dynamically sized, but to prevent thrashing, the size
will not go below 8 items.

## Memory
sym should be efficient in terms of memory, in the absence of other memory
calls it should expand and contract in place, without leaving holes. It
does this by copying all the interned items into another buffer either
after growing the table, or before shrinking it, and this buffer is
freed after all the elements have been rehashed.

## Hashing
I have not measured the performance of the hashing algorithm, since it
is easily swapped out without affecting the operation of the table, but
it seems to have decent performance.  The only requirement for a hashing
algorithm for this table is that it operates one character at a time,
since this happens while it is being copied into the string pool.

## Tuning
The table can be tuned by adjusting the parameters GROW and SHRINK in
sym.h, as these control when to resize the table.  They are in terms
of percentages.
