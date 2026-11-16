#ifndef NFL_NFL_H
#define NFL_NFL_H

// NFL, the Mii face library (docs/binary.md, "The Mii library"). Its names are not known: the functions keep their
// address names and the types and fields are the project's own, named only as far as the code shows what they are.

#include <nitro.h>

// A Mii's create ID: 8 bytes
typedef struct NFLCreateId {
    u8 data[8];
} NFLCreateId;

// A Mii's data, in the Wii's layout (RFLCharData), as lib/MiiData has it: the bit fields are declared low bit first
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

// A command in a display list, to be patched later: where its command byte and its parameters are
typedef struct NFLDLRef {
    u8 *cmd;
    u32 *param;
} NFLDLRef;

// The commands of a face part's display list that func_02071730 patches: a matrix, a texture and its palette
typedef struct NFLPatch {
    NFLDLRef mtx;
    NFLDLRef tex;
    NFLDLRef pltt;
} NFLPatch;

// Something the library keeps in a list from its work area (func_02072c5c): linked through unk2A0
typedef struct NFLObj {
    u8 unk0[0x10];
    u32 unk10; // a texture address (func_0207183c)
    u32 unk14; // a palette address
    u8 unk18[0x20 - 0x18];
    u32 unk20; // a texture address
    u32 unk24; // a palette address
    u8 unk28[0x84 - 0x28];
    u16 unk84;
    u16 unk86;
    u16 unk88;
    u16 unk8A;
    u16 unk8C;
    u16 unk8E;
    int unk90; // a texture format (GXTexFmt)
    u8 unk94[0xC4 - 0x94];
    VecFx32 unkC4;
    u8 unkD0;
    u8 unkD1;
    u8 unkD2;
    u8 unkD3[0xE0 - 0xD3];
    NFLPatch unkE0;
    NFLPatch unkF8;
    NFLPatch unk110;
    NFLPatch unk128;
    NFLPatch unk140;
    NFLDLRef unk158;
    NFLDLRef unk160;
    int unk168;
    u8 unk16C;
    u8 unk16D[0x170 - 0x16D];
    int unk170;
    u8 unk174[0x17C - 0x174];
    struct NFLObj *unk17C; // the next in the list from NFLWork's unk74
    u8 unk180[0x2A0 - 0x180];
    struct NFLObj *next;
} NFLObj;

// The work area the caller gives func_02072a58, which the library keeps in data_02145360
// An entry of NFLWork's unkA0, one for each of seven kinds (func_02072a58 sets unk8 to 0 or 7)
typedef struct NFLWorkEntry {
    u32 unk0;
    u32 unk4;
    int unk8;
} NFLWorkEntry;

// The work area the caller gives func_02072a58, which the library keeps in data_02145360. Its size is 0xF4: main
// passes it at data_02115480, and the next argument at data_02115574.
typedef struct NFLWork {
    u32 unk0;
    u32 unk4;
    u32 unk8;
    u32 unkC;
    u8 unk10[0x58 - 0x10];
    u32 unk58; // the state of a linear congruential generator, seeded with the time in seconds
    u32 unk5C; // its multiplier
    u32 unk60; // its increment
    void *unk64;
    u32 unk68;
    NFLObj *unk6C;
    u8 unk70;
    u8 unk71;
    u8 unk72[2];
    NFLObj *unk74;
    void *unk78;
    u32 unk7C;
    u32 unk80;
    u32 unk84;
    u32 unk88;
    int unk8C;
    int unk90;
    u32 unk94;
    u32 unk98;
    u8 unk9C;
    u8 unk9D[3];
    NFLWorkEntry unkA0[7];
} NFLWork; // 0xF4

// NFLWork from unk74 on. func_02072a58 reaches the entries off the address of unk74, as through a pointer to a
// structure that begins there: so unk74 to the end is likely one structure of the original's
typedef struct NFLWorkFrom74 {
    NFLObj *unk74;
    void *unk78;
    u32 unk7C;
    u32 unk80;
    u32 unk84;
    u32 unk88;
    int unk8C;
    int unk90;
    u32 unk94;
    u32 unk98;
    u8 unk9C;
    u8 unk9D[3];
    NFLWorkEntry unkA0[7];
} NFLWorkFrom74;

// A part of NFLMakeCtx that func_0206ed94 and func_020705d4 make a face part's display list from
typedef struct NFLMakePart {
    u32 unk0[8];
    u8 unk20[0x30 - 0x20];
} NFLMakePart;

// The state of the making of an NFLObj's display lists: unk1C8 the object, unk1CC the Mii's data
typedef struct NFLMakeCtx {
    u8 unk0[0x64];
    GXDLInfo unk64;
    NFLMakePart unk78[7];
    NFLObj *unk1C8;
    const NFLCharData *unk1CC;
} NFLMakeCtx;

extern NFLWork *data_02145360;

BOOL func_02072c1c(void);
BOOL func_02073284(const NFLCreateId *id);
BOOL func_0207331c(const NFLCreateId *id);

#endif
