#ifndef OV073_TUTORIALTOUCHBUTTON_H
#define OV073_TUTORIALTOUCHBUTTON_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageText.h"

// The button of the tutorial's touch screen: overlay 74's classes in the tutorial's cut. TutorialTouchSprite has no
// mReleased here (and no SetReleased), so it is 0xBC bytes and TutorialTouchButton 0xC8, with mKind, mState and
// mLabel four bytes earlier than overlay 74's; mKind is an int here.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. Overlay 74 has the
// same classes without the prefix, which tells the two apart: the program is linked as one, so they cannot have
// one name.

// A sprite with a rectangle around its position that can be touched.
class TutorialTouchSprite : public Sprite {
public:
    TutorialTouchSprite();
    BOOL Contains(ScreenPoint point);

    /* 0xAC */ int mX; // In pixels
    /* 0xB0 */ int mY;
    /* 0xB4 */ const ScreenRect *mTouchRect; // Relative to the position
    /* 0xB8 */ BOOL mEnabled;
}; // 0xBC

// The button (the tutorial has one, of kind 0: it has a text label that moves down two pixels with the button
// when it is held).
class TutorialTouchButton : public TutorialTouchSprite {
public:
    enum State {
        STATE_IDLE,
        STATE_PRESS,
        STATE_RELEASE,
        STATE_PLAY,
    };

    TutorialTouchButton();
    void Init(int kind);
    void Update();
    void Draw();
    BOOL Contains(ScreenPoint point);

    /* 0xBC */ int mKind;
    /* 0xC0 */ int mState;
    /* 0xC4 */ MessageText *mLabel;
}; // 0xC8

#endif // OV073_TUTORIALTOUCHBUTTON_H
