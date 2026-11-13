#include "../TutorialNames.h"

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "sound/SoundManager.h"

#include "TutorialRoomScene.h"

extern "C" {
// --- Main program, not named yet ---

extern TouchInput data_02121e58;

// The saved game's characters, by index
BOOL func_0202c3dc(GameData *gameData, int index);                           // Whether there is one
void func_0202c264(GameData *gameData, int index, SavedCharacter *character); // Reads one
void func_0202c28c(GameData *gameData, int index, SavedCharacter *character); // Writes one
// Resets the character to nothing, and sets its appearance (the scene does both when the saved game has no such one)
void func_020979e0(SavedCharacter *character);
void func_02098c40(SavedCharacter *character, int kind, int value);
}

#define BCA0 0x30414342

static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// The touch screen: the button and the cursor on the building front. TRUE when the player chose either.
// @0x0214bbe0
BOOL TutorialRoomScene::UpdateTouch() {
    if (mTouchStep == 0) {
        if (data_02121e58.mTouched) {
            ScreenPoint point = data_02121e58.mPoint;
            BOOL handled = FALSE;

            if (mButtons[0].Contains(point)) {
                handled = TRUE;
                mTouchStep++;
                mPressedButton = 0;
                mButtons[0].mState = 1;
                gSoundManager.StartSeqArc(1, 1, 0);
            }
            if (!handled) {
                if (mFront.mCursor.Contains(point)) {
                    gSoundManager.StartSeqArc(2, 9, 0);
                    return TRUE;
                }
            }
        }
    } else {
        if (data_02121e58.mTouching != 0) {
            if (mButtons[mPressedButton].Contains(data_02121e58.mPoint)) {
                mButtons[mPressedButton].mState = 1;
            } else {
                mButtons[mPressedButton].mState = 2;
            }
        } else {
            mTouchStep = 0;
            if (mButtons[mPressedButton].Contains(data_02121e58.mLastPoint)) {
                mButtons[mPressedButton].mState = 3;
                gSoundManager.StartSeqArc(2, 9, 0);
                return TRUE;
            } else {
                mButtons[mPressedButton].mState = 2;
            }
        }
    }
    return FALSE;
}

// The resident looks at the camera, turns to it and comes up to it (states 0x10 to 0x12); then the scene goes on
// to its state 9.
// @0x0214bdc0
void TutorialRoomScene::UpdateEnding() {
    if (mStep == 0) {
        mStep++;
        mCharacters[0].SetState(0x10);
        mCamera.mIgnoreInput = TRUE;
    } else if (mStep == 1) {
        if (mCharacters[0].mState == 0) {
            mStep++;
            mCharacters[0].SetState(0x11);
        }
    } else if (mStep == 2) {
        if (mCharacters[0].mState == 0) {
            mStep++;
            mCharacters[0].SetState(0x12);
        }
    } else if (mStep == 3) {
        if (mCharacters[0].mState == 0) {
            mStep++;
            mTimer = 0;
        }
    } else {
        mTimer++;
        if (mTimer > 10) {
            SetState(9);
            gSoundManager.StartSeqArc(2, 1, 11);
        }
    }
}

// @0x0214bec4
void TutorialRoomScene::ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode) {
    if (first != NULL) {
        mMessage.SetStringArg(first->mNameText, 0);
    }
    if (second != NULL) {
        mMessage.SetStringArg(second->mNameText, 1);
    }
    mMessage.mColor = 1;
    mMessage.SetState(drawMode);
    mMessage.mMessage = message;
}

// @0x0214bf48
BOOL TutorialRoomScene::AreCharactersNear(int first, int second) {
    if (mCharacters[first].IsWithinReach(&mCharacters[second], 0x2000, 0x2999) ||
        mCharacters[second].IsWithinReach(&mCharacters[first], 0x2000, 0x2999)) {
        return TRUE;
    }
    return FALSE;
}

// When the resident has nothing to do: stand for two seconds, or walk to a random place for three.
// @0x0214c004
void TutorialRoomScene::UpdateIdle() {
    if (mCharacters[0].mWait > 0 && mCharacters[0].mState != 0) {
        return;
    }
    if (Rand(100) < 70) {
        mCharacters[0].SetState(2);
        mCharacters[0].mWait = 120;
    } else {
        mCharacters[0].SetState(3);
        mCharacters[0].mWait = 180;
        FloorPos pos;
        func_ov073_0214de4c(&mCharacters[0], &pos);
        mCharacters[0].SetWalkTarget(pos.x, 0, pos.z);
    }
}

// @0x0214c0c8
void TutorialRoomScene::SaveCharacter(SavedCharacter *character) {
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c28c(&gGameData, index, character);
    }
}

// @0x0214c104
void TutorialRoomScene::SaveCharacters() {
    SaveCharacter(&mCharacters[0]);
    SaveCharacter(&mCharacters[1]);
}

// @0x0214c128
void TutorialRoomScene::LoadCharacter(SavedCharacter *character) {
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c264(&gGameData, index, character);
    } else {
        func_020979e0(character);
        func_02098c40(character, 40, index & 7);
        character->mIndex = index;
    }
}

// @0x0214c188
void TutorialRoomScene::LoadCharacters() {
    LoadCharacter(&mCharacters[0]);
    LoadCharacter(&mCharacters[1]);
}

// @0x0214c1ac
RoomCharacter::RoomCharacter() {
    mState = mPrevState = mPresent = 0;
    mWait = 0;
    MI_CpuClear16(mNameText, sizeof(mNameText));
    mStepSoundArmed = 0;
}

// @0x0214c20c
void RoomCharacter::SetState(int state) {
    mPrevState = mState;
    mState = state;
    mStep = 0;
}

// @0x0214c22c
void RoomCharacter::Update() {
    if (!mPresent) {
        return;
    }
    int state = mState;
    if (state != 0) {
        if (state <= 4) {
            UpdateStates01To04();
        } else if (state <= 8) {
            UpdateStates05To08();
        } else if (state <= 9) {
            UpdateState09();
        } else if (state <= 0xf) {
            UpdateStates0ATo0F();
        } else if (state <= 0x12) {
            UpdateStates10To12();
        } else {
            SetState(0);
        }
    }
    if (mWait > 0) {
        mWait--;
    }
    func_020910a8(this);
    func_020810e4(&mHeadIcon);
}

// States 1 to 4: standing (1), standing with the head turned somewhere (2), walking to the walk target (3) and
// standing in animation 0x19 (4).
// @0x0214c2d4
void RoomCharacter::UpdateStates01To04() {
    switch (mState) {
    case 1:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                ShowHeadIcon();
            }
        }
        break;
    case 2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                ShowHeadIcon();
                func_ov073_0214db10(this, 0, (0x28 - Rand(0x50)) << 8, 0, 5);
            }
        }
        break;
    case 3:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                ResetSoundPhaseUnlessWalking();
                SetAnimation(BCA0, GetWalkAnim());
                Character_SetFace(this, 8);
                mHeadTurnFrames = 5;
                ShowHeadIcon();
            }
        } else {
            if (func_ov073_0214dcc8(this, 0x3800, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        break;
    case 4:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e1b0(this);
                SetAnimation(BCA0, 0x19);
                Character_SetFace(this, 8);
                ShowHeadIcon();
                func_ov073_0214db10(this, 0, 0, 0, 5);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

// States 5 to 8: walking up to the partner (5), turning to the walk target (6), and two animations with a sound
// (7: animation 0x55 with head icon 13, 8: animation 0x4A).
// @0x0214c5a4
void RoomCharacter::UpdateStates05To08() {
    switch (mState) {
    case 5:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                ResetSoundPhaseUnlessWalking();
                SetAnimation(BCA0, GetWalkAnim());
                Character_SetFace(this, 8);
                ShowHeadIcon();
                mHeadTurnFrames = 4;
            }
        } else if (mStep == 1) {
            SetWalkTargetOnFloor(mPartner->GetFloorPos());
            if (func_ov073_0214dcc8(this, 0x3800, 0, 0x23D, 0x22, 5)) {
                mStep++;
            }
        }
        break;
    case 6:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                ResetSoundPhaseUnlessWalking();
                if (Character_TurnToWalkTarget(this, 0x100, 0)) {
                    mStep += 2;
                } else {
                    mStep++;
                    func_ov073_0214e148(this);
                    SetAnimation(BCA0, GetWalkAnim());
                }
                Character_SetFace(this, 8);
                mHeadTurnFrames = 4;
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mHeadPitch = func_ov073_0214dc58(this, 0, 0x1800);
            if (func_ov073_0214dd7c(this, 0x100, 0, 0x3800)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
                func_ov073_0214e148(this);
                SetAnimation(BCA0, GetIdleAnim());
                func_ov073_0214db10(this, func_ov073_0214dc58(this, 0, 0x1800), 0, 0, 3);
            } else {
                func_02091fbc(this, 1, 0, 0x1A);
            }
        }
        Character_MoveAlongAngle(this);
        break;
    case 7:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(BCA0, 0x55);
                Character_SetFace(this, 0x15);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(13);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(BCA0, 0x4A);
                Character_SetFace(this, 2);
                HeadIcon_SetMode(&mHeadIcon, 1);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 4, 2);
        }
        break;
    }
    PlaceHeadIcon();
}

// State 9: stand for half a second, play animation 0x3A once, then stand for 140 frames.
// @0x0214ca04
void RoomCharacter::UpdateState09() {
    switch (mState) {
    case 9:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 30;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, 7);
                Character_SetFace(this, 8);
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(BCA0, 0x3A);
                func_0208ef00(&mModel, 1, 0);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            if (AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 140;
                SetAnimation(BCA0, GetIdleAnim());
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

// States 10 to 15: coming in by the door (overlay 74's states 0x10C to 0x111).
// @0x0214cbb8
void RoomCharacter::UpdateStates0ATo0F() {
    switch (mState) {
    case 10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mStepSoundArmed = 1;
                SetAnimation(BCA0, GetWalkAnim());
                AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mAnmObj->frame = 0;
                Character_SetFace(this, 8);
                VecFx32 pos = { 0, 0, -0x9000 };
                SetPos(pos);
                Character_SetAngle(this, 0x4000);
                mHeadTurnActive = 0;
                mUnknown25D6 = 0;
                Character_ResetSoundPhase(this, 0);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mApartmentRoom->mParts, 1);
            FloorPos target = data_ov073_02150ba4.mInsideDoor;
            SetWalkTargetOnFloor(target);
        } else if (mStep == 2) {
            if (func_ov073_0214dcc8(this, 0x8000, 0, 0x23D, 0x28, 6)) {
                mStep++;
            }
        } else if (mStep == 3) {
            mStep++;
            func_020a4144(&mApartmentRoom->mParts, 2);
            func_02091fbc(this, 1, 0, 0x1A);
            FloorPos target = data_ov073_02150ba4.mRoomFront;
            SetWalkTargetOnFloor(target);
        } else {
            if (func_ov073_0214dcc8(this, 0x2000, 0, 0x23D, 0x25, 6)) {
                SetState(0);
            }
        }
        break;
    case 11:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mStepSoundArmed = 1;
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                VecFx32 pos = { 0, 0, -0x9000 };
                SetPos(pos);
                Character_SetAngle(this, 0x4000);
                mUnknown25D6 = 0;
                ShowHeadIcon();
                func_ov073_0214db10(this, 0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        }
        break;
    case 12:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mStepSoundArmed = 1;
                SetAnimation(BCA0, GetWalkAnim());
                Character_SetFace(this, 8);
                VecFx32 pos = { 0, 0, -0x9000 };
                SetPos(pos);
                Character_SetAngle(this, 0x4000);
                mUnknown25D6 = 0;
                ShowHeadIcon();
                func_ov073_0214db10(this, 0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mApartmentRoom->mParts, 1);
            FloorPos target = data_ov073_02150ba4.mDoorstep;
            SetWalkTargetOnFloor(target);
        } else if (mStep == 2) {
            if (func_ov073_0214dcc8(this, 0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                FloorPos target = data_ov073_02150ba4.mPastDoor;
                SetWalkTargetOnFloor(target);
                mHeadTurnFrames = 4;
            }
        } else if (mStep == 3) {
            if (func_ov073_0214dcc8(this, 0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                SetWalkTargetOnFloor(mPartner->GetFloorPos());
                func_020a4144(&mApartmentRoom->mParts, 2);
                mHeadTurnFrames = 3;
            }
            func_ov073_0214db10(this, 0, func_ov073_0214db3c(this, mPartner->GetFloorPos(), 0x3800), 0, mHeadTurnFrames);
        } else if (mStep == 4) {
            if (func_ov073_0214dd7c(this, 0x120, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(BCA0, GetIdleAnim());
                    func_ov073_0214db10(this, 0, 0, 0, mHeadTurnFrames);
                }
            }
        } else if (mStep == 5) {
            if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                if (!GetRelationToPartner()) {
                    SetAnimation(BCA0, 0x3A);
                } else {
                    SetAnimation(BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                mPartner->SetState(13);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mFinished) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    SetState(15);
                    mPartner->SetState(15);
                }
            }
        }
        break;
    case 13:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                if (!GetRelationToPartner()) {
                    SetAnimation(BCA0, 0x3A);
                } else {
                    SetAnimation(BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                ShowHeadIcon();
                func_ov073_0214db10(this, 0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mFinished) {
                SetState(0);
            }
        }
        break;
    case 14:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, GetWalkAnim());
                ShowHeadIcon();
                func_ov073_0214db10(this, 0, 0, 0, 3);
            }
        } else if (mStep == 1) {
            SetWalkTargetOnFloor(mPartner->GetFloorPos());
            if (func_ov073_0214dd7c(this, 0x140, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(BCA0, GetIdleAnim());
                }
                func_ov073_0214db10(this, 0, 0, 0, mHeadTurnFrames);
            }
        } else {
            SetWalkTargetOnFloor(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x60, 0);
            Character_MoveAlongAngle(this);
        }
        break;
    case 15:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                func_ov073_0214db10(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

// States 0x10 to 0x12: looking at the camera for a second and a half (0x10), turning to it (0x11) and coming half
// the way up to it (0x12). Overlay 74's states 0x113 to 0x115.
// @0x0214d6b8
void RoomCharacter::UpdateStates10To12() {
    switch (mState) {
    case 0x10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 90;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, 0);
                Character_SetFace(this, 8);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 1;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        TurnHeadToCamera(3);
        break;
    case 0x11:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov073_0214e148(this);
                SetAnimation(BCA0, 10);
                mTargetAngle = mAngle - mUnknown25D6;
                Character_ResetSoundPhase(this, 0);
                ShowHeadIcon();
            }
            TurnHeadToCamera(2);
        } else {
            TurnHeadToCamera(2);
            func_02091fbc(this, 1, 0, 0x1A);
            if (Character_TurnToTargetAngle(this, 0xA0, 0x10)) {
                SetState(0);
            }
            Character_MoveAlongAngle(this);
        }
        break;
    case 0x12:
        if (mStep == 0) {
            mStep++;
            if (mUnknown2AF0 < 0x1000) {
                mTimer = 40;
            } else {
                mTimer = 30;
            }
            VecFx32 offset = mCamera->mPosition;
            VEC_Subtract(&offset, &mModel.mPos, &offset);
            offset.x >>= 1;
            offset.y >>= 1;
            offset.z >>= 1;
            SetWalkTarget(offset.x, 0, offset.z);
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(0);
                func_ov073_0214e148(this);
                SetAnimation(BCA0, 7);
            }
            func_ov073_0214db10(this, mHeadPitch, GetHeadYawToCamera(), 0, mHeadTurnFrames);
            func_ov073_0214dcc8(this, 0x8000, 0, 0x28F, 0x1F, 6);
        }
        break;
    }
    PlaceHeadIcon();
}

// @0x0214dabc
void RoomCharacter::Draw() {
    if (mPresent) {
        func_02091968(this);
    }
}

// @0x0214dad8
void RoomCharacter::CopyName() {
    func_0207a80c(this, mNameText);
}
