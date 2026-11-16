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
    /* 0x18 */ NFLCreateId createId;
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

// An entry of NFLObj's two arrays at 0x1A4 and 0x228: a value from the Mii's data and a state (1 to 6 checked,
// 7 set)
typedef struct NFLObjEntry {
    u32 unk0;
    union {
        int value;
        u32 unk4;
    };
    union {
        int state;
        int unk8;
    };
} NFLObjEntry;

// An entry of the work area's array at 0x10
typedef struct NFLWorkUnk10 {
    u32 unk0;
    u32 unk4;
    u32 unk8;
} NFLWorkUnk10;

// A stream of func_02074b68 and the like: where it is in the work area's buffer, a resource index or an address, and
// its state (0 to 5). The objects' entries at 0x1A4 and 0x228 are streams too
typedef NFLObjEntry NFLWorkUnkA0;

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
    MtxFx33 unk5C; // Loaded as the vector matrix with G3_LoadMtx43
    u16 unk80[8]; // From the Mii's data (func_0206ddb8): 2 eyebrowY, 3 eyeRotation, 4 eyeType, 5 faceType, 6
                  // eyebrowRotation, 7 eyebrowType. u16: func_0207183c reads them with ldrh
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
    NFLPatch unkE0;
    NFLPatch unkF8;
    NFLPatch unk110;
    NFLPatch unk128;
    NFLPatch unk140;
    NFLDLRef unk158;
    NFLDLRef unk160;
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

// A part of NFLMakeCtx that func_0206ed94 and func_020705d4 make a face part's display list from
typedef struct NFLMakePart {
    fx32 unk0[8];
    u32 unk20;
    u32 unk24;
    int unk28;
    int unk2C;
} NFLMakePart;

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
    NFLMakePart unk78[7];
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
    u32 unk4; // Advanced with unk0, as a palette address
    u32 unk8;
    u32 unkC;
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
    NFLObj *unk74; // A list linked through unk17C
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
    // Past the work area's 0xF4 bytes: main passes a 0x4000-byte buffer right after it (data_02115574, kept at
    // unk78), and func_0206ddb8 stores 7 at 0x38 of it through data_02145360 (measured)
    u8 unkF4[0x12C - 0xF4];
    int unk12C;
} NFLWork;

// NFLWork from unk74 to its 0xF4 bytes' end. func_02072a58 reaches the entries off the address of unk74, as through a
// pointer to a structure that begins there: so unk74 to the end is likely one structure of the original's
typedef struct NFLWorkFrom74 {
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
} NFLWorkFrom74;

extern NFLWork *data_02145360;

int func_0206b0d8(NFLMakeCtx *ctx);
int func_0206b3f4(NFLMakeCtx *ctx);
int func_0206b740(NFLMakeCtx *ctx);
int func_0206bb24(NFLMakeCtx *ctx);
int func_0206bfa4(NFLMakeCtx *ctx);
int func_0206c2f8(NFLMakeCtx *ctx);
int func_0206c65c(NFLMakeCtx *ctx);
int func_0206c964(NFLMakeCtx *ctx);
const u8 *func_0206cf6c(int category, u32 index);
BOOL func_02072c1c(void);
BOOL func_02073284(const NFLCreateId *id);
BOOL func_0207331c(const NFLCreateId *id);
NFLObj *func_02073060(int index);
BOOL func_020731e0(const NFLCharData *mii);
BOOL func_020735dc(void);
BOOL func_020735fc(void);
void func_020715ac(NFLMakeCtx *ctx);
int func_020728fc(int index, int unk);
// What the display-list code passes the coordinate helpers (func_020743e4 on) is the object: how to map x, y and z to
// its axes (the indices at 0xD0 to 0xD2) and which to negate (unkC4)
typedef NFLObj NFLDrawContext;
void func_020743e4(const NFLDrawContext *ctx, VecFx16 *out, const VecFx16 *in);
void func_02074444(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in);

// G3C_Begin and G3C_End, by their code: not part of g3c.c's unit (0x02005124, 0x02005148)
void func_02005124(GXDLInfo *info, GXBegin primitive);
void func_02005148(GXDLInfo *info);


#endif
