#ifndef OV073_TUTORIALSKYBACKGROUND_H
#define OV073_TUTORIALSKYBACKGROUND_H

#include <nitro.h>
#include <nnsys.h>

#include "MainProgram.h"

class TutorialClock;

// The sky behind the apartment building: its background picture and palette depend on the building's size and
// on the time of day, and are loaded again when the time of day changes.
//
// The name is descriptive, not the original: no name of this class survives in the ROM. Overlay 74 has the same
// class without the prefix, which tells the two apart: the program is linked as one, so they cannot have one name.
class TutorialSkyBackground : public DayPhase {
public:
    enum State {
        STATE_NONE,
        STATE_WATCH,  // Wait for the time of day to change
        STATE_RELOAD, // Load the picture of the new time of day
    };

    TutorialSkyBackground();
    void Init(TutorialClock *clock, int buildingSize);
    void Refresh();
    void Update();
    void SetState(int state);
    void UpdateState();
    void UpdatePaths();
    void ApplyPalette();

    /* 0x28 */ TutorialClock *mClock;
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

#endif // OV073_TUTORIALSKYBACKGROUND_H
