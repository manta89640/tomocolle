#ifndef OV075_SCENE40ITEMMODEL_H
#define OV075_SCENE40ITEMMODEL_H

#include <nitro.h>

#include "item/Item.h"

#include "Scene40Physics.h"

// The scene's three item models (scene+0x69E54): an item the character is given, holds or throws, which falls and
// bounces (Scene40Physics). Its constructor (0x020431A0) and D1 (0x020431CC) follow the scene's constructor in the
// main program, for __cxa_vec_ctor; its table (0x02110A7C) is that file's data, its D0 (0x0204ADCC) at that file's
// end.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40ItemModel : public ItemModel {
public:
    void Load(int resourceId);
    void Hide();
    void ChangeItem(int item);
    VecFx32 GetLookPoint();

    /* 0x12C */ Scene40Motion mMotion;
    /* 0x144 */ BOOL mInUse; // Taken by Scene40Physics::TakeFreeItem
}; // 0x148

#endif // OV075_SCENE40ITEMMODEL_H
