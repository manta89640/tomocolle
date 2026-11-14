#ifndef OV073_TUTORIALCLOCK_H
#define OV073_TUTORIALCLOCK_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"

// The apartment scene's clock: the current date and time, read through the main program's func_02035870 (which
// applies the game's own offset to the real-time clock), and the sprites that show them.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. Overlay 74 has the
// same classes without the prefix, which tells the two apart: the program is linked as one, so they cannot have
// one name.
class TutorialClock {
public:
    TutorialClock();
    void Update();
    void UpdateIfArchiveIdle();

    /* 0x00 */ s64 mSeconds; // The same moment as seconds, as func_02035870 returns it
    /* 0x08 */ RTCTime mTime;
    /* 0x14 */ RTCDate mDate;
}; // 0x24

class TutorialClockDisplay {
public:
    enum State {
        STATE_SHOWN,
        STATE_HIDE,   // Hide every sprite and start waiting
        STATE_HIDDEN, // Show them again after 190 frames
    };

    enum {
        SPRITE_MONTH_TENS,
        SPRITE_MONTH_ONES,
        SPRITE_2,
        SPRITE_DAY_TENS,
        SPRITE_DAY_ONES,
        SPRITE_HALF_OF_DAY, // Hour / 12
        SPRITE_HOUR_TENS,
        SPRITE_HOUR_ONES,
        SPRITE_8,
        SPRITE_MINUTE_TENS,
        SPRITE_MINUTE_ONES,
        SPRITE_COUNT
    };

    void Init(TutorialClock *clock);
    void Update();
    void Refresh(BOOL all);
    void Draw();
    void SetState(int state);
    void SetVisible(BOOL visible);

    /* 0x000 */ TutorialClock *mClock;
    /* 0x004 */ int mState;
    /* 0x008 */ Sprite mSprites[SPRITE_COUNT];
    // What the sprites show now
    /* 0x76C */ u32 mUnknown76C;
    /* 0x770 */ u32 mMonth;
    /* 0x774 */ u32 mDay;
    /* 0x778 */ u32 mUnknown778;
    /* 0x77C */ u32 mHour;
    /* 0x780 */ u32 mMinute;
    /* 0x784 */ u32 mUnknown784;
    /* 0x788 */ s16 mUnknown788;
    /* 0x78A */ s16 mHiddenFrames;
}; // 0x78C

#endif // OV073_TUTORIALCLOCK_H
