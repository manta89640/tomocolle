#ifndef OV075_SCENE40SPEECHWINDOW_H
#define OV075_SCENE40SPEECHWINDOW_H

#include <nitro.h>

#include "text/SpeechWindow.h"

// The scene's speech window (scene+0x163B0): the main program's, shown only while the scene allows it and the
// screens are not switching (gScreenControl.mUnknown44 clear), and hidden by Update otherwise
class Scene40SpeechWindow : public SpeechWindow {
public:
    void Update();
    void Show(u32 message, int mode, int frames);

    /* 0x60C */ BOOL mEnabled;
}; // 0x610

#endif // OV075_SCENE40SPEECHWINDOW_H
