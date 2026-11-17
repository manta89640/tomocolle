#ifndef OV075_SCENE40TOUCH_H
#define OV075_SCENE40TOUCH_H

#include <nitro.h>

// The stylus on the touch screen, as the scene follows it (scene+0x69D2C): whether it went down, is held or went up
// this frame, where, which way it moves, how far it has rubbed, how fast it was flicked, and taps in a row.
// Scene40::Update updates it every frame (Update, 0x02179804).
//
// The name is descriptive: no name of this class survives in the ROM.
// A point or a move on the touch screen, in pixels
struct Scene40TouchPoint {
    int x;
    int y;
};

class Scene40Touch {
public:
    void Init();
    void Update();
    void Reset();
    Scene40TouchPoint GetVelocity();
    void CountTap();

    /* 0x00 */ BOOL mPressed;  // The stylus went down this frame
    /* 0x04 */ BOOL mHeld;     // It is down
    /* 0x08 */ BOOL mReleased; // It went up this frame
    /* 0x0C */ int mPressX;    // Where it went down
    /* 0x10 */ int mPressY;
    /* 0x14 */ int mX;         // Where it is (the mean of the last eight points while mSmooth is set)
    /* 0x18 */ int mY;
    /* 0x1C */ int mReleaseX;  // Where it went up
    /* 0x20 */ int mReleaseY;
    /* 0x24 */ int mDirX;      // Which way it moved: -1, 0 or 1
    /* 0x28 */ int mDirY;
    /* 0x2C */ BOOL mSmooth;
    /* 0x30 */ int mPoints[8][2]; // The last eight points, in 20.12
    /* 0x70 */ int mPointCount;
    /* 0x74 */ int mPointIndex;
    /* 0x78 */ int mDistance;  // How far it has moved while held (steps of more than one pixel)
    /* 0x7C */ int mHeldFrames;
    /* 0x80 */ int mVelocityX; // When it went up: the mean of the last five moves
    /* 0x84 */ int mVelocityY;
    /* 0x88 */ int mMoveIndex;
    /* 0x8C */ int mMoves[5][2];
    /* 0xB4 */ int mUnknownB4;
    /* 0xB8 */ int mTaps;      // Taps in a row: the second within 30 frames of the first, after 3
    /* 0xBC */ int mTapFrames; // Frames since it last went down
}; // 0xC0

#endif // OV075_SCENE40TOUCH_H
