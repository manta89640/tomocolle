// The first part of the engine: text and word lists, the lengths of sounds, frames into samples.
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

// Compares two strings, taking 'I' and 'U' for 'i' and 'u'
int ShttsDict_CompareWords(const char *a, const char *b) {
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

// Looks a word up in a sorted list. If it is there, writes it back as the list spells it and returns its value.
int ShttsDict_FindWord(char *text, ShttsDict *list) {
    int low = 0;
    int high = list->count - 1;

    while (low <= high) {
        int middle = (low + high) >> 1;
        s16 order = ShttsDict_CompareWords(text, list->words[middle].name);

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

int ShttsWord_GetDefaultAccent(void *arg) {
    s16 count = ShttsWord_CountMorae(arg) - 2;

    if (count < 0) {
        return 0;
    }
    return count;
}

// Replaces each name of a preset in the text, such as "<\Spk_male>" or "<\Name=value>", by the preset's commands,
// each on a line of its own, followed by the value if there is one. The result is cut off at 0x400 characters.
void ShttsText_ExpandPresets(char *text) {
    char pending = 0;
    char one[2] = {0};
    char out[0x400];
    int n;
    char *dst = out;
    int length = 0;
    int textLength;
    int i;

    memset(dst, 0, sizeof(out));
    textLength = strlen(text);
    for (i = 0; i < textLength; i++) {
        char name[30] = {0};
        char value[10] = {0};
        n = 0;

        if (text[i] == '<' && text[i + 1] == '\\') {
            int start = i + 1;
            int k;

            if (pending == 1) {
                length += strlen("\n");
                if (length >= 0x400) {
                    break;
                }
                strcat(dst, "\n");
                pending = 0;
            }
            while (text[i] != '>' && text[i] != '=') {
                name[n] = text[i];
                if (n > 20 || i == strlen(text) - 1 || (text[i] == '<' && text[i + 1] == '\\' && n != 0)) {
                    i = start;
                    memset(name, 0, sizeof(name));
                    break;
                }
                n++;
                i++;
            }
            n = 0;
            if (text[i] == '=') {
                while (text[i] != '>') {
                    value[n] = text[i];
                    if (n > 7 || i == strlen(text) - 1 || (text[i] == '<' && text[i + 1] == '\\')) {
                        i = start;
                        memset(name, 0, sizeof(name));
                        break;
                    }
                    i++;
                    n++;
                }
            }
            for (k = 0; k < SHTTS_PRESET_COUNT; k++) {
                if (strcmp(name, ShttsTextPresets[k].name) == 0) {
                    int valueLength = strlen(value + 1);
                    int commandsLength = strlen(ShttsTextPresets[k].commands);

                    length += commandsLength + valueLength;
                    if (length >= 0x400) {
                        goto done;
                    }
                    strcat(dst, ShttsTextPresets[k].commands);
                    if (value[1] == '+') {
                        strcat(dst, value + 2);
                    } else {
                        strcat(dst, value + 1);
                    }
                    if (strcmp(ShttsTextPresets[k].commands, "#p") == 0) {
                        length += strlen("\\");
                        if (length >= 0x400) {
                            goto done;
                        }
                        strcat(dst, "\\");
                    } else {
                        length += strlen("\n#q");
                        if (length >= 0x400) {
                            goto done;
                        }
                        strcat(dst, "\n#q");
                    }
                    length += strlen("\n");
                    if (length >= 0x400) {
                        goto done;
                    }
                    strcat(dst, "\n");
                    break;
                }
            }
        } else {
            strncpy(one, text + i, 1);
            length += strlen(one);
            if (length >= 0x400) {
                break;
            }
            strcat(dst, one);
            pending = 1;
        }
    }
done:
    strcpy(text, out);
}

// Gives each entry from first to last its length, and has ShttsSynth_StretchFrames fill out with that many pointers to the
// entry's frames (entries first to last of the frames). Returns the sum of the lengths.
s16 ShttsSynth_StretchSegments(ShttsStretchSegment *first, ShttsStretchSegment *last, ShttsStretchFrame *frames,
                  ShttsStretchFrame **out, Shtts *engine) {
    s16 i;
    s16 a = 0;
    s16 b;
    s16 count;
    s16 total;
    s16 length;
    s16 frameCount;
    s16 result;
    s8 next;
    ShttsStretchSegment *p;

    if (engine->runKind == -1 || engine->runKind == 2) {
        engine->stretchSide = -1;
    } else {
        engine->stretchSide = 1;
    }
    count = 0;
    p = first;
    total = 0;
    for (; p <= last; p++, count++) {
        total += p->length;
    }
    if (count == 0) {
        return 0;
    }
    frameCount = last->last - first->first + 1;
    b = 0;
    engine->framePointerCount = 0;
    result = 0;
    next = first[1].phoneme;
    for (i = 0; i < count; i++, first++) {
        s16 n;
        int phoneme;

        length = first->length;
        if (length < 0) {
            continue;
        }
        if (i + 1 < count && first[1].first == first[1].last && (ShttsPhonemeFlags[first->phoneme] & 0x80)) {
            i++;
            first++;
            length += first->length;
        }
        if (engine->runKind == 0 && (ShttsPhonemeFlags[next] & 0x56) == 0x40) {
            s16 x;

            x = first[1].first != first[1].last ? first[1].last - first[1].first + 1
                : first[2].first != first[2].last ? first[2].last - first[2].first + 1
                : first[3].first != first[3].last ? first[3].last - first[3].first + 1 : 0;
            if (frameCount > total) {
                if (i == 0 && count > 1) {
                    engine->stretchSide = 1;
                    length = (length + ((length + first[1].length) * (first->last - first->first + 1)) /
                                           (first->last - first->first + 1 + x) + 1) >> 1;
                    a = length;
                    if (length < 0) {
                        length = first->length;
                    }
                } else if (i > 0) {
                    if (i > 1 && b != 0) {
                        engine->stretchSide = -1;
                        length = total - a - b;
                    } else {
                        engine->stretchSide = 0;
                        b = length + first[-1].length - a;
                        length = b;
                    }
                }
            } else {
                if (i == 0 && count > 1) {
                    engine->stretchSide = 1;
                    if (ShttsPhonemeFlags[first[1].phoneme] & 0x40) {
                        length = first->length + ((first[1].length - x) >> 1);
                        a = length;
                    } else {
                        a = length;
                    }
                } else if (i > 0) {
                    if (i > 1 && b != 0) {
                        engine->stretchSide = -1;
                        length = total - b - a;
                    } else if (ShttsPhonemeFlags[first->phoneme] & 0x40) {
                        engine->stretchSide = 0;
                        b = first->last - first->first + 1;
                        length = b;
                    } else {
                        engine->stretchSide = -1;
                        length = total - a;
                    }
                }
            }
        }
        n = first->last - first->first + 1;
        phoneme = first->phoneme;
        if (n < 1) {
            length = 0;
        } else {
            s8 flags = ShttsPhonemeFlags[phoneme];

            engine->phonemeNoFlag10 = (flags & 0x10) == 0;
            engine->phonemeFlag04 = flags & 4;
            engine->phonemeFlag08 = flags & 8;
            engine->phonemeFlag02 = flags & 2;
            engine->phonemeFlag80 = flags & 0x80;
            if (engine->framePointerCount + length >= 0x9FF) {
                length = 0x9FF - engine->framePointerCount;
            }
            if (length <= 0) {
                length = 0;
            } else {
                engine->framePointerCount += length;
                ShttsSynth_StretchFrames(frames, n, length, out, engine);
            }
        }
        first->length = length;
        frames += first->last - first->first + 1;
        out += length;
        result += length;
        engine->stretchSide = -1;
    }
    return result;
}

// Fills dst with dstCount pointers to the srcCount entries at src, repeating or leaving out entries
void ShttsSynth_StretchFrames(ShttsStretchFrame *src, s16 srcCount, s16 dstCount, ShttsStretchFrame **dst, Shtts *engine) {
    s16 i;
    s16 j;
    s16 diff;
    s16 n;
    s16 first;

    if (dstCount == srcCount) {
        for (i = 0; i < dstCount; i++) {
            *dst++ = src;
            src++;
        }
        return;
    }
    if (engine->phonemeFlag80 != 0 || engine->phonemeFlag04 != 0 || engine->phonemeFlag08 != 0) {
        diff = dstCount - srcCount;
        if (diff < 0) {
            if (engine->stretchSide == 1 && engine->phonemeFlag80 != 0 && engine->runKind == -1 && -diff <= dstCount) {
                for (i = 0; i < dstCount; i++) {
                    *dst++ = src;
                    src++;
                    if (diff < 0) {
                        src++;
                        diff++;
                        if (diff < 0) {
                            src++;
                            diff++;
                        }
                    }
                }
                return;
            }
            if (engine->stretchSide == -1 && engine->phonemeFlag80 != 0 && engine->runKind == -1 && -diff <= dstCount) {
                dst += dstCount - 1;
                src += srcCount - 1;
                for (; dstCount > 0; dstCount--) {
                    *dst-- = src;
                    src--;
                    if (diff < 0) {
                        src--;
                        diff++;
                        if (diff < 0) {
                            src--;
                            diff++;
                        }
                    }
                }
                return;
            }
            i = 0;
            j = 1;
            while (i < dstCount) {
                while (j * dstCount <= i * srcCount && j < srcCount) {
                    src++;
                    j++;
                }
                *dst++ = src;
                i++;
            }
            return;
        }
        if (engine->stretchSide == 1 && engine->phonemeFlag80 != 0) {
            for (i = 0; i < diff; i++) {
                *dst++ = src;
            }
            for (i = 0; i < srcCount; i++) {
                *dst++ = src;
                src++;
            }
            return;
        }
        if (engine->stretchSide == -1 && engine->phonemeFlag80 != 0) {
            for (i = 0; i < srcCount; i++) {
                *dst++ = src;
                src++;
            }
            for (i = 0; i < diff; i++) {
                *dst++ = src - 1;
            }
            return;
        }
        j = 1;
        for (i = 0; i < dstCount; i++) {
            if (j * dstCount <= i * srcCount) {
                src++;
                j++;
            }
            *dst++ = src;
        }
        return;
    }
    n = 0;
    first = -1;
    for (i = 0; i < srcCount; i++) {
        if (src[i].voiced == 0) {
            first = i;
            break;
        }
    }
    if (first != -1) {
        int limit = srcCount - first;

        for (i = first; i < limit; i++) {
            if (src[i].voiced > 0) {
                break;
            }
        }
        n = i - first;
    }
    diff = dstCount - srcCount;
    if (diff < 0) {
        if (n != 0) {
            for (i = 0; i < first; i++) {
                *dst++ = src;
                src++;
            }
        }
        for (; diff < 0; diff++) {
            src++;
        }
    } else if (diff > 0) {
        if (engine->phonemeNoFlag10 == 0 && engine->phonemeFlag02 != 0) {
            if (n != 0) {
                for (i = 0; i < first; i++) {
                    *dst++ = src;
                    src++;
                }
            }
            for (i = 0; i < diff; i++) {
                *dst++ = ShttsFillerFrame;
            }
            dstCount -= diff;
            if (n >= 6) {
                for (i = 0; i < 6; i++) {
                    int v = (6 - i) * 40;

                    if (src[i].gain > (s16)v) {
                        src[i].gain -= v;
                    } else {
                        src[i].gain = 0;
                    }
                }
            }
        } else {
            if (n != 0) {
                for (i = 0; i < n; i++) {
                    *dst++ = src;
                    src++;
                }
                srcCount -= i;
                dstCount -= i;
            }
            diff = dstCount - srcCount;
            if (diff > srcCount) {
                for (i = 0; i < diff - srcCount; i++) {
                    *dst++ = src;
                }
                dstCount = srcCount * 2;
            }
            diff = dstCount - srcCount;
            for (i = 0; i < diff; i++) {
                *dst++ = src;
                *dst++ = src;
                src++;
            }
            dstCount = srcCount * 2 - dstCount;
        }
    }
    for (i = 0; i < dstCount; i++) {
        *dst++ = src;
        src++;
    }
}

void ShttsEffect_InitChorus(ShttsChorus *chorus, int rate) {
    if (rate <= 0) {
        rate = 1;
    }
    chorus->sweepPeriod = ((0x12C00 / rate) << 5) / 256;
    if (chorus->sweepPeriod == 0) {
        chorus->sweepPeriod = 1;
    }
    chorus->writePos = 0;
    chorus->sweepPos = 0;
    chorus->sweepTimer = 0;
    memset(chorus->ring, 0, sizeof(chorus->ring));
}

// A chorus: gives for each sample the mean of two earlier ones, at delays that sweep up and down and add up to 255
void ShttsEffect_ApplyChorus(ShttsChorus *chorus, s16 *in, s16 *out, int count) {
    int i;

    for (i = 0; i < count; i++) {
        int delay;

        chorus->ring[chorus->writePos] = in[i];
        out[i] = 0;
        delay = chorus->sweepPos;
        if (delay >= 0x100) {
            delay = 0x1FF - delay;
        }
        out[i] += chorus->ring[(chorus->writePos - delay) & 0xFFu] >> 1;
        delay = 0xFF - delay;
        out[i] += chorus->ring[(chorus->writePos - delay) & 0xFFu] >> 1;
        chorus->writePos++;
        if (chorus->writePos >= 0x100) {
            chorus->writePos = 0;
        }
        chorus->sweepTimer += 0x20;
        if (chorus->sweepTimer >= chorus->sweepPeriod) {
            chorus->sweepTimer = 0;
            chorus->sweepPos++;
            if (chorus->sweepPos >= 0x1FF) {
                chorus->sweepPos = 0;
            }
        }
    }
}

// The number of vowels, 'N's, 'Q's and '-'s in text
int ShttsWord_CountMorae(const char *text) {
    s16 count;

    if (text == NULL) {
        return 0;
    }
    count = 0;
    while (*text != 0) {
        switch (*text) {
        case '-':
        case 'A':
        case 'E':
        case 'I':
        case 'N':
        case 'O':
        case 'Q':
        case 'U':
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':
            count++;
            break;
        }
        text++;
    }
    return count;
}

// Has ShttsSynth_DecodeFrame fill frameWave from its second entry on, works out gainIndex (0 to 90) and pitchPeriod (0 to 239),
// scales what was filled in by twice the entry of ShttsGainLevels that gainIndex picks, and keeps the count first
int ShttsSynth_PrepareFrame(ShttsPreparedFrame *frame, int b, Shtts *engine) {
    s16 count;
    int gain;

    engine->frameWaveCount = ShttsSynth_DecodeFrame(frame, engine->frameWave, engine);
    if (frame->sampleCount == (s8)engine->frameLength || frame->sampleCount == 0) {
        b = 0;
    }
    engine->gainIndex = (frame->gain * 0xCCD + 0x4000) >> 15;
    if (engine->gainIndex > 90) {
        engine->gainIndex = 90;
    }
    if (engine->gainIndex < 0) {
        engine->gainIndex = 0;
    }
    if (b > 0) {
        s16 index = (b >> 7) - 28;
        s16 fraction;

        if (index >= 50) {
            index = 49;
        }
        if (index < 0) {
            index = 0;
        }
        fraction = b & 0x7F;
        engine->pitchPeriod = (ShttsPitchPeriodCoarse[index] * ShttsPitchPeriodFine[fraction]) >> 16;
        engine->pitchPeriod = (engine->sampleRate * engine->pitchPeriod) >> 16;
        engine->pitchPeriod = (engine->pitchPeriod * 0x3EE2) >> 16;
    } else {
        engine->pitchPeriod = b;
    }
    if (engine->pitchPeriod < 0) {
        engine->pitchPeriod = 0;
    } else if (engine->pitchPeriod >= 240) {
        engine->pitchPeriod = 239;
    }
    count = engine->frameWaveCount;
    if (count < 0) {
        count = -count;
    }
    gain = ShttsGainLevels[engine->gainIndex] * 2;
    if (count > 0) {
        ShttsMath_ScaleSamples(&engine->frameWave[1], &engine->frameWave[1], count, gain);
    }
    engine->frameWave[0] = count;
    return 1;
}

int ShttsSynth_GetPitchPeriod(Shtts *engine) {
    return engine->pitchPeriod;
}

// Copies frameWave to addWave, or sets the first of addWave to 0
void ShttsSynth_CopyFrameWave(Shtts *engine) {
    s16 i;

    if (engine->frameWaveCount > 0 && engine->pitchPeriod > 0) {
        for (i = 0; i <= engine->frameWaveCount; i++) {
            engine->addWave[i] = engine->frameWave[i];
        }
    } else if (engine->frameWaveCount < 0 || engine->pitchPeriod == 0) {
        for (i = 0; i <= engine->frameWave[0]; i++) {
            engine->addWave[i] = engine->frameWave[i];
        }
    } else {
        engine->addWave[0] = 0;
    }
}

// Walks the list in runs: a run ends at the entry whose count is 0 or whose last->pause is not 0. For each run it
// adds up ShttsWord_CountMorae of the texts, and then calls ShttsDuration_SetWordSyllables for each B of each entry with a list of pointers:
// ShttsBoundarySyllable for the entry's first B and otherwise where the walk of the B before ended, then the B's units, then
// the first unit of the next B, or ShttsBoundarySyllable if there is none.
void ShttsDuration_ProcessGroups(ShttsDurationAccentGroup *list, Shtts *engine) {
    ShttsDurationSyllable *units[98];
    char prevVowel = 0x1A;
    ShttsDurationAccentGroup *last = list;
    ShttsDurationAccentGroup *first = list;
    ShttsDurationAccentGroup *a;
    ShttsDurationWord *b;
    ShttsDurationSyllable *c;
    ShttsDurationSyllable **p;
    s16 total;
    s16 i;
    s8 isFirst;
    s8 isLast;
    s8 position;

    if (list == NULL) {
        return;
    }
    do {
        total = 0;
        b = first->first;
        for (i = 0; i < first->count; i++) {
            total += ShttsWord_CountMorae(b->text);
            b = b->next;
        }
        while (last->count != 0 && last->last->pause == 0) {
            last = last->next;
            for (i = 0; i < last->count; i++) {
                total += ShttsWord_CountMorae(b->text);
                b = b->next;
            }
        }
        for (a = first; a <= last; a++) {
            c = &ShttsBoundarySyllable;
            b = a->first;
            for (i = 0; i < a->count; i++, b = b->next) {
                s16 k;

                p = units;
                *p++ = c;
                c = b->first;
                for (k = 0; k < b->count; k++, c = c->next) {
                    *p++ = c;
                }
                if (b->next != NULL) {
                    if (b->next->first != NULL) {
                        *p = b->next->first;
                    } else {
                        *p = &ShttsBoundarySyllable;
                    }
                } else {
                    *p = &ShttsBoundarySyllable;
                }
                isFirst = a == first && i == 0;
                isLast = a == last && i == a->count - 1;
                if (a == list) {
                    position = 0;
                } else if (last->next == NULL) {
                    position = 2;
                } else {
                    position = 1;
                }
                engine->cur.speed = b->speed;
                ShttsDuration_SetWordSyllables((void *)units, b->count, total, isFirst, isLast, position, &prevVowel, b->volume, engine);
            }
        }
        last = last->next;
        first = last;
    } while (last != NULL);
}
