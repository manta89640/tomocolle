#ifndef OV106_MAINPROGRAM_H
#define OV106_MAINPROGRAM_H

#include "GameData.h"
#include "SceneManager.h"
#include "PreStartScene.h"
#include "sound/SoundManager.h"

// What overlay 106's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's classes, in PreStartScene.h, are also included by the main program's
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

extern ScreenControl data_0211d604;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_020278f8(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}

// The game's clock (data_02123440 points at it): the real-time clock with the game's own offset.
struct GameClock {
    /* 0x00 */ u8 mUnknown00[0x34];
    // Not 0 when the console is another than the one the game was last played on: PreStartScene then warns that
    // the time and the shops may come out wrong (etc message 0x1A2)
    /* 0x34 */ BOOL mUnknown34;
};


extern "C" {

// --- The saved game ---
BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
void func_0202c8f8(GameData *game, int unknown);
BOOL func_0202cd48(GameData *game); // Starts writing the save data; FALSE when it cannot
BOOL func_0202cd38(GameData *game); // Not 0 while the save data is being written

// --- The clock ---
extern GameClock *data_02123440;
s64 func_02035870(GameClock *clock, RTCDate *date, RTCTime *time); // The current time, with the game's offset
s64 func_02035ae0(GameClock *clock);
int func_02035aa4(GameClock *clock);

// --- What the residents did while the game was off ---
void func_020ad7e8(s64 time);
void func_020ad568(s64 time);
void func_020ad514(s64 time, RTCTime *timeOfDay);
void func_020ad294(s64 time, RTCTime *timeOfDay);
void func_020ae9f8(EncounterJudge *judge, int count, u8 *residents);
void func_020aea3c(EncounterJudge *judge, int count, u8 *residents, s64 time);
void func_020aecb0(EncounterJudge *judge, int count, u8 *residents);
void func_020aeef4(EncounterJudge *judge);
void func_020aef80(EncounterJudge *judge, int count, u8 *residents);
}

#endif // OV106_MAINPROGRAM_H
