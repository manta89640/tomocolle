#ifndef OV109_CHARACTERCALLS_H
#define OV109_CHARACTERCALLS_H

#include <nitro.h>
#include <nnsys.h>


struct CreateId;
struct CharacterRecord;

// The main program's Character (include/character/Character.h), as far as this overlay calls it: a
// declaration-only stand-in, which the calls reach by a cast
class Character {
public:
    void BlendToAnimation(u32 resType, int id);
    void Draw();
    void Init(int resourceId, int unk, u32 defaultMii);
    int PlaySoundAtAnimPhase(int slot, int player, int seqArc, int index, int quarters);
    void ResetSoundPhase(int slot);
    void SetAnimSpeed(fx32 speed);
    void SetAnimation(u32 resType, int id);
    void SetArchive(const char *archiveName);
    void SetVisible(BOOL visible);
    void UpdateBodyColors();
    void UpdateBodyScale();
};

#endif // OV109_CHARACTERCALLS_H
