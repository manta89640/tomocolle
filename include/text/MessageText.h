#ifndef TEXT_MESSAGETEXT_H
#define TEXT_MESSAGETEXT_H

#include <nitro.h>
#include <nnsys.h>

#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// A text box that shows a message of a message file, on sprites or on a background (it is both kinds of canvas).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class MessageText : public ObjTextCanvas, public BgTextCanvas {
public:
    MessageText();

    void InitObj(const NNSG2dFont *font, int screen, int width, int height, int charOffset, int hSpace, int vSpace,
                 int x, int y, int objParam, void *charBuffer);
    void InitObjForMessage(MessageFile *file, u32 message, const NNSG2dFont *font, int screen, int charOffset,
                           int hSpace, int vSpace, int x, int y, int objParam, void *charBuffer);
    void InitBg(const NNSG2dFont *font, int screen, int width, int height, int left, int top, int charOffset,
                int hSpace, int vSpace, int palette, void *charBuffer);
    int GetMessageWidth();
    int GetMessageHeight(int vSpace);

    // --- The text layout functions (0x0202ea24 to 0x0202f408) ---
    int MeasureLineWidth(const wchar_t *text, const NNSG2dFont *font);
    int MeasureHeight(const wchar_t *text, const NNSG2dFont *font);
    void SetMessageArg(const wchar_t *text, int index);
    void SetMessageArg(u32 message, int index);
    void SetStringArg(const wchar_t *text, int index);
    void ExpandTags(const wchar_t *text, wchar_t **out);
    void ExpandMessage();
    int GetHAlignOffset(const wchar_t *text, int align);
    int GetVAlignOffset(const wchar_t *text, int align);
    void DrawAll();
    void DrawNextChar(wchar_t **cursor);
    void UpdateDraw();

    // Expands the message and puts the drawing position at the start of its first line
    void BeginDraw() {
        ExpandMessage();
        int x = GetHAlignOffset(mCursor, mHAlign) + GetTextX();
        int y = GetVAlignOffset(mCursor, mVAlign) + GetTextY();
        SetCharPos(x, y);
    }

    /* 0x154 */ MessageFile *mMessageFile;
    /* 0x158 */ MessageFile *mArgMessageFile;   // Where SetMessageArg(u32, int) finds messages for mMessageArgs
    /* 0x15C */ u32 mMessage;
    /* 0x160 */ const wchar_t *mStringArgs[20]; // Strings put in place of the message's tags (SetStringArg sets)
    /* 0x1B0 */ const wchar_t *mMessageArgs[2]; // Messages expanded in place of its tags (SetMessageArg sets)
    /* 0x1B8 */ wchar_t *mCursor;               // The next character of mText to draw
    /* 0x1BC */ wchar_t mText[480];             // The message with its tags expanded (ExpandMessage fills it)
    // The next three are the state of UpdateDraw, which draws the text frame by frame
    /* 0x57C */ int mDrawMode;  // 0: nothing to do, 1: all at once, 2: a character at a time, 3: a line, 4: the rest
    /* 0x580 */ s16 mCharWait;  // Frames left before the next character
    /* 0x582 */ u8 mStarted;    // The text has been expanded and positioned
    /* 0x583 */ u8 mHAlign;     // 0: left, 1: centre, 2: right (GetHAlignOffset); set from the message's attributes
    /* 0x584 */ u8 mVAlign;     // The same vertically (GetVAlignOffset)
}; // 0x588 without the canvas, 0x5F0 with it

#endif // TEXT_MESSAGETEXT_H
