// One function, which was cut apart from the rest while it did not match.
// Where the engine's own files begin and end is not known. These four were cut where linking needed them cut:
// around that function, and where the engine's strings are interrupted by those of a file of tables that is not
// decompiled.

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

// Gives each sound of a word its two lengths and its pitch, from tables indexed by the sound's vowel, its
// consonant class, its neighbours and its place in the word.
// The flags and the place are s8, as the caller's variables are; the vowel of the second half is a variable of its
// own, read before the consonant; and the first loop's limit is compared through a cast. Each of those is needed
// for the registers and the stack slots to come out as the original's.
void ShttsDuration_SetWordSyllables(ShttsSyllable **list, int count, s16 moraCount, s8 first, s8 last, s8 textPlace, char *vowel, s16 volume,
                   Shtts *engine) {
    char consonants[0x80];
    char vowels[0x80];
    char kinds[0x80];
    int index = 0;
    const char * name;
    s16 m = 0;
    int group = 0;
    s16 j;
    int limit = 0;
    int voiced = 0;
    int place;
    s16 i;
    s8 k = 0;
    vowels[0] = *vowel;
    kinds[0] = *vowel;
    limit = count + 1;
    for (m = 1; m <= (int)limit; m++) {
        name = list[m]->name;
        if (name[0] == '-') {
            consonants[m] = 0x1B;
            vowels[m] = vowels[m - 1];
            kinds[m] = kinds[m - 1];
        } else {
            for (j = 0; j < 0x90; j++) {
                if (strcmp(name, ShttsSyllableNames[j]) == 0) {
                    consonants[m] = ShttsSyllableConsonantClass[j];
                    vowels[m] = ShttsSyllableVowelClass[j];
                    kinds[m] = ShttsSyllableLengthKind[j];
                    break;
                }
            }
        }
    }
    *vowel = vowels[count];
    index = ShttsDuration_ClassifyMoraCount(moraCount);
    for (i = 1; i <= count; i++) {
        char b;
        s16 v;

        if (last && i == count) {
            place = 2;
        } else if (first && i == 1) {
            place = 0;
        } else {
            place = 1;
        }
        if (place == 2 && textPlace == 2) {
            textPlace = 2;
        } else if (place == 0 && textPlace == 0) {
            textPlace = 0;
        } else {
            textPlace = 1;
        }
        if (consonants[i] < 0x1A) {
            s8 q;
            s16 value;

            k = kinds[i];
            switch (k) {
            case 6:
            case 7:
            case 8:
                group = 0;
                voiced = 0;
                break;
            case 9:
            case 10:
            case 11:
                group = 1;
                voiced = 0;
                break;
            case 12:
            case 13:
                group = 2;
                voiced = 0;
                break;
            case 14:
            case 15:
            case 16:
            case 24:
                group = 3;
                voiced = 0;
                break;
            case 17:
            case 18:
                group = 4;
                voiced = 0;
                break;
            case 19:
                group = 5;
                voiced = 0;
                break;
            case 20:
            case 21:
                group = 6;
                voiced = 0;
                break;
            case 23:
            case 27:
                group = 7;
                voiced = 0;
                break;
            case 26:
            case 28:
                k = (s8)(k - 20);
                group = 0;
                voiced = 1;
                break;
            case 29:
            case 31:
                k = (s8)(k - 20);
                group = 1;
                voiced = 1;
                break;
            case 32:
            case 33:
                k = (s8)(k - 20);
                group = 2;
                voiced = 1;
                break;
            case 25:
                k = (s8)(k - 10);
                group = 3;
                voiced = 1;
                break;
            case 34:
                k = (s8)(k - 15);
                group = 5;
                voiced = 1;
                break;
            case 40:
                k = (s8)(k - 30);
                group = 1;
                voiced = 1;
                break;
            }
            b = vowels[i - 1];
            if (place == 0) {
                b = 0x1A;
            }
            v = vowels[i];
            if (list[i]->qMark == 'Q') {
                q = 1;
            } else {
                q = 0;
            }
            value = ShttsConsonantLengths[group].base + ShttsConsonantLengths[group].byKind[k] + ShttsConsonantLengths[group].byPrevVowel[b] + ShttsConsonantLengths[group].byVowel[v] +
                    ShttsConsonantLengths[group].byQMark[q] + ShttsConsonantLengths[group].byTextPlace[textPlace] + ShttsConsonantLengths[group].byRunPlace[place];
            if (voiced == 1) {
                switch (k) {
                case 9:
                    value += 0xE40;
                    break;
                case 10:
                    value += 0xA00;
                    break;
                case 11:
                    value += 0x8C6;
                    break;
                case 15:
                    value += 0x6E0;
                    break;
                case 8:
                    value += 0xA0D;
                    break;
                case 12:
                    value += 0x880;
                    break;
                case 13:
                    value += 0xBB3;
                    break;
                case 6:
                    value += 0x673;
                    break;
                case 19:
                    value += 0xA40;
                    break;
                }
            }
            list[i]->consonantLength = value >> 6;
            if (list[i]->consonantLength < 25) {
                list[i]->consonantLength = 25;
            }
        } else {
            list[i]->consonantLength = 0;
        }

        {
            int base;
            s16 vowelClass;

            k = consonants[i];
            vowelClass = vowels[i];
            if (k == 0x1B) {
                if (place == 0) {
                    k = 0x1A;
                } else {
                    k = vowels[i - 1];
                }
            }
            b = consonants[i + 1];
            if (b == 0x1B) {
                b = vowels[i + 1];
            }
            if (place == 2) {
                b = 12;
            }
            if (vowelClass == 5) {
                list[i]->vowelLength = (ShttsVowel5Lengths.byConsonant[k] + 0x1E00 + ShttsVowel5Lengths.byNext[b] +
                                   ShttsVowel5Lengths.byTextPlace[textPlace] + ShttsVowel5Lengths.byRunPlace[place]) >>
                                  6;
                list[i]->gain = (s16)((ShttsSyllableGains.byConsonant[k] + ShttsSyllableGains.byNext[b] +
                                         ShttsSyllableGains.byMoraCount[index] + ShttsSyllableGains.byTextPlace[textPlace] +
                                         ShttsSyllableGains.byRunPlace[place]) >>
                                        6) +
                                  600;
                list[i]->consonantLength = 0;
            } else {
                list[i]->vowelLength = ((base = ShttsVowelLengths.byVowel[vowelClass] + 0x12FC) + ShttsVowelLengths.byConsonant[k] + ShttsVowelLengths.byNext[b] +
                                   ShttsVowelLengths.byTextPlace[textPlace] + ShttsVowelLengths.byRunPlace[place]) >>
                                  6;
                if (list[i]->vowelLength < 70) {
                    list[i]->vowelLength = 70;
                }
                list[i]->gain = (s16)((ShttsSyllableGains.byVowel[vowelClass] + ShttsSyllableGains.byConsonant[k] +
                                         ShttsSyllableGains.byNext[b] + ShttsSyllableGains.byMoraCount[index] +
                                         ShttsSyllableGains.byTextPlace[textPlace] + ShttsSyllableGains.byRunPlace[place]) >>
                                        6) +
                                  600;
                if (list[i]->qMark == 'L') {
                    list[i]->vowelLength += (s16)((ShttsVowelLengths.byConsonant[vowelClass] + (base + ShttsVowelLengths.byNext[vowelClass]) - 0x5D) >> 6);
                }
                if (list[i]->vowelMark == 'U') {
                    list[i]->vowelLength = (list[i]->vowelLength * 0x599A) >> 15;
                    list[i]->gain -= 30;
                }
            }
        }
    }
    ShttsDuration_ApplySpeed(list, kinds, count, engine);
    for (i = 1; i <= count; i++) {
        list[i]->gain = volume + list[i]->gain;
    }
}
