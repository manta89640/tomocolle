// The middle of the engine: reading words and commands, the text front end, fixed-point arithmetic, effects, pitch,
// the unit lookup and the thread.
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

// Scales unk_06 and unk_08 of each sound of a list by the speed, a percentage. For some kinds of sound, unk_06 only
// if the sound has a 'Q'.
void func_020866bc(ShttsUnk_02087d98 **list, char *kinds, int count, Shtts *engine) {
    s16 i;

    for (i = 1; i <= count; i++) {
        switch (kinds[i]) {
        case 6:
        case 7:
        case 8:
        case 14:
        case 23:
        case 24:
        case 26:
        case 27:
        case 28:
            if (list[i]->unk_04 == 'Q') {
                list[i]->unk_06 = (engine->cur.speed * list[i]->unk_06 + 50) / 100;
            }
            list[i]->unk_08 = (engine->cur.speed * list[i]->unk_08 + 50) / 100;
            break;
        default:
            list[i]->unk_06 = (engine->cur.speed * list[i]->unk_06 + 50) / 100;
            list[i]->unk_08 = (engine->cur.speed * list[i]->unk_08 + 50) / 100;
            break;
        }
    }
}

// Empties the echo's history
void func_02086814(Shtts *engine) {
    engine->unk_0E624 = 0;
    memset(engine->echoBuffer, 0, sizeof(engine->echoBuffer));
}

// The echo: adds to each sample the one of a delay ago, from a ring of 3001 that it feeds with the samples
// filtered and scaled by the gain
void func_0208683c(int count, s16 *src, s16 *dst, Shtts *engine) {
    int i;
    s16 gain = engine->cur.echoGain << 8;

    for (i = 0; i < count; i++) {
        int sample;
        int filtered = func_02086a20(engine->unk_0B730, data_020bac28, src[i] << 13) << 1;

        engine->unk_0B730[3] = engine->unk_0B730[2];
        engine->unk_0B730[2] = src[i] << 13;
        engine->unk_0B730[1] = engine->unk_0B730[0];
        engine->unk_0B730[0] = filtered << 1;
        if (engine->unk_0E624 < engine->cur.echoDelay) {
            engine->echoBuffer[engine->unk_0E624] = func_0208e81c(filtered, gain);
            sample = src[i];
        } else {
            engine->echoBuffer[engine->unk_0E624 % 3001] =
                func_0208e81c(filtered + engine->echoBuffer[(engine->unk_0E624 - engine->cur.echoDelay) % 3001], gain);
            sample = src[i] + (engine->echoBuffer[(engine->unk_0E624 - engine->cur.echoDelay) % 3001] >> 12);
        }
        if (sample > 0x7FFF) {
            sample = 0x7FFF;
        } else if (sample < -0x8000) {
            sample = -0x7FFF;
        }
        dst[i] = sample;
        engine->unk_0E624++;
    }
}

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

void func_02086a74(char *value, char **out) {
    *out = value;
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

// Reads lines of the text into words, until one ends a sentence or has a pause. A line is a word and up to six
// numbers after it, with spaces between. Returns how many words there are.
int func_02086b90(ShttsUnk_020870c8 *words, ShttsUnk_02087d98 *sounds, Shtts *engine) {
    char line[0xC0];
    char word[0xC1];
    char *cursor;
    int accent;
    int phrase;
    int pause;
    int end;
    char *text = engine->unk_01960;
    int n; // The word's type, then the speed's factor, then the length of its text
    s16 count;
    int textLength;
    int soundCount;
    char *token;
    s16 j;
    int c;
    s16 i;

    func_02087d98(sounds, engine);
    count = 0;
    words->pause = 0;
    words->next = words + 1;
    textLength = 0;
    soundCount = 0;
    words = words->next;
    while (1) {
        if (func_02086a7c(line, 0xC0, &engine->unk_0E628) < 0) {
            break;
        }
        i = strlen(line);
        if (line[i - 1] < ' ') {
            line[i - 1] = 0;
        }
        accent = 10;
        words->unk_0E = words->unk_14 = n = 0;
        phrase = accent;
        word[0] = 0;
        cursor = line;
        pause = 200;
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            strcpy(word, token);
        }
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            n = atoi(token);
        }
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            accent = atoi(token);
        }
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            phrase = atoi(token);
        }
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            pause = atoi(token);
        }
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            words->unk_0E = atoi(token);
        }
        token = func_02089da0(&cursor, " ");
        if (token != NULL) {
            words->unk_14 = atoi(token);
        }

        // Drops control characters, and a 'Q' that starts the word or follows another
        j = 0;
        for (i = 0; i < 0xC0 && word[i] != 0; i++) {
            c = word[i];
            if (c == 'Q' && (i == 0 || word[i - 1] == 'Q')) {
                continue;
            }
            if (c >= ' ') {
                word[j++] = c;
            }
        }
        word[j] = 0;
        if (j == 0) {
            continue;
        }
        if (strlen(word) > 0x60) {
            word[0x60] = 0;
        }
        words->unk_05 = n;
        words->unk_06 = n;
        words->unk_08 = (accent * engine->cur.accent * 0x28F + 0x8000) >> 16;
        words->unk_09 = (phrase * engine->cur.phrase * 0x28F + 0x8000) >> 16;
        n = (engine->cur.speed * 0x28F + 0x80) >> 8;
        words->pause = func_02087e44((n * pause + 0x80) >> 8, 0, 30000, "pause*speed");
        words->unk_0C = engine->cur.unk_00;
        words->speed = engine->cur.speed;
        words->volume = engine->cur.volume;
        words->unk_07 = 0;
        if (words->unk_06 == -1) {
            words->unk_06 = func_02084794(word, (ShttsWordList *)engine);
            if (words->unk_06 == -1) {
                words->unk_06 = func_02084814(word);
            }
        }
        end = func_02087bd4(word, line, engine);
        if ((int)(textLength + (strlen(line) + 1)) > 0x1C0) {
            line[0x1BF - textLength] = 0;
        }
        strcpy(text, line);
        words->text = text;
        n = strlen(text);
        if (n > 0) {
            func_020870c8(words, sounds, 0x80 - soundCount, engine);
            if (words->count == 0) {
                continue;
            }
            sounds += words->count;
            soundCount += words->count;
            if (words->unk_07 & 0x70) {
                words->unk_08 = words->unk_08 * ((((words->unk_07 & 0x70) >> 4) + 2) >> 1);
            }
            words->next = words + 1;
            words = words->next;
            count++;
        }
        text += n + 1;
        textLength += n + 1;
        if (end == -1) {
            break;
        }
        if (words[-1].pause > 0) {
            break;
        }
        if (soundCount >= 0x80 || count >= 0x40 || textLength >= 0x1BF) {
            break;
        }
    }
    if (count > 0 && words[-1].pause == 0) {
        words[-1].pause = 100;
    }
    words[-1].next = NULL;
    return count;
}

// Groups a list of words into runs, and the runs into runs of those. A word starts a run if its unk_08 or unk_09
// is set, if the word before it has a pause, or if it is the first; it starts a run of runs for the same reasons
// but unk_08.
void func_02086fd8(ShttsUnk_020870c8 *word, ShttsUnk_02086fd8_A *a, ShttsUnk_02086fd8_B *b) {
    ShttsUnk_020870c8 *first = word;

    a->count = 0;
    for (; word != NULL; word = word->next) {
        if ((word->unk_08 != 0 || word->unk_09 != 0) || a->count == 0 || word[-1].pause != 0 || word == first) {
            a->next = a + 1;
            a++;
            a->first = word;
            a->last = word;
            a->count = 1;
            a->unk_2 = word->unk_08;
            a->unk_1 = word->unk_06;
            if (word->unk_09 != 0 || word[-1].pause != 0 || word == first) {
                b->next = b + 1;
                b++;
                b->first = a;
                b->count = 1;
                b->unk_1 = word->unk_09;
            } else {
                b->count++;
            }
        } else {
            a->count++;
            a->last = word;
        }
    }
    a->next = NULL;
    b->next = NULL;
}

// Splits a word's text into its sounds, each a syllable that ends in a vowel, "N" or "-", and writes the text
// again from them. A vowel in upper case marks its sound with 'U', a 'Q' marks the sound that follows, and '?' and
// '!' are counted in the word.
// Returns how many sounds the word has.
int func_020870c8(ShttsUnk_020870c8 *word, ShttsUnk_02087d98 *sound, int max, Shtts *engine) {
    char *p;
    s16 length;
    s16 i;
    s16 j;
    int c;

    word->count = 0;
    word->first = sound;
    p = sound->name;
    length = strlen(word->text);
    for (i = 0; i < length; i++) {
        if (word->count >= max) {
            word->text[i] = 0;
            break;
        }
        c = word->text[i];
        switch (c) {
        case '?':
            if ((word->unk_07 & 7) < 3) {
                word->unk_07++;
            }
            break;
        case '!':
            if (((word->unk_07 & 0x70) >> 4) < 3) {
                word->unk_07 += 0x10;
            }
            break;
        case 'Q':
            sound->unk_04 = 'Q';
            break;
        case 'A':
        case 'I':
        case 'U':
        case 'E':
        case 'O':
            if (c <= 'Z' && c >= 'A') {
                c += 0x20;
            }
            sound->unk_05 = 'U';
            c = (char)c;
            // Fall through
        case 'a':
        case 'i':
        case 'u':
        case 'e':
        case 'o':
        case 'N':
        case '-':
            if (c == '-') {
                if (sound == engine->unk_00010) {
                    break;
                }
                if (sound->name[0] != 0) {
                    break;
                }
                sound[-1].unk_05 = 'L';
            }
            p[0] = c;
            p[1] = 0;
            if (strcmp(sound->name, "-") != 0) {
                for (j = 0; j < 0x90; j++) {
                    if (strcmp(sound->name, data_020bbf34[j]) == 0) {
                        break;
                    }
                }
                if (j == 0x90) {
                    func_02087d98(sound, engine);
                    p = sound->name;
                    break;
                }
            }
            sound->pitch = engine->cur.pitch;
            sound->next = sound + 1;
            sound = sound->next;
            func_02087d98(sound, engine);
            p = sound->name;
            word->count++;
            break;
        case '~':
            break;
        default:
            if (p - sound->name < 2) {
                *p++ = c;
            }
            break;
        }
    }
    func_02087d98(sound, engine);
    word->text[0] = 0;
    sound = word->first;
    for (i = 0; i < word->count; i++) {
        strcat(word->text, sound->name);
        sound = sound->next;
    }
    return word->count;
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

// Carries out a word that is a command, and otherwise copies it to out with a '-' for the second of two vowels
// that are the same. Returns -3 for a command, -1 for the end of a sentence, -2 for a comma, 0 for anything else.
int func_02087bd4(char *text, char *out, Shtts *engine) {
    s16 i;
    s16 pos;
    int c;

    *out = 0;
    if (func_020873a4(text, 1, engine) != 0) {
        return -3;
    }
    for (i = 0; i < 5; i++) {
        pos = 0;
        do {
            int found = func_02087dc8(text + pos, data_020bac34[i]);

            if (found < 0) {
                break;
            }
            pos += found + 1;
            text[pos] = '-';
            pos++;
        } while (pos < 0x60);
    }
    if (strcmp(text, "S") == 0) {
        return -1;
    }
    if (strcmp(text, "。") == 0) {
        return -1;
    }
    if (strcmp(text, ".") == 0) {
        return -1;
    }
    if (strcmp(text, "!") == 0) {
        return -1;
    }
    if (strcmp(text, "?") == 0) {
        return -1;
    }
    if (strcmp(text, "、") == 0) {
        return -2;
    }
    if (strcmp(text, "，") == 0) {
        return -2;
    }
    if (strcmp(text, ",") == 0) {
        return -2;
    }
    strcpy(out, text);
    c = text[strlen(text) - 1];
    if (c == '.') {
        return -1;
    }
    if (c == '!') {
        return -1;
    }
    if (c == '?') {
        return -1;
    }
    if (c == ',') {
        return -2;
    }
    return 0;
}

void func_02087d98(ShttsUnk_02087d98 *unk, Shtts *engine) {
    unk->name[0] = '\0';
    unk->unk_04 = unk->unk_05 = '-';
    unk->unk_08 = 0;
    unk->unk_06 = 0;
    unk->next = NULL;
    unk->pitch = engine->cur.pitch;
}

// Where one string first is in another, or -1
int func_02087dc8(const char *text, const char *find) {
    const char *t;
    const char *f = find;
    s16 i = 0;

    while (*text != 0) {
        if (*text == *f) {
            t = text;
            while (*f != 0) {
                if (*t != *f) {
                    break;
                }
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

// A value kept within a range
s16 func_02087e44(int value, s16 min, s16 max, const char *name) {
    s16 result = min;

    if (value >= min) {
        if (value > max) {
            result = max;
        } else {
            result = value;
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

// Turns text into lines for the synthesizer and appends them to dest. Kana become phonemes, which gather into a
// phrase until punctuation ends it; a phrase becomes one line of its phonemes and four numbers (see func_02089118).
// Commands between "#Q" and "#q", and other text that starts with '#', are passed on as lines of their own.
// Returns 0, or where in text it had got to when dest ran out of room.
const char *func_02087e94(const char *text, char *dest, u32 size) {
    char phonemes[0x60];
    char command[0xC1];
    s16 level;
    s16 accent;
    s16 pause;
    s16 pauseSet;
    s16 gap;
    const char *done;
    s16 result;
    s16 overflow;
    s16 total;
    s16 length;
    char *p;
    s16 n;

    pauseSet = 0;
    *dest = '\0';
    gap = 10;
    phonemes[0] = '\0';
    level = 10;
    accent = 0;
    pause = 0;
    overflow = 0;
    length = 0;
    total = 0;
    result = 0;
    done = text;
    while (*text != '\0') {
        // The cast is what makes the compiler read the character again after the loop's test, as the original does
        char c = *(s8 *)text;

        if (c == '#') {
            if (text[1] == 'Q') {
                text += 2;
                p = command;
                result = 0;
                n = 0;
                while (*text != '\0') {
                    if (*text == '#' && text[1] == 'q') {
                        text += 2;
                        break;
                    }
                    if (*text == '/' || *text == '\r' || *text == '\n') {
                        *p = '\0';
                        text++;
                        if (command[0] != '\0') {
                            result = func_02089094(dest, size, command);
                            if (result < 0) {
                                break;
                            }
                            done = text;
                        }
                        p = command;
                        n = 0;
                    } else if (n >= 0xBE) {
                        text++;
                    } else {
                        *p++ = *text++;
                        n++;
                    }
                }
                if (result < 0) {
                    break;
                }
                *p = '\0';
                if (command[0] != '\0') {
                    result = func_02089094(dest, size, command);
                }
                if (result < 0) {
                    break;
                }
                done = text;
            } else if (text[1] == 'p') {
                char number[7] = { 0, 0, 0, 0, 0, 0, 0 };
                s16 i;

                i = 0;
                while (text[i + 2] != '\0' && text[i + 2] != '\\') {
                    i++;
                    if (i > 5) {
                        i = 0;
                        break;
                    }
                }
                strncat(number, text + 2, i);
                pauseSet = func_02087e44(atoi(number), 0, 30000, "pause");
                text = text + 3 + i;
                done = text;
            } else if (text[1] == 'q') {
                text += 2;
                done = text;
            } else {
                p = command;
                n = 0;
                while (c != '\0') {
                    if (strncmp(text, "／", 2) == 0 || strncmp(text, "｜", 2) == 0 || strncmp(text, "！", 2) == 0 ||
                        strncmp(text, "？", 2) == 0 || strncmp(text, "．", 2) == 0 || strncmp(text, "＿", 2) == 0) {
                        text += 2;
                        break;
                    }
                    if (c == '/' || c == '\r' || c == '\n' || c == '/' || c == '|' || c == '!' || c == '?' ||
                        c == '.' || c == '_') {
                        text++;
                        break;
                    }
                    if (n >= 0xBE) {
                        text++;
                    } else {
                        *p++ = *text++;
                        n++;
                    }
                    c = *text;
                }
                *p = '\0';
                if (command[0] != '\0') {
                    result = func_02089094(dest, size, command);
                    if (result < 0) {
                        break;
                    }
                }
                done = text;
            }
        } else if (c == '.' || strncmp(text, "．", 2) == 0) {
            if (phonemes[0] != '\0') {
                strcat(phonemes, ".");
                if (pauseSet == 0) {
                    pauseSet = 1000;
                }
                if (accent == 0 && level == 10) {
                    func_020893e4(text, &level);
                }
                result = func_02089118(dest, size, phonemes, accent, level, gap,
                                       func_02087e44(pauseSet + pause, 0, 30000, "pause sum"));
                pauseSet = 0;
                if (result < 0) {
                    break;
                }
                gap = 10;
                overflow = 0;
                length = 0;
                total = 0;
                phonemes[0] = '\0';
                accent = 0;
                pause = 0;
                level = 10;
            }
            if (*text == '.') {
                text++;
            } else {
                text += 2;
            }
            done = text;
        } else if (c == '?' || strncmp(text, "？", 2) == 0 || c == '!' || strncmp(text, "！", 2) == 0) {
            if (phonemes[0] != '\0') {
                int i;

                if (accent == 0 && level == 10) {
                    func_020893e4(text, &level);
                }
                for (i = 0; i < 6; i++) {
                    if (strlen(phonemes) + 1 >= 0x60) {
                        break;
                    }
                    if (*text == '?') {
                        strcat(phonemes, "?");
                        text++;
                    } else if (*text == '!') {
                        strcat(phonemes, "!");
                        text++;
                    } else if (strncmp(text, "？", 2) == 0) {
                        strcat(phonemes, "?");
                        text += 2;
                    } else if (strncmp(text, "！", 2) == 0) {
                        strcat(phonemes, "!");
                        text += 2;
                    } else {
                        break;
                    }
                }
                if (pauseSet == 0) {
                    pauseSet = 1000;
                }
                result = func_02089118(dest, size, phonemes, accent, level, gap,
                                       func_02087e44(pauseSet + pause, 0, 30000, "pause sum"));
                pauseSet = 0;
                if (result < 0) {
                    break;
                }
                gap = 10;
                overflow = 0;
                length = 0;
                total = 0;
                phonemes[0] = '\0';
                accent = 0;
                pause = 0;
                level = 10;
            } else if (c == '?' || c == '!') {
                text++;
            } else {
                text += 2;
            }
            done = text;
        } else if (c == '\n' || c == '\r') {
            if (phonemes[0] != '\0') {
                if (pause != 0) {
                    if (accent == 0 && level == 10) {
                        func_020893e4(text, &level);
                    }
                    if (pauseSet != 0) {
                        result = func_02089118(dest, size, phonemes, accent, level, gap, pauseSet);
                        pauseSet = 0;
                    } else {
                        result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
                    }
                    total = 0;
                } else {
                    if (accent == 0 && level == 10) {
                        func_020893e4(text, &level);
                    }
                    result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
                }
                length = 0;
                if (result < 0) {
                    break;
                }
                accent = 0;
                phonemes[0] = '\0';
                if (pause != 0) {
                    gap = 10;
                } else {
                    gap = 0;
                }
                pause = 0;
                overflow = 0;
                level = 10;
            }
            text++;
            done = text;
        } else if (c == '|' || strncmp(text, "｜", 2) == 0) {
            if (phonemes[0] != '\0') {
                if (accent == 0 && level == 10) {
                    func_020893e4(text, &level);
                }
                if (pause != 0 && pauseSet != 0) {
                    result = func_02089118(dest, size, phonemes, accent, level, gap, pauseSet);
                    pauseSet = 0;
                    total = 0;
                } else {
                    result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
                    if (pause != 0) {
                        total = 0;
                    }
                }
                length = 0;
                if (result < 0) {
                    break;
                }
                accent = 0;
                pause = 0;
                overflow = 0;
                phonemes[0] = '\0';
                level = 10;
                gap = 5;
            }
            if (*text == '|') {
                text++;
            } else {
                text += 2;
            }
            done = text;
        } else if (c == '/' || strncmp(text, "／", 2) == 0) {
            if (phonemes[0] != '\0') {
                if (pause != 0) {
                    if (pauseSet != 0) {
                        if (accent == 0 && level == 10) {
                            func_020893e4(text, &level);
                        }
                        result = func_02089118(dest, size, phonemes, accent, level, gap, pauseSet);
                        pauseSet = 0;
                    } else {
                        if (accent == 0 && level == 10) {
                            func_020893e4(text, &level);
                        }
                        result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
                    }
                    total = 0;
                } else {
                    if (accent == 0 && level == 10) {
                        func_020893e4(text, &level);
                    }
                    result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
                }
                if (result < 0) {
                    break;
                }
                accent = 0;
                phonemes[0] = '\0';
                if (pause != 0) {
                    gap = 10;
                } else {
                    gap = 0;
                }
                pause = 0;
                overflow = 0;
                length = 0;
                level = 10;
            }
            if (*text == '/') {
                text++;
            } else {
                text += 2;
            }
            done = text;
        } else if (c == '_' || strncmp(text, "＿", 2) == 0) {
            if (phonemes[0] != '\0') {
                if (pause == 0 && accent == 0 && level == 10) {
                    func_020893e4(text, &level);
                }
                if (*text == '_') {
                    text++;
                } else {
                    text += 2;
                }
                c = *text;
                if (c == '0') {
                    pause += 100;
                    text++;
                } else if (strncmp(text, "０", 2) == 0) {
                    pause += 100;
                    text += 2;
                } else if (c == '1') {
                    pause += 200;
                    text++;
                } else {
                    pause += 200;
                }
            } else if (c == '_') {
                text++;
            } else {
                text += 2;
            }
            if (pause > 30000) {
                pause = 30000;
            }
        } else if (c == '\'' || strncmp(text, "’", 2) == 0) {
            char digit[3] = { 0, 0, 0 };
            char *p;
            s16 value;
            int n;

            accent = 0;
            for (p = phonemes; *p != '\0'; p++) {
                if (strchr("AIUEOaiueoN-Q", *p) != NULL) {
                    accent++;
                }
            }
            if (*text == '\'') {
                text++;
            } else {
                text += 2;
            }
            strncpy(digit, text, 1);
            p = digit;
            value = func_020892a8(p, text);
            n = 1;
            if (value == -1) {
                strncpy(p, text, 2);
                value = func_02089304(p, text);
                n = 2;
            }
            if (value == -1) {
                n = 0;
            }
            if (value == 0) {
                level = 5;
            } else if (value == 1) {
                level = 10;
            } else if (value == 2) {
                level = 15;
            } else {
                level = 10;
            }
            text += n;
        } else if (c == '%' || strncmp(text, "％", 2) == 0) {
            s16 n = strlen(phonemes);

            if (n >= 2) {
                char *p = phonemes + n;

                if (strchr("aiueo", p[-1]) != NULL && strchr("AIUEOaiueoN-Q", p[-2]) == NULL) {
                    p[-1] -= 0x20;
                }
            }
            if (*text == '%') {
                text++;
            } else {
                text += 2;
            }
        } else {
            s16 index = func_02089244(text, data_020bac4c[0], data_020bac48);

            if (index >= 0) {
                if (pause != 0) {
                    if (phonemes[0] != '\0') {
                        if (pauseSet != 0) {
                            if (accent == 0 && level == 10) {
                                func_020893e4(text, &level);
                            }
                            result = func_02089118(dest, size, phonemes, accent, level, gap, pauseSet);
                            pauseSet = 0;
                        } else {
                            if (accent == 0 && level == 10) {
                                func_020893e4(text, &level);
                            }
                            result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
                        }
                        length = 0;
                        total = 0;
                        if (result < 0) {
                            break;
                        }
                        gap = 10;
                        accent = 0;
                        pause = 0;
                        overflow = 0;
                        phonemes[0] = '\0';
                        level = 10;
                    }
                    done = text;
                }
                if (!overflow) {
                    s16 n = strlen(phonemes);
                    s16 add = strlen(&data_020bac4c[index][8]);

                    if (!(length + add < 0x40 && total + add < 0x100)) {
                        result = func_02089118(dest, size, phonemes, accent, level, gap, 100);
                        length = 0;
                        phonemes[0] = '\0';
                        done = text;
                        total = 0;
                        n = 0;
                        if (result < 0) {
                            break;
                        }
                        accent = 0;
                    }
                    if (n + add >= 0x60) {
                        overflow = 1;
                    } else {
                        char first = data_020bac4c[index][8];

                        if (n != 0 && phonemes[n - 1] == 'Q' && first == 'Q') {
                        } else if (n == 0 && first == '-') {
                        } else if (n == 0 && first == 'Q') {
                        } else {
                            strcat(phonemes, &data_020bac4c[index][8]);
                        }
                    }
                }
                text += strlen(data_020bac4c[index]);
                total += strlen(data_020bac4c[index]);
                length += strlen(data_020bac4c[index]);
            } else {
                text++;
                c = text[-1];
                if ((c >= (char)0x81 && c <= (char)0x9F) || (c >= (char)0xE0 && c <= (char)0xFC)) {
                    c = *text;
                    if ((c >= 0x40 && c <= 0x7E) || c <= (char)0xFC) {
                        text++;
                    }
                }
                done = text;
            }
        }
    }
    if (result < 0) {
        return done;
    }
    if (phonemes[0] != '\0') {
        if (pause == 0) {
            pause = 1000;
        }
        if (pauseSet != 0) {
            if (accent == 0 && level == 10) {
                func_020893e4(text, &level);
            }
            result = func_02089118(dest, size, phonemes, accent, level, gap, pauseSet);
        } else {
            if (accent == 0 && level == 10) {
                func_020893e4(text, &level);
            }
            result = func_02089118(dest, size, phonemes, accent, level, gap, pause);
        }
        if (result < 0) {
            return done;
        }
    }
    return 0;
}

// Writes a number in decimal
char *func_02088fc8(char *out, s16 value) {
    char *p = out;
    s16 div;
    s16 digit;

    if (value < 0) {
        *out = '-';
        value = -value;
        p = out + 1;
    }
    div = 1;
    while (div * 10 <= value && div < 0xCCC) {
        div *= 10;
    }
    for (; div >= 1; div /= 10) {
        digit = value / div;
        *p++ = digit + '0';
        value -= digit * div;
    }
    *p = 0;
    return out;
}

// Adds a line to text if there is room for it, and returns the line's length
s16 func_02089094(char *text, u32 size, const char *line) {
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

// Appends a line to dest: text and four numbers with spaces between. Returns the length of the line, 0 if text is
// empty, or -1 if dest has no room for it.
s16 func_02089118(char *dest, u32 size, const char *text, s16 a, s16 b, s16 c, s16 d) {
    char line[0xC0];
    char number[16];
    int length;

    if (text[0] == '\0') {
        return 0;
    }
    strcpy(line, text);
    strcat(line, " ");
    strcat(line, func_02088fc8(number, a));
    strcat(line, " ");
    strcat(line, func_02088fc8(number, b));
    strcat(line, " ");
    strcat(line, func_02088fc8(number, c));
    strcat(line, " ");
    strcat(line, func_02088fc8(number, d));
    strcat(line, "\n");
    length = strlen(line);
    if (length + strlen(dest) >= size) {
        return -1;
    }
    strcat(dest, line);
    return (s16)length;
}

// Which of a table's entries, of 12 bytes each, a text starts with, or -1
s16 func_02089244(const char *text, const char *table, int count) {
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

// Divides a by b, rounding to nearest, with the quotient in 1.15 fixed point and limited to +-0x7FFF
s16 func_02089444(int a, int b) {
    int i;
    int result;
    int sign = 1;

    if (b == -0x8000) {
        b = -0x7FFF;
    }
    if (a < 0) {
        a = -a;
        sign = -1;
    }
    if (b < 0) {
        b = (s16)-b;
        sign = -sign;
    }
    if (sign > 0) {
        a += b / 2;
    } else {
        a += (b - 1) / 2;
    }
    for (i = 15; i >= 0; i--) {
        if (a - (b << 15) >= 0) {
            a = ((a - (b << 15)) << 1) + 1;
        } else {
            a <<= 1;
        }
    }
    if (a & 0x8000) {
        result = sign * 0x7FFF;
    } else {
        result = (u16)a * sign;
    }
    return result;
}

// Decodes a frame into samples, which go to out after their count. Returns the count.
int func_020894cc(ShttsFrame *frame, s16 *out, Shtts *engine) {
    s16 buffer[0x100];
    u16 freq[10];
    s16 coef[11];
    s32 state[10];
    int count;
    int cut;
    int i;
    s16 n;
    int nibble;

    if (frame->unk_3 == 0) {
        *out = 0;
        return 0;
    }
    for (n = 0; n < 10; n++) {
        if (n % 2 == 1) {
            nibble = frame->unk_8[n / 2] & 0xF;
        } else {
            nibble = (frame->unk_8[n / 2] >> 4) & 0xF;
        }
        if (n == 0) {
            freq[n] = data_020bb720[n][nibble];
        } else {
            freq[n] = freq[n - 1] + data_020bb720[n][nibble];
        }
    }
    i = 0;
    cut = 0;
    if (engine->cur.unk_28 != 0x400) {
        if ((int)engine->cur.warp > 100) {
            cut = 10 - engine->cur.unk_2A;
            for (; i < 10; i++) {
                int b = (engine->cur.unk_28 * freq[i]) >> 10;
                int a = (freq[i] >> 1) + 0x8000;

                if (a < b) {
                    freq[i] = a;
                } else {
                    freq[i] = b;
                }
            }
        } else if ((int)engine->cur.warp < 100) {
            s16 base = freq[0] >> 2;

            for (; i < 10; i++) {
                int v = ((engine->cur.unk_28 * (freq[i] - base)) >> 10) + base;
                int low = freq[i] * 2 - 0x10000;

                if (v < low) {
                    v = low;
                }
                if (freq[i] < 0x7000) {
                    freq[i] = v;
                }
            }
        }
    }
    func_020897c8(10 - cut, freq, &coef[1]);
    memset(&coef[11] - cut, 0, cut * 2);
    if (frame->unk_2 != 0) {
        memset(state, 0, sizeof(state));
        if (data_020bc22c[frame->unk_4] & 0x84) {
            func_0208e83c(0x100, 10, coef, state, data_020bba60, buffer, engine);
        } else {
            func_0208e83c(0x100, 10, coef, state, data_020bb860, buffer, engine);
        }
        count = 0xF0;
        func_02089a3c(0x61C, buffer, out + 1, count);
    } else {
        count = engine->unk_00004;
        memset(state, 0, sizeof(state));
        func_0208e83c(count, 10, coef, state, &data_020bbc60[engine->unk_00004 * engine->unk_08722], buffer, engine);
        engine->unk_08722++;
        if (engine->unk_08722 >= 6) {
            engine->unk_08722 = 0;
        }
        func_02089a3c(0x800, buffer, out + 1, count);
    }
    *out = count;
    frame->unk_3 = count;
    return count;
}

// Turns `order` values into as many filter coefficients. Each value is first looked up, with interpolation, in the
// cosine table that data_020bbf30 points to, and negated. Then an impulse (0x2000000, then zeros) is put through two
// chains of order / 2 second-order sections, whose coefficients are the even and the odd cosines, and half the sum
// of the two chains' outputs, from the second on, is the result.
//
// The first loop only matches with the multiplication as an inline function returning s16 (func_020897c8_mult) and
// with `frac` reused for the product and then for the sum.
void func_020897c8(int order, u16 *in, s16 *out) {
    s16 cosines[10];
    s16 result[11];
    int p[6];
    int d[6];
    int c[6];
    int q[6];
    int b[6];
    int a[6];
    s16 half = order >> 1;
    int prev;
    s16 i;
    s16 j;
    s16 n;
    s16 frac;
    int x;
    int t;
    int r;
    int old;

    for (i = 0; i < order; i++) {
        t = (in[i] << 7) >> 1;
        j = t >> 15;
        frac = t & 0x7FFF;
        frac = func_020897c8_mult(data_020bbf30[j + 1] - data_020bbf30[j], frac);
        frac += data_020bbf30[j];
        cosines[i] = -frac;
    }
    for (j = 0; j < half + 1; j++) {
        d[j] = c[j] = b[j] = a[j] = 0;
    }
    prev = 0;
    x = 0x2000000;
    for (i = 0; i <= order; i++) {
        p[0] = x + prev;
        q[0] = x - prev;
        prev = x;
        for (j = 0; j < half; j++) {
            t = d[j];
            r = func_02089b5c(cosines[j * 2] << 16, t);
            old = c[j];
            c[j] = t;
            p[j + 1] = old + (p[j] + (r << 2));
            t = b[j];
            d[j] = p[j];
            r = func_02089b5c(cosines[j * 2 + 1] << 16, t);
            old = a[j];
            a[j] = t;
            q[j + 1] = old + (q[j] + (r << 2));
            b[j] = q[j];
        }
        result[i] = (p[half] + q[half]) >> 15;
        x = 0;
    }
    for (n = 0; n < order; n++) {
        out[n] = result[n + 1];
    }
}

// Scales `count` samples so that their root mean square is `level`, and keeps them in 16 bits. The roots of the
// usual counts (55, 60, 120 and 240) are constants.
void func_02089a3c(int level, s16 *in, s16 *out, int count) {
    s16 root;
    u32 sum;
    s16 i;
    int gain;
    int v;

    if (count == 55) {
        root = 59;
    } else if (count == 60) {
        root = 62;
    } else if (count == 120) {
        root = 88;
    } else if (count == 240) {
        root = 124;
    } else {
        root = func_02089bb0(count) >> 5;
    }
    sum = 0;
    for (i = 0; i < count; i++) {
        sum += (in[i] * in[i]) >> 6;
    }
    if (sum == 0) {
        gain = 1;
    } else {
        if (sum > 0x3FFF0001) {
            sum = 0x3FFF0001;
        }
        sum = (s16)(func_02089bb0(sum) >> 8);
        gain = func_02089444((s16)level * root, (s16)sum);
    }
    for (i = 0; i < count; i++) {
        v = (gain * in[i]) >> 6;
        if (v < -0x8000) {
            v = -0x8000;
        } else if (v > 0x7FFF) {
            v = 0x7FFF;
        }
        out[i] = v;
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

// The square root of a number, times 256, approximated with a quadratic after scaling to a fixed range
int func_02089bb0(int x) {
    s16 shift = 15;
    int bit;
    int y;

    for (bit = 0x40000000; bit > 1; bit >>= 1) {
        if (x & bit) {
            break;
        }
        shift--;
    }
    if (shift & 1) {
        shift++;
    }
    if (shift > 0) {
        x >>= shift;
    } else {
        x <<= -shift;
    }
    y = (x * 0x4767) >> 16;
    y = -y;
    y += 0x28A;
    y = x + ((y * x) >> 16) + 0x44E0;
    if (shift > 0) {
        return y << (shift / 2);
    }
    return y >> (-shift / 2);
}

// Sets up the engine's state for a new text
int func_02089c50(Shtts *engine) {
    engine->unk_04938[0] = 0;
    engine->unk_0EE0C = 0;
    engine->unk_0EE0E = 0;
    func_0208be38(engine);
    engine->unk_0B700 = 0;
    func_02089e48(engine->unk_0B704, 0, sizeof(engine->unk_0B704));
    engine->unk_08722 = 0;
    engine->unk_0EE04 = 0;
    engine->unk_0EE08 = 0;
    engine->unk_08906 = 0;
    engine->unk_0B6FC = 0;
    memset(engine->unk_0B730, 0, sizeof(engine->unk_0B730));
    return 0;
}

int func_02089cec(Shtts *engine) {
    return 0;
}

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

void func_02089e48(char *dst, int c, int count) {
    int i;

    for (i = 0; i < count; i++) {
        *dst++ = (char)c;
    }
}

// Called with -1 when something is wrong. Does nothing in this build.
void func_02089e70(int code) {
}

// Scales samples by a gain in 4.12 fixed point, keeping them within 16 bits
void func_02089e74(s16 *dst, s16 *src, int count, int gain) {
    s16 i = 0;

    if (count > 0) {
        while (i < count) {
            int sample = (*src * gain) >> 12;

            if (sample > 0x7FFF) {
                sample = 0x7FFF;
            } else if (sample < -0x7FFF) {
                sample = -0x7FFF;
            }
            *dst++ = sample;
            i++;
            src++;
        }
    }
}

// The "Fir" effect: adds to each sample the one cur.firDelay samples before it, times cur.firGain / 100. The
// history is a ring of 1001 samples.
void func_02089ed4(int count, s16 *in, s16 *out, Shtts *engine) {
    int gain = (int)(engine->cur.firGain << 8) / 100;
    int i;
    int v;

    for (i = 0; i < count; i++) {
        if (engine->unk_0EE00 < engine->cur.firDelay) {
            out[i] = in[i];
        } else {
            v = in[i] + ((gain * engine->firBuffer[(engine->unk_0EE00 - engine->cur.firDelay) % 1001]) >> 8);
            if (v > 0x7FFF) {
                v = 0x7FFF;
            } else if (v < -0x8000) {
                v = -0x8000;
            }
            out[i] = v;
        }
        engine->firBuffer[engine->unk_0EE00 % 1001] = in[i];
        engine->unk_0EE00++;
    }
}

// Empties the FIR filter's history
void func_0208a010(Shtts *engine) {
    engine->unk_0EE00 = 0;
    memset(engine->firBuffer, 0, sizeof(engine->firBuffer));
}

// Shapes the pitch of everything in a list: first func_0208a5b4 for each ShttsUnk_0208a5b4, then, for each stretch
// that ends at a ShttsUnk_0208a25c with unk_12 set, or at the list's end, func_0208a25c with the sum of unk_06 and
// unk_08 of its ShttsUnk_02087d98s.
void func_0208a038(ShttsUnk_0208a038 *list, Shtts *engine) {
    ShttsUnk_0208a038 *a;
    ShttsUnk_0208a038 *cur;
    ShttsUnk_0208a5b4 *b;
    ShttsUnk_0208a038 *e;
    ShttsUnk_0208a25c *c;
    ShttsUnk_02087d98 *d;
    s16 i;
    s16 j;
    s16 k;
    s16 notFirst;
    int length;

    for (e = list; e != NULL; e = e->next) {
        b = e->first;
        for (i = 0; i < e->count; i++) {
            func_0208a5b4(b, engine);
            b = b->next;
        }
    }
    cur = list;
    notFirst = 0;
    for (;;) {
        if (cur == NULL) {
            return;
        }
        length = 0;
        for (a = cur; a != NULL; a = a->next) {
            b = a->first;
            for (i = 0; i < a->count; i++) {
                c = b->first;
                for (j = 0; j < b->count; j++, c = c->next) {
                    d = c->first;
                    for (k = 0; k < c->count - 1; k++) {
                        length += d->unk_06 + d->unk_08;
                        d = d->next;
                    }
                    length += d->unk_06 + d->unk_08;
                    if (c->unk_12 != 0 || (a->next == NULL && b->next == NULL && b->last == c)) {
                        if (cur->next == NULL) {
                            if (c->unk_07 != 0) {
                                func_0208a25c(cur, length, d, 2, notFirst, engine);
                            } else {
                                func_0208a25c(cur, length, d, 1, notFirst, engine);
                            }
                        } else {
                            func_0208a25c(cur, length, d, 0, notFirst, engine);
                        }
                        if (list != a) {
                            notFirst = 1;
                        } else {
                            notFirst = 0;
                        }
                        cur = cur->next;
                        goto next;
                    }
                }
                b = b->next;
            }
        }
        return;
    next:;
    }
}

// Adds a curve (func_0208ab18) to the pitch of each ShttsUnk_02087d98 from the first of `unk` to `last`, at a rate
// that falls with `length`, and lowers the pitch of the last one when the unk_12 of the last ShttsUnk_0208a25c is
// near certain multiples of cur.speed. `kind` is 0 when more follows, 1 or 2 at the end.
int func_0208a25c(ShttsUnk_0208a038 *unk, int length, ShttsUnk_02087d98 *last, int kind, s16 notFirst,
                  Shtts *engine) {
    s16 offset;
    ShttsUnk_0208a5b4 *b;
    s16 depth;
    ShttsUnk_0208a25c *c;
    ShttsUnk_02087d98 *d;
    int pos;
    int rate;

    if (kind > 0) {
        if (length <= 400) {
            rate = 0x189D;
        } else {
            rate = (s16)(0x3E8000 / (length + 250));
        }
    } else {
        if (length <= 0x438) {
            rate = 0xA66;
        } else {
            rate = (s16)(0x35E800 / (length + 250));
        }
    }
    b = unk->first;
    if (notFirst == 1) {
        if (unk->unk_1 == 1) {
            if (engine->unk_0EE04 == 1 && engine->unk_0EE08 == 1) {
                depth = 60;
            } else {
                depth = 200;
            }
        } else {
            depth = unk->unk_1 * 105 - (b->first->first->pitch - engine->cur.pitch);
        }
    } else {
        depth = unk->unk_1 * 105;
    }
    engine->unk_0EE04 = notFirst;
    engine->unk_0EE08 = unk->unk_1;
    if (depth < 0) {
        depth = 0;
    }
    d = b->first->first;
    offset = func_02089444(0xFA000, rate) + (-83 - (d->unk_06 + ((d->unk_08 + 1) >> 1)));
    pos = 0;
    while (d != last) {
        pos += d->unk_06 + ((d->unk_08 + 1) >> 1);
        d->pitch += (s16)((func_0208ab18(rate, pos + offset) * (depth * 3) + 0x400) >> 11);
        pos += d->unk_08 >> 1;
        d = d->next;
    }
    d->pitch +=
        (s16)((depth * 3 * func_0208ab18(rate, pos + (d->unk_06 + ((d->unk_08 + 1) >> 1)) + offset) + 0x400) >> 11);
    c = b[unk->count - 1].last;
    if (kind == 1 && c->unk_12 > (1 - engine->cur.speed) * -10 && c->unk_12 < (engine->cur.speed + 1) * 10) {
        last->pitch -= unk->unk_1 / 10 * 170;
    } else if (kind == 0 && c->unk_12 > engine->cur.speed * 3 - 10 && c->unk_12 < engine->cur.speed * 3 + 10) {
        last->pitch -= unk->unk_1 / 10 * 120;
    } else if (kind == 0 && c->unk_12 > engine->cur.speed * 2 - 10 && c->unk_12 < engine->cur.speed * 2 + 10) {
        last->pitch -= unk->unk_1 / 10 * 70;
    }
    return 0;
}

// Raises and lowers the pitch of the ShttsUnk_02087d98s of one ShttsUnk_0208a5b4 around the position unk_1, by an
// amount that unk_2 scales. unk_1 from 30 up carries a direction in its tens, and cur.unk_14 chooses how unk_1 is
// reworked from the number that func_020857e8 counts.
void func_0208a5b4(ShttsUnk_0208a5b4 *unk, Shtts *engine) {
    ShttsUnk_02087d98 *first;
    int depth;
    int end;
    s16 i;
    int total;
    ShttsUnk_0208a25c *c;
    s16 n;
    ShttsUnk_02087d98 *next;
    ShttsUnk_02087d98 *d;
    int k;
    int dir;
    s8 accent;
    char lastA;
    char lastB;

    c = unk->first;
    first = c->first;
    end = 0;
    if ((first->unk_05 == 'L' || first[1].unk_04 == 'Q' || first[1].name[0] == 'N') && unk->unk_1 != 1) {
        depth = unk->unk_2 * 0x84B6;
    } else {
        depth = unk->unk_2 * 0x9375;
    }
    if (unk->last->unk_12 > engine->cur.speed * 3) {
        if (unk->last->unk_06 == 0) {
            depth = (depth * 0x666) >> 12;
        } else if (unk->last->unk_06 == 1) {
            if (unk->last->count < 3) {
                depth >>= 2;
            }
        } else if (unk->last->count - unk->last->unk_06 < 2) {
            depth >>= 1;
        }
    }
    total = 0;
    for (i = 0; i < unk->count; i++) {
        total = (s16)(total + func_020857e8(c->unk_00));
        c = c->next;
    }
    if (unk->unk_1 / 30 == 1) {
        unk->unk_1 -= 30;
        dir = -1;
    } else if (unk->unk_1 / 30 == 2) {
        unk->unk_1 -= 60;
        dir = 1;
    } else if (unk->unk_1 / 30 == 3) {
        unk->unk_1 -= 90;
        dir = 0;
    } else if (engine->cur.unk_14 == 1) {
        if (total == 2) {
            if (unk->unk_1 == 0) {
                unk->unk_1 = 1;
            } else if (unk->unk_1 == 1) {
                unk->unk_1 = 0;
            }
        } else if (total > 2 && unk->unk_1 == 1 && first->unk_05 != 'L') {
            n = 0;
            if (first->name[n] != 0) {
                while (first->name[n] != 0) {
                    n++;
                }
                lastA = first->name[(s16)(n - 1)];
            }
            next = first->next;
            n = 0;
            if (next->name[n] != 0) {
                while (next->name[n] != 0) {
                    n++;
                }
                lastB = next->name[(s16)(n - 1)];
            }
            if (lastA != lastB && next->unk_04 != 'Q') {
                unk->unk_1 = 2;
            }
        }
        if (unk->unk_1 == 2) {
            dir = -1;
        } else if (unk->unk_1 >= total) {
            dir = 1;
        } else if (total == 2) {
            dir = -1;
        } else if (total % 2 == 1) {
            dir = -1;
        } else {
            dir = 1;
        }
    } else if (engine->cur.unk_14 == 2) {
        dir = -1;
        if (total <= 3) {
            unk->unk_1 = total;
        } else {
            unk->unk_1 = total - 1 + total % 2;
        }
    } else {
        dir = 0;
    }
    if (unk->unk_1 == 1) {
        depth -= depth >> 2;
        end = 1;
        first->pitch += (s16)((depth + 0x200) >> 10);
        d = first->next;
        if (d == NULL) {
            return;
        }
    } else {
        if (dir == 1) {
            first->pitch += (s16)((depth + 0x200) >> 10);
        } else if (dir == -1) {
            first->pitch -= (s16)((depth + 0x400) >> 11);
        }
        accent = unk->unk_1;
        d = first->next;
        k = 3;
        if (accent == 0 || total < accent) {
            end = total;
        } else if (accent > 0) {
            end = accent;
        } else if (accent < 0) {
            k = 2;
            d = first;
            if (accent == -1) {
                end = total;
            } else {
                end = (s16)-accent;
            }
        }
        if (d == NULL) {
            return;
        }
        if (d->unk_04 != 'Q') {
            if (total > 1) {
                if (dir == 0) {
                    d->pitch += (s16)((depth * 3 + 0x800) >> 12);
                } else if (dir != -1 || accent == 2 || (total == 2 && accent == 0)) {
                    d->pitch += (s16)((depth + 0x200) >> 10);
                }
            }
            d = d->next;
        } else {
            k = (s16)(k - 1);
        }
        for (; k <= end; k = (s16)(k + 1)) {
            if (dir == 0) {
                d->pitch += (s16)((depth + 0x200) >> 10);
            } else if (k == end || (dir == 1 && k <= end)) {
                d->pitch += (s16)((depth + 0x200) >> 10);
            }
            if (d->unk_04 == 'Q' && k >= 2 && unk->unk_1 != 0 && total >= unk->unk_1) {
                k = (s16)(k + 1);
            }
            d = d->next;
        }
    }
    if (d == NULL) {
        return;
    }
    if (total - 1 > end) {
        if (dir == 0) {
            d->pitch += (s16)((depth + 0x400) >> 11);
        }
        return;
    }
    if (d == NULL) {
        return;
    }
    if (unk->last->unk_12 != 0) {
        return;
    }
    if (total <= 1) {
        return;
    }
    if (dir == 0) {
        d->pitch += (s16)((depth + 0x800) >> 12);
    }
}

// The value at a * b / 256 of a curve of five points: data_020bc20c holds where they are, data_020bc216 their
// values (which the result has divided by 16), and data_020bc220 the distance from each to the one before.
s16 func_0208ab18(int a, int b) {
    int x = (a * b) >> 8;
    s16 i;

    for (i = 1; i < 5; i++) {
        if (x < data_020bc20c[i]) {
            s16 lo = data_020bc216[i - 1];

            return func_02089444(((x - data_020bc20c[i - 1]) * (data_020bc216[i] - lo)) >> 4, data_020bc220[i]) +
                   (lo >> 4);
        }
    }
    return 0;
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

// Adds to unk_0 of each entry an amount that goes from `a` at the first to `b` at the last, each kept within 400
// either way, changing only past entries whose unk_2 is positive, and keeps the results from 0 to the greater of
// the two ends' new values.
void func_0208abf4(ShttsUnk_0208aba8 *table, int count, int a, int b) {
    s16 max;
    s16 i;
    int step;
    int y;
    int x;
    s16 v;

    if (a > 400 || a < -400) {
        a = (s16)(a > 0 ? 400 : -400);
    }
    if (b > 400 || b < -400) {
        b = (s16)(b > 0 ? 400 : -400);
    }
    if (table[0].unk_0 + a > (table + count)[-1].unk_0 + b) {
        max = table[0].unk_0 + a;
    } else {
        max = (table + count)[-1].unk_0 + b;
    }
    step = func_02089444((b - a) << 11, (s16)(count - 1)) << 5;
    if (step > 0x640000 || step < -0x640000) {
        step = step > 0 ? 0x640000 : -0x640000;
    }
    table = table + count - 1;
    x = b << 16;
    y = x;
    i = 0;
    if (count <= 0) {
        return;
    }
    for (; i < count; i++, table--) {
        if (table->unk_2 > 0) {
            v = table->unk_0 + (s16)(x >> 16);
        } else {
            v = table->unk_0 + (s16)(y >> 16);
        }
        if (v < 0) {
            v = 0;
        }
        if (max < v) {
            v = max;
        }
        table->unk_0 = v;
        x -= step;
        if (table->unk_2 > 0) {
            y = x;
        }
    }
}

// Synthesizes the segments. It takes them in runs that are consecutive entries of one unit, fills unk_04338 with the
// run's entries of data_020bc26c, has func_02084c1c make frames of it, func_0208b550 a contour for them in unk_04938,
// and func_0208c230 the samples. A segment of silence then moves the sample buffer on by its length.
void func_0208ad48(Shtts *engine) {
    s16 i;
    s16 last;
    s16 split;
    s16 *contour;
    s16 bound;
    int kind;
    s16 j;
    s16 k;
    s16 m;
    s16 total;
    s16 frames;
    int target;
    int sum;
    ShttsSegment *start;
    ShttsSegment *end;
    ShttsSegment *segment;
    ShttsSegment *cur;
    ShttsSegment *p;

    start = NULL; // Needed for the match: without it the zeros below are not shared in one register
    contour = &engine->unk_04938[1];
    last = engine->segmentCount - 1;
    engine->unk_0EE0E = 0;
    engine->unk_05D38[0] = data_020bc254[1];
    split = 0;
    i = 0;
    for (segment = engine->segments; i <= last; i++, segment++) {
        if (i == 0) {
            start = engine->segments;
        } else {
            if (segment->unit == segment[-1].unit && segment[-1].last + 1 == segment->first) {
                if (segment[-1].kind == 20) {
                    goto next;
                }
                if (segment - start <= 1) {
                    goto next;
                }
                if (!(data_020bc22c[segment[-1].kind] & 0x80)) {
                    goto next;
                }
            }
            end = segment - 1;
            total = end->last - start->first + 1;
            bound = start->last - start->first + 1;
            cur = start;
            kind = start->kind;
            for (j = 0; j < total; j++) {
                s8 c = data_020bc26c[start->unit + j].unk_0;
                s16 v = c & 0x7F;

                engine->unk_04338[j].unk_0 = v * 10;
                engine->unk_04338[j].unk_2 = (c >> 7) & 1;
                if (j >= bound) {
                    cur++;
                    kind = cur->kind;
                    bound = cur->last - start->first + 1;
                }
                engine->unk_04338[j].kind = kind;
                if (cur->unk_6 < 0) {
                    engine->unk_04338[j].unk_2 = 0;
                }
                if (engine->unk_04338[j].unk_2 == 0) {
                    engine->unk_04338[j].unk_3 = engine->unk_00004;
                } else {
                    engine->unk_04338[j].unk_3 = 120;
                }
                for (k = 0; k < 5; k++) {
                    engine->unk_04338[j].unk_8[k] = data_020bc26c[start->unit + j].unk_1[k];
                }
            }
            for (p = start; p <= end; p++) {
                if (p->unk_6 < 0) {
                    p->unk_6 = -p->unk_6;
                }
            }
            if (start == engine->segments) {
                s16 x = end->unk_8 - engine->unk_04338[total - 1].unk_0;

                func_0208abf4(engine->unk_04338, total, x, x);
                engine->unk_0EE0E = -1;
            } else if (i == last) {
                s16 x = start->unk_8 - engine->unk_04338[0].unk_0;

                func_0208abf4(engine->unk_04338, total, x, x);
                engine->unk_0EE0E = -2;
            } else if ((data_020bc22c[start->kind] & 0x80) && start->kind != 20 && (data_020bc22c[end->kind] & 0x80)
                       && end->kind != 20 && start != end) {
                func_0208aba8(engine->unk_04338, total, start->unk_8, end->unk_8);
                engine->unk_0EE0E = 0;
            } else if (engine->unk_0EE0E == 1) {
                s16 x = end->unk_8 - engine->unk_04338[total - 1].unk_0;

                func_0208abf4(engine->unk_04338, total, x, x);
                engine->unk_0EE0E = 2;
            } else {
                s16 x = start->unk_8 - engine->unk_04338[0].unk_0;

                func_0208abf4(engine->unk_04338, total, x, x);
                engine->unk_0EE0E = 1;
            }
            frames = func_02084c1c((void *)start, (void *)end, (void *)engine->unk_04338, (void *)&engine->unk_05D38[1], engine);
            if (split > 0 && engine->unk_0EE0E == 0) {
                func_0208b550(contour, (start->length + 1) / 2, start->unk_6, 7000, engine);
                func_0208b550(contour + (start->length + 1) / 2, frames - (start->length + 1) / 2, end->unk_6, 7000,
                              engine);
                split = 0;
            } else if (end->unk_6 != end[1].unk_6 && engine->unk_0EE0E != -2 && end->kind == end[1].kind) {
                if (engine->unk_0EE0E == -1) {
                    if (engine->unk_0EE0C == 0) {
                        if (data_020bc22c[start->kind] & 1) {
                            engine->unk_04938[0] = end->unk_6;
                        } else {
                            engine->unk_04938[0] = end->unk_6 - (frames - end->length / 2) * 15;
                        }
                    } else {
                        engine->unk_04938[0] += func_02089444(
                            engine->unk_0EE0C * (end->unk_6 - engine->unk_04938[0]),
                            (s16)(engine->unk_0EE0C + frames - end->length / 2));
                    }
                }
                // The original reads end->unk_6 once and keeps it across the call, which this form gives
                sum = end->unk_6
                      + func_02089444(end->length * (s16)(end[1].unk_6 - end->unk_6),
                                      (s16)(end->length + end[1].length));
                target = sum < -0x8000 ? -0x8000 : sum > 0x7FFF ? 0x7FFF : sum;
                func_0208b550(contour, frames - end->length / 2, end->unk_6, 7000, engine);
                func_0208b550(contour + frames - end->length / 2, end->length / 2, target, 7000, engine);
                split = 1;
            } else {
                if (engine->unk_0EE0E == -1) {
                    if (engine->unk_0EE0C == 0 || engine->cur.pitchTrans != 0) {
                        if ((data_020bc22c[start->kind] & 1) || engine->cur.phrase == 0) {
                            engine->unk_04938[0] = end->unk_6;
                        } else {
                            engine->unk_04938[0] = end->unk_6 - frames * 15;
                        }
                    } else {
                        engine->unk_04938[0] += func_02089444(
                            engine->unk_0EE0C * (end->unk_6 - engine->unk_04938[0]), (s16)(engine->unk_0EE0C + frames));
                    }
                } else if (engine->unk_0EE0E == -2) {
                    if (engine->cur.phrase == 0) {
                        end->unk_6 = engine->unk_04938[0];
                    } else {
                        end->unk_6 = engine->unk_04938[0] - (s16)frames * 10;
                    }
                } else if (engine->unk_04938[0] <= 0) {
                    engine->unk_04938[0] = start->unk_6;
                }
                func_0208b550(contour, frames, end->unk_6, 7000, engine);
            }
            func_0208c230(&engine->unk_05D38[1], contour, frames, engine);
            if (func_02089d4c(engine) == 1) {
                return;
            }
            engine->unk_04938[0] = engine->unk_04938[frames];
            engine->unk_0EE0C = engine->unk_05D38[frames]->unk_3;
            start = segment;
        }
    next:
        if (segment->kind == 0) {
            engine->phoneme = 0;
            for (m = 0; m < segment->length; m++) {
                func_0208bf28(engine->unk_00004, engine);
                if (func_02089d4c(engine) == 1) {
                    return;
                }
            }
            if (func_02089d4c(engine) == 1) {
                return;
            }
            engine->unk_0EE0C = segment->length;
        }
    }
}

// Writes count values that go from the one before out to target, neither above max. With the pitchTrans setting,
// only that many values make the move, after four tenths of the rest hold the first value; the others hold target.
void func_0208b550(s16 *out, s16 count, s16 target, s16 max, Shtts *engine) {
    s16 from = out[-1];
    s16 trans = engine->cur.pitchTrans;
    int hold;
    int tail;
    s16 i;
    int step;

    if (from > max) {
        from = max;
    }
    if (target > max) {
        target = max;
    }
    if (trans == 0) {
        if (count <= 0) {
            step = 0;
        } else {
            step = func_02089444((target - from) << 6, count);
        }
        i = 1;
        if (count >= 1) {
            for (; i <= count; i++) {
                *out++ = from + (s16)((i * step) >> 6);
            }
        }
        return;
    }
    if (trans < 0) {
        trans = 1;
    }
    if (count <= trans) {
        tail = 0;
        hold = tail;
    } else {
        int rest = count - trans;

        hold = rest * 4 / 10;
        count = trans;
        tail = rest - hold;
    }
    if (count <= 0) {
        step = 0;
    } else {
        step = func_02089444((target - from) << 6, count);
    }
    i = 0;
    if (hold > 0) {
        for (; i < hold; i++) {
            *out++ = from;
        }
    }
    i = 0;
    if (count > 0) {
        for (; i < count; i++) {
            *out++ = from + (s16)((i * step) >> 6);
        }
    }
    i = 0;
    if (tail > 0) {
        for (; i < tail; i++) {
            *out++ = target;
        }
    }
}

// The ring effect: multiplies samples by a sine wave of a frequency
void func_0208b6c0(int *phase, int count, int frequency, s16 *src, s16 *dst) {
    int i;
    int step = (frequency << 16) / 11000;

    for (i = 0; i < count; i++) {
        (*phase)++;
        dst[i] = (src[i] * data_020e8d08[((step * *phase) >> 8) & 0xFF]) >> 15;
    }
}

// Makes segments of a list of phones: takes them in groups that end at a phone of kind 6 or less, and has
// func_0208ba64 look each group up as a unit, in parts if there is no unit for the whole.
void func_0208b738(ShttsPhone *phones, void *unused, Shtts *engine) {
    ShttsPhone *start;
    s16 groups;
    s16 count;
    ShttsPhone *end;
    ShttsPhone *p;
    int mode;
    int result;
    int saved2;
    int saved;

    count = 0;
    p = phones;
    end = p;
    groups = 0;
    mode = -1;
    while (p->kind != 7) {
        start = end;
        while (p->kind > 6) {
            if (p->kind == 7) {
                return;
            }
            if (count >= 0x100) {
                return;
            }
            p++;
            count++;
        }
        if (p->kind == 7) {
            return;
        }
        if (count >= 0x100) {
            return;
        }
        if (p->kind == 0) {
            end = p - 1;
            p++;
            count++;
            mode = -2;
        } else {
            end = p;
            p++;
            count++;
        }
        if (end->unk_E == 'Q' && start != end
            && (end[-1].kind == 26 || end[-1].kind == 25 || end[-1].kind == 21 || end[-1].kind == 19
                || end[-1].kind == 18 || end[-1].kind == 15 || end[-1].kind == 20 || end[-1].kind == 23
                || end[-1].kind == 24)) {
            result = -1;
        } else if (engine->unk_01B21 == 1 && mode == 0 && end - start == 2
                   && (start[1].kind == 27 || start[1].kind == 12 || start[1].kind == 8 || start[1].kind == 36
                       || start[1].kind == 28 || start[1].kind == 11 || start[1].kind == 13 || start[1].kind == 37)) {
            saved = *(int *)&start[1]; // Read this way, the compiler does not merge it with the reads above
            start[1].kind = 0;
            func_0208ba64(1, start, start + 1, unused, groups, engine);
            start[1].kind = saved;
            result = func_0208ba64(-1, start + 1, end, unused, groups, engine);
        } else {
            result = func_0208ba64(mode, start, end, unused, groups, engine);
        }
        if (result < 0 && mode >= 0) {
            if (start->kind == end->kind && start + 1 == end) {
                int length = end->length + start->length;

                if (length > 0x7FFF) {
                    length = 0x7FFF;
                }
                end->length = length;
                func_0208ba64(mode, start + 1, end, unused, groups, engine);
            } else {
                mode = 1;
                if (func_0208ba64(mode, start, end - 1, unused, groups, engine) < 0) {
                    saved2 = end[-1].kind;
                    if (saved2 == 18) {
                        end[-1].kind = 2;
                    } else if (saved2 == 20) {
                        end[-1].kind = 3;
                    } else if (saved2 == 16) {
                        end[-1].kind = 15;
                    }
                    if (func_0208ba64(1, start, end - 1, unused, groups, engine) < 0) {
                        func_0208ba64(-2, start, start, unused, groups, engine);
                    }
                    end[-1].kind = saved2;
                }
                func_0208ba64(-1, end - 1, end, unused, groups, engine);
            }
        }
        if (mode == -2) {
            mode = -1;
            end = p;
        } else {
            mode = 0;
        }
        groups++;
    }
    if (end->kind == 7) {
        return;
    }
    func_0208ba64(-2, end, end, unused, groups, engine);
}

// Looks the phones from first to last up as a unit, with silence before them if mode is -1 and after them if it is
// -2, and adds a segment for each to the engine's. Returns -1 if there is no such unit.
int func_0208ba64(int mode, ShttsPhone *first, ShttsPhone *last, void *unused, s16 unused2, Shtts *engine) {
    int index[3];
    s16 lengths[10];
    s16 head;
    s16 i;
    s16 n;
    s16 tail;
    ShttsPhone *p;
    ShttsUnit *unit;
    ShttsSegment *out;
    s16 value;

    switch (mode) {
        case -2:
        case 1:
            head = 2;
            tail = 3;
            break;
        case -1:
        case 2:
            head = 1;
            tail = 2;
            break;
        default:
            head = 2;
            tail = 2;
            break;
    }
    index[2] = 7;
    index[1] = 7;
    index[0] = 7;
    n = 0;
    if (mode == -1) {
        index[n] = 0;
        n++;
    }
    for (p = first; p <= last; p++) {
        index[n] = p->kind;
        n++;
    }
    if (mode == -2) {
        index[n] = 0;
    }
    for (p = first, i = 0; i < 9 && p <= last; p++, i++) {
        if (p == first && head == 2) {
            lengths[i + 1] = ((p->length + 7) * 0xCCD) >> 15;
        } else if (p == last && tail == 2) {
            lengths[i + 1] = ((p->length + 3) * 0xCCD) >> 15;
        } else {
            lengths[i + 1] = ((p->length + 2) * 0x199A) >> 15;
        }
    }
    lengths[0] = i;
    unit = func_0208bdf4(index, engine);
    if (unit == NULL) {
        return -1;
    }
    p = first;
    i = 0;
    out = &engine->segments[engine->segmentCount];
    if (mode == 1) {
        last--;
    }
    for (; p <= last; p++) {
        if (engine->segmentCount >= 0x180) {
            goto done;
        }
        if (unit->unk_4[i] == unit->unk_4[i + 1]) {
            i++;
        }
        if (p->unk_A != 0 && p == first && mode != -1) {
            value = p->unk_A;
        } else {
            value = p->unk_8;
        }
        if (value == 0 && p[-1].kind != 0) {
            s16 after = p[1].unk_8;

            value = after
                    + (p[-1].length + p->length) * (p[-1].unk_8 - after)
                          / (p[1].length + (p[-1].length + p->length * 2));
        }
        out->kind = p->kind;
        out->length = lengths[i + 1];
        if (p->kind >= 1 && p->kind <= 6 && p->unk_F == 'U') {
            out->unk_6 = -value;
        } else {
            out->unk_6 = value;
        }
        out->unk_8 = p->unk_6;
        out->first = unit->unk_4[i];
        out->last = unit->unk_4[i + 1] - 1;
        i++;
        out->unit = unit->unk_0;
        out->unk_A = p->unk_C;
        out++;
        engine->segmentCount++;
    }
    if (p->kind == 0 && mode == -2 && engine->segmentCount < 0x180) {
        out->kind = p->kind;
        out->length = ((p->length + 2) * 0x199A) >> 15;
        engine->segmentCount++;
    }
done:
    return 0;
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

// Moves the place in the sample buffer on by count samples. When that leaves a half of the buffer, plays the half
// out and fills it with silence.
void func_0208bf28(int count, Shtts *engine) {
    if (count < 0) {
        return;
    }
    engine->unk_08CC8 += count;
    engine->unk_08CE0 += count;
    if (engine->unk_08CD0 == &engine->unk_08908[0] && engine->unk_08CC8 >= &engine->unk_08908[0xF0]) {
        func_0208e4e4(engine->unk_08CDE, &engine->unk_08908[0], engine);
        func_0208be9c(engine->unk_08CDE, &engine->unk_08908[0], engine);
        engine->unk_08CD0 = &engine->unk_08908[0xF0];
    } else if (engine->unk_08CD0 == &engine->unk_08908[0xF0]) {
        if ((engine->unk_08CC8 >= &engine->unk_08908[0] && engine->unk_08CC8 < &engine->unk_08908[0xF0])
            || engine->unk_08CC8 > &engine->unk_08908[0x1E0]) {
            func_0208e4e4(engine->unk_08CDE, &engine->unk_08908[0xF0], engine);
            func_0208be9c(engine->unk_08CDE, &engine->unk_08908[0xF0], engine);
            engine->unk_08CD0 = &engine->unk_08908[0];
        }
    }
    if (engine->unk_08CC8 >= engine->unk_08CD8) {
        engine->unk_08CC8 -= 0x1E0;
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

// Adds the samples of unk_08726 to the sample buffer, going round to its start at its end and keeping the sums
// within 16 bits. They go at the buffer's place, or half their count before it if both a and b are above 0.
void func_0208c0c8(int a, int b, Shtts *engine) {
    s16 *dst;
    s16 i;
    s16 count = engine->unk_08724[0];
    s16 *src;

    if (a > 0 && b > 0) {
        s16 half = count >> 1;

        dst = engine->unk_08CC8 - half;
    } else {
        dst = engine->unk_08CC8;
    }
    src = &engine->unk_08724[1];
    if (dst < engine->unk_08CD4) {
        dst = engine->unk_08CD8 - (engine->unk_08CD4 - dst);
    }
    i = 0;
    if (dst + count >= engine->unk_08CD8) {
        while (dst < engine->unk_08CD8) {
            int sample = *dst + *src;
            if (sample > 0x7FFF) {
                sample = 0x7FFF;
            } else if (sample < -0x7FFF) {
                sample = -0x7FFF;
            }
            *dst++ = sample;
            i++;
            src++;
        }
        dst = engine->unk_08CD4;
    }
    for (; i < count; i++, src++) {
        int sample = *dst + *src;
        if (sample > 0x7FFF) {
            sample = 0x7FFF;
        } else if (sample < -0x7FFF) {
            sample = -0x7FFF;
        }
        *dst++ = sample;
    }
    if (a == 0) {
        engine->unk_08CCC = engine->unk_08CC8 + b;
    } else {
        engine->unk_08CCC = dst;
    }
    if (engine->unk_08CCC >= engine->unk_08CD8) {
        engine->unk_08CCC = engine->unk_08CD4 + (engine->unk_08CCC - engine->unk_08CD8);
    }
}

// Makes the samples of count frames, each with its value of the contour, and puts them in the sample buffer.
// Stops early if the engine has been asked to stop.
void func_0208c230(ShttsUnk_05D38 **units, s16 *pitches, s16 count, Shtts *engine) {
    s16 i;
    int length;
    int step;

    for (i = 0; i < count; i++) {
        if ((*units)->phoneme != engine->phoneme && (*units)->phoneme != 0) {
            engine->phoneme = (*units)->phoneme;
        }
        func_020858c8((void *)*units++, *pitches++, engine);
        engine->unk_08906 += engine->unk_00004;
        length = func_02085a64(engine);
        step = length;
        if (length == 0) {
            step = engine->unk_00004;
        }
        while (engine->unk_08906 > 0) {
            func_02085a70(engine);
            if (length == 0) {
                func_0208e3b4(0, engine);
                func_0208c0c8(0, step, engine);
            } else {
                func_0208e3b4(length, engine);
                func_0208c0c8(0, length, engine);
            }
            engine->unk_08906 -= step;
            func_0208bf28(step, engine);
        }
        if (func_02089d4c(engine) == 1) {
            return;
        }
    }
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
void func_0208c4dc(int warp, int live, Shtts *engine) {
    if (warp < 80) {
        warp = 80;
    }
    if (warp > 200) {
        warp = 200;
    }
    if (live) {
        engine->cur.warp = warp;
        engine->cur.unk_28 = ((int)engine->cur.warp * 0x28F) >> 6;
        engine->cur.unk_2A = func_02089444(1000, (s16)warp);
    } else {
        engine->set.warp = warp;
        engine->set.unk_28 = ((int)engine->set.warp * 0x28F) >> 6;
        engine->set.unk_2A = func_02089444(1000, (s16)warp);
    }
}

int func_0208c568(Shtts *engine) {
    engine->unk_00000 = 11000;
    engine->unk_01B21 = 0;
    engine->unk_00004 = engine->unk_00000 / 200;
    return 0;
}

// Starts speaking the text: sets the state up, prepares the text, and synthesizes in a thread of its own, or in
// the caller's if unk_0EE11 is set.
void func_0208c5a8(Shtts *engine) {
    func_0208c568(engine);
    func_02089c50(engine);
    if (engine->cur.unk_00 == 1) {
        func_0208c404(engine);
        engine->cur.unk_00 = 1;
    } else {
        func_0208c404(engine);
    }
    if ((engine->unk_08CE4 = 1) != 0) {
        func_02084830(engine->unk_0B0E6);
        func_02087e94(engine->unk_0B0E6, engine->text, sizeof(engine->text));
    } else {
        func_02086b14(engine);
    }
    if (engine->unk_0EE11 != 0) {
        func_0208c690(engine);
        return;
    }
    OS_CreateThread(&engine->thread, func_0208c690, engine, engine->stack + sizeof(engine->stack),
                    sizeof(engine->stack), engine->threadPriority);
    OS_WakeupThreadDirect(&engine->thread);
}

// The thread that synthesizes: takes the text a part at a time, makes phones and then segments of each and
// synthesizes them, until the text ends or the engine is asked to stop. Then tells the callback, with -1 if it was
// stopped and 0 if not.
void func_0208c690(void *arg) {
    Shtts *engine = arg;
    int count;
    ShttsUnk_01660 *list;
    int added;

    func_0208be38(engine);
    func_02086a74(engine->text, &engine->unk_0E628);
    while (1) {
        if (engine->unk_0EE14 == 1) {
            if (func_0208cc10((void *)&engine->phones[0], engine) == -1) {
                break;
            }
            engine->segmentCount = 0;
            func_0208b738(&engine->phones[1], engine->unk_01654, engine);
            func_0208ad48(engine);
            continue;
        }
        if (func_02086b90((void *)engine->unk_00A24, engine->unk_00010, engine) == 0) {
            break;
        }
        func_02086fd8((void *)engine->unk_00A44, (void *)engine->unk_01244, (void *)engine->unk_01654);
        func_02085b30((void *)engine->unk_01254, engine);
        func_0208a038((void *)&engine->unk_01660, engine);
        count = 0;
        for (list = &engine->unk_01660; list != NULL; list = list->next) {
            added = func_0208c8e0((void *)list, (void *)&engine->phones[count], 0x100 - count);
            count += added;
            if (list->next == NULL || (added != 0 && engine->phones[count].kind == 0)) {
                func_0208cf40((void *)engine->phones, count, list->items[list->count - 1].unk_8[7], engine);
                engine->segmentCount = 0;
                count = 0;
                func_0208b738(&engine->phones[1], list, engine);
                func_020856b0(&engine->unk_0B4E8, engine->cur.chorus);
                func_0208ad48(engine);
                if (func_02089d4c(engine) == 1) {
                    break;
                }
            }
        }
        if (func_02089d4c(engine) == 1) {
            break;
        }
        func_0208c054(engine);
        func_0208be38(engine);
    }
    if (engine->callback != NULL) {
        if (func_02089d4c(engine) == 1) {
            engine->callback(-1, NULL, engine->callbackArg);
        } else {
            engine->callback(0, NULL, engine->callbackArg);
        }
    }
    engine->phoneme = 0;
    func_02089cf4(engine);
    engine->busy = 0;
    func_02089d08(engine);
    func_02089cec(engine);
}
