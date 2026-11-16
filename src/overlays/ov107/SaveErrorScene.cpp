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
    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(31, 31, 31), 31, 0x7FFF, 63, FALSE);

    VramTransferQueue_LoadFile(&gVramTransferQueue, "MLT:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, sScreenName, 0x200, 0, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MLT:/BG_down.ncl", 0x200, -1, -1, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "MLT:/Obj_down.nce", 0x1000, 0);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "MLT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MLT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    gScreenControl.Set(1);
    gScreenControl.Show(0x1200);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x7FFF, 0x101);

    mEtcMessages.Open("TEM:tmd_etc_msg_JP.bmg");
    mCommonMessages.Open("TCM:tmd_common_msg_JP.bmg");

    int charOffset = 0x200;
    MessageFile *etc = &mEtcMessages;
    MessageFile *common = &mCommonMessages;
    ScreenRect rect = sWindowRect;
    MessageWindow_Init(&mWindow[0], rect);
    MessageWindow_SetScreen(&mWindow[0], sScreenName, 0x200);
    func_020331bc(&mWindow[0], 0x3F00, rect);
    MessageWindow_AddTextButtonWithSoundIds(&mWindow[0], MessageWindow::BUTTON_OK, 0, 0, 1, sButtonPosition, sButtonSize, common, 4,
                  &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, 10);
    for (int i = 0; i < 2; i++) {
        MessageWindow_AddTextButtonWithSoundIds(&mWindow[0], MessageWindow::BUTTON_YES + i, 0, 0, 1, sChoicePositions[i], sButtonSize,
                      common, sChoiceMessages[i], &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, -1);
    }
    MessageWindow_AddText(&mWindow[0], etc, 0x1A2, &gFonts[0].mFont, 0x1000, sTextPosition, &charOffset);
    MessageWindow_SetTextsVAlign(&mWindow[0], 1);
    MessageWindow_SetTextsHAlign(&mWindow[0], 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 0);
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
    MessageWindow_Draw(&mWindow[0]);
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
    MessageWindow_Update(&mWindow[0]);
}

// Opens the window with a message and no buttons, and returns TRUE once it is shown
// @0x02149f10
BOOL SaveErrorScene::ShowMessage(u32 message) {
    if (mSubStep == 0) {
        mSubStep++;
        MessageWindow_SetTextMessage(&mWindow[0], 0, message);
        MessageWindow_Open(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW,
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
