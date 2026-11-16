#ifndef SCENE_TTSTESTSCENE_H
#define SCENE_TTSTESTSCENE_H

#include "Scene.h"
#include "voice/TtsPlayer.h"

// Scene 14 (main program, no overlay): a test of the speech synthesizer (TtsPlayer). A speaks a weather report
// ("Nara prefecture, tomorrow's weather...") and a test sentence with an echo straight to the stream, B stops the
// engine; with Y held, right synthesizes the short test sentence into the capture buffer and plays it, left stops
// that, up synthesizes it on the player's thread and down plays the capture once that thread is done. The scene
// factory (main func_0203c050, case 14) allocates 0x6A858 bytes.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class TtsTestScene : public Scene {
public:
    TtsTestScene(int sceneId);

    enum { CAPTURE_SAMPLES = 0x2AF80 };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ TtsPlayer mPlayer;
    /* 0x14954 */ u16 mUnknown14954;
    /* 0x14956 */ u8 mUnknown14956[2];
    /* 0x14958 */ s16 mCaptureBuffer[CAPTURE_SAMPLES];
}; // 0x6A858

#endif // SCENE_TTSTESTSCENE_H
