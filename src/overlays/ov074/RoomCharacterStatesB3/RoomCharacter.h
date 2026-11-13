#ifndef OV074_ROOMCHARACTER_A5_H
#define OV074_ROOMCHARACTER_A5_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

// A character standing in the apartment room: the room's resident, or the visitor (the class name is group a6's,
// see NOTES.md). This header declares what the three state functions of group a5 (states 0xb3 to 0xf1) use; the
// offsets are the evidence, the rest of the object is padding.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// A point on the floor of the room: the x and z of a VecFx32. Passed by value in two registers (0x021691fc,
// 0x02167930, main func_0209171c and func_02091598).
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;

    fx32 LengthSq() const { return FX_Mul(x, x) + FX_Mul(z, z); }
    fx32 Dot(const FloorPos &other) const { return FX_Mul(x, other.x) + FX_Mul(z, other.z); }
};

static inline FloorPos FloorPos_Subtract(FloorPos a, FloorPos b) {
    FloorPos result;
    result.x = a.x - b.x;
    result.z = a.z - b.z;
    return result;
}

// The resource of one animation, as far as its length is read here.
struct ModelAnimResource {
    /* 0x00 */ u32 mUnknownAt0;
    /* 0x04 */ u16 mFrameCount;
};

struct ModelAnimFrame {
    /* 0x00 */ fx32 mFrame; // Current frame
    /* 0x04 */ int mUnknownAt4;
    /* 0x08 */ ModelAnimResource *mResource;
};

// One entry of the model's animation table, as AnimModel_FindAnimation returns it (names are group a2's).
struct ModelAnim {
    /* 0x00 */ u8 mUnknownAt0[0xC];
    /* 0x0C */ ModelAnimFrame *mFrame;
    /* 0x10 */ fx32 mSpeed;
    /* 0x14 */ BOOL mFinished; // Set when a play-once animation has reached its end
    /* 0x18 */ BOOL mPlayOnce;
};

// The model of a character and its animations, at RoomCharacter+0x25e4. Main-program class, not decompiled.
struct CharacterModel {
    MtxFx33 *GetJointRotation(int joint) { return &mJointRotations[joint]; }
    MtxFx43 *GetJointMatrix(int joint) { return &mJointMatrices[joint]; }

    /* 0x000 */ u8 mUnknownAt0[0xE0];
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknownAtEC[0x18];
    /* 0x104 */ int mAnimId;      // The current 'BCA0' animation
    /* 0x108 */ int mPrevAnimId;
    /* 0x10C */ int mBlendFrames; // > 0 while the change to the current animation is still running
    /* 0x110 */ int mUnknownAt110;
    /* 0x114 */ MtxFx33 mJointRotations[10]; // By joint; the held items take their rotation from these
    /* 0x27C */ MtxFx43 mJointMatrices[12];  // By joint; CharacterModel_GetJointPosition returns the position (last row) of one
    /* 0x4BC */ u8 mUnknownAt4BC[0x4C];
};

// What a character holds in a hand; RoomCharacter has two (0x2b80, 0x2b84). Group a7's class.
struct HeldItem {
    /* 0x00 */ u8 mUnknownAt0[0xA4];
    /* 0xA4 */ u16 mRotX;
    /* 0xA6 */ u16 mRotY;
    /* 0xA8 */ u16 mRotZ;
};

// The mark shown over a character's head, at RoomCharacter+0x2b40. Main-program class, not decompiled.
struct HeadIcon {
    /* 0x00 */ u8 mUnknownAt0[0x8];
    /* 0x08 */ int mMode; // HeadIcon_SetMode sets it
    /* 0x0C */ u8 mUnknownAtC[0x30];
    /* 0x3C */ s16 mAngle;
    /* 0x3E */ s16 mIconId;
};

// The thing in the room that a character can look at (name is group a6's), with its place at 0x10.
struct RoomTarget {
    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mPos.x;
        pos.z = mPos.z;
        return pos;
    }

    /* 0x00 */ u8 mUnknownAt0[0x10];
    /* 0x10 */ VecFx32 mPos;
};

class RoomCharacter {
public:
    void UpdateStatesB3ToC9();
    void UpdateStatesCAToE0();
    void UpdateStatesE1ToF1();

    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mModel.mPos.x;
        pos.z = mModel.mPos.z;
        return pos;
    }
    FloorPos GetWalkTargetFloorPos() {
        FloorPos pos;
        pos.x = mWalkTarget.x;
        pos.z = mWalkTarget.z;
        return pos;
    }
    void SetWalkTarget(FloorPos pos) {
        mWalkTarget.x = pos.x;
        mWalkTarget.z = pos.z;
    }
    // The angle of the way from where the character stands to a point
    u16 AngleTo(FloorPos target) {
        FloorPos delta = FloorPos_Subtract(target, GetFloorPos());
        return FX_Atan2Idx(delta.z, delta.x);
    }
    BOOL IsWithinReach(FloorPos pos, fx32 reach, fx32 maxReach);

    /* 0x0000 */ u8 mUnknownAt0[0x224];
    /* 0x0224 */ u8 mEvent; // What the character does in the room (name is group a1's)
    /* 0x0225 */ u8 mUnknownAt225[0x70];
    /* 0x0295 */ u8 mMood; // 0..4: column of the animation tables (name is group a2's)
    /* 0x0296 */ u8 mUnknownAt296[0xE];
    /* 0x02A4 */ u16 mIndex; // Which saved character this is (name is group a1's)
    /* 0x02A6 */ s16 mReaction; // Name is group a1's; cleared here when the two are done
    /* 0x02A8 */ u8 mUnknownAt2A8[0x14];
    /* 0x02BC */ s8 mPersonality; // 0..15: data_020fba28 turns it into the row (0..3) of the animation tables
    /* 0x02BD */ u8 mUnknownAt2BD[0x2243];
    /* 0x2500 */ int mFace; // Set by Character_SetFace
    /* 0x2504 */ u8 mUnknownAt2504[0xC];
    /* 0x2510 */ BOOL mHeadTurnActive;
    /* 0x2514 */ u8 mUnknownAt2514[0xC2];
    /* 0x25D6 */ u16 mUnknownAngleAt25D6; // Given to the held item as its x rotation
    /* 0x25D8 */ u16 mUnknownAt25D8;
    /* 0x25DA */ u16 mHeadPitch;
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknownAt25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknownAt25E3;
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknownAt2AF0;
    /* 0x2AF4 */ fx32 mUnknownAt2AF4;
    /* 0x2AF8 */ VecFx32 mWalkTarget;
    /* 0x2B04 */ int mUnknownAt2B04;
    /* 0x2B08 */ u16 mMoveAngle;
    /* 0x2B0A */ u16 mAngle;       // The way the character faces
    /* 0x2B0C */ u16 mTargetAngle; // Character_TurnToTargetAngle turns mAngle towards it
    /* 0x2B0E */ u16 mUnknownAngleAt2B0E;
    /* 0x2B10 */ s8 mSoundPhase[4]; // Character_PlaySoundAtAnimPhase's progress through an animation, per slot
    /* 0x2B14 */ fx32 mAnimSpeed;   // func_020919e8 gives it to the model when an animation starts
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ int mOverrideState;
    /* 0x2B24 */ int mSlot; // 0 or 1: which of the scene's two characters this is. 0 decides for both of them.
    /* 0x2B28 */ int mWait; // Frames until the scene chooses again (name is group a1's)
    /* 0x2B2C */ BOOL mPresent;
    /* 0x2B30 */ RoomCharacter *mPartner;
    /* 0x2B34 */ VecFx32 mSavedWalkTarget;
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ HeldItem *mHeldItem[2];
    /* 0x2B88 */ void *mUnknownAt2B88;
    /* 0x2B8C */ RoomTarget *mLookTarget;
    /* 0x2B90 */ TtsPlayer *mTtsPlayer;
    /* 0x2B94 */ u8 mUnknownAt2B94[0x28];
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ BOOL mKeepSound; // SetState leaves the sound playing once when this is set
    /* 0x2BC4 */ u8 mUnknownAt2BC4[0x1A];
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ u16 mUnknownAt2BE2;
    /* 0x2BE4 */ u16 mCounter; // Cleared by SetState. Here: whose turn it is and how far (100 and up: the partner)
    /* 0x2BE6 */ s16 mTimer2;
    /* 0x2BE8 */ s16 mTimer3;
    /* 0x2BEA */ u8 mUnknownAt2BEA[0x1AC];
    /* 0x2D96 */ s8 mSubStep;
    /* 0x2D97 */ s8 mSubSubStep;
};

extern "C" BOOL Character_IsPointWithinReach(RoomCharacter *self, fx32 x, fx32 z, fx32 reach, fx32 maxReach);

// Whether a point is within reach (scaled by the model's size, at most maxReach): main Character_IsPointWithinReach
inline BOOL RoomCharacter::IsWithinReach(FloorPos pos, fx32 reach, fx32 maxReach) {
    return Character_IsPointWithinReach(this, pos.x, pos.z, reach, maxReach);
}

#endif // OV074_ROOMCHARACTER_A5_H
