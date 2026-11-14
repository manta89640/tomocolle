#ifndef TOUCHBUTTON_H
#define TOUCHBUTTON_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageText.h"

// The buttons of the apartment scene's touch screen.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// A sprite with a rectangle around its position that can be touched.
class TouchSprite : public Sprite {
public:
    TouchSprite();
    BOOL Contains(ScreenPoint point);
    BOOL SetReleased(BOOL released);

    /* 0xAC */ int mX; // In pixels
    /* 0xB0 */ int mY;
    /* 0xB4 */ const ScreenRect *mTouchRect; // Relative to the position
    /* 0xB8 */ BOOL mEnabled;
    /* 0xBC */ BOOL mReleased;
}; // 0xC0

// One of the three buttons (kind 0 has a text label that moves down two pixels with the button when it is held).
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

    /* 0xC0 */ s16 mKind;
    /* 0xC4 */ int mState;
    /* 0xC8 */ MessageText *mLabel;
}; // 0xCC

#endif // TOUCHBUTTON_H
