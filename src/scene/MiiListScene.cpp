#include "MiiListScene.h"

// The animation sequences of the buttons of the list's five tabs, released and pressed
static const u8 sTab3Sequences[2] = {0x5A, 0x5C};
static const u8 sTab3PressedSequences[2] = {0x5B, 0x5D};
static const u8 sTab4Sequences[5] = {0x5E, 0x60, 0x62, 0x64, 0x66};
static const u8 sTab4PressedSequences[5] = {0x5F, 0x61, 0x63, 0x65, 0x67};

// The places of the panel's controls
static const WindowPoint sPanelPosition2 = {0xF4, 0xB6};
static const WindowPoint sPanelSize0 = {0x0E, 0x0C};
static const WindowPoint sPanelPosition0 = {0xF4, 0x4C};
static const WindowPoint sPanelSize = {0xE4, 0x12};
static const WindowPoint sPanelPosition1 = {0xF4, 0x81};
static const WindowPoint sPanelTextPosition = {0x0D, 0x49};
static const WindowPoint sSortTextOffset = {8, 2};
static const WindowPoint sPanelSize2 = {0x0E, 0x0C};
static const WindowPoint sPanelPosition = {0x77, 0x4F};

// The sort menu: a button for each of the seven orders
static const ScreenRect sSortMenuRect = {0x14, 0x0B, 0xD8, 0xA9};
static const ScreenRect sPanelRect = {-8, -8, 0x10, 0x10};
static const int sSortMessages[7] = {0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85};
static const WindowPoint sSortButtonPositions[7] = {{0x80, 0x1E}, {0x80, 0x34}, {0x80, 0x4A}, {0x80, 0x60},
                                                    {0x80, 0x76}, {0x80, 0x8C}, {0x80, 0xA2}};
static const WindowPoint sSortButtonSizes[7] = {{0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14},
                                                {0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14}};

static const u8 sTab0Sequences[11] = {0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1A, 0x1C};
static const u8 sTab0PressedSequences[11] = {0x09, 0x0B, 0x0D, 0x0F, 0x11, 0x13, 0x15, 0x17, 0x19, 0x1B, 0x1D};
static const u8 sTab2Sequences[13] = {0x40, 0x42, 0x44, 0x46, 0x48, 0x4A, 0x4C, 0x4E, 0x50, 0x52, 0x54, 0x56, 0x58};
static const u8 sTab2PressedSequences[13] = {0x41, 0x43, 0x45, 0x47, 0x49, 0x4B, 0x4D, 0x4F, 0x51, 0x53, 0x55, 0x57,
                                             0x59};
static const u8 sTab1Sequences[16] = {0x1E, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2C,
                                      0x2E, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3A, 0x3C};
static const u8 sTab1PressedSequences[16] = {0x1F, 0x21, 0x23, 0x25, 0x27, 0x29, 0x2B, 0x2D,
                                             0x2F, 0x31, 0x33, 0x35, 0x37, 0x39, 0x3B, 0x3D};
static const char sSortMenuScreenName[] = "MLT:/Window_down.nsc";

static const u8 *sTabPressedSequences[5] = {sTab0PressedSequences, sTab1PressedSequences, sTab2PressedSequences,
                                            sTab3PressedSequences, sTab4PressedSequences};
static const u8 *sTabSequences[5] = {sTab0Sequences, sTab1Sequences, sTab2Sequences, sTab3Sequences,
                                     sTab4Sequences};
// Not constants: the static initialiser computes them from the buttons' positions
static const WindowPoint sSortTextPositions[7] = {
    {sSortButtonPositions[0].x - 0x20, sSortButtonPositions[0].y - 6},
    {sSortButtonPositions[1].x - 0x20, sSortButtonPositions[1].y - 6},
    {sSortButtonPositions[2].x - 0x20, sSortButtonPositions[2].y - 6},
    {sSortButtonPositions[3].x - 0x20, sSortButtonPositions[3].y - 6},
    {sSortButtonPositions[4].x - 0x20, sSortButtonPositions[4].y - 6},
    {sSortButtonPositions[5].x - 0x20, sSortButtonPositions[5].y - 6},
    {sSortButtonPositions[6].x - 0x20, sSortButtonPositions[6].y - 6},
};

void MiiListScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/List/MiiList_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
}

void MiiListScene::Init() {
    mState = STATE_IDLE;
    mCharOffset = 0x258;
    func_02028f24(&gVramTransferQueue, "MLT:/BG_list.ncg", 0x700, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/BG_list.ncl", 0x700, -1, -1, -1);
    func_02027d7c(&data_02121834, "MLT:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 0x100, 3);
    func_02027d7c(&data_02121834, "MLT:/BG_frame_L.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 0x200, 2);
    func_02027d7c(&data_02121834, "MLT:/Window_down.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_list.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_list.ncl", 0x1000, -1, -1, -1);
    data_0211d604.Set(0x1700);
    mEtcMessages.Open("TEM:tmd_etc_msg_JP.bmg");

    func_020a1fe0(&mPanel, 6);
    func_020a2104(&mPanel, 0x100, 0);
    func_020a2118(&mPanel, 0, 0x68, 0x68, sPanelPosition, sPanelSize);
    func_020a2280(&mPanel, 0x1000, sPanelTextPosition, &mCharOffset);
    mPanel.InitUpButton(0, 2, 3, sPanelPosition0, sPanelSize0);
    mPanel.InitDownButton(0, 4, 5, sPanelPosition2, sPanelSize2);
    func_020a26fc(&mPanel, 0, 6, 7, 0x81, 0x4A, 1, sPanelPosition1, sPanelRect);

    mTabs.InitTabs();
    for (int i = 0; i < 5; i++) {
        mTabs.AddButtons(i, 0, sTabSequences[i], sTabPressedSequences[i]);
    }
    mTabs.Reset();
    mList.Init(&mPanel, &mTabs);
    mList.Load();
    mSortKey = gGameData.mMiiListSortKey;
    mList.Sort(mSortKey, TRUE);

    Sprite_Init(&mSortButton, 0, 0x74, 0x80, 0x60, TRUE);
    func_02031260(&mSortButton, 4);
    func_02031278(&mSortButton, 0x74, 0x75);
    mSortButton.SetPosition(FxVec2(0xD0 << FX32_SHIFT, 0x0D << FX32_SHIFT));
    func_02031eb0(&mSortButton, 0x28, 0x16);
    func_0203119c(&mSortButton);
    mSortButton.mUnknown5E = 1;

    ScreenRect rect = sSortMenuRect;
    func_02032fb8(&mSortMenu, rect);
    func_02033174(&mSortMenu, sSortMenuScreenName, 0x400);
    func_020331bc(&mSortMenu, 0x3F00, rect);
    for (int i = 0; i <= 6; i++) {
        func_02033390(&mSortMenu, i, 0, 0x6A, 0x6B, sSortButtonPositions[i], sSortButtonSizes[i]);
    }
    for (int i = 0; i < 7; i++) {
        WindowPoint offset = sSortTextOffset;
        func_020338d0(&mSortMenu, &mEtcMessages, sSortMessages[i], &gFonts[0].mFont, 0x1000, offset,
                      sSortTextPositions[i], &mCharOffset);
    }
    func_02034f4c(&mSortMenu, 1);
}

void MiiListScene::Update() {
    switch (mState) {
    case STATE_IDLE:
        UpdateIdle();
        break;
    case STATE_SORT_MENU:
        UpdateSortMenu();
        break;
    }
}

void MiiListScene::UpdateIdle() {
    func_0203133c(&mSortButton);
    if (func_02031ccc(&mSortButton)) {
        mState = STATE_SORT_MENU;
        func_02034720(&mSortMenu, 0x12, 1);
    }
    func_020a0454(&mList);
}

void MiiListScene::UpdateSortMenu() {
    func_020341d4(&mSortMenu);
    switch (mSortMenu.mResult) {
    case 0:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 0) {
            mList.Sort(0, FALSE);
        }
        break;
    case 1:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 1) {
            mList.Sort(1, FALSE);
        }
        break;
    case 2:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 2) {
            mList.Sort(2, FALSE);
        }
        break;
    case 3:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 3) {
            mList.Sort(3, FALSE);
        }
        break;
    case 4:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 4) {
            mList.Sort(4, FALSE);
        }
        break;
    case 5:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 5) {
            mList.Sort(5, FALSE);
        }
        break;
    case 6:
        mSortMenu.mUnknownAF94 = 15;
        func_020349cc(&mSortMenu, 0x24);
        if (mList.mSortKey != 6) {
            mList.Sort(6, FALSE);
        }
        break;
    }
    if (mSortMenu.mTransitionDone) {
        mState = STATE_IDLE;
    }
}

void MiiListScene::Draw() {
    func_02031d7c(&mSortButton);
    func_020345d8(&mSortMenu);
    mList.Draw();
}

void MiiListScene::Exit() {
    if (mSortKey != mList.mSortKey) {
        gGameData.mMiiListSortKey = mList.mSortKey;
    }
    func_020346cc(&mSortMenu);
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
}
