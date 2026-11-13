// One function, apart from the rest because it does not match yet: the file it belongs to cannot be
// linked from source while it is in it.
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

// Gives each sound of a word its two lengths and its pitch, from tables indexed by the sound's vowel, its
// consonant class, its neighbours and its place in the word.
// Not matching yet: the same size as the original (0x8a8), with 59 instructions different. What is left is which
// registers and stack slots some values of the two loops get. Work on it was stopped part-way; the closest
// earlier attempts and what was tried are in the history of this file.
void func_02085d80(ShttsUnk_02087d98 **list, int count, s16 unk3, int first, int last, int unk6, char *vowel, s16 unk8,
                   Shtts *engine) {
    char consonants[0x80];
    char vowels[0x80];
    char kinds[0x80];
    int index = 0;
    s16 j;
    s16 m = 0;
    int group = 0;
    const char * name;
    int limit = 0;
    int voiced = 0;
    int place;
    s16 i;
    s8 k = 0;
    vowels[0] = *vowel;
    kinds[0] = *vowel;
    limit = count + 1;
    for (m = 1; m <= limit; m++) {
        name = list[m]->name;
        if (name[0] == '-') {
            consonants[m] = 0x1B;
            vowels[m] = vowels[m - 1];
            kinds[m] = kinds[m - 1];
        } else {
            for (j = 0; j < 0x90; j++) {
                if (strcmp(name, data_020bbf34[j]) == 0) {
                    consonants[m] = data_020b9d60[j];
                    vowels[m] = data_020b9cd0[j];
                    kinds[m] = data_020b9df0[j];
                    break;
                }
            }
        }
    }
    *vowel = vowels[count];
    index = func_02086628(unk3);
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
        if (place == 2 && unk6 == 2) {
            unk6 = 2;
        } else if (place == 0 && unk6 == 0) {
            unk6 = 0;
        } else {
            unk6 = 1;
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
            if (list[i]->unk_04 == 'Q') {
                q = 1;
            } else {
                q = 0;
            }
            value = data_020ba288[group].unk_00 + data_020ba288[group].unk_02[k] + data_020ba288[group].unk_3A[b] + data_020ba288[group].unk_70[v] +
                    data_020ba288[group].unk_A6[q] + data_020ba288[group].unk_AA[unk6] + data_020ba288[group].unk_B2[place];
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
            list[i]->unk_06 = value >> 6;
            if (list[i]->unk_06 < 25) {
                list[i]->unk_06 = 25;
            }
        } else {
            list[i]->unk_06 = 0;
        }

        {
            int base;

            k = consonants[i];
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
            v = vowels[i];
            if (v == 5) {
                list[i]->unk_08 = (data_020b9fb4.unk_02[k] + 0x1E00 + data_020b9fb4.unk_38[b] +
                                   data_020b9fb4.unk_B8[unk6] + data_020b9fb4.unk_C2[place]) >>
                                  6;
                list[i]->unk_0A = (s16)((data_020b9e80.unk_0C[k] + data_020b9e80.unk_42[b] +
                                         data_020b9e80.unk_A0[index] + data_020b9e80.unk_D8[unk6] +
                                         data_020b9e80.unk_EA[place]) >>
                                        6) +
                                  600;
                list[i]->unk_06 = 0;
            } else {
                list[i]->unk_08 = ((base = data_020ba0f4.unk_02[v] + 0x12FC) + data_020ba0f4.unk_0C[k] + data_020ba0f4.unk_42[b] +
                                   data_020ba0f4.unk_CA[unk6] + data_020ba0f4.unk_D2[place]) >>
                                  6;
                if (list[i]->unk_08 < 70) {
                    list[i]->unk_08 = 70;
                }
                list[i]->unk_0A = (s16)((data_020b9e80.unk_02[v] + data_020b9e80.unk_0C[k] +
                                         data_020b9e80.unk_42[b] + data_020b9e80.unk_A0[index] +
                                         data_020b9e80.unk_D8[unk6] + data_020b9e80.unk_EA[place]) >>
                                        6) +
                                  600;
                if (list[i]->unk_04 == 'L') {
                    list[i]->unk_08 += (s16)((base + data_020ba0f4.unk_42[v] + data_020ba0f4.unk_0C[v] - 0x5D) >> 6);
                }
                if (list[i]->unk_05 == 'U') {
                    list[i]->unk_08 = (list[i]->unk_08 * 0x599A) >> 15;
                    list[i]->unk_0A -= 30;
                }
            }
        }
    }
    func_020866bc(list, kinds, count, engine);
    for (i = 1; i <= count; i++) {
        list[i]->unk_0A = unk8 + list[i]->unk_0A;
    }
}
