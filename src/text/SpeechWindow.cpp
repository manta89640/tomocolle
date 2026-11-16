#include "text/SpeechWindow.h"

#include "../scene/MainProgram.h"

// Where the text is on the screen, in pixels
static const WindowPoint sTextPosition = {16, 162};

int SpeechWindow::Init(int charOffset, MessageFile *file, u32 planes, int screen, TtsPlayer *player, BOOL tall) {
    mPlanes = planes;
    mMessageFile = file;
    mTtsPlayer = player;
    if (tall) {
        mText[0].InitObj(&gFonts[0].mFont, screen, 28, 4, charOffset, 0, 1, 0, 0, 15, NULL);
    } else {
        mText[0].InitObj(&gFonts[0].mFont, screen, 28, 3, charOffset, 0, 1, 0, 0, 15, NULL);
    }
    // Through a copy and SetPositionFromVec: that gives the original's dead copies on the stack, in its order
    WindowPoint position = sTextPosition;
    mText[0].ObjTextCanvas::SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    mText[0].mSprite.mOverwritePalette = 0;
    // Counted up from 0: the original adds the count to a zero (add r0, r3, #0) that a plain return folds away
    int chars = 0;
    chars += mText[0].GetCharCount();
    return chars;
}

void SpeechWindow::Show(u32 message, int mode, int frames) {
    mText[0].mMessageFile = mMessageFile;
    mText[0].mMessage = message;
    NNS_G2dCharCanvasClear(&mText[0].mCanvas, 0);
    mText[0].DrawAll();
    mText[0].mSprite.mVisible = TRUE;
    data_0211d604.Show(mPlanes);
    mMode = mode;
    mFrame = 0;
    mFrames = frames;
    mShown = TRUE;
}

void SpeechWindow::Hide() {
    data_0211d604.mPlanes &= ~mPlanes;
    data_0211d604.mPlanesChanged = TRUE;
    mText[0].mSprite.mVisible = FALSE;
    mShown = FALSE;
}

void SpeechWindow::Update() {
    switch (mMode) {
    case MODE_UNTIL_SPOKEN:
        if (mTtsPlayer != NULL && mTtsPlayer->IsEngineIdle() == TRUE && mFrame > 30) {
            Hide();
        }
        mFrame++;
        break;
    case MODE_FRAMES:
        if (mFrame >= mFrames) {
            Hide();
        }
        mFrame++;
        break;
    case MODE_MANUAL:
        break;
    }
}

void SpeechWindow::SetStringArg(const wchar_t *text, int index) {
    mText[0].SetStringArg(text, index);
}

void SpeechWindow::SetMessageArg(const wchar_t *text, int index) {
    mText[0].SetMessageArg(text, index);
}

void SpeechWindow::Draw() {
    mText[0].ObjTextCanvas::Draw();
}
