#ifndef OV075_SCENE40CLOTHESLIST_H
#define OV075_SCENE40CLOTHESLIST_H

#include <nitro.h>

#include "character/Character.h"
#include "item/Item.h"

// The list page's clothes (list page + 0x6C): the main program's ClothesList, filled with the clothes the visited
// character owns (its record's bits) instead of the player's.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40ClothesList : public ClothesList {
public:
    void LoadOwnedBy(SavedCharacter *character);
    void GetOwned(Item **list, int *count, int kind);
    void Sort();
}; // 0x20D4

#endif // OV075_SCENE40CLOTHESLIST_H
