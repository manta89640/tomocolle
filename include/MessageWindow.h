#ifndef MESSAGEWINDOW_H
#define MESSAGEWINDOW_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// A size or a position in pixels, as the message window's functions take it (by value, in two registers).
struct WindowPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

// The main program's message window (virtual table data_021102e8, functions func_02032fb8 to func_02034f94): a
// window on a background that slides in and out, with sprites, buttons and up to eight message texts. Not
// decompiled: a stand-in with its size, as the constructors of the scenes that hold one show it, and the fields that
// the scenes touch. Its constructor is inline: the scenes' constructors (main program, after CreateScene) store its
// table and construct its sprites, buttons and texts in place, in loops, as the one below compiles. It has no
// destructor: arrays of windows are constructed by such loops, not by __cxa_vec_ctor. Overlay 73's header has
// the same stand-in.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
struct MessageWindow {
    // The flags of Open and Close (func_02034720, func_020349cc), as far as the scenes use them
    enum {
        FLAG_OPEN = 0x02,
        FLAG_CLOSE = 0x04,
        FLAG_8 = 0x08,
        FLAG_FROM_BELOW = 0x10, // func_02034720 starts the window 0xC0 pixels down for it
    };

    // The buttons that the save screens add, by the number they are added with (func_020336b4's second argument),
    // which is also what mResult becomes when one is pressed
    enum {
        BUTTON_OK,  // "ＯＫ", common message 4
        BUTTON_YES, // "はい", common message 2
        BUTTON_NO,  // "いいえ", common message 3
    };

    MessageWindow() {}

    // Its table (0x021102E0) has these two
    virtual void SetVisible(BOOL visible); // func_02034a94: of every sprite, button, text and plain sprite, and its planes
    virtual void func_02034bc4();         // Not known: it takes more arguments than four

    /* 0x0000 */ // The table
    /* 0x0004 */ SpriteButton mSprites[12];
    /* 0x0B74 */ TextButton mButtons[16];
    /* 0x7A74 */ MessageText mTexts[8];
    /* 0xA9F4 */ Sprite mPlainSprites[5];  // Sprites without a button: func_020345d8 draws as many as +0xAD64 says
    /* 0xAD50 */ u32 mUnknownAD50;          // Bit 0: overlay 109 tests it as "the window is open", overlay 110 waits for it
    /* 0xAD54 */ u8 mUnknownAD54[0x228];
    /* 0xAF7C */ int mUnknownAF7C;          // Overlay 76 reads the choice made from it (-1: none yet)
    /* 0xAF80 */ int mResult;               // func_02034720 sets it to -1; the button pressed, when one is
    /* 0xAF84 */ int mUnknownAF84;
    /* 0xAF88 */ BOOL mUnknownAF88;         // The scenes wait for it after opening a window without buttons
    /* 0xAF8C */ BOOL mTransitionDone;      // func_02034720 clears it; set when the window has finished moving
    /* 0xAF90 */ int mUnknownAF90;
    /* 0xAF94 */ int mUnknownSetBeforeClose; // The scenes set it (5 or 15) before func_020349cc; the update reads it
    /* 0xAF98 */ u8 mUnknownAF98[0xC];
}; // 0xAFA4

extern "C" {
// --- The message window ---
void func_02032fb8(MessageWindow *window, ScreenRect rect);                     // Sets it up, with its rectangle
void func_02033174(MessageWindow *window, const char *screenName, int plane);   // The screen file of its background
void func_020331bc(MessageWindow *window, int planes, ScreenRect rect);
// Adds a button with a text: (.., number, .., position, size, the text's message file and message, font, ..,
// character offset)
void func_020336b4(MessageWindow *window, int a, int b, int c, u8 d, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset, int e, int f,
                   int g, int h);
// Adds a button with a text: (number, .., position, size, the text's message file and message, font, plane,
// character offset, the button's sounds)
void func_020337b8(MessageWindow *window, int number, int a, int b, u8 c, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset,
                   const ButtonSounds *sounds);
// Adds a message text: (.., message file, message, font, plane, position, character offset); returns the character
// offset after it
int func_02033a58(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, int *charOffset);
void func_020341d4(MessageWindow *window);                                      // Updates
void func_020345d8(MessageWindow *window);                                      // Draws
void func_020346cc(MessageWindow *window);                                      // Releases it (the scenes' Exit)
void func_02034720(MessageWindow *window, int flags, int unknown);              // Opens it
void func_020349cc(MessageWindow *window, int flags);                           // Closes it
void func_02034a3c(MessageWindow *window, int text, int argIndex, const wchar_t *string); // SetStringArg and DrawAll of a text
void func_02034f38(MessageWindow *window, int text, u32 message);               // Sets the message of a text
void func_02034f4c(MessageWindow *window, u8 hAlign);                           // Of every text
void func_02034f94(MessageWindow *window, u8 vAlign);
}

#endif // MESSAGEWINDOW_H
