#ifndef OV074_CHARACTERCALLS_H
#define OV074_CHARACTERCALLS_H

#include <nitro.h>
#include <nnsys.h>

#include "FloorPos.h"

struct CreateId;
struct CharacterRecord;

// The main program's Character (include/character/Character.h), as far as this overlay calls it: a
// declaration-only stand-in, which the calls reach by a cast
class Character {
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
};

#endif // OV074_CHARACTERCALLS_H
