#include <nitro.h>
#include <nnsys.h>

#include "RoomCharacter.h"

// Functions of the main program that have no name yet.
extern "C" {
void Character_SetFace(RoomCharacter *self, int face);                 // Sets the face (expression) and restarts it
void Character_ResetSoundPhase(RoomCharacter *self, int slot);                 // Resets the sound phase of a slot
int func_02091fbc(RoomCharacter *self, int player, int seqArcNo, int index);
int Character_PlaySoundAtAnimPhase(RoomCharacter *self, int slot, int player, int seqArcNo, int index, int phaseMask);
void func_0209171c(RoomCharacter *self, FloorPos pos);             // Turns towards a point
void func_02091598(RoomCharacter *self, FloorPos pos);             // Sets the walk target away from a point
BOOL Character_TurnToTargetAngle(RoomCharacter *self, int speed, int accel);     // Turns towards mTargetAngle
ModelAnim *AnimModel_FindAnimation(CharacterModel *model, u32 resType, int id);
void func_0208ef00(CharacterModel *model, BOOL playOnce, BOOL finished);
VecFx32 CharacterModel_GetJointPosition(CharacterModel *model, int joint);           // Position of a joint
void HeadIcon_SetMode(HeadIcon *icon, int mode);
void AnimModel_FinishAnimations(HeldItem *item);

extern MATHRandContext16 gRandom;
extern const int data_020fba28[];
extern const s16 data_020fb870[][5];
extern const s16 data_020fb898[][5];
}

// Functions of the overlay that belong to other groups.
extern "C" {
void func_ov074_0215232c(RoomCharacter *self);
void func_ov074_02152370(RoomCharacter *self, int state);
void func_ov074_02152418(RoomCharacter *self, int state, int partnerState);
void func_ov074_02152450(RoomCharacter *self, int event);
void func_ov074_02152464(RoomCharacter *self);
BOOL func_ov074_02167930(RoomCharacter *self, FloorPos pos);
BOOL func_ov074_02167a58(RoomCharacter *self, int anim);
BOOL func_ov074_02167f78(RoomCharacter *self);
BOOL func_ov074_02168110(RoomCharacter *self);
void func_ov074_021682d0(RoomCharacter *self, int anim);
BOOL func_ov074_02168320(RoomCharacter *self, int a, int b, int c);
BOOL func_ov074_021684ac(RoomCharacter *self, int a, int b);
int func_ov074_02168720(RoomCharacter *self);
void func_ov074_02168774(RoomCharacter *self, u32 resType, int id);
BOOL func_ov074_02168804(RoomCharacter *self, int anim);
void func_ov074_0216898c(RoomCharacter *self, FloorPos *out);
void func_ov074_02168be0(RoomCharacter *self, FloorPos *out);
void func_ov074_021691d4(RoomCharacter *self, u16 pitch, u16 yaw, u16 roll, u8 frames);
u16 func_ov074_021691fc(RoomCharacter *self, FloorPos pos, int limit);
u16 func_ov074_02169280(RoomCharacter *self, VecFx32 from, VecFx32 to, int limit);
u16 func_ov074_02169318(RoomCharacter *self, int joint, int limit);
BOOL func_ov074_02169388(RoomCharacter *self, int a, int b, int c, int d, int e);
BOOL func_ov074_0216943c(RoomCharacter *self, int a, int b, int c);
void func_ov074_021694e8(RoomCharacter *self);
void func_ov074_0216950c(RoomCharacter *self, int a);
void func_ov074_021696b8(RoomCharacter *self);
void func_ov074_02169708(RoomCharacter *self);
BOOL func_ov074_0216973c(RoomCharacter *self, int anim);
void func_ov074_02169b00(RoomCharacter *self);
void func_ov074_02169b68(RoomCharacter *self);
void func_ov074_02169d90(RoomCharacter *self);
void func_ov074_02169f48(HeldItem *item, MtxFx43 *joint, MtxFx33 *rotation);
void func_ov074_02169f78(HeldItem *item, MtxFx43 *joint, MtxFx33 *rotation, fx32 x, fx32 y, fx32 z);

extern FloorPos data_ov074_0216d404[]; // Places in the room; not const: only then is it loaded as late as the original does
}

#define ANIM 0x30414342 // 'BCA0'

static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// States 0xb3 to 0xc9, two characters and the thing in the first hand (mHeldItem[0], which follows joint 2).
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
                if (Rand(100) < 70) {
                    func_ov074_02152418(this, 0xb4, 0xb4);
                } else {
                    func_ov074_02152418(this, 0xb5, 0xb5);
                }
            } else if (mPrevState == 0xb9 || mPrevState == 0xba || mPrevState == 0xbb) {
                func_ov074_02152418(this, 0xb9, 0xb9);
            } else {
                func_ov074_0216950c(this, 1);
                func_ov074_0216950c(mPartner, 1);
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    func_ov074_02168774(this, ANIM, 0x1a);
                }
                if (func_ov074_02168804(mPartner, mPartner->mModel.mAnimId)) {
                    func_ov074_02168774(mPartner, ANIM, 0x1a);
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
            func_ov074_02152418(this, 0xb8, 0xb8);
        }
        break;

    case 0xb4:
        if (mStep == 0) {
            mStep++;
            func_ov074_02152464(this);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_021684ac(this, 7, 0xff);
        } else {
            if (func_ov074_021684ac(this, 7, 0xff) && mModel.mBlendFrames <= 0) {
                func_ov074_02152370(this, 0xb6);
                Character_SetFace(this, 8);
                mHeldItem[0]->mRotZ = 0x4000;
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0xb6:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mPartner->mState == 0xb6 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mStep++;
                    func_ov074_02169b00(this);
                    mTimer = Rand(200) + 550;
                    int r = Rand(100);
                    if (r < 55) {
                        func_ov074_02168774(this, ANIM, 9);
                    } else if (r < 70) {
                        func_ov074_02168774(this, ANIM, 2);
                    } else if (r < 83) {
                        mTimer >>= 2;
                        func_ov074_02168774(this, ANIM, 7);
                    } else if (r < 92) {
                        mTimer >>= 1;
                        func_ov074_02168774(this, ANIM, 0x58);
                    } else if (r < 96) {
                        mTimer >>= 2;
                        func_ov074_02168774(this, ANIM, 0x4b);
                    } else {
                        mTimer >>= 3;
                        func_ov074_02168774(this, ANIM, 0x2a);
                        if (mModel.mBlendFrames > 0) {
                            Character_ResetSoundPhase(this, 0);
                            Character_ResetSoundPhase(mPartner, 0);
                        }
                    }
                    func_ov074_02168774(mPartner, ANIM, mModel.mAnimId);
                    if (Rand(100) < 50) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 10);
                    }
                    Character_SetFace(mPartner, mFace);
                    u16 yaw;
                    if (Rand(100) < 25) {
                        yaw = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2000);
                    } else {
                        yaw = 0;
                    }
                    func_ov074_021691d4(this, 0, yaw, 0, 4);
                    gSoundManager.StartSeqArcLimited(mSoundHandle, 0x1c, 5, 1);
                }
            } else {
                mStep++;
                u16 yaw;
                if (Rand(100) < 25) {
                    yaw = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2000);
                } else {
                    yaw = 0;
                }
                func_ov074_021691d4(this, 0, yaw, 0, 4);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mKeepSound = TRUE;
                    if (Rand(100) < 90) {
                        func_ov074_02152418(this, 0xb6, 0xb6);
                    } else {
                        func_ov074_02152418(this, 0xb7, 0xb7);
                    }
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0xb5:
        if (mStep == 0) {
            mStep++;
            func_ov074_02152464(this);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_021684ac(this, 7, 0xff);
        } else {
            if (func_ov074_021684ac(this, 7, 0xff) && mModel.mBlendFrames <= 0) {
                func_ov074_02152370(this, 0xb7);
                Character_SetFace(this, 8);
                mHeldItem[0]->mRotZ = 0x4000;
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0xb7:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mPartner->mState == 0xb7 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mStep++;
                    func_ov074_02169b68(this);
                    mTimer = Rand(200) + 450;
                    int r = Rand(100);
                    if (r < 50) {
                        func_ov074_02168774(this, ANIM, 0x19);
                    } else if (r < 70) {
                        func_ov074_02168774(this, ANIM, 0x1b);
                    } else if (r < 85) {
                        mTimer >>= 1;
                        func_ov074_02168774(this, ANIM, 0x1c);
                    } else {
                        mTimer >>= 1;
                        func_ov074_02168774(this, ANIM, 0x33);
                    }
                    func_ov074_02168774(mPartner, ANIM, mModel.mAnimId);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                        Character_ResetSoundPhase(mPartner, 0);
                    }
                    if (Rand(100) < 50) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 10);
                    }
                    Character_SetFace(mPartner, mFace);
                    u16 yaw;
                    if (Rand(100) < 25) {
                        yaw = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2000);
                    } else {
                        yaw = 0;
                    }
                    func_ov074_021691d4(this, 0, yaw, 0, 4);
                    gSoundManager.StartSeqArcLimited(mSoundHandle, 0x1c, 5, 1);
                }
            } else {
                mStep++;
                u16 yaw;
                if (Rand(100) < 25) {
                    yaw = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2000);
                } else {
                    yaw = 0;
                }
                func_ov074_021691d4(this, 0, yaw, 0, 4);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mKeepSound = TRUE;
                    if (Rand(100) < 80) {
                        func_ov074_02152418(this, 0xb7, 0xb7);
                    } else {
                        func_ov074_02152418(this, 0xb6, 0xb6);
                    }
                }
            }
            if (mModel.mBlendFrames <= 0 && mModel.mAnimId == 0x33) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 8, 1);
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0xb8:
        func_ov074_02167930(this, mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            func_ov074_02152418(this, 0xba, 0xbb);
        }
        break;

    case 0xb9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            func_ov074_021694e8(this);
            func_ov074_02168774(this, ANIM, data_020fb898[data_020fba28[mPersonality]][mMood]);
            SetWalkTarget(mPartner->GetFloorPos());
            func_ov074_02152464(this);
            func_ov074_0216950c(this, 1);
        } else {
            if (func_ov074_0216943c(this, 0x100, 0, 0x3800)) {
                func_ov074_021691d4(this, 0, 0, 0, 4);
                func_ov074_02152370(this, 0xbe);
            }
        }
        break;

    case 0xba:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Rand(60) + 70;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                func_ov074_02152418(this, 0xb9, 0xb9);
            }
        }
        break;

    case 0xbb:
        func_ov074_02168110(this);
        break;

    case 0xbc:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_02168774(this, ANIM, 7);
                func_ov074_02152464(this);
                func_ov074_021691d4(this, 0, 0, 0, 5);
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                func_ov074_02152370(this, 0xbd);
            }
        }
        func_ov074_021696b8(this);
        break;

    case 0xbd:
        if (mSlot == 0 && mPartner->mState == 0xbd) {
            func_ov074_02152418(this, 0xb4, 0xb4);
        }
        func_ov074_021696b8(this);
        break;

    case 0xbe:
        if (mStep == 0) {
            if (func_ov074_02168320(this, 0x20, 7, 0xff)) {
                mStep++;
                mHeldItem[0]->mRotZ = 0x4000;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                if (AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                    func_ov074_02152370(this, 0xbc);
                }
            }
        }
        mHeldItem[0]->mRotX = mUnknownAngleAt25D6;
        func_ov074_02169f78(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(0), 0x666, 0, 0);
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
                if (Rand(100) < 50) {
                    func_ov074_02152418(this, 0xc0, 0xc2);
                } else {
                    func_ov074_02152418(this, 0xc1, 0xc2);
                }
            } else {
                func_ov074_0216950c(this, 1);
                func_ov074_0216950c(mPartner, 1);
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    func_ov074_02168774(this, ANIM, 0x1a);
                }
                if (func_ov074_02168804(mPartner, mPartner->mModel.mAnimId)) {
                    func_ov074_02168774(mPartner, ANIM, 0x1a);
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
            func_ov074_02152418(this, 0xc4, 0xc4);
        }
        break;

    case 0xc0:
        if (mStep == 0) {
            mStep++;
            func_ov074_02152464(this);
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (func_ov074_021684ac(this, 0, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_02168774(this, ANIM, 0x21);
                Character_SetFace(this, 8);
                HeldItem *item = mHeldItem[0];
                item->mRotX = 0;
                item->mRotY = 0x7000;
                item->mRotZ = 0x1000;
                mTimer = Rand(200) + 700;
                mTimer2 = 60;
                mTimer3 = 100;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Rand(100) < 90) {
                    if (Rand(2) == 0) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 0x14);
                    }
                } else {
                    if (Rand(2) == 0) {
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
                mTimer3 = Rand(100) + 200;
                u16 yaw;
                if (Rand(100) < 30) {
                    yaw = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x1800);
                } else {
                    yaw = 0;
                }
                func_ov074_021691d4(this, 0, yaw, 0, 5);
            }
            mTimer--;
            if (mTimer <= 0) {
                if (Rand(100) < 40) {
                    func_ov074_02152370(this, 0xc0);
                } else {
                    func_ov074_02152370(this, 0xc1);
                }
                Character_SetFace(this, 8);
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xc1:
        if (mStep == 0) {
            mStep++;
            func_ov074_02152464(this);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_021694e8(this);
        } else if (mStep == 1) {
            if (func_ov074_021684ac(this, 0, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_02168774(this, ANIM, 0x22);
                Character_SetFace(this, 0x15);
                HeldItem *item = mHeldItem[0];
                item->mRotX = 0;
                item->mRotY = 0x7800;
                item->mRotZ = 0x1000;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 1);
                }
                FloorPos pos;
                func_ov074_0216898c(this, &pos);
                VEC_Set(&mWalkTarget, pos.x, 0, pos.z);
                mTimer = Rand(200) + 700;
                mTimer2 = 0;
                mCounter = 0;
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Rand(100) < 70) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, 0x16);
                }
            }
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1a, 3, 10);
            mTimer--;
            if (mTimer <= 0) {
                if (Rand(100) < 60) {
                    func_ov074_02152370(this, 0xc0);
                } else {
                    func_ov074_02152370(this, 0xc1);
                }
                Character_SetFace(this, 8);
            } else {
                if (IsWithinReach(mPartner->GetFloorPos(), 0x2666, 0x2666)) {
                    func_0209171c(this, mPartner->GetFloorPos());
                }
                if (func_ov074_02169388(this, 0x1800, 0, 0x199, 0x22, 6) || ++mCounter > 200) {
                    mCounter = 0;
                    FloorPos pos;
                    if (Rand(100) < 50) {
                        func_ov074_02168be0(this, &pos);
                    } else {
                        func_ov074_0216898c(this, &pos);
                    }
                    VEC_Set(&mWalkTarget, pos.x, 0, pos.z);
                }
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xc2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                mTimer = Rand(100) + 400;
                int r = Rand(100);
                if (r < 70) {
                    func_ov074_02168774(this, ANIM, 0x19);
                    Character_SetFace(this, func_ov074_02168720(this));
                } else if (r < 90) {
                    func_ov074_02168774(this, ANIM, 0x1b);
                    Character_SetFace(this, func_ov074_02168720(this));
                } else {
                    func_ov074_02168774(this, ANIM, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                func_ov074_0216950c(this, 1);
                func_ov074_02152464(this);
                mCounter = 0;
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int r = Rand(100);
                if (r < 55) {
                    if (mModel.mAnimId != 0x17) {
                        mCounter = 1;
                    } else {
                        func_ov074_021691d4(this, 0, 0, 0, 5);
                    }
                } else if (r < 90) {
                    if (mModel.mAnimId != 0x17) {
                        u16 yaw = (0x20 - Rand(0x40)) << 8;
                        u16 pitch = (0x10 - Rand(0x20)) << 8;
                        func_ov074_021691d4(this, pitch, yaw, 0, 5);
                    } else {
                        func_ov074_021691d4(this, 0, 0, 0, 5);
                    }
                } else {
                    func_ov074_021691d4(this, 0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                func_ov074_02152370(this, 0xc2);
            } else if (mCounter == 1) {
                u16 yaw = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000);
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), yaw, 0, 4);
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;

    case 0xc3:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            func_ov074_02169b00(this);
            func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            func_ov074_02152464(this);
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
                func_ov074_02152418(this, 0xc9, 0xc8);
            } else if (ownDistance <= 0x6000) {
                func_ov074_02152418(this, 0xc5, 0xc3);
            } else if (ownDistance < otherDistance && own.Dot(other) > 0) {
                func_ov074_02152418(this, 0xc6, 0xc3);
            } else {
                func_ov074_02152418(this, 0xc3, 0xc7);
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
                func_ov074_02167930(this, GetWalkTargetFloorPos());
            } else {
                func_ov074_02152418(this, 0xc3, 0xc7);
            }
        }
        break;

    case 0xc6:
        if (mStep == 0 && mSlot == 0) {
            FloorPos own = GetFloorPos();
            if (own.LengthSq() > 0x4000) {
                mWalkTarget.x = 0;
                mWalkTarget.z = 0;
                func_ov074_02167930(this, GetWalkTargetFloorPos());
            } else {
                func_ov074_02152418(this, 0xc5, 0xc3);
            }
        }
        break;

    case 0xc7:
        if (mSlot == 0) {
            break;
        }
        if (mStep == 0) {
            mStep++;
            mWalkTarget.x = 0;
            mWalkTarget.z = 0;
            mTargetAngle = AngleTo(GetWalkTargetFloorPos());
        } else if (mStep == 1) {
            if (func_ov074_02167a58(this, data_020fb898[data_020fba28[mPersonality]][mMood])) {
                mStep++;
                func_ov074_021691d4(this, 0, 0, 0, 4);
            }
        } else {
            if (GetFloorPos().LengthSq() < 0x1000) {
                func_ov074_02152418(this, 0xc8, 0xc9);
            } else {
                mWalkTarget.x = 0;
                mWalkTarget.z = 0;
                func_ov074_02167930(this, GetWalkTargetFloorPos());
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
            func_ov074_02169b00(this);
            func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
            Character_SetFace(this, func_ov074_02168720(this));
            func_ov074_0216950c(this, 1);
            func_ov074_02152464(this);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                func_ov074_02152370(this, 0xc2);
            }
        }
        break;

    case 0xc9:
        if (mStep == 0) {
            if (func_ov074_02168320(this, 0x20, 0, 0xff)) {
                mStep++;
                mTimer = 300;
                Character_SetFace(this, func_ov074_02168720(this));
                mHeldItem[0]->mRotY = 0x7000;
                gSoundManager.StartSeqArc(mSoundHandle, 0x1c, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                mStep++;
                func_ov074_02168774(this, ANIM, 0x21);
                mHeldItem[0]->mRotZ = 0x1000;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x1c, 0) == 0) {
                func_ov074_02152370(this, 0xc0);
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;
    }
    func_ov074_0215232c(this);
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
                int r = Rand(100);
                if (r < 35) {
                    func_ov074_02152418(this, 0xcb, 0xcb);
                } else if (r < 70) {
                    func_ov074_02152418(this, 0xcc, 0xcc);
                } else {
                    func_ov074_02152418(this, 0xce, 0xce);
                }
            } else {
                func_ov074_0216950c(this, 1);
                func_ov074_0216950c(mPartner, 1);
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    func_ov074_02168774(this, ANIM, 0x1a);
                }
                if (func_ov074_02168804(mPartner, mPartner->mModel.mAnimId)) {
                    func_ov074_02168774(mPartner, ANIM, 0x1a);
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
            func_ov074_02152418(this, 0xd5, 0xd5);
        }
        break;

    case 0xcb:
        if (func_ov074_0216973c(this, 0x59)) {
            func_ov074_02152370(this, 0xcf);
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcf:
        if (mStep == 0) {
            if (mSlot == 0 && mPartner->mState == 0xcf && mModel.mBlendFrames <= 0 &&
                mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Rand(300) + 2000;
                mTimer2 = Rand(60) + 90;
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
                        func_ov074_02168774(who, ANIM, 0x5a);
                        func_0208ef00(&who->mModel, TRUE, FALSE);
                        Character_ResetSoundPhase(who, 0);
                    }
                } else if (phase == 1) {
                    if (Character_PlaySoundAtAnimPhase(who, 0, 1, 0x25, 1, 4) == 2 ||
                        AnimModel_FindAnimation(&who->mModel, ANIM, who->mModel.mAnimId)->mFinished) {
                        mCounter++;
                        mTimer2 = Rand(15) + 2;
                        if (Rand(100) < 60) {
                            Character_SetFace(who, 0x15);
                        } else {
                            Character_SetFace(who, 8);
                        }
                    }
                } else {
                    mTimer2--;
                    if (mTimer2 <= 0 || AnimModel_FindAnimation(&who->mModel, ANIM, who->mModel.mAnimId)->mFinished) {
                        func_ov074_02168774(who, ANIM, 0x59);
                        Character_SetFace(who, 8);
                        if (Rand(100) < 9) {
                            mTimer2 = Rand(30) + 150;
                            mCounter = 0;
                            mTimer3++;
                        } else {
                            ModelAnim *anim = AnimModel_FindAnimation(&who->mModel, ANIM, who->mModel.mAnimId);
                            if (anim != NULL) {
                                int frames = (anim->mFrame->mResource->mFrameCount << FX32_SHIFT) >> FX32_SHIFT;
                                frames = (frames * who->mPartner->mAnimSpeed) >> FX32_SHIFT;
                                mTimer2 = (Rand(frames) >> 3) + (frames >> 4);
                            } else {
                                mTimer2 = Rand(18) + 2;
                            }
                            if (mCounter < 100) {
                                mCounter = 100;
                            } else {
                                mCounter = 0;
                            }
                        }
                        if (mTimer <= 0 && mTimer3 >= 4) {
                            func_ov074_02152418(this, 0xd3, 0xd3);
                        }
                    }
                }
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcc:
        if (func_ov074_0216973c(this, 0x2f)) {
            func_ov074_02152370(this, 0xd0);
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd0:
        if (mStep == 0) {
            if (mPartner->mState == 0xd0) {
                mStep++;
                mTimer = Rand(300) + 1700;
                mTimer2 = Rand(60) + 90;
            }
        } else if (mModel.mBlendFrames <= 0) {
            mTimer--;
            if (mTimer <= 0) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    func_ov074_02152418(this, 0xd3, 0xd3);
                    func_ov074_02168774(this, ANIM, 0x2f);
                    func_ov074_02168774(mPartner, ANIM, 0x2f);
                }
            } else {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Rand(110) + 120;
                    mTimer2 += ((FX32_ONE - mAnimSpeed) * 50) >> FX32_SHIFT;
                    int r = Rand(100);
                    if (mModel.mAnimId == 0x2f) {
                        r += 10;
                    } else {
                        r -= 10;
                    }
                    if (r < 50) {
                        func_ov074_02168774(this, ANIM, 0x2f);
                    } else {
                        func_ov074_02168774(this, ANIM, 0x30);
                    }
                    r = Rand(100);
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
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcd:
    case 0xce:
        if (func_ov074_0216973c(this, (mSlot == 0 && mState == 0xcd) ? 0x2d : 7)) {
            if (mState == 0xcd) {
                func_ov074_02152370(this, 0xd1);
            } else {
                func_ov074_02152370(this, 0xd2);
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
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
                        mTimer2 = Rand(60) + 90;
                        mTimer3 = 0;
                        if (mState != 0xd1) {
                            mTimer = 0;
                        } else {
                            mTimer = Rand(3) * 2 + 12;
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
                        mTimer2 = Rand(30) + 80;
                        func_ov074_02168774(who, ANIM, 0x2d);
                        Character_SetFace(who, 8);
                        func_ov074_021691d4(who, 0, 0, 0, 4);
                    }
                } else if (phase == 1) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        mTimer3++;
                        func_ov074_02168774(who, ANIM, 0x2e);
                        func_0208ef00(&who->mModel, TRUE, FALSE);
                        Character_ResetSoundPhase(who, 0);
                    }
                } else if (phase == 2) {
                    Character_PlaySoundAtAnimPhase(who, 0, mSoundHandle, 0x25, 2, 4);
                    if (AnimModel_FindAnimation(&who->mModel, ANIM, who->mModel.mAnimId)->mFinished) {
                        mCounter++;
                        mTimer2 = 250;
                        func_ov074_02168774(who, ANIM, 0x2d);
                        int r = Rand(100);
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
                        if (Rand(100) < 20) {
                            u16 yaw = func_ov074_021691fc(who->mPartner, who->GetFloorPos(), 0x2000);
                            func_ov074_021691d4(who->mPartner, func_ov074_02169318(who->mPartner, 0, 0x1800), yaw, 0, 5);
                        }
                    }
                } else if (phase == 3) {
                    if (gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0 || --mTimer2 <= 0) {
                        mCounter++;
                        Character_SetFace(who, 8);
                        func_ov074_021691d4(who->mPartner, 0, 0, 0, 5);
                    }
                } else {
                    mTimer2 = Rand(60) + 60;
                    if (mTimer3 >= 2 || Rand(100) < 20) {
                        mTimer3 = 0;
                        mTimer++;
                        func_ov074_02168774(who, ANIM, 7);
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
                        func_ov074_02152418(this, 0xd3, 0xd3);
                    }
                }
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd3:
        if (mStep == 0) {
            mStep++;
            mTimer = Rand(20) + 50;
            func_ov074_02152464(this);
            func_ov074_021691d4(this, 0, 0, 0, 4);
            Character_SetFace(this, 8);
        } else if (mStep == 1) {
            if (mSlot == 0) {
                if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mState == mPartner->mState) {
                    mTimer--;
                    if (mTimer <= 0) {
                        gSoundManager.StopSeqArc(1, 0);
                        mStep++;
                        mTimer = Rand(30) + 150;
                        RoomCharacter *first;
                        RoomCharacter *second;
                        if (Rand(2) < 1) {
                            first = this;
                            second = mPartner;
                        } else {
                            first = mPartner;
                            second = this;
                        }
                        int r = Rand(100);
                        if (r < 35) {
                            func_ov074_02168774(first, ANIM, 0x2a);
                            Character_ResetSoundPhase(first, 0);
                        } else if (r < 70) {
                            func_ov074_02168774(first, ANIM, 8);
                        } else {
                            func_ov074_02168774(first, ANIM, 0x4c);
                        }
                        if (Rand(100) < 60) {
                            Character_SetFace(first, 2);
                        } else {
                            Character_SetFace(first, 8);
                        }
                        r = Rand(100);
                        if (r < 35) {
                            func_ov074_02168774(second, ANIM, 6);
                        } else if (r < 70) {
                            func_ov074_02168774(second, ANIM, 1);
                        } else {
                            func_ov074_02168774(second, ANIM, 0x43);
                        }
                        if (Rand(100) < 60) {
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
                    mTimer = Rand(30) + 120;
                    func_ov074_02168774(this, ANIM, 7);
                    func_ov074_02168774(mPartner, ANIM, 7);
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0) {
                    func_ov074_02152418(this, 0xd4, 0xd4);
                }
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd4:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    mTimer = Rand(60) + 150;
                    func_ov074_02169b00(this);
                    func_ov074_02168774(this, ANIM, 0x4d);
                    func_ov074_02152464(this);
                    func_ov074_021691d4(this, 0, 0, 0, 4);
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
                }
            } else {
                mStep++;
                func_ov074_02152464(this);
                func_ov074_021691d4(this, 0, 0, 0, 4);
            }
        } else if (mSlot == 0) {
            mTimer--;
            if (mTimer <= 0) {
                int r = Rand(100);
                if (r < 35) {
                    func_ov074_02152418(this, 0xcb, 0xcb);
                } else if (r < 70) {
                    func_ov074_02152418(this, 0xcc, 0xcc);
                } else {
                    func_ov074_02152418(this, 0xce, 0xce);
                }
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd5:
        func_ov074_02167930(this, mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            func_ov074_02152418(this, 0xd6, 0xd7);
        }
        break;

    case 0xd6:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Rand(60) + 70;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                func_ov074_02152418(this, 0xd8, 0xd8);
            }
        }
        break;

    case 0xd7:
        func_ov074_02168110(this);
        break;

    case 0xd8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
            }
        } else if (mSlot == 0) {
            own = GetFloorPos();
            other = mPartner->GetFloorPos();
            if (FX_Mul(own.x, other.x) < 0) {
                func_ov074_02152418(this, 0xdb, 0xdb);
            } else if (own.x == other.x) {
                if (own.x > 0) {
                    func_ov074_02152418(this, 0xda, 0xdb);
                } else if (own.x < 0) {
                    func_ov074_02152418(this, 0xd9, 0xdb);
                } else {
                    func_ov074_02152418(this, 0xd9, 0xda);
                }
            } else if (own.x > other.x) {
                if (own.x > 0) {
                    func_ov074_02152418(this, 0xdb, 0xda);
                } else {
                    func_ov074_02152418(this, 0xd9, 0xdb);
                }
            } else {
                if (other.x > 0) {
                    func_ov074_02152418(this, 0xda, 0xdb);
                } else {
                    func_ov074_02152418(this, 0xdb, 0xd9);
                }
            }
        }
        break;

    case 0xd9: {
        FloorPos target = GetFloorPos();
        target.x = 0x3000;
        func_ov074_02167930(this, target);
        if (mModel.mPos.x >= 0x800) {
            func_ov074_02152370(this, 0xdb);
        }
        break;
    }

    case 0xda: {
        FloorPos target = GetFloorPos();
        target.x = -0x3000;
        func_ov074_02167930(this, target);
        if (mModel.mPos.x <= -0x800) {
            func_ov074_02152370(this, 0xdb);
        }
        break;
    }

    case 0xdb:
        if (mStep == 0) {
            if (mPartner->mState == 0xdb) {
                if (mModel.mPos.x >= 0) {
                    func_ov074_02152418(this, 0xdc, 0xdd);
                } else {
                    func_ov074_02152418(this, 0xdd, 0xdc);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
            }
        } else if (mPartner->mState == 0xdb) {
            if (mModel.mPos.x >= 0) {
                func_ov074_02152418(this, 0xdc, 0xdd);
            } else {
                func_ov074_02152418(this, 0xdd, 0xdc);
            }
        }
        break;

    case 0xdc:
    case 0xdd: {
        FloorPos target = data_ov074_0216d404[11];
        if (mState == 0xdd) {
            target.x = -target.x;
        }
        if (func_ov074_02167930(this, target)) {
            func_ov074_02152370(this, 0xde);
        }
        break;
    }

    case 0xde:
        if (mStep == 0) {
            mStep++;
            mTargetAngle = AngleTo(data_ov074_0216d404[12]);
        } else if (mStep == 1) {
            if (func_ov074_02167a58(this, data_020fb898[data_020fba28[mPersonality]][mMood])) {
                mStep++;
                func_ov074_021691d4(this, 0, 0, 0, 4);
                if (mPartner->mModel.mPos.z < 0x1800) {
                    func_ov074_02152370(this, 0xdf);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
            }
        } else {
            if (mPartner->mModel.mPos.z < 0x1800) {
                func_ov074_02152370(this, 0xdf);
            }
        }
        break;

    case 0xdf:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (func_ov074_02168320(this, 0x20, 2, 3)) {
                    mStep++;
                    AnimModel_FinishAnimations(mHeldItem[1]);
                    func_ov074_02169708(this);
                }
            } else {
                if (func_ov074_02168320(this, 0x20, 2, 0xff)) {
                    mStep++;
                    mHeldItem[0]->mRotY = 0x8000;
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, ANIM, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 150;
                func_ov074_02168774(this, ANIM, 0x4d);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = TRUE;
                func_ov074_02152370(this, 0xe0);
            }
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xe0:
        if (mPartner->mState == 0xe0) {
            func_ov074_021691d4(this, 0, 0, 0, 4);
            int r = Rand(100);
            if (r < 35) {
                func_ov074_02152418(this, 0xcb, 0xcb);
            } else if (r < 70) {
                func_ov074_02152418(this, 0xcc, 0xcc);
            } else {
                func_ov074_02152418(this, 0xce, 0xce);
            }
        } else if (mPartner->mState == 0xdf) {
            func_ov074_021691d4(this, 0, 0, 0, 4);
        } else {
            func_ov074_021691d4(this, 0, func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000), 0, 4);
        }
        func_ov074_02169f48(mHeldItem[0], mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;
    }
    func_ov074_0215232c(this);
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
                func_ov074_02152418(this, mPrevState, mPrevState);
            } else {
                func_ov074_0216950c(this, 1);
                func_ov074_0216950c(mPartner, 1);
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    func_ov074_02168774(this, ANIM, 0x1a);
                }
                if (func_ov074_02168804(mPartner, mPartner->mModel.mAnimId)) {
                    func_ov074_02168774(mPartner, ANIM, 0x1a);
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
            func_ov074_02152418(this, 0xe4, 0xe4);
        }
        break;

    case 0xe2:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            func_ov074_021682d0(this, data_020fb870[data_020fba28[mPersonality]][mMood]);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            Character_SetFace(this, 8);
            mTimer2 = 1;
            mTimer3 = 2;
        } else {
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Rand(100) + 300;
                if (mSlot == 0) {
                    if (Rand(100) < 35) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 2);
                    }
                    Character_SetFace(mPartner, mFace);
                }
            }
            mTimer3--;
            if (mTimer3 <= 0) {
                mTimer3 = Rand(100) + 400;
                if (mSlot == 0) {
                    func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), 0, 0, 4);
                    func_ov074_021691d4(mPartner, func_ov074_02169318(mPartner, 0, 0x1800), 0, 0, 4);
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
            func_ov074_02169b68(this);
            func_ov074_021682d0(this, 0x19);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            Character_SetFace(this, 2);
            mTimer2 = 200;
            mTimer3 = 300;
        } else {
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Rand(100) + 300;
                if (mSlot == 0) {
                    if (Rand(100) < 35) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 2);
                    }
                    Character_SetFace(mPartner, mFace);
                }
            }
            mTimer3--;
            if (mTimer3 <= 0) {
                mTimer3 = Rand(100) + 500;
                if (mSlot == 0) {
                    if (Rand(100) < 20) {
                        func_ov074_021691d4(this, 0, func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000), 0, 5);
                        func_ov074_021691d4(mPartner, 0, func_ov074_021691fc(mPartner, GetFloorPos(), 0x3000), 0, 5);
                    } else {
                        func_ov074_021691d4(this, 0, 0, 0, 5);
                        func_ov074_021691d4(mPartner, 0, 0, 0, 5);
                    }
                }
            }
        }
        break;

    case 0xe4:
    case 0xed:
        func_ov074_02167930(this, mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x2999)) {
            if (mState == 0xe4) {
                func_ov074_02152418(this, 0xe5, 0xe6);
            } else {
                func_ov074_02152418(this, 0xee, 0xef);
            }
        }
        break;

    case 0xe5:
    case 0xee:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Rand(60) + 70;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0xe5) {
                    func_ov074_02152418(this, 0xe7, 0xe7);
                } else {
                    func_ov074_02152418(this, 0xf0, 0xf0);
                }
            }
        }
        break;

    case 0xe6:
    case 0xef:
        func_ov074_02168110(this);
        break;

    case 0xe7:
    case 0xf0:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            func_ov074_021694e8(this);
            func_ov074_02168774(this, ANIM, data_020fb898[data_020fba28[mPersonality]][mMood]);
            Character_SetFace(this, 8);
            SetWalkTarget(mPartner->GetFloorPos());
            func_ov074_02152464(this);
            func_ov074_0216950c(this, 1);
        } else {
            if (func_ov074_0216943c(this, 0x100, 0, 0x3800)) {
                func_ov074_021691d4(this, 0, 0, 0, 4);
                if (mState == 0xe7) {
                    func_ov074_02152370(this, 0xe8);
                } else {
                    func_ov074_02152370(this, 0xf1);
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
            func_ov074_02169b00(this);
            func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
        } else if (mSlot == 0) {
            if (state == 0xe8) {
                if (mPartner->mState == 0xe8) {
                    func_ov074_02152418(this, 0xe2, 0xe2);
                }
            } else {
                if (mPartner->mState == 0xf1) {
                    func_ov074_02152418(this, 0xea, 0xea);
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
                func_ov074_02152418(this, mPrevState, mPrevState);
            } else {
                func_ov074_0216950c(this, 1);
                func_ov074_0216950c(mPartner, 1);
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    func_ov074_02168774(this, ANIM, 0x1a);
                }
                if (func_ov074_02168804(mPartner, mPartner->mModel.mAnimId)) {
                    func_ov074_02168774(mPartner, ANIM, 0x1a);
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
            func_ov074_02152418(this, 0xed, 0xed);
        }
        break;

    case 0xea:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02169b00(this);
            func_ov074_021682d0(this, data_020fb870[data_020fba28[mPersonality]][mMood]);
            func_ov074_021691d4(this, 0, 0, 0, 5);
            Character_SetFace(this, 2);
            mTimer = 80;
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 100;
                Character_SetFace(this, 5);
                HeadIcon_SetMode(&mHeadIcon, 1);
                if (mSlot == 0) {
                    mWait = 10000;
                    mPartner->mWait = 10000;
                    func_ov074_02152450(this, 0x18);
                    func_ov074_02152450(mPartner, 0x17);
                    func_ov074_02169d90(this);
                    func_ov074_02169d90(mPartner);
                }
            } else {
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), 0, 0, 4);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mSlot == 0) {
                    func_ov074_02152418(this, 0xeb, 0xeb);
                }
            } else {
                u16 pitch = func_ov074_02169280(this, CharacterModel_GetJointPosition(&mModel, 0), mLookTarget->mPos, 0x1800);
                u16 yaw = func_ov074_021691fc(this, mLookTarget->GetFloorPos(), 0x3800);
                func_ov074_021691d4(this, pitch, yaw, 0, 2);
            }
        }
        break;

    case 0xeb:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_021694e8(this);
            func_ov074_02168774(this, ANIM, data_020fb898[data_020fba28[mPersonality]][mMood]);
            Character_SetFace(this, 8);
            func_ov074_02152464(this);
            func_ov074_021691d4(this, 0, 0, 0, 3);
            mTargetAngle = mAngle + 0x5000 + Rand(0x1000) * 6;
        } else {
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0x200, 0)) {
                func_ov074_02152370(this, 0xec);
            }
        }
        break;

    case 0xec:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            func_ov074_02168774(this, ANIM, data_020fb870[data_020fba28[mPersonality]][mMood]);
        } else if (mPartner->mState == 0xec) {
            func_ov074_02152418(this, 2, 2);
            mReaction = 0;
            mPartner->mReaction = 0;
            mWait = 150;
            mPartner->mWait = Rand(30) + 160;
        }
        break;
    }
    func_ov074_0215232c(this);
}
