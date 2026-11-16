#ifndef OV075_SCENE40SPEECHWINDOW_H
#define OV075_SCENE40SPEECHWINDOW_H

#include "text/SpeechWindow.h"

class Scene40SpeechWindow : public SpeechWindow { public: void Update(); void Show(u32 message, int mode, int frames); /* 0x60C */ BOOL mEnabled; }; // 0x610

#endif // OV075_SCENE40SPEECHWINDOW_H
