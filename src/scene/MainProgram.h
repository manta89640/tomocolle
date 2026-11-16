#ifndef SCENE_MAINPROGRAM_H
#define SCENE_MAINPROGRAM_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Scene.h"
#include "MessageWindow.h"
#include "ScreenControl.h"
#include "Sprite.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// STAND-INS and declarations for what the main program's scenes in src/scene/ use of the rest of the main program,
// which is not decompiled yet: its global objects with the fields these scenes touch, and its functions by the
// names symbols.txt has for them. Most are the same as the overlays' (src/overlays/ov073/MainProgram.h,
// src/overlays/ov096/WeddingScene.h). The names are descriptive, not the original.

// A point that is built at run time: arrays of these are filled in by a file's static initialiser.
struct InitPoint {
    InitPoint(int x_, int y_) {
        x = x_;
        y = y_;
    }

    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

// The archive loader and the graphics resources, loaded by file name into VRAM.
struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;
struct Camera;

extern "C" {

// --- Archives ---
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);

// --- The screens ---
extern ScreenControl gScreenControl;

// --- Graphics resources ---
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
void VramManagers_Reset(Renderer3d *renderer);
int ObjResourceManager_LoadCell(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown);
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);

// --- 3D ---
extern Camera gCamera;
void Camera_Reset(Camera *camera);
void Camera_SetProjection(Camera *camera, int unknown);
extern MATHRandContext16 gRandom;

// --- The button ---
BOOL func_02031ccc(SpriteButton *button); // Whether it was pressed

// --- The message window ---
void MessageWindow_AddSpriteButton(MessageWindow *window, int index, int a, u8 b, u8 c, WindowPoint position, WindowPoint size);
void MessageWindow_AddTextButton(MessageWindow *window, int index, int a, int b, int c, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset);
void MessageWindow_AddTextInArea(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint a, WindowPoint b, int *charOffset);
void MessageWindow_AddSprite(MessageWindow *window, int a, int b, WindowPoint position);
void func_02033c88(MessageWindow *window, int plane, int objPlane, MessageFile *file, int message, int a,
                   MessageFile *buttonFile, int *charOffset);
void func_02033ddc(MessageWindow *window, int plane, int objPlane, MessageFile *file, int message, int a,
                   MessageFile *buttonFile, int *charOffset);

} // extern "C"

#endif // SCENE_MAINPROGRAM_H
