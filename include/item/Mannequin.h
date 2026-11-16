#ifndef ITEM_MANNEQUIN_H
#define ITEM_MANNEQUIN_H

#include <nitro.h>

#include "Model.h"
#include "character/CharacterModel.h"

// A body that can wear clothes, with a head placed on its neck: the clothes trade and the shops show clothes on
// it. The name is descriptive. The head is a model of its own with no class around it (the scenes' constructors
// store only AnimModel's tables at its start).
class Mannequin {
public:
    void InitHead(int resourceId, int unk);
    void LoadHead(int resourceId, int unk);
    void SetNeck(const MtxFx43 *matrix, const MtxFx33 *rotation);
    void DrawHead();
    void Init(int bodyResourceId, int bodyUnk, int headResourceId, int headUnk);
    void ApplyAngle();
    void SetAngle(u16 angle);
    void Turn(u16 delta);
    void Draw();
    void SetAnimation(u32 resType, int id);
    void BlendToAnimation(u32 resType, int id);
    void SetVisible(BOOL visible);
    void SetAlpha(int alpha);

    /* 0x000 */ AnimModel mHead;
    /* 0x0E0 */ MtxFx33 mNeckRotation; // The body's neck joint, as SetNeck copies it
    /* 0x104 */ MtxFx43 mNeckMatrix;
    /* 0x134 */ VecFx32 mHeadOffset;
    /* 0x140 */ VecFx32 mHeadScale;
    /* 0x14C */ MtxFx33 mUnknown14C;
    /* 0x170 */ MtxFx43 mUnknown170;
    /* 0x1A0 */ u16 mHeadRotation[3];
    /* 0x1A8 */ CharacterModel mBody;
    /* 0x6B0 */ u16 mAngle;
    /* 0x6B2 */ u16 mTargetAngle; // ApplyAngle turns the body to it
    /* 0x6B4 */ fx32 mAnimSpeed;
}; // 0x6B8

extern "C" {
extern const VecFx32 gMannequinHeadScale;
extern const VecFx32 gMannequinBodyScale;
}

#endif // ITEM_MANNEQUIN_H
