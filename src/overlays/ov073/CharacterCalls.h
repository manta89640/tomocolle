#ifndef OV073_CHARACTERCALLS_H
#define OV073_CHARACTERCALLS_H

#include <nitro.h>
#include <nnsys.h>

#include "Character.h"
#include "FloorPos.h"

struct CreateId;
struct CharacterRecord;

// The main program's Character (include/character/Character.h), as far as this overlay calls it: the saved
// character with its body, CharacterBase and CharacterModel as its bases (the overlay's character constructor calls
// both base-object constructors, C2). The calls reach it by a cast, which the matches depend on.
class Character : public CharacterBase, public CharacterModel {
public:
    void BlendToAnimation(u32 resType, int id);
    void Draw();
    void Init(int resourceId, int unk, u32 defaultMii);
    BOOL IsPointWithinReach(fx32 x, fx32 z, fx32 reach, fx32 maxReach);
    void MoveAlongAngle();
    int PlaySoundAtAnimPhase(int slot, int player, int seqArc, int index, int quarters);
    int PlaySoundOnce(int player, int index, int seqArc);
    void ResetSoundPhase(int slot);
    fx32 ScaleBySpeed(fx32 value);
    void SetAngle(u16 angle);
    void SetArchive(const char *archiveName);
    BOOL TurnToTargetAngle(int turnSpeed, int walkSpeed);
    BOOL TurnToWalkTarget(int turnSpeed, int walkSpeed);
    void Update();
    void UpdateAnimSpeed();
    void UpdateBodyColors();
    void UpdateBodyScale();
    void UpdateSpeedScales();
    BOOL WalkToTarget(fx32 reach, fx32 speed, int turnShift);

    /* 0x25E4 */ // CharacterModel
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;        // func_02091174 multiplies a distance by it; state 0x12 walks for
                                           // longer when it is under 1.0
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to
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

#endif // OV073_CHARACTERCALLS_H
