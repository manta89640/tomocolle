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
int func_02084c1c(ShttsUnk_02084c1c *first, ShttsUnk_02084c1c *last, ShttsUnk_020850e0 *frames,
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
                func_02085d80(units, b->count, total, isFirst, isLast, position, &unk, b->unk_16, engine);
            }
        }
        last = last->next;
        first = last;
    } while (last != NULL);
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

// Empties the echo's history
void func_02086814(Shtts *engine) {
    engine->unk_0E624 = 0;
    memset(engine->echoBuffer, 0, sizeof(engine->echoBuffer));
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

void func_02086a74(int value, int *out) {
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
// Not matching yet: 30 instructions differ, all of them register choices in the first loop, and the order of the
// two stores of `order` and `out` to the stack at the start; every instruction is otherwise the original's. The
// original keeps the address of `cosines` in r0, the counter in r2 (and its shifted next value in r7), the read
// value in r4, the table in r5 and the mask in r6; this has them in r7, r6, r3, r4 and r5. The other three loops
// match, and only do with exactly this sharing of variables (`t` for all three read values, `j` for the table index,
// the counter of the second loop and of the inner one) and the chained assignment in the second loop.
// Tried for the first loop without any change to it: every other sharing of its counter and temporaries with the
// other loops' variables (three exhaustive searches, about 1,400 builds); the order of declarations; int, s16 and
// u16 counters and `i++`, `++i`, `i += 1`, `i = (s16)(i + 1)`; while and do-while forms, with and without a guard
// and with the counter set before it; the body as one expression, with variables for the low entry, the difference
// and the sum, and with `*=`; an inline function for the lookup, for the loop, and for the whole body; a pointer
// to the element taken first; `order` as s16 or copied to a local; a const extern; 2D arrays. A local pointer to
// `cosines` or to the table does move the registers (so what is live across the loop decides them), but each takes
// a register the original gives to something else.
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
        cosines[i] = -(s16)(data_020bbf30[j] + ((((s16)(data_020bbf30[j + 1] - data_020bbf30[j]) * frac) << 1) >> 16));
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
    engine->unk_04938 = 0;
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

int func_02089cec(void) {
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

// The ring effect: multiplies samples by a sine wave of a frequency
void func_0208b6c0(int *phase, int count, int frequency, s16 *src, s16 *dst) {
    int i;
    int step = (frequency << 16) / 11000;

    for (i = 0; i < count; i++) {
        (*phase)++;
        dst[i] = (src[i] * data_020e8d08[((step * *phase) >> 8) & 0xFF]) >> 15;
    }
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
    engine->unk_0EEDC = unk;
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
