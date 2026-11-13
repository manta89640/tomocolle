#include "sound/ValueRamp.h"

ValueRamp::ValueRamp() {
    Reset();
}

void ValueRamp::Reset() {
    SetMode(MODE_STOPPED);
    mStep = 0;
    mTarget = 0;
    mValue = 0;
}

void ValueRamp::Update() {
    switch (mMode) {
    case MODE_STOPPED:
        break;
    case MODE_FREE: {
        mValue += mStep;
        int value = mValue;
        if (value > VALUE_MAX) {
            value = VALUE_MAX;
        } else if (value < VALUE_MIN) {
            value = VALUE_MIN;
        }
        mValue = value;
        break;
    }
    case MODE_TO_TARGET: {
        int step = mStep;
        if (step == 0) {
            SetMode(MODE_STOPPED);
            mValue = mTarget;
            break;
        }
        if (step < 0) {
            step = -step;
        }
        if (mTarget > mValue) {
            mValue += step;
            if (mTarget <= mValue) {
                SetMode(MODE_STOPPED);
                mValue = mTarget;
            }
        } else if (mTarget < mValue) {
            mValue -= step;
            if (mTarget >= mValue) {
                SetMode(MODE_STOPPED);
                mValue = mTarget;
            }
        } else {
            SetMode(MODE_STOPPED);
        }
        break;
    }
    }
}

void ValueRamp::SetMode(int mode) {
    mMode = mode;
}

void SoundParamRamps::Reset() {
    for (int i = 0; i < RAMP_COUNT; i++) {
        mRamps[i].SetMode(ValueRamp::MODE_STOPPED);
        mRamps[i].mStep = 0;
    }
    mRamps[RAMP_VOLUME].mValue = mRamps[RAMP_VOLUME].mTarget = 127 << 12;
    mRamps[RAMP_PITCH].mValue = mRamps[RAMP_PITCH].mTarget = 0;
    mRamps[RAMP_TEMPO_RATIO].mValue = mRamps[RAMP_TEMPO_RATIO].mTarget = 256 << 12;
    mRamps[RAMP_PAN].mValue = mRamps[RAMP_PAN].mTarget = 0;
}

void SoundParamRamps::Update() {
    for (int i = 0; i < RAMP_COUNT; i++) {
        mRamps[i].Update();
    }
}
