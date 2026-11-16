#ifndef ISLAND_SKY_H
#define ISLAND_SKY_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"

// The sky of the island's scenes (src/island/Sky.cpp): the time of day, and the upper screen's sky with its
// palettes by time of day and weather, its clouds, a flock of birds, an aeroplane and the moon.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The time of day, worked out from the real-time clock about every two minutes. Around sunrise and sunset (64
// minutes either side) the phase steps from 0 to 15, every 512 seconds; in between it is 15. A scene's sky
// derives from it (overlays 73 and 74 have their own classes over it).
class DayPhase {
public:
    void Init(const RTCDate *date, const RTCTime *time);
    void Update();
    void Update(const RTCDate *date, const RTCTime *time);
    int GetPhase();
    BOOL Compute(const RTCDate *date, const RTCTime *time);
    void SetSunTimes(const RTCDate *date);

    /* 0x00 */ BOOL mChanged;       // Set in the frame in which the phase changed
    /* 0x04 */ s64 mSunrise;        // Seconds since midnight
    /* 0x0C */ s64 mSunset;
    /* 0x14 */ int mFramesToCheck;  // Until the phase is worked out again
    /* 0x18 */ int mPhase;          // 0 to 15
    /* 0x1C */ BOOL mDaytime;       // Between the middle of the dawn and the middle of the dusk
    /* 0x20 */ BOOL mFine;          // The weather: the "Fine" palettes rather than the "Cloud" ones (1 at first)
    /* 0x24 */ BOOL mChanging;      // The "Chg" palettes (0 at first; nothing in the main program sets it)
}; // 0x28

// The time of sunrise and sunset in a month (1 to 12), in seconds since midnight, and as a time if asked
s64 GetSunrise(u32 month, RTCTime *time);
s64 GetSunset(u32 month, RTCTime *time);
// The colours of the two lights of the 3D scenes for a phase (0 to 31, as DayPhase::GetPhase gives it)
GXRgb GetSkyLightColor(int phase);
GXRgb GetSkyLightColor2(int phase);

// A sky that is only a palette, loaded by time of day ("XXX:/Fw_BG_up_*.ncl"): the shops' upper screen.
class SkyPalette : public DayPhase {
public:
    void Init(const char *archiveName, int plane);
    void Update();
    void LoadPalette();

    /* 0x28 */ char mArchiveName[4];
    /* 0x2C */ const GXRgb *mPalette;
    /* 0x30 */ int mPlane;
}; // 0x34

// The flock of birds that flies across the sky now and then, with their sound.
class SkyBirds {
public:
    void Init(int resource);
    void Update();
    void Draw();
    void Start();
    void Move();
    void SetMute(BOOL mute);
    void ResumeSound();

    /* 0x000 */ int mResource;
    /* 0x004 */ BOOL mFlying;
    /* 0x008 */ int mCount;
    /* 0x00C */ Sprite mSprites[10];
    /* 0x6C4 */ NNSG2dFVec2 mVelocities[10];
    /* 0x714 */ int mTimer;         // Frames until the next flock
    /* 0x718 */ int mVolume;
    /* 0x71C */ int mPitch;
    /* 0x720 */ int mPan;
    /* 0x724 */ int mSoundFrames;
    /* 0x728 */ int mVolumeStep;
    /* 0x72C */ BOOL mMute;
}; // 0x730

// What the clouds of a sky are like (the one entry of sCloudSettings). Positions in pixels, speeds in 20.12 fixed
// point per frame, gaps in pixels between a cloud and the next.
struct CloudSettings {
    /* 0x00 */ int mSpawnX;          // Where a cloud appears (352: they drift to the left)
    /* 0x04 */ int mVanishX;         // Where it is gone
    /* 0x08 */ int mRowY;            // The y of the first of the eight rows; each cloud has its row
    /* 0x0C */ int mRowHeight;
    /* 0x10 */ int mMinSpeed;
    /* 0x14 */ int mMaxSpeed;
    /* 0x18 */ int mBaseSpeed;       // The speed of the first row
    /* 0x1C */ int mRowSpeed;        // Added for each row further down
    /* 0x20 */ int mSpeedJitterMin;  // A random amount from here up to mSpeedJitterMax is added
    /* 0x24 */ int mSpeedJitterMax;
    /* 0x28 */ int mFirstXMin;       // Where the first cloud is when the sky is set up
    /* 0x2C */ int mFirstXMax;
    /* 0x30 */ int mShortGapMin;     // The gap to the next cloud, 19 times in 20
    /* 0x34 */ int mShortGapMax;
    /* 0x38 */ int mLongGapMin;      // and once in 20
    /* 0x3C */ int mLongGapMax;
    /* 0x40 */ u16 mFirstFrame;      // The cloud pictures are the frames from here up to mEndFrame
    /* 0x42 */ u16 mEndFrame;
    /* 0x44 */ int mFrameCount;      // Worked out when the settings are loaded
}; // 0x48

// The sky of the upper screen of the island's map and of other scenes (overlays 20, 34, 57, 72, 79).
class Sky : public DayPhase {
public:
    void Init(const char *archiveName, int resource, int objPlane, int bgPlane, BOOL palettes, BOOL clouds,
              BOOL birds, BOOL plane, BOOL moon);
    void Update(const RTCDate *date, const RTCTime *time);
    void Update();
    void UpdateObjects();
    void Draw();
    void InitPlane();
    void StartPlane();
    void MovePlane();
    void SetPlaneMute(BOOL mute);
    void ResumePlaneSound();
    void InitMoon();
    void SetMoonAge(const RTCDate *date);
    void PlaceMoon();
    void SetMute(BOOL birds, BOOL plane);
    void ResumeSound();
    void InitClouds();
    void LoadCloudSettings(int index);
    void ShuffleCloudFrames();
    BOOL FindFreeCloud(int *index, int previous);
    void PlaceCloud(int index, int x, u16 *frame, ScreenPoint *position, int *speed, int *gap);
    void StartCloud(int index, u16 frame, ScreenPoint position, int speed, int gap);
    void MoveClouds();
    void FillClouds();
    void AddCloud();
    void LoadPalettes();

    /* 0x028 */ BOOL mUnknown28;    // 1 after Init
    /* 0x02C */ char mArchiveName[4];
    /* 0x030 */ int mResource;      // The bank of the sprites' cells
    /* 0x034 */ int mObjPlane;
    /* 0x038 */ int mBgPlane;
    /* 0x03C */ fx32 mScrollY;
    /* 0x040 */ const GXRgb *mObjPalette;
    /* 0x044 */ const GXRgb *mBgPalette; // The colours of the loaded palette: [1] is the 3D clear colour
    /* 0x048 */ BOOL mPalettesOn;   // Load the palettes of the new time of day when it changes
    /* 0x04C */ BOOL mCloudsOn;
    /* 0x050 */ BOOL mBirdsOn;
    /* 0x054 */ BOOL mPlaneOn;
    /* 0x058 */ BOOL mMoonOn;
    /* 0x05C */ CloudSettings mClouds;
    /* 0x0A4 */ u16 mCloudFrames[10]; // A shuffled list of the cloud pictures
    /* 0x0B8 */ int mNextCloudFrame;
    /* 0x0BC */ int mCloudCount;
    /* 0x0C0 */ Sprite mCloudSprites[8];
    /* 0x620 */ int mCloudSpeeds[8];
    /* 0x640 */ Sprite *mLastCloud;
    /* 0x644 */ int mLastCloudIndex;
    /* 0x648 */ int mCloudGap;
    /* 0x64C */ SkyBirds mBirds;
    /* 0xD7C */ Sprite mPlane;
    /* 0xE28 */ NNSG2dFVec2 mPlaneVelocity;
    /* 0xE30 */ int mPlaneTimer;    // Frames until the next flight
    /* 0xE34 */ int mPlaneTempo;
    /* 0xE38 */ int mPlanePan;
    /* 0xE3C */ int mPlaneSoundFrames;
    /* 0xE40 */ BOOL mPlaneMute;
    /* 0xE44 */ Sprite mMoon;
    /* 0xEF0 */ int mMoonAge;       // Days since the new moon, 0 to 29
    /* 0xEF4 */ int mMoonFrame;
}; // 0xEF8

#endif // ISLAND_SKY_H
