// The last part of the engine: the sound list, speaking a text, the settings, the output stage.
// Where the engine's own files begin and end is not known. These four are cut where linking needs them cut:
// around the one function that does not match, and where the engine's strings are interrupted by those of a
// file of tables that is not decompiled.

#include "shtts.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ShttsParam_SetSpeed(Shtts *engine, int value);
int ShttsParam_SetPitch(Shtts *engine, int value);
int ShttsParam_SetVolume(Shtts *engine, int value);
int ShttsParam_SetWarp(Shtts *engine, int value);
int ShttsParam_SetRing(Shtts *engine, int value);
int ShttsParam_SetEchoDelay(Shtts *engine, int value);
int ShttsParam_SetEchoGain(Shtts *engine, int value);
int ShttsParam_SetFirDelay(Shtts *engine, int value);
int ShttsParam_SetFirGain(Shtts *engine, int value);
int ShttsParam_SetMode(Shtts *engine, int value);
int ShttsParam_SetChorus(Shtts *engine, int value);
int ShttsParam_SetAccent(Shtts *engine, int value);
int ShttsParam_SetPhrase(Shtts *engine, int value);
int ShttsParam_SetPitchTrans(Shtts *engine, int value);

// The next four are written in assembly in the original too: they use the processor's multiply-accumulate
// instructions for 32-bit by 16-bit fixed point, which the compiler does not emit, and their stack adjustments are
// not ones it would make. Each returns the sum of products of a filter's state and its coefficients.

// Takes a text and starts the thread that speaks it
static inline void ShttsStart(Shtts *engine, const char *text, ShttsCallback callback, void *arg, int runInCaller) {
    engine->runInCaller = runInCaller;
    ShttsEngine_SetCallback(engine, callback, arg);
    ShttsEngine_ResetSynthesis(engine);
    memset(engine->inputText, 0, sizeof(engine->inputText));
    strncpy(engine->inputText, text, sizeof(engine->inputText));
    if (engine->inputText[sizeof(engine->inputText) - 1] != 0) {
        // Too long: cut it, and not in the middle of a character of two bytes
        engine->inputText[sizeof(engine->inputText) - 1] = 0;
        if (engine->inputText[sizeof(engine->inputText) - 2] & 0x80) {
            engine->inputText[sizeof(engine->inputText) - 2] = 0;
        }
    }
    ShttsEngine_Start(engine);
}

// Fractional multiply of two Q15 values.
static inline s16 ShttsMath_MultiplyQ15(s16 a, s16 b) { return (a * b) >> 15; }

// Writes the sounds of lists of syllables to an array, max at most, and returns how many. A syllable named "-"
// lengthens the sound before it, or repeats it if that would be too long or would change endPitch.
int ShttsPhone_BuildFromSyllables(ShttsPhonePhraseGroup *lists, ShttsSound *sounds, int max) {
    char name[12];
    ShttsPhoneAccentGroup *list1;
    ShttsPhoneWord *list2;
    ShttsSyllable *syllable;
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
                if (syllable->vowelMark == 'U' && syllable->vowelLength > 20) {
                    syllable->consonantLength += syllable->vowelLength - 20;
                    syllable->vowelLength = 20;
                }
                if (syllable->consonantLength != 0) {
                    // The consonant, which is all of the name but its last letter
                    length = strlen(syllable->name);
                    strncpy(name, syllable->name, length - 1);
                    name[length - 1] = 0;
                    sound->phoneme = ShttsPhone_FindPhoneme(name);
                    sound->length = syllable->consonantLength;
                    sound->pitch = 0;
                    sound->gain = 0;
                    sound->unk_C = 0;
                    sound->endPitch = 0;
                    sound->qMark = syllable->qMark;
                    sound->vowelMark = syllable->vowelMark;
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
                    if (sound[-1].length + syllable->vowelLength > 0x7FFF
                        || (sound[-1].endPitch != 0 && sound[-1].endPitch != syllable->pitch)) {
                        sound->phoneme = sound[-1].phoneme;
                        sound->length = syllable->vowelLength;
                        sound->pitch = syllable->pitch;
                        sound->gain = syllable->gain;
                        sound->unk_C = 0;
                        sound->endPitch = 0;
                        sound->qMark = syllable->qMark;
                        sound->vowelMark = syllable->vowelMark;
                        if (sound->pitch <= 0) {
                            sound->pitch = 0;
                        }
                        if (sound->gain <= 0) {
                            sound->gain = 0;
                        }
                        sound++;
                        count++;
                        if (count < max - 1) {
                        } else {
                            goto end;
                        }
                    } else {
                        sound[-1].length = sound[-1].length + syllable->vowelLength;
                        sound[-1].endPitch = syllable->pitch;
                    }
                } else {
                    // The vowel
                    sound->phoneme = ShttsPhone_FindPhoneme(&syllable->name[last]);
                    sound->length = syllable->vowelLength;
                    sound->pitch = syllable->pitch;
                    sound->gain = syllable->gain;
                    sound->unk_C = 0;
                    sound->endPitch = 0;
                    sound->qMark = syllable->qMark;
                    sound->vowelMark = syllable->vowelMark;
                    if (sound->pitch <= 0) {
                        sound->pitch = 0;
                    }
                    if (sound->gain <= 0) {
                        sound->gain = 0;
                    }
                    sound++;
                    count++;
                    if (count < max - 1) {
                    } else {
                        goto end;
                    }
                }
            }
            if (list2->pause != 0) {
                sound->phoneme = ShttsPhone_FindPhoneme("S");
                sound->length = list2->pause;
                sound->pitch = 0;
                sound->gain = 0;
                sound->unk_C = 0;
                sound->endPitch = 0;
                sound->qMark = syllable->qMark;
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
int ShttsPhone_ReadStdin(ShttsSound *sounds, Shtts *engine) {
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
                engine->cur.unitSet = 0;
                break;
            case 'M':
            case 'm':
                engine->cur.unitSet = 1;
                break;
            case 'B':
            case 'b':
                if (line[2] == '+') {
                    sscanf(line + 3, "%hd", &bOffset);
                } else if (line[2] == '-') {
                    sscanf(line + 2, "%hd", &bOffset);
                }
                break;
            case 'S':
            case 's':
                sscanf(line + 2, "%hd", &aScale);
                break;
            case 'V':
            case 'v':
                sscanf(line + 2, "%hd", &cOffset);
                break;
            case 'L':
            case 'l':
                sscanf(line + 2, "%hd", &unk_C);
                break;
            }
        } else if (line[0] == '*') {
        } else if (line[0] == '-') {
            sscanf(line + 1, "%d %d", &a, &b);
            sound[-1].length += (s16)((aScale * a + 50) / 100);
            sound[-1].endPitch = bOffset + b;
        } else {
            c = 0;
            b = 0;
            a = 0;
            sscanf(line, "%s %d %d %d %c %c", name, &a, &b, &c, &sound->qMark, &sound->vowelMark);
            sound->phoneme = ShttsPhone_FindPhoneme(name);
            sound->length = (aScale * a + 50) / 100;
            count++;
            sound->pitch = bOffset + b;
            sound->gain = cOffset + c;
            sound->unk_C = unk_C;
            sound->endPitch = 0;
            if (sound->pitch <= 0) {
                sound->pitch = 0;
            }
            if (sound->gain <= 0) {
                sound->gain = 0;
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
// second sound that has a pitch, and 10 adds "n a" or "n e N" after a last sound of "N". Then by flags: bits 4 to 6
// change the last sound, and bits 0 to 2 repeat it, with the pitch set from the highest in the array.
void ShttsPhone_ShapeEnding(ShttsSound *sounds, int count, int flags, Shtts *engine) {
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
        if (end[-2].pitch != 0) {
            end[-2].pitch += 50;
            end[-2].length += 50;
        } else if (end[-3].pitch != 0) {
            end[-3].pitch += 50;
            end[-3].length += 50;
        }
        end[-1].pitch += 450;
        end[-1].length += 200;
        last++;
        count++;
        end[1].phoneme = end[0].phoneme;
        end[1].length = end[0].length;
        end[0].phoneme = end[-1].phoneme;
        end[0].pitch = end[-1].pitch - 40;
        if (end[0].pitch < 0) {
            end[0].pitch = 0;
        }
        end[0].length = 10;
        end[0].gain = end[-1].gain - 200;
        if (end[0].gain < 0) {
            end[0].gain = 0;
        }
        end[-1].endPitch = 0;
        end[0].endPitch = 0;
        end[0].vowelMark = '-';
    } else if (engine->cur.mode == 2) {
        int second;

        if (count <= 4) {
            return;
        }
        second = 0;
        for (last = sounds + 1; last < end; last++) {
            if (last->pitch != 0) {
                if (second == 1) {
                    last->endPitch = 0;
                    second = 0;
                    last->pitch += 300;
                    last->length += 150;
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
        length = end[-1].length;
        if (length <= 120) {
            pitch = end[-1].pitch;
            length = 120;
        } else if (end[-1].endPitch == 0) {
            pitch = end[-1].pitch - 50;
        } else {
            length /= 2;
            pitch = end[-1].endPitch - 50;
        }
        if ((end->length < 800 || count < 8) && !(end[-2].phoneme == 14 && before == 1)
            && !(before == 3 && end[-2].phoneme != 8 && end[-2].phoneme != 19)) {
            end[2].length = end[0].length;
            end[2].phoneme = ShttsPhone_FindPhoneme("S");
            end[0].phoneme = ShttsPhone_FindPhoneme("n");
            end[0].length = length / 2;
            end[0].gain = 0;
            end[0].pitch = 0;
            end[1].phoneme = ShttsPhone_FindPhoneme("a");
            end[1].length = length;
            end[1].pitch = pitch + 50;
            end[1].endPitch = 0;
            end[1].gain = end[-1].gain;
            last += 2;
            count += 2;
        } else if (end->length >= 800 && before != 5 && before != 4
                   && !(before == 1 && end[-2].phoneme != 25 && end[-2].phoneme != 12)) {
            end[3].length = end[0].length;
            end[3].phoneme = ShttsPhone_FindPhoneme("S");
            end[0].phoneme = ShttsPhone_FindPhoneme("n");
            end[0].length = length / 2;
            end[0].gain = 0;
            end[0].pitch = 0;
            end[1].phoneme = ShttsPhone_FindPhoneme("e");
            end[1].length = length;
            end[1].pitch = pitch;
            if (end[1].pitch < 0) {
                end[1].pitch = 0;
            }
            end[1].endPitch = 0;
            end[1].gain = end[-1].gain;
            end[2].phoneme = ShttsPhone_FindPhoneme("N");
            end[2].length = 1.4 * length;
            end[2].pitch = pitch;
            last += 3;
            count += 3;
            if (end[2].pitch < 0) {
                end[2].pitch = 0;
            }
            end[2].endPitch = 0;
            end[2].gain = end[-1].gain;
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
    high = sounds[1].pitch;
    low = high;
    for (i = 1; i < count; i++) {
        sound = &sounds[i];
        if (sounds[i].pitch != 0) {
            if (sounds[i].pitch > high) {
                high = sound->pitch;
            }
            if (sounds[i].pitch < low) {
                low = sound->pitch;
            }
            if (sounds[i].endPitch != 0) {
                if (sounds[i].endPitch > high) {
                    high = sound->endPitch;
                }
                if (sounds[i].endPitch < low) {
                    low = sound->endPitch;
                }
            }
        }
    }
    sounds += count;
    sound = sounds - 1;
    if (sound->endPitch != 0) {
        p = &sound->endPitch;
    } else {
        p = &sound->pitch;
    }
    n = flags & 0x70;
    if (n != 0) {
        sound = sounds - 1;
        n >>= 4;
        sound->gain += n * 30;
        sound->length *= 0.8;
        if (sound->length < 10) {
            sound->length = 10;
        }
        *p += n * 100;
    }
    n = flags & 7;
    if (n != 0) {
        s16 step;
        s16 top;
        s16 bottom;

        sounds[1].phoneme = sounds[0].phoneme;
        sounds[1].length = sounds[0].length;
        sounds[2].phoneme = 7;
        pitch = sounds[-1].pitch + n * (high - sounds[-1].pitch) / 3;
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
        sounds[0].pitch = top;
        sounds[0].endPitch = 0;
        sounds[0].length = sounds[-1].length >> 1;
        if (sounds[0].length < 10) {
            sounds[0].length = 10;
        }
        sounds[0].gain = sounds[-1].gain - 90;
        if (sounds[0].gain < 0) {
            sounds[0].gain = 0;
        }
        if (sounds[-1].endPitch != 0) {
            sounds[-1].endPitch = bottom;
        }
        sounds[-1].pitch = bottom;
        sounds[-1].length *= 0.8;
        if (sounds[-1].length < 10) {
            sounds[-1].length = 10;
        }
    }
}

void ShttsEngine_SetCallback(Shtts *engine, ShttsCallback callback, void *arg) {
    engine->callback = callback;
    engine->callbackArg = arg;
}

// Speaks a text, of 1023 bytes at most. If the engine is speaking already, mode says what to do: 0 stops that,
// 1 waits for it to end, and 2 does not speak the new text.
int Shtts_Speak(Shtts *engine, const char *text, u32 mode, u32 priority, ShttsCallback callback, void *arg) {
    int start = 0;

    SDK_USING_MIDDLEWARE(ShttsMiddlewareTag);
    if (mode > 2) {
        return -1;
    }
    if (priority > 31) {
        return -1;
    }
    engine->threadPriority = priority;
    OS_LockMutex(&engine->speakMutex);
    if (mode == 2) {
        ShttsEngine_Lock(engine);
        if (engine->busy == 0) {
            start = 1;
            engine->busy = 1;
            engine->stopRequested = 0;
        }
        ShttsEngine_Unlock(engine);
        if (start) {
            ShttsStart(engine, text, callback, arg, 0);
        }
    } else if (mode == 1) {
        while (ShttsEngine_IsBusy(engine) != 0) {
            OS_Sleep(100);
        }
        ShttsEngine_Lock(engine);
        engine->busy = 1;
        engine->stopRequested = 0;
        ShttsEngine_Unlock(engine);
        ShttsStart(engine, text, callback, arg, 0);
    } else if (mode == 0) {
        if (ShttsEngine_IsBusy(engine) == 1) {
            Shtts_Stop(engine);
        }
        ShttsEngine_Lock(engine);
        engine->busy = 1;
        engine->stopRequested = 0;
        ShttsEngine_Unlock(engine);
        ShttsStart(engine, text, callback, arg, 0);
    } else {
        ShttsEngine_Lock(engine);
        if (engine->busy == 0) {
            start = 1;
            engine->busy = 1;
            engine->stopRequested = 0;
        }
        ShttsEngine_Unlock(engine);
        if (start) {
            ShttsStart(engine, text, callback, arg, 1);
        }
    }
    OS_UnlockMutex(&engine->speakMutex);
    return 0;
}

// Asks the engine to stop synthesizing, and waits until it has, unless called from the engine's own thread
int Shtts_Stop(Shtts *engine) {
    int stopping = 0;
    OSThread *current;

    ShttsEngine_Lock(engine);
    if (engine->busy == 1) {
        stopping = 1;
        engine->stopRequested = 1;
    }
    ShttsEngine_Unlock(engine);

    current = OS_GetCurrentThread();
    if (stopping == 1 && current != &engine->thread) {
        while (ShttsEngine_IsBusy(engine) == 1) {
            OS_Sleep(10);
        }
    }
    return 0;
}

// Sets up an engine in memory the caller provides.
int Shtts_Init(Shtts *engine) {
    MIi_CpuClear32(0, engine, sizeof(Shtts));
    engine->sampleRate = 11000;
    engine->frameLength = 0x37;
    engine->dictWords = 0;
    engine->dictCount = 0;
    engine->cur.unitSet = 0;
    engine->cur.pitch = 0x13C2;
    engine->cur.accent = 100;
    engine->cur.phrase = 100;
    engine->cur.speed = 120;
    engine->cur.volume = 0;
    engine->splitUnitAtConsonant = 0;
    engine->cur.echoGain = 30;
    engine->cur.echoDelay = 0;
    engine->cur.accentRule = 0;
    engine->useInputText = 0;
    engine->cur.firDelay = 0;
    engine->cur.chorus = 0;
    engine->cur.firGain = 0;
    engine->cur.ring = 0;
    engine->ringPhase = 0;
    engine->samplesOwed = 0;
    engine->mixSize = 480;
    engine->mixHalfSize = 240;
    engine->mixedCount = 0;
    engine->cur.accentRule = 0;
    engine->useInputText = 0;
    engine->cur.firDelay = 0;
    engine->cur.chorus = 0;
    engine->cur.firGain = 0;
    engine->cur.ring = 0;
    engine->ringPhase = 0;
    engine->unk_0B720 = 0;
    engine->unvoicedBlock = 0;
    engine->phonemeFlag04 = 0;
    engine->phonemeFlag08 = 0;
    engine->phonemeNoFlag10 = 1;
    engine->phonemeFlag80 = 1;
    engine->phonemeFlag02 = 0;
    memset(engine->echoFilterState, 0, sizeof(engine->echoFilterState));
    engine->echoCount = 0;
    engine->firCount = 0;
    engine->prevPhraseNotFirst = 0;
    engine->prevPhraseStrength = 0;
    engine->prevRunWeight = 0;
    engine->busy = 0;
    engine->stopRequested = 0;
    engine->runInCaller = 0;
    engine->phonesFromStdin = 0;
    engine->cur.warp = 100;
    engine->cur.warpScale = 0x400;
    engine->cur.warpOrder = 10;
    engine->outputFilterOn = 1;
    OS_InitMutex(&engine->mutex);
    OS_InitMutex(&engine->speakMutex);
    engine->phoneme = 0;
    engine->stretchSide = 0;
    Shtts_ResetParams(engine);
    return 0;
}

int Shtts_ResetParams(Shtts *engine) {
    ShttsParam_SetDefaults(engine);
    return 0;
}

// 0 when idle, 1 while synthesizing, 2 while synthesizing and asked to stop
int Shtts_GetState(Shtts *engine) {
    int state;

    ShttsEngine_Lock(engine);
    state = engine->busy;
    if (state == 1 && engine->stopRequested == 1) {
        state = 2;
    }
    ShttsEngine_Unlock(engine);
    return state;
}

// The name of the sound being spoken, "X" if there is none
const char *Shtts_GetPhonemeName(Shtts *engine) {
    if (engine->phoneme < 0 || engine->phoneme > 38) {
        return ShttsPhonemeNames[7];
    }
    return ShttsPhonemeNames[engine->phoneme];
}

// Sets one of the caller's voice settings, which only works while the engine is idle. Each setting has its own
// range of values, see the functions below.
// objdiff scores this 99.95%, though its instructions and relocations are those of the original.
int Shtts_SetParam(Shtts *engine, int param, int value) {
    if (ShttsEngine_IsBusy(engine) != 0) {
        return -2;
    }
    switch (param) {
    case 1:
        return ShttsParam_SetSpeaker(engine, value);
    case 2:
        return ShttsParam_SetStyle(engine, value);
    case 3:
        return ShttsParam_SetSpeed(engine, value);
    case 4:
        return ShttsParam_SetPitch(engine, value);
    case 5:
        return ShttsParam_SetVolume(engine, value);
    case 6:
        return ShttsParam_SetWarp(engine, value);
    case 7:
        return ShttsParam_SetRing(engine, value);
    case 8:
        return ShttsParam_SetEchoDelay(engine, value);
    case 9:
        return ShttsParam_SetEchoGain(engine, value);
    case 10:
        return ShttsParam_SetFirDelay(engine, value);
    case 11:
        return ShttsParam_SetFirGain(engine, value);
    case 12:
        return ShttsParam_SetMode(engine, value);
    case 13:
        return ShttsParam_SetChorus(engine, value);
    case 14:
        return ShttsParam_SetAccent(engine, value);
    case 15:
        return ShttsParam_SetPhrase(engine, value);
    case 16:
        return ShttsParam_SetPitchTrans(engine, value);
    default:
        return -1;
    }
}

// Reads one of the caller's voice settings, by the numbers of Shtts_SetParam. The speaker and the way of speaking,
// settings 1 and 2, cannot be read back.
int Shtts_GetParam(Shtts *engine, int param) {
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
        return ShttsParam_GetMode(engine);
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
int ShttsParam_SetSpeaker(Shtts *engine, int value) {
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
    ShttsParam_ApplyPreset(engine, name);
    return 0;
}

// Setting 2: the way of speaking, 0x201 to 0x211
int ShttsParam_SetStyle(Shtts *engine, int value) {
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
    ShttsParam_ApplyPreset(engine, name);
    return 0;
}

// Applies the preset of a name to the caller's settings
int ShttsParam_ApplyPreset(Shtts *engine, const char *name) {
    const char *commands = ShttsParam_FindPreset(name);

    if (commands == NULL) {
        return -1;
    }
    ShttsParam_RunCommands(engine, commands);
    return 0;
}

// The commands of the preset of a name, or NULL
const char *ShttsParam_FindPreset(const char *name) {
    int i;

    for (i = 0; i < SHTTS_PRESET_COUNT; i++) {
        if (strcmp(name, ShttsPresets[i].name) == 0) {
            return ShttsPresets[i].commands;
        }
    }
    return NULL;
}

// Applies each command of a string of them
int ShttsParam_RunCommands(Shtts *engine, const char *commands) {
    const char *command = strchr(commands, '#');

    while (command != NULL) {
        ShttsParam_RunCommand(command, 0, engine);
        command = strchr(command + 1, '#');
    }
    return 0;
}

int ShttsParam_SetSpeed(Shtts *engine, int value) {
    if (value < 20 || value > 2200) {
        return -1;
    }
    engine->set.speed = value;
    return 0;
}

int ShttsParam_SetPitch(Shtts *engine, int value) {
    if (value < 3500 || value > 7000) {
        return -1;
    }
    engine->set.pitch = value;
    return 0;
}

int ShttsParam_SetVolume(Shtts *engine, int value) {
    if (value < -10000 || value > 360) {
        return -1;
    }
    engine->set.volume = value;
    return 0;
}

int ShttsParam_SetWarp(Shtts *engine, int value) {
    if (value < 80 || value > 130) {
        return -1;
    }
    ShttsParam_StoreWarp(value, 0, engine);
    return 0;
}

int ShttsParam_SetRing(Shtts *engine, int value) {
    if (value < 0 || value > 5000) {
        return -1;
    }
    engine->set.ring = value;
    return 0;
}

int ShttsParam_SetEchoDelay(Shtts *engine, int value) {
    if (value < 0 || value > 3000) {
        return -1;
    }
    engine->set.echoDelay = value;
    return 0;
}

int ShttsParam_SetEchoGain(Shtts *engine, int value) {
    if (value < 0 || value > 100) {
        return -1;
    }
    engine->set.echoGain = value;
    return 0;
}

int ShttsParam_SetFirDelay(Shtts *engine, int value) {
    if (value < 0 || value > 1000) {
        return -1;
    }
    engine->set.firDelay = value;
    return 0;
}

int ShttsParam_SetFirGain(Shtts *engine, int value) {
    if (value < 0 || value > 100) {
        return -1;
    }
    engine->set.firGain = value;
    return 0;
}

int ShttsParam_SetMode(Shtts *engine, int value) {
    if (value < 0 || value > 3) {
        return -1;
    }
    engine->set.accentRule = 0;
    engine->set.mode = 0;
    if (value == 1) {
        engine->set.mode = 1;
    } else if (value == 2) {
        engine->set.mode = 2;
    } else if (value == 3) {
        engine->set.accentRule = 1;
    }
    return 0;
}

int ShttsParam_SetChorus(Shtts *engine, int value) {
    if (value < 0 || value > 100) {
        return -1;
    }
    engine->set.chorus = value;
    return 0;
}

int ShttsParam_SetAccent(Shtts *engine, int value) {
    if (value < 0 || value > 400) {
        return -1;
    }
    engine->set.accent = value;
    return 0;
}

int ShttsParam_SetPhrase(Shtts *engine, int value) {
    if (value < 0 || value > 400) {
        return -1;
    }
    engine->set.phrase = value;
    return 0;
}

int ShttsParam_SetPitchTrans(Shtts *engine, int value) {
    if (value < 0 || value > 1000) {
        return -1;
    }
    engine->set.pitchTrans = value;
    return 0;
}

// The value that ShttsParam_SetMode was given
int ShttsParam_GetMode(Shtts *engine) {
    if (engine->set.accentRule == 0) {
        return engine->set.mode;
    }
    return 3;
}

// The number of a sound by its name, with "pau", a pause, for silence. A name that is not one is "X", 7.
int ShttsPhone_FindPhoneme(const char *name) {
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

// Fades samples of addWave out, or in and out, with ShttsSynth_FadeSamples
void ShttsSynth_FadeWave(int unk, Shtts *engine) {
    s16 count = engine->addWave[0] - unk;

    if (unk > 0 && count > 0) {
        s16 half = engine->addWave[0] >> 1;

        ShttsSynth_FadeSamples(0x10000, 0, half, &engine->addWave[engine->addWave[0] + 1 - half]);
    } else if (unk == 0) {
        count = engine->addWave[0] - engine->frameLength;
        if (count > 0) {
            ShttsSynth_FadeSamples(0, 0x10000, count, &engine->addWave[1]);
            ShttsSynth_FadeSamples(0x10000, 0, count, &engine->addWave[engine->frameLength]);
        }
    }
}

// Fades samples in from silence, or out to it
void ShttsSynth_FadeSamples(int from, int to, int count, s16 *data) {
    int gain;
    int step;
    s16 i;

    if (count <= 0) {
        return;
    }
    step = ShttsMath_DivideRounded(to - from, count);
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
void ShttsOutput_SendBlock(int count, s16 *buffer, Shtts *engine) {
    s16 block[0x1E1];
    s16 *out = &block[1];
    s16 coefficients[7] = { 0x0800, 0xFD83, 0x0800, 0xFF0D, 0x0612, 0xFF6A, 0x02DA };
    int i;
    int j;

    engine->outputFilterOn = 0;
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
    if (engine->outputFilterOn == 1) {
        ShttsSynth_FilterRecursive(count, 6, coefficients, (int *)engine->outputFilterState, buffer, out, engine);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if ((s32)engine->cur.firGain > 0) {
        memset(out, 0, 0x200);
        ShttsEffect_ApplyFir(count, buffer, out, engine);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if (engine->cur.ring != 0) {
        memset(out, 0, 0x200);
        ShttsEffect_ApplyRing((int *)&engine->ringPhase, count, engine->cur.ring, buffer, out);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if (engine->cur.chorus != 0) {
        ShttsEffect_ApplyChorus(&engine->chorusState, buffer, out, count);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    if (engine->cur.echoDelay > 0) {
        ShttsEffect_ApplyEcho(count, buffer, out, engine);
        for (i = 0; i < count; i++) {
            buffer[i] = out[i];
        }
    }
    out[0] = (engine->lastOutputSample + buffer[0]) / 2;
    out[1] = buffer[0];
    for (i = 1, j = 2; i < count; i++) {
        out[j++] = (buffer[i - 1] + buffer[i]) / 2;
        out[j++] = buffer[i];
    }
    engine->lastOutputSample = buffer[count - 1];
    j = count * 2;
    if (engine->callback != NULL) {
        engine->callback((s16)j, out, engine->callbackArg);
    }
}

// A product in fixed point, doubled
asm int ShttsEffect_MultiplyDoubled(int a, int b) {
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
int ShttsSynth_FilterRecursive(int count, int order, const s16 *coefficients, int *state, s16 *src, s16 *dst, Shtts *engine) {
    int peak = 0;
    int i;
    int j;
    int sum;
    int value;
    int sample;

    if (order == 6) {
        for (i = 0; i < count; i++) {
            sum = ShttsSynth_SumFilter6(state, coefficients);
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
            sum = ShttsSynth_SumFilter10(state, coefficients);
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

asm int ShttsSynth_SumFilter6(const int *state, const s16 *coefficients) {
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

asm int ShttsSynth_SumFilter10(const int *state, const s16 *coefficients) {
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
