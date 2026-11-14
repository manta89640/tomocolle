#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "../RoomCharacter.h"

// The states 0x70 to 0x92: three things the two characters of the room do together. Each group has the same shape:
// a hub state (0x70, 0x7f, 0x8b) in which slot 0 picks the next state for both, the activity states, and a way out
// (walk to the partner, 0x7b/0x86/0x8f, then a pair of closing states). mStep is the step inside a state.
//   0x70..0x7e: both play the same pair of animations (0x4e/0x4f, 0x50/0x51 or 0x52) in turns, with sounds of
//               sequence archive 0x24
//   0x7f..0x8a: animations 0x5b/0x5c with sounds of the same archive
//   0x8b..0x92: with held item 8 (StepChangeHeldItems(8, 0xff)): animations 0x3f..0x42, sound 4 of archive 0x25, and
//               in 0x8e walking about to random places
void RoomCharacter::UpdatePairStates70To92() {
    int state = mState;
    switch (state) {
    case 0x70:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            if (mPrevState > 0x70 && mPrevState <= 0x7e && mPrevState != 0x7b) {
                if (mSlot == 0) {
                    int diff = (s16)mAngle - (s16)mPartner->mAngle;
                    if (diff < 0) {
                        diff = -diff;
                    }
                    int r = Random(100);
                    u16 other;
                    if (r < 25) {
                        if (diff < 0x4000) {
                            mCounter = 0x71;
                        } else {
                            mCounter = 0x72;
                        }
                        other = mCounter;
                    } else if (r < 50) {
                        if (diff < 0x4000) {
                            mCounter = 0x73;
                        } else {
                            mCounter = 0x74;
                        }
                        other = mCounter;
                    } else if (r < 70) {
                        mCounter = 0x75;
                        other = mCounter;
                    } else if (r < 85) {
                        mCounter = 0x7c;
                        other = 0x7d;
                    } else {
                        other = 0x7e;
                        mCounter = other;
                    }
                    SetStateWithPartner(mCounter, other);
                }
            } else {
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                SetState(0x7b);
            }
            SetHeadTurn(0, 0, 0, 5);
            SetHeldItemsShowState(1);
        }
        break;
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
        if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mSlot != 0) {
            if (state == 0x71) {
                SetStateWithPartner(0x76, 0x76);
            } else if (state == 0x72) {
                SetStateWithPartner(0x77, 0x77);
            } else if (state == 0x73) {
                SetStateWithPartner(0x78, 0x78);
            } else if (state == 0x74) {
                SetStateWithPartner(0x79, 0x79);
            } else {
                SetStateWithPartner(0x7a, 0x7a);
            }
        }
        break;
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mCounter = 0;
                StartSound3IfAnimOfGroup();
                int prevAnim = mModel.mAnimId;
                mTimer = Random(80) + 0xfa;
                if (mState == 0x7a) {
                    SetAnimation(RESOURCE_BCA0, 0x52);
                } else {
                    if (mState == 0x78 || mState == 0x79) {
                        mTimer2 = 0x50;
                        mTimer3 = 0x51;
                    } else {
                        mTimer2 = 0x4e;
                        mTimer3 = 0x4f;
                    }
                    if (mSlot == 0) {
                        if (Random(0) & 1) {
                            SetAnimation(RESOURCE_BCA0, mTimer2);
                        } else {
                            SetAnimation(RESOURCE_BCA0, mTimer3);
                        }
                    } else if (mState == 0x76 || mState == 0x78) {
                        SetAnimation(RESOURCE_BCA0, mPartner->mModel.mAnimId);
                    } else if (mTimer2 == mPartner->mModel.mAnimId) {
                        SetAnimation(RESOURCE_BCA0, mTimer3);
                    } else {
                        SetAnimation(RESOURCE_BCA0, mTimer2);
                    }
                }
                if (Random(0) & 1) {
                    Character_SetFace(this, 8);
                } else {
                    Character_SetFace(this, 10);
                }
                if (prevAnim != mModel.mAnimId) {
                    func_0208ef28(&mModel, 1, 0, 0);
                    Character_ResetSoundPhase(this, 0);
                } else {
                    func_0208ef00(&mModel, 1, 0);
                }
                u16 look;
                if (Random(100) < 25) {
                    look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                SetHeadTurn(0, look, 0, 4);
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mState == 0x7a) {
                ModelAnimation *anim = GetCurrentAnimation();
                if (anim != NULL) {
                    if (anim->mAnmObj->frame < (NNS_G3dAnmObjGetNumFrame(anim->mAnmObj) >> 1)) {
                        Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 3, 2);
                    } else {
                        Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 4, 8);
                    }
                }
            } else if (mState == 0x78 || mState == 0x79) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 5, 2);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 0, 2);
            }
            if (GetCurrentAnimation()->mFinished) {
                mCounter++;
                if (mCounter >= 2) {
                    mCounter = 0;
                    if (mTimer > 0) {
                        if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                            Character_ResetSoundPhase(this, 0);
                            if (mState == 0x7a) {
                                SetAnimation(RESOURCE_BCA0, 0x52);
                                mPartner->SetAnimation(RESOURCE_BCA0, 0x52);
                            } else if (mTimer2 == mModel.mAnimId) {
                                SetAnimation(RESOURCE_BCA0, mTimer3);
                                if (mState == 0x76 || mState == 0x78) {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer3);
                                } else {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer2);
                                }
                            } else {
                                SetAnimation(RESOURCE_BCA0, mTimer2);
                                if (mState == 0x76 || mState == 0x78) {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer2);
                                } else {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer3);
                                }
                            }
                        } else {
                            mCounter = 2;
                        }
                    } else {
                        SetStateWithPartner(0x70, 0x70);
                        Character_SetFace(this, 8);
                    }
                } else {
                    Character_ResetSoundPhase(this, 0);
                }
                func_0208ef28(&mModel, 1, 0, 0);
            }
        }
        break;
    case 0x7b:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x7c, 0x7d);
        }
        break;
    case 0x7c:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x70, 0x70);
            }
        }
        break;
    case 0x7d:
        StepRandomAnimationFacingPartner();
        break;
    case 0x7e:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(60) + 60;
        } else {
            StepAnimationFacingPartner(GetIdleAnim());
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x70, 0x70);
            }
        }
        break;
    case 0x7f:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            if (mPrevState > 0x7f && mPrevState <= 0x8a && mPrevState != 0x86) {
                if (mSlot == 0) {
                    int next = Random(2) + 0x80;
                    if (mPrevState == 0x82 || mPrevState == 0x84) {
                        if (next != 0x80) {
                            next = 0x87;
                        }
                    } else if (mPrevState == 0x83 || mPrevState == 0x85) {
                        if (next != 0x81) {
                            next = 0x87;
                        }
                    } else if (mPrevState == 0x80 || mPrevState == 0x81 || mPrevState == 0x87) {
                    } else if ((u32)(mPrevState - 0x88) <= 2) {
                        next = 0x88;
                    } else {
                        next = 0x86;
                    }
                    SetStateWithPartner(next, next);
                }
            } else {
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                SetState(0x86);
                SetHeldItemsShowState(1);
            }
        }
        break;
    case 0x80:
    case 0x81:
        if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mSlot != 0) {
            if (state == 0x80) {
                SetStateWithPartner(0x82, 0x82);
            } else if (state == 0x81) {
                SetStateWithPartner(0x83, 0x83);
            } else {
                SetStateWithPartner(0x7f, 0x7f);
            }
        }
        break;
    case 0x82:
    case 0x83:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroup();
                if (mModel.mAnimId >= 0) {
                    if (mState == 0x82) {
                        if (mModel.mAnimId != 0x5b) {
                            SetAnimation(RESOURCE_BCA0, 0x1a);
                        }
                    } else if (mModel.mAnimId != 0x5c) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
                int r = Random(3);
                if (r < 1) {
                    Character_SetFace(this, 8);
                } else if (r < 2) {
                    Character_SetFace(this, 0xc);
                } else {
                    Character_SetFace(this, 0xb);
                }
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(80) + 400;
                if (mState == 0x82) {
                    SetAnimation(RESOURCE_BCA0, 0x5b);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x5c);
                }
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                func_0208ef00(&mModel, 1, 0);
                u16 look;
                if (Random(100) < 15) {
                    look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                SetHeadTurn(0, look, 0, 4);
            }
        } else {
            mTimer--;
            if (mState == 0x82) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 1, 4);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 2, 2);
            }
            if (GetCurrentAnimation()->mFinished) {
                if (mTimer <= 0) {
                    if (Random(100) < 50) {
                        SetStateWithPartner(0x7f, 0x7f);
                    } else if (mState == 0x82) {
                        SetStateWithPartner(0x84, 0x84);
                    } else {
                        SetStateWithPartner(0x85, 0x85);
                    }
                }
                func_0208ef28(&mModel, 1, 0, 0);
                Character_ResetSoundPhase(this, 0);
            }
        }
        break;
    case 0x84:
    case 0x85:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 0xfa;
                u16 look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                if (mSlot == 0) {
                    func_0208ef00(&mModel, 1, 0);
                    Character_SetFace(this, 0x15);
                } else {
                    if (Random(10) < 4) {
                        func_0208ef00(&mModel, 1, 0);
                    } else {
                        func_0208ef00(&mModel, 0, 0);
                    }
                    if (Random(100) < 65) {
                        look = 0;
                    }
                }
                SetHeadTurn(0, look, 0, 4);
            }
        } else {
            SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), mHeadYaw, 0, 4);
            if (mState == 0x84) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 1, 4);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 2, 2);
            }
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x7f, 0x7f);
            }
        }
        break;
    case 0x86:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x89, 0x8a);
        }
        break;
    case 0x87:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (Random(100) < 25) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, GetFace());
                }
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 60;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
                u16 look;
                if (Random(100) < 40) {
                    look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                SetHeadTurn(0, look, 0, 4);
            }
        } else {
            if (mSlot == 0 && mPartner->mState == 0x87 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetStateWithPartner(0x7f, 0x7f);
                    Character_SetFace(this, GetFace());
                }
            }
        }
        break;
    case 0x88:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            GetAngleAroundPartner(&angle, 0);
            mTargetAngle = angle;
        } else if (StepTurnWithAnimation(GetWalkAnim())) {
            SetHeadTurn(0, 0, 0, 4);
            SetState(0x87);
        }
        break;
    case 0x89:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x88, 0x88);
            }
        }
        break;
    case 0x8a:
        StepRandomAnimationFacingPartner();
        break;
    case 0x8b:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if ((u32)(mPrevState - 0x8c) <= 2) {
                    int r = Random(100);
                    if (r < 65) {
                        SetState(0x8c);
                    } else if (r < 85) {
                        SetState(0x8d);
                    } else {
                        SetState(0x8e);
                    }
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0x8f);
        }
        break;
    case 0x8c:
    case 0x8d:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                if (mState == 0x8c) {
                    StartSound0UnlessGroupA();
                    SetAnimation(RESOURCE_BCA0, 0x3f);
                } else {
                    StartSound3IfAnimOfGroup();
                    SetAnimation(RESOURCE_BCA0, 0x41);
                }
                Character_SetFace(this, 8);
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x96;
                gSoundManager.StartSeqArcLimited(mSoundHandle, 0x25, 4, 2);
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    int r = Random(100);
                    if (r < 65) {
                        SetState(0x8c);
                    } else if (r < 85) {
                        SetState(0x8d);
                    } else {
                        SetState(0x8e);
                    }
                } else if (GetCurrentAnimation()->mFinished || --mTimer2 <= 0) {
                    u16 look;
                    if (Random(10) < 3) {
                        look = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                    } else {
                        look = 0;
                    }
                    SetHeadTurn(0, look, 0, 4);
                    mTimer2 = Random(60) + 0x104;
                    if (mState == 0x8c) {
                        if (Random(100) < 70) {
                            SetAnimation(RESOURCE_BCA0, 0x3f);
                            Character_SetFace(this, GetFaceOfMood(Random(5)));
                        } else {
                            SetAnimation(RESOURCE_BCA0, 0x40);
                            func_0208ef00(&mModel, 1, 0);
                            Character_SetFace(this, 8);
                        }
                    }
                    if (Random(100) < 10) {
                        Character_SetFace(this, 0x15);
                    }
                }
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;
    case 0x8e:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x42);
                Character_SetFace(this, 8);
                if (mModel.mBlendFrames <= 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                FloorPos p;
                GetRandomPosInArea(&p);
                SetTarget(p.x, 0, p.z);
                mHeadTurnFrames = 5;
                mTimer = Random(100) + 500;
                gSoundManager.StartSeqArcLimited(mSoundHandle, 0x25, 4, 2);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 65) {
                    SetState(0x8c);
                } else if (r < 85) {
                    SetState(0x8d);
                } else {
                    SetState(0x8e);
                }
            } else {
                if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
                    FloorPos here = GetFloorPos();
                    if (ClampToWalkArea(&here)) {
                        SetState(0x8d);
                    } else {
                        func_02091598(this, mPartner->GetFloorPos());
                    }
                }
                if (StepWalkLooking(0x1000, 0, 0x199, 0x22, 6)) {
                    FloorPos p;
                    GetRandomPosInArea(&p);
                    SetTarget(p.x, 0, p.z);
                    Character_SetFace(this, GetFaceOfMood(Random(5)));
                }
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;
    case 0x8f:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x90, 0x91);
        }
        break;
    case 0x90:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x92, 0x92);
            }
        }
        break;
    case 0x91:
        StepRandomAnimationFacingPartner();
        break;
    case 0x92:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, 8, 0xff)) {
                mStep++;
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0x8d);
        }
        PlaceHeldItemBetweenHands(0);
        break;
    }
    PlaceHeadIcon();
}

// The states 0x93 to 0xb2: three more of the same shape, with the hubs 0x93, 0x9f and 0xa8.
//   0x93..0x9e: with held item 1, which is kept between the two hands (animations 0x3d, 0x28, 0x3c)
//   0x9f..0xa7: with held item 9 (animations 0x3f, 0x40) while the partner watches (0xa1, 0xa2)
//   0xa8..0xb2: with held items 9 and 5 (animations 0x53, 0x46), sound 3 of archive 0x1c
void RoomCharacter::UpdatePairStates93ToB2() {
    int state = mState;
    switch (state) {
    case 0x93:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int prev = mPrevState;
                if (prev == 0x94 || prev == 0x96) {
                    SetState(0x94);
                } else if ((u32)(prev - 0x97) <= 1) {
                    SetState(0x97);
                } else if (prev == 0x9e || prev == 0x95) {
                    SetState(0x95);
                } else if (prev == 0x9a || prev == 0x9b || prev == 0x9d) {
                    SetState(0x9c);
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0x99);
        }
        break;
    case 0x94:
    case 0x95:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(1, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSoundUnlessGroupB(0);
                SetAnimation(RESOURCE_BCA0, 0x3d);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0xc000 - mAngle;
                if (mState == 0x95) {
                    VecFx32 mid;
                    GetMidpoint(GetNode(2), GetNode(1), &mid);
                    mid.y = 0xcc;
                    mHeldItems[0]->SetPosition(mid);
                } else {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 2);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (mState == 0x94) {
                    if (Random(100) < 15) {
                        u16 look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
                        SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), look, 0, 4);
                    }
                    if (Random(100) < 10) {
                        Character_SetFace(this, 0x15);
                    }
                }
            }
            if (mState == 0x95) {
                VecFx32 mid;
                GetMidpoint(GetNode(2), GetNode(1), &mid);
                mid.y = 0xcc;
                mHeldItems[0]->SetPosition(mid);
                mTimer = 1;
            } else {
                mTimer = Random(100) + 300;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 15 && mState != 0x95) {
                    SetState(0x96);
                } else {
                    SetState(0x94);
                }
            }
        }
        break;
    case 0x96:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x28);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 6);
                AnimModel_FinishAnimations(mHeldItems[0]);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (Random(0) & 1) {
                    Character_SetFace(this, 7);
                }
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 5, 2);
            if (GetCurrentAnimation()->mFinished) {
                mStep++;
                mTimer = 0x14;
                Character_SetFace(this, 6);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x94);
                Character_SetFace(this, 8);
                gSoundManager.StartSeqArc(1, 0x1f, 6);
            }
        }
        break;
    case 0x97:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                int r = Random(100);
                if (r < 60) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                } else if (r < 85) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x33);
                }
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(100) + 300;
                if (Random(100) < 70 || mModel.mAnimId == 0x33) {
                    HeldItem *item = mPartner->mHeldItems[0];
                    u16 look = GetHeadYawTo(item->GetFloorPos(), 0x3000);
                    SetHeadTurn(GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), item->mPosition, 0x1800), look, 0, 4);
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 70) {
                    SetState(0x97);
                } else {
                    SetState(0x98);
                }
            }
            if (mModel.mAnimId == 0x33) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 8, 1);
            }
        }
        break;
    case 0x98:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StepAnimationThenFacePartner(0x1d);
                mTimer = Random(60) + 100;
            }
        } else if (StepAnimationThenFacePartner(0x1d) && mTimer <= 0) {
            SetState(0x97);
        } else {
            mTimer--;
        }
        break;
    case 0x99:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x9a, 0x9b);
        }
        break;
    case 0x9a:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x9c, 0x9c);
            }
        }
        break;
    case 0x9b:
        StepRandomAnimationFacingPartner();
        break;
    case 0x9c:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            GetAngleAroundPartner(&angle, 0x1000);
            mTargetAngle = angle;
        } else if (StepTurnWithAnimation(GetWalkAnim())) {
            SetHeadTurn(0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0x9d);
            } else {
                SetState(0x97);
            }
        }
        break;
    case 0x9d: {
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 1, 0xff)) {
                mStep++;
                mHeldItems[0]->mRotY = 0xc000 - mAngle;
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0x9e);
        }
        VecFx32 mid;
        GetMidpoint(GetNode(2), GetNode(1), &mid);
        mid.y = GetNode(2)->mPos.y;
        mHeldItems[0]->SetPosition(mid);
        break;
    }
    case 0x9e: {
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3c);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0xc000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (GetCurrentAnimation()->mFinished) {
            SetState(0x95);
        }
        VecFx32 mid;
        GetMidpoint(GetNode(2), GetNode(1), &mid);
        mid.y = GetNode(2)->mPos.y;
        mHeldItems[0]->SetPosition(mid);
        break;
    }
    case 0x9f:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int prev = mPrevState;
                if (prev == 0xa0) {
                    SetState(0xa0);
                } else if ((u32)(prev - 0xa1) <= 1) {
                    SetState(0xa1);
                } else if ((u32)(prev - 0xa4) <= 2) {
                    SetState(0xa4);
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0xa3);
        }
        break;
    case 0xa0:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x3f);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x96;
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    SetState(0xa0);
                } else if (GetCurrentAnimation()->mFinished || --mTimer2 <= 0) {
                    mTimer2 = Random(60) + 0xaa;
                    if (Random(100) < 70) {
                        SetAnimation(RESOURCE_BCA0, 0x3f);
                        Character_SetFace(this, GetFaceOfMood(Random(5)));
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x40);
                        func_0208ef00(&mModel, 1, 0);
                        Character_SetFace(this, 8);
                        Character_ResetSoundPhase(this, 0);
                    }
                    if (Random(100) < 8) {
                        SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x2800), 0, 4);
                    }
                    if (Random(100) < 30) {
                        Character_SetFace(this, 0x15);
                    }
                }
            }
            if (mModel.mAnimId == 0x40) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1c, 4, 2);
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;
    case 0xa1:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 65) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                    Character_SetFace(this, GetFace());
                } else if (r < 85) {
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
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (Random(100) < 70) {
                    if (mModel.mAnimId != 0x17) {
                        HeldItem *item = mPartner->mHeldItems[0];
                        u16 look = GetHeadYawTo(item->GetFloorPos(), 0x3000);
                        SetHeadTurn(GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), item->mPosition, 0x1800), look, 0, 4);
                    }
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 20 && mModel.mAnimId != 0x17) {
                    SetState(0xa2);
                } else {
                    SetState(0xa1);
                }
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;
    case 0xa2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 100;
            }
        } else if (StepAnimationThenFacePartner(0x1d) && mTimer <= 0) {
            SetState(0xa1);
        } else {
            mTimer--;
        }
        break;
    case 0xa3:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xa5, 0xa6);
        }
        break;
    case 0xa4:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            GetAngleAroundPartner(&angle, 0x1000);
            mTargetAngle = angle;
        } else if (StepTurnWithAnimation(GetWalkAnim())) {
            SetHeadTurn(0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0xa7);
            } else {
                SetState(0xa1);
            }
        }
        break;
    case 0xa5:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xa4, 0xa4);
            }
        }
        break;
    case 0xa6:
        StepRandomAnimationFacingPartner();
        break;
    case 0xa7:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, 9, 0xff)) {
                mStep++;
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0xa0);
        }
        PlaceHeldItemBetweenHands(0);
        break;
    case 0xa8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int prev = mPrevState;
                if ((u32)(prev - 0xa9) <= 1) {
                    SetState(0xa9);
                } else if ((u32)(prev - 0xab) <= 2) {
                    SetState(0xab);
                } else if ((u32)(prev - 0xaf) <= 2) {
                    SetState(0xaf);
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0xae);
        }
        break;
    case 0xa9:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x53);
                Character_SetFace(this, 8);
                mHeldItems[1]->mRotZ = 0x8000;
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x8c;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 3);
                UpdateVoiceOfSound();
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 80 || ClampToRect4E4(NULL)) {
                    SetState(0xa9);
                } else {
                    SetState(0xaa);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(60) + 0x8c;
                    if (GetCurrentAnimation()->mFinished) {
                        func_0208ef00(&mModel, 0, 0);
                        SetHeadTurn(0, 0, 0, 4);
                        gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 3);
                        UpdateVoiceOfSound();
                    } else if (Random(100) < 30) {
                        u16 lookA;
                        u16 lookB;
                        if (Random(100) < 60) {
                            lookA = -((Random(9) << 9) + 0x600);
                            lookB = 0;
                        } else {
                            lookA = 0xf700;
                            lookB = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                        }
                        SetHeadTurn(lookA, lookB, 0, 4);
                        func_0208ef00(&mModel, 1, 1);
                        gSoundManager.StopSeqArc(mSoundHandle, 0);
                    }
                }
            }
        }
        PlaceHeldItemsInHands();
        break;
    case 0xaa:
        if (mStep == 0) {
            mStep++;
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x46);
                Character_SetFace(this, 6);
                mHeldItems[1]->mRotZ = 0x8000;
                mTimer = Random(60) + 0x14a;
                if (Random(0) & 1) {
                    Character_SetFace(this, 7);
                }
                gSoundManager.StartSeqArc(1, 0x1e, 1);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0xa9);
            } else if (mTimer == 0x96) {
                Character_SetFace(this, 8);
            } else if (mTimer == 0x10e) {
                Character_SetFace(this, 6);
            }
        }
        PlaceHeldItemsInHands();
        break;
    case 0xab:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 65) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                    Character_SetFace(this, GetFace());
                } else if (r < 85) {
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
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (Random(100) < 70) {
                    if (mModel.mAnimId != 0x17) {
                        u16 look = GetHeadYawTo(mPartner->mHeldItems[0]->GetFloorPos(), 0x3000);
                        SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), look, 0, 4);
                    }
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            if (mPartner->mState == 0xaa) {
                mTimer--;
            }
            mTimer--;
            if (mTimer <= 0) {
                if (mModel.mAnimId == 0x17) {
                    SetState(0xab);
                } else {
                    int r = Random(100);
                    if (mPartner->mState == 0xaa) {
                        r -= 60;
                    }
                    if (r < 20) {
                        SetState(0xac);
                    } else if (r < 85) {
                        SetState(0xab);
                    } else {
                        SetState(0xad);
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;
    case 0xac:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 0x78;
            }
        } else if (StepAnimationThenFacePartner(0x1d) && mTimer <= 0) {
            SetState(0xab);
        } else {
            mTimer--;
        }
        break;
    case 0xad:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 0xb4;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), mUnknown25D6, 0, 4);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(10) < 5) {
                    SetState(0xab);
                } else {
                    SetState(0xad);
                }
            }
        }
        break;
    case 0xae:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xb0, 0xb1);
        }
        break;
    case 0xaf:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                RestartStepSound();
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetTargetFloorPos(mPartner->GetFloorPos());
                ShowHeadIcon();
                SetHeldItemsShowState(1);
            }
        } else if (StepTurnLooking(0x100, 0, 0x3800)) {
            SetHeadTurn(0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0xb2);
            } else {
                SetState(0xab);
            }
        }
        break;
    case 0xb0:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xaf, 0xaf);
            }
        }
        break;
    case 0xb1:
        StepRandomAnimationFacingPartner();
        break;
    case 0xb2:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, 9, 5)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                mHeldItems[1]->mRotZ = 0x8000;
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0xa9);
        }
        PlaceHeldItemsInHands();
        break;
    }
    PlaceHeadIcon();
}
