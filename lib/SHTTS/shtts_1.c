// The first part of the engine: text and word lists, the lengths of sounds, frames into samples.
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

int func_02084814(void *arg) {
    s16 count = func_020857e8(arg) - 2;

    if (count < 0) {
        return 0;
    }
    return count;
}

// Replaces each name of a preset in the text, such as "<\Spk_male>" or "<\Name=value>", by the preset's commands,
// each on a line of its own, followed by the value if there is one. The result is cut off at 0x400 characters.
void func_02084830(char *text) {
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
                if (strcmp(name, data_020b7fc8[k].name) == 0) {
                    int valueLength = strlen(value + 1);
                    int commandsLength = strlen(data_020b7fc8[k].commands);

                    length += commandsLength + valueLength;
                    if (length >= 0x400) {
                        goto done;
                    }
                    strcat(dst, data_020b7fc8[k].commands);
                    if (value[1] == '+') {
                        strcat(dst, value + 2);
                    } else {
                        strcat(dst, value + 1);
                    }
                    if (strcmp(data_020b7fc8[k].commands, "#p") == 0) {
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

// Gives each entry from first to last its length, and has func_020850e0 fill out with that many pointers to the
// entry's frames (entries first to last of the frames). Returns the sum of the lengths.
s16 func_02084c1c(ShttsUnk_02084c1c *first, ShttsUnk_02084c1c *last, ShttsUnk_020850e0 *frames,
                  ShttsUnk_020850e0 **out, Shtts *engine) {
    s16 i;
    s16 a = 0;
    s16 b;
    s16 count;
    s16 total;
    s16 length;
    s16 frameCount;
    s16 result;
    s8 next;
    ShttsUnk_02084c1c *p;

    if (engine->unk_0EE0E == -1 || engine->unk_0EE0E == 2) {
        engine->unk_0B72D = -1;
    } else {
        engine->unk_0B72D = 1;
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
    engine->unk_08538 = 0;
    result = 0;
    next = first[1].phoneme;
    for (i = 0; i < count; i++, first++) {
        s16 n;
        int phoneme;

        length = first->length;
        if (length < 0) {
            continue;
        }
        if (i + 1 < count && first[1].first == first[1].last && (data_020bc22c[first->phoneme] & 0x80)) {
            i++;
            first++;
            length += first->length;
        }
        if (engine->unk_0EE0E == 0 && (data_020bc22c[next] & 0x56) == 0x40) {
            s16 x;

            x = first[1].first != first[1].last ? first[1].last - first[1].first + 1
                : first[2].first != first[2].last ? first[2].last - first[2].first + 1
                : first[3].first != first[3].last ? first[3].last - first[3].first + 1 : 0;
            if (frameCount > total) {
                if (i == 0 && count > 1) {
                    engine->unk_0B72D = 1;
                    length = (length + ((length + first[1].length) * (first->last - first->first + 1)) /
                                           (first->last - first->first + 1 + x) + 1) >> 1;
                    a = length;
                    if (length < 0) {
                        length = first->length;
                    }
                } else if (i > 0) {
                    if (i > 1 && b != 0) {
                        engine->unk_0B72D = -1;
                        length = total - a - b;
                    } else {
                        engine->unk_0B72D = 0;
                        b = length + first[-1].length - a;
                        length = b;
                    }
                }
            } else {
                if (i == 0 && count > 1) {
                    engine->unk_0B72D = 1;
                    if (data_020bc22c[first[1].phoneme] & 0x40) {
                        length = first->length + ((first[1].length - x) >> 1);
                        a = length;
                    } else {
                        a = length;
                    }
                } else if (i > 0) {
                    if (i > 1 && b != 0) {
                        engine->unk_0B72D = -1;
                        length = total - b - a;
                    } else if (data_020bc22c[first->phoneme] & 0x40) {
                        engine->unk_0B72D = 0;
                        b = first->last - first->first + 1;
                        length = b;
                    } else {
                        engine->unk_0B72D = -1;
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
            s8 flags = data_020bc22c[phoneme];

            engine->unk_0B72A = (flags & 0x10) == 0;
            engine->unk_0B728 = flags & 4;
            engine->unk_0B729 = flags & 8;
            engine->unk_0B72C = flags & 2;
            engine->unk_0B72B = flags & 0x80;
            if (engine->unk_08538 + length >= 0x9FF) {
                length = 0x9FF - engine->unk_08538;
            }
            if (length <= 0) {
                length = 0;
            } else {
                engine->unk_08538 += length;
                func_020850e0(frames, n, length, out, engine);
            }
        }
        first->length = length;
        frames += first->last - first->first + 1;
        out += length;
        result += length;
        engine->unk_0B72D = -1;
    }
    return result;
}

// Fills dst with dstCount pointers to the srcCount entries at src, repeating or leaving out entries
void func_020850e0(ShttsUnk_020850e0 *src, s16 srcCount, s16 dstCount, ShttsUnk_020850e0 **dst, Shtts *engine) {
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
    if (engine->unk_0B72B != 0 || engine->unk_0B728 != 0 || engine->unk_0B729 != 0) {
        diff = dstCount - srcCount;
        if (diff < 0) {
            if (engine->unk_0B72D == 1 && engine->unk_0B72B != 0 && engine->unk_0EE0E == -1 && -diff <= dstCount) {
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
            if (engine->unk_0B72D == -1 && engine->unk_0B72B != 0 && engine->unk_0EE0E == -1 && -diff <= dstCount) {
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
        if (engine->unk_0B72D == 1 && engine->unk_0B72B != 0) {
            for (i = 0; i < diff; i++) {
                *dst++ = src;
            }
            for (i = 0; i < srcCount; i++) {
                *dst++ = src;
                src++;
            }
            return;
        }
        if (engine->unk_0B72D == -1 && engine->unk_0B72B != 0) {
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
        if (src[i].unk_2 == 0) {
            first = i;
            break;
        }
    }
    if (first != -1) {
        int limit = srcCount - first;

        for (i = first; i < limit; i++) {
            if (src[i].unk_2 > 0) {
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
        if (engine->unk_0B72A == 0 && engine->unk_0B72C != 0) {
            if (n != 0) {
                for (i = 0; i < first; i++) {
                    *dst++ = src;
                    src++;
                }
            }
            for (i = 0; i < diff; i++) {
                *dst++ = data_020bc258;
            }
            dstCount -= diff;
            if (n >= 6) {
                for (i = 0; i < 6; i++) {
                    int v = (6 - i) * 40;

                    if (src[i].unk_0 > (s16)v) {
                        src[i].unk_0 -= v;
                    } else {
                        src[i].unk_0 = 0;
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

// A chorus: gives for each sample the mean of two earlier ones, at delays that sweep up and down and add up to 255
void func_02085704(ShttsUnk_020856b0 *unk, s16 *in, s16 *out, int count) {
    int i;

    for (i = 0; i < count; i++) {
        int delay;

        unk->unk_000[unk->unk_204] = in[i];
        out[i] = 0;
        delay = unk->unk_208;
        if (delay >= 0x100) {
            delay = 0x1FF - delay;
        }
        out[i] += unk->unk_000[(unk->unk_204 - delay) & 0xFFu] >> 1;
        delay = 0xFF - delay;
        out[i] += unk->unk_000[(unk->unk_204 - delay) & 0xFFu] >> 1;
        unk->unk_204++;
        if (unk->unk_204 >= 0x100) {
            unk->unk_204 = 0;
        }
        unk->unk_20C += 0x20;
        if (unk->unk_20C >= unk->unk_200) {
            unk->unk_20C = 0;
            unk->unk_208++;
            if (unk->unk_208 >= 0x1FF) {
                unk->unk_208 = 0;
            }
        }
    }
}

// The number of vowels, 'N's, 'Q's and '-'s in text
int func_020857e8(const char *text) {
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

// Has func_020894cc fill unk_0853A from its second entry on, works out unk_0871E (0 to 90) and unk_0871C (0 to 239),
// scales what was filled in by twice the entry of data_020b9b04 that unk_0871E picks, and keeps the count first
int func_020858c8(ShttsUnk_020858c8 *unk, int b, Shtts *engine) {
    s16 count;
    int gain;

    engine->unk_08720 = func_020894cc(unk, engine->unk_0853A, engine);
    if (unk->unk_3 == (s8)engine->unk_00004 || unk->unk_3 == 0) {
        b = 0;
    }
    engine->unk_0871E = (unk->unk_0 * 0xCCD + 0x4000) >> 15;
    if (engine->unk_0871E > 90) {
        engine->unk_0871E = 90;
    }
    if (engine->unk_0871E < 0) {
        engine->unk_0871E = 0;
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
        engine->unk_0871C = (data_020b9aa0[index] * data_020b9bbc[fraction]) >> 16;
        engine->unk_0871C = (engine->unk_00000 * engine->unk_0871C) >> 16;
        engine->unk_0871C = (engine->unk_0871C * 0x3EE2) >> 16;
    } else {
        engine->unk_0871C = b;
    }
    if (engine->unk_0871C < 0) {
        engine->unk_0871C = 0;
    } else if (engine->unk_0871C >= 240) {
        engine->unk_0871C = 239;
    }
    count = engine->unk_08720;
    if (count < 0) {
        count = -count;
    }
    gain = data_020b9b04[engine->unk_0871E] * 2;
    if (count > 0) {
        func_02089e74(&engine->unk_0853A[1], &engine->unk_0853A[1], count, gain);
    }
    engine->unk_0853A[0] = count;
    return 1;
}

int func_02085a64(Shtts *engine) {
    return engine->unk_0871C;
}

// Copies unk_0853A to unk_08724, or sets the first of unk_08724 to 0
void func_02085a70(Shtts *engine) {
    s16 i;

    if (engine->unk_08720 > 0 && engine->unk_0871C > 0) {
        for (i = 0; i <= engine->unk_08720; i++) {
            engine->unk_08724[i] = engine->unk_0853A[i];
        }
    } else if (engine->unk_08720 < 0 || engine->unk_0871C == 0) {
        for (i = 0; i <= engine->unk_0853A[0]; i++) {
            engine->unk_08724[i] = engine->unk_0853A[i];
        }
    } else {
        engine->unk_08724[0] = 0;
    }
}

// Walks the list in runs: a run ends at the entry whose count is 0 or whose unk_8->unk_12 is not 0. For each run it
// adds up func_020857e8 of the texts, and then calls func_02085d80 for each B of each entry with a list of pointers:
// data_020b9cbc for the entry's first B and otherwise where the walk of the B before ended, then the B's units, then
// the first unit of the next B, or data_020b9cbc if there is none.
void func_02085b30(ShttsUnk_02085b30_A *list, Shtts *engine) {
    ShttsUnk_02085b30_C *units[98];
    char unk = 0x1A;
    ShttsUnk_02085b30_A *last = list;
    ShttsUnk_02085b30_A *first = list;
    ShttsUnk_02085b30_A *a;
    ShttsUnk_02085b30_B *b;
    ShttsUnk_02085b30_C *c;
    ShttsUnk_02085b30_C **p;
    s16 total;
    s16 i;
    s8 isFirst;
    s8 isLast;
    int position;

    if (list == NULL) {
        return;
    }
    do {
        total = 0;
        b = first->first;
        for (i = 0; i < first->count; i++) {
            total += func_020857e8(b->text);
            b = b->next;
        }
        while (last->count != 0 && last->unk_8->unk_12 == 0) {
            last = last->next;
            for (i = 0; i < last->count; i++) {
                total += func_020857e8(b->text);
                b = b->next;
            }
        }
        for (a = first; a <= last; a++) {
            c = &data_020b9cbc;
            b = a->first;
            for (i = 0; i < a->count; i++, b = b->next) {
                s16 k;

                p = units;
                *p++ = c;
                c = b->unk_18;
                for (k = 0; k < b->count; k++, c = c->next) {
                    *p++ = c;
                }
                if (b->next != NULL) {
                    if (b->next->unk_18 != NULL) {
                        *p = b->next->unk_18;
                    } else {
                        *p = &data_020b9cbc;
                    }
                } else {
                    *p = &data_020b9cbc;
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
                func_02085d80((void *)units, b->count, total, isFirst, isLast, position, &unk, b->unk_16, engine);
            }
        }
        last = last->next;
        first = last;
    } while (last != NULL);
}
