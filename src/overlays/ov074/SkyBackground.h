#ifndef SKYBACKGROUND_H
#define SKYBACKGROUND_H

#include <nitro.h>
#include <nnsys.h>

class Clock;

// The main program's time-of-day tracker (func_020a612c sets it up from a date and time, func_020a61cc updates
// it about every two minutes: it reloads its counter with 0x1C42 frames). Not decompiled: a stand-in with the
// fields the sky uses.
struct DayPhase {
    /* 0x00 */ u32 mChanged; // Not 0 in the frame the phase changed
    /* 0x04 */ u8 mUnknown04[0x10];
    /* 0x14 */ int mFramesToCheck;
    /* 0x18 */ int mPhase;       // func_020a6214 returns it, plus 16 when mUnknown1C is 0
    /* 0x1C */ int mUnknown1C;
    /* 0x20 */ u8 mUnknown20[8];
}; // 0x28

// The sky behind the apartment building: its background picture and palette depend on the building's size and
// on the time of day, and are loaded again when the time of day changes.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SkyBackground : public DayPhase {
public:
    enum State {
        STATE_NONE,
        STATE_WATCH,  // Wait for the time of day to change
        STATE_RELOAD, // Load the picture of the new time of day
    };

    SkyBackground();
    void Init(Clock *clock, int buildingSize);
    void Refresh();
    void Update();
    void SetState(int state);
    void UpdateState();
    void UpdatePaths();
    void ApplyPalette();

    /* 0x28 */ Clock *mClock;
    /* 0x2C */ int mState;
    /* 0x30 */ u16 *mColors;      // By time of day: what ApplyPalette gives to func_020278f8
    /* 0x34 */ void *mPalette;    // What loading mPalettePath returned
    /* 0x38 */ s16 mStep;
    /* 0x3A */ u8 mBuildingSize;
    /* 0x3B */ u8 mUnknown3B;
    /* 0x3C */ char mArchivePath[0x1C]; // "Scn/AFwBG/A0FwFBG0_LZ.bin": building size, then picture number
    /* 0x58 */ char mCharPath[0x10];    // "ASX:/Fw_BG0.ncg": picture number
    /* 0x68 */ char mPalettePath[0x10]; // "ASX:/Fw_BG0.ncl": picture number
}; // 0x78

#endif // SKYBACKGROUND_H
