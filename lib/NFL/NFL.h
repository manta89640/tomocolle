#ifndef NFL_NFL_H
#define NFL_NFL_H

// NFL, the Mii face library (docs/binary.md, "The Mii library"). Its names are not known: the functions keep their
// address names and the types and fields are the project's own, named only as far as the code shows what they are.

#include <nitro.h>

// A Mii's create ID: 8 bytes
typedef struct NFLCreateId {
    u8 data[8];
} NFLCreateId;

// Something the library keeps in a list from its work area (func_02072c5c): linked through unk2A0
typedef struct NFLObj {
    u8 unk0[0x170];
    int unk170;
    u8 unk174[0x2A0 - 0x174];
    struct NFLObj *next;
} NFLObj;

// The work area the caller gives func_02072a58, which the library keeps in data_02145360
typedef struct NFLWork {
    u8 unk0[0x64];
    void *unk64;
    u8 unk68[0x6C - 0x68];
    NFLObj *unk6C;
    u8 unk70;
    u8 unk71;
    u8 unk72[2];
    void *unk74;
    void *unk78;
    u8 unk7C[0x8C - 0x7C];
    int unk8C;
} NFLWork;

extern NFLWork *data_02145360;

BOOL func_02072c1c(void);
BOOL func_02073284(const NFLCreateId *id);
BOOL func_0207331c(const NFLCreateId *id);

#endif
