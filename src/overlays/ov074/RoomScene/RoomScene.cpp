#include "prelude.h"

typedef char CheckSavedCharacterSize[sizeof(SavedCharacter) == 0x2C0 ? 1 : -1];
typedef char CheckRoomSceneSize[sizeof(RoomScene) == 0x1D7C8 ? 1 : -1];

// The archive of each kind of thing a character can hold, by number (data_ov074_0216d754, used by group a7)

// Counts the saved characters, chooses the two to show, and asks for the scene's archives.
void RoomScene::StartLoading() {
    u8 last;
    u8 building;
    u8 first;
    u8 second;

    mClock.Update();
    gSoundManager.StartSeqIfNotPlaying(0, 11);
    gSoundManager.SetSeqTrackMuteOnly(0, 0x1C1, TRUE);
    gSoundManager.SetSeqTrackModDepth(0, 0xFFFF, 0);
    func_020ad568(mClock.mSeconds);
    func_020ad514(mClock.mSeconds, &mClock.mTime);
    func_020ad294(mClock.mSeconds, &mClock.mTime);

    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
            mFront.mResidents[func_0202c414(&gGameData, i)] = i;
        }
    }

    last = gSceneMemory.mCharacter;
    if (last < 100 && func_0202c3dc(&gGameData, last)) {
        ChooseCharacters(last);
    } else {
        ChooseCharacters(0);
        gSceneMemory.mCharacter = mCharacters[0].mIndex;
    }
    LoadCharacters();
    func_02091048(&mCharacters[0], "3HM");
    func_02091048(&mCharacters[1], "BI1");
    mRoom.SetStyle(mCharacters[0].mInterior);
    mRoom.mRoomIndex = mCharacters[0].mRoomNumber;

    if (mSceneId == 0x11) {
        building = gGameData.mBuilding & 3;
    } else {
        building = mSceneId - 0x12;
    }
    mFront.SetBuildingSize(building);
    mSky.Init(&mClock, building);

    ArchiveManager_MountCompressed(&gArchiveManager, "MNU", "Scn/Scene/ScenePlayRoom2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMM", "Msg/TmdMainRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASP", "Scn/AFwBG/FwBgSkyPlt_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "APX", mFront.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mSky.mArchivePath, 0);
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);

    GetEventItems(&first, &second);
    mItems[0].SetArchive("HI0", first);
    mItems[1].SetArchive("HI1", second);
    mItems[2].SetArchive("HI2", first);
    mItems[3].SetArchive("HI3", second);
    for (int i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        ArchiveManager_MountCompressed(&gArchiveManager, item->mArchiveName, item->mArchivePath, 0);
    }
}

// Which two things the resident holds for its event (the numbers of their archives), 0 for none.
void RoomScene::GetEventItems(u8 *first, u8 *second) {
    u32 event = mCharacters[0].mEvent;

    *first = 0;
    *second = 0;
    switch (event) {
    case 1:
    case 0x22:
        *first = 9;
        break;
    case 4:
        *first = 6;
        break;
    case 5:
    case 0x2A:
        *first = 2;
        *second = 3;
        break;
    case 8:
    case 0x26:
        *first = 7;
        break;
    case 9:
    case 0x20:
        *first = 1;
        break;
    case 10:
    case 0x1E:
        *first = 8;
        break;
    case 12:
    case 0x24:
        *first = 9;
        *second = 5;
        break;
    case 16:
    case 0x28:
        *first = 0;
        break;
    case 18:
        *first = 10;
        break;
    }
}

// Makes the saved character with that index the room's resident, and chooses the second character: the partner
// of the resident's event, or else the first other character there is.
void RoomScene::ChooseCharacters(int resident) {
    int other;

    if (mResidentCount == 0) {
        mCharacters[0].mIndex = 0;
        mCharacters[1].mIndex = 1;
    } else if (mResidentCount == 1) {
        mCharacters[0].mIndex = resident;
        if (resident != 1) {
            mCharacters[1].mIndex = 1;
        } else {
            mCharacters[1].mIndex = 2;
        }
    } else {
        if (func_02099cec(func_0202c468(&gGameData, resident))) {
            other = func_0202c490(&gGameData, resident);
        } else {
            other = mResidents[0];
            if (other == resident) {
                other = mResidents[1];
            }
        }
        mCharacters[0].mIndex = resident;
        mCharacters[1].mIndex = other;
    }
}

// Sets up the screens, the room, the two characters and the touch screen, when the archives are loaded.
// @0x0214a02c
void RoomScene::Init() {
    int i;
    void *file;
    void *animations;
    int charOffset;

    func_0202b0e8(&data_02122824);
    func_0202ae38(data_021223e4);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(1, 1, 1), 0, 0x7FFF, 63, FALSE);
    func_0202680c(0x4000);

    ObjResourceManager_Load(&gObjResourceManager, "MNU:/Obj_down.nce", "MNU:/Obj_down.ncg", 0x1000, 0, -1);
    func_02027534(&data_021213b8, "MNU:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "MNU:/Obj_up.nce", "MNU:/Obj_up.ncg", 0x10, 1, -1);
    func_02027534(&data_021213b8, "MNU:/Obj_up.ncl", 0x10, -1, -1, -1);
    mFront.mScreen = (u16 *)func_02027d7c(&data_02121834, "APX:/BG_window.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "APX:/BG_wall.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "APX:/BG_down.ncg", 0x600, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "APX:/BG_down.ncl", 0x600, -1, -1, -1);
    func_02027d7c(&data_02121834, "MNU:/BG_dialogue.nsc", 2, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "MNU:/BG_sub.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MNU:/BG_sub.ncl", 2, -1, -1, -1);
    mSky.mColors = (u16 *)func_02027534(&data_021213b8, "ASP:/Fw_BG_sky.ncl", 0x2F00, 0, 0, 2);
    func_02028f24(&gVramTransferQueue, mSky.mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
    mSky.mPalette = func_02027534(&data_021213b8, mSky.mPalettePath, 0x400, 0x80, 0x80, 0xC0);
    func_020278f8(&data_021213b8, 0, 1);
    func_02028280(&data_02121834, 1, 2);
    func_02028280(&data_02121834, 2, 1);
    func_02028108(&data_02121834, 2, 0);
    func_02028154(&data_02121834, 2, 0);
    func_02028108(&data_02121834, 0x200, 0);
    func_02028154(&data_02121834, 0x200, -0xC0);
    func_02028108(&data_02121834, 0x400, 0);
    func_02028154(&data_02121834, 0x400, -0xC0);
    data_0211d604.Set(0x1600);
    data_0211d604.Show(0x11);
    func_02023ee0(&data_0211d604, 2, 0x11, 8, 8);
    SetInputState(INPUT_IDLE);

    file = ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa");
    func_02080e98(&mParticles, file, 3, 0x50, 0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    mIcon[0].Load(&mParticles, 0);
    mRoom.Refresh();
    func_020a5514(&mRoom, 7);
    func_020a5760(&mRoom, 8);
    func_020a4acc(&mRoom.mParts, 10);
    SetDoorUnknown13C(&mRoom.mParts.mDoor, 2);
    for (i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        func_0202ad04(data_021223e4, i + 2, item->mArchiveName, 0);
        item->Init(i + 2);
    }
    mTtsPlayer.InitEngine();

    func_0202ad04(data_021223e4, 0, mCharacters[0].mArchiveName, 0);
    func_0202ad04(data_021223e4, 1, mCharacters[1].mArchiveName, 0);
    mCharacters[0].Init(0, 0, 0);
    mCharacters[1].Init(1, 0, 1);
    mCharacters[0].mPartner = &mCharacters[1];
    mCharacters[1].mPartner = &mCharacters[0];
    mCharacters[0].mHeldItems[0] = &mItems[0];
    mCharacters[0].mHeldItems[1] = &mItems[1];
    mCharacters[1].mHeldItems[0] = &mItems[2];
    mCharacters[1].mHeldItems[1] = &mItems[3];
    mCharacters[0].mSoundHandle = 4;
    mCharacters[1].mSoundHandle = 5;
    mCharacters[0].mPresent = FALSE;
    mCharacters[1].mPresent = FALSE;
    animations = func_0202ad04(data_021223e4, 6, "BIC", 0);
    for (int i = 0; i < 2; i++) {
        LoadCharacter(&mCharacters[i]);
        func_0208fefc(&mCharacters[i]);
        func_02091a24(&mCharacters[i]);
        mCharacters[i].mPersonality = func_02098f2c(&mCharacters[i]);
        mCharacters[i].ResetBody();
        func_02091a88(&mCharacters[i]);
        mCharacters[i].CopyName();
        mCharacters[i].mTtsPlayer = &mTtsPlayer;
        mCharacters[i].RefreshSpeechTexts();
        func_02029660((void *)mCharacters[i].mModel.mUnknown04, RESOURCE_BCA0, animations);
        mCharacters[i].SetState(1);
        mCharacters[i].LoadHeadIcon(&mParticles);
        mCharacters[i].mRoom = &mRoom;
        mCharacters[i].mCamera = &mCamera;
    }
    PlaceCharacters();
    StartCharacterEvents();

    mCamera.Reset();
    mCamera.mCharacters[0] = &mCharacters[0];
    mCamera.mCharacters[1] = &mCharacters[1];
    mCamera.mRoom = &mRoom;
    mLight.Init(&mCamera, &mRoom);
    AimCamera();
    StartRoomEvent();

    mRoomMessages.Open("TMM:/tmd_mainroom_msg_JP.bmg");
    mEtcMessages.Open("TEM:/tmd_etc_msg_JP.bmg");
    charOffset = 0xC0;
    mMessage.InitObjForMessage(&mRoomMessages, 3, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    mMessage.mSprite.SetPosition(0x10000, 0xA0000);
    mMessage.mVAlign = 1;
    charOffset += GetCanvasCharCount(&mMessage);
    if (mFront.mResidents[mCharacters[0].mRoomNumber] != 0xFF) {
        ShowEventMessage();
    }
    mNamePanel.SetNames(mRoom.mNumberText, mCharacters[0].mNameText);
    mNamePanel.Init(charOffset, &mRoomMessages, &mRoomMessages);

    mButtonLabel[0].InitObjForMessage(&mEtcMessages, 0xB3, &gFonts[0].mFont, 0x1000, 0x300, 0, 1, 0, 0, 15, NULL);
    mButtonLabel[0].mVAlign = 1;
    mButtonLabel[0].mShadowDirections = 0xAA;
    mButtonLabel[0].DrawAll();
    mUnknownClearedByInit = 0;
    mButtons[0].mLabel = &mButtonLabel[0];
    for (s16 j = 0; j < 2; j++) {
        mButtons[j].Init(j);
    }
    mFront.Init(mCharacters[0].mRoomNumber);
    mSky.Refresh();
    mClockDisplay.Init(&mClock);

    // Coming from the character distribution scene with a resident that has just moved in
    if (gSceneManager.mPreviousSceneId == 0x74 && GetEventEnd(&mCharacters[0]) == 0x7FFFFFFFFFFFFFFFLL) {
        SetInputState(INPUT_OFF);
        SetEventState(EVENT_AFTER_DISTRIBUTION, 60);
        mCamera.SetState(2, 0);
        mMessage.SetState(4);
    }
    UpdateButtons();
    mMessage.Update();
    NNS_GfdDumpLnkTexVramManager();
    func_02060c1c();
    gSoundManager.LoadGroup(0x4C);
}

void RoomScene::Update() {
    if ((data_0211f9d4.mHeld & 0x2FFF) != 0 || gTouchInput.mTouching != 0) {
        mIdleFrames = 0;
        mCamera.SetState(4, 0x474);
    }
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
    UpdateInput();
    UpdateEvent();
    UpdateObjects();
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

void RoomScene::Draw() {
    switch (mFadeState) {
    case 0:
        break;
    case 1:
        mFadeStep++;
        if (mFadeStep >= 10 || mPhase > PHASE_FADE_IN) {
            SetFadeState(0);
        } else if (mFadeStep >= 6) {
            if (mFadeStep == 6) {
                func_02023c54(&data_0211d604, 0x3F3F, 0x2AE1);
            }
            UpdateObjects();
        } else {
            func_02023be8(&data_0211d604, 0x3F3F, 0x10000);
            UpdateObjects();
        }
        break;
    case 2:
        mFadeStep++;
        if (ABS(func_02023c44(&data_0211d604, 0x3F)) > 10) {
            SetFadeState(0);
        } else if (mFadeStep == 1) {
            func_02023c54(&data_0211d604, 0x3F3F, 0x2000);
        } else if (mFadeStep > 1) {
            UpdateObjects();
        }
        break;
    }

    func_020a4a50(&mRoom.mParts);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].UpdateBodyIfPresent();
    }
    for (int i = 0; i < 4; i++) {
        mItems[i].Draw();
    }
    func_02080f00(&mParticles, 0);
    mMessage.Draw();
    mButtonLabel[0].Draw();
    mNamePanel.Draw();
    for (int i = 0; i < 2; i++) {
        mButtons[i].Draw();
    }
    mFront.Draw();
    mClockDisplay.Draw();
}

void RoomScene::Exit() {
    func_02023eac(&data_0211d604, 0x3F);
    FinishEncounter();
    SaveCharacters();
    func_0202683c();
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    func_0202aecc(data_021223e4, 2);
    func_0202aecc(data_021223e4, 3);
    func_0202aecc(data_021223e4, 4);
    func_0202aecc(data_021223e4, 5);
    func_0202aecc(data_021223e4, 6);
    func_020a5948(&mRoom);
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[0].mArchiveName);
    ArchiveManager_Unmount(&gArchiveManager, "MNU");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ArchiveManager_Unmount(&gArchiveManager, "TMM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "APX");
    ArchiveManager_Unmount(&gArchiveManager, "ASP");
    ArchiveManager_Unmount(&gArchiveManager, "ASX");
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[1].mArchiveName);
    for (int i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        ArchiveManager_Unmount(&gArchiveManager, item->mArchiveName);
    }
}

// Reads the clock and updates everything in the room and on the touch screen.
void RoomScene::UpdateObjects() {
    mClock.UpdateIfArchiveIdle();
    func_0207b138(mParticles.mManager);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Update();
    }
    for (int i = 0; i < 4; i++) {
        mItems[i].Update();
    }
    mIcon[0].Update();
    mCamera.Update();
    mRoom.Update();
    mLight.Update();
    mFront.Update();
    mSky.Update();
    mMessage.Update();
    mNamePanel.Update();
    for (int i = 0; i < 2; i++) {
        mButtons[i].Update();
    }
    mClockDisplay.Update();
    mTtsPlayer.Update();
}

// Puts the two characters at random places, on opposite sides of the room's middle, facing at random.
void RoomScene::PlaceCharacters() {
    fx32 radius = (MATH_Rand16(&gRandom, 9) << 9) + 0x1000;
    fx32 otherRadius = 0x3000 - radius;
    if (otherRadius > 0x2000) {
        otherRadius = 0x2000;
    }

    u16 angle = MATH_Rand16(&gRandom, 0x100) << 8;
    mCharacters[0].mModel.mPos.x = FX_Mul(radius, FX_CosIdx(angle));
    mCharacters[0].mModel.mPos.z = FX_Mul(radius, FX_SinIdx(angle));

    angle = angle + 0x4000 + (MATH_Rand16(&gRandom, 0x100) << 7);
    mCharacters[1].mModel.mPos.x = FX_Mul(otherRadius, FX_CosIdx(angle));
    mCharacters[1].mModel.mPos.z = FX_Mul(otherRadius, FX_SinIdx(angle));

    // The directions go through an int: that is what leaves two conversions to u16 behind
    int direction = MATH_Rand16(&gRandom, 0);
    Character_SetAngle(&mCharacters[0], direction);
    direction = MATH_Rand16(&gRandom, 0);
    Character_SetAngle(&mCharacters[1], direction);
}

// Starts what the resident's event has the characters do, unless the resident is out.
void RoomScene::StartCharacterEvents() {
    u8 event = mCharacters[0].mEvent;

    if (func_02099c94(event)) {
        mCharacters[0].mPresent = FALSE;
        return;
    }
    if (func_02099db8(event)) {
        StartPairEvent(1);
        mCharacters[0].mPresent = TRUE;
        mCharacters[1].mPresent = TRUE;
        return;
    }
    StartEvent(&mCharacters[0], 1);
    mCharacters[0].mPresent = TRUE;
}

// Points the camera at as many characters as are in the room.
void RoomScene::AimCamera() {
    int count = CountPresent();

    if (count == 0) {
        mCamera.SetMode(1);
        mLight.SetState(3);
    } else if (count == 1) {
        mCamera.SetMode(2);
        if (mCharacters[0].mEvent == 18) {
            mLight.SetState(3);
        } else {
            mLight.SetState(1);
        }
    } else {
        mCamera.SetMode(3);
        mLight.SetState(1);
    }
}

// Starts the event state for as many characters as are in the room.
void RoomScene::StartRoomEvent() {
    int count = CountPresent();

    if (count == 0) {
        SetEventState(EVENT_NOBODY_HOME, -1);
    } else if (count == 1) {
        SetEventState(EVENT_ALONE, -1);
    } else {
        SetEventState(EVENT_TOGETHER, -1);
    }
}

// Does what a button is for, when the stylus is let go on it.
void RoomScene::PressButton(int button) {
    switch (mButtons[button].mKind) {
    case 0:
        EnterRoom();
        break;
    case 1:
        SetInputState(INPUT_WAIT_LEAVE);
        SetEventState(EVENT_NONE);
        mTtsPlayer.Stop();
        mNextSceneId = 0x5F;
        gSceneMemory.mCharacter = mCharacters[0].mIndex;
        break;
    }
}

BOOL RoomScene::IsTouchEnabled() {
    switch (mInputState) {
    case INPUT_OFF:
        return FALSE;
    }
    switch (mEventState) {
    case EVENT_CHANGE_ROOM:
        return FALSE;
    }
    return TRUE;
}

BOOL RoomScene::CanEnterRoom() {
    if (!IsTouchEnabled()) {
        return FALSE;
    }
    switch (mEventState) {
    case EVENT_RESIDENT_ARRIVES:
    case EVENT_VISITOR_ARRIVES:
    case EVENT_LAST_LEAVES:
    case EVENT_VISITOR_LEAVES:
        return FALSE;
    }
    return TRUE;
}

// Leaves for the scene of the resident's room (scene 0x40), if there is somebody in.
void RoomScene::EnterRoom() {
    if (CountPresent() == 0) {
        return;
    }
    if (!CanEnterRoom()) {
        return;
    }
    if (mCharacters[0].mEvent == 18 || mCharacters[0].mEvent == 15 || mCharacters[0].mEvent == 17) {
        SetInputState(INPUT_WAIT_LEAVE);
    } else {
        SetInputState(INPUT_ENTER_ROOM);
        gSoundManager.StartSeqArc(2, 9, 0);
    }
    SetEventState(EVENT_NONE);
    mTtsPlayer.Stop();
    mNextSceneId = 0x40;
}

// Leaves for the information scene (scene 0xAE).
void RoomScene::GoToInformation() {
    SetInputState(INPUT_WAIT_LEAVE);
    SetEventState(EVENT_NONE);
    mTtsPlayer.Stop();
    mNextSceneId = 0xAE;
    gSceneMemory.mCharacter = mCharacters[0].mIndex;
}

// Enables the two buttons as the state allows.
void RoomScene::UpdateButtons() {
    if (!IsTouchEnabled()) {
        mButtons[1].SetReleased(FALSE);
    } else {
        mButtons[1].SetReleased(TRUE);
    }
    if (!CanEnterRoom() || CountPresent() == 0) {
        if (mButtons[0].SetReleased(FALSE)) {
            mButtons[0].mOverwritePriority = 11;
        }
    } else {
        if (mButtons[0].SetReleased(TRUE)) {
            mButtons[0].mOverwritePriority = -1;
        }
    }
}

void RoomScene::SetInputState(int state) {
    mInputState = state;
    mInputStep = 0;
}

// The touch screen: the two buttons, the building panel and its information button, and leaving the scene.
void RoomScene::UpdateInput() {
    if (mInputState != INPUT_WAIT_LEAVE && mInputState != INPUT_LEAVING) {
        UpdateButtons();
    }

    switch (mInputState) {
    case INPUT_OFF:
        break;

    case INPUT_IDLE:
        if (gTouchInput.mTouched) {
            ScreenPoint point = gTouchInput.mPoint;
            BOOL handled = FALSE;

            for (int i = 0; i < 2; i++) {
                if (mButtons[i].Contains(point)) {
                    handled = TRUE;
                    if (mButtons[i].mReleased) {
                        SetInputState(INPUT_TOUCH_BUTTON);
                        mPressedButton = i;
                        mButtons[i].mState = 1;
                        gSoundManager.StartSeqArc(2, 1, 0);
                    }
                    break;
                }
            }
            if (!handled) {
                if (mFront.mCursor.Contains(point)) {
                    if (mButtons[0].mReleased) {
                        EnterRoom();
                        handled = TRUE;
                    }
                } else if (mFront.mButton.Contains(point) && mButtons[1].mReleased) {
                    SetInputState(INPUT_INFO_BUTTON);
                    gSoundManager.StartSeqArc(2, 1, 0);
                    handled = TRUE;
                }
            }
            if (!handled) {
                SetInputState(INPUT_PANEL);
            }
        } else {
            if (gTouchInput.mTouching != 0) {
                return;
            }
            // After a minute of nothing, the panel goes to a random resident's room
            mIdleFrames++;
            if (mIdleFrames > 3600 && mResidentCount >= 2) {
                int count = mResidentCount;
                int room = func_0202c414(&gGameData, mResidents[MATH_Rand16(&gRandom, count)]);
                if ((u16)room != mFront.mSelectedRoom) {
                    mIdleFrames = 0;
                    mCamera.SetState(4, 0x474);
                    mFront.SelectRoom(room, -1);
                }
            }
            ChangeRoom();
        }
        break;

    case INPUT_TOUCH_BUTTON:
        if (gTouchInput.mTouching != 0) {
            if (mButtons[mPressedButton].Contains(gTouchInput.mPoint)) {
                mButtons[mPressedButton].mState = 1;
            } else {
                mButtons[mPressedButton].mState = 2;
            }
        } else {
            if (mButtons[mPressedButton].Contains(gTouchInput.mLastPoint) &&
                data_0211f9d4.mUnknown20) {
                mButtons[mPressedButton].mState = 3;
                PressButton(mPressedButton);
            } else {
                SetInputState(INPUT_IDLE);
                mButtons[mPressedButton].mState = 2;
            }
        }
        break;

    case INPUT_INFO_BUTTON:
        if (gTouchInput.mTouching != 0) {
            if (mFront.mButton.Contains(gTouchInput.mPoint)) {
                mFront.mButton.SetState(1);
            } else {
                mFront.mButton.SetState(2);
            }
        } else {
            if (mFront.mButton.Contains(gTouchInput.mLastPoint) && data_0211f9d4.mUnknown20) {
                mFront.mButton.SetState(3);
                GoToInformation();
            } else {
                SetInputState(INPUT_IDLE);
                mFront.mButton.SetState(2);
            }
        }
        break;

    case INPUT_PANEL:
        if (gTouchInput.mTouching != 0) {
            mFront.Touch(gTouchInput.mPoint);
            ChangeRoom();
        } else {
            SetInputState(INPUT_IDLE);
        }
        break;

    case INPUT_ENTER_ROOM:
        if (mInputStep == 0) {
            mInputStep++;
            mCharacters[0].SetState(0x113);
            if (mCharacters[1].mPresent) {
                mCharacters[1].SetState(0x113);
            }
            mCamera.SetState(0, 0);
            mCamera.mIgnoreInput = TRUE;
            mLight.SetState(2);
            mIcon[0].SetMode(1);
        } else if (mInputStep == 1) {
            if (mCharacters[0].mState != 0) {
                return;
            }
            mInputStep++;
            mCharacters[0].SetState(0x114);
        } else if (mInputStep == 2) {
            if (mCharacters[0].mState != 0) {
                return;
            }
            mInputStep++;
            if (CountPresent() >= 2) {
                mCharacters[0].SetState(0x116);
            } else {
                mCharacters[0].SetState(0x115);
            }
            // The resident walks half of the way to where the camera is
            VecFx32 target = mCamera.mPosition;
            VEC_Subtract(&target, &mCharacters[0].mModel.mPos, &target);
            target.x >>= 1;
            target.y >>= 1;
            target.z >>= 1;
            VEC_Set(&mCharacters[0].mTarget, target.x, 0, target.z);
        } else {
            if (mCharacters[0].mState != 0) {
                return;
            }
            SetInputState(INPUT_WAIT_LEAVE);
        }
        break;

    case INPUT_WAIT_LEAVE:
        if (!ArchiveManager_IsIdle(&gArchiveManager)) {
            return;
        }
        if (!IsTransitionDone()) {
            return;
        }
        SetInputState(INPUT_LEAVING);
        func_0203c02c(&gSceneManager, mNextSceneId);
        SetFadeState(2);
        if (mNextSceneId == 0x5F) {
            gSoundManager.StartSeqArc(2, 1, 13);
        } else if (mNextSceneId == 0xAE) {
            gSoundManager.StartSeqArc(2, 1, 10);
        } else {
            gSoundManager.StartSeqArc(2, 1, 11);
        }
        break;

    case INPUT_LEAVING:
        break;
    }
}

// Shows the room that was chosen on the building panel, if it is another: the characters are saved, the new
// room's are chosen and loaded, and everything is told to disappear and come back (EVENT_CHANGE_ROOM).
void RoomScene::ChangeRoom() {
    int i;
    u8 resident;

    if (mFront.mSelectedRoom == mRoom.mRoomIndex) {
        return;
    }
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    resident = mFront.mResidents[mFront.mSelectedRoom & 0xFF];
    mTtsPlayer.Stop();
    FinishEncounter();
    SaveCharacters();

    for (i = 0; i < 2; i++) {
        if (func_0202c3dc(&gGameData, mCharacters[i].mIndex)) {
            mFront.DrawWindow(mCharacters[i].mRoomNumber);
            if (func_02099d1c(&mCharacters[i])) {
                mFront.DrawWindow(func_0202c414(&gGameData, mCharacters[i].mEventPartner));
            }
        }
    }

    mRoom.mRoomIndex = mFront.mSelectedRoom;
    if (resident == 0xFF) {
        ChooseCharacters(0);
        LoadCharacters();
        mRoom.SetNextStyle(8);
        mNamePanel.SetState(2);
    } else {
        ChooseCharacters(resident);
        gSceneMemory.mCharacter = resident;
        LoadCharacters();
        mRoom.SetNextStyle(mCharacters[0].mInterior);
        mNamePanel.SetState(1);
    }

    mCharacters[0].mPresent = FALSE;
    mCharacters[1].mPresent = FALSE;
    mRoom.SetState(1);
    mCharacters[0].SetOverrideState(1);
    mCharacters[1].SetOverrideState(1);
    for (int i = 0; i < 4; i++) {
        mItems[i].SetLoadState(1);
    }
    SetEventState(EVENT_CHANGE_ROOM, 0);
    mIdleFrames = 0;
    mCamera.SetState(0, 0);
    mMessage.SetState(4);
    data_0211d604.Hide(1);
    mClockDisplay.SetState(1);
    mIcon[0].SetMode(1);
}

void RoomScene::SetEventState(int state) {
    mEventState = state;
    mEventStep = 0;
}

void RoomScene::SetEventState(int state, int timer) {
    SetEventState(state);
    mEventTimer = timer;
}

// Starts the event that is due for the saved character under mScanIndex. A character that is not one of the two
// in the room is loaded into `temp` for it. Returns the character.
SavedCharacter *RoomScene::StartDueEvent(s64 now, SavedCharacter *temp) {
    int events[2];
    SavedCharacter *first;
    SavedCharacter *second;

    SaveCharacters();
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }

    if (mScanIndex == mCharacters[0].mIndex) {
        temp = &mCharacters[0];
        first = &mCharacters[1];
        second = NULL;
    } else if (mScanIndex == mCharacters[1].mIndex) {
        temp = &mCharacters[1];
        first = &mCharacters[0];
        second = NULL;
    } else {
        temp->mIndex = mScanIndex;
        LoadCharacter((RoomCharacter *)temp);
        temp->mPersonality = func_02098f2c(temp);
        first = &mCharacters[0];
        second = &mCharacters[1];
    }
    if (mResidentCount == 1) {
        second = NULL;
        first = second;
    }
    func_02099c28(temp, now, &mClock.mTime, first, second);

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
    return temp;
}

// Ends the event of the saved character under mScanIndex, whose time is up. Returns the index of the character
// it leaves its mood to, or -1.
int RoomScene::EndDueEvent() {
    int events[2];
    SavedCharacter *character;
    int partnerIndex;
    SavedCharacter *partner;

    SaveCharacters();
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }
    SavedCharacter temp;

    if (mScanIndex == mCharacters[0].mIndex) {
        character = &mCharacters[0];
    } else if (mScanIndex == mCharacters[1].mIndex) {
        character = &mCharacters[1];
    } else {
        character = &temp;
        temp.mIndex = mScanIndex;
        LoadCharacter((RoomCharacter *)character);
        temp.mPersonality = func_02098f2c(character);
    }

    partnerIndex = -1;
    partner = NULL;
    if (character->mMoodLocked != 0 && func_0209a444(character) && mResidentCount > 1) {
        partnerIndex = character->mMoodPartner;
        if (partnerIndex == mCharacters[0].mIndex) {
            partner = &mCharacters[0];
        } else if (partnerIndex == mCharacters[1].mIndex) {
            partner = &mCharacters[1];
        }
    }
    func_0209a588(character, mClock.mSeconds, partner);

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
    return partnerIndex;
}

// For the saved character under mScanIndex when no event of its is due (func_020ad348 says when).
void RoomScene::UpdateAbsentCharacter(s64 now) {
    int events[2];
    SavedCharacter *character;

    SaveCharacters();
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }
    SavedCharacter temp;

    if (mScanIndex == mCharacters[0].mIndex) {
        character = &mCharacters[0];
    } else if (mScanIndex == mCharacters[1].mIndex) {
        character = &mCharacters[1];
    } else {
        character = &temp;
        temp.mIndex = mScanIndex;
        LoadCharacter((RoomCharacter *)character);
        temp.mPersonality = func_02098f2c(character);
    }
    func_0209b14c(character, now);

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
}

// Once in each round of the saved characters: gives a random one of those that can have it an event. Returns its
// index, or -1.
int RoomScene::StartRandomEvent(s64 now) {
    int events[2];
    int force;
    int result;

    SaveCharacters();
    result = -1;
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }

    if (func_020adb44(&force) > 0) {
        u16 indices[100];
        int count = func_020adc08(indices);
        u16 index = indices[MATH_Rand16(&gRandom, count)];
        SavedCharacter *character;
        SavedCharacter *partner = NULL;
        SavedCharacter temp;
        SavedCharacter partnerTemp;

        if (index == mCharacters[0].mIndex) {
            character = &mCharacters[0];
        } else if (index == mCharacters[1].mIndex) {
            character = &mCharacters[1];
        } else {
            temp.mIndex = index;
            LoadCharacter((RoomCharacter *)&temp);
            temp.mPersonality = func_02098f2c(&temp);
            character = &temp;
        }

        if (func_02099cdc(character)) {
            u8 partnerIndex = character->mEventPartner;
            if (partnerIndex == mCharacters[0].mIndex) {
                partner = &mCharacters[0];
            } else if (partnerIndex == mCharacters[1].mIndex) {
                partner = &mCharacters[1];
            } else {
                partnerTemp.mIndex = partnerIndex;
                LoadCharacter((RoomCharacter *)&partnerTemp);
                partnerTemp.mPersonality = func_02098f2c(&partnerTemp);
                partner = &partnerTemp;
            }
        }

        if (func_0209b208(character, now, force, partner, 0)) {
            SaveCharacter((RoomCharacter *)character);
            if (partner != NULL) {
                SaveCharacter((RoomCharacter *)partner);
            }
            result = character->mIndex;
        }
    }

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
    return result;
}

// Whether the room, the characters, the window view and the held things have all finished appearing or
// disappearing.
BOOL RoomScene::IsTransitionDone() {
    if (mRoom.mState != 0 || mCharacters[0].mOverrideState != 0 || mCharacters[1].mOverrideState != 0) {
        return FALSE;
    }
    if (mSky.mState == 2) {
        return FALSE;
    }
    for (int i = 0; i < 4; i++) {
        if (mItems[i].mLoadState != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// Starts a transition on the room, then on each character, then on the held things, each when the one before is
// done (mEventTimer counts the steps), and returns whether all are done.
BOOL RoomScene::StepTransition(int transition) {
    if (mEventTimer == 0) {
        if (IsTransitionDone()) {
            mEventTimer++;
            mRoom.SetState(transition);
        }
    } else if (mEventTimer == 1) {
        if (IsTransitionDone()) {
            mEventTimer++;
            mCharacters[0].SetOverrideState(transition);
        }
    } else if (mEventTimer == 2) {
        if (IsTransitionDone()) {
            mEventTimer++;
            mCharacters[1].SetOverrideState(transition);
        }
    } else if (mEventTimer == 3) {
        if (IsTransitionDone()) {
            mEventTimer++;
            for (int i = 0; i < 4; i++) {
                mItems[i].SetLoadState(transition);
            }
        }
    } else if (mEventTimer == 4) {
        if (IsTransitionDone()) {
            return TRUE;
        }
    }
    return FALSE;
}

// The numbers of the messages for what comes of an encounter, by what RoomCharacter::GetFlagOfPartner says of the
// resident: when it starts (0x0216d570) and when it ends (0x0216d59c).
extern "C" {
extern const int gEncounterMessages[];
extern const int gEncounterEndMessages[];
}

// What goes on in the room, each frame: the switch on mEventState. The states of a room with nobody, one or two
// characters in it (3 to 6) also look at one saved character each frame (mScanIndex), to start and end the events
// of all of them as the clock says, and at the end of each round give one of them a random event.
// @0x0214c4e8
void RoomScene::UpdateEvent() {
    mEventFrames++;

    switch (mEventState) {
    case EVENT_NONE:
        break;

    case EVENT_CHANGE_ROOM: {
        u8 resident = mFront.mResidents[mRoom.mRoomIndex & 0xFF];

        if (mEventStep == 0) {
            if (StepTransition(2)) {
                mEventStep++;
                mEventTimer = 0;
            }
        } else if (mEventStep == 1) {
            if (StepTransition(3)) {
                mEventStep++;
            }
        } else if (mEventStep == 2) {
            mEventStep++;
            func_02091a14(&mCharacters[0], 1);
            func_02091a14(&mCharacters[1], 1);
            func_020a4b74(&mRoom.mParts, 1);
            if (resident != 0xFF) {
                mCharacters[0].SetState(1);
                mCharacters[0].mWait = 0;
                mCharacters[1].SetState(1);
                mCharacters[1].mWait = 0;
                PlaceCharacters();
                StartCharacterEvents();
            }
            AimCamera();
        } else if (mEventStep == 3) {
            mEventStep++;
            mEventTimer = 4;
            func_020a4ab8(&mRoom.mParts, 3);
            mCamera.SetState(1, 0);
        } else if (mEventStep == 4) {
            if (!IsTransitionDone()) {
                break;
            }
            if (--mEventTimer > 0) {
                break;
            }
            if (resident == 0xFF) {
                SetEventState(EVENT_EMPTY_ROOM, -1);
            } else {
                StartRoomEvent();
                ShowEventMessage();
            }
            data_0211d604.Show(1);
        }
        break;
    }

    case EVENT_CHOOSE:
        ChooseEncounter();
        break;

    case EVENT_EMPTY_ROOM: {
        s64 now = mClock.mSeconds;

        if (mScanIndex >= 100) {
            mScanIndex = 0;
            int index = StartRandomEvent(now);
            if (index >= 0) {
                mFront.DrawWindow(func_0202c414(&gGameData, index));
            }
        } else {
            if (func_0202c3dc(&gGameData, mScanIndex)) {
                if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                    SavedCharacter temp;
                    StartDueEvent(now, &temp);
                    if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                        mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                    }
                } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                    int partner = EndDueEvent();
                    if (partner >= 0) {
                        mFront.DrawWindow(func_0202c414(&gGameData, partner));
                    }
                } else if (func_020ad348(mScanIndex, now)) {
                    UpdateAbsentCharacter(now);
                }
                mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
            }
            mScanIndex++;
        }
        break;
    }

    case EVENT_NOBODY_HOME:
        if (mEventStep == 0) {
            mEventStep++;
            mEventTimer = 60;
        } else {
            s64 now = mClock.mSeconds;

            if (mScanIndex >= 100) {
                mScanIndex = 0;
                int index = StartRandomEvent(now);
                if (index >= 0) {
                    mFront.DrawWindow(func_0202c414(&gGameData, index));
                }
            } else {
                if (func_0202c3dc(&gGameData, mScanIndex)) {
                    if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                        SavedCharacter temp;
                        StartDueEvent(now, &temp);
                        // The resident's new event has it at home
                        if (mScanIndex == mCharacters[0].mIndex && !func_02099c84(&mCharacters[0])) {
                            SetEventState(EVENT_CHOOSE);
                        }
                        if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                            mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                        }
                    } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                        int partner = EndDueEvent();
                        if (partner >= 0) {
                            mFront.DrawWindow(func_0202c414(&gGameData, partner));
                        }
                    } else if (func_020ad348(mScanIndex, now)) {
                        UpdateAbsentCharacter(now);
                    }
                    mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
                }
                mScanIndex++;
            }
            if (--mEventTimer <= 0) {
                SetEventState(EVENT_CHOOSE);
            }
        }
        break;

    case EVENT_ALONE:
        if (mEventStep == 0) {
            mEventStep++;
            if (mEventTimer < 0) {
                mEventTimer = 600;
            } else if (mEventTimer == 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else if (mEventStep == 1) {
            BOOL refresh;
            s64 now = mClock.mSeconds;

            if (mScanIndex >= 100) {
                mScanIndex = 0;
                int index = StartRandomEvent(now);
                if (index >= 0) {
                    mFront.DrawWindow(func_0202c414(&gGameData, index));
                }
            } else {
                if (func_0202c3dc(&gGameData, mScanIndex)) {
                    refresh = TRUE;

                    if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                        SavedCharacter temp;
                        SavedCharacter *character = StartDueEvent(now, &temp);

                        if (character->mIndex == mCharacters[0].mIndex) {
                            mEventStep++;
                            if (func_02099c84(&mCharacters[0])) {
                                refresh = FALSE;
                                if (mCharacters[0].mPrevEvent == 18) {
                                    mFront.DrawWindowOfKind(0, mCharacters[0].mRoomNumber);
                                }
                            } else if (func_02099da8(&mCharacters[0])) {
                                mCharacters[0].mWait = 0;
                                mCharacters[0].SetState(0x117);
                            }
                        } else if (func_02099d1c(character) && character->mEventPartner == mCharacters[0].mIndex) {
                            mEventStep++;
                            mCharacters[0].mWait = 0;
                            mCharacters[0].SetState(0x117);
                            refresh = FALSE;
                        } else if (func_02099da8(character) && character->mEventPartner == mCharacters[0].mIndex) {
                            mEventStep++;
                            mCharacters[0].mWait = 0;
                            mCharacters[0].SetState(0x117);
                            refresh = FALSE;
                        }

                        if (mScanIndex != mCharacters[0].mIndex) {
                            if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                                mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                            }
                        } else if (!func_02099c84(&mCharacters[0]) && func_02099da8(&mCharacters[0])) {
                            mFront.DrawWindow(func_0202c414(&gGameData, mCharacters[0].mEventPartner));
                        }
                    } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                        int partner = EndDueEvent();
                        if (partner >= 0) {
                            mFront.DrawWindow(func_0202c414(&gGameData, partner));
                        }
                    } else if (func_020ad348(mScanIndex, now)) {
                        UpdateAbsentCharacter(now);
                    }
                    if (refresh) {
                        mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
                    }
                }
                mScanIndex++;
            }

            if (mEventStep == 2) {
                break;
            }
            if (func_02099e64(&mCharacters[0])) {
                ChooseReactionsQuiet();
            }
            if (--mEventTimer <= 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else {
            SetEventState(EVENT_CHOOSE);
            StartEvent(&mCharacters[0], 0);
        }
        break;

    case EVENT_TOGETHER:
        if (mEventStep == 0) {
            mEventStep++;
            if (mEventTimer < 0) {
                mEventTimer = 600;
            } else if (mEventTimer == 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else if (mEventStep == 1) {
            BOOL refresh;
            s64 now = mClock.mSeconds;

            if (mScanIndex >= 100) {
                mScanIndex = 0;
                int index = StartRandomEvent(now);
                if (index >= 0) {
                    mFront.DrawWindow(func_0202c414(&gGameData, index));
                }
            } else {
                if (func_0202c3dc(&gGameData, mScanIndex)) {
                    refresh = TRUE;

                    if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                        SavedCharacter temp;
                        StartDueEvent(now, &temp);

                        BOOL withPartner = func_02099d1c(&mCharacters[1]);
                        if (!withPartner || (withPartner && mCharacters[1].mEventPartner != mCharacters[0].mIndex)) {
                            mEventStep++;
                            mCharacters[0].SetState(0x117);
                            mCharacters[1].SetState(0x117);
                            refresh = FALSE;
                            mCharacters[0].mWait = 0;
                            mCharacters[1].mWait = 0;
                        } else if (withPartner && mCharacters[1].mEventPartner == mCharacters[0].mIndex) {
                            if (mScanIndex == mCharacters[0].mIndex || mScanIndex == mCharacters[1].mIndex) {
                                mEventStep++;
                            }
                        }

                        if (mScanIndex != mCharacters[0].mIndex && mScanIndex != mCharacters[1].mIndex) {
                            if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                                mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                            }
                        }
                    } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                        int partner = EndDueEvent();
                        if (partner >= 0) {
                            mFront.DrawWindow(func_0202c414(&gGameData, partner));
                        }
                    } else if (func_020ad348(mScanIndex, now)) {
                        UpdateAbsentCharacter(now);
                    }
                    if (refresh) {
                        mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
                    }
                }
                mScanIndex++;
            }

            if (mEventStep == 2) {
                break;
            }
            if (func_02099e64(&mCharacters[0])) {
                UpdateMeeting();
                ChooseReactions();
            }
            if (--mEventTimer <= 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else {
            SetEventState(EVENT_CHOOSE);
            if (func_02099da8(&mCharacters[0])) {
                StartPairEvent(0);
            }
        }
        break;

    case EVENT_APPROACH:
        if (AreCharactersNear(0, 1, 0x2333, 0x2666)) {
            SetEventState(EVENT_MEET);
            mCharacters[0].SetState(0xF3);
            mCharacters[0].SetTargetFloorPos(mCharacters[1].GetFloorPos());
            mCharacters[1].SetState(0xF3);
            mCharacters[1].SetTargetFloorPos(mCharacters[0].GetFloorPos());
        }
        break;

    case EVENT_MEET:
        if (mEventStep == 0) {
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            int index = MATH_Rand16(&gRandom, 0) & 1;
            mEventStep++;
            RoomCharacter *partner = mCharacters[index].mPartner;
            func_02099088(&mCharacters[index], partner->mIndex,
                          data_021135c8[mCharacters[index].mRelation[partner->mIndex]][1] * 2);
            StartTalkReaction(index, 1);
            mCharacters[index].mReaction = 1;
            partner->mReaction = 1;
            mCharacters[index].mWait = 0x8E;
            partner->mWait = 0x8C;
            mEncounterPending = TRUE;
            mCharacters[index].SetHeadIconMode(1);
            partner->SetHeadIconMode(1);
        } else if (mEventStep == 1) {
            UpdateEncounter();
            if (mCharacters[0].mState != 0xF6 || mCharacters[1].mState != 0xF6) {
                break;
            }
            mEventStep++;
            mEncounterPending = FALSE;
            mCharacters[0].mReaction = 0;
            mCharacters[1].mReaction = 0;
            mCharacters[0].ResetBody();
            mCharacters[1].ResetBody();
            SaveCharacters();
        } else {
            if (JudgeEncounterEnd(mActor, mActorOther) != 10) {
                SetEventState(EVENT_ENCOUNTER_END);
                mCharacters[0].mWait = 20;
                mCharacters[1].mWait = 20;
            } else if (func_02099e64(&mCharacters[0])) {
                mCharacters[0].SetState(0x103);
                mCharacters[1].SetState(0x103);
                SetEventState(EVENT_CHOOSE_WALKER);
            } else {
                StartPairEvent(0);
                SetEventState(EVENT_TOGETHER, -1);
            }
        }
        break;

    case EVENT_CHOOSE_WALKER:
        if (mEventStep == 0) {
            FloorPos target;
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            mEventStep++;
            if (mCharacters[0].GetFloorPos().LengthSq() <= mCharacters[1].GetFloorPos().LengthSq()) {
                mActor = &mCharacters[0];
                mActorOther = &mCharacters[1];
            } else {
                mActor = &mCharacters[1];
                mActorOther = &mCharacters[0];
            }
            mActor->SetState(0xF5);
            mActor->GetRandomPos(&target);
            mActor->SetTargetFloorPos(target);
            mActor->mWait = MATH_Rand16(&gRandom, 10) * 2 + 60 + ((200 - func_02098ec4(mActor)) >> 2);
            mActorOther->SetState(4);
            mActorOther->mWait = 30;
        } else {
            if (mActor->mState == 0 || (mCharacters[0].mWait <= 0 && mCharacters[1].mWait <= 0)) {
                SetEventState(EVENT_TOGETHER, 240);
                mActor->SetState(1);
                mActor->mWait = MATH_Rand16(&gRandom, 10) + 10;
                mActorOther->SetState(1);
                mActorOther->mWait = MATH_Rand16(&gRandom, 15) + 30;
            } else if (AreCharactersNear(0, 1, 0x2333, 0x2666)) {
                func_0209171c(mActor, mActorOther->GetFloorPos());
            }
        }
        break;

    case EVENT_ENCOUNTER:
        if (mEventStep == 0) {
            if (mCharacters[0].mWait > 0 || mCharacters[1].mWait > 0) {
                break;
            }
            mEventStep++;
            mEventTimer = 40;
            ApplyEncounterResult(&mCharacters[0], &mCharacters[1],
                                 mEncounterJudge.Judge(&mCharacters[0], &mCharacters[1]), 1);
            mCharacters[0].SetState(0x104);
            mCharacters[1].SetState(0x104);
            mIcon[0].mIconId = 4;
            // The mark goes half way between the two
            VecFx32 middle = CharacterModel_GetJointPosition(&mCharacters[0].mModel, 3);
            VecFx32 other = CharacterModel_GetJointPosition(&mCharacters[1].mModel, 3);
            VEC_Add(&middle, &other, &middle);
            middle.x >>= 1;
            middle.y >>= 1;
            middle.z >>= 1;
            mIcon[0].SetPos(middle.x, middle.y, middle.z);
            mMessage.SetState(4);
        } else if (mEventStep == 1) {
            if (--mEventTimer > 0) {
                break;
            }
            mEventStep++;
            mIcon[0].Show();
            mIcon[0].Reload();
            mIcon[0].SetMode(2);
            int kind = mCharacters[0].GetFlagOfPartner();
            ShowMessage(&mCharacters[0], &mCharacters[1], gEncounterMessages[kind], 2);
            if (kind == 4) {
                gSoundManager.StartSeqArc(1, 0x26, 2);
            } else {
                gSoundManager.StartSeqArc(1, 0x26, 0);
            }
        } else {
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            SetEventState(EVENT_MEET);
            mIcon[0].SetMode(1);
        }
        break;

    case EVENT_ENCOUNTER_END:
        if (mEventStep == 0) {
            if (mCharacters[0].mWait > 0 || mCharacters[1].mWait > 0) {
                break;
            }
            mEventStep++;
            mEventTimer = 30;
            ApplyEncounterResult(&mCharacters[0], &mCharacters[1], JudgeEncounterEnd(mActor, mActorOther), 2);
            mCharacters[0].SetState(0x105);
            mCharacters[1].SetState(0x105);
            mMessage.SetState(4);
        } else if (mEventStep == 1) {
            if (--mEventTimer > 0) {
                break;
            }
            mEventStep++;
            ShowMessage(&mCharacters[0], &mCharacters[1],
                        gEncounterEndMessages[mCharacters[0].GetFlagOfPartner()], 2);
            gSoundManager.StartSeqArc(1, 0x26, 1);
        } else {
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            SetEventState(EVENT_TOGETHER, -1);
        }
        break;

    case EVENT_LAST_LEAVES:
        if (mEventStep == 0) {
            mEventStep++;
            mLight.SetState(2);
        } else {
            if (mActor->mPresent) {
                break;
            }
            SetEventState(EVENT_NOBODY_HOME);
            mCamera.SetMode(1);
            mLight.SetState(4);
            mFront.DrawWindow(func_0202c414(&gGameData, mActor->mIndex));
            if (func_02099d1c(mActor)) {
                mFront.DrawWindow(func_0202c414(&gGameData, mActor->mEventPartner));
            }
            SaveCharacter(mActor);
        }
        break;

    case EVENT_VISITOR_LEAVES:
        if (mEventStep == 0) {
            if (mActorOther->mState != 0) {
                break;
            }
            mEventStep++;
            mActor->SetState(0x107);
            mActorOther->SetState(0x109);
        } else {
            if (CountPresent() != 1) {
                break;
            }
            if (func_02099d1c(mActorOther)) {
                SetEventState(EVENT_CHOOSE, 60);
            } else {
                SetEventState(EVENT_ALONE, 60);
                StartEvent(mActorOther, 0);
            }
            mCamera.SetMode(2);
            mFront.DrawWindow(func_0202c414(&gGameData, mActor->mIndex));
            if (func_02099d1c(mActor)) {
                mFront.DrawWindow(func_0202c414(&gGameData, mActor->mEventPartner));
            }
            SaveCharacter(mActor);
        }
        break;

    case EVENT_RESIDENT_ARRIVES:
        if (mEventStep == 0) {
            mEventStep++;
            mCharacters[0].mPresent = TRUE;
            mCharacters[0].SetState(0x10C);
            mCamera.SetMode(2);
            mCamera.SetState(4, 0x474);
            mLight.SetState(2);
            if (mCharacters[0].mEvent == 18) {
                mFront.DrawWindowOfKind(0, mCharacters[0].mRoomNumber);
            } else {
                mFront.DrawWindow(mCharacters[0].mRoomNumber);
            }
            ShowMessage(&mCharacters[0], NULL, 9, 2);
        } else {
            if (mCharacters[0].mState != 0) {
                break;
            }
            SetEventState(EVENT_ALONE, 60);
            if (func_02099e64(&mCharacters[0])) {
                mCharacters[0].SetState(1);
            } else {
                StartEvent(&mCharacters[0], 0);
            }
            mCharacters[0].mWait = MATH_Rand16(&gRandom, 20) + 30;
            mCharacters[0].mReaction = 0;
        }
        break;

    case EVENT_VISITOR_ARRIVES:
        if (mEventStep == 0) {
            mEventStep++;
            mLight.SetState(2);
        } else if (mEventStep == 1) {
            if (mActorOther->mState != 0) {
                break;
            }
            if (!ArchiveManager_IsIdle(&gArchiveManager)) {
                break;
            }
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mActor->mIndex = GetCharacterCountForDisplay();
            LoadCharacter(mActor);
            mActor->SetOverrideState(1);
        } else if (mEventStep == 2) {
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mActor->SetOverrideState(2);
        } else if (mEventStep == 3) {
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mActor->SetOverrideState(3);
        } else if (mEventStep == 4) {
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mEventTimer = 0;
            mFront.DrawWindow(mActor->mRoomNumber);
            mMessage.SetState(4);
            gSoundManager.StartSeqArc(1, 0x1D, 0x10);
        } else if (mEventStep == 5) {
            if (++mEventTimer <= 40) {
                break;
            }
            mEventStep++;
            mEventTimer = 0;
            func_02091a14(mActor, 1);
            mActor->mPresent = TRUE;
            mActor->SetState(0x10D);
            mActorOther->SetState(0x110);
            mCamera.SetMode(3);
        } else if (mEventStep == 6) {
            if (++mEventTimer <= 60) {
                break;
            }
            mEventStep++;
            mEventTimer = 0;
            mActor->SetState(0x10E);
            ShowMessage(mActor, NULL, 9, 2);
        } else if (mEventStep == 7) {
            if (mActor->mState != 0) {
                break;
            }
            mEventStep++;
            mActor->SetState(1);
            mActor->mWait = MATH_Rand16(&gRandom, 20) + 30;
        } else {
            int nearer;
            int farther;
            if (mCharacters[0].GetFloorPos().LengthSq() <= mCharacters[1].GetFloorPos().LengthSq()) {
                nearer = 0;
                farther = 1;
            } else {
                nearer = 1;
                farther = 0;
            }
            StartEncounter(nearer, farther);
        }
        break;

    case EVENT_AFTER_DISTRIBUTION:
        if (mEventStep == 0) {
            if (--mEventTimer > 0) {
                break;
            }
            mEventStep++;
            mEventTimer = 60;
            func_02099324(&mCharacters[0], mClock.mSeconds);
            func_0209b1d8(&mCharacters[0]);
            SaveCharacter(&mCharacters[0]);
            mFront.DrawWindow(mCharacters[0].mRoomNumber);
            mLight.SetState(2);
        } else {
            if (--mEventTimer > 0) {
                break;
            }
            SetInputState(INPUT_IDLE);
            SetEventState(EVENT_CHOOSE);
        }
        break;
    }
}
