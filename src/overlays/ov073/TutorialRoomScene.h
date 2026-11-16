#ifndef OV073_TUTORIALROOMSCENE_H
#define OV073_TUTORIALROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Scene.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

#include "MessageWindow.h"
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
// The scene's message window: the main program's MessageWindow, with nothing of its own (its table, 0x0211090C, is
// SceneManager.cpp's, stored by the scene's constructor after MessageWindow's)
class TutorialMessageWindow : public MessageWindow {};

// The apartment scene as the tutorial shows it (overlay 73, scene id 0x6B): one room with its resident, a visitor
// who comes in, and a message window that explains what is going on. It is built from the classes of the apartment
// scene (overlay 74) in the tutorial's cut, and runs one of four scripted parts, chosen by the step the tutorial has
// reached. The scene factory (main SceneManager::CreateScene, case 107) loads overlay 73, allocates 0x134E8 bytes and calls
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

    TutorialRoomScene(int sceneId); // 0x0203EFC0, the main program's (src/scene/SceneManager.cpp)

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
    /* 0x06350 */ RoomParticleSystem mParticles;
    /* 0x06354 */ RoomHeadIcon mIcon[1];          // The mark shown between the two characters
    /* 0x06394 */ TutorialMessageWindow mWindow[1];
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
