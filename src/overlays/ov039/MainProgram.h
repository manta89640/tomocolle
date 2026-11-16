#ifndef OV039_MAINPROGRAM_H
#define OV039_MAINPROGRAM_H

#include "SceneManager.h"
#include "TravelScene.h"

// What overlay 39's code uses of the main program, which only the overlay's source includes (the scene's class, in
// TravelScene.h, is also included by the main program's scene constructors, src/scene/SceneManager.cpp).

// --- The main program, as far as this overlay uses it ---

// The main program's camera (data_02122764): SceneCast::mCamera points at it. As MainProgram3d.h has it.
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

// One of the main program's 3D lights (data_021227cc is an array of them; func_0202a96c resets one)
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
extern Renderer3d data_02122824;
extern Light data_021227cc[]; // The lights (as overlay 110 declares them)
extern Light data_021227d8[];
extern MATHRandContext16 gRandom;

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown);
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_0202b0e8(Renderer3d *renderer);
void func_0202a640(Camera *camera); // Resets it
void func_0202a96c(Light *light);   // Resets it
// The screens' brightness and fades (as overlays 72, 76 and 96 call them)
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int level);
BOOL ScreenControl_BlendFadeIn(ScreenControl *screens, int planes, int step);
BOOL ScreenControl_BlendFadeOut(ScreenControl *screens, int planes, int step);

int MiiData_GetSex(CharacterRecord *mii); // 0 for a man
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Copies a saved character out
}

#endif // OV039_MAINPROGRAM_H
