#ifndef OV075_SCENE40TOUCH_H
#define OV075_SCENE40TOUCH_H

#include <nitro.h>

// The stylus on the touch screen, as the scene follows it (scene+0x69D2C): whether it went down, is held or went up
// this frame, where, which way it moves, how far it has rubbed, how fast it was flicked, and taps in a row.
// Scene40::Update updates it every frame (Update, 0x02179804).
//
// The name is descriptive: no name of this class survives in the ROM.
// A point or a move on the touch screen, in pixels (or in 20.12)
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
    /* 0x0C */ Scene40TouchPoint mPressPoint;   // Where it went down
    /* 0x14 */ Scene40TouchPoint mPoint;        // Where it is (the mean of the last eight points while mSmooth is set)
    /* 0x1C */ Scene40TouchPoint mReleasePoint; // Where it went up
    /* 0x24 */ Scene40TouchPoint mDirection;    // Which way it moved: -1, 0 or 1 on each axis
    /* 0x2C */ BOOL mSmooth;
    /* 0x30 */ Scene40TouchPoint mPoints[8];    // The last eight points, in 20.12
    /* 0x70 */ int mPointCount;
    /* 0x74 */ int mPointIndex;
    /* 0x78 */ int mDistance;  // How far it has moved while held (steps of more than one pixel)
    /* 0x7C */ int mHeldFrames;
    /* 0x80 */ Scene40TouchPoint mVelocity; // When it went up: the mean of the last five moves
    /* 0x88 */ int mMoveIndex;
    /* 0x8C */ Scene40TouchPoint mMoves[5];
    /* 0xB4 */ int mUnknownB4;
    /* 0xB8 */ int mTaps;      // Taps in a row: the second within 30 frames of the first, after 3
    /* 0xBC */ int mTapFrames; // Frames since it last went down
}; // 0xC0

#endif // OV075_SCENE40TOUCH_H
