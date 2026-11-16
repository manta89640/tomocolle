#include "SaveErrorScene.h"
#include "MainProgram.h"

// Overlay 107: the screen that says the save data could not be written. One source file: SaveErrorScene and its
// virtual table, with the scene's implicit destructor at the end of .text.

// --- The file's constants ---
//
// In this order, the compiler (which sorts a file's constants by size, see docs/notes/ov073.md) puts them in the
// ROM's order.

// The texts of the yes and no buttons, common messages "はい" and "いいえ"
static const int sChoiceMessages[2] = { 2, 3 };
// Where the message window's text is
static const WindowPoint sTextPosition = { 38, 33 };
// The window's OK button: where it is and how large (the yes and no buttons are as large)
static const WindowPoint sButtonPosition = { 128, 149 };
static const WindowPoint sButtonSize = { 94, 30 };
// The rectangle of the message window, in pixels
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
// Where the yes and no buttons are
static const WindowPoint sChoicePositions[2] = { { 75, 149 }, { 181, 149 } };
// The screen (tile map) of the lower screen's background, which the window shows too
static const char sScreenName[] = "MLT:/BG_down.nsc";

// @0x02149a40
void SaveErrorScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/Scene/SceneSavePlay_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
}

// @0x02149aa0
void SaveErrorScene::Init() {
    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(31, 31, 31), 31, 0x7FFF, 63, FALSE);

    func_02028f24(&gVramTransferQueue, "MLT:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, sScreenName, 0x200, 0, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/BG_down.ncl", 0x200, -1, -1, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_down.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    data_0211d604.Set(1);
    data_0211d604.Show(0x1200);
    func_020278f8(&data_021213b8, 0x7FFF, 0x101);

    mEtcMessages.Open("TEM:tmd_etc_msg_JP.bmg");
    mCommonMessages.Open("TCM:tmd_common_msg_JP.bmg");

    int charOffset = 0x200;
    MessageFile *etc = &mEtcMessages;
    MessageFile *common = &mCommonMessages;
    ScreenRect rect = sWindowRect;
    func_02032fb8(&mWindow[0], rect);
    func_02033174(&mWindow[0], sScreenName, 0x200);
    func_020331bc(&mWindow[0], 0x3F00, rect);
    func_020336b4(&mWindow[0], MessageWindow::BUTTON_OK, 0, 0, 1, sButtonPosition, sButtonSize, common, 4,
                  &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, 10);
    for (int i = 0; i < 2; i++) {
        func_020336b4(&mWindow[0], MessageWindow::BUTTON_YES + i, 0, 0, 1, sChoicePositions[i], sButtonSize,
                      common, sChoiceMessages[i], &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, -1);
    }
    func_02033a58(&mWindow[0], etc, 0x1A2, &gFonts[0].mFont, 0x1000, sTextPosition, &charOffset);
    func_02034f94(&mWindow[0], 1);
    func_02034f4c(&mWindow[0], 1);
    func_02028280(&data_02121834, 0x200, 0);
    SetState(0);
}

// @0x02149e3c
void SaveErrorScene::Update() {
    switch (mState) {
    case 0:
        switch (mStep) {
        case 0:
            if (ShowMessage(0x1A6)) {
                mStep++;
                mSubStep = 0;
            }
            break;
        }
        break;
    }
    UpdateWindow();
}

// @0x02149e9c
void SaveErrorScene::Draw() {
    func_020345d8(&mWindow[0]);
}

// @0x02149eac
void SaveErrorScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
}

// @0x02149ee8
void SaveErrorScene::SetState(int state) {
    mState = state;
    mStep = 0;
    mSubStep = 0;
}

// @0x02149f00
void SaveErrorScene::UpdateWindow() {
    func_020341d4(&mWindow[0]);
}

// Opens the window with a message and no buttons, and returns TRUE once it is shown
// @0x02149f10
BOOL SaveErrorScene::ShowMessage(u32 message) {
    if (mSubStep == 0) {
        mSubStep++;
        func_02034f38(&mWindow[0], 0, message);
        func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW,
                      0);
        for (int i = 0; i < 3; i++) {
            mWindow[0].mButtons[i].mVisible = FALSE;
        }
    } else if (mSubStep == 1) {
        if (mWindow[0].mUnknownAF88) {
            mSubStep++;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}
