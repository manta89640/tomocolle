#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "RoomCharacter.h"

extern "C" {

// Main program, not named yet. All take the character (or its model, a held item or the head icon) as their object.
void Character_SetFace(RoomCharacter *self, int face); // Sets the face (expression) and restarts its animation
ModelAnimation *AnimModel_FindAnimation(CharacterModel *model, u32 resType, int id); // Finds an animation of the model
void AnimModel_FinishAnimations(HeldItem *item); // Marks all the item's animations finished
void func_0208ef00(CharacterModel *model, int playOnce, int finished); // Sets the current animation's two flags
void func_0208ef28(CharacterModel *model, int playOnce, fx32 frame, int finished); // The same and its frame
void Character_ResetSoundPhase(RoomCharacter *self, int slot); // Restarts slot's progress of Character_PlaySoundAtAnimPhase
// Starts sound `index` of sequence archive `seqArc` at given quarters (bit mask) of the current animation
void Character_PlaySoundAtAnimPhase(RoomCharacter *self, int slot, int handle, int seqArc, int index, int quarters);
void Character_SetWalkTarget(RoomCharacter *self, VecFx32 target); // Sets mTarget
BOOL Character_TurnToWalkTarget(RoomCharacter *self, int speed, int accel); // Turns towards mTarget; TRUE when facing it
void Character_SetAngle(RoomCharacter *self, u16 angle); // Sets mAngle (and the angle at 0x2b08)

// Overlay 74, group a7's range, not named yet
void func_ov074_021691d4(RoomCharacter *self, u16 pitch, u16 yaw, u16 roll, u8 frames); // Turns the head
u16 func_ov074_02169280(RoomCharacter *self, VecFx32 from, VecFx32 to, u16 limit); // Head pitch from..to, limited
// One step of walking to mTarget with sound index of archive seqArc; TRUE when there. Turns the head by headYaw
// less the walking direction when headLimit is below 0x4000
BOOL func_ov074_02169388(RoomCharacter *self, u32 headLimit, int headYaw, fx32 speed, int seqArc, int index);
void func_ov074_021694e8(RoomCharacter *self); // Character_ResetSoundPhase(this, 0) unless the animation is a moving one
void func_ov074_0216950c(RoomCharacter *self, int a); // Calls 0x02169eac(item, a) for both held items
void func_ov074_02169598(RoomCharacter *self, fx32 y); // Puts held item 0 between the hands, at height y
void func_ov074_02169634(RoomCharacter *self); // Puts held item 1 in the hand (joint 2) and item 0 at joint 0
void func_ov074_021696b8(RoomCharacter *self); // Puts held item 0 at joint 0, turned by the head's yaw
void func_ov074_02169a98(RoomCharacter *self); // Sets volume and pitch of mSoundHandle's sound by the character
void func_ov074_02169b00(RoomCharacter *self); // Sound 3 of archive 0x1e when the animation is of group A or B
void func_ov074_02169b68(RoomCharacter *self); // Sound 0 of archive 0x1e unless the animation is of group A
void func_ov074_02169bb4(RoomCharacter *self, int index); // Sound index of archive 0x1e unless of group B
void func_ov074_02169f48(HeldItem *item, MtxFx43 *joint, MtxFx33 *rotation); // Puts the item at a joint
// The same, moved by an offset in the joint's own directions
void func_ov074_02169f78(HeldItem *item, MtxFx43 *joint, MtxFx33 *rotation, fx32 x, fx32 y, fx32 z);

} // extern "C"

// Main program, not named yet: these two return a VecFx32
extern "C" VecFx32 CharacterModel_GetJointPosition(CharacterModel *model, int joint); // Position of one of the model's joints
extern "C" VecFx32 func_0208fc34(CharacterModel *model);            // Three scale factors from the model's 0xec, 0xf0

#define RES_TYPE_BCA0 0x30414342 // 'BCA0': a character animation

// The SDK's MATH_Rand16 on the game's generator, as a signed number: its results compare signed
static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

inline ModelAnimation *RoomCharacter::GetCurrentAnimation() {
    return AnimModel_FindAnimation(&mModel, RES_TYPE_BCA0, mModel.mAnimId);
}

inline BOOL RoomCharacter::IsAnimationFinished() {
    return GetCurrentAnimation()->mFinished;
}

// The states 0x40 to 0x67: ten activities of one character on its own. Each has a hub state (0x40, 0x44, 0x48,
// 0x4f, 0x54, 0x58, 0x5b, 0x5e, 0x61, 0x65) that waits for the animation change and then either goes on with the
// activity, when the state before was one of its own, or resets the animation (0x1a) and enters it.
void RoomCharacter::UpdateSoloStates40To67() {
    switch (mState) {
    case 0x40:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x41 || mPrevState == 0x42 || mPrevState == 0x43) {
                SetState(Random(3) + 0x41);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RES_TYPE_BCA0, 0x1A);
            }
            func_ov074_0216950c(this, 1);
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(Random(3) + 0x41);
        }
        break;

    case 0x41:
    case 0x42:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 600;
            mTimer2 = 0;
            mTimer3 = 200;
            if (mState == 0x41) {
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 2);
            } else {
                func_ov074_02169b68(this);
                SetAnimation(RES_TYPE_BCA0, 0x19);
            }
            Character_SetFace(this, 0x12);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 150;
                if (Random(2) == 0) {
                    Character_SetFace(this, 9);
                } else {
                    Character_SetFace(this, 10);
                }
            }
            if (mTimer3-- <= 0) {
                mTimer3 = 1000;
                if (Random(100) < 30) {
                    func_ov074_021691d4(this, 0, (0x28 - Random(0x50)) << 8, 0, 5);
                    mHeadTurnActive = 1;
                }
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x40);
                Character_SetFace(this, 8);
            }
        }
        break;

    case 0x43:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 600;
            mTimer2 = 0;
            func_ov074_02169b00(this);
            func_ov074_021694e8(this);
            SetAnimation(RES_TYPE_BCA0, 0xC);
            Character_SetFace(this, 0x12);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
            FloorPos pos;
            GetRandomPosInArea(&pos);
            SetTarget(pos.x, pos.z);
        } else {
            if (func_ov074_02169388(this, 0x3000, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 60) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTarget(pos.x, pos.z);
            }
            if (mTimer2-- <= 0) {
                mTimer2 = 150;
                if (Random(2) == 0) {
                    Character_SetFace(this, 9);
                } else {
                    Character_SetFace(this, 10);
                }
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x40);
                Character_SetFace(this, 8);
            }
        }
        break;

    case 0x44:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x45 || mPrevState == 0x46) {
                if (Random(10) < 7) {
                    SetState(0x45);
                } else {
                    SetState(0x46);
                }
            } else {
                func_ov074_0216950c(this, 1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x47);
        }
        break;

    case 0x45:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(7, 0xFF) && mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(100) + 700;
                int r = Random(100);
                if (r < 30) {
                    func_ov074_02169b00(this);
                    SetAnimation(RES_TYPE_BCA0, GetMoodAnimA());
                } else if (r < 33) {
                    func_ov074_02169b00(this);
                    mTimer >>= 2;
                    SetAnimation(RES_TYPE_BCA0, 0x4B);
                } else {
                    func_ov074_02169b68(this);
                    if (mMood == 3) {
                        SetAnimation(RES_TYPE_BCA0, 0x18);
                    } else if (Random(100) < 50) {
                        SetAnimation(RES_TYPE_BCA0, 0x19);
                    } else {
                        SetAnimation(RES_TYPE_BCA0, 0x1B);
                    }
                }
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mAngleZ = 0x4000;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 5);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = 1;
                if (Random(10) < 7) {
                    SetState(0x45);
                } else {
                    SetState(0x46);
                }
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0x46:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_021694e8(this);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(7, 0xFF) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, GetMoodAnimB());
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mAngleZ = 0x4000;
                FloorPos pos;
                GetRandomPosInArea(&pos);
                SetTarget(pos.x, pos.z);
                mTimer = Random(100) + 300;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 5);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = 1;
                if (Random(10) < 7) {
                    SetState(0x45);
                } else {
                    SetState(0x46);
                }
            }
            if (func_ov074_02169388(this, 0x3800, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 70) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTarget(pos.x, pos.z);
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0x47:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 7, 0xFF)) {
                mStep++;
                mHeldItems[0]->mAngleZ = 0x4000;
            }
        } else if (mModel.mBlendLeft <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x45);
            }
        }
        mHeldItems[0]->mAngleX = mHeadYawNow;
        func_ov074_02169f78(mHeldItems[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(0), 0x666, 0, 0);
        break;

    case 0x48:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x49 || mPrevState == 0x4D || mPrevState == 0x4E) {
                SetState(0x49);
            } else if (mPrevState == 0x4B || mPrevState == 0x4C) {
                SetState(0x4C);
            } else {
                func_ov074_0216950c(this, 1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x4A);
        }
        break;

    case 0x49:
    case 0x4C:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(1, 0xFF) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169bb4(this, 0);
                SetAnimation(RES_TYPE_BCA0, 0x3D);
                Character_SetFace(this, 8);
                mHeldItems[0]->mAngleY = 0xC000 - mAngle;
                if (mState == 0x4C) {
                    VecFx32 pos;
                    GetMidpoint(mModel.GetJointMatrix(2), mModel.GetJointMatrix(1), &pos);
                    pos.y = 0xCC;
                    mHeldItems[0]->SetPos(pos);
                } else {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 2);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
            }
            if (mState == 0x4C) {
                VecFx32 pos;
                GetMidpoint(mModel.GetJointMatrix(2), mModel.GetJointMatrix(1), &pos);
                pos.y = 0xCC;
                mHeldItems[0]->SetPos(pos);
                mTimer = 1;
            } else {
                mTimer = Random(100) + 300;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0x4C) {
                    SetState(0x49);
                } else {
                    int r = Random(100);
                    if (r < 70) {
                        SetState(0x49);
                    } else if (r < 90) {
                        SetState(0x4D);
                    } else {
                        SetState(0x4E);
                    }
                }
            }
        }
        break;

    case 0x4D:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            SetAnimation(RES_TYPE_BCA0, 0x28);
            func_0208ef00(&mModel, 1, 0);
            Character_SetFace(this, 6);
            AnimModel_FinishAnimations(mHeldItems[0]);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
            Character_ResetSoundPhase(this, 0);
        } else if (mStep == 1) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (Random(0) & 1) {
                Character_SetFace(this, 7);
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 5, 2);
            if (IsAnimationFinished()) {
                mStep++;
                mTimer = 20;
                Character_SetFace(this, 6);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x49);
                Character_SetFace(this, 8);
                gSoundManager.StartSeqArc(1, 0x1F, 6);
            }
        }
        break;

    case 0x4E:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            SetAnimation(RES_TYPE_BCA0, 0x3D);
            func_0208ef00(&mModel, 1, 0);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
        } else if (mStep == 1) {
            if (IsAnimationFinished()) {
                mStep++;
                mTimer = 110;
                Character_SetFace(this, 5);
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x49);
                Character_SetFace(this, 8);
            }
        }
        break;

    case 0x4A:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 1, 0xFF)) {
                mStep++;
                mHeldItems[0]->mAngleY = 0xC000 - mAngle;
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else if (mModel.mBlendLeft <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x4B);
            }
        }
        {
            VecFx32 pos;
            GetMidpoint(mModel.GetJointMatrix(2), mModel.GetJointMatrix(1), &pos);
            pos.y = mModel.GetJointMatrix(2)->m[3][1];
            mHeldItems[0]->SetPos(pos);
        }
        break;

    case 0x4B:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x3C);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 8);
                mHeldItems[0]->mAngleY = 0xC000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (IsAnimationFinished()) {
                SetState(0x4C);
            }
        }
        {
            VecFx32 pos;
            GetMidpoint(mModel.GetJointMatrix(2), mModel.GetJointMatrix(1), &pos);
            pos.y = mModel.GetJointMatrix(2)->m[3][1];
            mHeldItems[0]->SetPos(pos);
        }
        break;

    case 0x4F:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            int next;
            if (Random(100) < 10) {
                next = 0x53;
            } else {
                next = Random(3) + 0x50;
            }
            if (next == mPrevState) {
                SetState(next);
            } else {
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1A);
                }
                mNextState = next;
            }
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_0216950c(this, 1);
        } else if (mStep == 1) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (Random(2) == 0) {
                mTimer = 1;
                break;
            }
            mTimer = Random(30) + 60;
            func_ov074_02169b00(this);
            SetAnimation(RES_TYPE_BCA0, 7);
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendLeft <= 0) {
                SetState(mNextState);
            }
        }
        break;

    case 0x50:
        if (mStep == 0) {
            if (StepAlternateAnimations(0x4E, 0x4F)) {
                SetState(0x4F);
            }
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 0, 2);
        }
        break;

    case 0x51:
        if (mStep == 0) {
            if (StepAlternateAnimations(0x50, 0x51)) {
                SetState(0x4F);
            }
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 5, 2);
        }
        break;

    case 0x52:
        if (mStep == 0) {
            if (StepAlternateAnimations(0x52, 0x52)) {
                SetState(0x4F);
            }
            ModelAnimation *anim = GetCurrentAnimation();
            if (anim != NULL) {
                if (anim->mAnmObj->frame < (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << FX32_SHIFT) >> 1) {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 3, 2);
                } else {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 4, 8);
                }
            }
        }
        break;

    case 0x53:
        if (StepWalkToRandomPos()) {
            SetState(0x4F);
        }
        break;

    case 0x54:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x55 || mPrevState == 0x56) {
                SetState(0x55);
            } else {
                func_ov074_0216950c(this, 1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x57);
        }
        break;

    case 0x55:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                SetAnimation(RES_TYPE_BCA0, 0x53);
                Character_SetFace(this, 8);
                mHeldItems[1]->mAngleZ = 0x8000;
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 140;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 3);
                func_ov074_02169a98(this);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 80 || ClampToRect4E4(NULL)) {
                    SetState(0x55);
                } else {
                    SetState(0x56);
                }
            } else if (mModel.mBlendLeft <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(60) + 140;
                    if (IsAnimationFinished()) {
                        func_0208ef00(&mModel, 0, 0);
                        func_ov074_021691d4(this, 0, 0, 0, 4);
                        Character_SetFace(this, 8);
                        gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 3);
                        func_ov074_02169a98(this);
                    } else if (Random(100) < 20) {
                        func_0208ef00(&mModel, 1, 1);
                        func_ov074_021691d4(this, 0xE800, 0, 0, 4);
                        Character_SetFace(this, 0x12);
                        gSoundManager.StopSeqArc(mSoundHandle, 0);
                    }
                }
            }
        }
        func_ov074_02169634(this);
        break;

    case 0x56:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendLeft <= 0) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x46);
                Character_SetFace(this, 6);
                mHeldItems[1]->mAngleZ = 0x8000;
                mTimer = Random(60) + 330;
                if (Random(0) & 1) {
                    Character_SetFace(this, 7);
                }
                gSoundManager.StartSeqArc(1, 0x1E, 1);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x55);
            } else if (mTimer == 160) {
                Character_SetFace(this, 8);
            } else if (mTimer == 270) {
                Character_SetFace(this, 6);
            }
        }
        func_ov074_02169634(this);
        break;

    case 0x57:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3B, 9, 5)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                mHeldItems[1]->mAngleZ = 0x8000;
            }
        } else if (mModel.mBlendLeft <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x55);
            }
        }
        func_ov074_02169634(this);
        break;

    case 0x58:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x59) {
                SetState(0x59);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RES_TYPE_BCA0, 0x1A);
            }
            func_ov074_0216950c(this, 1);
        } else {
            SetState(0x5A);
        }
        break;

    case 0x59:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 550;
            BOOL wasGroupB = IsAnimOfGroupB(mModel.mAnimId);
            func_ov074_02169bb4(this, 1);
            int r = Random(4);
            if (r == 0) {
                SetAnimation(RES_TYPE_BCA0, 0x44);
            } else if (r == 1) {
                SetAnimation(RES_TYPE_BCA0, 0x45);
            } else if (r == 2) {
                SetAnimation(RES_TYPE_BCA0, 0x46);
            } else {
                SetAnimation(RES_TYPE_BCA0, 0x27);
            }
            if (mModel.mBlendLeft > 0 && wasGroupB) {
                gSoundManager.StartSeqArc(1, 0x1E, 2);
            }
            Character_SetFace(this, 0x12);
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x59);
            }
        }
        break;

    case 0x5A: {
        if (LengthSq(GetFloorPos()) < 0x4000) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x59);
            SetAnimation(RES_TYPE_BCA0, 0x1A);
        } else {
            mTarget.x = 0;
            mTarget.z = 0;
            StepWalkTo(GetTargetFloorPos());
        }
        break;
    }

    case 0x5B:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x5C || mPrevState == 0x5D) {
                if (Random(100) < 60) {
                    SetState(0x5C);
                } else {
                    SetState(0x5D);
                }
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RES_TYPE_BCA0, 0x1A);
            }
            func_ov074_0216950c(this, 1);
        } else {
            SetState(0x5C);
        }
        break;

    case 0x5C:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 500;
            mTimer2 = Random(60) + 120;
            func_ov074_02169b00(this);
            SetAnimation(RES_TYPE_BCA0, 5);
            Character_SetFace(this, 0x10);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x5B);
            }
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mTimer2--;
            if (mTimer2 > 0) {
                break;
            }
            mTimer2 = Random(60) + 140;
            if (Random(100) < 30) {
                Character_SetFace(this, 0x10);
                SetAnimation(RES_TYPE_BCA0, 4);
            } else {
                Character_SetFace(this, 4);
                SetAnimation(RES_TYPE_BCA0, 5);
            }
        }
        break;

    case 0x5D:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            FloorPos pos;
            GetRandomPosInArea(&pos);
            SetTarget(pos.x, pos.z);
            func_ov074_021694e8(this);
            mStep++;
            mTimer = Random(100) + 200;
            mTimer2 = Random(60) + 120;
            func_ov074_02169b00(this);
            SetAnimation(RES_TYPE_BCA0, 0xF);
            Character_SetFace(this, 0x10);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x5B);
            }
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Random(60) + 150;
                if (Random(100) < 40) {
                    Character_SetFace(this, 0x10);
                } else {
                    Character_SetFace(this, 4);
                }
            }
            if (func_ov074_02169388(this, 0x1000, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 70) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTarget(pos.x, pos.z);
            }
        }
        break;

    case 0x5E:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x5F) {
                SetState(0x5F);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RES_TYPE_BCA0, 0x1A);
            }
            func_ov074_0216950c(this, 1);
        } else {
            SetState(0x60);
        }
        break;

    case 0x5F:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 700;
            mTimer2 = 120;
            BOOL wasGroupB = IsAnimOfGroupB(mModel.mAnimId);
            func_ov074_02169bb4(this, 1);
            int r = Random(100);
            if (r < 60) {
                SetAnimation(RES_TYPE_BCA0, 0x27);
            } else if (r < 80) {
                SetAnimation(RES_TYPE_BCA0, 0x44);
            } else {
                SetAnimation(RES_TYPE_BCA0, 0x45);
            }
            if (mModel.mBlendLeft > 0 && wasGroupB) {
                gSoundManager.StartSeqArc(1, 0x1E, 2);
            }
            Character_SetFace(this, 6);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIconOfRecord();
            HeadIcon_SetMode(&mHeadIcon, 3);
        } else {
            if (func_02081024(&mHeadIcon, mHeadIcon.mShown->mFrame) == 8) {
                PlayRandomSoundOfFour();
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x5F);
                break;
            }
            mTimer2--;
            if (mTimer2 > 0) {
                break;
            }
            if (Random(100) < 80) {
                mTimer2 = 200;
                Character_SetFace(this, 6);
            } else {
                mTimer2 = 60;
                Character_SetFace(this, 2);
            }
        }
        break;

    case 0x60: {
        if (LengthSq(GetFloorPos()) < 0x4000) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x5F);
            SetAnimation(RES_TYPE_BCA0, 0x1A);
        } else {
            mTarget.x = 0;
            mTarget.z = 0;
            StepWalkTo(GetTargetFloorPos());
        }
        break;
    }

    case 0x61:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x62 || mPrevState == 0x63) {
                SetState(Random(2) + 0x62);
            } else {
                func_ov074_0216950c(this, 1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x64);
        }
        break;

    case 0x62:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xFF) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 0x21);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetAngles(0, 0x7000, 0x1000);
                mTimer = Random(200) + 800;
                mTimer2 = 60;
                if (mModel.mBlendLeft > 0) {
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
                        Character_SetFace(this, 0xE);
                    } else {
                        Character_SetFace(this, 0x16);
                    }
                }
            }
            if (mFace == 0x14 || mFace == 0x16) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 10);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 7, 2);
            }
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 40) {
                    SetState(0x62);
                } else {
                    SetState(0x63);
                }
                Character_SetFace(this, 8);
            }
        }
        func_ov074_02169f48(mHeldItems[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x63:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_021694e8(this);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xFF) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 0x22);
                Character_SetFace(this, 0x15);
                mHeldItems[0]->SetAngles(0, 0x7800, 0x1000);
                if (mModel.mBlendLeft > 0) {
                    Character_ResetSoundPhase(this, 1);
                }
                FloorPos pos;
                GetRandomPosInArea(&pos);
                SetTarget(pos.x, pos.z);
                mHeadTurnFrames = 5;
                mTimer = Random(200) + 800;
                mTimer2 = 0;
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
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 10);
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 60) {
                    SetState(0x62);
                } else {
                    SetState(0x63);
                }
                Character_SetFace(this, 8);
            }
            if (func_ov074_02169388(this, 0x1800, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 60) {
                    GetRandomCorner(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTarget(pos.x, pos.z);
            }
        }
        func_ov074_02169f48(mHeldItems[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x64:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 0, 0xFF)) {
                mStep++;
                mTimer = 300;
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mAngleY = 0x7000;
                gSoundManager.StartSeqArc(mSoundHandle, 0x1C, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0 && IsAnimationFinished()) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x21);
                mHeldItems[0]->mAngleZ = 0x1000;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x1C, 0) == 0) {
                SetState(0x62);
            }
        }
        func_ov074_02169f48(mHeldItems[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x65:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x66 || mPrevState == 0x67) {
                SetState(0x66);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RES_TYPE_BCA0, 0x1A);
            }
            func_ov074_0216950c(this, 1);
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x67);
        }
        break;

    case 0x66:
    case 0x67:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                if (mState == 0x67) {
                    mTimer = 10;
                } else {
                    mTimer = Random(100) + 300;
                }
                func_ov074_02169b00(this);
                if (mModel.mAnimId != 0x31) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetAnimation(RES_TYPE_BCA0, 0x31);
                Character_SetFace(this, 9);
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendLeft <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    if (mState == 0x67) {
                        mTimer = 0;
                    } else {
                        mTimer = Random(60) + 120;
                        int faceRoll = Random(100);
                        int animRoll = Random(100);
                        if (animRoll < 60) {
                            SetAnimation(RES_TYPE_BCA0, 0x31);
                            if (faceRoll < 28) {
                                Character_SetFace(this, 8);
                            } else {
                                Character_SetFace(this, 9);
                            }
                        } else if (animRoll < 92) {
                            SetAnimation(RES_TYPE_BCA0, GetMoodAnimA());
                            if (faceRoll < 60) {
                                Character_SetFace(this, 8);
                            } else {
                                Character_SetFace(this, GetFace());
                            }
                        } else {
                            SetAnimation(RES_TYPE_BCA0, 0x54);
                            if (faceRoll < 60) {
                                Character_SetFace(this, 6);
                            } else {
                                Character_SetFace(this, 4);
                            }
                        }
                        if (mModel.mBlendLeft > 0) {
                            Character_ResetSoundPhase(this, 0);
                        }
                    }
                }
            }
            func_ov074_021691d4(this, func_ov074_02169280(this, CharacterModel_GetJointPosition(&mModel, 0), mLookTarget->mPos, 0x1800), 0, 0, 2);
        }
        if (mModel.mAnimId == 0x31) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 3, 10);
        } else if (mModel.mAnimId == 0x54) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x20, 3, 2);
        }
        if (mState == 0x67) {
            Character_SetWalkTarget(this, mLookTarget->mPos);
            if (Character_TurnToWalkTarget(this, 0x300, 0)) {
                SetState(0x66);
            }
        } else {
            Character_SetAngle(this, Angle(GetOffsetTo(mLookTarget->GetFloorPos())));
        }
        break;
    }
    PlaceHeadIcon();
}

// The states 0x68 to 0x6f: one more activity of the same kind (hub 0x68), with held item 0 on the floor where
// the character stands, scaled by the model's factors.
void RoomCharacter::UpdateSoloStates68To6F() {
    switch (mState) {
    case 0x68:
        if (mStep == 0) {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x69 || mPrevState == 0x6B || mPrevState == 0x6F || mPrevState == 0x6A ||
                mPrevState == 0x6E) {
                SetState(0x69);
            } else {
                func_ov074_0216950c(this, 1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendLeft > 0) {
                break;
            }
            SetState(0x6C);
        }
        break;

    case 0x69:
    case 0x6A:
    case 0x6B:
        if (mStep == 0) {
            if (StepChangeHeldItems(10, 0xFF) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169bb4(this, 1);
                SetAnimation(RES_TYPE_BCA0, 0x1F);
                if (mState == 0x6B) {
                    mTimer = Random(500) + 100;
                    mTimer2 = Random(100) + 200;
                    Character_SetFace(this, 0x12);
                    HeadIcon_SetMode(&mHeadIcon, 1);
                } else {
                    mTimer = Random(400) + 2500;
                    mTimer2 = Random(100) + 800;
                    Character_SetFace(this, 6);
                    ShowHeadIconOfRecord();
                    HeadIcon_SetMode(&mHeadIcon, 3);
                }
                if (mState == 0x6A) {
                    mHeadYawNow = Random(0x4000) - 0x2000;
                } else {
                    func_ov074_021691d4(this, 0, 0, 0, 5);
                }
                mHeldItems[0]->SetAngles(0, 0x4000 - mAngle, 0);
                mHeldItems[0]->SetPos(mModel.mPos.x, 0, mModel.mPos.z);
            }
            mHeldItems[0]->SetScale(func_0208fc34(&mModel));
        } else {
            if (mState != 0x6B && func_02081024(&mHeadIcon, mHeadIcon.mShown->mFrame) == 8) {
                PlayRandomSoundOfFour();
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x69);
                break;
            }
            mTimer2--;
            if (mTimer2 > 0) {
                break;
            }
            mTimer2 = Random(300) + 1100;
            func_ov074_021691d4(this, 0, Random(0x4000) - 0x2000, Random(0x1200) - 0x900, 5);
        }
        break;

    case 0x6C: {
        if (LengthSq(GetFloorPos()) < 0x4000) {
            SetState(0x6D);
        } else {
            mTarget.x = 0;
            mTarget.z = 0;
            StepWalkTo(GetTargetFloorPos());
        }
        break;
    }

    case 0x6D:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 10, 0xFF)) {
                mStep++;
                mHeldItems[0]->mAngleY = 0x4000 - mAngle;
                mHeldItems[0]->SetScale(func_0208fc34(&mModel));
                gSoundManager.StartSeqArc(1, 0x1C, 6);
            }
        } else if (mModel.mBlendLeft <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x6E);
            }
        }
        func_ov074_02169598(this, 0);
        break;

    case 0x6E:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x3C);
                func_0208ef28(&mModel, 1, 0, 0);
                mHeldItems[0]->mAngleY = 0x4000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1C, 7, 4);
            if (IsAnimationFinished()) {
                SetState(0x6F);
            }
        }
        func_ov074_02169598(this, 0);
        break;

    case 0x6F:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x1A);
                ShowHeadIcon();
            }
        } else if (mModel.mBlendLeft <= 0) {
            SetState(0x6B);
        }
        break;
    }
    PlaceHeadIcon();
}
