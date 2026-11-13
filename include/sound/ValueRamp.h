#ifndef VALUERAMP_H
#define VALUERAMP_H

#include <nitro.h>

// A number that changes by a step each frame: either freely (within +-0x10000000) or towards a target, where it
// stops. The values are 20.12 fixed point numbers where SoundParamRamps uses them.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class ValueRamp {
public:
    enum Mode {
        MODE_STOPPED,   // Update does nothing
        MODE_FREE,      // The step is added each update, and the value kept within the limits
        MODE_TO_TARGET, // The value moves by the step's size towards the target and stops there
    };
    enum {
        VALUE_MAX = 0x10000000,
        VALUE_MIN = -0x10000000,
    };

    ValueRamp();
    void Reset();
    void Update();
    void SetMode(int mode);

    /* 0x0 */ int mMode;
    /* 0x4 */ int mValue;
    /* 0x8 */ int mTarget;
    /* 0xC */ int mStep;
};

// Four ramps for a playing sound: its volume, pitch, tempo ratio and pan, as 20.12 fixed point numbers. The game's
// scenes update it each frame and pass each value's integer part to SoundManager (e.g. overlay 93 at 0x0214af64:
// SetSeqArcVolume, SetSeqArcTrackPitch, SetSeqArcTempoRatio and SetSeqArcTrackPan, in this order).
class SoundParamRamps {
public:
    enum {
        RAMP_VOLUME,      // Starts at 127
        RAMP_PITCH,       // Starts at 0
        RAMP_TEMPO_RATIO, // Starts at 256, the normal tempo
        RAMP_PAN,         // Starts at 0
        RAMP_COUNT,
    };

    void Reset();
    void Update();

    /* 0x00 */ ValueRamp mRamps[RAMP_COUNT];
};

#endif // VALUERAMP_H
