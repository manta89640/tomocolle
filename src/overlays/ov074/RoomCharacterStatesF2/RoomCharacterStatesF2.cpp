#include "../RoomCharacter.h"

void RoomCharacter::UpdateStatesF2To103() {
    switch (mState) {
    case 0xF2:
        if (mStep == 0) {
            if (StepWalkTo(mPartner->GetFloorPos())) {
                mStep++;
            }
        }
        break;
    case 0xF3:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                func_ov074_021694e8(this);
                if (Character_TurnToWalkTarget(this, 0x100, 0)) {
                    mStep += 2;
                } else {
                    mStep++;
                    func_ov074_02169b00(this);
                    SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                }
                Character_SetFace(this, GetFace());
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 3);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mHeadPitch = func_ov074_02169318(this, 0, 0x1800);
            if (func_ov074_0216943c(this, 0x100, 0, 0x3800)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), 0, 0, 3);
            } else {
                func_02091fbc(this, 1, 0, 0x1A);
            }
        }
        Character_MoveAlongAngle(this);
        break;
    case 0xF4:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, 0x31);
                Character_SetFace(this, GetFace());
                mTimer = 70;
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x100, 0);
            Character_MoveAlongAngle(this);
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 3, 10);
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    case 0xF5:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_021694e8(this);
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mTimer = ((200 - func_02098ec4(this)) >> 3) + 25;
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else if (mStep == 1) {
            mTimer--;
            if (func_ov074_0216943c(this, 0x180, 4, 0x8000) || mTimer <= 0) {
                mStep++;
            }
        } else if (mStep == 2) {
            if (ClampToWalkArea(NULL) || func_ov074_02169388(this, 0x8000, 0, 0x23D, 0x1B, 6)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    case 0xF6:
        StepAnimationFacingPartner(GetIdleAnim());
        break;
    case 0xF7:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x55);
                Character_SetFace(this, 0x15);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), 0, 0, 4);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(13);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xF8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x56);
                Character_SetFace(this, 0x16);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(5);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xF9:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 3);
                Character_SetFace(this, 0x17);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), 0, 0, 4);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFA:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x2C);
                Character_SetFace(this, 0x18);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFB:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x37);
                Character_SetFace(this, 0x19);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(2);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFC:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x36);
                Character_SetFace(this, 0x15);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFD:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3E);
                Character_SetFace(this, 0x15);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFE:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x57);
                Character_SetFace(this, 0x18);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFF:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x48);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 0x13);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                HeadIcon_SetMode(&mHeadIcon, 1);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x49);
                SetHeadIcon(10);
                Character_ResetSoundPhase(this, 0);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 5, 2);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 6, 1);
        }
        break;
    case 0x100:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x4A);
                Character_SetFace(this, 2);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                HeadIcon_SetMode(&mHeadIcon, 1);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 4, 2);
        }
        break;
    case 0x101:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x38);
                Character_SetFace(this, 0x10);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        }
        break;
    case 0x102:
        if (mStep == 0) {
            mStep++;
        }
        break;
    case 0x103:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 16;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, 0x39);
                    func_0208ef28(&mModel, 1, 0, 0);
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 16;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetState(0);
                }
            }
        }
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateStates104To105() {
    switch (mState) {
    case 0x104:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 30;
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, 7);
                Character_SetFace(this, 8);
                func_ov074_0216950c(this, 1);
                HeadIcon_SetMode(&mHeadIcon, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3A);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 140;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;
    case 0x105:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 30;
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 60;
            }
        } else if (mStep == 2) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 100;
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

void RoomCharacter::UpdateStates106To10B() {
    switch (mState) {
    case 0x106:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                SetTargetFloorPos(gRoomPoints[4]);
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (func_ov074_02169388(this, 0x3800, 0, 0x199, 0x22, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[6]);
                mHeadTurnFrames = 3;
            }
        } else if (mStep == 2) {
            if (func_ov074_0216943c(this, 0x100, 0, 0x3800)) {
                mStep++;
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else if (mStep == 4) {
            mStep++;
            func_020a4144(&mRoom->mParts, 1);
        } else if (mStep == 5) {
            if (mRoom->mParts.mDoor.mMotion == 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetTargetFloorPos(gRoomPoints[5]);
            }
        } else if (mStep == 6) {
            if (func_ov074_02169388(this, 0x8000, 0, 0x28F, 0x51, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[10]);
            }
        } else if (mStep == 7) {
            mStep++;
            func_020a4144(&mRoom->mParts, 2);
        } else {
            func_ov074_02169388(this, 0x8000, 0, 0x28F, 0x28, 6);
            if (mRoom->mParts.mDoor.mMotion == 0) {
                mPresent = FALSE;
                SetState(0);
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        }
        break;
    case 0x107:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                Character_SetWalkTarget(this, data_ov074_0216d4d8);
                mSavedTarget = GetTarget();
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 4);
            }
        } else if (mStep == 1) {
            BOOL near = FALSE;
            if (IsWithinReach(mPartner->GetFloorPos(), 0x1800, 0x2333)) {
                func_ov074_021691d4(this, 0, func_ov074_021691fc(this, GetTargetFloorPos(), 0x3800), 0, mHeadTurnFrames);
                func_0209171c(this, mPartner->GetFloorPos());
                near = TRUE;
            }
            if (func_ov074_02169388(this, 0x8000, 0, 0x199, 0x22, 6)) {
                mStep++;
                if (mMood != 2) {
                    SetTargetFloorPos(mPartner->GetFloorPos());
                } else {
                    Character_SetWalkTarget(this, data_ov074_0216d490);
                }
                mSavedTarget = GetTarget();
                mHeadTurnFrames = 3;
            }
            Character_SetWalkTarget(this, mSavedTarget);
            if (!near) {
                func_ov074_021691d4(this, 0, func_ov074_021691b0(this, -mWalkAngle, 0x3800), 0, mHeadTurnFrames);
            }
        } else if (mStep == 2) {
            if (func_ov074_0216943c(this, 0x100, 0, 0x3800)) {
                mStep++;
                func_ov074_021691d4(this, 0, 0, 0, mHeadTurnFrames);
            }
        } else if (mStep == 3) {
            if (mPartner->mModel.mBlendFrames <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                if (mMood != 2) {
                    if (!GetFlagOfPartnerInline()) {
                        SetAnimation(RESOURCE_BCA0, 0x3A);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x39);
                    }
                    func_0208ef28(&mModel, 1, 0, 0);
                    mPartner->SetState(0x108);
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 4) {
            if (mMood != 2) {
                if (mModel.mAnimId == 0x3A) {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
                } else if (mModel.mAnimId == 0x39) {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
                }
                if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                    if (mPartner->mModel.mBlendFrames <= 0) {
                        mStep++;
                        SetTargetFloorPos(gRoomPoints[0]);
                        func_ov074_021694e8(this);
                        SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                        mPartner->SetState(0x10A);
                    }
                }
            } else {
                mStep++;
                SetTargetFloorPos(gRoomPoints[2]);
                mPartner->SetState(0x10A);
            }
        } else if (mStep == 5) {
            if (func_ov074_0216943c(this, 0x180, 4, 0x3800)) {
                mStep++;
                func_ov074_021691d4(this, 0, 0, 0, mHeadTurnFrames);
            }
        } else if (mStep == 6) {
            mStep++;
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            func_020a4144(&mRoom->mParts, 1);
        } else if (mStep == 7) {
            if (mRoom->mParts.mDoor.mMotion == 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetTargetFloorPos(gRoomPoints[3]);
            }
        } else if (mStep == 8) {
            if (func_ov074_02169388(this, 0x8000, 0, 0x28F, 0x51, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[7]);
            }
        } else if (mStep == 9) {
            mStep++;
            func_02091fbc(this, 1, 0, 0x1A);
            func_020a4144(&mRoom->mParts, 2);
        } else {
            func_ov074_02169388(this, 0x8000, 0, 0x28F, 0x28, 6);
            if (mRoom->mParts.mDoor.mMotion == 0) {
                mPresent = FALSE;
                SetState(0);
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        }
        break;
    case 0x108:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                if (!GetFlagOfPartnerInline()) {
                    SetAnimation(RESOURCE_BCA0, 0x3A);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                SetState(0);
            }
        }
        break;
    case 0x109:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 3);
            }
        } else if (mStep == 1) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            if (func_ov074_0216943c(this, 0xD0, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                    ShowHeadIcon();
                }
                func_ov074_021691d4(this, 0, 0, 0, 3);
            }
        } else if (mStep == 2) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x100, 0);
            Character_MoveAlongAngle(this);
        }
        break;
    case 0x10A:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    case 0x10B:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mSavedTarget = GetTarget();
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else if (mStep == 1) {
            if (IsWithinReach(mPartner->GetFloorPos(), 0x1B33, 0x2000)) {
                func_0209171c(this, mPartner->GetFloorPos());
            }
            if (func_ov074_02169388(this, 0x3800, 0, 0x23D, 0x28, 5)) {
                mStep++;
                mHeadTurnFrames = 3;
            }
            Character_SetWalkTarget(this, mSavedTarget);
        } else {
            SetTargetFloorPos(mPartner->GetFloorPos());
            if (func_ov074_0216943c(this, 0x120, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    SetState(0x10A);
                }
            }
        }
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateStates10CTo112() {
    switch (mState) {
    case 0x10C:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mAnmObj->frame = 0;
                Character_SetFace(this, GetFace());
                SetPos(data_ov074_0216d4a8);
                Character_SetAngle(this, 0x4000);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                mUnknown25D4 = 0;
                mUnknown25D6 = 0;
                mUnknown25D8 = 0;
                func_ov074_0216950c(this, 1);
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mRoom->mParts, 1);
            SetTargetFloorPos(gRoomPoints[8]);
            ShowHeadIcon();
        } else if (mStep == 2) {
            if (func_ov074_02169388(this, 0x8000, 0, 0x23D, 0x28, 6)) {
                mStep++;
                func_020a4144(&mRoom->mParts, 2);
            }
        } else if (mStep == 3) {
            mStep++;
            FloorPos pos;
            pos.x = MATH_Rand16(&gRandom, 16) * 0x400 - 0x1E66;
            pos.z = MATH_Rand16(&gRandom, 16) * 0x385 - 0x1999;
            SetTargetFloorPos(pos);
            func_02091fbc(this, 1, 0, 0x1A);
            mHeadTurnFrames = 5;
        } else {
            if (func_ov074_02169388(this, 0x2000, 0, 0x23D, 0x25, 6)) {
                SetState(0x111);
            }
        }
        break;
    case 0x10D:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                SetPos(data_ov074_0216d4c0);
                Character_SetAngle(this, 0x4000);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                mUnknown25D4 = 0;
                mUnknown25D6 = 0;
                mUnknown25D8 = 0;
                func_ov074_0216950c(this, 1);
            }
        }
        break;
    case 0x10E:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                SetPos(data_ov074_0216d4cc);
                Character_SetAngle(this, 0x4000);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                mUnknown25D4 = 0;
                mUnknown25D6 = 0;
                mUnknown25D8 = 0;
                func_ov074_0216950c(this, 1);
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mRoom->mParts, 1);
            SetTargetFloorPos(gRoomPoints[9]);
            ShowHeadIcon();
        } else if (mStep == 2) {
            if (func_ov074_02169388(this, 0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[14]);
                mHeadTurnFrames = 4;
            }
        } else if (mStep == 3) {
            if (func_ov074_02169388(this, 0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                SetTargetFloorPos(mPartner->GetFloorPos());
                func_020a4144(&mRoom->mParts, 2);
                mHeadTurnFrames = 3;
            }
            func_ov074_021691d4(this, 0, func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3800), 0, mHeadTurnFrames);
        } else if (mStep == 4) {
            if (func_ov074_0216943c(this, 0x120, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                    func_ov074_021691d4(this, 0, 0, 0, mHeadTurnFrames);
                }
            }
        } else if (mStep == 5) {
            if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                if (!GetFlagOfPartnerInline()) {
                    SetAnimation(RESOURCE_BCA0, 0x3A);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                mPartner->SetState(0x10F);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    SetState(0x111);
                    mPartner->SetState(0x111);
                }
            }
        }
        break;
    case 0x112:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mSavedTarget = GetTarget();
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else {
            if (func_ov074_02169388(this, 0x8000, 0, 0x23D, 0x22, 5)) {
                SetState(0x111);
            }
            Character_SetWalkTarget(this, mSavedTarget);
        }
        break;
    case 0x10F:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                if (!GetFlagOfPartnerInline()) {
                    SetAnimation(RESOURCE_BCA0, 0x3A);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                SetState(0);
            }
        }
        break;
    case 0x110:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 3);
            }
        } else if (mStep == 1) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            if (func_ov074_0216943c(this, 0x140, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                }
                func_ov074_021691d4(this, 0, 0, 0, mHeadTurnFrames);
            }
        } else {
            SetTargetFloorPos(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x60, 0);
            Character_MoveAlongAngle(this);
        }
        break;
    case 0x111:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                func_ov074_0216950c(this, 1);
                func_ov074_021691d4(this, 0, 0, 0, 5);
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

void RoomCharacter::UpdateStates113To116() {
    switch (mState) {
    case 0x113:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
                Character_SetFace(this, GetFaceOfMood(0));
                mHeadTurnActive = 1;
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 90;
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, 0);
                if (func_02099e34(mEvent)) {
                    Character_SetFace(this, 5);
                }
            }
        } else if (mStep == 2) {
            mTimer--;
            if (mTimer <= 0) {
                Character_SetFace(this, GetFaceOfMood(0));
                if (mSlot == 0) {
                    SetState(0);
                } else {
                    mStep++;
                }
            }
        }
        LookAtCamera(3);
        break;
    case 0x114:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(RESOURCE_BCA0, 10);
                mTargetAngle = mAngle - mUnknown25D6;
                ShowHeadIcon();
            }
        } else {
            mHeadTurnActive = 1;
            func_02091fbc(this, 1, 0, 0x1A);
            if (Character_TurnToTargetAngle(this, 0xE0, 0x10)) {
                SetState(0);
            }
            Character_MoveAlongAngle(this);
        }
        LookAtCamera(2);
        break;
    case 0x115:
    case 0x116:
        if (mStep == 0) {
            mStep++;
            if (mState == 0x115) {
                if (mUnknown2AF0 < 0x1000) {
                    mTimer = 40;
                } else {
                    mTimer = 30;
                }
            } else {
                mTimer = 10;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(0);
                func_ov074_02169b00(this);
                SetAnimation(RESOURCE_BCA0, 7);
            }
            mHeadTurnActive = 1;
            func_ov074_02169388(this, 0x8000, 0, 0x28F, 0x1D, 5);
        }
        LookAtCamera(2);
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateState117() {
    if (mState == 0x117) {
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            func_ov074_021691d4(this, 0, 0, 0, 5);
        }
    } else {
        SetState(0);
    }
    PlaceHeadIcon();
}

int RoomCharacter::StepWalkToRandomPos() {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            FloorPos pos;
            GetRandomPosInArea(&pos);
            SetTarget(pos.x, 0, pos.z);
            func_ov074_02169b00(this);
            func_ov074_021694e8(this);
            mSubStep++;
            SetAnimation(RESOURCE_BCA0, 0x14);
            Character_SetFace(this, 8);
            func_ov074_0216950c(this, 1);
            mHeadTurnFrames = 5;
            ShowHeadIcon();
        }
    } else {
        done = func_ov074_02169388(this, 0x3800, 0, 0x199, 0x22, 6);
    }
    return done;
}

int RoomCharacter::StepWalkTo(FloorPos pos) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            func_ov074_02169b00(this);
            func_ov074_021694e8(this);
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, GetFace());
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 4);
        }
    } else {
        SetTargetFloorPos(pos);
        if (func_ov074_02169388(this, 0x3800, 0, 0x23D, 0x22, 5)) {
            done = 1;
        }
    }
    return done;
}

int RoomCharacter::StepTurnWithAnimation(int anim) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            func_ov074_02169b00(this);
            SetAnimation(RESOURCE_BCA0, anim);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
            Character_SetFace(this, GetFace());
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 4);
        }
    } else if (mSubStep == 1) {
        func_02091fbc(this, 1, 0, 0x1A);
        if (Character_TurnToTargetAngle(this, 0x120, 0)) {
            mSubStep++;
            done = 1;
        } else {
            func_ov074_021691d4(this, 0, func_ov074_021691b0(this, -mTargetAngle, 0x3800), 0, 4);
        }
        Character_MoveAlongAngle(this);
    } else {
        done = 1;
    }
    return done;
}

int RoomCharacter::StepAlternateAnimations(int animA, int animB) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            mCounter = 0;
            func_ov074_02169b00(this);
            mTimer4 = MATH_Rand16(&gRandom, 80) + 250;
            if (MATH_Rand16(&gRandom, 0) & 1) {
                SetAnimation(RESOURCE_BCA0, animA);
            } else {
                SetAnimation(RESOURCE_BCA0, animB);
            }
            if (MATH_Rand16(&gRandom, 0) & 1) {
                Character_SetFace(this, 8);
            } else {
                Character_SetFace(this, 10);
            }
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
            if (mModel.mBlendFrames > 0) {
                func_0208ef28(&mModel, 1, 0, 0);
                Character_ResetSoundPhase(this, 0);
            } else {
                func_0208ef00(&mModel, 1, 0);
            }
        }
    } else {
        mTimer4--;
        if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
            mCounter++;
            if (mCounter >= 2) {
                mCounter = 0;
                if (mTimer4 > 0) {
                    if (animA == mModel.mAnimId) {
                        SetAnimation(RESOURCE_BCA0, animB);
                    } else {
                        SetAnimation(RESOURCE_BCA0, animA);
                    }
                    Character_ResetSoundPhase(this, 0);
                } else {
                    Character_SetFace(this, 8);
                    done = 1;
                }
            } else {
                Character_ResetSoundPhase(this, 0);
            }
            func_0208ef28(&mModel, 1, 0, 0);
        }
    }
    return done;
}

int RoomCharacter::StepAnimationFacingPartner(int anim) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            func_ov074_02169b00(this);
            SetAnimation(RESOURCE_BCA0, anim);
            Character_SetFace(this, GetFace());
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
            if (mModel.mAnimId == 0x32 && mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        }
    } else if (mSubStep == 1) {
        mSubStep++;
        u16 angle = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3800);
        func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), angle, 0, 4);
    } else {
        if (mModel.mAnimId == 0x32) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 7, 2);
        }
        done = 1;
    }
    return done;
}

int RoomCharacter::StepIdleThenAnimation55() {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            func_ov074_02169b00(this);
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        }
    } else if (mSubStep == 1) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            SetAnimation(RESOURCE_BCA0, 0x55);
            Character_SetFace(this, 0x15);
            u16 angle = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3800);
            func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), angle, 0, 3);
            Character_ResetSoundPhase(this, 1);
        }
    } else {
        Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 1);
        done = 1;
    }
    return done;
}

int RoomCharacter::StepRandomAnimationFacingPartner() {
    int anim = MATH_Rand16(&gRandom, 100);
    if (anim >= 50) {
        anim = 0x32;
    } else {
        anim = GetIdleAnim();
    }
    return StepAnimationFacingPartner(anim);
}

int RoomCharacter::StepAnimationThenFacePartner(int anim) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            func_ov074_02169b68(this);
            SetAnimation(RESOURCE_BCA0, anim);
            Character_SetFace(this, 0x15);
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            Character_ResetSoundPhase(this, 1);
        }
    } else if (mSubStep == 1) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            u16 angle = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000);
            func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), angle, 0, 4);
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 1);
        }
    } else {
        Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 1);
        done = 1;
    }
    return done;
}

void RoomCharacter::StartAnimationWithIcon(int anim) {
    SetAnimation(RESOURCE_BCA0, anim);
    SetHeadIcon(6);
    func_ov074_0216950c(this, 1);
}

int RoomCharacter::StepChangeItemsThenAnimation(int anim, int itemA, int itemB) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            func_ov074_02169b00(this);
            SetAnimation(RESOURCE_BCA0, 7);
            StepChangeHeldItems(itemA, itemB);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        }
        mHeldItems[0]->mVisible = 0;
        mHeldItems[1]->mVisible = 0;
    } else if (mSubStep == 1) {
        int changed = StepChangeHeldItems(itemA, itemB);
        if (mModel.mBlendFrames <= 0 && changed) {
            mSubStep++;
            SetAnimation(RESOURCE_BCA0, anim);
            func_0208ef28(&mModel, 1, 0, 0);
            Character_SetFace(this, 8);
        }
        mHeldItems[0]->mVisible = 0;
        mHeldItems[1]->mVisible = 0;
    } else if (mSubStep == 2) {
        mSubStep++;
        if (itemA != 0xFF) {
            mHeldItems[0]->mVisible = 1;
        }
        if (itemB != 0xFF) {
            mHeldItems[1]->mVisible = 1;
        }
        done = 1;
    } else {
        done = 1;
    }
    return done;
}

int RoomCharacter::StepChangeHeldItems(int itemA, int itemB) {
    u8 items[2];
    items[0] = itemA;
    items[1] = itemB;
    int done = 0;
    if (mSubSubStep == 0) {
        if (func_ov074_02169564(this)) {
            mSubSubStep++;
            for (int i = 0; i < 2; i++) {
                if (items[i] != 0xFF) {
                    mHeldItems[i]->SetKind(items[i]);
                } else {
                    mHeldItems[i]->SetShowState(0);
                    mHeldItems[i]->mVisible = 0;
                }
            }
        }
    } else if (mSubSubStep == 1) {
        if (func_ov074_02169564(this)) {
            int ready = 0;
            mSubSubStep++;
            for (int i = 0; i < 2; i++) {
                GXRgb color = func_02073514(func_0207a6c0(this));
                NNS_G3dMdlSetMdlDiff(mHeldItems[i]->mResMdl, 0, color);
                mHeldItems[i]->SetRotation(0, 0, 0);
                MTX_Identity33(&mHeldItems[i]->mRotation);
                if (items[i] != 0xFF) {
                    if (mHeldItems[i]->mVisible) {
                        ready++;
                    }
                } else {
                    if (!mHeldItems[i]->mVisible) {
                        ready++;
                    }
                }
            }
            if (ready == 2) {
                done = 1;
            }
        }
    } else if (mSubSubStep == 2) {
        mSubSubStep++;
        for (int i = 0; i < 2; i++) {
            if (items[i] != 0xFF) {
                mHeldItems[i]->mVisible = 1;
            } else {
                mHeldItems[i]->SetShowState(0);
                mHeldItems[i]->mVisible = 0;
            }
        }
        done = 1;
    } else {
        done = 1;
    }
    return done;
}

void RoomCharacter::UpdateBodyIfPresent() {
    if (mPresent) {
        func_02091968(this);
    }
}

int RoomCharacter::PickReaction() {
    return func_0209d804(this, mReaction);
}

void RoomCharacter::ResetBody() {
    func_02091a6c(this);
    func_020910d4(this);
}

u8 RoomCharacter::GetFlagOfPartner() {
    return GetFlagOfPartnerInline();
}

int RoomCharacter::GetFaceOfMood(int variant) {
    return gMoodFaces[variant];
}

int RoomCharacter::GetFace() {
    return gMoodFaces[mMood];
}

void RoomCharacter::GetMidpoint(PositionedNode *a, PositionedNode *b, VecFx32 *out) {
    out->x = (a->mPos.x + b->mPos.x) >> 1;
    out->y = (a->mPos.y + b->mPos.y) >> 1;
    out->z = (a->mPos.z + b->mPos.z) >> 1;
}

void RoomCharacter::SetAnimation(u32 resType, int id) {
    func_020919e8(this, resType, id);
    func_0208ef00(&mModel, 0, 0);
}

BOOL RoomCharacter::IsOneTwoOrFour(int value) {
    switch (value) {
    case 1:
    case 2:
    case 4:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsState8Or9() {
    switch (mState) {
    case 8:
    case 9:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsState10To12() {
    switch (mState) {
    case 0x10:
    case 0x11:
    case 0x12:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsAnimOfGroupB(int anim) {
    switch (anim) {
    case 0x1E:
    case 0x1F:
    case 0x27:
    case 0x3D:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x5B:
    case 0x5C:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsWalkAnim() {
    int anim = mModel.mAnimId;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (anim == data_020fb898[i][j]) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL RoomCharacter::IsMovingAnim() {
    switch (mModel.mAnimId) {
    case 0x22:
    case 0x35:
    case 0x42:
        return TRUE;
    }
    return IsWalkAnim();
}

BOOL RoomCharacter::IsAnimOfGroupA(int anim) {
    switch (anim) {
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x24:
    case 0x33:
    case 0x3F:
    case 0x40:
    case 0x53:
        return TRUE;
    }
    return FALSE;
}

void RoomCharacter::GetRandomPosInArea(FloorPos *out) {
    out->x = gWalkArea.x + MATH_Rand16(&gRandom, gWalkArea.w);
    out->z = gWalkArea.z + MATH_Rand16(&gRandom, gWalkArea.h);
}

void RoomCharacter::GetRandomPos(FloorPos *out) {
    out->x = MATH_Rand16(&gRandom, 0x4000) - 0x2000;
    out->z = MATH_Rand16(&gRandom, 0x4800) - 0x2800;
}

BOOL RoomCharacter::ClampToRect(FloorPos *pos, const FloorRect *rect) {
    fx32 left = rect->x;
    fx32 top = rect->z;
    fx32 right = left + rect->w;
    fx32 bottom = top + rect->h;
    BOOL clamped = FALSE;
    if (pos->x < left) {
        pos->x = left;
        clamped = TRUE;
    } else if (pos->x > right) {
        pos->x = right;
        clamped = TRUE;
    }
    if (pos->z < top) {
        pos->z = top;
        clamped = TRUE;
    } else if (pos->z > bottom) {
        pos->z = bottom;
        clamped = TRUE;
    }
    return clamped;
}

BOOL RoomCharacter::ClampToWalkArea(FloorPos *pos) {
    if (pos == NULL) {
        FloorPos own = GetFloorPos();
        return ClampToRect(&own, &gWalkArea);
    }
    return ClampToRect(pos, &gWalkArea);
}

BOOL RoomCharacter::ClampToRect4E4(FloorPos *pos) {
    if (pos == NULL) {
        FloorPos own = GetFloorPos();
        return ClampToRect(&own, &data_ov074_0216d4e4);
    }
    return ClampToRect(pos, &data_ov074_0216d4e4);
}

BOOL RoomCharacter::ClampToRect534(FloorPos *pos) {
    if (pos == NULL) {
        FloorPos own = GetFloorPos();
        return ClampToRect(&own, &data_ov074_0216d534);
    }
    return ClampToRect(pos, &data_ov074_0216d534);
}

void RoomCharacter::GetRandomCorner(FloorPos *out) {
    *out = gWalkAreaCorners[MATH_Rand16(&gRandom, 0) % 4u];
}

// Not matching yet: 0xf8 bytes against 0xfc; the instructions are the same but for register allocation in the
// two tails. The original loads the area's w/h into r2 and the generator's x/mul/add into r4/ip/lr (which in the
// second tail splits the `ldm` into `ldr` + `ldm`, the extra instruction); this loads w/h into lr/r4 and x into r2.
// Tried: the area as a direct global, a local pointer, a reference, an fx32 array, {min, size}; u32/int fields;
// if/else instead of the ternary; x read before the second random number; locals for the range and the result;
// int and u16 wrappers around MATH_Rand16; inline helpers for the two halves; the arms swapped; a switch.
void RoomCharacter::GetRandomEdgePos(FloorPos *out) {
    FloorRect *area = &gWalkArea;
    if (MATH_Rand16(&gRandom, 0) & 1) {
        out->x = (MATH_Rand16(&gRandom, 0) & 1) ? area->x : -area->x;
        out->z = MATH_Rand16(&gRandom, area->h) - (area->h >> 1);
    } else {
        out->z = (MATH_Rand16(&gRandom, 0) & 1) ? area->z : -area->z;
        out->x = MATH_Rand16(&gRandom, area->w) - (area->w >> 1);
    }
}
