#ifndef OV102_MAKEUPSCENE_H
#define OV102_MAKEUPSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "HeadIcon.h"
#include "SpriteButton.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "voice/TtsPlayer.h"

// Overlay 102, scene 0xB2: two characters who have fallen out make up, or fail to. The scene of touching a Mii
// (scene 0x40, overlay 75) sends a character there once it has calmed down ("I feel better. I think I will go and
// make up with them", trouble message 0x186), with the one it fought in gSceneMemory.mCharacters[0] and itself in
// [1]. The first waits in its room; there is a knock, the door opens, the second comes in, bows and walks up, and
// says "I am sorry about the other day" (trouble message 0x25B). By a chance that grows with how much the two like
// each other, the first answers "I am sorry too" (0x25D) and they are friends, lovers or spouses again, or "I will
// not forgive you" (0x25F): friends then part, lovers and spouses are left heartbroken. Back in scene 0x40,
// gSceneScratch[0] says which ("Thank you, we made up" or "It did not work...", 0x188 and 0x18A).
//
// The scene factory (CreateScene) loads overlay 102 and allocates 0x1BA84 bytes; the constructor is the main
// program's (0x02048EDC, src/scene/SceneManager.cpp). The names are descriptive, not the original: no name of
// these classes survives in the ROM.

extern "C" {
extern const int gPersonalityKinds[16];  // By mPersonality: the row of the two tables below
extern const s16 gStandAnimations[4][5]; // Animation ids, by that row and by mMood: standing
extern const s16 gWalkAnimations[4][5];  // The same: walking
}

class MakeUpCharacter;
class MakeUpCamera;
class MakeUpLights;
struct Camera; // The main program's camera and lights (MainProgram.h)
struct Light;

// What the scene gives its characters.
struct MakeUpContext {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ MessageFile *mCommonMessages;
    /* 0x08 */ MessageFile *mTroubleMessages;
    /* 0x0C */ MessageFile *mEtcMessages;
    /* 0x10 */ SpeechWindow *mWindow;
    /* 0x14 */ TtsPlayer *mTtsPlayer;
    /* 0x18 */ MessageText *mSpeechText;
    /* 0x1C */ MakeUpCamera *mCamera;
    /* 0x20 */ MakeUpLights *mLights;
    /* 0x24 */ MakeUpCharacter *mCharacters[2];
}; // 0x2C

// A character of the scene: the main program's Character with a state machine and a head icon. Its implicit
// constructor (CharacterBase's C2, CharacterModel's C2 for the second base, HeadIcon's C1 for the member) and
// destructor are the main program's copies after the scene's constructor, emitted for the scene's __cxa_vec_ctor.
class MakeUpCharacter : public Character {
public:
    enum State {
        STATE_STAND,       // The standing animation of its personality
        STATE_WALK,        // Walks to mWalkTarget
        STATE_TURN,        // Turns to face mTurnTarget
        STATE_BOW,         // Animation 0x39, once, with a sound half-way
        STATE_BOW_DEEP,    // Animation 0x3A, once, with a sound half-way
        STATE_SULK,        // Animation 0x55, a sound each time it loops, and head icon 13 after a while
        STATE_APOLOGIZE,   // "I am sorry about the other day"
        STATE_ACCEPT,      // "I am sorry too"
        STATE_REFUSE,      // "I will not forgive you"
        STATE_HIDE_ICON,
    };

    // What one argument of a message is
    enum ArgType {
        ARG_NONE,
        ARG_CHARACTER, // The name of the saved character with that index
    };

    void Init(MakeUpContext *context);
    void LoadHeadIcon(ParticleSystem *particles);
    void Update();
    void UpdateState();
    void SetState(int state);
    void Speak(int file, u32 message, int count, int *types, int *values, int mode);
    void Speak(int file, u32 message, int type, int value, int mode);
    void Speak(int file, u32 message, int mode);
    void PlaceHeadIcon();
    void SetHeadIcon(s16 id);
    void HideHeadIcon();

    VecFx32 GetPos() { return mPos; }
    s16 GetStandAnimation() { return gStandAnimations[gPersonalityKinds[mPersonality]][0]; }
    void SetTargetFloorPos(FloorPos pos) {
        mTarget.x = pos.x;
        mTarget.z = pos.z;
    }
    ModelAnimation *GetAnimation() { return AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId); }
    // The length of the current animation in frames
    int GetAnimationFrames() {
        ModelAnimation *animation = GetAnimation();
        return (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 +
               1;
    }

    /* 0x2B18 */ int mCurrentState;
    /* 0x2B1C */ int mState;
    /* 0x2B20 */ int mStep;         // The step of the state: the bows have only step 0
    /* 0x2B24 */ BOOL mEntered;     // The state has just been set
    /* 0x2B28 */ BOOL mDone;        // The state has done what it does: the scene goes on
    /* 0x2B2C */ int mFrame;        // Frames in the state
    /* 0x2B30 */ int mUnknown2B30;  // Cleared by SetState
    /* 0x2B34 */ int mWait;         // Frames STATE_SULK waits before its head icon
    /* 0x2B38 */ BOOL mAnimDone;    // The speaking states' animation has ended
    /* 0x2B3C */ BOOL mLipSync;     // The character's mouth follows the speech
    /* 0x2B40 */ MakeUpContext *mContext;
    /* 0x2B44 */ TtsPlayer *mTtsPlayer;
    /* 0x2B48 */ MakeUpCharacter *mPartner;
    /* 0x2B4C */ FloorPos mWalkTarget;
    /* 0x2B54 */ FloorPos mTurnTarget;
    /* 0x2B5C */ HeadIcon mHeadIcon;
}; // 0x2B9C

// The camera: it follows the first character, or the middle between the two.
class MakeUpCamera {
public:
    enum State {
        STATE_STILL,
        STATE_FOLLOW_FIRST,
        STATE_FOLLOW_BOTH,
    };

    void Init(MakeUpCharacter *first, MakeUpCharacter *second);
    void Reset(int previousScene);
    void Update();
    void SetState(int state);
    VecFx32 Approach(VecFx32 *goal, VecFx32 *current, int shift);

    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }
    void SetTarget(VecFx32 target) {
        mTarget.x = target.x;
        mTarget.y = target.y;
        mTarget.z = target.z;
    }

    /* 0x00 */ int mState;
    /* 0x04 */ Camera *mCamera;
    /* 0x08 */ MakeUpCharacter *mFirst;
    /* 0x0C */ MakeUpCharacter *mSecond;
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
}; // 0x28

// The two lights, which follow the camera and take the room's colour.
class MakeUpLights {
public:
    void Init(Camera *camera, Room *room);
    void Update();
    void SetColor(u16 color);

    /* 0x00 */ int mState;
    /* 0x04 */ Light *mLights[2];
    /* 0x0C */ Camera *mCamera;
    /* 0x10 */ Room *mRoom;
}; // 0x14

class MakeUpScene : public Scene {
public:
    enum State {
        STATE_WAIT,             // The first stands in its room
        STATE_KNOCK,
        STATE_OPEN_DOOR,
        STATE_COME_IN,          // The second walks in
        STATE_CLOSE_DOOR,
        STATE_PAUSE,
        STATE_BOW,
        STATE_PAUSE_2,
        STATE_FACE,             // The first turns to the second
        STATE_PAUSE_3,
        STATE_WALK_UP,          // The second walks up to it
        STATE_STAND,
        STATE_APOLOGIZE,        // The second says it is sorry
        STATE_THINK,            // A while, before the answer
        STATE_ACCEPT,
        STATE_REFUSE,
        STATE_PAUSE_4,
        STATE_LEAVE,
    };

    MakeUpScene(int sceneId); // 0x02048EDC, in the main program

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void InitLowerScreen();
    void Init3d();
    void GetCharacterIndices(int *first, int *second);
    void UpdateState();
    void NextState();
    void SetState(int state);
    BOOL DecideMakeUp(MakeUpCharacter *first, MakeUpCharacter *second);
    void ApplyResult(BOOL madeUp);

    /* 0x00010 */ int mCharOffset;
    /* 0x00014 */ int mUnknown14;
    /* 0x00018 */ int mDrawState;
    /* 0x0001C */ int mDrawFrames;
    /* 0x00020 */ int mState;
    /* 0x00024 */ int mStep;
    /* 0x00028 */ int mFrame;
    /* 0x0002C */ int mWait;       // Frames STATE_THINK waits
    /* 0x00030 */ BOOL mEntered;   // The state has just been set
    /* 0x00034 */ BOOL mMadeUp;    // Whether the two make up (DecideMakeUp, in Init)
    /* 0x00038 */ Room mRoom;      // The first character's
    /* 0x00940 */ TtsPlayer mTtsPlayer;
    /* 0x15284 */ MessageText mSpeechText;
    /* 0x15874 */ SpeechWindow mWindow;
    /* 0x15E80 */ MessageFile mMessageFiles[3];
    /* 0x15EC8 */ SpriteButton mButton15EC8; // The lower screen's four buttons, disabled
    /* 0x15FBC */ SpriteButton mButton15FBC;
    /* 0x160B0 */ SpriteButton mButton160B0;
    /* 0x161A4 */ SpriteButton mButton161A4;
    /* 0x16298 */ MakeUpCamera mCamera;
    /* 0x162C0 */ MakeUpLights mLights;
    /* 0x162D4 */ MakeUpContext mContext;
    /* 0x16300 */ int mCharacterIndices[2];
    /* 0x16308 */ MakeUpCharacter mCharacters[2];
    /* 0x1BA40 */ ParticleSystem mParticles;
    /* 0x1BA44 */ HeadIcon mHeadIcons[1];
}; // 0x1BA84

#endif // OV102_MAKEUPSCENE_H
