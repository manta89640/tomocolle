#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <nitro.h>

// The global object gGameData, which the main program sets up at start and whose functions come just before the
// text drawing (0x0202b840 to 0x0202d5a4). Only the fields that decompiled code uses are known; the name says no
// more than that the game's settings and counters are in it.
class GameData {
public:
    void SetCappedCounter(int value);

    /* 0x00 */ u8 mUnknown00[8];
    /* 0x08 */ int mUnknownState; // SoundManager::Init sets the sound settings to their defaults when this is 3
    /* 0x0C */ u8 mUnknown0C[0x44];
    // Kept between 0 and 9,999,999 by SetCappedCounter, which eight shop-like overlays call (56, 68, 75, 78, 82
    // to 85). Probably the player's money, but nothing in that function says so.
    /* 0x50 */ int mCappedCounter;
    /* 0x54 */ u8 mUnknown54[0x74];
    /* 0xC8 */ u8 mStereo;       // Not zero: stereo output (default 1)
    /* 0xC9 */ u8 mMusicVolume;  // 0 to 3, for sequences and streams (default 3)
    /* 0xCA */ u8 mEffectVolume; // 0 to 3, for sequence archive entries (default 3)
    /* 0xCB */ u8 mVoiceVolume;  // 0 to 3, for the speech synthesizer (default 3)
};

extern GameData gGameData;

#endif // GAMEDATA_H
