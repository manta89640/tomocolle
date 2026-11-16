#ifndef DRAWINGCANVAS_H
#define DRAWINGCANVAS_H

#include <nitro.h>

// A background drawn on with the stylus (main program 0x02025A9C to 0x02026350, not decompiled): a rectangle of a
// background's characters, 4 bits a pixel, into which the touch pen draws lines with a round brush of one of 16
// colours. Scenes 0x23 (VoteScene), 0x3A (TestNg55Scene) and 0x8E (TestNg129Scene) hold one. Measured from the
// code:
// - The constructor (C2 0x02025A9C, C1 0x02025B5C, byte-identical twins) builds the part at 0 (0x0202575C, which
//   holds the background's screen buffer, virtual table 0x0210F5C8, and its character buffer, table 0x0210F59C, at
//   +0x24), turns drawing on, puts both ends of the stroke at (128, 96), sets the clipping rectangle from
//   0x020B598C, and sets the brush size (0x02025D00, clamped to 0..7, the brush's rows from the sine table) to 2
//   and the colour (0x02025DC0, clamped to 0..15, a 4-bit pixel repeated eight times in a word) to 1.
// - 0x02025C8C draws a line from the previous touch point to the current one (0x02025E10) while drawing is on and
//   the pen is down, then queues the changed characters for VRAM.
//
// A STAND-IN with the size and the fields the constructor sets: the class is not polymorphic itself (its
// constructor stores no table of its own, so the part at 0 is a member), and its other functions are left in the
// gap. The name is descriptive, not the original: no name of this class survives in the ROM.
class DrawingCanvas {
public:
    DrawingCanvas();

    void Update();                // Draws the stroke of the touch pen and queues what changed (0x02025C8C)
    void SetBrushSize(int size);  // 0x02025D00
    void SetColor(int color);     // 0x02025DC0

    /* 0x000 */ u8 mBg[0x5C];           // The background's buffers (0x0202575C)
    /* 0x05C */ BOOL mEnabled;          // Lines are drawn from the touch pen
    /* 0x060 */ int mFrom[2];           // The stroke's previous point (x, y)
    /* 0x068 */ int mTo[2];             // Its current point
    /* 0x070 */ int mBrushSize;         // 0 to 7
    /* 0x074 */ int mColor;             // 0 to 15
    /* 0x078 */ u32 mUnknown078[8];
    /* 0x098 */ u32 mUnknown098[8];
    /* 0x0B8 */ u32 mUnknown0B8[8];
    /* 0x0D8 */ u32 mUnknown0D8[8];
    /* 0x0F8 */ int mBrushRows[8];      // The half-width of the brush in each row
    /* 0x118 */ u8 mSpans[192][2];      // The pixel rows a line covers: first and last column
    /* 0x298 */ int mClip[4];           // The rectangle drawn into
    /* 0x2A8 */ u8 mUnknown2A8[0xC];
}; // 0x2B4

#endif // DRAWINGCANVAS_H
