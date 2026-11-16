#include "KeyboardTestScene.h"

extern "C" {
// The graphics manager: saves and restores the graphics state of planes
void func_02024b10(u32 *manager, void *state, int planes);
void func_02024c14(u32 *manager, void *state, int planes);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_02028154(ScreenLoader *loader, int plane, int offset);
u16 KeyboardCore_GetChar(void);
}

// The library's character type is u16, not this compiler's wchar_t (as in overlay 0's TextTestScene)
static inline const NNSG2dChar *ToG2dChars(const wchar_t *text) {
    return (const NNSG2dChar *)text;
}

void KeyboardTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "KBD", "Cmn/Kbd/SoftKeyboard_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "MNU", "Scn/Scene/ScenePlayRoom2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
}

void KeyboardTestScene::Init() {
    MI_CpuClear16(mName, sizeof(mName));
    mLength = 0;
    mFiles[0].Open("TBM:/tmd_common_msg_JP.bmg");
    int charOffset = 0x384;
    mTextBox.Init(0x1000, 24, 2, charOffset, 0, 0, 0, 0, mCharBuffer);
    mTextBox.mColor = 8;
    mTextBox.SetPositionFromVec(0x28000, 0x14000);
    mTextBox.Transfer();
    charOffset += mTextBox.GetCharCount();
    mCharBox.Init(0x1000, 24, 2, charOffset, 0, 0, NULL);
    mCharBox.mColor = 8;
    mCharBox.SetPositionFromVec(0x28000, 0x28000);
    charOffset += mTextBox.GetCharCount();
    func_02024b10(gGraphicsManager, mSavedGraphics, 0x3F00);
    func_02027d7c(&data_02121834, "KBD:/bg_keyboard_10.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "KBD:/bg_keyboard.ncg", 0x400, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "KBD:/keyboardColor.ncl", 0x3F00, -1, -1, -1);
    data_0211d604.Show(0x400);
    mKeyboard.SetArchiveName("KBD");
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x08000,
                      GX_BG_EXTPLTT_01);
    data_0211d604.Show(0x100);
    mKeyboard.InitBg(0x100, 0);
    mKeyboard.SetMode(0, FALSE);
    mKeyboard.InitObj(0, &mFiles[0], charOffset, -1, 0, 13);
    mKeyboard.EnableTabs();
    mKeyboard.GetButtonCharCount();
}

void KeyboardTestScene::Update() {
    mKeyboard.Update(0, gTouchInput.mTp);
    if (gTouchInput.mTouched) {
        ScreenPoint point = gTouchInput.mPoint;
        if (mKeyboard.mButtons[0].Contains(point)) {
            Sprite_SetPlaying(&mKeyboard.mButtons[0], TRUE);
            Sprite_SetFrame(SecondButton(), 0);
            Sprite_SetPlaying(SecondButton(), FALSE);
        } else if (SecondButton()->Contains(point)) {
            Sprite_SetFrame(&mKeyboard.mButtons[0], 0);
            Sprite_SetPlaying(&mKeyboard.mButtons[0], FALSE);
            Sprite_SetPlaying(SecondButton(), TRUE);
        } else {
            wchar_t c = KeyboardCore_GetChar();
            if (c != 0) {
                mUnknown6008 = 0;
                if (c >= L' ' && mLength < NAME_LENGTH) {
                    mName[mLength] = c;
                    mTextBox.SetText(ToG2dChars(mName));
                    mTextBox.Redraw();
                    mTextBox.Transfer();
                    int x = mLength * 10 + 5;
                    const NNSG2dFont *font = mCharBox.mFont;
                    mCharBox.mCharX = x - (NNS_G2dFontGetCharWidths(font, c)->charWidth >> 1);
                    mCharBox.SetChar(c);
                    mCharBox.Redraw();
                    mLength++;
                }
            }
        }
    }
    if (gTouchInput.mTp.touch != 0 || gTouchInput.mUnknown4C != 0) {
        mKeyboard.TransferCharData();
    }
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
        func_02024c14(gGraphicsManager, mSavedGraphics, 0x3F00);
        func_02028108(&data_02121834, 0x100, 0);
        func_02028154(&data_02121834, 0x100, 0);
        func_02027d7c(&data_02121834, "MNU:/BG_down.nsc", 0x100, 0, 0, 0, 0, -1);
        func_02028f24(&gVramTransferQueue, "MNU:/BG_down.ncg", 0x100, 0, 0, 0, -1);
        func_02027534(&data_021213b8, "MNU:/BG_down.ncl", 0x100, -1, -1, -1);
        data_0211d604.mPlanes &= ~0x400;
        data_0211d604.mPlanesChanged = TRUE;
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_B) {
        func_02024c14(gGraphicsManager, mSavedGraphics, 0x3F00);
        func_02027d7c(&data_02121834, "KBD:/bg_keyboard0.nsc", 0x400, 0, 0, 0, 0, -1);
        func_02028f24(&gVramTransferQueue, "KBD:/bg_keyboard.ncg", 0x400, 0, 0, 0, -1);
        func_02027534(&data_021213b8, "KBD:/keyboardColor.ncl", 0x3F00, -1, -1, -1);
        data_0211d604.Show(0x400);
        G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                          GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
        data_0211d604.Show(0x100);
        mKeyboard.InitBg(0x100, 0);
        mKeyboard.LoadObj(0, -1, 13);
    }
}

void KeyboardTestScene::Draw() {
    mTextBox.Draw();
    mCharBox.Draw();
    mKeyboard.Draw();
}

void KeyboardTestScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "MNU");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
}
