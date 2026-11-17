#ifndef OV075_SCENE40CLOCK_H
#define OV075_SCENE40CLOCK_H

#include <nitro.h>

// The scene's clock, at scene+0x30A8C: the date and time last read (with the game's own offset to the real-time
// clock) and the same moment in seconds, as overlay 74's Clock and overlay 72's MapClock have them, in another order.
// Its constructor is the overlay's (0x02179728); the scene's constructor in the main program calls it. The talk
// chooses its messages by this date and time, and Exit gives the character the date as the date they last met.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class Scene40Clock {
public:
    Scene40Clock();
    void Update();
    void UpdateIfArchiveIdle();

    /* 0x00 */ RTCDate mDate;
    /* 0x10 */ RTCTime mTime;
    /* 0x1C */ s64 mSeconds; // The same moment as seconds, as Clock_GetDateTime returns it
}; // 0x24

#endif // OV075_SCENE40CLOCK_H
