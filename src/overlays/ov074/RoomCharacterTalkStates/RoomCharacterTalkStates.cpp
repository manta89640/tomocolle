#include <nitro.h>
#include <nnsys.h>

#include "RoomCharacter.h"

// Functions of the main program that have no name yet.
extern "C" {
void Character_SetFace(RoomCharacter *self, int face);                       // Sets the face (expression) and restarts its animation
void func_02090a58(RoomCharacter *self, const char *phoneme, int face);  // Lip sync: opens the mouth on a vowel
BOOL func_02090c78(RoomCharacter *self, u16 *angle, int limit);          // Clamps an angle to +-limit
BOOL func_02091b80(RoomCharacter *self, fx32 x, fx32 z, int range);
BOOL func_02091c80(RoomCharacter *self, fx32 x, fx32 z);
void Character_SetWalkTarget(RoomCharacter *self, VecFx32 target);                 // Sets the walk target
void Character_ResetSoundPhase(RoomCharacter *self, int slot);                       // Resets the sound phase of a slot
void func_02091fa0(RoomCharacter *self);                                 // Resets both sound phases
int func_02091fbc(RoomCharacter *self, int player, int seqArcNo, int index);
int Character_PlaySoundAtAnimPhase(RoomCharacter *self, int slot, int player, int seqArcNo, int index, int phaseMask);
BOOL Character_TurnToTargetAngle(RoomCharacter *self, int speed, int accel);           // Turns towards mTargetAngle
void Character_MoveAlongAngle(RoomCharacter *self);                                 // Moves by mSpeed along mMoveAngle
void func_0208ef28(CharacterModel *model, BOOL a, fx32 frame, BOOL finished);
void func_0208ef00(CharacterModel *model, BOOL a, BOOL finished);
ModelAnim *AnimModel_FindAnimation(CharacterModel *model, u32 resType, int id);
void AnimModel_FinishAnimations(HeldItem *item);
void func_02080fc4(HeadIcon *mark);
void func_020810c8(HeadIcon *mark);
void HeadIcon_SetMode(HeadIcon *mark, int mode);
void func_0209d898(RoomCharacter *self, TtsPlayer *player);              // Applies the character's voice to the engine
int func_02098e88(RoomCharacter *self, int max);

extern MATHRandContext16 gRandom;
extern const int data_020fba28[];
extern const s16 data_020fb870[][5];
extern const s16 data_020fb898[][5];
}

// Functions of the overlay that belong to other groups.
extern "C" {
BOOL func_ov074_02167840(RoomCharacter *self);
BOOL func_ov074_02167930(RoomCharacter *self, FloorPos pos);
BOOL func_ov074_02167a58(RoomCharacter *self, int anim);
BOOL func_ov074_02167f78(RoomCharacter *self);
BOOL func_ov074_02168110(RoomCharacter *self);
BOOL func_ov074_02168320(RoomCharacter *self, int a, int b, int c);
BOOL func_ov074_021684ac(RoomCharacter *self, int a, int b);
int func_ov074_02168710(RoomCharacter *self, int index);
int func_ov074_02168720(RoomCharacter *self);
BOOL func_ov074_021687dc(RoomCharacter *self);
BOOL func_ov074_02168804(RoomCharacter *self, int anim);
BOOL func_ov074_02168900(RoomCharacter *self, int anim);
void func_ov074_0216898c(RoomCharacter *self, FloorPos *out);
void func_ov074_02168c28(RoomCharacter *self, FloorPos *out);
void func_ov074_02168d24(RoomCharacter *self, FloorPos pos, FloorPos *out);
u16 func_ov074_02168dc4(RoomCharacter *self, u16 angle);
u16 func_ov074_021691b0(RoomCharacter *self, u16 angle, int limit);
void func_ov074_021691d4(RoomCharacter *self, u16 pitch, u16 yaw, u16 roll, u8 frames);
u16 func_ov074_021691fc(RoomCharacter *self, FloorPos pos, int limit);
u16 func_ov074_02169318(RoomCharacter *self, int a, int limit);
BOOL func_ov074_02169388(RoomCharacter *self, int a, int b, int c, int d, int e);
void func_ov074_021694e8(RoomCharacter *self);
void func_ov074_0216950c(RoomCharacter *self, int a);
void func_ov074_02169598(RoomCharacter *self, int a);
void func_ov074_02169708(RoomCharacter *self);
void func_ov074_02169b00(RoomCharacter *self);
void func_ov074_02169b68(RoomCharacter *self);
void func_ov074_02169c04(RoomCharacter *self);
void func_ov074_02169eac(HeldItem *item, int a);
void func_ov074_02169f48(HeldItem *item, JointWork60 *a, JointWork48 *b);

extern const FloorPos data_ov074_0216d47c;
extern FloorPos data_ov074_0216d724;
}

// Only for state 24 of UpdateTalkStates, see there.
struct RoomCharacterAlias : RoomCharacter {};

#define ANIM 0x30414342 // 'BCA0'

static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

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
            func_ov074_02169b00(this);
            SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            {
                u16 yaw;
                u16 pitch;
                if (mState == 2) {
                    yaw = (0x20 - Rand(0x40)) << 8;
                    if (Rand(10) < 4) {
                        pitch = (0x10 - Rand(0x20)) << 8;
                    } else {
                        pitch = 0;
                    }
                } else {
                    pitch = 0;
                    yaw = SignedAngle(mHeadYaw) >> 1;
                    func_02090c78(this, &yaw, 0x3000);
                }
                func_ov074_021691d4(this, pitch, yaw, 0, 5);
            }
        } else if (mStep == 1 && mState == 4) {
            if (!func_02091b80(this, mPartner->mModel.mPos.x, mPartner->mModel.mPos.z, 0x3000) &&
                mPartner->mState != 8 && mPartner->mState != 10 && mPartner->mState != 0x18 &&
                mPartner->mState != 0x19) {
                break;
            }
            {
                u16 yaw = func_ov074_021691fc(this, FloorPos(mPartner->mModel.mPos.x, mPartner->mModel.mPos.z), 0x3000);
                u16 pitch;
                if (mMood != 3 && Rand(100) < 50) {
                    pitch = func_ov074_02169318(this, 0, 0x1800);
                } else {
                    pitch = 0;
                }
                func_ov074_021691d4(this, pitch, yaw, 0, 5);
                if (mPartner->mState == 0x18) {
                    if (mFace != 0x1a) {
                        Character_SetFace(this, 0x1a);
                    }
                } else {
                    int face = mFace;
                    if (face != func_ov074_02168720(this)) {
                        Character_SetFace(this, func_ov074_02168720(this));
                    }
                }
            }
        }
        break;

    case 5:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                if (func_ov074_021687dc(mPartner)) {
                    mPartner->SetState(0x14);
                } else {
                    mPartner->SetState(6);
                }
            }
        } else {
            func_ov074_02167f78(this);
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
            func_ov074_02168110(this);
        } else {
            func_ov074_02168110(this);
        }
        break;

    case 7:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mSavedPhrase[mMood] && mTtsPlayer->IsEngineIdle()) {
                mStep++;
                func_0209d898(this, mTtsPlayer);
                mTtsPlayer->Speak(mSpeechText[mMood], 0);
            } else {
                SetState(0);
            }
        } else {
            if (mTtsPlayer->IsEngineIdle()) {
                Character_SetFace(this, func_ov074_02168720(this));
                SetState(0);
            } else {
                func_02090a58(this, mTtsPlayer->GetPhonemeName(), func_ov074_02168720(this));
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
            func_ov074_02169b00(this);
            func_ov074_021694e8(this);
            SetAnimation(ANIM, data_020fb898[data_020fba28[mAnimRowIndex]][mMood]);
            Character_SetFace(this, func_ov074_02168720(this));
            {
                VecFx32 target = mWalkTarget;
                mSavedWalkTarget.x = target.x;
                mSavedWalkTarget.y = target.y;
                mSavedWalkTarget.z = target.z;
            }
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            int speed;
            if (mState == 9) {
                speed = 0x8000;
                func_ov074_021691d4(
                    this, 0, func_ov074_021691fc(this, FloorPos(mPartner->mModel.mPos.x, mPartner->mModel.mPos.z), 0x3000),
                    0, 4);
            } else {
                speed = 0x3800;
            }
            if (func_ov074_02169388(this, speed, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        break;

    case 10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_021694e8(this);
                SetAnimation(ANIM, data_020fb898[data_020fba28[mAnimRowIndex]][mMood]);
                Character_SetFace(this, func_ov074_02168720(this));
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else {
            if (func_ov074_02169388(this, 0x8000, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        Character_SetWalkTarget(this, mSavedWalkTarget);
        break;

    case 11:
        if (mStep == 0) {
            mStep++;
            {
                u16 turn = Rand(0x2000) + 0x1800;
                u16 target;
                if (Rand(100) < 50) {
                    target = mAngle - turn;
                } else {
                    target = mAngle + turn;
                }
                mTargetAngle = target;
            }
            func_ov074_02167a58(this, data_020fb898[data_020fba28[mAnimRowIndex]][mMood]);
        } else if (mStep == 1) {
            if (func_ov074_02167a58(this, data_020fb898[data_020fba28[mAnimRowIndex]][mMood])) {
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
            func_ov074_02169b68(this);
            SetAnimation(ANIM, 0x17);
            Character_SetFace(this, 6);
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
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
            if (func_ov074_02168804(this, mModel.mAnimId)) {
                func_ov074_02169b68(this);
                SetAnimation(ANIM, 0x1a);
            }
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetAnimation(ANIM, 0x28);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 6);
            Character_ResetSoundPhase(this, 0);
        } else if (mStep == 2) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (Rand(0) & 1) {
                Character_SetFace(this, 7);
            }
        } else if (mStep == 3) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 5, 2);
            if (AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
                if (Rand(10) < 8) {
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
            func_ov074_02169b00(this);
            SetAnimation(ANIM, 0x29);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 1);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_02080fc4(&mHeadIcon);
            mHeadIcon.mKind = 0xf;
            func_020810c8(&mHeadIcon);
            HeadIcon_SetMode(&mHeadIcon, 3);
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
                ModelAnim *anim = AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId);
                if (anim->mFrame->mFrame >= ((anim->mFrame->mResource->mFrameCount << 12) * 5) >> 3) {
                    mTimer++;
                    if (Rand(10) < 8) {
                        Character_SetFace(this, 6);
                    } else {
                        Character_SetFace(this, 4);
                    }
                }
            }
            if (AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
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
            func_ov074_02169b68(this);
            if (func_ov074_02168900(this, mModel.mAnimId) && (Rand(0) & 1)) {
                SetAnimation(ANIM, 0x1b);
            } else {
                SetAnimation(ANIM, 0x19);
            }
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            {
                u16 yaw;
                u16 pitch;
                if (mState == 0x11) {
                    yaw = (0x20 - Rand(0x40)) << 8;
                    pitch = (0 - Rand(0x10)) << 8;
                } else {
                    pitch = 0;
                    yaw = 0;
                }
                func_ov074_021691d4(this, pitch, yaw, 0, 5);
            }
        } else if (mState == 0x12) {
            if (!func_02091c80(this, mPartner->mModel.mPos.x, mPartner->mModel.mPos.z) && mPartner->mState != 8 &&
                mPartner->mState != 10 && mPartner->mState != 0x18 && mPartner->mState != 0x19) {
                break;
            }
            {
                u16 yaw = func_ov074_021691fc(this, FloorPos(mPartner->mModel.mPos.x, mPartner->mModel.mPos.z), 0x3000);
                u16 pitch = 0;
                if (Rand(10) < 5) {
                    pitch = func_ov074_02169318(this, 0, 0x1800);
                }
                func_ov074_021691d4(this, pitch, yaw, 0, 5);
                if (mPartner->mState == 0x18) {
                    if (mFace != 0x11) {
                        Character_SetFace(this, 0x11);
                    }
                } else {
                    int face = mFace;
                    if (face != func_ov074_02168720(this)) {
                        Character_SetFace(this, func_ov074_02168720(this));
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
            func_ov074_02169b68(this);
            if (Rand(10) < 4) {
                if (func_ov074_02168900(this, mModel.mAnimId) && (Rand(0) & 1)) {
                    SetAnimation(ANIM, 0x1b);
                } else {
                    SetAnimation(ANIM, 0x19);
                }
            } else {
                SetAnimation(ANIM, 0x1d);
            }
            Character_SetFace(this, 0x15);
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (func_ov074_021687dc(mPartner)) {
                mPartner->SetState(0x14);
            } else {
                mPartner->SetState(6);
            }
        } else {
            u16 yaw = func_ov074_021691fc(this, FloorPos(mPartner->mModel.mPos.x, mPartner->mModel.mPos.z), 0x3800);
            func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), yaw, 0, 3);
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
            func_ov074_02169b68(this);
            if (Rand(10) < 4) {
                if (Rand(0) & 1) {
                    SetAnimation(ANIM, 0x19);
                } else {
                    SetAnimation(ANIM, 0x1b);
                }
            } else {
                SetAnimation(ANIM, 0x1c);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            {
                u16 yaw = func_ov074_021691fc(this, FloorPos(mPartner->mModel.mPos.x, mPartner->mModel.mPos.z), 0x3800);
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), yaw, 0, 4);
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
            func_ov074_02169b68(this);
            SetAnimation(ANIM, 0x18);
            Character_SetFace(this, 6);
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        }
        break;

    case 22:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            if (data_020fba28[mAnimRowIndex] == 0) {
                SetAnimation(ANIM, 0x23);
            } else if (data_020fba28[mAnimRowIndex] == 1) {
                SetAnimation(ANIM, 0x23);
            } else {
                SetAnimation(ANIM, 0x24);
            }
            Character_SetFace(this, 6);
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
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
            func_ov074_02169b00(this);
            SetAnimation(ANIM, 0x2b);
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
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
            func_ov074_02169b00(this);
            // Both arms start the same animation, and the original has both calls. The compiler folds two arms that
            // are the same in the syntax tree into one, so they differed in the source in some way that leaves no
            // trace in the code. What that was is not known; the cast stands in for it.
            if (data_020fba28[mAnimRowIndex] == 0) {
                SetAnimation(ANIM, 0x26);
            } else {
                ((RoomCharacterAlias *)this)->SetAnimation(ANIM, 0x26);
            }
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            if (Rand(2) == 0) {
                Character_SetFace(this, 6);
            } else {
                Character_SetFace(this, 4);
            }
            func_ov074_0216950c(this, 1);
            ShowHeadIcon();
            mTimer = 0x46;
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnim *anim = AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId);
            if (anim->mFrame->mFrame >= (anim->mFrame->mResource->mFrameCount << 12) >> 1) {
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
                SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
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
            func_ov074_02169b00(this);
            SetAnimation(ANIM, 0x25);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 4);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnim *anim = AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId);
            if (anim->mFrame->mFrame >= (anim->mFrame->mResource->mFrameCount << 12) >> 2) {
                mStep++;
                gSoundManager.StartSeqArc(1, 0x22, 6);
            }
        } else if (mStep == 2) {
            ModelAnim *anim = AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId);
            if (anim->mFrame->mFrame >= (anim->mFrame->mResource->mFrameCount << 12) >> 1) {
                mStep++;
                Character_SetFace(this, 7);
                func_02080fc4(&mHeadIcon);
                mHeadIcon.mKind = 0xb;
                func_020810c8(&mHeadIcon);
                HeadIcon_SetMode(&mHeadIcon, 3);
                gSoundManager.StartSeqArc(1, 0x22, 7);
            }
        } else if (mStep == 3) {
            if (AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
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
            func_ov074_02169b00(this);
            if (data_020fba28[mAnimRowIndex] == 0) {
                SetAnimation(ANIM, 0x15);
            } else if (data_020fba28[mAnimRowIndex] == 1) {
                SetAnimation(ANIM, 0x15);
            } else {
                SetAnimation(ANIM, 0x16);
            }
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 4);
            func_ov074_0216950c(this, 1);
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnim *anim = AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId);
            if (anim->mFrame->mFrame >= (anim->mFrame->mResource->mFrameCount << 12) >> 1) {
                mStep++;
                Character_SetFace(this, 7);
                func_02080fc4(&mHeadIcon);
                mHeadIcon.mKind = 8;
                func_020810c8(&mHeadIcon);
                HeadIcon_SetMode(&mHeadIcon, 3);
                gSoundManager.StartSeqArc(1, 0x20, 2);
            }
        } else if (mStep == 2) {
            if (AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x14;
            }
        } else if (mStep == 3) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(ANIM, data_020fb870[data_020fba28[mAnimRowIndex]][mMood]);
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
                func_ov074_02169b00(this);
                SetAnimation(ANIM, 0x47);
                Character_SetFace(this, func_ov074_02168720(this));
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                func_ov074_021691d4(this, 0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                Character_SetFace(this, 6);
                mTimer = Rand(0x32) + 0x50;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mTimer = Rand(0x1e) + 0x28;
                if (Rand(0) & 1) {
                    Character_SetFace(this, 6);
                } else {
                    Character_SetFace(this, func_ov074_02168720(this));
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
