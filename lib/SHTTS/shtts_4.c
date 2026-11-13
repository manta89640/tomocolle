// The last part of the engine: the sound list, speaking a text, the settings, the output stage.
// Where the engine's own files begin and end is not known. These four are cut where linking needs them cut:
// around the one function that does not match, and where the engine's strings are interrupted by those of a
// file of tables that is not decompiled.

#include "shtts.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// The next four are written in assembly in the original too: they use the processor's multiply-accumulate
// instructions for 32-bit by 16-bit fixed point, which the compiler does not emit, and their stack adjustments are
// not ones it would make. Each returns the sum of products of a filter's state and its coefficients.

// Takes a text and starts the thread that speaks it
static inline void ShttsStart(Shtts *engine, const char *text, ShttsCallback callback, void *arg, int unk) {
    engine->unk_0EE11 = unk;
    func_0208d4f0(engine, callback, arg);
    func_02089c50(engine);
    memset(engine->unk_0B0E6, 0, sizeof(engine->unk_0B0E6));
    strncpy(engine->unk_0B0E6, text, sizeof(engine->unk_0B0E6));
    if (engine->unk_0B0E6[sizeof(engine->unk_0B0E6) - 1] != 0) {
        // Too long: cut it, and not in the middle of a character of two bytes
        engine->unk_0B0E6[sizeof(engine->unk_0B0E6) - 1] = 0;
        if (engine->unk_0B0E6[sizeof(engine->unk_0B0E6) - 2] & 0x80) {
            engine->unk_0B0E6[sizeof(engine->unk_0B0E6) - 2] = 0;
        }
    }
    func_0208c5a8(engine);
}

// Fractional multiply of two Q15 values.
static inline s16 func_020897c8_mult(s16 a, s16 b) { return (a * b) >> 15; }

// Writes the sounds of lists of syllables to an array, max at most, and returns how many. A syllable named "-"
// lengthens the sound before it, or repeats it if that would be too long or would change unk_A.
int func_0208c8e0(ShttsUnk_0208c8e0 *lists, ShttsSound *sounds, int max) {
    char name[12];
    ShttsUnk_0208c8e0_1 *list1;
    ShttsUnk_0208c8e0_2 *list2;
    ShttsUnk_02087d98 *syllable;
    s16 i;
    s16 j;
    s16 k;
    s16 length;
    int last;
    ShttsSound *sound;
    int count = 0;

    sound = sounds + 1;
    for (i = lists->count, list1 = lists->first; i > 0; i--, list1 = list1->next) {
        for (j = list1->count, list2 = list1->first; j > 0; j--, list2 = list2->next) {
            for (k = list2->count, syllable = list2->first; k > 0; k--, syllable = syllable->next) {
                if (syllable->unk_05 == 'U' && syllable->unk_08 > 20) {
                    syllable->unk_06 += syllable->unk_08 - 20;
                    syllable->unk_08 = 20;
                }
                if (syllable->unk_06 != 0) {
                    // The consonant, which is all of the name but its last letter
                    length = strlen(syllable->name);
                    strncpy(name, syllable->name, length - 1);
                    name[length - 1] = 0;
                    sound->phoneme = func_0208e354(name);
                    sound->unk_4 = syllable->unk_06;
                    sound->unk_8 = 0;
                    sound->unk_6 = 0;
                    sound->unk_C = 0;
                    sound->unk_A = 0;
                    sound->unk_E = syllable->unk_04;
                    sound->unk_F = syllable->unk_05;
                    sound++;
                    count++;
                    if (count < max - 1) {
                    } else {
                        goto end;
                    }
                } else {
                    length = 1;
                }
                last = length - 1;
                if (syllable->name[last] == '-') {
                    if (sound[-1].unk_4 + syllable->unk_08 > 0x7FFF
                        || (sound[-1].unk_A != 0 && sound[-1].unk_A != syllable->pitch)) {
                        sound->phoneme = sound[-1].phoneme;
                        sound->unk_4 = syllable->unk_08;
                        sound->unk_8 = syllable->pitch;
                        sound->unk_6 = syllable->unk_0A;
                        sound->unk_C = 0;
                        sound->unk_A = 0;
                        sound->unk_E = syllable->unk_04;
                        sound->unk_F = syllable->unk_05;
                        if (sound->unk_8 <= 0) {
                            sound->unk_8 = 0;
                        }
                        if (sound->unk_6 <= 0) {
                            sound->unk_6 = 0;
                        }
                        sound++;
                        count++;
                        if (count < max - 1) {
                        } else {
                            goto end;
                        }
                    } else {
                        sound[-1].unk_4 = sound[-1].unk_4 + syllable->unk_08;
                        sound[-1].unk_A = syllable->pitch;
                    }
                } else {
                    // The vowel
                    sound->phoneme = func_0208e354(&syllable->name[last]);
                    sound->unk_4 = syllable->unk_08;
                    sound->unk_8 = syllable->pitch;
                    sound->unk_6 = syllable->unk_0A;
                    sound->unk_C = 0;
                    sound->unk_A = 0;
                    sound->unk_E = syllable->unk_04;
                    sound->unk_F = syllable->unk_05;
                    if (sound->unk_8 <= 0) {
                        sound->unk_8 = 0;
                    }
                    if (sound->unk_6 <= 0) {
                        sound->unk_6 = 0;
                    }
                    sound++;
                    count++;
                    if (count < max - 1) {
                    } else {
                        goto end;
                    }
                }
            }
            if (list2->unk_12 != 0) {
                sound->phoneme = func_0208e354("S");
                sound->unk_4 = list2->unk_12;
                sound->unk_8 = 0;
                sound->unk_6 = 0;
                sound->unk_C = 0;
                sound->unk_A = 0;
                sound->unk_E = syllable->unk_04;
                sound++;
                count++;
                if (count < max - 1) {
                } else {
                    goto end;
                }
            }
        }
    }
end:
    sound->phoneme = 7;
    return count;
}

// Reads sounds from standard input, a line each: a name, three numbers and two characters. A line that starts with
// '-' lengthens the sound before, one with '*' is skipped, and one with '#' is a command. Returns how many sounds
// it read, or -1 if there was nothing to read.
int func_0208cc10(ShttsSound *sounds, Shtts *engine) {
    char line[128];
    char name[32];
    int a;
    int b;
    int c;
    s16 bOffset = 0;
    s16 aScale = 100;
    s16 cOffset = 0;
    s16 unk_C = 0;
    char *read;
    ShttsSound *sound;
    int count = 0;

    sound = sounds + 1;

    while ((read = fgets(line, sizeof(line), stdin)) != NULL) {
        if (line[0] == '#') {
            switch (line[1]) {
            case 'F':
            case 'f':
                engine->cur.unk_00 = 0;
                break;
            case 'M':
            case 'm':
                engine->cur.unk_00 = 1;
                break;
            case 'B':
            case 'b':
                if (line[2] == '+') {
                    func_020188f4(line + 3, "%hd", &bOffset);
                } else if (line[2] == '-') {
                    func_020188f4(line + 2, "%hd", &bOffset);
                }
                break;
            case 'S':
            case 's':
                func_020188f4(line + 2, "%hd", &aScale);
                break;
            case 'V':
            case 'v':
                func_020188f4(line + 2, "%hd", &cOffset);
                break;
            case 'L':
            case 'l':
                func_020188f4(line + 2, "%hd", &unk_C);
                break;
            }
        } else if (line[0] == '*') {
        } else if (line[0] == '-') {
            func_020188f4(line + 1, "%d %d", &a, &b);
            sound[-1].unk_4 += (s16)((aScale * a + 50) / 100);
            sound[-1].unk_A = bOffset + b;
        } else {
            c = 0;
            b = 0;
            a = 0;
            func_020188f4(line, "%s %d %d %d %c %c", name, &a, &b, &c, &sound->unk_E, &sound->unk_F);
            sound->phoneme = func_0208e354(name);
            sound->unk_4 = (aScale * a + 50) / 100;
            count++;
            sound->unk_8 = bOffset + b;
            sound->unk_6 = cOffset + c;
            sound->unk_C = unk_C;
            sound->unk_A = 0;
            if (sound->unk_8 <= 0) {
                sound->unk_8 = 0;
            }
            if (sound->unk_6 <= 0) {
                sound->unk_6 = 0;
            }
            sound++;
            if (sound[-1].phoneme == 0) {
                break;
            }
        }
    }
    sound->phoneme = 7;
    if (count == 0 && read == NULL) {
        return -1;
    }
    return count;
}

// Changes the end of an array of sounds. First by the mode setting: 1 repeats the last sound, 2 changes every
// second sound that has unk_8, and 10 adds "n a" or "n e N" after a last sound of "N". Then by flags: bits 4 to 6
// change the last sound, and bits 0 to 2 repeat it, with unk_8 set from the highest in the array.
void func_0208cf40(ShttsSound *sounds, int count, int flags, Shtts *engine) {
    int length;
    int pitch;
    ShttsSound *last;
    ShttsSound *end;
    int before;
    int high;
    int low;
    int i;
    int n;
    s16 *p;
    ShttsSound *sound;

    end = sounds + count;
    last = end;
    if (engine->cur.mode == 1) {
        if (count <= 4) {
            return;
        }
        if (count >= 253) {
            return;
        }
        if (end->phoneme != 0) {
            return;
        }
        if (end[-2].unk_8 != 0) {
            end[-2].unk_8 += 50;
            end[-2].unk_4 += 50;
        } else if (end[-3].unk_8 != 0) {
            end[-3].unk_8 += 50;
            end[-3].unk_4 += 50;
        }
        end[-1].unk_8 += 450;
        end[-1].unk_4 += 200;
        last++;
        count++;
        end[1].phoneme = end[0].phoneme;
        end[1].unk_4 = end[0].unk_4;
        end[0].phoneme = end[-1].phoneme;
        end[0].unk_8 = end[-1].unk_8 - 40;
        if (end[0].unk_8 < 0) {
            end[0].unk_8 = 0;
        }
        end[0].unk_4 = 10;
        end[0].unk_6 = end[-1].unk_6 - 200;
        if (end[0].unk_6 < 0) {
            end[0].unk_6 = 0;
        }
        end[-1].unk_A = 0;
        end[0].unk_A = 0;
        end[0].unk_F = '-';
    } else if (engine->cur.mode == 2) {
        int second;

        if (count <= 4) {
            return;
        }
        second = 0;
        for (last = sounds + 1; last < end; last++) {
            if (last->unk_8 != 0) {
                if (second == 1) {
                    last->unk_A = 0;
                    second = 0;
                    last->unk_8 += 300;
                    last->unk_4 += 150;
                } else {
                    second = 1;
                }
            }
        }
    } else if (engine->cur.mode == 10) {
        if (count <= 4 || count >= 251 || end->phoneme != 0) {
            return;
        }
        before = end[-1].phoneme;
        if (before != 6) {
            return;
        }
        length = end[-1].unk_4;
        if (length <= 120) {
            pitch = end[-1].unk_8;
            length = 120;
        } else if (end[-1].unk_A == 0) {
            pitch = end[-1].unk_8 - 50;
        } else {
            length /= 2;
            pitch = end[-1].unk_A - 50;
        }
        if ((end->unk_4 < 800 || count < 8) && !(end[-2].phoneme == 14 && before == 1)
            && !(before == 3 && end[-2].phoneme != 8 && end[-2].phoneme != 19)) {
            end[2].unk_4 = end[0].unk_4;
            end[2].phoneme = func_0208e354("S");
            end[0].phoneme = func_0208e354("n");
            end[0].unk_4 = length / 2;
            end[0].unk_6 = 0;
            end[0].unk_8 = 0;
            end[1].phoneme = func_0208e354("a");
            end[1].unk_4 = length;
            end[1].unk_8 = pitch + 50;
            end[1].unk_A = 0;
            end[1].unk_6 = end[-1].unk_6;
            last += 2;
            count += 2;
        } else if (end->unk_4 >= 800 && before != 5 && before != 4
                   && !(before == 1 && end[-2].phoneme != 25 && end[-2].phoneme != 12)) {
            end[3].unk_4 = end[0].unk_4;
            end[3].phoneme = func_0208e354("S");
            end[0].phoneme = func_0208e354("n");
            end[0].unk_4 = length / 2;
            end[0].unk_6 = 0;
            end[0].unk_8 = 0;
            end[1].phoneme = func_0208e354("e");
            end[1].unk_4 = length;
            end[1].unk_8 = pitch;
            if (end[1].unk_8 < 0) {
                end[1].unk_8 = 0;
            }
            end[1].unk_A = 0;
            end[1].unk_6 = end[-1].unk_6;
            end[2].phoneme = func_0208e354("N");
            end[2].unk_4 = 1.4 * length;
            end[2].unk_8 = pitch;
            last += 3;
            count += 3;
            if (end[2].unk_8 < 0) {
                end[2].unk_8 = 0;
            }
            end[2].unk_A = 0;
            end[2].unk_6 = end[-1].unk_6;
        }
    }
    last[1].phoneme = 7;

    if (count <= 0) {
        return;
    }
    if (flags == 0) {
        return;
    }
    if (count >= 253) {
        return;
    }
    // The lowest is found too, and not used
    high = sounds[1].unk_8;
    low = high;
    for (i = 1; i < count; i++) {
        sound = &sounds[i];
        if (sounds[i].unk_8 != 0) {
            if (sounds[i].unk_8 > high) {
                high = sound->unk_8;
            }
            if (sounds[i].unk_8 < low) {
                low = sound->unk_8;
            }
            if (sounds[i].unk_A != 0) {
                if (sounds[i].unk_A > high) {
                    high = sound->unk_A;
                }
                if (sounds[i].unk_A < low) {
                    low = sound->unk_A;
                }
            }
        }
    }
    sounds += count;
    sound = sounds - 1;
    if (sound->unk_A != 0) {
        p = &sound->unk_A;
    } else {
        p = &sound->unk_8;
    }
    n = flags & 0x70;
    if (n != 0) {
        sound = sounds - 1;
        n >>= 4;
        sound->unk_6 += n * 30;
        sound->unk_4 *= 0.8;
        if (sound->unk_4 < 10) {
            sound->unk_4 = 10;
        }
        *p += n * 100;
    }
    n = flags & 7;
    if (n != 0) {
        s16 step;
        s16 top;
        s16 bottom;

        sounds[1].phoneme = sounds[0].phoneme;
        sounds[1].unk_4 = sounds[0].unk_4;
        sounds[2].phoneme = 7;
        pitch = sounds[-1].unk_8 + n * (high - sounds[-1].unk_8) / 3;
        if (pitch < 0) {
            pitch = 0;
        } else if (pitch > 0x7FFF) {
            pitch = 0x7FFF;
        }
        step = n * 220;
        pitch = (s16)pitch;
        top = pitch + (step >> 1);
        bottom = top - step;
        if (bottom < 0) {
            bottom = 0;
        }
        sounds[0].phoneme = sounds[-1].phoneme;
        sounds[0].unk_8 = top;
        sounds[0].unk_A = 0;
        sounds[0].unk_4 = sounds[-1].unk_4 >> 1;
        if (sounds[0].unk_4 < 10) {
            sounds[0].unk_4 = 10;
        }
        sounds[0].unk_6 = sounds[-1].unk_6 - 90;
        if (sounds[0].unk_6 < 0) {
            sounds[0].unk_6 = 0;
        }
        if (sounds[-1].unk_A != 0) {
            sounds[-1].unk_A = bottom;
        }
        sounds[-1].unk_8 = bottom;
        sounds[-1].unk_4 *= 0.8;
        if (sounds[-1].unk_4 < 10) {
            sounds[-1].unk_4 = 10;
        }
    }
}

void func_0208d4f0(Shtts *engine, ShttsCallback callback, void *arg) {
    engine->callback = callback;
    engine->callbackArg = arg;
}

// Speaks a text, of 1023 bytes at most. If the engine is speaking already, mode says what to do: 0 stops that,
// 1 waits for it to end, and 2 does not speak the new text.
int func_0208d500(Shtts *engine, const char *text, u32 mode, u32 unk, ShttsCallback callback, void *arg) {
    int start = 0;

    SDK_USING_MIDDLEWARE(data_02000bd8);
    if (mode > 2) {
        return -1;
    }
    if (unk > 31) {
        return -1;
    }
    engine->threadPriority = unk;
    OS_LockMutex(&engine->unk_0EF08);
    if (mode == 2) {
        func_02089cf4(engine);
        if (engine->busy == 0) {
            start = 1;
            engine->busy = 1;
            engine->stopRequested = 0;
        }
        func_02089d08(engine);
        if (start) {
            ShttsStart(engine, text, callback, arg, 0);
        }
    } else if (mode == 1) {
        while (func_02089d1c(engine) != 0) {
            OS_Sleep(100);
        }
        func_02089cf4(engine);
        engine->busy = 1;
        engine->stopRequested = 0;
        func_02089d08(engine);
        ShttsStart(engine, text, callback, arg, 0);
    } else if (mode == 0) {
        if (func_02089d1c(engine) == 1) {
            func_0208d878(engine);
        }
        func_02089cf4(engine);
        engine->busy = 1;
        engine->stopRequested = 0;
        func_02089d08(engine);
        ShttsStart(engine, text, callback, arg, 0);
    } else {
        func_02089cf4(engine);
        if (engine->busy == 0) {
            start = 1;
            engine->busy = 1;
            engine->stopRequested = 0;
        }
        func_02089d08(engine);
        if (start) {
            ShttsStart(engine, text, callback, arg, 1);
        }
    }
    OS_UnlockMutex(&engine->unk_0EF08);
    return 0;
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
int func_0208d8fc(Shtts *engine) {
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
    engine->unk_0B720 = 0;
    engine->unk_08722 = 0;
    engine->unk_0B728 = 0;
    engine->unk_0B729 = 0;
    engine->unk_0B72A = 1;
    engine->unk_0B72B = 1;
    engine->unk_0B72C = 0;
    memset(engine->unk_0B730, 0, sizeof(engine->unk_0B730));
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

// Fades samples of unk_08724 out, or in and out, with func_0208e464
void func_0208e3b4(int unk, Shtts *engine) {
    s16 count = engine->unk_08724[0] - unk;

    if (unk > 0 && count > 0) {
        s16 half = engine->unk_08724[0] >> 1;

        func_0208e464(0x10000, 0, half, &engine->unk_08724[engine->unk_08724[0] + 1 - half]);
    } else if (unk == 0) {
        count = engine->unk_08724[0] - engine->unk_00004;
        if (count > 0) {
            func_0208e464(0, 0x10000, count, &engine->unk_08724[1]);
            func_0208e464(0x10000, 0, count, &engine->unk_08724[engine->unk_00004]);
        }
    }
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
    i = 0;
    if (count > 0) {
        for (; i < count; i++) {
            *data = (*data * gain) >> 16;
            data++;
            gain += step;
        }
    }
}

// Takes a block of samples the engine has made: doubles them, softening what is over 0x6978, applies the effects
// that are set, and hands the block to the caller's function at twice the rate, with a sample halfway between each
// two.
void func_0208e4e4(int count, s16 *buffer, Shtts *engine) {
    s16 block[0x1E1];
    s16 *out = &block[1];
    s16 coefficients[7] = { 0x0800, 0xFD83, 0x0800, 0xFF0D, 0x0612, 0xFF6A, 0x02DA };
    int i;
    int j;

    engine->unk_0EE18 = 0;
    for (i = 0; i < count; i++) {
        int sample = buffer[i] * 2;

        if (sample > 0x6978) {
            sample = ((sample - 0x6978) >> 1) + 0x6978;
            if (sample > 0x7FFF) {
                sample = 0x7FFF;
            }
        } else if (sample < -0x6978) {
            sample = -0x6978 + ((sample + 0x6978) >> 1);
            if (sample < -0x8000) {
                sample = -0x8000;
            }
        }
        buffer[i] = sample;
    }
    if (engine->unk_0EE18 == 1) {
        func_0208e83c(count, 6, coefficients, (int *)engine->unk_0B704, buffer, out, engine);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if ((s32)engine->cur.firGain > 0) {
        memset(out, 0, 0x200);
        func_02089ed4(count, buffer, out, engine);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if (engine->cur.ring != 0) {
        memset(out, 0, 0x200);
        func_0208b6c0((int *)&engine->unk_0B6FC, count, engine->cur.ring, buffer, out);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if (engine->cur.chorus != 0) {
        func_02085704(&engine->unk_0B4E8, buffer, out, count);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if (engine->cur.echoDelay > 0) {
        func_0208683c(count, buffer, out, engine);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    out[0] = (engine->unk_0B700 + buffer[0]) / 2;
    out[1] = buffer[0];
    for (i = 1, j = 2; i < count; i++) {
        out[j++] = (buffer[i - 1] + buffer[i]) / 2;
        out[j++] = buffer[i];
    }
    engine->unk_0B700 = buffer[count - 1];
    j = count * 2;
    if (engine->callback != NULL) {
        engine->callback((s16)j, out, engine->callbackArg);
    }
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

// A recursive filter over a block of samples, of 6 or 10 coefficients, with its history in state. Returns the
// largest magnitude that did not fit in 16 bits, 0 if all did.
int func_0208e83c(int count, int order, const s16 *coefficients, int *state, s16 *src, s16 *dst, Shtts *engine) {
    int peak = 0;
    int i;
    int j;
    int sum;
    int value;
    int sample;

    if (order == 6) {
        for (i = 0; i < count; i++) {
            sum = func_0208ea20(state, coefficients);
            for (j = order - 1; j > 0; j--) {
                state[j] = state[j - 1];
            }
            value = (src[i] << 7) - sum;
            state[0] = value << 5;
            sample = (value >> 7) + (value >> 8);
            if (sample > 0x6978) {
                sample = ((sample - 0x6978) >> 1) + 0x6978;
                if (sample > 0x7FFF) {
                    if (peak < sample) {
                        peak = sample;
                    }
                    sample = 0x7FFF;
                }
            } else if (sample < -0x6978) {
                sample = -0x6978 + ((sample + 0x6978) >> 1);
                if (sample < -0x8000) {
                    if (peak < -sample) {
                        peak = -sample;
                    }
                    sample = -0x8000;
                }
            }
            dst[i] = sample;
        }
    } else {
        for (i = 0; i < count; i++) {
            sum = func_0208ea84(state, coefficients);
            for (j = order - 1; j > 0; j--) {
                state[j] = state[j - 1];
            }
            value = (src[i] << 7) - sum;
            state[0] = value << 5;
            sample = value >> 7;
            if (sample > 0x7FFF) {
                if (peak < sample) {
                    peak = sample;
                }
                sample = 0x7FFF;
            } else if (sample < -0x8000) {
                if (peak < -sample) {
                    peak = -sample;
                }
                sample = -0x8000;
            }
            dst[i] = sample;
        }
    }
    return peak;
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
