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
    // SoundManager::Init sets the sound settings to their defaults when this is 3. Overlay 106 (PreStartScene) reads
    // it as the outcome of loading the save data: on 0 it goes on, on 1 it goes on as on a first start, on 2 it says
    // the data is broken and initialises it, on 3 and 4 it says the data could not be read.
    /* 0x08 */ int mUnknownState;
    /* 0x0C */ u8 mUnknown0C[0x41];
    // The order the list of Miis is sorted in, 0 to 6 (name, nickname, sex, year of birth, birthday, blood type,
    // personality): the main program's MiiListScene sorts its list by it and keeps the order chosen there
    /* 0x4D */ u8 mMiiListSortKey;
    /* 0x4E */ u8 mUnknown4E[2];
    // Kept between 0 and 9,999,999 by SetCappedCounter, which eight shop-like overlays call (56, 68, 75, 78, 82
    // to 85). Probably the player's money, but nothing in that function says so.
    /* 0x50 */ int mCappedCounter;
    /* 0x54 */ u8 mUnknown54[0x5D];
    // Overlay 106 (PreStartScene) sets it when it is 0 and then asks for scene 0x69; while it is set, it brings the
    // residents up to date and asks for scene 0x6A. It clears it when the save data was broken.
    /* 0xB1 */ u8 mUnknownB1;
    /* 0xB2 */ u8 mUnknownB2[0x10];
    /* 0xC2 */ u16 mTroublesSolved; // Overlay 99 shows it as the number of troubles solved ("悩み事解決数")
    /* 0xC4 */ u8 mUnknownC4[4];
    /* 0xC8 */ u8 mStereo;       // Not zero: stereo output (default 1)
    /* 0xC9 */ u8 mMusicVolume;  // 0 to 3, for sequences and streams (default 3)
    /* 0xCA */ u8 mEffectVolume; // 0 to 3, for sequence archive entries (default 3)
    /* 0xCB */ u8 mVoiceVolume;  // 0 to 3, for the speech synthesizer (default 3)
};

extern GameData gGameData;

#endif // GAMEDATA_H
