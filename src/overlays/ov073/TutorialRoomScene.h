#ifndef OV073_TUTORIALROOMSCENE_H
#define OV073_TUTORIALROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Scene.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

#include "MainProgram.h"
#include "Room.h"
#include "TutorialApartmentFront.h"
#include "TutorialClock.h"
#include "TutorialNamePanel.h"
#include "TutorialRoomCamera.h"
#include "TutorialRoomCharacter.h"
#include "TutorialRoomLight.h"
#include "TutorialSkyBackground.h"
#include "TutorialTimedMessage.h"
#include "TutorialTouchButton.h"

// A size or a position in pixels, as the message window's functions take it (by value, in two registers).
struct WindowPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

// The main program's message window (virtual table data_02110914, functions func_02032fb8 to func_02034f94): a
// window on a background that slides in and out, with sprites, buttons and up to eight message texts. A stand-in
// with its size, as the scene's constructor shows it, and the fields the scene touches.
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

extern "C" {
// --- The message window ---
void func_02032fb8(MessageWindow *window, ScreenRect rect);                     // Sets it up, with its rectangle
void func_02033174(MessageWindow *window, const char *screenName, int plane);   // The screen file of its background
void func_020331bc(MessageWindow *window, int planes, ScreenRect rect);
// Adds a button with a text: (.., position, size, the text's message file and message, font, .., character offset)
void func_020336b4(MessageWindow *window, int a, int b, int c, u8 d, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset, int e, int f,
                   int g, int h);
// Adds a message text: (.., message file, message, font, plane, position, character offset)
void func_02033a58(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, int *charOffset);
void func_020341d4(MessageWindow *window);                                      // Updates
void func_020345d8(MessageWindow *window);                                      // Draws
void func_02034720(MessageWindow *window, int flags, int unknown);              // Opens it
void func_020349cc(MessageWindow *window, int flags);                           // Closes it
void func_02034a3c(MessageWindow *window, int text, int argIndex, const wchar_t *string); // SetStringArg and DrawAll of a text
void func_02034f38(MessageWindow *window, int text, u32 message);               // Sets the message of a text
void func_02034f4c(MessageWindow *window, u8 hAlign);                           // Of every text
void func_02034f94(MessageWindow *window, u8 vAlign);
}

// The apartment scene as the tutorial shows it (overlay 73, scene id 0x6B): one room with its resident, a visitor
// who comes in, and a message window that explains what is going on. It is built from the classes of the apartment
// scene (overlay 74) in the tutorial's cut, and runs one of four scripted parts, chosen by the step the tutorial has
// reached. The scene factory (main func_0203c050, case 107) loads overlay 73, allocates 0x134E8 bytes and calls
// the constructor, which is in the main program (func_0203efc0), like overlay 74's.
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// archive, "Scn/Scene/ScenePlayRoomTut_LZ.bin".
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
        STATE_ENDING,           // After the touch: the resident runs its states 0x10 to 0x12 (UpdateEnding)
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
    virtual ~TutorialRoomScene();

    BOOL ChooseCharacters();
    void UseDefaultCharacters();
    void StartTutorialStep();
    void UpdateState();
    void UpdateObjects();
    void UpdateFade();
    void SetState(int state);
    void SetFadeState(int state);
    BOOL UpdateTouch();
    void UpdateEnding();
    void ShowMessage(TutorialRoomCharacter *first, TutorialRoomCharacter *second, int message, int drawMode);
    BOOL AreCharactersNear(int first, int second);
    void UpdateIdle();
    void SaveCharacter(SavedCharacter *character);
    void SaveCharacters();
    void LoadCharacter(SavedCharacter *character);
    void LoadCharacters();

    /* 0x00010 */ int mState;
    /* 0x00014 */ int mFadeState;
    /* 0x00018 */ TutorialRoomCharacter mCharacters[2]; // [0] is the room's resident, [1] the visitor
    /* 0x05740 */ TutorialApartmentRoom mRoom;
    /* 0x06060 */ TutorialRoomCamera mCamera;
    /* 0x060BC */ TutorialRoomLight mLight;
    /* 0x060D8 */ TutorialApartmentFront mFront;
    /* 0x062D8 */ TutorialSkyBackground mSky;
    /* 0x06350 */ ParticleSystem mParticles;
    /* 0x06354 */ HeadIcon mIcon[1];          // The mark shown between the two characters
    /* 0x06394 */ MessageWindow mWindow[1];
    /* 0x11338 */ TutorialNamePanel mNamePanel;
    /* 0x11FCC */ TutorialTouchButton mButtons[1];
    /* 0x12094 */ MessageText mButtonLabel[1];
    /* 0x12684 */ TutorialClock mClock;
    /* 0x126A8 */ TutorialClockDisplay mClockDisplay;
    /* 0x12E34 */ TutorialTimedMessage mTimedMessage; // What the characters say
    /* 0x1342C */ MessageFile mEtcMessages;    // "TEM:/tmd_etc_msg_JP.bmg"
    /* 0x13444 */ MessageFile mCommonMessages; // "TCM:/tmd_common_msg_JP.bmg"
    /* 0x1345C */ MessageFile mRoomMessages;   // "TMM:/tmd_mainroom_msg_JP.bmg"
    /* 0x13474 */ s16 mStep;                  // Step inside mState
    /* 0x13476 */ s16 mTimer;                 // Frames of the step
    /* 0x13478 */ s16 mFadeStep;              // Frames of mFadeState
    /* 0x1347A */ s16 mTouchStep;             // 0: waiting for a touch, 1: the stylus is down on the button
    /* 0x1347C */ s16 mPressedButton;         // Which of mButtons the stylus went down on; Init clears it
    /* 0x1347E */ s16 mResidentCount;         // How many of the 100 saved characters exist
    /* 0x13480 */ u16 mTutorialStep;          // How far the tutorial is, as the scene before left it
    /* 0x13482 */ u8 mResidents[100];         // The indices of the saved characters that exist
}; // 0x134E8

#endif // OV073_TUTORIALROOMSCENE_H
