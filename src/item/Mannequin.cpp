#include "item/Mannequin.h"

// The mannequin (main program 0x020a00bc to 0x020a03e8): a body that wears clothes and a separate head model
// placed on the body's neck joint each time it is drawn. The clothes trade (overlay 110) and the shops show
// clothes on it.

extern "C" {
void Model_BindResource(Model *model, u32 resType, int resourceId, int unk);
void Model_SetAlpha(Model *model, int alpha);    // Sets a model's alpha (0 to 31)
void AnimModel_SetSpeed(AnimModel *model, fx32 speed); // Sets the speed of a model's animations
}

// The scale the scenes give the head (overlay 110 takes 4/10 of it)
const VecFx32 gMannequinHeadScale = {FX32_ONE, FX32_ONE, FX32_ONE};
static const VecFx32 sUnknownOffset = {0, 0xAE1, 0}; // Read by nothing the link kept
// The scale of the head as a model, and as the mannequin keeps it
static const VecFx32 sHeadModelScale = {FX32_ONE, FX32_ONE, FX32_ONE};
static const VecFx32 sHeadScale = {FX32_ONE, FX32_ONE, FX32_ONE};
// The scale the scenes give the body
const VecFx32 gMannequinBodyScale = {0x1800, 0x1333, 0x1333};

// Keeps sUnknownOffset, as code the link dropped did
void Mannequin_Unused(VecFx32 *offset) {
    *offset = sUnknownOffset;
}

void Mannequin::InitHead(int resourceId, int unk) {
    mHeadRotation[0] = mHeadRotation[1] = mHeadRotation[2] = 0;
    mHeadOffset.x = 0;
    mHeadOffset.y = 0;
    mHeadOffset.z = 0;
    mHeadScale.x = FX32_ONE;
    mHeadScale.y = FX32_ONE;
    mHeadScale.z = FX32_ONE;
    MTX_Identity33(&mUnknown14C);
    MTX_Identity33(&mNeckRotation);
    MTX_Identity43(&mNeckMatrix);
    MTX_Identity43(&mUnknown170);
    mHead.Load(resourceId, unk);
    VecFx32 offset = {0, 0, 0};
    mHeadOffset = offset;
    mHead.mPosition.x = mHeadOffset.x;
    mHead.mPosition.y = mHeadOffset.y;
    mHead.mPosition.z = mHeadOffset.z;
    VecFx32 modelScale = sHeadModelScale;
    mHead.mScale.x = modelScale.x;
    mHead.mScale.y = modelScale.y;
    mHead.mScale.z = modelScale.z;
    VecFx32 scale = sHeadScale;
    mHeadScale = scale;
}

void Mannequin::LoadHead(int resourceId, int unk) {
    mHead.RemoveAnims();
    Model_BindResource(&mHead, RESOURCE_BMD0, resourceId, unk);
}

void Mannequin::SetNeck(const MtxFx43 *matrix, const MtxFx33 *rotation) {
    MI_Copy48B(matrix, &mNeckMatrix);
    MI_Copy36B(rotation, &mNeckRotation);
}

void Mannequin::DrawHead() {
    MtxFx33 rotation;
    MTX_Identity33(&rotation);
    MTX_RotZ33(&rotation, FX_SinIdx(0xC000), FX_CosIdx(0xC000));
    MTX_Concat33(&rotation, &mNeckRotation, &rotation);
    MTX_Copy33(&rotation, &mHead.mRotation);
    mHead.mPosition.x = mNeckMatrix.m[3][0];
    mHead.mPosition.y = mNeckMatrix.m[3][1];
    mHead.mPosition.z = mNeckMatrix.m[3][2];
    mHead.mScale.x = mHeadScale.x;
    mHead.mScale.y = mHeadScale.y;
    mHead.mScale.z = mHeadScale.z;
    mHead.Draw();
}

void Mannequin::Init(int bodyResourceId, int bodyUnk, int headResourceId, int headUnk) {
    CharacterModel::Load(bodyResourceId, bodyUnk);
    InitHead(headResourceId, headUnk);
    mAngle = mTargetAngle = 0;
    mAnimSpeed = FX32_ONE;
}

void Mannequin::ApplyAngle() {
    mAnim.mRotY = 0x4000 - mTargetAngle;
}

void Mannequin::SetAngle(u16 angle) {
    mTargetAngle = angle;
    mAngle = mTargetAngle;
}

void Mannequin::Turn(u16 delta) {
    mAngle += delta;
    mTargetAngle += delta;
}

void Mannequin::Draw() {
    CharacterModel::Draw();
    SetNeck(&mJointMatrices[0], &mJointRotations[0]);
    DrawHead();
}

void Mannequin::SetAnimation(u32 resType, int id) {
    CharacterModel::SetAnimation(resType, id);
    AnimModel_SetSpeed(&mAnim, mAnimSpeed);
}

void Mannequin::BlendToAnimation(u32 resType, int id) {
    CharacterModel::BlendToAnimation(resType, id);
    AnimModel_SetSpeed(&mAnim, mAnimSpeed);
}

void Mannequin::SetVisible(BOOL visible) {
    mAnim.mVisible = visible;
    mHead.mVisible = visible;
}

void Mannequin::SetAlpha(int alpha) {
    Model_SetAlpha(&mHead, alpha);
    Model_SetAlpha(&mAnim, alpha);
}
