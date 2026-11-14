#include "WeddingScene.h"

// The item the wedding gives: the wedding album ("kekkon arubamu" in the item messages)
#define ITEM_WEDDING_ALBUM 0xAA

// In this order, the compiler (which sorts a file's constants by size) puts them in the ROM's order
static const VecFx32 sCameraStartTarget = {0, 0x1999, 0};
static const VecFx32 sCameraStartPosition = {0, 0x41D0, 0xC000};
static const VecFx32 sFarAway = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
static const VecFx32 sCameraTarget = {0, 0x1999, 0};
static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};

void WeddingScene::StartLoading() {
    func_0202c264(&gGameData, GetCharacterIndex(0), &mCharacters[0]);
    mCharacters[0].mIndex = GetCharacterIndex(0);
    func_02091048(&mCharacters[0], "3H0");
    func_0202c264(&gGameData, GetCharacterIndex(1), &mCharacters[1]);
    mCharacters[1].mIndex = GetCharacterIndex(1);
    func_02091048(&mCharacters[1], "3H1");
    mRoom.mStyle = mCharacters[0].mInterior;
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TIM", "Msg/TmdItemMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SMT", "Scn/Scene/SceneMiiTouch_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_LZ.bin", 0);
    mGift.SetItem("3IM", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mGift.mArchiveName, mGift.mArchivePath, 0);

    if (gSceneManager.mPreviousSceneId == 0x40) {
        gSoundManager.StartSeqIfNotPlaying(0, 0x18);
        gSoundManager.StartSeqIfNotPlaying(1, 0x19);
        gSoundManager.MoveSeqVolume(1, 0, 0);
    } else if (gSceneManager.mPreviousSceneId == 0xC0) {
        gSoundManager.StartSeq(0, 0x17);
    } else {
        gSoundManager.StartSeq(0, 0x18);
        gSoundManager.StartSeq(1, 0x19);
        gSoundManager.MoveSeqVolume(1, 0, 0);
    }
    gSoundManager.LoadGroup(0x4D);
}

void WeddingScene::Init() {
    mCharOffset = 0x100;
    mUnknown14 = 0x190;
    mDrawState = 0;
    mDrawFrames = 0;
    if (gSceneManager.mPreviousSceneId == 0xC0) {
        mState = STATE_FAREWELL;
    } else {
        mState = STATE_ANNOUNCE;
    }
    mInventory.Load();
    mMessageFiles[0].Open("TCM:/tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("TTM:/tmd_trouble_msg_JP.bmg");
    mMessageFiles[2].Open("TIM:/tmd_item_msg_JP.bmg");
    mMessageFiles[3].Open("TEM:/tmd_etc_msg_JP.bmg");
    mTtsPlayer.InitEngine();
    mSpeechText.mMessageFile = &mMessageFiles[1];
    mCharOffset += mWindow.Init(mCharOffset, &mMessageFiles[1], 4, 0x10, &mTtsPlayer, FALSE);
    mWindow.mText.mHAlign = mWindow.mText.mVAlign = 1;
    mWindow.Hide();
    InitLowerScreen();
    Init3d();

    func_02027534(&data_021213b8, "SMT:/BG_up.ncl", 0x2F00, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "SMT:/BG_up.ncg", 0x2F00, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "SMT:/BG_common.nsc", 0x100, 0, 0, 0, 0, -1);
    data_0211d604.Set(0x1111);
    func_02028280(&data_02121834, 0x100, 3);
    func_02028108(&data_02121834, 0x2F00, 0);
    func_02028154(&data_02121834, 0x2F00, 0);
    func_02027290(&gObjResourceManager, "SMT:/Obj_up.nce", 0x1000, 1);
    func_02028f24(&gVramTransferQueue, "SMT:/Obj_up.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "SMT:/Obj_up.ncl", 0x1000, -1, -1, -1);
    data_0211d604.mUnknown44 = 0;
    data_0211d604.mUnknown44Changed = TRUE;
    func_020278f8(&data_021213b8, 0, 0x101);

    mContext.mSceneId = mSceneId;
    mContext.mCommonMessages = &mMessageFiles[0];
    mContext.mTroubleMessages = &mMessageFiles[1];
    mContext.mItemMessages = &mMessageFiles[2];
    mContext.mEtcMessages = &mMessageFiles[3];
    mContext.mWindow = &mWindow;
    mContext.mTtsPlayer = &mTtsPlayer;
    mContext.mSpeechText = &mSpeechText;
    mContext.mCamera = &mCamera;
    mContext.mLights = &mLights;
    mContext.mCharacters[0] = &mCharacters[0];
    mContext.mCharacters[1] = &mCharacters[1];
    mContext.mGift = &mGift;
    mCharacters[0].Init(&mContext);
    mCharacters[1].Init(&mContext);
    mCharacters[0].mPartner = &mCharacters[1];
    mCharacters[1].mPartner = &mCharacters[0];
    mCharacters[0].TurnHeadToCamera();
    mCharacters[1].TurnHeadToCamera();
    SetSpeaker(0);
}

// The touch screen: the window for the messages over a background
void WeddingScene::InitLowerScreen() {
    func_02027534(&data_021213b8, "SMT:/BG_down.ncl", 0x2F, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "SMT:/BG_down.ncg", 6, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "SMT:/Window_down.nsc", 2, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "SMT:/BG_dialogue.nsc", 4, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 4, 0);
    func_02028280(&data_02121834, 2, 1);
    func_02028280(&data_02121834, 1, 3);
    func_02023ee0(&data_0211d604, 4, 0x11, 8, 8);
}

// The top screen: the room, the camera, the lights and the two characters
void WeddingScene::Init3d() {
    VecFx32 points[2];
    void *animations;

    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    func_02028280(&data_02121834, 1, 3);
    func_0202a640(&data_02122764);
    mCamera.Init(&mCharacters[0], &mCharacters[1]);
    mCamera.Reset(gSceneManager.mPreviousSceneId);
    mCamera.GetStandingPoints(&points[0], &points[1]);
    mLights.Init(&data_02122764, &mRoom);
    func_020a5514(&mRoom, 3);
    func_020a5760(&mRoom, 4);
    func_020a4ab8(&mRoom.mParts, 2);
    ModelResources_SetModel(data_021223e4, 10, mGift.mModelPath);
    mGift.Load(10);
    mGift.SetPosition(sFarAway);
    mGift.mVisible = FALSE;
    animations = func_0202ad04(data_021223e4, 2, "3HA", 0);
    ModelResources_SetModel(data_021223e4, 0, mCharacters[0].mModelPath);
    ModelResources_SetModel(data_021223e4, 1, mCharacters[1].mModelPath);
    func_02090fdc(&mCharacters[0], 0, 0, 0);
    func_02090fdc(&mCharacters[1], 1, 0, 0);
    for (int i = 0; i < 2; i++) {
        WeddingCharacter *character = &mCharacters[i];
        func_0202c264(&gGameData, GetCharacterIndex(i), character);
        func_0208fefc(character);
        func_02091a88(character);
        func_02091a24(character);
        func_02091a6c(character);
        mCharacters[i].mPersonality = func_02098f2c(character);
        func_0209decc(character);
        func_020910d4(character);
        func_02029660(mCharacters[i].mModel.mResourceSet, RESOURCE_BCA0, animations);
        mCharacters[i].mModel.mUnknown4A4 = TRUE;
        Character_SetFace(character, 10);
        Character_SetAnimation(character, RESOURCE_BCA0,
                      ResourceSet_FindIndexByName(mCharacters[i].mModel.mResourceSet, RESOURCE_BCA0, "idle00"));
        mCharacters[i].SetPos(points[i]);
        character->FaceCamera();
        character->TurnHeadToCamera();
    }
}

void WeddingScene::Update() {
    mCamera.Update();
    mLights.Update();
    mWindow.Update();
    mCharacters[0].Update();
    mCharacters[1].Update();

    switch (mState) {
    case STATE_ANNOUNCE:
        mCharacters[0].SetState(WeddingCharacter::STATE_ANNOUNCE);
        mState++;
        break;
    case STATE_WAIT_ANNOUNCE:
        if (mCharacters[0].mDone) {
            SetSpeaker(1);
            mCharacters[1].SetState(WeddingCharacter::STATE_THANK);
            mCharacters[0].SetState(WeddingCharacter::STATE_LOOK_AT_PARTNER);
            mState++;
        }
        break;
    case STATE_WAIT_THANKS:
        if (mCharacters[1].mDone) {
            SetSpeaker(0);
            mCharacters[0].SetState(WeddingCharacter::STATE_PROMISE);
            mState++;
        }
        break;
    case STATE_WAIT_PROMISE:
        if (mCharacters[0].mDone) {
            wchar_t second[11];
            wchar_t first[11];
            MI_CpuClear16(first, sizeof(first));
            MI_CpuClear16(second, sizeof(second));
            func_0207a80c(&mCharacters[0], first);
            func_0207a80c(&mCharacters[1], second);
            mWindow.mText.mHAlign = 1;
            mWindow.SetStringArg(first, 0);
            mWindow.SetStringArg(second, 1);
            // "<first> and <second> got married!"
            mWindow.Show(0x256, 2, 120);
            mFrame = 0;
            mState++;
            gSoundManager.MoveSeqVolume(1, 0x7F, 60);
        }
        break;
    case STATE_SHOW_MARRIED:
        if (mFrame >= 260) {
            mWindow.mText.mHAlign = 0;
            SetSpeaker(1);
            mCharacters[1].SetState(WeddingCharacter::STATE_GIVE);
            mState++;
        }
        break;
    case STATE_WAIT_GIFT:
        if (mCharacters[1].mDone) {
            mState++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 120);
            gSoundManager.MoveSeqVolume(1, 0, 120);
        }
        break;
    case STATE_FADE_OUT: {
        int level = mFrame / 4;
        if (level > 16) {
            level = 16;
        } else if (level < 0) {
            level = 0;
        }
        func_02023f4c(&data_0211d604, 0x3F3F, level);
        if (mFrame >= 64) {
            mState++;
            mFrame = 0;
        }
        break;
    }
    case STATE_GIVE_ALBUM:
        if (mFrame >= 56) {
            mInventory.mItems[ITEM_WEDDING_ALBUM].Add(1);
            func_0203c02c(&gSceneManager, 0xC0);
            gSceneMemory.mCharacters[0] = mCharacters[0].mIndex;
            gSceneMemory.mCharacters[1] = mCharacters[1].mIndex;
        }
        break;
    case STATE_FAREWELL:
        SetSpeaker(0);
        mCharacters[0].SetState(WeddingCharacter::STATE_FAREWELL);
        mState++;
        break;
    case STATE_WAIT_FAREWELL:
        if (mCharacters[0].mDone) {
            mState++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 60);
        }
        break;
    case STATE_FADE_OUT_FAREWELL: {
        int level = mFrame * 2;
        if (level > 16) {
            level = 16;
        } else if (level < 0) {
            level = 0;
        }
        func_02023f4c(&data_0211d604, 0x3F3F, level);
        if (mFrame >= 8) {
            mState++;
            mFrame = 0;
        }
        break;
    }
    case STATE_LEAVE:
        if (mFrame >= 52) {
            s64 now = Clock_GetSeconds(data_02123440);
            mCharacters[0].mAwayUntil = now + GetHoneymoonMinutes(&mCharacters[0]) * 60;
            mCharacters[1].mAwayUntil = now + GetHoneymoonMinutes(&mCharacters[1]) * 60;
            mCharacters[0].mMoodLocked = 3;
            mCharacters[1].mMoodLocked = 3;
            func_0203c02c(&gSceneManager, 0x77);
            gSceneMemory.mCharacters[0] = mCharacters[0].mIndex;
            gSceneMemory.mCharacters[1] = mCharacters[1].mIndex;
        }
        break;
    }
    mFrame++;
    mTtsPlayer.Update();
}

void WeddingScene::Draw() {
    mWindow.Draw();
    switch (mDrawState) {
    case 0:
        func_020a5960(&mRoom);
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
    func_02091968(&mCharacters[0]);
    func_02091968(&mCharacters[1]);
    func_020a4a50(&mRoom.mParts);
    mGift.Draw();
}

void WeddingScene::Exit() {
    mInventory.Save();
    func_0202c28c(&gGameData, mCharacters[0].mIndex, &mCharacters[0]);
    func_0202c28c(&gGameData, mCharacters[1].mIndex, &mCharacters[1]);
    ArchiveManager_Unmount(&gArchiveManager, "SMT");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TIM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    func_0202aecc(data_021223e4, 2);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    func_020a5948(&mRoom);
    ArchiveManager_Unmount(&gArchiveManager, "3H0");
    ArchiveManager_Unmount(&gArchiveManager, "3H1");
    ArchiveManager_Unmount(&gArchiveManager, mGift.mArchiveName);
    func_0202aecc(data_021223e4, 10);
}

// Which saved character each of the two is: when the scene comes back from scene 0x40 or 0xC0, the ones that it
// left there; otherwise the first two
u8 WeddingScene::GetCharacterIndex(int slot) {
    int previous = gSceneManager.mPreviousSceneId;
    if (previous == 0x40 || previous == 0xC0) {
        return gSceneMemory.mCharacters[(u8)slot];
    }
    return slot;
}

// Gives the speech engine one character's voice, and moves that character's mouth with it
void WeddingScene::SetSpeaker(int slot) {
    if (slot == 0) {
        func_0209d898(&mCharacters[0], &mTtsPlayer);
        mCharacters[0].mLipSync = TRUE;
        mCharacters[1].mLipSync = FALSE;
    } else {
        func_0209d898(&mCharacters[1], &mTtsPlayer);
        mCharacters[0].mLipSync = FALSE;
        mCharacters[1].mLipSync = TRUE;
    }
}

// --- The characters ---------------------------------------------------------------------------------------------

void WeddingCharacter::Init(WeddingContext *context) {
    mContext = context;
    mTtsPlayer = context->mTtsPlayer;
    mCurrentState = STATE_NONE;
    mLipSync = FALSE;
}

void WeddingCharacter::Update() {
    func_020910a8(this);
    UpdateState();
    if (mTtsPlayer->IsEngineIdle()) {
        return;
    }
    if (mLipSync) {
        func_02090a58(this, mTtsPlayer->GetPhonemeName(), 1);
    }
}

void WeddingCharacter::UpdateState() {
    switch (mCurrentState = mState) {
    case STATE_NONE:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;

    case STATE_ANNOUNCE:
        if (mEntered) {
            mEntered = FALSE;
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame >= mWait) {
                mStep++;
                Speak(0x24F, 0); // "We are getting married"
            }
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                mDone = TRUE;
                Character_SetFace(this, 10);
            }
            break;
        }
        break;

    case STATE_THANK:
        if (mEntered) {
            mEntered = FALSE;
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            mWait = 10;
            if (mFrame >= mWait) {
                mStep++;
                Speak(0x251, 0); // "Thank you"
                PlayAnimation("greet01", TRUE);
            }
            break;
        case 1:
            if (GetAnimation()->mFinished) {
                PlayAnimation("idle00", FALSE);
                mStep++;
            }
            break;
        case 2:
            if (mTtsPlayer->IsEngineIdle()) {
                Character_SetFace(this, 10);
                mDone = TRUE;
            }
            break;
        }
        break;

    case STATE_LOOK_AT_PARTNER:
        if (mEntered) {
            mEntered = FALSE;
            LookAtPartner();
            Character_SetFace(this, 9);
        }
        break;

    case STATE_PROMISE:
        if (mEntered) {
            mEntered = FALSE;
            LookAtCamera();
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame >= mWait) {
                mStep++;
                Speak(0x253, 0); // "We will live happily ever after"
            }
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                Character_SetFace(this, 10);
                mDone = TRUE;
            }
            break;
        }
        break;

    case STATE_GIVE:
        if (mEntered) {
            mEntered = FALSE;
            mContext->mGift->ChangeItem(ITEM_WEDDING_ALBUM);
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            mWait = 10;
            if (mFrame >= mWait) {
                mStep++;
                Speak(0x257, 0); // "Please take this as a keepsake"
            }
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                mStep++;
                mFrame = 0;
                PlayAnimation("i_give01", TRUE);
            }
            break;
        case 2:
            // Holding the gift out: a sound at a quarter of the animation, and from the half on the gift grows in
            // the hands
            if (!GetAnimation()->mFinished) {
                ModelAnimation *animation = GetAnimation();
                int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / animation->mSpeed + 1;
                int quarter = frames / 4;
                int half = frames / 2;
                if (mFrame == quarter) {
                    gSoundManager.StartSeqArc(1, 0x1A, 8);
                } else if (mFrame == half) {
                    mContext->mGift->SetScale(((mFrame - half + 1) << FX32_SHIFT) / (frames - half));
                    mContext->mGift->mVisible = TRUE;
                    HoldOutGift();
                } else if (mFrame > half) {
                    mContext->mGift->SetScale(((mFrame - half + 1) << FX32_SHIFT) / (frames - half));
                    HoldOutGift();
                }
            } else {
                mStep++;
                PlayAnimation("i_give00", TRUE);
            }
            // fallthrough
        case 3:
            HoldOutGift();
            if (GetAnimation()->mFinished) {
                mStep++;
                mFrame = 0;
            }
            break;
        case 4:
            mWait = 60;
            if (mFrame >= mWait) {
                mDone = TRUE;
            }
            break;
        }
        break;

    case STATE_FAREWELL:
        if (mEntered) {
            mEntered = FALSE;
            break;
        }
        if (mDone) {
            break;
        }
        switch (mStep) {
        case 0:
            mWait = 60;
            if (mFrame >= mWait) {
                mStep++;
                Speak(0x259, 0); // "We are off on our honeymoon"
            }
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                Character_SetFace(this, 10);
                mFrame = 0;
                mStep++;
            }
            break;
        case 2:
            if (mFrame >= 20) {
                mDone = TRUE;
            }
            break;
        }
        break;
    }
    mFrame++;
}

void WeddingCharacter::SetState(int state) {
    mState = state;
    mStep = 0;
    mEntered = TRUE;
    mDone = FALSE;
    mFrame = 0;
}

void WeddingCharacter::FaceCamera() {
    FloorPos camera;
    camera.x = data_02122764.mEye.x;
    camera.z = data_02122764.mEye.z;
    Character_SetAngle(this, AngleTo(camera));
}

// Shows a message of the trouble messages (file 0) or of the other messages (file 1) in the window, with its
// arguments, and speaks its pronunciation, which is the message after it
void WeddingCharacter::Speak(int file, u32 message, int count, int *types, int *values, int mode) {
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
    mContext->mWindow->mText.mHAlign = 0;
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
                func_0202c264(&gGameData, values[i], &character);
                func_0207a80c(&character, names[i]);
                func_020372d0(pronunciations[i], names[i], sizeof(names[i]));
                break;
            case ARG_ITEM:
                wcscpy(names[i], mContext->mItemMessages->GetText(values[i]));
                wcscpy(pronunciations[i], mContext->mItemMessages->GetText(values[i] + 0xFF));
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

void WeddingCharacter::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void WeddingCharacter::Speak(u32 message, int mode) {
    Speak(0, message, ARG_NONE, 0, mode);
}

// Puts the gift between the hands: each hand's point is the hand, or a little way from the joint above it towards
// the hand if the hand is nearer than that
void WeddingCharacter::HoldOutGift() {
    VecFx32 middle;
    VecFx32 rightHand;
    VecFx32 leftHand;
    VecFx32 rightArm;
    VecFx32 leftArm;
    VecFx32 rightOffset;
    VecFx32 leftOffset;
    VecFx32 rightReach;
    VecFx32 leftReach;
    VecFx32 rightPoint;
    VecFx32 leftPoint;

    if (mContext->mGift == NULL) {
        return;
    }
    mModel.mUnknown45C = TRUE;
    rightHand = CharacterModel_GetJointPosition(&mModel, 2);
    rightArm = CharacterModel_GetJointPosition(&mModel, 9);
    leftHand = CharacterModel_GetJointPosition(&mModel, 1);
    leftArm = CharacterModel_GetJointPosition(&mModel, 8);

    VEC_Subtract(&rightHand, &rightArm, &rightOffset);
    VEC_Normalize(&rightOffset, &rightReach);
    ScaleVec(0x999, &rightReach, &rightReach);
    if (LengthSq(&rightReach) > LengthSq(&rightOffset)) {
        VEC_Add(&rightArm, &rightReach, &rightPoint);
        rightPoint.y = rightHand.y;
    } else {
        rightPoint = rightHand;
    }

    VEC_Subtract(&leftHand, &leftArm, &leftOffset);
    VEC_Normalize(&leftOffset, &leftReach);
    ScaleVec(0x999, &leftReach, &leftReach);
    if (LengthSq(&leftReach) > LengthSq(&leftOffset)) {
        VEC_Add(&leftArm, &leftReach, &leftPoint);
        leftPoint.y = leftHand.y;
    } else {
        leftPoint = leftHand;
    }

    VEC_Add(&rightPoint, &leftPoint, &middle);
    middle.x /= 2;
    middle.y /= 2;
    middle.z /= 2;
    mContext->mGift->SetPosition(middle);
}

// Turns the head towards a point, as far as the limits allow
void WeddingCharacter::LookAt(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurnActive = TRUE;
    func_02090c78(this, &pitch, pitchLimit);
    func_02090c78(this, &yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
}

// The same, at once
void WeddingCharacter::TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurnActive = TRUE;
    func_02090c78(this, &pitch, pitchLimit);
    func_02090c78(this, &yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadPitchNow = pitch;
    mHeadYawNow = yaw;
}

void WeddingCharacter::LookAtCamera() {
    LookAt(&data_02122764.mEye, 0x2000, 0x3200);
}

void WeddingCharacter::TurnHeadToCamera() {
    TurnHeadTo(&data_02122764.mEye, 0x2000, 0x3200);
}

void WeddingCharacter::LookAtPartner() {
    VecFx32 target;
    const VecFx32 &head = CharacterModel_GetJointPosition(&mPartner->mModel, 0);
    target.x = head.x;
    target.z = head.z;
    target.y = head.y + 0xB33;
    LookAt(&target, 0xB00, 0x3200);
}

u16 WeddingCharacter::GetHeadPitchTo(VecFx32 *target) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    CharacterModel_GetJointMatrices(&mModel, 0, &matrix, &rotation);
    fx32 dz = target->z - mModel.mPos.z;
    fx32 dx = target->x - mModel.mPos.x;
    fx32 eyes = matrix._31 + 0xCCC;
    return -FX_Atan2Idx(target->y - eyes, SqrtXZ(dx, dz));
}

u16 WeddingCharacter::GetHeadYawTo(VecFx32 *target) {
    return mAngle - FX_Atan2Idx(target->z - mModel.mPos.z, target->x - mModel.mPos.x);
}

// --- The camera -------------------------------------------------------------------------------------------------

void WeddingCamera::Init(WeddingCharacter *first, WeddingCharacter *second) {
    mCamera = &data_02122764;
    func_0202a640(mCamera);
    mCamera->SetPosition(sCameraStartPosition);
    mCamera->SetTarget(sCameraStartTarget);
    mCamera->mAngleX = 0;
    mCamera->mAngleY = 0;
    mCamera->mAngleZ = 0;
    func_0202a748(mCamera);
    mFirst = first;
    mSecond = second;
    CopyVec(&mPosition, &mCamera->mPosition);
    CopyVec(&mTarget, &mCamera->mTarget);
    mState = 1;
}

// Puts the camera 14 in front of the target, in the direction it was (or, coming back from another scene, in the
// direction that scene left it)
inline void WeddingCamera::SetDefaultTarget() {
    SetTarget(sCameraTarget);
}

void WeddingCamera::Reset(BOOL restore) {
    if (restore) {
        Restore();
    }
    SetDefaultTarget();
    FloorPos direction = NormalizeXZ(mPosition.x - mTarget.x, mPosition.z - mTarget.z);
    mPosition.x = mTarget.x + direction.x * 14;
    mPosition.z = mTarget.z + direction.z * 14;
    mPosition.y = 0x41D0;
    CopyVec(&mCamera->mPosition, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    func_0202a748(mCamera);
}

// Follows the middle between the two, keeping the distance and the height
void WeddingCamera::Update() {
    switch (mState) {
    case 0:
        return;
    case 1:
        break;
    default:
        return;
    }
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
}

// Where the two stand: on either side of the target, across the camera's view
void WeddingCamera::GetStandingPoints(VecFx32 *first, VecFx32 *second) {
    FloorPos across = NormalizeXZ(mTarget.z - mPosition.z, -(mTarget.x - mPosition.x));
    fx32 x = FX_Mul(across.x, 0xB33);
    fx32 z = FX_Mul(across.z, 0xB33);
    first->x = mTarget.x + x;
    first->z = mTarget.z + z;
    first->y = 0;
    second->x = mTarget.x - x;
    second->z = mTarget.z - z;
    second->y = 0;
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 WeddingCamera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = Shift(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

// --- The lights -------------------------------------------------------------------------------------------------

void WeddingLights::Init(Camera *camera, Room *room) {
    VecFx32 view;
    mState = 0;
    mCamera = camera;
    mRoom = room;
    VEC_Subtract(&mCamera->mPosition, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        mLights[i] = &data_021227cc[i];
        func_0202a96c(&data_021227cc[i]);
        mLights[i]->mColor = data_020f72f8[mRoom->mStyle].mLightColor;
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void WeddingLights::Update() {
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
        SetColor(func_020a4d90(mRoom));
        mState = 0;
        break;
    }
}

void WeddingLights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// --- The gift -----------------------------------------------------------------------------------------------------

void WeddingGiftModel::Load(int resourceId) {
    LoadModel(resourceId);
}

// Shows another item: lets go of the old one's files, and loads the new one's
void WeddingGiftModel::ChangeItem(int item) {
    func_0202aecc(data_021223e4, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(data_021223e4, mResourceId, mModelPath);
    LoadAnimation(mResourceId);
    ResetScale();
}
