#ifndef OV075_SCENE40POOL_H
#define OV075_SCENE40POOL_H

#include <nitro.h>

// INTERIM: the file's 4-byte-aligned .rodata objects are one pool at 0x0217B420 (data_ov075_0217b420), which the
// code reads at constant offsets from its start. Until the pool is laid out (every static of the file declared,
// so that the compiler's size-sorted list puts each at its ROM offset; ov075-e does that last, see
// docs/decompiling.md, "How a file's data is laid out"), a function that reads it names its object with this
// macro at its place, in its own section of the file:
//
//     #define sLevelTextPosition SCENE40_POOL(ScreenPoint, 0x2C) // TEMP pool
//
// and uses the name as it would the static. When the pool is laid out, each such #define becomes the static
// declaration, `static const ScreenPoint sLevelTextPosition = {...};`, with the ROM's values.
extern "C" const u8 data_ov075_0217b420[];

#define SCENE40_POOL(type, offset) (*(const type *)(data_ov075_0217b420 + (offset)))

#endif // OV075_SCENE40POOL_H
