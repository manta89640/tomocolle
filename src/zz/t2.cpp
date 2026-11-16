#include <nitro.h>
struct Data {
    char magic[16];
    u8 id[8];
    int a;
    u8 b, c;
    u16 d;
    int e[50];
    u8 f[6];
    u8 g[2];
    u8 h[0x10];
    u32 sum;
};
struct Block : public Data {
    BOOL loaded, valid;
};
void f(Block *b, Data *d) { (Data &)*b = *d; }
