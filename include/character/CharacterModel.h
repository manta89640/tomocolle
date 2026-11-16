#ifndef CHARACTER_CHARACTERMODEL_H
#define CHARACTER_CHARACTERMODEL_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"

// STAND-IN for the item class of the main program (0x0209e140 on), as far as the character model uses it: the
// clothes the model wears. Its virtual function 2 returns the clothes' model code, the end of the body archive's name.
class ClothesItem {
public:
    ClothesItem(); // _ZN11ClothesItemC1Ev
    virtual u32 VirtualUnknown0();
    virtual u32 VirtualUnknown1();
    virtual const char *GetModelCode();

    /* 0x04 */ u16 mId; // Row of the clothes table (data_020f3ff0)
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
    /* 0x14 */ int mUnknown14;
}; // 0x18

// One row of the clothes table (data_020f3ff0), as the character model reads it.
struct ClothesData {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ u32 mUnknown0CBits0 : 4;
    /*      */ u32 mColor : 3;          // The digit of the body texture's name ("body_c<n>.ntfp")
    /*      */ u32 mHasFemaleBody : 1;  // The body has a female variant ("body<code>F")
    /*      */ u32 mUnknown0CBits8 : 24;
    /* 0x10 */ u32 mUnknown10;
}; // 0x14

extern "C" {
extern const ClothesData data_020f3ff0[];
}

// The model of a character: its body, wearing clothes, with the joints the game follows (the head, the hands,
// elbows, knees and ankles) and a shadow. The model is a member, not a base: its constructor stores no virtual
// table of its own, only those of AnimModel's constructor.
class CharacterModel {
public:
    // The joints whose matrices the model keeps (mJointMatrices)
    enum Joint {
        JOINT_NECK,
        JOINT_L_HAND,
        JOINT_R_HAND,
        JOINT_BODY,
        JOINT_L_KNEE,
        JOINT_R_KNEE,
        JOINT_L_ANKLE,
        JOINT_R_ANKLE,
        JOINT_L_ELBOW,
        JOINT_R_ELBOW,
        JOINT_COUNT,
    };

    CharacterModel();
    ~CharacterModel() { NNS_G3dRenderObjResetCallBack(&mAnim.mRenderObj); }

    void Load(int resourceId, int unk);
    void Draw();
    void Reload(int resourceId, int unk);
    void MakeBodyArchivePath();
    void SetArchive(const char *archiveName, int clothes, BOOL female);
    void SetClothes(int clothes, BOOL female);
    void UseFemaleBody(BOOL female);
    void SetAnimFlags(BOOL playOnce, BOOL finished);
    void SetAnimFlagsAndFrame(BOOL playOnce, fx32 frame, BOOL finished);
    void MakeModel(int resourceId, int unk);
    void SetAnimation(u32 resType, int id);
    void BlendToAnimation(u32 resType, int id);
    void RemoveAnimation(u32 resType);
    void SetPaletteColor1(GXRgb color);
    void LoadTexturePalette();
    void SetPaletteColors(GXRgb color0, GXRgb color1);
    void GetJointMatrices(int joint, MtxFx43 *matrix, MtxFx33 *rotation);
    VecFx32 GetJointPosition(int joint);
    BOOL HitTestJoint(int joint);
    BOOL HitTestJoint(const VecFx32 *origin, const VecFx32 *dir, int joint);
    BOOL HitTestJoint(VecFx32 origin, VecFx32 dir, int joint) { return HitTestJoint(&origin, &dir, joint); }
    void UpdateBlend();
    void TurnJoint(NNSG3dRS *rs);
    static void OnNodeDesc(NNSG3dRS *rs);
    static void OnNodeDescDone(NNSG3dRS *rs);
    static void OnShadowNode(NNSG3dRS *rs);
    static void InitCallbacks(NNSG3dRS *rs);
    void SetScaleFromBody(u32 height, u32 build);
    VecFx32 GetHeldItemScale();

    /* 0x000 */ AnimModel mAnim;
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ VecFx32 mScale;              // Of the body, from the Mii's height and build
    /* 0x0F8 */ VecFx32 mInvScale;
    /* 0x104 */ int mAnimId;                 // The current 'BCA0' animation, -1 for none
    /* 0x108 */ int mPrevAnimId;             // The one being blended out of
    /* 0x10C */ fx32 mBlendFrames;           // > 0 while the change to the current animation is still running
    /* 0x110 */ int mPatternAnimId;          // The current 'BTP0' animation, -1 for none
    /* 0x114 */ MtxFx33 mJointRotations[JOINT_COUNT]; // By joint: the rotation of its matrix
    /* 0x27C */ MtxFx43 mJointMatrices[JOINT_COUNT];
    /* 0x45C */ BOOL mTrackLimbs;            // Also keep the matrices of the elbows, knees and ankles
    /* 0x460 */ BOOL mTurnJoints;            // Turn the joints by mJointAngles
    /* 0x464 */ u16 mJointAngles[10][3];     // x, y, z of the turnable joints (node descriptions 2, 3, 5, 6, 12,
                                             // 13, 14, 16, 17 and 19)
    /* 0x4A0 */ u8 mUnknown4A0[4];
    /* 0x4A4 */ BOOL mShadowVisible;        // The visibility of the shadow node
    /* 0x4A8 */ BOOL mShadowAtHeight;       // Draw the shadow at mShadowHeight instead of its joint's height
    /* 0x4AC */ fx32 mShadowHeight;
    /* 0x4B0 */ ClothesItem mClothes;
    /* 0x4C8 */ char mArchiveName[4];       // Name its archive is mounted under
    /* 0x4CC */ char mArchivePath[0x1C];    // and the archive's path
    /* 0x4E8 */ char mModelPath[0xE];       // "XXX:/body.imd", XXX the archive's name
    /* 0x4F6 */ char mTexturePath[0x12];    // "XXX:/body_c0.ntfp"
}; // 0x508

#endif // CHARACTER_CHARACTERMODEL_H
