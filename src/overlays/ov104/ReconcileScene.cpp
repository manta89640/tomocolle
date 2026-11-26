#include "ReconcileScene.h"
#include "MainProgram.h"

// The commands of the script (ScriptCommand::mOp)
enum ScriptOp {
    OP_CAMERA_POSITION,       // The camera's position: args 0 to 2
    OP_CAMERA_TARGET,         // Its target: args 0 to 2
    OP_CAMERA_FIXED,
    OP_CAMERA_FOLLOW_FIRST,
    OP_CAMERA_FOLLOW_BOTH,
    OP_UPDATE_VIEW,           // The camera's eye, the lights' directions and the room
    OP_LIGHTS_INTERIOR_COLOR,
    OP_LIGHTS_ROOM_COLOR,
    OP_LIGHTS_BLACK,
    OP_LIGHTS_FADE_OUT,
    OP_DOOR_OPEN,
    OP_DOOR_CLOSE,
    OP_NOP_12,
    OP_PLACE,                 // Character arg 0 to the position args 1 to 3
    OP_FACE,                  // Character arg 0 to the angle arg 1
    OP_PLACE_2,
    OP_PLACE_3,
    OP_NOP_17,
    OP_SET_STATE,             // Character arg 0 to state arg 1
    OP_PLAY_ANIMATION,        // Character arg 0 plays animation arg 1, looping
    OP_PLAY_ANIMATION_ONCE,
    OP_SPEAK,                 // Character arg 0 says trouble message arg 1
    OP_LOOK_AHEAD,
    OP_RESET_HEAD,
    OP_LOOK_AT_CAMERA,
    OP_TURN_HEAD_TO_CAMERA,
    OP_LOOK_AT_PARTNER,
    OP_TURN_HEAD_TO_PARTNER,
    OP_START_MUSIC,           // Sequence arg 0
    OP_STOP_MUSIC,
    OP_JUMP,                  // By arg 0 commands
    OP_JUMP_IF_MADE_UP,       // By arg 0 commands if the two made up, by arg 1 if not
    OP_WAIT,                  // Until arg 0 frames have passed
    OP_WAIT_CHARACTER,        // Until character arg 0 is done
    OP_WAIT_CHARACTERS,       // Until both are
    OP_WAIT_ANIMATION,
    OP_WAIT_SPEECH,
    OP_WAIT_DOOR,
    OP_END,
};

// One command of the script: what it does, and four arguments, of which the first three or the last three are a
// vector for some commands
struct ScriptCommand {
    int mOp;
    int mArgs[4];

    VecFx32 &VecAt(int i) const { return *(VecFx32 *)&mArgs[i]; }
};

// Read by nothing in the ROM: the same words as overlay 96's data_ov096_0214c80c, at the same place before the
// file's other constants (see Ov104_UnusedData at the end of the file).
static const int data_ov104_0214d800[2] = {25, 128};
static const VecFx32 sSecondPosition = {0, 0, -0x1800};
static const VecFx32 sFirstPosition = {0, 0, 0x1800};
static const ScreenPoint sLinePositions[3] = {{140, 156}, {140, 168}, {140, 180}};
static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};
static const wchar_t sLines[3][16] = {
    L"X/Y：操作切り替え",
    L"START：パラメータ出力",
    L"SELECT：イベントに移る",
};

static const ScriptCommand sScript[48] = {
    {OP_CAMERA_POSITION, {59700, 20944, -24420, 0}},
    {OP_CAMERA_TARGET, {-48000, -6759, 19800, 0}},
    {OP_UPDATE_VIEW, {0, 0, 0, 0}},
    {OP_LIGHTS_ROOM_COLOR, {0, 0, 0, 0}},
    {OP_PLACE, {0, 0, 0, 6963}},
    {OP_PLACE, {1, 0, 0, -6963}},
    {OP_FACE, {0, 0xC000, 0, 0}},
    {OP_FACE, {1, 0x4000, 0, 0}},
    {OP_SET_STATE, {0, ReconcileCharacter::STATE_STAND, 0, 0}},
    {OP_SET_STATE, {1, ReconcileCharacter::STATE_STAND, 0, 0}},
    {OP_START_MUSIC, {115, 0, 0, 0}},
    {OP_WAIT, {90, 0, 0, 0}},
    {OP_WAIT, {180, 0, 0, 0}},
    {OP_CAMERA_POSITION, {-11340, 7632, 21540, 0}},
    {OP_CAMERA_TARGET, {6000, 7833, -16860, 0}},
    {OP_UPDATE_VIEW, {0, 0, 0, 0}},
    {OP_WAIT, {180, 0, 0, 0}},
    {OP_CAMERA_POSITION, {-13140, 4816, -23340, 0}},
    {OP_CAMERA_TARGET, {4740, 7833, 14340, 0}},
    {OP_UPDATE_VIEW, {0, 0, 0, 0}},
    {OP_WAIT, {180, 0, 0, 0}},
    {OP_CAMERA_POSITION, {-54120, 5840, 600, 0}},
    {OP_CAMERA_TARGET, {14700, 6553, -600, 0}},
    {OP_UPDATE_VIEW, {0, 0, 0, 0}},
    {OP_WAIT, {90, 0, 0, 0}},
    {OP_JUMP_IF_MADE_UP, {1, 15, 0, 0}},
    {OP_CAMERA_POSITION, {-43560, 5840, -31080, 0}},
    {OP_CAMERA_TARGET, {30780, 6553, 21540, 0}},
    {OP_UPDATE_VIEW, {0, 0, 0, 0}},
    {OP_LIGHTS_INTERIOR_COLOR, {0, 0, 0, 0}},
    {OP_SET_STATE, {0, ReconcileCharacter::STATE_START_AGAIN, 0, 0}},
    {OP_WAIT_CHARACTER, {0, 0, 0, 0}},
    {OP_WAIT, {180, 0, 0, 0}},
    {OP_CAMERA_POSITION, {-41400, 5840, 33480, 0}},
    {OP_CAMERA_TARGET, {30720, 6553, -25200, 0}},
    {OP_UPDATE_VIEW, {0, 0, 0, 0}},
    {OP_SET_STATE, {1, ReconcileCharacter::STATE_AGREE, 0, 0}},
    {OP_WAIT_CHARACTER, {1, 0, 0, 0}},
    {OP_WAIT, {180, 0, 0, 0}},
    {OP_END, {0, 0, 0, 0}},
    {OP_SET_STATE, {0, ReconcileCharacter::STATE_STAND_2, 0, 0}},
    {OP_SET_STATE, {1, ReconcileCharacter::STATE_STAND_2, 0, 0}},
    {OP_WAIT, {30, 0, 0, 0}},
    {OP_START_MUSIC, {116, 0, 0, 0}},
    {OP_WAIT, {150, 0, 0, 0}},
    {OP_LIGHTS_FADE_OUT, {0, 0, 0, 0}},
    {OP_WAIT, {100, 0, 0, 0}},
    {OP_END, {0, 0, 0, 0}},
};

// The map's scale: 600 units of the room to a pixel... of fx32 (1 / 600)
static const fx32 sMapScale = FX_Inv(600);

// Which saved character's walking and standing animations to use
static inline int GetPersonalityKind(ReconcileCharacter *character) { return gPersonalityKinds[character->mPersonality]; }

static inline int Rand(int max) { return MATH_Rand16(&gRandom, max); }

// --- The scene -----------------------------------------------------------------------------------------------------

void ReconcileScene::StartLoading() {
    ChooseCharacters(&mCharacterIndices[0], &mCharacterIndices[1]);
    GameData_ReadCharacter(&gGameData, mCharacterIndices[0], &mCharacters[0]);
    mCharacters[0].mIndex = mCharacterIndices[0];
    mCharacters[0].Character::SetArchive("3H0");
    GameData_ReadCharacter(&gGameData, mCharacterIndices[1], &mCharacters[1]);
    mCharacters[1].mIndex = mCharacterIndices[1];
    mCharacters[1].Character::SetArchive("3H1");
    mRoom.mId = mCharacters[1].mInterior;
    mRoom.Mount("CO0", 0);
    mRoom.MountAll("RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SMT", "Scn/Scene/SceneMiiTouch_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_event_LZ.bin", 0);
}

void ReconcileScene::Init() {
    mCharOffset = 0x100;
    mMapCharOffset = 0x190;
    mDrawState = 0;
    mDrawFrames = 0;
    mUnknown24 = FALSE;
    mMessageFiles[0].Open("TCM:tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("TTM:tmd_trouble_msg_JP.bmg");
    mMessageFiles[2].Open("TEM:tmd_etc_msg_JP.bmg");
    mTtsPlayer.InitEngine();
    mSpeechText.mMessageFile = &mMessageFiles[1];
    mCharOffset += mWindow.Init(mCharOffset, &mMessageFiles[1], 4, 0x10, &mTtsPlayer, FALSE);
    mWindow.mText[0].mVAlign = 1;
    mWindow.mText[0].mHAlign = 1;
    mWindow.Hide();
    InitLowerScreen();
    Init3d();
    mMap.Init(&mCharacters[0], &mCharacters[1], mMapCharOffset);

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
    mMadeUp = DecideOutcome();
    mScript.Reset();
    mScript.SetCharacters(&mCharacters[0], &mCharacters[1]);
    mScript.mRoom = &mRoom;
    mScript.mCamera = &mCamera;
    mScript.mLights = &mLights;
    mScript.mTtsPlayer = &mTtsPlayer;
    mScript.mMadeUp = &mMadeUp;
    mScript.Update();
}

void ReconcileScene::Update() {
    mCamera.Update();
    mLights.Update();
    if (mUnknown24) {
        return;
    }
    mWindow.Update();
    func_0207b138(mParticles.mManager);
    mHeadIcons[0].Update();
    mCharacters[0].Update();
    mCharacters[1].Update();
    mRoom.Update();
    mTtsPlayer.Update();
    mScript.Update();
    if (mScript.mDone == TRUE) {
        Finish();
    }
}

void ReconcileScene::Draw() {
    mWindow.Draw();
    switch (mDrawState) {
    case 0:
        mRoom.Update();
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
    mRoom.RoomModels::Draw();
}

void ReconcileScene::Exit() {
    GameData_WriteCharacter(&gGameData, mCharacters[0].mIndex, &mCharacters[0]);
    GameData_WriteCharacter(&gGameData, mCharacters[1].mIndex, &mCharacters[1]);
    DestroyMainFrameHeap();
    ArchiveManager_Unmount(&gArchiveManager, "SMT");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ModelResources_ReleaseModel(gModelResources, 2);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    mRoom.ReleaseAll();
    ArchiveManager_Unmount(&gArchiveManager, "3H0");
    ArchiveManager_Unmount(&gArchiveManager, "3H1");
}

// Whether the two make it up: one chance in two
BOOL ReconcileScene::DecideOutcome() {
    return MATH_Rand16(&gRandom, 100) < 50;
}

// Back to the touch scene (0x40) with the outcome, which is saved; from anywhere else, to scene 0
void ReconcileScene::Finish() {
    if (gSceneManager.mPreviousSceneId == 0x40) {
        MI_CpuFill8(gSceneScratch, 0, 100);
        gSceneScratch[0] = mMadeUp == FALSE;
        ApplyOutcome(mMadeUp);
        gSceneManager.RequestScene(0x40);
    } else {
        gSceneManager.RequestScene(0);
    }
}

static inline int GetTroubleKind(SavedCharacter *character) {
    return data_02114d78[character->mTroubleCategory][character->mTroubleId].mKind;
}

// Lovers and spouses who made up are that again, and the trouble of the second ends (kind 11) or goes (kind 9 for
// lovers, 10 for spouses). Otherwise they are ex-lovers or divorced, and the second gets heartbreak (category 4,
// id 1 or 2) over the first, unless it has a trouble that keeps it
void ReconcileScene::ApplyOutcome(BOOL madeUp) {
    int id = 0xFF;
    BOOL parted = FALSE;
    int relation = mCharacters[0].mRelationRank[mCharacters[1].mIndex];
    if (relation == RELATION_LOVERS_FIGHTING || relation == RELATION_LOVERS) {
        if (madeUp) {
            mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_LOVERS, 100);
            mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_LOVERS, 100);
            int kind = GetTroubleKind(&mCharacters[1]);
            if (kind == 11) {
                mCharacters[1].EndTrouble(Clock_GetSeconds(gGameClock));
            } else if (kind == 9) {
                mCharacters[1].ClearTrouble();
            }
        } else {
            mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_EX_LOVERS, 100);
            mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_EX_LOVERS, 100);
            parted = TRUE;
            id = 1;
        }
    } else if (relation == RELATION_SPOUSES_FIGHTING || relation == RELATION_SPOUSES) {
        if (madeUp) {
            mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_SPOUSES, 100);
            mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_SPOUSES, 100);
            int kind = GetTroubleKind(&mCharacters[1]);
            if (kind == 11) {
                mCharacters[1].EndTrouble(Clock_GetSeconds(gGameClock));
            } else if (kind == 10) {
                mCharacters[1].ClearTrouble();
            }
        } else {
            mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_DIVORCED, 100);
            mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_DIVORCED, 100);
            parted = TRUE;
            id = 2;
        }
    }

    if (parted && mCharacters[1].IsTroubleOfCategory34OrKind20()) {
        parted = FALSE;
        if (mCharacters[1].mTroubleCategory == 4 || GetTroubleKind(&mCharacters[1]) == 4) {
            parted = TRUE;
        } else if (GetTroubleKind(&mCharacters[1]) == 9 || GetTroubleKind(&mCharacters[1]) == 10) {
            parted = TRUE;
        }
    }
    if (!parted) {
        return;
    }
    s64 now = Clock_GetSeconds(gGameClock);
    mCharacters[1].ClearTrouble();
    mCharacters[1].mTroubleCategory = 4;
    mCharacters[1].mTroubleId = id;
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

// Back from the touch scene, the two it left; otherwise two of the saved characters at random
void ReconcileScene::ChooseCharacters(int *first, int *second) {
    int indices[100];
    if (gSceneManager.mPreviousSceneId == 0x40) {
        *first = gSceneMemory.mCharacters[0];
        *second = gSceneMemory.mCharacters[1];
        return;
    }
    int count = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            indices[count] = i;
            count++;
        }
    }
    u16 n = count;
    for (int i = 0; i < 200; i++) {
        u16 a = MATH_Rand16(&gRandom, n);
        u16 b = MATH_Rand16(&gRandom, n);
        int t = indices[a];
        indices[a] = indices[b];
        indices[b] = t;
    }
    *first = indices[0];
    *second = indices[1];
}

// The touch screen: the window for the messages over a background
void ReconcileScene::InitLowerScreen() {
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

// The top screen: the room, the camera, the lights, the effects and the two characters
void ReconcileScene::Init3d() {
    void *animations;

    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    CreateMainFrameHeap(0x4000);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 3);
    mCamera.Init(&mCharacters[0], &mCharacters[1]);
    mCamera.Reset(gSceneManager.mPreviousSceneId);
    mLights.Init(&gCamera, &mRoom);
    mRoom.Load(3);
    mRoom.LoadParts(4);
    mRoom.RoomModels::SetState(2);
    ParticleSystem_Load(&mParticles, ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa"), 3, 0x50,
                        0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    mHeadIcons[0].Load(&mParticles, 0);
    animations = ModelResources_BindArchive(gModelResources, 2, "3HA", 0);
    ModelResources_SetModel(gModelResources, 0, mCharacters[0].mModelPath);
    ModelResources_SetModel(gModelResources, 1, mCharacters[1].mModelPath);
    mCharacters[0].Character::Init(0, 0, 0);
    mCharacters[1].Character::Init(1, 0, 0);
    for (int i = 0; i < 2; i++) {
        ReconcileCharacter *character = &mCharacters[i];
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
        character->Character::SetAnimation(RESOURCE_BCA0, 0x15);
    }
    mCharacters[0].SetPos(sFirstPosition);
    mCharacters[1].SetPos(sSecondPosition);
    mCharacters[0].Character::SetAngle(0xC000);
    mCharacters[1].Character::SetAngle(0x4000);
}

// --- The script ----------------------------------------------------------------------------------------------------

void ReconcileScript::Reset() {
    mDone = FALSE;
    mStep = 0;
    mFrame = 0;
    mCharacters[0] = NULL;
    mCharacters[1] = NULL;
    mRoom = NULL;
    mCamera = NULL;
    mLights = NULL;
    mMadeUp = NULL;
}

void ReconcileScript::SetCharacters(ReconcileCharacter *first, ReconcileCharacter *second) {
    mCharacters[0] = first;
    mCharacters[1] = second;
}

// Runs the commands from the current one on until one waits
void ReconcileScript::Update() {
    while (Execute(mStep)) {
        mStep++;
        mFrame = 0;
    }
    mFrame++;
}

// Runs one command; FALSE if the script stays on it
BOOL ReconcileScript::Execute(int step) {
    BOOL next = TRUE;
    switch (sScript[step].mOp) {
    case OP_CAMERA_POSITION:
        mCamera->SetPosition(sScript[step].VecAt(0));
        break;
    case OP_CAMERA_TARGET:
        mCamera->SetTarget(sScript[step].VecAt(0));
        Camera_UpdateEye(mCamera->mCamera);
        break;
    case OP_CAMERA_FIXED:
        mCamera->SetMode(ReconcileCamera::MODE_FIXED);
        break;
    case OP_CAMERA_FOLLOW_FIRST:
        mCamera->SetMode(ReconcileCamera::MODE_FOLLOW_FIRST);
        break;
    case OP_CAMERA_FOLLOW_BOTH:
        mCamera->SetMode(ReconcileCamera::MODE_FOLLOW_BOTH);
        break;
    case OP_UPDATE_VIEW:
        Camera_UpdateEye(mCamera->mCamera);
        mLights->UpdateDirections();
        mRoom->Update();
        break;
    case OP_LIGHTS_INTERIOR_COLOR:
        mLights->SetState(ReconcileLights::STATE_INTERIOR_COLOR);
        break;
    case OP_LIGHTS_ROOM_COLOR:
        mLights->SetState(ReconcileLights::STATE_ROOM_COLOR);
        break;
    case OP_LIGHTS_BLACK:
        mLights->SetState(ReconcileLights::STATE_BLACK);
        break;
    case OP_LIGHTS_FADE_OUT:
        mLights->SetState(ReconcileLights::STATE_FADE_OUT);
        break;
    case OP_DOOR_OPEN:
        mRoom->mDoor.SetMotion(RoomDoor::MOTION_OPEN);
        break;
    case OP_DOOR_CLOSE:
        mRoom->mDoor.SetMotion(RoomDoor::MOTION_CLOSE);
        break;
    case OP_NOP_12:
        break;
    case OP_PLACE:
        mCharacters[sScript[step].mArgs[0]]->SetPos(sScript[step].VecAt(1));
        break;
    case OP_FACE:
        mCharacters[sScript[step].mArgs[0]]->Character::SetAngle(sScript[step].mArgs[1]);
        break;
    case OP_PLACE_2:
        mCharacters[sScript[step].mArgs[0]]->SetPos(sScript[step].VecAt(1));
        break;
    case OP_PLACE_3:
        mCharacters[sScript[step].mArgs[0]]->SetPos(sScript[step].VecAt(1));
        break;
    case OP_NOP_17:
        break;
    case OP_SET_STATE:
        mCharacters[sScript[step].mArgs[0]]->SetState(sScript[step].mArgs[1]);
        break;
    case OP_PLAY_ANIMATION: {
        int i = sScript[step].mArgs[0];
        mCharacters[i]->Character::BlendToAnimation(RESOURCE_BCA0, sScript[step].mArgs[1]);
        mCharacters[i]->GetAnimation()->mPlayOnce = FALSE;
        break;
    }
    case OP_PLAY_ANIMATION_ONCE: {
        int i = sScript[step].mArgs[0];
        mCharacters[i]->Character::BlendToAnimation(RESOURCE_BCA0, sScript[step].mArgs[1]);
        mCharacters[i]->GetAnimation()->mPlayOnce = TRUE;
        break;
    }
    case OP_SPEAK: {
        int i = sScript[step].mArgs[0];
        mCharacters[i]->SetVoiceOf(mTtsPlayer);
        mCharacters[i]->mLipSync = TRUE;
        mCharacters[i]->Speak(0, sScript[step].mArgs[1], 0);
        break;
    }
    case OP_LOOK_AHEAD:
        mCharacters[sScript[step].mArgs[0]]->LookAhead();
        break;
    case OP_RESET_HEAD:
        mCharacters[sScript[step].mArgs[0]]->ResetHead();
        break;
    case OP_LOOK_AT_CAMERA:
        mCharacters[sScript[step].mArgs[0]]->LookAtCamera();
        break;
    case OP_TURN_HEAD_TO_CAMERA:
        mCharacters[sScript[step].mArgs[0]]->TurnHeadToCamera();
        break;
    case OP_LOOK_AT_PARTNER:
        mCharacters[sScript[step].mArgs[0]]->LookAtPartner();
        break;
    case OP_TURN_HEAD_TO_PARTNER:
        mCharacters[sScript[step].mArgs[0]]->TurnHeadToPartner();
        break;
    case OP_START_MUSIC:
        gSoundManager.StartSeqIfNotPlaying(0, sScript[step].mArgs[0]);
        break;
    case OP_STOP_MUSIC:
        gSoundManager.StopSeq(0, 1);
        break;
    case OP_JUMP:
        mStep += sScript[step].mArgs[0] - 1;
        break;
    case OP_JUMP_IF_MADE_UP:
        if (*mMadeUp) {
            mStep += sScript[step].mArgs[0] - 1;
        } else {
            mStep += sScript[step].mArgs[1] - 1;
        }
        break;
    case OP_WAIT:
        if (mFrame < sScript[step].mArgs[0]) {
            next = FALSE;
        }
        break;
    case OP_WAIT_CHARACTER:
        if (!mCharacters[sScript[step].mArgs[0]]->mDone) {
            next = FALSE;
        }
        break;
    case OP_WAIT_CHARACTERS:
        if (!mCharacters[0]->mDone || !mCharacters[1]->mDone) {
            next = FALSE;
        }
        break;
    case OP_WAIT_ANIMATION:
        if (mCharacters[sScript[step].mArgs[0]]->GetAnimation()->mFinished) {
            next = FALSE;
        }
        break;
    case OP_WAIT_SPEECH:
        if (!mTtsPlayer->IsEngineIdle()) {
            next = FALSE;
        }
        break;
    case OP_WAIT_DOOR:
        if (mRoom->mDoor.mMotion != RoomDoor::MOTION_NONE) {
            next = FALSE;
        }
        break;
    case OP_END:
        mDone = TRUE;
        next = FALSE;
        break;
    }
    return next;
}

// --- The characters ------------------------------------------------------------------------------------------------

void ReconcileCharacter::Init(ReconcileContext *context) {
    mContext = context;
    mTtsPlayer = context->mTtsPlayer;
    mCurrentState = mState = STATE_STAND;
    mLipSync = FALSE;
}

void ReconcileCharacter::LoadHeadIcon(ParticleSystem *particles) {
    mHeadIcon.Load(particles, 0);
}

void ReconcileCharacter::Update() {
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

static inline void SetFloorTarget(ReconcileCharacter *character, FloorPos target) {
    character->mTarget.x = target.x;
    character->mTarget.z = target.z;
}

void ReconcileCharacter::UpdateState() {
    switch (mCurrentState = mState) {
    case STATE_STAND:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x15);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
        }
        break;

    case STATE_START_AGAIN:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x20);
            GetAnimation()->mPlayOnce = TRUE;
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x261, 0); // "Let us start again"
        } else if (!mDone && mTtsPlayer->IsEngineIdle()) {
            mLipSync = FALSE;
            mDone = TRUE;
        }
        break;

    case STATE_AGREE:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x20);
            GetAnimation()->mPlayOnce = TRUE;
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x263, 0); // "Yes, please"
        } else if (!mDone && mTtsPlayer->IsEngineIdle()) {
            mLipSync = FALSE;
            mDone = TRUE;
        }
        break;

    case STATE_STAND_2:
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
            Character::BlendToAnimation(RESOURCE_BCA0, gWalkAnimations[GetPersonalityKind(this)][0]);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            Character::ResetSoundPhase(0);
            SetFloorTarget(this, mWalkTargets[0]);
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
            Character::BlendToAnimation(RESOURCE_BCA0, gWalkAnimations[GetPersonalityKind(this)][0]);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetFloorTarget(this, mWalkTargets[1]);
        } else if (!mDone) {
            if (Character::TurnToWalkTarget(0x280, 0)) {
                mDone = TRUE;
            }
            Character::MoveAlongAngle();
            Character::PlaySoundOnce(1, 0, 0x1A);
        }
        break;

    case STATE_ACT:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x18);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
        } else if (!mDone && !mUnknown2B20) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            if (mFrame == frames / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 4);
            }
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[GetPersonalityKind(this)][0]);
                GetAnimation()->mPlayOnce = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_ACT_2:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x19);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
        } else if (!mDone && !mUnknown2B20) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            if (mFrame == frames / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 2);
            }
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[GetPersonalityKind(this)][0]);
                GetAnimation()->mPlayOnce = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_SULK:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x1C);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(0x15);
            mWait = MATH_Rand16(&gRandom, 50) + 10;
        } else if (!mDone) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10;
            if (mFrame % (frames + 1) == 0) {
                gSoundManager.StartSeqArc(1, 0x1A, 3);
            }
            if (mWait == mFrame) {
                mHeadIcon.Show();
                mHeadIcon.mIconId = 0xD;
                mHeadIcon.Reload();
                mHeadIcon.SetMode(3);
            }
        }
        break;

    case STATE_APOLOGIZE:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x19);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25B, 0); // "I am sorry about the other day"
        } else if (!mDone) {
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[GetPersonalityKind(this)][0]);
                Character::UpdateAnimSpeed();
                mAnimDone = TRUE;
            }
            if (mTtsPlayer->IsEngineIdle() && mAnimDone) {
                mLipSync = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_APOLOGIZE_TOO:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x19);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25D, 0); // "I am sorry too"
        } else if (!mDone) {
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[GetPersonalityKind(this)][0]);
                Character::UpdateAnimSpeed();
                mAnimDone = TRUE;
            }
            if (mTtsPlayer->IsEngineIdle() && mAnimDone) {
                mLipSync = FALSE;
                mDone = TRUE;
            }
        }
        break;

    case STATE_REFUSE:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[GetPersonalityKind(this)][0]);
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25F, 0); // "I will not forgive you"
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

void ReconcileCharacter::SetState(int state) {
    mState = state;
    mUnknown2B20 = FALSE;
    mEntered = TRUE;
    mDone = FALSE;
    mFrame = 0;
    mStep = 0;
    mWait = 0;
    mAnimDone = FALSE;
}

// Shows a message of the trouble messages (file 0) or of the etc messages (file 1) in the window, with its
// arguments, and speaks its pronunciation, which is the message after it
void ReconcileCharacter::Speak(int file, u32 message, int count, int *types, int *values, int mode) {
    SavedCharacter character;
    wchar_t speech[130];
    wchar_t names[4][26];
    wchar_t pronunciations[4][26];
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

void ReconcileCharacter::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void ReconcileCharacter::Speak(int file, u32 message, int mode) {
    Speak(file, message, ARG_NONE, 0, mode);
}

// Turns the head towards a point at once, as far as the limits allow
void ReconcileCharacter::TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
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

// The head straight, at once, and left alone
void ReconcileCharacter::ResetHead() {
    mHeadTurn = FALSE;
    mHeadPitch = 0;
    mHeadYaw = 0;
    mHeadRoll = 0;
    mHeadPitchNow = 0;
    mHeadYawNow = 0;
    mHeadRollNow = 0;
}

void ReconcileCharacter::TurnHeadToCamera() {
    TurnHeadTo(&gCamera.mEye, 0x2000, 0x3F00);
}

void ReconcileCharacter::TurnHeadToPartner() {
    VecFx32 target;
    const VecFx32 &head = mPartner->GetJointPosition(0);
    target.x = head.x;
    target.z = head.z;
    target.y = head.y + 0x800;
    TurnHeadTo(&target, 0xB00, 0x3F00);
}

// Turns the head towards a point, as far as the limits allow
void ReconcileCharacter::LookAt(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
}

// Turns the head straight
void ReconcileCharacter::LookAhead() {
    mHeadTurn = TRUE;
    mHeadPitch = 0;
    mHeadYaw = 0;
    mHeadRoll = 0;
}

void ReconcileCharacter::LookAtCamera() {
    LookAt(&gCamera.mEye, 0x2000, 0x3F00);
}

void ReconcileCharacter::LookAtPartner() {
    VecFx32 target;
    const VecFx32 &head = mPartner->GetJointPosition(0);
    target.x = head.x;
    target.z = head.z;
    target.y = head.y + 0x800;
    LookAt(&target, 0xB00, 0x3F00);
}

u16 ReconcileCharacter::GetHeadPitchTo(VecFx32 *target) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(0, &matrix, &rotation);
    fx32 dz = target->z - mPos.z;
    fx32 dx = target->x - mPos.x;
    fx32 eyes = matrix._31 + 0xCCC;
    return -FX_Atan2Idx(target->y - eyes, SqrtXZ(dx, dz));
}

u16 ReconcileCharacter::GetHeadYawTo(VecFx32 *target) {
    return mAngle - FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

// The head icon over the head, turned with it
void ReconcileCharacter::UpdateHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mHeadYawNow;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcon.SetPos(matrices[0]._30, matrices[0]._31, matrices[0]._32);
}

// --- The camera ----------------------------------------------------------------------------------------------------

void ReconcileCamera::Init(ReconcileCharacter *first, ReconcileCharacter *second) {
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
    mMode = MODE_FIXED;
}

// Puts the camera 14 in front of the target, in the direction it was, at a fixed height
void ReconcileCamera::Reset(BOOL restore) {
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

void ReconcileCamera::SetPosition(VecFx32 position) {
    mPosition.x = position.x;
    mPosition.y = position.y;
    mPosition.z = position.z;
    CopyVec(&mCamera->mPosition, &mPosition);
}

void ReconcileCamera::SetTarget(VecFx32 target) {
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
    CopyVec(&mCamera->mTarget, &mTarget);
}

void ReconcileCamera::SetMode(int mode) {
    mMode = mode;
    switch (mode) {
    case MODE_FIXED:
        break;
    case MODE_FOLLOW_FIRST:
        if (mFirst == NULL) {
            mMode = MODE_FIXED;
        }
        break;
    case MODE_FOLLOW_BOTH:
        if (mFirst == NULL || mSecond == NULL) {
            mMode = MODE_FIXED;
        }
        break;
    }
}

// Follows the first character, or the middle between the two, keeping the distance and the height
void ReconcileCamera::Update() {
    switch (mMode) {
    case MODE_FIXED:
        return;
    case MODE_FOLLOW_FIRST: {
        VecFx32 target;
        VecFx32 position;
        target = mFirst->mPos;
        target.x = Clamp(target.x, -0x5000, 0x5000);
        target.z = Clamp(target.z, -0x5000, 0x5000);
        target.y = mTarget.y;
        mTarget = Approach(&target, &mTarget, 5);
        FloorPos direction = NormalizeXZ(mPosition.x - target.x, mPosition.z - target.z);
        position.x = target.x + direction.x * 14;
        position.z = target.z + direction.z * 14;
        position.y = mPosition.y;
        mPosition = Approach(&position, &mPosition, 5);
        CopyVec(&mCamera->mTarget, &mTarget);
        CopyVec(&mCamera->mPosition, &mPosition);
        break;
    }
    case MODE_FOLLOW_BOTH: {
        VecFx32 middle;
        VecFx32 position;
        VecFx32 second;
        VecFx32 first;
        first = mFirst->mPos;
        second = mSecond->mPos;
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
    default:
        return;
    }
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 ReconcileCamera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = Shift(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

// --- The lights ----------------------------------------------------------------------------------------------------

void ReconcileLights::Init(Camera *camera, Room *room) {
    VecFx32 view;
    mState = STATE_NONE;
    mCamera = camera;
    mRoom = room;
    VEC_Subtract(&mCamera->mPosition, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        mLights[i] = &gLights[i];
        Light_Reset(&gLights[i]);
        mLights[i]->mColor = mRoom->GetLightColor();
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void ReconcileLights::Update() {
    UpdateDirections();
    switch (mState) {
    case STATE_NONE:
        break;
    case STATE_INTERIOR_COLOR:
        SetColor(gInteriorTable[mRoom->mId].mLightColor);
        SetState(STATE_NONE);
        break;
    case STATE_ROOM_COLOR:
        SetColor(mRoom->GetLightColor());
        SetState(STATE_NONE);
        break;
    case STATE_BLACK:
        SetColor(0);
        SetState(STATE_NONE);
        break;
    case STATE_FADE_OUT:
        if (mStep == 0) {
            mFromColor = mLights[0]->mColor;
            mToColor = 0;
            mStep++;
        } else {
            u16 color;
            if (mFrame >= 64) {
                color = 0;
                SetState(STATE_NONE);
            } else {
                BlendColor(mFromColor, mToColor, &color, 64);
            }
            SetColor(color);
        }
        break;
    }
    mFrame++;
}

void ReconcileLights::UpdateDirections() {
    VecFx32 view;
    VEC_Subtract(&mCamera->mEye, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void ReconcileLights::SetState(int state) {
    mState = state;
    mStep = 0;
    mFrame = 0;
}

void ReconcileLights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// The colour mFrame frames of "frames" from one colour to another. The blue is computed from the first colour's
// green, not its blue: so the original has it.
void ReconcileLights::BlendColor(u16 from, u16 to, u16 *color, int frames) {
    int r0, g0, b0;
    int r1, g1, b1;
    SplitColor(from, &r0, &g0, &b0);
    SplitColor(to, &r1, &g1, &b1);
    int r = r0 + (r1 - r0) * mFrame / frames;
    int g = g0 + (g1 - g0) * mFrame / frames;
    int b = b0 + (b1 - g0) * mFrame / frames;
    r = Clamp(r, 0, 31);
    g = Clamp(g, 0, 31);
    b = Clamp(b, 0, 31);
    *color = r | (g << 5) | (b << 10);
}

// --- The debug map -------------------------------------------------------------------------------------------------

static inline ScreenPoint ZeroPoint() {
    ScreenPoint point;
    point.x = 0;
    point.y = 0;
    return point;
}

void ReconcileDebugMap::Reset() {
    mUnknown000 = 0;
    mUnknown004 = 0;
    mUnknown008 = 0;
    ClearPoints();
}

void ReconcileDebugMap::ClearPoints() {
    mUnknown00C[0] = ZeroPoint();
    mUnknown00C[1] = ZeroPoint();
    mUnknown00C[2] = ZeroPoint();
    mUnknown078 = 0;
    mUnknown074 = 0;
    for (int i = 0; i < 10; i++) {
        mUnknown024[i].x = 0;
        mUnknown024[i].y = 0;
    }
}

static inline void SetPixelPosition(ObjTextBox *box, ScreenPoint point) {
    box->SetPosition(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
}

void ReconcileDebugMap::Init(ReconcileCharacter *first, ReconcileCharacter *second, int charOffset) {
    Reset();
    mUnknown07C = TRUE;
    mFirst = first;
    mSecond = second;
    mUnknown778 = 0;

    mMarks[0].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[0].mColor = 2;
    mMarks[0].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[0].GetCharCount();
    mMarks[0].SetText((const NNSG2dChar *)L"Ｃ");
    NNS_G2dCharCanvasClear(&mMarks[0].mCanvas, 0);
    mMarks[0].Redraw();

    mMarks[1].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[1].mColor = 8;
    mMarks[1].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[1].GetCharCount();
    mMarks[1].SetText((const NNSG2dChar *)L"Ａ");
    NNS_G2dCharCanvasClear(&mMarks[1].mCanvas, 0);
    mMarks[1].Redraw();

    mMarks[2].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[2].mColor = 8;
    mMarks[2].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[2].GetCharCount();
    mMarks[2].SetText((const NNSG2dChar *)L"●");
    NNS_G2dCharCanvasClear(&mMarks[2].mCanvas, 0);
    mMarks[2].Redraw();

    mMarks[3].Init(0x1000, 2, 2, charOffset, 0, 1, 0, 0, NULL);
    mMarks[3].mColor = 8;
    mMarks[3].mSprite.mOverwritePalette = 1;
    charOffset += mMarks[3].GetCharCount();
    mMarks[3].SetText((const NNSG2dChar *)L"○");
    NNS_G2dCharCanvasClear(&mMarks[3].mCanvas, 0);
    mMarks[3].Redraw();

    for (int i = 0; i < 3; i++) {
        mLines[i].Init(0x1000, 0x10, 2, charOffset, 0, 1, 0, 0, NULL);
        mLines[i].mColor = 8;
        mLines[i].mSprite.mOverwritePalette = 2;
        SetPixelPosition(&mLines[i], sLinePositions[i]);
        charOffset += mLines[i].GetCharCount();
        mLines[i].SetText((const NNSG2dChar *)sLines[i]);
        NNS_G2dCharCanvasClear(&mLines[i].mCanvas, 0);
        mLines[i].Redraw();
    }
    PlaceMarks();
}

// Puts the marks where the camera, its target and the two characters are
void ReconcileDebugMap::PlaceMarks() {
    ScreenPoint first;
    ScreenPoint second;
    ScreenPoint target;
    ScreenPoint camera;
    VecFx32 firstPos;
    VecFx32 secondPos;
    VecFx32 targetPos = gCamera.mTarget;
    VecFx32 cameraPos = gCamera.mPosition;

    ToScreen(&camera, &cameraPos);
    ToScreen(&target, &targetPos);
    firstPos = mFirst->mPos;
    secondPos = mSecond->mPos;
    ToScreen(&first, &firstPos);
    ToScreen(&second, &secondPos);
    SetPixelPosition(&mMarks[0], camera);
    SetPixelPosition(&mMarks[1], target);
    SetPixelPosition(&mMarks[2], first);
    SetPixelPosition(&mMarks[3], second);
}

// Where a point of the room (x, z) is on the map: the room's centre at (128, 96)
void ReconcileDebugMap::ToScreen(ScreenPoint *point, VecFx32 *position) {
    point->x = (FX_Mul(position->x, sMapScale) + (128 << FX32_SHIFT)) >> FX32_SHIFT;
    point->y = (FX_Mul(position->z, sMapScale) + (96 << FX32_SHIFT)) >> FX32_SHIFT;
}

// Not in the ROM: the link left it out, as nothing calls it. Something of this file read data_ov104_0214d800, or it
// would not be in the ROM; what it did with it is not known, and this stands in for it (as in overlay 96).
extern "C" const int *Ov104_UnusedData(void) {
    return data_ov104_0214d800;
}
