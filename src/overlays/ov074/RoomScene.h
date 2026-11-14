#ifndef OV074_ROOMSCENE_H
#define OV074_ROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

#include "text/MessageFile.h"
#include "text/MessageText.h"

// The overlay's shared headers: the character (with Character.h, FloorPos.h, HeldItem.h, Model.h, Room.h and
// RoomCamera.h), and the scene's other objects, which are the unit RoomObjects'
#include "RoomCharacter.h"
#include "ApartmentFront.h"
#include "Clock.h"
#include "RoomLight.h"
#include "SkyBackground.h"
#include "SpeechBalloon.h"
#include "TimedMessage.h"
#include "TouchButton.h"

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
// STAND-INS for main-program classes that are not decompiled and not in the overlay's shared headers.

// The main program's particle system for one effect file (func_02080e80 constructs it, func_02080e98 loads it).
struct ParticleSystem {
    /* 0x0 */ void *mManager; // func_0207b138 updates it
}; // 0x4

// What the main program's functions that decide what comes of two characters meeting work on (RoomEncounter's
// header has the others of them). It has no data.
struct EncounterJudge {
    int Judge(RoomCharacter *first, RoomCharacter *second);                // 0x020adfbc: 10 when nothing comes of it
    int JudgeEnd(RoomCharacter *first, RoomCharacter *second, BOOL param); // 0x020ae1f8
    void Apply(RoomCharacter *first, RoomCharacter *second, int relation, s64 time);            // 0x020ae75c
    void ApplyEnd(RoomCharacter *first, RoomCharacter *second, int relation, s64 time, int param); // 0x020ae80c

    u8 mUnknown[4];
};

// ---------------------------------------------------------------------------------------------------------------
// ADDITIONS that the shared headers need (they are read-only for this unit). Each is here only until it is there.

// Character.h, SavedCharacter: mEventEndLo and mEventEndHi are one s64 (the scene compares it whole).
inline s64 GetEventEnd(SavedCharacter *character) {
    return *(s64 *)&character->mEventEndLo;
}

// Room.h, RoomDoorModel: the first four bytes of mUnknown13C are an int that the scene's Init sets to 2.
inline void SetDoorUnknown13C(RoomDoorModel *door, int value) {
    *(int *)door->mUnknown13C = value;
}

// RoomObjects/TimedMessage.h (or MessageText): how many sprite characters (tiles) the text's canvas takes
inline int GetCanvasCharCount(MessageText *text) {
    return ((text->mCanvas.areaWidth * text->mCanvas.areaHeight - 1) >> text->mVramMode) + 1;
}

// RoomCamera.h, SavedCamera (data_0213ba74, what the scenes remember for each other): it goes on past the camera.
struct SceneMemory {
    /* 0x00 */ SavedCamera mCamera;
    /* 0x20 */ u8 mCharacter; // The saved character whose room was shown last
};
#define gSceneMemory (*(SceneMemory *)&data_0213ba74)

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
    void ChooseTalkReaction(int index);                      // 0x0214fad4
    void StartTalkReaction(int index, int reaction);         // 0x0214fbb0
    void StartEvent(RoomCharacter *character, int phase);    // 0x0214fd84
    void StartPairEvent(int phase);                          // 0x02150538
    int CountPresent();                                      // 0x02151420
    int GetCharacterCountForDisplay();                       // 0x02151454
    void UpdateMeeting();                                    // 0x021514f0
    void StartMeeting(int first, int second);                // 0x02151554
    void ApproachWaiting(int index, int other, int distanceDifference); // 0x02151bf4
    void AnswerWalker(int index, int walker);                // 0x02151d78
    BOOL AreCharactersNear(int first, int second, fx32 reach, fx32 maxReach); // 0x0215206c
    void SetFadeState(int state);                            // 0x02152124
    void SaveCharacter(RoomCharacter *character);           // 0x0215213c
    void SaveCharacters();                                   // 0x0215217c
    void LoadCharacter(RoomCharacter *character);           // 0x021521a0
    void LoadCharacters();                                   // 0x02152204

    /* 0x00010 */ int mInputState;
    /* 0x00014 */ int mEventState;
    /* 0x00018 */ int mFadeState;         // 1: the room is being shown, 2: the scene is being left
    /* 0x0001C */ int mEventTimer;        // Frames left of an event's step, or the step of a transition
    /* 0x00020 */ RoomCharacter mCharacters[2]; // [0] is the room's resident
    /* 0x05B50 */ RoomCharacter *mActor;      // The two characters of an encounter, the one that acts first
    /* 0x05B54 */ RoomCharacter *mActorOther; // (names are group a1's)
    /* 0x05B58 */ HeldItem mItems[4];     // Two for each character
    /* 0x05FA8 */ ApartmentRoom mRoom;
    /* 0x068C8 */ RoomCamera mCamera;
    /* 0x0692C */ RoomLight mLight;
    /* 0x06948 */ ApartmentFront mFront;
    /* 0x06B48 */ SkyBackground mSky;
    /* 0x06BC0 */ ParticleSystem mParticles;
    /* 0x06BC4 */ HeadIcon mIcon[1];      // A mark shown between the two characters (the class of their head icons)
    /* 0x06C04 */ NamePanel mNamePanel;
    /* 0x07898 */ TouchButton mButtons[2];
    /* 0x07A30 */ MessageText mButtonLabel[1];
    /* 0x08020 */ Clock mClock;
    /* 0x08044 */ ClockDisplay mClockDisplay;
    /* 0x087D0 */ MessageFile mEtcMessages;  // "TEM:/tmd_etc_msg_JP.bmg"
    /* 0x087E8 */ MessageFile mRoomMessages; // "TMM:/tmd_mainroom_msg_JP.bmg"
    /* 0x08800 */ TimedMessage mMessage;   // What the characters say and what goes on
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
