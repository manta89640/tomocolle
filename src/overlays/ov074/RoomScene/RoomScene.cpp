#include "prelude.h"

// The archive of each kind of thing a character can hold, by number (data_ov074_0216d754, used by group a7)

// Counts the saved characters, chooses the two to show, and asks for the scene's archives.
void RoomScene::StartLoading() {
    u8 last;
    u8 building;
    u8 first;
    u8 second;

    func_ov074_0216ccac(&mClock);
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
            mPanel.mRoomResidents[func_0202c414(&gGameData, i)] = i;
        }
    }

    last = data_0213ba74.mCharacter;
    if (last < 100 && func_0202c3dc(&gGameData, last)) {
        ChooseCharacters(last);
    } else {
        ChooseCharacters(0);
        data_0213ba74.mCharacter = mCharacters[0].mIndex;
    }
    LoadCharacters();
    func_02091048(&mCharacters[0], "3HM");
    func_02091048(&mCharacters[1], "BI1");
    func_ov074_0216a2dc(&mRoom, mCharacters[0].mInterior);
    mRoom.mRoomNumber = mCharacters[0].mRoom;

    if (mSceneId == 0x11) {
        building = gGameData.mBuilding & 3;
    } else {
        building = mSceneId - 0x12;
    }
    func_ov074_0216bc08(&mPanel, building);
    func_ov074_0216c3cc(&mWindowView, &mClock, building);

    ArchiveManager_MountCompressed(&gArchiveManager, "MNU", "Scn/Scene/ScenePlayRoom2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMM", "Msg/TmdMainRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASP", "Scn/AFwBG/FwBgSkyPlt_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "APX", mPanel.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mWindowView.mArchivePath, 0);
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);

    GetEventItems(&first, &second);
    func_ov074_02169e4c(&mItems[0], "HI0", first);
    func_ov074_02169e4c(&mItems[1], "HI1", second);
    func_ov074_02169e4c(&mItems[2], "HI2", first);
    func_ov074_02169e4c(&mItems[3], "HI3", second);
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
//
// Not matching yet: 0x92c bytes against 0x930, everything else identical. Three places differ:
// 1. The loop over the two characters: the original has the counter in r7, &mCharacters[i] in r8 and the pointer
//    for the indexed fields in r9; this gives r9, r7, r8 (same instructions, registers rotated).
// 2. `mCamera.mRoom = &mRoom` computes the address into r2 in the original, r0 here.
// 3. The test of the resident's event end against 0x7FFFFFFFFFFFFFFF is one 64-bit comparison in the original
//    (mvn r1; ldr hi; ldr lo; sub r0,r1,#0x80000000; cmp hi; cmpeq lo, behind a bne on the scene id); two u32
//    fields give a chain of conditional instructions instead. It needs the field as an s64 (at 0x21C, so the
//    saved character's layout has to allow a 64-bit member at an address that is not a multiple of 8).
// Tried: a pointer variable for the character (wrong: the original indexes), nested call against a variable for
// the particle file (the variable is right).
void RoomScene::Init() {
    int i;
    void *file;
    void *animations;
    int charOffset;

    func_0202b0e8(&data_02122824);
    func_0202ae38(&data_021223e4);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(1, 1, 1), 0, 0x7FFF, 63, FALSE);
    func_0202680c(0x4000);

    ObjResourceManager_Load(&gObjResourceManager, "MNU:/Obj_down.nce", "MNU:/Obj_down.ncg", 0x1000, 0, -1);
    func_02027534(&data_021213b8, "MNU:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "MNU:/Obj_up.nce", "MNU:/Obj_up.ncg", 0x10, 1, -1);
    func_02027534(&data_021213b8, "MNU:/Obj_up.ncl", 0x10, -1, -1, -1);
    mPanel.mWindowScreen = func_02027d7c(&data_02121834, "APX:/BG_window.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "APX:/BG_wall.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "APX:/BG_down.ncg", 0x600, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "APX:/BG_down.ncl", 0x600, -1, -1, -1);
    func_02027d7c(&data_02121834, "MNU:/BG_dialogue.nsc", 2, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "MNU:/BG_sub.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MNU:/BG_sub.ncl", 2, -1, -1, -1);
    mWindowView.mSkyPalette = func_02027534(&data_021213b8, "ASP:/Fw_BG_sky.ncl", 0x2F00, 0, 0, 2);
    func_02028f24(&gVramTransferQueue, mWindowView.mCharName, 0x400, 0, 0x1000, 0x3000, 0);
    mWindowView.mPalette = func_02027534(&data_021213b8, mWindowView.mPaletteName, 0x400, 0x80, 0x80, 0xC0);
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
    func_020810b4(&mIcon[0], &mParticles, 0);
    func_ov074_0216a2c4(&mRoom);
    func_020a5514(&mRoom, 7);
    func_020a5760(&mRoom, 8);
    func_020a4acc(mRoom.mDoor, 10);
    mRoom.mUnknownSetTo2ByInit = 2;
    for (i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        func_0202ad04(&data_021223e4, i + 2, item->mArchiveName, 0);
        func_ov074_02169e84(item, i + 2);
    }
    mTtsPlayer.InitEngine();

    func_0202ad04(&data_021223e4, 0, mCharacters[0].mArchiveName, 0);
    func_0202ad04(&data_021223e4, 1, mCharacters[1].mArchiveName, 0);
    func_ov074_02152268(&mCharacters[0], 0, 0, 0);
    func_ov074_02152268(&mCharacters[1], 1, 0, 1);
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
    animations = func_0202ad04(&data_021223e4, 6, "BIC", 0);
    for (i = 0; i < 2; i++) {
        LoadCharacter(&mCharacters[i]);
        func_0208fefc(&mCharacters[i]);
        func_02091a24(&mCharacters[i]);
        mCharacters[i].mPersonality = func_02098f2c(&mCharacters[i]);
        func_ov074_021686dc(&mCharacters[i]);
        func_02091a88(&mCharacters[i]);
        func_ov074_0215243c(&mCharacters[i]);
        mCharacters[i].mTtsPlayer = &mTtsPlayer;
        func_ov074_02169c6c(&mCharacters[i]);
        func_02029660(mCharacters[i].mModel.mResource, 'BCA0', animations);
        func_ov074_02152370(&mCharacters[i], 1);
        func_ov074_02152308(&mCharacters[i], &mParticles);
        mCharacters[i].mRoomModel = &mRoom;
        mCharacters[i].mCamera = &mCamera;
    }
    PlaceCharacters();
    StartCharacterEvents();

    func_ov074_0216a514(&mCamera);
    mCamera.mCharacters[0] = &mCharacters[0];
    mCamera.mCharacters[1] = &mCharacters[1];
    mCamera.mRoom = &mRoom;
    func_ov074_0216b55c(&mCameraFollower, &mCamera);
    AimCamera();
    StartRoomEvent();

    mRoomMessages.Open("TMM:/tmd_mainroom_msg_JP.bmg");
    mEtcMessages.Open("TEM:/tmd_etc_msg_JP.bmg");
    charOffset = 0xC0;
    mSpeechText.InitObjForMessage(&mRoomMessages, 3, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    mSpeechText.mSprite.SetPosition(0x10000, 0xA0000);
    mSpeechText.mVAlign = 1;
    charOffset += mSpeechText.GetCharCount();
    if (mPanel.mRoomResidents[mCharacters[0].mRoom] != 0xFF) {
        ShowEventMessage();
    }
    func_ov074_0216c92c(&mNamePlate, mRoom.mUnknown914, mCharacters[0].mNameText);
    func_ov074_0216c618(&mNamePlate, charOffset, &mRoomMessages, &mRoomMessages);

    mButtonLabel[0].InitObjForMessage(&mEtcMessages, 0xB3, &gFonts[0].mFont, 0x1000, 0x300, 0, 1, 0, 0, 15, NULL);
    mButtonLabel[0].mVAlign = 1;
    mButtonLabel[0].mShadowDirections = 0xAA;
    mButtonLabel[0].DrawAll();
    mUnknownClearedByInit = 0;
    mButtons[0].mLabel = &mButtonLabel[0];
    for (s16 j = 0; j < 2; j++) {
        func_ov074_0216ca4c(&mButtons[j], j);
    }
    func_ov074_0216bc18(&mPanel, mCharacters[0].mRoom);
    func_ov074_0216c404(&mWindowView);
    func_ov074_0216cd10(&mClockDisplay, &mClock);

    // Coming from the character distribution scene with a resident that has just moved in
    if (gSceneManager.mPreviousSceneId == 0x74 && mCharacters[0].mEventEndHi == 0x7FFFFFFF &&
        mCharacters[0].mEventEndLo == 0xFFFFFFFF) {
        SetInputState(INPUT_OFF);
        SetEventState(EVENT_AFTER_DISTRIBUTION, 60);
        func_ov074_0216a62c(&mCamera, 2, 0);
        func_ov074_0216d1a8(&mSpeechText, 4);
    }
    UpdateButtons();
    func_ov074_0216d070(&mSpeechText);
    NNS_GfdDumpLnkTexVramManager();
    func_02060c1c();
    gSoundManager.LoadGroup(0x4C);
}

void RoomScene::Update() {
    if ((data_0211f9d4.mHeld & 0x2FFF) != 0 || data_02121e58.mTouching != 0) {
        mIdleFrames = 0;
        func_ov074_0216a62c(&mCamera, 4, 0x474);
    }
    if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
        mCamera.mTurnSpeedX = 0x10000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
        mCamera.mTurnSpeedX = -0x10000;
    }
    if (data_0211f9d4.mHeld & PAD_KEY_UP) {
        mCamera.mTurnSpeedY = 0xE000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
        mCamera.mTurnSpeedY = -0xE000;
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

    func_020a4a50(mRoom.mDoor);
    for (int i = 0; i < 2; i++) {
        func_ov074_021686a4(&mCharacters[i]);
    }
    for (int i = 0; i < 4; i++) {
        mItems[i].Draw();
    }
    func_02080f00(&mParticles, 0);
    mSpeechText.Draw();
    mButtonLabel[0].Draw();
    func_ov074_0216c8f0(&mNamePlate);
    for (int i = 0; i < 2; i++) {
        func_ov074_0216cc2c(&mButtons[i]);
    }
    func_ov074_0216be1c(&mPanel);
    func_ov074_0216cfc0(&mClockDisplay);
}

void RoomScene::Exit() {
    func_02023eac(&data_0211d604, 0x3F);
    FinishEncounter();
    SaveCharacters();
    func_0202683c();
    func_0202aecc(&data_021223e4, 0);
    func_0202aecc(&data_021223e4, 1);
    func_0202aecc(&data_021223e4, 2);
    func_0202aecc(&data_021223e4, 3);
    func_0202aecc(&data_021223e4, 4);
    func_0202aecc(&data_021223e4, 5);
    func_0202aecc(&data_021223e4, 6);
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
    func_ov074_0216ccd4(&mClock);
    func_0207b138(mParticles.mManager);
    for (int i = 0; i < 2; i++) {
        func_ov074_02152500(&mCharacters[i]);
    }
    for (int i = 0; i < 4; i++) {
        func_ov074_02169ebc(&mItems[i]);
    }
    func_020810e4(&mIcon[0]);
    func_ov074_0216a638(&mCamera);
    func_ov074_0216a330(&mRoom);
    func_ov074_0216b5bc(&mCameraFollower);
    func_ov074_0216bd60(&mPanel);
    func_ov074_0216c410(&mWindowView);
    func_ov074_0216d070(&mSpeechText);
    func_ov074_0216c7a4(&mNamePlate);
    for (int i = 0; i < 2; i++) {
        func_ov074_0216cb34(&mButtons[i]);
    }
    func_ov074_0216cdc8(&mClockDisplay);
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
        func_ov074_0216a618(&mCamera, 1);
        func_ov074_0216b8a8(&mCameraFollower, 3);
    } else if (count == 1) {
        func_ov074_0216a618(&mCamera, 2);
        if (mCharacters[0].mEvent == 18) {
            func_ov074_0216b8a8(&mCameraFollower, 3);
        } else {
            func_ov074_0216b8a8(&mCameraFollower, 1);
        }
    } else {
        func_ov074_0216a618(&mCamera, 3);
        func_ov074_0216b8a8(&mCameraFollower, 1);
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
    switch (mButtons[button].mAction) {
    case 0:
        EnterRoom();
        break;
    case 1:
        SetInputState(INPUT_WAIT_LEAVE);
        SetEventState(EVENT_NONE);
        mTtsPlayer.Stop();
        mNextSceneId = 0x5F;
        data_0213ba74.mCharacter = mCharacters[0].mIndex;
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
    case EVENT_RESIDENT_LEAVES:
    case EVENT_NEW_RESIDENT:
    case EVENT_STATE_14:
    case EVENT_STATE_15:
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
    data_0213ba74.mCharacter = mCharacters[0].mIndex;
}

// Enables the two buttons as the state allows.
void RoomScene::UpdateButtons() {
    if (!IsTouchEnabled()) {
        func_ov074_0216ca14(&mButtons[1], FALSE);
    } else {
        func_ov074_0216ca14(&mButtons[1], TRUE);
    }
    if (!CanEnterRoom() || CountPresent() == 0) {
        if (func_ov074_0216ca14(&mButtons[0], FALSE)) {
            mButtons[0].mUnknownSetWithEnabled = 11;
        }
    } else {
        if (func_ov074_0216ca14(&mButtons[0], TRUE)) {
            mButtons[0].mUnknownSetWithEnabled = -1;
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
        if (data_02121e58.mTouched) {
            TouchPoint point = data_02121e58.mPoint;
            BOOL handled = FALSE;

            for (int i = 0; i < 2; i++) {
                if (func_ov074_0216cc38(&mButtons[i], point)) {
                    handled = TRUE;
                    if (mButtons[i].mEnabled) {
                        SetInputState(INPUT_TOUCH_BUTTON);
                        mPressedButton = i;
                        mButtons[i].mPressState = 1;
                        gSoundManager.StartSeqArc(2, 1, 0);
                    }
                    break;
                }
            }
            if (!handled) {
                if (func_ov074_0216b9c8(&mPanel, point)) {
                    if (mButtons[0].mEnabled) {
                        EnterRoom();
                        handled = TRUE;
                    }
                } else if (func_ov074_0216bb20(&mPanel.mInfoButton, point) && mButtons[1].mEnabled) {
                    SetInputState(INPUT_INFO_BUTTON);
                    gSoundManager.StartSeqArc(2, 1, 0);
                    handled = TRUE;
                }
            }
            if (!handled) {
                SetInputState(INPUT_PANEL);
            }
        } else {
            if (data_02121e58.mTouching != 0) {
                return;
            }
            // After a minute of nothing, the panel goes to a random resident's room
            mIdleFrames++;
            if (mIdleFrames > 3600 && mResidentCount >= 2) {
                int count = mResidentCount;
                int room = func_0202c414(&gGameData, mResidents[MATH_Rand16(&gRandom, count)]);
                if ((u16)room != mPanel.mSelectedRoom) {
                    mIdleFrames = 0;
                    func_ov074_0216a62c(&mCamera, 4, 0x474);
                    func_ov074_0216be3c(&mPanel, room, -1);
                }
            }
            ChangeRoom();
        }
        break;

    case INPUT_TOUCH_BUTTON:
        if (data_02121e58.mTouching != 0) {
            if (func_ov074_0216cc38(&mButtons[mPressedButton], data_02121e58.mPoint)) {
                mButtons[mPressedButton].mPressState = 1;
            } else {
                mButtons[mPressedButton].mPressState = 2;
            }
        } else {
            if (func_ov074_0216cc38(&mButtons[mPressedButton], data_02121e58.mLastPoint) &&
                data_0211f9d4.mUnknown20) {
                mButtons[mPressedButton].mPressState = 3;
                PressButton(mPressedButton);
            } else {
                SetInputState(INPUT_IDLE);
                mButtons[mPressedButton].mPressState = 2;
            }
        }
        break;

    case INPUT_INFO_BUTTON:
        if (data_02121e58.mTouching != 0) {
            if (func_ov074_0216bb20(&mPanel.mInfoButton, data_02121e58.mPoint)) {
                func_ov074_0216bb18(&mPanel.mInfoButton, 1);
            } else {
                func_ov074_0216bb18(&mPanel.mInfoButton, 2);
            }
        } else {
            if (func_ov074_0216bb20(&mPanel.mInfoButton, data_02121e58.mLastPoint) && data_0211f9d4.mUnknown20) {
                func_ov074_0216bb18(&mPanel.mInfoButton, 3);
                GoToInformation();
            } else {
                SetInputState(INPUT_IDLE);
                func_ov074_0216bb18(&mPanel.mInfoButton, 2);
            }
        }
        break;

    case INPUT_PANEL:
        if (data_02121e58.mTouching != 0) {
            func_ov074_0216bd2c(&mPanel, data_02121e58.mPoint);
            ChangeRoom();
        } else {
            SetInputState(INPUT_IDLE);
        }
        break;

    case INPUT_ENTER_ROOM:
        if (mInputStep == 0) {
            mInputStep++;
            func_ov074_02152370(&mCharacters[0], 0x113);
            if (mCharacters[1].mPresent) {
                func_ov074_02152370(&mCharacters[1], 0x113);
            }
            func_ov074_0216a62c(&mCamera, 0, 0);
            mCamera.mLeaving = TRUE;
            func_ov074_0216b8a8(&mCameraFollower, 2);
            HeadIcon_SetMode(&mIcon[0], 1);
        } else if (mInputStep == 1) {
            if (mCharacters[0].mState != 0) {
                return;
            }
            mInputStep++;
            func_ov074_02152370(&mCharacters[0], 0x114);
        } else if (mInputStep == 2) {
            if (mCharacters[0].mState != 0) {
                return;
            }
            mInputStep++;
            if (CountPresent() >= 2) {
                func_ov074_02152370(&mCharacters[0], 0x116);
            } else {
                func_ov074_02152370(&mCharacters[0], 0x115);
            }
            // The resident walks half of the way to where the camera looks
            VecFx32 target = mCamera.mLookAt;
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

    if (mPanel.mSelectedRoom == mRoom.mRoomNumber) {
        return;
    }
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    resident = mPanel.mRoomResidents[mPanel.mSelectedRoom & 0xFF];
    mTtsPlayer.Stop();
    FinishEncounter();
    SaveCharacters();

    for (i = 0; i < 2; i++) {
        if (func_0202c3dc(&gGameData, mCharacters[i].mIndex)) {
            func_ov074_0216c1f0(&mPanel, mCharacters[i].mRoom);
            if (func_02099d1c(&mCharacters[i])) {
                func_ov074_0216c1f0(&mPanel, func_0202c414(&gGameData, mCharacters[i].mEventPartner));
            }
        }
    }

    mRoom.mRoomNumber = mPanel.mSelectedRoom;
    if (resident == 0xFF) {
        ChooseCharacters(0);
        LoadCharacters();
        func_ov074_0216a2d0(&mRoom, 8);
        func_ov074_0216c918(&mNamePlate, 2);
    } else {
        ChooseCharacters(resident);
        data_0213ba74.mCharacter = resident;
        LoadCharacters();
        func_ov074_0216a2d0(&mRoom, mCharacters[0].mInterior);
        func_ov074_0216c918(&mNamePlate, 1);
    }

    mCharacters[0].mPresent = FALSE;
    mCharacters[1].mPresent = FALSE;
    func_ov074_0216a354(&mRoom, 1);
    func_ov074_02169890(&mCharacters[0], 1);
    func_ov074_02169890(&mCharacters[1], 1);
    for (int i = 0; i < 4; i++) {
        func_ov074_02169fc8(&mItems[i], 1);
    }
    SetEventState(EVENT_CHANGE_ROOM, 0);
    mIdleFrames = 0;
    func_ov074_0216a62c(&mCamera, 0, 0);
    func_ov074_0216d1a8(&mSpeechText, 4);
    data_0211d604.Hide(1);
    func_ov074_0216cfe8(&mClockDisplay, 1);
    HeadIcon_SetMode(&mIcon[0], 1);
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
        LoadCharacter(temp);
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
        LoadCharacter(character);
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
        LoadCharacter(character);
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
            LoadCharacter(&temp);
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
                LoadCharacter(&partnerTemp);
                partnerTemp.mPersonality = func_02098f2c(&partnerTemp);
                partner = &partnerTemp;
            }
        }

        if (func_0209b208(character, now, force, partner, 0)) {
            SaveCharacter(character);
            if (partner != NULL) {
                SaveCharacter(partner);
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
    if (mRoom.mTransition != 0 || mCharacters[0].mOverrideState != 0 || mCharacters[1].mOverrideState != 0) {
        return FALSE;
    }
    if (mWindowView.mLoadState == 2) {
        return FALSE;
    }
    for (int i = 0; i < 4; i++) {
        if (mItems[i].mTransition != 0) {
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
            func_ov074_0216a354(&mRoom, transition);
        }
    } else if (mEventTimer == 1) {
        if (IsTransitionDone()) {
            mEventTimer++;
            func_ov074_02169890(&mCharacters[0], transition);
        }
    } else if (mEventTimer == 2) {
        if (IsTransitionDone()) {
            mEventTimer++;
            func_ov074_02169890(&mCharacters[1], transition);
        }
    } else if (mEventTimer == 3) {
        if (IsTransitionDone()) {
            mEventTimer++;
            for (int i = 0; i < 4; i++) {
                func_ov074_02169fc8(&mItems[i], transition);
            }
        }
    } else if (mEventTimer == 4) {
        if (IsTransitionDone()) {
            return TRUE;
        }
    }
    return FALSE;
}

// NOT WRITTEN: RoomScene::UpdateEvent() at 0x0214c4e8 (0x2110 bytes), the switch on mEventState (17 states, see
// the EventState enum). What is known of it is in the notes of group a0 in NOTES.md.
