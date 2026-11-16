#include "TtsTestScene.h"
#include "MainProgram3d.h"

// What the scene speaks: the text in the engine's notation, as wide characters (A) and in Shift JIS (the second
// is not used)
struct TtsTestTexts {
    const wchar_t *mWide;
    const char *mUnused;
    const char *mCapture;
};

static TtsTestTexts sTexts = {
    L"ナラ’ケンノ＿アスノ／テ’ンキワ＿ハレ＿＿トコロニヨ’リ＿ニワカア’メガ／フ’ルデショー．\t<\\Echo>コレワ_テ'ストデス．アイウエオカキクケコサシスセソタチツテト.",
    "ナラ’ケンノ＿アスノ／テ’ンキワ＿ハレ＿＿トコロニヨ’リ＿ニワカア’メガ／フ’ルデショー．",
    "<\\Echo>コレワ_テ'ストデス.",
};

void TtsTestScene::StartLoading() {
}

void TtsTestScene::Init() {
    mUnknown14954 = 0;
    mPlayer.InitEngine();
    mPlayer.SetCaptureBuffer(mCaptureBuffer, CAPTURE_SAMPLES);
}

void TtsTestScene::Update() {
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
        mPlayer.Speak((const u16 *)sTexts.mWide, 0);
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_B) {
        mPlayer.StopEngine();
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_Y) {
        if (data_0211f9d4.mTrigger & PAD_KEY_RIGHT) {
            mPlayer.StopEngine();
            mPlayer.mCaptureText = sTexts.mCapture;
            mPlayer.Synthesize(NULL);
            mPlayer.PlayCapture();
        } else if (data_0211f9d4.mTrigger & PAD_KEY_LEFT) {
            mPlayer.StopCapture();
        } else if (data_0211f9d4.mTrigger & PAD_KEY_UP) {
            mPlayer.StopEngine();
            mPlayer.mCaptureText = sTexts.mCapture;
            mPlayer.StartSynthesizeThread();
        } else if (data_0211f9d4.mTrigger & PAD_KEY_DOWN) {
            if (OS_IsThreadTerminated(&mPlayer.mThread)) {
                mPlayer.PlayCapture();
            }
        }
    }
    mPlayer.Update();
}

void TtsTestScene::Draw() {
}

void TtsTestScene::Exit() {
}
