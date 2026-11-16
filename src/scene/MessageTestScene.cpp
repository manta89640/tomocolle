#include "MessageTestScene.h"
#include "MainProgram3d.h"

extern "C" {
extern Font gFonts[3];
}

// The library's character type is u16, not this compiler's wchar_t (as in overlay 0's TextTestScene)
static inline const NNSG2dChar *ToG2dChars(const wchar_t *text) {
    return (const NNSG2dChar *)text;
}

// The string put in the second background text's first tag
static const wchar_t *sStringArg = L"あいうえお";

void MessageTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ITM", "Msg/TmdItemMsgJP_LZ.bin", 0);
}

#pragma opt_propagation off
void MessageTestScene::Init() {
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x2800, 0x101);
    mMessage = 0;
    mFiles[0].Open("TBM:/tmd_etc_msg_JP.bmg");
    mFiles[1].Open("ITM:/tmd_item_msg_JP.bmg");

    int charOffset = 0;
    mCanvases[0].InitText(&gFonts[0].mFont, 0x10, 24, 8, charOffset, 1, 1, 0, 0, 15, NULL, 2,
                          NNS_G2D_CHARA_COLORMODE_16);
    mCanvases[0].mText = ToG2dChars(L"\xE000\xE001\n\xE008\xE000\xE029");
    NNS_G2dCharCanvasClear(&mCanvases[0].mCanvas, 0);
    mCanvases[0].SetPosition(0x2000, 0x2000);
    mCanvases[0].mTextX = 0x60;
    mCanvases[0].mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_CENTER | NNS_G2D_HORIZONTALALIGN_CENTER;
    mCanvases[0].DrawText();
    mObjTexts[0].InitObjForMessage(&mFiles[0], charOffset, &gFonts[0].mFont, 0x10,
                                   charOffset + mCanvases[0].GetCharCount(), 0, 1, 0, 0, 15, NULL);
    mObjTexts[0].DrawAll();
    mObjTexts[0].SetPosition(0x8000, 0x3C000);

    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x18000);
    MI_CpuClearFast(G2_GetBG2CharPtr(), 0x20);
    MI_CpuClearFast(G2_GetBG2ScrPtr(), 0x800);
    data_0211d604.Show(4);
    charOffset = 1;
    mBgTexts[0].InitBg(&gFonts[0].mFont, 4, 24, 8, charOffset, 16, charOffset, 0, 1, 15, mCharBuffer);
    mBgTexts[0].mMessageFile = &mFiles[0];
    mBgTexts[0].mMessage = 0;
    mBgTexts[0].mShadowDirections = CharCanvas::SHADOW_RIGHT | CharCanvas::SHADOW_RIGHT_DOWN | CharCanvas::SHADOW_DOWN;
    NNS_G2dCharCanvasClear(&mBgTexts[0].mCanvas, 3);
    mBgTexts[0].DrawAll();
    mBgTexts[0].BgTextCanvas::Transfer();
    mBgTexts[1].InitBg(&gFonts[0].mFont, 4, 24, 6, charOffset, 2,
                       charOffset + mBgTexts[0].mCanvas.areaWidth * mBgTexts[0].mCanvas.areaHeight, 0, charOffset,
                       15, NULL);
    mBgTexts[1].mMessageFile = &mFiles[0];
    mBgTexts[1].mArgMessageFile = &mFiles[1];
    mBgTexts[1].mMessage = 0;
    mBgTexts[1].DrawAll();
    mBgTexts[1].SetStringArg(sStringArg, 0);
    mBgTexts[1].SetMessageArg((u32)0, 0);
    mBgTexts[1].SetMessageArg((u32)charOffset, charOffset);
}

#pragma opt_propagation reset

void MessageTestScene::Update() {
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
        mObjTexts[0].mMessage = mMessage;
        NNS_G2dCharCanvasClear(&mObjTexts[0].mCanvas, 6);
        mObjTexts[0].mDrawMode = 2;
        mObjTexts[0].mStarted = FALSE;
        mBgTexts[0].mMessage = mMessage;
        NNS_G2dCharCanvasClear(&mBgTexts[0].mCanvas, 3);
        mBgTexts[0].mDrawMode = 1;
        mBgTexts[0].mStarted = FALSE;
        mBgTexts[0].UpdateDraw();
        mBgTexts[0].BgTextCanvas::Transfer();
        mBgTexts[1].mMessage = mMessage;
        NNS_G2dCharCanvasClear(&mBgTexts[1].mCanvas, 2);
        mBgTexts[1].mDrawMode = 2;
        mBgTexts[1].mStarted = FALSE;
        mMessage++;
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_Y) {
        mBgTexts[1].mDrawMode = 4;
        mBgTexts[1].mStarted = FALSE;
    }
    mObjTexts[0].UpdateDraw();
    mBgTexts[1].UpdateDraw();
}

void MessageTestScene::Draw() {
    mObjTexts[0].Draw();
    mCanvases[0].Draw();
}

void MessageTestScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "ITM");
}
