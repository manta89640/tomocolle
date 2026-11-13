#include "voice/TtsPlayer.h"

extern "C" {

// The engine, which is not decompiled yet
void func_0208d8fc(void *engine);
void func_0208dab4(void *engine);
void func_0208d878(void *engine);
void func_0208db2c(void *engine, int param, int value);
void func_0208dca4(void *engine);
int func_0208dac4(void *engine);
void func_0208daf8(void *engine);
typedef void (*EngineCallback)(int samples, s16 *data, TtsPlayer *player);
void func_0208d500(void *engine, const char *text, int param, int unk, EngineCallback callback, TtsPlayer *player);

// The sound manager's volumes, and the mutex around its use of the sound library
int func_02035bf4(void *);
int func_02035c14(void *);
extern u8 data_0213b5cc[];
extern OSMutex data_0213b698;

} // extern "C"

#define STREAM_BUFFER_SIZE 0x12C0

static TtsPlayer *sInstance;
static u8 sStreamBuffer[STREAM_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);

TtsPlayer::TtsPlayer() {
    sInstance = this;
    mEngine = NULL;
    mState = STATE_CLOSED;
    mStreamPos = NULL;
    mBufferedCount = 0;
    mEngineDone = 0;
    mCaptureBuffer = NULL;
    mCaptureText = NULL;
    mCapturePos = 0;
    mWaveOut = NULL;
    mThread.state = OS_THREAD_STATE_TERMINATED;
}

TtsPlayer::~TtsPlayer() {
    if (mEngine != NULL) {
        StopEngine();
    }
    if (mState != STATE_CLOSED) {
        CloseStream();
    }
    if (mWaveOut != NULL) {
        if (IsCapturePlaying()) {
            StopCapture();
        }
    }
    if (!OS_IsThreadTerminated(&mThread)) {
        OS_DestroyThread(&mThread);
    }
    sInstance = NULL;
}

void TtsPlayer::InitEngine() {
    mEngine = mEngineWork;
    func_0208d8fc(mEngine);
    func_0208db2c(mEngine, 5, 0x82);
}

void TtsPlayer::func_02083f8c() {
    func_0208dab4(mEngine);
}

void TtsPlayer::StopEngine() {
    func_0208d878(mEngine);
}

void TtsPlayer::SetEngineParam(int param, int value) {
    func_0208db2c(mEngine, param, value);
}

void TtsPlayer::func_02083fbc() {
    func_0208dca4(mEngine);
}

BOOL TtsPlayer::IsEngineIdle() {
    return func_0208dac4(mEngine) == 0;
}

void TtsPlayer::func_02083fe8() {
    func_0208daf8(mEngine);
}

static void StreamCallback(NNSSndStrmCallbackStatus status, int numChannels, void *buffer[], u32 len,
                           NNSSndStrmFormat format, void *arg);
static void StreamSamples(int samples, s16 *data, TtsPlayer *player);

int TtsPlayer::OpenStream(u8 channel) {
    u8 channels[1];

    if (mState != STATE_CLOSED) {
        return -1;
    }
    mState = STATE_OPENED;
    mStreamPos = (s16 *)sStreamBuffer;
    mBufferedCount = 0;
    mEngineDone = 0;
    MIi_CpuClearFast(0, sStreamBuffer, STREAM_BUFFER_SIZE);

    OS_LockMutex(&data_0213b698);
    NNS_SndStrmInit(&mStream);
    channels[0] = channel;
    if (!NNS_SndStrmAllocChannel(&mStream, 1, channels)) {
        OS_UnlockMutex(&data_0213b698);
        mState = STATE_CLOSED;
        return -1;
    }
    if (!NNS_SndStrmSetup(&mStream, NNS_SND_STRM_FORMAT_PCM16, sStreamBuffer, STREAM_BUFFER_SIZE, 0x17, 5,
                          StreamCallback, this)) {
        NNS_SndStrmFreeChannel(&mStream);
        OS_UnlockMutex(&data_0213b698);
        mState = STATE_CLOSED;
        return -1;
    }
    NNS_SndStrmSetChannelVolume(&mStream, 0, func_02035bf4(data_0213b5cc));
    OS_UnlockMutex(&data_0213b698);
    return 0;
}

void TtsPlayer::CloseStream() {
    if (mState == STATE_CLOSED) {
        return;
    }
    OS_LockMutex(&data_0213b698);
    NNS_SndStrmStop(&mStream);
    NNS_SndStrmFreeChannel(&mStream);
    OS_UnlockMutex(&data_0213b698);
    mState = STATE_CLOSED;
}

// Called by the sound library each time the stream wants its next block
static void StreamCallback(NNSSndStrmCallbackStatus status, int numChannels, void *buffer[], u32 len,
                           NNSSndStrmFormat format, void *arg) {
    TtsPlayer *player = (TtsPlayer *)arg;

    player->mStreamPos = (s16 *)buffer[0];
    if (status == NNS_SND_STRM_CALLBACK_SETUP) {
        player->mState = TtsPlayer::STATE_FILLING;
        return;
    }
    if (player->mBufferedCount != 0) {
        player->mBufferedCount--;
        return;
    }
    MIi_CpuClearFast(0, buffer[0], len);
}

// Called by the engine with each block of samples it synthesizes, and with none when it is done
static void StreamSamples(int samples, s16 *data, TtsPlayer *player) {
    if (samples <= 0) {
        player->mEngineDone = 1;
        if (samples == -1) {
            player->mBufferedCount = 0;
        }
        OS_Sleep(5);
        return;
    }
    if (player->mState != TtsPlayer::STATE_FILLING && player->mState != TtsPlayer::STATE_PLAYING) {
        return;
    }
    while (player->mBufferedCount >= 5) {
        if (func_0208dac4(player->mEngine) != 1) {
            return;
        }
        OS_Sleep(10);
    }
    MIi_CpuCopy32(data, player->mStreamPos, samples * 2);
    player->mBufferedCount++;
    if (player->mState != TtsPlayer::STATE_FILLING) {
        return;
    }
    player->mStreamPos += samples;
    OS_Sleep(5);
}

void TtsPlayer::Update() {
    if (mState <= STATE_OPENED) {
        return;
    }
    if (mState == STATE_FILLING) {
        if (mEngineDone != 0) {
            mState = STATE_DRAINING;
            OS_LockMutex(&data_0213b698);
            NNS_SndStrmStart(&mStream);
            OS_UnlockMutex(&data_0213b698);
        } else if (mBufferedCount >= 5) {
            mState = STATE_PLAYING;
            OS_LockMutex(&data_0213b698);
            NNS_SndStrmStart(&mStream);
            OS_UnlockMutex(&data_0213b698);
        }
    } else if (mState == STATE_PLAYING) {
        if (mEngineDone != 0) {
            mState = STATE_DRAINING;
        }
    } else if (mState == STATE_DRAINING && mBufferedCount == 0) {
        CloseStream();
    }
}

int TtsPlayer::Speak(const char *text, int param) {
    Stop();
    if (OpenStream(4) != 0) {
        return -1;
    }
    func_0208d500(mEngine, text, (u8)param, 1, StreamSamples, this);
    return 0;
}

void TtsPlayer::Stop() {
    func_0208d878(mEngine);
    CloseStream();
}

int TtsPlayer::Speak(const u16 *text, int param) {
    // Room for the text and the two zero bytes that end it
    char sjis[0x200 + 2];
    int length = 0x200;

    STD_ConvertStringUnicodeToSjis(sjis, &length, text, NULL, NULL);
    MI_CpuFill8(sjis + length, 0, 2);
    return Speak(sjis, param);
}

void TtsPlayer::SetCaptureBuffer(s16 *buffer, int samples) {
    mCaptureSamples = samples;
    mCaptureBuffer = buffer;
    ClearCaptureBuffer();
}

void TtsPlayer::ClearCaptureBuffer() {
    mCapturePos = 0;
    MIi_CpuClearFast(0, mCaptureBuffer, mCaptureSamples * 2);
}

void TtsPlayer::PlayCapture() {
    OS_LockMutex(&data_0213b698);
    NNSSndWaveOutHandle handle = NNS_SndWaveOutAllocChannel(4);
    if (handle == NULL) {
        OS_UnlockMutex(&data_0213b698);
        return;
    }
    mWaveOut = handle;
    NNS_SndWaveOutStart(mWaveOut, NNS_SND_WAVE_FORMAT_PCM16, mCaptureBuffer, FALSE, 0, mCaptureSamples, 22000,
                        func_02035c14(data_0213b5cc), 0x8000, 0x40);
    NNS_SndWaveOutFreeChannel(mWaveOut);
    OS_UnlockMutex(&data_0213b698);
}

void TtsPlayer::StopCapture() {
    if (mWaveOut == NULL) {
        return;
    }
    OS_LockMutex(&data_0213b698);
    NNS_SndWaveOutStop(mWaveOut);
    OS_UnlockMutex(&data_0213b698);
}

BOOL TtsPlayer::IsCapturePlaying() {
    OS_LockMutex(&data_0213b698);
    BOOL playing = NNS_SndWaveOutIsPlaying(mWaveOut);
    OS_UnlockMutex(&data_0213b698);
    return playing;
}

// Called by the engine with each block of samples, when they are to be kept instead of played
static void CaptureSamples(int samples, s16 *data, TtsPlayer *player) {
    int count = player->mCaptureSamples - player->mCapturePos;
    if (count > samples) {
        count = samples;
    }
    if (count <= 0) {
        return;
    }
    MI_CpuCopy8(data, player->mCaptureBuffer + player->mCapturePos, count * 2);
    player->mCapturePos += count;
}

void TtsPlayer::Synthesize(const char *text) {
    ClearCaptureBuffer();
    if (text == NULL) {
        text = mCaptureText;
    }
    func_0208d500(mEngine, text, 0, 1, CaptureSamples, this);
    while (func_0208dac4(mEngine) == 1) {
        OS_Sleep(20);
    }
}

static void SynthesizeThread(void *arg) {
    TtsPlayer *player = (TtsPlayer *)arg;

    player->ClearCaptureBuffer();
    func_0208d500(player->mEngine, player->mCaptureText, 0, 1, CaptureSamples, player);
    while (func_0208dac4(player->mEngine) == 1) {
        OS_Sleep(5);
    }
}

void TtsPlayer::StartSynthesizeThread() {
    if (OS_IsThreadTerminated(&mThread) != TRUE) {
        return;
    }
    OS_CreateThread(&mThread, SynthesizeThread, this, &mThread, sizeof(mThreadStack), 20);
    OS_WakeupThreadDirect(&mThread);
}
