#ifndef OV073_CHARACTERCALLS_H
#define OV073_CHARACTERCALLS_H

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
};

#endif // OV073_CHARACTERCALLS_H
