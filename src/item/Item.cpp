#include "item/Item.h"

#include "GameData.h"

// The items the player owns (main program 0x0209e140 to 0x020a00bc): the clothes, the goods, the 3D model of a
// kind of goods, and the random choice of the goods and clothes on offer.

extern "C" {
// The save data's counts of the clothes and goods
int func_0202c770(GameData *game, int clothes); // The clothes' count
int func_0202c784(GameData *game, int clothes); // The clothes' second value, -1 when they are not owned
void func_0202c798(GameData *game, int clothes, u8 count);
void func_0202c7ac(GameData *game, int clothes, s8 value);
int func_0202c7dc(GameData *game, int item); // The goods' count, -1 when they are not owned
void func_0202c7f0(GameData *game, int item, s8 count);

// The game's clock
extern void *data_02123440;
void Clock_GetDate(void *clock, RTCDate *date);
BOOL func_02035aa4(void *clock);

extern MATHRandContext16 gRandom;

// The model library
void *func_0202a5fc(Model *model, u32 resType, int unk);
void func_0202a5d8(Model *model, void *anim, u32 resType, int unk);
void func_02029c20(Model *model, u32 resType, int resourceId, int unk);
}

#define SORT_BUFFER_SIZE 200

static inline u16 Rand(u16 max) {
    return MATH_Rand16(&gRandom, max);
}

// The season of each month, 1 (spring) to 4 (winter)
static const int sClothesSeasons[12] = {4, 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4};

ClothesItem::ClothesItem() {
    mId = 0;
}

void ClothesItem::Clear() {
    Item::Clear();
    mUnknown14 = 0;
}

void ClothesItem::Settle() {
    Item::Settle();
}

// Whether clothes can be worn in a month: they have no season, or the month's is theirs, or one of their two
static BOOL IsClothesInSeason(int clothes, int month) {
    int type = data_020f3ff0[clothes].mSeason;
    int season = sClothesSeasons[month - 1];
    BOOL result = TRUE;
    switch (type) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        if (season != type) {
            result = FALSE;
        }
        break;
    case 5:
        if (season != 1 && season != 2) {
            result = FALSE;
        }
        break;
    case 6:
        if (season != 3 && season != 4) {
            result = FALSE;
        }
        break;
    case 7:
        if (season != 1 && season != 3) {
            result = FALSE;
        }
        break;
    }
    return result;
}

void ClothesList::Load() {
    mCount = 0;
    for (int i = 0; i < CLOTHES_SIZE; i++) {
        mItems[i].Clear();
        mItems[i].mId = i;
        BOOL owned = TRUE;
        int count = func_0202c770(&gGameData, i);
        int value = func_0202c784(&gGameData, i);
        if (value == -1) {
            owned = FALSE;
            count = 0;
            value = 0;
        }
        mItems[i].mOwned = owned;
        mItems[i].mUnknown0C = count;
        mItems[i].mCount = count;
        mItems[i].mUnknown14 = value;
        mCount++;
    }
    for (int i = 0; i < mCount; i++) {
        mItems[i].Settle();
        mList[i] = &mItems[i];
    }
}

void ClothesList::Save() {
    for (int i = 0; i < mCount; i++) {
        mItems[i].Settle();
        u16 id = mItems[i].mId;
        if (mItems[i].mOwned != FALSE) {
            func_0202c798(&gGameData, id, mItems[i].mCount);
            func_0202c7ac(&gGameData, id, mItems[i].mUnknown14);
        } else if (mItems[i].mOwned == FALSE) {
            func_0202c798(&gGameData, id, 0);
            func_0202c7ac(&gGameData, id, -1);
        }
    }
}

static s32 CompareClothes(void *a, void *b);
static s32 CompareClothesHeldFirst(void *a, void *b);

void ClothesList::Sort() {
    u32 buffer[SORT_BUFFER_SIZE];
    MATH_QSort(mList, mCount, sizeof(ClothesItem *), CompareClothes, buffer);
}

void ClothesList::SortHeldFirst() {
    u32 buffer[SORT_BUFFER_SIZE];
    MATH_QSort(mList, mCount, sizeof(ClothesItem *), CompareClothesHeldFirst, buffer);
}

static s32 CompareClothes(void *a, void *b) {
    return data_020f3ff0[(*(ClothesItem **)a)->mId].mSortKey - data_020f3ff0[(*(ClothesItem **)b)->mId].mSortKey;
}

static s32 CompareClothesHeldFirst(void *a, void *b) {
    BOOL heldA = FALSE;
    BOOL heldB = FALSE;
    if ((*(ClothesItem **)a)->mCount > 0) {
        heldA = TRUE;
    }
    if ((*(ClothesItem **)b)->mCount > 0) {
        heldB = TRUE;
    }
    if (heldA != heldB) {
        return -(heldA - heldB);
    }
    return CompareClothes(a, b);
}

void ClothesList::GetOwned(Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        ClothesItem *item = mList[i];
        if (item->mOwned && kind == data_020f3ff0[item->mId].mKind) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void ClothesList::GetOnOffer(Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        ClothesItem *item = mList[i];
        BOOL owned = item->mOwned;
        const ClothesInfo *info = &data_020f3ff0[item->mId];
        BOOL ok = TRUE;
        ok &= kind == info->mKind;
        ok &= info->mAvailable0 == 1;
        BOOL ok2 = TRUE;
        if (!owned) {
            ok2 = info->mAvailable2 == 1;
        }
        if (ok & ok2) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void ClothesList::GetOnOfferInSeason(Item **list, int *count, int kind, s64 time) {
    RTCDate date;
    RTCTime clock;
    if (time != -1) {
        RTC_ConvertSecondToDateTime(&date, &clock, time);
    }
    *count = 0;
    if (func_02035aa4(data_02123440)) {
        return;
    }
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        const ClothesInfo *info = &data_020f3ff0[mList[i]->mId];
        BOOL ok = TRUE;
        ok &= kind == info->mKind;
        ok &= info->mAvailable0 == 1;
        if (time != -1) {
            if (info->mSeason == 0) {
                ok = FALSE;
            } else {
                ok &= IsClothesInSeason(mList[i]->mId, date.month);
            }
        }
        if (ok) {
            list[n++] = mList[i];
            (*count)++;
        }
    }
}

void ClothesList::GetHeld(Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        ClothesItem *item = mList[i];
        if (item->mCount > 0 && kind == data_020f3ff0[item->mId].mKind) {
            list[n++] = item;
            (*count)++;
        }
    }
}

BOOL ClothesList::HasAnyOwned() {
    BOOL result = FALSE;
    for (int i = 0; i < mCount; i++) {
        if (mList[i]->mOwned) {
            result = TRUE;
            break;
        }
    }
    return result;
}

// Reads the clothes the save data has into an array, from the first clothes on (the clothes 0 are not read)
void LoadClothes(ClothesItem *items, int max, int *count) {
    *count = 1;
    for (int i = 1; i < CLOTHES_SIZE && *count < max; i++) {
        int value = func_0202c784(&gGameData, i);
        if (value != -1) {
            items[*count].Clear();
            items[*count].mId = i;
            items[*count].mOwned = TRUE;
            items[*count].mUnknown0C = func_0202c770(&gGameData, i);
            items[*count].mCount = func_0202c770(&gGameData, i);
            items[*count].mUnknown14 = value;
            (*count)++;
        }
    }
    for (int i = 0; i < *count; i++) {
        items[i].Settle();
    }
}

// Writes an array of clothes to the save data, those that changed
void SaveClothes(ClothesItem *items, int count) {
    for (int i = 0; i < count; i++, items++) {
        items->Settle();
        u16 id = items->mId;
        if (items->mOwned &&
            (items->mUnknown14 != func_0202c784(&gGameData, id) || items->mCount != func_0202c770(&gGameData, id))) {
            func_0202c798(&gGameData, id, items->mCount);
            func_0202c7ac(&gGameData, id, items->mUnknown14);
        } else if (!items->mOwned && func_0202c784(&gGameData, id) != -1) {
            func_0202c798(&gGameData, id, 0);
            func_0202c7ac(&gGameData, id, -1);
        }
    }
}

// The weight of clothes in the random choice: 0 unless they can be offered in this way and in this month
static u8 GetClothesWeight(int clothes, int availability, int month) {
    u8 weight = 0;
    BOOL available = FALSE;
    switch (availability) {
    case 0:
        if (data_020f3ff0[clothes].mAvailable0) {
            available = TRUE;
        }
        break;
    case 1:
        if (data_020f3ff0[clothes].mAvailable1) {
            available = TRUE;
        }
        break;
    case 2:
        if (data_020f3ff0[clothes].mAvailable2) {
            available = TRUE;
        }
        break;
    }
    if (available) {
        switch (gGameData.mUnknown4E[0]) {
        case 0:
            weight = data_020f3ff0[clothes].mWeights[0];
            break;
        case 1:
            weight = data_020f3ff0[clothes].mWeights[1];
            break;
        case 2:
            weight = data_020f3ff0[clothes].mWeights[2];
            break;
        case 3:
            weight = data_020f3ff0[clothes].mWeights[3];
            break;
        }
        if (!IsClothesInSeason(clothes, month)) {
            weight = 0;
        }
    }
    return weight;
}

static s32 CompareClothesIds(void *a, void *b);

// Chooses count different clothes at random by their weights, sorts them and returns the first
u16 ChooseClothes(int availability, u16 *clothes, int count) {
    RTCDate date;
    u32 buffer[SORT_BUFFER_SIZE];
    BOOL free[CLOTHES_SIZE];
    Clock_GetDate(data_02123440, &date);
    for (int i = 0; i < CLOTHES_SIZE; i++) {
        free[i] = TRUE;
    }
    for (int n = 0; n < count; n++) {
        int total = 0;
        for (u16 i = 0; i < CLOTHES_SIZE; i++) {
            if (free[i] && data_020f3ff0[i].mAvailable2 != 1) {
                total += GetClothesWeight(i, availability, date.month);
            }
        }
        u16 choice = Rand(total);
        int sum = 0;
        u16 i = 0;
        for (;;) {
            if (free[i] && data_020f3ff0[i].mAvailable2 != 1) {
                sum += GetClothesWeight(i, availability, date.month);
            }
            if (choice < sum) {
                break;
            }
            i++;
        }
        free[i] = FALSE;
        clothes[n] = i;
    }
    MATH_QSort(clothes, count, sizeof(u16), CompareClothesIds, buffer);
    return clothes[0];
}

static s32 CompareClothesIds(void *a, void *b) {
    return data_020f3ff0[*(u16 *)a].mSortKey - data_020f3ff0[*(u16 *)b].mSortKey;
}

void Item::Clear() {
    mOwned = FALSE;
    mUnknown0C = 0;
    mCount = 0;
}

void Item::Settle() {
    if (mCount > 0 && mOwned == FALSE) {
        mOwned = TRUE;
    }
    if (mUnknown0C < 0) {
        mCount -= mUnknown0C;
        mUnknown0C = 0;
    }
}

BOOL Item::Add(int count) {
    if (mCount + count > 99 || count + mUnknown0C < 0) {
        return FALSE;
    }
    if (mOwned == FALSE) {
        mOwned = TRUE;
    }
    mCount += count;
    mUnknown0C += count;
    return TRUE;
}

InventoryItem::InventoryItem() {
    mId = 0;
}

void Inventory::Load() {
    mCount = INVENTORY_SIZE;
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        mItems[i].Clear();
        mItems[i].mId = i;
    }
    for (int i = 0; i < INVENTORY_SAVED; i++) {
        BOOL owned = TRUE;
        int count = func_0202c7dc(&gGameData, i);
        if (count == -1) {
            owned = FALSE;
            count = 0;
        }
        mItems[i].mOwned = owned;
        mItems[i].mUnknown0C = count;
        mItems[i].mCount = count;
    }
    for (int i = 0; i < mCount; i++) {
        mItems[i].Settle();
        mList[i] = &mItems[i];
    }
}

void Inventory::Save() {
    for (int i = 0; i < mCount; i++) {
        mItems[i].Settle();
    }
    for (int i = 0; i < INVENTORY_SAVED; i++) {
        if (mItems[i].mOwned != FALSE) {
            func_0202c7f0(&gGameData, mItems[i].mId, mItems[i].mCount);
        } else if (mItems[i].mOwned == FALSE) {
            func_0202c7f0(&gGameData, mItems[i].mId, -1);
        }
    }
}

static s32 CompareItems(void *a, void *b);
static s32 CompareItemsHeldFirst(void *a, void *b);

void Inventory::Sort() {
    u32 buffer[SORT_BUFFER_SIZE];
    MATH_QSort(mList, mCount, sizeof(InventoryItem *), CompareItems, buffer);
}

void Inventory::SortHeldFirst() {
    u32 buffer[SORT_BUFFER_SIZE];
    MATH_QSort(mList, mCount, sizeof(InventoryItem *), CompareItemsHeldFirst, buffer);
}

static s32 CompareItems(void *a, void *b) {
    return data_020f5f0c[(*(InventoryItem **)a)->mId].mSortKey - data_020f5f0c[(*(InventoryItem **)b)->mId].mSortKey;
}

static s32 CompareItemsHeldFirst(void *a, void *b) {
    BOOL heldA = FALSE;
    BOOL heldB = FALSE;
    if ((*(InventoryItem **)a)->mCount > 0) {
        heldA = TRUE;
    }
    if ((*(InventoryItem **)b)->mCount > 0) {
        heldB = TRUE;
    }
    if (heldA != heldB) {
        return -(heldA - heldB);
    }
    return CompareItems(a, b);
}

void Inventory::GetOwned(Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mOwned && kind == data_020f5f0c[item->mId].mKind) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetOwned(Item **list, int *count, int kind, u32 flags) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mOwned && kind == data_020f5f0c[item->mId].mKind && (flags & data_020f5f0c[item->mId].mFlags)) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetOwnedKinds4And5(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mOwned && (data_020f5f0c[item->mId].mKind == 4 || data_020f5f0c[item->mId].mKind == 5)) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetOwnedKind3(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mOwned && data_020f5f0c[item->mId].mKind == 3) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetOwnedKinds4And5Again(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mOwned && (data_020f5f0c[item->mId].mKind == 4 || data_020f5f0c[item->mId].mKind == 5)) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetHeld(Item **list, int *count, int kind, u32 flags) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mCount > 0 && kind == data_020f5f0c[item->mId].mKind && (flags & data_020f5f0c[item->mId].mFlags)) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetHeldKinds4And5(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        if (item->mCount > 0 && (data_020f5f0c[item->mId].mKind == 4 || data_020f5f0c[item->mId].mKind == 5)) {
            list[n++] = item;
            (*count)++;
        }
    }
}

void Inventory::GetListed(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InventoryItem *item = mList[i];
        int kind = data_020f5f0c[item->mId].mKind;
        if (kind != 0 && (item->mOwned || kind == 2)) {
            list[n++] = item;
            (*count)++;
        }
    }
}

BOOL Inventory::HasAnyOwnedKind1() {
    BOOL result = FALSE;
    for (int i = 0; i < mCount; i++) {
        if (data_020f5f0c[mItems[i].mId].mKind == 1 && mItems[i].mOwned) {
            result = TRUE;
            break;
        }
    }
    return result;
}

BOOL Inventory::HasAnyOwnedKind3() {
    BOOL result = FALSE;
    for (int i = 0; i < mCount; i++) {
        if (data_020f5f0c[mItems[i].mId].mKind == 3 && mItems[i].mOwned) {
            result = TRUE;
            break;
        }
    }
    return result;
}

BOOL Inventory::HasAnyOwnedKinds4And5() {
    BOOL result = FALSE;
    for (int i = 0; i < mCount; i++) {
        if ((data_020f5f0c[mItems[i].mId].mKind == 4 || data_020f5f0c[mItems[i].mId].mKind == 5) && mItems[i].mOwned) {
            result = TRUE;
            break;
        }
    }
    return result;
}

ItemModel::ItemModel() {
    STD_CopyString(mArchivePath, "Mdl/Item/item000_3D_LZ.bin");
    STD_CopyString(mModelPath, "XXX:/item.imd");
}

void ItemModel::SetItem(const char *archiveName, int item) {
    STD_CopyString(mArchiveName, archiveName);
    MI_CpuCopy8(mArchiveName, mModelPath, 3);
    mItem.mId = (u8)item;
    STD_CopyString(mArchivePath, "Mdl/Item/item");
    STD_ConcatenateString(mArchivePath, mItem.GetModelName());
    STD_ConcatenateString(mArchivePath, "_3D_LZ.bin");
}

void ItemModel::LoadModel(int resourceId) {
    mResourceId = resourceId;
    Model::Load(resourceId, 0);
    mBaseScale = FX32_ONE;
}

void ItemModel::LoadAnimation(int resourceId) {
    mBaseScale = FX32_ONE;
    void *anim = func_0202a5fc(this, RESOURCE_BCA0, 0);
    NNS_G3dRenderObjResetCallBack(&mRenderObj);
    RemoveAnims();
    func_02029c20(this, RESOURCE_BMD0, resourceId, 0);
    mRenderObj.ptrUser = this;
    func_0202a5d8(this, anim, RESOURCE_BCA0, 0);
}

void ItemModel::ResetScale() {
    mBaseScale = ((u32)data_020f5f0c[mItem.mId].mScale << FX32_SHIFT) / 10;
    mScale.x = mBaseScale;
    mScale.y = mBaseScale;
    mScale.z = mBaseScale;
}

void ItemModel::SetScale(fx32 scale) {
    mScale.x = FX_Mul(mBaseScale, scale);
    mScale.y = FX_Mul(mBaseScale, scale);
    mScale.z = FX_Mul(mBaseScale, scale);
}

// Reads the goods the save data has into an array
void LoadItems(InventoryItem *items, int max, int *count) {
    *count = 0;
    for (int i = 0; i < INVENTORY_SIZE && i < max; i++) {
        items[i].Clear();
        items[i].mId = i;
        (*count)++;
    }
    for (int i = 0; i < INVENTORY_SAVED && i < max; i++) {
        BOOL owned = TRUE;
        int value = func_0202c7dc(&gGameData, i);
        if (value == -1) {
            owned = FALSE;
            value = 0;
        }
        items[i].mOwned = owned;
        items[i].mUnknown0C = value;
        items[i].mCount = value;
    }
    for (int i = 0; i < *count; i++) {
        items[i].Settle();
    }
}

// The weight of goods in the random choice: 0 unless they can be offered in this way, are of this kind and of no
// season or this one
static u8 GetItemWeight(int item, int availability, int kind, int season) {
    u8 weight = 0;
    BOOL available = FALSE;
    switch (availability) {
    case 0:
        if (data_020f5f0c[item].mAvailable0) {
            available = TRUE;
        }
        break;
    case 1:
        if (data_020f5f0c[item].mAvailable1) {
            available = TRUE;
        }
        break;
    case 2:
        if (data_020f5f0c[item].mAvailable2) {
            available = TRUE;
        }
        break;
    }
    const ItemInfo *info = &data_020f5f0c[item];
    BOOL ofKind = FALSE;
    if (kind == info->mKind) {
        ofKind = TRUE;
    }
    if (available && ofKind) {
        switch (gGameData.mUnknown4E[0]) {
        case 0:
            weight = info->mWeights & 0xF;
            break;
        case 1:
            weight = (info->mWeights >> 4) & 0xF;
            break;
        case 2:
            weight = (info->mWeights >> 8) & 0xF;
            break;
        case 3:
            weight = (info->mWeights >> 12) & 0xF;
            break;
        }
        if (info->mSeason != 0 && season != info->mSeason) {
            weight = 0;
        }
    }
    return weight;
}

static u8 GetItemWeightInGroup(int item, int availability, int kind, int season, int group) {
    u8 weight = 0;
    if (group == data_020f5f0c[item].mGroup) {
        weight = GetItemWeight(item, availability, kind, season);
    }
    return weight;
}

// The seasons of the months for the three choices of goods
static const int sItemSeasons[12] = {4, 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4};
static const int sItemSeasons2[12] = {4, 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4};
static const int sItemSeasons3[12] = {4, 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4};
static const int sItemSeasons4[12] = {4, 4, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4};

// Chooses goods of a kind at random by their weights, other than one
u16 ChooseItem(int availability, int kind, int exclude) {
    RTCDate date;
    Clock_GetDate(data_02123440, &date);
    int total = 0;
    int season = sItemSeasons3[date.month - 1];
    for (u16 i = 0; i < INVENTORY_SIZE; i++) {
        if (exclude != i) {
            total += GetItemWeight(i, availability, kind, season);
        }
    }
    u16 choice = Rand(total);
    int sum = 0;
    u16 i = 0;
    for (;;) {
        if (i != exclude) {
            sum += GetItemWeight(i, availability, kind, season);
        }
        if (choice < sum) {
            break;
        }
        i++;
    }
    return i;
}

// The same among the goods of a group
u16 ChooseItemInGroup(int availability, int kind, int group) {
    RTCDate date;
    Clock_GetDate(data_02123440, &date);
    int total = 0;
    int season = sItemSeasons4[date.month - 1];
    for (u16 i = 0; i < INVENTORY_SIZE; i++) {
        total += GetItemWeightInGroup(i, availability, kind, season, group);
    }
    u16 choice = Rand(total);
    int sum = 0;
    u16 i = 0;
    for (;;) {
        sum += GetItemWeightInGroup(i, availability, kind, season, group);
        if (choice < sum) {
            break;
        }
        i++;
    }
    return i;
}

static s32 CompareItemIds(void *a, void *b);

// Chooses count different goods at random by their weights, sorts them and returns the first
u16 ChooseItems(int availability, int kind, u16 *items, int count) {
    RTCDate date;
    u32 buffer[SORT_BUFFER_SIZE];
    BOOL free[INVENTORY_SIZE];
    Clock_GetDate(data_02123440, &date);
    int season = sItemSeasons2[date.month - 1];
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        free[i] = TRUE;
    }
    for (int n = 0; n < count; n++) {
        int total = 0;
        for (u16 i = 0; i < INVENTORY_SIZE; i++) {
            if (free[i]) {
                total += GetItemWeight(i, availability, kind, season);
            }
        }
        u16 choice = Rand(total);
        int sum = 0;
        u16 i = 0;
        for (;;) {
            if (free[i]) {
                sum += GetItemWeight(i, availability, kind, season);
            }
            if (choice < sum) {
                break;
            }
            i++;
        }
        free[i] = FALSE;
        items[n] = i;
    }
    MATH_QSort(items, count, sizeof(u16), CompareItemIds, buffer);
    return items[0];
}

// The same, leaving out the goods with flag 0x1000
u16 ChooseItemsNot1000(int availability, int kind, u16 *items, int count) {
    RTCDate date;
    u32 buffer[SORT_BUFFER_SIZE];
    BOOL free[INVENTORY_SIZE];
    Clock_GetDate(data_02123440, &date);
    int season = sItemSeasons[date.month - 1];
    for (int i = 0; i < INVENTORY_SIZE; i++) {
        free[i] = TRUE;
    }
    for (int n = 0; n < count; n++) {
        int total = 0;
        for (u16 i = 0; i < INVENTORY_SIZE; i++) {
            if (free[i] && !(data_020f5f0c[i].mFlags & 0x1000)) {
                total += GetItemWeight(i, availability, kind, season);
            }
        }
        u16 choice = Rand(total);
        int sum = 0;
        u16 i = 0;
        for (;;) {
            if (free[i] && !(data_020f5f0c[i].mFlags & 0x1000)) {
                sum += GetItemWeight(i, availability, kind, season);
            }
            if (choice < sum) {
                break;
            }
            i++;
        }
        free[i] = FALSE;
        items[n] = i;
    }
    MATH_QSort(items, count, sizeof(u16), CompareItemIds, buffer);
    return items[0];
}

// Five goods that are always the same
static const u16 sFixedItems[5] = {0x42, 0x7D, 0x36, 0x17, 0x35};

u16 GetFixedItems(u16 *items, int count) {
    for (int i = 0; i < count; i++) {
        items[i] = sFixedItems[i];
    }
    return items[0];
}

static s32 CompareItemIds(void *a, void *b) {
    return data_020f5f0c[*(u16 *)a].mSortKey - data_020f5f0c[*(u16 *)b].mSortKey;
}
