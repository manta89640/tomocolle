#include "text/TextUtil.h"

#include <wchar.h>

extern "C" {
// Reverses a wide string in place (in the C library's part of the main program, not named yet)
wchar_t *func_0201f544(wchar_t *text);
}

// How the speech engine reads the digits and the letters
static const wchar_t *sDigitReadings[10] = {
    L"ゼロ", L"イチ", L"ニ", L"サン", L"ヨ’ン", L"ゴ", L"ロク", L"ナナ", L"ハチ", L"キュ’ー",
};
static const wchar_t *sLetterReadings[26] = {
    L"エー",   L"ビー",     L"シー", L"ディー", L"イー",     L"エフ",     L"ジー", L"エイチ", L"アイ",
    L"ジェー", L"ケー",     L"エル", L"エム",   L"エヌ",     L"オー",     L"ピー", L"キュー", L"アール",
    L"エス",   L"ティー",   L"ユー", L"ブイ",   L"ダブリュ", L"エックス", L"ワイ", L"ゼット",
};

// Full-width characters
#define FULL_WIDTH_ZERO 0xFF10    // '０'
#define FULL_WIDTH_MINUS 0xFF0D   // The full-width minus
#define FULL_WIDTH_POINT 0xFF0E   // '．'
#define FULL_WIDTH_SPACE 0x3000
#define FULL_WIDTH_A 0xFF21       // 'Ａ'
#define FULL_WIDTH_SMALL_A 0xFF41 // 'ａ'

wchar_t *FormatNumber(int value, wchar_t *text, u32 size, int width, BOOL zeroPad) {
    MI_CpuClear16(text, size);
    int left = size / 2 - 1;
    BOOL negative;
    int length = 0;
    if (value < 0) {
        value = -value;
        left--;
        negative = TRUE;
    } else {
        negative = FALSE;
    }
    do {
        if (left <= 0) {
            break;
        }
        text[length] = FULL_WIDTH_ZERO + value % 10;
        length++;
        left--;
        width--;
    } while ((value /= 10) != 0);
    if (!zeroPad && negative) {
        text[length] = FULL_WIDTH_MINUS;
        length++;
    }
    while (width > 0) {
        if (left <= 0) {
            break;
        }
        if (zeroPad) {
            text[length++] = FULL_WIDTH_ZERO;
        } else {
            text[length++] = FULL_WIDTH_SPACE;
        }
        width--;
        left--;
    }
    if (zeroPad && negative) {
        text[length] = FULL_WIDTH_MINUS;
        length++;
    }
    text[length] = 0;
    func_0201f544(text);
    return text;
}

wchar_t *FormatNumberWithCommas(int value, wchar_t *text, u32 size, int width, BOOL zeroPad) {
    MI_CpuClear16(text, size);
    BOOL negative;
    int length = 0;
    int left = size / 2 - 1;
    int columns = width + width / 3;
    int position = 0;
    if (value < 0) {
        value = -value;
        left--;
        negative = TRUE;
    } else {
        negative = FALSE;
    }
    do {
        if (left <= 0) {
            break;
        }
        if (position % 4 != 3) {
            text[length] = FULL_WIDTH_ZERO + value % 10;
            length++;
            value /= 10;
        } else {
            text[length] = L',';
            length++;
        }
        left--;
        columns--;
        position++;
    } while (value != 0);
    if (!zeroPad && negative) {
        text[length] = FULL_WIDTH_MINUS;
        length++;
    }
    while (columns > 0) {
        if (left <= 0) {
            break;
        }
        if (position % 4 != 3) {
            if (zeroPad) {
                text[length++] = FULL_WIDTH_ZERO;
            } else {
                text[length++] = FULL_WIDTH_SPACE;
            }
        } else {
            if (zeroPad) {
                text[length++] = L',';
            } else {
                text[length++] = FULL_WIDTH_SPACE;
            }
        }
        columns--;
        left--;
        position++;
    }
    if (zeroPad && negative) {
        text[length] = FULL_WIDTH_MINUS;
        length++;
    }
    text[length] = 0;
    func_0201f544(text);
    return text;
}

wchar_t *FormatDecimal(int value, wchar_t *text, u32 size, int width, BOOL zeroPad, int pointPosition) {
    MI_CpuClear16(text, size);
    BOOL negative;
    int length = 0;
    int left = size / 2 - 1;
    if (value < 0) {
        value = -value;
        left--;
        negative = TRUE;
    } else {
        negative = FALSE;
    }
    int position = 0;
    do {
        if (left <= 0) {
            break;
        }
        if (position != pointPosition) {
            text[length] = FULL_WIDTH_ZERO + value % 10;
            length++;
            value /= 10;
            left--;
            width--;
        } else {
            text[length] = FULL_WIDTH_POINT;
            length++;
            left--;
            width--;
        }
        position++;
    } while (value != 0);
    if (!zeroPad && negative) {
        text[length] = FULL_WIDTH_MINUS;
        length++;
    }
    while (width > 0) {
        if (left <= 0) {
            break;
        }
        if (zeroPad) {
            text[length++] = FULL_WIDTH_ZERO;
        } else {
            text[length++] = FULL_WIDTH_SPACE;
        }
        width--;
        left--;
    }
    if (zeroPad && negative) {
        text[length] = FULL_WIDTH_MINUS;
        length++;
    }
    text[length] = 0;
    func_0201f544(text);
    return text;
}

void CopyToKatakana(wchar_t *dst, const wchar_t *src) {
    wcscpy(dst, src);
    ToKatakana(dst);
}

void ToKatakana(wchar_t *text) {
    u32 length = wcslen(text);
    for (u32 i = 0; i < length; i++) {
        if (text[i] >= 0x3041 && text[i] <= 0x3093) {
            text[i] += 0x30A1 - 0x3041;
        }
    }
}

void ReplaceSymbolsForSpeech(wchar_t *text) {
    u32 length = wcslen(text);
    for (u32 i = 0; i < length; i++) {
        wchar_t c = text[i];
        if (c == 0x3001 || c == 0x2022) {
            text[i] = 0xFF3F;
        } else if (c == 0x3002 || c == 0x2026) {
            text[i] = L'.';
        } else if (c == 0xFF5E) {
            text[i] = 0x30FC;
        } else if (c == 0x2606 || c == 0x2605 || c == 0xE017) {
            text[i] = L'!';
        }
    }
}

const wchar_t *GetLetterReading(wchar_t c) {
    int letter;
    switch (c) {
    case L'A': case L'a': case 0xFF21: case 0xFF41: letter = 0; break;
    case L'B': case L'b': case 0xFF22: case 0xFF42: letter = 1; break;
    case L'C': case L'c': case 0xFF23: case 0xFF43: letter = 2; break;
    case L'D': case L'd': case 0xFF24: case 0xFF44: letter = 3; break;
    case L'E': case L'e': case 0xFF25: case 0xFF45: letter = 4; break;
    case L'F': case L'f': case 0xFF26: case 0xFF46: letter = 5; break;
    case L'G': case L'g': case 0xFF27: case 0xFF47: letter = 6; break;
    case L'H': case L'h': case 0xFF28: case 0xFF48: letter = 7; break;
    case L'I': case L'i': case 0xFF29: case 0xFF49: letter = 8; break;
    case L'J': case L'j': case 0xFF2A: case 0xFF4A: letter = 9; break;
    case L'K': case L'k': case 0xFF2B: case 0xFF4B: letter = 10; break;
    case L'L': case L'l': case 0xFF2C: case 0xFF4C: letter = 11; break;
    case L'M': case L'm': case 0xFF2D: case 0xFF4D: letter = 12; break;
    case L'N': case L'n': case 0xFF2E: case 0xFF4E: letter = 13; break;
    case L'O': case L'o': case 0xFF2F: case 0xFF4F: letter = 14; break;
    case L'P': case L'p': case 0xFF30: case 0xFF50: letter = 15; break;
    case L'Q': case L'q': case 0xFF31: case 0xFF51: letter = 16; break;
    case L'R': case L'r': case 0xFF32: case 0xFF52: letter = 17; break;
    case L'S': case L's': case 0xFF33: case 0xFF53: letter = 18; break;
    case L'T': case L't': case 0xFF34: case 0xFF54: letter = 19; break;
    case L'U': case L'u': case 0xFF35: case 0xFF55: letter = 20; break;
    case L'V': case L'v': case 0xFF36: case 0xFF56: letter = 21; break;
    case L'W': case L'w': case 0xFF37: case 0xFF57: letter = 22; break;
    case L'X': case L'x': case 0xFF38: case 0xFF58: letter = 23; break;
    case L'Y': case L'y': case 0xFF39: case 0xFF59: letter = 24; break;
    case L'Z': case L'z': case 0xFF3A: case 0xFF5A: letter = 25; break;
    default: letter = -1; break;
    }
    if (letter != -1) {
        return sLetterReadings[letter];
    }
    return NULL;
}

const wchar_t *GetDigitReading(wchar_t c) {
    for (int i = 0; i < 10; i++) {
        if (c == (u16)(L'0' + i) || c == (u16)(0xFF10 + i)) {
            return sDigitReadings[i];
        }
    }
    return NULL;
}

void MakeSpeechTextInPlace(wchar_t *text, u32 size) {
    MakeSpeechText(text, text, size);
}

void MakeSpeechText(wchar_t *dst, const wchar_t *src, u32 size) {
    wchar_t katakana[0x80];
    wchar_t speech[0x80];
    wchar_t c;
    MI_CpuClear16(katakana, sizeof(katakana));
    CopyToKatakana(katakana, src);
    ReplaceSymbolsForSpeech(katakana);
    MI_CpuClear16(speech, sizeof(speech));
    u32 length = wcslen(src);
    u32 count = 0;
    for (u32 i = 0; i < length; i++) {
        c = katakana[i];
        const wchar_t *reading = GetLetterReading(c);
        if (reading != NULL) {
            count += wcslen(reading);
        } else {
            reading = GetDigitReading(c);
            if (reading != NULL) {
                count += wcslen(reading);
            } else if ((c >= 0x30A1 && c <= 0x30F4) || c == 0x30FC || c == 0xFF3F || c == L'_' || c == L'.' ||
                       c == L'?' || c == 0xFF1F || c == L'!' || c == 0xFF01) {
                count++;
            } else if (c == L' ' || c == 0x3000) {
                continue;
            } else {
                count++;
                c = 0x30F3;
            }
        }
        if (count >= 0x7F) {
            break;
        }
        if (reading != NULL) {
            wcscat(speech, reading);
        } else {
            wcsncat(speech, &c, 1);
        }
    }
    MI_CpuClear16(dst, size);
    wcsncpy(dst, speech, size / 2 - 1);
}

void ToFullWidthDigits(wchar_t *text) {
    u32 length = wcslen(text);
    for (u32 i = 0; i < length; i++) {
        if (text[i] >= L'0' && text[i] <= L'9') {
            text[i] += 0xFF10 - L'0';
        }
    }
}

BOOL IsFullWidthAlphanumeric(wchar_t c) {
    if ((c >= 0xFF21 && c <= 0xFF3A) || (c >= 0xFF41 && c <= 0xFF5A) || (c >= 0xFF10 && c <= 0xFF19)) {
        return TRUE;
    }
    return FALSE;
}

wchar_t ToHalfWidth(wchar_t c) {
    if (IsFullWidthAlphanumeric(c)) {
        c = (u16)(c - (0xFF21 - L'A'));
    }
    return c;
}

int CountSyllables(const wchar_t *text) {
    return SplitSyllables(text, NULL, 0);
}

int SplitSyllables(const wchar_t *text, wchar_t (*syllables)[4], int count) {
    if (syllables != NULL) {
        MI_CpuClear8(syllables, count * sizeof(*syllables));
    }
    int n = 0;
    u32 length = wcslen(text);
    u32 i = 0;
    while (i < length) {
        wchar_t syllable[3];
        MI_CpuClear8(syllable, sizeof(syllable));
        u32 rest = length - i;
        syllable[0] = text[i];
        if (rest >= 2) {
            syllable[1] = text[i + 1];
            if (rest >= 3) {
                syllable[2] = text[i + 2];
            }
        }
        if (rest >= 3 &&
            ((syllable[0] == L'ス' && syllable[1] == L'ゥ' && syllable[2] == L'ィ') ||
             (syllable[0] == L'ス' && syllable[1] == L'ゥ' && syllable[2] == L'ィ') ||
             (syllable[0] == L'ズ' && syllable[1] == L'ゥ' && syllable[2] == L'ィ') ||
             (syllable[0] == L'デ' && syllable[1] == L'ィ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ド' && syllable[1] == L'ゥ' && syllable[2] == L'ァ') ||
             (syllable[0] == L'ド' && syllable[1] == L'ゥ' && syllable[2] == L'ィ') ||
             (syllable[0] == L'ド' && syllable[1] == L'ゥ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ド' && syllable[1] == L'ゥ' && syllable[2] == L'ォ') ||
             (syllable[0] == L'テ' && syllable[1] == L'ィ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ト' && syllable[1] == L'ゥ' && syllable[2] == L'ァ') ||
             (syllable[0] == L'ト' && syllable[1] == L'ゥ' && syllable[2] == L'ィ') ||
             (syllable[0] == L'ト' && syllable[1] == L'ゥ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ト' && syllable[1] == L'ゥ' && syllable[2] == L'ォ') ||
             (syllable[0] == L'ヴ' && syllable[1] == L'ィ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ヴ' && syllable[1] == L'ゥ' && syllable[2] == L'ァ') ||
             (syllable[0] == L'ヴ' && syllable[1] == L'ゥ' && syllable[2] == L'ィ') ||
             (syllable[0] == L'ヴ' && syllable[1] == L'ゥ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ヴ' && syllable[1] == L'ゥ' && syllable[2] == L'ォ') ||
             (syllable[0] == L'フ' && syllable[1] == L'ィ' && syllable[2] == L'ェ') ||
             (syllable[0] == L'ヅ' && syllable[1] == L'ゥ' && syllable[2] == L'ィ'))) {
            if (syllables != NULL) {
                syllables[n][0] = syllable[0];
                syllables[n][1] = syllable[1];
                syllables[n][2] = syllable[2];
            }
            n++;
            i += 3;
        } else if (rest >= 2 &&
                   ((syllable[0] == L'イ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'キ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'キ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'キ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'キ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ク' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ク' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ク' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ク' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'シ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'シ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'シ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'シ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ス' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ス' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ス' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ツ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ツ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ツ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ツ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ニ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ニ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ニ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ニ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ヌ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ヌ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ヌ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ヌ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ミ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ミ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ミ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ミ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ム' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ム' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ム' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ム' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'リ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'リ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'リ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'リ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ル' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ル' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ル' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ル' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'グ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'グ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'グ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'グ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ギ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ズ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ズ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ズ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'デ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ド' && syllable[1] == L'ゥ') ||
                    (syllable[0] == L'デ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'デ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'デ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ブ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ブ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ブ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ブ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ピ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ピ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ピ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ピ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'プ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'プ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'プ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'プ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'テ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ト' && syllable[1] == L'ゥ') ||
                    (syllable[0] == L'テ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'テ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'テ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'チ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ヴ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ビ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ス' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'フ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ヒ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ズ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ヂ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ャ') ||
                    (syllable[0] == L'ヂ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ュ') ||
                    (syllable[0] == L'ヂ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ヂ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ジ' && syllable[1] == L'ョ') ||
                    (syllable[0] == L'ヅ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ヅ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ヅ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ィ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ォ') ||
                    (syllable[0] == L'デ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'テ' && syllable[1] == L'ェ') ||
                    (syllable[0] == L'ウ' && syllable[1] == L'ァ') ||
                    (syllable[0] == L'ヅ' && syllable[1] == L'ィ'))) {
            if (syllables != NULL) {
                syllables[n][0] = syllable[0];
                syllables[n][1] = syllable[1];
            }
            n++;
            i += 2;
        } else {
            if (syllables != NULL) {
                syllables[n][0] = syllable[0];
            }
            n++;
            i++;
        }
    }
    return n;
}

void RemoveSpeechSymbols(wchar_t *text) {
    int kept = 0;
    u32 length = wcslen(text);
    for (u32 i = 0; i < length; i++) {
        wchar_t c = text[i];
        if (c != 0xFF3F && c != L'_' && c != L'.' && c != L'?' && c != 0xFF1F && c != L'!' && c != 0xFF01) {
            text[kept++] = c;
        }
    }
    for (; kept < length; kept++) {
        text[kept] = 0;
    }
}
