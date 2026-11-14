#ifndef SPEECHBALLOON_H
#define SPEECHBALLOON_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageText.h"

// A panel sprite (sequence 4 of the scene's sprite bank 1, in the middle of the screen) with two lines of
// message text on it, each of which takes a name as its first argument. It shows itself for three seconds.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class NamePanel : public Sprite {
public:
    enum State {
        STATE_IDLE,
        STATE_SHOW_BOTH,  // Draw both lines and show the panel
        STATE_SHOW_FIRST, // The same with the second line hidden
        STATE_SHOWN,      // Hide after 180 frames
        STATE_HIDE,
    };

    NamePanel();
    int Init(int charOffset, MessageFile *file1, MessageFile *file2);
    void Update();
    void Draw();
    void SetState(int state);
    void SetNames(const wchar_t *name1, const wchar_t *name2);
    void SetTextVisible(BOOL visible);
    void SetVisible(BOOL visible);

    /* 0x000 */ // Sprite
    /* 0x0AC */ MessageText mText1;
    /* 0x69C */ MessageText mText2;
    /* 0xC8C */ int mState;
    /* 0xC90 */ s16 mStep;
    /* 0xC92 */ s16 mFrames;
}; // 0xC94

#endif // SPEECHBALLOON_H
