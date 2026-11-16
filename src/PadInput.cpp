#include "PadInput.h"

// The screens' power (0x0202B6D0 on, not decompiled): a STAND-IN with the one field this file reads
struct ScreenPower {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ BOOL mLcdOff; // The screens were turned off for want of input
    /* 0x08 */ int mUnknown08;
};
extern ScreenPower data_02123434;

PadInput data_0211f9d4;

void PadInput::Init() {
    mFolded = FALSE;
    mActive = TRUE;
    Update();
    mUnknown14 = 0;
    mUnknown18 = 0;
}

void PadInput::Update() {
    mFolded = PAD_DetectFold();
    if (mFolded || data_02123434.mLcdOff) {
        mRepeatTimer = 0;
        mPrevHeld = 0;
        mRepeat = 0;
        mHeld = 0;
        mTrigger = 0;
        mActive = FALSE;
        return;
    }
    mActive = TRUE;
    u16 held = PAD_Read();
    mPrevHeld = mHeld;
    mTrigger = (mPrevHeld ^ held) & held;
    mHeld = held;
    if (mTrigger != 0) {
        mRepeat = mTrigger;
        mRepeatTimer = 0;
        return;
    }
    mRepeat = 0;
    if (mPrevHeld == mHeld) {
        mRepeatTimer++;
        if (mRepeatTimer >= 24) {
            mRepeat = mHeld;
            mRepeatTimer = 16;
        }
    } else {
        mPrevHeld = 0;
    }
}
