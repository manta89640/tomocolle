#include "../TutorialNames.h"

#include "prelude.h"

// The screen (tile map) of the message window's background. A constant of its own: the two uses share one address
static const char sWindowScreenName[] = "MLT:/Window_down2.nsc";
// The rectangle of the message window, in pixels
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
// The window's button: where it is and how large; and where the window's text is.
// These three are small constants (8 bytes), which the compiler keeps in one pool for the whole source file and
// reads from the pool's start: the original loads them from data_ov073_02150ba4 + 0x3C (button position), + 0x14
// (button size) and + 0x0C (text position). The offsets come out only when the overlay's other small constants
// are in the same file (groups b1's and b2's: the button sequences at + 0, an fx32 at + 8, the button position at
// + 0x1C, three pairs at + 0x24).
static const WindowPoint sWindowButtonPosition = { 128, 149 };
static const WindowPoint sWindowButtonSize = { 94, 30 };
static const WindowPoint sWindowTextPosition = { 38, 33 };

static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

// How many characters (tiles) of VRAM a sprite text canvas takes: ObjTextCanvas::GetCharCount, with the canvas
// reached through a pointer. Both uses in Init match in this form; the header's form (mCanvas read directly)
// matches the second use only, so this is probably how the class's own function reads.
static inline int GetCharCount(ObjTextCanvas &text) {
    const NNSG2dCharCanvas *canvas = &text.mCanvas;
    return ((canvas->areaWidth * canvas->areaHeight - 1) >> text.mVramMode) + 1;
}

// Halves a vector in place
static inline void Halve(VecFx32 *v) {
    v->x >>= 1;
    v->y >>= 1;
    v->z >>= 1;
}

// Lists the saved characters, chooses the two to show, and asks for the scene's archives.
// @0x02149a40
void TutorialRoomScene::StartLoading() {
    mClock.Update();
    gSoundManager.StartSeqIfNotPlaying(0, 11);
    gSoundManager.SetSeqTrackMuteOnly(0, 0x1C1, TRUE);
    gSoundManager.SetSeqTrackModDepth(0, 0xFFFF, 0);
    mTutorialStep = data_0213ba74.mTutorialStep;

    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
            mFront.mResidents[func_0202c414(&gGameData, i)] = i;
        }
    }

    if (!ChooseCharacters()) {
        func_0203c02c(&gSceneManager, 0x69);
        return;
    }
    LoadCharacters();
    if (mTutorialStep == 13) {
        // The visitor's event is to visit the resident
        mCharacters[1].mEvent = 0x17;
        mCharacters[1].mEventPartner = mCharacters[0].mIndex;
        mCharacters[0].mEvent = 0x18;
        mCharacters[0].mEventPartner = mCharacters[1].mIndex;
        SaveCharacters();
    } else if (mTutorialStep == 16) {
        mCharacters[0].mEvent = 0;
        mCharacters[1].mEvent = 0;
        SaveCharacters();
    }
    func_02091048(&mCharacters[0], "MT0");
    func_02091048(&mCharacters[1], "MT1");
    mRoom.SetStyle(mCharacters[0].mInterior);
    mRoom.mRoomIndex = mCharacters[0].mRoom;
    mFront.SetBuildingSize(0);
    mSky.Init(&mClock, 0);

    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/Scene/ScenePlayRoomTut_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMM", "Msg/TmdMainRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASP", "Scn/AFwBG/FwBgSkyPlt_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "APX", mFront.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mSky.mArchivePath, 0);
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);
}

// Sets up the screens, the room, the two characters, the message window and the touch screen, when the archives
// are loaded, and starts the part of the tutorial that is due.
//
// Not matching yet: 6 instructions, all of them the offsets of the three small constants in the file's pool
// (see sWindowButtonPosition above): "ldr [r1, #0x3c]/[r1, #0x40]", "[r1, #0x14]/[r1, #0x18]" and
// "[r0, #0xc]/[r0, #0x10]" in the original are #8/#12, #16/#20 and #0/#4 here. Everything else is identical.
// @0x02149d8c
void TutorialRoomScene::Init() {
    int i;
    void *file;
    void *animations;

    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(1, 1, 1), 0, 0x7FFF, 63, FALSE);
    func_0202680c(0x4000);

    func_02028f24(&gVramTransferQueue, "MLT:/BG_down.ncg", 0x100, 0, 0, 0x4000, -1);
    func_02027534(&data_021213b8, "MLT:/BG_down.ncl", 0x100, -1, -1, -1);
    func_02027d7c(&data_02121834, sWindowScreenName, 0x100, 0, 0, 0, 0, -1);
    mFront.mScreen = func_02027d7c(&data_02121834, "APX:/BG_window.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "APX:/BG_wall.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "APX:/BG_down.ncg", 0x600, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "APX:/BG_down.ncl", 0x600, -1, -1, 0x140);
    func_02028f24(&gVramTransferQueue, "MLT:/BG_sub.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/BG_sub.ncl", 2, -1, -1, -1);
    func_02027d7c(&data_02121834, "MLT:/BG_dialogue.nsc", 2, 0, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_down.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "MLT:/Obj_up.nce", "MLT:/Obj_up.ncg", 0x10, 1, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_up.ncl", 0x10, -1, -1, -1);
    mSky.mColors = (u16 *)func_02027534(&data_021213b8, "ASP:/Fw_BG_sky.ncl", 0x2F00, 0, 0, 2);
    func_02028f24(&gVramTransferQueue, mSky.mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
    mSky.mPalette = func_02027534(&data_021213b8, mSky.mPalettePath, 0x400, 0x80, 0x80, 0xC0);
    func_020278f8(&data_021213b8, 0, 0x101);
    func_02028280(&data_02121834, 1, 2);
    func_02028280(&data_02121834, 2, 1);
    data_0211d604.Set(0x1600);
    data_0211d604.Show(0x11);
    func_02023ee0(&data_0211d604, 2, 0x11, 8, 8);

    mEtcMessages.Open("TEM:/tmd_etc_msg_JP.bmg");
    mCommonMessages.Open("TCM:/tmd_common_msg_JP.bmg");
    mRoomMessages.Open("TMM:/tmd_mainroom_msg_JP.bmg");
    mRoom.Refresh();
    func_020a5514(&mRoom, 3);
    func_020a5760(&mRoom, 4);
    func_020a4acc(&mRoom.mParts, 10);

    file = ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa");
    func_02080e98(&mParticles, file, 3, 0x50, 0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    func_020810b4(&mIcon[0], &mParticles, 0);

    func_0202ad04(data_021223e4, 0, mCharacters[0].mArchiveName, 0);
    func_0202ad04(data_021223e4, 1, mCharacters[1].mArchiveName, 0);
    func_02090fdc(&mCharacters[0], 0, 0, 0);
    func_02090fdc(&mCharacters[1], 1, 0, 1);
    mCharacters[0].mPartner = &mCharacters[1];
    mCharacters[1].mPartner = &mCharacters[0];
    animations = func_0202ad04(data_021223e4, 2, "BIC", 0);
    for (i = 0; i < 2; i++) {
        LoadCharacter(&mCharacters[i]);
        func_0208fefc(&mCharacters[i]);
        func_02091a24(&mCharacters[i]);
        mCharacters[i].mPersonality = func_02098f2c(&mCharacters[i]);
        func_02091a6c(&mCharacters[i]);
        func_020910d4(&mCharacters[i]);
        func_02091a88(&mCharacters[i]);
        mCharacters[i].CopyName();
        func_02029660(mCharacters[i].mModel.mResource, RESOURCE_BCA0, animations);
        Character_SetAngle(&mCharacters[i], 0x4000);
        mCharacters[i].LoadHeadIcon(&mParticles);
        mCharacters[i].mApartmentRoom = &mRoom;
        mCharacters[i].mCamera = &mCamera;
    }

    mCamera.Reset();
    mCamera.mCharacters[0] = &mCharacters[0];
    mCamera.mCharacters[1] = &mCharacters[1];
    mCamera.mRoom = &mRoom;
    mLight.Init(&mCamera, &mRoom);

    int charOffset = 0x200;
    ScreenRect rect = sWindowRect;
    func_02032fb8(&mWindow[0], rect);
    func_02033174(&mWindow[0], sWindowScreenName, 0x100);
    func_020331bc(&mWindow[0], 0x3F00, rect);
    func_020336b4(&mWindow[0], 0, 0, 6, 7, sWindowButtonPosition, sWindowButtonSize, &mCommonMessages, 4,
                  &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, 10);
    func_02033a58(&mWindow[0], &mEtcMessages, 0x159, &gFonts[0].mFont, 0x1000, sWindowTextPosition, &charOffset);
    func_02034f94(&mWindow[0], 1);
    func_02034f4c(&mWindow[0], 1);
    func_02028280(&data_02121834, 0x100, 0);
    mWindow[0].mTexts[0].SetStringArg(mCharacters[0].mNameText, 0);

    mButtonLabel[0].InitObjForMessage(&mEtcMessages, 0xB3, &gFonts[0].mFont, 0x1000, charOffset, 0, 1, 0, 0, 15,
                                      NULL);
    mButtonLabel[0].mVAlign = 1;
    mButtonLabel[0].mShadowDirections = 0xAA;
    mButtonLabel[0].DrawAll();
    charOffset += GetCharCount(mButtonLabel[0]);

    int speechOffset = 0xC0;
    mTimedMessage.InitObjForMessage(&mRoomMessages, 3, &gFonts[0].mFont, 0x10, speechOffset, 0, 1, 0, 0, 15, NULL);
    mTimedMessage.mSprite.SetPosition(0x10000, 0xA0000);
    mTimedMessage.mVAlign = 1;
    speechOffset += GetCharCount(mTimedMessage);
    mNamePanel.SetNames(mRoom.mNumberText, mCharacters[0].mNameText);
    mNamePanel.Init(speechOffset, &mRoomMessages, &mRoomMessages);

    mButtons[0].mLabel = &mButtonLabel[0];
    mButtons[0].Init(0);
    mPressedButton = 0;
    mFront.Init(mCharacters[0].mRoom);
    mSky.Refresh();
    mClockDisplay.Init(&mClock);

    StartTutorialStep();
    if (mCharacters[1].mPresent) {
        mCamera.SetMode(RoomCamera::MODE_BOTH);
    } else if (mCharacters[0].mPresent) {
        mCamera.SetMode(RoomCamera::MODE_CHARACTER);
    } else {
        mCamera.SetMode(RoomCamera::MODE_ROOM);
    }
    gSoundManager.LoadGroup(0x4C);
}

// @0x0214a784
void TutorialRoomScene::Update() {
    if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
        mCamera.mInput.y = 0x10000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
        mCamera.mInput.y = -0x10000;
    }
    if (data_0211f9d4.mHeld & PAD_KEY_UP) {
        mCamera.mInput.x = 0xE000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
        mCamera.mInput.x = -0xE000;
    }
    UpdateState();
    UpdateObjects();
}

// @0x0214a814
void TutorialRoomScene::Draw() {
    UpdateFade();
    func_020a4a50(&mRoom.mParts);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Draw();
    }
    func_02080f00(&mParticles, 0);
    mTimedMessage.Draw();
    func_020345d8(&mWindow[0]);
    mButtonLabel[0].Draw();
    mNamePanel.Draw();
    mButtons[0].Draw();
    mFront.Draw();
    mClockDisplay.Draw();
}

// @0x0214a8b8
void TutorialRoomScene::Exit() {
    SaveCharacters();
    func_02023f4c(&data_0211d604, 0x3F00, 0);
    func_02023eac(&data_0211d604, 0x3F3F);
    func_0202683c();
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    func_0202aecc(data_021223e4, 2);
    func_020a5948(&mRoom);
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TMM");
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ArchiveManager_Unmount(&gArchiveManager, "APX");
    ArchiveManager_Unmount(&gArchiveManager, "ASP");
    ArchiveManager_Unmount(&gArchiveManager, "ASX");
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[0].mArchiveName);
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[1].mArchiveName);
}

// Chooses the resident and the visitor from the saved characters that exist, for the part of the tutorial that is
// due. Returns FALSE if the scene has no part for the tutorial's step.
// @0x0214a9d8
BOOL TutorialRoomScene::ChooseCharacters() {
    switch (mTutorialStep) {
    case 8:
        if (mResidentCount == 0) {
            UseDefaultCharacters();
        } else if (mResidentCount == 1) {
            mCharacters[0].mIndex = mResidents[0];
            if (mResidents[0] != 1) {
                mCharacters[1].mIndex = 1;
            } else {
                mCharacters[1].mIndex = 2;
            }
        } else {
            mCharacters[0].mIndex = mResidents[0];
            mCharacters[1].mIndex = mResidents[1];
        }
        break;
    case 13:
        if (mResidentCount <= 1) {
            UseDefaultCharacters();
        } else {
            mCharacters[0].mIndex = mResidents[1];
            mCharacters[1].mIndex = mResidents[0];
        }
        break;
    case 16:
        if (mResidentCount <= 1) {
            UseDefaultCharacters();
        } else {
            mCharacters[0].mIndex = mResidents[1];
            mCharacters[1].mIndex = mResidents[0];
        }
        break;
    case 21:
        if (mResidentCount <= 1) {
            UseDefaultCharacters();
        } else {
            mCharacters[0].mIndex = mResidents[1];
            mCharacters[1].mIndex = mResidents[0];
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

// The first two saved characters, whether they exist or not.
// @0x0214ab2c
void TutorialRoomScene::UseDefaultCharacters() {
    mCharacters[0].mIndex = 0;
    mCharacters[1].mIndex = 1;
}

// Starts the part of the tutorial that is due: the state, who is in the room and where, the camera and the light.
// @0x0214ab48
void TutorialRoomScene::StartTutorialStep() {
    switch (mTutorialStep) {
    case 8:
        SetState(STATE_RESIDENT_ARRIVES);
        func_02023f4c(&data_0211d604, 0x1F00, -10);
        mCamera.mOrbit.y = 0x1400000;
        mLight.SetState(RoomLight::STATE_SET_ROOM_COLOR);
        break;
    case 13:
        SetState(STATE_VISITOR_ARRIVES);
        mCharacters[0].mPresent = TRUE;
        mCharacters[0].SetState(4);
        Character_SetAngle(&mCharacters[0], 0x1800);
        func_02023f4c(&data_0211d604, 0x1F00, -10);
        mCamera.mOrbit.y = 0x1500000;
        mLight.SetState(RoomLight::STATE_SET_STYLE_COLOR);
        mTimedMessage.SetState(TimedMessage::STATE_SHOW);
        mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
        mTimedMessage.mMessage = 3;
        break;
    case 16: {
        SetState(STATE_EXPLAIN);
        mCharacters[0].mPresent = TRUE;
        mCharacters[0].SetState(3);
        mCharacters[0].mWait = 120;
        mCharacters[0].mModel.mPos.x = -0x1000;
        Character_SetAngle(&mCharacters[0], 0x2000);
        FloorPos target;
        mCharacters[0].GetRandomPoint(&target);
        mCharacters[0].SetTarget(target.x, target.z);
        func_02023f4c(&data_0211d604, 0x1F00, -10);
        mCamera.mOrbit.y = -0x1000000;
        mLight.SetState(RoomLight::STATE_SET_STYLE_COLOR);
        mTimedMessage.SetState(TimedMessage::STATE_SHOW);
        mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
        mTimedMessage.mMessage = 3;
        break;
    }
    case 21: {
        SetState(STATE_WAIT_TOUCH);
        mCharacters[0].mPresent = TRUE;
        mCharacters[0].SetState(3);
        mCharacters[0].mWait = 120;
        mCharacters[0].mModel.mPos.x = 0x1000;
        Character_SetAngle(&mCharacters[0], 0x5800);
        FloorPos target;
        mCharacters[0].GetRandomPoint(&target);
        mCharacters[0].SetTarget(target.x, target.z);
        mCamera.mOrbit.y = 0x1200000;
        mLight.SetState(RoomLight::STATE_SET_STYLE_COLOR);
        mTimedMessage.SetState(TimedMessage::STATE_SHOW);
        mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
        mTimedMessage.mMessage = 3;
        break;
    }
    }
}

// The script of each part of the tutorial.
// @0x0214ae08
void TutorialRoomScene::UpdateState() {
    switch (mState) {
    case STATE_NONE:
        break;

    case STATE_RESIDENT_ARRIVES:
        if (mStep == 0) {
            mTimer++;
            if (mTimer > 55) {
                mStep++;
                mTimer = 0;
                mLight.SetState(RoomLight::STATE_FADE_STYLE_COLOR);
                mCharacters[0].mEvent = 0;
                SaveCharacter(&mCharacters[0]);
                mFront.DrawWindow(mCharacters[0].mRoom);
            }
        } else if (mStep == 1) {
            mTimer++;
            if (mTimer > 60) {
                mStep++;
                mCharacters[0].mPresent = TRUE;
                mCharacters[0].SetState(10);
                mCamera.SetMode(RoomCamera::MODE_CHARACTER);
                ShowMessage(&mCharacters[0], NULL, 9, 2);
            }
        } else if (mStep == 2) {
            if (mCharacters[0].mState == 0) {
                mStep++;
                mTimer = 0;
                mCharacters[0].SetState(1);
                mCharacters[0].mWait = 80;
            }
        } else if (mStep == 3) {
            mTimer++;
            if (mTimer > 240) {
                mStep++;
                func_02023f4c(&data_0211d604, 0x1F00, 0);
                func_02034f38(&mWindow[0], 0, 0x159);
                func_02034a3c(&mWindow[0], 0, 0, mCharacters[0].mNameText);
                func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 0);
            }
            UpdateIdle();
        } else if (mStep == 4) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_FROM_BELOW);
            }
            UpdateIdle();
        } else if (mStep == 5) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
            }
            UpdateIdle();
        } else {
            if (UpdateTouch()) {
                mStep++;
                SetState(STATE_ENDING);
            } else {
                UpdateIdle();
            }
        }
        break;

    case STATE_VISITOR_ARRIVES:
        if (mStep == 0) {
            mTimer++;
            if (mTimer > 170) {
                mStep++;
                mTimer = 0;
                gSoundManager.StartSeqArc(1, 0x1D, 16);
            }
        } else if (mStep == 1) {
            mTimer++;
            if (mTimer > 40) {
                mStep++;
                mTimer = 0;
                mCharacters[1].mPresent = TRUE;
                mCharacters[1].SetState(11);
                mCharacters[0].SetState(14);
                mCamera.SetMode(RoomCamera::MODE_BOTH);
            }
        } else if (mStep == 2) {
            mTimer++;
            if (mTimer > 60) {
                mStep++;
                mTimer = 0;
                mCharacters[1].SetState(12);
                mTimedMessage.SetState(TimedMessage::STATE_SHOW);
                mTimedMessage.SetStringArg(mCharacters[1].mNameText, 0);
                mTimedMessage.mMessage = 9;
            }
        } else if (mStep == 3) {
            if (mCharacters[1].mState == 0) {
                mStep++;
                mCharacters[1].SetState(5);
            }
        } else {
            if (AreCharactersNear(0, 1)) {
                SetState(STATE_TALK);
                mCharacters[0].SetState(6);
                mCharacters[1].SetState(6);
            }
        }
        break;

    case STATE_TALK:
        if (mStep == 0) {
            if (mCharacters[0].mState == 0 && mCharacters[1].mState == 0) {
                mStep++;
                mCharacters[1].SetState(7);
                mCharacters[1].mWait = 160;
                mCharacters[0].mWait = 150;
                func_02099088(&mCharacters[1], mCharacters[0].mIndex, data_021135c8[0][1] * 2);
            }
        } else {
            // The one whose wait has run out says something, and both like each other more for it
            RoomCharacter *speaker = NULL;
            RoomCharacter *listener;
            for (int i = 0; i < 2; i++) {
                if (mCharacters[i].mWait <= 0) {
                    speaker = &mCharacters[i];
                    listener = speaker->mPartner;
                    break;
                }
            }
            if (speaker == NULL) {
                break;
            }
            if (Random(100) < 70) {
                speaker->SetState(7);
                func_02099088(speaker, listener->mIndex, data_021135c8[0][1] * 2);
            } else {
                speaker->SetState(8);
                func_02099088(speaker, listener->mIndex, data_021135c8[0][10] * 2);
            }
            speaker->mWait = 180;
            listener->mWait = 170;
            if (speaker->mAffinity[listener->mIndex] >= 200 && listener->mAffinity[speaker->mIndex] >= 200) {
                SetState(STATE_BECOME_FRIENDS);
            }
        }
        break;

    case STATE_BECOME_FRIENDS:
        if (mStep == 0) {
            if (mCharacters[0].mWait <= 0 && mCharacters[1].mWait <= 0) {
                mStep++;
                mTimer = 0;
                func_020990b0(&mCharacters[0], mCharacters[1].mIndex, 1, 100);
                func_020990b0(&mCharacters[1], mCharacters[0].mIndex, 1, 100);
                mCharacters[0].SetState(9);
                mCharacters[1].SetState(9);
                // The mark goes halfway between the two
                VecFx32 position = CharacterModel_GetJointPosition(&mCharacters[0].mModel, 3);
                VecFx32 other = CharacterModel_GetJointPosition(&mCharacters[1].mModel, 3);
                VEC_Add(&position, &other, &position);
                Halve(&position);
                VEC_Set(&mIcon[0].mPos, position.x, position.y, position.z);
            }
        } else if (mStep == 1) {
            mTimer++;
            if (mTimer > 40) {
                mStep++;
                func_02080fc4(&mIcon[0]);
                mIcon[0].mIconId = 4;
                func_020810c8(&mIcon[0]);
                HeadIcon_SetMode(&mIcon[0], 2);
                gSoundManager.StartSeqArc(1, 0x26, 0);
                mTimedMessage.SetState(TimedMessage::STATE_SHOW);
                mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
                mTimedMessage.SetStringArg(mCharacters[1].mNameText, 1);
                mTimedMessage.mMessage = 10;
            }
        } else {
            if (mCharacters[0].mState == 0 && mCharacters[1].mState == 0 && mTimedMessage.mDrawMode == 0) {
                SetState(STATE_EXPLAIN_FRIENDS);
                mCharacters[0].SetState(1);
                mCharacters[1].SetState(1);
                HeadIcon_SetMode(&mIcon[0], 1);
            }
        }
        break;

    case STATE_EXPLAIN_FRIENDS:
        if (mStep == 0) {
            mStep++;
            mWindow[0].mTexts[0].mMessage = 0x165;
            func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 0);
        } else if (mStep == 1) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindow[0], 0x1C);
            }
        } else if (mStep == 2) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mWindow[0].mTexts[0].mMessage = 0x166;
                func_02034720(&mWindow[0], 0x1A, 0);
            }
        } else if (mStep == 3) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], 0x1C);
            }
        } else if (mStep == 4) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mWindow[0].mTexts[0].mMessage = 0x167;
                func_02034720(&mWindow[0], 0x1A, 0);
            }
        } else if (mStep == 5) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], 0x1C);
            }
        } else if (mStep == 6) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mTimer = 0;
            }
        } else {
            mTimer++;
            if (mTimer > 50) {
                SetState(STATE_LEAVE);
            }
        }
        break;

    case STATE_EXPLAIN:
        if (mStep == 0) {
            mTimer++;
            if (mTimer > 60) {
                mStep++;
                func_02023f4c(&data_0211d604, 0x1F00, 0);
                func_02034f38(&mWindow[0], 0, 0x16C);
                func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 0);
            }
            UpdateIdle();
        } else if (mStep == 1) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_FROM_BELOW);
            }
            UpdateIdle();
        } else if (mStep == 2) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
            }
            UpdateIdle();
        } else {
            if (UpdateTouch()) {
                SetState(STATE_ENDING);
            } else {
                UpdateIdle();
            }
        }
        break;

    case STATE_WAIT_TOUCH:
        if (mStep == 0) {
            if (UpdateTouch()) {
                SetState(STATE_ENDING);
            } else {
                UpdateIdle();
            }
        }
        break;

    case STATE_ENDING:
        UpdateEnding();
        break;

    case STATE_LEAVE:
        SetState(STATE_LEAVING);
        func_0203c02c(&gSceneManager, 0x69);
        break;

    case STATE_LEAVING:
        break;
    }
}

// Reads the clock and updates everything in the room and on the touch screen.
// @0x0214ba98
void TutorialRoomScene::UpdateObjects() {
    mClock.UpdateIfArchiveIdle();
    mCamera.Update();
    func_020341d4(&mWindow[0]);
    func_0207b138(mParticles.mManager);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Update();
    }
    func_020810e4(&mIcon[0]);
    mRoom.Update();
    mLight.Update();
    mFront.Update();
    mSky.Update();
    mTimedMessage.Update();
    mNamePanel.Update();
    mClockDisplay.Update();
    mButtons[0].Update();
}

// While the scene is being shown, the objects are updated from Draw (Update does not run yet).
// @0x0214bb64
void TutorialRoomScene::UpdateFade() {
    switch (mFadeState) {
    case FADE_NONE:
        break;
    case FADE_SHOW:
        mFadeStep++;
        if (mFadeStep > 3 || mPhase > PHASE_FADE_IN) {
            SetFadeState(FADE_NONE);
        } else {
            UpdateObjects();
        }
        break;
    }
}

// @0x0214bbb4
void TutorialRoomScene::SetState(int state) {
    mState = state;
    mStep = 0;
    mTimer = 0;
}

// @0x0214bbcc
void TutorialRoomScene::SetFadeState(int state) {
    mFadeState = state;
    mFadeStep = 0;
}
