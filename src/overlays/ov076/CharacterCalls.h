#ifndef OV076_CHARACTERCALLS_H
#define OV076_CHARACTERCALLS_H

#include <nitro.h>
#include <nnsys.h>


struct CreateId;
struct CharacterRecord;

// The main program's Character (include/character/Character.h), as far as this overlay calls it: a
// declaration-only stand-in, which the calls reach by a cast
class Character {
public:
    void Draw();
    void Init(int resourceId, int unk, u32 defaultMii);
    void SetAngle(u16 angle);
    void SetAnimation(u32 resType, int id);
    void SetArchive(const char *archiveName);
    void SetVisible(BOOL visible);
    void Turn(u16 delta);
    void Update();
    void UpdateAnimSpeed();
    void UpdateBodyColors();
    void UpdateBodyScale();
};

// The main program's CharacterSort (include/character/Character.h), as far as this overlay calls it: a
// declaration-only stand-in, which the calls reach by a cast
class CharacterSort {
public:
    static void RemoveSpaces(wchar_t *str);
};

#endif // OV076_CHARACTERCALLS_H
