#ifndef OV074_ROOMSCENE_H
#define OV074_ROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "RoomCharacter.h"

// The apartment scene (overlay 74): one room of the building with its resident and a visitor in it, and on the
// touch screen the building with a window for each room.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. `RoomScene` is after
// the scene's archive, "Scn/Scene/ScenePlayRoom2_LZ.bin", and its messages, "tmd_mainroom_msg_JP.bmg".

// ---------------------------------------------------------------------------------------------------------------
// The main program's scene base class (its constructor is inlined into each scene's; func_0204af20 runs a scene).
// Not decompiled: this is what the runner shows of it.
class Scene {
public:
    // What mPhase counts through
    enum Phase {
        PHASE_START_LOADING, // StartLoading() is called once
        PHASE_INIT,          // When the archives are loaded: Init(), Draw()
        PHASE_FADE_IN,       // Draw() while the screens fade in
        PHASE_RUN,           // Update(), Draw()
        PHASE_FADE_OUT,      // Draw() while the screens fade out (the scene manager sets this to leave the scene)
        PHASE_EXIT,          // When the archive loader is idle: Exit()
        PHASE_DONE,          // The scene manager deletes the scene
    };

    virtual void StartLoading() = 0;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Exit() = 0;
    virtual ~Scene() {}

    /* 0x04 */ int mSceneId;
    /* 0x08 */ int mPhase;
    /* 0x0C */ BOOL mSkipFade;
};

// ---------------------------------------------------------------------------------------------------------------
// STAND-INS for the scene's members whose classes belong to other groups of functions (a7's, mostly, and the main
// program's). Each has the size the scene's constructor (main program, func_0203ed10) shows and the fields that the
// scene's own functions touch. Replace them with the real classes.

// A point of the touch screen, as the touch input gives it and the hit tests take it (by value).
struct TouchPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

// One of the things a character holds (0x02169dcc constructs it; name is group a6's). It has virtual functions
// (table data_ov074_0216d7d0).
struct HeldItem {
    virtual void Virtual0();
    virtual void Virtual1();
    virtual void Virtual2();
    virtual void Virtual3();
    virtual void Draw(); // 0x02169ee0: the scene's Draw calls it

    /* 0x04 */ u8 mUnknown04[0xE4];
    /* 0xE8 */ int mTransition;      // 0x02169fc8 sets it; not 0 while it runs
    /* 0xEC */ u8 mUnknownEC[4];
    /* 0xF0 */ char mArchiveName[4]; // "HI0" to "HI3"
    /* 0xF4 */ char mArchivePath[0x20];
}; // 0x114

// The room's model (0x0216a278 constructs it; its base class is the main program's, func_020a54c4...).
struct RoomModel {
    /* 0x000 */ u8 mUnknown000[0x14];
    /* 0x014 */ u8 mDoor[0x188];     // A main-program object (func_020a4a50 draws it); a6 has it as the door
    /* 0x19C */ int mUnknownSetTo2ByInit;
    /* 0x1A0 */ u8 mUnknown1A0[0x768];
    /* 0x908 */ int mTransition;     // 0x0216a354 sets it; not 0 while it runs
    /* 0x90C */ u8 mUnknown90C[2];
    /* 0x90E */ u16 mRoomNumber;     // The room that is shown
    /* 0x910 */ u8 mUnknown910[4];
    /* 0x914 */ u8 mUnknown914[0xC]; // Given to 0x0216c92c with the resident's name
}; // 0x920

// The camera on the room (0x0216a4a4 constructs it).
struct RoomCamera {
    /* 0x00 */ u8 mUnknown00[4];
    /* 0x04 */ int mZoomState;       // 0x0216a62c sets it, with mZoomTimer
    /* 0x08 */ int mTargetMode;      // 0x0216a618 sets it: 1 the room, 2 one character, 3 both
    /* 0x0C */ u8 mUnknown0C[4];
    /* 0x10 */ VecFx32 mLookAt;
    /* 0x1C */ u8 mUnknown1C[0x24];
    /* 0x40 */ fx32 mTurnSpeedY;     // The scene sets these two from the + Control Pad
    /* 0x44 */ fx32 mTurnSpeedX;
    /* 0x48 */ u8 mUnknown48[4];
    /* 0x4C */ BOOL mLeaving;        // Set when the player goes into the room
    /* 0x50 */ RoomCharacter *mCharacters[2];
    /* 0x58 */ RoomModel *mRoom;
    /* 0x5C */ u8 mUnknown5C[6];
    /* 0x62 */ s16 mZoomTimer;
}; // 0x64

// Something that follows the camera (0x0216b544 constructs it, 0x0216b55c gives it the camera and the room,
// 0x0216b8a8 sets its state). The symbols have it as RoomLight.
struct CameraFollower {
    /* 0x00 */ u8 mUnknown00[0x1C];
}; // 0x1C

// A button of the building panel (0x0216bb20 is its hit test, 0x0216bb18 sets how it is shown).
struct PanelButton {
    /* 0x00 */ u8 mUnknown00[0xC0];
};

// The building on the touch screen, with a window for each room (0x0216bba4 constructs it).
struct BuildingPanel {
    /* 0x000 */ u8 mUnknown000[0xB8];
    /* 0x0B8 */ PanelButton mInfoButton;   // Leads to the information scene
    /* 0x178 */ void *mWindowScreen;       // What func_02027d7c returns for "APX:/BG_window.nsc"
    /* 0x17C */ char mArchivePath[0x1C];   // "Scn/AFwBG/Apart0_LZ.bin", for the building's number
    /* 0x198 */ u16 mSelectedRoom;         // The room the player chose
    /* 0x19A */ u8 mUnknown19A[2];
    /* 0x19C */ u8 mRoomResidents[100];    // For each room, the index of its resident, or 0xFF
}; // 0x200

// The view out of the room's window, which follows the clock (0x0216c374 constructs it).
struct WindowView {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ int mLoadState;             // 2 while it is loading
    /* 0x30 */ void *mSkyPalette;          // What func_02027534 returns for "ASP:/Fw_BG_sky.ncl"
    /* 0x34 */ void *mPalette;             // and for mPaletteName
    /* 0x38 */ u8 mUnknown38[4];
    /* 0x3C */ char mArchivePath[0x1C];    // "Scn/AFwBG/A0FwFBG0_LZ.bin"
    /* 0x58 */ char mCharName[0x10];       // "ASX:/Fw_BG0.ncg"
    /* 0x68 */ char mPaletteName[0x10];    // "ASX:/Fw_BG0.ncl"
}; // 0x78

// The main program's particle system for one effect file (func_02080e80 constructs it, func_02080e98 loads it).
struct ParticleSystem {
    /* 0x0 */ void *mManager; // func_0207b138 updates it
}; // 0x4

// The plate with the resident's name (0x0216c5e8 constructs it).
struct NamePlate {
    /* 0x000 */ u8 mUnknown000[0xC94];
}; // 0xC94

// A button on the touch screen (0x0216ca30 constructs it).
struct TouchButton {
    /* 0x00 */ u8 mUnknown00[0x78];
    /* 0x78 */ int mUnknownSetWithEnabled; // 11 when the button is disabled, -1 when it is enabled
    /* 0x7C */ u8 mUnknown7C[0x40];
    /* 0xBC */ BOOL mEnabled;              // 0x0216ca14 sets it
    /* 0xC0 */ s16 mAction;                // 0: go into the room, 1: back to the map
    /* 0xC2 */ u8 mUnknownC2[2];
    /* 0xC4 */ int mPressState;            // 1: pressed, 2: not pressed, 3: let go on the button
    /* 0xC8 */ MessageText *mLabel;
}; // 0xCC

// The time now (0x0216cc70 constructs it, 0x0216ccac and 0x0216ccd4 read the clock into it).
struct RoomClock {
    /* 0x00 */ s64 mSeconds; // RTC seconds, what the characters' events are timed by
    /* 0x08 */ RTCTime mTime;
    /* 0x14 */ RTCDate mDate;
}; // 0x24

// The clock shown on the touch screen (0x0216cd10 gives it the RoomClock).
struct ClockDisplay {
    /* 0x000 */ u8 mUnknown000[0x78C];
}; // 0x78C

// The text of what the characters say (0x0216d018 constructs it; 0x0216d1a8 sets its state).
class SpeechText : public MessageText {
public:
    SpeechText();

    // How many sprite characters (tiles) the canvas takes
    int GetCharCount() { return ((mCanvas.areaWidth * mCanvas.areaHeight - 1) >> mVramMode) + 1; }

    /* 0x588 */ int mState;
    /* 0x58C */ s16 mStep;
}; // 0x590 without the canvas, 0x5F8 with it

// What the main program's func_020adfbc... work on, which decide what comes of two characters meeting (name is
// group a1's). It has no data.
struct EncounterJudge {
    u8 mUnknown[4];
};

// ---------------------------------------------------------------------------------------------------------------

class RoomScene : public Scene {
public:
    // mInputState: what the touch screen is doing
    enum InputState {
        INPUT_OFF,
        INPUT_IDLE,          // Waiting for a touch
        INPUT_TOUCH_BUTTON,  // The stylus went down on one of the two buttons
        INPUT_INFO_BUTTON,   // or on the panel's information button
        INPUT_PANEL,         // or anywhere else: the building panel follows it
        INPUT_ENTER_ROOM,    // The characters turn to the player before the scene is left for the room
        INPUT_WAIT_LEAVE,    // Waiting for everything to finish, then the next scene is asked for
        INPUT_LEAVING,
    };

    // mEventState: what goes on in the room
    enum EventState {
        EVENT_NONE,
        EVENT_CHANGE_ROOM,      // The room is taken away and the chosen one is brought in
        EVENT_CHOOSE,           // ChooseEncounter() decides what comes next, by who is in the room
        EVENT_EMPTY_ROOM,       // The room has no resident
        EVENT_NOBODY_HOME,      // The room has a resident, who is not in
        EVENT_ALONE,            // One character in the room
        EVENT_TOGETHER,         // Two
        EVENT_APPROACH,         // The two walk up to each other
        EVENT_MEET,             // One of them speaks to the other
        EVENT_CHOOSE_WALKER,    // The one nearer the middle walks off, the other waits
        EVENT_ENCOUNTER,        // What comes of the meeting is judged and shown
        EVENT_ENCOUNTER_END,
        EVENT_RESIDENT_ARRIVES, // Nobody was in and the resident's event has it at home: it comes in (state 0x10C)
        EVENT_VISITOR_ARRIVES,  // The partner of the resident's event is loaded and comes in (state 0x10D)
        EVENT_LAST_LEAVES,      // The only character in the room goes out (state 0x106) until it is not present
        EVENT_VISITOR_LEAVES,   // One of two goes out (states 0x107 and 0x109) until one is left
        EVENT_AFTER_DISTRIBUTION, // Coming from the character distribution scene
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void GetEventItems(u8 *first, u8 *second);
    void ChooseCharacters(int resident);
    void UpdateObjects();
    void PlaceCharacters();
    void StartCharacterEvents();
    void AimCamera();
    void StartRoomEvent();
    void PressButton(int button);
    BOOL IsTouchEnabled();
    BOOL CanEnterRoom();
    void EnterRoom();
    void GoToInformation();
    void UpdateButtons();
    void SetInputState(int state);
    void UpdateInput();
    void ChangeRoom();
    void SetEventState(int state);
    void SetEventState(int state, int timer);
    SavedCharacter *StartDueEvent(s64 now, SavedCharacter *temp);
    int EndDueEvent();
    void UpdateAbsentCharacter(s64 now);
    int StartRandomEvent(s64 now);
    BOOL IsTransitionDone();
    BOOL StepTransition(int transition);
    void UpdateEvent();

    // --- Group a1's (0x0214e5f8 to 0x02152228), under the names its notes give them ---
    void ChooseEncounter();                                  // 0x0214e5f8
    void ChooseReactionsQuiet();                             // 0x0214ea60
    void ChooseReactions();                                  // 0x0214ee90
    void ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode); // 0x0214f4ec
    void ShowEventMessage();                                 // 0x0214f564
    void ApplyEncounterResult(RoomCharacter *first, RoomCharacter *second, int result, int kind); // 0x0214f67c
    int JudgeEncounterEnd(RoomCharacter *first, RoomCharacter *second); // 0x0214f788
    void StartEncounter(int nearer, int farther);            // 0x0214f7cc
    void UpdateEncounter();                                  // 0x0214f950
    void FinishEncounter();                                  // 0x0214f9d0
    void StartTalkReaction(int index, int reaction);         // 0x0214fbb0
    void StartEvent(RoomCharacter *character, int phase);    // 0x0214fd84
    void StartPairEvent(int phase);                          // 0x02150538
    int CountPresent();                                      // 0x02151420
    int GetCharacterCountForDisplay();                       // 0x02151454
    void UpdateMeeting();                                    // 0x021514f0
    BOOL AreCharactersNear(int first, int second, fx32 reach, fx32 maxReach); // 0x0215206c
    void SetFadeState(int state);                            // 0x02152124
    void SaveCharacter(SavedCharacter *character);           // 0x0215213c
    void SaveCharacters();                                   // 0x0215217c
    void LoadCharacter(SavedCharacter *character);           // 0x021521a0
    void LoadCharacters();                                   // 0x02152204

    /* 0x00010 */ int mInputState;
    /* 0x00014 */ int mEventState;
    /* 0x00018 */ int mFadeState;         // 1: the room is being shown, 2: the scene is being left
    /* 0x0001C */ int mEventTimer;        // Frames left of an event's step, or the step of a transition
    /* 0x00020 */ RoomCharacter mCharacters[2]; // [0] is the room's resident
    /* 0x05B50 */ RoomCharacter *mActor;      // The two characters of an encounter, the one that acts first
    /* 0x05B54 */ RoomCharacter *mActorOther; // (names are group a1's)
    /* 0x05B58 */ HeldItem mItems[4];     // Two for each character
    /* 0x05FA8 */ RoomModel mRoom;
    /* 0x068C8 */ RoomCamera mCamera;
    /* 0x0692C */ CameraFollower mCameraFollower;
    /* 0x06948 */ BuildingPanel mPanel;
    /* 0x06B48 */ WindowView mWindowView;
    /* 0x06BC0 */ ParticleSystem mParticles;
    /* 0x06BC4 */ HeadIcon mIcon[1];      // A mark shown between the two characters (the class of their head icons)
    /* 0x06C04 */ NamePlate mNamePlate;
    /* 0x07898 */ TouchButton mButtons[2];
    /* 0x07A30 */ MessageText mButtonLabel[1];
    /* 0x08020 */ RoomClock mClock;
    /* 0x08044 */ ClockDisplay mClockDisplay;
    /* 0x087D0 */ MessageFile mEtcMessages;  // "TEM:/tmd_etc_msg_JP.bmg"
    /* 0x087E8 */ MessageFile mRoomMessages; // "TMM:/tmd_mainroom_msg_JP.bmg"
    /* 0x08800 */ SpeechText mSpeechText;
    /* 0x08DF8 */ EncounterJudge mEncounterJudge;
    /* 0x08DFC */ TtsPlayer mTtsPlayer;
    /* 0x1D740 */ int mPressedButton;     // Which of mButtons the stylus went down on
    /* 0x1D744 */ int mUnknown1D744;
    /* 0x1D748 */ int mNextSceneId;
    /* 0x1D74C */ int mUnknownClearedByInit;
    /* 0x1D750 */ BOOL mEncounterPending; // Name is group a1's
    /* 0x1D754 */ s16 mResidentCount;     // How many of the 100 saved characters exist
    /* 0x1D756 */ s16 mScanIndex;         // The saved character whose event is looked at this frame (0 to 100)
    /* 0x1D758 */ s16 mInputStep;         // Step inside mInputState
    /* 0x1D75A */ s16 mEventStep;         // Step inside mEventState
    /* 0x1D75C */ s16 mFadeStep;          // Frames of mFadeState
    /* 0x1D75E */ u16 mIdleFrames;        // Frames since the player last did anything
    /* 0x1D760 */ u16 mEventFrames;       // Counted by UpdateEvent
    /* 0x1D762 */ u8 mUnknown1D762[2];
    /* 0x1D764 */ u8 mResidents[100];     // The indices of the saved characters that exist
}; // 0x1D7C8

#endif // OV074_ROOMSCENE_H
