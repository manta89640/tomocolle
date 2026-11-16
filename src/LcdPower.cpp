#include "LcdPower.h"
#include "PadInput.h"

LcdPower gLcdPower;

void LcdPower::Init() {
    SetMode(1);
    mLcdOff = FALSE;
}

void LcdPower::Update() {
    if (mMode == 0) {
        return;
    }
    if (mMode != 1) {
        return;
    }
    if (!mLcdOff) {
        if (data_0211f9d4.mFolded) {
            mFrames++;
            if (mFrames > 20 && TurnOff()) {
                mLcdOff = TRUE;
                mFrames = 0;
            }
        } else {
            mFrames = 0;
        }
    } else {
        if (!data_0211f9d4.mFolded) {
            mFrames++;
            if (mFrames > 10 && TurnOn()) {
                mLcdOff = FALSE;
                mFrames = 0;
            }
        } else {
            mFrames = 0;
        }
    }
}

void LcdPower::SetMode(int mode) {
    mMode = mode;
    mFrames = 0;
}

// Turns both screens off; if the LCDs cannot be turned off, shows them again
BOOL LcdPower::TurnOff() {
    GX_DispOff();
    GXS_DispOff();
    BOOL done = PM_SetLCDPower(PM_LCD_POWER_OFF);
    if (!done) {
        GX_DispOn();
        GXS_DispOn();
    }
    return done;
}

BOOL LcdPower::TurnOn() {
    BOOL done = PM_SetLCDPower(PM_LCD_POWER_ON);
    if (done) {
        GX_DispOn();
        GXS_DispOn();
    }
    return done;
}
