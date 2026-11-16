#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include <nitro.h>

#include "Sprite.h"

// --- The main program's scroll bar and touch scroll area (0x020325dc to 0x02032f6c, not decompiled) ---

// A scroll bar: a sprite whose knob can be dragged. A STAND-IN with what the list's code touches.
struct ScrollBar : public Sprite {
    /* 0x0AC */ u8 mUnknownAC[0x10];
    /* 0x0BC */ BOOL mDragging; // While it is set, the list takes its scroll position from the bar
    /* 0x0C0 */ u8 mUnknownC0[4];
    /* 0x0C4 */ BOOL mReleased; // When it is set, the list snaps its scroll position to a row
    /* 0x0C8 */ u8 mUnknownC8[0x18];
    /* 0x0E0 */ BOOL mUnknownE0;
    /* 0x0E4 */ BOOL mUnknownE4;
    /* 0x0E8 */ u8 mUnknownE8[0x3C];
    /* 0x124 */ BOOL mUnknown124;
    /* 0x128 */ u8 mUnknown128[0x14];
}; // 0x13C

// A rectangle of the touch screen that the stylus scrolls by dragging. A STAND-IN as above.
struct TouchScrollArea {
    /* 0x00 */ u8 mUnknown00[0x10];
    /* 0x10 */ BOOL mDragging; // As the scroll bar's
    /* 0x14 */ BOOL mReleased;
    /* 0x18 */ u8 mUnknown18[0xC];
    /* 0x24 */ BOOL mUnknown24;
    /* 0x28 */ u8 mUnknown28[0xC];
    /* 0x34 */ int mScroll; // The scroll position the drag has reached
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ BOOL mUnknown44;
    /* 0x48 */ u8 mUnknown48[0x14];
}; // 0x5C

extern "C" {
void ScrollBar_Init(ScrollBar *bar, int a, int b, int c, int d, int e, int f, int g); // Sets it up
void ScrollBar_Update(ScrollBar *bar);                                                  // Updates it
int ScrollBar_GetPosition(ScrollBar *bar, int min, int max);       // The position of the knob between min and max
void ScrollBar_SetPosition(ScrollBar *bar, int min, int max, int value); // Puts the knob at value between min and max
void func_02032aa4(ScrollBar *bar, int a, int b);
void func_02032ac0(ScrollBar *bar, int a, int b);
void func_02032acc(ScrollBar *bar, int a, int b, int c);
void TouchScrollArea_Init(TouchScrollArea *area, ScreenRect rect);     // Sets it up on a rectangle
void TouchScrollArea_Update(TouchScrollArea *area, int scroll, int min, int max); // Updates it
void func_02032f60(TouchScrollArea *area, int a, int b);
}

#endif // SCROLLBAR_H
