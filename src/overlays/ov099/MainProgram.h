#ifndef OV099_MAINPROGRAM_H
#define OV099_MAINPROGRAM_H

#include "InformationScene.h"
#include "SceneManager.h"
#include "text/TextUtil.h"

// What overlay 99's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's classes, in InformationScene.h, are also included by the main program's
// scene constructors, src/scene/SceneManager.cpp).

// --- Other main-program things --------------------------------------------------------------------------------

// One saved character, as far as this overlay reads one: a STAND-IN of the main program's (overlay 74's
// SavedCharacter has more).
struct SavedCharacter {
    SavedCharacter();
    int GetParameter(int param); // A parameter of the Mii data, or of the character (CharacterProperty)
    s8 GetPersonality();         // Its personality, 0 to 15

    /* 0x000 */ u8 mUnknown000[0x50];
    /* 0x050 */ u32 mUnknown050Bits0 : 17;
    /*       */ u32 mBirthYear : 12; // BIRTH_YEAR_UNKNOWN when not given; the month and day are GetParameter's
    /*       */ u32 mUnknown050Bits29 : 3;
    /* 0x054 */ u8 mUnknown054[0xA7];
    /* 0x0FB */ u8 mUnknown0FB;      // Summed over the residents for the stars of how comfortable they are
    /* 0x0FC */ u8 mUnknown0FC[0x1C4];
}; // 0x2C0

#define BIRTH_YEAR_UNKNOWN 0xFFF

// What SavedCharacter::GetParameter returns of a character, besides the Mii data's parameters
enum CharacterProperty {
    PROPERTY_BIRTH_MONTH = 0x26,
    PROPERTY_BIRTH_DAY = 0x27,
};

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

extern "C" {
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern ScreenControl gScreenControl;
extern void *data_02123440; // The game's clock
// The mansion's name (what the information's title shows)
extern wchar_t data_02123510[];

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void Clock_GetDate(void *clock, Date *date); // The date with the game's offset applied

BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
BOOL func_0202c604(GameData *game, int index); // The gender bit of its Mii (set for a woman)
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one

}


#endif // OV099_MAINPROGRAM_H
