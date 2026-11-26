#ifndef OV103_MAINPROGRAM_H
#define OV103_MAINPROGRAM_H

// What overlay 103 uses of the main program that has no shared header yet: declarations and stand-ins private to the
// overlay, which only DivorceScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes
// DivorceScene.h with the other scenes' headers). As overlay 96's MainProgram.h has them.

#include "DivorceScene.h"

#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "MainHeap.h"
#include "room/Room.h"
#include "sound/SoundManager.h"
#include "text/TextUtil.h"

#include <wchar.h>

extern "C" {
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
// Gives a resource set the animations of an archive
void ResourceSet_SetArchive(void *resourceSet, u32 resType, void *archive);
void MiiData_GetName(SavedCharacter *character, wchar_t *name); // Its name, as a string
}

// The saved game
struct SavedGame;
extern "C" {
extern SavedGame gGameData;
void GameData_ReadCharacter(SavedGame *game, int index, SavedCharacter *character); // Copies a saved character out
void GameData_WriteCharacter(SavedGame *game, int index, SavedCharacter *character); // and back in
BOOL GameData_CharacterExists(SavedGame *game, int index);
}

// The main program's camera
struct Camera {
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

// What the trouble tables (data_02114d78, by category and id) say of a trouble, as far as is used here (as
// src/resident/ResidentLife.cpp has it)
struct TroubleInfo {
    /* 0x0 */ u32 mKind : 8;
    /*     */ u32 mMinutes : 16; // How long it lasts
    /*     */ u32 mUnknown0Bits24 : 8;
    /* 0x4 */ u32 mUnknown4;
    /* 0x8 */ int mUnknown8;
}; // 0xC

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern Camera gCamera;
extern Light gLights[];
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern void *gGameClock;
extern MATHRandContext16 gRandom;
extern const TroubleInfo *const data_02114d78[];
extern const int gPersonalityKinds[16];  // By mPersonality: the row of the two tables below
extern const s16 gStandAnimations[4][5]; // Animation ids, by that row and by mood: standing
extern const s16 gWalkAnimations[4][5];  // The same: walking

void Camera_Reset(Camera *camera);
void Camera_UpdateEye(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirection(Light *light, int unk, u16 angle, int unk2, VecFx32 direction);
void SplitColor(int color, int *r, int *g, int *b); // Splits a colour into its components
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
void VramManagers_Reset(Renderer3d *renderer);
int ObjResourceManager_LoadCell(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, int a, int b);
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);
void ScreenLoader_SetScrollY(ScreenLoader *loader, int plane, int y);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int level);
s64 Clock_GetSeconds(void *clock);
s64 Clock_GetDateTime(void *clock, RTCDate *date, RTCTime *time);
void ParticleSystem_Load(ParticleSystem *particles, void *file, int a, int b, int c, int d, int e);
void func_02080ee4(ParticleSystem *particles);
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws
void func_0207b138(void *manager);                                // Updates
}

static inline const TroubleInfo *GetTroubleInfo(int category, int id) {
    return &data_02114d78[category][id];
}

static inline int GetTroubleKind(int category, int id) {
    return data_02114d78[category][id].mKind;
}

static inline int ToTroubleKind(int category, int id) {
    id = data_02114d78[category][id].mKind;
    return id;
}

// --- Inlines -------------------------------------------------------------------------------------------------------

static inline u16 Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

static inline void Swap(int &a, int &b) {
    int t = a;
    a = b;
    b = t;
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

// The shift is unsigned: as an int, WeddingCamera::Approach (overlay 96) loads it after the vector's x
static inline VecFx32 Shift(const VecFx32 &v, u32 shift) {
    VecFx32 result;
    result.x = v.x >> shift;
    result.y = v.y >> shift;
    result.z = v.z >> shift;
    return result;
}

#endif // OV103_MAINPROGRAM_H
