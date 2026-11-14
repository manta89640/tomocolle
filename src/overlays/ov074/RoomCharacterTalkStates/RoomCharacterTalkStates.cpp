#include <nitro.h>
#include <nnsys.h>

#include "../RoomCharacter.h"

// Only for state 24 of UpdateTalkStates, see there.
struct RoomCharacterAlias : RoomCharacter {};

// An angle as a signed number, -0x7fff..0x8000.
static inline int SignedAngle(u16 angle) {
    int v = angle;
    if (v > 0x8000) {
        v -= 0x10000;
    }
    return v;
}

void RoomCharacter::UpdateTalkStates() {
    switch (mState) {
    case 1:
    case 2:
    case 3:
    case 4:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            {
                u16 yaw;
                u16 pitch;
                if (mState == 2) {
                    yaw = (0x20 - Random(0x40)) << 8;
                    if (Random(10) < 4) {
                        pitch = (0x10 - Random(0x20)) << 8;
                    } else {
                        pitch = 0;
                    }
                } else {
                    pitch = 0;
                    yaw = SignedAngle(mHeadYaw) >> 1;
                    func_02090c78(this, &yaw, 0x3000);
                }
                SetHeadTurn(pitch, yaw, 0, 5);
            }
        } else if (mStep == 1 && mState == 4) {
            if (!func_02091b80(this, mPartner->mModel.mPos.x, mPartner->mModel.mPos.z, 0x3000) &&
                mPartner->mState != 8 && mPartner->mState != 10 && mPartner->mState != 0x18 &&
                mPartner->mState != 0x19) {
                break;
            }
            {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                u16 pitch;
                if (mMood != 3 && Random(100) < 50) {
                    pitch = GetHeadPitchToPartner(0, 0x1800);
                } else {
                    pitch = 0;
                }
                SetHeadTurn(pitch, yaw, 0, 5);
                if (mPartner->mState == 0x18) {
                    if (mFace != 0x1a) {
                        Character_SetFace(this, 0x1a);
                    }
                } else {
                    int face = mFace;
                    if (face != GetFace()) {
                        Character_SetFace(this, GetFace());
                    }
                }
            }
        }
        break;

    case 5:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                if (mPartner->IsState10To12()) {
                    mPartner->SetState(0x14);
                } else {
                    mPartner->SetState(6);
                }
            }
        } else {
            StepIdleThenAnimation55();
        }
        break;

    case 6:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StepRandomAnimationFacingPartner();
        } else {
            StepRandomAnimationFacingPartner();
        }
        break;

    case 7:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mSpeechParam[mMood] && mTtsPlayer->IsEngineIdle()) {
                mStep++;
                func_0209d898(this, mTtsPlayer);
                mTtsPlayer->Speak(mSpeechText[mMood], 0);
            } else {
                SetState(0);
            }
        } else {
            if (mTtsPlayer->IsEngineIdle()) {
                Character_SetFace(this, GetFace());
                SetState(0);
            } else {
                func_02090a58(this, mTtsPlayer->GetPhonemeName(), GetFace());
            }
        }
        break;

    case 8:
    case 9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, GetFace());
            {
                VecFx32 target = mTarget;
                mSavedTarget.x = target.x;
                mSavedTarget.y = target.y;
                mSavedTarget.z = target.z;
            }
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            int speed;
            if (mState == 9) {
                speed = 0x8000;
                SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3000), 0, 4);
            } else {
                speed = 0x3800;
            }
            if (StepWalkLooking(speed, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        break;

    case 10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else {
            if (StepWalkLooking(0x8000, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        Character_SetWalkTarget(this, mSavedTarget);
        break;

    case 11:
        if (mStep == 0) {
            mStep++;
            {
                u16 turn = Random(0x2000) + 0x1800;
                u16 target;
                if (Random(100) < 50) {
                    target = mAngle - turn;
                } else {
                    target = mAngle + turn;
                }
                mTargetAngle = target;
            }
            StepTurnWithAnimation(GetWalkAnim());
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(GetWalkAnim())) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0);
        }
        break;

    case 12:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, 0x17);
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;

    case 13:
    case 27:
        break;

    case 14:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (IsAnimOfGroupB(mModel.mAnimId)) {
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetAnimation(RESOURCE_BCA0, 0x28);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 6);
            Character_ResetSoundPhase(this, 0);
        } else if (mStep == 2) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (Random(0) & 1) {
                Character_SetFace(this, 7);
            }
        } else if (mStep == 3) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 5, 2);
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                if (Random(10) < 8) {
                    Character_SetFace(this, 6);
                } else {
                    Character_SetFace(this, 4);
                }
                gSoundManager.StartSeqArc(1, 0x1f, 6);
            }
        } else if (mStep == 4) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = 0x32;
            Character_SetFace(this, 8);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 15:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x29);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 1);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            mHeadIcon.Show();
            mHeadIcon.mIconId = 0xf;
            mHeadIcon.Reload();
            mHeadIcon.SetMode(3);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = 0;
            Character_SetFace(this, 7);
            gSoundManager.StartSeqArc(1, 0x22, 8);
        } else if (mStep == 2) {
            if (mTimer == 0) {
                ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
                if (anim->mAnmObj->frame >= ((((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) * 5) >> 3) {
                    mTimer++;
                    if (Random(10) < 8) {
                        Character_SetFace(this, 6);
                    } else {
                        Character_SetFace(this, 4);
                    }
                }
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 16:
    case 17:
    case 18:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            if (IsAnimOfGroupA(mModel.mAnimId) && (Random(0) & 1)) {
                SetAnimation(RESOURCE_BCA0, 0x1b);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x19);
            }
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            {
                u16 yaw;
                u16 pitch;
                if (mState == 0x11) {
                    yaw = (0x20 - Random(0x40)) << 8;
                    pitch = (0 - Random(0x10)) << 8;
                } else {
                    pitch = 0;
                    yaw = 0;
                }
                SetHeadTurn(pitch, yaw, 0, 5);
            }
        } else if (mState == 0x12) {
            if (!func_02091c80(this, mPartner->mModel.mPos.x, mPartner->mModel.mPos.z) && mPartner->mState != 8 &&
                mPartner->mState != 10 && mPartner->mState != 0x18 && mPartner->mState != 0x19) {
                break;
            }
            {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                u16 pitch = 0;
                if (Random(10) < 5) {
                    pitch = GetHeadPitchToPartner(0, 0x1800);
                }
                SetHeadTurn(pitch, yaw, 0, 5);
                if (mPartner->mState == 0x18) {
                    if (mFace != 0x11) {
                        Character_SetFace(this, 0x11);
                    }
                } else {
                    int face = mFace;
                    if (face != GetFace()) {
                        Character_SetFace(this, GetFace());
                    }
                }
            }
        }
        break;

    case 19:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            if (Random(10) < 4) {
                if (IsAnimOfGroupA(mModel.mAnimId) && (Random(0) & 1)) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                }
            } else {
                SetAnimation(RESOURCE_BCA0, 0x1d);
            }
            Character_SetFace(this, 0x15);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPartner->IsState10To12()) {
                mPartner->SetState(0x14);
            } else {
                mPartner->SetState(6);
            }
        } else {
            u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
            SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), yaw, 0, 3);
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 3, 1);
        }
        break;

    case 20:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            if (Random(10) < 4) {
                if (Random(0) & 1) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                }
            } else {
                SetAnimation(RESOURCE_BCA0, 0x1c);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), yaw, 0, 4);
            }
        } else {
            if (mModel.mAnimId == 0x1c) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 7, 2);
            }
        }
        break;

    case 21:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, 0x18);
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        }
        break;

    case 22:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            if (data_020fba28[mPersonality] == 0) {
                SetAnimation(RESOURCE_BCA0, 0x23);
            } else if (data_020fba28[mPersonality] == 1) {
                SetAnimation(RESOURCE_BCA0, 0x23);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x24);
            }
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                func_02091fa0(this);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x20, 1, 2);
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x20, 0, 8);
        }
        break;

    case 23:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x2b);
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 0xf, 2);
        }
        break;

    case 24:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            // Both arms start the same animation, and the original has both calls. The compiler folds two arms that
            // are the same in the syntax tree into one, so they differed in the source in some way that leaves no
            // trace in the code. What that was is not known; the cast stands in for it.
            if (data_020fba28[mPersonality] == 0) {
                SetAnimation(RESOURCE_BCA0, 0x26);
            } else {
                ((RoomCharacterAlias *)this)->SetAnimation(RESOURCE_BCA0, 0x26);
            }
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            if (Random(2) == 0) {
                Character_SetFace(this, 6);
            } else {
                Character_SetFace(this, 4);
            }
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            mTimer = 0x46;
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 1) {
                int voice;
                mStep++;
                mTimer = 0x32;
                voice = func_02098e88(this, 6);
                if (voice == 0) {
                    gSoundManager.StartSeqArc(1, 0x22, 0);
                } else if (voice == 1) {
                    gSoundManager.StartSeqArc(1, 0x22, 1);
                } else if (voice == 2) {
                    gSoundManager.StartSeqArc(1, 0x22, 2);
                } else if (voice == 3) {
                    gSoundManager.StartSeqArc(1, 0x22, 3);
                } else if (voice == 4) {
                    gSoundManager.StartSeqArc(1, 0x22, 4);
                } else {
                    gSoundManager.StartSeqArc(1, 0x22, 5);
                }
            }
        } else if (mStep == 2) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 25:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x25);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 4);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 2) {
                mStep++;
                gSoundManager.StartSeqArc(1, 0x22, 6);
            }
        } else if (mStep == 2) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 1) {
                mStep++;
                Character_SetFace(this, 7);
                mHeadIcon.Show();
                mHeadIcon.mIconId = 0xb;
                mHeadIcon.Reload();
                mHeadIcon.SetMode(3);
                gSoundManager.StartSeqArc(1, 0x22, 7);
            }
        } else if (mStep == 3) {
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 26:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            if (data_020fba28[mPersonality] == 0) {
                SetAnimation(RESOURCE_BCA0, 0x15);
            } else if (data_020fba28[mPersonality] == 1) {
                SetAnimation(RESOURCE_BCA0, 0x15);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x16);
            }
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 4);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 1) {
                mStep++;
                Character_SetFace(this, 7);
                mHeadIcon.Show();
                mHeadIcon.mIconId = 8;
                mHeadIcon.Reload();
                mHeadIcon.SetMode(3);
                gSoundManager.StartSeqArc(1, 0x20, 2);
            }
        } else if (mStep == 2) {
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x14;
            }
        } else if (mStep == 3) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 28:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x47);
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                Character_SetFace(this, 6);
                mTimer = Random(0x32) + 0x50;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mTimer = Random(0x1e) + 0x28;
                if (Random(0) & 1) {
                    Character_SetFace(this, 6);
                } else {
                    Character_SetFace(this, GetFace());
                }
            }
        }
        if (mModel.mAnimId == 0x47) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 7, 0xe);
        }
        break;

    default:
        SetState(0);
        break;
    }
    PlaceHeadIcon();
}
