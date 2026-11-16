#ifndef SCENE_MAINPROGRAM_H
#define SCENE_MAINPROGRAM_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Scene.h"
#include "ScreenControl.h"
#include "Sprite.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// STAND-INS and declarations for what the main program's scenes in src/scene/ use of the rest of the main program,
// which is not decompiled yet: its global objects with the fields these scenes touch, and its functions by the
// names symbols.txt has for them. Most are the same as the overlays' (src/overlays/ov073/MainProgram.h,
// src/overlays/ov096/WeddingScene.h). The names are descriptive, not the original.

// A size or a position in pixels, as the message window's functions take it (by value, in two registers).
struct WindowPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

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

// A sprite that can be touched, as a button (0xF4 bytes; func_0203117c constructs it, func_0203133c updates it,
// func_02031d7c draws it). Only the size is known here: the scenes go through its functions.
struct SpriteButton : public Sprite {
    /* 0xAC */ u8 mUnknownAC[0x48];
}; // 0xF4

// The main program's message window (virtual table data_021102e8, functions func_02032fb8 to func_02034f94): a
// window on a background that slides in and out, with sprites, buttons and texts. Overlay 73's stand-in, with the
// fields these scenes touch.
struct MessageWindow {
    /* 0x0000 */ void *mVtable;
    /* 0x0004 */ u8 mUnknown0004[0xAF80];
    /* 0xAF84 */ int mResult;           // -1, or the button that was chosen
    /* 0xAF88 */ int mUnknownAF88;
    /* 0xAF8C */ BOOL mTransitionDone;  // Set when the window has finished moving
    /* 0xAF90 */ int mUnknownAF90;
    /* 0xAF94 */ int mUnknownAF94;      // The scenes set it (5 or 15) before func_020349cc
    /* 0xAF98 */ int mUnknownAF98;      // The scenes keep there what to do when the window has closed
    /* 0xAF9C */ u8 mUnknownAF9C[8];
}; // 0xAFA4

extern "C" {

// --- Archives ---
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);

// --- Graphics resources ---
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void func_020278f8(PaletteLoader *loader, u16 color, int unknown);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);

// --- 3D ---
extern Camera data_02122764;
void func_0202a640(Camera *camera);
void func_0202a93c(Camera *camera, int unknown);
extern MATHRandContext16 gRandom;

// --- The button ---
void func_0203119c(SpriteButton *button);
void func_02031260(SpriteButton *button, int mode);
void func_02031278(SpriteButton *button, u16 sequence, u16 pressedSequence);
void func_0203133c(SpriteButton *button); // Updates it
BOOL func_02031ccc(SpriteButton *button); // Whether it was pressed
void func_02031d7c(SpriteButton *button); // Draws it
void func_02031eb0(SpriteButton *button, int width, int height); // The rectangle that can be touched

// --- The message window ---
void func_02032fb8(MessageWindow *window, ScreenRect rect);
void func_02033174(MessageWindow *window, const char *screenName, int plane);
void func_020331bc(MessageWindow *window, int planes, ScreenRect rect);
void func_02033390(MessageWindow *window, int index, int a, u8 b, u8 c, WindowPoint position, WindowPoint size);
void func_0203373c(MessageWindow *window, int index, int a, int b, int c, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset);
void func_020338d0(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint a, WindowPoint b, int *charOffset);
void func_02033a58(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, int *charOffset);
void func_02033bb4(MessageWindow *window, int a, int b, WindowPoint position);
void func_02033c88(MessageWindow *window, int plane, int objPlane, MessageFile *file, int message, int a,
                   MessageFile *buttonFile, int *charOffset);
void func_02033ddc(MessageWindow *window, int plane, int objPlane, MessageFile *file, int message, int a,
                   MessageFile *buttonFile, int *charOffset);
void func_020341d4(MessageWindow *window);                         // Updates
void func_020345d8(MessageWindow *window);                         // Draws
void func_020346cc(MessageWindow *window);
void func_02034720(MessageWindow *window, int flags, int unknown); // Opens it
void func_020349cc(MessageWindow *window, int flags);              // Closes it
void func_02034a3c(MessageWindow *window, int text, int argIndex, const wchar_t *string);
void func_02034f38(MessageWindow *window, int text, u32 message);
void func_02034f4c(MessageWindow *window, u8 hAlign);

} // extern "C"

#endif // SCENE_MAINPROGRAM_H
