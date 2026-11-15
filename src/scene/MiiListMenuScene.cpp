#include "MiiListMenuScene.h"

// The file's constants. In this order, the compiler (which sorts a file's constants by size, and objects of one
// size by their place in the list) puts them in the ROM's order: so they are not grouped by what they are for.
// The two buttons at the top are sButton*; the Mii menu (window 0: three buttons with texts, edit, change clothes
// and erase, then a cancel button) is sMiiMenu* and sCancel*; the sort menu (window 1: a button for each of the
// seven orders) is sSort*.
static const u8 sButtonSequences[2] = {0x76, 0x76};
static const u8 sButtonPressedSequences[2] = {0x77, 0x77};
static const ScreenRect sMiiMenuRect = {0x14, 0x13, 0xD8, 0x99};
static const ScreenPoint sButtonSizes[2] = {{0x28, 0x16}, {0x28, 0x16}};
static const ScreenRect sSortMenuRect = {0x14, 0x0B, 0xD8, 0xA9};
static const char sMiiMenuScreenName[] = "MLT:/Window_common.nsc";
static const u8 sMiiMenuSequencesA[3] = {0x6A, 0x6A, 0x6A};
static const u8 sMiiMenuSequencesB[3] = {0x6B, 0x6B, 0x6B};
static const WindowPoint sMiiMenuButtonPositions[3] = {{0x80, 0x2E}, {0x80, 0x4E}, {0x80, 0x6E}};
static const WindowPoint sMiiMenuButtonSizes[3] = {{0xBE, 0x1A}, {0xBE, 0x1A}, {0xBE, 0x1A}};
static const int sMiiMenuMessages[3] = {0x75, 0x77, 0x79};
// Not constants: the static initialiser computes them from the buttons' positions
static const WindowPoint sMiiMenuTextPositions[3] = {
    {sMiiMenuButtonPositions[0].x - 0x5C, sMiiMenuButtonPositions[0].y - 6},
    {sMiiMenuButtonPositions[1].x - 0x5C, sMiiMenuButtonPositions[1].y - 6},
    {sMiiMenuButtonPositions[2].x - 0x5C, sMiiMenuButtonPositions[2].y - 6},
};
static const ScreenPoint sButtonPositions[2] = {{0x16, 0x0D}, {0xD0, 0x0D}};
static const char sSortMenuScreenName[] = "MLT:/Window_down.nsc";
static const WindowPoint sSortTextOffsets[7] = {{8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 2}};
static const WindowPoint sSortButtonPositions[7] = {{0x80, 0x1E}, {0x80, 0x34}, {0x80, 0x4A}, {0x80, 0x60},
                                                    {0x80, 0x76}, {0x80, 0x8C}, {0x80, 0xA2}};
static const int sSortMessages[7] = {0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85};
static const WindowPoint sSortButtonSizes[7] = {{0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14},
                                                {0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14}};
static const WindowPoint sSortTextPositions[7] = {
    {sSortButtonPositions[0].x - 0x20, sSortButtonPositions[0].y - 6},
    {sSortButtonPositions[1].x - 0x20, sSortButtonPositions[1].y - 6},
    {sSortButtonPositions[2].x - 0x20, sSortButtonPositions[2].y - 6},
    {sSortButtonPositions[3].x - 0x20, sSortButtonPositions[3].y - 6},
    {sSortButtonPositions[4].x - 0x20, sSortButtonPositions[4].y - 6},
    {sSortButtonPositions[5].x - 0x20, sSortButtonPositions[5].y - 6},
    {sSortButtonPositions[6].x - 0x20, sSortButtonPositions[6].y - 6},
};

static const WindowPoint sMiiMenuSpritePosition = {0xB5, 0x95}; // func_02033bb4's: a sprite, sequence 0x76
static const WindowPoint sCancelPosition = {0x4B, 0x95};
static const WindowPoint sCancelSize = {0x5E, 0x1E};

void MiiListMenuScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/List/MiiList_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
}

// NOT MATCHING: two instructions are in the other order. Before the second MessageFile::Open the original loads the
// path into r1 and then computes the file's address into r0; this gives the address first, as before the first
// Open (which matches). Tried without effect: the files as an array (which also changes the destructor), pointers
// or references to them, the path in a variable, the screen setup after or before the Opens, the loops' variables
// in every scope, the button loop with a pointer or as do-while.
void MiiListMenuScene::Init() {
    mState = STATE_IDLE;
    mCharOffset = 0x258;
    func_02028f24(&gVramTransferQueue, "MLT:/BG_list.ncg", 0x400, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/BG_list.ncl", 0x400, -1, -1, -1);
    func_02027d7c(&data_02121834, "MLT:/Window_down.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_list.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_list.ncl", 0x1000, -1, -1, -1);
    data_0211d604.Set(0x1400);
    mCommonMessages.Open("TCM:tmd_common_msg_JP.bmg");
    mEtcMessages.Open("TEM:tmd_etc_msg_JP.bmg");

    for (int i = 0; i < 2; i++) {
        Sprite_Init(&mButtons[i], 0, sButtonSequences[i], 0x80, 0x60, TRUE);
        func_02031260(&mButtons[i], 4);
        func_02031278(&mButtons[i], sButtonSequences[i], sButtonPressedSequences[i]);
        mButtons[i].SetPosition(FxVec2(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        func_0203119c(&mButtons[i]);
        mButtons[i].mUnknown5E = 1;
    }

    int charOffset = 0;
    int maxCharOffset = 0;

    // The Mii menu
    charOffset = mCharOffset;
    ScreenRect rect = sMiiMenuRect;
    func_02032fb8(&mWindows[0], rect);
    func_02033174(&mWindows[0], sMiiMenuScreenName, 0x400);
    func_020331bc(&mWindows[0], 0x3F00, rect);
    for (int i = 0; i <= 2; i++) {
        func_02033390(&mWindows[0], i, 0, sMiiMenuSequencesA[i], sMiiMenuSequencesB[i],
                      sMiiMenuButtonPositions[i], sMiiMenuButtonSizes[i]);
    }
    func_0203373c(&mWindows[0], 3, 0, 0x76, 0x77, sCancelPosition, sCancelSize, &mCommonMessages, 5,
                  &gFonts[2].mFont, 0x1000, &charOffset);
    for (int i = 0; i < 3; i++) {
        func_02033a58(&mWindows[0], &mEtcMessages, sMiiMenuMessages[i], &gFonts[0].mFont, 0x1000,
                      sMiiMenuTextPositions[i], &charOffset);
    }
    func_02034f4c(&mWindows[0], 1);
    func_02033bb4(&mWindows[0], 0, 0x76, sMiiMenuSpritePosition);
    if (charOffset > maxCharOffset) {
        maxCharOffset = charOffset;
    }

    // The sort menu
    charOffset = mCharOffset;
    ScreenRect sortRect = sSortMenuRect;
    func_02032fb8(&mWindows[1], sortRect);
    func_02033174(&mWindows[1], sSortMenuScreenName, 0x400);
    func_020331bc(&mWindows[1], 0x3F00, sortRect);
    for (int i = 0; i <= 6; i++) {
        func_02033390(&mWindows[1], i, 0, 0x69, 0x66, sSortButtonPositions[i], sSortButtonSizes[i]);
    }
    for (int i = 0; i < 7; i++) {
        func_020338d0(&mWindows[1], &mEtcMessages, sSortMessages[i], &gFonts[0].mFont, 0x1000,
                      sSortTextOffsets[i], sSortTextPositions[i], &charOffset);
    }
    func_02034f4c(&mWindows[1], 1);
    if (charOffset > maxCharOffset) {
        maxCharOffset = charOffset;
    }

    // "Move this Mii out?"
    charOffset = mCharOffset;
    func_02033ddc(&mWindows[2], 0x400, 0x1000, &mEtcMessages, 0x7A, 0, &mCommonMessages, &charOffset);
    if (charOffset > maxCharOffset) {
        maxCharOffset = charOffset;
    }

    // "... has moved away"
    charOffset = mCharOffset;
    func_02033c88(&mWindows[3], 0x400, 0x1000, &mEtcMessages, 0x7E, 0, &mCommonMessages, &charOffset);
    if (charOffset > maxCharOffset) {
        maxCharOffset = charOffset;
    }

    mCharOffset = maxCharOffset;
    func_020278f8(&data_021213b8, 0xFFFF, 0x101);
}

void MiiListMenuScene::Update() {
    switch (mState) {
    case STATE_IDLE:
        UpdateIdle();
        break;
    case STATE_MII_MENU:
        UpdateMiiMenu();
        break;
    case STATE_SORT_MENU:
        UpdateSortMenu();
        break;
    case STATE_CONFIRM:
        UpdateConfirm();
        break;
    case STATE_MOVED_OUT:
        UpdateMovedOut();
        break;
    }
}

void MiiListMenuScene::UpdateIdle() {
    for (int i = 0; i < 2; i++) {
        func_0203133c(&mButtons[i]);
    }
    int pressed = -1;
    for (int i = 0; i < 2; i++) {
        if (func_02031ccc(&mButtons[i])) {
            pressed = i;
        }
    }
    switch (pressed) {
    case 0:
        mState = STATE_MII_MENU;
        func_02034720(&mWindows[0], 0x12, 1);
        break;
    case 1:
        mState = STATE_SORT_MENU;
        func_02034720(&mWindows[1], 0x12, 1);
        break;
    }
}

void MiiListMenuScene::UpdateMiiMenu() {
    func_020341d4(&mWindows[0]);
    switch (mWindows[0].mResult) {
    case 0: // Edit: not done here
        break;
    case 1: // Change clothes: not done here
        break;
    case 2:
        mWindows[0].mUnknownAF98 = NEXT_CONFIRM;
        mWindows[0].mUnknownAF94 = 15;
        func_020349cc(&mWindows[0], 0x1C);
        break;
    case 3:
        mWindows[0].mUnknownAF98 = NEXT_IDLE;
        mWindows[0].mUnknownAF94 = 5;
        func_020349cc(&mWindows[0], 0x24);
        break;
    }
    if (mWindows[0].mTransitionDone) {
        switch (mWindows[0].mUnknownAF98) {
        case NEXT_CONFIRM: {
            mState = STATE_CONFIRM;
            func_02034f38(&mWindows[2], 0, 0x7A);
            wchar_t name[] = L"「削除したいMii」";
            func_02034a3c(&mWindows[2], 0, 0, name);
            func_02034720(&mWindows[2], 0x1A, 1);
            break;
        }
        case NEXT_IDLE:
            mState = STATE_IDLE;
            break;
        }
    }
}

void MiiListMenuScene::UpdateSortMenu() {
    func_020341d4(&mWindows[1]);
    switch (mWindows[1].mResult) {
    case 0:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    case 1:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    case 2:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    case 3:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    case 4:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    case 5:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    case 6:
        mWindows[1].mUnknownAF94 = 15;
        func_020349cc(&mWindows[1], 0x24);
        break;
    }
    if (mWindows[1].mTransitionDone) {
        mState = STATE_IDLE;
    }
}

void MiiListMenuScene::UpdateConfirm() {
    func_020341d4(&mWindows[2]);
    switch (mWindows[2].mResult) {
    case 0:
        mWindows[2].mUnknownAF98 = 0;
        mWindows[2].mUnknownAF94 = 15;
        func_020349cc(&mWindows[2], 0x1C);
        break;
    case 1:
        mWindows[2].mUnknownAF98 = 1;
        mWindows[2].mUnknownAF94 = 15;
        func_020349cc(&mWindows[2], 0x14);
        break;
    }
    if (mWindows[2].mTransitionDone) {
        switch (mWindows[2].mUnknownAF98) {
        case 0: {
            mState = STATE_MOVED_OUT;
            wchar_t name[] = L"「削除したMii」";
            func_02034a3c(&mWindows[3], 0, 0, name);
            func_02034720(&mWindows[3], 0x1A, 1);
            break;
        }
        case 1:
            mState = STATE_IDLE;
            break;
        }
    }
}

void MiiListMenuScene::UpdateMovedOut() {
    func_020341d4(&mWindows[3]);
    if (mWindows[3].mResult != -1) {
        mWindows[3].mUnknownAF94 = 5;
        func_020349cc(&mWindows[3], 0x14);
    }
    if (mWindows[3].mTransitionDone) {
        mState = STATE_IDLE;
    }
}

void MiiListMenuScene::Draw() {
    for (int i = 0; i < 4; i++) {
        func_020345d8(&mWindows[i]);
    }
    for (int i = 0; i < 2; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

void MiiListMenuScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
}
