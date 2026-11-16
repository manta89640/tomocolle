#include <nitro.h>
struct Inner { u8 a[6]; u16 b; int c[10]; };
struct Outer { char m[16]; Inner in; u8 x, y, z, w; u32 sum; };
struct Block : public Outer { BOOL loaded; };
void f(Block *b, Outer *d) { (Outer &)*b = *d; }
