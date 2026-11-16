#ifndef ISLAND_ISLANDPROGRESS_H
#define ISLAND_ISLANDPROGRESS_H

#include <nitro.h>

class Inventory;

// What the island has come to (src/island/IslandProgress.cpp): the counts that decide which of the island's
// facilities opens next (GetNewFacility, an index of the island's state bytes, func_02039c00) and which of the
// map's buildings can be entered (overlay 72's SelectBuilding asks the four Has* functions). Overlays 71, 72, 75, 94
// and 109 set one up.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class IslandProgress {
public:
    void Init(Inventory *inventory, int *money);
    void Count();
    int GetNewFacility(BOOL check20);
    BOOL HasSevenResidentsThreeOfEachSex();
    BOOL HasTwoWithBirthdays();
    BOOL HasTwoOfEachSex();
    BOOL HasFourResidents();

    /* 0x00 */ int mMen;
    /* 0x04 */ int mWomen;
    /* 0x08 */ int mWithBirthday;   // Residents whose year, month and day of birth are all set
    /* 0x0C */ int mResidents;
    /* 0x10 */ int mGoods;          // Owned items of kinds 4, 5 and 6
    /* 0x14 */ int mMoneyCount;     // The money (gGameData's capped counter, or *mMoney)
    /* 0x18 */ int mEntriesA;       // Used entries of the ten at gGameData + 0x155E4
    /* 0x1C */ int mEntriesB;       // Used entries of the ten at gGameData + 0x154B8
    /* 0x20 */ int mTroublesSolved;
    /* 0x24 */ Inventory *mInventory; // The items to count, or NULL to load them from the saved game
    /* 0x28 */ int *mMoney;         // The money to count, or NULL for the saved game's
}; // 0x2C

#endif // ISLAND_ISLANDPROGRESS_H
