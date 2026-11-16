// The on-screen keyboard's core: see docs/binary.md, "The keyboard".

#include <nitro.h>
#include <nnsys.h>

// A key: a rectangle in pixels from the keyboard's origin, and the code it gives
struct KeyRect {
    u16 x : 8;
    u16 y : 8;
    u16 w : 8;
    u16 h : 8;
    s16 link; // The next key of the same group, in keys from this one
    u16 code;
};

struct KeyboardState;

struct KeyLayout {
    const KeyRect *keys;
    void *charFile; // The NCGR file of the layout's graphics
    void (*func)(KeyboardState *kb);
    u16 unk_c;
    u16 numKeys;
};

struct KeyRepeat {
    u16 unk_0;
    u16 unk_2;
    u16 unk_4;
    u16 unk_6;
};

struct KeyboardState {
    const KeyRect *pressed; // The key touched first
    const KeyRect *current; // The key under the pen now
    const KeyLayout *layout;
    void (*unk_c)(void *arg, u16 code, int x, int y);
    void *unk_10;
    void (*unk_14)(void *arg, int sound);
    void *unk_18;
    const u16 *chars;
    KeyRepeat unk_20;
    u16 code;
    u16 unk_2a;
    u16 unk_2c;
    u16 unk_2e;
    int unk_30;
    const KeyRect *cursor;
    BOOL unk_38;
    BOOL unk_3c;
    void (*unk_40)(void *arg, int action, u16 a, u16 b, u16 x, u16 y);
    void *unk_44;
    const KeyRect *unk_48;
    u16 unk_4c;
};

#define ARRAY_COUNT(a) (sizeof(a) / sizeof((a)[0]))
// The number of keys of a table, from the bytes between its start and its end
#define KEY_COUNT(keys) (((const u8 *)&(keys)[ARRAY_COUNT(keys)] - (const u8 *)(keys)) / sizeof(KeyRect))

extern "C" {

// --- Not in this file, and not named yet ---
void *ArchiveManager_GetFileByName(void *archive, const char *fileName);
extern u8 gArchiveManager[];
BOOL func_020594dc(void *file, NNSG2dCharacterData **charData);

// The input code before this file
void func_020813fc(void);
NNSG2dFont *func_02081414(void);
void func_02081440(void);
void func_0208145c(u16 pad, TPData touch);
u16 func_02081570(u16 mask);
u16 func_02081598(void);
int func_020815a8(void);
u16 func_020815b8(void);
u16 func_020815c8(void);
void func_020815f8(KeyRepeat *repeat, u16 a, u16 b);
void func_02081658(KeyRepeat *repeat);
void func_02081688(u16 mask);
void func_020816b4(void);
u16 func_02081768(void);

u16 func_0208179c(const KeyLayout *layout);
const KeyRect *func_020817f0(KeyboardState *kb, int x, int y);
BOOL func_020819a4(void *file, NNSG2dCharacterData **charData);
void func_020819e8(KeyboardState *kb);
void func_02081b70(KeyboardState *kb);
BOOL func_02081cb8(KeyboardState *kb);
void func_02081cf8(u16 *screen, int palette, int charBase);
void func_02081d68(void *baseBuffer, void *buffer, const char (*names)[0x30]);
void func_02081df0(int x, int y, BOOL unk_3c, void (*func)(void *arg, int sound), void *arg);
void func_02081f38(int layout);
void func_02081ff0(int index, BOOL sound);
void func_02082068(u16 pad, TPData touch);
u16 func_02082560(void);
u16 func_02082570(KeyboardState *kb, u16 code);
void *func_0208265c(void);
u32 func_02082680(void);
void func_02082688(u32 *rows, u32 color, int x0, int y0, int x1, int y1);
void func_02082738(u8 *chars, int width, u32 color, int x0, int y0, int x1, int y1);
void func_0208288c(KeyboardState *kb);
const KeyRect *func_02082914(u32 dir, const KeyRect *cursor, const KeyRect *keys);
void func_02082b48(KeyboardState *kb);
void func_02082b68(KeyboardState *kb);
void func_02082b90(KeyboardState *kb);
void func_02082bb0(KeyboardState *kb);
void func_02082bd0(KeyboardState *kb);
void func_02082bf0(KeyboardState *kb);
void func_02082c58(KeyboardState *kb);
void func_02082c8c(KeyboardState *kb);
void func_02082c90(KeyboardState *kb, const u16 **tables);
void func_02082d70(KeyboardState *kb);
void func_02082d74(KeyboardState *kb);
void func_02082d78(u16 code);
void func_02082e18(KeyboardState *kb);
void func_02082e70(const KeyRect *keys, u16 count, void (*func)(void *, int, u16, u16, u16, u16), void *arg);
void func_02082e8c(void *arg, int sound);

}

// The character and key tables of the layouts are not this file's: they lie before its data, in the order of no
// one file's data, and the code reads them as separately linked binary data is read, the character tables (UTF-16
// text with a byte-order mark, which is skipped) through a cast and the key tables' sizes as the bytes between their
// start and their end. See docs/binary.md, "The keyboard's core".
extern "C" {
extern wchar_t KeyboardCharsQwertyUpper[46];
extern wchar_t KeyboardCharsQwertyLower[46];
extern wchar_t KeyboardCharsQwertySymbols[46];
extern wchar_t KeyboardCharsEuro[56];
extern wchar_t KeyboardCharsHiragana[52];
extern wchar_t KeyboardCharsKatakana[52];
extern wchar_t KeyboardCharsPicto[56];
extern wchar_t KeyboardCharsSign[52];
extern KeyRect KeyboardKeysEnglish[49];
extern KeyRect KeyboardKeysEuro[54];
extern KeyRect KeyboardKeysKana[58];
extern KeyRect KeyboardKeysSign[53];
extern KeyRect KeyboardKeysPicto[57];
}

// The kana, and what a modifier key turns each of them into (a space for none)
static wchar_t sKanaHandakuten[60] = L"　　　　　　　　　　　　　　　　　　　　ぱぴぷぺぽ　　　　　　　　　　　　　　　　　　　　　　　　パピプペポ　　　　";
static wchar_t sKanaSmall[60] = L"ぁぃぅぇぉ　　　　　　　　　　　　っ　　　　　　　ゃゅょゎァィゥェォヵ　　ヶ　　　　　　　　ッ　　　　　　　ャュョヮ";
static wchar_t sKanaPlain[60] = L"あいうえおかきくけこさしすせそたちつてとはひふへほやゆよわアイウエオカキクケコサシスセソタチツテトハヒフヘホヤユヨワ";
static wchar_t sKanaDakuten[60] = L"　　　　　がぎぐげござじずぜぞだぢづでどばびぶべぼ　　　　　　ヴ　　ガギグゲゴザジズゼゾダヂヅデドバビブベボ　　　　";
static const wchar_t *sKanaTables[4] = {sKanaPlain, sKanaDakuten, sKanaHandakuten, sKanaSmall};
static KeyRect data_02112dec[2] = {
    {104, 89, 92, 13, 0, 0}, {4, 89, 92, 13, 0, 0},
};

struct KanaPair {
    const wchar_t *from;
    const wchar_t *to;
};
static const KanaPair sKanaPairDakuten = {sKanaTables[0], sKanaTables[1]};
static const KanaPair sKanaPairHandakuten = {sKanaTables[0], sKanaTables[2]};
static const KanaPair sKanaPairSmall = {sKanaTables[0], sKanaTables[3]};

static KeyLayout sLayouts[5] = {
    {KeyboardKeysEnglish, NULL, func_02082c58, 4, 0},
    {KeyboardKeysKana, NULL, func_02082bf0, 7, 0},
    {KeyboardKeysEuro, NULL, func_02082c8c, 2, 0},
    {KeyboardKeysSign, NULL, func_02082d70, 2, 0},
    {KeyboardKeysPicto, NULL, func_02082d74, 2, 0},
};

static int sOriginY;
static u8 *sBaseCharData;
static int sOriginX;
static BOOL sInitialized;
static u8 *sCharData;
static KeyboardState sKeyboard;
static u32 data_021486d8[6]; // Only a function the link drops refers to it

extern "C" u16 func_0208179c(const KeyLayout *layout) {
    for (u16 i = 0; i < layout->numKeys; i++) {
        if (layout->keys[i].code == layout->unk_c + 1) {
            return i;
        }
    }
    return 0;
}

extern "C" const KeyRect *func_020817f0(KeyboardState *kb, int x, int y) {
    const KeyLayout *layout;
    const KeyRect *keys;
    int i;
    int numKeys;
    const KeyRect *key;
    layout = kb->layout;
    keys = layout->keys;
    numKeys = layout->numKeys;
    for (i = 0; i < numKeys; i++) {
        key = &keys[i];
        if (sOriginY + key->y <= y && y < sOriginY + (key->y + key->h) && sOriginX + key->x <= x &&
            x < sOriginX + (key->x + key->w)) {
            return &keys[i];
        }
    }
    if (kb->unk_3c) {
        keys = data_02112dec;
        for (i = 0; i < ARRAY_COUNT(data_02112dec); i++) {
            key = &keys[i];
            if (sOriginY + key->y <= y && y < sOriginY + (key->y + key->h) && sOriginX + key->x <= x &&
                x < sOriginX + (key->x + key->w)) {
                return &keys[i];
            }
        }
    }
    if (kb->unk_4c != 0) {
        // The original reads the count again at the end of each pass, but not for the first test: a volatile read
        // there, and the first test written out, give that
        i = 0;
        if (i < kb->unk_4c) {
            key = kb->unk_48;
            do {
                if (key->y <= y && y < key->y + key->h && key->x <= x && x < key->x + key->w) {
                    return key;
                }
                key++;
                i++;
            } while (i < *(volatile u16 *)&kb->unk_4c);
        }
    }
    return NULL;
}

extern "C" BOOL func_020819a4(void *file, NNSG2dCharacterData **charData) {
    NNSG2dBinaryBlockHeader *block =
        NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader *)file, NNS_G2D_BINBLK_SIG_CHARACTERDATA);
    *charData = (NNSG2dCharacterData *)(block + 1);
    if ((u32)*charData < (u32)(*charData)->pRawData) {
        return FALSE;
    }
    func_020594dc(file, charData);
    return TRUE;
}

extern "C" void func_020819e8(KeyboardState *kb) {
    int i;
    const NNSG2dFont *font;
    const KeyLayout *layout;
    NNSG2dCharCanvas canvas;
    NNSG2dCharacterData *charData;
    NNSG2dGlyph glyph;
    int cellHeight;
    u16 first;

    font = func_02081414();
    layout = kb->layout;
    cellHeight = NNS_G2dFontGetCellHeight(font);
    charData = (NNSG2dCharacterData *)&sKeyboard;
    func_020819a4(layout->charFile, &charData);
    MI_CpuCopyFast(charData->pRawData, sBaseCharData, 0x2280);
    NNS_G2dCharCanvasInitForBG(&canvas, sBaseCharData, 25, 11, NNS_G2D_CHARA_COLORMODE_16);
    first = func_0208179c(layout);
    for (i = 0; i < layout->numKeys - first; i++) {
        const KeyRect *key = &layout->keys[first + i];
        NNS_G2dFontGetGlyph(&glyph, font, kb->chars[i]);
        NNS_G2dCharCanvasDrawGlyph(&canvas, font, key->x + (key->w - glyph.pWidths->glyphWidth) / 2,
                                   key->y + (key->h - cellHeight) / 2, 7, &glyph);
    }
}

extern "C" void func_02081b70(KeyboardState *kb) {
    const KeyLayout *layout = kb->layout;
    MI_CpuCopyFast(sBaseCharData, sCharData, 0x2280);
    if (kb->unk_2e != 0) {
        const KeyRect *key = &layout->keys[kb->unk_2e - 1];
        func_02082738(sCharData, 200, 8, key->x, key->y, key->x + key->w, key->y + key->h);
    }
    const KeyRect *key = kb->pressed;
    if (key == NULL || kb->current == NULL || kb->current->code == kb->unk_2e || key->code == kb->unk_2e ||
        key->code == 0) {
        return;
    }
    if (kb->current->code >= 0x100) {
        return;
    }
    if (key->code != kb->current->code) {
        return;
    }
    do {
        func_02082738(sCharData, 200, 8, key->x, key->y, key->x + key->w, key->y + key->h);
        key += key->link;
    } while (key != kb->pressed);
}

extern "C" BOOL func_02081cb8(KeyboardState *kb) {
    BOOL result = FALSE;
    BOOL both = kb->pressed != NULL && kb->current != NULL;
    if (both) {
        if (kb->current->code == kb->pressed->code) {
            result = TRUE;
        }
    }
    return result;
}

extern "C" void func_02081cf8(u16 *screen, int palette, int charBase) {
    const int width = 25;
    const int blank = 0x113;
    const int height = 11;
    int x, y;
    MI_CpuFill16(screen, blank, 0x800);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            ((u16(*)[32])screen)[y][x] = (charBase + y * width + x) | (palette << 12);
        }
    }
    DC_StoreRange(screen, 0x800);
}

extern "C" void func_02081d68(void *baseBuffer, void *buffer, const char (*names)[0x30]) {
    sBaseCharData = (u8 *)baseBuffer;
    sCharData = (u8 *)buffer;
    sLayouts[0].charFile = ArchiveManager_GetFileByName(gArchiveManager, names[1]);
    sLayouts[1].charFile = ArchiveManager_GetFileByName(gArchiveManager, names[0]);
    sLayouts[2].charFile = ArchiveManager_GetFileByName(gArchiveManager, names[2]);
    sLayouts[3].charFile = ArchiveManager_GetFileByName(gArchiveManager, names[3]);
    sLayouts[4].charFile = ArchiveManager_GetFileByName(gArchiveManager, names[4]);
}

extern "C" void func_02081df0(int x, int y, BOOL unk_3c, void (*func)(void *arg, int sound), void *arg) {
    sOriginX = (u16)x;
    sOriginY = (u16)y;
    sKeyboard.unk_38 = FALSE;
    sKeyboard.unk_3c = unk_3c;
    sKeyboard.unk_14 = func;
    sKeyboard.unk_18 = arg;
    func_02081440();
    if (sInitialized) {
        return;
    }
    sInitialized = TRUE;
    func_020813fc();
    func_02081688(PAD_BUTTON_B | PAD_BUTTON_X | PAD_KEY_UP | PAD_KEY_DOWN | PAD_KEY_LEFT | PAD_KEY_RIGHT);
    sLayouts[0].numKeys = KEY_COUNT(KeyboardKeysEnglish);
    sLayouts[1].numKeys = KEY_COUNT(KeyboardKeysKana);
    sLayouts[2].numKeys = KEY_COUNT(KeyboardKeysEuro);
    sLayouts[3].numKeys = KEY_COUNT(KeyboardKeysSign);
    sLayouts[4].numKeys = KEY_COUNT(KeyboardKeysPicto);
    sKeyboard.unk_c = NULL;
    sKeyboard.unk_10 = NULL;
    sKeyboard.cursor = &sLayouts[1].keys[sLayouts[1].unk_c];
    func_02081f38(1);
    sKeyboard.unk_48 = NULL;
    sKeyboard.unk_4c = 0;
    sKeyboard.unk_40 = NULL;
    sKeyboard.unk_44 = NULL;
}

extern "C" void func_02081f38(int layout) {
    sKeyboard.unk_30 = layout;
    sKeyboard.pressed = NULL;
    sKeyboard.current = NULL;
    sKeyboard.code = 0;
    sKeyboard.unk_2a = 0;
    sKeyboard.unk_2e = 0;
    sKeyboard.unk_2c = 0;
    func_020815f8(&sKeyboard.unk_20, 1, 60);
    switch (layout) {
    case 0:
        func_02082b48(&sKeyboard);
        break;
    case 1:
        func_02082b68(&sKeyboard);
        break;
    case 2:
        func_02082b90(&sKeyboard);
        break;
    case 3:
        func_02082bb0(&sKeyboard);
        break;
    case 4:
        func_02082bd0(&sKeyboard);
        break;
    }
    func_020819e8(&sKeyboard);
    func_02081b70(&sKeyboard);
    func_0208288c(&sKeyboard);
}

extern "C" void func_02081ff0(int index, BOOL sound) {
    const int unused2[5] = {2, 3, 1, 4, 0};
    int layouts[5] = {0, 2, 1, 3, 4};
    const int unused1[5] = {2, 0, 3, 4, 1};
    layouts[0] = 1;
    layouts[1] = 0;
    layouts[2] = 2;
    func_02081f38(layouts[index]);
    if (sound) {
        func_02082e8c(sKeyboard.unk_18, 0);
    }
}

extern "C" void func_02082068(u16 pad, TPData touch) {
    u16 key = 0xFF00;
    u16 touched = 0xFF00;
    int action = 0;
    func_0208145c(pad, touch);
    func_020816b4();
    sKeyboard.code = 0;
    sKeyboard.unk_2a = 0;
    if (sKeyboard.unk_38) {
        sKeyboard.unk_38 = FALSE;
        sKeyboard.code = sKeyboard.cursor->code;
        if (sKeyboard.cursor->code != 0) {
            func_02082d78(sKeyboard.code);
        }
    } else if (func_02081768() & PAD_BUTTON_B) {
        sKeyboard.code = 1;
        func_02082e8c(sKeyboard.unk_18, 1);
    } else if (func_02081768() & PAD_BUTTON_X) {
        if (sKeyboard.unk_30 == 0) {
            if (sKeyboard.unk_2e == 0) {
                sKeyboard.code = 3;
            } else if (sKeyboard.unk_2e == 3) {
                sKeyboard.code = 4;
            } else if (sKeyboard.unk_2e == 4) {
                sKeyboard.code = 4;
            }
        }
        func_02082e8c(sKeyboard.unk_18, 2);
    } else if (func_02081768() & PAD_KEY_UP) {
        sKeyboard.cursor = func_02082914(PAD_KEY_UP, sKeyboard.cursor, sKeyboard.layout->keys);
    } else if (func_02081768() & PAD_KEY_DOWN) {
        sKeyboard.cursor = func_02082914(PAD_KEY_DOWN, sKeyboard.cursor, sKeyboard.layout->keys);
    } else if (func_02081768() & PAD_KEY_LEFT) {
        sKeyboard.cursor = func_02082914(PAD_KEY_LEFT, sKeyboard.cursor, sKeyboard.layout->keys);
    } else if (func_02081768() & PAD_KEY_RIGHT) {
        sKeyboard.cursor = func_02082914(PAD_KEY_RIGHT, sKeyboard.cursor, sKeyboard.layout->keys);
    } else if (func_02081570(PAD_BUTTON_START) && sKeyboard.unk_3c) {
        sKeyboard.cursor = data_02112dec;
    }

    if (func_02081598()) {
        sKeyboard.current = func_020817f0(&sKeyboard, func_020815b8(), func_020815c8());
        if (sKeyboard.current != NULL) {
            touched = sKeyboard.current->code;
        }
    }

    if (func_020815a8() == 1) {
        sKeyboard.pressed = func_020817f0(&sKeyboard, func_020815b8(), func_020815c8());
        if (sKeyboard.pressed != NULL) {
            u16 code = sKeyboard.pressed->code;
            if (code == 1) {
                func_02082e8c(sKeyboard.unk_18, 1);
            } else if (code == 20) {
                if (!sKeyboard.unk_3c) {
                    func_02082e8c(sKeyboard.unk_18, 3);
                }
            } else if (code != 0 && code <= sKeyboard.layout->unk_c) {
                func_02082e8c(sKeyboard.unk_18, 4);
            } else if (code >= 0x100) {
                key = code;
                action = 1;
            } else if (code != 0) {
                func_02082e8c(sKeyboard.unk_18, 2);
            }
        }
    } else if (func_020815a8() == 3) {
        if (sKeyboard.unk_c != NULL && sKeyboard.unk_2c != 0) {
            sKeyboard.unk_c(sKeyboard.unk_10, sKeyboard.unk_2c, func_020815b8(), func_020815c8());
        }
        const KeyRect *k = func_020817f0(&sKeyboard, func_020815b8(), func_020815c8());
        if (k != NULL) {
            touched = k->code;
            if (touched >= 0x100) {
                action = 3;
            }
        } else {
            touched = 0xFF00;
        }
        if (sKeyboard.pressed != NULL) {
            key = sKeyboard.pressed->code;
            if (key >= 0x100) {
                action = 3;
            }
        } else {
            key = 0xFF00;
        }
        sKeyboard.unk_2c = 0;
        sKeyboard.pressed = NULL;
    }

    if (func_02081cb8(&sKeyboard)) {
        if (func_020815a8() == 1) {
            sKeyboard.code = sKeyboard.pressed->code;
            u16 code = sKeyboard.pressed->code;
            if (code != 1 && code != 20 && (code == 0 || code > sKeyboard.layout->unk_c)) {
                if (code >= 0x100) {
                    key = code;
                }
            }
        }
    } else {
        func_02081658(&sKeyboard.unk_20);
    }

    func_02082e18(&sKeyboard);
    sKeyboard.unk_2a = func_02082570(&sKeyboard, sKeyboard.code);
    if (sKeyboard.code != 0) {
        sKeyboard.layout->func(&sKeyboard);
    }
    if (action == 0 && sKeyboard.pressed != NULL && sKeyboard.pressed->code >= 0x100 &&
        (sKeyboard.current == NULL || sKeyboard.current->code != sKeyboard.pressed->code)) {
        key = sKeyboard.pressed->code;
        action = 4;
    }
    if (action != 0 && sKeyboard.unk_40 != NULL) {
        sKeyboard.unk_40(sKeyboard.unk_44, action, key, touched, func_020815b8(), func_020815c8());
    }
    func_02081b70(&sKeyboard);
}

extern "C" u16 func_02082560(void) {
    return sKeyboard.unk_2a;
}

extern "C" u16 func_02082570(KeyboardState *kb, u16 code) {
    const u32 space = L' ';
    if (code == 0 || code >= 0x100) {
        return 0;
    }
    if (code <= kb->layout->unk_c) {
        switch (code) {
        case 1:
            return 8;
        case 20:
            return 10;
        case 2:
            return space;
        case 5:
            if (kb->unk_30 == 1) {
                return 7;
            }
            break;
        case 6:
            if (kb->unk_30 == 1) {
                return 9;
            }
            break;
        case 7:
            if (kb->unk_30 == 1) {
                return 11;
            }
            break;
        default:
            return 0;
        }
    } else {
        u16 c = kb->chars[code - kb->layout->unk_c - 1];
        if (c <= space) {
            return 0;
        }
        return c;
    }
    return 0;
}

extern "C" void *func_0208265c(void) {
    DC_StoreRange(sCharData, 0x2280);
    return sCharData;
}

extern "C" u32 func_02082680(void) {
    return 0x2280;
}

extern "C" void func_02082688(u32 *rows, u32 color, int x0, int y0, int x1, int y1) {
    u32 mask = 0;
    for (int i = 0; i < x1 - x0; i++) {
        mask |= color << (i * 4);
    }
    mask <<= x0 * 4;
    for (int y = y0; y < y1; y++) {
        u32 m = mask;
        if ((rows[y] & 0xF0000000) == 0) {
            m &= ~0xF0000000;
        }
        if ((rows[y] & 0x0F000000) == 0) {
            m &= ~0x0F000000;
        }
        if ((rows[y] & 0x00F00000) == 0) {
            m &= ~0x00F00000;
        }
        if ((rows[y] & 0x000F0000) == 0) {
            m &= ~0x000F0000;
        }
        if ((rows[y] & 0x0000F000) == 0) {
            m &= ~0x0000F000;
        }
        if ((rows[y] & 0x00000F00) == 0) {
            m &= ~0x00000F00;
        }
        if ((rows[y] & 0x000000F0) == 0) {
            m &= ~0x000000F0;
        }
        if ((rows[y] & 0x0000000F) == 0) {
            m &= ~0x0000000F;
        }
        rows[y] += m;
    }
}

extern "C" void func_02082738(u8 *chars, int width, u32 color, int x0, int y0, int x1, int y1) {
    int tx, ty;
    int tx0, ty0, tx1, ty1;
    if (x0 >= x1 || y0 >= y1) {
        return;
    }
    tx1 = ((x1 - 1) >> 3) + 1;
    ty1 = ((y1 - 1) >> 3) + 1;
    tx0 = x0 >> 3;
    ty0 = y0 >> 3;
    for (ty = ty0; ty < ty1; ty++) {
        for (tx = tx0; tx < tx1; tx++) {
            int lx0 = x0 - tx * 8;
            if (lx0 < 0) {
                lx0 = 0;
            }
            int ly0 = y0 - ty * 8 < 0 ? 0 : y0 - ty * 8;
            int lx1 = x1 - tx * 8;
            if (lx1 > 8) {
                lx1 = 8;
            }
            int ly1 = y1 - ty * 8 > 8 ? 8 : y1 - ty * 8;
            func_02082688((u32 *)(chars + (tx + ty * (width >> 3)) * 32), color, lx0, ly0, lx1, ly1);
        }
    }
}

extern "C" void func_0208288c(KeyboardState *kb) {
    const KeyRect *cursor = kb->cursor;
    int x = sOriginX + (cursor->x + cursor->x + cursor->w) / 2;
    int y = sOriginY + (cursor->y + cursor->y + cursor->h) / 2;
    const KeyRect *key = func_020817f0(kb, x + 3, y - 3);
    if (key != NULL) {
        kb->cursor = key;
    } else {
        kb->cursor = &kb->layout->keys[kb->layout->unk_c];
    }
}

extern "C" const KeyRect *func_02082914(u32 dir, const KeyRect *cursor, const KeyRect *keys) {
    int x, endY, endX, dy, dx, px, y;
    func_02082e8c(sKeyboard.unk_18, 10);
    if (dir & PAD_KEY_UP) {
        dx = -8;
        dy = -8;
        x = sOriginX + (cursor->x + cursor->x + cursor->w) / 2;
        y = sOriginY + cursor->y;
        endX = x - 16;
        endY = y - 80;
    } else if (dir & PAD_KEY_DOWN) {
        dx = 8;
        dy = 8;
        x = sOriginX + (cursor->x + cursor->x + cursor->w) / 2;
        y = sOriginY + (cursor->y + cursor->h);
        endX = x + 16;
        endY = y + 80;
    } else if (dir & PAD_KEY_LEFT) {
        x = sOriginX + cursor->x;
        y = sOriginY + (cursor->y + cursor->y + cursor->h) / 2;
        endY = y + 16;
        endX = x - 80;
        dx = -8;
        dy = 8;
    } else if (dir & PAD_KEY_RIGHT) {
        dx = 8;
        x = sOriginX + (cursor->x + cursor->w);
        y = sOriginY + (cursor->y + cursor->y + cursor->h) / 2;
        dy = 8;
        endX = x + 80;
        endY = y + 16;
    }
    for (; y != endY; y += dy) {
        for (px = x; px != endX; px += dx) {
            int tx = px;
            int ty = y;
            if (tx - sOriginX < 0) {
                tx += 200;
            } else if (tx - sOriginX > 200) {
                tx -= 200;
            }
            if (ty - sOriginY < 0) {
                ty += 104;
            } else if (ty - sOriginY > 104) {
                ty -= 104;
            }
            const KeyRect *key = func_020817f0(&sKeyboard, tx, ty);
            if (key != NULL && key != cursor) {
                return key;
            }
        }
    }
    return cursor;
}

extern "C" void func_02082b48(KeyboardState *kb) {
    kb->layout = &sLayouts[0];
    kb->chars = (const u16 *)KeyboardCharsQwertyLower + 1;
}

extern "C" void func_02082b68(KeyboardState *kb) {
    kb->layout = &sLayouts[1];
    kb->chars = (const u16 *)KeyboardCharsHiragana + 1;
    kb->unk_2e = 3;
}

extern "C" void func_02082b90(KeyboardState *kb) {
    kb->layout = &sLayouts[2];
    kb->chars = (const u16 *)KeyboardCharsEuro + 1;
}

extern "C" void func_02082bb0(KeyboardState *kb) {
    kb->layout = &sLayouts[3];
    kb->chars = (const u16 *)KeyboardCharsSign + 1;
}

extern "C" void func_02082bd0(KeyboardState *kb) {
    kb->layout = &sLayouts[4];
    kb->chars = (const u16 *)KeyboardCharsPicto + 1;
}

extern "C" void func_02082bf0(KeyboardState *kb) {
    if (kb->code < 3 || kb->code > 4) {
        return;
    }
    switch (kb->code) {
    case 3:
        kb->chars = (const u16 *)KeyboardCharsHiragana + 1;
        kb->unk_2e = 3;
        break;
    case 4:
        kb->chars = (const u16 *)KeyboardCharsKatakana + 1;
        kb->unk_2e = 4;
        break;
    }
    func_020819e8(kb);
}

extern "C" void func_02082c58(KeyboardState *kb) {
    const u16 *tables[3] = {(const u16 *)KeyboardCharsQwertyLower, (const u16 *)KeyboardCharsQwertySymbols, (const u16 *)KeyboardCharsQwertyUpper};
    func_02082c90(kb, tables);
}

extern "C" void func_02082c8c(KeyboardState *kb) {
}

extern "C" void func_02082c90(KeyboardState *kb, const u16 **tables) {
    switch (kb->unk_2e) {
    case 0:
        if (kb->code == 3) {
            kb->unk_2e = 3;
        } else if (kb->code == 4) {
            kb->unk_2e = 4;
        }
        break;
    case 3:
        if (kb->code == 3) {
            kb->unk_2e = 0;
        } else if (kb->code == 4) {
            kb->unk_2e = 4;
        } else {
            kb->unk_2e = 0;
        }
        break;
    case 4:
        if (kb->code == 3) {
            kb->unk_2e = 3;
        } else if (kb->code == 4) {
            kb->unk_2e = 0;
        }
        break;
    }
    switch (kb->unk_2e) {
    case 0:
        kb->chars = tables[0] + 1;
        break;
    case 3:
        kb->chars = tables[1] + 1;
        break;
    case 4:
        kb->chars = tables[2] + 1;
        break;
    }
    func_020819e8(kb);
}

extern "C" void func_02082d70(KeyboardState *kb) {
}

extern "C" void func_02082d74(KeyboardState *kb) {
}

extern "C" void func_02082d78(u16 code) {
    if (code == 1) {
        func_02082e8c(sKeyboard.unk_18, 1);
    } else if (code == 20) {
        if (sKeyboard.unk_3c) {
            func_02082e8c(sKeyboard.unk_18, 9);
        } else {
            func_02082e8c(sKeyboard.unk_18, 3);
        }
    } else if (code != 0 && code <= sKeyboard.layout->unk_c) {
        func_02082e8c(sKeyboard.unk_18, 4);
    } else if (code != 0) {
        func_02082e8c(sKeyboard.unk_18, 2);
    }
}

extern "C" void func_02082e18(KeyboardState *kb) {
    const KeyRect *key = func_020817f0(kb, func_020815b8(), func_020815c8());
    if (kb->pressed == NULL) {
        return;
    }
    if (key != NULL && kb->pressed == key) {
        return;
    }
    u16 c = func_02082570(kb, kb->pressed->code);
    if (c >= 0x20) {
        kb->unk_2c = c;
    }
}

extern "C" void func_02082e70(const KeyRect *keys, u16 count, void (*func)(void *, int, u16, u16, u16, u16),
                              void *arg) {
    sKeyboard.unk_48 = keys;
    sKeyboard.unk_4c = count;
    sKeyboard.unk_40 = func;
    sKeyboard.unk_44 = arg;
}

extern "C" void func_02082e8c(void *arg, int sound) {
    if (sKeyboard.unk_14 != NULL) {
        sKeyboard.unk_14(arg, sound);
    }
}

// Nothing calls this: it keeps sKanaPairs, which only the static initialiser refers to, as in the original
extern "C" const KanaPair *KeyboardCore_Unused(int i) {
    data_021486d8[i] = 0;
    switch (i) {
    case 0:
        return &sKanaPairDakuten;
    case 1:
        return &sKanaPairHandakuten;
    }
    return &sKanaPairSmall;
}
