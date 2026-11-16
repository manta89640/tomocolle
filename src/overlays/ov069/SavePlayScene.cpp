#include "SavePlayScene.h"
#include "MainProgram.h"

// Overlay 69: saving (SavePlayScene). One source file: the scene, its camera and its lights, with the scene's
// implicit destructor, which the compiler emits with the scene's virtual table, at the end of .text.

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

static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// @0x02149a40
void SavePlayScene::StartLoading() {
    if (mSceneId == 0x2D) {
        gSceneManager.SetSoundGroup(0x36);
        gSoundManager.StopSeq(0, 0);
        gSoundManager.StartSeqIfNotPlaying(0, 0x61);
    } else {
        gSoundManager.StopSeq(0, 0);
        gSoundManager.StartSeqIfNotPlaying(0, 0x60);
        gSoundManager.SetSeqTrackMuteOnly(0, 0xFFF, TRUE);
        gSoundManager.SetSeqTempoRatio(0, 0xC6);
    }
    gSoundManager.LoadGroup(0x5C);

    for (int i = 0; i < 100; i++) {
        if (func_0202c354(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
        }
    }

    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_save_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/Scene/SceneSavePlay_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);

    mCharacters[0].Reset();
    if (mResidentCount == 0) {
        mCharacters[0].mIndex = 0;
    } else {
        mCharacters[0].mIndex = mResidents[Rand(mResidentCount)];
        func_0202c264(&gGameData, mCharacters[0].mIndex, &mCharacters[0]);
    }
    mCharacters[0].Character::SetArchive("MT0");
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
}

// @0x02149c4c
void SavePlayScene::Init() {
    func_0202b0e8(&data_02122824);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(31, 31, 31), 31, 0x7FFF, 63, FALSE);

    func_02028f24(&gVramTransferQueue, "MLT:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, sScreenName, 0x200, 0, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MLT:/BG_down.ncl", 0x200, -1, -1, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_down.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "MLT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    gScreenControl.Set(1);
    gScreenControl.Show(0x1200);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x7FFF, 0x101);

    if (mSceneId == 0x2D) {
        SetState(STATE_ASK_SAVE_WIRELESS);
    } else {
        SetState(STATE_ASK_SAVE);
    }
    mNextSceneId = gSceneManager.mPreviousSceneId;
    if (mNextSceneId == 0x6A) {
        mNextSceneId = 0x5F;
    }

    mEtcMessages.Open("TEM:tmd_etc_msg_JP.bmg");
    mCommonMessages.Open("TCM:tmd_common_msg_JP.bmg");

    func_0202ad04(data_021223e4, 0, mCharacters[0].mArchiveName, 0);
    mCharacters[0].Character::Init(0, 0, 0);
    void *animations = func_0202ad04(data_021223e4, 1, "BIC", 0);
    func_0202c264(&gGameData, mCharacters[0].mIndex, &mCharacters[0]);
    mCharacters[0].CharacterBase::MakeFace();
    mCharacters[0].Character::UpdateBodyScale();
    mCharacters[0].mPersonality = mCharacters[0].GetPersonality();
    mCharacters[0].UpdateBiorhythm();
    mCharacters[0].Character::UpdateAnimSpeed();
    mCharacters[0].Character::UpdateSpeedScales();
    mCharacters[0].Character::UpdateBodyColors();
    func_02029660((void *)mCharacters[0].mAnim.mUnknown04, RESOURCE_BCA0, animations);
    mCharacters[0].Character::SetAngle(0x4000);

    int count = 0;
    void *resourceSet = (void *)mCharacters[0].mAnim.mUnknown04;
    for (;;) {
        if (NNS_G3dGetAnmByIdx(func_020295a4(resourceSet, RESOURCE_BCA0), count) == NULL) {
            break;
        }
        count++;
    }
    mAnimationCount = count;
    mCharacters[0].Character::SetAnimation(RESOURCE_BCA0, Rand(mAnimationCount));
    mCharacters[0].Character::SetVisible(0);
    mCharacters[0].mShadowVisible = FALSE;
    mCamera.Init();
    mLights.Init(&mCamera);

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
    func_02033a58(&mWindow[0], etc, 0x17C, &gFonts[0].mFont, 0x1000, sTextPosition, &charOffset);
    func_02034f94(&mWindow[0], 1);
    func_02034f4c(&mWindow[0], 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 0);
}

// @0x0214a1ac
void SavePlayScene::Update() {
    if (mState != STATE_NONE) {
        if (mState <= STATE_STOPPED) {
            UpdateSaveStates();
        } else {
            UpdateWirelessStates();
        }
    }
    UpdateObjects();
}

// @0x0214a1e4
void SavePlayScene::Draw() {
    mCharacters[0].Character::Draw();
    func_020345d8(&mWindow[0]);
}

// @0x0214a204
void SavePlayScene::Exit() {
    gSoundManager.SetSeqTempoRatio(0, 0x100);
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[0].mArchiveName);
}

// @0x0214a294
void SavePlayScene::SetState(int state) {
    mState = state;
    mStep = 0;
    mSubStep = 0;
    mFrames = 0;
}

// @0x0214a2c0
void SavePlayScene::UpdateSaveStates() {
    switch (mState) {
    case STATE_NONE:
        break;
    case STATE_ASK_SAVE:
        if (Ask(0x17C, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW)) {
            if (mAnswer == MessageWindow::BUTTON_YES) {
                SetState(STATE_SAVING);
            } else {
                SetState(STATE_LEAVE);
            }
        }
        break;
    case STATE_SAVING:
        if (mStep == 0) {
            mStep++;
            mCharacters[0].Character::SetVisible(1);
        } else if (ShowMessage(0x17D)) {
            SetState(STATE_SAVE);
        }
        break;
    case STATE_SAVE:
        if (Save(SAVE_0)) {
            SetState(STATE_ASK_CONTINUE);
            gSoundManager.StartSeqArc(1, 0x18, 0);
        }
        break;
    case STATE_ASK_CONTINUE:
        if (mStep == 0) {
            mStep++;
            mCharacters[0].Character::SetVisible(0);
        } else if (Ask(0x17E, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW)) {
            if (mAnswer == MessageWindow::BUTTON_YES) {
                SetState(STATE_CONTINUE);
            } else {
                SetState(STATE_SAVE_QUIT);
            }
        }
        break;
    case STATE_CONTINUE:
        if (ShowOk(0x17F)) {
            SetState(STATE_SAVE_CONTINUE);
        }
        break;
    case STATE_SAVE_CONTINUE:
        if (Save(SAVE_2)) {
            SetState(STATE_LEAVE);
        }
        break;
    case STATE_SAVE_QUIT:
        if (Save(SAVE_3)) {
            SetState(STATE_QUIT);
        }
        break;
    case STATE_QUIT:
        if (ShowMessage(0x180)) {
            SetState(STATE_FADE_OUT);
            gSoundManager.MoveSeqVolume(0, 0, 0x78);
        }
        break;
    case STATE_LEAVE:
        if (mStep == 0) {
            mStep++;
            gSceneManager.RequestScene(mNextSceneId);
        }
        break;
    case STATE_FADE_OUT:
        mStep++;
        if (mStep > 0x7C) {
            SetState(STATE_STOPPED);
        }
        break;
    case STATE_STOPPED:
        if (mStep == 0) {
            mStep++;
        }
        break;
    }
}

// @0x0214a548
void SavePlayScene::UpdateWirelessStates() {
    switch (mState) {
    case STATE_ASK_SAVE_WIRELESS:
        if (Ask(0x181, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_8 | MessageWindow::FLAG_FROM_BELOW)) {
            if (mAnswer == MessageWindow::BUTTON_YES) {
                SetState(STATE_SAVING_WIRELESS);
            } else {
                SetState(STATE_LEAVE_WIRELESS);
                mNextSceneId = 0x5E;
            }
        }
        break;
    case STATE_SAVING_WIRELESS:
        if (mStep == 0) {
            mStep++;
            mCharacters[0].Character::SetVisible(1);
        } else if (ShowMessage(0x17D)) {
            SetState(STATE_SAVE_WIRELESS);
        }
        break;
    case STATE_SAVE_WIRELESS:
        if (Save(SAVE_1)) {
            SetState(STATE_SAVED_WIRELESS);
            gSoundManager.StartSeqArc(1, 0x18, 0);
        }
        break;
    case STATE_SAVED_WIRELESS:
        if (mStep == 0) {
            mStep++;
            mCharacters[0].Character::SetVisible(0);
        } else if (ShowOk(0x182)) {
            SetState(STATE_LEAVE_WIRELESS);
            mNextSceneId = 0x29;
        }
        break;
    case STATE_LEAVE_WIRELESS:
        if (mStep == 0) {
            mStep++;
            gSceneManager.RequestScene(mNextSceneId);
        }
        break;
    case STATE_17:
        break;
    }
}

// Writes the save data in one of four ways, waits until it is written (at least 120 frames, for the first two, and
// then closes the window), and returns TRUE when done
// @0x0214a6e0
BOOL SavePlayScene::Save(int kind) {
    if (mSubStep == 0) {
        BOOL ok;
        mSubStep++;
        mFrames = 0;
        if (kind == SAVE_0) {
            func_0202c8f8(&gGameData, 0);
            ok = func_0202cd48(&gGameData);
        } else if (kind == SAVE_1) {
            func_0202c8f8(&gGameData, 1);
            ok = func_0202cd48(&gGameData);
        } else if (kind == SAVE_2) {
            ok = func_0202cd54(&gGameData);
        } else if (kind == SAVE_3) {
            ok = func_0202cd74(&gGameData);
        } else {
            return TRUE;
        }
        if (!ok) {
            gSceneManager.RequestScene(0xC2);
        }
    } else if (mSubStep == 1) {
        mFrames++;
        if (!func_0202cd38(&gGameData)) {
            mSubStep++;
        }
    } else if (mSubStep == 2) {
        if (kind == SAVE_0 || kind == SAVE_1) {
            if (mFrames < 0x78) {
                mFrames++;
            } else {
                mSubStep++;
                mWindow[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_8 |
                                               MessageWindow::FLAG_FROM_BELOW);
            }
        } else {
            return TRUE;
        }
    } else if (mSubStep == 3) {
        if (mWindow[0].mTransitionDone) {
            mSubStep++;
        }
    } else {
        return TRUE;
    }
    return FALSE;
}

// Opens the window with a message and no buttons, and returns TRUE once it is shown
// @0x0214a888
BOOL SavePlayScene::ShowMessage(u32 message) {
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
// @0x0214a934
BOOL SavePlayScene::ShowOk(u32 message) {
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
// @0x0214aa0c
BOOL SavePlayScene::Ask(u32 message, int flags) {
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

// @0x0214ab34
void SavePlayScene::UpdateObjects() {
    func_020341d4(&mWindow[0]);
    mCamera.Update();
    mCharacters[0].Character::Update();
    mLights.Update();
}

// @0x0214ab6c
SavePlayCamera::SavePlayCamera() {
    mCamera = &data_02122764;
}

// @0x0214ab7c
void SavePlayCamera::Init() {
    func_0202a640(mCamera);
    mCamera->mUnknown00 = 0x281;
    mCamera->mUnknown04 = 0xFCE;
    VEC_Set(&mCamera->mPosition, 0x13000, 0x3666, 0x18000);
    VEC_Set(&mCamera->mTarget, 0, 0x1000, 0);
}

// @0x0214abe8
void SavePlayCamera::Update() {
}

// @0x0214abec
void SavePlayLights::Init(SavePlayCamera *camera) {
    mCamera = camera;
    for (int i = 0; i < 2; i++) {
        mLights[i] = &data_021227cc[i];
        func_0202a96c(mLights[i]);
    }
}

// @0x0214ac24
void SavePlayLights::Update() {
    func_0202ab2c(mLights[0], mCamera->mCamera->mAngleX, mCamera->mCamera->mAngleY, mCamera->mCamera->mAngleZ, 0x1388,
                  -0xFA0, -0x1B58);
    func_0202ab2c(mLights[1], mCamera->mCamera->mAngleX, mCamera->mCamera->mAngleY, mCamera->mCamera->mAngleZ, -0x2328,
                  -0x7D0, -0xE74);
}
