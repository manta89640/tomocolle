#include <nitro.h>

#include "GameData.h"
#include "island/Apartment.h"
#include "island/IslandProgress.h"

// The player's items, as far as the island's progress reads them (the item files, main program 0x0209e140 on)
class InventoryItem {
public:
    InventoryItem();
    virtual void UnknownVirtual0();

    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
}; // 0x14

class Inventory {
public:
    void Load();

    /* 0x0000 */ int mCount;
    /* 0x0004 */ InventoryItem mItems[255];
    /* 0x13F0 */ InventoryItem *mList[255];
}; // 0x17EC

// The saved game's records that Count reads past the fields GameData.h has
struct ProgressEntry {
    /* 0x0 */ s8 mUnknown0;
    /* 0x1 */ s8 mUnknown1;
    /* 0x2 */ s8 mUnknown2; // -1 when the entry is empty
};

struct GameDataProgress {
    /* 0x00000 */ u8 mUnknown00000[0x154B8];
    /* 0x154B8 */ ProgressEntry mEntriesB[10];
    /* 0x154D6 */ u8 mUnknown154D6[0x10E];
    /* 0x155E4 */ ProgressEntry mEntriesA[10];
};

#define GAMEDATA_ENTRIES_A(i) (((GameDataProgress *)&gGameData)->mEntriesA[i])
#define GAMEDATA_ENTRIES_B(i) (((GameDataProgress *)&gGameData)->mEntriesB[i])

// What the item table (data_020f5f0c, 0x14 bytes an item) says of an item, as far as is used here
struct ItemInfo {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ u32 mUnknown0CBits0 : 4;
    /*      */ u32 mKind : 3;
    /*      */ u32 mUnknown0CBits7 : 25;
    /* 0x10 */ u8 mUnknown10[4];
}; // 0x14

struct IslandState;

// The save data's header (include/save/SaveBlocks.h), as far as this file calls it: a declaration-only stand-in,
// which the calls reach by a cast
class SaveHeader {
public:
    u8 GetUnknown44(int index); // A facility's state: 0 not built, 1 built, 2 new, 3 not open yet
};

extern "C" {
extern const ItemInfo data_020f5f0c[];
extern IslandState data_0212348c; // The state of the island's buildings and features
BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
BOOL func_0202c604(GameData *game, int index); // Its sex (MiiData_GetSex): 0 male, 1 female
u32 func_0202c6a8(GameData *game, int index);  // Its year of birth, 0xFFF for none
u32 func_0202c6d0(GameData *game, int index);  // Its month of birth
u32 func_0202c6f8(GameData *game, int index);  // Its day of birth
}

void IslandProgress::Init(Inventory *inventory, int *money) {
    mInventory = inventory;
    mMoney = money;
    mMen = 0;
    mWomen = 0;
    mWithBirthday = 0;
    mResidents = 0;
    mGoods = 0;
    mMoneyCount = 0;
    mEntriesA = 0;
    mEntriesB = 0;
    mTroublesSolved = 0;
    Count();
}

void IslandProgress::Count() {
    mMen = 0;
    mWomen = 0;
    mWithBirthday = 0;
    mResidents = 0;
    mGoods = 0;
    mMoneyCount = 0;
    mEntriesA = 0;
    mEntriesB = 0;
    mTroublesSolved = 0;
    for (int i = 0; i < 100; i++) {
        if (!func_0202c3dc(&gGameData, i)) {
            continue;
        }
        if (!func_0202c604(&gGameData, i)) {
            mMen++;
        } else {
            mWomen++;
        }
        if (func_0202c6a8(&gGameData, i) == 0xFFF) {
            continue;
        }
        u32 month = func_0202c6d0(&gGameData, i);
        u32 day = func_0202c6f8(&gGameData, i);
        if (month != 0 && month <= 12 && day != 0 && day <= 31) {
            mWithBirthday++;
        }
    }
    mResidents = mMen + mWomen;
    Inventory own;
    Inventory *inventory = mInventory;
    if (inventory == NULL) {
        own.Load();
        inventory = &own;
    }
    for (int i = 0; i < inventory->mCount; i++) {
        InventoryItem *item = &inventory->mItems[i];
        if (item->mOwned) {
            u32 kind = data_020f5f0c[item->mId].mKind;
            if (kind == 4 || kind - 5 <= 1) { // 4, 5 or 6
                mGoods++;
            }
        }
    }
    mMoneyCount = mMoney != NULL ? *mMoney : gGameData.mCappedCounter;
    mEntriesA = 0;
    for (int i = 0; i < 10; i++) {
        ProgressEntry entry = GAMEDATA_ENTRIES_A(i);
        if (entry.mUnknown2 != -1) {
            mEntriesA++;
        }
    }
    mEntriesB = 0;
    for (int i = 0; i < 10; i++) {
        ProgressEntry entry = GAMEDATA_ENTRIES_B(i);
        if (entry.mUnknown2 != -1) {
            mEntriesB++;
        }
    }
    mTroublesSolved = gGameData.mTroublesSolved;
}

int IslandProgress::GetNewFacility(BOOL check20) {
    int facility = -1;
    if (check20 && *(int *)((u8 *)&gGameData + 0x2C) == 1) {
        facility = 0x20;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(7) == 3) {
        facility = 7;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(3) == 3) {
        facility = 3;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(5) == 3) {
        facility = 5;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(4) == 3) {
        facility = 4;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(2) == 0 && mMen >= 2 && mWomen >= 2 && mTroublesSolved >= 15) {
        facility = 2;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(12) == 0 && mResidents >= 3) {
        facility = 12;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(6) == 0 && mResidents >= 5 && mTroublesSolved >= 10) {
        facility = 6;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(10) == 0 && mMen >= 2 && mWomen >= 2 && mTroublesSolved >= 5) {
        facility = 10;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(8) == 0 && mGoods >= 5) {
        facility = 8;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(11) == 0 && mMoneyCount >= 20000) {
        facility = 11;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(9) == 0 && mMen >= 3 && mWomen >= 3 && mResidents >= 7) {
        facility = 9;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(1) == 0 && mTroublesSolved >= 60 && mResidents >= 4) {
        facility = 1;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(13) == 3) {
        facility = 13;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(14) == 3) {
        facility = 14;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(15) == 0 && ((SaveHeader *)&data_0212348c)->GetUnknown44(6) != 0 && mResidents >= 10 &&
               mTroublesSolved >= 40) {
        facility = 15;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(0) == 0 && mMen >= 2 && mWomen >= 2 && mTroublesSolved >= 30) {
        facility = 0;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(16) == 0 && ((SaveHeader *)&data_0212348c)->GetUnknown44(6) != 0 && mEntriesA >= 10) {
        facility = 16;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(17) == 0 && ((SaveHeader *)&data_0212348c)->GetUnknown44(6) != 0 && mEntriesB >= 10) {
        facility = 17;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(18) == 0 && ((SaveHeader *)&data_0212348c)->GetUnknown44(6) != 0 &&
               mTroublesSolved >= 90) {
        facility = 18;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(20) == 0 && mTroublesSolved >= 100) {
        facility = 20;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(21) == 0 && mTroublesSolved >= 200) {
        facility = 21;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(22) == 0 && mTroublesSolved >= 400) {
        facility = 22;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(23) == 0 && mTroublesSolved >= 700) {
        facility = 23;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(24) == 0 && mTroublesSolved >= 1000) {
        facility = 24;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(25) == 0 && mResidents >= 10) {
        facility = 25;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(26) == 0 && mResidents >= 30) {
        facility = 26;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(27) == 0 && mResidents >= 50) {
        facility = 27;
    } else if (((SaveHeader *)&data_0212348c)->GetUnknown44(28) == 0 && mResidents >= 100) {
        facility = 28;
    } else {
        int stage = gGameData.mApartmentStage;
        if (GetApartmentStage(mResidents) > stage) {
            facility = 7;
        }
    }
    return facility;
}

BOOL IslandProgress::HasSevenResidentsThreeOfEachSex() {
    return mResidents >= 7 && mMen >= 3 && mWomen >= 3;
}

BOOL IslandProgress::HasTwoWithBirthdays() {
    return mWithBirthday >= 2;
}

BOOL IslandProgress::HasTwoOfEachSex() {
    return mMen >= 2 && mWomen >= 2;
}

BOOL IslandProgress::HasFourResidents() {
    return mResidents >= 4;
}
