#include "Scene40.h"
#include "Scene40Context.h"
#include "Scene40SpeechWindow.h"
#include "Scene40Camera.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "sound/SoundManager.h"
#include <wchar.h>

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
extern const u32 data_ov075_0217b82c[];         // The message of each song slot given as a present
extern const char *const data_ov075_02182d10[4][5]; // Animation names, by personality kind and mood
extern const int gPersonalityKinds[16];
extern const char *const data_ov075_02182d60[4][5]; // Walking animation names, by personality kind and mood
extern MATHRandContext16 gRandom;
void func_ov075_02177f54(Scene40Character *character);
void func_ov075_02176958(Scene40Character *character);
void func_ov075_0217654c(Scene40Character *character);
void func_ov075_0217680c(Scene40Character *character);
void func_ov075_021768f0(Scene40Character *character);
void func_ov075_02176924(Scene40Character *character);
void func_ov075_021767b8(Scene40Character *character);
void func_ov075_02176e74(Scene40Character *character);
void func_ov075_02176eb8(Scene40Character *character);
void MiiData_GetName(const void *mii, wchar_t *name);
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
int func_ov075_0217520c(Scene40Character *character);
void func_ov075_02174a14(Scene40Character *character);
void func_ov075_02174ad0(Scene40Character *character);
void func_ov075_02174c00(Scene40Character *character);
void func_ov075_0217ade4(void *models, Scene40ItemModel **model);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int level);
}

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78), as SavedCharacter.cpp has it
struct TroubleInfo {
    u32 kind : 8;
    u32 minutes : 16;
    u32 weight : 8;
    u32 unk4Bits0 : 8;
    u32 mood : 8;
    u32 cooldownMinutes : 12;
    u32 hasTarget : 4;
    u32 flags;
}; // 0xC
extern "C" TroubleInfo *data_02114d78[];

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
    mBalloon.Update();
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
    mBalloon.Draw();
    mCover.Draw();
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
            mHeldItem->Hide();
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
            mHeldItem->Hide();
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
                mHeldItem->Hide();
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
                mHeldItem->Hide();
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
            mCover.SetKind(Scene40Cover::KIND_CURTAIN);
            mCover.SetPosition(mPos, mAnim.mRotY);
            mCover.Appear();
            gSoundManager.StartSeqArc(1, 10, 1);
            if (Random(100) < 50) {
                mUnknown24E = 1;
            }
            return;
        }
        switch (mStep) {
        case 0:
            PlaceHeldItem();
            if (!mCover.HasAppeared()) {
                return;
            }
            mHeldItem->Hide();
            mHeldItem = NULL;
            mCover.PlayWait();
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
            AddUnknown1C4ByTrouble();
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
            mCover.Disappear();
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 3:
            if (!mCover.HasDisappeared()) {
                return;
            }
            mCover.mModel.mVisible = FALSE;
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
            mHeldItem->Hide();
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
            mCover.SetKind(Scene40Cover::KIND_CURTAIN);
            mCover.SetPosition(mPos, mAnim.mRotY);
            mCover.Appear();
            gSoundManager.StartSeqArc(1, 10, 1);
            mStep++;
            break;
        case 1:
            if (!mCover.HasAppeared()) {
                return;
            }
            mCover.PlayWait();
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
            mCover.Disappear();
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (!mCover.HasDisappeared()) {
                return;
            }
            mCover.mModel.mVisible = FALSE;
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
            AddUnknown1C4ByTrouble();
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
            mHeldItem->Hide();
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
                mHeldItem->Hide();
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

// What the character does with a present it has been given, by the present (the trouble messages it speaks):
// 0x67 the camera ("I'll go and take a picture": the screen fades out, it comes back holding the photo, scene
// 0xBF shows it); 0x68 "The title is ..."; 0x69 to 0x6C what it says to the player's opinion of the photo;
// 0x6D a present to try out ("Please try it", scene 0x55); 0x6E it wakes with a start ("What was I doing just now?");
// 0x6F the bath ("I'll go and take a bath", scene 0xB0) and 0x70, 0x71 "That was refreshing";
// 0x72 food: it eats, in bites, then 0x73 or 0x77 by what the food was; 0x74 to 0x7A medicine for a cold (the
// sound group changes and a message shows what it did, then "I feel the fever has gone down", "Quite bitter", "I
// feel much better", "A strange taste"); 0x7B, 0x7C the fan ("I'll cool off", scene 0x54, "Nicely cooled");
// 0x7D, 0x7E the music box (scene 0xA3); 0x7F, 0x80 cooking (scene 0xAA); 0x81, 0x82 an experience (scene 0x22);
// 0x83 to 0x85 an outing it is sent on, alone (scene 0x78) or with a partner (scene 0x79), "Here is a souvenir".
void Scene40Character::UpdateStates67To85() {
    Camera *camera = &gCamera;
    FloorPos target;
    int brightness;

    switch (mState) {
    case 0x67:
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
            Speak(0, 0x12E, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSoundManager.MoveSeqVolume(0, 0, 30);
            mStep++;
            mFrame = 0;
            break;
        case 4:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            mHeldItem->Hide();
            mHeldItem = NULL;
            SetNamedAnimation("idle00");
            break;
        case 5:
            if (mFrame == 32) {
                gSoundManager.StartSeqArc(1, 10, 18);
            }
            if (mFrame == 170) {
                gSoundManager.MoveSeqVolume(0, 127, 30);
            }
            if (mFrame < 192) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 6:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, 16 - brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            ScreenControl_SetBlendAlpha(&gScreenControl, 4, 17, 8, 8);
            break;
        case 7:
            if (mFrame < 40) {
                return;
            }
            Speak(0, 0x130, 0);
            TurnHeadToCamera();
            mStep++;
            mFrame = 0;
            break;
        case 8:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            UpdateAnimSpeed();
            PlayAnimation("i_give01", TRUE);
            func_ov075_0217ade4(mContext->mUnknown5C, &mHeldItem);
            mHeldItem->ChangeItem(250);
            mHeldItem->mVisible = FALSE;
            mStep++;
            mFrame = 0;
            break;
        case 9:
            if (!GetAnimation()->mFinished) {
                ModelAnimation *animation = GetAnimation();
                int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
                int quarter = frames / 4;
                int half = frames / 2;
                if (mFrame == quarter) {
                    gSoundManager.StartSeqArc(1, 26, 8);
                } else if (mFrame == half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    mHeldItem->mVisible = TRUE;
                    PlaceHeldItem(scale);
                } else if (mFrame > half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    PlaceHeldItem(scale);
                }
            } else {
                PlayAnimation("i_give00", TRUE);
                SetAnimSpeed(0xA66);
                mHeldItem->SetScale(FX32_ONE);
                PlaceHeldItem();
                mFrame = 0;
                mStep++;
            }
            break;
        case 10:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            gSoundManager.StartSeqArc(1, 10, 19);
            break;
        case 11:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 12:
            if (mFrame < 32) {
                return;
            }
            gSceneManager.RequestScene(0xBF);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x68:
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
            Speak(0, 0x132, 7, 0, 2);
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
    case 0x69:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            SetFace(2);
            mContext->mTtsPlayer->StopEngine();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 45) {
                return;
            }
            Speak(0, 0x134, 0);
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
    case 0x6A:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x136, 0);
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
    case 0x6B:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            SetFace(6);
            mContext->mTtsPlayer->StopEngine();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 60) {
                return;
            }
            Speak(0, 0x138, 0);
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
    case 0x6C:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x13A, 2, mIndex, 0);
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
    case 0x6D:
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
            mFrame = 0;
            mStep++;
            break;
        case 3:
            if (mFrame < 30) {
                return;
            }
            mFrame = 0;
            mStep++;
            mHeadTurn = 1;
            mHeadRoll = 0xF500;
            break;
        case 4:
            if (mFrame < 70) {
                return;
            }
            TurnHeadToCamera();
            mFrame = 0;
            mStep++;
            break;
        case 5:
            if (mFrame < 50) {
                return;
            }
            Speak(0, 0x124, 0);
            mStep++;
            break;
        case 6:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0x55);
            for (u8 i = 0; i < 12; i++) {
                gSceneMemory.mCharacters[i] = 0xFF;
            }
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x6E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(6);
            BlendToNamedAnimation("idle00");
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            ResetHeadTurn();
            func_ov075_02177f54(this);
            mUnknown24A = 0;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 120;
            if (mFrame < mWait) {
                return;
            }
            SetFace(5);
            SetFace(5);
            SetAnimFlags(TRUE, TRUE);
            mStep += 2;
            mLipSync = FALSE;
            Speak(0, 0x126, 0);
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 80) {
                return;
            }
            mLipSync = TRUE;
            SetFace(8);
            SetAnimFlags(FALSE, FALSE);
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mFrame < 15) {
                return;
            }
            TurnHeadToCamera();
            mStep++;
            mFrame = 0;
            break;
        case 4:
            mWait = 60;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x128, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0);
            break;
        }
        break;
    case 0x6F:
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
            Speak(0, 0x12A, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            ClearMoodIfTroubleOfKind8();
            gSceneManager.RequestScene(0xB0);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x70:
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
            Speak(0, 0x12C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0);
            break;
        }
        break;
    case 0x71:
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
            Speak(0, 0x12C, 0);
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
    case 0x72:
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
            ShowBalloon(3);
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
            Speak(0, 0x100, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3: {
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            BlendToNamedAnimation("i_eat00");
            UpdateAnimSpeed();
            ResetHeadTurn();
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int length = frames * 25 / 10;
            if (length > 80) {
                mWait = length;
                mUnknown3098 = 0;
                mUnknown309C = 3;
            } else {
                mWait = 80;
                mUnknown3098 = 0;
                mUnknown309C = 80 / frames + 1;
                mWait = 80 / frames * frames + frames / 2;
            }
            mFrame = 0;
            mStep++;
            break;
        }
        case 4:
            PlaceHeldItem();
            func_ov075_02174a14(this);
            func_ov075_02174ad0(this);
            func_ov075_02174c00(this);
            if (mFrame < mWait) {
                return;
            }
            mHeldItem->Hide();
            mHeldItem = NULL;
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][0]);
            mStep++;
            break;
        case 5:
            if (mBlendFrames > 0) {
                return;
            }
            switch (mGift.mGiftKind) {
            case 8:
                SetState(0x73);
                break;
            case 9:
                SetState(0x77);
                break;
            }
            break;
        }
        break;
    case 0x73:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x74:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x75);
            gSoundManager.StartSeq(0, 0x85);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B0, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 150) {
                return;
            }
            Speak(0, 0x14C, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x75:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 0) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x75);
            gSoundManager.StartSeq(0, 0x85);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B1, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 150) {
                return;
            }
            Speak(0, 0x14C, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x76:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            Speak(0, 0x14E, 0);
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
    case 0x77:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x78:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x76);
            gSoundManager.StartSeq(0, 0x86);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B2, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 180) {
                return;
            }
            Speak(0, 0x150, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x79:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 0) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x76);
            gSoundManager.StartSeq(0, 0x86);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B3, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 180) {
                return;
            }
            Speak(0, 0x150, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x7A:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            Speak(0, 0x152, 0);
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
    case 0x7B:
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
            Speak(0, 0x13C, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0x54);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x7C:
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
            Speak(0, 0x13E, 0);
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
    case 0x7D:
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
            Speak(0, 0x144, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            ClearMoodIfTroubleOfKind8();
            gSceneManager.RequestScene(0xA3);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x7E:
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
            Speak(0, 0x146, 0);
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
    case 0x7F:
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
            Speak(0, 0x148, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xAA);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x80:
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
            Speak(0, 0x14A, 0);
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
    case 0x81:
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
            Speak(0, 0x140, 0);
            TurnHeadToCamera();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 40);
            break;
        case 4:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 5:
            if (mFrame < 32) {
                return;
            }
            for (u8 i = 0; i < 12; i++) {
                gSceneMemory.mCharacters[i] = 0xFF;
            }
            gSceneManager.RequestScene(0x22);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x82:
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
            Speak(0, 0x142, 0);
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
    case 0x83:
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
            Speak(0, 0x154, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            Speak(0, 0x156, 0);
            mStep++;
            break;
        case 4: {
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            ClearMoodIfTroubleOfCategory4OrKind8();
            int partner = func_ov075_0217520c(this);
            if (partner != -1) {
                gSceneManager.RequestScene(0x79);
                gSceneMemory.mCharacters[0] = mIndex;
                gSceneMemory.mCharacters[1] = partner;
            } else {
                gSceneManager.RequestScene(0x78);
                gSceneMemory.mCharacters[0] = mIndex;
            }
            break;
        }
        }
        break;
    case 0x84:
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
            Speak(0, 0x158, 0);
            mStep++;
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                Speak(0, 0x15A, 0);
                mStep++;
            }
            // fallthrough
        case 2:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x8B);
            break;
        }
        break;
    case 0x85:
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
            if (gSceneManager.mPreviousSceneId != 0x77) {
                Speak(0, 0x15C, 0);
            } else {
                Speak(0, 0x15E, 0);
            }
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
    }
}

// The character's state when the player has touched it, and its talk about its troubles (the trouble messages
// it speaks): 0x86 it stands facing the camera (or walks to face it, 0x88) and, after a while, goes on with what
// it has to say; 0x89..0x8C it hands the player a present; 0x8D, 0x8E it levels up; 0x8F it walks back into place;
// 0x90..0x92 asking how to talk to someone it wants as a friend ("How should I talk?", scene 0xAF), 0x93, 0x94
// whether it became friends; 0x95..0x9C a confession ("How should I confess?", "Should I change first?" with the
// curtain, scene 0x5A), 0x9C "Why do you object?" and its answers 0x9D..0xA0; 0xA1..0xA4 a fight ("Can I have a
// moment?", scene 0xAB, "Then calm this anger", scene 0xB2), 0xA5..0xAB making up, separations and their outcomes
// with a sound and a line in the speech window; 0xAD..0xB0 the word the player teaches it (0xAF repeats it, 0xB0
// stores it as its motto); 0xB1 recovered from heartbreak.
void Scene40Character::UpdateStates86ToB2() {
    Camera *camera = &gCamera;
    FloorPos target;
    int brightness;

    switch (mState) {
    case 0x86:
        if (mEntered) {
            mEntered = FALSE;
            UpdateCameraAngle();
            if (IsFacingCamera()) {
                BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
                UpdateAnimSpeed();
                TurnHeadToCamera();
            } else {
                SetState(0x88);
            }
            if (mUnknown2ED4) {
                gSceneManager.SetSoundGroup(0x37);
                gSoundManager.StartSeqIfNotPlaying(0, 0x62);
            }
        } else if (mFrame == 30) {
            func_ov075_02176958(this);
            func_ov075_0217654c(this);
            if (mUnknown2ED4) {
                SetState(0x87);
            }
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        if (mFrame < 60) {
            return;
        }
        if (mUnknown2ED0) {
            mUnknown2ED0 = FALSE;
            SetState(0);
        } else if (IsInTrouble() && data_02114d78[mTroubleCategory][mTroubleId].kind == 18) {
            SetState(0xAC);
        }
        if (mUnknown2ED8) {
            func_ov075_0217680c(this);
            return;
        }
        if (mUnknown2EDC) {
            func_ov075_021768f0(this);
            return;
        }
        if (!IsInTrouble()) {
            return;
        }
        if (data_02114d78[mTroubleCategory][mTroubleId].kind != 6) {
            return;
        }
        func_ov075_02176924(this);
        break;
    case 0x87:
        if (mEntered) {
            mEntered = FALSE;
            return;
        }
        if (mStep != 0) {
            return;
        }
        if (gSoundManager.GetSeqTick(0) < 548) {
            return;
        }
        func_ov075_021767b8(this);
        break;
    case 0x88:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation(data_ov075_02182d60[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        if (TurnToWalkTarget(0x280, 0)) {
            SetState(0x86);
        }
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        break;
    case 0x89:
        if (mEntered) {
            mEntered = FALSE;
            mUnknown2B28 = 0;
            SetFace(8);
            PlayAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood], FALSE);
            UpdateAnimSpeed();
            mShadowVisible = TRUE;
            TurnHeadToCamera();
            Speak(0, 0x1C9, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x8B);
        break;
    case 0x8A:
        if (mEntered) {
            mEntered = FALSE;
            mUnknown2B28 = 0;
            SetFace(8);
            PlayAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood], FALSE);
            UpdateAnimSpeed();
            mShadowVisible = TRUE;
            TurnHeadToCamera();
            Speak(0, 0x1CB, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x8B);
        break;
    case 0x8B:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            UpdateAnimSpeed();
            PlayAnimation("i_give01", TRUE);
            func_ov075_0217ade4(mContext->mUnknown5C, &mHeldItem);
            mHeldItem->ChangeItem(mGift.mMessage);
            mHeldItem->mVisible = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                ModelAnimation *animation = GetAnimation();
                int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
                int quarter = frames / 4;
                int half = frames / 2;
                if (mFrame == quarter) {
                    gSoundManager.StartSeqArc(1, 26, 8);
                } else if (mFrame == half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    mHeldItem->mVisible = TRUE;
                    PlaceHeldItem(scale);
                } else if (mFrame > half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    PlaceHeldItem(scale);
                }
            } else {
                PlayAnimation("i_give00", TRUE);
                SetAnimSpeed(0xA66);
                mHeldItem->SetScale(FX32_ONE);
                PlaceHeldItem();
                mFrame = 0;
                mStep++;
            }
            break;
        case 1:
            PlaceHeldItem();
            if (!GetAnimation()->mFinished) {
                return;
            }
            SetState(0x8C);
            break;
        }
        break;
    case 0x8C:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x8D:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("levelup00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            ResetHeadTurn();
            mHeadIcons[0].Show();
            mHeadIcons[0].mIconId = 6;
            mHeadIcons[0].Reload();
            mHeadIcons[0].SetMode(3);
            mHeadIconPlaces[0] = ICON_AT_HEAD;
            mHeadIcons[1].Show();
            mHeadIcons[1].mIconId = 7;
            mHeadIcons[1].Reload();
            mHeadIcons[1].SetMode(3);
            mHeadIconPlaces[1] = ICON_AT_BODY;
            return;
        }
        if (mFrame == 1) {
            gSoundManager.StartSeqArc(1, 27, 9);
            gSoundManager.StopSeq(0, 1);
        } else if (GetAnimation()->mFinished) {
            SetFace(9);
            SetState(0x8E);
        }
        break;
    case 0x8E:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x8F:
        if (mEntered) {
            mEntered = FALSE;
            mUnknown2B28 = 0;
            SetFace(8);
            mShadowVisible = TRUE;
            UpdateCameraAngle();
            if (IsFacingCameraClosely()) {
                PlayAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood], FALSE);
                UpdateAnimSpeed();
                mStep = 1;
                return;
            } else {
                PlayAnimation(data_ov075_02182d60[gPersonalityKinds[mPersonality]][mMood], FALSE);
                UpdateAnimSpeed();
                ResetSoundPhase(0);
                mUnknown2EA8 = mContext->mUnknown54->GetEyeFloorPos();
                SetTargetFloorPos(mUnknown2EA8);
                mStep = 0;
            }
            return;
        }
        switch (mStep) {
        case 0:
            if (TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            MoveAlongAngle();
            PlaySoundOnce(1, 0, 26);
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            TurnHeadToCamera();
            mFrame = 0;
            mStep++;
            break;
        case 2:
            if (mFrame >= 35) {
                mStep++;
            }
            break;
        case 3:
            Speak(0, 0x1CD, 0);
            mStep++;
            break;
        case 4:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x8B);
            break;
        }
        break;
    case 0x90:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x160, 2);
        }
        break;
    case 0x91:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x162, 2);
        }
        break;
    case 0x92:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x164, 2);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        gSceneManager.RequestScene(0xAF);
        gSceneMemory.mCharacters[0] = mTroubleTarget;
        gSceneMemory.mCharacters[1] = mIndex;
        break;
    case 0x93:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            gSoundManager.MoveSeqVolume(0, 0, 10);
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x6C);
            gSoundManager.StartSeq(0, 0x7C);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1A7, SpeechWindow::MODE_FRAMES, 120);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 120) {
                return;
            }
            Speak(0, 0x168, 0);
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x94:
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
            if (mFrame < 40) {
                return;
            }
            Speak(0, 0x16A, 0);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x95:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x16C, 2);
        }
        break;
    case 0x96:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x16E, 2);
        }
        break;
    case 0x97:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x170, 2);
        }
        break;
    case 0x98:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x172, 0);
            mStep = 0;
            mFrame = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 60);
            break;
        case 1:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 52) {
                return;
            }
            gSceneManager.RequestScene(0x5A);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0x99:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 45) {
                return;
            }
            Speak(0, 0x174, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 60);
            break;
        case 2:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mFrame < 52) {
                return;
            }
            gSceneManager.RequestScene(0x5A);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0x9A:
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
            SetState(0x9B);
            break;
        }
        break;
    case 0x9B:
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
            mCover.SetKind(Scene40Cover::KIND_CURTAIN);
            mCover.SetPosition(mPos, mAnim.mRotY);
            mCover.Appear();
            gSoundManager.StartSeqArc(1, 10, 1);
            mStep++;
            break;
        case 1:
            if (!mCover.HasAppeared()) {
                return;
            }
            mCover.PlayWait();
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
            mCover.Disappear();
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (!mCover.HasDisappeared()) {
                return;
            }
            mCover.mModel.mVisible = FALSE;
            SetState(0x99);
            break;
        }
        break;
    case 0x9C:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(13);
            BlendToNamedAnimation("idle00");
            mTtsPlayer->StopEngine();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 45) {
                return;
            }
            SetFace(8);
            Speak(0, 0x176, 2);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            mStep++;
            break;
        case 1:
            break;
        }
        break;
    case 0x9D:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x178, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0x9E:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x17A, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0x9F:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x17C, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0xA0:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x17E, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0xA1:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(10);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][1]);
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
            mStep++;
            Speak(0, 0x182, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xAB);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0xA2:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 10;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x184, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0);
            break;
        }
        break;
    case 0xA3:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][0]);
            TurnHeadToCamera();
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            mFrame = 0;
            mStep = 0;
            return;
        }
        if (mFrame < 72) {
            return;
        }
        SetState(0xA4);
        break;
    case 0xA4:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][0]);
            TurnHeadToCamera();
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
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
            mStep++;
            Speak(0, 0x186, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xB2);
            gSceneMemory.mCharacters[0] = mTroubleTarget;
            gSceneMemory.mCharacters[1] = mIndex;
            break;
        }
        break;
    case 0xA5:
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
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x6F);
            gSoundManager.StartSeq(0, 0x7F);
            gSoundManager.LoadGroup(0x4D);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1A9, SpeechWindow::MODE_MANUAL, 120);
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x7F) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            SetFace(8);
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x188, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xA6:
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
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x70);
            gSoundManager.StartSeq(0, 0x80);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("sign00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(4);
            mStep++;
            mFrame = 0;
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1AA, SpeechWindow::MODE_MANUAL, 120);
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x80) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x18A, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xA7:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 10;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x18C, 1, mTroubleTarget, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 120);
            break;
        case 2:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mFrame < 112) {
                return;
            }
            gSceneManager.RequestScene(0xB3);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0xA8:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 45;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x18E, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xB5);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0xA9:
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
        case 0: {
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x71);
            gSoundManager.StartSeq(0, 0x81);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("sign00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(4);
            mStep++;
            mFrame = 0;
            SavedCharacter other;
            wchar_t otherName[11];
            wchar_t name[11];
            GameData_ReadCharacter(&gGameData, mTroubleTarget, &other);
            MiiData_GetName(this, name);
            MiiData_GetName(&other, otherName);
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->SetStringArg(name, 0);
            mContext->mSpeechWindow->SetStringArg(otherName, 1);
            mContext->mSpeechWindow->Show(0x1AD, SpeechWindow::MODE_MANUAL, 120);
            break;
        }
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x81) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x190, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAA:
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
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x72);
            gSoundManager.StartSeq(0, 0x82);
            gSoundManager.LoadGroup(0x4D);
            SetFace(8);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1AB, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x82) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x192, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAB:
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
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x73);
            gSoundManager.StartSeq(0, 0x83);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("sign00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(4);
            mStep++;
            mFrame = 0;
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1AA, SpeechWindow::MODE_MANUAL, 120);
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x83) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x194, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAC:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0xAD:
        if (mEntered) {
            mEntered = FALSE;
            func_ov075_02176e74(this);
        }
        break;
    case 0xAE:
        if (mEntered) {
            mEntered = FALSE;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            func_ov075_02176eb8(this);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAF:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 23;
            if (mFrame < mWait) {
                return;
            }
            if (wcslen(mTaughtWord) <= 13) {
                Speak(0, 0x118, 6, 0, 2);
            } else {
                Speak(0, 0x11A, 6, 0, 2);
            }
            mStep++;
            break;
        case 1:
            break;
        }
        break;
    case 0xB0:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x11C, 0);
            switch (mTroubleId) {
            case 0:
                wcscpy(mMotto, mTaughtWord);
                break;
            case 1:
                wcscpy(mMottos[0], mTaughtWord);
                break;
            case 2:
                wcscpy(mMottos[1], mTaughtWord);
                break;
            case 3:
                wcscpy(mMottos[2], mTaughtWord);
                break;
            case 4:
                wcscpy(mMottos[3], mTaughtWord);
                break;
            }
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0xB1:
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
            if (mFrame < 40) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x74);
            gSoundManager.StartSeq(0, 0x84);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("levelup00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(1);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B4, SpeechWindow::MODE_FRAMES, 120);
            SetFace(8);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 120) {
                return;
            }
            BlendToNamedAnimation(data_ov075_02182d10[gPersonalityKinds[mPersonality]][mMood]);
            Speak(0, 0x180, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xB2:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    }
}

void Scene40Balloon::Draw() {
    mModel.Draw();
}

void Scene40Cover::Draw() {
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
