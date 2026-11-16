#include "OwnerInfo.h"

OwnerInfo gOwnerInfo;

void OwnerInfo::Init() {
    RTC_Init();
    OS_GetOwnerInfo(&mInfo);
    mFavoriteColor = OS_GetFavoriteColor(mInfo.favoriteColor);
    OS_GetMacAddress(mMacAddress);
}

// The time of day from the real-time clock
void OwnerInfo::GetTime(int *hour, int *minute, int *second) {
    RTCTime time;
    RTC_GetTime(&time);
    *hour = time.hour;
    *minute = time.minute;
    *second = time.second;
}
