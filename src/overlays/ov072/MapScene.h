#ifndef OV072_MAPSCENE_H
#define OV072_MAPSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "G3Sprite.h"
#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "island/Apartment.h"
#include "island/IslandProgress.h"
#include "island/Sky.h"
#include "resident/ResidentLife.h"
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

// The main program's camera and lights, and its particle manager (MainProgram.h)
struct Camera;
struct Light;
struct ParticleManager;

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
    // [0]: of the music (sequence archive handle 0); [1]: only the volume, of handle 5, the surroundings. An array:
    // the scene's constructor constructs both in one loop.
    /* 0x04 */ SoundParamRamps mRamps[2];
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

    MapScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)

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
    /* 0x0582C */ Sky mSky;
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
    /* 0x1CAA8 */ IslandProgress mFacilities;
    /* 0x1CAD4 */ MapClock mClock;
    /* 0x1CAF8 */ MapClockDisplay mClockDisplay;
    /* 0x1D284 */ MoneyDisplay mMoneyDisplay;
    /* 0x1D4F4 */ int mMoney;
    /* 0x1D4F8 */ u32 mIdleFrames;
}; // 0x1D4FC

#endif // OV072_MAPSCENE_H
