#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageText.h"

// The button of the tutorial's touch screen (overlay 73): overlay 74's classes in the tutorial's cut. TouchSprite
// has no mReleased here (and no SetReleased), so it is 0xBC bytes and TouchButton 0xC8, with mKind, mState and
// mLabel four bytes earlier than overlay 74's; mKind is an int here.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// A sprite with a rectangle around its position that can be touched.
class TouchSprite : public Sprite {
public:
    TouchSprite();
    BOOL Contains(ScreenPoint point);

    /* 0xAC */ int mX; // In pixels
    /* 0xB0 */ int mY;
    /* 0xB4 */ const ScreenRect *mTouchRect; // Relative to the position
    /* 0xB8 */ BOOL mEnabled;
}; // 0xBC

// The button (the tutorial has one, of kind 0: it has a text label that moves down two pixels with the button
// when it is held).
class TouchButton : public TouchSprite {
public:
    enum State {
        STATE_IDLE,
        STATE_PRESS,
        STATE_RELEASE,
        STATE_PLAY,
    };

    TouchButton();
    void Init(int kind);
    void Update();
    void Draw();
    BOOL Contains(ScreenPoint point);

    /* 0xBC */ int mKind;
    /* 0xC0 */ int mState;
    /* 0xC4 */ MessageText *mLabel;
}; // 0xC8

#endif // TOUCHBUTTON_H
