#include <nitro.h>

#include "GameData.h"

// The shops' stock: what the island's shops sell on a day. The saved game holds two records of it (GameData_GetShopStock
// reads one, GameData_SetShopStock writes one): record 0 is today's, record 1 the next day's.
struct ShopStock {
    void MakeFirst();
    void Make();
    void SetDate(const RTCDate *date);

    /* 0x00 */ RTCDate mDate;       // The day the stock is for
    /* 0x10 */ u16 mClothes[5];
    /* 0x1A */ u16 mGoods1[5];      // Goods of kind 1 (on the first day a fixed list)
    /* 0x24 */ u16 mGoods4[5];      // Goods of kind 4
    /* 0x2E */ u8 mInteriors[4];    // Three interiors
    /* 0x32 */ u16 mGood;           // One more of kind 1
}; // 0x34

extern "C" {
extern void *gGameClock;
s64 Clock_GetSeconds(void *clock);
int func_02035aa4(void *clock);
void GameData_GetShopStock(GameData *game, int index, ShopStock *stock);
void GameData_SetShopStock(GameData *game, int index, const ShopStock *stock);
}

// The item files' pickers (main program, 0x0209e140 on)
u16 ChooseClothes(int availability, u16 *clothes, int count);
u16 ChooseItems(int availability, int kind, u16 *items, int count);
u16 ChooseItemsNot1000(int availability, int kind, u16 *items, int count);
u16 GetFixedItems(u16 *items, int count);
u8 ChooseInteriors(u8 *interiors, int count);

void ShopStock::MakeFirst() {
    ChooseClothes(0, mClothes, 5);
    GetFixedItems(mGoods1, 5);
    ChooseItemsNot1000(0, 4, mGoods4, 5);
    ChooseInteriors(mInteriors, 3);
    ChooseItems(0, 1, &mGood, 1);
}

void ShopStock::Make() {
    ChooseClothes(0, mClothes, 5);
    ChooseItemsNot1000(0, 1, mGoods1, 5);
    ChooseItemsNot1000(0, 4, mGoods4, 5);
    ChooseInteriors(mInteriors, 3);
    ChooseItems(0, 1, &mGood, 1);
}

void ShopStock::SetDate(const RTCDate *date) {
    mDate = *date;
}

// The date of the game's day that a time falls on: a day starts at 5 in the morning.
void GetGameDate(s64 seconds, RTCDate *date) {
    RTCTime time;
    RTC_ConvertSecondToDateTime(date, &time, seconds);
    if (time.hour >= 5) {
        return;
    }
    RTC_ConvertSecondToDateTime(date, &time, seconds - 86400);
}

static inline RTCTime Midnight() {
    RTCTime time = {0, 0, 0};
    return time;
}

// The number of the day since 2000-01-01
int GetDayNumber(const RTCDate *date) {
    RTCTime time = Midnight();
    return RTC_ConvertDateTimeToSecond(date, &time) / 86400;
}

void InitShopStocks() {
    ShopStock first;
    ShopStock next;
    RTCDate today;
    GetGameDate(Clock_GetSeconds(gGameClock), &today);
    first.MakeFirst();
    first.SetDate(&today);
    next.Make();
    next.SetDate(&today);
    GameData_SetShopStock(&gGameData, 0, &first);
    GameData_SetShopStock(&gGameData, 1, &next);
}

void UpdateShopStocks() {
    ShopStock current;
    ShopStock next;
    RTCDate stockDate;
    RTCDate today;
    GameData_GetShopStock(&gGameData, 0, &current);
    GameData_GetShopStock(&gGameData, 1, &next);
    if (current.mClothes[0] == 0 || current.mInteriors[0] == 0 || next.mClothes[0] == 0 || next.mInteriors[0] == 0) {
        InitShopStocks();
        GameData_GetShopStock(&gGameData, 0, &current);
        GameData_GetShopStock(&gGameData, 1, &next);
    }
    stockDate = current.mDate;
    GetGameDate(Clock_GetSeconds(gGameClock), &today);
    int todayNumber = GetDayNumber(&today);
    int stockNumber = GetDayNumber(&stockDate);
    BOOL update = TRUE;
    BOOL sameMonth = TRUE;
    if (stockDate.year == today.year && stockDate.month == today.month) {
        sameMonth = FALSE;
    }
    if (!sameMonth && stockDate.day == today.day) {
        update = FALSE;
    }
    update &= (todayNumber - stockNumber != -1);
    if (stockDate.year == 0 && stockDate.month == 1 && stockDate.day == 1 && today.year == 99 && today.month == 12 &&
        today.day == 31) {
        update = FALSE;
    }
    if (!update) {
        return;
    }
    if (func_02035aa4(gGameClock) != 0) {
        return;
    }
    next.SetDate(&today);
    GameData_SetShopStock(&gGameData, 0, &next);
    next.Make();
    next.SetDate(&today);
    GameData_SetShopStock(&gGameData, 1, &next);
}
