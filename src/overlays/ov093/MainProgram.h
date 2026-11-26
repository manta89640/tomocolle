#ifndef OV093_MAINPROGRAM_H
#define OV093_MAINPROGRAM_H

#include <nitro.h>
#include <nnsys.h>

#include "IslandNameScene.h"
#include "PadInput.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "character/Character.h"
#include "sound/SoundManager.h"

// What overlay 93 uses of the main program that has no shared header yet: STAND-INS and declarations, which only
// the overlay's source includes (the scene factory, src/scene/SceneManager.cpp, includes IslandNameScene.h with the
// other scenes' headers). The names are descriptive, not the original.

// The main program's camera (gCamera)
struct Camera {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ u8 mUnknown10[4];
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
    /* 0x6 */ GXRgb mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x28];
    /* 0x28 */ TPData mTp;             // As the SDK reads it; mTp.touch is set while the screen is touched
    /* 0x30 */ ScreenPoint mLastPoint;
    /* 0x38 */ ScreenPoint mPoint;
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;
};

struct ArchiveLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern ArchiveLoader gArchiveManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern TouchInput gTouchInput;
extern Camera gCamera;
extern Light gLights[];
extern Light gLight1; // The second light, referred to by its own address
extern void *gGameClock;
extern wchar_t data_02123510[]; // The island's name: this scene asks for it

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void VramManagers_Reset(Renderer3d *renderer);
void Clock_GetDate(void *clock, RTCDate *date); // The date with the game's offset applied

void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
void Camera_Reset(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);

// The character the keyboard typed: 8 erases, 7 and 9 add the voicing marks, 11 makes a kana small
u16 KeyboardCore_GetChar(void);
}

#endif // OV093_MAINPROGRAM_H
