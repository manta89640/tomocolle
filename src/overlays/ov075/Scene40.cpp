#include "Scene40.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneMemory.h"
#include "sound/SoundManager.h"

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

// Not decompiled yet
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
void func_ov075_02161988(Scene40Character *character);
void func_ov075_02161d94(Scene40Character *character);
void func_ov075_02178364(Scene40CharacterPartA *part);
void func_ov075_0217af40(ItemModel *item); // Hides the held item's model and resets it
extern const u32 data_ov075_0217b82c[];         // The message of each song slot given as a present
extern const char *const data_ov075_02182d10[4][5]; // Animation names, by personality kind and mood
extern const int gPersonalityKinds[16];
extern const char *const data_ov075_02182d60[4][5]; // Walking animation names, by personality kind and mood
extern MATHRandContext16 gRandom;
void func_ov075_0217510c(Scene40Character *character);
void func_ov075_02178f50(Scene40CharacterPartB *part);
BOOL func_ov075_021791cc(Scene40CharacterPartB *part);
void func_ov075_02179034(Scene40CharacterPartB *part);
void func_ov075_021790ac(Scene40CharacterPartB *part);
BOOL func_ov075_02179230(Scene40CharacterPartB *part);
}

static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}


// Overlay 75: scenes 0x40 and 0x6C, the scene of touching a Mii ("Scn/Scene/SceneMiiTouch_LZ.bin"). One source
// file by its one static initialiser. Most of it is not decompiled yet.

// --- The scene -----------------------------------------------------------------------------------------------------

// For the first nine frames the room and the character are updated from here as well
void Scene40::Draw() {
    MessageWindow_Draw(&mWindows[0]);
    MessageWindow_Draw(&mWindows[1]);
    func_ov075_0214fc00(mUnknown30B50);
    Sprite_Draw(&mSprite013D0);

    switch (mDrawState) {
    case 0:
        mRoom.Update();
        mCharacter.Update(FALSE);
        mDrawFrames++;
        if (mDrawFrames >= 9) {
            mDrawState = 1;
        }
        break;
    case 1:
        break;
    }

    ParticleSystem_Draw(&mParticles, 0);
    mCharacter.Draw();
    mFace.Draw();
    mMannequin.Draw();
    mRoom.RoomModels::Draw();
    for (int i = 0; i < 3; i++) {
        mItemModels[i].Draw();
    }
    mSpeechWindow.Draw();
}

// NOT MATCHING (the copy of the camera's eye and target only): the original keeps the camera's address in r5 and
// loads the words in pairs before storing them (x, y; store x; z, target x; store y and z together; ...), where
// this loads and stores one at a time through r1. Whole-vector and field-by-field assignments compile alike; a
// local pointer to the camera is worse.
// Saves what the scene changed (the character, with the date of this meeting, and the lists of clothes, goods and
// interiors), keeps the camera for the next scene, and unmounts the scene's archives
void Scene40::Exit() {
    RTCDate date;
    date.year = mUnknown30A8C.mDate.year;
    date.month = mUnknown30A8C.mDate.month;
    date.day = mUnknown30A8C.mDate.day;
    date.week = mUnknown30A8C.mDate.week;
    mCharacter.SetMeetDate(&date);
    GameData_WriteCharacter(&gGameData, mCharacter.mIndex, &mCharacter);
    if ((int)gGameData.mUnknownB2[0] < (int)mCharacter.mSatisfyLevel) {
        gGameData.mUnknownB2[0] = mCharacter.mSatisfyLevel;
    }
    mClothesList.Save();
    mInventory.Save();
    mInteriors.Save();
    gGameData.SetCappedCounter(mUnknown309B4);

    gSceneMemory.mCameraPosition = gCamera.mEye;
    gSceneMemory.mCameraTarget = gCamera.mTarget;

    DestroyMainFrameHeap();
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "SMT");
    ArchiveManager_Unmount(&gArchiveManager, "LTL");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TLM");
    ArchiveManager_Unmount(&gArchiveManager, "TIM");
    ArchiveManager_Unmount(&gArchiveManager, "TDM");
    ArchiveManager_Unmount(&gArchiveManager, "TRM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ModelResources_ReleaseModel(gModelResources, 2);
    ModelResources_ReleaseModel(gModelResources, 4);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ArchiveManager_Unmount(&gArchiveManager, "3HB");
    ArchiveManager_Unmount(&gArchiveManager, "3HT");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ModelResources_ReleaseModel(gModelResources, 12);
    ModelResources_ReleaseModel(gModelResources, 13);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ModelResources_ReleaseModel(gModelResources, 17);
    ArchiveManager_Unmount(&gArchiveManager, "3PM");
    mRoom.ReleaseAll();
    ArchiveManager_Unmount(&gArchiveManager, "3HM");
    for (int i = 0; i < 3; i++) {
        ArchiveManager_Unmount(&gArchiveManager, mItemModels[i].mArchiveName);
        ModelResources_ReleaseModel(gModelResources, i + 14);
    }
}

// --- The scene's character ------------------------------------------------------------------------------------------

// With speaking set, the lips follow the scene's speech
void Scene40Character::Update(BOOL speaking) {
    Character::Update();
    func_ov075_02161988(this);
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    func_ov075_02178364(&mPartA);
    if (!speaking) {
        return;
    }
    func_ov075_02161d94(this);
    if (mTtsPlayer->IsEngineIdle()) {
        return;
    }
    if (!mLipSync) {
        return;
    }
    LipSync(mTtsPlayer->GetPhonemeName(), 1);
}

// objdiff scores this 99.5% though its instructions and relocations equal the original's (checked with objdump
// against the delinked object; check_relocs lists nothing): the two functions it calls are byte for byte the same,
// and the report seems to pair one call with the other's.
void Scene40Character::Draw() {
    Character::Draw();
    mPartA.Draw();
    mPartB.Draw();
}

// --- The scene's character's states 0x44 to 0xB2 (0x02168354 to 0x021722c8) ----------------------------------------

// The character gives back food it will not eat: "I am full" (0x44) or "I ate this a moment ago" (0x46), then
// waits (0x45, 0x47)
void Scene40Character::UpdateStates44To47() {
    Camera *camera = &gCamera;
    FloorPos target;

    switch (mState) {
    case 0x44:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_wait00", FALSE);
            UpdateAnimSpeed();
            Speak(0, 0x104, 0);
            TurnHeadToCamera();
            mStep = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            BlendToNamedAnimation("i_walk00");
            ResetHeadTurn();
            mStep++;
            break;
        case 1:
            MoveAlongAngle();
            if (!TurnToWalkTarget(0x280, 0)) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            TurnHeadToCamera();
            mStep++;
            break;
        case 2:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 3:
            mWait = 31;
            if (mFrame < mWait) {
                return;
            }
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            func_ov075_0217af40(mHeldItem);
            mHeldItem = NULL;
            mFrame = 0;
            mStep++;
            break;
        case 4:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x45);
            break;
        }
        break;
    case 0x45:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x46:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_wait00", FALSE);
            UpdateAnimSpeed();
            TurnHeadToCamera();
            mStep = 0;
            mFrame = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            Speak(0, 0x106, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            BlendToNamedAnimation("i_walk00");
            ResetHeadTurn();
            mStep++;
            break;
        case 2:
            MoveAlongAngle();
            if (!TurnToWalkTarget(0x280, 0)) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (mFrame >= 20) {
                mHeldItem->mVisible = FALSE;
            }
            if (mFrame < 40) {
                return;
            }
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            func_ov075_0217af40(mHeldItem);
            mHeldItem = NULL;
            mFrame = 0;
            mStep++;
            break;
        case 5:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x47);
            break;
        }
        break;
    case 0x47:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    }
}

// The character receives a present from the player: walks to face the camera (0x48), holds it up (0x49) and shows
// what it was in the speech window, takes it (0x4A, then waits in 0x4B), or waits (0x4C); 0x4D holds the last frame
// of holding up and then goes back to state 0; 0x4E speaks a message before taking it
void Scene40Character::UpdateStates48To4E() {
    Camera *camera = &gCamera;
    FloorPos target;

    switch (mState) {
    case 0x48:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        SetState(0x49);
        break;
    case 0x49:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_hoist00", TRUE);
            mHeadIcons[1].Show();
            mHeadIcons[1].mIconId = 4;
            mHeadIcons[1].Reload();
            mHeadIcons[1].SetMode(3);
            mHeadIconPlaces[1] = 3;
            return;
        }
        PlaceHeldItemInHand();
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                return;
            }
            u32 message;
            int mode = SpeechWindow::MODE_FRAMES;
            switch (mGift.mGiftKind) {
            case 3:
                message = data_ov075_0217b82c[mGift.mItem.InventoryItem::mId];
                SetSongSlot(mGift.mItem.InventoryItem::mId, 0xFF);
                mUnknown299 |= 1 << (mGift.mItem.InventoryItem::mId & 0x1F);
                break;
            case 4:
                switch (mGift.mItem.InventoryItem::mId) {
                case 236:
                    message = 0xA5;
                    break;
                case 241:
                    message = 0xA6;
                    break;
                case 243:
                    message = 0xA7;
                    break;
                case 242:
                    message = 0xA8;
                    break;
                case 239:
                    message = 0xA9;
                    break;
                case 240:
                    message = 0xAA;
                    break;
                case 237:
                    message = 0xAB;
                    break;
                case 238:
                    message = 0xAC;
                    break;
                }
                mUnknown1C8.m[mGift.mItem.InventoryItem::mId >> 5] |= 1 << (mGift.mItem.InventoryItem::mId & 0x1F);
                StartLongActWithTool(mGift.mItem.InventoryItem::mId);
                break;
            case 5:
                message = 0xB2;
                mode = SpeechWindow::MODE_MANUAL;
                break;
            case 2:
                message = 0xB0;
                break;
            }
            gSoundManager.StartSeqIfNotPlaying(0, 22);
            mSpeechWindow->mMessageFile = mContext->mMiiTouchMessages;
            mSpeechWindow->mText[0].mHAlign = 1;
            mSpeechWindow->Show(message, mode, 120);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mWait = 120;
            if (mFrame < mWait) {
                return;
            }
            switch (mGift.mGiftKind) {
            case 3:
            case 4:
                SetState(0x4A);
                break;
            case 5:
                SetState(0x4E);
                break;
            case 2:
                SetState(0x4C);
                break;
            }
            break;
        }
        break;
    case 0x4A:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_get01", TRUE);
            UpdateAnimSpeed();
            ResetHeadTurn();
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int half = frames / 2;
            int threeQuarters = frames * 3 / 4;
            if (mFrame < half) {
                PlaceHeldItem();
                mHeldItem->SetScale(((half - mFrame) << FX32_SHIFT) / half);
            } else if (mFrame == half) {
                func_ov075_0217af40(mHeldItem);
                mHeldItem = NULL;
            } else if (mFrame == threeQuarters) {
                gSoundManager.StartSeqArc(1, 26, 8);
            } else if (GetAnimation()->mFinished && mFrame > threeQuarters) {
                BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            break;
        }
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x4B);
            break;
        }
        break;
    case 0x4B:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x4C:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x4D:
        if (mEntered) {
            mEntered = FALSE;
            SetNamedAnimation("i_hoist00");
            ModelAnimation *animation = GetAnimation();
            SetAnimFlagsAndFrame(TRUE, (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) - 1, TRUE);
            return;
        }
        if (mFrame < 30) {
            return;
        }
        SetState(0);
        break;
    case 0x4E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            Speak(0, 0x11E, 8, 0, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x4A);
        break;
    }
}

// The character receives clothes, an interior or goods it was given, or gives them back:
// 0x4F takes what it holds (as 0x4A) and waits in 0x50;
// 0x51 to 0x54 clothes: it faces the camera, and changes into them behind the curtain (0x52, then state 0x2E) or
// says "I already have these clothes" and gives them back (0x53, then waits in 0x54);
// 0x55, 0x56: it walks to the middle of the room and changes behind the curtain into what it wears;
// 0x57 to 0x61 an interior: it faces the camera and holds it up (0x58, then waits in 0x59), or says "I already have
// this interior" and gives it back (0x5A, 0x5B); 0x5C to 0x61 say what it thinks of its new room, from "I like it
// very much" to "personally it is a bit...", then state 0x33;
// 0x62 to 0x66 another present: it faces the camera, holds it up, speaks of it for 120 frames and takes it.
void Scene40Character::UpdateStates4FTo66() {
    Camera *camera = &gCamera;
    FloorPos target;

    switch (mState) {
    case 0x4F:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_get01", TRUE);
            UpdateAnimSpeed();
            ResetHeadTurn();
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int half = frames / 2;
            int threeQuarters = frames * 3 / 4;
            if (mFrame < half) {
                PlaceHeldItem();
                mHeldItem->SetScale(((half - mFrame) << FX32_SHIFT) / half);
            } else if (mFrame == half) {
                func_ov075_0217af40(mHeldItem);
                mHeldItem = NULL;
            } else if (mFrame == threeQuarters) {
                gSoundManager.StartSeqArc(1, 26, 8);
            } else if (GetAnimation()->mFinished && mFrame > threeQuarters) {
                BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            break;
        }
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x50);
            break;
        }
        break;
    case 0x50:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x51:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            if ((mOwnedClothes.m[mGift.mItem.InventoryItem::mId >> 5] >> (mGift.mItem.InventoryItem::mId & 0x1F)) & 1) {
                SetState(0x53);
            } else {
                SetState(0x52);
            }
            break;
        }
        break;
    case 0x52:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation("i_wait00");
            UpdateAnimSpeed();
            mPartB.Switch(1);
            mPartB.SetPosition(mPos, mAnim.mRotY);
            func_ov075_02178f50(&mPartB);
            gSoundManager.StartSeqArc(1, 10, 1);
            if (Random(100) < 50) {
                mUnknown24E = 1;
            }
            return;
        }
        switch (mStep) {
        case 0:
            PlaceHeldItem();
            if (!func_ov075_021791cc(&mPartB)) {
                return;
            }
            func_ov075_0217af40(mHeldItem);
            mHeldItem = NULL;
            func_ov075_02179034(&mPartB);
            mFrame = 0;
            mStep++;
            break;
        case 1:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            mClothesId = mGift.mItem.InventoryItem::mId;
            OwnClothes(mGift.mItem.InventoryItem::mId);
            func_ov075_0217510c(this);
            ModelResources_ReleaseModel(gModelResources, 0);
            ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
            SetArchive(mArchiveName);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
            ModelResources_SetModel(gModelResources, 0, mModelPath);
            MakeModel(0, 0);
            UpdateBodyColors();
            BlendToNamedAnimation("idle00");
            UpdateAnimSpeed();
            mFrame = 0;
            mStep++;
            break;
        case 2:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            func_ov075_021790ac(&mPartB);
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 3:
            if (!func_ov075_02179230(&mPartB)) {
                return;
            }
            mPartB.mModel.mVisible = FALSE;
            mUnknown30A0 = TRUE;
            SetState(0x2E);
            break;
        }
        break;
    case 0x53:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x10A, 0);
            TurnHeadToCamera();
            BlendToNamedAnimation("i_wait00");
            mStep = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            mStep++;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 2:
            if (mFrame >= 20) {
                mHeldItem->mVisible = FALSE;
            }
            if (mFrame < 40) {
                return;
            }
            mFrame = 0;
            mStep++;
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            func_ov075_0217af40(mHeldItem);
            mHeldItem = NULL;
            break;
        case 3:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x54);
            break;
        }
        break;
    case 0x54:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x55:
        if (mEntered) {
            mEntered = FALSE;
            ShowBalloon(3);
            ResetHeadTurn();
            mUnknown2EA0.x = 0;
            mUnknown2EA0.z = 0;
            SetTargetFloorPos(mUnknown2EA0);
            if (!WalkToTarget(ScaleBySpeed(0x7A), 0x22, 4)) {
                ResetSoundPhase(0);
                BlendToNamedAnimation(data_ov075_02182d60[gPersonalityKinds[mPersonality]][mMood]);
                mStep = 0;
                MoveAlongAngle();
            } else {
                target.x = camera->mEye.x;
                target.z = camera->mEye.z;
                SetTargetFloorPos(target);
                MoveAlongAngle();
                if (!TurnToWalkTarget(0x280, 0)) {
                    ResetSoundPhase(0);
                    BlendToNamedAnimation(data_ov075_02182d60[gPersonalityKinds[mPersonality]][mMood]);
                    mStep = 1;
                } else {
                    BlendToNamedAnimation("idle00");
                    mStep = 3;
                }
                MoveAlongAngle();
            }
            return;
        }
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (WalkToTarget(ScaleBySpeed(0x7A), 0x22, 4)) {
                target.x = camera->mEye.x;
                target.z = camera->mEye.z;
                SetTargetFloorPos(target);
                mStep++;
            }
            MoveAlongAngle();
            break;
        case 1:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                mStep++;
            }
            MoveAlongAngle();
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("idle00");
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x56);
            break;
        }
        break;
    case 0x56:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 20;
            if (mFrame < mWait) {
                return;
            }
            mPartB.Switch(1);
            mPartB.SetPosition(mPos, mAnim.mRotY);
            func_ov075_02178f50(&mPartB);
            gSoundManager.StartSeqArc(1, 10, 1);
            mStep++;
            break;
        case 1:
            if (!func_ov075_021791cc(&mPartB)) {
                return;
            }
            func_ov075_02179034(&mPartB);
            mFrame = 0;
            mStep++;
            break;
        case 2:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            ModelResources_ReleaseModel(gModelResources, 0);
            ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
            SetArchive(mArchiveName);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
            ModelResources_SetModel(gModelResources, 0, mModelPath);
            MakeModel(0, 0);
            UpdateBodyColors();
            mFrame = 0;
            mStep++;
            break;
        case 3:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            func_ov075_021790ac(&mPartB);
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (!func_ov075_02179230(&mPartB)) {
                return;
            }
            mPartB.mModel.mVisible = FALSE;
            SetState(0);
            break;
        }
        break;
    case 0x57:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        if (mOwnedInteriors[mGift.mItem.InventoryItem::mId >> 5] & (1 << (mGift.mItem.InventoryItem::mId & 0x1F))) {
            SetState(0x5A);
        } else {
            SetState(0x58);
        }
        break;
    case 0x58:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_hoist00", TRUE);
            func_ov075_0217510c(this);
            return;
        }
        PlaceHeldItemInHand();
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                return;
            }
            gSoundManager.StartSeqArc(1, 10, 20);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mWait = 60;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x59);
            break;
        }
        break;
    case 0x59:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x5A:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x116, 0);
            TurnHeadToCamera();
            BlendToNamedAnimation("i_wait00");
            mStep = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            mStep++;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 2:
            if (mFrame >= 20) {
                mHeldItem->mVisible = FALSE;
            }
            if (mFrame < 40) {
                return;
            }
            mFrame = 0;
            mStep++;
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            func_ov075_0217af40(mHeldItem);
            mHeldItem = NULL;
            break;
        case 3:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x5B);
            break;
        }
        break;
    case 0x5B:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x5C:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x10C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x5D:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x10C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x5E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x10E, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x5F:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x110, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x60:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x112, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x61:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x114, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x62:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        SetState(0x63);
        break;
    case 0x63:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_hoist00", TRUE);
            return;
        }
        PlaceHeldItemInHand();
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x64);
            break;
        }
        break;
    case 0x64:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x11E, 8, 0, 0);
            mFrame = 0;
            return;
        }
        if (mFrame < 120) {
            return;
        }
        SetState(0x65);
        break;
    case 0x65:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_get01", TRUE);
            UpdateAnimSpeed();
            ResetHeadTurn();
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int half = frames / 2;
            int threeQuarters = frames * 3 / 4;
            if (mFrame < half) {
                PlaceHeldItem();
                mHeldItem->SetScale(((half - mFrame) << FX32_SHIFT) / half);
            } else if (mFrame == half) {
                func_ov075_0217af40(mHeldItem);
                mHeldItem = NULL;
            } else if (mFrame == threeQuarters) {
                gSoundManager.StartSeqArc(1, 26, 8);
            } else if (GetAnimation()->mFinished && mFrame > threeQuarters) {
                BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            break;
        }
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x66);
            break;
        }
        break;
    case 0x66:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    }
}

void Scene40CharacterPartA::Draw() {
    mModel.Draw();
}

void Scene40CharacterPartB::Draw() {
    mModel.Draw();
}

// --- The scene's list: the shops' item list with a mark beside each row ------------------------------------------

void Scene40ListView::Draw() {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Draw(&mMarks[i]);
    }
    ItemListView::Draw();
}

void Scene40ListView::SetVisible(BOOL visible) {
    ItemListView::SetVisible(visible);
    for (int i = 0; i < mRowCount; i++) {
        mMarks[i].mVisible = visible;
    }
}

// A row's mark shows that the character has had the row's item: a bit of its record for each
void Scene40ListView::DrawRow(int row, int item) {
    ItemListView::DrawRow(row, item);
    BOOL visible = TRUE;
    switch (mMarkKind) {
    case MARK_NONE:
        visible = FALSE;
        break;
    case MARK_ITEMS:
        visible = mCharacter->mUnknown1C8.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_CLOTHES:
        visible = mCharacter->mOwnedClothes.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_INTERIORS:
        visible = mCharacter->mOwnedInteriors[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    }
    mMarks[row].mVisible = visible;
}

void Scene40ListView::ClearRow(int row) {
    ItemListView::ClearRow(row);
    mMarks[row].mVisible = FALSE;
}

// --- The scene's clock, at 0x30A8C --------------------------------------------------------------------------------

Scene40Unknown30A8C::Scene40Unknown30A8C() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

void Scene40Unknown30A8C::Read() {
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

void Scene40Unknown30A8C::ReadIfIdle() {
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}
