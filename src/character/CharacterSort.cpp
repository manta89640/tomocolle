#include "character/Character.h"
#include "text/TextUtil.h"

#include <wchar.h>

// The order of the resident list: names compared as the list sorts them, with the characters normalized in steps
// (spaces removed, ligatures expanded, case and accents folded, long vowel marks expanded, katakana as hiragana,
// voiced marks removed, small kana enlarged) and then compared by a collation key per character; and the other
// sort orders (personality, birthday, sex, blood type), each falling back to the name.

extern "C" {
void MiiData_GetName(const CharacterRecord *mii, wchar_t *name);
int MiiData_GetSex(const CharacterRecord *mii);
}

void CharacterSort::RemoveSpaces(wchar_t *str) {
    u32 kept = 0;
    u32 length = wcslen(str);
    for (u32 i = 0; i < length; i++) {
        wchar_t c = str[i];
        if (c != L' ' && c != 0x3000) {
            str[kept++] = c;
        }
    }
    str[kept] = 0;
}

void CharacterSort::KatakanaToHiragana(wchar_t *str) {
    u32 length = wcslen(str);
    for (u32 i = 0; i < length; i++) {
        wchar_t c = str[i];
        if (c >= 0x30A1 && c <= 0x30F3) {
            str[i] = c - 0x60;
        } else if (c == 0x30F4) {
            str[i] = 0x26FD;
        } else if (c == 0x30F5) {
            str[i] = 0x26FE;
        } else if (c == 0x30F6) {
            str[i] = 0x26FF;
        }
    }
}

// Makes small kana their full-size letters (also small ka and ke, and 0x26FE and 0x26FF, which become ka and ke)
void CharacterSort::EnlargeSmallKana(wchar_t *str) {
    u32 length = wcslen(str);
    for (int i = 0; i < length; i++) {
        wchar_t c = str[i];
        if (c < 0x3040 && c > 0x30FE) {
            continue;
        }
        switch (c) {
        case 0x3041:
        case 0x3043:
        case 0x3045:
        case 0x3047:
        case 0x3049:
        case 0x3063:
        case 0x3083:
        case 0x3085:
        case 0x3087:
        case 0x308E:
        case 0x30A1:
        case 0x30A3:
        case 0x30A5:
        case 0x30A7:
        case 0x30A9:
        case 0x30C3:
        case 0x30E3:
        case 0x30E5:
        case 0x30E7:
        case 0x30EE:
            str[i] = c + 1;
            break;
        case 0x30F5:
            str[i] = 0x30AB;
            break;
        case 0x30F6:
            str[i] = 0x30B1;
            break;
        case 0x26FE:
            str[i] = 0x304B;
            break;
        case 0x26FF:
            str[i] = 0x3051;
            break;
        }
    }
}

// Removes the voiced and half-voiced marks from kana (0x26FD and katakana vu become u)
void CharacterSort::RemoveVoicedMarks(wchar_t *str) {
    u32 length = wcslen(str);
    for (int i = 0; i < length; i++) {
        wchar_t c = str[i];
        if (c < 0x3040 && c > 0x30FE) {
            continue;
        }
        switch (c) {
        case 0x304C:
        case 0x304E:
        case 0x3050:
        case 0x3052:
        case 0x3054:
        case 0x3056:
        case 0x3058:
        case 0x305A:
        case 0x305C:
        case 0x305E:
        case 0x3060:
        case 0x3062:
        case 0x3065:
        case 0x3067:
        case 0x3069:
        case 0x3070:
        case 0x3073:
        case 0x3076:
        case 0x3079:
        case 0x307C:
        case 0x30AC:
        case 0x30AE:
        case 0x30B0:
        case 0x30B2:
        case 0x30B4:
        case 0x30B6:
        case 0x30B8:
        case 0x30BA:
        case 0x30BC:
        case 0x30BE:
        case 0x30C0:
        case 0x30C2:
        case 0x30C5:
        case 0x30C7:
        case 0x30C9:
        case 0x30D0:
        case 0x30D3:
        case 0x30D6:
        case 0x30D9:
        case 0x30DC:
            str[i] = c - 1;
            break;
        case 0x3071:
        case 0x3074:
        case 0x3077:
        case 0x307A:
        case 0x307D:
        case 0x30D1:
        case 0x30D4:
        case 0x30D7:
        case 0x30DA:
        case 0x30DD:
            str[i] = c - 2;
            break;
        case 0x30F4:
            str[i] = 0x30A6;
            break;
        case 0x26FD:
            str[i] = 0x3046;
            break;
        }
    }
}

// Writes a long vowel mark (U+30FC, or the full-width tilde) after a kana out as the kana's vowel, in the kana's
// script: a hiragana or katakana a, i, u, e or o
void CharacterSort::ExpandLongVowelMarks(wchar_t *str) {
    u32 length = wcslen(str);
    if (length == 0) {
        str[length] = 0;
        return;
    }
    if (length == 1) {
        str[length] = 0;
        return;
    }
    for (int i = 0; i < length - 1; i++) {
        wchar_t c = str[i];
        wchar_t next = str[i + 1];
        if (next == 0x30FC || next == 0xFF5E) {
            if (c == 0x3041 ||
                c == 0x3042 ||
                c == 0x304B ||
                c == 0x304C ||
                c == 0x3055 ||
                c == 0x3056 ||
                c == 0x305F ||
                c == 0x3060 ||
                c == 0x306A ||
                c == 0x306F ||
                c == 0x3070 ||
                c == 0x3071 ||
                c == 0x307E ||
                c == 0x3083 ||
                c == 0x3084 ||
                c == 0x3089 ||
                c == 0x308E ||
                c == 0x308F ||
                c == 0x26FE) {
                str[i + 1] = 0x3042;
            } else if (c == 0x30A1 ||
                c == 0x30A2 ||
                c == 0x30AB ||
                c == 0x30AC ||
                c == 0x30B5 ||
                c == 0x30B6 ||
                c == 0x30BF ||
                c == 0x30C0 ||
                c == 0x30CA ||
                c == 0x30CF ||
                c == 0x30D0 ||
                c == 0x30D1 ||
                c == 0x30DE ||
                c == 0x30E3 ||
                c == 0x30E4 ||
                c == 0x30E9 ||
                c == 0x30EE ||
                c == 0x30EF ||
                c == 0x30F5) {
                str[i + 1] = 0x30A2;
            } else if (c == 0x3043 ||
                c == 0x3044 ||
                c == 0x304D ||
                c == 0x304E ||
                c == 0x3057 ||
                c == 0x3058 ||
                c == 0x3061 ||
                c == 0x3062 ||
                c == 0x306B ||
                c == 0x3072 ||
                c == 0x3073 ||
                c == 0x3074 ||
                c == 0x307F ||
                c == 0x308A ||
                c == 0x3090) {
                str[i + 1] = 0x3044;
            } else if (c == 0x30A3 ||
                c == 0x30A4 ||
                c == 0x30AD ||
                c == 0x30AE ||
                c == 0x30B7 ||
                c == 0x30B8 ||
                c == 0x30C1 ||
                c == 0x30C2 ||
                c == 0x30CB ||
                c == 0x30D2 ||
                c == 0x30D3 ||
                c == 0x30D4 ||
                c == 0x30DF ||
                c == 0x30EA) {
                str[i + 1] = 0x30A4;
            } else if (c == 0x3045 ||
                c == 0x3046 ||
                c == 0x304F ||
                c == 0x3050 ||
                c == 0x3059 ||
                c == 0x305A ||
                c == 0x3063 ||
                c == 0x3064 ||
                c == 0x3065 ||
                c == 0x306C ||
                c == 0x3075 ||
                c == 0x3076 ||
                c == 0x3077 ||
                c == 0x3080 ||
                c == 0x3085 ||
                c == 0x3086 ||
                c == 0x308B ||
                c == 0x26FD) {
                str[i + 1] = 0x3046;
            } else if (c == 0x30A5 ||
                c == 0x30A6 ||
                c == 0x30AF ||
                c == 0x30B0 ||
                c == 0x30B9 ||
                c == 0x30BA ||
                c == 0x30C3 ||
                c == 0x30C4 ||
                c == 0x30C5 ||
                c == 0x30CC ||
                c == 0x30D5 ||
                c == 0x30D6 ||
                c == 0x30D7 ||
                c == 0x30E0 ||
                c == 0x30E5 ||
                c == 0x30E6 ||
                c == 0x30EB ||
                c == 0x30F4) {
                str[i + 1] = 0x30A6;
            } else if (c == 0x3047 ||
                c == 0x3048 ||
                c == 0x3051 ||
                c == 0x3052 ||
                c == 0x305B ||
                c == 0x305C ||
                c == 0x3066 ||
                c == 0x3067 ||
                c == 0x306D ||
                c == 0x3078 ||
                c == 0x3079 ||
                c == 0x307A ||
                c == 0x3081 ||
                c == 0x308C ||
                c == 0x3091 ||
                c == 0x26FF) {
                str[i + 1] = 0x3048;
            } else if (c == 0x30A7 ||
                c == 0x30A8 ||
                c == 0x30B1 ||
                c == 0x30B2 ||
                c == 0x30BB ||
                c == 0x30BC ||
                c == 0x30C6 ||
                c == 0x30C7 ||
                c == 0x30CD ||
                c == 0x30D8 ||
                c == 0x30D9 ||
                c == 0x30DA ||
                c == 0x30E1 ||
                c == 0x30EC ||
                c == 0x30F6) {
                str[i + 1] = 0x30A8;
            } else if (c == 0x3049 ||
                c == 0x304A ||
                c == 0x3053 ||
                c == 0x3054 ||
                c == 0x305D ||
                c == 0x305E ||
                c == 0x3068 ||
                c == 0x3069 ||
                c == 0x306E ||
                c == 0x307B ||
                c == 0x307C ||
                c == 0x307D ||
                c == 0x3082 ||
                c == 0x3087 ||
                c == 0x3088 ||
                c == 0x308D ||
                c == 0x3092) {
                str[i + 1] = 0x304A;
            } else if (c == 0x30A9 ||
                c == 0x30AA ||
                c == 0x30B3 ||
                c == 0x30B4 ||
                c == 0x30BD ||
                c == 0x30BE ||
                c == 0x30C8 ||
                c == 0x30C9 ||
                c == 0x30CE ||
                c == 0x30DB ||
                c == 0x30DC ||
                c == 0x30DD ||
                c == 0x30E2 ||
                c == 0x30E7 ||
                c == 0x30E8 ||
                c == 0x30ED ||
                c == 0x30F2) {
                str[i + 1] = 0x30AA;
            }
        }
    }
}

// Writes the ligatures out as two letters: U+00C6, U+00E6, U+0152 and U+0153 as AE, ae, OE and oe (at most 40
// characters)
void CharacterSort::ExpandLigatures(wchar_t *str) {
    int n = 0;
    wchar_t expanded[42];
    u32 length = wcslen(str);
    for (u32 i = 0; i < length; i++) {
        switch (str[i]) {
        case 0x00C6:
            expanded[n] = L'A';
            n++;
            if (n < 40) {
                expanded[n] = L'E';
                n++;
            }
            break;
        case 0x00E6:
            expanded[n] = L'a';
            n++;
            if (n < 40) {
                expanded[n] = L'e';
                n++;
            }
            break;
        case 0x0152:
            expanded[n] = L'O';
            n++;
            if (n < 40) {
                expanded[n] = L'E';
                n++;
            }
            break;
        case 0x0153:
            expanded[n] = L'o';
            n++;
            if (n < 40) {
                expanded[n] = L'e';
                n++;
            }
            break;
        default:
            expanded[n] = str[i];
            n++;
            break;
        }
        if (n >= 40) {
            break;
        }
    }
    expanded[n] = 0;
    wcscpy(str, expanded);
}

void CharacterSort::ToUpperCase(wchar_t *str) {
    u32 length = wcslen(str);
    for (int i = 0; i < length; i++) {
        wchar_t c = str[i];
        if ((c >= 0x0061 && c <= 0x007A) || (c >= 0x00E0 && c <= 0x00FE && c != 0x00F7) ||
            (c >= 0x03B1 && c <= 0x03CB && c != 0x03C2)) {
            str[i] = c - 0x20;
        } else if (c == 0x0153) {
            str[i] = 0x0152;
        } else if (c == 0x00FF) {
            str[i] = 0x0178;
        } else if (c == 0x03AC) {
            str[i] = 0x0386;
        } else if (c == 0x03AD) {
            str[i] = 0x0388;
        } else if (c == 0x03AE) {
            str[i] = 0x0389;
        } else if (c == 0x03B8) {
            str[i] = 0x0398;
        } else if (c == 0x03AF) {
            str[i] = 0x038A;
        } else if (c == 0x03CA) {
            str[i] = 0x03AA;
        } else if (c == 0x03AB) {
            str[i] = 0x038E;
        } else if (c == 0x03CD) {
            str[i] = 0x038E;
        } else if (c == 0x03CE) {
            str[i] = 0x038F;
        } else if (c == 0x03C2) {
            str[i] = 0x03A3;
        }
    }
}

void CharacterSort::ToUpperCaseWithoutAccents(wchar_t *str) {
    u32 length = wcslen(str);
    for (int i = 0; i < length; i++) {
        wchar_t c = str[i];
        if (c >= 0x3040 && c <= 0x30FE) {
            // Kana are left alone
        } else if (c == 0x0061 || c == 0x00C0 || c == 0x00E0 || c == 0x00C1 || c == 0x00E1 || c == 0x00C2 ||
                   c == 0x00E2 || c == 0x00C4 || c == 0x00E4 || c == 0x00C3 || c == 0x00E3 || c == 0x00C5 ||
                   c == 0x00E5) {
            str[i] = 0x0041;
        } else if (c == 0x00C7 || c == 0x0063 || c == 0x00E7) {
            str[i] = 0x0043;
        } else if (c == 0x0065 || c == 0x00C8 || c == 0x00E8 || c == 0x00C9 || c == 0x00E9 || c == 0x00CA ||
                   c == 0x00EA || c == 0x00CB || c == 0x00EB) {
            str[i] = 0x0045;
        } else if (c == 0x0069 || c == 0x00CC || c == 0x00EC || c == 0x00CD || c == 0x00ED || c == 0x00CE ||
                   c == 0x00EE || c == 0x00CF || c == 0x00EF) {
            str[i] = 0x0049;
        } else if (c == 0x006E) {
            str[i] = 0x004E;
        } else if (c == 0x00F1) {
            str[i] = 0x00D1;
        } else if (c == 0x006F || c == 0x00D2 || c == 0x00F2 || c == 0x00D3 || c == 0x00F3 || c == 0x00D4 ||
                   c == 0x00F4 || c == 0x00D6 || c == 0x00F6 || c == 0x00D5 || c == 0x00F5 || c == 0x00D8 ||
                   c == 0x00F8) {
            str[i] = 0x004F;
        } else if (c == 0x0073 || c == 0x00DF) {
            str[i] = 0x0053;
        } else if (c == 0x0075 || c == 0x00D9 || c == 0x00F9 || c == 0x00DA || c == 0x00FA || c == 0x00DB ||
                   c == 0x00FB || c == 0x00DC || c == 0x00FC) {
            str[i] = 0x0055;
        } else if (c == 0x0079 || c == 0x00DD || c == 0x00FD || c == 0x0178 || c == 0x00FF) {
            str[i] = 0x0059;
        } else if (c == 0x03B1 || c == 0x0386 || c == 0x0020 || c == 0x03AC) {
            str[i] = 0x0391;
        } else if (c == 0x03B5 || c == 0x0388 || c == 0x03AD) {
            str[i] = 0x0395;
        } else if (c == 0x03B7 || c == 0x0389 || c == 0x03AE) {
            str[i] = 0x0397;
        } else if (c == 0x03B9 || c == 0x038A || c == 0x03AF || c == 0x03AA || c == 0x03CA || c == 0x0020 ||
                   c == 0x0390) {
            str[i] = 0x0399;
        } else if (c == 0x03BD) {
            str[i] = 0x039D;
        } else if (c == 0x03BF || c == 0x038C || c == 0x03CC) {
            str[i] = 0x039F;
        } else if (c == 0x03C2 || c == 0x03C3) {
            str[i] = 0x03A3;
        } else if (c == 0x03C5 || c == 0x038E || c == 0x03AB || c == 0x03CD || c == 0x03CB || c == 0x03B0) {
            str[i] = 0x03A5;
        } else if (c == 0x03C9 || c == 0x038F || c == 0x03CE) {
            str[i] = 0x03A9;
        } else if ((c >= 0x0061 && c <= 0x007A) || (c >= 0x03B1 && c <= 0x03C9)) {
            str[i] = c - 0x20;
        }
    }
}

void CharacterSort::ToCollationKeys(wchar_t *str) {
    u32 length = wcslen(str);
    for (u32 i = 0; i < length; i++) {
        str[i] = GetCollationKey(str[i]);
    }
}

int CharacterSort::GetCollationKey(wchar_t c) {
    int rank;
    switch (ToHalfWidth(c)) {
    case 0x0021:
        rank = 1;
        break;
    case 0x003F:
        rank = 2;
        break;
    case 0x00A1:
        rank = 3;
        break;
    case 0x00BF:
        rank = 4;
        break;
    case 0x0022:
        rank = 5;
        break;
    case 0x0023:
        rank = 6;
        break;
    case 0x0024:
        rank = 7;
        break;
    case 0x0025:
        rank = 8;
        break;
    case 0x0026:
        rank = 9;
        break;
    case 0x0027:
        rank = 10;
        break;
    case 0x0028:
        rank = 11;
        break;
    case 0x0029:
        rank = 12;
        break;
    case 0x002A:
        rank = 13;
        break;
    case 0x002B:
        rank = 14;
        break;
    case 0x002C:
        rank = 15;
        break;
    case 0x002D:
        rank = 16;
        break;
    case 0x002E:
        rank = 17;
        break;
    case 0x002F:
        rank = 18;
        break;
    case 0x003A:
        rank = 19;
        break;
    case 0x003B:
        rank = 20;
        break;
    case 0x037E:
        rank = 21;
        break;
    case 0x003C:
        rank = 22;
        break;
    case 0x003D:
        rank = 23;
        break;
    case 0x003E:
        rank = 24;
        break;
    case 0x0040:
        rank = 25;
        break;
    case 0x005B:
        rank = 26;
        break;
    case 0x005C:
        rank = 27;
        break;
    case 0x005E:
        rank = 29;
        break;
    case 0x005F:
        rank = 30;
        break;
    case 0x0060:
        rank = 31;
        break;
    case 0x007B:
        rank = 32;
        break;
    case 0x007C:
        rank = 33;
        break;
    case 0x007D:
        rank = 34;
        break;
    case 0x007E:
        rank = 35;
        break;
    case 0x00A2:
        rank = 36;
        break;
    case 0x00A3:
        rank = 37;
        break;
    case 0x00A5:
        rank = 38;
        break;
    case 0x00A9:
        rank = 39;
        break;
    case 0x00AB:
        rank = 40;
        break;
    case 0x00AE:
        rank = 41;
        break;
    case 0x00B1:
        rank = 42;
        break;
    case 0x00B7:
        rank = 43;
        break;
    case 0x00BB:
        rank = 44;
        break;
    case 0x00D7:
        rank = 45;
        break;
    case 0x00F7:
        rank = 46;
        break;
    case 0x2018:
        rank = 47;
        break;
    case 0x201C:
        rank = 48;
        break;
    case 0x201D:
        rank = 49;
        break;
    case 0x201E:
        rank = 50;
        break;
    case 0x20AC:
        rank = 51;
        break;
    case 0x2122:
        rank = 52;
        break;
    case 0x2190:
        rank = 53;
        break;
    case 0x2191:
        rank = 54;
        break;
    case 0x2192:
        rank = 55;
        break;
    case 0x2193:
        rank = 56;
        break;
    case 0x221E:
        rank = 57;
        break;
    case 0x25A0:
        rank = 58;
        break;
    case 0x25A1:
        rank = 59;
        break;
    case 0x25B2:
        rank = 60;
        break;
    case 0x25B3:
        rank = 61;
        break;
    case 0x25BC:
        rank = 62;
        break;
    case 0x25BD:
        rank = 63;
        break;
    case 0x25C6:
        rank = 64;
        break;
    case 0x25C7:
        rank = 65;
        break;
    case 0x25CB:
        rank = 66;
        break;
    case 0x25CE:
        rank = 67;
        break;
    case 0x25CF:
        rank = 68;
        break;
    case 0x2605:
        rank = 69;
        break;
    case 0x2606:
        rank = 70;
        break;
    case 0x266A:
        rank = 71;
        break;
    case 0x266D:
        rank = 72;
        break;
    case 0x00A6:
        rank = 73;
        break;
    case 0x00A7:
        rank = 74;
        break;
    case 0x00A8:
        rank = 75;
        break;
    case 0x00AA:
        rank = 76;
        break;
    case 0x00B0:
        rank = 77;
        break;
    case 0x00B5:
        rank = 78;
        break;
    case 0x00B4:
        rank = 79;
        break;
    case 0x00BA:
        rank = 80;
        break;
    case 0x0384:
        rank = 81;
        break;
    case 0x0385:
        rank = 82;
        break;
    case 0x0387:
        rank = 83;
        break;
    case 0x2019:
        rank = 84;
        break;
    case 0x2022:
        rank = 85;
        break;
    case 0x2026:
        rank = 86;
        break;
    case 0x203B:
        rank = 87;
        break;
    case 0x2234:
        rank = 88;
        break;
    case 0x3001:
        rank = 89;
        break;
    case 0x3002:
        rank = 90;
        break;
    case 0x300C:
        rank = 91;
        break;
    case 0x300D:
        rank = 92;
        break;
    case 0x3012:
        rank = 93;
        break;
    case 0xFF01:
        rank = 94;
        break;
    case 0xFF1F:
        rank = 95;
        break;
    case 0xE000:
        rank = 96;
        break;
    case 0xE001:
        rank = 97;
        break;
    case 0xE002:
        rank = 98;
        break;
    case 0xE003:
        rank = 99;
        break;
    case 0xE004:
        rank = 100;
        break;
    case 0xE005:
        rank = 101;
        break;
    case 0xE006:
        rank = 102;
        break;
    case 0xE007:
        rank = 103;
        break;
    case 0xE008:
        rank = 104;
        break;
    case 0xE009:
        rank = 105;
        break;
    case 0xE00A:
        rank = 106;
        break;
    case 0xE00B:
        rank = 107;
        break;
    case 0xE00C:
        rank = 108;
        break;
    case 0xE00D:
        rank = 109;
        break;
    case 0xE00E:
        rank = 110;
        break;
    case 0xE00F:
        rank = 111;
        break;
    case 0xE010:
        rank = 112;
        break;
    case 0xE011:
        rank = 113;
        break;
    case 0xE012:
        rank = 114;
        break;
    case 0xE013:
        rank = 115;
        break;
    case 0xE014:
        rank = 116;
        break;
    case 0xE015:
        rank = 117;
        break;
    case 0xE016:
        rank = 118;
        break;
    case 0xE017:
        rank = 119;
        break;
    case 0xE018:
        rank = 120;
        break;
    case 0xE019:
        rank = 121;
        break;
    case 0xE01A:
        rank = 122;
        break;
    case 0xE01B:
        rank = 123;
        break;
    case 0xE01C:
        rank = 124;
        break;
    case 0xE01D:
        rank = 125;
        break;
    case 0xE01E:
        rank = 126;
        break;
    case 0xE01F:
        rank = 127;
        break;
    case 0xE020:
        rank = 128;
        break;
    case 0xE021:
        rank = 129;
        break;
    case 0xE022:
        rank = 130;
        break;
    case 0xE023:
        rank = 131;
        break;
    case 0xE024:
        rank = 132;
        break;
    case 0xE025:
        rank = 133;
        break;
    case 0xE026:
        rank = 134;
        break;
    case 0xE027:
        rank = 135;
        break;
    case 0xE028:
        rank = 136;
        break;
    case 0xE029:
        rank = 137;
        break;
    case 0xE02A:
        rank = 138;
        break;
    case 0xE02B:
        rank = 139;
        break;
    case 0xE02C:
        rank = 140;
        break;
    case 0xE02D:
        rank = 141;
        break;
    case 0xE02E:
        rank = 142;
        break;
    case 0xE02F:
        rank = 143;
        break;
    case 0xE030:
        rank = 144;
        break;
    case 0xE031:
        rank = 145;
        break;
    case 0xE032:
        rank = 146;
        break;
    case 0xE033:
        rank = 147;
        break;
    case 0xE034:
        rank = 148;
        break;
    case 0xE035:
        rank = 149;
        break;
    case 0xE036:
        rank = 150;
        break;
    case 0xE037:
        rank = 151;
        break;
    case 0xE038:
        rank = 152;
        break;
    case 0xE039:
        rank = 153;
        break;
    case 0xE03A:
        rank = 154;
        break;
    case 0xE03B:
        rank = 155;
        break;
    case 0xE03C:
        rank = 156;
        break;
    case 0xE03D:
        rank = 157;
        break;
    case 0xE03E:
        rank = 158;
        break;
    case 0xE03F:
        rank = 159;
        break;
    case 0x0030:
        rank = 160;
        break;
    case 0x0031:
        rank = 161;
        break;
    case 0x0032:
        rank = 162;
        break;
    case 0x0033:
        rank = 163;
        break;
    case 0x0034:
        rank = 164;
        break;
    case 0x0035:
        rank = 165;
        break;
    case 0x0036:
        rank = 166;
        break;
    case 0x0037:
        rank = 167;
        break;
    case 0x0038:
        rank = 168;
        break;
    case 0x0039:
        rank = 169;
        break;
    case 0x3041:
        rank = 170;
        break;
    case 0x30A1:
        rank = 171;
        break;
    case 0x3042:
        rank = 172;
        break;
    case 0x30A2:
        rank = 173;
        break;
    case 0x3043:
        rank = 174;
        break;
    case 0x30A3:
        rank = 175;
        break;
    case 0x3044:
        rank = 176;
        break;
    case 0x30A4:
        rank = 177;
        break;
    case 0x3045:
        rank = 178;
        break;
    case 0x30A5:
        rank = 179;
        break;
    case 0x3046:
        rank = 180;
        break;
    case 0x26FD:
        rank = 181;
        break;
    case 0x30A6:
        rank = 182;
        break;
    case 0x30F4:
        rank = 183;
        break;
    case 0x3047:
        rank = 184;
        break;
    case 0x30A7:
        rank = 185;
        break;
    case 0x3048:
        rank = 186;
        break;
    case 0x30A8:
        rank = 187;
        break;
    case 0x3049:
        rank = 188;
        break;
    case 0x30A9:
        rank = 189;
        break;
    case 0x304A:
        rank = 190;
        break;
    case 0x30AA:
        rank = 191;
        break;
    case 0x26FE:
        rank = 192;
        break;
    case 0x30F5:
        rank = 193;
        break;
    case 0x304B:
        rank = 194;
        break;
    case 0x30AB:
        rank = 195;
        break;
    case 0x304C:
        rank = 196;
        break;
    case 0x30AC:
        rank = 197;
        break;
    case 0x304D:
        rank = 198;
        break;
    case 0x30AD:
        rank = 199;
        break;
    case 0x304E:
        rank = 200;
        break;
    case 0x30AE:
        rank = 201;
        break;
    case 0x304F:
        rank = 202;
        break;
    case 0x30AF:
        rank = 203;
        break;
    case 0x3050:
        rank = 204;
        break;
    case 0x30B0:
        rank = 205;
        break;
    case 0x26FF:
        rank = 206;
        break;
    case 0x30F6:
        rank = 207;
        break;
    case 0x3051:
        rank = 208;
        break;
    case 0x30B1:
        rank = 209;
        break;
    case 0x3052:
        rank = 210;
        break;
    case 0x30B2:
        rank = 211;
        break;
    case 0x3053:
        rank = 212;
        break;
    case 0x30B3:
        rank = 213;
        break;
    case 0x3054:
        rank = 214;
        break;
    case 0x30B4:
        rank = 215;
        break;
    case 0x3055:
        rank = 216;
        break;
    case 0x30B5:
        rank = 217;
        break;
    case 0x3056:
        rank = 218;
        break;
    case 0x30B6:
        rank = 219;
        break;
    case 0x3057:
        rank = 220;
        break;
    case 0x30B7:
        rank = 221;
        break;
    case 0x3058:
        rank = 222;
        break;
    case 0x30B8:
        rank = 223;
        break;
    case 0x3059:
        rank = 224;
        break;
    case 0x30B9:
        rank = 225;
        break;
    case 0x305A:
        rank = 226;
        break;
    case 0x30BA:
        rank = 227;
        break;
    case 0x305B:
        rank = 228;
        break;
    case 0x30BB:
        rank = 229;
        break;
    case 0x305C:
        rank = 230;
        break;
    case 0x30BC:
        rank = 231;
        break;
    case 0x305D:
        rank = 232;
        break;
    case 0x30BD:
        rank = 233;
        break;
    case 0x305E:
        rank = 234;
        break;
    case 0x30BE:
        rank = 235;
        break;
    case 0x305F:
        rank = 236;
        break;
    case 0x30BF:
        rank = 237;
        break;
    case 0x3060:
        rank = 238;
        break;
    case 0x30C0:
        rank = 239;
        break;
    case 0x3061:
        rank = 240;
        break;
    case 0x30C1:
        rank = 241;
        break;
    case 0x3062:
        rank = 242;
        break;
    case 0x30C2:
        rank = 243;
        break;
    case 0x3063:
        rank = 244;
        break;
    case 0x30C3:
        rank = 245;
        break;
    case 0x3064:
        rank = 246;
        break;
    case 0x30C4:
        rank = 247;
        break;
    case 0x3065:
        rank = 248;
        break;
    case 0x30C5:
        rank = 249;
        break;
    case 0x3066:
        rank = 250;
        break;
    case 0x30C6:
        rank = 251;
        break;
    case 0x3067:
        rank = 252;
        break;
    case 0x30C7:
        rank = 253;
        break;
    case 0x3068:
        rank = 254;
        break;
    case 0x30C8:
        rank = 255;
        break;
    case 0x3069:
        rank = 256;
        break;
    case 0x30C9:
        rank = 257;
        break;
    case 0x306A:
        rank = 258;
        break;
    case 0x30CA:
        rank = 259;
        break;
    case 0x306B:
        rank = 260;
        break;
    case 0x30CB:
        rank = 261;
        break;
    case 0x306C:
        rank = 262;
        break;
    case 0x30CC:
        rank = 263;
        break;
    case 0x306D:
        rank = 264;
        break;
    case 0x30CD:
        rank = 265;
        break;
    case 0x306E:
        rank = 266;
        break;
    case 0x30CE:
        rank = 267;
        break;
    case 0x306F:
        rank = 268;
        break;
    case 0x30CF:
        rank = 269;
        break;
    case 0x3070:
        rank = 270;
        break;
    case 0x30D0:
        rank = 271;
        break;
    case 0x3071:
        rank = 272;
        break;
    case 0x30D1:
        rank = 273;
        break;
    case 0x3072:
        rank = 274;
        break;
    case 0x30D2:
        rank = 275;
        break;
    case 0x3073:
        rank = 276;
        break;
    case 0x30D3:
        rank = 277;
        break;
    case 0x3074:
        rank = 278;
        break;
    case 0x30D4:
        rank = 279;
        break;
    case 0x3075:
        rank = 280;
        break;
    case 0x30D5:
        rank = 281;
        break;
    case 0x3076:
        rank = 282;
        break;
    case 0x30D6:
        rank = 283;
        break;
    case 0x3077:
        rank = 284;
        break;
    case 0x30D7:
        rank = 285;
        break;
    case 0x3078:
        rank = 286;
        break;
    case 0x30D8:
        rank = 287;
        break;
    case 0x3079:
        rank = 288;
        break;
    case 0x30D9:
        rank = 289;
        break;
    case 0x307A:
        rank = 290;
        break;
    case 0x30DA:
        rank = 291;
        break;
    case 0x307B:
        rank = 292;
        break;
    case 0x30DB:
        rank = 293;
        break;
    case 0x307C:
        rank = 294;
        break;
    case 0x30DC:
        rank = 295;
        break;
    case 0x307D:
        rank = 296;
        break;
    case 0x30DD:
        rank = 297;
        break;
    case 0x307E:
        rank = 298;
        break;
    case 0x30DE:
        rank = 299;
        break;
    case 0x307F:
        rank = 300;
        break;
    case 0x30DF:
        rank = 301;
        break;
    case 0x3080:
        rank = 302;
        break;
    case 0x30E0:
        rank = 303;
        break;
    case 0x3081:
        rank = 304;
        break;
    case 0x30E1:
        rank = 305;
        break;
    case 0x3082:
        rank = 306;
        break;
    case 0x30E2:
        rank = 307;
        break;
    case 0x3083:
        rank = 308;
        break;
    case 0x30E3:
        rank = 309;
        break;
    case 0x3084:
        rank = 310;
        break;
    case 0x30E4:
        rank = 311;
        break;
    case 0x3085:
        rank = 312;
        break;
    case 0x30E5:
        rank = 313;
        break;
    case 0x3086:
        rank = 314;
        break;
    case 0x30E6:
        rank = 315;
        break;
    case 0x3087:
        rank = 316;
        break;
    case 0x30E7:
        rank = 317;
        break;
    case 0x3088:
        rank = 318;
        break;
    case 0x30E8:
        rank = 319;
        break;
    case 0x3089:
        rank = 320;
        break;
    case 0x30E9:
        rank = 321;
        break;
    case 0x308A:
        rank = 322;
        break;
    case 0x30EA:
        rank = 323;
        break;
    case 0x308B:
        rank = 324;
        break;
    case 0x30EB:
        rank = 325;
        break;
    case 0x308C:
        rank = 326;
        break;
    case 0x30EC:
        rank = 327;
        break;
    case 0x308D:
        rank = 328;
        break;
    case 0x30ED:
        rank = 329;
        break;
    case 0x308E:
        rank = 330;
        break;
    case 0x30EE:
        rank = 331;
        break;
    case 0x308F:
        rank = 332;
        break;
    case 0x30EF:
        rank = 333;
        break;
    case 0x3092:
        rank = 334;
        break;
    case 0x30F2:
        rank = 335;
        break;
    case 0x3093:
        rank = 336;
        break;
    case 0x30F3:
        rank = 337;
        break;
    case 0x30FC:
        rank = 338;
        break;
    case 0xFF5E:
        rank = 339;
        break;
    case 0x0041:
        rank = 340;
        break;
    case 0x0061:
        rank = 341;
        break;
    case 0x00C1:
        rank = 342;
        break;
    case 0x00E1:
        rank = 343;
        break;
    case 0x00C0:
        rank = 344;
        break;
    case 0x00E0:
        rank = 345;
        break;
    case 0x00C2:
        rank = 346;
        break;
    case 0x00E2:
        rank = 347;
        break;
    case 0x00C4:
        rank = 348;
        break;
    case 0x00E4:
        rank = 349;
        break;
    case 0x00C3:
        rank = 350;
        break;
    case 0x00E3:
        rank = 351;
        break;
    case 0x00C5:
        rank = 352;
        break;
    case 0x00E5:
        rank = 353;
        break;
    case 0x00C6:
        rank = 354;
        break;
    case 0x00E6:
        rank = 355;
        break;
    case 0x0042:
        rank = 356;
        break;
    case 0x0062:
        rank = 357;
        break;
    case 0x0043:
        rank = 358;
        break;
    case 0x00C7:
        rank = 359;
        break;
    case 0x0063:
        rank = 360;
        break;
    case 0x00E7:
        rank = 361;
        break;
    case 0x0044:
        rank = 362;
        break;
    case 0x0064:
        rank = 363;
        break;
    case 0x0045:
        rank = 364;
        break;
    case 0x0065:
        rank = 365;
        break;
    case 0x00C9:
        rank = 366;
        break;
    case 0x00E9:
        rank = 367;
        break;
    case 0x00C8:
        rank = 368;
        break;
    case 0x00E8:
        rank = 369;
        break;
    case 0x00CA:
        rank = 370;
        break;
    case 0x00EA:
        rank = 371;
        break;
    case 0x00CB:
        rank = 372;
        break;
    case 0x00EB:
        rank = 373;
        break;
    case 0x0046:
        rank = 374;
        break;
    case 0x0066:
        rank = 375;
        break;
    case 0x0047:
        rank = 376;
        break;
    case 0x0067:
        rank = 377;
        break;
    case 0x0048:
        rank = 378;
        break;
    case 0x0068:
        rank = 379;
        break;
    case 0x0049:
        rank = 380;
        break;
    case 0x0069:
        rank = 381;
        break;
    case 0x00CD:
        rank = 382;
        break;
    case 0x00ED:
        rank = 383;
        break;
    case 0x00CC:
        rank = 384;
        break;
    case 0x00EC:
        rank = 385;
        break;
    case 0x00CE:
        rank = 386;
        break;
    case 0x00EE:
        rank = 387;
        break;
    case 0x00CF:
        rank = 388;
        break;
    case 0x00EF:
        rank = 389;
        break;
    case 0x004A:
        rank = 390;
        break;
    case 0x006A:
        rank = 391;
        break;
    case 0x004B:
        rank = 392;
        break;
    case 0x006B:
        rank = 393;
        break;
    case 0x004C:
        rank = 394;
        break;
    case 0x006C:
        rank = 395;
        break;
    case 0x004D:
        rank = 396;
        break;
    case 0x006D:
        rank = 397;
        break;
    case 0x004E:
        rank = 398;
        break;
    case 0x006E:
        rank = 399;
        break;
    case 0x00D1:
        rank = 400;
        break;
    case 0x00F1:
        rank = 401;
        break;
    case 0x004F:
        rank = 402;
        break;
    case 0x006F:
        rank = 403;
        break;
    case 0x00D3:
        rank = 404;
        break;
    case 0x00F3:
        rank = 405;
        break;
    case 0x00D2:
        rank = 406;
        break;
    case 0x00F2:
        rank = 407;
        break;
    case 0x00D4:
        rank = 408;
        break;
    case 0x00F4:
        rank = 409;
        break;
    case 0x00D6:
        rank = 410;
        break;
    case 0x00F6:
        rank = 411;
        break;
    case 0x00D5:
        rank = 412;
        break;
    case 0x00F5:
        rank = 413;
        break;
    case 0x00D8:
        rank = 414;
        break;
    case 0x00F8:
        rank = 415;
        break;
    case 0x0152:
        rank = 416;
        break;
    case 0x0153:
        rank = 417;
        break;
    case 0x0050:
        rank = 418;
        break;
    case 0x0070:
        rank = 419;
        break;
    case 0x0051:
        rank = 420;
        break;
    case 0x0071:
        rank = 421;
        break;
    case 0x0052:
        rank = 422;
        break;
    case 0x0072:
        rank = 423;
        break;
    case 0x0053:
        rank = 424;
        break;
    case 0x0073:
        rank = 425;
        break;
    case 0x00DF:
        rank = 426;
        break;
    case 0x0054:
        rank = 427;
        break;
    case 0x0074:
        rank = 428;
        break;
    case 0x0055:
        rank = 429;
        break;
    case 0x0075:
        rank = 430;
        break;
    case 0x00DA:
        rank = 431;
        break;
    case 0x00FA:
        rank = 432;
        break;
    case 0x00D9:
        rank = 433;
        break;
    case 0x00F9:
        rank = 434;
        break;
    case 0x00DB:
        rank = 435;
        break;
    case 0x00FB:
        rank = 436;
        break;
    case 0x00DC:
        rank = 437;
        break;
    case 0x00FC:
        rank = 438;
        break;
    case 0x0056:
        rank = 439;
        break;
    case 0x0076:
        rank = 440;
        break;
    case 0x0057:
        rank = 441;
        break;
    case 0x0077:
        rank = 442;
        break;
    case 0x0058:
        rank = 443;
        break;
    case 0x0078:
        rank = 444;
        break;
    case 0x0059:
        rank = 445;
        break;
    case 0x0079:
        rank = 446;
        break;
    case 0x00DD:
        rank = 447;
        break;
    case 0x00FD:
        rank = 448;
        break;
    case 0x0178:
        rank = 449;
        break;
    case 0x00FF:
        rank = 450;
        break;
    case 0x005A:
        rank = 451;
        break;
    case 0x007A:
        rank = 452;
        break;
    case 0x0391:
        rank = 453;
        break;
    case 0x03B1:
        rank = 454;
        break;
    case 0x0386:
        rank = 455;
        break;
    case 0x03AC:
        rank = 456;
        break;
    case 0x0392:
        rank = 457;
        break;
    case 0x03B2:
        rank = 458;
        break;
    case 0x0393:
        rank = 459;
        break;
    case 0x03B3:
        rank = 460;
        break;
    case 0x0394:
        rank = 461;
        break;
    case 0x03B4:
        rank = 462;
        break;
    case 0x0395:
        rank = 463;
        break;
    case 0x03B5:
        rank = 464;
        break;
    case 0x0388:
        rank = 465;
        break;
    case 0x03AD:
        rank = 466;
        break;
    case 0x0396:
        rank = 467;
        break;
    case 0x03B6:
        rank = 468;
        break;
    case 0x0397:
        rank = 469;
        break;
    case 0x03B7:
        rank = 470;
        break;
    case 0x0389:
        rank = 471;
        break;
    case 0x03AE:
        rank = 472;
        break;
    case 0x0398:
        rank = 473;
        break;
    case 0x03B8:
        rank = 474;
        break;
    case 0x0399:
        rank = 475;
        break;
    case 0x03B9:
        rank = 476;
        break;
    case 0x038A:
        rank = 477;
        break;
    case 0x03AF:
        rank = 478;
        break;
    case 0x03AA:
        rank = 479;
        break;
    case 0x03CA:
        rank = 480;
        break;
    case 0x0390:
        rank = 481;
        break;
    case 0x039A:
        rank = 482;
        break;
    case 0x03BA:
        rank = 483;
        break;
    case 0x039B:
        rank = 484;
        break;
    case 0x03BB:
        rank = 485;
        break;
    case 0x039C:
        rank = 486;
        break;
    case 0x03BC:
        rank = 487;
        break;
    case 0x039D:
        rank = 488;
        break;
    case 0x03BD:
        rank = 489;
        break;
    case 0x039E:
        rank = 490;
        break;
    case 0x03BE:
        rank = 491;
        break;
    case 0x039F:
        rank = 492;
        break;
    case 0x03BF:
        rank = 493;
        break;
    case 0x038C:
        rank = 494;
        break;
    case 0x03CC:
        rank = 495;
        break;
    case 0x03A0:
        rank = 496;
        break;
    case 0x03C0:
        rank = 497;
        break;
    case 0x03A1:
        rank = 498;
        break;
    case 0x03C1:
        rank = 499;
        break;
    case 0x03A3:
        rank = 500;
        break;
    case 0x03C2:
        rank = 501;
        break;
    case 0x03C3:
        rank = 502;
        break;
    case 0x03A4:
        rank = 503;
        break;
    case 0x03C4:
        rank = 504;
        break;
    case 0x03A5:
        rank = 505;
        break;
    case 0x03C5:
        rank = 506;
        break;
    case 0x038E:
        rank = 507;
        break;
    case 0x03CD:
        rank = 508;
        break;
    case 0x03AB:
        rank = 509;
        break;
    case 0x03CB:
        rank = 510;
        break;
    case 0x03B0:
        rank = 511;
        break;
    case 0x03A6:
        rank = 512;
        break;
    case 0x03C6:
        rank = 513;
        break;
    case 0x03A7:
        rank = 514;
        break;
    case 0x03C7:
        rank = 515;
        break;
    case 0x03A8:
        rank = 516;
        break;
    case 0x03C8:
        rank = 517;
        break;
    case 0x03A9:
        rank = 518;
        break;
    case 0x03C9:
        rank = 519;
        break;
    case 0x038F:
        rank = 520;
        break;
    case 0x03CE:
        rank = 521;
        break;
    case 0x0020:
        rank = 522;
        break;
    case 0x3000:
        rank = 523;
        break;
    default:
        rank = 0x20C;
        break;
    }
    rank *= 10;
    if (IsFullWidthAlphanumeric(c)) {
        rank += 5;
    }
    return rank;
}

void CharacterSort::MakeSortKeys(wchar_t *str) {
    ToCollationKeys(str);
}

// Compares two names: by field 0 and 2 the characters' names, 1 and 3 their family names, 4 their Miis' names, 5
// the arguments themselves as strings. Each round normalizes less; the first that differs decides. Equal names
// order by index, except with fields 0, 3 and 5.
int CharacterSort::CompareNames(const SavedCharacter *a, const SavedCharacter *b, int field) {
    wchar_t miiNameA[21];
    wchar_t miiNameB[21];
    wchar_t workA[41];
    wchar_t workB[41];
    const wchar_t *nameA;
    const wchar_t *nameB;
    int result;

    MI_CpuClear16(miiNameA, sizeof(miiNameA));
    MI_CpuClear16(miiNameB, sizeof(miiNameB));
    if (field == 5) {
        nameA = (const wchar_t *)a;
        nameB = (const wchar_t *)b;
    } else if (field == 0 || field == 2) {
        nameA = a->mName;
        nameB = b->mName;
    } else if (field == 1 || field == 3) {
        nameA = a->mFamilyName;
        nameB = b->mFamilyName;
    } else {
        MiiData_GetName(a, miiNameA);
        MiiData_GetName(b, miiNameB);
        nameA = miiNameA;
        nameB = miiNameB;
    }

    wcscpy(workA, nameA);
    wcscpy(workB, nameB);
    RemoveSpaces(workA);
    RemoveSpaces(workB);
    if (wcslen(workA) == 0 && wcslen(workB) == 0) {
        if (field == 5) {
            return 0;
        }
        if (field == 0 || field == 3) {
            return 0;
        }
        return a->mIndex - b->mIndex;
    }
    if (wcslen(workA) == 0) {
        return 1;
    }
    if (wcslen(workB) == 0) {
        return -1;
    }

    ExpandLigatures(workA);
    ExpandLigatures(workB);
    ToUpperCaseWithoutAccents(workA);
    ToUpperCaseWithoutAccents(workB);
    ExpandLongVowelMarks(workA);
    ExpandLongVowelMarks(workB);
    KatakanaToHiragana(workA);
    KatakanaToHiragana(workB);
    RemoveVoicedMarks(workA);
    RemoveVoicedMarks(workB);
    EnlargeSmallKana(workA);
    EnlargeSmallKana(workB);
    MakeSortKeys(workA);
    MakeSortKeys(workB);
    result = wcscmp(workA, workB);
    if (result != 0) {
        return result;
    }

    wcscpy(workA, nameA);
    wcscpy(workB, nameB);
    RemoveSpaces(workA);
    RemoveSpaces(workB);
    ExpandLigatures(workA);
    ExpandLigatures(workB);
    ToUpperCase(workA);
    ToUpperCase(workB);
    ExpandLongVowelMarks(workA);
    ExpandLongVowelMarks(workB);
    KatakanaToHiragana(workA);
    KatakanaToHiragana(workB);
    MakeSortKeys(workA);
    MakeSortKeys(workB);
    result = wcscmp(workA, workB);
    if (result != 0) {
        return result;
    }

    wcscpy(workA, nameA);
    wcscpy(workB, nameB);
    RemoveSpaces(workA);
    RemoveSpaces(workB);
    ToUpperCase(workA);
    ToUpperCase(workB);
    KatakanaToHiragana(workA);
    KatakanaToHiragana(workB);
    MakeSortKeys(workA);
    MakeSortKeys(workB);
    result = wcscmp(workA, workB);
    if (result != 0) {
        return result;
    }

    wcscpy(workA, nameA);
    wcscpy(workB, nameB);
    RemoveSpaces(workA);
    RemoveSpaces(workB);
    MakeSortKeys(workA);
    MakeSortKeys(workB);
    result = wcscmp(workA, workB);
    if (result != 0) {
        return result;
    }

    wcscpy(workA, nameA);
    wcscpy(workB, nameB);
    MakeSortKeys(workA);
    MakeSortKeys(workB);
    result = wcscmp(workA, workB);
    if (result != 0) {
        return result;
    }

    if (field == 5) {
        return 0;
    }
    if (field == 0 || field == 3) {
        return 0;
    }
    return a->mIndex - b->mIndex;
}

int CharacterSort::CompareByName(const SavedCharacter *a, const SavedCharacter *b) {
    int result = CompareNames(a, b, 0);
    if (result != 0) {
        return result;
    }
    return CompareNames(a, b, 1);
}

int CharacterSort::CompareByFamilyName(const SavedCharacter *a, const SavedCharacter *b) {
    int result = CompareNames(a, b, 3);
    if (result != 0) {
        return result;
    }
    return CompareNames(a, b, 2);
}

int CharacterSort::CompareByMiiName(const SavedCharacter *a, const SavedCharacter *b) {
    return CompareNames(a, b, 4);
}

int CharacterSort::CompareStrings(const wchar_t *a, const wchar_t *b) {
    return CompareNames((const SavedCharacter *)a, (const SavedCharacter *)b, 5);
}

int CharacterSort::CompareByPersonality(SavedCharacter *a, SavedCharacter *b) {
    int personalityA = a->GetPersonality();
    int personalityB = b->GetPersonality();
    if (a->mUnknown050Bit0) {
        personalityA += 100;
    }
    if (b->mUnknown050Bit0) {
        personalityB += 100;
    }
    int result = personalityA - personalityB;
    if (result != 0) {
        return result;
    }
    return CompareByFamilyName(a, b);
}

int CharacterSort::CompareByBirthday(SavedCharacter *a, SavedCharacter *b) {
    int dayB, monthA, monthB, dayA, yearA, yearB;
    yearA = a->mBirthYear;
    yearB = b->mBirthYear;
    monthA = a->GetParameter(PARAM_BIRTH_MONTH);
    monthB = b->GetParameter(PARAM_BIRTH_MONTH);
    dayA = a->GetParameter(PARAM_BIRTH_DAY);
    dayB = b->GetParameter(PARAM_BIRTH_DAY);
    if (yearA == 0xFFF) {
        yearA = 2100;
    }
    if (yearB == 0xFFF) {
        yearB = 2100;
    }
    if (monthA == 0) {
        monthA = 13;
    }
    if (monthB == 0) {
        monthB = 13;
    }
    if (dayA == 0) {
        dayA = 32;
    }
    if (dayB == 0) {
        dayB = 32;
    }
    u32 keyA = yearA * 366 + monthA * 31 + dayA;
    u32 keyB = yearB * 366 + monthB * 31 + dayB;
    int result = keyA - keyB;
    if (result != 0) {
        return result;
    }
    return CompareByFamilyName(a, b);
}

int CharacterSort::CompareByBirthdayInYear(SavedCharacter *a, SavedCharacter *b) {
    int monthA = a->GetParameter(PARAM_BIRTH_MONTH);
    int monthB = b->GetParameter(PARAM_BIRTH_MONTH);
    int dayA = a->GetParameter(PARAM_BIRTH_DAY);
    int dayB = b->GetParameter(PARAM_BIRTH_DAY);
    if (monthA == 0) {
        monthA = 13;
    }
    if (monthB == 0) {
        monthB = 13;
    }
    if (dayA == 0) {
        dayA = 32;
    }
    if (dayB == 0) {
        dayB = 32;
    }
    u32 keyA = monthA * 31 + dayA;
    u32 keyB = monthB * 31 + dayB;
    int result = keyA - keyB;
    if (result != 0) {
        return result;
    }
    return CompareByFamilyName(a, b);
}

int CharacterSort::CompareBySex(SavedCharacter *a, SavedCharacter *b) {
    int sexA = MiiData_GetSex(a);
    int result = sexA - MiiData_GetSex(b);
    if (result != 0) {
        return result;
    }
    return CompareByFamilyName(a, b);
}

int CharacterSort::CompareByBloodType(SavedCharacter *a, SavedCharacter *b) {
    int result = a->mBloodType - b->mBloodType;
    if (result == 0) {
        result = CompareByFamilyName(a, b);
    }
    return result;
}

// The group of the resident list's index that a character's collation key falls in: 0 to 9 the rows of the kana
// table (a, ka, sa, ...), 10 before them, 11 and 12 after
int CharacterSort::GetInitialGroup(wchar_t c) {
    int key = GetCollationKey(c);
    if (key < 0x6A4) {
        return 10;
    }
    if (key < 0x780) {
        return 0;
    }
    if (key < 0x870) {
        return 1;
    }
    if (key < 0x938) {
        return 2;
    }
    if (key < 0xA14) {
        return 3;
    }
    if (key < 0xA78) {
        return 4;
    }
    if (key < 0xBA4) {
        return 5;
    }
    if (key < 0xC08) {
        return 6;
    }
    if (key < 0xC80) {
        return 7;
    }
    if (key < 0xCE4) {
        return 8;
    }
    if (key < 0xD34) {
        return 9;
    }
    if (key < 0x152C) {
        return 11;
    }
    return 12;
}

int CharacterSort::GetNameInitialGroup(const wchar_t *name) {
    wchar_t buffer[42];
    wcscpy(buffer, name);
    RemoveSpaces(buffer);
    return GetInitialGroup(buffer[0]);
}
