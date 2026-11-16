#ifndef OV096_MAINPROGRAM_H
#define OV096_MAINPROGRAM_H

// What overlay 96 uses of the main program that has no shared header yet: declarations and stand-ins private to the
// overlay, which only WeddingScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes
// WeddingScene.h with the other scenes' headers).

#include "WeddingScene.h"

extern "C" {
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
// Gives a resource set the animations of an archive
void func_02029660(void *resourceSet, u32 resType, void *archive);
void func_0209decc(Character *character);
void func_0209d898(Character *character, TtsPlayer *player); // Gives the speech engine the character's voice
void MiiData_GetName(SavedCharacter *character, wchar_t *name); // Its name, as a string
}

// --- Other main-program things --------------------------------------------------------------------------------

// The saved game
struct SavedGame;
extern "C" {
extern SavedGame gGameData;
void func_0202c264(SavedGame *game, int index, SavedCharacter *character); // Copies a saved character out
void func_0202c28c(SavedGame *game, int index, SavedCharacter *character); // and back in
}

// The main program's camera
struct Camera {
    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }
    void SetTarget(VecFx32 target) {
        mTarget.x = target.x;
        mTarget.y = target.y;
        mTarget.z = target.z;
    }

    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights
struct Light {
    /* 0x0 */ u8 mUnknown0[6];
    /* 0x6 */ u16 mColor;
    /* 0x8 */ u8 mUnknown8[4];
}; // 0xC

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern Camera data_02122764;
extern Light data_021227cc[];
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
extern void *data_02123440;
// By SavedCharacter::mUnknown0227, a table of these by mUnknown0226
struct EventInfo {
    /* 0x0 */ u32 mUnknown0;
    /* 0x4 */ u32 mUnknown4 : 16;
    /*     */ u32 mMinutes : 12; // How long it lasts
    /*     */ u32 mUnknown4Bits28 : 4;
    /* 0x8 */ u32 mUnknown8;
}; // 0xC
extern const EventInfo *data_02114d78[];

void func_0202a640(Camera *camera);
void func_0202a748(Camera *camera);
void func_0202a96c(Light *light);
void Light_SetDirection(Light *light, int unk, u16 angle, int unk2, VecFx32 direction);
void ScaleVec(fx32 scale, const VecFx32 *in, VecFx32 *out);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, int a, int b);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028154(ScreenLoader *loader, int plane, int y);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int level);
s64 Clock_GetSeconds(void *clock);
}


// How long a character's honeymoon lasts
inline u32 GetHoneymoonMinutes(SavedCharacter *character) {
    return data_02114d78[character->mTroubleCategory][character->mTroubleId].mMinutes;
}


// --- Vector helpers -----------------------------------------------------------------------------------------------

static inline fx32 LengthSq(const VecFx32 *v) {
    return FX_Mul(v->x, v->x) + FX_Mul(v->y, v->y) + FX_Mul(v->z, v->z);
}

// The length of (x, z), by the hardware square root
static inline fx32 SqrtXZ(fx32 x, fx32 z) {
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

// (x, z) made one long, as VEC_Normalize does it
static inline FloorPos NormalizeXZ(fx32 x, fx32 z) {
    FloorPos result;
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    s32 sqrt = (s32)CP_GetSqrtResult32();
    t = CP_GetDivResult64();
    t = t * sqrt;
    result.x = (fx32)((t * x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    result.z = (fx32)((t * z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    return result;
}

static inline fx32 Clamp(fx32 value, fx32 min, fx32 max) {
    if (value > max) {
        value = max;
    } else if (value < min) {
        value = min;
    }
    return value;
}

static inline void CopyVec(VecFx32 *dst, const VecFx32 *src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

// The shift is unsigned: as an int, WeddingCamera::Approach loads it after the vector's x, not before
static inline VecFx32 Shift(const VecFx32 &v, u32 shift) {
    VecFx32 result;
    result.x = v.x >> shift;
    result.y = v.y >> shift;
    result.z = v.z >> shift;
    return result;
}

#endif // OV096_MAINPROGRAM_H
