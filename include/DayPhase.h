#ifndef DAYPHASE_H
#define DAYPHASE_H

#include <nitro.h>

// The main program's time-of-day tracker (not decompiled; func_020a6214 reads it): a STAND-IN with the fields that
// overlays 72, 73 and 74 touch. The island map's sky and the apartment's sky derive from it. One definition for
// every scene header (the scene constructors of src/scene/SceneManager.cpp include several at once).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
struct DayPhase {
    /* 0x00 */ u32 mChanged; // Not 0 in the frame the phase changed
    /* 0x04 */ u8 mUnknown04[0x10];
    /* 0x14 */ int mFramesToCheck;
    /* 0x18 */ int mPhase;       // func_020a6214 returns it, plus 16 when mUnknown1C is 0
    /* 0x1C */ int mUnknown1C;
    /* 0x20 */ u8 mUnknown20[8];
}; // 0x28

#endif // DAYPHASE_H
