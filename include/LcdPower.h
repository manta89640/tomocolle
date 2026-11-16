#ifndef LCDPOWER_H
#define LCDPOWER_H

#include <nitro.h>

// Turns the screens off while the lid is closed: after 20 frames closed it turns the LCDs off, and after 10 frames
// open it turns them on again. The scene manager updates it while a scene runs; PadInput reads no buttons while
// the screens are off.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class LcdPower {
public:
    // Empty, and inline: the file's static initialiser is a lone return (0x020B05D0)
    LcdPower() {}
    void Init();
    void Update();
    void SetMode(int mode);
    BOOL TurnOff();
    BOOL TurnOn();

    /* 0x00 */ int mMode;    // 1: watch the lid (from Init); 0: do nothing
    /* 0x04 */ BOOL mLcdOff; // The screens are off
    /* 0x08 */ int mFrames;  // Frames the lid has been closed (while on) or open (while off)
}; // 0xC

extern LcdPower gLcdPower;

#endif // LCDPOWER_H
