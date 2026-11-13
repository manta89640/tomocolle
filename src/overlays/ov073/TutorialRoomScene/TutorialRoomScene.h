#ifndef OV073_TUTORIALROOMSCENE_H
#define OV073_TUTORIALROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

#include "ApartmentFront.h"
#include "Clock.h"
#include "Room.h"
#include "RoomCamera.h"
#include "RoomCharacter.h"
#include "RoomLight.h"
#include "SkyBackground.h"
#include "SpeechBalloon.h"
#include "TimedMessage.h"
#include "TouchButton.h"

// The apartment scene as the tutorial shows it (overlay 73): one room with its resident, a visitor who comes in,
// and a message window that explains what is going on. It is built from the classes of the apartment scene
// (overlay 74) and runs one of four scripted parts, chosen by the step the tutorial has reached.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. `TutorialRoomScene`
// is after the scene's archive, "Scn/Scene/ScenePlayRoomTut_LZ.bin".

// ---------------------------------------------------------------------------------------------------------------
// The main program's scene base class (its constructor is inlined into each scene's; func_0204af20 runs a scene).
// Not decompiled: this is what the runner shows of it (the same as in overlay 74's RoomScene.h).
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
}; // 0x10

// ---------------------------------------------------------------------------------------------------------------
// STAND-INS for main-program classes that are not decompiled: their sizes as the scene's constructor (main
// program, func_0203efc0) shows them, and the fields the scene's functions touch.

// The main program's particle system for one effect file (func_02080e80 constructs it, func_02080e98 loads it).
struct ParticleSystem {
    /* 0x0 */ void *mManager; // func_0207b138 updates it
}; // 0x4

// A size or a position in pixels, as the message window's functions take it (by value, in two registers).
struct WindowPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

// The main program's message window (virtual table data_02110914, functions func_02032fb8 to func_02034f94): a
// window on a background that slides in and out, with sprites, buttons and up to eight message texts.
struct MessageWindow {
    // The flags of Open and Close (func_02034720, func_020349cc), as far as the scene uses them
    enum {
        FLAG_OPEN = 0x02,
        FLAG_CLOSE = 0x04,
        FLAG_FROM_BELOW = 0x10, // func_02034720 starts the window 0xC0 pixels down for it
    };

    /* 0x0000 */ void *mVtable;
    /* 0x0004 */ u8 mSprites[12][0xF4];     // Constructed by func_0203117c
    /* 0x0B74 */ u8 mButtons[16][0x6F0];    // Constructed by func_02031f1c
    /* 0x7A74 */ MessageText mTexts[8];
    /* 0xA9F4 */ u8 mUnknownA9F4[0x58C];
    /* 0xAF80 */ int mResult;               // func_02034720 sets it to -1; the scene waits for it to change
    /* 0xAF84 */ int mUnknownAF84;
    /* 0xAF88 */ int mUnknownAF88;
    /* 0xAF8C */ BOOL mTransitionDone;      // func_02034720 clears it; set when the window has finished moving
    /* 0xAF90 */ int mUnknownAF90;
    /* 0xAF94 */ int mUnknownSetBeforeClose; // The scene sets it (5 or 15) before func_020349cc; the update reads it
    /* 0xAF98 */ u8 mUnknownAF98[0xC];
}; // 0xAFA4

// ---------------------------------------------------------------------------------------------------------------

class TutorialRoomScene : public Scene {
public:
    // mState: the parts of the tutorial that the scene plays. Which one it starts with depends on mTutorialStep.
    enum State {
        STATE_NONE,
        STATE_RESIDENT_ARRIVES, // Step 8: the room is empty, then the resident comes in; ends with a touch
        STATE_VISITOR_ARRIVES,  // Step 13: a second character comes in and the two walk up to each other
        STATE_TALK,             // They talk until each has full affinity for the other
        STATE_BECOME_FRIENDS,   // The mark between them and the message that they are friends
        STATE_EXPLAIN_FRIENDS,  // Three pages of the message window
        STATE_EXPLAIN,          // Step 16: the resident alone and one page of the window; ends with a touch
        STATE_WAIT_TOUCH,       // Step 21: the resident alone; ends with a touch
        STATE_ENDING,           // After the touch: the resident runs its states 16 to 18 (0x0214bdc0, group b1)
        STATE_LEAVE,            // Ask for the next scene
        STATE_LEAVING,
    };

    // mFadeState
    enum FadeState {
        FADE_NONE,
        FADE_SHOW, // The scene's first frames: the objects are updated from Draw until the scene runs
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    BOOL ChooseCharacters();
    void UseDefaultCharacters();
    void StartTutorialStep();
    void UpdateState();
    void UpdateObjects();
    void UpdateFade();
    void SetState(int state);
    void SetFadeState(int state);

    // --- Group b1's (0x0214bbe0 to 0x0214c1ac), under the names its notes give them ---
    BOOL UpdateTouch();                                      // 0x0214bbe0
    void UpdateEnding();                                     // 0x0214bdc0
    void ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode); // 0x0214bec4
    BOOL AreCharactersNear(int first, int second);           // 0x0214bf48
    void UpdateIdle();                                       // 0x0214c004
    void SaveCharacter(SavedCharacter *character);           // 0x0214c0c8
    void SaveCharacters();                                   // 0x0214c104
    void LoadCharacter(SavedCharacter *character);           // 0x0214c128
    void LoadCharacters();                                   // 0x0214c188

    /* 0x00010 */ int mState;
    /* 0x00014 */ int mFadeState;
    /* 0x00018 */ RoomCharacter mCharacters[2]; // [0] is the room's resident, [1] the visitor
    /* 0x05740 */ ApartmentRoom mRoom;
    /* 0x06060 */ RoomCamera mCamera;
    /* 0x060BC */ RoomLight mLight;
    /* 0x060D8 */ ApartmentFront mFront;
    /* 0x062D8 */ SkyBackground mSky;
    /* 0x06350 */ ParticleSystem mParticles;
    /* 0x06354 */ HeadIcon mIcon[1];          // The mark shown between the two characters
    /* 0x06394 */ MessageWindow mWindow[1];
    /* 0x11338 */ NamePanel mNamePanel;
    /* 0x11FCC */ TouchButton mButtons[1];
    /* 0x12094 */ MessageText mButtonLabel[1];
    /* 0x12684 */ Clock mClock;
    /* 0x126A8 */ ClockDisplay mClockDisplay;
    /* 0x12E34 */ TimedMessage mTimedMessage; // What the characters say
    /* 0x1342C */ MessageFile mEtcMessages;    // "TEM:/tmd_etc_msg_JP.bmg"
    /* 0x13444 */ MessageFile mCommonMessages; // "TCM:/tmd_common_msg_JP.bmg"
    /* 0x1345C */ MessageFile mRoomMessages;   // "TMM:/tmd_mainroom_msg_JP.bmg"
    /* 0x13474 */ s16 mStep;                  // Step inside mState
    /* 0x13476 */ s16 mTimer;                 // Frames of the step
    /* 0x13478 */ s16 mFadeStep;              // Frames of mFadeState
    /* 0x1347A */ s16 mTouchStep;             // Group b1: the step of the touch input
    /* 0x1347C */ s16 mPressedButton;         // Group b1: index of the button being pressed; Init clears it
    /* 0x1347E */ s16 mResidentCount;         // How many of the 100 saved characters exist
    /* 0x13480 */ u16 mTutorialStep;          // How far the tutorial is, as the scene before left it
    /* 0x13482 */ u8 mResidents[100];         // The indices of the saved characters that exist
}; // 0x134E8

#endif // OV073_TUTORIALROOMSCENE_H
