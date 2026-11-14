#ifndef TIMEDMESSAGE_H
#define TIMEDMESSAGE_H

#include <nitro.h>
#include <nnsys.h>

#include "text/MessageText.h"

// A message text that shows itself (at once or line by line), stays for three seconds and hides again. While it
// is shown it sets bit 1 of the display state data_0211d604.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class TimedMessage : public MessageText {
public:
    enum State {
        STATE_IDLE,
        STATE_SHOW,         // Draw the whole message, then wait
        STATE_SHOW_BY_LINE, // Draw it line by line, show it when it is complete, then wait
        STATE_WAIT,         // 180 frames
        STATE_HIDE,
    };

    TimedMessage();
    void Update();
    void SetState(int state);
    void SetVisible(BOOL visible);

    /* 0x588 */ int mState;
    /* 0x58C */ s16 mStep;
    /* 0x58E */ s16 mFrames;
}; // 0x590 without the canvas, 0x5F8 with it

#endif // TIMEDMESSAGE_H
