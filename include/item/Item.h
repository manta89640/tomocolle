#ifndef ITEM_ITEM_H
#define ITEM_ITEM_H

#include <nitro.h>

#include "Model.h"

// The things the player can own: goods (data_020f5f0c), clothes (data_020f3ff0) and room interiors (data_020f72f8),
// each kind with a table of what each item is and a class that reads its entry (main program 0x0209e140 to
// 0x020a00bc, src/item/Item.cpp, and the interiors in src/room/Room.cpp). The names are descriptive: no name of
// these classes survives in the ROM.

// An entry of the goods table (data_020f5f0c, 255 entries)
struct ItemInfo {
    /* 0x00 */ u32 mPrice : 24;   // What GetPrice returns (250, 480, ...: the values look like prices)
    /*      */ u32 mSortKey : 8;  // The lists are sorted by it
    /* 0x04 */ u32 mWeight0 : 4; // The weights for the random choice, by gGameData.mUnknown4E[0]
    /*      */ u32 mWeight1 : 4;
    /*      */ u32 mWeight2 : 4;
    /*      */ u32 mWeight3 : 4;
    /*      */ u32 mScale : 8;    // The model's scale in tenths
    /*      */ u32 mUnknown04Bits24 : 8;
    /* 0x08 */ char mModelName[4]; // The number of its model, "Mdl/Item/item%s_3D_LZ.bin"
    /* 0x0C */ u32 mSeason : 4;     // 0, or the only season (1 to 4) it is offered in
    /*      */ u32 mKind : 3;       // The tab it is listed on
    /*      */ u32 mUnknown0CBit7 : 1;
    /*      */ u32 mAvailable0 : 1; // Whether the random choice can give it, by its first parameter (0 to 2)
    /*      */ u32 mAvailable1 : 1;
    /*      */ u32 mAvailable2 : 1;
    /*      */ u32 mTaste0 : 4;     // How much each kind of character likes it, 0 to 15 (SavedCharacter::GetTaste)
    /*      */ u32 mTaste1 : 4;
    /*      */ u32 mTaste2 : 4;
    /*      */ u32 mTaste3 : 4;
    /*      */ u32 mFoodSize : 2;   // How much eating it fills (a row of gStr_2K)
    /*      */ u32 mUnknown0CBit29 : 1;
    /*      */ u32 mGroup : 2; // ChooseItemInGroup chooses among the goods of one
    /* 0x10 */ u32 mFlags; // Bits the lists can be filtered by; for food, the kinds the food troubles ask for
}; // 0x14

// An entry of the clothes table (data_020f3ff0, 300 entries)
struct ClothesInfo {
    /* 0x00 */ u32 mPrice : 23;
    /*      */ u32 mSortKey : 9;
    /* 0x04 */ u8 mWeights[4];     // The weights for the random choice, by gGameData.mUnknown4E[0]
    /* 0x08 */ char mModelName[4]; // The number of its body model, "Mdl/Body/body%s"
    /* 0x0C */ u32 mSeason : 4;    // 0: any; 1 to 4 one season; 5 to 7 two of them (see IsClothesInSeason)
    /*      */ u32 mBodyColor : 3; // The character model writes '0' + it into "body_c0.ntfp"
    /*      */ u32 mHasFemaleBody : 1; // The character model then loads "Mdl/Body/body<name>F"
    /*      */ u32 mAvailable0 : 1; // As ItemInfo's
    /*      */ u32 mAvailable1 : 1;
    /*      */ u32 mAvailable2 : 1;
    /*      */ u32 mSex : 2;  // 1: men's, 2: women's (SavedCharacter::GetClothesLiking, ChooseWantedClothes)
    /*      */ u32 mKind : 3; // The tab it is listed on
    /*      */ u32 mTaste3 : 4; // How much each kind of personality likes them, 0 to 15 (GetClothesLiking)
    /*      */ u32 mTaste1 : 4;
    /*      */ u32 mTaste2 : 4;
    /*      */ u32 mTaste0 : 4;
    /* 0x10 */ u32 mStyles : 23; // The kinds the clothes troubles ask for (CLOTHES_WARM, ...)
    /*      */ u32 mUnknown10Bits23 : 9;
}; // 0x14

// Bits of ItemInfo::mFlags and ClothesInfo::mStyles, as the troubles' table names the same kinds
#define ITEM_BAD_FOOD (1 << 12) // Food that tastes bad: the food troubles that choose a food never choose one
#define CLOTHES_WARM (1 << 0)   // Warm clothes: not asked for from April to September
#define CLOTHES_COOL (1 << 1)   // Cool clothes: not asked for from October to March

extern "C" {
extern const ClothesInfo data_020f3ff0[];
extern const ItemInfo data_020f5f0c[];
}

// What the player owns of one item: the base class of the three kinds. Its seven virtual functions read the
// item's entry of its kind's table; the base declares them pure (its virtual table is all zeros).
class Item {
public:
    Item() { mId = 0; }

    virtual u32 GetPrice() = 0;
    virtual u32 GetPrice2() = 0; // The same field as GetPrice in all three kinds
    virtual const char *GetModelName() = 0;
    virtual u8 GetWeight0() = 0;
    virtual u8 GetWeight1() = 0;
    virtual u8 GetWeight2() = 0;
    virtual u8 GetWeight3() = 0;

    void Clear();
    void Settle();
    BOOL Add(int count);

    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned; // Set once the count has been above 0
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount; // 0 to 99
}; // 0x14

// A kind of clothes.
class ClothesItem : public Item {
public:
    ClothesItem();

    virtual u32 GetPrice() { return data_020f3ff0[mId].mPrice; }
    virtual u32 GetPrice2() { return data_020f3ff0[mId].mPrice; }
    virtual const char *GetModelName() { return data_020f3ff0[mId].mModelName; }
    virtual u8 GetWeight0() { return data_020f3ff0[mId].mWeights[0]; }
    virtual u8 GetWeight1() { return data_020f3ff0[mId].mWeights[1]; }
    virtual u8 GetWeight2() { return data_020f3ff0[mId].mWeights[2]; }
    virtual u8 GetWeight3() { return data_020f3ff0[mId].mWeights[3]; }

    void Clear();
    void Settle();

    /* 0x14 */ int mUnknown14;
}; // 0x18

// A kind of goods.
class InventoryItem : public Item {
public:
    InventoryItem();

    virtual u32 GetPrice() { return data_020f5f0c[mId].mPrice; }
    virtual u32 GetPrice2() { return data_020f5f0c[mId].mPrice; }
    virtual const char *GetModelName() { return data_020f5f0c[mId].mModelName; }
    virtual u8 GetWeight0() { return data_020f5f0c[mId].mWeight0; }
    virtual u8 GetWeight1() { return data_020f5f0c[mId].mWeight1; }
    virtual u8 GetWeight2() { return data_020f5f0c[mId].mWeight2; }
    virtual u8 GetWeight3() { return data_020f5f0c[mId].mWeight3; }
}; // 0x14

#define INVENTORY_SIZE 255
#define INVENTORY_SAVED 236
#define CLOTHES_SIZE 300

// The goods the player owns, as the save data has them, and a list of pointers to them to sort and show.
class Inventory {
public:
    void Load();
    void Save();
    void Sort();
    void SortHeldFirst();
    // Each fills a list with the items that pass a test, and its count
    void GetOwned(Item **list, int *count, int kind);
    void GetOwned(Item **list, int *count, int kind, u32 flags);
    void GetOwnedKinds4And5(Item **list, int *count);
    void GetOwnedKind3(Item **list, int *count);
    void GetOwnedKinds4And5Again(Item **list, int *count);
    void GetHeld(Item **list, int *count, int kind, u32 flags);
    void GetHeldKinds4And5(Item **list, int *count);
    void GetListed(Item **list, int *count);
    BOOL HasAnyOwnedKind1();
    BOOL HasAnyOwnedKind3();
    BOOL HasAnyOwnedKinds4And5();

    /* 0x0000 */ int mCount;
    /* 0x0004 */ InventoryItem mItems[INVENTORY_SIZE];
    /* 0x13F0 */ InventoryItem *mList[INVENTORY_SIZE];
}; // 0x17EC

// The clothes, in the same way.
class ClothesList {
public:
    void Load();
    void Save();
    void Sort();
    void SortHeldFirst();
    void GetOwned(Item **list, int *count, int kind);
    void GetOnOffer(Item **list, int *count, int kind);
    void GetOnOfferInSeason(Item **list, int *count, int kind, s64 time);
    void GetHeld(Item **list, int *count, int kind);
    BOOL HasAnyOwned();

    /* 0x0000 */ int mCount;
    /* 0x0004 */ ClothesItem mItems[CLOTHES_SIZE];
    /* 0x1C24 */ ClothesItem *mList[CLOTHES_SIZE];
}; // 0x20D4

// The 3D model of a kind of goods, and the goods it shows.
class ItemModel : public AnimModel, public InventoryItem {
public:
    ItemModel();

    void SetItem(const char *archiveName, int item);
    void LoadModel(int resourceId);
    void LoadAnimation(int resourceId);
    void ResetScale();
    void SetScale(fx32 scale);

    /* 0x0E0 InventoryItem */
    /* 0x0F4 */ int mResourceId;
    /* 0x0F8 */ char mArchiveName[4];
    /* 0x0FC */ char mArchivePath[0x1E];
    /* 0x11A */ char mModelPath[0xE];
    /* 0x128 */ fx32 mBaseScale; // From the table's mScale; SetScale multiplies it
}; // 0x12C

void LoadClothes(ClothesItem *items, int max, int *count);
void SaveClothes(ClothesItem *items, int count);
u16 ChooseClothes(int availability, u16 *clothes, int count);
void LoadItems(InventoryItem *items, int max, int *count);
u16 ChooseItem(int availability, int kind, int exclude);
u16 ChooseItemInGroup(int availability, int kind, int group);
u16 ChooseItems(int availability, int kind, u16 *items, int count);
u16 ChooseItemsNot1000(int availability, int kind, u16 *items, int count);
u16 GetFixedItems(u16 *items, int count);

#endif // ITEM_ITEM_H
