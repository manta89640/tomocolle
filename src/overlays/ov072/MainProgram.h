#ifndef OV072_MAINPROGRAM_H
#define OV072_MAINPROGRAM_H

#include "MapScene.h"
#include "text/TextUtil.h"
#include "SceneManager.h"

// What overlay 72's code uses of the main program, which is not decompiled: STAND-INS and declarations, which only
// the overlay's source includes (the scene's classes, in MapScene.h, are also included by the main program's scene
// constructors, src/scene/SceneManager.cpp).

// --- The main program's globals --------------------------------------------------------------------------------

// The screens: which planes are shown, and two values the scenes set (each with a flag that it changed)
struct ScreenControl {
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }
    void Set(u32 planes) {
        mPlanes = planes;
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

// The buttons
struct PadInput {
    /* 0x00 */ u32 mTrigger; // Bit 3 (START) takes the player to scene 0x2C
};

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x30];
    /* 0x30 */ ScreenPoint mLastPoint;
    /* 0x38 */ ScreenPoint mPoint;
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched; // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C; // Picks mLastPoint over mPoint
};

// What the scenes leave for each other (gSceneMemory)
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x1E];
    /* 0x1E */ u8 mTutorialStep;
    /* 0x1F */ u8 mUnknown1F;
    /* 0x20 */ u8 mCharacters[12]; // The characters the next scene shows (0xFF: none); the news scene reads them
};

// The main program's camera (gCamera)
struct Camera {
    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }
    void SetTarget(VecFx32 target) {
        mTarget.x = target.x;
        mTarget.y = target.y;
        mTarget.z = target.z;
    }

    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ u8 mUnknown10[4];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights
struct Light;
extern "C" void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
struct Light {
    // Points the light, relative to the camera
    void SetDirection(Camera *camera, VecFx32 direction) { Light_SetDirectionByCamera(this, camera, direction); }

    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ GXRgb mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC



// One of the eleven news items of the saved game (GameData_GetNewsItem copies one out, GameData_SetNewsItem in): the news scene
// (overlay 87, scene 0x46) shows them.
struct NewsItem {
    /* 0x00 */ u8 mKind;      // An entry of gNewsKinds; 0x1D is a birthday
    /* 0x01 */ u8 mUnknown01[2];
    /* 0x03 */ u8 mCharacter; // Of a birthday: whose
    /* 0x04 */ u8 mUnknown04[8];
    /* 0x0C */ u32 mUnknown0CBit0 : 1;
    /*      */ u32 mUnknown0CBit1 : 1;
    /*      */ u32 mUnknown0CBits2 : 5;
    /*      */ u32 mMonth : 4;
    /*      */ u32 mDay : 5;
    /*      */ u32 mHour : 5;
    /*      */ u32 mUnknown0CBits21 : 11;
    u8 GetMonth() { return mMonth; }
    u8 GetDay() { return mDay; }
    u8 GetHour() { return mHour; }
    /* 0x10 */ u16 mYear;     // Since 2000
    /* 0x12 */ u8 mUnknown12[2];
}; // 0x14

// What a news kind is (gNewsKinds, 0xAC of them, 0x20 bytes each)
struct NewsKind {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ u32 mUnknown04Bits0 : 16;
    /*      */ u32 mMonths : 12; // Bit n: the kind may be picked in month n + 1
    /*      */ u32 mUnknown04Bits28 : 4;
    /* 0x08 */ u8 mUnknown08[0x18];
}; // 0x20

// The saved game's character records, as the scenes read them (0x2C0 bytes, constructor func_020979c0)
struct SavedCharacter {
    SavedCharacter();
    int GetParameter(int param);            // As include/character/Character.h has them
    static BOOL IsAwayState(int state);

    /* 0x000 */ u8 mUnknown000[0x4F];
    /* 0x04F */ u8 mRoom;  // Its room's number in the apartment building
    /* 0x050 */ u8 mUnknown050[4];
    /* 0x054 */ u32 mUnknown054;
    /* 0x058 */ u8 mUnknown058[0x1CC];
    /* 0x224 */ u8 mEvent; // What the character is doing in its room
    /* 0x225 */ u8 mUnknown225[0x9B];
}; // 0x2C0

// An item: the base of an item in the inventory
class Item {
public:
    virtual void UnknownVirtual0();

    BOOL Add(int count);

    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
}; // 0x14

class InventoryItem : public Item {
public:
    InventoryItem();
}; // 0x14

// The player's items, loaded from and saved to the saved game together
class Inventory {
public:
    void Load();
    void Save();

    /* 0x0000 */ int mCount;
    /* 0x0004 */ InventoryItem mItems[255];
    /* 0x13F0 */ InventoryItem *mList[255];
}; // 0x17EC


// The save data's header (gap38-a's SaveHeader, include/save/SaveBlocks.h), as far as the scene calls it: a
// declaration-only stand-in, which the calls reach by a cast
class SaveHeader {
public:
    u8 GetUnknown44(int index);
    void SetUnknown44(int index, u8 value);
};

// The state of each building and feature of the island, as a byte from 0x60 on (SaveHeader::GetUnknown44 reads one,
// SetUnknown44 writes one): 0 not built, 1 built, 2 new, 3 not open yet
struct IslandState {
    /* 0x00 */ u8 mUnknown00[0x60];
    /* 0x60 */ u8 mStates[1];
};


// --- The main program's 2D classes ----------------------------------------------------------------------------



extern "C" {
// --- Main-program functions, by their address names ---
extern ScreenControl gScreenControl;
extern PadInput gPadInput;
extern TouchInput gTouchInput;
extern MATHRandContext16 gRandom;
extern SceneMemory gSceneMemory;
// 100 bytes (cleared with MI_CpuFill8 by overlays 68 and 75) that scenes leave for the next one, read as different
// types by each. The map leaves the sky's clear colour and light colours in it as three GXRgb.
extern u8 gSceneScratch[100];
extern IslandState gIslandState;
extern Camera gCamera;
extern Light gLights;
extern Light gLight1;
extern int gGameClock[];
extern wchar_t data_02123510[]; // The player's name
extern const NewsKind gNewsKinds[];

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);

BOOL ScreenControl_BlendFadeIn(ScreenControl *screens, int planes, int level);
BOOL ScreenControl_BlendFadeOut(ScreenControl *screens, int planes, int level);
void CreateMainFrameHeap(int size);
void AllocFromMainFrameHeap();
void DestroyMainFrameHeap();
int ObjResourceManager_LoadCell(CellLoader *loader, const char *cellName, int plane, int a);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);

int func_0202a1f8(AnimModel *model, u32 resType, int slot);
void Camera_Reset(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
void ModelResources_BeginDraw(void *resources);
void ModelResources_Reset(void *resources);
void VramManagers_Reset(Renderer3d *renderer);

BOOL GameData_CharacterExists(GameData *game, int index); // Whether the saved character with that index exists
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character);
u8 GameData_GetCharacterEvent(GameData *game, int character);
u8 GameData_GetCharacterTroubleState(GameData *game, int character);
int GameData_GetCharacterTroubleCategory(GameData *game, int character);
int GameData_GetCharacterTroubleId(GameData *game, int character);
void GameData_GetNewsItem(GameData *game, int index, NewsItem *item);
void GameData_SetNewsItem(GameData *game, int index, NewsItem *item);
BOOL func_0202cb08(GameData *game);
BOOL func_0202cd54(GameData *game);

// --- The sprite buttons (the rest are in SpriteButton.h) ---
// Whether the stylus is on the button (the scene ignores touches elsewhere while one is)
BOOL SpriteButton_IsTouched(SpriteButton *button);

// --- The message window ---

// --- The clock ---
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time);
void Clock_GetDate(int offset, RTCDate *date);
void Clock_GetTime(int offset, RTCTime *time);

void NewsItem_Make(NewsItem *item, int kind); // Makes a news item of a kind (gNewsKinds), with its characters
void NNS_GfdDumpLnkPlttVramManager();

// --- Particles ---
ParticleManager *func_0207ab20(void (*alloc)(), int a, int b, int c, int d, int e);
void func_0207ac78(ParticleManager *manager, void *resource);
void func_0207b110(ParticleManager *manager);
void func_0207b124(ParticleManager *manager);
void func_0207b138(ParticleManager *manager);
void func_0207b258(ParticleManager *manager, void *camera);
void *func_0207b2fc(ParticleManager *manager, int resource, const VecFx32 *position);
void func_0207b364(ParticleManager *manager, void *emitter);

void MiiData_GetName(SavedCharacter *character, wchar_t *name);
void func_0209c44c(SavedCharacter *character, int change);

struct Unknown02114d78Entry {
    u32 mUnknown0 : 8;
    u32 mUnknown0Bits8 : 24;
    int mUnknown4;
    int mUnknown8;
};
extern const Unknown02114d78Entry *const data_02114d78[];

// --- The island ---

}

#define gIsland gIslandState


#endif // OV072_MAINPROGRAM_H
