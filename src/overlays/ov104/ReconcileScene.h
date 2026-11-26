#ifndef OV104_RECONCILESCENE_H
#define OV104_RECONCILESCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "HeadIcon.h"
#include "Model.h"
#include "Scene.h"
#include "character/Character.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 104, scene 0xB5: two characters who are lovers or married and have fallen out make it up, or part. They
// stand facing each other in a room; one says "I am sorry about the other day" (trouble message 0x25B), the other
// "I am sorry too" (0x25D) or "I will not forgive you" (0x25F), and they say "Let us start again" (0x261), "Yes,
// please" (0x263). The outcome is drawn at random, one in two (ReconcileScene::DecideOutcome): made up, they are
// lovers or spouses again; otherwise ex-lovers or divorced (ReconcileScene::ApplyOutcome). What the characters and
// the camera do comes from a table of commands (sScript, run by ReconcileScript). The scene factory (CreateScene)
// loads overlay 104 and allocates 0x1C390 bytes; the constructor is the main program's (0x0204963C,
// src/scene/SceneManager.cpp).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

struct Camera; // The main program's camera and lights (MainProgram.h)
struct Light;

class ReconcileCamera;
class ReconcileCharacter;
class ReconcileLights;

// What the scene gives its characters.
struct ReconcileContext {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ MessageFile *mCommonMessages;
    /* 0x08 */ MessageFile *mTroubleMessages;
    /* 0x0C */ MessageFile *mEtcMessages;
    /* 0x10 */ SpeechWindow *mWindow;
    /* 0x14 */ TtsPlayer *mTtsPlayer;
    /* 0x18 */ MessageText *mSpeechText;
    /* 0x1C */ ReconcileCamera *mCamera;
    /* 0x20 */ ReconcileLights *mLights;
    /* 0x24 */ ReconcileCharacter *mCharacters[2];
}; // 0x2C

// A character of the scene. Its implicit constructor and destructor are the main program's copies after the scene's
// constructor (0x020497C0, 0x020497EC), emitted for the scene's __cxa_vec_ctor.
class ReconcileCharacter : public Character {
public:
    enum State {
        STATE_STAND,         // Stands
        STATE_START_AGAIN,   // "Let us start again" (0x261)
        STATE_AGREE,         // "Yes, please" (0x263)
        STATE_STAND_2,       // Stands, another animation
        STATE_WALK,          // Walks to mWalkTargets[0]
        STATE_TURN,          // Turns towards mWalkTargets[1]
        STATE_ACT,           // An animation once, with a sound halfway
        STATE_ACT_2,         // The same, another animation and sound
        STATE_SULK,          // An animation with a sound in each loop, and an icon over the head after a while
        STATE_APOLOGIZE,     // "I am sorry about the other day" (0x25B)
        STATE_APOLOGIZE_TOO, // "I am sorry too" (0x25D)
        STATE_REFUSE,        // "I will not forgive you" (0x25F)
        STATE_HIDE_ICON,     // The head icon's mode 1
    };

    // What one argument of a message is
    enum ArgType {
        ARG_NONE,
        ARG_CHARACTER, // The name of the saved character with that index
    };

    void Init(ReconcileContext *context);
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
    void SetPos(VecFx32 pos) {
        mPos.x = pos.x;
        mPos.y = pos.y;
        mPos.z = pos.z;
    }

    /* 0x2B18 */ int mCurrentState;
    /* 0x2B1C */ int mState;
    /* 0x2B20 */ BOOL mUnknown2B20; // Cleared by SetState; while set, STATE_ACT and STATE_ACT_2 wait
    /* 0x2B24 */ BOOL mEntered;     // The state has just been set
    /* 0x2B28 */ BOOL mDone;        // The state has done what it does: the script goes on
    /* 0x2B2C */ int mFrame;        // Frames in the state
    /* 0x2B30 */ int mStep;
    /* 0x2B34 */ int mWait;         // When STATE_SULK shows the icon
    /* 0x2B38 */ BOOL mAnimDone;    // The speaking states' animation is over
    /* 0x2B3C */ BOOL mLipSync;     // The character's mouth follows the speech
    /* 0x2B40 */ ReconcileContext *mContext;
    /* 0x2B44 */ TtsPlayer *mTtsPlayer;
    /* 0x2B48 */ ReconcileCharacter *mPartner;
    /* 0x2B4C */ FloorPos mWalkTargets[2]; // Where STATE_WALK and STATE_TURN go; nothing in the overlay sets them
    /* 0x2B5C */ HeadIcon mHeadIcon;
}; // 0x2B9C

// The camera: placed by the script, or following one character or the middle of the two.
class ReconcileCamera {
public:
    enum Mode {
        MODE_FIXED,
        MODE_FOLLOW_FIRST,
        MODE_FOLLOW_BOTH,
    };

    void Init(ReconcileCharacter *first, ReconcileCharacter *second);
    void Reset(BOOL restore);
    void SetPosition(VecFx32 position);
    void SetTarget(VecFx32 target);
    void SetMode(int mode);
    void Update();
    VecFx32 Approach(VecFx32 *goal, VecFx32 *current, int shift);

    /* 0x00 */ int mMode;
    /* 0x04 */ Camera *mCamera;
    /* 0x08 */ ReconcileCharacter *mFirst;
    /* 0x0C */ ReconcileCharacter *mSecond;
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
}; // 0x28

// The two lights, which follow the camera, take the room's colour, or fade to black.
class ReconcileLights {
public:
    enum State {
        STATE_NONE,
        STATE_INTERIOR_COLOR, // The colour of the room's interior, from the table
        STATE_ROOM_COLOR,     // The room's light colour
        STATE_BLACK,
        STATE_FADE_OUT,       // To black in 64 frames
    };

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

// What the scene does, as a list of commands (sScript): the camera, the lights, the door, the characters' places,
// states and speech, the music, jumps and waits.
class ReconcileScript {
public:
    void Reset();
    void SetCharacters(ReconcileCharacter *first, ReconcileCharacter *second);
    void Update();
    BOOL Execute(int step);

    /* 0x00 */ BOOL mDone;
    /* 0x04 */ int mStep;
    /* 0x08 */ int mFrame; // Frames since the step began
    /* 0x0C */ ReconcileCharacter *mCharacters[2];
    /* 0x14 */ Room *mRoom;
    /* 0x18 */ ReconcileCamera *mCamera;
    /* 0x1C */ ReconcileLights *mLights;
    /* 0x20 */ TtsPlayer *mTtsPlayer;
    /* 0x24 */ BOOL *mMadeUp;
}; // 0x28

// A development aid on the lower screen: a map of the camera ("Ｃ"), its target ("Ａ") and the two characters ("●",
// "○") seen from above, and three lines of instructions ("X/Y：操作切り替え", "START：パラメータ出力",
// "SELECT：イベントに移る"). It is set up and placed once; nothing in the overlay updates or draws it.
class ReconcileDebugMap {
public:
    void Reset();
    void ClearPoints();
    void Init(ReconcileCharacter *first, ReconcileCharacter *second, int charOffset);
    void PlaceMarks();
    void ToScreen(ScreenPoint *point, VecFx32 *position);

    /* 0x000 */ int mUnknown000;
    /* 0x004 */ int mUnknown004;
    /* 0x008 */ int mUnknown008;
    /* 0x00C */ ScreenPoint mUnknown00C[3];
    /* 0x024 */ ScreenPoint mUnknown024[10];
    /* 0x074 */ int mUnknown074;
    /* 0x078 */ int mUnknown078;
    /* 0x07C */ BOOL mUnknown07C;
    /* 0x080 */ u8 mUnknown080[8];
    /* 0x088 */ ObjTextBox mMarks[4]; // The camera, its target, the first character, the second
    /* 0x778 */ int mUnknown778;
    /* 0x77C */ ObjTextBox mLines[3]; // The instructions
    /* 0xCB0 */ ReconcileCharacter *mFirst;
    /* 0xCB4 */ ReconcileCharacter *mSecond;
}; // 0xCB8

class ReconcileScene : public Scene {
public:
    ReconcileScene(int sceneId); // 0x0204963C, in the main program

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    BOOL DecideOutcome();
    void Finish();
    void ApplyOutcome(BOOL madeUp);
    void ChooseCharacters(int *first, int *second);
    void InitLowerScreen();
    void Init3d();

    /* 0x00010 */ int mCharOffset;
    /* 0x00014 */ int mMapCharOffset;
    /* 0x00018 */ int mDrawState;
    /* 0x0001C */ int mDrawFrames;
    /* 0x00020 */ BOOL mMadeUp;     // DecideOutcome's draw
    /* 0x00024 */ BOOL mUnknown24;  // Cleared by Init; while set, Update only moves the camera and the lights
    /* 0x00028 */ Room mRoom;
    /* 0x00930 */ TtsPlayer mTtsPlayer;
    /* 0x15274 */ MessageText mSpeechText;
    /* 0x15864 */ SpeechWindow mWindow;
    /* 0x15E70 */ MessageFile mMessageFiles[3]; // Common, trouble and etc messages
    /* 0x15EB8 */ ReconcileScript mScript;
    /* 0x15EE0 */ ReconcileCamera mCamera;
    /* 0x15F08 */ ReconcileLights mLights;
    /* 0x15F28 */ ReconcileDebugMap mMap;
    /* 0x16BE0 */ ReconcileContext mContext;
    /* 0x16C0C */ int mCharacterIndices[2]; // Of the saved characters
    /* 0x16C14 */ ReconcileCharacter mCharacters[2];
    /* 0x1C34C */ ParticleSystem mParticles;
    /* 0x1C350 */ HeadIcon mHeadIcons[1];
}; // 0x1C390

#endif // OV104_RECONCILESCENE_H
