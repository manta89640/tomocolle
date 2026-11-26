#ifndef OV094_MAINPROGRAM_H
#define OV094_MAINPROGRAM_H

#include "PersonalityScene.h"
#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "island/Apartment.h"
#include "sound/SoundManager.h"

// What overlay 94's code uses of the main program, which only the overlay's source includes (the scene's classes, in
// PersonalityScene.h, are also included by the main program's scene constructors, src/scene/SceneManager.cpp).

// The main program's camera (SceneCast::mCamera points at it), as far as this overlay sets it
struct Camera {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ u8 mUnknown08[0xC];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
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

#define RESOURCE_BCA0 0x30414342 // Joint animations

extern "C" {
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern Light gLights;
extern Light gLight1;
extern void *gGameClock;

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
int ObjResourceManager_LoadCell(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void VramManagers_Reset(Renderer3d *renderer);
void Camera_Reset(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
s64 Clock_GetSeconds(void *clock);
void MiiData_GetName(CharacterRecord *character, wchar_t *name); // Its name, as a string
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Copies a saved character back in

extern const int gPersonalityKinds[16];   // By personality: the row of the table below
extern const s16 gStandAnimations[4][5]; // Animation ids, by that row and by mood: standing
}

#endif // OV094_MAINPROGRAM_H
