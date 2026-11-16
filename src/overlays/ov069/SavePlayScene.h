#ifndef OV069_SAVEPLAYSCENE_H
#define OV069_SAVEPLAYSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "character/Character.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "text/MessageFile.h"

// The main program's camera and lights, which the scene's camera and lights point at (MainProgram.h)
struct Camera;
struct Light;

// --- The scene's objects ----------------------------------------------------------------------------------------

// The scene's character: a resident picked at random, playing one of the animations of "Mdl/Ica/ica_mii_save_LZ.bin"
// in front of the camera. A class of its own with nothing added: its constructor and destructor are the main
// program's copies after the scene's constructor (0x02041054, 0x02041074), as each scene's character class has them.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavePlayCharacter : public Character {
public:
    SavePlayCharacter();
    ~SavePlayCharacter();
}; // 0x2B18

// The scene's camera: it only sets the main program's camera up.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavePlayCamera {
public:
    SavePlayCamera();
    void Init();
    void Update();

    /* 0x0 */ Camera *mCamera;
}; // 0x4

// The scene's two lights, which take the camera's angles.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavePlayLights {
public:
    void Init(SavePlayCamera *camera);
    void Update();

    /* 0x0 */ Light *mLights[2];
    /* 0x8 */ SavePlayCamera *mCamera;
}; // 0xC

// --- The scene ----------------------------------------------------------------------------------------------------

// Overlay 69, scenes 0x2C and 0x2D: saving. A resident stands on the upper screen and a message window on the lower
// one asks whether to save what has happened so far (etc message 0x17C) and then whether to go on (0x17E): yes goes
// back to the scene the player came from, no writes the data once more and says to turn the power off (0x17F,
// 0x180). Scene 0x2D comes before wireless play: it says the game is saved first (0x181), and goes on to scene 0x29
// once it is (0x182), or to scene 0x5E if the player says no. A failed write asks for scene 0xC2 (overlay 107). The
// scene factory (main CreateScene, cases 44 and 45) loads overlay 69 and allocates 0x6DB84 bytes; the constructor
// is the main program's (func_02040f38).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// archive, "Scn/Scene/SceneSavePlay_LZ.bin".
class SavePlayScene : public Scene {
public:
    SavePlayScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)
    // mState
    enum State {
        STATE_NONE,
        STATE_ASK_SAVE,       // "Save what has happened so far?" Yes saves, no leaves
        STATE_SAVING,         // "Saving..."
        STATE_SAVE,
        STATE_ASK_CONTINUE,   // "Saved. Go on playing?"
        STATE_CONTINUE,       // "Save again when you stop"
        STATE_SAVE_CONTINUE,
        STATE_SAVE_QUIT,
        STATE_QUIT,           // "Turn the power off now. Come and play again"
        STATE_LEAVE,          // Ask for mNextSceneId
        STATE_FADE_OUT,       // The music fades out for 124 frames
        STATE_STOPPED,
        // Before wireless play (scene 0x2D)
        STATE_ASK_SAVE_WIRELESS, // "Wireless play first saves what has happened so far. All right?"
        STATE_SAVING_WIRELESS,
        STATE_SAVE_WIRELESS,
        STATE_SAVED_WIRELESS,    // "Saved. Now for wireless play"
        STATE_LEAVE_WIRELESS,
        STATE_17,
    };

    // What Save does
    enum SaveKind {
        SAVE_0, // func_0202c8f8(0), then write
        SAVE_1, // func_0202c8f8(1), then write
        SAVE_2, // func_0202cd54
        SAVE_3, // func_0202cd74
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    void UpdateSaveStates();
    void UpdateWirelessStates();
    BOOL Save(int kind);
    BOOL ShowMessage(u32 message);
    BOOL ShowOk(u32 message);
    BOOL Ask(u32 message, int flags);
    void UpdateObjects();

    /* 0x00010 */ SavePlayCharacter mCharacters[1];
    /* 0x02B28 */ SavePlayCamera mCamera;
    /* 0x02B2C */ SavePlayLights mLights;
    /* 0x02B38 */ MessageWindow mWindow[1];
    /* 0x0DADC */ MessageFile mEtcMessages;
    /* 0x0DAF4 */ MessageFile mCommonMessages;
    /* 0x0DB0C */ int mState;
    /* 0x0DB10 */ int mNextSceneId;
    /* 0x0DB14 */ u8 mUnknownDB14[0x60000]; // Nothing in the overlay or the constructor touches it
    /* 0x6DB14 */ s16 mStep;
    /* 0x6DB16 */ u16 mAnimationCount; // Of the character's animations: one of them is picked at random
    /* 0x6DB18 */ s16 mResidentCount;
    /* 0x6DB1A */ s16 mFrames;         // That Save has waited
    /* 0x6DB1C */ s16 mAnswer;         // The button that answered the last question
    /* 0x6DB1E */ s8 mSubStep;         // Of Save, ShowMessage, ShowOk and Ask
    /* 0x6DB1F */ s8 mUnknown6DB1F;
    /* 0x6DB20 */ u8 mResidents[100];  // The indices of the saved characters that exist
}; // 0x6DB84

#endif // OV069_SAVEPLAYSCENE_H
