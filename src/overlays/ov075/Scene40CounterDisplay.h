#ifndef OV075_SCENE40COUNTERDISPLAY_H
#define OV075_SCENE40COUNTERDISPLAY_H

#include <nitro.h>

#include "Sprite.h"
#include "text/TextCanvas.h"

struct Scene40Context;

// The scene's counter (pages + 0x386B0, scene 0x69200): the number the context's mUnknown10 points at (the scene's
// copy of GameData's capped counter, 0 to 9,999,999, which Exit writes back), with commas, beside an icon. When the
// number changes it counts there 50 a frame with sound 4 or 5 of archive 27, and 6 or 7 when it arrives. Hidden
// while the speech window is shown.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40CounterDisplay {
public:
    enum State {
        STATE_IDLE,
        STATE_START,
        STATE_COUNTING,
        STATE_ARRIVED,
    };

    void Init(Scene40Context *context, int *charOffset);
    void Update();
    void Draw();
    void Add(int amount);
    void AddTimes60(int count);
    void Redraw();

    /* 0x000 */ Scene40Context *mContext;
    /* 0x004 */ int mState;
    /* 0x008 */ BOOL mFalling;
    /* 0x00C */ int *mCounter; // What it shows: the context's mUnknown10
    /* 0x010 */ int mShown;    // The number shown, counting to *mCounter
    /* 0x014 */ int mChange;   // *mCounter - mShown when the counter last changed
    /* 0x018 */ Sprite mIcon;
    /* 0x0C4 */ ObjTextCanvas mText;
}; // 0x27C

#endif // OV075_SCENE40COUNTERDISPLAY_H
