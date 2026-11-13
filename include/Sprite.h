#ifndef SPRITE_H
#define SPRITE_H

#include <nitro.h>
#include <nnsys.h>

// A point in pixels, as the touch screen reports it. Passed by value in two registers.
struct ScreenPoint {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
};

// A rectangle in pixels; func_02028d88 tests whether a ScreenPoint is inside one.
struct ScreenRect {
    /* 0x0 */ int x;
    /* 0x4 */ int y;
    /* 0x8 */ int width;
    /* 0xC */ int height;
};

// A point in 20.12 fixed point, with a constructor.
struct FxVec2 : public NNSG2dFVec2 {
    FxVec2() {}
    FxVec2(fx32 x_, fx32 y_) {
        x = x_;
        y = y_;
    }
};

// The sprite class of the main program: a cell animation of the library with where and how it is drawn. Its
// functions are the main program's (Sprite_Init, Sprite_Draw and their neighbours), not decompiled yet, so only
// the fields the apartment scene's code shows are named. It is 0xAC bytes: the clock display of overlay 74 holds
// eleven of them 0xAC apart, and the classes derived from it there put their first field at 0xAC.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
struct Sprite {
    static NNSG2dFVec2 MakeVec(fx32 x, fx32 y) {
        NNSG2dFVec2 v;
        v.x = x;
        v.y = y;
        return v;
    }
    // A position is built as an FxVec2 and passed by value: that leaves the two dead copies of it that every
    // caller has on its stack (Sprite_Init has them as well).
    void SetPosition(FxVec2 position) { mPosition = position; }
    void SetPosition(fx32 x, fx32 y) { SetPosition(FxVec2(x, y)); }
    void ResetPosition() { SetPosition(FxVec2(0, 0)); }
    // The same by way of MakeVec, which orders the dead copies as the clock display's code has them. Which of the
    // two forms the sprite class really has will show when its own functions are decompiled.
    void SetPositionFromVec(fx32 x, fx32 y) {
        NNSG2dFVec2 position = MakeVec(x, y);
        mPosition = position;
    }
    void SetPosition(NNSG2dFVec2 position) { mPosition = position; }
    // The position in pixels, by way of a copy as above
    ScreenPoint GetPixelPosition() {
        ScreenPoint point;
        point.x = mPosition.x >> FX32_SHIFT;
        point.y = mPosition.y >> FX32_SHIFT;
        ScreenPoint copy = point;
        return copy;
    }

    /* 0x00 */ NNSG2dCellAnimation mAnim;   // Sprite_Draw gives it to NNS_G2dDrawCellAnimation's parts
    /* 0x58 */ int mResource;               // Which loaded cell/animation bank the sprite is of (Sprite_Init)
    /* 0x5C */ u16 mSequence;               // The animation sequence that Sprite_SetSequence last set
    /* 0x5E */ u8 mUnknown5E;
    /* 0x60 */ BOOL mVisible;               // Sprite_Draw returns at once when it is 0
    /* 0x64 */ fx32 mScreenOffsetY[2];      // Added to the y position, per screen
    /* 0x6C */ int mUnknown6C;
    /* 0x70 */ u32 mPlanes;                 // Mask of the planes it is drawn on
    /* 0x74 */ int mOverwritePalette;       // -1, or the palette the renderer is told to use
    /* 0x78 */ int mOverwritePriority;
    /* 0x7C */ int mPaletteOffset;
    /* 0x80 */ int mUnknown80;
    /* 0x84 */ BOOL mFlipH;
    /* 0x88 */ BOOL mFlipV;
    /* 0x8C */ int mUnknown8C;
    /* 0x90 */ int mFrame;                  // The animation frame shown, and the one before it
    /* 0x94 */ int mPrevFrame;
    /* 0x98 */ fx32 mScaleX;
    /* 0x9C */ fx32 mScaleY;
    /* 0xA0 */ u16 mRotation;
    /* 0xA4 */ NNSG2dFVec2 mPosition;
}; // 0xAC

extern "C" {
// --- The main program's sprite functions, not decompiled yet ---
// Sets a sprite up: the bank it is of, its animation sequence, its position in pixels, and whether it animates.
void Sprite_Init(Sprite *sprite, int resource, int sequence, int x, int y, BOOL playing);
void Sprite_Draw(Sprite *sprite);
// Changes the animation sequence; does nothing if it is the current one unless "restart" is set.
void Sprite_SetSequence(Sprite *sprite, int sequence, BOOL restart);
// Shows the given frame of the animation (it stores it in mFrame).
void Sprite_SetFrame(Sprite *sprite, int frame);
// Lets the animation run or stops it; running also makes the sprite visible.
void Sprite_SetPlaying(Sprite *sprite, BOOL playing);
// Whether the point is inside the rectangle.
BOOL func_02028d88(const ScreenRect *rect, const ScreenPoint *point);
}

#endif // SPRITE_H
