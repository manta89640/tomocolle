#ifndef NFL_NFL_H
#define NFL_NFL_H

// NFL, the Mii face library (docs/binary.md, "The Mii library"). Nothing in the ROM gives its functions' names (no
// string, no assert), so every name here is the project's own, given for what the code does: NFL_ for the functions
// called from outside the library, NFLi_ for the rest, sNFL for its data. Fields are named as far as the code shows
// what they are.
//
// The resource (NFL_Res.dat) is read by category (NFLi_GetResourceItem): 0 beard meshes, 1 beard textures, 2 eye
// textures, 3 eyebrow textures, 4 face meshes, 5 face textures, 6 the hair's skin-coloured meshes, 7 the glasses'
// mesh, 8 glasses textures, 9 hair meshes, 10 hair textures, 11 the mole's texture, 12 the shared palette, 13 mouth
// textures, 14 mustache textures, 15 the nose's skin-coloured meshes, 16 nose meshes, 17 nose textures, 18 the hair's
// favourite-colour meshes (by the parts and fields that read each).

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

// Where a polygon attribute command was put in a display list, to be patched later (NFL_SetFaceDrawParam): flip
// says the mesh was drawn mirrored (hair flip), so its back faces are culled
typedef struct NFLPolyAttrPatch {
    u8 *cmd;
    u32 *param;
    BOOL flip;
} NFLPolyAttrPatch;

// A command in a display list, to be patched later: where its command byte and its parameters are
typedef struct NFLDLRef {
    u8 *cmd;
    u32 *param;
} NFLDLRef;

// The commands of a face part's display list that NFLi_PatchFacePart patches: a matrix, a texture and its palette
typedef struct NFLPatch {
    NFLDLRef mtx;
    NFLDLRef tex;
    NFLDLRef pltt;
} NFLPatch;

// A stream: something to be copied into the work area's ring buffer (NFL_FillBuffer) and from there into VRAM
// (NFL_LoadVram). bufOffset is where it is in the buffer; state is 0 to be copied, 1 and 3 to 5 copies in progress or
// cut short by the budget, 2 copied, 7 nothing to load. The work area's seven streams keep their VRAM address in
// vramAddr; a face's texture and palette streams keep the value of the Mii's data they were made from (the part
// functions compare it) in value
typedef struct NFLStream {
    u32 bufOffset;
    union {
        int value;
        u32 vramAddr;
    };
    int state;
} NFLStream;

// One of the work area's six shared texture slots: the textures of the expressions (slots 0 to 4) and the mole's
// (slot 5), given VRAM once (allocated) and used by every face
typedef struct NFLSharedTex {
    u32 texAddr;
    u32 plttAddr;
    u32 allocated;
} NFLSharedTex;

// A face: the display lists of a Mii's head that NFL_MakeFace builds from its data, the face parts' second list at
// partsDl, and the streams that load its textures and palettes. The library keeps its faces in a list (NFL_AddFace),
// linked through next, and those with streams to load in a second one (the work area's loadList), linked through
// nextLoad
typedef struct NFLFace {
    u32 dlSize;          // The size of the display list at dl
    u32 partsDlSize;     // The size of the display list at partsDl
    u32 colorTexAddr;    // The colour texture (NFLi_MakeColorTexture): its VRAM address
    u32 colorPlttAddr;   // and its palette's
    u32 eyeTexAddr;      // The eyes' texture and palette
    u32 eyePlttAddr;
    u32 expr4EyeTexAddr; // Expression 4's eye texture and palette (NFLi_PrepareExpression4Texture)
    u32 expr4EyePlttAddr;
    u32 mouthTexAddr;    // The mouth's texture and palette
    u32 mouthPlttAddr;
    u32 partDepth;       // NFL_MakeFace's third argument: added to the depth of the face parts' quads
    MtxFx43 mtx;
    MtxFx33 nrmMtx; // Loaded as the vector matrix with G3_LoadMtx43
    u16 exprFields[8]; // From the Mii's data (NFLi_PrepareFacePartTextures) for the expressions: 0 eyeColor, 1
                       // mouthColor, 2 eyebrowY, 3 eyeRotation, 4 eyeType, 5 faceType, 6 eyebrowRotation, 7 eyebrowType.
                       // u16: NFLi_ApplyExpression0 reads them with ldrh
    int texFmt;        // The work area's texFmt when the face was made
    NFLPolyAttrPatch polyPatches[4]; // Of the hair's three meshes and the glasses'
    VecFx32 negate;                  // Which of the face's axes to negate: x for y, y for z, z for x (NFLi_MapVecFx32)
    u8 yAxis;                        // The face's axis that the library's y goes to
    u8 zAxis;                        // and z
    u8 xAxis;                        // and x
    u8 unkD3;
    u32 texBase;                     // The face's texture VRAM, from the work area's faceTexBase
    u32 plttBase;                    // and palette VRAM, from facePlttBase
    const NFLCharData *mii;          // The Mii's data it was made from
    NFLPatch browPatch;              // The face parts' commands that the expressions rewrite: the eyebrow of kind 3
    NFLPatch browMirrorPatch;        // and of kind 4, mirrored (NFLi_MakeEyeBrowPart),
    NFLPatch eyePatch;               // the eyes, the same,
    NFLPatch eyeMirrorPatch;
    NFLPatch mouthPatch;             // the mouth,
    NFLDLRef browTrans;              // and the translations of the two eyebrows
    NFLDLRef browMirrorTrans;
    int expression;                  // The expression NFL_SetExpression applied
    u8 exprFlags;                    // The expressions it may apply (bit n for kind n), the work area's exprFlags
    u8 unk16D[3];
    int state; // 4 to be made (NFL_AddFace, NFL_ResetFace), 0 textures to load, 1 palettes to load, 2 loaded, 3 failed
    int loadedSize;           // Of the stream being loaded, the bytes already in VRAM
    int copiedSize;           // and the bytes already copied into the buffer
    struct NFLFace *nextLoad; // The work area's loadList
    s16 keptFields[18];       // The fields of the Mii's data the face was made from (NFLi_KeepFields)
    NFLStream texStreams[11]; // Its textures: 0 the colour texture, 1 eyes, 2 eyebrows, 3 mouth, 4 mustache,
                              // 5 expression 4's eyes, 6 glasses, 7 beard, 8 face, 9 hair, 10 nose
    NFLStream plttStreams[10]; // Their palettes, 0 to 9 the same but for 5. NFLi_CopyFacePltts fills an eleventh,
                               // which lies over next
    struct NFLFace *next;
    u8 dl[0x1AE0];
    u8 partsDl[0x494];
} NFLFace;

// A face part of NFLMakeCtx that NFLi_MakeMouthMolePart and NFLi_MakeEyeBrowPart make a quad of: place is x, y,
// scale x, scale y, width, height, rotation and kind (0 the mole, 1 the mouth, 2 the mustache, 3 and 4 the two sides
// of the eyes and eyebrows), from NFLi_PlaceFaceParts; then its texture
typedef struct NFLMakePart {
    fx32 place[8];
    u32 texAddr;
    u32 plttAddr;
    int texSizeS;
    int texSizeT;
} NFLMakePart;

// The state of NFL_MakeFace while it makes an NFLFace's display lists
typedef struct NFLMakeCtx {
    VecFx32 anchors[3]; // Read from the face mesh: where the nose and glasses (0), the beard (1) and the hair (2) go
    u32 texBase;
    u32 plttBase;
    struct {
        u32 texAddr;
        u32 plttAddr;
    } partAddr[7]; // The textures of the meshes, set by the part functions: 1 the face's, 2 the hair's, 3 the
                   // beard's, 4 the glasses', 5 the favourite-colour hair's, 6 the nose's
    GXDLInfo dl;
    NFLMakePart parts[7]; // 0 to 3 the eyes and eyebrows, 4 the mouth, 5 the mustache, 6 the mole
    NFLFace *face;
    const NFLCharData *mii;
} NFLMakeCtx;

// What NFL_SetFaceDrawParam sets the polygon attributes and material colours of an NFLFace's display lists to
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

// The work area the caller gives NFL_Init, which the library keeps in sNFLWork
typedef struct NFLWork {
    u32 texAddr;     // The next free texture VRAM of the shared slots (NFL_Init's desc), advanced as they are given
    u32 plttAddr;    // and palette VRAM
    u32 faceTexBase; // The faces' texture VRAM, after the shared slots'
    u32 facePlttBase; // and palette VRAM
    NFLSharedTex sharedTex[6];
    u32 randState; // The state of the LCG of NFL_Random, seeded with the RTC time in seconds
    u32 randMul;
    u32 randAdd;
    void *resource; // NFL_Res.dat
    int texFmt;     // The faces' texture format (NFL_Init's desc): GX_TEXFMT_DIRECT (7), where 4-bit images are
                    // converted to direct colours, or a palette one
    NFLFace *faces;
    u8 maxFaces;
    u8 exprFlags; // NFL_Init's flags: the expressions the faces may take, and the shared streams to load
    u8 unk72[2];
    NFLFace *loadList; // The faces with streams to load, linked through nextLoad
    u8 *buf;           // The ring buffer the streams are copied into
    u32 bufSize;
    u32 readOffset;  // Where NFL_LoadVram goes on from
    u32 writeOffset; // Where NFL_FillBuffer goes on from
    u32 pending;     // The bytes between them
    int phase;       // 1 filling, 2 filled, 3 loading, 0 loaded, 4 nothing to do
    int stage;       // What is being loaded: 0 the shared textures, 1 the shared palette, 2 the faces (3 after a failure)
    u32 loadedSize;  // Of the stream being loaded, the bytes already in VRAM
    u32 copiedSize;  // and the bytes already copied into the buffer
    u8 started;      // Set by NFL_FillBuffer's first stream of the frame, which starts the buffer over when it can
    NFLStream streams[7]; // 0 to 5 the shared slots' textures (sNFLSharedStreamSources), 6 the shared palette
    // Past the work area's 0xF4 bytes: main passes a 0x4000-byte buffer right after it (data_02115574, kept at
    // buf), and NFLi_PrepareFacePartTextures stores 7 at 0x38 of it through sNFLWork (measured)
    u8 unkF4[0x12C - 0xF4];
    int unk12C;
} NFLWork;

// NFLWork from loadList to its 0xF4 bytes' end, the state of the loading. NFL_Init reaches the streams off the address
// of loadList, as through a pointer to a structure that begins there: so loadList to the end is likely one structure
// of the original's
typedef struct NFLLoadState {
    NFLFace *loadList;
    u8 *buf;
    u32 bufSize;
    u32 readOffset;
    u32 writeOffset;
    u32 pending;
    int phase;
    int stage;
    u32 loadedSize;
    u32 copiedSize;
    u8 started;
    NFLStream streams[7];
} NFLLoadState;

extern NFLWork *sNFLWork;

int NFLi_MakeBeardMesh(NFLMakeCtx *ctx);
int NFLi_MakeFaceMesh(NFLMakeCtx *ctx);
int NFLi_MakeFavoriteColorHairMesh(NFLMakeCtx *ctx);
int NFLi_MakeHairMesh(NFLMakeCtx *ctx);
int NFLi_MakeHairSkinMesh(NFLMakeCtx *ctx);
int NFLi_MakeGlassesMesh(NFLMakeCtx *ctx);
int NFLi_MakeNoseMesh(NFLMakeCtx *ctx);
int NFLi_MakeNoseSkinMesh(NFLMakeCtx *ctx);
const u8 *NFLi_GetResourceItem(int category, u32 index);
BOOL NFLi_IsInitialized(void);
BOOL NFL_IsCreateIdZero(const NFLCreateId *id);
BOOL NFL_IsCreateIdUnassigned(const NFLCreateId *id);
NFLFace *NFLi_GetFace(int index);
BOOL NFLi_IsValidCharData(const NFLCharData *mii);
BOOL NFLi_HasResource(void);
BOOL NFLi_HasBuffer(void);
void NFLi_MakeFacePartsDL(NFLMakeCtx *ctx);
int NFL_SetExpression(int index, int kind);
// What the display-list code passes the coordinate helpers (NFLi_MapVecFx16 on) is the face: how to map x, y and z to
// its axes (xAxis, yAxis, zAxis) and which to negate (negate)
typedef NFLFace NFLDrawContext;
void NFLi_MapVecFx16(const NFLDrawContext *ctx, VecFx16 *out, const VecFx16 *in);
void NFLi_MapVecFx32(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in);

// G3C_Begin and G3C_End, by their code: not part of g3c.c's unit (0x02005124, 0x02005148)
void func_02005124(GXDLInfo *info, GXBegin primitive);
void func_02005148(GXDLInfo *info);


#endif
