#ifndef OV090_MAINPROGRAM_H
#define OV090_MAINPROGRAM_H

// What overlay 90 uses of the main program that has no shared header yet: declarations and stand-ins private to the
// overlay, which only RomanceScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes
// RomanceScene.h with the other scenes' headers).

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "save/SaveBlocks.h"
#include "sound/SoundManager.h"

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BMA0 0x30414D42 // Material animation
#define RESOURCE_BTA0 0x30415442 // Texture SRT animation

// The main program's camera
struct Camera {
    /* 0x00 */ u8 mUnknown00[0x10];
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
};

// One of the main program's 3D lights
struct Light {
    /* 0x0 */ u8 mUnknown0[6];
    /* 0x6 */ GXRgb mColor;
    /* 0x8 */ u8 mUnknown8[4];
}; // 0xC

// The bytes after gSceneMemory's 0x3C: the first of gSceneScratch, which this file reaches through gSceneMemory as
// well (the literal is gSceneMemory's address and the offset is in the instruction), as overlay 75 does
struct RomanceSceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacters[12];
    /* 0x2C */ u16 mUnknown2C[8];
    /* 0x3C */ u8 mUnknown3C[2];
    /* 0x3E */ u8 mRejected; // The answer, for scene 0x40
}; // 0x3F

static inline RomanceSceneMemory &GetSceneMemory() {
    return (RomanceSceneMemory &)gSceneMemory;
}

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78), as src/character/SavedCharacter.cpp
// has it
struct TroubleInfo {
    u32 kind : 8;
    u32 minutes : 16;         // How long it lasts at most
    u32 weight : 8;           // Its weight in the random choice (ChooseTrouble)
    u32 unk4Bits0 : 8;
    u32 mood : 8;             // The mood it puts the character in (SetTroubleMood), 0 to 4
    u32 cooldownMinutes : 12; // Added to mTroubleEnd once it is over
    u32 hasTarget : 4;        // Not 0: about a character or an item, which StartTrouble chooses (mTroubleTarget)
    u32 flags;                // Food troubles: the kinds of food; clothes troubles: the kinds of clothes
}; // 0xC

extern "C" {
extern const TroubleInfo *const data_02114d78[];
extern Camera gCamera;
extern Light gLights[];
extern Light gLight1; // gLights[1]
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern void *gGameClock;
extern u8 gSceneScratch[];
extern MATHRandContext16 gRandom;
extern SaveHeader gIslandState;

void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name); // The id of a resource by name
void ResourceSet_SetArchive(void *resourceSet, u32 resType, void *archive); // Gives a resource set an archive's
BOOL GameData_CharacterExists(GameData *game, int index);
BOOL GameData_GetCharacterSex(GameData *game, int index); // Set for a woman
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character); // Copies a saved character out
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // and back in
int MiiData_GetSex(CharacterRecord *character); // 0 for a man

void Camera_Reset(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
void Clock_GetDate(void *clock, RTCDate *date);
s64 Clock_GetDateTime(void *clock, RTCDate *date, RTCTime *time);
s64 Clock_GetSeconds(void *clock);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void VramManagers_Reset(Renderer3d *renderer);
int ObjResourceManager_LoadCell(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown);
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
}

#endif // OV090_MAINPROGRAM_H
