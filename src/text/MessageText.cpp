#include "text/MessageText.h"

#include <wchar.h>

extern "C" {

// Not decompiled yet, outside this file

// Mounts a compressed archive file under a name
void ArchiveManager_MountCompressed(void *manager, const char *name, const char *path, BOOL wait);
// Finds a file in a mounted archive (NNS_FndGetArchiveFileByName)
void *ArchiveManager_GetFileByName(void *manager, const char *path);
extern u8 gArchiveManager[];

} // extern "C"

MessageFileList gMessageFiles;

MessageFile::MessageFile() {
    mFile = NULL;
    mHeader = NULL;
    mInfo = NULL;
    mText = NULL;
    mStrings = NULL;
    mIds = NULL;
}

MessageFile::~MessageFile() {
}

// Shown in place of a message that is missing or empty, and of the "no message" number
// (the compiler emits a file's constants in the reverse order of their definitions)
static const wchar_t sNoMessageText[] = L"XXX";
static const wchar_t sMissingText[] = L"???";

static inline u32 SwapBytes(u32 v) {
    return ((v & 0xFF000000) >> 24) | ((v & 0xFF0000) >> 8) | ((v & 0xFF00) << 8) | (v << 24);
}

void MessageFile::Open(const char *path) {
    mFile = (u8 *)ArchiveManager_GetFileByName(gArchiveManager, path);
    mHeader = (FileHeader *)mFile;
    u32 i;
    BlockHeader *block = (BlockHeader *)(mFile + sizeof(FileHeader));
    for (i = 0; i < mHeader->mBlockCount; i++) {
        void *data = block + 1;
        switch (SwapBytes(block->mKind)) {
        case 'INF1':
            mInfo = (InfoBlock *)data;
            break;
        case 'DAT1':
            mText = (u8 *)data;
            break;
        case 'STR1':
            mStrings = data;
            break;
        case 'MID1':
            mIds = (IdBlock *)data;
            break;
        case 'FLW1':
            break;
        case 'FLI1':
            break;
        }
        block = (BlockHeader *)((u8 *)block + block->mSize);
    }
}

const wchar_t *MessageFile::GetText(u32 message) {
    if (message == NO_MESSAGE) {
        return sNoMessageText;
    }
    if (message < mIds->mCount) {
        const wchar_t *text = (const wchar_t *)(mText + (mInfo->mEntries[message].mTextOffset & ~1));
        if (wcslen(text) == 0) {
            text = sMissingText;
        }
        return text;
    }
    return sMissingText;
}

u16 MessageFile::GetWidth(u32 message) {
    if (message == NO_MESSAGE) {
        return 0;
    }
    if (message >= mIds->mCount) {
        return 0;
    }
    return mInfo->mEntries[message].mWidth;
}

u8 MessageFile::GetAlignment(u32 message) {
    if (message == NO_MESSAGE) {
        return 0;
    }
    if (message >= mIds->mCount) {
        return 0;
    }
    return mInfo->mEntries[message].mAlignment;
}

u8 MessageFile::GetLineCount(u32 message) {
    if (message == NO_MESSAGE) {
        return 0;
    }
    if (message >= mIds->mCount) {
        return 0;
    }
    return mInfo->mEntries[message].mLineCount;
}

void MessageFile::Load(int language) {
    switch (language) {
    case 0:
        ArchiveManager_MountCompressed(gArchiveManager, "BMG", "Msg/TmdMsgJP_LZ.bin", TRUE);
        Open("BMG:/tmd_msg_JP.bmg");
        break;
    default:
        ArchiveManager_MountCompressed(gArchiveManager, "BMG", "Msg/TmdMsgJP_LZ.bin", TRUE);
        Open("BMG:/tmd_msg_JP.bmg");
        break;
    }
}

MessageText::MessageText() {
    mDrawMode = 0;
    mMessage = 0;
    mCursor = NULL;
    mCharWait = 0;
    mStarted = 0;
    mHAlign = 0;
    mVAlign = 0;
    mMessageFile = NULL;
    mArgMessageFile = NULL;
    MI_CpuClear32(mStringArgs, sizeof(mStringArgs));
    MI_CpuClear32(mMessageArgs, sizeof(mMessageArgs));
}

void MessageText::InitObj(const NNSG2dFont *font, int screen, int width, int height, int charOffset, int hSpace,
                          int vSpace, int x, int y, int objParam, void *charBuffer) {
    ObjTextCanvas::InitText(font, screen, width, height, charOffset, hSpace, vSpace, x, y, objParam, charBuffer, 2,
                        NNS_G2D_CHARA_COLORMODE_16);
    NNS_G2dCharCanvasClear(&mCanvas, 0);
}

void MessageText::InitObjForMessage(MessageFile *file, u32 message, const NNSG2dFont *font, int screen,
                                    int charOffset, int hSpace, int vSpace, int x, int y, int objParam,
                                    void *charBuffer) {
    mMessageFile = file;
    mMessage = message;
    mFont = font;
    int width = GetMessageWidth();
    InitObj(font, screen, width, GetMessageHeight(vSpace), charOffset, hSpace, vSpace, x, y, objParam, charBuffer);
    mHAlign = mMessageFile->GetAlignment(mMessage);
}

void MessageText::InitBg(const NNSG2dFont *font, int screen, int width, int height, int left, int top,
                         int charOffset, int hSpace, int vSpace, int palette, void *charBuffer) {
    BgTextCanvas::InitText(font, screen, width, height, left, top, charOffset, hSpace, vSpace, palette, charBuffer, NNS_G2D_TEXT_BG_WIDTH_256,
                       NNS_G2D_CHARA_COLORMODE_16);
    NNS_G2dCharCanvasClear(&mCanvas, 0);
}

int MessageText::GetMessageWidth() {
    return (mMessageFile->GetWidth(mMessage) + 7) >> 3;
}

int MessageText::GetMessageHeight(int vSpace) {
    u8 cellHeight = NNS_G2dFontGetCellHeight(mFont);
    u8 lines = mMessageFile->GetLineCount(mMessage);
    return (lines * (vSpace + cellHeight) + 7) >> 3;
}

// The text layout of MessageText (0x0202ea24 to 0x0202f408): measuring, expanding tags, and drawing.
//
// A message is UTF-16 text with tags in it (the BMG format's escape sequences). A tag is, in bytes:
//   +0 the character 0x1A, +2 the tag's size in bytes, +3 its group, +4 (16 bits) its type, +6 its arguments.
// Group 1 tags stand for an argument, +6 being which one: types 1 to 3, 10, 30 and 31 a string of mStringArgs,
// types 20 and 21 the same cut to 12 characters, type 40 cut to 20, and type 250 a message of mMessageArgs,
// expanded in turn. The tag of group 0xFF and type 0 sets the colour to its 16-bit argument: it is kept when a
// message is expanded, and obeyed when the text is drawn.

// What is left of a character's width to the right of its glyph, and how far a glyph moves the drawing position
// once its left margin is added. (That these are functions, one unsigned, is what makes the sum compile as it
// does in DrawNextChar.)
static inline u32 GlyphRight(const NNSG2dCharWidths *widths) {
    return widths->charWidth - (widths->left + widths->glyphWidth);
}
static inline int GlyphAdvance(const NNSG2dCharWidths *widths) {
    return widths->glyphWidth + GlyphRight(widths);
}

// The width in pixels of the text's first line
int MessageText::MeasureLineWidth(const wchar_t *text, const NNSG2dFont *font) {
    int width = 0;
    while (*text != 0) {
        u16 c = *text;
        if (c == 0x1a) {
            text = (const wchar_t *)((const u8 *)text + (((const u8 *)text)[2] & ~1));
        } else if (c == '\n') {
            break;
        } else if (c >= 0x20) {
            int w = NNS_G2dFontGetCharWidth(font, c);
            width += w + mTextCanvas.hSpace;
            text++;
        } else {
            text++;
        }
    }
    return width;
}

// The height in pixels of all the text's lines
int MessageText::MeasureHeight(const wchar_t *text, const NNSG2dFont *font) {
    int fontHeight = NNS_G2dFontGetHeight(font);
    int lineHeight = fontHeight + mTextCanvas.vSpace;
    int height = 0;
    while (*text != 0) {
        height += lineHeight;
        while (*text != 0) {
            u16 c = *text;
            if (c == 0x1a) {
                text = (const wchar_t *)((const u8 *)text + (((const u8 *)text)[2] & ~1));
            } else if (c == '\n') {
                text++;
                break;
            } else {
                text++;
            }
        }
    }
    return height;
}

// Copies a string without its terminator, at most max characters of it if max is positive, all of it if
// negative. Returns how many characters it copied.
int CopyText(const wchar_t *src, wchar_t *dst, int max) {
    int count = 0;
    if (src == NULL) {
        return 0;
    }
    if (max > 0) {
        for (; count < max; count++) {
            if (*src == 0) {
                break;
            }
            *dst = *src;
            dst++;
            src++;
        }
    } else if (max < 0) {
        while (*src != 0) {
            *dst = *src;
            dst++;
            src++;
            count++;
        }
    }
    return count;
}

void MessageText::SetMessageArg(const wchar_t *text, int index) {
    mMessageArgs[index] = text;
}

void MessageText::SetMessageArg(u32 message, int index) {
    mMessageArgs[index] = mArgMessageFile->GetText(message);
}

void MessageText::SetStringArg(const wchar_t *text, int index) {
    mStringArgs[index] = text;
}

// Copies text to *out, which it moves along, putting the arguments in place of their tags
void MessageText::ExpandTags(const wchar_t *src, wchar_t **dst) {
    if (src == NULL) {
        return;
    }
    while (*src != 0) {
        if (*src == 0x1a) {
            const u8 *tag = (const u8 *)src;
            if (tag[3] == 1) {
                switch (src[2]) {
                case 1:
                case 2:
                case 3:
                    *dst += CopyText(mStringArgs[tag[6]], *dst, -1);
                    break;
                case 10:
                    *dst += CopyText(mStringArgs[tag[6]], *dst, -1);
                    break;
                case 20:
                case 21:
                    *dst += CopyText(mStringArgs[tag[6]], *dst, 12);
                    break;
                case 30:
                case 31:
                    *dst += CopyText(mStringArgs[tag[6]], *dst, -1);
                    break;
                case 40:
                    *dst += CopyText(mStringArgs[tag[6]], *dst, 20);
                    break;
                case 250:
                    ExpandTags(mMessageArgs[tag[6]], dst);
                    break;
                }
            } else if (tag[3] == 0xff) {
                switch (src[2]) {
                case 0:
                    MI_CpuCopy16(src, *dst, tag[2]);
                    *dst = (wchar_t *)((u8 *)*dst + (tag[2] & ~1));
                    break;
                }
            }
            src = (const wchar_t *)((const u8 *)src + (tag[2] & ~1));
        } else {
            **dst = *src;
            src++;
            (*dst)++;
        }
    }
    **dst = 0;
}

// Expands the message into mText, and starts over at its first character
void MessageText::ExpandMessage() {
    wchar_t *dst;
    const wchar_t *src = mMessageFile->GetText(mMessage);
    dst = mText;
    ExpandTags(src, &dst);
    mCursor = mText;
}

// How far from the canvas's left edge a line starts: align is 0 for left, 1 for centre, 2 for right. The canvas's
// size is in 8-pixel cells.
int MessageText::GetHAlignOffset(const wchar_t *text, int align) {
    if (align == 1) {
        int size = MeasureLineWidth(text, mFont);
        return (mCanvas.areaWidth * 8 - size) >> 1;
    }
    if (align == 2) {
        int size = MeasureLineWidth(text, mFont);
        return mCanvas.areaWidth * 8 - size;
    }
    return 0;
}

int MessageText::GetVAlignOffset(const wchar_t *text, int align) {
    if (align == 1) {
        int size = MeasureHeight(text, mFont);
        return (mCanvas.areaHeight * 8 - size) >> 1;
    }
    if (align == 2) {
        int size = MeasureHeight(text, mFont);
        return mCanvas.areaHeight * 8 - size;
    }
    return 0;
}

// Draws the whole message at once
void MessageText::DrawAll() {
    BeginDraw();
    while (*mCursor != 0) {
        DrawNextChar(&mCursor);
    }
}

// Draws the next visible character at *cursor, obeying the tags and line breaks before it, and moves the cursor
// past it. It stops without drawing at the end of the text.
void MessageText::DrawNextChar(wchar_t **cursor) {
    BOOL more = TRUE;
    do {
        wchar_t *text = *cursor;
        u16 c = *text;
        if (c == 0x1a) {
            const u8 *tag = (const u8 *)text;
            if (tag[3] == 0xff) {
                switch (text[2]) {
                case 0:
                    mColor = text[3];
                    break;
                }
            }
            *cursor = (wchar_t *)((u8 *)*cursor + (tag[2] & ~1));
        } else if (c == 0) {
            more = FALSE;
        } else if (c == '\n') {
            *cursor = text + 1;
            SetCharX(GetHAlignOffset(*cursor, mHAlign) + GetTextX());
            int height = NNS_G2dFontGetHeight(mFont);
            mCharY += height + mTextCanvas.vSpace;
        } else if (c >= 0x20) {
            NNSG2dGlyph glyph;
            const NNSG2dFont *font = mFont;
            NNS_G2dFontGetGlyph(&glyph, font, c);
            mCharX += glyph.pWidths->left;
            DrawGlyph(&glyph);
            mCharX += mTextCanvas.hSpace + GlyphAdvance(glyph.pWidths);
            more = FALSE;
            (*cursor)++;
        } else {
            *cursor = text + 1;
        }
    } while (more);
}

// Each frame: draws as much of the message as mDrawMode says
void MessageText::UpdateDraw() {
    switch (mDrawMode) {
    case 0:
        break;
    case 1:
        mDrawMode = 0;
        DrawAll();
        break;
    case 2:
        if (mStarted == 0) {
            mStarted++;
            mCharWait = 0;
            BeginDraw();
        } else if (mCharWait <= 0) {
            DrawNextChar(&mCursor);
            if (*mCursor == 0) {
                mDrawMode = 0;
                mColor = 1;
            } else {
                mCharWait += 2; // Two frames between characters
            }
        } else {
            mCharWait--;
        }
        break;
    case 3:
        if (mStarted == 0) {
            mStarted++;
            BeginDraw();
        } else {
            while (*mCursor != 0 && *mCursor != '\n') {
                DrawNextChar(&mCursor);
            }
            if (*mCursor == '\n') {
                DrawNextChar(&mCursor);
            } else if (*mCursor == 0) {
                mDrawMode = 0;
                mStarted = 0;
            }
        }
        break;
    case 4:
        while (*mCursor != 0) {
            DrawNextChar(&mCursor);
        }
        mDrawMode = 0;
        mColor = 1;
        break;
    }
}
