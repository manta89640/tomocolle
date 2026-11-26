#include "AffinityScene.h"
#include "MainProgram.h"

// --- The scene's data ---

// The touch screen's four buttons: sort, back, decide, choose again
static const u8 sButtonSequences[4] = {0x7B, 0x00, 0x76, 0x76};
static const u8 sButtonPressedSequences[4] = {0x7C, 0x01, 0x77, 0x77};
static const WindowPoint sButtonPositions[4] = {{0xD5, 0x13}, {0xF2, 0x0D}, {0x80, 0xAD}, {0xC2, 0xAD}};
static const WindowPoint sButtonSizes[4] = {{0x1A, 0x22}, {0x18, 0x16}, {0x74, 0x1E}, {0x74, 0x1E}};
static const ButtonSounds sButtonSounds[4] = {{10, -1, 0, -1}, {13, 1, 0, -1}, {10, -1, 0, -1}, {12, -1, 0, -1}};
static const int sButtonMessages[4] = {-1, -1, 0x8C, 0x95};
static const int sButtonFonts[4] = {-1, -1, 2, 2};
// Where the decide and choose-again buttons go once the first Mii is chosen
static const WindowPoint sSecondButtonPositions[2] = {{0xC2, 0xAD}, {0x3E, 0xAD}};

// The result screen's two buttons: back and again
static const u16 sResultButtonSequences[2] = {0, 4};
static const WindowPoint sResultButtonPositions[2] = {{0xF2, 0x0D}, {0x80, 0xAD}};
static const WindowPoint sResultButtonSizes[2] = {{0x18, 0x16}, {0x9E, 0x1E}};
static const ButtonSounds sResultButtonSounds[2] = {{12, 1, 0, -1}, {10, -1, 0, -1}};
static const int sResultButtonMessages[2] = {-1, 0xB0};
static const int sResultButtonFonts[2] = {-1, 2};

// The list's panel
static const WindowPoint sPanelButtonPosition = {0x77, 0x2F};
static const WindowPoint sPanelButtonSize = {0xE4, 0x12};
static const WindowPoint sPanelTextPosition = {0x0D, 0x29};
static const WindowPoint sPanelUpPosition = {0xF4, 0x2C};
static const WindowPoint sPanelUpSize = {0x0E, 0x0C};
static const WindowPoint sPanelDownPosition = {0xF4, 0x96};
static const WindowPoint sPanelDownSize = {0x0E, 0x0C};
static const WindowPoint sPanelScrollBarPosition = {0xF4, 0x61};
static const ScreenRect sPanelScrollBarRect = {-8, -8, 0x10, 0x10};

// The sort menu: a button for each of the seven orders
static const ScreenRect sSortMenuRect = {0x14, 0x0C, 0xD8, 0xA8};
static const u8 sSortButtonSequences[7] = {0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A, 0x6A};
static const u8 sSortButtonPressedSequences[7] = {0x6B, 0x6B, 0x6B, 0x6B, 0x6B, 0x6B, 0x6B};
static const WindowPoint sSortButtonPositions[7] = {{0x80, 0x1E}, {0x80, 0x34}, {0x80, 0x4A}, {0x80, 0x60},
                                                    {0x80, 0x76}, {0x80, 0x8C}, {0x80, 0xA2}};
static const WindowPoint sSortButtonSizes[7] = {{0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14},
                                                {0x80, 0x14}, {0x80, 0x14}, {0x80, 0x14}};
static const WindowPoint sSortTextOffsets[7] = {{8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 2}, {8, 2}};
static const int sSortMessages[7] = {0x7F, 0x80, 0x81, 0x82, 0x83, 0x84, 0x85};
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

// The message window and the question window
static const ScreenRect sMessageRect = {0x14, 0x14, 0xD8, 0x98};
static const WindowPoint sMessageButtonPosition = {0x80, 0x91};
static const WindowPoint sMessageButtonSize = {0x5E, 0x1E};
static const WindowPoint sMessageTextPosition = {0x26, 0x21};
static const ScreenRect sQuestionRect = {0x14, 0x14, 0xD8, 0x98};
static const WindowPoint sQuestionYesPosition = {0xB5, 0x95};
static const WindowPoint sQuestionYesSize = {0x5E, 0x1E};
static const WindowPoint sQuestionNoPosition = {0x4B, 0x95};
static const WindowPoint sQuestionNoSize = {0x5E, 0x1E};
static const WindowPoint sQuestionTextPosition = {0x26, 0x21};

// --- The scene ---

void AffinityScene::StartLoading() {
    mPart.MountArchives();
    ArchiveManager_MountCompressed(&gArchiveManager, "SFL", "Scn/Scene/SceneAffinity_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "LST", "Scn/List/List_RightJobList_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TME", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMJ", "Msg/TmdContentsMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMC", "Msg/TmdcommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "CAP", "Scn/Scene/NgCaption_LZ.bin", 0);
    gSoundManager.LoadGroup(0x51);
    gSoundManager.LoadGroup(0x2F);
    if (gSceneManager.mPreviousSceneId == 0x80) {
        gSoundManager.StartSeqIfNotPlaying(0, 0x40);
    }
}

void AffinityScene::Init() {
    gSoundManager.StartSeqIfNotPlaying(0, 0x40);
    mCharOffset = 0x226;
    mUpperCharOffset = 0x190;
    mScroll = 0;
    VramTransferQueue_LoadFile(&gVramTransferQueue, "LST:/BG_list.ncg", 0x700, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "LST:/BG_list.ncl", 0x700, -1, -1, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "LST:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
    ScreenLoader_LoadFile(&gScreenLoader, "LST:/BG_RightJob_frame.nsc", 0x200, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 2);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "CAP:/caption.ncg", 2, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "CAP:/caption.nsc", 2, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 0);
    ObjResourceManager_LoadCell(&gObjResourceManager, "LST:/Obj_Mii_list.nce", 0x1000, 0);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "LST:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "LST:/Obj_list.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SFL:/Obj_up.nce", 0x10, 1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SFL:/Obj_up.ncg", 0x10, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SFL:/Obj_up.ncl", 0x10, -1, -1, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 0x10, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 4, 2);
    ScreenLoader_SetPriority(&gScreenLoader, 8, 3);
    gScreenControl.Set(0x171D);
    ScreenControl_SetBlendAlpha(&gScreenControl, 2, 0x1D, 8, 8);
    mMessageFiles[0].Open("TME:tmd_etc_msg_JP.bmg");
    mMessageFiles[1].Open("TMJ:tmd_contents_msg_JP.bmg");
    mMessageFiles[2].Open("TMC:tmd_common_msg_JP.bmg");

    mPanel.Init(6);
    mPanel.SetBackground(0x100, 0x26);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x5FDF, 0x100);
    mPanel.InitButtons(0, 0x68, 0x69, sPanelButtonPosition, sPanelButtonSize);
    mPanel.InitTexts(0x1000, sPanelTextPosition, &mCharOffset);
    mPanel.InitUpButton(0, 2, 3, sPanelUpPosition, sPanelUpSize);
    mPanel.InitDownButton(0, 4, 5, sPanelDownPosition, sPanelDownSize);
    mPanel.InitScrollBar(0, 6, 7, 0x61, 0x4A, 1, sPanelScrollBarPosition, sPanelScrollBarRect);
    mPanel.SetUpScrollBar(2, 6, 7);
    mTabs.Init(0, 8);
    mList.Init(&mPanel, &mTabs);
    mList.Load();
    mSortKey = gGameData.mMiiListSortKey;
    mList.mSelectInGroup = FALSE;
    if (gSceneManager.mPreviousSceneId == 0x20) {
        if (!mList.SelectIndex(gSceneMemory.mCharacters[0])) {
            mList.SelectIndex(0);
        }
        mList.Sort(mSortKey, FALSE);
    } else {
        mList.Sort(mSortKey, TRUE);
    }

    for (int i = 0; i < 4; i++) {
        Sprite_Init(&mButtons[i], 0, sButtonSequences[i], 0x80, 0x60, TRUE);
        SpriteButton_SetKind(&mButtons[i], 4);
        SpriteButton_SetSequences(&mButtons[i], sButtonSequences[i], sButtonPressedSequences[i]);
        mButtons[i].SetPosition(FxVec2(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        SpriteButton_Reset(&mButtons[i]);
        mButtons[i].mUnknown5E = 1;
        SpriteButton_SetSounds(&mButtons[i], &sButtonSounds[i]);
        if (sButtonMessages[i] == -1) {
            mButtons[i].mText.mSprite.mVisible = FALSE;
        } else {
            mCharOffset += TextButton_SetMessage(&mButtons[i], &mMessageFiles[1], sButtonMessages[i],
                                                 &gFonts[sButtonFonts[i]].mFont, 0x1000, mCharOffset);
        }
    }
    SpriteButton_SetDisabled(&mButtons[2], TRUE);
    mButtons[3].mVisible = FALSE;
    int i;
    for (i = 0; i < 2; i++) {
        Sprite_Init(&mResultButtons[i], 0, 0, 0x80, 0x60, TRUE);
        mResultButtons[i].mVisible = FALSE;
    }
    for (i = 0; i < 2; i++) {
        mResultSprites[i].mVisible = FALSE;
    }

    mUpperCharOffset += mSpeechWindow.Init(mUpperCharOffset, &mMessageFiles[1], 2, 0x10, NULL, FALSE);
    mPart.Init(&mUpperCharOffset, &mCharOffset);
    if (mList.mCount != 0 && mList.mSelectedIndex != 0xFFFF && mList.mSelected != -1) {
        ShowSelection();
    }

    int charOffset = mCharOffset;
    ScreenRect sortRect = sSortMenuRect;
    MessageWindow_Init(&mWindows[0], sortRect);
    MessageWindow_SetScreen(&mWindows[0], "LST:/Window_down.nsc", 0x400);
    func_020331bc(&mWindows[0], 0x3F00, sortRect);
    for (int i = 0; i < 7; i++) {
        func_020331f8(&mWindows[0], i, 0, sSortButtonSequences[i], sSortButtonPressedSequences[i],
                      sSortButtonPositions[i], sSortButtonSizes[i], 10, -1, 0, -1, 1);
    }
    for (int i = 0; i < 7; i++) {
        MessageWindow_AddTextInArea(&mWindows[0], &mMessageFiles[0], sSortMessages[i], &gFonts[0].mFont, 0x1000,
                                    sSortTextOffsets[i], sSortTextPositions[i], &mCharOffset);
    }
    MessageWindow_SetTextsHAlign(&mWindows[0], 1);

    mCharOffset = charOffset;
    ScreenRect messageRect = sMessageRect;
    MessageWindow_Init(&mWindows[1], messageRect);
    MessageWindow_SetScreen(&mWindows[1], "LST:/Window_common.nsc", 0x400);
    func_020331bc(&mWindows[1], 0x3F00, messageRect);
    func_020334ac(&mWindows[1], 0, 0, 0x74, 0x75, sMessageButtonPosition, sMessageButtonSize, &mMessageFiles[2], 4,
                  &gFonts[1].mFont, 0x1000, &mCharOffset, 10, -1, 0, -1, 1);
    MessageWindow_AddText(&mWindows[1], &mMessageFiles[1], 0x8D, &gFonts[0].mFont, 0x1000, sMessageTextPosition,
                          &mCharOffset);
    MessageWindow_SetTextsHAlign(&mWindows[1], 1);
    MessageWindow_SetTextsVAlign(&mWindows[1], 1);

    mCharOffset = charOffset;
    ScreenRect questionRect = sQuestionRect;
    MessageWindow_Init(&mWindows[2], questionRect);
    MessageWindow_SetScreen(&mWindows[2], "LST:/Window_common.nsc", 0x400);
    func_020331bc(&mWindows[2], 0x3F00, questionRect);
    func_020334ac(&mWindows[2], 0, 0, 0x74, 0x75, sQuestionYesPosition, sQuestionYesSize, &mMessageFiles[1], 0x94,
                  &gFonts[1].mFont, 0x1000, &mCharOffset, 10, -1, 0, -1, 1);
    func_020334ac(&mWindows[2], 1, 0, 0x74, 0x75, sQuestionNoPosition, sQuestionNoSize, &mMessageFiles[1], 0x95,
                  &gFonts[1].mFont, 0x1000, &mCharOffset, 12, -1, 0, -1, 1);
    MessageWindow_AddText(&mWindows[2], &mMessageFiles[1], 0x93, &gFonts[0].mFont, 0x1000, sQuestionTextPosition,
                          &mCharOffset);
    MessageWindow_SetTextsHAlign(&mWindows[2], 1);
    MessageWindow_SetTextsVAlign(&mWindows[2], 1);

    mTtsPlayer.InitEngine();
    mState = STATE_INTRO;
    MessageWindow_Open(&mWindows[1], 0x12, 1);
}

void AffinityScene::Update() {
    mTtsPlayer.Update();
    switch (mState) {
    case STATE_CHOOSE_FIRST:
    case STATE_CHOOSE_SECOND:
        UpdateChoosing();
        break;
    case STATE_SORT_FIRST:
    case STATE_SORT_SECOND:
        UpdateSortMenu();
        break;
    case STATE_INTRO:
    case STATE_FIRST_CHOSEN:
        UpdateMessage();
        break;
    case STATE_QUESTION:
        UpdateQuestion();
        break;
    case STATE_FADE_OUT:
        if (ScreenControl_BlendFadeOut(&gScreenControl, 0x3F00, 1)) {
            ScreenLoader_LoadFile(&gScreenLoader, "SFL:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
            VramTransferQueue_LoadFile(&gVramTransferQueue, "SFL:/BG_down.ncg", 0x100, 0, 0, 0, -1);
            PaletteLoader_LoadFile(&gPaletteLoader, "SFL:/BG_down.ncl", 0x100, -1, -1, -1);
            for (int i = 0; i < 4; i++) {
                mButtons[i].mVisible = FALSE;
            }
            ObjResourceManager_LoadCell(&gObjResourceManager, "SFL:/Obj_down.nsc", 0x1000, 0);
            VramTransferQueue_LoadFile(&gVramTransferQueue, "SFL:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
            PaletteLoader_LoadFile(&gPaletteLoader, "SFL:/Obj_down.ncl", 0x1000, -1, -1, -1);
            for (int i = 0; i < 2; i++) {
                u16 sequence = sResultButtonSequences[i];
                Sprite_Init(&mResultButtons[i], 0, sequence, 0x80, 0x60, TRUE);
                SpriteButton_SetKind(&mResultButtons[i], 4);
                SpriteButton_SetSequences(&mResultButtons[i], sequence, (u16)(sequence + 1));
                mResultButtons[i].SetPosition(
                    FxVec2(sResultButtonPositions[i].x << FX32_SHIFT, sResultButtonPositions[i].y << FX32_SHIFT));
                SpriteButton_SetTouchRect(&mResultButtons[i], sResultButtonSizes[i].x, sResultButtonSizes[i].y);
                SpriteButton_Reset(&mResultButtons[i]);
                mResultButtons[i].mUnknown5E = 1;
                SpriteButton_SetSounds(&mResultButtons[i], &sResultButtonSounds[i]);
                if (sResultButtonMessages[i] == -1) {
                    mResultButtons[i].mText.mSprite.mVisible = FALSE;
                } else {
                    mCharOffset += TextButton_SetMessage(&mResultButtons[i], &mMessageFiles[1],
                                                         sResultButtonMessages[i],
                                                         &gFonts[sResultButtonFonts[i]].mFont, 0x1000, mCharOffset);
                }
            }
            mResultButtons[0].mVisible = FALSE;
            mResultButtons[1].mVisible = FALSE;
            for (int i = 0; i < 2; i++) {
                Sprite_Init(&mResultSprites[i], 0, (u16)(i + 6), 0x80, 0x60, TRUE);
                mResultSprites[i].mVisible = FALSE;
            }
            mResultSprites[0].mVisible = TRUE;
            mState = STATE_FADE_IN;
            mFrame = 0;
        }
        break;
    case STATE_FADE_IN:
        ScreenControl_BlendFadeIn(&gScreenControl, 0x3F00, -1);
        if (mFrame++ > 120) {
            mResultButtons[0].mVisible = TRUE;
            mResultButtons[1].mVisible = TRUE;
            mResultSprites[0].mVisible = FALSE;
            mPart.ShowResult();
            mState = STATE_RESULT;
            mFrame = 0;
        }
        break;
    case STATE_RESULT:
    case STATE_RESULT_SHOWN:
        if (mState == STATE_RESULT) {
            if (mFrame == 60) {
                mState = STATE_RESULT_SHOWN;
                mResultSprites[1].mVisible = TRUE;
                gSoundManager.StartSeqArc(2, 0xD, 3);
                mFrame++;
            } else if (mFrame < 60) {
                mFrame++;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (SpriteButton_Update(&mResultButtons[i])) {
                switch (i) {
                case 0:
                    gSceneManager.RequestScene(0x5F);
                    break;
                case 1:
                    gSceneManager.RequestScene(mSceneId);
                    break;
                }
            }
        }
        break;
    case STATE_STEPS:
        UpdateSteps();
        break;
    }
    mPart.Update();
    mSpeechWindow.Update();
    mScroll += 0x4CC;
    ScreenLoader_SetScrollX(&gScreenLoader, 4, mScroll >> FX32_SHIFT);
}

void AffinityScene::UpdateChoosing() {
    int pressed = -1;
    for (int i = 0; i < 4; i++) {
        if (SpriteButton_Update(&mButtons[i])) {
            pressed = i;
        }
    }
    switch (pressed) {
    case 0:
        if (mState == STATE_CHOOSE_FIRST) {
            mState = STATE_SORT_FIRST;
        } else if (mState == STATE_CHOOSE_SECOND) {
            mState = STATE_SORT_SECOND;
        }
        MessageWindow_Open(&mWindows[0], 0x12, 1);
        break;
    case 1:
        gSceneManager.RequestScene(0x5F);
        break;
    case 2:
        if (mState == STATE_CHOOSE_FIRST) {
            wchar_t name[11];
            MI_CpuClear16(name, sizeof(name));
            if (mPart.mMode == AffinityPart::MODE_FIRST_MAN) {
                MiiData_GetName(&mPart.mCast.mFaces[0], name);
            } else {
                MiiData_GetName(&mPart.mCast.mFaces[1], name);
            }
            mWindows[1].mTexts[0].SetStringArg(name, 0);
            mWindows[1].mTexts[0].ExpandMessage();
            MessageWindow_SetTextMessage(&mWindows[1], 0, 0x8E);
            mState = STATE_FIRST_CHOSEN;
            MessageWindow_Open(&mWindows[1], 0x12, 1);
        } else if (mState == STATE_CHOOSE_SECOND) {
            mState = STATE_QUESTION;
            MessageWindow_Open(&mWindows[2], 0x12, 1);
        }
        break;
    case 3:
        gSceneManager.RequestScene(mSceneId);
        break;
    }
    mList.Update();
    if (mList.mChanged && mList.mSelectedIndex != 0xFFFF && mList.mSelected != -1) {
        ShowSelection();
        mList.mSelectInGroup = TRUE;
    }
}

// Shows the Mii selected in the list on the top screen, and whether it can be chosen: one without a birthday
// cannot, and neither can the first one again
void AffinityScene::ShowSelection() {
    u32 year = mList.mResidents[mList.mSelected].mBirthYear;
    u32 month = mList.mResidents[mList.mSelected].GetParameter(PARAM_BIRTH_MONTH);
    u32 day = mList.mResidents[mList.mSelected].GetParameter(PARAM_BIRTH_DAY);
    if (mState == STATE_CHOOSE_SECOND && mList.mResidents[mList.mSelected].mIndex == mPart.mFirst) {
        if (mState == STATE_CHOOSE_FIRST) {
            mPart.SetFirst(&mList.mResidents[mList.mSelected], TRUE);
        } else if (mState == STATE_CHOOSE_SECOND) {
            mPart.SetSecond(&mList.mResidents[mList.mSelected], TRUE);
        }
        SpriteButton_SetDisabled(&mButtons[2], TRUE);
        mSpeechWindow.Show(0x92, SpeechWindow::MODE_MANUAL, 120);
        return;
    }
    if (year >= 1900 && year <= 2099 && month >= 1 && month <= 12 && day >= 1 && day <= 31) {
        if (mState == STATE_CHOOSE_FIRST) {
            mPart.SetFirst(&mList.mResidents[mList.mSelected], FALSE);
        } else if (mState == STATE_CHOOSE_SECOND) {
            mPart.SetSecond(&mList.mResidents[mList.mSelected], FALSE);
        }
        SpriteButton_SetDisabled(&mButtons[2], FALSE);
        mSpeechWindow.Hide();
        return;
    }
    if (mState == STATE_CHOOSE_FIRST) {
        mPart.SetFirst(&mList.mResidents[mList.mSelected], TRUE);
    } else if (mState == STATE_CHOOSE_SECOND) {
        mPart.SetSecond(&mList.mResidents[mList.mSelected], TRUE);
    }
    SpriteButton_SetDisabled(&mButtons[2], TRUE);
    mSpeechWindow.Show(0x91, SpeechWindow::MODE_MANUAL, 120);
}

void AffinityScene::UpdateSortMenu() {
    MessageWindow_Update(&mWindows[0]);
    switch (mWindows[0].mResult) {
    case 0:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 0) {
            mList.Sort(0, FALSE);
        }
        break;
    case 1:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 1) {
            mList.Sort(1, FALSE);
        }
        break;
    case 2:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 2) {
            mList.Sort(2, FALSE);
        }
        break;
    case 3:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 3) {
            mList.Sort(3, FALSE);
        }
        break;
    case 4:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 4) {
            mList.Sort(4, FALSE);
        }
        break;
    case 5:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 5) {
            mList.Sort(5, FALSE);
        }
        break;
    case 6:
        mWindows[0].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[0], 0x24);
        if (mList.mSortKey != 6) {
            mList.Sort(6, FALSE);
        }
        break;
    }
    if (mWindows[0].mTransitionDone) {
        if (mState == STATE_SORT_FIRST) {
            mState = STATE_CHOOSE_FIRST;
        } else if (mState == STATE_SORT_SECOND) {
            mState = STATE_CHOOSE_SECOND;
        }
    }
}

void AffinityScene::UpdateMessage() {
    MessageWindow_Update(&mWindows[1]);
    if (mWindows[1].mResult != -1) {
        mWindows[1].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[1], 0x24);
    }
    if (!mWindows[1].mTransitionDone) {
        return;
    }
    switch (mState) {
    case STATE_INTRO:
        mState = STATE_CHOOSE_FIRST;
        break;
    case STATE_FIRST_CHOSEN:
        mButtons[3].mVisible = TRUE;
        SpriteButton_SetDisabled(&mButtons[2], TRUE);
        for (int i = 2; i <= 3; i++) {
            mButtons[i].SetPosition(FxVec2(sSecondButtonPositions[i - 2].x << FX32_SHIFT,
                                           sSecondButtonPositions[i - 2].y << FX32_SHIFT));
        }
        mList.ClearSelection();
        mState = STATE_CHOOSE_SECOND;
        break;
    case STATE_QUESTION:
        mPart.StartThinking();
        mFrame = 0;
        mState = STATE_FADE_OUT;
        break;
    }
}

void AffinityScene::UpdateQuestion() {
    MessageWindow_Update(&mWindows[2]);
    int result = mWindows[2].mResult;
    if (result != -1) {
        mWindows[2].mUnknownSetBeforeClose = 15;
        MessageWindow_Close(&mWindows[2], 0x24);
        mAnswer = result;
    }
    if (!mWindows[2].mTransitionDone) {
        return;
    }
    switch (mAnswer) {
    case 0:
        gSoundManager.StartSeqIfNotPlaying(0, 0x41);
        mPart.StartThinking();
        mFrame = 0;
        mState = STATE_FADE_OUT;
        break;
    case 1:
        gSceneManager.RequestScene(mSceneId);
        break;
    }
}

void AffinityScene::UpdateSteps() {
    switch (mStep) {
    case 0:
        if (mStepFrame >= 10) {
            NextStep();
        }
        mStepFrame++;
        break;
    case 1:
    case 2:
        if (mTtsPlayer.IsEngineIdle() == TRUE) {
            NextStep();
        }
        break;
    case 3:
        if (mStepFrame >= 60) {
            NextStep();
        }
        mStepFrame++;
        break;
    case 4:
        NextStep();
        break;
    case 5:
        if (mStepFrame >= 10) {
            gSceneMemory.mCharacters[0] = mList.mResidents[mList.mSelected].mIndex;
            gSceneManager.RequestScene(0x20);
        }
        mStepFrame++;
        break;
    }
}

void AffinityScene::SetStep(int step) {
    switch (step) {
    case 0:
    case 3:
    case 5:
        mStepFrame = 0;
        mSpeechWindow.Hide();
        break;
    case 1:
    case 2:
    case 4:
        break;
    }
    mStep = step;
}

void AffinityScene::NextStep() {
    SetStep(mStep + 1);
}

void AffinityScene::Draw() {
    for (int i = 0; i < 4; i++) {
        TextButton_Draw(&mButtons[i]);
    }
    for (int i = 0; i < 2; i++) {
        TextButton_Draw(&mResultButtons[i]);
    }
    for (int i = 0; i < 2; i++) {
        Sprite_Draw(&mResultSprites[i]);
    }
    mPart.Draw();
    MessageWindow_Draw(&mWindows[0]);
    MessageWindow_Draw(&mWindows[1]);
    MessageWindow_Draw(&mWindows[2]);
    switch (mState) {
    case STATE_FADE_IN:
    case STATE_RESULT:
    case STATE_RESULT_SHOWN:
        break;
    default:
        mList.Draw();
        break;
    }
    mSpeechWindow.Draw();
}

void AffinityScene::Exit() {
    ScreenLoader_SetScrollX(&gScreenLoader, 4, 0);
    if (mSortKey != mList.mSortKey) {
        gGameData.mMiiListSortKey = mList.mSortKey;
    }
    MessageWindow_Release(&mWindows[0]);
    MessageWindow_Release(&mWindows[1]);
    MessageWindow_Release(&mWindows[2]);
    ArchiveManager_Unmount(&gArchiveManager, "SFL");
    ArchiveManager_Unmount(&gArchiveManager, "LST");
    ArchiveManager_Unmount(&gArchiveManager, "TME");
    ArchiveManager_Unmount(&gArchiveManager, "TMJ");
    ArchiveManager_Unmount(&gArchiveManager, "TMC");
    ArchiveManager_Unmount(&gArchiveManager, "CAP");
    func_02024234(&gScreenControl, 0x3F00, 0);
    ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F00, 0);
    ScreenControl_ClearBlend(&gScreenControl, 0x3F00);
    mPart.Exit();
}

// --- The top screen ---

void AffinityPart::MountArchives() {
    ArchiveManager_MountCompressed(&gArchiveManager, "C00", "Scn/Affn/affinity_skin_c0_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "C01", "Scn/Affn/affinity_skin_c1_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "C02", "Scn/Affn/affinity_skin_c2_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "C03", "Scn/Affn/affinity_skin_c3_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "C04", "Scn/Affn/affinity_skin_c4_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "C05", "Scn/Affn/affinity_skin_c5_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "M00", "Scn/Affn/affinity_frame_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "M01", "Scn/Affn/affinity_number_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMT", "Msg/TmdcontentsMsgJP_LZ.bin", 0);
}
