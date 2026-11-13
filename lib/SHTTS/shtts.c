#include "shtts.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void func_02089cf4(Shtts *engine) {
    OS_LockMutex(&engine->mutex);
}

void func_02089d08(Shtts *engine) {
    OS_UnlockMutex(&engine->mutex);
}

// Whether the engine is synthesizing
int func_02089d1c(Shtts *engine) {
    int busy;

    OS_LockMutex(&engine->mutex);
    busy = engine->busy;
    OS_UnlockMutex(&engine->mutex);
    return busy;
}

// Empties the echo's history
void func_02086814(Shtts *engine) {
    engine->unk_0E624 = 0;
    memset(engine->echoBuffer, 0, sizeof(engine->echoBuffer));
}

// Carries out one command of the text: '#', a letter, and for most a number, which after a '+' is added to the
// value in use instead of replacing it. With live set it changes the settings in use, otherwise the caller's.
// Returns 0 if the text is not a command, and -3 if it is, known or not.
int func_020873a4(const char *command, int live, Shtts *engine) {
    int value;

    if (command[0] == '#') {
        switch (command[1]) {
        case 'F':
            engine->cur.pitch = 0x13C2;
            engine->cur.unk_00 = 0;
            return -3;
        case 'M':
            engine->cur.pitch = 0x1200;
            return -3;
        case 'B':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.pitch + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.pitch = func_02087e44(value, 3500, 7000, "Pitch");
            } else {
                if (command[2] == '+') {
                    value = engine->set.pitch + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.pitch = func_02087e44(value, 3500, 7000, "Pitch");
            }
            return -3;
        case 'C':
            if (live) {
                value = atoi(command + 2);
                engine->cur.unk_14 = 0;
                engine->cur.mode = 0;
                if (value == 1) {
                    engine->cur.mode = 1;
                } else if (value == 2) {
                    engine->cur.mode = 2;
                } else if (value != 0) {
                    engine->cur.unk_14 = 1;
                }
            } else {
                value = atoi(command + 2);
                engine->set.unk_14 = 0;
                engine->set.mode = 0;
                if (value == 1) {
                    engine->set.mode = 1;
                } else if (value == 2) {
                    engine->set.mode = 2;
                } else if (value != 0) {
                    engine->set.unk_14 = 1;
                }
            }
            return -3;
        case 'E':
            // Emphasis: higher, stronger, slower and louder
            engine->cur.pitch = func_02087e44(engine->cur.pitch + 100, 3500, 7000, "Pitch");
            engine->cur.accent = func_02087e44(engine->cur.accent + 5, 0, 400, "Accent");
            engine->cur.phrase = func_02087e44(engine->cur.phrase + 5, 0, 400, "Phrase");
            engine->cur.speed = func_02087e44(engine->cur.speed - 10, 20, 2200, "Speed");
            engine->cur.volume = func_02087e44(engine->cur.volume + 10, -10000, 360, "Volume");
            return -3;
        case 'D':
            // The opposite
            engine->cur.pitch = func_02087e44(engine->cur.pitch - 100, 3500, 7000, "Pitch");
            engine->cur.accent = func_02087e44(engine->cur.accent - 5, 0, 400, "Accent");
            engine->cur.phrase = func_02087e44(engine->cur.phrase - 5, 0, 400, "Phrase");
            engine->cur.speed = func_02087e44(engine->cur.speed + 10, 20, 2200, "Speed");
            engine->cur.volume = func_02087e44(engine->cur.volume - 10, -10000, 360, "Volume");
            return -3;
        case 'A':
            value = func_02087e44(atoi(command + 2), 0, 400, "Accent");
            if (live) {
                engine->cur.accent = value;
            } else {
                engine->set.accent = value;
            }
            return -3;
        case 'P':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.phrase + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.phrase = func_02087e44(value, 0, 400, "Phrase");
            } else {
                if (command[2] == '+') {
                    value = engine->set.phrase + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.phrase = func_02087e44(value, 0, 400, "Phrase");
            }
            return -3;
        case 'S':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.speed + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.speed = func_02087e44(value, 20, 2200, "Speed");
            } else {
                if (command[2] == '+') {
                    value = engine->set.speed + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.speed = func_02087e44(value, 20, 2200, "Speed");
            }
            return -3;
        case 'V':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.volume + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.volume = func_02087e44(value, -10000, 360, "Volume");
            } else {
                if (command[2] == '+') {
                    value = engine->set.volume + func_02087e64(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.volume = func_02087e44(value, -10000, 360, "Volume");
            }
            return -3;
        case 'I':
            value = func_02087e44(atoi(command + 2), 0, 3000, "Echo Delay");
            if (!live) {
                engine->set.echoDelay = value;
            } else {
                engine->cur.echoDelay = value;
                func_02086814(engine);
            }
            return -3;
        case 'J':
            value = func_02087e44(atoi(command + 2), 0, 100, "Echo Gain");
            if (live) {
                engine->cur.echoGain = value;
            } else {
                engine->set.echoGain = value;
            }
            return -3;
        case 'R':
            value = func_02087e44(atoi(command + 2), 0, 1000, "Fir Delay");
            if (!live) {
                engine->set.firDelay = value;
            } else {
                func_0208a010(engine);
                engine->cur.firDelay = value;
            }
            return -3;
        case 'G':
            value = func_02087e44(atoi(command + 2), 0, 100, "Fir Gain");
            if (!live) {
                engine->set.firGain = value;
            } else {
                func_0208a010(engine);
                engine->cur.firGain = value;
            }
            return -3;
        case 'r':
            value = func_02087e44(atoi(command + 2), 0, 5000, "Ring");
            if (live) {
                engine->cur.ring = value;
            } else {
                engine->set.ring = value;
            }
            return -3;
        case 'k':
            value = func_02087e44(atoi(command + 2), 0, 100, "Chorus");
            if (live) {
                engine->cur.chorus = value;
            } else {
                engine->set.chorus = value;
            }
            return -3;
        case 'X':
            func_0208c4dc(func_02087e44(atoi(command + 2), 80, 130, "Warp"), live, engine);
            return -3;
        case 'Z':
            func_0208c404(engine);
            return -3;
        case 'T':
            if (isdigit(command[2])) {
                value = func_02087e44(atoi(command + 2), 0, 1000, "PTrans");
                if (live) {
                    engine->cur.pitchTrans = value;
                } else {
                    engine->set.pitchTrans = value;
                }
            }
            return -3;
        default:
            return -3;
        }
    }
    return 0;
}

// A value kept within a range
int func_02087e44(int value, int min, int max, const char *name) {
    int result = min;

    if (value >= min) {
        if (value > max) {
            result = max;
        } else {
            result = (s16)value;
        }
    }
    return result;
}

// The number that text starts with, kept within what 16 bits hold
int func_02087e64(const char *text) {
    int value = atoi(text);
    int result = -0x8000;

    if (value >= -0x8000) {
        if (value > 0x7FFF) {
            result = 0x7FFF;
        } else {
            result = (s16)value;
        }
    }
    return result;
}

// Empties the FIR filter's history
void func_0208a010(Shtts *engine) {
    engine->unk_0EE00 = 0;
    memset(engine->firBuffer, 0, sizeof(engine->firBuffer));
}

// Sets the caller's voice settings back to their defaults
void func_0208c398(Shtts *engine) {
    engine->set.unk_00 = 0;
    engine->set.pitch = 0x13C2;
    engine->set.accent = 100;
    engine->set.phrase = 100;
    engine->set.speed = 100;
    engine->set.volume = 0;
    engine->set.warp = 100;
    engine->set.unk_28 = 0x400;
    engine->set.unk_2A = 10;
    engine->set.unk_14 = 0;
    engine->set.mode = 0;
    engine->set.echoDelay = 0;
    engine->set.echoGain = 0;
    engine->set.firGain = 0;
    engine->set.firDelay = 0;
    engine->set.ring = 0;
    engine->set.chorus = 0;
    engine->set.pitchTrans = 0;
}

// Puts the caller's settings in use
void func_0208c404(Shtts *engine) {
    engine->cur.unk_00 = engine->set.unk_00;
    engine->cur.pitch = engine->set.pitch;
    engine->cur.accent = engine->set.accent;
    engine->cur.phrase = engine->set.phrase;
    engine->cur.speed = engine->set.speed;
    engine->cur.volume = engine->set.volume;
    engine->cur.warp = engine->set.warp;
    engine->cur.unk_28 = engine->set.unk_28;
    engine->cur.unk_2A = engine->set.unk_2A;
    engine->cur.unk_14 = engine->set.unk_14;
    engine->cur.mode = engine->set.mode;
    engine->cur.echoDelay = engine->set.echoDelay;
    if (engine->cur.echoDelay != 0) {
        func_02086814(engine);
    }
    engine->cur.echoGain = engine->set.echoGain;
    engine->cur.firGain = engine->set.firGain;
    engine->cur.firDelay = engine->set.firDelay;
    if (engine->cur.firDelay != 0 || engine->cur.firGain != 0) {
        func_0208a010(engine);
    }
    engine->cur.ring = engine->set.ring;
    engine->cur.chorus = engine->set.chorus;
    engine->cur.pitchTrans = engine->set.pitchTrans;
}

// Sets the warp, 80 to 200, and the two values worked out from it.
// Not matching yet: the original multiplies once, before it tests live, and this does in each branch.
void func_0208c4dc(int warp, int live, Shtts *engine) {
    if (warp < 80) {
        warp = 80;
    }
    if (warp > 200) {
        warp = 200;
    }
    if (live) {
        engine->cur.warp = warp;
        engine->cur.unk_28 = (warp * 0x28F) >> 6;
        engine->cur.unk_2A = func_02089444(1000, (s16)warp);
    } else {
        engine->set.warp = warp;
        engine->set.unk_28 = (warp * 0x28F) >> 6;
        engine->set.unk_2A = func_02089444(1000, (s16)warp);
    }
}

void func_0208d4f0(Shtts *engine, ShttsCallback callback, void *arg) {
    engine->callback = callback;
    engine->callbackArg = arg;
}

// Asks the engine to stop synthesizing, and waits until it has, unless called from the engine's own thread
int func_0208d878(Shtts *engine) {
    int stopping = 0;
    OSThread *current;

    func_02089cf4(engine);
    if (engine->busy == 1) {
        stopping = 1;
        engine->stopRequested = 1;
    }
    func_02089d08(engine);

    current = OS_GetCurrentThread();
    if (stopping == 1 && current != &engine->thread) {
        while (func_02089d1c(engine) == 1) {
            OS_Sleep(10);
        }
    }
    return 0;
}

// Sets up an engine in memory the caller provides.
// Not matching yet: the same instructions, but the original keeps the pointer of the loop below in r5 and the
// engine's address plus 0xB000 in lr, and this has them the other way round.
int func_0208d8fc(Shtts *engine) {
    int i;
    u8 *p;

    MIi_CpuClear32(0, engine, sizeof(Shtts));
    engine->unk_00000 = 11000;
    engine->unk_00004 = 0x37;
    engine->unk_00008 = 0;
    engine->unk_0000C = 0;
    engine->cur.unk_00 = 0;
    engine->cur.pitch = 0x13C2;
    engine->cur.accent = 100;
    engine->cur.phrase = 100;
    engine->cur.speed = 120;
    engine->cur.volume = 0;
    engine->unk_01B21 = 0;
    engine->cur.echoGain = 30;
    engine->cur.echoDelay = 0;
    engine->cur.unk_14 = 0;
    engine->unk_08CE4 = 0;
    engine->cur.firDelay = 0;
    engine->cur.chorus = 0;
    engine->cur.firGain = 0;
    engine->cur.ring = 0;
    engine->unk_0B6FC = 0;
    engine->unk_08906 = 0;
    engine->unk_08CDC = 480;
    engine->unk_08CDE = 240;
    engine->unk_08CE0 = 0;
    engine->cur.unk_14 = 0;
    engine->unk_08CE4 = 0;
    engine->cur.firDelay = 0;
    engine->cur.chorus = 0;
    engine->cur.firGain = 0;
    engine->cur.ring = 0;
    engine->unk_0B6FC = 0;
    p = engine->unk_0B730;
    engine->unk_0B720 = 0;
    engine->unk_08722 = 0;
    engine->unk_0B728 = 0;
    engine->unk_0B729 = 0;
    engine->unk_0B72A = 1;
    engine->unk_0B72B = 1;
    engine->unk_0B72C = 0;
    for (i = 0; i < 4; i++) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p[3] = 0;
        p += 4;
    }
    engine->unk_0E624 = 0;
    engine->unk_0EE00 = 0;
    engine->unk_0EE04 = 0;
    engine->unk_0EE08 = 0;
    engine->unk_0EE0C = 0;
    engine->busy = 0;
    engine->stopRequested = 0;
    engine->unk_0EE11 = 0;
    engine->unk_0EE14 = 0;
    engine->cur.warp = 100;
    engine->cur.unk_28 = 0x400;
    engine->cur.unk_2A = 10;
    engine->unk_0EE18 = 1;
    OS_InitMutex(&engine->mutex);
    OS_InitMutex(&engine->unk_0EF08);
    engine->phoneme = 0;
    engine->unk_0B72D = 0;
    func_0208dab4(engine);
    return 0;
}

int func_0208dab4(Shtts *engine) {
    func_0208c398(engine);
    return 0;
}

// 0 when idle, 1 while synthesizing, 2 while synthesizing and asked to stop
int func_0208dac4(Shtts *engine) {
    int state;

    func_02089cf4(engine);
    state = engine->busy;
    if (state == 1 && engine->stopRequested == 1) {
        state = 2;
    }
    func_02089d08(engine);
    return state;
}

// The name of the sound being spoken, "X" if there is none
const char *func_0208daf8(Shtts *engine) {
    if (engine->phoneme < 0 || engine->phoneme > 38) {
        return ShttsPhonemeNames[7];
    }
    return ShttsPhonemeNames[engine->phoneme];
}

int func_0208e09c(Shtts *engine, int value);
int func_0208e0cc(Shtts *engine, int value);
int func_0208e0fc(Shtts *engine, int value);
int func_0208e12c(Shtts *engine, int value);
int func_0208e160(Shtts *engine, int value);
int func_0208e190(Shtts *engine, int value);
int func_0208e1c0(Shtts *engine, int value);
int func_0208e1e8(Shtts *engine, int value);
int func_0208e210(Shtts *engine, int value);
int func_0208e238(Shtts *engine, int value);
int func_0208e298(Shtts *engine, int value);
int func_0208e2c0(Shtts *engine, int value);
int func_0208e2e8(Shtts *engine, int value);
int func_0208e310(Shtts *engine, int value);

// Sets one of the caller's voice settings, which only works while the engine is idle. Each setting has its own
// range of values, see the functions below.
// objdiff scores this 99.95%, though its instructions and relocations are those of the original.
int func_0208db2c(Shtts *engine, int param, int value) {
    if (func_02089d1c(engine) != 0) {
        return -2;
    }
    switch (param) {
    case 1:
        return func_0208dda4(engine, value);
    case 2:
        return func_0208de90(engine, value);
    case 3:
        return func_0208e09c(engine, value);
    case 4:
        return func_0208e0cc(engine, value);
    case 5:
        return func_0208e0fc(engine, value);
    case 6:
        return func_0208e12c(engine, value);
    case 7:
        return func_0208e160(engine, value);
    case 8:
        return func_0208e190(engine, value);
    case 9:
        return func_0208e1c0(engine, value);
    case 10:
        return func_0208e1e8(engine, value);
    case 11:
        return func_0208e210(engine, value);
    case 12:
        return func_0208e238(engine, value);
    case 13:
        return func_0208e298(engine, value);
    case 14:
        return func_0208e2c0(engine, value);
    case 15:
        return func_0208e2e8(engine, value);
    case 16:
        return func_0208e310(engine, value);
    default:
        return -1;
    }
}

int func_0208e09c(Shtts *engine, int value) {
    if (value < 20 || value > 2200) {
        return -1;
    }
    engine->set.speed = value;
    return 0;
}

int func_0208e0cc(Shtts *engine, int value) {
    if (value < 3500 || value > 7000) {
        return -1;
    }
    engine->set.pitch = value;
    return 0;
}

int func_0208e0fc(Shtts *engine, int value) {
    if (value < -10000 || value > 360) {
        return -1;
    }
    engine->set.volume = value;
    return 0;
}

int func_0208e12c(Shtts *engine, int value) {
    if (value < 80 || value > 130) {
        return -1;
    }
    func_0208c4dc(value, 0, engine);
    return 0;
}

int func_0208e160(Shtts *engine, int value) {
    if (value < 0 || value > 5000) {
        return -1;
    }
    engine->set.ring = value;
    return 0;
}

int func_0208e190(Shtts *engine, int value) {
    if (value < 0 || value > 3000) {
        return -1;
    }
    engine->set.echoDelay = value;
    return 0;
}

int func_0208e1c0(Shtts *engine, int value) {
    if (value < 0 || value > 100) {
        return -1;
    }
    engine->set.echoGain = value;
    return 0;
}

int func_0208e1e8(Shtts *engine, int value) {
    if (value < 0 || value > 1000) {
        return -1;
    }
    engine->set.firDelay = value;
    return 0;
}

int func_0208e210(Shtts *engine, int value) {
    if (value < 0 || value > 100) {
        return -1;
    }
    engine->set.firGain = value;
    return 0;
}

int func_0208e238(Shtts *engine, int value) {
    if (value < 0 || value > 3) {
        return -1;
    }
    engine->set.unk_14 = 0;
    engine->set.mode = 0;
    if (value == 1) {
        engine->set.mode = 1;
    } else if (value == 2) {
        engine->set.mode = 2;
    } else if (value == 3) {
        engine->set.unk_14 = 1;
    }
    return 0;
}

int func_0208e298(Shtts *engine, int value) {
    if (value < 0 || value > 100) {
        return -1;
    }
    engine->set.chorus = value;
    return 0;
}

int func_0208e2c0(Shtts *engine, int value) {
    if (value < 0 || value > 400) {
        return -1;
    }
    engine->set.accent = value;
    return 0;
}

int func_0208e2e8(Shtts *engine, int value) {
    if (value < 0 || value > 400) {
        return -1;
    }
    engine->set.phrase = value;
    return 0;
}

int func_0208e310(Shtts *engine, int value) {
    if (value < 0 || value > 1000) {
        return -1;
    }
    engine->set.pitchTrans = value;
    return 0;
}

// Reads one of the caller's voice settings, by the numbers of func_0208db2c. The speaker and the way of speaking,
// settings 1 and 2, cannot be read back.
int func_0208dca4(Shtts *engine, int param) {
    switch (param) {
    case 3:
        return engine->set.speed;
    case 4:
        return engine->set.pitch;
    case 5:
        return engine->set.volume;
    case 6:
        return engine->set.warp;
    case 7:
        return engine->set.ring;
    case 8:
        return engine->set.echoDelay;
    case 9:
        return engine->set.echoGain;
    case 10:
        return engine->set.firDelay;
    case 11:
        return engine->set.firGain;
    case 12:
        return func_0208e338(engine);
    case 13:
        return engine->set.chorus;
    case 14:
        return engine->set.accent;
    case 15:
        return engine->set.phrase;
    case 16:
        return engine->set.pitchTrans;
    default:
        return 0x80000000;
    }
}

// Setting 1: the speaker, 0x101 to 0x10C
int func_0208dda4(Shtts *engine, int value) {
    const char *name;

    switch (value) {
    case 0x101:
        name = "<\\Spk_male";
        break;
    case 0x102:
        name = "<\\Spk_female";
        break;
    case 0x103:
        name = "<\\Spk_admale";
        break;
    case 0x104:
        name = "<\\Spk_adfemale";
        break;
    case 0x105:
        name = "<\\Spk_emale";
        break;
    case 0x106:
        name = "<\\Spk_efemale";
        break;
    case 0x107:
        name = "<\\Spk_chmale";
        break;
    case 0x108:
        name = "<\\Spk_chfemale";
        break;
    case 0x109:
        name = "<\\Spk_sisters";
        break;
    case 0x10A:
        name = "<\\Spk_meimu";
        break;
    case 0x10B:
        name = "<\\Spk_space";
        break;
    case 0x10C:
        name = "<\\Spk_giant";
        break;
    default:
        return -1;
    }
    func_0208dfcc(engine, name);
    return 0;
}

// Setting 2: the way of speaking, 0x201 to 0x211
int func_0208de90(Shtts *engine, int value) {
    const char *name;

    switch (value) {
    case 0x201:
        name = "<\\angry";
        break;
    case 0x202:
        name = "<\\business";
        break;
    case 0x203:
        name = "<\\calm";
        break;
    case 0x204:
        name = "<\\depressed";
        break;
    case 0x205:
        name = "<\\excited";
        break;
    case 0x206:
        name = "<\\falsetto";
        break;
    case 0x207:
        name = "<\\happy";
        break;
    case 0x208:
        name = "<\\loud";
        break;
    case 0x209:
        name = "<\\monotone";
        break;
    case 0x20A:
        name = "<\\perky";
        break;
    case 0x20B:
        name = "<\\quiet";
        break;
    case 0x20C:
        name = "<\\sarcastic";
        break;
    case 0x20D:
        name = "<\\scared";
        break;
    case 0x20E:
        name = "<\\shout";
        break;
    case 0x20F:
        name = "<\\tence";
        break;
    case 0x210:
        name = "<\\whisper";
        break;
    case 0x211:
        name = "<\\sing";
        break;
    default:
        return -1;
    }
    func_0208dfcc(engine, name);
    return 0;
}

// Applies the preset of a name to the caller's settings
int func_0208dfcc(Shtts *engine, const char *name) {
    const char *commands = func_0208dff8(name);

    if (commands == NULL) {
        return -1;
    }
    func_0208e04c(engine, commands);
    return 0;
}

// The commands of the preset of a name, or NULL
const char *func_0208dff8(const char *name) {
    int i;

    for (i = 0; i < SHTTS_PRESET_COUNT; i++) {
        if (strcmp(name, ShttsPresets[i].name) == 0) {
            return ShttsPresets[i].commands;
        }
    }
    return NULL;
}

// Applies each command of a string of them
int func_0208e04c(Shtts *engine, const char *commands) {
    const char *command = strchr(commands, '#');

    while (command != NULL) {
        func_020873a4(command, 0, engine);
        command = strchr(command + 1, '#');
    }
    return 0;
}

// The value that func_0208e238 was given
int func_0208e338(Shtts *engine) {
    if (engine->set.unk_14 == 0) {
        return engine->set.mode;
    }
    return 3;
}

// The number of a sound by its name, with "pau", a pause, for silence. A name that is not one is "X", 7.
int func_0208e354(const char *name) {
    s16 i;

    for (i = 0; i < SHTTS_PHONEME_COUNT; i++) {
        if (strcmp(name, ShttsPhonemeNames[i]) == 0) {
            return i;
        }
    }
    if (strcmp(name, "pau") == 0) {
        return 0;
    }
    return 7;
}
