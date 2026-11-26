#include "DivorceScene.h"
#include "MainProgram.h"

// The script's commands (DivorceScript::Run)
enum {
    CMD_CAMERA_POSITION,     // 0: the camera's position (a vector)
    CMD_CAMERA_TARGET,       // 1: its target (a vector)
    CMD_CAMERA_STILL,        // 2: the camera's states
    CMD_CAMERA_FOLLOW_FIRST, // 3
    CMD_CAMERA_FOLLOW_BOTH,  // 4
    CMD_UPDATE_VIEW,         // 5: the camera's eye and the lights' directions follow at once
    CMD_LIGHTS_WHITE,        // 6: the lights' states
    CMD_LIGHTS_GREY,         // 7
    CMD_LIGHTS_BLACK,        // 8
    CMD_LIGHTS_FADE_OUT,     // 9
    CMD_DOOR_OPEN,           // 10: the room's door (there is no room)
    CMD_DOOR_CLOSE,          // 11
    CMD_NONE_12,             // 12
    CMD_SET_POSITION,        // 13: a character's position (the character, a vector)
    CMD_SET_ANGLE,           // 14: the way it faces
    CMD_SET_POSITION_15,     // 15
    CMD_SET_POSITION_16,     // 16
    CMD_NONE_17,             // 17
    CMD_SET_STATE,           // 18: a character's state (DivorceCharacter::State)
    CMD_PLAY_ANIMATION,      // 19: an animation, looping
    CMD_PLAY_ANIMATION_ONCE, // 20: an animation, once
    CMD_SAY,                 // 21: a message of the trouble messages, spoken
    CMD_LOOK_AHEAD,          // 22: the head's turns
    CMD_RESET_HEAD,          // 23
    CMD_LOOK_AT_CAMERA,      // 24
    CMD_TURN_HEAD_TO_CAMERA, // 25
    CMD_LOOK_AT_PARTNER,     // 26
    CMD_TURN_HEAD_TO_PARTNER, // 27
    CMD_HEAD_PITCH_E000,     // 28 to 33: one angle of the head
    CMD_HEAD_PITCH_1800,
    CMD_HEAD_YAW_C100,
    CMD_HEAD_YAW_3F00,
    CMD_HEAD_ROLL_E000,
    CMD_HEAD_ROLL_2000,
    CMD_FACE_8,              // 34 to 44: a face (CharacterBase::SetFace)
    CMD_FACE_9,
    CMD_FACE_10,
    CMD_FACE_11,
    CMD_FACE_4,
    CMD_FACE_13,
    CMD_FACE_14,
    CMD_FACE_15,
    CMD_FACE_16,
    CMD_FACE_17,
    CMD_FACE_18,
    CMD_SEQ_VOLUME,          // 45: the volume of sequence 0 (the volume, frames)
    CMD_JUMP,                // 46: goes on by a number of commands
    CMD_BRANCH,              // 47: the same, by one number or the other
    CMD_WAIT,                // 48: waits a number of frames
    CMD_FADE_OUT,            // 49: darkens the screens
    CMD_WAIT_CHARACTER,      // 50: waits until a character's state is done
    CMD_WAIT_BOTH,           // 51: and both
    CMD_WAIT_ANIMATION,      // 52
    CMD_WAIT_SPEECH,         // 53: waits until the speech engine is idle
    CMD_WAIT_DOOR,           // 54
    CMD_END,                 // 55
};

// Read by nothing in the ROM: overlay 96 has the same two words in the same place (see Ov103_UnusedData)
static const int data_ov103_0214d9c4[2] = {25, 128};
static const VecFx32 sFirstPosition = {0, 0, 0x1800};
static const VecFx32 sSecondPosition = {0, 0, -0x1800};
static const wchar_t sCameraMark[] = L"Ｃ";
static const wchar_t sTargetMark[] = L"Ａ";
static const wchar_t sFirstMark[] = L"●";
static const wchar_t sSecondMark[] = L"○";
static const ScreenRect sSpriteRect = {0, 0, 32, 32};
static const ScreenPoint sHelpPositions[3] = {{140, 156}, {140, 168}, {140, 180}};
static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};
static const wchar_t sHelpTexts[3][16] = {
    L"X/Y：操作切り替え",
    L"START：パラメータ出力",
    L"SELECT：イベントに戻る",
};
static const DivorceCommand sScript[36] = {
    {CMD_CAMERA_POSITION, {57600, 9936, 51600, 0}},
    {CMD_CAMERA_TARGET, {-31200, 9369, -29880, 0}},
    {CMD_UPDATE_VIEW, {0, 0, 0, 0}},
    {CMD_LIGHTS_WHITE, {0, 0, 0, 0}},
    {CMD_SET_POSITION, {0, -5340, 0, 0}},
    {CMD_SET_POSITION, {1, 5340, 0, 0}},
    {CMD_SET_ANGLE, {0, 1024, 0, 0}},
    {CMD_SET_ANGLE, {1, 31744, 0, 0}},
    {CMD_SET_STATE, {0, DivorceCharacter::STATE_LIFTED, 0, 0}},
    {CMD_SET_STATE, {1, DivorceCharacter::STATE_LIFTED, 0, 0}},
    {CMD_CAMERA_POSITION, {57600, 9936, 51600, 0}},
    {CMD_CAMERA_TARGET, {-31200, 9369, -29880, 0}},
    {CMD_UPDATE_VIEW, {0, 0, 0, 0}},
    {CMD_WAIT, {180, 0, 0, 0}},
    {CMD_TURN_HEAD_TO_PARTNER, {0, 0, 0, 0}},
    {CMD_TURN_HEAD_TO_PARTNER, {1, 0, 0, 0}},
    {CMD_CAMERA_POSITION, {27600, 4048, 21000, 0}},
    {CMD_CAMERA_TARGET, {-31200, 13465, -24600, 0}},
    {CMD_UPDATE_VIEW, {0, 0, 0, 0}},
    {CMD_WAIT, {90, 0, 0, 0}},
    {CMD_SAY, {0, 0x265, 0, 0}},
    {CMD_WAIT_SPEECH, {0, 0, 0, 0}},
    {CMD_CAMERA_POSITION, {-27660, 15312, 25560, 0}},
    {CMD_CAMERA_TARGET, {48960, -9575, -48060, 0}},
    {CMD_UPDATE_VIEW, {0, 0, 0, 0}},
    {CMD_FACE_4, {1, 0, 0, 0}},
    {CMD_WAIT, {100, 0, 0, 0}},
    {CMD_CAMERA_POSITION, {69600, 15312, 34740, 0}},
    {CMD_CAMERA_TARGET, {-95340, -9575, -50400, 0}},
    {CMD_UPDATE_VIEW, {0, 0, 0, 0}},
    {CMD_HEAD_PITCH_1800, {1, 0, 0, 0}},
    {CMD_WAIT, {180, 0, 0, 0}},
    {CMD_SEQ_VOLUME, {0, 150, 0, 0}},
    {CMD_FADE_OUT, {0, 0, 0, 0}},
    {CMD_WAIT, {142, 0, 0, 0}},
    {CMD_END, {0, 0, 0, 0}},
};

// The scale of the debugging view's map: 0x258 (0.15) of the floor to a pixel
static const fx32 sMapScale = FX_Inv(0x258);

void DivorceScene::StartLoading() {
    ChooseCharacters(&mCharacterIndices[0], &mCharacterIndices[1]);
    GameData_ReadCharacter(&gGameData, mCharacterIndices[0], &mCharacters[0]);
    mCharacters[0].mIndex = mCharacterIndices[0];
    mCharacters[0].Character::SetArchive("3H0");
    GameData_ReadCharacter(&gGameData, mCharacterIndices[1], &mCharacters[1]);
    mCharacters[1].mIndex = mCharacterIndices[1];
    mCharacters[1].Character::SetArchive("3H1");
    ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/divo_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SMT", "Scn/Scene/SceneMiiTouch_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SKY", "Scn/Scene/Sky_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_event_LZ.bin", 0);
    gSoundManager.StartSeqIfNotPlaying(0, 0x67);
    gSoundManager.LoadGroup(0x4D);
}

void DivorceScene::Init() {
    mCharOffset = 0x100;
    mDebugCharOffset = 0x190;
    mDrawState = 0;
    mDrawFrames = 0;
    mPaused = FALSE;
    mMessageFiles[0].Open("TCM:tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("TTM:tmd_trouble_msg_JP.bmg");
    mMessageFiles[2].Open("TEM:tmd_etc_msg_JP.bmg");
    mTtsPlayer.InitEngine();
    mSpeechText.mMessageFile = &mMessageFiles[1];
    mCharOffset += mWindow.Init(mCharOffset, &mMessageFiles[1], 4, 0x10, &mTtsPlayer, FALSE);
    mWindow.mText[0].mHAlign = mWindow.mText[0].mVAlign = 1;
    mWindow.Hide();
    InitLowerScreen();
    Init3d();
    mDebugView.Init(&mCharacters[0], &mCharacters[1], mDebugCharOffset);

    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/BG_up.ncl", 0x2F00, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/BG_up.ncg", 0x2F00, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_common.nsc", 0x100, 0, 0, 0, 0, -1);
    gScreenControl.Set(0x1111);
    ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
    ScreenLoader_SetScrollX(&gScreenLoader, 0x2F00, 0);
    ScreenLoader_SetScrollY(&gScreenLoader, 0x2F00, 0);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_up.nce", 0x1000, 1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/Obj_up.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/Obj_up.ncl", 0x1000, -1, -1, -1);
    gScreenControl.mUnknown44 = 0;
    gScreenControl.mUnknown44Changed = TRUE;
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0, 0x101);

    mContext.mSceneId = mSceneId;
    mContext.mCommonMessages = &mMessageFiles[0];
    mContext.mTroubleMessages = &mMessageFiles[1];
    mContext.mEtcMessages = &mMessageFiles[2];
    mContext.mWindow = &mWindow;
    mContext.mTtsPlayer = &mTtsPlayer;
    mContext.mSpeechText = &mSpeechText;
    mContext.mCamera = &mCamera;
    mContext.mLights = &mLights;
    mContext.mCharacters[0] = &mCharacters[0];
    mContext.mCharacters[1] = &mCharacters[1];
    mCharacters[0].Init(&mContext);
    mCharacters[1].Init(&mContext);
    mCharacters[0].mPartner = &mCharacters[1];
    mCharacters[1].mPartner = &mCharacters[0];
    mCharacters[0].LoadHeadIcon(&mParticles);
    mCharacters[1].LoadHeadIcon(&mParticles);
    mScript.Reset();
    mScript.SetCharacters(&mCharacters[0], &mCharacters[1]);
    mScript.mCamera = &mCamera;
    mScript.mLights = &mLights;
    mScript.mTtsPlayer = &mTtsPlayer;
    mScript.mCondition = &mCondition;
    mScript.Update();
}

void DivorceScene::Update() {
    mCamera.Update();
    mLights.Update();
    if (mPaused) {
        return;
    }
    mWindow.Update();
    func_0207b138(mParticles.mManager);
    mHeadIcons[0].Update();
    mCharacters[0].Update();
    mCharacters[1].Update();
    mSky.Update();
    if (mSky.mChanged) {
        mG3Sprite.mUnknown42 = mSky.mPalette[1];
    }
    mTtsPlayer.Update();
    mScript.Update();
    if (mScript.mDone == TRUE) {
        Finish();
    }
}

void DivorceScene::Draw() {
    mWindow.Draw();
    switch (mDrawState) {
    case 0:
        mCharacters[0].Update();
        mCharacters[1].Update();
        mDrawFrames++;
        if (mDrawFrames >= 2) {
            mDrawState = 1;
        }
        break;
    case 1:
        break;
    }
    mCharacters[0].Character::Draw();
    mCharacters[1].Character::Draw();
    ParticleSystem_Draw(&mParticles, 0);
    mModel.Draw();
    G3Sprite_Draw(&mG3Sprite);
}

void DivorceScene::Exit() {
    GameData_WriteCharacter(&gGameData, mCharacters[0].mIndex, &mCharacters[0]);
    GameData_WriteCharacter(&gGameData, mCharacters[1].mIndex, &mCharacters[1]);
    DestroyMainFrameHeap();
    ArchiveManager_Unmount(&gArchiveManager, "SMT");
    ArchiveManager_Unmount(&gArchiveManager, "SKY");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ModelResources_ReleaseModel(gModelResources, 2);
    ModelResources_ReleaseModel(gModelResources, 3);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ArchiveManager_Unmount(&gArchiveManager, "3SS");
    ArchiveManager_Unmount(&gArchiveManager, "3H0");
    ArchiveManager_Unmount(&gArchiveManager, "3H1");
}

// When the script is over: the two separate, and the scene goes back to scene 0x40 if it came from there
void DivorceScene::Finish() {
    Separate();
    if (gSceneManager.mPreviousSceneId == 0x40) {
        gSceneManager.RequestScene(0x40);
    } else {
        gSceneManager.RequestScene(0);
    }
}

// Lovers become ex-lovers, spouses divorced. Unless the second has a trouble that the separation does not end, it
// gets one of category 4 about the first (kind 1 for lovers, 2 for spouses), and leaves what it was doing with its
// partner.
void DivorceScene::Separate() {
    u8 kind = 0xFF;
    BOOL separated = FALSE;
    int rank = mCharacters[0].mRelationRank[mCharacters[1].mIndex];
    if (rank == RELATION_LOVERS_FIGHTING || rank == RELATION_LOVERS) {
        mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_EX_LOVERS, 100);
        mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_EX_LOVERS, 100);
        kind = 1;
        separated = TRUE;
    } else if (rank == RELATION_SPOUSES_FIGHTING || rank == RELATION_SPOUSES) {
        mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_DIVORCED, 100);
        mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_DIVORCED, 100);
        kind = 2;
        separated = TRUE;
    }
    if (separated && mCharacters[1].IsTroubleOfCategory34OrKind20()) {
        int category = mCharacters[1].mTroubleCategory;
        separated = FALSE;
        if (category == 4) {
            separated = TRUE;
        } else {
            int troubleKind = data_02114d78[category][mCharacters[1].mTroubleId].mKind;
            if (troubleKind == 4) {
                separated = TRUE;
            } else if (troubleKind == 9 || troubleKind == 10) {
                separated = TRUE;
            }
        }
    }
    if (!separated) {
        return;
    }

    s64 now = Clock_GetSeconds(gGameClock);
    mCharacters[1].ClearTrouble();
    mCharacters[1].mTroubleCategory = 4;
    mCharacters[1].mTroubleId = kind;
    mCharacters[1].mTroubleState = 1;
    mCharacters[1].mTroubleTarget = mCharacters[0].mIndex;
    mCharacters[1].mTroubleEnd =
        now + data_02114d78[mCharacters[1].mTroubleCategory][mCharacters[1].mTroubleId].mMinutes * 60;
    mCharacters[1].mMood = 3;
    mCharacters[1].ResetTroubleProgress();
    mCharacters[1].mSatisfyMeter = 0;
    if (mCharacters[1].IsLongActPaired()) {
        SavedCharacter partner;
        u8 index = mCharacters[1].mLongActPartner;
        if (mCharacters[0].mIndex == (u16)index) {
            mCharacters[0].ClearLongAct();
        } else {
            GameData_ReadCharacter(&gGameData, index, &partner);
            partner.mIndex = index;
            partner.ClearLongAct();
            GameData_WriteCharacter(&gGameData, index, &partner);
        }
    }
    mCharacters[1].ClearLongAct();
}

// Which two saved characters separate: the two that scene 0x40 left in the scene memory, or two residents at random
void DivorceScene::ChooseCharacters(int *first, int *second) {
    int residents[100];
    if (gSceneManager.mPreviousSceneId == 0x40) {
        *first = gSceneMemory.mCharacters[0];
        *second = gSceneMemory.mCharacters[1];
        return;
    }
    int count = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            residents[count] = i;
            count++;
        }
    }
    for (int i = 0; i < 200; i++) {
        Swap(residents[Rand(count)], residents[Rand(count)]);
    }
    *first = residents[0];
    *second = residents[1];
}

// The touch screen: the window for the messages over a background
void DivorceScene::InitLowerScreen() {
    ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_down.nce", 0x10, 0);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/Obj_down.ncg", 0x10, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/Obj_down.ncl", 0x10, -1, -1, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_up.nce", 0x1000, 1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/Obj_up.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/Obj_up.ncl", 0x1000, -1, -1, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/BG_down.ncl", 0x2F, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/BG_down.ncg", 6, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/Window_down.nsc", 2, 0, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_dialogue.nsc", 4, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 4, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 3);
    ScreenControl_SetBlendAlpha(&gScreenControl, 4, 0x11, 8, 8);
}

// The top screen: the model, the sky's colour, the camera, the lights, the particles and the two characters
void DivorceScene::Init3d() {
    RTCDate date;
    RTCTime time;
    void *animations;

    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    CreateMainFrameHeap(0x4000);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 3);
    mCamera.Init(&mCharacters[0], &mCharacters[1]);
    mCamera.Reset(gSceneManager.mPreviousSceneId);
    mLights.Init(&gCamera, NULL);
    ModelResources_SetModel(gModelResources, 3, "3SS:/divorce.imd");
    mModel.Load(3, 0);
    Clock_GetDateTime(gGameClock, &date, &time);
    mSky.Init("SKY", 1);
    mG3Sprite.mUnknown48 = 0;
    ScreenRect rect = sSpriteRect;
    G3Sprite_SetTexture(&mG3Sprite, 0, &rect);
    mG3Sprite.mScaleX = mG3Sprite.mScaleY = 0x28000;
    func_0202b4c4(&mG3Sprite);
    mG3Sprite.mX = 0;
    mG3Sprite.mY = 0;
    mG3Sprite.mDepth = 500;
    mG3Sprite.mUnknown42 = mSky.mPalette[1];
    void *effects = ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa");
    ParticleSystem_Load(&mParticles, effects, 3, 0x50, 0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    mHeadIcons[0].Load(&mParticles, 0);
    animations = ModelResources_BindArchive(gModelResources, 2, "3HA", 0);
    ModelResources_SetModel(gModelResources, 0, mCharacters[0].mModelPath);
    ModelResources_SetModel(gModelResources, 1, mCharacters[1].mModelPath);
    mCharacters[0].Character::Init(0, 0, 0);
    mCharacters[1].Character::Init(1, 0, 0);
    for (int i = 0; i < 2; i++) {
        DivorceCharacter *character = &mCharacters[i];
        GameData_ReadCharacter(&gGameData, mCharacterIndices[i], character);
        character->CharacterBase::MakeFace();
        character->Character::UpdateBodyColors();
        character->Character::UpdateBodyScale();
        character->Character::UpdateAnimSpeed();
        mCharacters[i].mPersonality = character->GetPersonality();
        character->UpdateBiorhythm();
        character->Character::UpdateSpeedScales();
        ResourceSet_SetArchive((void *)mCharacters[i].mAnim.mUnknown04, RESOURCE_BCA0, animations);
        mCharacters[i].mShadowVisible = TRUE;
        character->CharacterBase::SetFace(8);
        character->Character::SetAnimation(RESOURCE_BCA0, 0x21);
    }
    mCharacters[0].SetPos(sFirstPosition);
    mCharacters[1].SetPos(sSecondPosition);
    mCharacters[0].Character::SetAngle(0xC000);
    mCharacters[1].Character::SetAngle(0x4000);
}

// --- The script -------------------------------------------------------------------------------------------------

void DivorceScript::Reset() {
    mDone = FALSE;
    mIndex = 0;
    mFrame = 0;
    mCharacters[0] = NULL;
    mCharacters[1] = NULL;
    mRoom = NULL;
    mCamera = NULL;
    mLights = NULL;
    mCondition = NULL;
}

void DivorceScript::SetCharacters(DivorceCharacter *first, DivorceCharacter *second) {
    mCharacters[0] = first;
    mCharacters[1] = second;
}

// Runs the commands until one waits
void DivorceScript::Update() {
    BOOL next;
    do {
        next = Run(mIndex);
        if (next) {
            mIndex++;
            mFrame = 0;
        }
    } while (next);
    mFrame++;
}

// Runs one command; FALSE when the script has to wait in it
BOOL DivorceScript::Run(int index) {
    BOOL next = TRUE;
    switch (sScript[index].mCommand) {
    case CMD_CAMERA_POSITION:
        mCamera->SetPosition(*(VecFx32 *)sScript[index].mArgs);
        break;
    case CMD_CAMERA_TARGET:
        mCamera->SetTarget(*(VecFx32 *)sScript[index].mArgs);
        Camera_UpdateEye(mCamera->mCamera);
        break;
    case CMD_CAMERA_STILL:
        mCamera->SetState(0);
        break;
    case CMD_CAMERA_FOLLOW_FIRST:
        mCamera->SetState(1);
        break;
    case CMD_CAMERA_FOLLOW_BOTH:
        mCamera->SetState(2);
        break;
    case CMD_UPDATE_VIEW:
        Camera_UpdateEye(mCamera->mCamera);
        mLights->UpdateDirections();
        break;
    case CMD_LIGHTS_WHITE:
        mLights->SetState(1);
        break;
    case CMD_LIGHTS_GREY:
        mLights->SetState(2);
        break;
    case CMD_LIGHTS_BLACK:
        mLights->SetState(3);
        break;
    case CMD_LIGHTS_FADE_OUT:
        mLights->SetState(4);
        break;
    case CMD_DOOR_OPEN:
        mRoom->mDoor.SetMotion(RoomDoor::MOTION_OPEN);
        break;
    case CMD_DOOR_CLOSE:
        mRoom->mDoor.SetMotion(RoomDoor::MOTION_CLOSE);
        break;
    case CMD_SET_POSITION:
        mCharacters[sScript[index].mArgs[0]]->SetPos(*(VecFx32 *)&sScript[index].mArgs[1]);
        break;
    case CMD_SET_ANGLE:
        mCharacters[sScript[index].mArgs[0]]->Character::SetAngle(sScript[index].mArgs[1]);
        break;
    case CMD_SET_POSITION_15:
        mCharacters[sScript[index].mArgs[0]]->SetPos(*(VecFx32 *)&sScript[index].mArgs[1]);
        break;
    case CMD_SET_POSITION_16:
        mCharacters[sScript[index].mArgs[0]]->SetPos(*(VecFx32 *)&sScript[index].mArgs[1]);
        break;
    case CMD_SET_STATE:
        mCharacters[sScript[index].mArgs[0]]->SetState(sScript[index].mArgs[1]);
        break;
    case CMD_PLAY_ANIMATION: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->Character::BlendToAnimation(RESOURCE_BCA0, sScript[index].mArgs[1]);
        mCharacters[i]->GetAnimation()->mPlayOnce = FALSE;
        break;
    }
    case CMD_PLAY_ANIMATION_ONCE: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->Character::BlendToAnimation(RESOURCE_BCA0, sScript[index].mArgs[1]);
        mCharacters[i]->GetAnimation()->mPlayOnce = TRUE;
        break;
    }
    case CMD_SAY: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->SetVoiceOf(mTtsPlayer);
        mCharacters[i]->mLipSync = TRUE;
        mCharacters[i]->Speak(0, sScript[index].mArgs[1], 0);
        break;
    }
    case CMD_LOOK_AHEAD:
        mCharacters[sScript[index].mArgs[0]]->LookAhead();
        break;
    case CMD_RESET_HEAD:
        mCharacters[sScript[index].mArgs[0]]->ResetHead();
        break;
    case CMD_LOOK_AT_CAMERA:
        mCharacters[sScript[index].mArgs[0]]->LookAtCamera();
        break;
    case CMD_TURN_HEAD_TO_CAMERA:
        mCharacters[sScript[index].mArgs[0]]->TurnHeadToCamera();
        break;
    case CMD_LOOK_AT_PARTNER:
        mCharacters[sScript[index].mArgs[0]]->LookAtPartner();
        break;
    case CMD_TURN_HEAD_TO_PARTNER:
        mCharacters[sScript[index].mArgs[0]]->TurnHeadToPartner();
        break;
    case CMD_HEAD_PITCH_E000: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->mHeadTurn = TRUE;
        mCharacters[i]->mHeadPitch = 0xE000;
        break;
    }
    case CMD_HEAD_PITCH_1800: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->mHeadTurn = TRUE;
        mCharacters[i]->mHeadPitch = 0x1800;
        break;
    }
    case CMD_HEAD_YAW_C100: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->mHeadTurn = TRUE;
        mCharacters[i]->mHeadYaw = 0xC100;
        break;
    }
    case CMD_HEAD_YAW_3F00: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->mHeadTurn = TRUE;
        mCharacters[i]->mHeadYaw = 0x3F00;
        break;
    }
    case CMD_HEAD_ROLL_E000: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->mHeadTurn = TRUE;
        mCharacters[i]->mHeadRoll = 0xE000;
        break;
    }
    case CMD_HEAD_ROLL_2000: {
        int i = sScript[index].mArgs[0];
        mCharacters[i]->mHeadTurn = TRUE;
        mCharacters[i]->mHeadRoll = 0x2000;
        break;
    }
    case CMD_FACE_8:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(8);
        break;
    case CMD_FACE_9:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(9);
        break;
    case CMD_FACE_10:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(10);
        break;
    case CMD_FACE_11:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(11);
        break;
    case CMD_FACE_4:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(4);
        break;
    case CMD_FACE_13:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(13);
        break;
    case CMD_FACE_14:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(14);
        break;
    case CMD_FACE_15:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(15);
        break;
    case CMD_FACE_16:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(16);
        break;
    case CMD_FACE_17:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(17);
        break;
    case CMD_FACE_18:
        mCharacters[sScript[index].mArgs[0]]->CharacterBase::SetFace(18);
        break;
    case CMD_SEQ_VOLUME:
        gSoundManager.MoveSeqVolume(0, sScript[index].mArgs[0], sScript[index].mArgs[1]);
        break;
    case CMD_JUMP:
        mIndex += sScript[index].mArgs[0] - 1;
        break;
    case CMD_BRANCH:
        if (*mCondition) {
            mIndex += sScript[index].mArgs[0] - 1;
        } else {
            mIndex += sScript[index].mArgs[1] - 1;
        }
        break;
    case CMD_WAIT:
        if (mFrame < sScript[index].mArgs[0]) {
            next = FALSE;
        }
        break;
    case CMD_FADE_OUT: {
        int level = mFrame * 2;
        if (level > 16) {
            level = 16;
        } else if (level < 0) {
            level = 0;
        }
        ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, level);
        if (mFrame < 8) {
            next = FALSE;
        }
        break;
    }
    case CMD_WAIT_CHARACTER:
        if (!mCharacters[sScript[index].mArgs[0]]->mDone) {
            next = FALSE;
        }
        break;
    case CMD_WAIT_BOTH:
        if (!mCharacters[0]->mDone || !mCharacters[1]->mDone) {
            next = FALSE;
        }
        break;
    case CMD_WAIT_ANIMATION:
        if (mCharacters[sScript[index].mArgs[0]]->GetAnimation()->mFinished) {
            next = FALSE;
        }
        break;
    case CMD_WAIT_SPEECH:
        if (!mTtsPlayer->IsEngineIdle()) {
            next = FALSE;
        }
        break;
    case CMD_WAIT_DOOR:
        if (mRoom->mDoor.mMotion != RoomDoor::MOTION_NONE) {
            next = FALSE;
        }
        break;
    case CMD_END:
        mDone = TRUE;
        next = FALSE;
        break;
    }
    return next;
}

// --- The characters ---------------------------------------------------------------------------------------------

void DivorceCharacter::Init(DivorceContext *context) {
    mContext = context;
    mTtsPlayer = context->mTtsPlayer;
    mCurrentState = mState = STATE_STAND;
    mLipSync = FALSE;
}

void DivorceCharacter::LoadHeadIcon(ParticleSystem *particles) {
    mHeadIcon.Load(particles, 0);
}

void DivorceCharacter::Update() {
    Character::Update();
    UpdateState();
    mHeadIcon.Update();
    if (mTtsPlayer->IsEngineIdle()) {
        return;
    }
    if (mLipSync) {
        CharacterBase::LipSync(mTtsPlayer->GetPhonemeName(), 1);
    }
}

void DivorceCharacter::UpdateState() {
    switch (mCurrentState = mState) {
    case STATE_STAND:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            mShadowVisible = TRUE;
        }
        break;

    case STATE_LIFTED:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x21);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            mPos.y = 0xB33;
            mShadowVisible = FALSE;
        }
        break;

    case STATE_SAY_261:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x20);
            GetAnimation()->mPlayOnce = TRUE;
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x261, 0);
        } else if (!mDone && mTtsPlayer->IsEngineIdle()) {
            mLipSync = FALSE;
            mDone = TRUE;
        }
        break;

    case STATE_SAY_263:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x20);
            GetAnimation()->mPlayOnce = TRUE;
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x263, 0);
        } else if (!mDone && mTtsPlayer->IsEngineIdle()) {
            mLipSync = FALSE;
            mDone = TRUE;
        }
        break;

    case STATE_POSE_16:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x16);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
        }
        break;

    case STATE_WALK:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gWalkAnimations[gPersonalityKinds[mPersonality]][0]);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            Character::ResetSoundPhase(0);
            SetWalkTargetFloorPos(mWalkTarget);
        } else if (!mDone) {
            if (Character::WalkToTarget(Character::ScaleBySpeed(0x23D), 0x22, 4)) {
                mDone = TRUE;
            }
            Character::MoveAlongAngle();
            Character::PlaySoundOnce(1, 0, 0x1A);
        }
        break;

    case STATE_TURN:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gWalkAnimations[gPersonalityKinds[mPersonality]][0]);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetWalkTargetFloorPos(mTurnTarget);
        } else if (!mDone) {
            if (Character::TurnToWalkTarget(0x280, 0)) {
                mDone = TRUE;
            }
            Character::MoveAlongAngle();
            Character::PlaySoundOnce(1, 0, 0x1A);
        }
        break;

    case STATE_ACT_18:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x18);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
        } else if (!mDone && mStep == 0) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            if (mFrame == frames / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 4);
            }
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
                GetAnimation()->mPlayOnce = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_ACT_19:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x19);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
        } else if (!mDone && mStep == 0) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            if (mFrame == frames / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 2);
            }
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
                GetAnimation()->mPlayOnce = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_ACT_1C:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x1C);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(0x15);
            mIconFrame = Rand(50) + 10;
        } else if (!mDone) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            if (mFrame % frames == 0) {
                gSoundManager.StartSeqArc(1, 0x1A, 3);
            }
            if (mIconFrame == mFrame) {
                mHeadIcon.Show();
                mHeadIcon.mIconId = 0xD;
                mHeadIcon.Reload();
                mHeadIcon.SetMode(3);
            }
        }
        break;

    case STATE_SAY_25B:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x19);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25B, 0);
        } else if (!mDone) {
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
                Character::UpdateAnimSpeed();
                mAnimDone = TRUE;
            }
            if (mTtsPlayer->IsEngineIdle() && mAnimDone) {
                mLipSync = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_SAY_25D:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x19);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25D, 0);
        } else if (!mDone) {
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
                Character::UpdateAnimSpeed();
                mAnimDone = TRUE;
            }
            if (mTtsPlayer->IsEngineIdle() && mAnimDone) {
                mLipSync = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_SAY_25F:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25F, 0);
        } else if (!mDone && mTtsPlayer->IsEngineIdle()) {
            mLipSync = FALSE;
            mDone = TRUE;
        }
        break;

    case STATE_HIDE_ICON:
        if (mEntered) {
            mEntered = FALSE;
            mHeadIcon.SetMode(1);
            mDone = TRUE;
        }
        break;
    }
    UpdateHeadIcon();
    mFrame++;
}

void DivorceCharacter::SetState(int state) {
    mState = state;
    mStep = 0;
    mEntered = TRUE;
    mDone = FALSE;
    mFrame = 0;
    mUnknown2B30 = 0;
    mIconFrame = 0;
    mAnimDone = FALSE;
}

// Shows a message of the trouble messages (file 0) or of the other messages (file 1) in the window, with its
// arguments, and speaks its pronunciation, which is the message after it
void DivorceCharacter::Speak(int file, u32 message, int count, int *types, int *values, int mode) {
    SavedCharacter character;
    wchar_t speech[130];
    wchar_t names[2][26];
    wchar_t pronunciations[2][26];
    MessageFile *messages;

    switch (file) {
    case 0:
        messages = mContext->mTroubleMessages;
        break;
    case 1:
        messages = mContext->mEtcMessages;
        break;
    }
    mContext->mWindow->mMessageFile = messages;
    mContext->mWindow->mText[0].mHAlign = 0;
    mContext->mSpeechText->mMessageFile = messages;
    mContext->mSpeechText->mMessage = message + 1;
    switch (file) {
    case 0:
    case 1:
        for (int i = 0; i < count; i++) {
            switch (types[i]) {
            case ARG_NONE:
                break;
            case ARG_CHARACTER:
                GameData_ReadCharacter(&gGameData, values[i], &character);
                MiiData_GetName(&character, names[i]);
                MakeSpeechText(pronunciations[i], names[i], sizeof(names[i]));
                break;
            }
            mContext->mWindow->SetStringArg(names[i], i);
            mContext->mSpeechText->SetStringArg(pronunciations[i], i);
        }
        break;
    }
    mContext->mWindow->Show(message, mode, 120);
    mContext->mWindow->mMessageFile = mContext->mTroubleMessages;
    mContext->mSpeechText->ExpandMessage();
    wcscpy(speech, mContext->mSpeechText->mText);
    mContext->mTtsPlayer->Speak((const u16 *)speech, 0);
}

void DivorceCharacter::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void DivorceCharacter::Speak(int file, u32 message, int mode) {
    Speak(file, message, ARG_NONE, 0, mode);
}

// Turns the head towards a point at once, as far as the limits allow, and straightens it
void DivorceCharacter::TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = 0;
    mHeadPitchNow = pitch;
    mHeadYawNow = yaw;
    mHeadRollNow = 0;
}

// The head as the animation has it
void DivorceCharacter::ResetHead() {
    mHeadTurn = FALSE;
    mHeadPitch = 0;
    mHeadYaw = 0;
    mHeadRoll = 0;
    mHeadPitchNow = 0;
    mHeadYawNow = 0;
    mHeadRollNow = 0;
}

void DivorceCharacter::TurnHeadToCamera() {
    TurnHeadTo(&gCamera.mEye, 0x2000, 0x3F00);
}

void DivorceCharacter::TurnHeadToPartner() {
    VecFx32 target;
    const VecFx32 &head = mPartner->GetJointPosition(0);
    target.x = head.x;
    target.z = head.z;
    target.y = head.y + 0x800;
    TurnHeadTo(&target, 0xB00, 0x3F00);
}

// Turns the head towards a point, as far as the limits allow
void DivorceCharacter::LookAt(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
}

// Turns the head straight ahead
void DivorceCharacter::LookAhead() {
    mHeadTurn = TRUE;
    mHeadPitch = 0;
    mHeadYaw = 0;
    mHeadRoll = 0;
}

void DivorceCharacter::LookAtCamera() {
    LookAt(&gCamera.mEye, 0x2000, 0x3F00);
}

void DivorceCharacter::LookAtPartner() {
    VecFx32 target;
    const VecFx32 &head = mPartner->GetJointPosition(0);
    target.x = head.x;
    target.z = head.z;
    target.y = head.y + 0xB33;
    LookAt(&target, 0xB00, 0x3F00);
}

u16 DivorceCharacter::GetHeadPitchTo(VecFx32 *target) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(0, &matrix, &rotation);
    fx32 dz = target->z - mPos.z;
    fx32 dx = target->x - mPos.x;
    fx32 eyes = matrix._31 + 0xB33;
    return -FX_Atan2Idx(target->y - eyes, SqrtXZ(dx, dz));
}

u16 DivorceCharacter::GetHeadYawTo(VecFx32 *target) {
    return mAngle - FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

// Keeps the head icon over the head, turned with it
void DivorceCharacter::UpdateHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mHeadYawNow;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcon.SetPos(matrices[0]._30, matrices[0]._31, matrices[0]._32);
}

// --- The camera -------------------------------------------------------------------------------------------------

void DivorceCamera::Init(DivorceCharacter *first, DivorceCharacter *second) {
    mCamera = &gCamera;
    Camera_Reset(mCamera);
    mCamera->mAngleX = 0;
    mCamera->mAngleY = 0;
    mCamera->mAngleZ = 0;
    Camera_UpdateEye(mCamera);
    mFirst = first;
    mSecond = second;
    CopyVec(&mPosition, &mCamera->mPosition);
    CopyVec(&mTarget, &mCamera->mTarget);
    mState = 0;
}

// Coming back from another scene: the camera 14 in front of its target, in the direction it was
void DivorceCamera::Reset(BOOL restore) {
    if (!restore) {
        return;
    }
    FloorPos direction = NormalizeXZ(mPosition.x - mTarget.x, mPosition.z - mTarget.z);
    mPosition.x = mTarget.x + direction.x * 14;
    mPosition.z = mTarget.z + direction.z * 14;
    mPosition.y = 0x41D0;
    mTarget.y = 0x1999;
    CopyVec(&mCamera->mPosition, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    Camera_UpdateEye(mCamera);
}

void DivorceCamera::SetPosition(VecFx32 position) {
    mPosition.x = position.x;
    mPosition.y = position.y;
    mPosition.z = position.z;
    CopyVec(&mCamera->mPosition, &mPosition);
}

void DivorceCamera::SetTarget(VecFx32 target) {
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
    CopyVec(&mCamera->mTarget, &mTarget);
}

void DivorceCamera::SetState(int state) {
    mState = state;
    switch (state) {
    case 0:
        return;
    case 1:
        if (mFirst == NULL) {
            mState = 0;
        }
        break;
    case 2:
        if (mFirst == NULL || mSecond == NULL) {
            mState = 0;
        }
        break;
    }
}

// Follows the first character, or the middle between the two, keeping the distance and the height
void DivorceCamera::Update() {
    switch (mState) {
    case 0:
        return;
    case 1: {
        VecFx32 middle;
        VecFx32 position;
        middle = mFirst->GetPos();
        middle.x = Clamp(middle.x, -0x5000, 0x5000);
        middle.z = Clamp(middle.z, -0x5000, 0x5000);
        middle.y = mTarget.y;
        mTarget = Approach(&middle, &mTarget, 5);
        FloorPos direction = NormalizeXZ(mPosition.x - middle.x, mPosition.z - middle.z);
        position.x = middle.x + direction.x * 14;
        position.z = middle.z + direction.z * 14;
        position.y = mPosition.y;
        mPosition = Approach(&position, &mPosition, 5);
        CopyVec(&mCamera->mTarget, &mTarget);
        CopyVec(&mCamera->mPosition, &mPosition);
        break;
    }
    case 2: {
        VecFx32 middle;
        VecFx32 position;
        VecFx32 second;
        VecFx32 first;
        first = mFirst->GetPos();
        second = mSecond->GetPos();
        VEC_Add(&first, &second, &middle);
        middle.x >>= 1;
        middle.y >>= 1;
        middle.z >>= 1;
        middle.x = Clamp(middle.x, -0x5000, 0x5000);
        middle.z = Clamp(middle.z, -0x5000, 0x5000);
        middle.y = mTarget.y;
        mTarget = Approach(&middle, &mTarget, 5);
        FloorPos direction = NormalizeXZ(mPosition.x - middle.x, mPosition.z - middle.z);
        position.x = middle.x + direction.x * 14;
        position.z = middle.z + direction.z * 14;
        position.y = mPosition.y;
        mPosition = Approach(&position, &mPosition, 5);
        CopyVec(&mCamera->mTarget, &mTarget);
        CopyVec(&mCamera->mPosition, &mPosition);
        break;
    }
    }
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 DivorceCamera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = Shift(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

// --- The lights -------------------------------------------------------------------------------------------------

void DivorceLights::Init(Camera *camera, Room *room) {
    VecFx32 view;
    mState = 0;
    mCamera = camera;
    mRoom = room;
    VEC_Subtract(&mCamera->mPosition, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        mLights[i] = &gLights[i];
        Light_Reset(&gLights[i]);
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void DivorceLights::Update() {
    UpdateDirections();
    switch (mState) {
    case 0:
        break;
    case 1:
        SetColor(GX_RGB(31, 31, 31));
        SetState(0);
        break;
    case 2:
        SetColor(GX_RGB(15, 15, 15));
        SetState(0);
        break;
    case 3:
        SetColor(GX_RGB(0, 0, 0));
        SetState(0);
        break;
    case 4:
        if (mStep == 0) {
            mFromColor = mLights[0]->mColor;
            mToColor = GX_RGB(0, 0, 0);
            mStep++;
        } else {
            u16 color;
            if (mFrame >= 64) {
                color = GX_RGB(0, 0, 0);
                SetState(0);
            } else {
                BlendColor(mFromColor, mToColor, &color, 64);
            }
            SetColor(color);
        }
        break;
    }
    mFrame++;
}

void DivorceLights::UpdateDirections() {
    VecFx32 view;
    VEC_Subtract(&mCamera->mEye, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void DivorceLights::SetState(int state) {
    mState = state;
    mStep = 0;
    mFrame = 0;
}

void DivorceLights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// The colour mFrame frames of "frames" on the way from one colour to the other. The blue takes its step from the
// green of the first colour, as the original computes it.
void DivorceLights::BlendColor(u16 from, u16 to, u16 *color, int frames) {
    int fromR, fromG, fromB;
    int toR, toG, toB;
    SplitColor(from, &fromR, &fromG, &fromB);
    SplitColor(to, &toR, &toG, &toB);
    int b = (toB - fromG) * mFrame / frames;
    int r = (toR - fromR) * mFrame / frames;
    int g = (toG - fromG) * mFrame / frames;
    *color = (fromR + r) | ((fromG + g) << 5) | ((fromB + b) << 10);
}

// --- The debugging view -----------------------------------------------------------------------------------------

void DivorceDebugView::Reset() {
    mUnknown000 = 0;
    mUnknown004 = 0;
    mUnknown008 = 0;
    ClearPoints();
}

void DivorceDebugView::ClearPoints() {
    {
        NNSG2dFVec2 v = {0, 0};
        mUnknown00C = v;
    }
    {
        NNSG2dFVec2 v = {0, 0};
        mUnknown014 = v;
    }
    {
        NNSG2dFVec2 v = {0, 0};
        mUnknown01C = v;
    }
    mUnknown074 = mUnknown078 = 0;
    for (int i = 0; i < 10; i++) {
        mPoints[i].x = 0;
        mPoints[i].y = 0;
    }
}

static inline void InitBox(ObjTextBox *box, int width, int *charOffset) {
    box->Init(0x1000, width, 2, *charOffset, 0, 1, 0, 0, NULL);
}

void DivorceDebugView::Init(DivorceCharacter *first, DivorceCharacter *second, int charOffset) {
    Reset();
    mUnknown07C = 1;
    mFirst = first;
    mSecond = second;
    mUnknown778 = 0;

    mMarks[0].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[0].mColor = 2;
    mMarks[0].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[0].GetCharCount();
    mMarks[0].SetText((const NNSG2dChar *)sCameraMark);
    NNS_G2dCharCanvasClear(&mMarks[0].mCanvas, 0);
    mMarks[0].Redraw();

    mMarks[1].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[1].mColor = 8;
    mMarks[1].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[1].GetCharCount();
    mMarks[1].SetText((const NNSG2dChar *)sTargetMark);
    NNS_G2dCharCanvasClear(&mMarks[1].mCanvas, 0);
    mMarks[1].Redraw();

    mMarks[2].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[2].mColor = 8;
    mMarks[2].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[2].GetCharCount();
    mMarks[2].SetText((const NNSG2dChar *)sFirstMark);
    NNS_G2dCharCanvasClear(&mMarks[2].mCanvas, 0);
    mMarks[2].Redraw();

    mMarks[3].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[3].mColor = 8;
    mMarks[3].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[3].GetCharCount();
    mMarks[3].SetText((const NNSG2dChar *)sSecondMark);
    NNS_G2dCharCanvasClear(&mMarks[3].mCanvas, 0);
    mMarks[3].Redraw();

    for (int i = 0; i < 3; i++) {
        mHelp[i].Init(0x1000, 0x10, 2, charOffset, 0, 1, 0, 0, NULL);
        mHelp[i].mColor = 8;
        mHelp[i].mSprite.mOverwritePalette = 2;
        ScreenPoint position = sHelpPositions[i];
        mHelp[i].SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
        charOffset += mHelp[i].GetCharCount();
        mHelp[i].SetText((const NNSG2dChar *)sHelpTexts[i]);
        NNS_G2dCharCanvasClear(&mHelp[i].mCanvas, 0);
        mHelp[i].Redraw();
    }
    UpdateMarks();
}

static inline void PlaceMark(ObjTextBox *box, ScreenPoint point) {
    box->SetPosition(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
}

// Puts the marks of the camera, its target and the two characters on the map
void DivorceDebugView::UpdateMarks() {
    VecFx32 cameraPosition = gCamera.mPosition;
    VecFx32 cameraTarget = gCamera.mTarget;
    VecFx32 second;
    VecFx32 first;
    ScreenPoint secondPoint;
    ScreenPoint firstPoint;
    ScreenPoint cameraPoint;
    ScreenPoint targetPoint;
    ToScreen(&cameraPoint, &cameraPosition);
    ToScreen(&targetPoint, &cameraTarget);
    first = mFirst->GetPos();
    second = mSecond->GetPos();
    ToScreen(&firstPoint, &first);
    ToScreen(&secondPoint, &second);
    PlaceMark(&mMarks[0], cameraPoint);
    PlaceMark(&mMarks[1], targetPoint);
    PlaceMark(&mMarks[2], firstPoint);
    PlaceMark(&mMarks[3], secondPoint);
}

// Where a point of the floor is on the map, in pixels: (0, 0) at the screen's middle
void DivorceDebugView::ToScreen(ScreenPoint *point, VecFx32 *position) {
    point->x = (FX_Mul(position->x, sMapScale) + (128 << FX32_SHIFT)) >> FX32_SHIFT;
    point->y = (FX_Mul(position->z, sMapScale) + (96 << FX32_SHIFT)) >> FX32_SHIFT;
}

// Not in the ROM: the link left it out, as nothing calls it. Something of this file read data_ov103_0214d9c4 (as
// overlay 96's file reads the same two words), or it would not be in the ROM.
extern "C" const int *Ov103_UnusedData(void) {
    return data_ov103_0214d9c4;
}
