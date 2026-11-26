#ifndef OV100_MAINPROGRAM_H
#define OV100_MAINPROGRAM_H

// What overlay 100 uses of the main program that has no shared header yet, private to the overlay: only
// AlbumScene.cpp includes it (the scene factory, src/scene/SceneManager.cpp, includes AlbumScene.h).

#include "AlbumScene.h"
#include "GameData.h"
#include "PadInput.h"
#include "SceneManager.h"
#include "ScreenControl.h"
#include "ScrollBar.h"
#include "sound/SoundManager.h"
#include "text/TextUtil.h"

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
};

// The main program's camera
struct Camera {
    /* 0x00 */ u8 mUnknown00[0x14];
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

// What holds the lights (data_021226e4): this scene sets their colours through it
struct Lighting {
    /* 0x00 */ u8 mUnknown00[0xE8];
    /* 0xE8 */ Light mLights[2];
};

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern TouchInput gTouchInput;
extern Camera gCamera;
extern Light gLights[];
extern Light gLight1[];
extern Lighting data_021226e4;
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
extern const VecFx32 gMannequinHeadScale;
extern const VecFx32 gMannequinBodyScale;
// The personalities' kinds (0 to 3), by personality
extern const int gPersonalityKinds[];

void Camera_Reset(Camera *camera);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
void VramManagers_Reset(Renderer3d *renderer);
int ObjResourceManager_LoadCell(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown);
void PaletteLoader_QueueColors(PaletteLoader *loader, void *data, int plane, int a, int b, int c);
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_QueueScreen(ScreenLoader *loader, void *data, int plane, int a, int b);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);
void ScreenLoader_SetScrollY(ScreenLoader *loader, int plane, int y);
void *VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void VramTransferQueue_AddCharData(CharLoader *loader, void *data, int plane, int offset, int size);

void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
void ModelResources_ReleaseModel(void *resources, int resourceId);
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
// Gives a resource set the animations of an archive
void ResourceSet_SetArchive(void *resourceSet, u32 resType, void *archive);
void Model_SetDiffuse(Model *model, int color);
void Model_SetAmbient(Model *model, int color);
void Model_SetSpecular(Model *model, int color);
void Model_SetEmission(Model *model, int color);

// --- The saved game ---
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character); // Reads one
int GameData_GetGoodsCount(GameData *game, int item);         // -1 when they were never had
int GameData_GetClothesCount(GameData *game, int clothes);    // The same for clothes
int GameData_GetInteriorCount(GameData *game, int interior);  // and interiors

int MiiData_GetSex(SavedCharacter *character);
GXRgb NFL_GetFavoriteColor(int index); // lib/NFL

// --- The main program's touch buttons (include/SpriteButton.h) ---
void SpriteButton_SetDisabled(SpriteButton *button, BOOL disabled); // Draws the button darkened while it is set
}

#endif // OV100_MAINPROGRAM_H
