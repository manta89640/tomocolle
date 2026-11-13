#include "shtts.h"

#include <ctype.h>
#include <stdio.h>
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
    p = (u8 *)engine + 0x730;
    engine->unk_0B720 = 0;
    engine->unk_08722 = 0;
    engine->unk_0B728 = 0;
    engine->unk_0B729 = 0;
    engine->unk_0B72A = 1;
    engine->unk_0B72B = 1;
    engine->unk_0B72C = 0;
    p += 0xB000;
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

// The functions from here on are not yet in the order they have in the ROM

// Compares two strings, taking 'I' and 'U' for 'i' and 'u'
int func_0208473c(const char *a, const char *b) {
    while (*a != 0 || *b != 0) {
        int ca = *a;
        int cb = *b;

        if (ca != cb) {
            if (ca == 'I' || ca == 'U') {
                ca += 0x20;
            }
            if (cb == 'I' || cb == 'U') {
                cb += 0x20;
            }
            if (ca != cb) {
                return ca - cb;
            }
        }
        a++;
        b++;
    }
    return 0;
}

int func_02084814(void *arg) {
    s16 count = func_020857e8(arg) - 2;

    if (count < 0) {
        return 0;
    }
    return count;
}

void func_020856b0(ShttsUnk_020856b0 *unk, int rate) {
    if (rate <= 0) {
        rate = 1;
    }
    unk->unk_200 = ((0x12C00 / rate) << 5) / 256;
    if (unk->unk_200 == 0) {
        unk->unk_200 = 1;
    }
    unk->unk_204 = 0;
    unk->unk_208 = 0;
    unk->unk_20C = 0;
    memset(unk->unk_000, 0, sizeof(unk->unk_000));
}

int func_02085a64(Shtts *engine) {
    return engine->unk_0871C;
}

void func_02086a74(int value, int *out) {
    *out = value;
}

void func_02087d98(ShttsUnk_02087d98 *unk, Shtts *engine) {
    char dash = '-';

    unk->unk_00 = 0;
    unk->unk_05 = dash;
    unk->unk_04 = dash;
    unk->unk_08 = 0;
    unk->unk_06 = 0;
    unk->unk_10 = 0;
    unk->pitch = engine->cur.pitch;
}

// 0, 1 or 2 for "0", "1" or "2", and -1 for anything else
int func_020892a8(const char *text, const char *unused) {
    if (strcmp(text, "0") == 0) {
        return 0;
    }
    if (strcmp(text, "1") == 0) {
        return 1;
    }
    if (strcmp(text, "2") == 0) {
        return 2;
    }
    return -1;
}

// The same for the full-width digits
int func_02089304(const char *text, const char *unused) {
    if (strcmp(text, "０") == 0) {
        return 0;
    }
    if (strcmp(text, "１") == 0) {
        return 1;
    }
    if (strcmp(text, "２") == 0) {
        return 2;
    }
    return -1;
}

void func_020893e4(const char *text, s16 *out) {
    s16 unused = 0;
    int level = func_02089360(text, &unused);

    if (level == 0) {
        *out = 5;
    } else if (level == 1) {
        *out = 10;
    } else if (level == 2) {
        *out = 15;
    } else {
        *out = 10;
    }
}

// The product of two numbers in 16.16 fixed point
int func_02089b5c(int a, int b) {
    int sign = 1;
    int product;

    if (a < 0) {
        a = -a;
        sign = -1;
    }
    if (b < 0) {
        b = -b;
        sign = -sign;
    }
    product = (a >> 16) * (b >> 16) + (((a >> 16) * (u16)b + (u16)a * (b >> 16) + 0x8000) >> 16);
    return product * sign;
}

int func_02089cec(void) {
    return 0;
}

// Whether the engine has been asked to stop
int func_02089d4c(Shtts *engine) {
    int stopRequested;

    OS_LockMutex(&engine->mutex);
    stopRequested = engine->stopRequested;
    OS_UnlockMutex(&engine->mutex);
    return stopRequested;
}

// Where a character first is in a string, or NULL. Unlike strchr, it does not find the end of the string.
char *func_02089d7c(char *text, int c) {
    while (*text != 0 && *text != c) {
        text++;
    }
    if (*text == 0) {
        return NULL;
    }
    return text;
}

void func_02089e48(char *dst, int c, int count) {
    int i;

    for (i = 0; i < count; i++) {
        *dst++ = (char)c;
    }
}

// Called with -1 when something is wrong. Does nothing in this build.
void func_02089e70(int code) {
}

void func_0208aba8(ShttsUnk_0208aba8 *table, int count, int a, int b) {
    if (a < 0) {
        a = 0;
    } else {
        a = (s16)(a - table[0].unk_0);
    }
    if (b < 0) {
        b = a;
    } else {
        b = (s16)(b - table[count - 1].unk_0);
    }
    func_0208abf4(table, count, a, b);
}

ShttsUnit *func_0208bdf4(int *index, Shtts *engine) {
    ShttsUnit *unit = &data_020e8f08[engine->cur.unk_00][index[0]][index[1]][index[2]];

    if (unit->unk_2 == 0) {
        return NULL;
    }
    return unit;
}

void func_0208be38(Shtts *engine) {
    engine->unk_08CD4 = engine->unk_08908;
    engine->unk_08CD8 = engine->unk_08908 + 0x1E0;
    engine->unk_08CDC = 0x1E0;
    engine->unk_08CDE = 0xF0;
    engine->unk_08CD0 = engine->unk_08CD4;
    engine->unk_08CCC = engine->unk_08CC8 = engine->unk_08CD4;
    engine->unk_08CE0 = 0;
    func_0208be9c(engine->unk_08CDC, engine->unk_08CD4, engine);
}

int func_0208c568(Shtts *engine) {
    engine->unk_00000 = 11000;
    engine->unk_01B21 = 0;
    engine->unk_00004 = engine->unk_00000 / 200;
    return 0;
}

// Looks a word up in a sorted list. If it is there, writes it back as the list spells it and returns its value.
int func_02084794(char *text, ShttsWordList *list) {
    int low = 0;
    int high = list->count - 1;

    while (low <= high) {
        int middle = (low + high) >> 1;
        s16 order = func_0208473c(text, list->words[middle].name);

        if (order < 0) {
            high = middle - 1;
        } else if (order > 0) {
            low = middle + 1;
        } else {
            strcpy(text, list->words[middle].name);
            return list->words[middle].value;
        }
    }
    return -1;
}

// Reads the text to speak from standard input, a line at a time
void func_02086b14(Shtts *engine) {
    char line[150];
    int length = 0;

    while (1) {
        if (fgets(line, sizeof(line), stdin) == NULL) {
            return;
        }
        if (length > 3000) {
            return;
        }
        strcat(engine->text, line);
        length += sizeof(line);
        strcpy(line, "");
    }
}

// Where one string first is in another, or -1
int func_02087dc8(const char *text, const char *find) {
    const char *f = find;
    s16 i = 0;

    while (*text != 0) {
        if (*text == *f) {
            const char *t = text;

            while (*f != 0 && *t == *f) {
                f++;
                t++;
            }
            if (*f == 0) {
                return i;
            }
            f = find;
        }
        text++;
        i++;
    }
    return -1;
}

// Adds a line to text if there is room for it, and returns the line's length
int func_02089094(char *text, u32 size, const char *line) {
    char buffer[196];
    int length;

    if (*line == 0) {
        return 0;
    }
    strcpy(buffer, line);
    strcat(buffer, "\n");
    length = strlen(buffer);
    if (length + strlen(text) >= size) {
        return -1;
    }
    strcat(text, buffer);
    return (s16)length;
}

// Which of a table's entries, of 12 bytes each, a text starts with, or -1
int func_02089244(const char *text, const char *table, int count) {
    s16 i;

    for (i = 0; i < count; i++) {
        if (strncmp(text, table, strlen(table)) == 0) {
            break;
        }
        table += 12;
    }
    if (i >= count) {
        i = -1;
    }
    return i;
}

// Reads the digit before a position in text, half-width or full-width, and says in out how many bytes it took
int func_02089360(const char *end, s16 *out) {
    char digit[3] = { 0, 0, 0 };
    int value;

    strncpy(digit, end - 1, 1);
    value = func_020892a8(digit, end - 1);
    *out = 1;
    if (value == -1) {
        strncpy(digit, end - 2, 2);
        value = func_02089304(digit, end - 2);
        *out = 2;
    }
    if (value == -1) {
        *out = 0;
    }
    return value;
}

// Sets up the engine's state for a new text
int func_02089c50(Shtts *engine) {
    int i;
    u8 *p;

    engine->unk_04938 = 0;
    engine->unk_0EE0C = 0;
    engine->unk_0EE0E = 0;
    func_0208be38(engine);
    engine->unk_0B700 = 0;
    func_02089e48(engine->unk_0B704, 0, sizeof(engine->unk_0B704));
    p = (u8 *)engine + 0x730;
    engine->unk_08722 = 0;
    engine->unk_0EE04 = 0;
    engine->unk_0EE08 = 0;
    engine->unk_08906 = 0;
    engine->unk_0B6FC = 0;
    p += 0xB000;
    for (i = 0; i < 4; i++) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p[3] = 0;
        p += 4;
    }
    return 0;
}

// Scales samples by a gain in 4.12 fixed point, keeping them within 16 bits
void func_02089e74(s16 *dst, s16 *src, int count, int gain) {
    s16 i;

    for (i = 0; i < count; i++) {
        int sample = (*src * gain) >> 12;

        if (sample > 0x7FFF) {
            sample = 0x7FFF;
        } else if (sample < -0x7FFF) {
            sample = -0x7FFF;
        }
        *dst++ = sample;
        src++;
    }
}

// The ring effect: multiplies samples by a sine wave of a frequency
void func_0208b6c0(int *phase, int count, int frequency, s16 *src, s16 *dst) {
    int i;
    int step = (frequency << 16) / 11000;

    for (i = 0; i < count; i++) {
        (*phase)++;
        dst[i] = (src[i] * data_020e8d08[((u32)((step * *phase) >> 8) << 24) >> 24]) >> 15;
    }
}

// Plays out the half of the sample buffer that has been filled, and moves on to the other half
void func_0208c054(Shtts *engine) {
    if (engine->unk_08CD0 >= engine->unk_08CC8) {
        return;
    }
    func_0208e4e4(engine->unk_08CDE, engine->unk_08CD0, engine);
    func_0208be9c(engine->unk_08CDE, engine->unk_08CD0, engine);
    if (engine->unk_08CD0 == &engine->unk_08908[0xF0]) {
        engine->unk_08CD0 = &engine->unk_08908[0];
    } else {
        engine->unk_08CD0 = &engine->unk_08908[0xF0];
    }
    engine->unk_08CC8 = engine->unk_08CD0;
}

// Fades samples in from silence, or out to it
void func_0208e464(int from, int to, int count, s16 *data) {
    int gain;
    int step;
    s16 i;

    if (count <= 0) {
        return;
    }
    step = func_02089444(to - from, count);
    if (from == 0) {
        gain = from;
    } else if (to == 0) {
        gain = -(step * (count - 1));
    }
    for (i = 0; i < count; i++) {
        *data = (*data * gain) >> 16;
        data++;
        gain += step;
    }
}

// The next four are written in assembly in the original too: they use the processor's multiply-accumulate
// instructions for 32-bit by 16-bit fixed point, which the compiler does not emit, and their stack adjustments are
// not ones it would make. Each returns the sum of products of a filter's state and its coefficients.

asm int func_02086a20(const int *state, const s16 *coefficients, int input) {
    stmfd sp!, {r3, r4, lr}
    sub sp, sp, #3
    mov lr, #0
    ldr r3, [r0, #0]
    ldrsh r4, [r1, #0]
    smlawb lr, r3, r4, lr
    ldr r3, [r0, #4]
    ldrsh r4, [r1, #2]
    smlawb lr, r3, r4, lr
    ldrsh r4, [r1, #4]
    smlawb lr, r2, r4, lr
    ldr r3, [r0, #8]
    ldrsh r4, [r1, #6]
    smlawb lr, r3, r4, lr
    ldr r3, [r0, #12]
    ldrsh r4, [r1, #8]
    smlawb lr, r3, r4, lr
    mov r0, lr
    add sp, sp, #3
    ldmfd sp!, {r3, r4, lr}
    bx lr
}

asm int func_0208ea20(const int *state, const s16 *coefficients) {
    stmfd sp!, {r2, r3, r4, lr}
    sub sp, sp, #4
    mov lr, #0
    ldr r2, [r0, #0]
    ldrsh r3, [r1, #2]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #4]
    ldrsh r3, [r1, #4]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #8]
    ldrsh r3, [r1, #6]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #12]
    ldrsh r3, [r1, #8]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #16]
    ldrsh r3, [r1, #10]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #20]
    ldrsh r3, [r1, #12]
    smlawb lr, r2, r3, lr
    mov r0, lr
    add sp, sp, #4
    ldmfd sp!, {r2, r3, r4, lr}
    bx lr
}

asm int func_0208ea84(const int *state, const s16 *coefficients) {
    stmfd sp!, {r2, r3, r4, lr}
    sub sp, sp, #4
    mov lr, #0
    ldr r2, [r0, #0]
    ldrsh r3, [r1, #2]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #4]
    ldrsh r3, [r1, #4]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #8]
    ldrsh r3, [r1, #6]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #12]
    ldrsh r3, [r1, #8]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #16]
    ldrsh r3, [r1, #10]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #20]
    ldrsh r3, [r1, #12]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #24]
    ldrsh r3, [r1, #14]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #28]
    ldrsh r3, [r1, #16]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #32]
    ldrsh r3, [r1, #18]
    smlawb lr, r2, r3, lr
    ldr r2, [r0, #36]
    ldrsh r3, [r1, #20]
    smlawb lr, r2, r3, lr
    mov r0, lr
    add sp, sp, #4
    ldmfd sp!, {r2, r3, r4, lr}
    bx lr
}

// A product in fixed point, doubled
asm int func_0208e81c(int a, int b) {
    stmfd sp!, {r2, lr}
    sub sp, sp, #2
    mov lr, #0
    smulwb r0, r0, r1
    mov r0, r0, lsl #1
    add sp, sp, #2
    ldmfd sp!, {r2, lr}
    bx lr
}

int func_02086628(s16 value) {
    if (value <= 3) {
        return 0;
    }
    if (value > 3 && value < 9) {
        return (s16)(value - 3);
    }
    if (value == 9 || value == 10) {
        return 6;
    }
    if (value >= 11 && value <= 13) {
        return 7;
    }
    if (value >= 14 && value <= 16) {
        return 8;
    }
    if (value >= 17 && value <= 25) {
        return 9;
    }
    if (value > 25) {
        return 10;
    }
    return -1;
}

// Copies a line of text, up to size, and moves the cursor past it. Returns its length, -1 at the end of the text.
int func_02086a7c(char *dst, int size, char **cursor) {
    s16 length;
    int last;

    if (cursor == NULL || *cursor == NULL) {
        return -2;
    }
    if (**cursor == 0) {
        return -1;
    }
    last = size - 1;
    length = 0;
    while (**cursor != 0 && length < last) {
        if (**cursor == '\n' || **cursor == '\r') {
            (*cursor)++;
            break;
        }
        *dst++ = *(*cursor)++;
        length++;
    }
    *dst = 0;
    return length;
}

// The next word of text that the characters of delimiters separate, as strtok does it but with the cursor in the
// caller's hands
char *func_02089da0(char **cursor, char *delimiters) {
    char *word;

    while (**cursor != 0 && func_02089d7c(delimiters, **cursor) != NULL) {
        (*cursor)++;
    }
    word = *cursor;
    if (*word == 0) {
        return NULL;
    }
    while (**cursor != 0 && func_02089d7c(delimiters, **cursor) == NULL) {
        (*cursor)++;
    }
    if (**cursor != 0) {
        *(*cursor)++ = 0;
    }
    return word;
}

// Fills count samples with silence from a place in the sample buffer, going round to its start at its end
void func_0208be9c(int count, s16 *buffer, Shtts *engine) {
    s16 i;

    if (count < 0) {
        func_02089e70(-1);
    }
    i = 0;
    if (buffer + count >= engine->unk_08CD8) {
        while (buffer < engine->unk_08CD8) {
            *buffer++ = 0;
            i++;
        }
        buffer = engine->unk_08CD4;
    }
    for (; i < count; i++) {
        *buffer++ = 0;
    }
}
