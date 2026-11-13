#ifndef VOICE_TTSPLAYER_H
#define VOICE_TTSPLAYER_H

#include <nitro.h>
#include <nnsys.h>

// The speech synthesizer: the game's wrapper around Sharp's text-to-speech engine (SHTTS 1.2.0), which it keeps
// inside itself. It plays what the engine synthesizes in one of two ways: straight to a sound stream as it is
// produced, or into a buffer of the caller's that is played afterwards as one wave.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class TtsPlayer {
public:
    enum State {
        STATE_CLOSED,   // No stream
        STATE_OPENED,   // Stream set up, waiting for its first callback
        STATE_FILLING,  // Collecting samples before starting the stream
        STATE_PLAYING,  // Stream started, engine still synthesizing
        STATE_DRAINING, // Engine done, playing out what is buffered
    };

    TtsPlayer();
    ~TtsPlayer();

    void InitEngine();
    void func_02083f8c();
    void StopEngine();
    void SetEngineParam(int param, int value);
    void func_02083fbc();
    BOOL IsEngineIdle();
    void func_02083fe8();

    int OpenStream(u8 channel);
    void CloseStream();
    void Update();
    int Speak(const char *text, int param);
    void Stop();
    int Speak(const u16 *text, int param);

    void SetCaptureBuffer(s16 *buffer, int samples);
    void ClearCaptureBuffer();
    void PlayCapture();
    void StopCapture();
    BOOL IsCapturePlaying();
    void Synthesize(const char *text);
    void StartSynthesizeThread();

    /* 0x00000 */ u32 mState;
    /* 0x00004 */ void *mEngine;
    /* 0x00008 */ u8 mEngineWork[0x14000];
    /* 0x14008 */ NNSSndStrm mStream;
    /* 0x14064 */ s16 *mStreamPos;     // Where in the stream's buffer the next samples go
    /* 0x14068 */ int mBufferedCount;  // Blocks of samples written and not yet played
    /* 0x1406C */ int mEngineDone;
    /* 0x14070 */ s16 *mCaptureBuffer;
    /* 0x14074 */ int mCaptureSamples;
    /* 0x14078 */ int mCapturePos;
    /* 0x1407C */ const char *mCaptureText;
    /* 0x14080 */ NNSSndWaveOutHandle mWaveOut;
    /* 0x14084 */ u8 mThreadStack[0x800];
    /* 0x14884 */ OSThread mThread;
};

#endif // VOICE_TTSPLAYER_H
