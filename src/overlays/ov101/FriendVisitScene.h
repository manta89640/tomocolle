#ifndef OV101_FRIENDVISITSCENE_H
#define OV101_FRIENDVISITSCENE_H

#include <nitro.h>

#include "HeadIcon.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "voice/TtsPlayer.h"

// Overlay 101, scene 0xAF: a resident who wants to make friends with another goes and talks to them. It is the
// trouble whose talk is in the trouble messages 0x160 to 0x169 ("How should I talk?", "I see, I will go and talk to
// them a little", "Thank you! Thanks to you we became friends!").
//
// The scene has two modes, by the scene it comes from:
// - From the touch scene (0x40, MODE_SET_OFF): the resident (gSceneMemory.mCharacters[0]) says it will go and talk,
//   then the scene asks for itself, with the two characters swapped.
// - From itself (MODE_VISIT): the resident (now the second) rings at the other's door, comes in, and the two talk
//   in the manner the player chose in scene 0x40 (gSceneMemory's talk answer); whether they become friends is drawn
//   at Init by the relation meter, that manner, the trouble and how many friends each has. The scene then leaves
//   the outcome in gSceneScratch[0] (0: friends, 1: not) and goes back to scene 0x40.
//
// The constructor is the main program's (0x020489B4, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214D0C0). The names are descriptive, not the original: no name of these classes survives in the
// ROM.

class FriendVisitCharacter;
class FriendVisitCamera;
class FriendVisitLights;
struct Camera;
struct Light;

// What the scene gives its characters.
struct FriendVisitContext {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ MessageFile *mCommonMessages;
    /* 0x08 */ MessageFile *mTroubleMessages;
    /* 0x0C */ MessageFile *mEtcMessages;
    /* 0x10 */ SpeechWindow *mWindow;
    /* 0x14 */ TtsPlayer *mTtsPlayer;
    /* 0x18 */ MessageText *mSpeechText;
    /* 0x1C */ FriendVisitCamera *mCamera;
    /* 0x20 */ FriendVisitLights *mLights;
    /* 0x24 */ FriendVisitCharacter *mCharacters[2];
}; // 0x2C

// A character of the scene. Its implicit constructor (CharacterBase's C2, CharacterModel's C2 for the second base,
// HeadIcon's C1 for the member) and destructor are the main program's copies after the scene's constructor, emitted
// for the scene's __cxa_vec_ctor.
class FriendVisitCharacter : public Character {
public:
    enum State {
        STATE_STAND,     // Stands
        STATE_WALK_IN,   // Walks to mWalkTarget
        STATE_TURN,      // Turns towards mTurnTarget
        STATE_BOW,       // Animation 0x39, once
        STATE_BOW2,      // Animation 0x3A, once
        STATE_TALK,      // Animation 0x55, and after a while the head icon 0xD
        STATE_TALK2,     // Animation 0x4A, then back to STATE_TALK
        STATE_STOP_ICON, // Lets the head icon end
        STATE_SET_OFF,   // Says it will go and talk (trouble message 0x166)
    };

    // What one argument of a message is
    enum ArgType {
        ARG_NONE,
        ARG_CHARACTER, // The name of the saved character with that index
    };

    void Init(FriendVisitContext *context);
    void LoadHeadIcon(ParticleSystem *particles);
    void Update();
    void UpdateState();
    void SetState(int state);
    void FaceCamera();
    void Speak(int file, u32 message, int count, int *types, int *values, int mode);
    void Speak(int file, u32 message, int type, int value, int mode);
    void Speak(int file, u32 message, int mode);
    void LookAt(VecFx32 *target, int pitchLimit, int yawLimit);
    void LookAtCamera();
    u16 GetHeadPitchTo(VecFx32 *target);
    u16 GetHeadYawTo(VecFx32 *target);
    void PlaceHeadIcon();

    /* 0x2B18 */ int mCurrentState;
    /* 0x2B1C */ int mState;
    /* 0x2B20 */ BOOL mEntered;   // The state has just been set
    /* 0x2B24 */ BOOL mDone;      // The state has done what it does: the scene goes on
    /* 0x2B28 */ int mFrame;      // Frames in the state
    /* 0x2B2C */ int mWait;       // Frames a step waits
    /* 0x2B30 */ int mIconFrame;  // STATE_TALK: the frame the head icon comes at
    /* 0x2B34 */ int mStep;
    /* 0x2B38 */ BOOL mLipSync;   // The character's mouth follows the speech
    /* 0x2B3C */ FriendVisitContext *mContext;
    /* 0x2B40 */ TtsPlayer *mTtsPlayer;
    /* 0x2B44 */ FriendVisitCharacter *mPartner;
    /* 0x2B48 */ FloorPos mWalkTarget; // Where STATE_WALK_IN walks to
    /* 0x2B50 */ FloorPos mTurnTarget; // What STATE_TURN turns to
    /* 0x2B58 */ HeadIcon mHeadIcon;
}; // 0x2B98

// The camera: it follows the first character, or the middle between the two.
class FriendVisitCamera {
public:
    void Init(FriendVisitCharacter *first, FriendVisitCharacter *second);
    void Reset(int previousSceneId);
    void Update();
    void SetState(int state);
    VecFx32 Approach(VecFx32 *goal, VecFx32 *current, int shift);

    /* 0x00 */ int mState; // 0: still, 1: follows the first character, 2: follows both
    /* 0x04 */ Camera *mCamera;
    /* 0x08 */ FriendVisitCharacter *mFirst;
    /* 0x0C */ FriendVisitCharacter *mSecond;
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
}; // 0x28

// The two lights, which follow the camera and take the room's colour.
class FriendVisitLights {
public:
    void Init(Camera *camera, Room *room);
    void Update();
    void SetColor(u16 color);

    /* 0x00 */ int mState;
    /* 0x04 */ Light *mLights[2];
    /* 0x0C */ Camera *mCamera;
    /* 0x10 */ Room *mRoom;
}; // 0x14

class FriendVisitScene : public Scene {
public:
    // mMode
    enum Mode {
        MODE_SET_OFF, // From the touch scene: the resident says it will go
        MODE_VISIT,   // The visit
    };

    FriendVisitScene(int sceneId); // 0x020489B4, in the main program

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void InitLowerScreen();
    void Init3d();
    void ChooseCharacters();
    void SetSpeaker(int slot);
    void UpdateState();
    void UpdateSetOff();
    void UpdateVisit();
    void NextState();
    void SetState(int state);
    BOOL DrawFriendship(FriendVisitCharacter *host, FriendVisitCharacter *visitor);
    void ApplyOutcome(BOOL friends);

    /* 0x00010 */ int mCharOffset;
    /* 0x00014 */ int mUnknown14;
    /* 0x00018 */ int mDrawState;
    /* 0x0001C */ int mDrawFrames;
    /* 0x00020 */ int mState;
    /* 0x00024 */ int mFrame;
    /* 0x00028 */ int mWait;
    /* 0x0002C */ BOOL mEntered;      // The state has just been set
    /* 0x00030 */ int mMode;
    /* 0x00034 */ BOOL mFriends;      // The outcome, drawn at Init
    /* 0x00038 */ BOOL mMannerMatched; // The manner of talking the player chose suits the visitor
    /* 0x0003C */ Room mRoom;
    /* 0x00944 */ TtsPlayer mTtsPlayer;
    /* 0x15288 */ MessageText mSpeechText;
    /* 0x15878 */ SpeechWindow mWindow;
    /* 0x15E84 */ MessageFile mMessageFiles[3];
    /* 0x15ECC */ SpriteButton mButton;
    /* 0x15FC0 */ SpriteButton mButton15FC0;
    /* 0x160B4 */ SpriteButton mButton160B4;
    /* 0x161A8 */ SpriteButton mButton161A8;
    /* 0x1629C */ FriendVisitCamera mCamera;
    /* 0x162C4 */ FriendVisitLights mLights;
    /* 0x162D8 */ FriendVisitContext mContext;
    /* 0x16304 */ FriendVisitCharacter mCharacters[2];
    /* 0x1BA34 */ int mCharacterIndices[2]; // Which saved characters the two are
    /* 0x1BA3C */ ParticleSystem mParticles;
    /* 0x1BA40 */ HeadIcon mHeadIcons[1];
}; // 0x1BA80

#endif // OV101_FRIENDVISITSCENE_H
