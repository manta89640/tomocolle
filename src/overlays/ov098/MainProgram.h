#ifndef OV098_MAINPROGRAM_H
#define OV098_MAINPROGRAM_H

#include "OptionScene.h"
#include "SceneManager.h"
#include "text/TextUtil.h"

// What overlay 98's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's classes, in OptionScene.h, are also included by the main program's
// scene constructors, src/scene/SceneManager.cpp).

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;

struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0xD0];
    /* 0xD4 */ BOOL mPlanesChanged;
};

// The touch screen.
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x30];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;        // Which of the two points the scene tests
};

extern "C" {
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern ScreenControl gScreenControl;
extern TouchInput gTouchInput;
extern void *data_02123440; // The game's clock

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);

// The date and the time as the clock gives them (RTCDate and RTCTime: the year is 0 to 99)
void Clock_GetDate(void *clock, RTCDate *date);
void Clock_GetTime(void *clock, RTCTime *time);
// Sets the clock's offset so that it shows that date and time
void func_02035460(void *clock, RTCDate *date, RTCTime *time, BOOL unknown);
void func_02035668(void *clock, RTCDate *date, RTCTime *time); // func_02035460 with TRUE

BOOL func_0202cd38(GameData *game); // Whether the save data is still being written
void func_0202c8f8(GameData *game, int unknown);
BOOL func_0202cd48(GameData *game);
BOOL func_0202cd94(GameData *game);

}


#endif // OV098_MAINPROGRAM_H
