#ifndef NFL_NFL_H
#define NFL_NFL_H

// NFL, the Mii face library (docs/binary.md, "The Mii library"). Its names are not known: the functions keep their
// address names and the types and fields are the project's own, named only as far as the code shows what they are.

#include <nitro.h>

// A Mii's create ID: 8 bytes
typedef struct NFLCreateId {
    u8 data[8];
} NFLCreateId;

// A Mii's data, as lib/MiiData has it: the Wii's RFLCharData, bit fields declared from the low bit
typedef struct NFLCharData {
    /* 0x00 */ u16 unk00Bit0 : 1;
               u16 favoriteColor : 4;
               u16 birthDay : 5;
               u16 birthMonth : 4;
               u16 sex : 1;
               u16 unk00Bit15 : 1;
    /* 0x02 */ u16 name[10];
    /* 0x16 */ u8 height;
    /* 0x17 */ u8 build;
    /* 0x18 */ u8 createId[8];
    /* 0x20 */ u16 unk20Bits0 : 2;
               u16 unk20Bit2 : 1;
               u16 unk20Bits3 : 3;
               u16 faceTexture : 4;
               u16 faceColor : 3;
               u16 faceType : 3;
    /* 0x22 */ u16 unk22Bits0 : 5;
               u16 hairFlip : 1;
               u16 hairColor : 3;
               u16 hairType : 7;
    /* 0x24 */ u16 unk24Bits0 : 6;
               u16 eyebrowRotation : 5;
               u16 eyebrowType : 5;
    /* 0x26 */ u16 eyebrowX : 4;
               u16 eyebrowY : 5;
               u16 eyebrowScale : 4;
               u16 eyebrowColor : 3;
    /* 0x28 */ u16 eyeY : 5;
               u16 eyeRotation : 5;
               u16 eyeType : 6;
    /* 0x2A */ u16 unk2ABits0 : 5;
               u16 eyeX : 4;
               u16 eyeScale : 4;
               u16 eyeColor : 3;
    /* 0x2C */ u16 unk2CBits0 : 3;
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
    /* 0x34 */ u16 unk34Bit0 : 1;
               u16 moleX : 5;
               u16 moleY : 5;
               u16 moleScale : 4;
               u16 moleType : 1;
    /* 0x36 */ u16 creatorName[10];
} NFLCharData; // 0x4A

// Where a polygon attribute command was put in a display list, to be patched later (func_0206d984)
typedef struct NFLDLPatch {
    u8 *cmd;
    u32 *param;
    BOOL unk8;
} NFLDLPatch;

// An entry of NFLObj's two arrays at 0x1A4 and 0x228: a value from the Mii's data and a state (1 to 6 checked,
// 7 set)
typedef struct NFLObjEntry {
    int unk0;
    int value;
    int state;
} NFLObjEntry;

// Something the library keeps in a list from its work area (func_02072c5c): linked through unk2A0. func_0206d4f8
// builds its two display lists at unk2A4 and unk1D84 from a Mii's data
typedef struct NFLObj {
    u32 unk0; // The size of the display list at unk2A4
    u32 unk4; // The size of the display list at unk1D84
    u32 unk8;
    u32 unkC;
    u32 unk10;
    u32 unk14;
    u32 unk18;
    u32 unk1C;
    u32 unk20;
    u32 unk24;
    u32 unk28;
    MtxFx43 unk2C;
    MtxFx43 unk5C;
    u8 unk8C[0x90 - 0x8C];
    int unk90;
    NFLDLPatch unk94[4];
    VecFx32 unkC4;
    u8 unkD0;
    u8 unkD1;
    u8 unkD2;
    u8 unkD3;
    u32 unkD4;
    u32 unkD8;
    const NFLCharData *unkDC;
    u8 unkE0[0x168 - 0xE0];
    int unk168;
    u8 unk16C;
    u8 unk16D[3];
    int unk170;
    int unk174;
    int unk178;
    struct NFLObj *unk17C;
    s16 unk180[18];
    NFLObjEntry unk1A4[11];
    NFLObjEntry unk228[10];
    struct NFLObj *next;
    u8 unk2A4[0x1AE0];
    u8 unk1D84[0x494];
} NFLObj;

// The state of func_0206d4f8 while it makes an NFLObj's display lists
typedef struct NFLMakeCtx {
    VecFx32 unk0[3];
    u32 unk24;
    u32 unk28;
    struct {
        u32 unk0;
        u32 unk4;
    } unk2C[7];
    GXDLInfo unk64;
    u8 unk78[0x1C8 - 0x78];
    NFLObj *unk1C8;
    const NFLCharData *unk1CC;
} NFLMakeCtx;

// What func_0206d984 sets the polygon attributes and material colours of an NFLObj's display lists to
typedef struct NFLDrawParam {
    int light;
    GXPolygonMode polyMode;
    int polygonID;
    int alpha;
    int misc;
    GXRgb diffuse;
    GXRgb ambient;
    GXRgb specular;
    GXRgb emission;
} NFLDrawParam;

// The work area the caller gives func_02072a58, which the library keeps in data_02145360
typedef struct NFLWork {
    u32 unk0; // A VRAM address that func_0206e38c and the four after it advance
    u8 unk4[4];
    u32 unk8;
    u32 unkC;
    struct {
        u32 unk0;
        u32 unk4;
        int unk8;
    } unk10[5];
    u8 unk4C[0x64 - 0x4C];
    void *unk64;
    int unk68;
    NFLObj *unk6C;
    u8 unk70;
    u8 unk71;
    u8 unk72[2];
    NFLObj *unk74; // A list linked through unk17C
    void *unk78;
    u8 unk7C[0x8C - 0x7C];
    int unk8C;
    u8 unk90[0xA8 - 0x90];
    struct {
        int unk0;
        u8 unk4[8];
    } unkA8[5];
} NFLWork;

extern NFLWork *data_02145360;

BOOL func_02072c1c(void);
BOOL func_02073284(const NFLCreateId *id);
BOOL func_0207331c(const NFLCreateId *id);
NFLObj *func_02073060(int index);

#endif
