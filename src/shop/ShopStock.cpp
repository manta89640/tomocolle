#include <nitro.h>

#include "GameData.h"

// The shops' stock: what the island's shops sell on a day. The saved game holds two records of it (func_0202c864
// reads one, func_0202c878 writes one): record 0 is today's, record 1 the next day's.
struct ShopStock {
    void MakeFirst();
    void Make();
    void SetDate(const RTCDate *date);

    /* 0x00 */ RTCDate mDate;       // The day the stock is for
    /* 0x10 */ u16 mCostumes[5];    // func_0209eb54 picks them
    /* 0x1A */ u16 mUnknown1A[5];
    /* 0x24 */ u16 mUnknown24[5];
    /* 0x2E */ u8 mInteriors[4];    // func_020a5a28 picks them
    /* 0x32 */ u16 mUnknown32;
}; // 0x34

extern "C" {
extern void *data_02123440;
s64 Clock_GetSeconds(void *clock);
int func_02035aa4(void *clock);
void func_0202c864(GameData *game, int index, ShopStock *stock);
void func_0202c878(GameData *game, int index, const ShopStock *stock);
void func_0209eb54(int a, u16 *list, int count);
u16 func_020a0010(u16 *list, int count);
void func_0209fe48(int a, int b, u16 *list, int count);
void func_020a5a28(u8 *list, int count);
void func_0209fcc4(int a, int b, u16 *list, int count);
}

void ShopStock::MakeFirst() {
    func_0209eb54(0, mCostumes, 5);
    func_020a0010(mUnknown1A, 5);
    func_0209fe48(0, 4, mUnknown24, 5);
    func_020a5a28(mInteriors, 3);
    func_0209fcc4(0, 1, &mUnknown32, 1);
}

void ShopStock::Make() {
    func_0209eb54(0, mCostumes, 5);
    func_0209fe48(0, 1, mUnknown1A, 5);
    func_0209fe48(0, 4, mUnknown24, 5);
    func_020a5a28(mInteriors, 3);
    func_0209fcc4(0, 1, &mUnknown32, 1);
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
    GetGameDate(Clock_GetSeconds(data_02123440), &today);
    first.MakeFirst();
    first.SetDate(&today);
    next.Make();
    next.SetDate(&today);
    func_0202c878(&gGameData, 0, &first);
    func_0202c878(&gGameData, 1, &next);
}

void UpdateShopStocks() {
    ShopStock current;
    ShopStock next;
    RTCDate stockDate;
    RTCDate today;
    func_0202c864(&gGameData, 0, &current);
    func_0202c864(&gGameData, 1, &next);
    if (current.mCostumes[0] == 0 || current.mInteriors[0] == 0 || next.mCostumes[0] == 0 || next.mInteriors[0] == 0) {
        InitShopStocks();
        func_0202c864(&gGameData, 0, &current);
        func_0202c864(&gGameData, 1, &next);
    }
    stockDate = current.mDate;
    GetGameDate(Clock_GetSeconds(data_02123440), &today);
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
    if (func_02035aa4(data_02123440) != 0) {
        return;
    }
    next.SetDate(&today);
    func_0202c878(&gGameData, 0, &next);
    next.Make();
    next.SetDate(&today);
    func_0202c878(&gGameData, 1, &next);
}
