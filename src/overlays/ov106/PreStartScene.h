#ifndef OV106_PRESTARTSCENE_H
#define OV106_PRESTARTSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "resident/ResidentLife.h"

#include "MessageWindow.h"
#include "Scene.h"
#include "Sprite.h"
#include "text/MessageFile.h"

// --- The main program's classes that the scene holds -----------------------------------------------------------

// EncounterJudge: resident/ResidentLife.h

// --- The scene's objects ----------------------------------------------------------------------------------------

// The date and time when the scene starts. The constructor is the overlay's, and the scene's constructor in the main
// program calls it.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class PreStartClock {
public:
    PreStartClock();
    void Update();

    /* 0x00 */ s64 mSeconds; // The same moment as seconds, as Clock_GetDateTime returns it
    /* 0x08 */ s64 mUnknown08; // What func_02035ae0 returns
    /* 0x10 */ RTCDate mDate;
    /* 0x20 */ RTCTime mTime;
    /* 0x2C */ int mUnknown2C; // What func_02035aa4 returns; while it is 0 the residents' update calls func_020ae9f8
}; // 0x30

// The sprite on the upper screen that is shown while the save data is written (in the middle of the screen,
// sequence 0 of "MNU:/Obj_up.nce").
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavingIcon : public Sprite {
public:
    void Init();
    void Draw();
}; // 0xAC

// --- The scene ----------------------------------------------------------------------------------------------------

// Overlay 106, scene 0x39: what comes before the game starts, after the save data has been loaded. It looks at the
// outcome of loading (gGameData.mUnknownState) and says what has to be said in a message window on the lower screen:
// that the data could not be read (and nothing after it), that it was broken and is initialised (and writes it),
// or that this is another console than last time (yes goes on, no says to turn the power off). Then, if the game had
// been started before (gGameData.mUnknownB1), it brings the residents up to date with the clock and asks for scene
// 0x6A; otherwise it asks for scene 0x69. The scene factory (main CreateScene, case 57) loads overlay 106 and
// allocates 0xB13C bytes; the constructor is the main program's (func_02042174).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// archive, "Scn/Scene/ScenePreStart_LZ.bin".
class PreStartScene : public Scene {
public:
    // mState
    enum State {
        STATE_START,        // Bring the residents up to date if the game was started before, and leave
        STATE_OTHER_DS,     // "This is another console than last time: play on?" No: "turn the power off"
        STATE_INITIALISE,   // "The data is broken and will be initialised", then write it
        STATE_READ_FAILED,  // "The data could not be read: turn the power off and put the card back in"
        STATE_LEAVE,        // Ask for mNextSceneId
        STATE_STOPPED,      // Nothing more: the player has to turn the console off
    };

    PreStartScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    void UpdateWindow();
    void UpdateResidents();
    BOOL ShowMessage(u32 message);
    BOOL ShowOk(u32 message);
    BOOL Ask(u32 message, int flags);

    /* 0x0010 */ int mLoadResult; // gGameData.mUnknownState when the scene started
    /* 0x0014 */ int mState;
    /* 0x0018 */ int mNextSceneId;
    /* 0x001C */ PreStartClock mClock;
    /* 0x004C */ SavingIcon mSavingIcon;
    /* 0x00F8 */ EncounterJudge mJudge;
    /* 0x00FC */ MessageWindow mWindow[1];
    /* 0xB0A0 */ MessageFile mEtcMessages;
    /* 0xB0B8 */ MessageFile mCommonMessages;
    /* 0xB0D0 */ s16 mAnswer;  // The button that answered the last question
    /* 0xB0D2 */ s8 mSubStep;  // Of ShowMessage, ShowOk and Ask
    /* 0xB0D3 */ s8 mStep;
    /* 0xB0D4 */ s16 mResidentCount;
    /* 0xB0D6 */ u8 mResidents[100]; // The indices of the saved characters that exist
}; // 0xB13C

#endif // OV106_PRESTARTSCENE_H
