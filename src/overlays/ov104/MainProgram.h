#ifndef OV104_MAINPROGRAM_H
#define OV104_MAINPROGRAM_H

// What overlay 104 uses of the main program that has no shared header yet: declarations and stand-ins private to the
// overlay, which only ReconcileScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes
// ReconcileScene.h with the other scenes' headers).

#include "GameData.h"
#include "MainHeap.h"
#include "ReconcileScene.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "sound/SoundManager.h"
#include "text/TextUtil.h"

#include <wchar.h>

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

// What the main program's table of troubles and outings (data_02114d78, by category and id) has for one
struct TroubleInfo {
    /* 0x0 */ u32 mKind : 8;
    /*     */ u32 mMinutes : 16; // How long it lasts
    /*     */ u32 mUnknown0Bits24 : 8;
    /* 0x4 */ u32 mUnknown4;
    /* 0x8 */ u32 mUnknown8;
}; // 0xC

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;
struct GameClock;

extern "C" {
extern Camera gCamera;
extern Light gLights[];
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern GameClock *gGameClock;
extern MATHRandContext16 gRandom;
extern u8 gSceneScratch[100]; // What a scene leaves for the next, beside gSceneMemory
extern const TroubleInfo *const data_02114d78[];
extern const int gPersonalityKinds[];          // Each personality's kind, 0 to 3
extern const s16 gStandAnimations[4][5];      // Animation ids, by kind and mood: standing
extern const s16 gWalkAnimations[4][5];       // and walking

void Camera_Reset(Camera *camera);
void Camera_UpdateEye(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirection(Light *light, int unk, u16 angle, int unk2, VecFx32 direction);
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
void SplitColor(int color, int *r, int *g, int *b); // Splits a 15-bit colour
s64 Clock_GetSeconds(GameClock *clock);
BOOL GameData_CharacterExists(GameData *game, int index);
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character);
void MiiData_GetName(SavedCharacter *character, wchar_t *name); // Its name, as a string
// Gives a resource set the animations of an archive
void ResourceSet_SetArchive(void *resourceSet, u32 resType, void *archive);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id

// The particle system and the head icon (0x02080e80 to 0x020810e4)
void ParticleSystem_Load(ParticleSystem *particles, void *file, int a, int b, int c, int d, int e);
void func_02080ee4(ParticleSystem *particles);
void ParticleSystem_Draw(ParticleSystem *particles, int unknown);
void func_0207b138(void *manager); // Updates a particle system's manager
}

// --- Helpers -------------------------------------------------------------------------------------------------------

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

// The shift is unsigned, as overlay 96's
static inline VecFx32 Shift(const VecFx32 &v, u32 shift) {
    VecFx32 result;
    result.x = v.x >> shift;
    result.y = v.y >> shift;
    result.z = v.z >> shift;
    return result;
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

// The length of (x, z), by the hardware square root
static inline fx32 SqrtXZ(fx32 x, fx32 z) {
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

#endif // OV104_MAINPROGRAM_H
