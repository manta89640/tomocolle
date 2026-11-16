#ifndef OV075_SCENE40CHARACTER_H
#define OV075_SCENE40CHARACTER_H

#include <nitro.h>

#include "HeadIcon.h"
#include "Model.h"
#include "character/Character.h"
#include "item/Item.h"
#include "voice/TtsPlayer.h"

struct Scene40Context;
class Scene40SpeechWindow;

extern "C" {
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
}

// Overlay 75's character, the Mii the player visits and touches, and the two objects with a model it carries. The
// names are descriptive: no name of these classes survives in the ROM.

// The goods and clothes the scene shows, two bases: the constructor stores both of this class's tables (this
// file's data, 0x02110B4C with the second at +0x2C) after both base constructors.
class Scene40Items : public InventoryItem, public ClothesItem {}; // 0x2C

// A present the character receives (0x0215efc0 sets it). The item is a member: as a base, the class gets a table
// of its own and the scene's constructor no longer matches.
class Scene40Gift {
public:
    void Set(int kind, int id);

    /* 0x00 */ Scene40Items mItem;
    /* 0x2C */ int mKind;     // 0 goods, 1 to 4 the other kinds of present
    /* 0x30 */ int mGiftKind; // What it is, as the character's states tell them apart: by the goods' category and id,
                              // or 1, 2, 3 and 5 for the kinds 1 to 4
    /* 0x34 */ int mMessage;  // The message of kinds 1 to 4 (0xF7, 0xF9, 0xF4, 0xF5)
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

// The Mii the player visits: a Character with the scene's speech and messages, two head icons, a balloon over its
// head and a cover (bed or curtain), and a present it may be given. Its behaviour is a state machine of 210 states
// (mState): UpdateState runs the group function of the state's range, each a switch over its states, as overlay 74's
// RoomCharacter does.
class Scene40Character : public Character {
public:
    // mHeadIconPlaces: where a head icon is shown
    enum HeadIconPlace {
        ICON_AT_HEAD,  // Over the neck joint, turned with the head
        ICON_AT_BODY,  // At the body joint
        ICON_AT_FEET,  // Where the character stands
        ICON_AT_HAND,  // At the right hand
    };

    void Init(int resourceId, int unk, u32 defaultMii);
    void LoadHeadIcons(ParticleSystem *particles);
    void PlaceHeadIcons();
    void PlaceHeadIconAtHead(int icon);
    void PlaceHeadIconAtBody(int icon);
    void PlaceHeadIconAtFeet(int icon);
    void PlaceHeadIconAtHand(int icon);
    void ShowMoodHeadIcon();
    void InitBalloon(int resourceId, int unk, CharacterBase *face);
    void InitCover(int kind);
    void ShowBed(VecFx32 position, u16 angle, GXRgb color);
    void SetContext(Scene40Context *context);
    void Update(BOOL speaking);
    void Draw(); // The character, then the two objects with a model each at 0x2BE4 and 0x2CDC
    void UpdateState();

    // The states, a switch over a range of states each
    void UpdateStates00To19();
    void UpdateStates1ATo1C();
    void UpdateStates1DTo27();
    void UpdateStates28To2D();
    void UpdateStates2ETo43();
    void UpdateStates44To47();
    void UpdateStates48To4E();
    void UpdateStates4FTo66();
    void UpdateStates67To85();
    void UpdateStates86ToB2();
    void UpdateStatesB3ToBC();
    void UpdateStatesBDToC5();
    void UpdateStatesC6ToD1();

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
    void BlendToAnimation(const char *name) {
        Character::BlendToAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mAnim.mUnknown04, RESOURCE_BCA0, name));
    }
    ModelAnimation *GetAnimation() { return AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId); }
    void SetTargetFloorPos(FloorPos pos) {
        mTarget.x = pos.x;
        mTarget.z = pos.z;
    }

    /* 0x2B18 */ u8 mUnknown2B18[0xBC]; // An object of the overlay's (0x0217aea8 initialises it)
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
    /* 0x2EA0 */ int mUnknown2EA0;
    /* 0x2EA4 */ int mUnknown2EA4;
    /* 0x2EA8 */ int mUnknown2EA8;
    /* 0x2EAC */ int mUnknown2EAC;
    /* 0x2EB0 */ u16 mUnknown2EB0;
    /* 0x2EB2 */ u16 mCameraAngle; // mAngle less the angle to the camera's eye (0x02173f18)
    /* 0x2EB4 */ int mUnknown2EB4;
    /* 0x2EB8 */ int mUnknown2EB8;
    /* 0x2EBC */ int mUnknown2EBC;
    /* 0x2EC0 */ int mUnknown2EC0;
    /* 0x2EC4 */ BOOL mLipSync;    // Update moves the lips with the speech only while it is set
    /* 0x2EC8 */ u8 mUnknown2EC8[0x8];
    /* 0x2ED0 */ int mUnknown2ED0[4];
    /* 0x2EE0 */ int mUnknown2EE0[3];
    /* 0x2EEC */ u8 mUnknown2EEC[0x4];
    /* 0x2EF0 */ int mCharacterCount; // Saved characters that exist
    /* 0x2EF4 */ int mFriendCount;
    /* 0x2EF8 */ int mFriends[100];   // Their indices: rank 1 and a meter over 100
    /* 0x3088 */ int mBestFriend;     // Rank 9 and a meter over 50, -1 for none
    /* 0x308C */ int mLover;          // Rank 2, -1 for none
    /* 0x3090 */ int mSpouse;         // Rank 4, -1 for none
    /* 0x3094 */ BOOL mUnknown3094;
    /* 0x3098 */ int mUnknown3098;
    /* 0x309C */ int mUnknown309C;
    /* 0x30A0 */ int mUnknown30A0;
    /* 0x30A4 */ u8 mUnknown30A4[0x44];
    /* 0x30E8 */ int mUnknown30E8;
    /* 0x30EC */ BOOL mUnknown1C4Added[3]; // Each of AddUnknown1C4(2), (5), (2) (0x0217507c, 0x021750ac, 0x021750dc) is made once a visit
}; // 0x30F8

#endif // OV075_SCENE40CHARACTER_H
