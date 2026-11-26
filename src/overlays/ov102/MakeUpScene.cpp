#include "MakeUpScene.h"
#include "MainProgram.h"

#include <wchar.h>

// Overlay 102: two characters who fell out make up, or fail to (MakeUpScene, scene 0xB2). One source file: its
// constants are one pool at the start of .rodata, its strings the .data after the scene's virtual table.

// A point in pixels
struct PixelPoint {
    int x;
    int y;
};

// The file's constants. Their order decides where the compiler puts them.
static const PixelPoint data_ov102_0214cd64 = {0x80, 0x19};
static const PixelPoint sButton3Position = {0xF2, 0x0D};
static const PixelPoint sButton2Size = {0x20, 0x16};
static const PixelPoint sButton3Size = {0x18, 0x16};
static const FloorPos sWalkUpTarget = {0, -0x2000};
static const PixelPoint sButton0Position = {0x12, 0x0D};
static const PixelPoint sButton0Size = {0x20, 0x16};
static const FloorPos sComeInTarget = {0, -0x4800};
static const PixelPoint sButton2Position = {0x12, 0x3D};
static const PixelPoint sButton1Size = {0x20, 0x16};
static const PixelPoint sButton1Position = {0x12, 0x25};
static const VecFx32 sCameraResetPosition = {0x87A7, 0x41D0, 0xAED8};
static const VecFx32 sSecondStartPosition = {0, 0, -0x8000};
static const VecFx32 sSecondResetPosition = {0, 0, -0x8000};
static const VecFx32 sCameraResetTarget = {0, 0x1999, 0};
static const VecFx32 sCameraStartPosition = {0x87A7, 0x41D0, 0xAED8};
static const VecFx32 sCameraStartTarget = {0, 0x1999, 0};
static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};

// The SDK's MATH_Rand16 on the game's generator
static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

static inline s16 StandAnimation(int kind) {
    return gStandAnimations[kind][0];
}

static inline s16 StandAnimationOf(int kind) {
    kind = gPersonalityKinds[kind];
    return gStandAnimations[kind][0];
}

static inline VecFx32 ZeroVec() {
    VecFx32 v = {0, 0, 0};
    return v;
}

static inline void SetCharacterPos(MakeUpCharacter *character, VecFx32 pos) {
    character->mPos.x = pos.x;
    character->mPos.y = pos.y;
    character->mPos.z = pos.z;
}

static inline void SetWalkTarget(MakeUpCharacter *character, FloorPos pos) {
    character->mWalkTarget.x = pos.x;
    character->mWalkTarget.z = pos.z;
}

static inline void SetTurnTarget(MakeUpCharacter *character, FloorPos pos) {
    character->mTurnTarget.x = pos.x;
    character->mTurnTarget.z = pos.z;
}

void MakeUpScene::StartLoading() {
    GetCharacterIndices(&mCharacterIndices[0], &mCharacterIndices[1]);
    GameData_ReadCharacter(&gGameData, mCharacterIndices[0], &mCharacters[0]);
    mCharacters[0].mIndex = mCharacterIndices[0];
    mCharacters[0].Character::SetArchive("3H0");
    GameData_ReadCharacter(&gGameData, mCharacterIndices[1], &mCharacters[1]);
    mCharacters[1].mIndex = mCharacterIndices[1];
    mCharacters[1].Character::SetArchive("3H1");
    mRoom.mId = mCharacters[0].mInterior;
    mRoom.Mount("CO0", 0);
    mRoom.MountAll("RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SMT", "Scn/Scene/SceneMiiTouch_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    gSoundManager.StartSeqIfNotPlaying(0, 0xB);
    gSoundManager.SetSeqTrackMuteOnly(0, 0x1BC, TRUE);
    gSoundManager.LoadGroup(0x4C);
}

void MakeUpScene::Init() {
    mCharOffset = 0x100;
    mUnknown14 = 0x190;
    mDrawState = 0;
    mDrawFrames = 0;
    mState = STATE_WAIT;
    mStep = 0;
    mFrame = 0;
    mEntered = TRUE;
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
    mMadeUp = DecideMakeUp(&mCharacters[0], &mCharacters[1]);
}

// The touch screen: the window for the messages over a background, and four buttons that are shown disabled
void MakeUpScene::InitLowerScreen() {
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

    Sprite_Init(&mButton15EC8, 0, 4, 0x80, 0x60, TRUE);
    SpriteButton_SetSequences(&mButton15EC8, 4, 6);
    mButton15EC8.SetPositionFromVec(sButton0Position.x << FX32_SHIFT, sButton0Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton15EC8, sButton0Size.x, sButton0Size.y);
    SpriteButton_Reset(&mButton15EC8);
    SpriteButton_SetKind(&mButton15EC8, 4);
    mButton15EC8.mOverwritePalette = 1;
    mButton15EC8.mUnknownDC = 1;
    mButton15EC8.mSounds.mSounds[0] = 10;
    mButton15EC8.mSounds.mSounds[2] = 0;

    Sprite_Init(&mButton15FBC, 0, 7, 0x80, 0x60, TRUE);
    SpriteButton_SetSequences(&mButton15FBC, 7, 8);
    mButton15FBC.SetPositionFromVec(sButton1Position.x << FX32_SHIFT, sButton1Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton15FBC, sButton1Size.x, sButton1Size.y);
    SpriteButton_Reset(&mButton15FBC);
    SpriteButton_SetKind(&mButton15FBC, 4);
    mButton15FBC.mOverwritePalette = 1;
    mButton15FBC.mUnknownDC = 1;
    mButton15FBC.mSounds.mSounds[0] = 10;
    mButton15FBC.mSounds.mSounds[2] = 0;

    Sprite_Init(&mButton160B0, 0, 2, 0x80, 0x60, TRUE);
    SpriteButton_SetSequences(&mButton160B0, 2, 3);
    mButton160B0.SetPositionFromVec(sButton2Position.x << FX32_SHIFT, sButton2Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton160B0, sButton2Size.x, sButton2Size.y);
    SpriteButton_Reset(&mButton160B0);
    SpriteButton_SetKind(&mButton160B0, 4);
    mButton160B0.mOverwritePalette = 1;
    mButton160B0.mUnknownDC = 1;
    mButton160B0.mSounds.mSounds[0] = 10;
    mButton160B0.mSounds.mSounds[2] = 0;

    Sprite_Init(&mButton161A4, 0, 0, 0x80, 0x60, TRUE);
    SpriteButton_SetSequences(&mButton161A4, 0, 1);
    mButton161A4.SetPositionFromVec(sButton3Position.x << FX32_SHIFT, sButton3Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton161A4, sButton3Size.x, sButton3Size.y);
    SpriteButton_Reset(&mButton161A4);
    SpriteButton_SetKind(&mButton161A4, 4);
    mButton161A4.mOverwritePalette = 1;
    mButton161A4.mUnknownDC = 1;
    mButton161A4.mSounds.mSounds[0] = 13;
    mButton161A4.mSounds.mSounds[2] = 0;

    SpriteButton_SetDisabled(&mButton15EC8, TRUE);
    SpriteButton_SetDisabled(&mButton15FBC, TRUE);
    SpriteButton_SetDisabled(&mButton160B0, TRUE);
    SpriteButton_SetDisabled(&mButton161A4, TRUE);
}

// The top screen: the room, the camera, the lights, the head icons' particles and the two characters
void MakeUpScene::Init3d() {
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
    mRoom.SetState(2);
    void *file = ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa");
    ParticleSystem_Load(&mParticles, file, 3, 0x50, 0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    mHeadIcons[0].Load(&mParticles, 0);
    animations = ModelResources_BindArchive(gModelResources, 2, "3HA", 0);
    ModelResources_SetModel(gModelResources, 0, mCharacters[0].mModelPath);
    ModelResources_SetModel(gModelResources, 1, mCharacters[1].mModelPath);
    mCharacters[0].Character::Init(0, 0, 0);
    mCharacters[1].Character::Init(1, 0, 0);
    for (int i = 0; i < 2; i++) {
        MakeUpCharacter *character = &mCharacters[i];
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
        character->Character::SetAnimation(RESOURCE_BCA0, StandAnimationOf(mCharacters[i].mPersonality));
    }
    SetCharacterPos(&mCharacters[0], ZeroVec());
    SetCharacterPos(&mCharacters[1], sSecondStartPosition);
    mCharacters[0].Character::SetAngle(0x7000);
    mCharacters[1].Character::SetAngle(0x3800);
}

void MakeUpScene::Update() {
    mCamera.Update();
    mLights.Update();
    mWindow.Update();
    func_0207b138(mParticles.mManager);
    mCharacters[0].Update();
    mCharacters[1].Update();
    mRoom.Update();
    mHeadIcons[0].Update();
    UpdateState();
    mTtsPlayer.Update();
}

void MakeUpScene::Draw() {
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
    mRoom.Draw();
}

void MakeUpScene::Exit() {
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

// Which two saved characters meet: coming from scene 0x40, the ones it left; otherwise two of the saved characters,
// chosen by shuffling them
void MakeUpScene::GetCharacterIndices(int *first, int *second) {
    int list[100];

    if (gSceneManager.mPreviousSceneId == 0x40) {
        *first = gSceneMemory.mCharacters[0];
        *second = gSceneMemory.mCharacters[1];
        return;
    }
    int count = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            list[count] = i;
            count++;
        }
    }
    for (int i = 0; i < 200; i++) {
        u16 a = MATH_Rand16(&gRandom, count);
        u16 b = MATH_Rand16(&gRandom, count);
        int t = list[a];
        list[a] = list[b];
        list[b] = t;
    }
    *first = list[0];
    *second = list[1];
}

void MakeUpScene::UpdateState() {
    switch (mState) {
    case STATE_WAIT:
        if (mEntered) {
            mEntered = FALSE;
            SetCharacterPos(&mCharacters[0], ZeroVec());
            SetCharacterPos(&mCharacters[1], sSecondResetPosition);
            mCharacters[0].SetState(MakeUpCharacter::STATE_STAND);
            mCharacters[1].SetState(MakeUpCharacter::STATE_STAND);
            mCharacters[0].SetHeadIcon(0);
        } else if (mFrame >= 80) {
            NextState();
        }
        break;
    case STATE_KNOCK:
        if (mEntered) {
            mEntered = FALSE;
            gSoundManager.StartSeqArc(1, 0x1D, 0x10);
        } else if (mFrame >= 60) {
            NextState();
        }
        break;
    case STATE_OPEN_DOOR:
        if (mEntered) {
            mEntered = FALSE;
            mRoom.mDoor.SetMotion(1);
            mCamera.SetState(MakeUpCamera::STATE_FOLLOW_BOTH);
        } else if (mRoom.mDoor.mMotion == 0) {
            NextState();
        }
        break;
    case STATE_COME_IN:
        if (mEntered) {
            mEntered = FALSE;
            SetWalkTarget(&mCharacters[1], sComeInTarget);
            mCharacters[1].SetState(MakeUpCharacter::STATE_WALK);
        } else if (mCharacters[1].mDone) {
            mCharacters[1].SetState(MakeUpCharacter::STATE_STAND);
            NextState();
        }
        break;
    case STATE_CLOSE_DOOR:
        if (mEntered) {
            mEntered = FALSE;
            mRoom.mDoor.SetMotion(2);
        } else if (mRoom.mDoor.mMotion == 0) {
            NextState();
        }
        break;
    case STATE_PAUSE:
        if (mEntered) {
            mEntered = FALSE;
        } else if (mFrame >= 30) {
            NextState();
        }
        // fallthrough
    case STATE_BOW:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[1].SetState(MakeUpCharacter::STATE_BOW);
        } else if (mCharacters[1].mDone) {
            NextState();
        }
        break;
    case STATE_PAUSE_2:
        if (mEntered) {
            mEntered = FALSE;
        } else if (mFrame >= 70) {
            NextState();
        }
        // fallthrough
    case STATE_FACE:
        if (mEntered) {
            mEntered = FALSE;
            SetTurnTarget(&mCharacters[0], mCharacters[1].GetFloorPos());
            mCharacters[0].SetState(MakeUpCharacter::STATE_TURN);
        } else if (mCharacters[0].mDone) {
            mCharacters[0].SetState(MakeUpCharacter::STATE_STAND);
            NextState();
        }
        break;
    case STATE_PAUSE_3:
        if (mEntered) {
            mEntered = FALSE;
        } else if (mFrame >= 20) {
            NextState();
        }
        // fallthrough
    case STATE_WALK_UP:
        if (mEntered) {
            mEntered = FALSE;
            SetWalkTarget(&mCharacters[1], sWalkUpTarget);
            mCharacters[1].SetState(MakeUpCharacter::STATE_WALK);
        } else if (mCharacters[1].mDone) {
            NextState();
        }
        break;
    case STATE_STAND:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[1].SetState(MakeUpCharacter::STATE_STAND);
        } else if (mFrame >= 40) {
            NextState();
        }
        break;
    case STATE_APOLOGIZE:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[1].SetState(MakeUpCharacter::STATE_APOLOGIZE);
        } else if (mCharacters[1].mDone) {
            NextState();
        }
        break;
    case STATE_THINK:
        if (mEntered) {
            mEntered = FALSE;
            BOOL wait = MATH_Rand16(&gRandom, 5) != 0;
            mWait = wait * (MATH_Rand16(&gRandom, 100) + 100) + 30;
        } else if (mFrame >= mWait) {
            if (mMadeUp) {
                SetState(STATE_ACCEPT);
            } else {
                SetState(STATE_REFUSE);
            }
        }
        break;
    case STATE_ACCEPT:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].HideHeadIcon();
        } else {
            switch (mStep) {
            case 0:
                if (mFrame >= 50) {
                    mCharacters[0].SetState(MakeUpCharacter::STATE_ACCEPT);
                    mStep++;
                }
                break;
            case 1:
                if (mCharacters[0].mDone) {
                    SetState(STATE_PAUSE_4);
                }
                break;
            }
        }
        break;
    case STATE_REFUSE:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetState(MakeUpCharacter::STATE_REFUSE);
        } else if (mCharacters[0].mDone) {
            SetState(STATE_PAUSE_4);
        }
        break;
    case STATE_PAUSE_4:
        if (mEntered) {
            mEntered = FALSE;
        } else if (mFrame >= 30) {
            NextState();
        }
        break;
    case STATE_LEAVE:
        if (mEntered) {
            mEntered = FALSE;
        } else if (gSceneManager.mPreviousSceneId == 0x40) {
            MI_CpuFill8(gSceneScratch, 0, 100);
            u8 *scratch = gSceneScratch;
            if (mMadeUp) {
                scratch[0] = 0;
                ApplyResult(mMadeUp);
            } else {
                scratch[0] = 1;
                ApplyResult(mMadeUp);
            }
            gSceneMemory.mCharacters[0] = mCharacterIndices[1];
            gSceneMemory.mCharacters[1] = mCharacterIndices[0];
            gSceneManager.RequestScene(0x40);
        } else {
            gSceneManager.RequestScene(0);
        }
        break;
    }
    mFrame++;
}

void MakeUpScene::NextState() {
    mState++;
    mStep = 0;
    mFrame = 0;
    mEntered = TRUE;
}

void MakeUpScene::SetState(int state) {
    mState = state;
    mStep = 0;
    mFrame = 0;
    mEntered = TRUE;
}

// Whether the two make up: by a chance of a quarter of how much they like each other, plus 25, in 100
BOOL MakeUpScene::DecideMakeUp(MakeUpCharacter *first, MakeUpCharacter *second) {
    int chance = (second->mRelationMeter[first->mIndex] + first->mRelationMeter[second->mIndex]) / 4 + 25;
    return chance > MATH_Rand16(&gRandom, 100);
}

// What making up, or not, does to the two: friends, lovers or spouses again; or friends no more, or lovers and
// spouses heartbroken
void MakeUpScene::ApplyResult(BOOL madeUp) {
    s64 now = Clock_GetSeconds(gGameClock);
    if (mCharacters[0].mTroubleCategory == 3 && mCharacters[0].mTroubleId == 4) {
        mCharacters[0].mTroubleEnd =
            now + data_02114d78[mCharacters[0].mTroubleCategory][mCharacters[0].mTroubleId].cooldownMinutes * 60;
        mCharacters[0].mTroubleState = 3;
    }
    if (mCharacters[1].mTroubleCategory == 3 && mCharacters[1].mTroubleId == 4) {
        mCharacters[1].mTroubleEnd =
            now + data_02114d78[mCharacters[1].mTroubleCategory][mCharacters[1].mTroubleId].cooldownMinutes * 60;
        mCharacters[1].mTroubleState = 3;
    }
    if (madeUp) {
        u8 rank = RELATION_FRIENDS;
        BOOL fighting = FALSE;
        switch (mCharacters[0].mRelationRank[mCharacters[1].mIndex]) {
        case RELATION_FRIENDS_FIGHTING:
            fighting = TRUE;
            break;
        case RELATION_LOVERS_FIGHTING:
            fighting = TRUE;
            rank = RELATION_LOVERS;
            break;
        case RELATION_SPOUSES_FIGHTING:
            fighting = TRUE;
            rank = RELATION_SPOUSES;
            break;
        }
        if (fighting) {
            mCharacters[0].SetRelation(mCharacters[1].mIndex, rank, 100);
            mCharacters[1].SetRelation(mCharacters[0].mIndex, rank, 100);
        }
        mCharacters[0].mMood = 0;
        mCharacters[1].mMood = 0;
        return;
    }
    switch (mCharacters[0].mRelationRank[mCharacters[1].mIndex]) {
    case RELATION_FRIENDS_FIGHTING:
        mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_NONE, 100);
        mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_NONE, 100);
        mCharacters[0].mMood = 0;
        mCharacters[0].ClearTrouble();
        break;
    case RELATION_LOVERS_FIGHTING: {
        s64 time = Clock_GetSeconds(gGameClock);
        mCharacters[0].ClearTrouble();
        mCharacters[0].mTroubleCategory = 3;
        mCharacters[0].mTroubleId = 5;
        mCharacters[0].mTroubleState = 1;
        mCharacters[0].mTroubleTarget = mCharacters[1].mIndex;
        mCharacters[0].mTroubleEnd =
            time + data_02114d78[mCharacters[0].mTroubleCategory][mCharacters[0].mTroubleId].minutes * 60;
        mCharacters[0].mMood = 0;
        break;
    }
    case RELATION_SPOUSES_FIGHTING: {
        s64 time = Clock_GetSeconds(gGameClock);
        mCharacters[0].ClearTrouble();
        mCharacters[0].mTroubleCategory = 3;
        mCharacters[0].mTroubleId = 6;
        mCharacters[0].mTroubleState = 1;
        mCharacters[0].mTroubleTarget = mCharacters[1].mIndex;
        mCharacters[0].mTroubleEnd =
            time + data_02114d78[mCharacters[0].mTroubleCategory][mCharacters[0].mTroubleId].minutes * 60;
        mCharacters[0].mMood = 0;
        break;
    }
    }
}

// --- The characters ---------------------------------------------------------------------------------------------

void MakeUpCharacter::Init(MakeUpContext *context) {
    mContext = context;
    mTtsPlayer = context->mTtsPlayer;
    mCurrentState = mState = STATE_STAND;
    mLipSync = FALSE;
}

void MakeUpCharacter::LoadHeadIcon(ParticleSystem *particles) {
    mHeadIcon.Load(particles, 0);
}

void MakeUpCharacter::Update() {
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

void MakeUpCharacter::UpdateState() {
    switch (mCurrentState = mState) {
    case STATE_STAND:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
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
            SetTargetFloorPos(mWalkTarget);
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
            SetTargetFloorPos(mTurnTarget);
        } else if (!mDone) {
            if (Character::TurnToWalkTarget(0x280, 0)) {
                mDone = TRUE;
            }
            Character::MoveAlongAngle();
            Character::PlaySoundOnce(1, 0, 0x1A);
        }
        break;
    case STATE_BOW:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x39);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
        } else if (!mDone) {
            switch (mStep) {
            case 0:
            if (mFrame == GetAnimationFrames() / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 4);
            }
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
                GetAnimation()->mPlayOnce = FALSE;
                mDone = TRUE;
            }
            break;
            }
        }
        break;
    case STATE_BOW_DEEP:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x3A);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
        } else if (!mDone) {
            switch (mStep) {
            case 0:
            if (mFrame == GetAnimationFrames() / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 2);
            }
            if (GetAnimation()->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
                GetAnimation()->mPlayOnce = FALSE;
                mDone = TRUE;
            }
            break;
            }
        }
        break;
    case STATE_SULK:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x55);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(0x15);
            mWait = Random(50) + 10;
        } else if (!mDone) {
            if (mFrame % GetAnimationFrames() == 0) {
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
            Character::BlendToAnimation(RESOURCE_BCA0, 0x3A);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25B, 0); // "I am sorry about the other day"
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
    case STATE_ACCEPT:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x3A);
            GetAnimation()->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25D, 0); // "I am sorry too"
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
    case STATE_REFUSE:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mPersonality]][0]);
            CharacterBase::SetFace(8);
            SetVoiceOf(mTtsPlayer);
            mLipSync = TRUE;
            Speak(0, 0x25F, 0); // "I will not forgive you"
        } else if (!mDone) {
            if (mTtsPlayer->IsEngineIdle()) {
                mLipSync = FALSE;
                mDone = TRUE;
            }
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
    PlaceHeadIcon();
    mFrame++;
}

void MakeUpCharacter::SetState(int state) {
    mState = state;
    mStep = FALSE;
    mEntered = TRUE;
    mDone = FALSE;
    mFrame = 0;
    mUnknown2B30 = 0;
    mWait = 0;
    mAnimDone = FALSE;
}

// Shows a message of the trouble messages (file 0) or of the other messages (file 1) in the window, with its
// arguments, and speaks its pronunciation, which is the message after it
void MakeUpCharacter::Speak(int file, u32 message, int count, int *types, int *values, int mode) {
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

void MakeUpCharacter::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void MakeUpCharacter::Speak(int file, u32 message, int mode) {
    Speak(file, message, ARG_NONE, 0, mode);
}

// Puts the head icon over the head, turned with it
void MakeUpCharacter::PlaceHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mHeadYawNow;
    MtxFx43 *head = &mJointMatrices[0];
    mHeadIcon.SetPos(head->_30, head->_31, head->_32);
}

void MakeUpCharacter::SetHeadIcon(s16 id) {
    mHeadIcon.Show();
    mHeadIcon.mIconId = id;
    mHeadIcon.Reload();
    mHeadIcon.SetMode(3);
}

void MakeUpCharacter::HideHeadIcon() {
    mHeadIcon.SetMode(1);
}

// --- The camera -------------------------------------------------------------------------------------------------

void MakeUpCamera::Init(MakeUpCharacter *first, MakeUpCharacter *second) {
    mCamera = &gCamera;
    Camera_Reset(mCamera);
    mCamera->SetPosition(sCameraStartPosition);
    mCamera->SetTarget(sCameraStartTarget);
    mCamera->mAngleX = 0;
    mCamera->mAngleY = 0;
    mCamera->mAngleZ = 0;
    Camera_UpdateEye(mCamera);
    mFirst = first;
    mSecond = second;
    CopyVec(&mPosition, &mCamera->mPosition);
    CopyVec(&mTarget, &mCamera->mTarget);
    mState = STATE_FOLLOW_FIRST;
}

// Puts the camera 14 in front of the target, in the direction of its starting place
void MakeUpCamera::Reset(int previousScene) {
    SetPosition(sCameraResetPosition);
    SetTarget(sCameraResetTarget);
    FloorPos direction = NormalizeXZ(mPosition.x - mTarget.x, mPosition.z - mTarget.z);
    mPosition.x = mTarget.x + direction.x * 14;
    mPosition.z = mTarget.z + direction.z * 14;
    mPosition.y = 0x41D0;
    mTarget.y = 0x1999;
    CopyVec(&mCamera->mPosition, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    Camera_UpdateEye(mCamera);
}

// Follows the first character, or the middle between the two, keeping the distance and the height
void MakeUpCamera::Update() {
    switch (mState) {
    case STATE_STILL:
        return;
    case STATE_FOLLOW_FIRST: {
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
    case STATE_FOLLOW_BOTH: {
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
    default:
        return;
    }
}

void MakeUpCamera::SetState(int state) {
    mState = state;
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 MakeUpCamera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = Shift(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

// --- The lights -------------------------------------------------------------------------------------------------

void MakeUpLights::Init(Camera *camera, Room *room) {
    VecFx32 view;
    mState = 0;
    mCamera = camera;
    mRoom = room;
    VEC_Subtract(&mCamera->mPosition, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        mLights[i] = &gLights[i];
        Light_Reset(&gLights[i]);
        mLights[i]->mColor = gInteriorTable[mRoom->mId].mLightColor;
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void MakeUpLights::Update() {
    VecFx32 view;
    switch (mState) {
    case 0: {
        VEC_Subtract(&mCamera->mEye, &mCamera->mTarget, &view);
        u16 angle = FX_Atan2Idx(view.x, view.z);
        for (int i = 0; i < 2; i++) {
            Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
        }
        break;
    }
    case 1:
        SetColor(mRoom->GetLightColor());
        mState = 0;
        break;
    }
}

void MakeUpLights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// Not in the ROM: the link left it out, as nothing calls it. Something of this file read data_ov102_0214cd64, or it
// would not be in the ROM; what it did with it is not known, and this stands in for it.
extern "C" const PixelPoint *Ov102_UnusedData(void) {
    return &data_ov102_0214cd64;
}
