#ifndef OV069_MAINPROGRAM_H
#define OV069_MAINPROGRAM_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "SceneManager.h"
#include "Model.h"
#include "character/Character.h"
#include "sound/SoundManager.h"

// What overlay 69's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's class, in SavePlayScene.h, is also included by the main program's
// scene constructors, src/scene/SceneManager.cpp).

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// The screens: which planes are shown.
struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0x44];
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x14];
    /* 0xEC */ int mUnknownEC;
};

// Graphics resources, loaded by file name into VRAM.
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);

extern ScreenControl gScreenControl;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}

// The main program's camera (data_02122764; func_0202a640 resets it). A stand-in with the fields the scene sets.
struct Camera {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ u8 mUnknown08[0xC];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights (data_021227cc is an array of them; func_0202a96c resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

extern "C" {

// --- The game's random number generator: every use is the inline code of the SDK's MATH_Rand16 ---
extern MATHRandContext16 gRandom;

// --- The saved game ---
BOOL func_0202c354(GameData *game, int index); // Whether there is a saved character with that index
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
void func_0202c8f8(GameData *game, int unknown);
BOOL func_0202cd48(GameData *game); // Starts writing the save data; FALSE when it cannot
BOOL func_0202cd54(GameData *game); // The same after func_0202c8f8(game, 1) and func_0202d07c(game, 0)
BOOL func_0202cd74(GameData *game); // The same after func_0202c8f8(game, 0) and func_0202d07c(game, 0)
BOOL func_0202cd38(GameData *game); // Not 0 while the save data is being written

// --- 3D ---
extern Camera data_02122764;
void func_0202a640(Camera *camera);
extern Light data_021227cc[];
// Resets a light: direction straight down, colour white, enabled
void func_0202a96c(Light *light);
// Sets a light's direction: the vector, rotated by the three angles and normalized
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
void func_02029660(void *resourceSet, u32 resType, void *archive); // Gives a resource set the animations of an archive
void *func_020295a4(void *resourceSet, u32 resType);

// --- Characters ---
}


#endif // OV069_MAINPROGRAM_H
