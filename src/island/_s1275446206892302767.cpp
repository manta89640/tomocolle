#include <nitro.h>
#include <nnsys.h>
#include <math.h>

#include "Sprite.h"
#include "island/Sky.h"
#include "sound/SoundManager.h"

struct PaletteLoader;
struct ScreenLoader;

extern "C" {
extern void *data_02123440; // The game's clock
extern MATHRandContext16 gRandom;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
void func_02035870(void *clock, RTCDate *date, RTCTime *time); // The date and time, with the game's offset
void Clock_GetTime(void *clock, RTCTime *time);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, u16 offset, int a, int size);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_020145d8(char *dst, const char *src); // Appends a string
}

static inline u16 Rand(u16 max) {
    return MATH_Rand16(&gRandom, max);
}

// Points of the sky's objects; the aeroplane starts from sPoints[3] at a random height
static const ScreenPoint sPoints[8] = {
    {48, 48}, {800, 0}, {-2200, -250}, {-50, 64}, {264, 32}, {-50, 160}, {264, 160}, {-1800, 250},
};

// The colours of the 3D scenes' two lights through the day: the 16 phases of the dawn, then the 16 of the dusk
static const GXRgb sLightColors[32] = {
    0x7DEC, 0x7DEC, 0x7DEC, 0x7DEC, 0x6A92, 0x6A92, 0x6A92, 0x4B18, 0x4B18, 0x4B18, 0x4B18, 0x4B18,
    0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7F3B, 0x7F3B, 0x7F3B, 0x7F3B,
    0x7EB7, 0x7EB7, 0x7EB7, 0x7EB7, 0x7E52, 0x7E52, 0x6A92, 0x7DEC,
};

// Midnight, from which the time of day is counted
static const RTCTime sMidnight = {0, 0, 0};

static const CloudSettings sCloudSettings[1] = {
    {352, -112, 38, 17, -1100, -700, -600, -50, -300, 100, -80, 0, 70, 100, 150, 200, 39, 46, 0},
};

static const RTCTime sSunriseTimes[12] = {
    {6, 40, 0}, {6, 20, 0}, {6, 0, 0}, {5, 40, 0}, {5, 20, 0}, {5, 0, 0},
    {5, 20, 0}, {5, 40, 0}, {6, 0, 0}, {6, 20, 0}, {6, 40, 0}, {7, 0, 0},
};

// Sunrise and sunset in each month
static const RTCTime sSunsetTimes[12] = {
    {17, 20, 0}, {17, 40, 0}, {18, 0, 0},  {18, 20, 0}, {18, 40, 0}, {19, 0, 0},
    {18, 40, 0}, {18, 20, 0}, {18, 0, 0},  {17, 40, 0}, {17, 20, 0}, {17, 0, 0},
};

static const GXRgb sLightColors2[32] = {
    0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94, 0x7E94,
    0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x7FFF, 0x473F, 0x473F, 0x473F, 0x473F,
    0x3EBF, 0x3EBF, 0x3EBF, 0x3EBF, 0x629A, 0x629A, 0x7E94, 0x7E94,
};


static inline u32 ToSeconds(const RTCTime &time) {
    return time.hour * 3600 + time.minute * 60 + time.second;
}

// --- DayPhase ---------------------------------------------------------------------------------------------------

void DayPhase::Init(const RTCDate *date, const RTCTime *time) {
    mSunrise = 0;
    mSunset = 0;
    mPhase = 15;
    mDaytime = TRUE;
    mChanged = FALSE;
    mFine = TRUE;
    mChanging = FALSE;
    SetSunTimes(date);
    Compute(date, time);
    mFramesToCheck = 0x1C42;
}

void DayPhase::Update() {
    RTCDate date;
    RTCTime time;
    func_02035870(data_02123440, &date, &time);
    Update(&date, &time);
}

void DayPhase::Update(const RTCDate *date, const RTCTime *time) {
    BOOL changed = FALSE;
    mChanged = FALSE;
    if (--mFramesToCheck <= 0) {
        changed = Compute(date, time);
        mFramesToCheck = 0x1C42;
    }
    mChanged |= changed;
}

int DayPhase::GetPhase() {
    int phase = mPhase;
    if (!mDaytime) {
        phase += 16;
    }
    return phase;
}

BOOL DayPhase::Compute(const RTCDate *date, const RTCTime *time) {
    int oldPhase = mPhase;
    BOOL oldDaytime = mDaytime;
    s64 now = RTC_ConvertDateTimeToSecond(date, time) - RTC_ConvertDateTimeToSecond(date, &sMidnight);
    s64 dawn = mSunrise - 3840;
    s64 dusk = mSunset - 3840;
    s64 day = mSunrise + 3840;
    s64 night = mSunset + 3840;
    if (now >= dawn && now < day) {
        mPhase = (int)(now - dawn) >> 9;
        mDaytime = TRUE;
    } else if (now >= dusk && now < night) {
        mPhase = (int)(now - dusk) >> 9;
        mDaytime = FALSE;
    } else if (now >= day && now < dusk) {
        mPhase = 15;
        mDaytime = TRUE;
    } else {
        mPhase = 15;
        mDaytime = FALSE;
    }
    return mPhase != oldPhase || mDaytime != oldDaytime;
}

void DayPhase::SetSunTimes(const RTCDate *date) {
    mSunrise = GetSunrise(date->month, NULL);
    mSunset = GetSunset(date->month, NULL);
}

s64 GetSunrise(u32 month, RTCTime *time) {
    static const u32 sSeconds[12] = {
        ToSeconds(sSunriseTimes[0]), ToSeconds(sSunriseTimes[1]),  ToSeconds(sSunriseTimes[2]),
        ToSeconds(sSunriseTimes[3]), ToSeconds(sSunriseTimes[4]),  ToSeconds(sSunriseTimes[5]),
        ToSeconds(sSunriseTimes[6]), ToSeconds(sSunriseTimes[7]),  ToSeconds(sSunriseTimes[8]),
        ToSeconds(sSunriseTimes[9]), ToSeconds(sSunriseTimes[10]), ToSeconds(sSunriseTimes[11]),
    };
    if (month <= 0 || month > 12) {
        if (month > 12) {
            month = 12;
        } else if (month < 1) {
            month = 1;
        }
    }
    if (time != NULL) {
        *time = sSunriseTimes[month - 1];
    }
    return sSeconds[month - 1];
}

s64 GetSunset(u32 month, RTCTime *time) {
    static const u32 sSeconds[12] = {
        ToSeconds(sSunsetTimes[0]), ToSeconds(sSunsetTimes[1]),  ToSeconds(sSunsetTimes[2]),
        ToSeconds(sSunsetTimes[3]), ToSeconds(sSunsetTimes[4]),  ToSeconds(sSunsetTimes[5]),
        ToSeconds(sSunsetTimes[6]), ToSeconds(sSunsetTimes[7]),  ToSeconds(sSunsetTimes[8]),
        ToSeconds(sSunsetTimes[9]), ToSeconds(sSunsetTimes[10]), ToSeconds(sSunsetTimes[11]),
    };
    if (month <= 0 || month > 12) {
        if (month > 12) {
            month = 12;
        } else if (month < 1) {
            month = 1;
        }
    }
    if (time != NULL) {
        *time = sSunsetTimes[month - 1];
    }
    return sSeconds[month - 1];
}

GXRgb GetSkyLightColor(int phase) {
    if (phase > 31) {
        phase = 31;
    } else if (phase < 0) {
        phase = 0;
    }
    return sLightColors[phase];
}

GXRgb GetSkyLightColor2(int phase) {
    if (phase > 31) {
        phase = 31;
    } else if (phase < 0) {
        phase = 0;
    }
    return sLightColors2[phase];
}

// --- SkyPalette -------------------------------------------------------------------------------------------------

void SkyPalette::Init(const char *archiveName, int plane) {
    RTCDate date;
    RTCTime time;
    func_02035870(data_02123440, &date, &time);
    DayPhase::Init(&date, &time);
    MI_CpuFill8(mArchiveName, 0, sizeof(mArchiveName));
    mArchiveName[0] = archiveName[0];
    mArchiveName[1] = archiveName[1];
    mArchiveName[2] = archiveName[2];
    mPlane = plane;
    LoadPalette();
}

void SkyPalette::Update() {
    DayPhase::Update();
    if (mChanged) {
        LoadPalette();
    }
}

void SkyPalette::LoadPalette() {
    char path[0x20];
    MI_CpuFill8(path, 0, sizeof(path));
    if (mDaytime) {
        if (mChanging) {
            STD_CopyString(path, mArchiveName);
            func_020145d8(path, ":/Fw_BG_up_DAY_Chg.ncl");
        } else if (mFine) {
            STD_CopyString(path, mArchiveName);
            func_020145d8(path, ":/Fw_BG_up_AM_Fine.ncl");
        } else {
            STD_CopyString(path, mArchiveName);
            func_020145d8(path, ":/Fw_BG_up_AM_Cloud.ncl");
        }
    } else {
        if (mChanging) {
            STD_CopyString(path, mArchiveName);
            func_020145d8(path, ":/Fw_BG_up_NIGHT_Chg.ncl");
        } else if (mFine) {
            STD_CopyString(path, mArchiveName);
            func_020145d8(path, ":/Fw_BG_up_PM_Fine.ncl");
        } else {
            STD_CopyString(path, mArchiveName);
            func_020145d8(path, ":/Fw_BG_up_PM_Cloud.ncl");
        }
    }
    mPalette = func_02027534(&data_021213b8, path, mPlane, mPhase * 32, 0, 0x20);
}

// --- Sky --------------------------------------------------------------------------------------------------------

void Sky::Init(const char *archiveName, int resource, int objPlane, int bgPlane, BOOL palettes, BOOL clouds,
               BOOL birds, BOOL plane, BOOL moon) {
    mPalettesOn = palettes;
    mCloudsOn = clouds;
    mBirdsOn = birds;
    mPlaneOn = plane;
    mMoonOn = moon;
    mUnknown28 = TRUE;
    RTCDate date;
    RTCTime time;
    func_02035870(data_02123440, &date, &time);
    DayPhase::Init(&date, &time);
    MI_CpuFill8(mArchiveName, 0, sizeof(mArchiveName));
    mArchiveName[0] = archiveName[0];
    mArchiveName[1] = archiveName[1];
    mArchiveName[2] = archiveName[2];
    mResource = resource;
    mObjPlane = objPlane;
    mBgPlane = bgPlane;
    mScrollY = 0;
    LoadPalettes();
    InitClouds();
    ShuffleCloudFrames();
    if (mCloudsOn) {
        FillClouds();
    }
    mBirds.Init(mResource);
    InitPlane();
    InitMoon();
    if (mMoonOn) {
        SetMoonAge(&date);
        PlaceMoon();
    }
    gSoundManager.LoadGroup(0x46);
}

void Sky::Update(const RTCDate *date, const RTCTime *time) {
    RTCDate dateCopy = *date;
    RTCTime timeCopy;
    MI_CpuCopy32(date, &dateCopy, sizeof(RTCDate));
    MI_CpuCopy32(time, &timeCopy, sizeof(RTCTime));
    DayPhase::Update(&dateCopy, &timeCopy);
    UpdateObjects();
}

void Sky::Update() {
    DayPhase::Update();
    UpdateObjects();
}

void Sky::UpdateObjects() {
    if (mChanged && mPalettesOn) {
        LoadPalettes();
    }
    func_02028108(&data_02121834, mBgPlane, mScrollY >> FX32_SHIFT);
    if (mBirdsOn) {
        mBirds.Update();
    }
    if (mCloudsOn) {
        MoveClouds();
        ScreenPoint position = mLastCloud->GetPixelPosition();
        if (mCloudGap < mClouds.mSpawnX - position.x) {
            AddCloud();
        }
    }
    if (!mPlaneOn) {
        return;
    }
    MovePlane();
    if (--mPlaneTimer > 0) {
        return;
    }
    StartPlane();
    mPlaneTimer = Rand(18000) + 18000;
}

void Sky::Draw() {
    mBirds.Draw();
    for (int i = 0; i < mCloudCount; i++) {
        Sprite_Draw(&mCloudSprites[i]);
    }
    Sprite_Draw(&mPlane);
    Sprite_Draw(&mMoon);
}

void Sky::InitPlane() {
    Sprite_Init(&mPlane, mResource, 0x36, 0x80, 0x60, TRUE);
    mPlane.mOverwritePalette = 3;
    mPlane.mVisible = FALSE;
    mPlaneVelocity.x = 0;
    mPlaneVelocity.y = 0;
    mPlaneTempo = 0;
    mPlanePan = 0;
    mPlaneSoundFrames = 0;
    mPlaneMute = FALSE;
    if (Rand(20) == 0) {
        mPlaneTimer = Rand(180) + 120;
    } else {
        mPlaneTimer = Rand(18000) + 18000;
    }
}

void Sky::StartPlane() {
    if (mPlane.mVisible) {
        return;
    }
    ScreenPoint start = sPoints[3];
    start.y = Rand(0x60) + 0x40;
    mPlane.SetPosition(start.x << FX32_SHIFT, start.y << FX32_SHIFT);
    mPlaneVelocity.x = 800;
    mPlaneVelocity.y = 0;
    mPlane.mVisible = TRUE;
    mPlaneTempo = 200;
    mPlanePan = -20;
    mPlaneSoundFrames = 0;
    gSoundManager.SetSeqArcTempoRatio(3, mPlaneTempo);
    gSoundManager.SetSeqArcTrackPan(3, 0xFFFF, mPlanePan);
    gSoundManager.StartSeqArc(3, 3, 8);
    SetPlaneMute(mPlaneMute);
}

void Sky::MovePlane() {
    if (!mPlane.mVisible) {
        return;
    }
    NNSG2dFVec2 velocity = mPlaneVelocity;
    mPlane.mPosition.x += velocity.x;
    mPlane.mPosition.y += velocity.y;
    if (306 < mPlane.mPosition.x >> FX32_SHIFT) {
        mPlane.mVisible = FALSE;
        gSoundManager.StopSeqArc(3, 0x78);
    }
    if (++mPlaneSoundFrames != 20) {
        return;
    }
    mPlaneSoundFrames = 0;
    mPlanePan++;
    gSoundManager.SetSeqArcTrackPan(3, 0xFFFF, mPlanePan);
}

void Sky::SetPlaneMute(BOOL mute) {
    gSoundManager.SetSeqArcTrackMute(3, 0xFFFF, mute);
    mPlaneMute = mute;
}

void Sky::ResumePlaneSound() {
    if (!mPlane.mVisible) {
        return;
    }
    gSoundManager.SetSeqArcTempoRatio(3, mPlaneTempo);
    gSoundManager.SetSeqArcTrackPan(3, 0xFFFF, mPlanePan);
    gSoundManager.StartSeqArc(3, 3, 8);
}

void Sky::InitMoon() {
    Sprite_Init(&mMoon, mResource, 0x39, 0x80, 0x60, TRUE);
    mMoon.mOverwritePalette = 3;
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mMoon.mAnim));
    mMoon.mVisible = FALSE;
}

void Sky::SetMoonAge(const RTCDate *date) {
    mMoonAge = (((date->year + 10) % 19) * 11 + date->month + date->day) % 30;
    if (date->month == 1 || date->month == 2) {
        mMoonAge = (mMoonAge + 2) % 30;
    }
    mMoonFrame = (mMoonAge + 2) / 3;
    Sprite_SetFrame(&mMoon, mMoonFrame);
    mMoon.mVisible = TRUE;
}

void Sky::PlaceMoon() {
    RTCTime time;
    Clock_GetTime(data_02123440, &time);
    int minutes = (time.hour * 60 + time.minute + 1440 - (mMoonAge * 1440 / 30 + 360)) % 1440;
    mMoon.SetPosition((minutes * 256 / 1440) << FX32_SHIFT, FX32_CONST(48));
}

void Sky::SetMute(BOOL birds, BOOL plane) {
    mBirds.SetMute(birds);
    SetPlaneMute(plane);
}

void Sky::ResumeSound() {
    mBirds.ResumeSound();
    ResumePlaneSound();
}

void Sky::InitClouds() {
    LoadCloudSettings(0);
    mCloudCount = 8;
    for (u16 i = 0; i < mCloudCount; i++) {
        Sprite_Init(&mCloudSprites[i], mResource, 0x14, 0x80, 0x60, TRUE);
        mCloudSprites[i].mOverwritePalette = 3;
        mCloudSprites[i].mVisible = FALSE;
    }
}

void Sky::LoadCloudSettings(int index) {
    mClouds.mSpawnX = sCloudSettings[index].mSpawnX;
    mClouds.mVanishX = sCloudSettings[index].mVanishX;
    mClouds.mRowY = sCloudSettings[index].mRowY;
    mClouds.mRowHeight = sCloudSettings[index].mRowHeight;
    mClouds.mMinSpeed = sCloudSettings[index].mMinSpeed;
    mClouds.mMaxSpeed = sCloudSettings[index].mMaxSpeed;
    mClouds.mBaseSpeed = sCloudSettings[index].mBaseSpeed;
    mClouds.mRowSpeed = sCloudSettings[index].mRowSpeed;
    mClouds.mSpeedJitterMin = sCloudSettings[index].mSpeedJitterMin;
    mClouds.mSpeedJitterMax = sCloudSettings[index].mSpeedJitterMax;
    mClouds.mFirstXMin = sCloudSettings[index].mFirstXMin;
    mClouds.mFirstXMax = sCloudSettings[index].mFirstXMax;
    mClouds.mShortGapMin = sCloudSettings[index].mShortGapMin;
    mClouds.mShortGapMax = sCloudSettings[index].mShortGapMax;
    mClouds.mLongGapMin = sCloudSettings[index].mLongGapMin;
    mClouds.mLongGapMax = sCloudSettings[index].mLongGapMax;
    mClouds.mFirstFrame = sCloudSettings[index].mFirstFrame;
    mClouds.mEndFrame = sCloudSettings[index].mEndFrame;
    mClouds.mFrameCount = mClouds.mEndFrame - mClouds.mFirstFrame;
}

void Sky::ShuffleCloudFrames() {
    mNextCloudFrame = 0;
    for (int i = 0; i < mClouds.mFrameCount; i++) {
        mCloudFrames[i] = mClouds.mFirstFrame + i;
    }
    for (int i = 0; i < mClouds.mFrameCount * 2; i++) {
        u16 a = Rand(mClouds.mFrameCount);
        u16 b = Rand(mClouds.mFrameCount);
        u16 frame = mCloudFrames[a];
        mCloudFrames[a] = mCloudFrames[b];
        mCloudFrames[b] = frame;
    }
}

BOOL Sky::FindFreeCloud(int *index, int previous) {
    BOOL found = FALSE;
    int tries = 1;
    do {
        u16 i = Rand(8);
        if (!mCloudSprites[i].mVisible && fabs(previous - i) > 1.0) {
            *index = i;
            found = TRUE;
            break;
        }
    } while (tries++ < 10);
    return found;
}

void Sky::PlaceCloud(int index, int x, u16 *frame, ScreenPoint *position, int *speed, int *gap) {
    int range = mClouds.mSpeedJitterMax - mClouds.mSpeedJitterMin;
    if (range == 0) {
        range = 1;
    }
    *frame = mCloudFrames[mNextCloudFrame];
    if (++mNextCloudFrame >= mClouds.mFrameCount) {
        ShuffleCloudFrames();
    }
    position->x = x;
    position->y = mClouds.mRowY + index * mClouds.mRowHeight;
    *speed = mClouds.mBaseSpeed + mClouds.mRowSpeed * index + Rand(range) + mClouds.mSpeedJitterMin;
    if (*speed > mClouds.mMaxSpeed) {
        *speed = mClouds.mMaxSpeed;
    } else if (*speed < mClouds.mMinSpeed) {
        *speed = mClouds.mMinSpeed;
    }
    if (Rand(20) != 0) {
        *gap = mClouds.mShortGapMin + Rand(mClouds.mShortGapMax - mClouds.mShortGapMin);
    } else {
        *gap = mClouds.mLongGapMin + Rand(mClouds.mLongGapMax - mClouds.mLongGapMin);
    }
}

void Sky::StartCloud(int index, u16 frame, ScreenPoint position, int speed, int gap) {
    Sprite *sprite = &mCloudSprites[index];
    Sprite_Init(sprite, mResource, frame, 0x80, 0x60, TRUE);
    mCloudSprites[index].mOverwritePalette = 3;
    mCloudSprites[index].SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    mCloudSpeeds[index] = speed;
    mCloudSprites[index].mVisible = TRUE;
    mLastCloud = sprite;
    mCloudGap = gap;
}

void Sky::MoveClouds() {
    for (int i = 0; i < mCloudCount; i++) {
        if (mCloudSprites[i].mVisible) {
            mCloudSprites[i].mPosition.x += mCloudSpeeds[i];
        }
        if (mClouds.mVanishX > mCloudSprites[i].mPosition.x >> FX32_SHIFT) {
            mCloudSprites[i].mVisible = FALSE;
        }
    }
}

void Sky::FillClouds() {
    int x = mClouds.mFirstXMin + Rand(mClouds.mFirstXMax - mClouds.mFirstXMin);
    int index;
    ScreenPoint position;
    int speed;
    int gap;
    u16 frame;
    FindFreeCloud(&index, 0xFF);
    PlaceCloud(index, x, &frame, &position, &speed, &gap);
    StartCloud(index, frame, position, speed, gap);
    mLastCloudIndex = index;
    x = mCloudGap + mLastCloud->GetPixelPosition().x;
    while (x < mClouds.mSpawnX) {
        if (!FindFreeCloud(&index, mLastCloudIndex)) {
            x += 20;
            continue;
        }
        PlaceCloud(index, x, &frame, &position, &speed, &gap);
        StartCloud(index, frame, position, speed, gap);
        mLastCloudIndex = index;
        x = mCloudGap + mLastCloud->GetPixelPosition().x;
    }
}

void Sky::AddCloud() {
    int index;
    ScreenPoint position;
    int speed;
    int gap;
    u16 frame;
    if (!FindFreeCloud(&index, mLastCloudIndex)) {
        return;
    }
    PlaceCloud(index, mClouds.mSpawnX, &frame, &position, &speed, &gap);
    StartCloud(index, frame, position, speed, gap);
    mLastCloudIndex = index;
}

void Sky::LoadPalettes() {
    char objPath[0x20];
    char bgPath[0x20];
    MI_CpuFill8(objPath, 0, sizeof(objPath));
    MI_CpuFill8(bgPath, 0, sizeof(bgPath));
    if (mDaytime) {
        if (mChanging) {
            STD_CopyString(objPath, mArchiveName);
            func_020145d8(objPath, ":/Fw_Obj_up_DAY_Chg.ncl");
            STD_CopyString(bgPath, mArchiveName);
            func_020145d8(bgPath, ":/Fw_BG_up_DAY_Chg.ncl");
        } else if (mFine) {
            STD_CopyString(objPath, mArchiveName);
            func_020145d8(objPath, ":/Fw_Obj_up_AM_Fine.ncl");
            STD_CopyString(bgPath, mArchiveName);
            func_020145d8(bgPath, ":/Fw_BG_up_AM_Fine.ncl");
        } else {
            STD_CopyString(objPath, mArchiveName);
            func_020145d8(objPath, ":/Fw_Obj_up_AM_Cloud.ncl");
            STD_CopyString(bgPath, mArchiveName);
            func_020145d8(bgPath, ":/Fw_BG_up_AM_Cloud.ncl");
        }
    } else {
        if (mChanging) {
            STD_CopyString(objPath, mArchiveName);
            func_020145d8(objPath, ":/Fw_Obj_up_NIGHT_Chg.ncl");
            STD_CopyString(bgPath, mArchiveName);
            func_020145d8(bgPath, ":/Fw_BG_up_NIGHT_Chg.ncl");
        } else if (mFine) {
            STD_CopyString(objPath, mArchiveName);
            func_020145d8(objPath, ":/Fw_Obj_up_PM_Fine.ncl");
            STD_CopyString(bgPath, mArchiveName);
            func_020145d8(bgPath, ":/Fw_BG_up_PM_Fine.ncl");
        } else {
            STD_CopyString(objPath, mArchiveName);
            func_020145d8(objPath, ":/Fw_Obj_up_PM_Cloud.ncl");
            STD_CopyString(bgPath, mArchiveName);
            func_020145d8(bgPath, ":/Fw_BG_up_PM_Cloud.ncl");
        }
    }
    mObjPalette = func_02027534(&data_021213b8, objPath, mObjPlane, mPhase * 32, 0, 0x20);
    mBgPalette = func_02027534(&data_021213b8, bgPath, mBgPlane, mPhase * 32, 0, 0x20);
}

// --- SkyBirds ---------------------------------------------------------------------------------------------------

void SkyBirds::Init(int resource) {
    mResource = resource;
    mFlying = FALSE;
    mCount = 0;
    for (int i = 0; i < 10; i++) {
        Sprite_Init(&mSprites[i], mResource, 0x37, 0x80, 0x60, TRUE);
        mSprites[i].mOverwritePalette = 3;
        mSprites[i].mVisible = FALSE;
        mVelocities[i].x = 0;
        mVelocities[i].y = 0;
    }
    mTimer = Rand(1200) + 1200;
    mVolume = 0;
    mPitch = 0;
    mPan = 0;
    mSoundFrames = 0;
    mMute = FALSE;
}

void SkyBirds::Update() {
    Move();
    if (--mTimer > 0) {
        return;
    }
    Start();
    mTimer = Rand(1200) + 1200;
}

void SkyBirds::Draw() {
    for (int i = 0; i < 10; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

void SkyBirds::Start() {
    if (mFlying) {
        return;
    }
    mCount = Rand(4) + 1;
    int vx = Rand(400) - 2200;
    int vy = Rand(500) - 250;
    int x = 0;
    for (int i = 0; i < mCount; i++) {
        if (Rand(4) != 0) {
            Sprite_Init(&mSprites[i], mResource, 0x37, 0x80, 0x60, TRUE);
            Sprite_SetFrame(&mSprites[i], Rand(9) + 1);
        } else {
            Sprite_Init(&mSprites[i], mResource, 0x38, 0x80, 0x60, TRUE);
            Sprite_SetFrame(&mSprites[i], Rand(69) + 1);
        }
        mSprites[i].SetPosition((x + 264) << FX32_SHIFT, (Rand(128) + 32) << FX32_SHIFT);
        mSprites[i].mVisible = TRUE;
        mVelocities[i].x = vx;
        mVelocities[i].y = vy;
        x += Rand(40) + 10;
    }
    if (mCount == 1) {
        gSoundManager.StartSeqArc(2, 3, 6);
    } else {
        gSoundManager.StartSeqArc(2, 3, 7);
    }
    mVolume = 60;
    mPitch = 60;
    mPan = 30;
    mSoundFrames = 0;
    mVolumeStep = 1;
    gSoundManager.SetSeqArcVolume(2, mVolume);
    gSoundManager.SetSeqArcTrackPitch(2, 0xFFFF, mPitch);
    gSoundManager.SetSeqArcTrackPan(2, 0xFFFF, mPan);
    SetMute(mMute);
    mFlying = TRUE;
}

void SkyBirds::Move() {
    if (!mFlying) {
        return;
    }
    int flying = 0;
    for (int i = 0; i < mCount; i++) {
        if (mSprites[i].mVisible) {
            NNSG2dFVec2 velocity = mVelocities[i];
            mSprites[i].mPosition.x += velocity.x;
            mSprites[i].mPosition.y += velocity.y;
            flying++;
        }
        if (-8 > mSprites[i].mPosition.x >> FX32_SHIFT) {
            mSprites[i].mVisible = FALSE;
        }
    }
    if (flying == 0) {
        mFlying = FALSE;
        gSoundManager.StopSeqArc(2, 0x78);
        return;
    }
    if (++mSoundFrames == 5 || mSoundFrames == 10) {
        mVolume += mVolumeStep;
        if (mVolume > 127) {
            mVolume = 127;
        } else if (mVolume < 0) {
            mVolume = 0;
        }
        mPitch--;
        gSoundManager.SetSeqArcVolume(2, mVolume);
        gSoundManager.SetSeqArcTrackPitch(2, 0xFFFF, mPitch);
    }
    if (mSoundFrames != 10) {
        return;
    }
    mSoundFrames = 0;
    mPan--;
    gSoundManager.SetSeqArcTrackPan(2, 0xFFFF, mPan);
    if (mPan == 0) {
        mVolumeStep = -1;
    }
}

void SkyBirds::SetMute(BOOL mute) {
    gSoundManager.SetSeqArcTrackMute(2, 0xFFFF, mute);
    mMute = mute;
}

void SkyBirds::ResumeSound() {
    if (!mFlying) {
        return;
    }
    if (mCount == 1) {
        gSoundManager.StartSeqArc(2, 3, 6);
    } else {
        gSoundManager.StartSeqArc(2, 3, 7);
    }
    gSoundManager.SetSeqArcVolume(2, mVolume);
    gSoundManager.SetSeqArcTrackPitch(2, 0xFFFF, mPitch);
    gSoundManager.SetSeqArcTrackPan(2, 0xFFFF, mPan);
}
