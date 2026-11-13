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

int ShttsDuration_ClassifyMoraCount(s16 value) {
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

// Scales consonantLength and vowelLength of each sound of a list by the speed, a percentage. For some kinds of sound, consonantLength only
// if the sound has a 'Q'.
void ShttsDuration_ApplySpeed(ShttsSyllable **list, char *kinds, int count, Shtts *engine) {
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
            if (list[i]->qMark == 'Q') {
                list[i]->consonantLength = (engine->cur.speed * list[i]->consonantLength + 50) / 100;
            }
            list[i]->vowelLength = (engine->cur.speed * list[i]->vowelLength + 50) / 100;
            break;
        default:
            list[i]->consonantLength = (engine->cur.speed * list[i]->consonantLength + 50) / 100;
            list[i]->vowelLength = (engine->cur.speed * list[i]->vowelLength + 50) / 100;
            break;
        }
    }
}

// Empties the echo's history
void ShttsEffect_ResetEcho(Shtts *engine) {
    engine->echoCount = 0;
    memset(engine->echoBuffer, 0, sizeof(engine->echoBuffer));
}

// The echo: adds to each sample the one of a delay ago, from a ring of 3001 that it feeds with the samples
// filtered and scaled by the gain
void ShttsEffect_ApplyEcho(int count, s16 *src, s16 *dst, Shtts *engine) {
    int i;
    s16 gain = engine->cur.echoGain << 8;

    for (i = 0; i < count; i++) {
        int sample;
        int filtered = ShttsEffect_SumEchoFilter(engine->echoFilterState, ShttsEchoFilterCoefs, src[i] << 13) << 1;

        engine->echoFilterState[3] = engine->echoFilterState[2];
        engine->echoFilterState[2] = src[i] << 13;
        engine->echoFilterState[1] = engine->echoFilterState[0];
        engine->echoFilterState[0] = filtered << 1;
        if (engine->echoCount < engine->cur.echoDelay) {
            engine->echoBuffer[engine->echoCount] = ShttsEffect_MultiplyDoubled(filtered, gain);
            sample = src[i];
        } else {
            engine->echoBuffer[engine->echoCount % 3001] =
                ShttsEffect_MultiplyDoubled(filtered + engine->echoBuffer[(engine->echoCount - engine->cur.echoDelay) % 3001], gain);
            sample = src[i] + (engine->echoBuffer[(engine->echoCount - engine->cur.echoDelay) % 3001] >> 12);
        }
        if (sample > 0x7FFF) {
            sample = 0x7FFF;
        } else if (sample < -0x8000) {
            sample = -0x7FFF;
        }
        dst[i] = sample;
        engine->echoCount++;
    }
}

asm int ShttsEffect_SumEchoFilter(const int *state, const s16 *coefficients, int input) {
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

void ShttsLine_SetCursor(char *value, char **out) {
    *out = value;
}

// Copies a line of text, up to size, and moves the cursor past it. Returns its length, -1 at the end of the text.
int ShttsLine_ReadNext(char *dst, int size, char **cursor) {
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
void ShttsText_ReadStdin(Shtts *engine) {
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
int ShttsWord_ReadWords(ShttsSpokenWord *words, ShttsSyllable *sounds, Shtts *engine) {
    char line[0xC0];
    char word[0xC1];
    char *cursor;
    int accent;
    int phrase;
    int pause;
    int end;
    char *text = engine->wordText;
    int n; // The word's type, then the speed's factor, then the length of its text
    s16 count;
    int textLength;
    int soundCount;
    char *token;
    s16 j;
    int c;
    s16 i;

    ShttsWord_ClearSyllable(sounds, engine);
    count = 0;
    words->pause = 0;
    words->next = words + 1;
    textLength = 0;
    soundCount = 0;
    words = words->next;
    while (1) {
        if (ShttsLine_ReadNext(line, 0xC0, &engine->lineCursor) < 0) {
            break;
        }
        i = strlen(line);
        if (line[i - 1] < ' ') {
            line[i - 1] = 0;
        }
        accent = 10;
        words->lineValue6 = words->lineValue7 = n = 0;
        phrase = accent;
        word[0] = 0;
        cursor = line;
        pause = 200;
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            strcpy(word, token);
        }
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            n = atoi(token);
        }
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            accent = atoi(token);
        }
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            phrase = atoi(token);
        }
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            pause = atoi(token);
        }
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            words->lineValue6 = atoi(token);
        }
        token = ShttsUtil_NextToken(&cursor, " ");
        if (token != NULL) {
            words->lineValue7 = atoi(token);
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
        words->accentGiven = n;
        words->accentPosition = n;
        words->accentStrength = (accent * engine->cur.accent * 0x28F + 0x8000) >> 16;
        words->phraseStrength = (phrase * engine->cur.phrase * 0x28F + 0x8000) >> 16;
        n = (engine->cur.speed * 0x28F + 0x80) >> 8;
        words->pause = ShttsParam_Clamp((n * pause + 0x80) >> 8, 0, 30000, "pause*speed");
        words->unitSet = engine->cur.unitSet;
        words->speed = engine->cur.speed;
        words->volume = engine->cur.volume;
        words->marks = 0;
        if (words->accentPosition == -1) {
            words->accentPosition = ShttsDict_FindWord(word, (ShttsDict *)engine);
            if (words->accentPosition == -1) {
                words->accentPosition = ShttsWord_GetDefaultAccent(word);
            }
        }
        end = ShttsWord_PrepareText(word, line, engine);
        if ((int)(textLength + (strlen(line) + 1)) > 0x1C0) {
            line[0x1BF - textLength] = 0;
        }
        strcpy(text, line);
        words->text = text;
        n = strlen(text);
        if (n > 0) {
            ShttsWord_SplitSyllables(words, sounds, 0x80 - soundCount, engine);
            if (words->count == 0) {
                continue;
            }
            sounds += words->count;
            soundCount += words->count;
            if (words->marks & 0x70) {
                words->accentStrength = words->accentStrength * ((((words->marks & 0x70) >> 4) + 2) >> 1);
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

// Groups a list of words into runs, and the runs into runs of those. A word starts a run if its accentStrength or phraseStrength
// is set, if the word before it has a pause, or if it is the first; it starts a run of runs for the same reasons
// but accentStrength.
void ShttsWord_GroupWords(ShttsSpokenWord *word, ShttsAccentGroup *a, ShttsPhraseGroup *b) {
    ShttsSpokenWord *first = word;

    a->count = 0;
    for (; word != NULL; word = word->next) {
        if ((word->accentStrength != 0 || word->phraseStrength != 0) || a->count == 0 || word[-1].pause != 0 || word == first) {
            a->next = a + 1;
            a++;
            a->first = word;
            a->last = word;
            a->count = 1;
            a->accentStrength = word->accentStrength;
            a->accentPosition = word->accentPosition;
            if (word->phraseStrength != 0 || word[-1].pause != 0 || word == first) {
                b->next = b + 1;
                b++;
                b->first = a;
                b->count = 1;
                b->phraseStrength = word->phraseStrength;
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
int ShttsWord_SplitSyllables(ShttsSpokenWord *word, ShttsSyllable *sound, int max, Shtts *engine) {
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
            if ((word->marks & 7) < 3) {
                word->marks++;
            }
            break;
        case '!':
            if (((word->marks & 0x70) >> 4) < 3) {
                word->marks += 0x10;
            }
            break;
        case 'Q':
            sound->qMark = 'Q';
            break;
        case 'A':
        case 'I':
        case 'U':
        case 'E':
        case 'O':
            if (c <= 'Z' && c >= 'A') {
                c += 0x20;
            }
            sound->vowelMark = 'U';
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
                if (sound == engine->syllables) {
                    break;
                }
                if (sound->name[0] != 0) {
                    break;
                }
                sound[-1].vowelMark = 'L';
            }
            p[0] = c;
            p[1] = 0;
            if (strcmp(sound->name, "-") != 0) {
                for (j = 0; j < 0x90; j++) {
                    if (strcmp(sound->name, ShttsSyllableNames[j]) == 0) {
                        break;
                    }
                }
                if (j == 0x90) {
                    ShttsWord_ClearSyllable(sound, engine);
                    p = sound->name;
                    break;
                }
            }
            sound->pitch = engine->cur.pitch;
            sound->next = sound + 1;
            sound = sound->next;
            ShttsWord_ClearSyllable(sound, engine);
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
    ShttsWord_ClearSyllable(sound, engine);
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
int ShttsParam_RunCommand(const char *command, int live, Shtts *engine) {
    int value;

    if (command[0] == '#') {
        switch (command[1]) {
        case 'F':
            engine->cur.pitch = 0x13C2;
            engine->cur.unitSet = 0;
            return -3;
        case 'M':
            engine->cur.pitch = 0x1200;
            return -3;
        case 'B':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.pitch + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.pitch = ShttsParam_Clamp(value, 3500, 7000, "Pitch");
            } else {
                if (command[2] == '+') {
                    value = engine->set.pitch + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.pitch = ShttsParam_Clamp(value, 3500, 7000, "Pitch");
            }
            return -3;
        case 'C':
            if (live) {
                value = atoi(command + 2);
                engine->cur.accentRule = 0;
                engine->cur.mode = 0;
                if (value == 1) {
                    engine->cur.mode = 1;
                } else if (value == 2) {
                    engine->cur.mode = 2;
                } else if (value != 0) {
                    engine->cur.accentRule = 1;
                }
            } else {
                value = atoi(command + 2);
                engine->set.accentRule = 0;
                engine->set.mode = 0;
                if (value == 1) {
                    engine->set.mode = 1;
                } else if (value == 2) {
                    engine->set.mode = 2;
                } else if (value != 0) {
                    engine->set.accentRule = 1;
                }
            }
            return -3;
        case 'E':
            // Emphasis: higher, stronger, slower and louder
            engine->cur.pitch = ShttsParam_Clamp(engine->cur.pitch + 100, 3500, 7000, "Pitch");
            engine->cur.accent = ShttsParam_Clamp(engine->cur.accent + 5, 0, 400, "Accent");
            engine->cur.phrase = ShttsParam_Clamp(engine->cur.phrase + 5, 0, 400, "Phrase");
            engine->cur.speed = ShttsParam_Clamp(engine->cur.speed - 10, 20, 2200, "Speed");
            engine->cur.volume = ShttsParam_Clamp(engine->cur.volume + 10, -10000, 360, "Volume");
            return -3;
        case 'D':
            // The opposite
            engine->cur.pitch = ShttsParam_Clamp(engine->cur.pitch - 100, 3500, 7000, "Pitch");
            engine->cur.accent = ShttsParam_Clamp(engine->cur.accent - 5, 0, 400, "Accent");
            engine->cur.phrase = ShttsParam_Clamp(engine->cur.phrase - 5, 0, 400, "Phrase");
            engine->cur.speed = ShttsParam_Clamp(engine->cur.speed + 10, 20, 2200, "Speed");
            engine->cur.volume = ShttsParam_Clamp(engine->cur.volume - 10, -10000, 360, "Volume");
            return -3;
        case 'A':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 400, "Accent");
            if (live) {
                engine->cur.accent = value;
            } else {
                engine->set.accent = value;
            }
            return -3;
        case 'P':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.phrase + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.phrase = ShttsParam_Clamp(value, 0, 400, "Phrase");
            } else {
                if (command[2] == '+') {
                    value = engine->set.phrase + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.phrase = ShttsParam_Clamp(value, 0, 400, "Phrase");
            }
            return -3;
        case 'S':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.speed + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.speed = ShttsParam_Clamp(value, 20, 2200, "Speed");
            } else {
                if (command[2] == '+') {
                    value = engine->set.speed + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.speed = ShttsParam_Clamp(value, 20, 2200, "Speed");
            }
            return -3;
        case 'V':
            if (live) {
                if (command[2] == '+') {
                    value = engine->cur.volume + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->cur.volume = ShttsParam_Clamp(value, -10000, 360, "Volume");
            } else {
                if (command[2] == '+') {
                    value = engine->set.volume + ShttsParam_ParseShort(command + 3);
                } else {
                    value = atoi(command + 2);
                }
                engine->set.volume = ShttsParam_Clamp(value, -10000, 360, "Volume");
            }
            return -3;
        case 'I':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 3000, "Echo Delay");
            if (!live) {
                engine->set.echoDelay = value;
            } else {
                engine->cur.echoDelay = value;
                ShttsEffect_ResetEcho(engine);
            }
            return -3;
        case 'J':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 100, "Echo Gain");
            if (live) {
                engine->cur.echoGain = value;
            } else {
                engine->set.echoGain = value;
            }
            return -3;
        case 'R':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 1000, "Fir Delay");
            if (!live) {
                engine->set.firDelay = value;
            } else {
                ShttsEffect_ResetFir(engine);
                engine->cur.firDelay = value;
            }
            return -3;
        case 'G':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 100, "Fir Gain");
            if (!live) {
                engine->set.firGain = value;
            } else {
                ShttsEffect_ResetFir(engine);
                engine->cur.firGain = value;
            }
            return -3;
        case 'r':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 5000, "Ring");
            if (live) {
                engine->cur.ring = value;
            } else {
                engine->set.ring = value;
            }
            return -3;
        case 'k':
            value = ShttsParam_Clamp(atoi(command + 2), 0, 100, "Chorus");
            if (live) {
                engine->cur.chorus = value;
            } else {
                engine->set.chorus = value;
            }
            return -3;
        case 'X':
            ShttsParam_StoreWarp(ShttsParam_Clamp(atoi(command + 2), 80, 130, "Warp"), live, engine);
            return -3;
        case 'Z':
            ShttsParam_UseSetParams(engine);
            return -3;
        case 'T':
            if (isdigit(command[2])) {
                value = ShttsParam_Clamp(atoi(command + 2), 0, 1000, "PTrans");
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
int ShttsWord_PrepareText(char *text, char *out, Shtts *engine) {
    s16 i;
    s16 pos;
    int c;

    *out = 0;
    if (ShttsParam_RunCommand(text, 1, engine) != 0) {
        return -3;
    }
    for (i = 0; i < 5; i++) {
        pos = 0;
        do {
            int found = ShttsUtil_FindString(text + pos, ShttsRepeatedVowels[i]);

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

void ShttsWord_ClearSyllable(ShttsSyllable *syllable, Shtts *engine) {
    syllable->name[0] = '\0';
    syllable->qMark = syllable->vowelMark = '-';
    syllable->vowelLength = 0;
    syllable->consonantLength = 0;
    syllable->next = NULL;
    syllable->pitch = engine->cur.pitch;
}

// Where one string first is in another, or -1
int ShttsUtil_FindString(const char *text, const char *find) {
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
s16 ShttsParam_Clamp(int value, s16 min, s16 max, const char *name) {
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
int ShttsParam_ParseShort(const char *text) {
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
// phrase until punctuation ends it; a phrase becomes one line of its phonemes and four numbers (see ShttsText_AppendWordLine).
// Commands between "#Q" and "#q", and other text that starts with '#', are passed on as lines of their own.
// Returns 0, or where in text it had got to when dest ran out of room.
const char *ShttsText_ConvertToLines(const char *text, char *dest, u32 size) {
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
                            result = ShttsText_AppendLine(dest, size, command);
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
                    result = ShttsText_AppendLine(dest, size, command);
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
                pauseSet = ShttsParam_Clamp(atoi(number), 0, 30000, "pause");
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
                    result = ShttsText_AppendLine(dest, size, command);
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
                    ShttsText_ReadStrengthBefore(text, &level);
                }
                result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap,
                                       ShttsParam_Clamp(pauseSet + pause, 0, 30000, "pause sum"));
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
                    ShttsText_ReadStrengthBefore(text, &level);
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
                result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap,
                                       ShttsParam_Clamp(pauseSet + pause, 0, 30000, "pause sum"));
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
                        ShttsText_ReadStrengthBefore(text, &level);
                    }
                    if (pauseSet != 0) {
                        result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pauseSet);
                        pauseSet = 0;
                    } else {
                        result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
                    }
                    total = 0;
                } else {
                    if (accent == 0 && level == 10) {
                        ShttsText_ReadStrengthBefore(text, &level);
                    }
                    result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
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
                    ShttsText_ReadStrengthBefore(text, &level);
                }
                if (pause != 0 && pauseSet != 0) {
                    result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pauseSet);
                    pauseSet = 0;
                    total = 0;
                } else {
                    result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
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
                            ShttsText_ReadStrengthBefore(text, &level);
                        }
                        result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pauseSet);
                        pauseSet = 0;
                    } else {
                        if (accent == 0 && level == 10) {
                            ShttsText_ReadStrengthBefore(text, &level);
                        }
                        result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
                    }
                    total = 0;
                } else {
                    if (accent == 0 && level == 10) {
                        ShttsText_ReadStrengthBefore(text, &level);
                    }
                    result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
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
                    ShttsText_ReadStrengthBefore(text, &level);
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
            value = ShttsText_ParseStrengthDigit(p, text);
            n = 1;
            if (value == -1) {
                strncpy(p, text, 2);
                value = ShttsText_ParseWideStrengthDigit(p, text);
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
            s16 index = ShttsText_FindKana(text, ShttsKanaTable[0], ShttsKanaCount);

            if (index >= 0) {
                if (pause != 0) {
                    if (phonemes[0] != '\0') {
                        if (pauseSet != 0) {
                            if (accent == 0 && level == 10) {
                                ShttsText_ReadStrengthBefore(text, &level);
                            }
                            result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pauseSet);
                            pauseSet = 0;
                        } else {
                            if (accent == 0 && level == 10) {
                                ShttsText_ReadStrengthBefore(text, &level);
                            }
                            result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
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
                    s16 add = strlen(&ShttsKanaTable[index][8]);

                    if (!(length + add < 0x40 && total + add < 0x100)) {
                        result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, 100);
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
                        char first = ShttsKanaTable[index][8];

                        if (n != 0 && phonemes[n - 1] == 'Q' && first == 'Q') {
                        } else if (n == 0 && first == '-') {
                        } else if (n == 0 && first == 'Q') {
                        } else {
                            strcat(phonemes, &ShttsKanaTable[index][8]);
                        }
                    }
                }
                text += strlen(ShttsKanaTable[index]);
                total += strlen(ShttsKanaTable[index]);
                length += strlen(ShttsKanaTable[index]);
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
                ShttsText_ReadStrengthBefore(text, &level);
            }
            result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pauseSet);
        } else {
            if (accent == 0 && level == 10) {
                ShttsText_ReadStrengthBefore(text, &level);
            }
            result = ShttsText_AppendWordLine(dest, size, phonemes, accent, level, gap, pause);
        }
        if (result < 0) {
            return done;
        }
    }
    return 0;
}

// Writes a number in decimal
char *ShttsText_FormatNumber(char *out, s16 value) {
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
s16 ShttsText_AppendLine(char *text, u32 size, const char *line) {
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
s16 ShttsText_AppendWordLine(char *dest, u32 size, const char *text, s16 a, s16 b, s16 c, s16 d) {
    char line[0xC0];
    char number[16];
    int length;

    if (text[0] == '\0') {
        return 0;
    }
    strcpy(line, text);
    strcat(line, " ");
    strcat(line, ShttsText_FormatNumber(number, a));
    strcat(line, " ");
    strcat(line, ShttsText_FormatNumber(number, b));
    strcat(line, " ");
    strcat(line, ShttsText_FormatNumber(number, c));
    strcat(line, " ");
    strcat(line, ShttsText_FormatNumber(number, d));
    strcat(line, "\n");
    length = strlen(line);
    if (length + strlen(dest) >= size) {
        return -1;
    }
    strcat(dest, line);
    return (s16)length;
}

// Which of a table's entries, of 12 bytes each, a text starts with, or -1
s16 ShttsText_FindKana(const char *text, const char *table, int count) {
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
int ShttsText_ParseStrengthDigit(const char *text, const char *unused) {
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
int ShttsText_ParseWideStrengthDigit(const char *text, const char *unused) {
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
int ShttsText_ReadDigitBefore(const char *end, s16 *out) {
    char digit[3] = { 0, 0, 0 };
    int value;

    strncpy(digit, end - 1, 1);
    value = ShttsText_ParseStrengthDigit(digit, end - 1);
    *out = 1;
    if (value == -1) {
        strncpy(digit, end - 2, 2);
        value = ShttsText_ParseWideStrengthDigit(digit, end - 2);
        *out = 2;
    }
    if (value == -1) {
        *out = 0;
    }
    return value;
}

void ShttsText_ReadStrengthBefore(const char *text, s16 *out) {
    s16 unused = 0;
    int level = ShttsText_ReadDigitBefore(text, &unused);

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
s16 ShttsMath_DivideRounded(int a, int b) {
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
int ShttsSynth_DecodeFrame(ShttsFrame *frame, s16 *out, Shtts *engine) {
    s16 buffer[0x100];
    u16 freq[10];
    s16 coef[11];
    s32 state[10];
    int count;
    int cut;
    int i;
    s16 n;
    int nibble;

    if (frame->sampleCount == 0) {
        *out = 0;
        return 0;
    }
    for (n = 0; n < 10; n++) {
        if (n % 2 == 1) {
            nibble = frame->lspIndices[n / 2] & 0xF;
        } else {
            nibble = (frame->lspIndices[n / 2] >> 4) & 0xF;
        }
        if (n == 0) {
            freq[n] = ShttsLspCodebook[n][nibble];
        } else {
            freq[n] = freq[n - 1] + ShttsLspCodebook[n][nibble];
        }
    }
    i = 0;
    cut = 0;
    if (engine->cur.warpScale != 0x400) {
        if ((int)engine->cur.warp > 100) {
            cut = 10 - engine->cur.warpOrder;
            for (; i < 10; i++) {
                int b = (engine->cur.warpScale * freq[i]) >> 10;
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
                int v = ((engine->cur.warpScale * (freq[i] - base)) >> 10) + base;
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
    ShttsSynth_ConvertLspToLpc(10 - cut, freq, &coef[1]);
    memset(&coef[11] - cut, 0, cut * 2);
    if (frame->voiced != 0) {
        memset(state, 0, sizeof(state));
        if (ShttsPhonemeFlags[frame->phoneme] & 0x84) {
            ShttsSynth_FilterRecursive(0x100, 10, coef, state, ShttsVoicedExcitationAlt, buffer, engine);
        } else {
            ShttsSynth_FilterRecursive(0x100, 10, coef, state, ShttsVoicedExcitation, buffer, engine);
        }
        count = 0xF0;
        ShttsSynth_NormalizeLevel(0x61C, buffer, out + 1, count);
    } else {
        count = engine->frameLength;
        memset(state, 0, sizeof(state));
        ShttsSynth_FilterRecursive(count, 10, coef, state, &ShttsUnvoicedExcitation[engine->frameLength * engine->unvoicedBlock], buffer, engine);
        engine->unvoicedBlock++;
        if (engine->unvoicedBlock >= 6) {
            engine->unvoicedBlock = 0;
        }
        ShttsSynth_NormalizeLevel(0x800, buffer, out + 1, count);
    }
    *out = count;
    frame->sampleCount = count;
    return count;
}

// Turns `order` values into as many filter coefficients. Each value is first looked up, with interpolation, in the
// cosine table that ShttsCosineTable points to, and negated. Then an impulse (0x2000000, then zeros) is put through two
// chains of order / 2 second-order sections, whose coefficients are the even and the odd cosines, and half the sum
// of the two chains' outputs, from the second on, is the result.
//
// The first loop only matches with the multiplication as an inline function returning s16 (ShttsMath_MultiplyQ15) and
// with `frac` reused for the product and then for the sum.
void ShttsSynth_ConvertLspToLpc(int order, u16 *in, s16 *out) {
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
        frac = ShttsMath_MultiplyQ15(ShttsCosineTable[j + 1] - ShttsCosineTable[j], frac);
        frac += ShttsCosineTable[j];
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
            r = ShttsMath_MultiplyFixed(cosines[j * 2] << 16, t);
            old = c[j];
            c[j] = t;
            p[j + 1] = old + (p[j] + (r << 2));
            t = b[j];
            d[j] = p[j];
            r = ShttsMath_MultiplyFixed(cosines[j * 2 + 1] << 16, t);
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
void ShttsSynth_NormalizeLevel(int level, s16 *in, s16 *out, int count) {
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
        root = ShttsMath_ApproximateSqrt(count) >> 5;
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
        sum = (s16)(ShttsMath_ApproximateSqrt(sum) >> 8);
        gain = ShttsMath_DivideRounded((s16)level * root, (s16)sum);
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
int ShttsMath_MultiplyFixed(int a, int b) {
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
int ShttsMath_ApproximateSqrt(int x) {
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
int ShttsEngine_ResetSynthesis(Shtts *engine) {
    engine->pitchContour[0] = 0;
    engine->prevRunWeight = 0;
    engine->runKind = 0;
    ShttsOutput_Reset(engine);
    engine->lastOutputSample = 0;
    ShttsUtil_FillBytes(engine->outputFilterState, 0, sizeof(engine->outputFilterState));
    engine->unvoicedBlock = 0;
    engine->prevPhraseNotFirst = 0;
    engine->prevPhraseStrength = 0;
    engine->samplesOwed = 0;
    engine->ringPhase = 0;
    memset(engine->echoFilterState, 0, sizeof(engine->echoFilterState));
    return 0;
}

int ShttsEngine_EndThread(Shtts *engine) {
    return 0;
}

void ShttsEngine_Lock(Shtts *engine) {
    OS_LockMutex(&engine->mutex);
}

void ShttsEngine_Unlock(Shtts *engine) {
    OS_UnlockMutex(&engine->mutex);
}

// Whether the engine is synthesizing
int ShttsEngine_IsBusy(Shtts *engine) {
    int busy;

    OS_LockMutex(&engine->mutex);
    busy = engine->busy;
    OS_UnlockMutex(&engine->mutex);
    return busy;
}

// Whether the engine has been asked to stop
int ShttsEngine_IsStopRequested(Shtts *engine) {
    int stopRequested;

    OS_LockMutex(&engine->mutex);
    stopRequested = engine->stopRequested;
    OS_UnlockMutex(&engine->mutex);
    return stopRequested;
}

// Where a character first is in a string, or NULL. Unlike strchr, it does not find the end of the string.
char *ShttsUtil_FindChar(char *text, int c) {
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
char *ShttsUtil_NextToken(char **cursor, char *delimiters) {
    char *word;

    while (**cursor != 0 && ShttsUtil_FindChar(delimiters, **cursor) != NULL) {
        (*cursor)++;
    }
    word = *cursor;
    if (*word == 0) {
        return NULL;
    }
    while (**cursor != 0 && ShttsUtil_FindChar(delimiters, **cursor) == NULL) {
        (*cursor)++;
    }
    if (**cursor != 0) {
        *(*cursor)++ = 0;
    }
    return word;
}

void ShttsUtil_FillBytes(char *dst, int c, int count) {
    int i;

    for (i = 0; i < count; i++) {
        *dst++ = (char)c;
    }
}

// Called with -1 when something is wrong. Does nothing in this build.
void ShttsUtil_ReportError(int code) {
}

// Scales samples by a gain in 4.12 fixed point, keeping them within 16 bits
void ShttsMath_ScaleSamples(s16 *dst, s16 *src, int count, int gain) {
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
void ShttsEffect_ApplyFir(int count, s16 *in, s16 *out, Shtts *engine) {
    int gain = (int)(engine->cur.firGain << 8) / 100;
    int i;
    int v;

    for (i = 0; i < count; i++) {
        if (engine->firCount < engine->cur.firDelay) {
            out[i] = in[i];
        } else {
            v = in[i] + ((gain * engine->firBuffer[(engine->firCount - engine->cur.firDelay) % 1001]) >> 8);
            if (v > 0x7FFF) {
                v = 0x7FFF;
            } else if (v < -0x8000) {
                v = -0x8000;
            }
            out[i] = v;
        }
        engine->firBuffer[engine->firCount % 1001] = in[i];
        engine->firCount++;
    }
}

// Empties the FIR filter's history
void ShttsEffect_ResetFir(Shtts *engine) {
    engine->firCount = 0;
    memset(engine->firBuffer, 0, sizeof(engine->firBuffer));
}

// Shapes the pitch of everything in a list: first ShttsPitch_AddAccent for each ShttsPitchAccentGroup, then, for each stretch
// that ends at a ShttsPitchWord with pause set, or at the list's end, ShttsPitch_AddPhraseCurve with the sum of consonantLength and
// vowelLength of its ShttsSyllables.
void ShttsPitch_ShapeGroups(ShttsPitchPhraseGroup *list, Shtts *engine) {
    ShttsPitchPhraseGroup *a;
    ShttsPitchPhraseGroup *cur;
    ShttsPitchAccentGroup *b;
    ShttsPitchPhraseGroup *e;
    ShttsPitchWord *c;
    ShttsSyllable *d;
    s16 i;
    s16 j;
    s16 k;
    s16 notFirst;
    int length;

    for (e = list; e != NULL; e = e->next) {
        b = e->first;
        for (i = 0; i < e->count; i++) {
            ShttsPitch_AddAccent(b, engine);
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
                        length += d->consonantLength + d->vowelLength;
                        d = d->next;
                    }
                    length += d->consonantLength + d->vowelLength;
                    if (c->pause != 0 || (a->next == NULL && b->next == NULL && b->last == c)) {
                        if (cur->next == NULL) {
                            if (c->marks != 0) {
                                ShttsPitch_AddPhraseCurve(cur, length, d, 2, notFirst, engine);
                            } else {
                                ShttsPitch_AddPhraseCurve(cur, length, d, 1, notFirst, engine);
                            }
                        } else {
                            ShttsPitch_AddPhraseCurve(cur, length, d, 0, notFirst, engine);
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

// Adds a curve (ShttsPitch_EvalPhraseCurve) to the pitch of each ShttsSyllable from the first of `group` to `last`, at a rate
// that falls with `length`, and lowers the pitch of the last one when the pause of the last ShttsPitchWord is
// near certain multiples of cur.speed. `kind` is 0 when more follows, 1 or 2 at the end.
int ShttsPitch_AddPhraseCurve(ShttsPitchPhraseGroup *group, int length, ShttsSyllable *last, int kind, s16 notFirst,
                  Shtts *engine) {
    s16 offset;
    ShttsPitchAccentGroup *b;
    s16 depth;
    ShttsPitchWord *c;
    ShttsSyllable *d;
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
    b = group->first;
    if (notFirst == 1) {
        if (group->phraseStrength == 1) {
            if (engine->prevPhraseNotFirst == 1 && engine->prevPhraseStrength == 1) {
                depth = 60;
            } else {
                depth = 200;
            }
        } else {
            depth = group->phraseStrength * 105 - (b->first->first->pitch - engine->cur.pitch);
        }
    } else {
        depth = group->phraseStrength * 105;
    }
    engine->prevPhraseNotFirst = notFirst;
    engine->prevPhraseStrength = group->phraseStrength;
    if (depth < 0) {
        depth = 0;
    }
    d = b->first->first;
    offset = ShttsMath_DivideRounded(0xFA000, rate) + (-83 - (d->consonantLength + ((d->vowelLength + 1) >> 1)));
    pos = 0;
    while (d != last) {
        pos += d->consonantLength + ((d->vowelLength + 1) >> 1);
        d->pitch += (s16)((ShttsPitch_EvalPhraseCurve(rate, pos + offset) * (depth * 3) + 0x400) >> 11);
        pos += d->vowelLength >> 1;
        d = d->next;
    }
    d->pitch +=
        (s16)((depth * 3 * ShttsPitch_EvalPhraseCurve(rate, pos + (d->consonantLength + ((d->vowelLength + 1) >> 1)) + offset) + 0x400) >> 11);
    c = b[group->count - 1].last;
    if (kind == 1 && c->pause > (1 - engine->cur.speed) * -10 && c->pause < (engine->cur.speed + 1) * 10) {
        last->pitch -= group->phraseStrength / 10 * 170;
    } else if (kind == 0 && c->pause > engine->cur.speed * 3 - 10 && c->pause < engine->cur.speed * 3 + 10) {
        last->pitch -= group->phraseStrength / 10 * 120;
    } else if (kind == 0 && c->pause > engine->cur.speed * 2 - 10 && c->pause < engine->cur.speed * 2 + 10) {
        last->pitch -= group->phraseStrength / 10 * 70;
    }
    return 0;
}

// Raises and lowers the pitch of the ShttsSyllables of one ShttsPitchAccentGroup around the position accentPosition, by an
// amount that accentStrength scales. accentPosition from 30 up carries a direction in its tens, and cur.accentRule chooses how accentPosition is
// reworked from the number that ShttsWord_CountMorae counts.
void ShttsPitch_AddAccent(ShttsPitchAccentGroup *group, Shtts *engine) {
    ShttsSyllable *first;
    int depth;
    int end;
    s16 i;
    int total;
    ShttsPitchWord *c;
    s16 n;
    ShttsSyllable *next;
    ShttsSyllable *d;
    int k;
    int dir;
    s8 accent;
    char lastA;
    char lastB;

    c = group->first;
    first = c->first;
    end = 0;
    if ((first->vowelMark == 'L' || first[1].qMark == 'Q' || first[1].name[0] == 'N') && group->accentPosition != 1) {
        depth = group->accentStrength * 0x84B6;
    } else {
        depth = group->accentStrength * 0x9375;
    }
    if (group->last->pause > engine->cur.speed * 3) {
        if (group->last->accentPosition == 0) {
            depth = (depth * 0x666) >> 12;
        } else if (group->last->accentPosition == 1) {
            if (group->last->count < 3) {
                depth >>= 2;
            }
        } else if (group->last->count - group->last->accentPosition < 2) {
            depth >>= 1;
        }
    }
    total = 0;
    for (i = 0; i < group->count; i++) {
        total = (s16)(total + ShttsWord_CountMorae(c->text));
        c = c->next;
    }
    if (group->accentPosition / 30 == 1) {
        group->accentPosition -= 30;
        dir = -1;
    } else if (group->accentPosition / 30 == 2) {
        group->accentPosition -= 60;
        dir = 1;
    } else if (group->accentPosition / 30 == 3) {
        group->accentPosition -= 90;
        dir = 0;
    } else if (engine->cur.accentRule == 1) {
        if (total == 2) {
            if (group->accentPosition == 0) {
                group->accentPosition = 1;
            } else if (group->accentPosition == 1) {
                group->accentPosition = 0;
            }
        } else if (total > 2 && group->accentPosition == 1 && first->vowelMark != 'L') {
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
            if (lastA != lastB && next->qMark != 'Q') {
                group->accentPosition = 2;
            }
        }
        if (group->accentPosition == 2) {
            dir = -1;
        } else if (group->accentPosition >= total) {
            dir = 1;
        } else if (total == 2) {
            dir = -1;
        } else if (total % 2 == 1) {
            dir = -1;
        } else {
            dir = 1;
        }
    } else if (engine->cur.accentRule == 2) {
        dir = -1;
        if (total <= 3) {
            group->accentPosition = total;
        } else {
            group->accentPosition = total - 1 + total % 2;
        }
    } else {
        dir = 0;
    }
    if (group->accentPosition == 1) {
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
        accent = group->accentPosition;
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
        if (d->qMark != 'Q') {
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
            if (d->qMark == 'Q' && k >= 2 && group->accentPosition != 0 && total >= group->accentPosition) {
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
    if (group->last->pause != 0) {
        return;
    }
    if (total <= 1) {
        return;
    }
    if (dir == 0) {
        d->pitch += (s16)((depth + 0x800) >> 12);
    }
}

// The value at a * b / 256 of a curve of five points: ShttsPhraseCurveX holds where they are, ShttsPhraseCurveY their
// values (which the result has divided by 16), and ShttsPhraseCurveDX the distance from each to the one before.
s16 ShttsPitch_EvalPhraseCurve(int a, int b) {
    int x = (a * b) >> 8;
    s16 i;

    for (i = 1; i < 5; i++) {
        if (x < ShttsPhraseCurveX[i]) {
            s16 lo = ShttsPhraseCurveY[i - 1];

            return ShttsMath_DivideRounded(((x - ShttsPhraseCurveX[i - 1]) * (ShttsPhraseCurveY[i] - lo)) >> 4, ShttsPhraseCurveDX[i]) +
                   (lo >> 4);
        }
    }
    return 0;
}

void ShttsSynth_FitFrameGains(ShttsRunFrame *table, int count, int a, int b) {
    if (a < 0) {
        a = 0;
    } else {
        a = (s16)(a - table[0].gain);
    }
    if (b < 0) {
        b = a;
    } else {
        b = (s16)(b - table[count - 1].gain);
    }
    ShttsSynth_RampFrameGains(table, count, a, b);
}

// Adds to gain of each entry an amount that goes from `a` at the first to `b` at the last, each kept within 400
// either way, changing only past entries whose voiced is positive, and keeps the results from 0 to the greater of
// the two ends' new values.
void ShttsSynth_RampFrameGains(ShttsRunFrame *table, int count, int a, int b) {
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
    if (table[0].gain + a > (table + count)[-1].gain + b) {
        max = table[0].gain + a;
    } else {
        max = (table + count)[-1].gain + b;
    }
    step = ShttsMath_DivideRounded((b - a) << 11, (s16)(count - 1)) << 5;
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
        if (table->voiced > 0) {
            v = table->gain + (s16)(x >> 16);
        } else {
            v = table->gain + (s16)(y >> 16);
        }
        if (v < 0) {
            v = 0;
        }
        if (max < v) {
            v = max;
        }
        table->gain = v;
        x -= step;
        if (table->voiced > 0) {
            y = x;
        }
    }
}

// Synthesizes the segments. It takes them in runs that are consecutive entries of one unit, fills runFrames with the
// run's entries of ShttsUnitFrames, has ShttsSynth_StretchSegments make frames of it, ShttsSynth_WritePitchRamp a contour for them in pitchContour,
// and ShttsSynth_SynthesizeFrames the samples. A segment of silence then moves the sample buffer on by its length.
void ShttsSynth_SynthesizeSegments(Shtts *engine) {
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
    contour = &engine->pitchContour[1];
    last = engine->segmentCount - 1;
    engine->runKind = 0;
    engine->framePointers[0] = ShttsFixedFrames[1];
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
                if (!(ShttsPhonemeFlags[segment[-1].kind] & 0x80)) {
                    goto next;
                }
            }
            end = segment - 1;
            total = end->last - start->first + 1;
            bound = start->last - start->first + 1;
            cur = start;
            kind = start->kind;
            for (j = 0; j < total; j++) {
                s8 c = ShttsUnitFrames[start->unit + j].gainAndVoiced;
                s16 v = c & 0x7F;

                engine->runFrames[j].gain = v * 10;
                engine->runFrames[j].voiced = (c >> 7) & 1;
                if (j >= bound) {
                    cur++;
                    kind = cur->kind;
                    bound = cur->last - start->first + 1;
                }
                engine->runFrames[j].kind = kind;
                if (cur->pitch < 0) {
                    engine->runFrames[j].voiced = 0;
                }
                if (engine->runFrames[j].voiced == 0) {
                    engine->runFrames[j].sampleCount = engine->frameLength;
                } else {
                    engine->runFrames[j].sampleCount = 120;
                }
                for (k = 0; k < 5; k++) {
                    engine->runFrames[j].lspIndices[k] = ShttsUnitFrames[start->unit + j].lspIndices[k];
                }
            }
            for (p = start; p <= end; p++) {
                if (p->pitch < 0) {
                    p->pitch = -p->pitch;
                }
            }
            if (start == engine->segments) {
                s16 x = end->gain - engine->runFrames[total - 1].gain;

                ShttsSynth_RampFrameGains(engine->runFrames, total, x, x);
                engine->runKind = -1;
            } else if (i == last) {
                s16 x = start->gain - engine->runFrames[0].gain;

                ShttsSynth_RampFrameGains(engine->runFrames, total, x, x);
                engine->runKind = -2;
            } else if ((ShttsPhonemeFlags[start->kind] & 0x80) && start->kind != 20 && (ShttsPhonemeFlags[end->kind] & 0x80)
                       && end->kind != 20 && start != end) {
                ShttsSynth_FitFrameGains(engine->runFrames, total, start->gain, end->gain);
                engine->runKind = 0;
            } else if (engine->runKind == 1) {
                s16 x = end->gain - engine->runFrames[total - 1].gain;

                ShttsSynth_RampFrameGains(engine->runFrames, total, x, x);
                engine->runKind = 2;
            } else {
                s16 x = start->gain - engine->runFrames[0].gain;

                ShttsSynth_RampFrameGains(engine->runFrames, total, x, x);
                engine->runKind = 1;
            }
            frames = ShttsSynth_StretchSegments((void *)start, (void *)end, (void *)engine->runFrames, (void *)&engine->framePointers[1], engine);
            if (split > 0 && engine->runKind == 0) {
                ShttsSynth_WritePitchRamp(contour, (start->length + 1) / 2, start->pitch, 7000, engine);
                ShttsSynth_WritePitchRamp(contour + (start->length + 1) / 2, frames - (start->length + 1) / 2, end->pitch, 7000,
                              engine);
                split = 0;
            } else if (end->pitch != end[1].pitch && engine->runKind != -2 && end->kind == end[1].kind) {
                if (engine->runKind == -1) {
                    if (engine->prevRunWeight == 0) {
                        if (ShttsPhonemeFlags[start->kind] & 1) {
                            engine->pitchContour[0] = end->pitch;
                        } else {
                            engine->pitchContour[0] = end->pitch - (frames - end->length / 2) * 15;
                        }
                    } else {
                        engine->pitchContour[0] += ShttsMath_DivideRounded(
                            engine->prevRunWeight * (end->pitch - engine->pitchContour[0]),
                            (s16)(engine->prevRunWeight + frames - end->length / 2));
                    }
                }
                // The original reads end->pitch once and keeps it across the call, which this form gives
                sum = end->pitch
                      + ShttsMath_DivideRounded(end->length * (s16)(end[1].pitch - end->pitch),
                                      (s16)(end->length + end[1].length));
                target = sum < -0x8000 ? -0x8000 : sum > 0x7FFF ? 0x7FFF : sum;
                ShttsSynth_WritePitchRamp(contour, frames - end->length / 2, end->pitch, 7000, engine);
                ShttsSynth_WritePitchRamp(contour + frames - end->length / 2, end->length / 2, target, 7000, engine);
                split = 1;
            } else {
                if (engine->runKind == -1) {
                    if (engine->prevRunWeight == 0 || engine->cur.pitchTrans != 0) {
                        if ((ShttsPhonemeFlags[start->kind] & 1) || engine->cur.phrase == 0) {
                            engine->pitchContour[0] = end->pitch;
                        } else {
                            engine->pitchContour[0] = end->pitch - frames * 15;
                        }
                    } else {
                        engine->pitchContour[0] += ShttsMath_DivideRounded(
                            engine->prevRunWeight * (end->pitch - engine->pitchContour[0]), (s16)(engine->prevRunWeight + frames));
                    }
                } else if (engine->runKind == -2) {
                    if (engine->cur.phrase == 0) {
                        end->pitch = engine->pitchContour[0];
                    } else {
                        end->pitch = engine->pitchContour[0] - (s16)frames * 10;
                    }
                } else if (engine->pitchContour[0] <= 0) {
                    engine->pitchContour[0] = start->pitch;
                }
                ShttsSynth_WritePitchRamp(contour, frames, end->pitch, 7000, engine);
            }
            ShttsSynth_SynthesizeFrames(&engine->framePointers[1], contour, frames, engine);
            if (ShttsEngine_IsStopRequested(engine) == 1) {
                return;
            }
            engine->pitchContour[0] = engine->pitchContour[frames];
            engine->prevRunWeight = engine->framePointers[frames]->sampleCount;
            start = segment;
        }
    next:
        if (segment->kind == 0) {
            engine->phoneme = 0;
            for (m = 0; m < segment->length; m++) {
                ShttsOutput_Advance(engine->frameLength, engine);
                if (ShttsEngine_IsStopRequested(engine) == 1) {
                    return;
                }
            }
            if (ShttsEngine_IsStopRequested(engine) == 1) {
                return;
            }
            engine->prevRunWeight = segment->length;
        }
    }
}

// Writes count values that go from the one before out to target, neither above max. With the pitchTrans setting,
// only that many values make the move, after four tenths of the rest hold the first value; the others hold target.
void ShttsSynth_WritePitchRamp(s16 *out, s16 count, s16 target, s16 max, Shtts *engine) {
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
            step = ShttsMath_DivideRounded((target - from) << 6, count);
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
        step = ShttsMath_DivideRounded((target - from) << 6, count);
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
void ShttsEffect_ApplyRing(int *phase, int count, int frequency, s16 *src, s16 *dst) {
    int i;
    int step = (frequency << 16) / 11000;

    for (i = 0; i < count; i++) {
        (*phase)++;
        dst[i] = (src[i] * ShttsSineTable[((step * *phase) >> 8) & 0xFF]) >> 15;
    }
}

// Makes segments of a list of phones: takes them in groups that end at a phone of kind 6 or less, and has
// ShttsUnit_AddSegments look each group up as a unit, in parts if there is no unit for the whole.
void ShttsUnit_SelectUnits(ShttsPhone *phones, void *unused, Shtts *engine) {
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
        if (end->qMark == 'Q' && start != end
            && (end[-1].kind == 26 || end[-1].kind == 25 || end[-1].kind == 21 || end[-1].kind == 19
                || end[-1].kind == 18 || end[-1].kind == 15 || end[-1].kind == 20 || end[-1].kind == 23
                || end[-1].kind == 24)) {
            result = -1;
        } else if (engine->splitUnitAtConsonant == 1 && mode == 0 && end - start == 2
                   && (start[1].kind == 27 || start[1].kind == 12 || start[1].kind == 8 || start[1].kind == 36
                       || start[1].kind == 28 || start[1].kind == 11 || start[1].kind == 13 || start[1].kind == 37)) {
            saved = *(int *)&start[1]; // Read this way, the compiler does not merge it with the reads above
            start[1].kind = 0;
            ShttsUnit_AddSegments(1, start, start + 1, unused, groups, engine);
            start[1].kind = saved;
            result = ShttsUnit_AddSegments(-1, start + 1, end, unused, groups, engine);
        } else {
            result = ShttsUnit_AddSegments(mode, start, end, unused, groups, engine);
        }
        if (result < 0 && mode >= 0) {
            if (start->kind == end->kind && start + 1 == end) {
                int length = end->length + start->length;

                if (length > 0x7FFF) {
                    length = 0x7FFF;
                }
                end->length = length;
                ShttsUnit_AddSegments(mode, start + 1, end, unused, groups, engine);
            } else {
                mode = 1;
                if (ShttsUnit_AddSegments(mode, start, end - 1, unused, groups, engine) < 0) {
                    saved2 = end[-1].kind;
                    if (saved2 == 18) {
                        end[-1].kind = 2;
                    } else if (saved2 == 20) {
                        end[-1].kind = 3;
                    } else if (saved2 == 16) {
                        end[-1].kind = 15;
                    }
                    if (ShttsUnit_AddSegments(1, start, end - 1, unused, groups, engine) < 0) {
                        ShttsUnit_AddSegments(-2, start, start, unused, groups, engine);
                    }
                    end[-1].kind = saved2;
                }
                ShttsUnit_AddSegments(-1, end - 1, end, unused, groups, engine);
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
    ShttsUnit_AddSegments(-2, end, end, unused, groups, engine);
}

// Looks the phones from first to last up as a unit, with silence before them if mode is -1 and after them if it is
// -2, and adds a segment for each to the engine's. Returns -1 if there is no such unit.
int ShttsUnit_AddSegments(int mode, ShttsPhone *first, ShttsPhone *last, void *unused, s16 unused2, Shtts *engine) {
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
    unit = ShttsUnit_Find(index, engine);
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
        if (unit->bounds[i] == unit->bounds[i + 1]) {
            i++;
        }
        if (p->endPitch != 0 && p == first && mode != -1) {
            value = p->endPitch;
        } else {
            value = p->pitch;
        }
        if (value == 0 && p[-1].kind != 0) {
            s16 after = p[1].pitch;

            value = after
                    + (p[-1].length + p->length) * (p[-1].pitch - after)
                          / (p[1].length + (p[-1].length + p->length * 2));
        }
        out->kind = p->kind;
        out->length = lengths[i + 1];
        if (p->kind >= 1 && p->kind <= 6 && p->vowelMark == 'U') {
            out->pitch = -value;
        } else {
            out->pitch = value;
        }
        out->gain = p->gain;
        out->first = unit->bounds[i];
        out->last = unit->bounds[i + 1] - 1;
        i++;
        out->unit = unit->firstFrame;
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

ShttsUnit *ShttsUnit_Find(int *index, Shtts *engine) {
    ShttsUnit *unit = &ShttsUnits[engine->cur.unitSet][index[0]][index[1]][index[2]];

    if (unit->exists == 0) {
        return NULL;
    }
    return unit;
}

void ShttsOutput_Reset(Shtts *engine) {
    engine->mixStart = engine->mixBuffer;
    engine->mixEnd = engine->mixBuffer + 0x1E0;
    engine->mixSize = 0x1E0;
    engine->mixHalfSize = 0xF0;
    engine->mixHalf = engine->mixStart;
    engine->mixWaveEnd = engine->mixPos = engine->mixStart;
    engine->mixedCount = 0;
    ShttsOutput_ClearSamples(engine->mixSize, engine->mixStart, engine);
}

// Fills count samples with silence from a place in the sample buffer, going round to its start at its end
void ShttsOutput_ClearSamples(int count, s16 *buffer, Shtts *engine) {
    s16 i;

    if (count < 0) {
        ShttsUtil_ReportError(-1);
    }
    i = 0;
    if (buffer + count >= engine->mixEnd) {
        while (buffer < engine->mixEnd) {
            *buffer++ = 0;
            i++;
        }
        buffer = engine->mixStart;
    }
    for (; i < count; i++) {
        *buffer++ = 0;
    }
}

// Moves the place in the sample buffer on by count samples. When that leaves a half of the buffer, plays the half
// out and fills it with silence.
void ShttsOutput_Advance(int count, Shtts *engine) {
    if (count < 0) {
        return;
    }
    engine->mixPos += count;
    engine->mixedCount += count;
    if (engine->mixHalf == &engine->mixBuffer[0] && engine->mixPos >= &engine->mixBuffer[0xF0]) {
        ShttsOutput_SendBlock(engine->mixHalfSize, &engine->mixBuffer[0], engine);
        ShttsOutput_ClearSamples(engine->mixHalfSize, &engine->mixBuffer[0], engine);
        engine->mixHalf = &engine->mixBuffer[0xF0];
    } else if (engine->mixHalf == &engine->mixBuffer[0xF0]) {
        if ((engine->mixPos >= &engine->mixBuffer[0] && engine->mixPos < &engine->mixBuffer[0xF0])
            || engine->mixPos > &engine->mixBuffer[0x1E0]) {
            ShttsOutput_SendBlock(engine->mixHalfSize, &engine->mixBuffer[0xF0], engine);
            ShttsOutput_ClearSamples(engine->mixHalfSize, &engine->mixBuffer[0xF0], engine);
            engine->mixHalf = &engine->mixBuffer[0];
        }
    }
    if (engine->mixPos >= engine->mixEnd) {
        engine->mixPos -= 0x1E0;
    }
}

// Plays out the half of the sample buffer that has been filled, and moves on to the other half
void ShttsOutput_Flush(Shtts *engine) {
    if (engine->mixHalf >= engine->mixPos) {
        return;
    }
    ShttsOutput_SendBlock(engine->mixHalfSize, engine->mixHalf, engine);
    ShttsOutput_ClearSamples(engine->mixHalfSize, engine->mixHalf, engine);
    if (engine->mixHalf == &engine->mixBuffer[0xF0]) {
        engine->mixHalf = &engine->mixBuffer[0];
    } else {
        engine->mixHalf = &engine->mixBuffer[0xF0];
    }
    engine->mixPos = engine->mixHalf;
}

// Adds the samples of addWave to the sample buffer, going round to its start at its end and keeping the sums
// within 16 bits. They go at the buffer's place, or half their count before it if both a and b are above 0.
void ShttsOutput_AddWave(int a, int b, Shtts *engine) {
    s16 *dst;
    s16 i;
    s16 count = engine->addWave[0];
    s16 *src;

    if (a > 0 && b > 0) {
        s16 half = count >> 1;

        dst = engine->mixPos - half;
    } else {
        dst = engine->mixPos;
    }
    src = &engine->addWave[1];
    if (dst < engine->mixStart) {
        dst = engine->mixEnd - (engine->mixStart - dst);
    }
    i = 0;
    if (dst + count >= engine->mixEnd) {
        while (dst < engine->mixEnd) {
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
        dst = engine->mixStart;
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
        engine->mixWaveEnd = engine->mixPos + b;
    } else {
        engine->mixWaveEnd = dst;
    }
    if (engine->mixWaveEnd >= engine->mixEnd) {
        engine->mixWaveEnd = engine->mixStart + (engine->mixWaveEnd - engine->mixEnd);
    }
}

// Makes the samples of count frames, each with its value of the contour, and puts them in the sample buffer.
// Stops early if the engine has been asked to stop.
void ShttsSynth_SynthesizeFrames(ShttsFrameRef **units, s16 *pitches, s16 count, Shtts *engine) {
    s16 i;
    int length;
    int step;

    for (i = 0; i < count; i++) {
        if ((*units)->phoneme != engine->phoneme && (*units)->phoneme != 0) {
            engine->phoneme = (*units)->phoneme;
        }
        ShttsSynth_PrepareFrame((void *)*units++, *pitches++, engine);
        engine->samplesOwed += engine->frameLength;
        length = ShttsSynth_GetPitchPeriod(engine);
        step = length;
        if (length == 0) {
            step = engine->frameLength;
        }
        while (engine->samplesOwed > 0) {
            ShttsSynth_CopyFrameWave(engine);
            if (length == 0) {
                ShttsSynth_FadeWave(0, engine);
                ShttsOutput_AddWave(0, step, engine);
            } else {
                ShttsSynth_FadeWave(length, engine);
                ShttsOutput_AddWave(0, length, engine);
            }
            engine->samplesOwed -= step;
            ShttsOutput_Advance(step, engine);
        }
        if (ShttsEngine_IsStopRequested(engine) == 1) {
            return;
        }
    }
}

// Sets the caller's voice settings back to their defaults
void ShttsParam_SetDefaults(Shtts *engine) {
    engine->set.unitSet = 0;
    engine->set.pitch = 0x13C2;
    engine->set.accent = 100;
    engine->set.phrase = 100;
    engine->set.speed = 100;
    engine->set.volume = 0;
    engine->set.warp = 100;
    engine->set.warpScale = 0x400;
    engine->set.warpOrder = 10;
    engine->set.accentRule = 0;
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
void ShttsParam_UseSetParams(Shtts *engine) {
    engine->cur.unitSet = engine->set.unitSet;
    engine->cur.pitch = engine->set.pitch;
    engine->cur.accent = engine->set.accent;
    engine->cur.phrase = engine->set.phrase;
    engine->cur.speed = engine->set.speed;
    engine->cur.volume = engine->set.volume;
    engine->cur.warp = engine->set.warp;
    engine->cur.warpScale = engine->set.warpScale;
    engine->cur.warpOrder = engine->set.warpOrder;
    engine->cur.accentRule = engine->set.accentRule;
    engine->cur.mode = engine->set.mode;
    engine->cur.echoDelay = engine->set.echoDelay;
    if (engine->cur.echoDelay != 0) {
        ShttsEffect_ResetEcho(engine);
    }
    engine->cur.echoGain = engine->set.echoGain;
    engine->cur.firGain = engine->set.firGain;
    engine->cur.firDelay = engine->set.firDelay;
    if (engine->cur.firDelay != 0 || engine->cur.firGain != 0) {
        ShttsEffect_ResetFir(engine);
    }
    engine->cur.ring = engine->set.ring;
    engine->cur.chorus = engine->set.chorus;
    engine->cur.pitchTrans = engine->set.pitchTrans;
}

// Sets the warp, 80 to 200, and the two values worked out from it.
void ShttsParam_StoreWarp(int warp, int live, Shtts *engine) {
    if (warp < 80) {
        warp = 80;
    }
    if (warp > 200) {
        warp = 200;
    }
    if (live) {
        engine->cur.warp = warp;
        engine->cur.warpScale = ((int)engine->cur.warp * 0x28F) >> 6;
        engine->cur.warpOrder = ShttsMath_DivideRounded(1000, (s16)warp);
    } else {
        engine->set.warp = warp;
        engine->set.warpScale = ((int)engine->set.warp * 0x28F) >> 6;
        engine->set.warpOrder = ShttsMath_DivideRounded(1000, (s16)warp);
    }
}

int ShttsEngine_SetSampleRate(Shtts *engine) {
    engine->sampleRate = 11000;
    engine->splitUnitAtConsonant = 0;
    engine->frameLength = engine->sampleRate / 200;
    return 0;
}

// Starts speaking the text: sets the state up, prepares the text, and synthesizes in a thread of its own, or in
// the caller's if runInCaller is set.
void ShttsEngine_Start(Shtts *engine) {
    ShttsEngine_SetSampleRate(engine);
    ShttsEngine_ResetSynthesis(engine);
    if (engine->cur.unitSet == 1) {
        ShttsParam_UseSetParams(engine);
        engine->cur.unitSet = 1;
    } else {
        ShttsParam_UseSetParams(engine);
    }
    if ((engine->useInputText = 1) != 0) {
        ShttsText_ExpandPresets(engine->inputText);
        ShttsText_ConvertToLines(engine->inputText, engine->text, sizeof(engine->text));
    } else {
        ShttsText_ReadStdin(engine);
    }
    if (engine->runInCaller != 0) {
        ShttsEngine_RunThread(engine);
        return;
    }
    OS_CreateThread(&engine->thread, ShttsEngine_RunThread, engine, engine->stack + sizeof(engine->stack),
                    sizeof(engine->stack), engine->threadPriority);
    OS_WakeupThreadDirect(&engine->thread);
}

// The thread that synthesizes: takes the text a part at a time, makes phones and then segments of each and
// synthesizes them, until the text ends or the engine is asked to stop. Then tells the callback, with -1 if it was
// stopped and 0 if not.
void ShttsEngine_RunThread(void *arg) {
    Shtts *engine = arg;
    int count;
    ShttsThreadPhraseGroup *list;
    int added;

    ShttsOutput_Reset(engine);
    ShttsLine_SetCursor(engine->text, &engine->lineCursor);
    while (1) {
        if (engine->phonesFromStdin == 1) {
            if (ShttsPhone_ReadStdin((void *)&engine->phones[0], engine) == -1) {
                break;
            }
            engine->segmentCount = 0;
            ShttsUnit_SelectUnits(&engine->phones[1], engine->phraseGroupHead, engine);
            ShttsSynth_SynthesizeSegments(engine);
            continue;
        }
        if (ShttsWord_ReadWords((void *)engine->wordHead, engine->syllables, engine) == 0) {
            break;
        }
        ShttsWord_GroupWords((void *)engine->words, (void *)engine->accentGroupHead, (void *)engine->phraseGroupHead);
        ShttsDuration_ProcessGroups((void *)engine->accentGroups, engine);
        ShttsPitch_ShapeGroups((void *)&engine->phraseGroups, engine);
        count = 0;
        for (list = &engine->phraseGroups; list != NULL; list = list->next) {
            added = ShttsPhone_BuildFromSyllables((void *)list, (void *)&engine->phones[count], 0x100 - count);
            count += added;
            if (list->next == NULL || (added != 0 && engine->phones[count].kind == 0)) {
                ShttsPhone_ShapeEnding((void *)engine->phones, count, list->items[list->count - 1].last[7], engine);
                engine->segmentCount = 0;
                count = 0;
                ShttsUnit_SelectUnits(&engine->phones[1], list, engine);
                ShttsEffect_InitChorus(&engine->chorusState, engine->cur.chorus);
                ShttsSynth_SynthesizeSegments(engine);
                if (ShttsEngine_IsStopRequested(engine) == 1) {
                    break;
                }
            }
        }
        if (ShttsEngine_IsStopRequested(engine) == 1) {
            break;
        }
        ShttsOutput_Flush(engine);
        ShttsOutput_Reset(engine);
    }
    if (engine->callback != NULL) {
        if (ShttsEngine_IsStopRequested(engine) == 1) {
            engine->callback(-1, NULL, engine->callbackArg);
        } else {
            engine->callback(0, NULL, engine->callbackArg);
        }
    }
    engine->phoneme = 0;
    ShttsEngine_Lock(engine);
    engine->busy = 0;
    ShttsEngine_Unlock(engine);
    ShttsEngine_EndThread(engine);
}
