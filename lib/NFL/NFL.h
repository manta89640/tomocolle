#ifndef NFL_NFL_H
#define NFL_NFL_H

// NFL, the Mii face library (docs/binary.md, "The Mii library"). Its names are not known: the functions keep their
// address names and the types and fields are the project's own, named only as far as the code shows what they are.

#include <nitro.h>

// A Mii's create ID: 8 bytes
typedef struct NFLCreateId {
    u8 data[8];
} NFLCreateId;

// Something the library keeps in a list from its work area (func_02072c5c): linked through unk2A0. The objects in
// the list at unk74 of the work area are linked through unk17C, with a state at unk170 (func_020749e4).
typedef struct NFLWorkUnk10 {
    u32 unk0;
    u32 unk4;
    u32 unk8;
} NFLWorkUnk10;

// A stream of func_02074b68 and the like: where it is in the work area's buffer, a resource index or an address, and
// its state (0 to 5)
typedef struct NFLWorkUnkA0 {
    u32 unk0;
    u32 unk4;
    int unk8;
} NFLWorkUnkA0;

typedef struct NFLObj {
    u8 unk0[0x170];
    int unk170;
    u32 unk174;
    u32 unk178;
    struct NFLObj *unk17C;
    s16 unk180[0x12]; // The parameters func_02075528 makes its palettes from
    NFLWorkUnkA0 unk1A4[11];
    NFLWorkUnkA0 unk228[10]; // func_02075f50 walks eleven, the last over next

    struct NFLObj *next;
} NFLObj;

// The work area the caller gives func_02072a58, which the library keeps in data_02145360
typedef struct NFLWork {
    u8 unk0[0x10];
    NFLWorkUnk10 unk10[6];
    u32 unk58; // The state of the LCG of func_020743a0, seeded with the RTC time in seconds
    u32 unk5C;
    u32 unk60;
    void *unk64;
    int unk68;
    NFLObj *unk6C;
    u8 unk70;
    u8 unk71;
    u8 unk72[2];
    NFLObj *unk74;
    u8 *unk78;
    u32 unk7C;
    u32 unk80;
    u32 unk84;
    u32 unk88;
    int unk8C;
    int unk90;
    u32 unk94;
    u32 unk98;
    u8 unk9C;
    NFLWorkUnkA0 unkA0[7];
} NFLWork;

extern NFLWork *data_02145360;

BOOL func_02072c1c(void);
BOOL func_02073284(const NFLCreateId *id);
BOOL func_0207331c(const NFLCreateId *id);

// A Mii's data, 0x4A bytes in the Wii's layout (RFLCharData): the same record as lib/MiiData's MiiData, whose
// field names these are (docs/binary.md, "The Mii data"). Bit fields are allocated from the low bit.
typedef struct NFLCharData {
    /* 0x00 */ u16 unknown00Bit0 : 1;
               u16 favoriteColor : 4;
               u16 birthDay : 5;
               u16 birthMonth : 4;
               u16 sex : 1;
               u16 unknown00Bit15 : 1;
    /* 0x02 */ u16 name[10];
    /* 0x16 */ u8 height;
    /* 0x17 */ u8 build;
    /* 0x18 */ NFLCreateId createId;
    /* 0x20 */ u16 unknown20Bits0 : 2;
               u16 unknown20Bit2 : 1;
               u16 unknown20Bits3 : 3;
               u16 faceTexture : 4;
               u16 faceColor : 3;
               u16 faceType : 3;
    /* 0x22 */ u16 unknown22Bits0 : 5;
               u16 hairFlip : 1;
               u16 hairColor : 3;
               u16 hairType : 7;
    /* 0x24 */ u16 unknown24Bits0 : 6;
               u16 eyebrowRotation : 5;
               u16 eyebrowType : 5;
    /* 0x26 */ u16 eyebrowX : 4;
               u16 eyebrowY : 5;
               u16 eyebrowScale : 4;
               u16 eyebrowColor : 3;
    /* 0x28 */ u16 eyeY : 5;
               u16 eyeRotation : 5;
               u16 eyeType : 6;
    /* 0x2A */ u16 unknown2ABits0 : 5;
               u16 eyeX : 4;
               u16 eyeScale : 4;
               u16 eyeColor : 3;
    /* 0x2C */ u16 unknown2CBits0 : 3;
               u16 noseY : 5;
               u16 noseScale : 4;
               u16 noseType : 4;
    /* 0x2E */ u16 mouthY : 5;
               u16 mouthScale : 4;
               u16 mouthColor : 2;
               u16 mouthType : 5;
    /* 0x30 */ u16 glassesY : 5;
               u16 glassesScale : 4;
               u16 glassesColor : 3;
               u16 glassesType : 4;
    /* 0x32 */ u16 beardY : 5;
               u16 beardScale : 4;
               u16 beardColor : 3;
               u16 beardType : 2;
               u16 mustacheType : 2;
    /* 0x34 */ u16 unknown34Bit0 : 1;
               u16 moleX : 5;
               u16 moleY : 5;
               u16 moleScale : 4;
               u16 moleType : 1;
    /* 0x36 */ u16 creatorName[10];
} NFLCharData; // 0x4A

// What the display-list code (func_0206ed94) passes the coordinate helpers (func_020743e4 on): how to map x, y and z
// to its axes (the indices at 0xD0 to 0xD2) and which to negate (0xC4 to 0xCC). The name is the project's own.
typedef struct NFLDrawContext {
    u8 unk0[0xC4];
    int unkC4;
    int unkC8;
    int unkCC;
    u8 unkD0;
    u8 unkD1;
    u8 unkD2;
} NFLDrawContext;

#endif
