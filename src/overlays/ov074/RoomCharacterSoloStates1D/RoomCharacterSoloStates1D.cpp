#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "../RoomCharacter.h"

// The states 0x1d to 0x3f: what a character does on its own (docs/notes/ov074.md has the notes on the
// states). Each family has a chooser state (0x1d, 0x22, 0x29, 0x2e, 0x34) that picks the next
// state at random when the previous state was of the same family, and otherwise resets the animation and goes to
// the family's entry state.
//   0x1d..0x21 and 0x22..0x26: the same activity with held item 9 and with held item 8 (animations 0x3f..0x42)
//   0x27, 0x28: animation 6
//   0x29..0x2d: animations 0x5b/0x5c, or StepWalkToRandomPos
//   0x2e..0x33: with held item 6, placed by HeldItem::SetJoint every frame, standing and walking about
//   0x34..0x3c: with held items 2 and 3, after walking to data_ov074_0216d47c
//   0x3d..0x3f: walks to the corner of the walk area away from where it is and stays there (animation 0x17)
void RoomCharacter::UpdateSoloStates1DTo3F() {
    switch (mState) {
    case 0x1d:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if ((u32)(mPrevState - 0x1e) <= 2) {
                int r = Random(100);
                if (r < 60) {
                    SetState(0x1e);
                } else if (r < 80) {
                    SetState(0x1f);
                } else {
                    SetState(0x20);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x21);
        }
        break;

    case 0x1e:
    case 0x23:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(mState == 0x1e ? 9 : 8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x3f);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 0x1f4;
                mTimer2 = Random(0x3c) + 0x96;
                if (mState == 0x23) {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 4);
                }
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    if (mState == 0x1e) {
                        SetState(0x1d);
                    } else {
                        SetState(0x22);
                    }
                } else if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished || --mTimer2 <= 0) {
                    if (mState == 0x1e) {
                        mTimer2 = Random(60) + 0xaa;
                    } else {
                        mTimer2 = Random(60) + 0x104;
                    }
                    if (Random(100) < 70) {
                        SetAnimation(RESOURCE_BCA0, 0x3f);
                        Character_SetFace(this, GetFaceOfMood(Random(5)));
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x40);
                        func_0208ef00(&mModel, TRUE, FALSE);
                        Character_SetFace(this, 8);
                        Character_ResetSoundPhase(this, 0);
                    }
                }
            }
            if (mState == 0x1e && mModel.mAnimId == 0x40) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1c, 4, 2);
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;

    case 0x1f:
    case 0x24:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            RestartStepSound();
        } else if (mStep == 1) {
            if (StepChangeHeldItems(mState == 0x1f ? 9 : 8, 0xff) && mModel.mBlendFrames <= 0) {
                FloorPos pos;
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x42);
                Character_SetFace(this, 8);
                GetRandomPosInArea(&pos);
                SetTarget(pos.x, 0, pos.z);
                mHeadTurnFrames = 5;
                mTimer = Random(100) + 0x1f4;
                if (mState == 0x24) {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 4);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0x1f) {
                    SetState(0x1d);
                } else {
                    SetState(0x22);
                }
            }
            if (StepWalkLooking(0x1000, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 70) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTarget(pos.x, 0, pos.z);
                Character_SetFace(this, GetFaceOfMood(Random(5)));
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;

    case 0x20:
    case 0x25:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(mState == 0x20 ? 9 : 8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x41);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 0x190;
                if (mState == 0x25) {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 4);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0x20) {
                    SetState(0x1d);
                } else {
                    SetState(0x22);
                }
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;

    case 0x21:
    case 0x26:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, mState == 0x21 ? 9 : 8, 0xff)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                if (mState == 0x21) {
                    SetState(0x20);
                } else {
                    SetState(0x25);
                }
            }
        }
        PlaceHeldItemBetweenHands(0);
        break;

    case 0x22:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if ((u32)(mPrevState - 0x23) <= 2) {
                int r = Random(100);
                if (r < 60) {
                    SetState(0x23);
                } else if (r < 80) {
                    SetState(0x24);
                } else {
                    SetState(0x25);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetState(0x26);
        }
        break;

    case 0x27:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x28) {
                SetState(0x28);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            SetHeldItemsShowState(1);
        } else {
            SetState(0x28);
        }
        break;

    case 0x28:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer2 = 100;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 6);
            Character_SetFace(this, 8);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0xd100, 0, 0, 5);
        } else {
            if (mTimer2-- <= 0) {
                if (Random(0) & 1) {
                    Character_SetFace(this, 1);
                    mTimer2 = 0x50;
                } else {
                    Character_SetFace(this, 6);
                    mTimer2 = 0x3c;
                }
            }
        }
        break;

    case 0x29:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            {
                int next;
                if (Random(100) < 10) {
                    next = 0x2c;
                } else {
                    next = Random(2) + 0x2a;
                }
                if (next == mPrevState || mPrevState == 0x2d) {
                    SetState(next);
                } else {
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                    SetState(0x2d);
                }
            }
            SetHeldItemsShowState(1);
        }
        break;

    case 0x2a:
    case 0x2b:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroup();
            if (mModel.mAnimId >= 0) {
                if (mState == 0x2a) {
                    if (mModel.mAnimId != 0x5b) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                } else {
                    if (mModel.mAnimId != 0x5c) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
            {
                int r = Random(3);
                if (r < 1) {
                    Character_SetFace(this, 8);
                } else if (r < 2) {
                    Character_SetFace(this, 0xc);
                } else {
                    Character_SetFace(this, 0xb);
                }
            }
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(0x50) + 0x12c;
            if (mState == 0x2a) {
                SetAnimation(RESOURCE_BCA0, 0x5b);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x5c);
            }
            func_0208ef00(&mModel, TRUE, FALSE);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            mTimer--;
            if (mState == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 1, 4);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 2, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                if (mTimer <= 0) {
                    SetState(0x29);
                    Character_SetFace(this, 8);
                }
                func_0208ef28(&mModel, TRUE, 0, FALSE);
                Character_ResetSoundPhase(this, 0);
            }
        }
        break;

    case 0x2c:
        if (StepWalkToRandomPos()) {
            SetState(0x29);
        }
        break;

    case 0x2d:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1a);
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
            if (Random(2) >= 1) {
                mTimer = 1;
            } else {
                mTimer = Random(0x3c) + 0x28;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
            }
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(0x29);
            }
        }
        break;

    case 0x2e:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if ((u32)(mPrevState - 0x2f) <= 1) {
                if (Random(100) < 25) {
                    SetState(0x33);
                } else {
                    SetState(Random(2) + 0x2f);
                }
            } else if ((u32)(mPrevState - 0x32) <= 1) {
                SetState(0x32);
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x31);
        }
        break;

    case 0x2f:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(6, 0xff) && mModel.mBlendFrames <= 0) {
                u16 angle;
                StartSound3IfAnimOfGroup();
                FloorPos here = GetFloorPos();
                angle = SnapAngle(FX_Atan2Idx(here.z, here.x));
                if (angle != mAngle) {
                    mStep++;
                    RestartStepSound();
                    if (mModel.mAnimId != 0x34 && mModel.mAnimId != 0x35) {
                        Character_ResetSoundPhase(this, 1);
                    }
                    if (mModel.mAnimId != 0x35) {
                        SetAnimation(RESOURCE_BCA0, 0x14);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x35);
                    }
                    Character_SetFace(this, 8);
                    mTargetAngle = angle;
                } else {
                    mStep += 2;
                    SetAnimation(RESOURCE_BCA0, 0x34);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                mHeldItems[0]->SetRotation(0xe000, 0, 0xf000);
            }
        } else if (mStep == 2) {
            u16 yaw;
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0xe0, 0)) {
                mStep++;
                yaw = 0;
            } else {
                yaw = GetHeadYaw(-mWalkAngle, 0x2000);
            }
            SetHeadTurn(0, yaw, 0, 5);
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x34);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0xe000, 0, 0xf000);
            }
        } else if (mStep == 4) {
            mStep++;
            mTimer = Random(100) + 0xfa;
            {
                int r = Random(3);
                if (r == 0) {
                    Character_SetFace(this, 8);
                } else if (r == 1) {
                    Character_SetFace(this, 0x12);
                } else {
                    Character_SetFace(this, 0x10);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 25) {
                    SetState(0x33);
                } else {
                    SetState(Random(2) + 0x2f);
                }
            }
        }
        if (mModel.mAnimId == 0x34 || mModel.mAnimId == 0x35) {
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1c, 1, 0x14);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x30:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(6, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                if (mModel.mAnimId != 0x34 && mModel.mAnimId != 0x35) {
                    Character_ResetSoundPhase(this, 1);
                }
                SetAnimation(RESOURCE_BCA0, 0x35);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0xe000, 0, 0xf000);
            }
        } else if (mStep == 2) {
            mStep++;
            mTimer = Random(0x3c) + 0x118;
            StepWalkLooking(0x8000, 0, 0x199, 0x22, 6);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 25) {
                    SetState(0x33);
                } else {
                    SetState(Random(2) + 0x2f);
                }
            }
            if (StepWalkLooking(0x2400, -0x1800, 0x199, 0x22, 5)) {
                FloorPos target;
                FloorPos pos;
                target.x = mTarget.x;
                target.z = mTarget.z;
                GetWalkAreaCorner(target, &pos);
                SetTargetXZ(pos.x, pos.z);
            }
        }
        if (mModel.mAnimId == 0x34 || mModel.mAnimId == 0x35) {
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1c, 1, 0x14);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x31:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 6, 0xff)) {
                mStep++;
                mHeldItems[0]->mRotX = 0xe000;
            }
        } else {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                SetState(0x32);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x32:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                FloorPos pos;
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, 0x14);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0xe000, 0, 0);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                GetRandomEdgePos(&pos);
                SetTarget(pos.x, 0, pos.z);
            }
        } else {
            if (StepWalkLooking(0x3800, 0, 0x199, 0x22, 6)) {
                SetState(0x2f);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x33:
        if (mStep == 0) {
            mStep++;
            mHeldItems[0]->SetRotation(0xe000, 0, 0);
            FloorPos here = GetFloorPos();
            mTargetAngle = FX_Atan2Idx(here.z, here.x) + 0x8000;
            ShowHeadIcon();
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(0x14)) {
                mStep++;
            }
            mHeldItems[0]->SetShowState(0);
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(0x3c) + 0x78;
                SetAnimation(RESOURCE_BCA0, 7);
            }
        } else {
            if (mTimer-- <= 0) {
                SetState(0x32);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x34:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState > 0x34 && mPrevState <= 0x3a) {
                int r = Random(100);
                if (r < 35) {
                    SetState(0x35);
                } else if (r < 70) {
                    SetState(0x36);
                } else {
                    SetState(0x38);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x3c);
        }
        break;

    case 0x35:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (Random(0) & 1) {
                    SetAnimation(RESOURCE_BCA0, 0x59);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x5a);
                }
                func_0208ef00(&mModel, TRUE, FALSE);
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                mTimer = Random(300) + 0x5dc;
                mTimer2 = Random(2) + 1;
                mTimer3 = Random(100) + 200;
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    SetState(0x39);
                    Character_SetFace(this, 8);
                    SetAnimation(RESOURCE_BCA0, 0x59);
                    func_0208ef00(&mModel, FALSE, FALSE);
                } else if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        int face;
                        mTimer2 = Random(2) + 1;
                        face = Random(100);
                        if (Random(100) < 50) {
                            SetAnimation(RESOURCE_BCA0, 0x59);
                            Character_SetFace(this, 8);
                        } else {
                            SetAnimation(RESOURCE_BCA0, 0x5a);
                            if (face < 50) {
                                Character_SetFace(this, 0x15);
                            } else {
                                Character_SetFace(this, 8);
                            }
                        }
                        func_0208ef28(&mModel, TRUE, 0, FALSE);
                    } else {
                        func_0208ef28(&mModel, TRUE, 0, FALSE);
                    }
                } else {
                    mTimer3--;
                    if (mTimer3 <= 0) {
                        mTimer3 = Random(0x118) + 0x1e;
                        mTimer3 += ((0x1000 - mModel.mUnknownD8) * 0x3c) >> 12;
                        if (mModel.mAnimId == 0x5a) {
                            SetAnimation(RESOURCE_BCA0, 0x59);
                            Character_SetFace(this, 8);
                            func_0208ef00(&mModel, TRUE, FALSE);
                        }
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x5a) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x25, 1, 4);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x36:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (Random(0) & 1) {
                    SetAnimation(RESOURCE_BCA0, 0x2f);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x30);
                }
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                mTimer = Random(300) + 0x5dc;
                mTimer2 = Random(200) + 0x32;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mModel.mBlendFrames <= 0) {
                    SetState(0x39);
                    SetAnimation(RESOURCE_BCA0, 0x2f);
                    Character_SetFace(this, 8);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    int face;
                    mTimer2 = Random(0x96) + 0x78;
                    mTimer2 += ((0x1000 - mModel.mUnknownD8) * 0x32) >> 12;
                    face = Random(100);
                    if (Random(100) < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x2f);
                        Character_SetFace(this, 8);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x30);
                        if (face < 20) {
                            Character_SetFace(this, 0x15);
                        } else if (face < 40) {
                            Character_SetFace(this, 0xb);
                        } else if (face < 60) {
                            Character_SetFace(this, 0x10);
                        } else {
                            Character_SetFace(this, 8);
                        }
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

    case 0x37:
    case 0x38:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x2d);
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                if (mState == 0x37) {
                    mTimer = Random(4) + 0xe;
                } else {
                    mTimer = 0;
                }
                mTimer2 = Random(0x3c) + 0x3c;
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mStep++;
                    mTimer2 = 0xfa;
                    SetAnimation(RESOURCE_BCA0, 0x2d);
                    Character_SetFace(this, 8);
                }
            }
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0) {
                    mStep++;
                    mTimer2 = Random(0x32) + 0x3c;
                    func_0208ef00(&mModel, TRUE, FALSE);
                    Character_SetFace(this, 8);
                }
            }
        } else if (mStep == 4) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mStep++;
                    mTimer2 = Random(0x1e) + 0x32;
                    SetAnimation(RESOURCE_BCA0, 0x2e);
                    func_0208ef28(&mModel, TRUE, 0, FALSE);
                    mTimer++;
                }
            }
        } else if (mStep == 5) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    if (mTimer >= 0x15 || (mTimer >= 0xb && Random(100) < (mTimer * 3) >> 3)) {
                        mStep++;
                        mTimer = 300;
                        mTimer2 = 0;
                        SetAnimation(RESOURCE_BCA0, 0x2d);
                        Character_SetFace(this, 8);
                    } else {
                        int r;
                        mStep = 3;
                        mTimer2 = Random(0x32) + 0xfa;
                        SetAnimation(RESOURCE_BCA0, 0x2d);
                        r = Random(100);
                        if (r < 15) {
                            Character_SetFace(this, 2);
                        } else if (r < 30) {
                            Character_SetFace(this, 4);
                        } else if (r < 45) {
                            Character_SetFace(this, 6);
                        } else if (r < 60) {
                            Character_SetFace(this, 5);
                        } else {
                            Character_SetFace(this, 8);
                        }
                    }
                }
            }
        } else {
            mTimer2++;
            if (mTimer2 > 0x3c) {
                mTimer--;
                if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0) {
                    SetState(0x39);
                }
            }
        }
        if (mModel.mAnimId == 0x2e) {
            Character_PlaySoundAtAnimPhase(this, 0, mSoundHandle, 0x25, 2, 4);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x39:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(0x14) + 0x32;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
            Character_SetFace(this, 8);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    int r;
                    gSoundManager.StopSeqArc(1, 0);
                    Character_ResetSoundPhase(this, 0);
                    mStep++;
                    r = Random(100);
                    if (r < 25) {
                        SetAnimation(RESOURCE_BCA0, 0x2a);
                        Character_SetFace(this, 2);
                        mTimer2 = Random(2) + 2;
                        Character_ResetSoundPhase(this, 0);
                    } else if (r < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x16);
                        Character_SetFace(this, 4);
                        mTimer2 = 1;
                    } else if (r < 75) {
                        SetAnimation(RESOURCE_BCA0, 0x54);
                        mTimer2 = Random(3) + 1;
                        Character_ResetSoundPhase(this, 0);
                        if (mTimer2 == 1) {
                            Character_SetFace(this, 5);
                        } else {
                            Character_SetFace(this, 6);
                        }
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x43);
                        mTimer2 = 1;
                        Character_SetFace(this, 1);
                    }
                    func_0208ef00(&mModel, TRUE, FALSE);
                    ShowHeadIcon();
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mStep++;
                    if (mModel.mAnimId == 0x16) {
                        mTimer = 0x32;
                    } else if (mModel.mAnimId == 0x43) {
                        mTimer = 0x6e;
                    } else {
                        mTimer = 0;
                    }
                } else {
                    func_0208ef28(&mModel, TRUE, 0, FALSE);
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            } else if (mModel.mAnimId == 0x54) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x20, 3, 2);
            }
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    mStep++;
                    mTimer = Random(0x50) + 0x78;
                    SetAnimation(RESOURCE_BCA0, 7);
                    Character_SetFace(this, 8);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x3a);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x3a:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                mTimer = Random(0x3c) + 0x96;
                SetAnimation(RESOURCE_BCA0, 0x4d);
                Character_SetFace(this, 8);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 4);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 35) {
                    SetState(0x35);
                } else if (r < 70) {
                    SetState(0x36);
                } else {
                    SetState(0x38);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x3b:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 2, 3)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                PlaceHeldItemsAtStart();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x5a;
                SetAnimation(RESOURCE_BCA0, 0x4d);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 35) {
                    SetState(0x35);
                } else if (r < 70) {
                    SetState(0x36);
                } else {
                    SetState(0x38);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x3c:
        if (mStep == 0) {
            if (StepWalkTo(data_ov074_0216d47c)) {
                mStep += 2;
                mSubStep = 0;
                mTargetAngle = 0x4000;
            }
        } else if (mStep == 2) {
            u16 yaw;
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0xa0, 0)) {
                mStep++;
                yaw = 0;
            } else {
                yaw = GetHeadYaw(-mTargetAngle, 0x3800);
            }
            SetHeadTurn(0, yaw, 0, 4);
            Character_MoveAlongAngle(this);
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x3b);
            SetAnimation(RESOURCE_BCA0, 7);
        }
        break;

    case 0x3d:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x3e) {
                SetState(0x3e);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            SetHeldItemsShowState(1);
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x3f);
        }
        break;

    case 0x3e:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, 0x17);
            func_0208ef28(&mModel, TRUE, 0x24000, TRUE);
            Character_SetFace(this, 8);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            mTimer = 500;
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mTimer = 500;
                ShowHeadIcon();
            }
        }
        break;

    case 0x3f:
        if (mStep == 0) {
            fx32 x;
            fx32 z;
            mStep++;
            FloorRect *area = &gWalkArea;
            x = mModel.mPos.x < 0 ? area->x : -area->x;
            z = mModel.mPos.z < 0 ? area->z : -area->z;
            SetTarget(x, 0, z);
        } else if (mStep == 1) {
            if (StepWalkTo(GetTargetFloorPos())) {
                mStep++;
                mSubStep = 0;
            }
        } else if (mStep == 2) {
            mStep++;
            FloorPos twice = GetTargetFloorPos();
            twice.x *= 2;
            twice.z *= 2;
            mTargetAngle = FX_Atan2Idx(twice.z, twice.x);
            func_02091fbc(this, 1, 0, 0x1a);
        } else {
            u16 yaw;
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0xa0, 0)) {
                SetState(0x3e);
                yaw = 0;
            } else {
                yaw = GetHeadYaw(-mWalkAngle, 0x3800);
                mHeadTurnActive = TRUE;
            }
            SetHeadTurn(0, yaw, 0, 5);
            Character_MoveAlongAngle(this);
        }
        break;
    }
    PlaceHeadIcon();
}
