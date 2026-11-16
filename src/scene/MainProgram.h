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
extern ScreenControl data_0211d604;

// --- Graphics resources ---
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);

// --- 3D ---
extern Camera data_02122764;
void func_0202a640(Camera *camera);
void func_0202a93c(Camera *camera, int unknown);
extern MATHRandContext16 gRandom;

// --- The button ---
BOOL func_02031ccc(SpriteButton *button); // Whether it was pressed

// --- The message window ---
void func_02033390(MessageWindow *window, int index, int a, u8 b, u8 c, WindowPoint position, WindowPoint size);
void func_0203373c(MessageWindow *window, int index, int a, int b, int c, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset);
void func_020338d0(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint a, WindowPoint b, int *charOffset);
void func_02033bb4(MessageWindow *window, int a, int b, WindowPoint position);
void func_02033c88(MessageWindow *window, int plane, int objPlane, MessageFile *file, int message, int a,
                   MessageFile *buttonFile, int *charOffset);
void func_02033ddc(MessageWindow *window, int plane, int objPlane, MessageFile *file, int message, int a,
                   MessageFile *buttonFile, int *charOffset);

} // extern "C"

#endif // SCENE_MAINPROGRAM_H
