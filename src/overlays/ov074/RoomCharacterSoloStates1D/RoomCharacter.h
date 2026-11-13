#ifndef OV074_ROOMCHARACTER_A4_H
#define OV074_ROOMCHARACTER_A4_H

#include <nitro.h>
#include <nnsys.h>

// A character standing in the apartment room (the class and most field names are groups a6's, a1's and a2's, see
// NOTES.md). This header declares what group a4's state functions use; this copy is for 0x02154608 (states 0x1d
// to 0x3f) and extends the one in a4.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// A point on the floor of the room: the x and z of a VecFx32. Passed by value in two registers.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
};

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

// The model of a character with its animations. Main-program class, not decompiled; only what this file uses.
struct CharacterModel {
    /* 0x000 */ u8 mUnknown000[0xD8];
    /* 0x0D8 */ fx32 mUnknownScaleAtD8; // 0x1000 minus this lengthens the timers of the states 0x35 and 0x36 (name is a2's)
    /* 0x0DC */ int mUnknown0DC;
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x18];
    /* 0x104 */ int mAnimId;     // Current 'BCA0' animation
    /* 0x108 */ int mPrevAnimId;
    /* 0x10C */ int mBlendLeft;  // > 0 while the change to the current animation is still running
    /* 0x110 */ u8 mUnknown110[0x4];
};

// A node of the model with a position at 0x24 (name is group a6's).
struct PositionedNode {
    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ VecFx32 mPos;
}; // 0x30

// A pair of nodes: the character has six of these 0x60-byte entries at 0x2860. The play states take the point
// between the first node of entry 1 and the second node of entry 0 as the place of the thing held in both hands.
struct PositionedNodePair {
    /* 0x00 */ PositionedNode mFirst;
    /* 0x30 */ PositionedNode mSecond;
}; // 0x60

// An entry of the 0x48-byte table at 0x26f8; 0x02169f48 is given entry 1 with node 2 to place held item 0.
struct UnknownNode48 {
    u8 mUnknown00[0x48];
};

// One of the two things a character holds in its hands (name is group a6's; only what this file uses).
struct HeldItem {
    /* 0x00 */ u8 mUnknown00[0x68];
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ u8 mUnknown8C[0xC];
    /* 0x98 */ VecFx32 mPos;
    /* 0xA4 */ u16 mAngleX;
    /* 0xA6 */ u16 mAngleY;
    /* 0xA8 */ u16 mAngleZ;

    FloorPos GetFloorPos() {
        FloorPos p;
        p.x = mPos.x;
        p.z = mPos.z;
        return p;
    }
    void SetAngles(u16 x, u16 y, u16 z) {
        mAngleX = x;
        mAngleY = y;
        mAngleZ = z;
    }
    void SetPos(VecFx32 pos) {
        mPos.x = pos.x;
        mPos.y = pos.y;
        mPos.z = pos.z;
    }
};

extern "C" {
extern MATHRandContext16 gRandom; // The game's random number generator (group a6 calls it gRandom)
extern const int data_020fba28[];       // By the character's 0x2bc: the row of the two tables below
extern const s16 data_020fb870[][5];    // Animation ids, by that row and by mMood
extern const s16 data_020fb898[][5];    // The same for a second kind of animation
}

class RoomCharacter {
public:
    // Group a1's
    void PlaceHeadIcon();                                // 0x0215232c
    void SetState(int state);                            // 0x02152370
    void SetStateWithPartner(int state, int partnerState); // 0x02152418
    void ShowHeadIcon();                                 // 0x02152464
    // Group a4's: called by a1's Update for the states 0x70..0x92 and 0x93..0xb2
    void UpdateSoloStates1DTo3F(); // 0x02154608
    void UpdatePairStates70To92();
    void UpdatePairStates93ToB2();
    // Group a6's
    int WalkTo(FloorPos pos);                                              // 0x02167930
    int ChangeHeldItems(int itemA, int itemB);                             // 0x021684ac
    int GetExpressionOfVariant(int variant);                               // 0x02168710
    int GetExpression();                                                   // 0x02168720
    void GetMidpoint(PositionedNode *a, PositionedNode *b, VecFx32 *out);  // 0x02168734
    void SetAnimation(u32 resType, int id);                                // 0x02168774
    void GetRandomPosInArea(FloorPos *out);                                // 0x0216898c
    BOOL ClampToWalkArea(FloorPos *pos);                                   // 0x02168ae4
    BOOL ClampToRect4E4(FloorPos *pos);                                    // 0x02168b38

    FloorPos GetFloorPos() {
        FloorPos p;
        p.x = mModel.mPos.x;
        p.z = mModel.mPos.z;
        return p;
    }
    FloorPos GetTargetFloorPos() {
        FloorPos p;
        p.x = mTarget.x;
        p.z = mTarget.z;
        return p;
    }
    void SetTargetFloorPos(FloorPos pos) {
        mTarget.x = pos.x;
        mTarget.z = pos.z;
    }
    void SetTargetXZ(fx32 x, fx32 z) {
        mTarget.x = x;
        mTarget.z = z;
    }
    void SetTarget(fx32 x, fx32 y, fx32 z) {
        mTarget.x = x;
        mTarget.y = y;
        mTarget.z = z;
    }
    s16 GetVariantAnim870() { return data_020fb870[data_020fba28[mUnknownAnimRowKey]][mMood]; }
    s16 GetVariantAnim898() { return data_020fb898[data_020fba28[mUnknownAnimRowKey]][mMood]; }
    PositionedNode *GetNode(int i) { return &mNodes[i]; }
    UnknownNode48 *GetNode48(int i) { return &mNodes48[i]; }
    ModelAnimation *GetCurrentAnimation();

    /* 0x0000 */ u8 mUnknown0000[0x295];
    /* 0x0295 */ u8 mMood; // 0..4 (name is group a2's): column of the animation tables
    /* 0x0296 */ u8 mUnknown0296[0x26];
    /* 0x02BC */ s8 mUnknownAnimRowKey; // Index into data_020fba28, which gives the row of the animation tables
    /* 0x02BD */ u8 mUnknown02BD[0x2243];
    /* 0x2500 */ int mFace; // Set by Character_SetFace (name is group a2's)
    /* 0x2504 */ u8 mUnknown2504[0xC];
    /* 0x2510 */ BOOL mHeadTurnActive; // 0x021691d4 sets it with the head angles (name is a2's)
    /* 0x2514 */ u8 mUnknown2514[0xC2];
    /* 0x25D6 */ u16 mUnknownAngle25D6; // Given back to 0x021691d4 as the head's yaw; also added to the head icon's angle
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u16 mHeadPitch; // The three angles 0x021691d4 sets (names are group a2's)
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknown25E3;
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x26F8 */ UnknownNode48 mNodes48[5];
    /* 0x2860 */ PositionedNode mNodes[12];
    /* 0x2AA0 */ u8 mUnknown2AA0[0x58];
    /* 0x2AF8 */ VecFx32 mTarget; // Where it walks to
    /* 0x2B04 */ u8 mUnknown2B04[6];
    /* 0x2B0A */ u16 mAngle;       // The way the character faces
    /* 0x2B0C */ u16 mTargetAngle; // The way it turns to (name is group a2's)
    /* 0x2B0E */ u16 mUnknownAngle2B0E; // Its negative is the head's yaw while turning to mTargetAngle (name is a2's)
    /* 0x2B10 */ u8 mUnknown2B10[8];
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ int mUnknown2B20;
    /* 0x2B24 */ int mSlot; // 0 or 1: which of the scene's two characters this is. Slot 0 decides for both
    /* 0x2B28 */ u8 mUnknown2B28[8];
    /* 0x2B30 */ RoomCharacter *mPartner;
    /* 0x2B34 */ u8 mUnknown2B34[0x4C];
    /* 0x2B80 */ HeldItem *mHeldItems[2];
    /* 0x2B88 */ u8 mUnknown2B88[0x34];
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ u8 mUnknown2BC0[0x1E];
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ u16 mUnknown2BE2;
    /* 0x2BE4 */ u16 mStateCount; // Cleared by SetState. Here: animation loops counted, or the state chosen for slot 0
    /* 0x2BE6 */ s16 mTimer2;     // A second countdown (name is group a2's); in the states 0x76..0x7a an animation id
    /* 0x2BE8 */ s16 mTimer3;     // In the states 0x76..0x7a the other animation id
    /* 0x2BEA */ u8 mUnknown2BEA[0x1AC];
    /* 0x2D96 */ s8 mSubStep; // Step of the helper routines the states call, WalkTo among them (name is a6's)
    /* 0x2D97 */ s8 mSubSubStep;
};

#endif // OV074_ROOMCHARACTER_A4_H
