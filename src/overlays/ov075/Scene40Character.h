#ifndef OV075_SCENE40CHARACTER_H
#define OV075_SCENE40CHARACTER_H

#include <nitro.h>

#include "HeadIcon.h"
#include "Model.h"
#include "character/Character.h"
#include "item/Item.h"
#include "voice/TtsPlayer.h"

#include "Scene40Context.h"
#include "Scene40SpeechWindow.h"

extern "C" {
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
}

// Overlay 75's character, the Mii the player visits and touches, and the two objects with a model it carries. The
// names are descriptive: no name of these classes survives in the ROM.

// The goods and clothes the scene shows, two bases: the constructor stores both of this class's tables (this
// file's data, 0x02110B4C with the second at +0x2C) after both base constructors.
class Scene40Items : public InventoryItem, public ClothesItem {}; // 0x2C

// A present the character receives (ov075-c's; 0x0215efc0 sets it). The goods and clothes are a member, not a base:
// the scene's constructor stores only Scene40Items' tables there.
class Scene40Gift {
public:
    /* 0x00 */ Scene40Items mItem;
    /* 0x2C */ int mKind;
    /* 0x30 */ int mGiftKind;
    /* 0x34 */ int mMessage;
}; // 0x38

// Two objects of the scene's character, each with a model that its Draw draws (0x02178700, 0x02178cf0). What they
// show is not known: the names say only where they are.
class Scene40CharacterPartA {
public:
    void Draw();

    /* 0x00 */ u8 mUnknown00[0x10];
    /* 0x10 */ AnimModel mModel;
    /* 0xF0 */ u8 mUnknownF0[0x8];
}; // 0xF8

class Scene40CharacterPartB {
public:
    void Draw();

    /* 0x00 */ u8 mUnknown00[0x4];
    /* 0x04 */ AnimModel mModel;
}; // 0xE4

// A character with two models, the goods and clothes it holds and two head icons
class Scene40Character : public Character {
public:
    void Draw(); // The character, then the two objects with a model each at 0x2BE4 and 0x2CDC
    void Update(BOOL speaking);

    // The states 0x44 to 0xB2, a switch over a range of states each (0x02168354 to 0x021722c8)
    void UpdateStates44To47();
    void UpdateStates48To4E();
    void UpdateStates4FTo66();
    void UpdateStates67To85();
    void UpdateStates86ToB2();

    void SetState(int state);
    void PlaceHeldItem();
    void PlaceHeldItemInHand();
    void StartLongActWithTool(int item);
    void Speak(int file, u32 message, int type, int value, int mode);
    void Speak(int file, u32 message, int mode);
    void ResetHeadTurn();
    void TurnHeadToCamera();

    // Starts the 'BCA0' animation of that name, blended from the one before (once: play it once)
    void PlayAnimation(const char *name, BOOL once) {
        Character::BlendToAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mAnim.mUnknown04, RESOURCE_BCA0, name));
        AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId)->mPlayOnce = once;
    }
    void BlendToNamedAnimation(const char *name) {
        Character::BlendToAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mAnim.mUnknown04, RESOURCE_BCA0, name));
    }
    void SetNamedAnimation(const char *name) {
        Character::SetAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mAnim.mUnknown04, RESOURCE_BCA0, name));
    }
    ModelAnimation *GetAnimation() { return AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId); }
    void SetTargetFloorPos(FloorPos pos) {
        mTarget.x = pos.x;
        mTarget.z = pos.z;
    }

    /* 0x2B18 */ u8 mUnknown2B18[0xBC];
    /* 0x2BD4 */ ItemModel *mHeldItem;  // The item it holds out or receives
    /* 0x2BD8 */ TtsPlayer *mTtsPlayer; // The scene's, for the lip movement
    /* 0x2BDC */ Scene40SpeechWindow *mSpeechWindow;
    /* 0x2BE0 */ Scene40Context *mContext;
    /* 0x2BE4 */ Scene40CharacterPartA mPartA;
    /* 0x2CDC */ Scene40CharacterPartB mPartB;
    /* 0x2DC0 */ u8 mUnknown2DC0[0x4];
    /* 0x2DC4 */ Scene40Gift mGift;
    /* 0x2DFC */ HeadIcon mHeadIcons[2];
    /* 0x2E7C */ int mHeadIconPlaces[2];
    /* 0x2E84 */ int mState;     // The state UpdateState runs
    /* 0x2E88 */ int mPrevState;
    /* 0x2E8C */ int mNextState; // SetState sets it; mState takes it at the next update
    /* 0x2E90 */ int mStep;      // The step of the state
    /* 0x2E94 */ BOOL mEntered;  // The state has just been set
    /* 0x2E98 */ int mFrame;     // Frames in the state, or in the step
    /* 0x2E9C */ int mWait;      // Frames a step waits
    /* 0x2EA0 */ u8 mUnknown2EA0[0x24];
    /* 0x2EC4 */ BOOL mLipSync;         // Update moves the lips with the speech only while it is set
    /* 0x2EC8 */ u8 mUnknown2EC8[0x230];
}; // 0x30F8

#endif // OV075_SCENE40CHARACTER_H
