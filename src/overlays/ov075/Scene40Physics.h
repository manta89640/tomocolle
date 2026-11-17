#ifndef OV075_SCENE40PHYSICS_H
#define OV075_SCENE40PHYSICS_H

#include <nitro.h>

class Scene40Character;
class Scene40ItemModel;

// How a thing that can be thrown moves (Scene40Character+0x2B18, Scene40ItemModel+0x12C)
struct Scene40Motion {
    void Reset();
    void UpdateAtRest(VecFx32 *pos, VecFx32 *velocity);

    /* 0x00 */ VecFx32 mVelocity;
    /* 0x0C */ BOOL mAtRest; // On the floor and not moving
    /* 0x10 */ BOOL mUnknown10; // While set it does not move
    /* 0x14 */ int mUnknown14;
}; // 0x18

// The scene's falling and bouncing (scene+0x69E24): the character and the item models fall back to the floor with
// gravity, bounce off the room's walls and come to rest. Scene40::Init gives it the character and each item model.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40Physics {
public:
    void Init();
    void AddItem(Scene40ItemModel *item);
    void Update();
    void UpdateCharacter();
    void UpdateItems();

    /* 0x00 */ Scene40Character *mCharacter;
    /* 0x04 */ Scene40ItemModel *mItems[10];
    /* 0x2C */ int mItemCount;
}; // 0x30

#endif // OV075_SCENE40PHYSICS_H
