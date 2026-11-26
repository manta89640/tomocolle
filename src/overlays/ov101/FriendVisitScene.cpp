#include "FriendVisitScene.h"
#include "MainProgram.h"

// The touch screen's four buttons, which the scene sets up and disables
static const ScreenPoint sUnusedPoint = {128, 25};
static const FloorPos sDoorInside = {0, -0x4800};      // Where the visitor stops once in
static const ScreenPoint sButtonPosition = {18, 13};
static const FloorPos sMeetingPoint = {0, -0x2000};    // Where the visitor goes to talk
static const ScreenPoint sButton161A8Position = {242, 13};
static const ScreenPoint sButtonSize = {32, 22};
static const ScreenPoint sButton15FC0Size = {32, 22};
static const ScreenPoint sButton160B4Size = {32, 22};
static const ScreenPoint sButton15FC0Position = {18, 37};
static const ScreenPoint sButton160B4Position = {18, 61};
static const ScreenPoint sButton161A8Size = {24, 22};

static const VecFx32 sCameraPosition = {0x87A7, 0x41D0, 0xAED8};
static const VecFx32 sCameraStartPosition = {0x87A7, 0x41D0, 0xAED8};
static const VecFx32 sCameraStartTarget = {0, 0x1999, 0};
static const VecFx32 sCameraTarget = {0, 0x1999, 0};
static const VecFx32 sCameraPositionFromTouch = {-0xAED8, 0x41D0, -0x87A7};
static const VecFx32 sCameraTargetFromTouch = {0, 0x1999, 0};
static const VecFx32 sVisitorStart = {0, 0, -0x8000}; // Behind the door
static const VecFx32 sCameraPositionVisit = {0x87A7, 0x41D0, 0xAED8};
static const VecFx32 sVisitorStartSetOff = {0, 0, -0x8000};
static const VecFx32 sVisitorStartVisit = {0, 0, -0x8000};
static const VecFx32 sCameraTargetVisit = {0, 0x1999, 0};
static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};

static inline s16 StandAnimationOf(int kind) {
    kind = gPersonalityKinds[kind];
    return gStandAnimations[kind][0];
}

static inline s16 GetStandAnimation(FriendVisitCharacter *character) {
    return gStandAnimations[gPersonalityKinds[character->mPersonality]][0];
}

// --- The scene ----------------------------------------------------------------------------------------------------

void FriendVisitScene::StartLoading() {
    ChooseCharacters();
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

void FriendVisitScene::Init() {
    mCharOffset = 0x100;
    mUnknown14 = 0x190;
    mDrawState = 0;
    mDrawFrames = 0;
    mState = 0;
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
    SetSpeaker(0);
    mFriends = DrawFriendship(&mCharacters[0], &mCharacters[1]);
}

// The touch screen: the window for the messages over a background, and four buttons, all disabled
void FriendVisitScene::InitLowerScreen() {
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

    Sprite_Init(&mButton, 0, 4, 128, 96, TRUE);
    SpriteButton_SetSequences(&mButton, 4, 6);
    mButton.SetPositionFromVec(sButtonPosition.x << FX32_SHIFT, sButtonPosition.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton, sButtonSize.x, sButtonSize.y);
    SpriteButton_Reset(&mButton);
    SpriteButton_SetKind(&mButton, 4);
    mButton.mOverwritePalette = 1;
    mButton.mUnknownDC = 1;
    mButton.mSounds.mSounds[0] = 10;
    mButton.mSounds.mSounds[2] = 0;

    Sprite_Init(&mButton15FC0, 0, 7, 128, 96, TRUE);
    SpriteButton_SetSequences(&mButton15FC0, 7, 8);
    mButton15FC0.SetPositionFromVec(sButton15FC0Position.x << FX32_SHIFT, sButton15FC0Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton15FC0, sButton15FC0Size.x, sButton15FC0Size.y);
    SpriteButton_Reset(&mButton15FC0);
    SpriteButton_SetKind(&mButton15FC0, 4);
    mButton15FC0.mOverwritePalette = 1;
    mButton15FC0.mUnknownDC = 1;
    mButton15FC0.mSounds.mSounds[0] = 10;
    mButton15FC0.mSounds.mSounds[2] = 0;

    Sprite_Init(&mButton160B4, 0, 2, 128, 96, TRUE);
    SpriteButton_SetSequences(&mButton160B4, 2, 3);
    mButton160B4.SetPositionFromVec(sButton160B4Position.x << FX32_SHIFT, sButton160B4Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton160B4, sButton160B4Size.x, sButton160B4Size.y);
    SpriteButton_Reset(&mButton160B4);
    SpriteButton_SetKind(&mButton160B4, 4);
    mButton160B4.mOverwritePalette = 1;
    mButton160B4.mUnknownDC = 1;
    mButton160B4.mSounds.mSounds[0] = 10;
    mButton160B4.mSounds.mSounds[2] = 0;

    Sprite_Init(&mButton161A8, 0, 0, 128, 96, TRUE);
    SpriteButton_SetSequences(&mButton161A8, 0, 1);
    mButton161A8.SetPositionFromVec(sButton161A8Position.x << FX32_SHIFT, sButton161A8Position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButton161A8, sButton161A8Size.x, sButton161A8Size.y);
    SpriteButton_Reset(&mButton161A8);
    SpriteButton_SetKind(&mButton161A8, 4);
    mButton161A8.mOverwritePalette = 1;
    mButton161A8.mUnknownDC = 1;
    mButton161A8.mSounds.mSounds[0] = 13;
    mButton161A8.mSounds.mSounds[2] = 0;

    SpriteButton_SetDisabled(&mButton, TRUE);
    SpriteButton_SetDisabled(&mButton15FC0, TRUE);
    SpriteButton_SetDisabled(&mButton160B4, TRUE);
    SpriteButton_SetDisabled(&mButton161A8, TRUE);
}

// The top screen: the room, the camera, the lights, the particles and the two characters
void FriendVisitScene::Init3d() {
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
    ParticleSystem_Load(&mParticles, ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa"), 3, 0x50, 0x15,
                        0x16, 0x1E);
    func_02080ee4(&mParticles);
    mHeadIcons[0].Load(&mParticles, 0);
    animations = ModelResources_BindArchive(gModelResources, 2, "3HA", 0);
    ModelResources_SetModel(gModelResources, 0, mCharacters[0].mModelPath);
    ModelResources_SetModel(gModelResources, 1, mCharacters[1].mModelPath);
    mCharacters[0].Character::Init(0, 0, 0);
    mCharacters[1].Character::Init(1, 0, 0);
    VecFx32 positions[2];
    VecFx32 zero = {0, 0, 0};
    VecFx32 start = sVisitorStart;
    positions[0] = zero;
    positions[1] = start;
    for (int i = 0; i < 2; i++) {
        FriendVisitCharacter *character = &mCharacters[i];
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
        mCharacters[i].SetPos(positions[i]);
        character->Character::SetAngle(0x3800);
    }
    switch (mMode) {
    case MODE_SET_OFF:
        mCharacters[0].FaceCamera();
        mCharacters[0].LookAtCamera();
        break;
    case MODE_VISIT:
        break;
    }
}

void FriendVisitScene::Update() {
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

void FriendVisitScene::Draw() {
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

void FriendVisitScene::Exit() {
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

// Which saved characters the two are, and the mode: from the touch scene and from this scene, the ones those left
// in gSceneMemory; otherwise the first two
void FriendVisitScene::ChooseCharacters() {
    if (gSceneManager.mPreviousSceneId == 0x40) {
        mCharacterIndices[0] = gSceneMemory.mCharacters[0];
        mCharacterIndices[1] = gSceneMemory.mCharacters[1];
        mMode = MODE_SET_OFF;
    } else if (gSceneManager.mPreviousSceneId == 0xAF) {
        mCharacterIndices[0] = gSceneMemory.mCharacters[0];
        mCharacterIndices[1] = gSceneMemory.mCharacters[1];
        mMode = MODE_VISIT;
    } else {
        mCharacterIndices[0] = 0;
        mCharacterIndices[1] = 1;
        mMode = MODE_VISIT;
    }
}

// Gives the speech engine one character's voice, and moves that character's mouth with it
void FriendVisitScene::SetSpeaker(int slot) {
    if (slot == 0) {
        mCharacters[0].SetVoiceOf(&mTtsPlayer);
        mCharacters[0].mLipSync = TRUE;
        mCharacters[1].mLipSync = FALSE;
    } else {
        mCharacters[1].SetVoiceOf(&mTtsPlayer);
        mCharacters[0].mLipSync = FALSE;
        mCharacters[1].mLipSync = TRUE;
    }
}

void FriendVisitScene::UpdateState() {
    switch (mMode) {
    case MODE_SET_OFF:
        UpdateSetOff();
        break;
    case MODE_VISIT:
        UpdateVisit();
        break;
    }
}

// The resident says it will go and talk, and the scene starts again as the visit, the two swapped
void FriendVisitScene::UpdateSetOff() {
    switch (mState) {
    case 0:
        if (mEntered) {
            mEntered = FALSE;
            VecFx32 zero = {0, 0, 0};
            mCharacters[0].SetPos(zero);
            mCharacters[1].SetPos(sVisitorStartSetOff);
            mCharacters[0].SetState(FriendVisitCharacter::STATE_SET_OFF);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_STAND);
        } else if (mCharacters[0].mDone) {
            gSceneManager.RequestScene(0xAF);
            gSceneMemory.mCharacters[0] = gSceneMemory.mCharacters[1];
            gSceneMemory.mCharacters[1] = mCharacters[0].mIndex;
        }
        break;
    }
}

static inline void SetWalkTarget(FriendVisitCharacter *character, FloorPos target) {
    character->mWalkTarget = target;
}

static inline VecFx32 ZeroVec() {
    VecFx32 v = {0, 0, 0};
    return v;
}

static inline void SetTurnTarget(FriendVisitCharacter *c, FloorPos t) {
    c->mTurnTarget = t;
}

// The visit: the second character rings, comes in and talks with the first
void FriendVisitScene::UpdateVisit() {
    switch (mState) {
    case 0:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetPos(ZeroVec());
            mCharacters[1].SetPos(sVisitorStartVisit);
            mCharacters[0].SetState(FriendVisitCharacter::STATE_STAND);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_STAND);
        } else if (mFrame >= 80) {
            NextState();
        }
        break;
    case 1: // The bell
        if (mEntered) {
            mEntered = FALSE;
            gSoundManager.StartSeqArc(1, 0x1D, 0x10);
        } else if (mFrame >= 60) {
            NextState();
        }
        break;
    case 2: // The door opens, the camera takes in both
        if (mEntered) {
            mEntered = FALSE;
            mRoom.mDoor.SetMotion(RoomDoor::MOTION_OPEN);
            mCamera.SetState(2);
        } else if (mRoom.mDoor.mMotion == RoomDoor::MOTION_NONE) {
            NextState();
        }
        break;
    case 3: // The host turns to the visitor
        if (mEntered) {
            mEntered = FALSE;
            SetTurnTarget(&mCharacters[0], mCharacters[1].GetFloorPos());
            mCharacters[0].SetState(FriendVisitCharacter::STATE_TURN);
        } else if (mCharacters[0].mDone) {
            mCharacters[0].SetState(FriendVisitCharacter::STATE_STAND);
            NextState();
        }
        break;
    case 4: // The visitor comes in
        if (mEntered) {
            mEntered = FALSE;
            SetWalkTarget(&mCharacters[1], sDoorInside);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_WALK_IN);
        } else if (mCharacters[1].mDone) {
            mCharacters[1].SetState(FriendVisitCharacter::STATE_STAND);
            NextState();
        }
        break;
    case 5: // The door closes
        if (mEntered) {
            mEntered = FALSE;
            mRoom.mDoor.SetMotion(RoomDoor::MOTION_CLOSE);
        } else if (mRoom.mDoor.mMotion == RoomDoor::MOTION_NONE) {
            NextState();
        }
        break;
    case 6:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetState(FriendVisitCharacter::STATE_BOW);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_BOW);
        } else if (mCharacters[0].mDone && mCharacters[1].mDone) {
            NextState();
        }
        break;
    case 7: // The visitor goes to the host
        if (mEntered) {
            mEntered = FALSE;
            SetWalkTarget(&mCharacters[1], sMeetingPoint);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_WALK_IN);
        } else if (mCharacters[1].mDone) {
            NextState();
        }
        break;
    case 8: // They talk
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetState(FriendVisitCharacter::STATE_TALK);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_TALK);
            mWait = 100;
        } else if (mMannerMatched) {
            if (mFrame >= mWait) {
                SetState(9);
            }
        } else if (mFrame >= 200) {
            SetState(10);
        }
        break;
    case 9:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[1].SetState(FriendVisitCharacter::STATE_TALK2);
        } else if (mFrame >= 120) {
            SetState(10);
        }
        break;
    case 10:
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetState(FriendVisitCharacter::STATE_STOP_ICON);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_STOP_ICON);
        } else if (mFriends) {
            SetState(11);
        } else {
            SetState(13);
        }
        break;
    case 11: // They became friends: the icon between them
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetState(FriendVisitCharacter::STATE_BOW2);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_BOW2);
            VecFx32 first = mCharacters[0].GetJointPosition(CharacterModel::JOINT_BODY);
            VecFx32 second = mCharacters[1].GetJointPosition(CharacterModel::JOINT_BODY);
            VEC_Add(&first, &second, &first);
            first.x >>= 1;
            first.y >>= 1;
            first.z >>= 1;
            mHeadIcons[0].SetPos(first.x, first.y, first.z);
        } else {
            if (mFrame == 40) {
                mHeadIcons[0].Show();
                mHeadIcons[0].mIconId = 4;
                mHeadIcons[0].Reload();
                mHeadIcons[0].SetMode(2);
                gSoundManager.StartSeqArc(1, 0x26, 0);
            }
            if (mCharacters[0].mDone && mCharacters[1].mDone && mFrame >= 40) {
                NextState();
            }
        }
        break;
    case 12:
        if (mEntered) {
            mEntered = FALSE;
        } else if (mFrame >= 45) {
            SetState(14);
        }
        break;
    case 13: // They did not
        if (mEntered) {
            mEntered = FALSE;
            mCharacters[0].SetState(FriendVisitCharacter::STATE_STAND);
            mCharacters[1].SetState(FriendVisitCharacter::STATE_STAND);
        } else if (mFrame >= 90) {
            SetState(14);
        }
        break;
    case 14:
        if (mEntered) {
            mEntered = FALSE;
        } else if (mFrame >= 0) {
            u8 *const scratch = gSceneScratch;
            if (mFriends) {
                scratch[0] = 0;
                ApplyOutcome(mFriends);
            } else {
                scratch[0] = 1;
                ApplyOutcome(mFriends);
            }
            if (gSceneManager.mPreviousSceneId == 0xAF) {
                gSceneManager.RequestScene(0x40);
            } else {
                gSceneManager.RequestScene(0);
            }
        }
        break;
    }
    mFrame++;
}

void FriendVisitScene::NextState() {
    mState++;
    mFrame = 0;
    mEntered = TRUE;
}

void FriendVisitScene::SetState(int state) {
    mState = state;
    mFrame = 0;
    mEntered = TRUE;
}

// Whether the two become friends: a chance in a hundred of half the visitor's relation meter with the host, 30 more
// if the manner of talking suits the visitor's personality, 20 more for a trouble of kind 11; certain for a trouble
// of kind 4 about the host, and when either has no friend yet
BOOL FriendVisitScene::DrawFriendship(FriendVisitCharacter *host, FriendVisitCharacter *visitor) {
    int chance;
    BOOL matched;
    int hostFriends;
    int visitorFriends;

    chance = visitor->mRelationMeter[host->mIndex];
    matched = FALSE;
    mMannerMatched = FALSE;
    chance /= 2;
    switch (GetSceneMemory().mTalkAnswers[1]) {
    case 0:
        if (gPersonalityKinds[visitor->mPersonality] == 3) {
            matched = TRUE;
        }
        break;
    case 1:
        if (gPersonalityKinds[visitor->mPersonality] == 2) {
            matched = TRUE;
        }
        break;
    case 2:
        if (gPersonalityKinds[visitor->mPersonality] == 1) {
            matched = TRUE;
        }
        break;
    case 3:
        if (gPersonalityKinds[visitor->mPersonality] == 0) {
            matched = TRUE;
        }
        break;
    }
    if (matched) {
        mMannerMatched = TRUE;
        chance += 30;
    }
    int kind = GetTroubleKind(visitor->mTroubleCategory, visitor->mTroubleId);
    if (kind == 11) {
        chance += 20;
    }
    if (kind == 4 && visitor->mTroubleTarget == host->mIndex) {
        chance = 100;
    }
    hostFriends = 0;
    visitorFriends = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            if (host->mRelationRank[i] == RELATION_FRIENDS && i != host->mIndex) {
                hostFriends++;
            }
            if (visitor->mRelationRank[i] == RELATION_FRIENDS && i != visitor->mIndex) {
                visitorFriends++;
            }
        }
    }
    if (hostFriends <= 0 || visitorFriends <= 0) {
        chance = 100;
    }
    int draw = Rand(100);
    return chance > draw;
}

// Friends: the relation of both, and the visitor's trouble ends (kind 11) or is solved (kind 4 about the host).
// Otherwise two who were nothing to each other like each other a little more.
void FriendVisitScene::ApplyOutcome(BOOL friends) {
    if (friends) {
        mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_FRIENDS, 100);
        mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_FRIENDS, 100);
        if (!mCharacters[1].IsInTrouble()) {
            return;
        }
        int kind = GetTroubleKind(mCharacters[1].mTroubleCategory, mCharacters[1].mTroubleId);
        if (kind == 11) {
            mCharacters[1].EndTrouble(Clock_GetSeconds(gGameClock));
        } else if (kind == 4 && mCharacters[1].mTroubleTarget == mCharacters[0].mIndex) {
            mCharacters[1].ClearTrouble();
        }
    } else if (mCharacters[0].mRelationRank[mCharacters[1].mIndex] == RELATION_NONE &&
               mCharacters[1].mRelationRank[mCharacters[0].mIndex] == RELATION_NONE) {
        mCharacters[0].AddRelationMeter(mCharacters[1].mIndex, 40);
        mCharacters[1].AddRelationMeter(mCharacters[0].mIndex, 40);
    }
}

// --- The characters ---------------------------------------------------------------------------------------------


static inline s16 GetWalkAnimation(FriendVisitCharacter *character) {
    return gWalkAnimations[gPersonalityKinds[character->mPersonality]][0];
}

static inline ModelAnimation *GetAnimation(FriendVisitCharacter *character) {
    return AnimModel_FindAnimation(&character->mAnim, RESOURCE_BCA0, character->mAnimId);
}

// How many frames the current animation lasts
static inline int GetAnimationFrames(ModelAnimation *animation) {
    return (((NNSG3dResAnmCommon *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10;
}

static inline void SetTargetFloorPos(FriendVisitCharacter *character, FloorPos target) {
    character->mTarget.x = target.x;
    character->mTarget.z = target.z;
}

void FriendVisitCharacter::Init(FriendVisitContext *context) {
    mContext = context;
    mTtsPlayer = context->mTtsPlayer;
    mState = STATE_STAND;
    mCurrentState = STATE_STAND;
    mLipSync = FALSE;
}

void FriendVisitCharacter::LoadHeadIcon(ParticleSystem *particles) {
    mHeadIcon.Load(particles, 0);
}

void FriendVisitCharacter::Update() {
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

void FriendVisitCharacter::UpdateState() {
    switch (mCurrentState = mState) {
    case STATE_STAND:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, GetStandAnimation(this));
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
        }
        break;

    case STATE_WALK_IN:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, GetWalkAnimation(this));
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            Character::ResetSoundPhase(0);
            SetTargetFloorPos(this, mWalkTarget);
            break;
        }
        if (mDone) {
            break;
        }
        if (Character::WalkToTarget(Character::ScaleBySpeed(0x23D), 0x22, 4)) {
            mDone = TRUE;
        }
        Character::MoveAlongAngle();
        Character::PlaySoundOnce(1, 0, 0x1A);
        break;

    case STATE_TURN:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, GetWalkAnimation(this));
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(8);
            SetTargetFloorPos(this, mTurnTarget);
            break;
        }
        if (mDone) {
            break;
        }
        if (Character::TurnToWalkTarget(0x280, 0)) {
            mDone = TRUE;
        }
        Character::MoveAlongAngle();
        Character::PlaySoundOnce(1, 0, 0x1A);
        break;

    case STATE_BOW:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x39);
            GetAnimation(this)->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            if (mFrame == (GetAnimationFrames(GetAnimation(this)) + 1) / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 4);
            }
            if (GetAnimation(this)->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, GetStandAnimation(this));
                GetAnimation(this)->mPlayOnce = FALSE;
                mDone = TRUE;
            }
            break;
        }
        break;

    case STATE_BOW2:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x3A);
            GetAnimation(this)->mPlayOnce = TRUE;
            CharacterBase::SetFace(8);
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            if (mFrame == (GetAnimationFrames(GetAnimation(this)) + 1) / 2) {
                gSoundManager.StartSeqArc(1, 0x1F, 2);
            }
            if (GetAnimation(this)->mFinished) {
                Character::BlendToAnimation(RESOURCE_BCA0, GetStandAnimation(this));
                GetAnimation(this)->mPlayOnce = FALSE;
                mDone = TRUE;
            }
            break;
        }
        break;

    case STATE_TALK:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x55);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(0x15);
            mIconFrame = Rand(50) + 10;
            break;
        }
        if (mDone) {
            break;
        }
        if (mFrame % (GetAnimationFrames(GetAnimation(this)) + 1) == 0) {
            gSoundManager.StartSeqArc(1, 0x1A, 3);
        }
        if (mIconFrame == mFrame) {
            mHeadIcon.Show();
            mHeadIcon.mIconId = 0xD;
            mHeadIcon.Reload();
            mHeadIcon.SetMode(3);
        }
        break;

    case STATE_TALK2:
        if (mEntered) {
            mEntered = FALSE;
            Character::BlendToAnimation(RESOURCE_BCA0, 0x4A);
            Character::UpdateAnimSpeed();
            CharacterBase::SetFace(9);
            mHeadIcon.SetMode(1);
            mWait = Rand(40) + 40;
            break;
        }
        if (mDone) {
            break;
        }
        Character::PlaySoundAtAnimPhase(0, 1, 0x1A, 4, 2);
        if (mFrame >= mWait) {
            SetState(STATE_TALK);
        }
        break;

    case STATE_STOP_ICON:
        if (mEntered) {
            mEntered = FALSE;
            mHeadIcon.SetMode(1);
            mDone = TRUE;
        }
        break;

    case STATE_SET_OFF:
        if (mEntered) {
            mEntered = FALSE;
            CharacterBase::SetFace(8);
            Character::BlendToAnimation(RESOURCE_BCA0, GetStandAnimation(this));
            Character::UpdateAnimSpeed();
            GetAnimation(this)->mPlayOnce = FALSE;
            LookAtCamera();
            mFrame = 0;
            mStep = 0;
            break;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame >= mWait) {
                Speak(0, 0x166, 2); // "I see, I will go and talk to them a little"
                mStep++;
            }
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                mDone = TRUE;
            }
            break;
        }
        break;
    }
    PlaceHeadIcon();
    mFrame++;
}

void FriendVisitCharacter::SetState(int state) {
    mState = state;
    mStep = 0;
    mEntered = TRUE;
    mDone = FALSE;
    mFrame = 0;
}

static inline u16 AngleTo(FriendVisitCharacter *character, FloorPos target) {
    FloorPos delta = target - character->GetFloorPos();
    return FX_Atan2Idx(delta.z, delta.x);
}

void FriendVisitCharacter::FaceCamera() {
    FloorPos camera;
    camera.x = gCamera.mEye.x;
    camera.z = gCamera.mEye.z;
    Character::SetAngle(AngleTo(this, camera));
}

// Shows a message of the trouble messages (file 0) or of the other messages (file 1) in the window, with its
// arguments, and speaks its pronunciation, which is the message after it
void FriendVisitCharacter::Speak(int file, u32 message, int count, int *types, int *values, int mode) {
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

void FriendVisitCharacter::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void FriendVisitCharacter::Speak(int file, u32 message, int mode) {
    Speak(file, message, ARG_NONE, 0, mode);
}

// Turns the head towards a point, as far as the limits allow
void FriendVisitCharacter::LookAt(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
}

void FriendVisitCharacter::LookAtCamera() {
    LookAt(&gCamera.mEye, 0x2000, 0x3F00);
}

u16 FriendVisitCharacter::GetHeadPitchTo(VecFx32 *target) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(0, &matrix, &rotation);
    fx32 dz = target->z - mPos.z;
    fx32 dx = target->x - mPos.x;
    fx32 eyes = matrix._31 + 0xCCC;
    return -FX_Atan2Idx(target->y - eyes, SqrtXZ(dx, dz));
}

u16 FriendVisitCharacter::GetHeadYawTo(VecFx32 *target) {
    return mAngle - FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

// Keeps the head icon over the neck, turned with the head
void FriendVisitCharacter::PlaceHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mHeadYawNow;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcon.SetPos(matrices[JOINT_NECK]._30, matrices[JOINT_NECK]._31, matrices[JOINT_NECK]._32);
}

// --- The camera -------------------------------------------------------------------------------------------------

static inline void SetPosition(FriendVisitCamera *camera, VecFx32 position) {
    camera->mPosition.x = position.x;
    camera->mPosition.y = position.y;
    camera->mPosition.z = position.z;
}

static inline void SetTarget(FriendVisitCamera *camera, VecFx32 target) {
    camera->mTarget.x = target.x;
    camera->mTarget.y = target.y;
    camera->mTarget.z = target.z;
}

static inline void SetCameraPosition(Camera *camera, VecFx32 position) {
    camera->mPosition.x = position.x;
    camera->mPosition.y = position.y;
    camera->mPosition.z = position.z;
}

static inline void SetCameraTarget(Camera *camera, VecFx32 target) {
    camera->mTarget.x = target.x;
    camera->mTarget.y = target.y;
    camera->mTarget.z = target.z;
}

static inline VecFx32 GetPos(FriendVisitCharacter *character) {
    return character->mPos;
}

void FriendVisitCamera::Init(FriendVisitCharacter *first, FriendVisitCharacter *second) {
    mCamera = &gCamera;
    Camera_Reset(mCamera);
    SetCameraPosition(mCamera, sCameraStartPosition);
    SetCameraTarget(mCamera, sCameraStartTarget);
    mCamera->mAngleX = 0;
    mCamera->mAngleY = 0;
    mCamera->mAngleZ = 0;
    Camera_UpdateEye(mCamera);
    mFirst = first;
    mSecond = second;
    CopyVec(&mPosition, &mCamera->mPosition);
    CopyVec(&mTarget, &mCamera->mTarget);
    mState = 1;
}

// Puts the camera 14 in front of the target, in the direction that suits the scene it comes from
void FriendVisitCamera::Reset(int previousSceneId) {
    if (previousSceneId == 0x40) {
        SetPosition(this, sCameraPositionFromTouch);
        SetTarget(this, sCameraTargetFromTouch);
    } else if (previousSceneId == 0xAF) {
        SetPosition(this, sCameraPositionVisit);
        SetTarget(this, sCameraTargetVisit);
    } else {
        SetPosition(this, sCameraPosition);
        SetTarget(this, sCameraTarget);
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

// Follows the first character, or the middle between the two, keeping the distance and the height
void FriendVisitCamera::Update() {
    switch (mState) {
    case 0:
        return;
    case 1: {
        VecFx32 middle;
        VecFx32 position;
        middle = GetPos(mFirst);
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
        first = GetPos(mFirst);
        second = GetPos(mSecond);
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

void FriendVisitCamera::SetState(int state) {
    mState = state;
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 FriendVisitCamera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = Shift(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

// --- The lights -------------------------------------------------------------------------------------------------

void FriendVisitLights::Init(Camera *camera, Room *room) {
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

void FriendVisitLights::Update() {
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

void FriendVisitLights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// Not in the ROM: the link left it out, as nothing calls it. Something of this file read sUnusedPoint, or it would
// not be in the ROM; what it did with it is not known, and this stands in for it.
extern "C" const ScreenPoint *Ov101_UnusedData(void) {
    return &sUnusedPoint;
}
