#ifndef TEXT_SPEECHWINDOW_H
#define TEXT_SPEECHWINDOW_H

#include <nitro.h>
#include <nnsys.h>

#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// The speech window: a message on sprites, shown on some planes of the screens and hidden again either when the
// speech engine is done (and at least 30 frames have passed) or after a number of frames. About fifty overlays
// use it for what a character says.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SpeechWindow {
public:
    // How the window is hidden again (mMode)
    enum Mode {
        MODE_UNTIL_SPOKEN, // When the speech player's engine is idle, after more than 30 frames
        MODE_FRAMES,       // After mFrames frames
        MODE_MANUAL,       // Only by Hide()
    };

    // Sets the text up on the planes "planes" of "screen", 28 characters wide and 3 lines high (4 if "tall"), from
    // the character "charOffset" on. Returns the number of characters it takes.
    int Init(int charOffset, MessageFile *file, u32 planes, int screen, TtsPlayer *player, BOOL tall);
    void Show(u32 message, int mode, int frames);
    void Hide();
    void Update();
    void SetStringArg(const wchar_t *text, int index);
    void SetMessageArg(const wchar_t *text, int index);
    void Draw();

    /* 0x000 */ MessageFile *mMessageFile;
    /* 0x004 */ TtsPlayer *mTtsPlayer;
    /* 0x008 */ MessageText mText;
    /* 0x5F8 */ int mMode;
    /* 0x5FC */ int mFrames;
    /* 0x600 */ int mFrame; // Frames since Show()
    /* 0x604 */ u32 mPlanes;
    /* 0x608 */ BOOL mShown;
}; // 0x60C

#endif // TEXT_SPEECHWINDOW_H
