#ifndef OV074_ROOMCHARACTER_A3_H
#define OV074_ROOMCHARACTER_A3_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"

// A character standing in the apartment room (the class and most field names are groups a6's, a1's, a2's, a4's and
// a5's, see NOTES.md). This header declares what group a3's two state functions use: the states 0x40 to 0x6f, in
// which one character does something on its own, mostly with the things it holds.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

class RoomCharacter;
struct HeadIcon;
struct HeadIconFrame;

extern "C" {
// Main program, not named yet
void func_02080fc4(HeadIcon *icon);  // Removes the icon that is shown (frees mShown, clears it)
void func_02080ff0(HeadIcon *icon);  // Clears three flags of the icon that is shown: plays it again
void func_020810c8(HeadIcon *icon);  // Creates the icon of kind mKind
void HeadIcon_SetMode(HeadIcon *icon, int mode); // Sets mMode
int func_02081024(HeadIcon *icon, HeadIconFrame *frame); // frame ? frame->mValue : 0; does not use the icon
// A number below max from a generator seeded by the character (func_020985f8), not the game's generator
int func_02098e88(RoomCharacter *self, int max);
}

// A point on the floor of the room: the x and z of a VecFx32. Passed by value in two registers.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
};

// The square of a point's distance from the middle of the room
inline fx32 LengthSq(const FloorPos &pos) {
    return FX_Mul(pos.x, pos.x) + FX_Mul(pos.z, pos.z);
}

inline FloorPos operator-(FloorPos a, FloorPos b) {
    FloorPos result;
    result.x = a.x - b.x;
    result.z = a.z - b.z;
    return result;
}

// The direction of an offset on the floor, as an angle index
inline u16 Angle(FloorPos offset) {
    return FX_Atan2Idx(offset.z, offset.x);
}

// One animation of a model, as AnimModel_FindAnimation finds it by resource type and id.
struct ModelAnimation {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ int mId;
    /* 0x08 */ u32 mResType;
    /* 0x0C */ NNSG3dAnmObj *mAnmObj; // Its frame is compared with the animation's frame count
    /* 0x10 */ u32 mUnknown10;
    /* 0x14 */ int mFinished; // Set when a play-once animation reaches its end
    /* 0x18 */ int mPlayOnce;
};

// The model of a character with its animations and joints. Main-program class, not decompiled; only what this file
// uses. The joint arrays are group a5's finding (CharacterModel_GetJointPosition(model, joint) returns mJointMatrices[joint]'s
// translation).
struct CharacterModel {
    MtxFx33 *GetJointRotation(int i) { return &mJointRotations[i]; }
    MtxFx43 *GetJointMatrix(int i) { return &mJointMatrices[i]; }

    /* 0x000 */ u8 mUnknown000[0xE0];
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x18];
    /* 0x104 */ int mAnimId;     // Current 'BCA0' animation
    /* 0x108 */ int mPrevAnimId;
    /* 0x10C */ int mBlendLeft;  // > 0 while the change to the current animation is still running
    /* 0x110 */ u8 mUnknown110[0x4];
    /* 0x114 */ MtxFx33 mJointRotations[10];
    /* 0x27C */ MtxFx43 mJointMatrices[12]; // Joints 1 and 2 are the two hands
}; // 0x4BC

// What func_02081024 reads of the icon that is shown.
struct HeadIconFrame {
    /* 0x00 */ u8 mUnknown00[0x26];
    /* 0x26 */ u16 mValue; // The states 0x5f, 0x69 and 0x6a play a sound in each frame in which it is 8
};
struct HeadIconShown {
    /* 0x00 */ u8 mUnknown00[0x8];
    /* 0x08 */ HeadIconFrame *mFrame;
};

// The icon shown over a character's head (name is group a6's). Main-program class (func_02080fc4...), not
// decompiled.
struct HeadIcon {
    /* 0x00 */ void *mUnknown00;
    /* 0x04 */ HeadIconShown *mShown; // The icon that is shown, NULL when none
    /* 0x08 */ int mMode;
    /* 0x0C */ u8 mUnknown0C[0x30];
    /* 0x3C */ u16 mAngle;
    /* 0x3E */ s16 mKind;
}; // 0x40

// One of the two things a character holds in its hands (name is group a6's, the class is group a7's; only what
// this file uses).
struct HeldItem {
    void SetPos(VecFx32 pos) {
        mPos.x = pos.x;
        mPos.y = pos.y;
        mPos.z = pos.z;
    }
    void SetPos(fx32 x, fx32 y, fx32 z) {
        mPos.x = x;
        mPos.y = y;
        mPos.z = z;
    }
    void SetScale(VecFx32 scale) {
        mScale.x = scale.x;
        mScale.y = scale.y;
        mScale.z = scale.z;
    }
    void SetAngles(u16 x, u16 y, u16 z) {
        mAngleX = x;
        mAngleY = y;
        mAngleZ = z;
    }

    /* 0x00 */ u8 mUnknown00[0x68];
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ VecFx32 mScale; // The states 0x69 to 0x6d give it the result of func_0208fc34(model), three factors
    /* 0x98 */ VecFx32 mPos;
    /* 0xA4 */ u16 mAngleX;
    /* 0xA6 */ u16 mAngleY;
    /* 0xA8 */ u16 mAngleZ;
};

// The object a character looks at across the room, with a place (name is group a6's).
struct RoomTarget {
    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mPos.x;
        pos.z = mPos.z;
        return pos;
    }

    /* 0x00 */ u8 mUnknown00[0x10];
    /* 0x10 */ VecFx32 mPos;
};

extern "C" {
extern MATHRandContext16 gRandom; // The game's random number generator
extern const int data_020fba28[];       // By the character's 0x2bc: the row of the two tables below
extern const s16 data_020fb870[][5];    // Animation ids, by that row and by mMood (a2: standing)
extern const s16 data_020fb898[][5];    // The same for a second kind of animation (a2: walking)
}

class RoomCharacter {
public:
    // Group a1's
    void PlaceHeadIcon();      // 0x0215232c
    void SetState(int state);  // 0x02152370
    void ShowHeadIcon();       // 0x02152464
    // Group a3's: called by a1's Update for the states 0x40..0x67 and 0x68..0x6f
    void UpdateSoloStates40To67();
    void UpdateSoloStates68To6F();
    // Group a6's
    BOOL StepWalkToRandomPos();                                     // 0x02167840
    void StepWalkTo(FloorPos pos);                                  // 0x02167930
    BOOL StepAlternateAnimations(int animA, int animB);             // 0x02167bb4
    BOOL StepChangeItemsThenAnimation(int anim, u8 itemA, u8 itemB); // 0x02168320
    BOOL StepChangeHeldItems(u8 itemA, u8 itemB);                   // 0x021684ac
    int GetFace();                                                  // 0x02168720
    void GetMidpoint(MtxFx43 *a, MtxFx43 *b, VecFx32 *out);         // 0x02168734
    void SetAnimation(u32 resType, int id);                         // 0x02168774
    BOOL IsAnimOfGroupB(int anim);                                  // 0x02168804
    void GetRandomPosInArea(FloorPos *out);                         // 0x0216898c
    BOOL ClampToRect4E4(FloorPos *pos);                             // 0x02168b38
    void GetRandomCorner(FloorPos *out);                            // 0x02168be0
    void GetRandomEdgePos(FloorPos *out);                           // 0x02168c28

    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mModel.mPos.x;
        pos.z = mModel.mPos.z;
        return pos;
    }
    FloorPos GetTargetFloorPos() {
        FloorPos pos;
        pos.x = mTarget.x;
        pos.z = mTarget.z;
        return pos;
    }
    // From where the character stands to a point (the main program's Character_TurnToWalkTarget has the same code inline)
    FloorPos GetOffsetTo(FloorPos to) { return to - GetFloorPos(); }
    void SetTarget(fx32 x, fx32 z) {
        mTarget.x = x;
        mTarget.y = 0;
        mTarget.z = z;
    }
    s16 GetMoodAnimA() { return data_020fb870[data_020fba28[mUnknownAnimRowKey]][mMood]; }
    s16 GetMoodAnimB() { return data_020fb898[data_020fba28[mUnknownAnimRowKey]][mMood]; }
    ModelAnimation *GetCurrentAnimation();
    BOOL IsAnimationFinished();

    // Shows the head icon of a kind, from its start when that kind is already there
    void ShowHeadIconOfKind(int kind) {
        if (mHeadIcon.mKind != kind) {
            func_02080fc4(&mHeadIcon);
            mHeadIcon.mKind = kind;
            func_020810c8(&mHeadIcon);
        } else {
            func_02080ff0(&mHeadIcon);
        }
    }
    // Kind 9, or kind 3 for the characters whose record has mUnknownPicksHeadIcon set
    void ShowHeadIconOfRecord() {
        if (mUnknownPicksHeadIcon == 0) {
            ShowHeadIconOfKind(9);
        } else {
            ShowHeadIconOfKind(3);
        }
    }
    // Plays one of the four sounds 10 to 13 of sequence archive 0x22, picked by the character's own generator
    void PlayRandomSoundOfFour() {
        int pick = func_02098e88(this, 4);
        if (pick == 0) {
            gSoundManager.StartSeqArc(1, 0x22, 10);
        } else if (pick == 1) {
            gSoundManager.StartSeqArc(1, 0x22, 11);
        } else if (pick == 2) {
            gSoundManager.StartSeqArc(1, 0x22, 12);
        } else {
            gSoundManager.StartSeqArc(1, 0x22, 13);
        }
    }

    /* 0x0000 */ u8 mUnknown0000[0x246];
    /* 0x0246 */ u8 mUnknownPicksHeadIcon; // Of the saved record. Only known use: ShowHeadIconOfRecord
    /* 0x0247 */ u8 mUnknown0247[0x4E];
    /* 0x0295 */ u8 mMood; // 0..4 (name is group a2's): column of the animation tables
    /* 0x0296 */ u8 mUnknown0296[0x26];
    /* 0x02BC */ s8 mUnknownAnimRowKey; // Index into data_020fba28, which gives the row of the animation tables
    /* 0x02BD */ u8 mUnknown02BD[0x2243];
    /* 0x2500 */ int mFace; // Set by Character_SetFace (name is group a2's)
    /* 0x2504 */ u8 mUnknown2504[0xC];
    /* 0x2510 */ BOOL mHeadTurnActive; // 0x021691d4 sets it with the head angles (name is group a2's)
    /* 0x2514 */ u8 mUnknown2514[0xC2];
    /* 0x25D6 */ u16 mHeadYawNow; // The yaw the head has: the head icon turns by it (0x0215232c), and a held item
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u16 mHeadPitch; // The three angles 0x021691d4 sets (names are group a2's)
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknown25E3;
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AA0 */ u8 mUnknown2AA0[0x58];
    /* 0x2AF8 */ VecFx32 mTarget; // Where it walks to
    /* 0x2B04 */ u8 mUnknown2B04[6];
    /* 0x2B0A */ u16 mAngle; // The way the character faces
    /* 0x2B0C */ u8 mUnknown2B0C[0xC];
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ u8 mUnknown2B20[0x20];
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ HeldItem *mHeldItems[2];
    /* 0x2B88 */ void *mUnknown2B88;
    /* 0x2B8C */ RoomTarget *mLookTarget;
    /* 0x2B90 */ u8 mUnknown2B90[0x2C];
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ BOOL mKeepSound; // SetState stops the sound of mSoundHandle unless this is set, and clears it
    /* 0x2BC4 */ u8 mUnknown2BC4[0x1A];
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ u16 mUnknown2BE2;
    /* 0x2BE4 */ u16 mNextState; // In state 0x4f: the state chosen, entered when the animation before it is over
    /* 0x2BE6 */ s16 mTimer2;    // A second countdown (name is group a2's)
    /* 0x2BE8 */ s16 mTimer3;
};

#endif // OV074_ROOMCHARACTER_A3_H
