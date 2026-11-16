#ifndef OV096_WEDDINGSCENE_H
#define OV096_WEDDINGSCENE_H

#include <nitro.h>
#include "text/TextUtil.h"
#include <nnsys.h>

#include "Model.h"
#include "Scene.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "character/Character.h"
#include "item/Item.h"
#include "room/Room.h"
#include "text/SpeechWindow.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

#include <wchar.h>

// Overlay 96: the wedding announcement. Two characters who are to marry stand in front of the camera; one says
// "We are getting married", the other thanks the player, the first promises a happy life, the screen announces the
// marriage, and the second hands the player a wedding album, which goes into the inventory. When the scene comes
// back from the album (scene 0xC0), they say goodbye and leave on their honeymoon.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. What it uses of the
// main program that has no shared header is in MainProgram.h, beside it.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

class WeddingCharacter;
class WeddingScene;

extern "C" {
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
}

// The item the wedding gives.
class WeddingGiftModel : public ItemModel {
public:
    void Load(int resourceId);
    void ChangeItem(int item);
}; // 0x12C

struct Camera; // The main program's camera and lights (MainProgram.h)
struct Light;

// --- This overlay's classes --------------------------------------------------------------------------------------

// What the scene gives its characters.
struct WeddingContext {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ MessageFile *mCommonMessages;
    /* 0x08 */ MessageFile *mTroubleMessages;
    /* 0x0C */ MessageFile *mItemMessages;
    /* 0x10 */ MessageFile *mEtcMessages;
    /* 0x14 */ SpeechWindow *mWindow;
    /* 0x18 */ TtsPlayer *mTtsPlayer;
    /* 0x1C */ MessageText *mSpeechText;
    /* 0x20 */ class WeddingCamera *mCamera;
    /* 0x24 */ class WeddingLights *mLights;
    /* 0x28 */ WeddingCharacter *mCharacters[2];
    /* 0x30 */ WeddingGiftModel *mGift;
}; // 0x34

class WeddingCharacter : public Character {
public:
    enum State {
        STATE_NONE,
        STATE_ANNOUNCE, // "We are getting married"
        STATE_THANK,    // "Thank you", with a greeting
        STATE_LOOK_AT_PARTNER,
        STATE_PROMISE,  // "We will live happily ever after"
        STATE_GIVE,     // "Please take this as a keepsake", and hands over the album
        STATE_FAREWELL, // "We are off on our honeymoon"
    };

    // What one argument of a message is
    enum ArgType {
        ARG_NONE,
        ARG_CHARACTER, // The name of the saved character with that index
        ARG_ITEM,      // The name of the item with that number
    };

    void Init(WeddingContext *context);
    void Update();
    void UpdateState();
    void SetState(int state);
    void FaceCamera();
    void Speak(int file, u32 message, int count, int *types, int *values, int mode);
    void Speak(int file, u32 message, int type, int value, int mode);
    void Speak(u32 message, int mode);
    void HoldOutGift();
    void LookAt(VecFx32 *target, int pitchLimit, int yawLimit);
    void TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit);
    void LookAtCamera();
    void TurnHeadToCamera();
    void LookAtPartner();
    u16 GetHeadPitchTo(VecFx32 *target);
    u16 GetHeadYawTo(VecFx32 *target);

    void PlayAnimation(const char *name, BOOL once) {
        Character::BlendToAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mAnim.mUnknown04, RESOURCE_BCA0, name));
        AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId)->mPlayOnce = once;
    }
    VecFx32 GetPos() { return mPos; }
    void SetPos(VecFx32 pos) {
        mPos.x = pos.x;
        mPos.y = pos.y;
        mPos.z = pos.z;
    }
    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mPos.x;
        pos.z = mPos.z;
        return pos;
    }
    u16 AngleTo(FloorPos target) {
        FloorPos delta = target - GetFloorPos();
        return FX_Atan2Idx(delta.z, delta.x);
    }
    ModelAnimation *GetAnimation() { return AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId); }

    /* 0x2B18 */ int mCurrentState;
    /* 0x2B1C */ int mState;
    /* 0x2B20 */ BOOL mEntered;   // The state has just been set
    /* 0x2B24 */ BOOL mDone;      // The state has done what it does: the scene goes on
    /* 0x2B28 */ int mFrame;      // Frames in the state, or in the step
    /* 0x2B2C */ int mWait;       // Frames a step waits
    /* 0x2B30 */ int mStep;
    /* 0x2B34 */ BOOL mLipSync;   // The character's mouth follows the speech
    /* 0x2B38 */ TtsPlayer *mTtsPlayer;
    /* 0x2B3C */ WeddingContext *mContext;
    /* 0x2B40 */ WeddingCharacter *mPartner;
}; // 0x2B44

// The camera: it stays in front of the two, between them.
class WeddingCamera {
public:
    void Init(WeddingCharacter *first, WeddingCharacter *second);
    void Reset(BOOL restore);
    void Update();
    void GetStandingPoints(VecFx32 *first, VecFx32 *second);
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
    /* 0x08 */ WeddingCharacter *mFirst;
    /* 0x0C */ WeddingCharacter *mSecond;
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
}; // 0x28

// The two lights, which follow the camera and take the room's colour.
class WeddingLights {
public:
    void Init(Camera *camera, Room *room);
    void Update();
    void SetColor(u16 color);

    /* 0x00 */ int mState;
    /* 0x04 */ Light *mLights[2];
    /* 0x0C */ Camera *mCamera;
    /* 0x10 */ Room *mRoom;
}; // 0x14

class WeddingScene : public Scene {
public:
    enum State {
        STATE_ANNOUNCE,
        STATE_WAIT_ANNOUNCE,
        STATE_WAIT_THANKS,
        STATE_WAIT_PROMISE,
        STATE_SHOW_MARRIED,
        STATE_WAIT_GIFT,
        STATE_FADE_OUT,
        STATE_GIVE_ALBUM,
        STATE_FAREWELL,
        STATE_WAIT_FAREWELL,
        STATE_FADE_OUT_FAREWELL,
        STATE_LEAVE,
    };

    WeddingScene(int sceneId); // 0x02048600, in the main program

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void InitLowerScreen();
    void Init3d();
    u8 GetCharacterIndex(int slot);
    void SetSpeaker(int slot);

    /* 0x00010 */ int mCharOffset;
    /* 0x00014 */ int mUnknown14;
    /* 0x00018 */ int mDrawState;
    /* 0x0001C */ int mDrawFrames;
    /* 0x00020 */ int mState;
    /* 0x00024 */ int mFrame;
    /* 0x00028 */ Inventory mInventory;
    /* 0x01814 */ WeddingContext mContext;
    /* 0x01848 */ MessageFile mMessageFiles[4];
    /* 0x018A8 */ Room mRoom;
    /* 0x021B0 */ TtsPlayer mTtsPlayer;
    /* 0x16AF4 */ MessageText mSpeechText;
    /* 0x170E4 */ SpeechWindow mWindow;
    /* 0x176F0 */ WeddingCamera mCamera;
    /* 0x17718 */ WeddingLights mLights;
    /* 0x1772C */ WeddingCharacter mCharacters[2];
    /* 0x1CDB4 */ WeddingGiftModel mGift;
}; // 0x1CEE0

#endif // OV096_WEDDINGSCENE_H
