#ifndef SPRITEBUTTON_H
#define SPRITEBUTTON_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// The main program's buttons on the touch screen (functions 0x0203115c to 0x02032158). Not decompiled: STAND-INS
// with their sizes and the fields that the scenes' code touches. The sizes are measured by the arrays that the
// scenes' constructors build, and by the message window, which holds twelve of the first and sixteen of the
// second (MessageWindow.h).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The sounds a button plays (sequence archive entries of gSoundManager; -1 for none), in the order of its fields.
// What each is for is not known yet: func_0203133c plays them as the button is touched and released.
struct ButtonSounds {
    /* 0x0 */ int mSounds[4];
}; // 0x10

// A sprite that is a button on the touch screen. A STAND-IN: the constructor func_0203117c sets no sounds and
// mUnknownDC; func_0203133c, its update, returns whether it was pressed; func_02031d7c draws it.
struct SpriteButton : public Sprite {
    SpriteButton(); // func_0203117c

    /* 0x0AC */ int mUnknownAC; // These four and mUnknownF0 are cleared by func_0203119c
    /* 0x0B0 */ int mUnknownB0;
    /* 0x0B4 */ int mUnknownB4;
    /* 0x0B8 */ int mUnknownB8;
    /* 0x0BC */ ScreenRect mTouchRect; // Where it takes a touch, around its position (func_02031eb0 sets it)
    /* 0x0CC */ ButtonSounds mSounds;  // func_02032158 copies them in
    /* 0x0DC */ BOOL mUnknownDC;
    /* 0x0E0 */ int mUnknownE0; // func_02031260 sets it to 1 and mUnknownE4 to 0
    /* 0x0E4 */ int mUnknownE4;
    /* 0x0E8 */ int mKind;      // What the button does when touched (func_02031260); func_0203133c switches on it
    /* 0x0EC */ u16 mSequence;  // The animation sequences of the button as it is and as it is pressed
    /* 0x0EE */ u16 mPressedSequence;
    /* 0x0F0 */ int mUnknownF0;
}; // 0xF4

// A sprite button with a message text on it: the window's buttons and the scenes' buttons with a label. The
// constructor func_02031f1c constructs the text and sets mMoveText; func_02031f58 puts a message on it and returns
// how many characters of VRAM the text took; func_0203206c draws both.
struct TextButton : public SpriteButton {
    TextButton(); // func_02031f1c

    /* 0x0F4 */ MessageText mText;
    /* 0x6E4 */ int mHalfWidth;  // Of the text in pixels (func_02031f58)
    /* 0x6E8 */ int mHalfHeight;
    /* 0x6EC */ BOOL mMoveText;  // The text moves down two pixels while the button shows its pressed sequence
}; // 0x6F0

extern "C" {
void func_0203119c(SpriteButton *button);                          // Resets it to its normal sequence
void func_02031260(SpriteButton *button, int kind);                // Sets mKind
void func_02031278(SpriteButton *button, u16 sequence, u16 pressed); // Sets the two sequences
BOOL func_0203133c(SpriteButton *button);                          // Updates it: whether it was pressed
void func_02031d7c(SpriteButton *button);                          // Draws it
void func_02031eb0(SpriteButton *button, int width, int height);   // Its touch rectangle, centred on it
void func_02032158(SpriteButton *button, const ButtonSounds *sounds); // Copies its sounds in
// For the kinds 2, 3 and 5: shows the button selected (its pressed sequence) or not
void func_02031e28(SpriteButton *button, BOOL selected);

int func_02031f58(TextButton *button, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                  int charOffset);
void func_0203206c(TextButton *button); // Draws it
}

#endif // SPRITEBUTTON_H
