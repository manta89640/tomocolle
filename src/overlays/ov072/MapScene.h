#ifndef OV072_MAPSCENE_H
#define OV072_MAPSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "sound/SoundManager.h"
#include "sound/ValueRamp.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

#include <wchar.h>

// Overlay 72: the island map (the scene's archive is "Scn/Scene/SceneMap2_LZ.bin"). The island is shown in 3D
// with its buildings; the player touches one to bring the camera to it and open its window on the touch screen,
// and enters it with the window's buttons or a second touch. The scene also announces new buildings, plays the
// first visits of the tutorial, and has a title mode that shows the island from turning cameras.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The main program's
// classes below are STAND-INS (not decompiled): their sizes and the fields that this overlay's code touches, most
// of them as overlays 73, 74 and 96 have them.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BTA0 0x30415442 // Texture SRT animation
#define RESOURCE_BMA0 0x30414D42 // Material colour animation
#define RESOURCE_BTP0 0x30505442 // Texture pattern animation

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

class Scene;
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

// What the scenes leave for each other (data_0213ba74)
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x1E];
    /* 0x1E */ u8 mTutorialStep;
    /* 0x1F */ u8 mUnknown1F;
    /* 0x20 */ u8 mCharacters[12]; // The characters the next scene shows (0xFF: none); the news scene reads them
};

// The main program's camera (data_02122764)
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
extern "C" void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);
struct Light {
    // Points the light, relative to the camera
    void SetDirection(Camera *camera, VecFx32 direction) { func_0202aa28(this, camera, direction); }

    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ GXRgb mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

// The main program's time-of-day tracker, as overlay 73 has it
struct DayPhase {
    /* 0x00 */ u32 mChanged; // Not 0 in the frame the phase changed
    /* 0x04 */ u8 mUnknown04[0x10];
    /* 0x14 */ int mFramesToCheck;
    /* 0x18 */ int mPhase;
    /* 0x1C */ int mUnknown1C;
    /* 0x20 */ u8 mUnknown20[8];
}; // 0x28

// The sky of the upper screen (main program, func_020a679c to func_020a6a6c): a background and its sprites, which
// change with the time of day.
struct MapSky : public DayPhase {
    /* 0x028 */ u8 mUnknown028[0x1C];
    /* 0x044 */ const GXRgb *mColors; // Of the time of day: [1] is the 3D clear colour
    /* 0x048 */ u8 mUnknown048[0xEB0];
}; // 0xEF8

// One of the eleven news items of the saved game (func_0202c8a8 copies one out, func_0202c8bc in): the news scene
// (overlay 87, scene 0x46) shows them.
struct NewsItem {
    /* 0x00 */ u8 mKind;      // An entry of data_020fa1d0; 0x1D is a birthday
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

// What a news kind is (data_020fa1d0, 0xAC of them, 0x20 bytes each)
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

    /* 0x4 */ u16 mId;
}; // 0x8

class InventoryItem : public Item {
public:
    InventoryItem();       // func_0209ee00
    BOOL Add(int count);

    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
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

// What the island has: the shops' stock, the size of the apartment building (main program func_020af224 to
// func_020af9dc). A stand-in of its size.
struct IslandFacilities {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ int mApartmentSize; // Picks the building's model and sizes, and is at least 4 for the fountain's events
    /* 0x10 */ u8 mUnknown10[0x1C];
}; // 0x2C

// The state of each building and feature of the island, as a byte from 0x60 on (func_02039c00 reads one,
// func_02039c0c writes one): 0 not built, 1 built, 2 new, 3 not open yet
struct IslandState {
    /* 0x00 */ u8 mUnknown00[0x60];
    /* 0x60 */ u8 mStates[1];
};

// --- The main program's models (0x02029b04 to 0x0202b008), as overlay 74 has them ---------------------------

extern "C" {
void func_020267f4(void *block); // Frees a block of the main heap
}

class Model {
public:
    virtual ~Model() {
        if (mResource != NULL) {
            func_020267f4(mResource);
        }
    }
    virtual void Load(int resourceId, int unk); // func_02029b04
    virtual void UnknownVirtual3();             // func_02029bd4
    virtual void Draw();                        // func_02029df4

    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }
    void SetPosition(fx32 x, fx32 y, fx32 z) {
        mPosition.x = x;
        mPosition.y = y;
        mPosition.z = z;
    }

    /* 0x04 */ int mUnknown04;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ NNSG3dResMdl *mResMdl;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ NNSG3dRenderObj mRenderObj;
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ VecFx32 mScale;
    /* 0x98 */ VecFx32 mPosition;
    /* 0xA4 */ u16 mRotX;
    /* 0xA6 */ u16 mRotY;
    /* 0xA8 */ u16 mRotZ;
    /* 0xAC */ void *mResource;
    /* 0xB0 */ BOOL mVisible;
}; // 0xB4

class AnimModel : public Model {
public:
    AnimModel();             // func_0203e020
    virtual ~AnimModel();    // func_0202af98
    virtual void Draw();     // func_0202a0e4

    void SetAnimation(u32 resourceType, int index); // func_0202a2e0

    /* 0xB4 */ void *mAnims[8];
    /* 0xD4 */ int mAnimCount;
    /* 0xD8 */ fx32 mUnknownD8;
    /* 0xDC */ int mUnknownDC;
}; // 0xE0

// --- The main program's 2D classes ----------------------------------------------------------------------------

// A textured square drawn by the 3D engine (main program 0x0202b0e8 to 0x0202b6d0; as src/scene/G3SpriteTestScene.h
// has it): the scene shows the apartment building's windows with them, each with one of seven textures
// ("w_live.tga" ... "w_game.tga").
class G3Sprite;
extern "C" {
void func_0202b1f0(G3Sprite *quad, int texture, ScreenRect *rect);
}
class G3Sprite {
public:
    virtual ~G3Sprite(); // func_0202b6cc, which does nothing

    void SetPosition(VecFx32 position) {
        mX = position.x;
        mY = position.y;
        mDepth = -position.z / 16;
    }

    /* 0x04 */ int mTexture;
    /* 0x08 */ BOOL mVisible;
    /* 0x0C */ int mDepth;
    /* 0x10 */ u8 mUnknown10[0xC];
    /* 0x1C */ fx32 mScaleX;
    /* 0x20 */ fx32 mScaleY;
    /* 0x24 */ int mX;
    /* 0x28 */ int mY;
    /* 0x2C */ u8 mUnknown2C[0x10];
    /* 0x3C */ GXRgb mColor;
    /* 0x3E */ u8 mUnknown3E[2];
    /* 0x40 */ GXRgb mUnknown40;
    /* 0x42 */ GXRgb mUnknown42;
    /* 0x44 */ int mUnknown44;
    /* 0x48 */ u8 mUnknown48[4];
}; // 0x4C

// The main program's message window (virtual table data_021102e8, functions func_02032fb8 to func_02034f94): a
// window on a background that slides in and out, with sprites, buttons and up to eight message texts. As overlay
// 73 has it.
struct WindowPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

struct MessageWindow;
extern "C" void func_02034fc8(MessageWindow *window, int text, int x, int y);
struct MessageWindow {
    // Where a text of the window is
    void SetTextPosition(int text, WindowPoint position) { func_02034fc8(this, text, position.x, position.y); }

    /* 0x0000 */ void *mVtable;
    /* 0x0004 */ u8 mSprites[12][0xF4];
    /* 0x0B74 */ u8 mButtons[16][0x6F0];
    /* 0x7A74 */ MessageText mTexts[8];
    /* 0xA9F4 */ u8 mUnknownA9F4[0x58C];
    /* 0xAF80 */ int mResult;               // func_02034720 sets it to -1
    /* 0xAF84 */ int mUnknownAF84;
    /* 0xAF88 */ int mUnknownAF88;
    /* 0xAF8C */ BOOL mTransitionDone;      // Set when the window has finished moving
    /* 0xAF90 */ int mUnknownAF90;
    /* 0xAF94 */ int mUnknownSetBeforeClose;
    /* 0xAF98 */ int mUnknownAF98;
    /* 0xAF9C */ int mUnknownAF9C;
    /* 0xAFA0 */ int mUnknownAFA0;
}; // 0xAFA4

extern "C" {
// --- Main-program functions, by their address names ---
extern ScreenControl data_0211d604;
extern PadInput data_0211f9d4;
extern TouchInput gTouchInput;
extern MATHRandContext16 gRandom;
extern SceneManager gSceneManager;
extern SceneMemory data_0213ba74;
// 100 bytes (cleared with MI_CpuFill8 by overlays 68 and 75) that scenes leave for the next one, read as different
// types by each. The map leaves the sky's clear colour and light colours in it as three GXRgb.
extern u8 data_0213bab0[100];
extern IslandState data_0212348c;
extern Camera data_02122764;
extern Light data_021227cc;
extern Light data_021227d8;
extern int data_02123440[];
extern wchar_t data_02123510[]; // The player's name
extern const NewsKind data_020fa1d0[];

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;
struct G3SpriteTextures;
struct ParticleManager;
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
extern u8 data_021223e4[];
extern G3SpriteTextures data_021232e4;
extern u8 data_02142438[];

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);

BOOL func_02024004(ScreenControl *screens, int planes, int level);
BOOL func_02024118(ScreenControl *screens, int planes, int level);
void func_0202680c(int size);
void func_02026824();
void func_0202683c();
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);

int func_0202a1f8(AnimModel *model, u32 resType, int slot);
void func_0202a640(Camera *camera);
void func_0202a96c(Light *light);
void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
void func_0202ad84(void *resources);
void func_0202ae38(void *resources);
void func_0202aecc(void *resources, int resourceId);
void func_0202b0e8(Renderer3d *renderer);
void func_0202b4a4(G3Sprite *quad);
void func_0202b4c4(G3Sprite *quad);
void func_0202b4d8(G3SpriteTextures *textures);
void func_0202b4dc(G3SpriteTextures *textures, int index, const char *path, int a, int width, int height);

BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
void func_0202c264(GameData *game, int index, SavedCharacter *character);
void func_0202c28c(GameData *game, int index, SavedCharacter *character);
u8 func_0202c468(GameData *game, int character);
u8 func_0202c53c(GameData *game, int character);
int func_0202c5b4(GameData *game, int character);
int func_0202c58c(GameData *game, int character);
void func_0202c8a8(GameData *game, int index, NewsItem *item);
void func_0202c8bc(GameData *game, int index, NewsItem *item);
BOOL func_0202cb08(GameData *game);
BOOL func_0202cd54(GameData *game);

// --- The sprite buttons (the rest are in SpriteButton.h) ---
// Whether the stylus is on the button (the scene ignores touches elsewhere while one is)
BOOL func_02031294(SpriteButton *button);

// --- The message window ---
void func_02032fb8(MessageWindow *window, ScreenRect rect);
void func_02033174(MessageWindow *window, const char *screenName, int plane);
void func_020337b8(MessageWindow *window, int a, int b, int c, int d, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset,
                   const int *e);
void func_02033a58(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, int *charOffset);
void func_020341d4(MessageWindow *window);
void func_020345d8(MessageWindow *window);
void func_02034720(MessageWindow *window, int flags, int unknown);
void func_020349cc(MessageWindow *window, int flags);
void func_02034a3c(MessageWindow *window, int text, int argIndex, const wchar_t *string);
void func_02034f38(MessageWindow *window, int text, u32 message);
void func_02034f94(MessageWindow *window, u8 vAlign);

// --- The clock ---
s64 func_02035870(int offset, RTCDate *date, RTCTime *time);
void Clock_GetDate(int offset, RTCDate *date);
void Clock_GetTime(int offset, RTCTime *time);
void func_02036874(int value, wchar_t *buffer, int size, int a, int b);

u8 func_02039c00(IslandState *island, int building);
void func_02039c0c(IslandState *island, int building, int state);
void func_0203c02c(SceneManager *manager, int sceneId);
void NewsItem_Make(NewsItem *item, int kind); // Makes a news item of a kind (data_020fa1d0), with its characters
void func_02060c1c();

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
int func_02098dac(SavedCharacter *character, int which);
BOOL func_02099c94(int event);
void func_0209c44c(SavedCharacter *character, int change);

struct Unknown02114d78Entry {
    u32 mUnknown0 : 8;
    u32 mUnknown0Bits8 : 24;
    int mUnknown4;
    int mUnknown8;
};
extern const Unknown02114d78Entry *const data_02114d78[];

// --- The island ---
int func_020a6074(u16 size);
int func_020a60a0();
int func_020a60b8(int stage);
struct WindowCell {
    /* 0x0 */ int mColumn;
    /* 0x4 */ int mFloor;
};
WindowCell func_020a60cc(int window);
int func_020a6214(DayPhase *phase);
u32 func_020a6390(int month, int a);
u32 func_020a6480(int month, int a);
GXRgb func_020a6574(int phase);
GXRgb func_020a659c(int phase);
void func_020a679c(MapSky *sky, const char *archiveName, int a, int b, int c, int d, int e, int f, int g, int h);
void func_020a6930(MapSky *sky);
void func_020a6a6c(MapSky *sky);

void func_020ad294(s64 now, RTCTime *time);
void func_020ad514(s64 now, RTCTime *time);
void func_020ad568(s64 now);
void func_020af224(IslandFacilities *island, int a, int b);
int func_020af4d8(IslandFacilities *island, BOOL a);
BOOL func_020af988(IslandFacilities *island);
BOOL func_020af9ac(IslandFacilities *island);
BOOL func_020af9c0(IslandFacilities *island);
BOOL func_020af9dc(IslandFacilities *island);
}

#define gSceneMemory data_0213ba74
#define gIsland data_0212348c

// --- This overlay's classes -----------------------------------------------------------------------------------

class MapClock;

// A point in pixels
struct Point {
    int x;
    int y;
};

// The 3D camera: it flies to a point and a direction over 40 frames, slowing down at the end.
class MapCamera {
public:
    enum { MOVE_FRAMES = 40 };

    void Reset();
    void Update();
    void MoveTo(VecFx32 position);
    void MoveTo(VecFx32 position, VecFx32 offset, int frame);
    BOOL IsAtHome();

    /* 0x00 */ Camera *mCamera;
    /* 0x04 */ int mFrame;
    /* 0x08 */ VecFx32 mGoal;       // Where the camera goes
    /* 0x14 */ BOOL mMoving;
    /* 0x18 */ VecFx32 mOffset;     // From the camera to what it looks at
    /* 0x24 */ VecFx32 mGoalOffset;
}; // 0x30

// The island's sounds: the music of the time of day and the sounds of the season's surroundings.
class MapSound {
public:
    enum Mode {
        MODE_START,    // The music of the time of day, and the surroundings
        MODE_RESTART,  // The surroundings only
        MODE_CONTINUE, // The same, without loading them again
        MODE_UNKNOWN3,
        MODE_TUTORIAL, // The music starts from silence
    };

    void Init(MapClock *clock, int mode);
    void SetMode(MapClock *clock, int mode);
    void Update(MapClock *clock);
    void SetBuilding(int model);
    void Stop();
    void StartSurroundings(int kind, int season, int timeOfDay);
    void StartSurroundingSound(u8 index);
    void MarkLongSound(u8 index);
    BOOL IsLongSound(int index);
    BOOL IsCommonSound(int index);
    int GetSeason(int month);
    int GetTimeOfDay(RTCTime *time);
    int GetDayOrNight(RTCTime *time);

    /* 0x00 */ int mMode;
    /* 0x04 */ SoundParamRamps mRamps;            // Of the music (sequence archive handle 0)
    /* 0x44 */ SoundParamRamps mSurroundingRamps; // Only the volume, of handle 5
    /* 0x84 */ int mSurroundingState;
    /* 0x88 */ u32 mDayStart;                     // Of the month, in minutes
    /* 0x8C */ u32 mNightStart;
    /* 0x90 */ int mSeason;
    /* 0x94 */ int mTimeOfDay;
    /* 0x98 */ BOOL mLongSound;                   // A long sound of the surroundings plays
    /* 0x9C */ int mFrames;
    /* 0xA0 */ int mDayOrNight;
}; // 0xA4

// The title mode's camera: ten views of the island, each a camera that moves and turns for a time.
class TitleCamera {
public:
    void Start(Camera *camera);
    void Update();
    void SetView(int view);

    /* 0x0 */ int mView;
    /* 0x4 */ int mFrame;
    /* 0x8 */ Camera *mCamera;
}; // 0xC

// The clock, as overlay 74's
class MapClock {
public:
    MapClock();
    void Update();
    void UpdateIfArchiveIdle();

    /* 0x00 */ s64 mSeconds;
    /* 0x08 */ RTCTime mTime;
    /* 0x14 */ RTCDate mDate;
}; // 0x24

class MapClockDisplay {
public:
    enum State {
        STATE_SHOWN,
        STATE_HIDE,
        STATE_HIDDEN,
    };
    enum { SPRITE_COUNT = 11 };

    void Init(MapClock *clock);
    void Update();
    void Refresh(BOOL all);
    void Draw();
    void SetState(int state);
    void SetVisible(BOOL visible);

    /* 0x000 */ MapClock *mClock;
    /* 0x004 */ int mState;
    /* 0x008 */ Sprite mSprites[SPRITE_COUNT];
    /* 0x76C */ u32 mUnknown76C;
    /* 0x770 */ u32 mMonth;
    /* 0x774 */ u32 mDay;
    /* 0x778 */ u32 mUnknown778;
    /* 0x77C */ u32 mHour;
    /* 0x780 */ u32 mMinute;
    /* 0x784 */ u32 mUnknown784;
    /* 0x788 */ s16 mUnknown788;
    /* 0x78A */ s16 mHiddenFrames;
}; // 0x78C

// The player's money on the lower screen: it counts up to the amount, with a sound.
class MoneyDisplay {
public:
    enum State {
        STATE_NONE,
        STATE_START,
        STATE_COUNT,
        STATE_DONE,
    };

    void Init(int *money, int *charOffset);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Refresh();
    void SetTextPosition(Point position) { mText.SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT); }

    /* 0x000 */ int mState;
    /* 0x004 */ int *mMoney;
    /* 0x008 */ int mShown;
    /* 0x00C */ Sprite mSprite;
    /* 0x0B8 */ ObjTextCanvas mText;
}; // 0x270

class MapScene : public Scene {
public:
    // mState
    enum State {
        STATE_TITLE,        // The title mode: views of the island until a touch
        STATE_LEAVE_TITLE,
        STATE_TUTORIAL,     // The tutorial's messages
        STATE_NEW_BUILDING, // A building has been built
        STATE_MAP,          // The player picks a building
    };
    // mWindowState: of the window of a building on the touch screen
    enum WindowState {
        WINDOW_OPENING,
        WINDOW_OPEN,
        WINDOW_CLOSED,
    };
    enum {
        MODEL_COUNT = 13, // The ground and twelve buildings
        MODEL_APARTMENT = 8,
        RESIDENT_COUNT = 100,
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
    virtual ~MapScene();

    void SetState(int state);
    VecFx32 GetWindowPosition(int window);
    void SelectBuilding(int model);
    void OpenWindow();
    void Deselect(BOOL sound, int unknown);
    void SetTutorialStep(int step);
    BOOL IsNewsDue();
    void MakeDailyNews();
    void AddNews(int kind);
    int GetWindowTexture(int resident);
    void UpdateWindows();
    void StartEffects(const VecFx32 *position);
    void StartEffect2(const VecFx32 *position);
    // Starts the effect of an emitter slot at a place, stopping what the slot had
    void StartEmitter(int slot, const VecFx32 *position);
    // Puts the 3D camera where the island is seen whole
    void ResetCamera();
    void ShowBuilding(int model);
    void EnterBuilding(int model, BOOL newsDue, int button);
    BOOL IsNewDay();
    void UpdateNewMarks();

    /* 0x00010 */ int mIslandStage;               // 0..4: how large the island is
    /* 0x00014 */ AnimModel mModels[MODEL_COUNT];
    /* 0x00B74 */ MapCamera mCamera;
    /* 0x00BA4 */ Light *mLights[2];
    /* 0x00BAC */ int mWindowCount;
    /* 0x00BB0 */ G3Sprite mWindows[150];     // Of the apartment building, one per room
    /* 0x03838 */ int mWindowState;
    /* 0x0383C */ int mWindowFrames;
    /* 0x03840 */ BOOL mHasInfo;                  // The window of the selected building has its own picture
    /* 0x03844 */ int mUnknown3844;
    /* 0x03848 */ int mSelected;                  // The model of the selected building, or -1
    /* 0x0384C */ int mUnknown384C;
    /* 0x03850 */ int mUnknown3850;
    /* 0x03854 */ Sprite mNewMarks[MODEL_COUNT];  // Over the buildings that are new
    /* 0x04110 */ Sprite mTitleSprite;
    /* 0x041BC */ Sprite mSprite41BC;
    /* 0x04268 */ SpriteButton mBackButtons[1];
    /* 0x0435C */ TextButton mButtons[3];
    /* 0x0582C */ MapSky mSky;
    /* 0x06724 */ MessageFile mMessageFiles[2];   // "TCO:tmd_common_msg_JP.bmg", "TET:tmd_etc_msg_JP.bmg"
    /* 0x06754 */ int mResidentRooms[RESIDENT_COUNT]; // The room of each saved character, or -1
    /* 0x068E4 */ int mRoomResidents[RESIDENT_COUNT]; // The saved character in each room, or -1
    /* 0x06A74 */ MessageWindow mMessageWindows[2];
    /* 0x1C9BC */ int mLastBuilding;
    /* 0x1C9C0 */ int mUnknown1C9C0;
    /* 0x1C9C4 */ BOOL mNewsDue;
    /* 0x1C9C8 */ int mUnknown1C9C8;
    /* 0x1C9CC */ int mTutorialStep;
    /* 0x1C9D0 */ int mTutorialFrames;
    /* 0x1C9D4 */ int mState;
    /* 0x1C9D8 */ int mFrames;
    /* 0x1C9DC */ int mNewBuilding;               // The building or feature that has just been built, or -1
    /* 0x1C9E0 */ MapSound mSound;
    /* 0x1CA84 */ int mUnknown1CA84;
    /* 0x1CA88 */ ParticleManager *mParticles;
    /* 0x1CA8C */ void *mEmitters[3];
    /* 0x1CA98 */ void *mParticleResource;
    /* 0x1CA9C */ TitleCamera mTitleCamera;
    /* 0x1CAA8 */ IslandFacilities mFacilities;
    /* 0x1CAD4 */ MapClock mClock;
    /* 0x1CAF8 */ MapClockDisplay mClockDisplay;
    /* 0x1D284 */ MoneyDisplay mMoneyDisplay;
    /* 0x1D4F4 */ int mMoney;
    /* 0x1D4F8 */ u32 mIdleFrames;
}; // 0x1D4FC

#endif // OV072_MAPSCENE_H
