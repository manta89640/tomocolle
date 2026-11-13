#ifndef OV073_TOUCHBUTTON_H
#define OV073_TOUCHBUTTON_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageText.h"

// The button of the tutorial's touch screen (overlay 73): overlay 74's `TouchButton` in the tutorial's cut, 0xC8
// bytes instead of 0xCC (the scene's constructor steps over one of 0xC8, and the scene stores the label 0xC4 into
// it). Its functions are group b2's; this is what the scene's functions use of it.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class TouchButton : public Sprite {
public:
    TouchButton();       // 0x02150264
    void Init(int kind); // 0x02150280
    void Update();       // 0x02150354
    void Draw();         // 0x0215044c

    /* 0xAC */ u8 mUnknownAC[0x18];
    /* 0xC4 */ MessageText *mLabel; // The text on the button
}; // 0xC8

#endif // OV073_TOUCHBUTTON_H
