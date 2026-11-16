#include "PreStartScene.h"
#include "MainProgram.h"

// Overlay 106: what comes before the game starts (PreStartScene). One source file: the scene, its clock and its
// saving icon, with the scene's implicit destructor, which the compiler emits with the scene's virtual table, at the
// end of .text.

// --- The file's constants ---
//
// In this order, the compiler (which sorts a file's constants by size, see docs/notes/ov073.md) puts them in the
// ROM's order.

// Where the message window's text is
static const WindowPoint sTextPosition = { 38, 33 };
// The texts of the yes and no buttons, common messages "はい" and "いいえ"
static const int sChoiceMessages[2] = { 2, 3 };
// The window's OK button: where it is and how large (the yes and no buttons are as large)
static const WindowPoint sButtonPosition = { 128, 149 };
static const WindowPoint sButtonSize = { 94, 30 };
// Where the yes and no buttons are
static const WindowPoint sChoicePositions[2] = { { 75, 149 }, { 181, 149 } };
// The rectangle of the message window, in pixels
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
// The screen (tile map) of the lower screen's background, which the window shows too
static const char sScreenName[] = "MLT:/BG_down.nsc";

// @0x02149a40
void PreStartScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/Scene/SceneSavePlay_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "MNU", "Scn/Scene/ScenePreStart_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
}

// @0x02149abc
void PreStartScene::Init() {
    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(31, 31, 31), 31, 0x7FFF, 63, FALSE);

    func_02028f24(&gVramTransferQueue, "MLT:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, sScreenName, 0x200, 0, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MLT:/BG_down.ncl", 0x200, -1, -1, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_down.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MLT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    func_02027290(&gObjResourceManager, "MNU:/Obj_up.nce", 0x10, 1);
    func_02028f24(&gVramTransferQueue, "MNU:/Obj_up.ncg", 0x10, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MNU:/Obj_up.ncl", 0x10, -1, -1, -1);
    data_0211d604.Set(1);
    data_0211d604.Show(0x1210);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x7FFF, 0x101);

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
        func_020336b4(&mWindow[0], MessageWindow::BUTTON_YES + i, 0, 0, 1, sChoicePositions[i], sButtonSize, common,
                      sChoiceMessages[i], &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, -1);
    }
    func_02033a58(&mWindow[0], etc, 0x1A2, &gFonts[0].mFont, 0x1000, sTextPosition, &charOffset);
    func_02034f94(&mWindow[0], 1);
    func_02034f4c(&mWindow[0], 1);
    func_02028280(&data_02121834, 0x200, 0);

    mSavingIcon.Init();
    mNextSceneId = 0x69;
    SetState(STATE_START);
    mLoadResult = gGameData.mUnknownState;
    switch (mLoadResult) {
    case 0:
        if (!gGameData.mUnknownB1) {
            SetState(STATE_START);
            return;
        }
        mClock.Update();
        if (data_02123440->mUnknown34) {
            SetState(STATE_OTHER_DS);
            return;
        }
        SetState(STATE_START);
        return;
    case 1:
        SetState(STATE_START);
        gGameData.mUnknownB1 = FALSE;
        return;
    case 2:
        SetState(STATE_INITIALISE);
        gGameData.mUnknownB1 = FALSE;
        return;
    case 3:
        SetState(STATE_READ_FAILED);
        return;
    case 4:
        SetState(STATE_READ_FAILED);
        break;
    }
}

// @0x02149fc0
void PreStartScene::Update() {
    switch (mState) {
    case STATE_START:
        if (!gGameData.mUnknownB1) {
            gGameData.mUnknownB1 = TRUE;
            mNextSceneId = 0x69;
            SetState(STATE_LEAVE);
        } else {
            UpdateResidents();
            mNextSceneId = 0x6A;
            SetState(STATE_LEAVE);
        }
        break;
    case STATE_OTHER_DS:
        if (mStep == 0) {
            if (Ask(0x1A2, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW)) {
                if (mAnswer == MessageWindow::BUTTON_YES) {
                    SetState(STATE_START);
                } else {
                    mStep++;
                    mSubStep = 0;
                }
            }
        } else {
            if (ShowMessage(0x1A3)) {
                SetState(STATE_STOPPED);
            }
        }
        break;
    case STATE_INITIALISE:
        if (mStep == 0) {
            if (ShowOk(0x1A7)) {
                mStep++;
                mSubStep = 0;
            }
        } else if (mStep == 1) {
            if (ShowMessage(0x17D)) {
                mStep++;
                mSubStep = 0;
                mSavingIcon.mVisible = TRUE;
                func_0202c8f8(&gGameData, 0);
                if (!func_0202cd48(&gGameData)) {
                    gSceneManager.RequestScene(0xC2);
                }
            }
        } else if (mStep == 2) {
            if (!func_0202cd38(&gGameData)) {
                mStep++;
                mSubStep = 0;
                mWindow[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_8 |
                                               MessageWindow::FLAG_FROM_BELOW);
                mSavingIcon.mVisible = FALSE;
            }
        } else if (mStep == 3) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mSubStep = 0;
            }
        } else {
            if (ShowOk(0x1A8)) {
                SetState(STATE_START);
            }
        }
        break;
    case STATE_READ_FAILED:
        if (ShowMessage(0x1A4)) {
            SetState(STATE_STOPPED);
        }
        break;
    case STATE_LEAVE:
        gSceneManager.RequestScene(mNextSceneId);
        break;
    }
    UpdateWindow();
}

// @0x0214a230
void PreStartScene::Draw() {
    func_020345d8(&mWindow[0]);
    mSavingIcon.Draw();
}

// @0x0214a24c
void PreStartScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "MNU");
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
}

// @0x0214a298
void PreStartScene::SetState(int state) {
    mState = state;
    mStep = 0;
    mSubStep = 0;
}

// @0x0214a2b0
void PreStartScene::UpdateWindow() {
    func_020341d4(&mWindow[0]);
}

// Lists the residents and has the main program work out, from the clock, what they did while the game was off
// @0x0214a2c0
void PreStartScene::UpdateResidents() {
    int i;
    int unknown2C = mClock.mUnknown2C;
    RTCTime time = mClock.mTime;
    s64 seconds = mClock.mSeconds;

    for (i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
        }
    }
    ShiftEventTimes(seconds);
    LimitEventTimes(seconds);
    if (unknown2C == 0) {
        mJudge.MeetWhileAway(mResidentCount, mResidents);
    }
    ::UpdateResidents(seconds, &time);
    StartNextEvents(seconds, &time);
    mJudge.PickItemsWhileAway(mResidentCount, mResidents, seconds);
    mJudge.ChangeClothesWhileAway(mResidentCount, mResidents);
    mJudge.ClearUnknown24E();
    mJudge.SetRandomUnknown24E(mResidentCount, mResidents);
}

// Opens the window with a message and no buttons, and returns TRUE once it is shown
// @0x0214a3f4
BOOL PreStartScene::ShowMessage(u32 message) {
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

// Opens the window with a message and the OK button, and returns TRUE once it has been pressed and the window has
// closed
// @0x0214a480
BOOL PreStartScene::ShowOk(u32 message) {
    if (mSubStep == 0) {
        mSubStep++;
        mWindow[0].mTexts[0].mMessage = message;
        func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW,
                      0);
        mWindow[0].mButtons[MessageWindow::BUTTON_YES].mVisible = FALSE;
        mWindow[0].mButtons[MessageWindow::BUTTON_NO].mVisible = FALSE;
    } else if (mSubStep == 1) {
        if (mWindow[0].mResult != -1) {
            mSubStep++;
            mWindow[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_8 |
                                           MessageWindow::FLAG_FROM_BELOW);
        }
    } else if (mSubStep == 2) {
        if (mWindow[0].mTransitionDone) {
            mSubStep++;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}

// Opens the window with a question and the yes and no buttons, and returns TRUE once one has been pressed (it is
// in mAnswer) and the window has closed
// @0x0214a530
BOOL PreStartScene::Ask(u32 message, int flags) {
    if (mSubStep == 0) {
        mSubStep++;
        mAnswer = -1;
        func_02034f38(&mWindow[0], 0, message);
        func_02034720(&mWindow[0], flags, 0);
        mWindow[0].mButtons[MessageWindow::BUTTON_OK].mVisible = FALSE;
    } else if (mSubStep == 1) {
        int result = mWindow[0].mResult;
        if (result == MessageWindow::BUTTON_YES || result == MessageWindow::BUTTON_NO) {
            mSubStep++;
            mAnswer = result;
            mWindow[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_8 |
                                           MessageWindow::FLAG_FROM_BELOW);
            if (result == MessageWindow::BUTTON_YES) {
                gSoundManager.StartSeqArc(1, 1, 10);
            } else {
                gSoundManager.StartSeqArc(1, 1, 12);
            }
        }
    } else if (mSubStep == 2) {
        if (mWindow[0].mTransitionDone) {
            mSubStep++;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}

// @0x0214a624
PreStartClock::PreStartClock() {
    mSeconds = 0;
    mUnknown08 = 0;
    mUnknown2C = 0;
    RTC_ConvertSecondToDateTime(&mDate, &mTime, mSeconds);
}

// @0x0214a65c
void PreStartClock::Update() {
    mSeconds = func_02035870(data_02123440, &mDate, &mTime);
    mUnknown08 = func_02035ae0(data_02123440);
    mUnknown2C = func_02035aa4(data_02123440);
}

// @0x0214a6ac
void SavingIcon::Init() {
    Sprite_Init(this, 1, 0, 128, 95, TRUE);
    mVisible = FALSE;
}

// @0x0214a6e4
void SavingIcon::Draw() {
    Sprite_Draw(this);
}
