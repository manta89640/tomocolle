#ifndef OV076_MAINPROGRAM_H
#define OV076_MAINPROGRAM_H

#include "MiiEditScene.h"

// What overlay 76's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's classes, in MiiEditScene.h, are also included by the main program's
// scene constructors, src/scene/SceneManager.cpp).

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The main program's camera (data_02122764; func_0202a640 resets it). As overlay 109 has it.
struct Camera {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ u8 mUnknown08[0xC];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
};

// The screens: which planes are shown (data_0211d604).
struct ScreenControl {
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }
    void SetUnknown44(int value) {
        mUnknown44 = value;
        mUnknown44Changed = TRUE;
    }
    void SetUnknown48(int value) {
        mUnknown48 = value;
        mUnknown48Changed = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0x40];
    /* 0x44 */ int mUnknown44;
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x10];
    /* 0xE8 */ BOOL mUnknown44Changed;
    /* 0xEC */ BOOL mUnknown48Changed;
};

// Graphics resources, loaded by file name into VRAM.
struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
extern ScreenControl data_0211d604;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}


// A Mii's data: the first 0x4A bytes of a saved character, copied as a structure
struct MiiData {
    /* 0x00 */ u8 mBytes[0x4A];
};

extern "C" {
// Of the Mii library (lib/NFL)
int NFL_GetFaceStatus(int index);
}

// Whether the character's face is not in the Mii library (yet): a BOOL made by an inline function, as the scene's
// Draw tests it
static inline BOOL IsFaceClear(CharacterBase *character) { return !NFL_GetFaceStatus(character->mFaceId); }

#endif // OV076_MAINPROGRAM_H
