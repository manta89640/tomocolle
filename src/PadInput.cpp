#include "PadInput.h"
#include "LcdPower.h"

PadInput gPadInput;

void PadInput::Init() {
    mFolded = FALSE;
    mActive = TRUE;
    Update();
    mUnknown14 = 0;
    mUnknown18 = 0;
}

void PadInput::Update() {
    mFolded = PAD_DetectFold();
    if (mFolded || gLcdPower.mLcdOff) {
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
