#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "RoomCharacter.h"

extern "C" {
// Main program, not named yet. All take the character (or its model, or a held item) as their object.
void Character_SetFace(RoomCharacter *self, int face);     // Sets the face (expression) and restarts its animation
void func_0208ef00(CharacterModel *model, int playOnce, int finished); // Sets the current animation's two flags
void func_0208ef28(CharacterModel *model, int playOnce, fx32 frame, int finished); // The same and its frame
void Character_ResetSoundPhase(RoomCharacter *self, int slot);     // Restarts slot's progress of Character_PlaySoundAtAnimPhase
// Starts sound `index` of sequence archive `seqArc` at given quarters (bit mask) of the current animation
int Character_PlaySoundAtAnimPhase(RoomCharacter *self, int slot, int handle, int seqArc, int index, int quarters);
ModelAnimation *AnimModel_FindAnimation(CharacterModel *model, u32 resType, int id); // Finds an animation of the model
BOOL Character_IsPointWithinReach(RoomCharacter *self, fx32 x, fx32 z, fx32 distance, fx32 maxDistance); // Is (x, z) that near?
void func_02091598(RoomCharacter *self, FloorPos pos); // Steps away from pos
VecFx32 CharacterModel_GetJointPosition(CharacterModel *model, int index); // Position of one of the model's nodes
void AnimModel_FinishAnimations(HeldItem *item);                    // Marks all the item's animations finished

// Overlay 74, other groups', not named yet
BOOL func_ov074_02167a58(RoomCharacter *self, int anim);
void func_ov074_02167e04(RoomCharacter *self, int anim);
BOOL func_ov074_02167f78(RoomCharacter *self);
void func_ov074_02168110(RoomCharacter *self);
BOOL func_ov074_02168184(RoomCharacter *self, int anim);
BOOL func_ov074_02168320(RoomCharacter *self, int anim, u8 itemA, u8 itemB);
BOOL func_ov074_02168804(RoomCharacter *self, int anim);
BOOL func_ov074_02169144(RoomCharacter *self, u16 *angle, fx32 a);
void func_ov074_021691d4(RoomCharacter *self, u16 pitch, u16 yaw, u16 roll, u8 frames); // Turns the head
u16 func_ov074_021691fc(RoomCharacter *self, FloorPos pos, fx32 limit); // Head yaw towards pos, limited
u16 func_ov074_02169280(RoomCharacter *self, VecFx32 from, VecFx32 to, u16 limit); // Head pitch from..to, limited
u16 func_ov074_02169318(RoomCharacter *self, int node, fx32 limit); // Head pitch towards the partner's node
BOOL func_ov074_02169388(RoomCharacter *self, fx32 a, int b, int c, int d, int e);
BOOL func_ov074_0216943c(RoomCharacter *self, int a, int b, fx32 c);
void func_ov074_021694e8(RoomCharacter *self);
void func_ov074_0216950c(RoomCharacter *self, int a); // Calls 0x02169eac(item, a) for both held items
void func_ov074_02169598(RoomCharacter *self, fx32 y); // Puts held item 0 between the hands, at height y
void func_ov074_02169634(RoomCharacter *self);
void func_ov074_02169a98(RoomCharacter *self);
void func_ov074_02169b00(RoomCharacter *self);
void func_ov074_02169b68(RoomCharacter *self);
void func_ov074_02169bb4(RoomCharacter *self, int a);
void func_ov074_02169c04(RoomCharacter *self);
}

#define RES_TYPE_BCA0 0x30414342 // 'BCA0': a character animation

// The SDK's MATH_Rand16 on the game's generator, as a signed number: its results compare signed
static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

inline ModelAnimation *RoomCharacter::GetCurrentAnimation() {
    return AnimModel_FindAnimation(&mModel, RES_TYPE_BCA0, mModel.mAnimId);
}

inline BOOL RoomCharacter::IsWithin(FloorPos pos, fx32 distance, fx32 maxDistance) {
    return Character_IsPointWithinReach(this, pos.x, pos.z, distance, maxDistance);
}

// The states 0x70 to 0x92: three things the two characters of the room do together. Each group has the same shape:
// a hub state (0x70, 0x7f, 0x8b) in which slot 0 picks the next state for both, the activity states, and a way out
// (walk to the partner, 0x7b/0x86/0x8f, then a pair of closing states). mStep is the step inside a state.
//   0x70..0x7e: both play the same pair of animations (0x4e/0x4f, 0x50/0x51 or 0x52) in turns, with sounds of
//               sequence archive 0x24
//   0x7f..0x8a: animations 0x5b/0x5c with sounds of the same archive
//   0x8b..0x92: with held item 8 (ChangeHeldItems(8, 0xff)): animations 0x3f..0x42, sound 4 of archive 0x25, and
//               in 0x8e walking about to random places
void RoomCharacter::UpdatePairStates70To92() {
    int state = mState;
    switch (state) {
    case 0x70:
        if (mStep == 0 && mModel.mBlendLeft <= 0) {
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
                            mStateCount = 0x71;
                        } else {
                            mStateCount = 0x72;
                        }
                        other = mStateCount;
                    } else if (r < 50) {
                        if (diff < 0x4000) {
                            mStateCount = 0x73;
                        } else {
                            mStateCount = 0x74;
                        }
                        other = mStateCount;
                    } else if (r < 70) {
                        mStateCount = 0x75;
                        other = mStateCount;
                    } else if (r < 85) {
                        mStateCount = 0x7c;
                        other = 0x7d;
                    } else {
                        other = 0x7e;
                        mStateCount = other;
                    }
                    SetStateWithPartner(mStateCount, other);
                }
            } else {
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1a);
                }
                SetState(0x7b);
            }
            func_ov074_021691d4(this, 0, 0, 0, 5);
            func_ov074_0216950c(this, 1);
        }
        break;
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
        if (mModel.mBlendLeft <= 0 && mPartner->mModel.mBlendLeft <= 0 && mSlot != 0) {
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mStateCount = 0;
                func_ov074_02169b00(this);
                int prevAnim = mModel.mAnimId;
                mTimer = Random(80) + 0xfa;
                if (mState == 0x7a) {
                    SetAnimation(RES_TYPE_BCA0, 0x52);
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
                            SetAnimation(RES_TYPE_BCA0, mTimer2);
                        } else {
                            SetAnimation(RES_TYPE_BCA0, mTimer3);
                        }
                    } else if (mState == 0x76 || mState == 0x78) {
                        SetAnimation(RES_TYPE_BCA0, mPartner->mModel.mAnimId);
                    } else if (mTimer2 == mPartner->mModel.mAnimId) {
                        SetAnimation(RES_TYPE_BCA0, mTimer3);
                    } else {
                        SetAnimation(RES_TYPE_BCA0, mTimer2);
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
                    look = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                func_ov074_021691d4(this, 0, look, 0, 4);
                func_ov074_0216950c(this, 1);
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
                mStateCount++;
                if (mStateCount >= 2) {
                    mStateCount = 0;
                    if (mTimer > 0) {
                        if (mModel.mBlendLeft <= 0 && mPartner->mModel.mBlendLeft <= 0) {
                            Character_ResetSoundPhase(this, 0);
                            if (mState == 0x7a) {
                                SetAnimation(RES_TYPE_BCA0, 0x52);
                                mPartner->SetAnimation(RES_TYPE_BCA0, 0x52);
                            } else if (mTimer2 == mModel.mAnimId) {
                                SetAnimation(RES_TYPE_BCA0, mTimer3);
                                if (mState == 0x76 || mState == 0x78) {
                                    mPartner->SetAnimation(RES_TYPE_BCA0, mTimer3);
                                } else {
                                    mPartner->SetAnimation(RES_TYPE_BCA0, mTimer2);
                                }
                            } else {
                                SetAnimation(RES_TYPE_BCA0, mTimer2);
                                if (mState == 0x76 || mState == 0x78) {
                                    mPartner->SetAnimation(RES_TYPE_BCA0, mTimer2);
                                } else {
                                    mPartner->SetAnimation(RES_TYPE_BCA0, mTimer3);
                                }
                            }
                        } else {
                            mStateCount = 2;
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
        WalkTo(mPartner->GetFloorPos());
        if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x7c, 0x7d);
        }
        break;
    case 0x7c:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x70, 0x70);
            }
        }
        break;
    case 0x7d:
        func_ov074_02168110(this);
        break;
    case 0x7e:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(60) + 60;
        } else {
            func_ov074_02167e04(this, GetVariantAnim870());
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x70, 0x70);
            }
        }
        break;
    case 0x7f:
        if (mStep == 0 && mModel.mBlendLeft <= 0) {
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
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1a);
                }
                SetState(0x86);
                func_ov074_0216950c(this, 1);
            }
        }
        break;
    case 0x80:
    case 0x81:
        if (mModel.mBlendLeft <= 0 && mPartner->mModel.mBlendLeft <= 0 && mSlot != 0) {
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169c04(this);
                if (mModel.mAnimId >= 0) {
                    if (mState == 0x82) {
                        if (mModel.mAnimId != 0x5b) {
                            SetAnimation(RES_TYPE_BCA0, 0x1a);
                        }
                    } else if (mModel.mAnimId != 0x5c) {
                        SetAnimation(RES_TYPE_BCA0, 0x1a);
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
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(80) + 400;
                if (mState == 0x82) {
                    SetAnimation(RES_TYPE_BCA0, 0x5b);
                } else {
                    SetAnimation(RES_TYPE_BCA0, 0x5c);
                }
                if (mModel.mBlendLeft > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                func_0208ef00(&mModel, 1, 0);
                u16 look;
                if (Random(100) < 15) {
                    look = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                func_ov074_021691d4(this, 0, look, 0, 4);
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(60) + 0xfa;
                u16 look = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000);
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
                func_ov074_021691d4(this, 0, look, 0, 4);
            }
        } else {
            func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), mHeadYaw, 0, 4);
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
        WalkTo(mPartner->GetFloorPos());
        if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x89, 0x8a);
        }
        break;
    case 0x87:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                if (func_ov074_02168804(this, mModel.mAnimId)) {
                    SetAnimation(RES_TYPE_BCA0, 0x1a);
                }
                if (Random(100) < 25) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, GetExpression());
                }
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(60) + 60;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 7);
                u16 look;
                if (Random(100) < 40) {
                    look = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                func_ov074_021691d4(this, 0, look, 0, 4);
            }
        } else {
            if (mSlot == 0 && mPartner->mState == 0x87 && mModel.mBlendLeft <= 0 && mPartner->mModel.mBlendLeft <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetStateWithPartner(0x7f, 0x7f);
                    Character_SetFace(this, GetExpression());
                }
            }
        }
        break;
    case 0x88:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            func_ov074_02169144(this, &angle, 0);
            mTargetAngle = angle;
        } else if (func_ov074_02167a58(this, GetVariantAnim898())) {
            func_ov074_021691d4(this, 0, 0, 0, 4);
            SetState(0x87);
        }
        break;
    case 0x89:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x88, 0x88);
            }
        }
        break;
    case 0x8a:
        func_ov074_02168110(this);
        break;
    case 0x8b:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
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
                    func_ov074_0216950c(this, 1);
                    if (func_ov074_02168804(this, mModel.mAnimId)) {
                        SetAnimation(RES_TYPE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendLeft <= 0) {
            SetState(0x8f);
        }
        break;
    case 0x8c:
    case 0x8d:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (ChangeHeldItems(8, 0xff) && mModel.mBlendLeft <= 0) {
                mStep++;
                if (mState == 0x8c) {
                    func_ov074_02169b68(this);
                    SetAnimation(RES_TYPE_BCA0, 0x3f);
                } else {
                    func_ov074_02169b00(this);
                    SetAnimation(RES_TYPE_BCA0, 0x41);
                }
                Character_SetFace(this, 8);
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x96;
                gSoundManager.StartSeqArcLimited(mSoundHandle, 0x25, 4, 2);
            }
        } else {
            mTimer--;
            if (mModel.mBlendLeft <= 0) {
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
                        look = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2000);
                    } else {
                        look = 0;
                    }
                    func_ov074_021691d4(this, 0, look, 0, 4);
                    mTimer2 = Random(60) + 0x104;
                    if (mState == 0x8c) {
                        if (Random(100) < 70) {
                            SetAnimation(RES_TYPE_BCA0, 0x3f);
                            Character_SetFace(this, GetExpressionOfVariant(Random(5)));
                        } else {
                            SetAnimation(RES_TYPE_BCA0, 0x40);
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
        func_ov074_02169598(this, 0xcc);
        break;
    case 0x8e:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (ChangeHeldItems(8, 0xff) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 0x42);
                Character_SetFace(this, 8);
                if (mModel.mBlendLeft <= 0) {
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
                if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
                    FloorPos here = GetFloorPos();
                    if (ClampToWalkArea(&here)) {
                        SetState(0x8d);
                    } else {
                        func_02091598(this, mPartner->GetFloorPos());
                    }
                }
                if (func_ov074_02169388(this, 0x1000, 0, 0x199, 0x22, 6)) {
                    FloorPos p;
                    GetRandomPosInArea(&p);
                    SetTarget(p.x, 0, p.z);
                    Character_SetFace(this, GetExpressionOfVariant(Random(5)));
                }
            }
        }
        func_ov074_02169598(this, 0xcc);
        break;
    case 0x8f:
        WalkTo(mPartner->GetFloorPos());
        if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x90, 0x91);
        }
        break;
    case 0x90:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x92, 0x92);
            }
        }
        break;
    case 0x91:
        func_ov074_02168110(this);
        break;
    case 0x92:
        if (mStep == 0) {
            if (func_ov074_02168320(this, 0x3b, 8, 0xff)) {
                mStep++;
            }
        } else if (mModel.mBlendLeft <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0x8d);
        }
        func_ov074_02169598(this, 0);
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
            if (mModel.mBlendLeft <= 0) {
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
                    func_ov074_0216950c(this, 1);
                    if (func_ov074_02168804(this, mModel.mAnimId)) {
                        SetAnimation(RES_TYPE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendLeft <= 0) {
            SetState(0x99);
        }
        break;
    case 0x94:
    case 0x95:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (ChangeHeldItems(1, 0xff) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169bb4(this, 0);
                SetAnimation(RES_TYPE_BCA0, 0x3d);
                Character_SetFace(this, 8);
                mHeldItems[0]->mAngleY = 0xc000 - mAngle;
                if (mState == 0x95) {
                    VecFx32 mid;
                    GetMidpoint(GetNode(2), GetNode(1), &mid);
                    mid.y = 0xcc;
                    mHeldItems[0]->SetPos(mid);
                } else {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 2);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                if (mState == 0x94) {
                    if (Random(100) < 15) {
                        u16 look = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x3800);
                        func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), look, 0, 4);
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
                mHeldItems[0]->SetPos(mid);
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 0x28);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 6);
                AnimModel_FinishAnimations(mHeldItems[0]);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                int r = Random(100);
                if (r < 60) {
                    SetAnimation(RES_TYPE_BCA0, 0x19);
                } else if (r < 85) {
                    SetAnimation(RES_TYPE_BCA0, 0x1b);
                } else {
                    SetAnimation(RES_TYPE_BCA0, 0x33);
                }
                Character_SetFace(this, GetExpression());
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
                if (mModel.mBlendLeft > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(100) + 300;
                if (Random(100) < 70 || mModel.mAnimId == 0x33) {
                    HeldItem *item = mPartner->mHeldItems[0];
                    u16 look = func_ov074_021691fc(this, item->GetFloorPos(), 0x3000);
                    func_ov074_021691d4(
                        this, func_ov074_02169280(this, CharacterModel_GetJointPosition(&mModel, 0), item->mPos, 0x1800), look, 0, 4);
                } else {
                    func_ov074_021691d4(this, 0, 0, 0, 5);
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02168184(this, 0x1d);
                mTimer = Random(60) + 100;
            }
        } else if (func_ov074_02168184(this, 0x1d) && mTimer <= 0) {
            SetState(0x97);
        } else {
            mTimer--;
        }
        break;
    case 0x99:
        WalkTo(mPartner->GetFloorPos());
        if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x9a, 0x9b);
        }
        break;
    case 0x9a:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x9c, 0x9c);
            }
        }
        break;
    case 0x9b:
        func_ov074_02168110(this);
        break;
    case 0x9c:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            func_ov074_02169144(this, &angle, 0x1000);
            mTargetAngle = angle;
        } else if (func_ov074_02167a58(this, GetVariantAnim898())) {
            func_ov074_021691d4(this, 0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0x9d);
            } else {
                SetState(0x97);
            }
        }
        break;
    case 0x9d: {
        if (mStep == 0) {
            if (func_ov074_02168320(this, 0x20, 1, 0xff)) {
                mStep++;
                mHeldItems[0]->mAngleY = 0xc000 - mAngle;
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else if (mModel.mBlendLeft <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0x9e);
        }
        VecFx32 mid;
        GetMidpoint(GetNode(2), GetNode(1), &mid);
        mid.y = GetNode(2)->mPos.y;
        mHeldItems[0]->SetPos(mid);
        break;
    }
    case 0x9e: {
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x3c);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 8);
                mHeldItems[0]->mAngleY = 0xc000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                func_ov074_021691d4(this, 0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (GetCurrentAnimation()->mFinished) {
            SetState(0x95);
        }
        VecFx32 mid;
        GetMidpoint(GetNode(2), GetNode(1), &mid);
        mid.y = GetNode(2)->mPos.y;
        mHeldItems[0]->SetPos(mid);
        break;
    }
    case 0x9f:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                int prev = mPrevState;
                if (prev == 0xa0) {
                    SetState(0xa0);
                } else if ((u32)(prev - 0xa1) <= 1) {
                    SetState(0xa1);
                } else if ((u32)(prev - 0xa4) <= 2) {
                    SetState(0xa4);
                } else {
                    func_ov074_0216950c(this, 1);
                    if (func_ov074_02168804(this, mModel.mAnimId)) {
                        SetAnimation(RES_TYPE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendLeft <= 0) {
            SetState(0xa3);
        }
        break;
    case 0xa0:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (ChangeHeldItems(9, 0xff) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                SetAnimation(RES_TYPE_BCA0, 0x3f);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x96;
            }
        } else {
            mTimer--;
            if (mModel.mBlendLeft <= 0) {
                if (mTimer <= 0) {
                    SetState(0xa0);
                } else if (GetCurrentAnimation()->mFinished || --mTimer2 <= 0) {
                    mTimer2 = Random(60) + 0xaa;
                    if (Random(100) < 70) {
                        SetAnimation(RES_TYPE_BCA0, 0x3f);
                        Character_SetFace(this, GetExpressionOfVariant(Random(5)));
                    } else {
                        SetAnimation(RES_TYPE_BCA0, 0x40);
                        func_0208ef00(&mModel, 1, 0);
                        Character_SetFace(this, 8);
                        Character_ResetSoundPhase(this, 0);
                    }
                    if (Random(100) < 8) {
                        func_ov074_021691d4(
                            this, 0, func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2800), 0, 4);
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
        func_ov074_02169598(this, 0xcc);
        break;
    case 0xa1:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 65) {
                    SetAnimation(RES_TYPE_BCA0, 0x19);
                    Character_SetFace(this, GetExpression());
                } else if (r < 85) {
                    SetAnimation(RES_TYPE_BCA0, 0x1b);
                    Character_SetFace(this, GetExpression());
                } else {
                    SetAnimation(RES_TYPE_BCA0, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendLeft > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                if (Random(100) < 70) {
                    if (mModel.mAnimId != 0x17) {
                        HeldItem *item = mPartner->mHeldItems[0];
                        u16 look = func_ov074_021691fc(this, item->GetFloorPos(), 0x3000);
                        func_ov074_021691d4(
                            this, func_ov074_02169280(this, CharacterModel_GetJointPosition(&mModel, 0), item->mPos, 0x1800), look, 0,
                            4);
                    }
                } else {
                    func_ov074_021691d4(this, 0, 0, 0, 5);
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(60) + 100;
            }
        } else if (func_ov074_02168184(this, 0x1d) && mTimer <= 0) {
            SetState(0xa1);
        } else {
            mTimer--;
        }
        break;
    case 0xa3:
        WalkTo(mPartner->GetFloorPos());
        if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xa5, 0xa6);
        }
        break;
    case 0xa4:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            func_ov074_02169144(this, &angle, 0x1000);
            mTargetAngle = angle;
        } else if (func_ov074_02167a58(this, GetVariantAnim898())) {
            func_ov074_021691d4(this, 0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0xa7);
            } else {
                SetState(0xa1);
            }
        }
        break;
    case 0xa5:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xa4, 0xa4);
            }
        }
        break;
    case 0xa6:
        func_ov074_02168110(this);
        break;
    case 0xa7:
        if (mStep == 0) {
            if (func_ov074_02168320(this, 0x3b, 9, 0xff)) {
                mStep++;
            }
        } else if (mModel.mBlendLeft <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0xa0);
        }
        func_ov074_02169598(this, 0);
        break;
    case 0xa8:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                int prev = mPrevState;
                if ((u32)(prev - 0xa9) <= 1) {
                    SetState(0xa9);
                } else if ((u32)(prev - 0xab) <= 2) {
                    SetState(0xab);
                } else if ((u32)(prev - 0xaf) <= 2) {
                    SetState(0xaf);
                } else {
                    func_ov074_0216950c(this, 1);
                    if (func_ov074_02168804(this, mModel.mAnimId)) {
                        SetAnimation(RES_TYPE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendLeft <= 0) {
            SetState(0xae);
        }
        break;
    case 0xa9:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            func_ov074_021691d4(this, 0, 0, 0, 4);
        } else if (mStep == 1) {
            if (ChangeHeldItems(9, 5) && mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                SetAnimation(RES_TYPE_BCA0, 0x53);
                Character_SetFace(this, 8);
                mHeldItems[1]->mAngleZ = 0x8000;
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x8c;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 3);
                func_ov074_02169a98(this);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 80 || ClampToRect4E4(NULL)) {
                    SetState(0xa9);
                } else {
                    SetState(0xaa);
                }
            } else if (mModel.mBlendLeft <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(60) + 0x8c;
                    if (GetCurrentAnimation()->mFinished) {
                        func_0208ef00(&mModel, 0, 0);
                        func_ov074_021691d4(this, 0, 0, 0, 4);
                        gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 3);
                        func_ov074_02169a98(this);
                    } else if (Random(100) < 30) {
                        u16 lookA;
                        u16 lookB;
                        if (Random(100) < 60) {
                            lookA = -((Random(9) << 9) + 0x600);
                            lookB = 0;
                        } else {
                            lookA = 0xf700;
                            lookB = func_ov074_021691fc(this, mPartner->GetFloorPos(), 0x2000);
                        }
                        func_ov074_021691d4(this, lookA, lookB, 0, 4);
                        func_0208ef00(&mModel, 1, 1);
                        gSoundManager.StopSeqArc(mSoundHandle, 0);
                    }
                }
            }
        }
        func_ov074_02169634(this);
        break;
    case 0xaa:
        if (mStep == 0) {
            mStep++;
            func_ov074_021691d4(this, 0, 0, 0, 5);
        } else if (mStep == 1) {
            if (ChangeHeldItems(9, 5) && mModel.mBlendLeft <= 0) {
                mStep++;
                SetAnimation(RES_TYPE_BCA0, 0x46);
                Character_SetFace(this, 6);
                mHeldItems[1]->mAngleZ = 0x8000;
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
        func_ov074_02169634(this);
        break;
    case 0xab:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_02169b68(this);
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 65) {
                    SetAnimation(RES_TYPE_BCA0, 0x19);
                    Character_SetFace(this, GetExpression());
                } else if (r < 85) {
                    SetAnimation(RES_TYPE_BCA0, 0x1b);
                    Character_SetFace(this, GetExpression());
                } else {
                    SetAnimation(RES_TYPE_BCA0, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendLeft > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                func_ov074_0216950c(this, 1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                if (Random(100) < 70) {
                    if (mModel.mAnimId != 0x17) {
                        u16 look = func_ov074_021691fc(this, mPartner->mHeldItems[0]->GetFloorPos(), 0x3000);
                        func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), look, 0, 4);
                    }
                } else {
                    func_ov074_021691d4(this, 0, 0, 0, 5);
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
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(60) + 0x78;
            }
        } else if (func_ov074_02168184(this, 0x1d) && mTimer <= 0) {
            SetState(0xab);
        } else {
            mTimer--;
        }
        break;
    case 0xad:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                mTimer = Random(60) + 0xb4;
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_021691d4(this, func_ov074_02169318(this, 0, 0x1800), mUnknownAngle25D6, 0, 4);
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
        WalkTo(mPartner->GetFloorPos());
        if (IsWithin(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xb0, 0xb1);
        }
        break;
    case 0xaf:
        if (mStep == 0) {
            if (mModel.mBlendLeft <= 0) {
                mStep++;
                func_ov074_021694e8(this);
                func_ov074_02169b00(this);
                SetAnimation(RES_TYPE_BCA0, GetVariantAnim898());
                SetTargetFloorPos(mPartner->GetFloorPos());
                ShowHeadIcon();
                func_ov074_0216950c(this, 1);
            }
        } else if (func_ov074_0216943c(this, 0x100, 0, 0x3800)) {
            func_ov074_021691d4(this, 0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0xb2);
            } else {
                SetState(0xab);
            }
        }
        break;
    case 0xb0:
        if (mStep == 0) {
            if (func_ov074_02167f78(this)) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            func_ov074_02167f78(this);
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xaf, 0xaf);
            }
        }
        break;
    case 0xb1:
        func_ov074_02168110(this);
        break;
    case 0xb2:
        if (mStep == 0) {
            if (func_ov074_02168320(this, 0x3b, 9, 5)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                mHeldItems[1]->mAngleZ = 0x8000;
            }
        } else if (mModel.mBlendLeft <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0xa9);
        }
        func_ov074_02169634(this);
        break;
    }
    PlaceHeadIcon();
}
