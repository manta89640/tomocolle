#ifndef OV103_DIVORCESCENE_H
#define OV103_DIVORCESCENE_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "G3Sprite.h"
#include "HeadIcon.h"
#include "Model.h"
#include "island/Sky.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 103, scene 0xB3: a couple separates. Two characters who are lovers or married (the pair that scene 0x40
// left in the scene memory, or two residents at random) stand in front of the camera, before the model
// "divorce.imd" ("Mdl/Misc/divo_3D_LZ.bin"); a script of commands (DivorceScript) moves them, turns their heads,
// changes their faces and makes them speak. When it ends, lovers become ex-lovers and spouses divorced, the second
// character gets a trouble of category 4 for it, and the scene goes back to scene 0x40, or to scene 0.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The scene factory
// (CreateScene) loads overlay 103 and allocates 0x1BBE8 bytes; the constructor is the main program's (0x0204909C,
// src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214DDA4).

class DivorceCharacter;
class DivorceCamera;
class DivorceLights;
class Room;
struct Camera;
struct Light;

// What the scene gives its characters.
struct DivorceContext {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ MessageFile *mCommonMessages;
    /* 0x08 */ MessageFile *mTroubleMessages;
    /* 0x0C */ MessageFile *mEtcMessages;
    /* 0x10 */ SpeechWindow *mWindow;
    /* 0x14 */ TtsPlayer *mTtsPlayer;
    /* 0x18 */ MessageText *mSpeechText;
    /* 0x1C */ DivorceCamera *mCamera;
    /* 0x20 */ DivorceLights *mLights;
    /* 0x24 */ DivorceCharacter *mCharacters[2];
}; // 0x2C

// A character of the scene: the main program's Character with what the script sets it doing.
class DivorceCharacter : public Character {
public:
    enum State {
        STATE_STAND,        // Stands, with the shadow shown
        STATE_LIFTED,       // Animation 0x21, a little above the floor, without the shadow
        STATE_WALK,         // Walks to mWalkTarget
        STATE_TURN,         // Turns to mTurnTarget, walking
        STATE_SAY_261,      // Says message 0x261 of the trouble messages
        STATE_SAY_263,      // And 0x263
        STATE_POSE_16,      // Animation 0x16
        STATE_ACT_18,       // Animation 0x18 once, with a sound halfway
        STATE_ACT_19,       // Animation 0x19 once, with another sound halfway
        STATE_ACT_1C,       // Animation 0x1C, with a sound at each loop, and head icon 0xD after a while
        STATE_SAY_25B,      // Animation 0x19 once while it says message 0x25B
        STATE_SAY_25D,      // The same with 0x25D
        STATE_SAY_25F,      // Stands and says 0x25F
        STATE_HIDE_ICON,    // The head icon's mode 1
    };

    // What one argument of a message is
    enum ArgType {
        ARG_NONE,
        ARG_CHARACTER, // The name of the saved character with that index
    };

    void Init(DivorceContext *context);
    void LoadHeadIcon(ParticleSystem *particles);
    void Update();
    void UpdateState();
    void SetState(int state);
    void Speak(int file, u32 message, int count, int *types, int *values, int mode);
    void Speak(int file, u32 message, int type, int value, int mode);
    void Speak(int file, u32 message, int mode);
    void TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit);
    void ResetHead();
    void TurnHeadToCamera();
    void TurnHeadToPartner();
    void LookAt(VecFx32 *target, int pitchLimit, int yawLimit);
    void LookAhead();
    void LookAtCamera();
    void LookAtPartner();
    u16 GetHeadPitchTo(VecFx32 *target);
    u16 GetHeadYawTo(VecFx32 *target);
    void UpdateHeadIcon();

    ModelAnimation *GetAnimation() { return AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId); }
    VecFx32 GetPos() { return mPos; }
    void SetWalkTargetFloorPos(FloorPos target) {
        mTarget.x = target.x;
        mTarget.z = target.z;
    }

    /* 0x2B18 */ int mCurrentState;
    /* 0x2B1C */ int mState;
    /* 0x2B20 */ int mStep;
    /* 0x2B24 */ BOOL mEntered;    // The state has just been set
    /* 0x2B28 */ BOOL mDone;       // The state has done what it does: the script goes on
    /* 0x2B2C */ int mFrame;       // Frames in the state
    /* 0x2B30 */ int mUnknown2B30;
    /* 0x2B34 */ int mIconFrame;   // STATE_ACT_1C: the frame the head icon appears in
    /* 0x2B38 */ BOOL mAnimDone;   // The animation is over and it stands again
    /* 0x2B3C */ BOOL mLipSync;    // The character's mouth follows the speech
    /* 0x2B40 */ DivorceContext *mContext;
    /* 0x2B44 */ TtsPlayer *mTtsPlayer;
    /* 0x2B48 */ DivorceCharacter *mPartner;
    /* 0x2B4C */ FloorPos mWalkTarget;
    /* 0x2B54 */ FloorPos mTurnTarget;
    /* 0x2B5C */ HeadIcon mHeadIcon;
}; // 0x2B9C

// The camera: in state 1 it follows the first character, in state 2 the middle between the two, keeping the
// distance and the height.
class DivorceCamera {
public:
    void Init(DivorceCharacter *first, DivorceCharacter *second);
    void Reset(BOOL restore);
    void SetPosition(VecFx32 position);
    void SetTarget(VecFx32 target);
    void SetState(int state);
    void Update();
    VecFx32 Approach(VecFx32 *goal, VecFx32 *current, int shift);

    /* 0x00 */ int mState;
    /* 0x04 */ Camera *mCamera;
    /* 0x08 */ DivorceCharacter *mFirst;
    /* 0x0C */ DivorceCharacter *mSecond;
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
}; // 0x28

// The two lights, which follow the camera; their colour can be set or blended to the light colour of the camera's.
class DivorceLights {
public:
    void Init(Camera *camera, Room *room);
    void Update();
    void UpdateDirections();
    void SetState(int state);
    void SetColor(u16 color);
    void BlendColor(u16 from, u16 to, u16 *color, int frames);

    /* 0x00 */ int mState;
    /* 0x04 */ int mStep;
    /* 0x08 */ int mFrame;
    /* 0x0C */ Light *mLights[2];
    /* 0x14 */ Camera *mCamera;
    /* 0x18 */ Room *mRoom;
    /* 0x1C */ u16 mFromColor;
    /* 0x1E */ u16 mToColor;
}; // 0x20

// One command of the script: what it is and up to four arguments (a character, a vector, a message, frames)
struct DivorceCommand {
    /* 0x00 */ int mCommand;
    /* 0x04 */ int mArgs[4];
}; // 0x14

// The scene's script: a list of commands that move the characters, the camera and the lights. Each frame it runs
// commands until one has to wait.
class DivorceScript {
public:
    void Reset();
    void SetCharacters(DivorceCharacter *first, DivorceCharacter *second);
    void Update();
    BOOL Run(int index);

    /* 0x00 */ BOOL mDone;
    /* 0x04 */ int mIndex;  // The command being run
    /* 0x08 */ int mFrame;  // Frames in it
    /* 0x0C */ DivorceCharacter *mCharacters[2];
    /* 0x14 */ Room *mRoom; // Never set in this scene
    /* 0x18 */ DivorceCamera *mCamera;
    /* 0x1C */ DivorceLights *mLights;
    /* 0x20 */ TtsPlayer *mTtsPlayer;
    /* 0x24 */ BOOL *mCondition; // What command 47 branches on
}; // 0x28

// A debugging view of the lower screen, left from development: the camera ("Ｃ"), its target ("Ａ") and the two
// characters ("●", "○") on a map of the floor, with three lines of help ("X/Y：操作切り替え", "START：パラメータ出力",
// "SELECT：イベントに戻る"). The scene sets it up; nothing draws it.
class DivorceDebugView {
public:
    void Reset();
    void ClearPoints();
    void Init(DivorceCharacter *first, DivorceCharacter *second, int charOffset);
    void UpdateMarks();
    void ToScreen(ScreenPoint *point, VecFx32 *position);

    /* 0x000 */ int mUnknown000;
    /* 0x004 */ int mUnknown004;
    /* 0x008 */ int mUnknown008;
    /* 0x00C */ NNSG2dFVec2 mUnknown00C;
    /* 0x014 */ NNSG2dFVec2 mUnknown014;
    /* 0x01C */ NNSG2dFVec2 mUnknown01C;
    /* 0x024 */ ScreenPoint mPoints[10];
    /* 0x074 */ int mUnknown074;
    /* 0x078 */ int mUnknown078;
    /* 0x07C */ int mUnknown07C;
    /* 0x080 */ u8 mUnknown080[8];
    /* 0x088 */ ObjTextBox mMarks[4];
    /* 0x778 */ int mUnknown778;
    /* 0x77C */ ObjTextBox mHelp[3];
    /* 0xCB0 */ DivorceCharacter *mFirst;
    /* 0xCB4 */ DivorceCharacter *mSecond;
}; // 0xCB8

class DivorceScene : public Scene {
public:
    DivorceScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void Finish();
    void Separate();
    void ChooseCharacters(int *first, int *second);
    void InitLowerScreen();
    void Init3d();

    /* 0x00010 */ int mCharOffset;
    /* 0x00014 */ int mDebugCharOffset;
    /* 0x00018 */ int mDrawState;
    /* 0x0001C */ int mDrawFrames;
    /* 0x00020 */ BOOL mCondition;  // What the script's command 47 branches on; never set
    /* 0x00024 */ BOOL mPaused;     // Only the camera and the lights move; never set
    /* 0x00028 */ AnimModel mModel; // "divorce.imd"
    /* 0x00108 */ SkyPalette mSky;
    /* 0x0013C */ G3Sprite mG3Sprite;
    /* 0x00188 */ TtsPlayer mTtsPlayer;
    /* 0x14ACC */ MessageText mSpeechText;
    /* 0x150BC */ SpeechWindow mWindow;
    /* 0x156C8 */ MessageFile mMessageFiles[3];
    /* 0x15710 */ DivorceScript mScript;
    /* 0x15738 */ DivorceCamera mCamera;
    /* 0x15760 */ DivorceLights mLights;
    /* 0x15780 */ DivorceDebugView mDebugView;
    /* 0x16438 */ DivorceContext mContext;
    /* 0x16464 */ int mCharacterIndices[2];
    /* 0x1646C */ DivorceCharacter mCharacters[2];
    /* 0x1BBA4 */ ParticleSystem mParticles;
    /* 0x1BBA8 */ HeadIcon mHeadIcons[1];
}; // 0x1BBE8

#endif // OV103_DIVORCESCENE_H
