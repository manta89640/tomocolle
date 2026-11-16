#ifndef OV074_CHARACTERCALLS_H
#define OV074_CHARACTERCALLS_H

#include <nitro.h>
#include <nnsys.h>

#include "Character.h"
#include "FloorPos.h"

struct CreateId;
struct CharacterRecord;

// The main program's Character (include/character/Character.h), as far as this overlay calls it: the saved
// character with its body, CharacterBase and CharacterModel as its bases (the overlay's character constructors call
// both base-object constructors, C2). The calls reach it by a cast, which the matches depend on.
class Character : public CharacterBase, public CharacterModel {
public:
    void BlendToAnimation(u32 resType, int id);
    void Draw();
    FloorPos GetPointInFront(fx32 distance);
    void Init(int resourceId, int unk, u32 defaultMii);
    BOOL IsAhead(fx32 x, fx32 z, fx32 width);
    BOOL IsBeside(fx32 x, fx32 z, fx32 width);
    BOOL IsInFront(fx32 x, fx32 z);
    BOOL IsNearerThan(FloorPos pos, fx32 distanceSq);
    BOOL IsPointWithinReach(fx32 x, fx32 z, fx32 reach, fx32 maxReach);
    void MoveAlongAngle();
    int PlaySoundAtAnimPhase(int slot, int player, int seqArc, int index, int quarters);
    int PlaySoundOnce(int player, int index, int seqArc);
    void ResetSoundPhase(int slot);
    void ResetSoundPhases();
    fx32 ScaleBySpeed(fx32 value);
    void SetAngle(u16 angle);
    void SetArchive(const char *archiveName);
    void SetVisible(BOOL visible);
    void SetWalkTarget(VecFx32 target);
    void SetWalkTargetAwayFrom(FloorPos pos);
    void SetWalkTargetReflected(FloorPos pos);
    BOOL TurnToTargetAngle(int turnSpeed, int walkSpeed);
    BOOL TurnToWalkTarget(int turnSpeed, int walkSpeed);
    void Update();
    void UpdateAnimSpeed();
    void UpdateBodyColors();
    void UpdateBodyScale();
    void UpdateClothes();
    void UpdateSpeedScales();
    BOOL WalkToTarget(fx32 reach, fx32 speed, int turnShift);

    /* 0x25E4 */ // CharacterModel
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;        // func_02091174 multiplies a distance by it
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to (Character_SetWalkTarget)
    /* 0x2B04 */ int mUnknown2B04;
    /* 0x2B08 */ u16 mMoveAngle;           // The way Character_MoveAlongAngle moves it
    /* 0x2B0A */ u16 mAngle;               // The way the character faces (Character_SetAngle sets both)
    /* 0x2B0C */ u16 mTargetAngle;         // Character_TurnToTargetAngle turns mAngle towards it
    /* 0x2B0E */ u16 mWalkAngle;           // The direction to mTarget, as func_02091194 and Character_TurnToWalkTarget
                                           // find it; its negative is the head's yaw while turning
    /* 0x2B10 */ s8 mSoundPhase[2];        // Character_PlaySoundAtAnimPhase's progress through an animation, per slot
    /* 0x2B12 */ u8 mUnknown2B12[2];
    /* 0x2B14 */ fx32 mAnimSpeed;          // func_020919e8 gives it to the model when an animation starts
}; // 0x2B18

#endif // OV074_CHARACTERCALLS_H
