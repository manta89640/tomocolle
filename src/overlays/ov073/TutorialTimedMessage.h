#ifndef OV073_TUTORIALTIMEDMESSAGE_H
#define OV073_TUTORIALTIMEDMESSAGE_H

#include <nitro.h>
#include <nnsys.h>

#include "text/MessageText.h"

// A message text that shows itself (at once or line by line), stays for three seconds and hides again. While it
// is shown it sets bit 1 of the display state data_0211d604.
//
// The name is descriptive, not the original: no name of this class survives in the ROM. Overlay 74 has the same
// class without the prefix, which tells the two apart: the program is linked as one, so they cannot have one name.
class TutorialTimedMessage : public MessageText {
public:
    enum State {
        STATE_IDLE,
        STATE_SHOW,         // Draw the whole message, then wait
        STATE_SHOW_BY_LINE, // Draw it line by line, show it when it is complete, then wait
        STATE_WAIT,         // 180 frames
        STATE_HIDE,
    };

    TutorialTimedMessage();
    void Update();
    void SetState(int state);
    void SetVisible(BOOL visible);

    /* 0x588 */ int mState;
    /* 0x58C */ s16 mStep;
    /* 0x58E */ s16 mFrames;
}; // 0x590 without the canvas, 0x5F8 with it

#endif // OV073_TUTORIALTIMEDMESSAGE_H
