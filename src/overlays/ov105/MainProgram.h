#ifndef OV105_MAINPROGRAM_H
#define OV105_MAINPROGRAM_H

#include "NintendoLogoScene.h"
#include "SceneManager.h"
#include "ScreenControl.h"

// What overlay 105's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's class, in NintendoLogoScene.h, is also included by the main program's
// scene constructors, src/scene/SceneManager.cpp).

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// Graphics resources, loaded by file name into VRAM.
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);

// The screens (ScreenControl.h). For each screen in the mask (0x3F the main engine's planes, 0x3F00 the sub
// engine's), sets how much the brightness changes each frame while the screen fades (+0x0C and +0x10, which
// func_02023cd4 and func_02023ddc add to the brightness at +0x04 and +0x08 until it reaches 0 or one end)
void func_02023c54(ScreenControl *screens, int mask, fx32 speed);

extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_020278f8(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}

#endif // OV105_MAINPROGRAM_H
