#ifndef SCREENCONTROL_H
#define SCREENCONTROL_H

#include <nitro.h>

// The screens: which planes are shown (data_0211d604, functions 0x02023848 on, not decompiled). What the scenes
// and the scene manager show of it. The names are descriptive, not the original.
struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0x40];
    /* 0x44 */ int mUnknown44;
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x10];
    /* 0xE8 */ BOOL mUnknown44Changed;
    /* 0xEC */ BOOL mUnknown48Changed;
};

extern "C" {
extern ScreenControl data_0211d604;
}

#endif // SCREENCONTROL_H
