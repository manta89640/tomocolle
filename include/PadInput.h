#ifndef PADINPUT_H
#define PADINPUT_H

#include <nitro.h>

// The buttons, read once a frame by main() (PadInput::Update). Nothing is pressed while the lid is closed or the
// screens are off (gLcdPower).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. The overlays' sources declare
// the object with stand-ins of their own (PadInput, PadInputState).
class PadInput {
public:
    // Empty, and inline: the file's static initialiser is a lone return (0x020B04EC)
    PadInput() {}
    void Init();
    void Update();

    /* 0x00 */ u32 mTrigger;     // Pressed this frame
    /* 0x04 */ u32 mHeld;        // Held
    /* 0x08 */ u32 mRepeat;      // Pressed this frame, then every 8 frames after 24 frames of the same buttons
    /* 0x0C */ u32 mPrevHeld;    // mHeld of the frame before; cleared when the buttons held change
    /* 0x10 */ u32 mRepeatTimer; // Frames the same buttons have been held
    /* 0x14 */ int mUnknown14;   // Cleared by Init, not used by the file
    /* 0x18 */ int mUnknown18;   // Cleared by Init, not used by the file
    /* 0x1C */ BOOL mFolded;     // The lid is closed
    /* 0x20 */ BOOL mActive;     // The buttons were read this frame
}; // 0x24

// main() initialises it and updates it every frame. It keeps its address as its name: the overlays' sources refer
// to it by that name.
extern PadInput data_0211f9d4;

#endif // PADINPUT_H
