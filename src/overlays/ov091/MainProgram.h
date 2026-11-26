#ifndef OV091_MAINPROGRAM_H
#define OV091_MAINPROGRAM_H

#include "CityHallScene.h"
#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "save/SaveBlocks.h"

// What overlay 91's code uses of the main program that has no shared header yet: declarations and stand-ins that
// only the overlay's source includes (the scene's class, in CityHallScene.h, is also included by the main program's
// scene constructors, src/scene/SceneManager.cpp).

// The main program's camera (gCamera), as src/scene/MainProgram3d.h has it
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
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern Camera gCamera;
extern Light gLights[];
extern Light gLight1;
extern SaveHeader gIslandState;
extern u8 gSceneScratch[0x70];

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);
void ScreenLoader_SetScrollY(ScreenLoader *loader, int plane, int y);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void VramManagers_Reset(Renderer3d *renderer);
void Camera_Reset(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id

BOOL GameData_IsCharacterSlotUsed(GameData *game, int index); // Whether there is a saved character with that index
}

// gSceneMemory and gSceneScratch are one block of the main program's .bss (0x0213BA74, 0x3C + 0x70 bytes), which
// src/scene/EmptyScene.cpp defines as two objects. This overlay clears the scratch's first byte through gSceneMemory's
// address (gSceneMemory + 0x3C), as overlay 75 reads it, so its source saw the scratch as part of gSceneMemory.
struct CityHallSceneMemory {
    /* 0x00 */ SceneMemory mMemory;
    /* 0x3C */ u8 mScratch[1];
};

static inline CityHallSceneMemory &GetSceneMemory() {
    return (CityHallSceneMemory &)gSceneMemory;
}

#endif // OV091_MAINPROGRAM_H
