#include <nitro.h>
#include <nnsys.h>

#include "../RoomCharacter.h"

// States 0xb3 to 0xc9, two characters and the thing in the first hand (mHeldItems[0], which follows joint 2).
// 0xb3..0xbe: both take it out (0xb4/0xb5), use it together for a random time with sequence 0x1c playing (0xb6/0xb7),
// or one walks up to the other first (0xb8..0xbb). 0xbf..0xc9: one uses it (0xc0 standing, 0xc1 walking about)
// while the other watches (0xc2); 0xc4..0xc9 bring the two to the middle of the room first.
// Only the character with mSlot == 0 chooses the next states, for both.
void RoomCharacter::UpdateStatesB3ToC9() {
    // Declared here and not in their case: that is where the original has them on its stack
    FloorPos own;
    FloorPos other;

    switch (mState) {
    case 0xb3:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xb3 && mPrevState <= 0xb7) {
                if (Random(100) < 70) {
                    SetStateWithPartner(0xb4, 0xb4);
                } else {
                    SetStateWithPartner(0xb5, 0xb5);
                }
            } else if (mPrevState == 0xb9 || mPrevState == 0xba || mPrevState == 0xbb) {
                SetStateWithPartner(0xb9, 0xb9);
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xb8, 0xb8);
        }
        break;

    case 0xb4:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            StepChangeHeldItems(7, 0xff);
        } else {
            if (StepChangeHeldItems(7, 0xff) && mModel.mBlendFrames <= 0) {
                SetState(0xb6);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotZ = 0x4000;
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb6:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mPartner->mState == 0xb6 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mStep++;
                    StartSound3IfAnimOfGroup();
                    mTimer = Random(200) + 550;
                    int r = Random(100);
                    if (r < 55) {
                        SetAnimation(RESOURCE_BCA0, 9);
                    } else if (r < 70) {
                        SetAnimation(RESOURCE_BCA0, 2);
                    } else if (r < 83) {
                        mTimer >>= 2;
                        SetAnimation(RESOURCE_BCA0, 7);
                    } else if (r < 92) {
                        mTimer >>= 1;
                        SetAnimation(RESOURCE_BCA0, 0x58);
                    } else if (r < 96) {
                        mTimer >>= 2;
                        SetAnimation(RESOURCE_BCA0, 0x4b);
                    } else {
                        mTimer >>= 3;
                        SetAnimation(RESOURCE_BCA0, 0x2a);
                        if (mModel.mBlendFrames > 0) {
                            Character_ResetSoundPhase(this, 0);
                            Character_ResetSoundPhase(mPartner, 0);
                        }
                    }
                    mPartner->SetAnimation(RESOURCE_BCA0, mModel.mAnimId);
                    if (Random(100) < 50) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 10);
                    }
                    Character_SetFace(mPartner, mFace);
                    u16 yaw;
                    if (Random(100) < 25) {
                        yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                    } else {
                        yaw = 0;
                    }
                    SetHeadTurn(0, yaw, 0, 4);
                    gSoundManager.StartSeqArcLimited(mSoundHandle, 0x1c, 5, 1);
                }
            } else {
                mStep++;
                u16 yaw;
                if (Random(100) < 25) {
                    yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                } else {
                    yaw = 0;
                }
                SetHeadTurn(0, yaw, 0, 4);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mKeepSound = TRUE;
                    if (Random(100) < 90) {
                        SetStateWithPartner(0xb6, 0xb6);
                    } else {
                        SetStateWithPartner(0xb7, 0xb7);
                    }
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb5:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            StepChangeHeldItems(7, 0xff);
        } else {
            if (StepChangeHeldItems(7, 0xff) && mModel.mBlendFrames <= 0) {
                SetState(0xb7);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotZ = 0x4000;
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb7:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mPartner->mState == 0xb7 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mStep++;
                    StartSound0UnlessGroupA();
                    mTimer = Random(200) + 450;
                    int r = Random(100);
                    if (r < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x19);
                    } else if (r < 70) {
                        SetAnimation(RESOURCE_BCA0, 0x1b);
                    } else if (r < 85) {
                        mTimer >>= 1;
                        SetAnimation(RESOURCE_BCA0, 0x1c);
                    } else {
                        mTimer >>= 1;
                        SetAnimation(RESOURCE_BCA0, 0x33);
                    }
                    mPartner->SetAnimation(RESOURCE_BCA0, mModel.mAnimId);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                        Character_ResetSoundPhase(mPartner, 0);
                    }
                    if (Random(100) < 50) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 10);
                    }
                    Character_SetFace(mPartner, mFace);
                    u16 yaw;
                    if (Random(100) < 25) {
                        yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                    } else {
                        yaw = 0;
                    }
                    SetHeadTurn(0, yaw, 0, 4);
                    gSoundManager.StartSeqArcLimited(mSoundHandle, 0x1c, 5, 1);
                }
            } else {
                mStep++;
                u16 yaw;
                if (Random(100) < 25) {
                    yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                } else {
                    yaw = 0;
                }
                SetHeadTurn(0, yaw, 0, 4);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mKeepSound = TRUE;
                    if (Random(100) < 80) {
                        SetStateWithPartner(0xb7, 0xb7);
                    } else {
                        SetStateWithPartner(0xb6, 0xb6);
                    }
                }
            }
            if (mModel.mBlendFrames <= 0 && mModel.mAnimId == 0x33) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 8, 1);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb8:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xba, 0xbb);
        }
        break;

    case 0xb9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            SetTargetFloorPos(mPartner->GetFloorPos());
            ShowHeadIcon();
            SetHeldItemsShowState(1);
        } else {
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                SetHeadTurn(0, 0, 0, 4);
                SetState(0xbe);
            }
        }
        break;

    case 0xba:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 70;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xb9, 0xb9);
            }
        }
        break;

    case 0xbb:
        StepRandomAnimationFacingPartner();
        break;

    case 0xbc:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0xbd);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xbd:
        if (mSlot == 0 && mPartner->mState == 0xbd) {
            SetStateWithPartner(0xb4, 0xb4);
        }
        PlaceHeldItemAtHead();
        break;

    case 0xbe:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 7, 0xff)) {
                mStep++;
                mHeldItems[0]->mRotZ = 0x4000;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                    SetState(0xbc);
                }
            }
        }
        mHeldItems[0]->mRotX = mUnknown25D6;
        mHeldItems[0]->SetJointOffset(mModel.GetJointMatrix(2), mModel.GetJointRotation(0), 0x666, 0, 0);
        break;

    case 0xbf:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xbf && mPrevState <= 0xc1) {
                if (Random(100) < 50) {
                    SetStateWithPartner(0xc0, 0xc2);
                } else {
                    SetStateWithPartner(0xc1, 0xc2);
                }
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xc4, 0xc4);
        }
        break;

    case 0xc0:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x21);
                Character_SetFace(this, 8);
                HeldItem *item = mHeldItems[0];
                item->mRotX = 0;
                item->mRotY = 0x7000;
                item->mRotZ = 0x1000;
                mTimer = Random(200) + 700;
                mTimer2 = 60;
                mTimer3 = 100;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Random(100) < 90) {
                    if (Random(2) == 0) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 0x14);
                    }
                } else {
                    if (Random(2) == 0) {
                        Character_SetFace(this, 0xe);
                    } else {
                        Character_SetFace(this, 0x16);
                    }
                }
            }
            if (mFace == 0x14 || mFace == 0x16) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 3, 10);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 7, 2);
            }
            if (mTimer3-- <= 0) {
                mTimer3 = Random(100) + 200;
                u16 yaw;
                if (Random(100) < 30) {
                    yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x1800);
                } else {
                    yaw = 0;
                }
                SetHeadTurn(0, yaw, 0, 5);
            }
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 40) {
                    SetState(0xc0);
                } else {
                    SetState(0xc1);
                }
                Character_SetFace(this, 8);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xc1:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            RestartStepSound();
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x22);
                Character_SetFace(this, 0x15);
                HeldItem *item = mHeldItems[0];
                item->mRotX = 0;
                item->mRotY = 0x7800;
                item->mRotZ = 0x1000;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 1);
                }
                FloorPos pos;
                GetRandomPosInArea(&pos);
                VEC_Set(&mTarget, pos.x, 0, pos.z);
                mTimer = Random(200) + 700;
                mTimer2 = 0;
                mCounter = 0;
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Random(100) < 70) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, 0x16);
                }
            }
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1a, 3, 10);
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 60) {
                    SetState(0xc0);
                } else {
                    SetState(0xc1);
                }
                Character_SetFace(this, 8);
            } else {
                if (IsWithinReach(mPartner->GetFloorPos(), 0x2666, 0x2666)) {
                    func_0209171c(this, mPartner->GetFloorPos());
                }
                if (StepWalkLooking(0x1800, 0, 0x199, 0x22, 6) || ++mCounter > 200) {
                    mCounter = 0;
                    FloorPos pos;
                    if (Random(100) < 50) {
                        GetRandomCorner(&pos);
                    } else {
                        GetRandomPosInArea(&pos);
                    }
                    VEC_Set(&mTarget, pos.x, 0, pos.z);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xc2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 70) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                    Character_SetFace(this, GetFace());
                } else if (r < 90) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                    Character_SetFace(this, GetFace());
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                mCounter = 0;
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int r = Random(100);
                if (r < 55) {
                    if (mModel.mAnimId != 0x17) {
                        mCounter = 1;
                    } else {
                        SetHeadTurn(0, 0, 0, 5);
                    }
                } else if (r < 90) {
                    if (mModel.mAnimId != 0x17) {
                        u16 yaw = (0x20 - Random(0x40)) << 8;
                        u16 pitch = (0x10 - Random(0x20)) << 8;
                        SetHeadTurn(pitch, yaw, 0, 5);
                    } else {
                        SetHeadTurn(0, 0, 0, 5);
                    }
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0xc2);
            } else if (mCounter == 1) {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), yaw, 0, 4);
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;

    case 0xc3:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
        }
        break;

    case 0xc4:
        if (mStep != 0) {
            break;
        }
        if (mSlot == 0) {
            if (mPartner->mState != 0xc4) {
                break;
            }
            mStep++;
            own = GetFloorPos();
            other = mPartner->GetFloorPos();
            fx32 ownDistance = own.LengthSq();
            fx32 otherDistance = other.LengthSq();
            if (otherDistance < 0x1000) {
                SetStateWithPartner(0xc9, 0xc8);
            } else if (ownDistance <= 0x6000) {
                SetStateWithPartner(0xc5, 0xc3);
            } else if (ownDistance < otherDistance && own.Dot(other) > 0) {
                SetStateWithPartner(0xc6, 0xc3);
            } else {
                SetStateWithPartner(0xc3, 0xc7);
            }
        } else {
            mStep++;
        }
        break;

    case 0xc5:
        if (mStep == 0 && mSlot == 0) {
            FloorPos own = GetFloorPos();
            if (own.LengthSq() <= 0x6000) {
                func_02091598(this, mPartner->GetFloorPos());
                StepWalkTo(GetTargetFloorPos());
            } else {
                SetStateWithPartner(0xc3, 0xc7);
            }
        }
        break;

    case 0xc6:
        if (mStep == 0 && mSlot == 0) {
            FloorPos own = GetFloorPos();
            if (own.LengthSq() > 0x4000) {
                mTarget.x = 0;
                mTarget.z = 0;
                StepWalkTo(GetTargetFloorPos());
            } else {
                SetStateWithPartner(0xc5, 0xc3);
            }
        }
        break;

    case 0xc7:
        if (mSlot == 0) {
            break;
        }
        if (mStep == 0) {
            mStep++;
            mTarget.x = 0;
            mTarget.z = 0;
            mTargetAngle = AngleTo(GetTargetFloorPos());
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(GetWalkAnim())) {
                mStep++;
                SetHeadTurn(0, 0, 0, 4);
            }
        } else {
            if (GetFloorPos().LengthSq() < 0x1000) {
                SetStateWithPartner(0xc8, 0xc9);
            } else {
                mTarget.x = 0;
                mTarget.z = 0;
                StepWalkTo(GetTargetFloorPos());
            }
        }
        break;

    case 0xc8:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = 150;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0xc2);
            }
        }
        break;

    case 0xc9:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 0, 0xff)) {
                mStep++;
                mTimer = 300;
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mRotY = 0x7000;
                gSoundManager.StartSeqArc(mSoundHandle, 0x1c, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x21);
                mHeldItems[0]->mRotZ = 0x1000;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x1c, 0) == 0) {
                SetState(0xc0);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;
    }
    PlaceHeadIcon();
}

// States 0xca to 0xe0, two characters taking turns: in 0xcf and 0xd1/0xd2 mCounter says whose turn it is (100 and
// up: the partner's) and how far it is; 0xd3/0xd4 end a round, 0xd5..0xd7 bring the two together, 0xd8..0xde walk
// them to either side of the room (by the sign of x) and to a fixed place, 0xdf/0xe0 start again from there.
void RoomCharacter::UpdateStatesCAToE0() {
    // Declared here and not in their case: that is where the original has them on its stack
    FloorPos own;
    FloorPos other;

    switch (mState) {
    case 0xca:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xca && mPrevState <= 0xd4) {
                int r = Random(100);
                if (r < 35) {
                    SetStateWithPartner(0xcb, 0xcb);
                } else if (r < 70) {
                    SetStateWithPartner(0xcc, 0xcc);
                } else {
                    SetStateWithPartner(0xce, 0xce);
                }
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xd5, 0xd5);
        }
        break;

    case 0xcb:
        if (StepPutHeldItemsDown(0x59)) {
            SetState(0xcf);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcf:
        if (mStep == 0) {
            if (mSlot == 0 && mPartner->mState == 0xcf && mModel.mBlendFrames <= 0 &&
                mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(300) + 2000;
                mTimer2 = Random(60) + 90;
                mTimer3 = 0;
                mCounter = 0;
            }
        } else if (mModel.mBlendFrames <= 0 && mSlot == 0) {
            RoomCharacter *who;
            u16 phase = mCounter;
            if (phase < 100) {
                who = this;
            } else {
                phase -= 100;
                who = mPartner;
            }
            mTimer--;
            if (who->mModel.mBlendFrames <= 0) {
                if (phase == 0) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        who->SetAnimation(RESOURCE_BCA0, 0x5a);
                        func_0208ef00(&who->mModel, TRUE, FALSE);
                        Character_ResetSoundPhase(who, 0);
                    }
                } else if (phase == 1) {
                    if (Character_PlaySoundAtAnimPhase(who, 0, 1, 0x25, 1, 4) == 2 ||
                        AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId)->mFinished) {
                        mCounter++;
                        mTimer2 = Random(15) + 2;
                        if (Random(100) < 60) {
                            Character_SetFace(who, 0x15);
                        } else {
                            Character_SetFace(who, 8);
                        }
                    }
                } else {
                    mTimer2--;
                    if (mTimer2 <= 0 || AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId)->mFinished) {
                        who->SetAnimation(RESOURCE_BCA0, 0x59);
                        Character_SetFace(who, 8);
                        if (Random(100) < 9) {
                            mTimer2 = Random(30) + 150;
                            mCounter = 0;
                            mTimer3++;
                        } else {
                            ModelAnimation *anim = AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId);
                            if (anim != NULL) {
                                int frames = (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << FX32_SHIFT) >> FX32_SHIFT;
                                frames = (frames * who->mPartner->mAnimSpeed) >> FX32_SHIFT;
                                mTimer2 = (Random(frames) >> 3) + (frames >> 4);
                            } else {
                                mTimer2 = Random(18) + 2;
                            }
                            if (mCounter < 100) {
                                mCounter = 100;
                            } else {
                                mCounter = 0;
                            }
                        }
                        if (mTimer <= 0 && mTimer3 >= 4) {
                            SetStateWithPartner(0xd3, 0xd3);
                        }
                    }
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcc:
        if (StepPutHeldItemsDown(0x2f)) {
            SetState(0xd0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd0:
        if (mStep == 0) {
            if (mPartner->mState == 0xd0) {
                mStep++;
                mTimer = Random(300) + 1700;
                mTimer2 = Random(60) + 90;
            }
        } else if (mModel.mBlendFrames <= 0) {
            mTimer--;
            if (mTimer <= 0) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    SetStateWithPartner(0xd3, 0xd3);
                    SetAnimation(RESOURCE_BCA0, 0x2f);
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x2f);
                }
            } else {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(110) + 120;
                    mTimer2 += ((FX32_ONE - mAnimSpeed) * 50) >> FX32_SHIFT;
                    int r = Random(100);
                    if (mModel.mAnimId == 0x2f) {
                        r += 10;
                    } else {
                        r -= 10;
                    }
                    if (r < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x2f);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x30);
                    }
                    r = Random(100);
                    if (r < 20) {
                        Character_SetFace(this, 0x15);
                    } else if (r < 40) {
                        Character_SetFace(this, 0xb);
                    } else if (r < 60) {
                        Character_SetFace(this, 0x10);
                    } else {
                        Character_SetFace(this, 8);
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x30) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x25, 3, 10);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcd:
    case 0xce:
        if (StepPutHeldItemsDown((mSlot == 0 && mState == 0xcd) ? 0x2d : 7)) {
            if (mState == 0xcd) {
                SetState(0xd1);
            } else {
                SetState(0xd2);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd1:
    case 0xd2:
        if (mStep == 0) {
            if (mSlot == 0) {
                switch (mPartner->mState) {
                case 0xd1:
                case 0xd2:
                    if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                        mStep++;
                        mCounter = 0;
                        mTimer2 = Random(60) + 90;
                        mTimer3 = 0;
                        if (mState != 0xd1) {
                            mTimer = 0;
                        } else {
                            mTimer = Random(3) * 2 + 12;
                        }
                    }
                    break;
                }
            }
        } else if (mSlot == 0) {
            RoomCharacter *who;
            u16 phase = mCounter;
            if (phase < 100) {
                who = this;
            } else {
                phase -= 100;
                who = mPartner;
            }
            if (who->mModel.mBlendFrames <= 0) {
                if (phase == 0) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        mTimer2 = Random(30) + 80;
                        who->SetAnimation(RESOURCE_BCA0, 0x2d);
                        Character_SetFace(who, 8);
                        who->SetHeadTurn(0, 0, 0, 4);
                    }
                } else if (phase == 1) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        mTimer3++;
                        who->SetAnimation(RESOURCE_BCA0, 0x2e);
                        func_0208ef00(&who->mModel, TRUE, FALSE);
                        Character_ResetSoundPhase(who, 0);
                    }
                } else if (phase == 2) {
                    Character_PlaySoundAtAnimPhase(who, 0, mSoundHandle, 0x25, 2, 4);
                    if (AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId)->mFinished) {
                        mCounter++;
                        mTimer2 = 250;
                        who->SetAnimation(RESOURCE_BCA0, 0x2d);
                        int r = Random(100);
                        if (r < 15) {
                            Character_SetFace(who, 2);
                        } else if (r < 30) {
                            Character_SetFace(who, 4);
                        } else if (r < 45) {
                            Character_SetFace(who, 6);
                        } else if (r < 60) {
                            Character_SetFace(who, 5);
                        } else {
                            Character_SetFace(who, 8);
                        }
                        if (Random(100) < 20) {
                            u16 yaw = who->mPartner->GetHeadYawTo(who->GetFloorPos(), 0x2000);
                            who->mPartner->SetHeadTurn(who->mPartner->GetHeadPitchToPartner(0, 0x1800), yaw, 0, 5);
                        }
                    }
                } else if (phase == 3) {
                    if (gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0 || --mTimer2 <= 0) {
                        mCounter++;
                        Character_SetFace(who, 8);
                        who->mPartner->SetHeadTurn(0, 0, 0, 5);
                    }
                } else {
                    mTimer2 = Random(60) + 60;
                    if (mTimer3 >= 2 || Random(100) < 20) {
                        mTimer3 = 0;
                        mTimer++;
                        who->SetAnimation(RESOURCE_BCA0, 7);
                        if (mCounter < 100) {
                            mCounter = 100;
                        } else {
                            mCounter = 0;
                        }
                    } else {
                        if (mCounter < 100) {
                            mCounter = 0;
                        } else {
                            mCounter = 100;
                        }
                    }
                    if (mTimer >= 20) {
                        SetStateWithPartner(0xd3, 0xd3);
                    }
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd3:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(20) + 50;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
            Character_SetFace(this, 8);
        } else if (mStep == 1) {
            if (mSlot == 0) {
                if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mState == mPartner->mState) {
                    mTimer--;
                    if (mTimer <= 0) {
                        gSoundManager.StopSeqArc(1, 0);
                        mStep++;
                        mTimer = Random(30) + 150;
                        RoomCharacter *first;
                        RoomCharacter *second;
                        if (Random(2) < 1) {
                            first = this;
                            second = mPartner;
                        } else {
                            first = mPartner;
                            second = this;
                        }
                        int r = Random(100);
                        if (r < 35) {
                            first->SetAnimation(RESOURCE_BCA0, 0x2a);
                            Character_ResetSoundPhase(first, 0);
                        } else if (r < 70) {
                            first->SetAnimation(RESOURCE_BCA0, 8);
                        } else {
                            first->SetAnimation(RESOURCE_BCA0, 0x4c);
                        }
                        if (Random(100) < 60) {
                            Character_SetFace(first, 2);
                        } else {
                            Character_SetFace(first, 8);
                        }
                        r = Random(100);
                        if (r < 35) {
                            second->SetAnimation(RESOURCE_BCA0, 6);
                        } else if (r < 70) {
                            second->SetAnimation(RESOURCE_BCA0, 1);
                        } else {
                            second->SetAnimation(RESOURCE_BCA0, 0x43);
                        }
                        if (Random(100) < 60) {
                            Character_SetFace(second, 4);
                        } else {
                            Character_SetFace(second, 8);
                        }
                    }
                }
            } else {
                mStep++;
            }
        } else if (mStep == 2) {
            if (mSlot == 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    mStep++;
                    mTimer = Random(30) + 120;
                    SetAnimation(RESOURCE_BCA0, 7);
                    mPartner->SetAnimation(RESOURCE_BCA0, 7);
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetStateWithPartner(0xd4, 0xd4);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd4:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    mTimer = Random(60) + 150;
                    StartSound3IfAnimOfGroup();
                    SetAnimation(RESOURCE_BCA0, 0x4d);
                    ShowHeadIcon();
                    SetHeadTurn(0, 0, 0, 4);
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
                }
            } else {
                mStep++;
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 4);
            }
        } else if (mSlot == 0) {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 35) {
                    SetStateWithPartner(0xcb, 0xcb);
                } else if (r < 70) {
                    SetStateWithPartner(0xcc, 0xcc);
                } else {
                    SetStateWithPartner(0xce, 0xce);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd5:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xd6, 0xd7);
        }
        break;

    case 0xd6:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 70;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xd8, 0xd8);
            }
        }
        break;

    case 0xd7:
        StepRandomAnimationFacingPartner();
        break;

    case 0xd8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else if (mSlot == 0) {
            own = GetFloorPos();
            other = mPartner->GetFloorPos();
            if (FX_Mul(own.x, other.x) < 0) {
                SetStateWithPartner(0xdb, 0xdb);
            } else if (own.x == other.x) {
                if (own.x > 0) {
                    SetStateWithPartner(0xda, 0xdb);
                } else if (own.x < 0) {
                    SetStateWithPartner(0xd9, 0xdb);
                } else {
                    SetStateWithPartner(0xd9, 0xda);
                }
            } else if (own.x > other.x) {
                if (own.x > 0) {
                    SetStateWithPartner(0xdb, 0xda);
                } else {
                    SetStateWithPartner(0xd9, 0xdb);
                }
            } else {
                if (other.x > 0) {
                    SetStateWithPartner(0xda, 0xdb);
                } else {
                    SetStateWithPartner(0xdb, 0xd9);
                }
            }
        }
        break;

    case 0xd9: {
        FloorPos target = GetFloorPos();
        target.x = 0x3000;
        StepWalkTo(target);
        if (mModel.mPos.x >= 0x800) {
            SetState(0xdb);
        }
        break;
    }

    case 0xda: {
        FloorPos target = GetFloorPos();
        target.x = -0x3000;
        StepWalkTo(target);
        if (mModel.mPos.x <= -0x800) {
            SetState(0xdb);
        }
        break;
    }

    case 0xdb:
        if (mStep == 0) {
            if (mPartner->mState == 0xdb) {
                if (mModel.mPos.x >= 0) {
                    SetStateWithPartner(0xdc, 0xdd);
                } else {
                    SetStateWithPartner(0xdd, 0xdc);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else if (mPartner->mState == 0xdb) {
            if (mModel.mPos.x >= 0) {
                SetStateWithPartner(0xdc, 0xdd);
            } else {
                SetStateWithPartner(0xdd, 0xdc);
            }
        }
        break;

    case 0xdc:
    case 0xdd: {
        FloorPos target = gRoomPoints[11];
        if (mState == 0xdd) {
            target.x = -target.x;
        }
        if (StepWalkTo(target)) {
            SetState(0xde);
        }
        break;
    }

    case 0xde:
        if (mStep == 0) {
            mStep++;
            mTargetAngle = AngleTo(gRoomPoints[12]);
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(GetWalkAnim())) {
                mStep++;
                SetHeadTurn(0, 0, 0, 4);
                if (mPartner->mModel.mPos.z < 0x1800) {
                    SetState(0xdf);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else {
            if (mPartner->mModel.mPos.z < 0x1800) {
                SetState(0xdf);
            }
        }
        break;

    case 0xdf:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (StepChangeItemsThenAnimation(0x20, 2, 3)) {
                    mStep++;
                    AnimModel_FinishAnimations(mHeldItems[1]);
                    PlaceHeldItemsAtStart();
                }
            } else {
                if (StepChangeItemsThenAnimation(0x20, 2, 0xff)) {
                    mStep++;
                    mHeldItems[0]->mRotY = 0x8000;
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 150;
                SetAnimation(RESOURCE_BCA0, 0x4d);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = TRUE;
                SetState(0xe0);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xe0:
        if (mPartner->mState == 0xe0) {
            SetHeadTurn(0, 0, 0, 4);
            int r = Random(100);
            if (r < 35) {
                SetStateWithPartner(0xcb, 0xcb);
            } else if (r < 70) {
                SetStateWithPartner(0xcc, 0xcc);
            } else {
                SetStateWithPartner(0xce, 0xce);
            }
        } else if (mPartner->mState == 0xdf) {
            SetHeadTurn(0, 0, 0, 4);
        } else {
            SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3000), 0, 4);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;
    }
    PlaceHeadIcon();
}

// States 0xe1 to 0xf1, two characters side by side: 0xe2/0xe3 stay there, changing face and looking at each other
// now and then; 0xe4..0xe8 and 0xed..0xf1 are the same walk up to the partner, ending in 0xe2 or 0xea; 0xea looks at
// the room's target, then gives both characters their next event (0x18 and 0x17) and saves them; 0xeb/0xec turn
// away and hand both back to state 2.
void RoomCharacter::UpdateStatesE1ToF1() {
    int state = mState;

    switch (state) {
    case 0xe1:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xe1 && mPrevState <= 0xe3) {
                SetStateWithPartner(mPrevState, mPrevState);
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xe4, 0xe4);
        }
        break;

    case 0xe2:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            StartAnimationWithIcon(GetIdleAnim());
            SetHeadTurn(0, 0, 0, 5);
            Character_SetFace(this, 8);
            mTimer2 = 1;
            mTimer3 = 2;
        } else {
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Random(100) + 300;
                if (mSlot == 0) {
                    if (Random(100) < 35) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 2);
                    }
                    Character_SetFace(mPartner, mFace);
                }
            }
            mTimer3--;
            if (mTimer3 <= 0) {
                mTimer3 = Random(100) + 400;
                if (mSlot == 0) {
                    SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
                    mPartner->SetHeadTurn(mPartner->GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
                }
            }
        }
        break;

    case 0xe3:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            StartAnimationWithIcon(0x19);
            SetHeadTurn(0, 0, 0, 5);
            Character_SetFace(this, 2);
            mTimer2 = 200;
            mTimer3 = 300;
        } else {
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Random(100) + 300;
                if (mSlot == 0) {
                    if (Random(100) < 35) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 2);
                    }
                    Character_SetFace(mPartner, mFace);
                }
            }
            mTimer3--;
            if (mTimer3 <= 0) {
                mTimer3 = Random(100) + 500;
                if (mSlot == 0) {
                    if (Random(100) < 20) {
                        SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3000), 0, 5);
                        mPartner->SetHeadTurn(0, mPartner->GetHeadYawTo(GetFloorPos(), 0x3000), 0, 5);
                    } else {
                        SetHeadTurn(0, 0, 0, 5);
                        mPartner->SetHeadTurn(0, 0, 0, 5);
                    }
                }
            }
        }
        break;

    case 0xe4:
    case 0xed:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x2999)) {
            if (mState == 0xe4) {
                SetStateWithPartner(0xe5, 0xe6);
            } else {
                SetStateWithPartner(0xee, 0xef);
            }
        }
        break;

    case 0xe5:
    case 0xee:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 70;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0xe5) {
                    SetStateWithPartner(0xe7, 0xe7);
                } else {
                    SetStateWithPartner(0xf0, 0xf0);
                }
            }
        }
        break;

    case 0xe6:
    case 0xef:
        StepRandomAnimationFacingPartner();
        break;

    case 0xe7:
    case 0xf0:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, 8);
            SetTargetFloorPos(mPartner->GetFloorPos());
            ShowHeadIcon();
            SetHeldItemsShowState(1);
        } else {
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                SetHeadTurn(0, 0, 0, 4);
                if (mState == 0xe7) {
                    SetState(0xe8);
                } else {
                    SetState(0xf1);
                }
            }
        }
        break;

    case 0xe8:
    case 0xf1:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
        } else if (mSlot == 0) {
            if (state == 0xe8) {
                if (mPartner->mState == 0xe8) {
                    SetStateWithPartner(0xe2, 0xe2);
                }
            } else {
                if (mPartner->mState == 0xf1) {
                    SetStateWithPartner(0xea, 0xea);
                }
            }
        }
        break;

    case 0xe9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xe9 && mPrevState <= 0xea) {
                SetStateWithPartner(mPrevState, mPrevState);
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xed, 0xed);
        }
        break;

    case 0xea:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            StartAnimationWithIcon(GetIdleAnim());
            SetHeadTurn(0, 0, 0, 5);
            Character_SetFace(this, 2);
            mTimer = 80;
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 100;
                Character_SetFace(this, 5);
                mHeadIcon.SetMode(1);
                if (mSlot == 0) {
                    mWait = 10000;
                    mPartner->mWait = 10000;
                    SetEvent(0x18);
                    mPartner->SetEvent(0x17);
                    Save();
                    mPartner->Save();
                }
            } else {
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mSlot == 0) {
                    SetStateWithPartner(0xeb, 0xeb);
                }
            } else {
                u16 pitch = GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), mCamera->mPosition, 0x1800);
                u16 yaw = GetHeadYawTo(mCamera->GetFloorPos(), 0x3800);
                SetHeadTurn(pitch, yaw, 0, 2);
            }
        }
        break;

    case 0xeb:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, 8);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 3);
            mTargetAngle = mAngle + 0x5000 + Random(0x1000) * 6;
        } else {
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0x200, 0)) {
                SetState(0xec);
            }
        }
        break;

    case 0xec:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
        } else if (mPartner->mState == 0xec) {
            SetStateWithPartner(2, 2);
            mReaction = 0;
            mPartner->mReaction = 0;
            mWait = 150;
            mPartner->mWait = Random(30) + 160;
        }
        break;
    }
    PlaceHeadIcon();
}
