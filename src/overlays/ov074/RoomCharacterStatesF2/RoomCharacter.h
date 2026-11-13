#ifndef OV074_ROOMCHARACTER_H
#define OV074_ROOMCHARACTER_H

#include <nitro.h>
#include <nnsys.h>

class TtsPlayer;

// A point on the floor of the room: the x and z of a VecFx32.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
};

// A rectangle on the floor.
struct FloorRect {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
    /* 0x8 */ fx32 w;
    /* 0xC */ fx32 h;
};

// One animation of a model, as AnimModel_FindAnimation finds it by resource type and id.
struct ModelAnimation {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ int mId;
    /* 0x08 */ u32 mResType;
    /* 0x0C */ NNSG3dAnmObj *mAnmObj; // Character_PlaySoundAtAnimPhase compares its frame with the animation's frame count
    /* 0x10 */ u32 mUnknown10;
    /* 0x14 */ int mFinished; // Set when a play-once animation reaches its end (Character_PlaySoundAtAnimPhase does nothing once set)
    /* 0x18 */ int mPlayOnce;
};

// The model of a character with its animations. Main-program class, not decompiled; only what this file uses.
struct CharacterModel {
    /* 0x000 */ u8 mUnknown000[0xE0];
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x8];
    /* 0x0F4 */ fx32 mScale; // Character_IsPointWithinReach scales a reach distance by it
    /* 0x0F8 */ u8 mUnknown0F8[0xC];
    /* 0x104 */ int mAnimId;     // Current 'BCA0' animation
    /* 0x108 */ int mPrevAnimId; // The one being blended out of
    /* 0x10C */ int mBlendFrames;  // func_0208f050 sets it to 0xC000 on a change of animation; states wait for <= 0
    /* 0x110 */ u8 mUnknown110[0x4];
};

// Something with a position at 0x24 (a node of a model). Only what this file uses.
struct PositionedNode {
    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ VecFx32 mPos;
};

// The icon shown over a character's head. Main-program class (func_02080fc4...), not decompiled.
struct HeadIcon {
    /* 0x00 */ void *mUnknown00;
    /* 0x04 */ void *mUnknown04;
    /* 0x08 */ int mMode;
    /* 0x0C */ u8 mUnknown0C[0x30];
    /* 0x3C */ u16 mAngle;
    /* 0x3E */ s16 mIconId;
};

// One of the two things a character holds in its hands (group a7's class; only what this file uses).
struct HeldItem {
    void SetAngles(s16 x, s16 y, s16 z) {
        mAngleX = x;
        mAngleY = y;
        mAngleZ = z;
    }


    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ NNSG3dResMdl *mModel;
    /* 0x10 */ u8 mUnknown10[0x58];
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ u8 mUnknown8C[0x18];
    /* 0xA4 */ s16 mAngleX;
    /* 0xA6 */ s16 mAngleY;
    /* 0xA8 */ s16 mAngleZ;
    /* 0xAA */ u8 mUnknownAA[0x6];
    /* 0xB0 */ int mVisible;
    /* 0xB4 */ u8 mUnknownB4[0x30];
    /* 0xE4 */ int mUnknownE4;
    /* 0xE8 */ int mBusy;
};

// The object a character looks at across the room, with a place and a heading.
struct RoomWalkTarget {
    /* 0x00 */ u8 mUnknown00[0x10];
    /* 0x10 */ fx32 mX;
    /* 0x14 */ fx32 mY;
    /* 0x18 */ fx32 mZ;
    /* 0x1C */ u8 mUnknown1C[0xC];
    /* 0x28 */ int mAngle;
};

// The room's door: func_020a4144 sets the motion at 0x14+0x184; 0x14+0x184 is 0 again when it is done.
struct RoomDoorMotion {
    /* 0x000 */ u8 mUnknown000[0x184];
    /* 0x184 */ int mMotion;
};
struct RoomDoor {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ RoomDoorMotion mMotion;
};

extern MATHRandContext16 gRandom; // gRandom
extern const s16 data_020fb870[4][5];
extern const s16 data_020fb898[4][5];
extern const int data_020fba28[16];
extern FloorRect gWalkArea; // data_ov074_0216d724
extern FloorPos gWalkAreaCorners[4]; // data_ov074_0216d734
extern FloorPos gRoomPoints[]; // data_ov074_0216d404
extern const VecFx32 data_ov074_0216d490;
extern const VecFx32 data_ov074_0216d4a8;
extern const VecFx32 data_ov074_0216d4c0;
extern const VecFx32 data_ov074_0216d4cc;
extern const VecFx32 data_ov074_0216d4d8;
extern const FloorRect data_ov074_0216d4e4;
extern const FloorRect data_ov074_0216d534;
extern const int gMoodFaces[5]; // data_ov074_0216d544

// A character standing in the apartment room: the room's resident, or the visitor.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class RoomCharacter {
public:
    void UpdateStatesF2To103();
    void UpdateStates104To105();
    void UpdateStates106To10B();
    void UpdateStates10CTo112();
    void UpdateStates113To116();
    void UpdateState117();

    int StepWalkToRandomPos();
    int StepWalkTo(FloorPos pos);
    int StepTurnWithAnimation(int anim);
    int StepAlternateAnimations(int animA, int animB);
    int StepAnimationFacingPartner(int anim);
    int StepIdleThenAnimation55();
    int StepRandomAnimationFacingPartner();
    int StepAnimationThenFacePartner(int anim);
    void StartAnimationWithIcon(int anim);
    int StepChangeItemsThenAnimation(int anim, int itemA, int itemB);
    int StepChangeHeldItems(int itemA, int itemB);
    void UpdateBodyIfPresent();
    int PickReaction();
    void ResetBody();
    u8 GetFlagOfPartner();
    int GetFaceOfMood(int variant);
    int GetFace();
    void GetMidpoint(PositionedNode *a, PositionedNode *b, VecFx32 *out);
    void SetAnimation(u32 resType, int id);
    BOOL IsOneTwoOrFour(int value);
    BOOL IsState8Or9();
    BOOL IsState10To12();
    BOOL IsAnimOfGroupB(int anim);
    BOOL IsWalkAnim();
    BOOL IsMovingAnim();
    BOOL IsAnimOfGroupA(int anim);
    void GetRandomPosInArea(FloorPos *out);
    void GetRandomPos(FloorPos *out);
    BOOL ClampToRect(FloorPos *pos, const FloorRect *rect);
    BOOL ClampToWalkArea(FloorPos *pos);
    BOOL ClampToRect4E4(FloorPos *pos);
    BOOL ClampToRect534(FloorPos *pos);
    void GetRandomCorner(FloorPos *out);
    void GetRandomEdgePos(FloorPos *out);

    FloorPos GetFloorPos() {
        FloorPos p;
        p.x = mModel.mPos.x;
        p.z = mModel.mPos.z;
        return p;
    }

    FloorPos GetWalkTargetOnFloor() {
        FloorPos p;
        p.x = mWalkTarget.x;
        p.z = mWalkTarget.z;
        return p;
    }
    VecFx32 GetWalkTarget() { return mWalkTarget; }
    void SetPos(VecFx32 pos) { mModel.mPos = pos; }
    u8 PartnerFlag() { return mPerResidentFlag[mPartner->mIndex]; }
    BOOL IsWithinReach(FloorPos pos, fx32 reach, fx32 maxReach);
    void SetWalkTarget(fx32 x, fx32 y, fx32 z) {
        mWalkTarget.x = x;
        mWalkTarget.y = y;
        mWalkTarget.z = z;
    }
    void SetWalkTargetOnFloor(FloorPos pos) {
        mWalkTarget.x = pos.x;
        mWalkTarget.z = pos.z;
    }
    s16 GetIdleAnim() { return data_020fb870[data_020fba28[mBodyType]][mMood]; }
    s16 GetWalkAnim() { return data_020fb898[data_020fba28[mBodyType]][mMood]; }
    void TurnToLookTarget(u8 speed);
    void SetHeadIcon(s16 id);

    /* 0x0000 */ u8 mUnknown0000[0x160];
    /* 0x0160 */ u8 mPerResidentFlag[0xC4];
    /* 0x0224 */ u8 mEvent;
    /* 0x0225 */ u8 mUnknown0225[0x70];
    /* 0x0295 */ u8 mMood;
    /* 0x0296 */ u8 mUnknown0296[0xE];
    /* 0x02A4 */ u16 mIndex;
    /* 0x02A6 */ s16 mReaction;
    /* 0x02A8 */ u8 mUnknown02A8[0x14];
    /* 0x02BC */ s8 mBodyType;
    /* 0x02BD */ u8 mUnknown02BD[0x2243];
    /* 0x2500 */ int mFace;
    /* 0x2504 */ u8 mUnknown2504[0xC];
    /* 0x2510 */ int mHeadTurnActive;
    /* 0x2514 */ u8 mUnknown2514[0xC0];
    /* 0x25D4 */ u16 mUnknown25D4;
    /* 0x25D6 */ u16 mUnknown25D6;
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u16 mHeadPitch;
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknown25E3;
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x26F8 */ u8 mUnknown26F8[0x168];
    /* 0x2860 */ PositionedNode mNode2860;
    /* 0x2890 */ u8 mUnknown2890[0x25C];
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mWalkTarget;
    /* 0x2B04 */ int mUnknown2B04;
    /* 0x2B08 */ u16 mMoveAngle;
    /* 0x2B0A */ u16 mAngle;
    /* 0x2B0C */ u16 mTargetAngle;
    /* 0x2B0E */ u16 mUnknown2B0E;
    /* 0x2B10 */ s8 mSoundPhase[4];
    /* 0x2B14 */ int mUnknown2B14;
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ int mUnknown2B20;
    /* 0x2B24 */ int mSlot;
    /* 0x2B28 */ int mUnknown2B28;
    /* 0x2B2C */ BOOL mPresent;
    /* 0x2B30 */ RoomCharacter *mPartner;
    /* 0x2B34 */ VecFx32 mSavedWalkTarget;
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ HeldItem *mHeldItems[2];
    /* 0x2B88 */ RoomDoor *mDoor;
    /* 0x2B8C */ RoomWalkTarget *mLookTarget;
    /* 0x2B90 */ TtsPlayer *mTtsPlayer;
    /* 0x2B94 */ u8 mUnknown2B94[0x30];
    /* 0x2BC4 */ int mStepSoundArmed;
    /* 0x2BC8 */ u8 mUnknown2BC8[0x16];
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ u16 mUnknown2BE2;
    /* 0x2BE4 */ u16 mRepeatCount;
    /* 0x2BE6 */ u8 mUnknown2BE6[4];
    /* 0x2BEA */ s16 mRepeatTimer;
    /* 0x2BEC */ u8 mUnknown2BEC[0x1AA];
    /* 0x2D96 */ s8 mSubStep;
    /* 0x2D97 */ s8 mSubSubStep;
};

extern "C" {
void func_ov074_021691d4(RoomCharacter *self, u16 a, u16 b, u16 c, u8 speed);
u16 func_ov074_021691fc(RoomCharacter *self, FloorPos pos, fx32 limit);
BOOL Character_IsPointWithinReach(RoomCharacter *self, fx32 x, fx32 z, fx32 reach, fx32 maxReach);
void func_02080fc4(HeadIcon *icon);
void func_020810c8(HeadIcon *icon);
void HeadIcon_SetMode(HeadIcon *icon, int mode);
}

inline void RoomCharacter::TurnToLookTarget(u8 speed) {
    FloorPos pos;
    pos.x = mLookTarget->mX;
    pos.z = mLookTarget->mZ;
    u16 angle = mLookTarget->mAngle >> 12;
    func_ov074_021691d4(this, -(angle >> 2), func_ov074_021691fc(this, pos, 0x3800), 0, speed);
}

inline BOOL RoomCharacter::IsWithinReach(FloorPos pos, fx32 reach, fx32 maxReach) {
    return Character_IsPointWithinReach(this, pos.x, pos.z, reach, maxReach);
}

inline void RoomCharacter::SetHeadIcon(s16 id) {
    func_02080fc4(&mHeadIcon);
    mHeadIcon.mIconId = id;
    func_020810c8(&mHeadIcon);
    HeadIcon_SetMode(&mHeadIcon, 3);
}

#endif // OV074_ROOMCHARACTER_H
