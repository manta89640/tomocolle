#include <nitro.h>
#include <nnsys.h>

#include "RoomScene.h"

extern "C" {
// --- Main program, not named yet ---

// The game's random number generator: every use is the inline code of the SDK's MATH_Rand16
extern MATHRandContext16 gRandom;
// For each kind of relation, how much each reaction (0..11) changes the affinity by, halved
extern const s8 *data_021135c8[];
// The saved game (named gGameData by the sound code, which reads the volume settings from it)
extern struct SavedGame gGameData;

// Tests of a character's event (mEvent, 0x224)
BOOL func_02099c84(CharacterBase *character);
BOOL func_02099d1c(CharacterBase *character); // The event is one with a partner (0x17, 0x19, ... 0x2d)
BOOL func_02099d2c(u8 event);                 // The same, of an event number
BOOL func_02099da8(CharacterBase *character);
BOOL func_02099e64(CharacterBase *character);
// A value from 0 to 200 computed from the bytes at 0x4a and 0x4b; the higher, the shorter the character waits
int func_02098ec4(CharacterBase *character);
// Sets what the character is to the character with that index: mRelation[index] and the byte at 0xfc + index
void func_020990b0(CharacterBase *character, int index, u8 relation, int affinity);
// Sets the character's mood from the result of an encounter, unless its mood is locked
void func_02098fe0(CharacterBase *character, int result, int relation);
// Adds to the character's affinity for the character with that index, keeping it in 0..200
void func_02099088(CharacterBase *character, int index, int change);
// Chooses the character's reaction (0..11) to its partner
int func_0209d5ac(CharacterBase *character, s8 relationKind, s8 partnerReaction, CharacterBase *partner);
// Resets the character to nothing, and sets its appearance (the scene does both when the saved game has no such one)
void func_020979e0(CharacterBase *character);
void func_02098c40(CharacterBase *character, int kind, int value);

// A distance multiplied by the character's 0x2af0
fx32 func_02091174(RoomCharacter *character, fx32 distance);
// Sets the way the character faces
void Character_SetAngle(RoomCharacter *character, u16 angle);
// Makes the character walk towards a point, and away from it
void func_02091598(RoomCharacter *character, FloorPos pos);
void func_0209171c(RoomCharacter *character, FloorPos pos);
// The point at that distance in front of the character
FloorPos func_02091abc(RoomCharacter *character, fx32 distance);
void func_02090fdc(RoomCharacter *character, int param1, int param2, int param3);
// Moves the character and its model for this frame
void func_020910a8(RoomCharacter *character);

// The saved game's characters, by index
BOOL func_0202c3dc(SavedGame *savedGame, int index);
void func_0202c264(SavedGame *savedGame, int index, CharacterBase *character); // Reads one
void func_0202c28c(SavedGame *savedGame, int index, CharacterBase *character); // Writes one

// Copies the character's name out as a string
void func_0207a80c(CharacterBase *character, wchar_t *name);
// Sets a string that replaces a tag of the message (MessageText::mStringArgs)
void func_0202ebe8(MessageText *message, const wchar_t *string, int index);
// The speech engine is still speaking
int func_0208dac4(void *engine);

// --- Overlay 74, other groups' ---

// The two-step state of the scene's object at 0x692c
void func_ov074_0216b8a8(void *object, int state);
void func_ov074_0216c1f0(void *object, u8 param);
// Starts drawing the scene's message in that mode
void func_ov074_0216d1a8(MessageText *message, int drawMode);

// The character's next reaction, from its last one
int func_ov074_021686c0(RoomCharacter *character);
// Tests of a state: standing (1, 2, 4), walking (8, 9), sitting (0x10..0x12)
BOOL func_ov074_02168798(RoomCharacter *character, int state);
BOOL func_ov074_021687b8(RoomCharacter *character);
BOOL func_ov074_021687dc(RoomCharacter *character);
// Random points of the room, and tests of a point against the room (a null point is the character's own place)
void func_ov074_0216898c(RoomCharacter *character, FloorPos *pos);
void func_ov074_02168a20(RoomCharacter *character, FloorPos *pos);
BOOL func_ov074_02168ae4(RoomCharacter *character, FloorPos *pos);
BOOL func_ov074_02168b38(RoomCharacter *character, FloorPos *pos);
BOOL func_ov074_02168b8c(RoomCharacter *character, FloorPos *pos);
void func_ov074_02168be0(RoomCharacter *character, FloorPos *pos);
void func_ov074_02168c28(RoomCharacter *character, FloorPos *pos);
void func_ov074_02168d24(RoomCharacter *character, FloorPos pos, FloorPos *result);
u16 func_ov074_02168dc4(RoomCharacter *character, u16 angle);
void func_ov074_02169144(RoomCharacter *character, u16 *angle, fx32 param);
u16 func_ov074_021691fc(RoomCharacter *character, FloorPos pos, fx32 param);
// Shows or hides the two things the character holds
void func_ov074_02169540(RoomCharacter *character, int visible);
void func_ov074_02169890(RoomCharacter *character, int overrideState);
void func_ov074_021698a8(RoomCharacter *character);
void func_ov074_02169d5c(RoomCharacter *character);

// The functions of the states, by range of mState
void func_ov074_02152690(RoomCharacter *character);
void func_ov074_02154608(RoomCharacter *character);
void func_ov074_021578a8(RoomCharacter *character);
void func_ov074_0215ac7c(RoomCharacter *character);
void func_ov074_0215b45c(RoomCharacter *character);
void func_ov074_0215d1b4(RoomCharacter *character);
void func_ov074_0215f610(RoomCharacter *character);
void func_ov074_021616e8(RoomCharacter *character);
void func_ov074_021633d4(RoomCharacter *character);
void func_ov074_0216420c(RoomCharacter *character);
void func_ov074_0216526c(RoomCharacter *character);
void func_ov074_021655a4(RoomCharacter *character);
void func_ov074_02166664(RoomCharacter *character);
void func_ov074_02167384(RoomCharacter *character);
void func_ov074_0216779c(RoomCharacter *character);

extern const FloorPos data_ov074_0216d404[];
extern const s16 data_ov074_0216d484[];
}

#define RAND(max) MATH_Rand16(&gRandom, max)

// The same as a function: its int result makes a comparison of it signed
static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

static inline fx32 LengthSq(const FloorPos &pos) {
    return FX_Mul(pos.x, pos.x) + FX_Mul(pos.z, pos.z);
}

// 0x0214e5f8
void RoomScene::ChooseEncounter() {
    int count = CountPresent();
    if (count == 0) {
        if (func_02099c84(&mCharacters[0])) {
            SetEventState(4);
        } else {
            SetEventState(12);
        }
    } else if (count == 1) {
        RoomCharacter *character = mCharacters[0].mPresent ? &mCharacters[0] : &mCharacters[1];
        if (func_02099c84(character)) {
            SetEventState(14);
            character->SetState(0x106);
            mActor = character;
        } else if (func_02099da8(character)) {
            if (mResidentCount != 1) {
                FloorPos pos = character->GetFloorPos();
                if (pos.z < -0x1800) {
                    if (pos.x < 0) {
                        character->mTarget.x = -0x2666;
                        character->mTarget.z = -0x1999;
                    } else {
                        character->mTarget.x = 0x2666;
                        character->mTarget.z = -0x1999;
                    }
                    character->SetState(0x112);
                } else {
                    character->SetState(0x111);
                }
                SetEventState(13);
                mActor = character->mPartner;
                mActorOther = character;
            } else {
                SetEventState(5, -1);
            }
        } else {
            SetEventState(5, -1);
        }
    } else if (count == 2) {
        int chance = RAND(1000);
        int withPartner = func_02099d1c(&mCharacters[1]);
        if (!withPartner || (withPartner && mCharacters[1].mEventPartner != mCharacters[0].mIndex)) {
            FloorPos pos1 = mCharacters[1].GetFloorPos();
            FloorPos pos0 = mCharacters[0].GetFloorPos();
            if (pos0.z < -0x1800) {
                if (pos1.x <= 0 && pos1.x < pos0.x && pos0.x < 0x2000) {
                    mCharacters[0].mTarget.x = 0x2666;
                    mCharacters[0].mTarget.z = -0x2000;
                    mCharacters[0].SetState(0x10b);
                } else if (pos1.x >= 0 && pos1.x > pos0.x && pos0.x > -0x2000) {
                    mCharacters[0].mTarget.x = -0x2666;
                    mCharacters[0].mTarget.z = -0x2000;
                    mCharacters[0].SetState(0x10b);
                } else if (pos0.x > -0x2000 && pos0.x < 0x2000) {
                    if (pos1.x < 0) {
                        mCharacters[0].mTarget.x = 0x2666;
                        mCharacters[0].mTarget.z = -0x2000;
                    } else {
                        mCharacters[0].mTarget.x = -0x2666;
                        mCharacters[0].mTarget.z = -0x2000;
                    }
                    mCharacters[0].SetState(0x10b);
                } else {
                    mCharacters[0].SetState(0x10a);
                }
            } else {
                mCharacters[0].SetState(0x10a);
            }
            RoomCharacter *other = &mCharacters[1];
            other->SetState(1);
            SetEventState(15);
            mActor = other;
            mActorOther = &mCharacters[0];
        } else if (chance < 150 && func_02099e64(&mCharacters[0])) {
            mEventFrames = 0;
            int nearer;
            int farther;
            if (LengthSq(mCharacters[0].GetFloorPos()) <= LengthSq(mCharacters[1].GetFloorPos())) {
                nearer = 0;
                farther = 1;
            } else {
                nearer = 1;
                farther = 0;
            }
            StartEncounter(nearer, farther);
        } else {
            SetEventState(6, -1);
        }
    } else {
        SetEventState(6, -1);
    }
}

// 0x0214ea60
void RoomScene::ChooseReactionsQuiet() {
    for (int i = 0; i < 2; i++) {
        if (!mCharacters[i].mPresent) {
            continue;
        }
        if (mCharacters[i].mWait > 0 && mCharacters[i].mState != 0) {
            continue;
        }
        int patience = 200 - func_02098ec4(&mCharacters[i]);
        mCharacters[i].mWait = 240 - RAND(50) + (patience >> 2);
        int reaction = func_ov074_021686c0(&mCharacters[i]);
        mCharacters[i].mReaction = reaction;
        switch (reaction) {
        case 0: {
            int chance = RAND(100);
            if (chance < 50) {
                mCharacters[i].SetState(1);
            } else if (chance < 55) {
                mCharacters[i].SetState(7);
                mCharacters[i].mWait = 10000;
            } else {
                mCharacters[i].SetState(2);
            }
            break;
        }
        case 1: {
            FloorPos pos;
            func_ov074_0216898c(&mCharacters[i], &pos);
            if (!func_02091edc(&mCharacters[i], pos, func_02091174(&mCharacters[i], 0x800))) {
                mCharacters[i].SetState(8);
                mCharacters[i].SetTarget(pos.x, 0, pos.z);
            } else {
                pos = func_02091abc(&mCharacters[i], 0x1000);
                func_ov074_02168ae4(&mCharacters[i], &pos);
                FloorPos back;
                back = func_02091abc(&mCharacters[i], -0x1000);
                func_ov074_02168ae4(&mCharacters[i], &back);
                if (pos.LengthSq() <= back.LengthSq()) {
                    mCharacters[i].SetTarget(pos.x, pos.z);
                } else {
                    mCharacters[i].SetTarget(back.x, back.z);
                }
                mCharacters[i].SetState(8);
            }
            break;
        }
        case 5:
            mCharacters[i].SetState(12);
            break;
        case 2:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 6:
            mCharacters[i].SetState(14);
            mCharacters[i].mWait = 1000;
            break;
        case 3:
            mCharacters[i].SetState(15);
            mCharacters[i].mWait = 1000;
            break;
        case 4: {
            int chance = RAND(100);
            if (chance < 50) {
                mCharacters[i].SetState(16);
            } else {
                mCharacters[i].SetState(17);
            }
            break;
        }
        case 7:
            mCharacters[i].SetState(21);
            break;
        case 8:
            mCharacters[i].SetState(22);
            break;
        case 9:
            mCharacters[i].SetState(23);
            break;
        case 10:
            mCharacters[i].SetState(24);
            break;
        case 11:
            mCharacters[i].SetState(25);
            break;
        case 12:
            mCharacters[i].SetState(26);
            break;
        case 13:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 14:
            mCharacters[i].SetState(28);
            mCharacters[i].mWait += 30;
            break;
        default:
            mCharacters[i].mReaction = 0;
            mCharacters[i].mWait = 10;
            break;
        }
    }
}

// Not matching yet: same size (0x65c), same instructions and stack frame; only the callee-saved registers are
// named differently throughout (95 instructions). Original: r4 = &gRandom, r5 = i, r6 = partner and the generator's
// temporary, r7 = this + i * 0x2d98, r8 = &mCharacters[i]; mine: r4 = this + ..., r5 = &mCharacters[i], r6 = &gRandom,
// r7 = i, r8 = partner. Tried: RAND macro instead of Rand(), a local pointer to gRandom, `partner`, `chance`,
// `patience` and `i` declared at the top of the function, no `patience` local, a `character` local (changes size).
// @0x0214ee90
void RoomScene::ChooseReactions() {
    for (int i = 0; i < 2; i++) {
        if (!mCharacters[i].mPresent) {
            continue;
        }
        if (mCharacters[i].mWait > 0 && mCharacters[i].mState != 0) {
            continue;
        }
        int patience = 200 - func_02098ec4(&mCharacters[i]);
        mCharacters[i].mWait = 240 - Rand(50) + (patience >> 2);
        int reaction = func_ov074_021686c0(&mCharacters[i]);
        mCharacters[i].mReaction = reaction;
        switch (reaction) {
        case 0: {
            int chance = Rand(100);
            if (chance < 30) {
                mCharacters[i].SetState(1);
            } else if (chance < 45) {
                mCharacters[i].SetState(2);
            } else {
                int chance2 = Rand(100);
                RoomCharacter *partner = mCharacters[i].mPartner;
                if (mCharacters[i].IsFacing(partner) && partner->IsBeside(&mCharacters[i], 0x1000) &&
                    (partner->IsOneTwoOrFour(partner->mState) || partner->IsState10To12()) && chance2 < 40) {
                    mCharacters[i].SetState(5);
                    mCharacters[i].mWait >>= 1;
                    partner->mWait = mCharacters[i].mWait + Rand(10);
                } else {
                    mCharacters[i].SetState(4);
                }
            }
            break;
        }
        case 1: {
            FloorPos pos;
            if (Rand(100) < 85 || mCharacters[i].ClampToRect4E4(NULL) ||
                mCharacters[i].mPartner->ClampToWalkArea(NULL)) {
                mCharacters[i].GetRandomPos(&pos);
            } else {
                FloorPos middle = Half(mCharacters[0].GetFloorPos() + mCharacters[1].GetFloorPos());
                pos = middle;
            }
            if (!func_02091edc(&mCharacters[i], pos, func_02091174(&mCharacters[i], 0x800))) {
                if (Rand(100) < 40) {
                    mCharacters[i].SetState(8);
                } else {
                    mCharacters[i].SetState(9);
                }
                mCharacters[i].SetTarget(pos.x, 0, pos.z);
            } else {
                mCharacters[i].SetState(11);
            }
            break;
        }
        case 5:
            mCharacters[i].SetState(12);
            break;
        case 2:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 6:
            mCharacters[i].SetState(14);
            mCharacters[i].mWait = 1000;
            break;
        case 3:
            mCharacters[i].SetState(15);
            mCharacters[i].mWait = 1000;
            break;
        case 4: {
            int chance = Rand(100);
            if (chance < 27) {
                mCharacters[i].SetState(16);
            } else if (chance < 45) {
                mCharacters[i].SetState(17);
            } else {
                int chance2 = Rand(100);
                RoomCharacter *partner = mCharacters[i].mPartner;
                if (mCharacters[i].IsFacing(partner) && partner->IsBeside(&mCharacters[i], 0x1000) &&
                    (partner->IsOneTwoOrFour(partner->mState) || partner->IsState10To12()) && chance2 < 40) {
                    mCharacters[i].SetState(19);
                    mCharacters[i].mWait >>= 1;
                    partner->mWait = mCharacters[i].mWait + Rand(10);
                } else {
                    mCharacters[i].SetState(18);
                }
            }
            break;
        }
        case 7:
            mCharacters[i].SetState(21);
            break;
        case 8:
            mCharacters[i].SetState(22);
            break;
        case 9:
            mCharacters[i].SetState(23);
            break;
        case 10:
            mCharacters[i].SetState(24);
            break;
        case 11:
            mCharacters[i].SetState(25);
            break;
        case 12:
            mCharacters[i].SetState(26);
            break;
        case 13:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 14:
            mCharacters[i].SetState(28);
            mCharacters[i].mWait += 30;
            break;
        default:
            mCharacters[i].mReaction = 0;
            mCharacters[i].mWait = 10;
            break;
        }
    }
}

// 0x0214f4ec
void RoomScene::ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode) {
    if (first != NULL) {
        func_0202ebe8(&mMessage, first->mNameText, 0);
    }
    if (second != NULL) {
        func_0202ebe8(&mMessage, second->mNameText, 1);
    }
    mMessage.mColor = 1;
    func_ov074_0216d1a8(&mMessage, drawMode);
    mMessage.mMessage = message;
}

// 0x0214f564
void RoomScene::ShowEventMessage() {
    if (mCharacters[1].mPresent) {
        ShowMessage(&mCharacters[0], &mCharacters[1], 4, 1);
    } else if (mCharacters[0].mPresent) {
        ShowMessage(&mCharacters[0], NULL, 3, 1);
    } else {
        u8 event = mCharacters[0].mEvent;
        if (func_02099d2c(event)) {
            ShowMessage(&mCharacters[0], &mCharacters[1], 5, 1);
        } else {
            switch (event) {
            case 20:
                ShowMessage(&mCharacters[0], NULL, 6, 1);
                break;
            case 19:
                ShowMessage(&mCharacters[0], NULL, 7, 1);
                break;
            case 21:
                ShowMessage(&mCharacters[0], NULL, 8, 1);
                break;
            }
        }
    }
}

// 0x0214f67c
void RoomScene::ApplyEncounterResult(RoomCharacter *first, RoomCharacter *second, int result, int kind) {
    u8 relation = first->mRelation[second->mIndex];
    u16 firstIndex = first->mIndex;
    func_020990b0(first, second->mIndex, result, 100);
    func_020990b0(second, firstIndex, result, 100);
    func_02098fe0(first, result, relation);
    func_02098fe0(second, result, relation);
    if (kind == 1) {
        mEncounterJudge.Apply(first, second, relation, mTime);
    } else if (kind == 2) {
        mEncounterJudge.ApplyEnd(first, second, relation, mTime, 0);
    }
    SaveCharacter(first);
    SaveCharacter(second);
}

// 0x0214f788
int RoomScene::JudgeEncounterEnd(RoomCharacter *first, RoomCharacter *second) {
    if (first->mPrevState == 0xf9 || second->mPrevState == 0xf9) {
        return mEncounterJudge.JudgeEnd(first, second, FALSE);
    } else {
        return mEncounterJudge.JudgeEnd(first, second, TRUE);
    }
}

// 0x0214f7cc
void RoomScene::StartEncounter(int nearer, int farther) {
    BOOL near = AreCharactersNear(0, 1, 0x2333, 0x2666);
    if (mCharacters[nearer].IsAhead(&mCharacters[farther], 0x1b33) &&
        mCharacters[farther].IsAhead(&mCharacters[nearer], 0x3bae) &&
        !mCharacters[nearer].IsNearerThan(&mCharacters[farther], 0x8333) && !near) {
        mCharacters[farther].SetState(0xf2);
        mCharacters[nearer].SetState(0xf4);
    } else if (near) {
        mCharacters[0].SetState(1);
        mCharacters[1].SetState(1);
    } else {
        mCharacters[0].SetState(0xf2);
        mCharacters[1].SetState(0xf2);
    }
    SetEventState(7);
    mCharacters[0].mWait = 10000;
    mCharacters[1].mWait = 10000;
}

// 0x0214f950
void RoomScene::UpdateEncounter() {
    for (int i = 0; i < 2; i++) {
        if (mCharacters[i].mWait <= 0) {
            ChooseTalkReaction(i);
            if (mEncounterJudge.Judge(&mCharacters[i], mCharacters[i].mPartner) != 10) {
                SetEventState(10);
            }
        }
    }
}

// 0x0214f9d0
void RoomScene::FinishEncounter() {
    if (mEncounterPending && CountPresent() >= 2) {
        for (int i = 0; i < 20; i++) {
            ChooseTalkReaction(mCharacters[0].mWait > mCharacters[1].mWait ? 1 : 0);
            if (mCharacters[0].mState == 0xf6 && mCharacters[1].mState == 0xf6) {
                break;
            }
        }
        int result = mEncounterJudge.Judge(&mCharacters[0], &mCharacters[1]);
        if (result != 10) {
            ApplyEncounterResult(&mCharacters[0], &mCharacters[1], result, 1);
        } else {
            result = JudgeEncounterEnd(mActor, mActorOther);
            if (result != 10) {
                ApplyEncounterResult(&mCharacters[0], &mCharacters[1], result, 2);
            }
        }
    }
    mEncounterPending = FALSE;
}

// 0x0214fad4
void RoomScene::ChooseTalkReaction(int index) {
    u16 partnerIndex;
    RoomCharacter *partner = mCharacters[index].mPartner;
    partnerIndex = partner->mIndex;
    u8 relation = mCharacters[index].mRelation[partnerIndex];
    int reaction = func_0209d5ac(&mCharacters[index], relation, partner->mReaction, partner);
    int patience = (200 - func_02098ec4(&mCharacters[index])) >> 2;
    mCharacters[index].mReaction = reaction;
    mCharacters[index].mWait = patience + 160;
    partner->mWait = patience + 150;
    func_02099088(&mCharacters[index], partnerIndex, data_021135c8[relation][reaction] * 2);
    mActor = &mCharacters[index];
    mActorOther = mCharacters[index].mPartner;
    StartTalkReaction(index, reaction);
}

// 0x0214fbb0
void RoomScene::StartTalkReaction(int index, int reaction) {
    switch (reaction) {
    case 0:
        if (!mCharacters[index].mMoodLocked) {
            int state = mCharacters[index].mState;
            if (state == 0xf9) {
                mCharacters[index].mMood = 2;
            } else if (state == 0xf8 || state == 0xff) {
                mCharacters[index].mMood = 1;
            } else if (state == 0x100) {
                mCharacters[index].mMood = 0;
            }
        }
        mCharacters[index].SetState(0xf6);
        break;
    case 1:
        mCharacters[index].SetState(0xf7);
        break;
    case 2:
        mCharacters[index].SetState(0xf8);
        break;
    case 3:
        mCharacters[index].SetState(0xf9);
        break;
    case 4:
        mCharacters[index].SetState(0xfa);
        break;
    case 5:
        mCharacters[index].SetState(0xfb);
        break;
    case 6:
        mCharacters[index].SetState(0xfc);
        break;
    case 7:
        mCharacters[index].SetState(0xfd);
        break;
    case 8:
        mCharacters[index].SetState(0xfe);
        break;
    case 9:
        mCharacters[index].SetState(0xff);
        break;
    case 10:
        mCharacters[index].SetState(0x100);
        break;
    case 11:
        mCharacters[index].SetState(0x101);
        break;
    default:
        mCharacters[index].mReaction = 0;
        break;
    }
}

// @0x0214fd84
void RoomScene::StartEvent(RoomCharacter *character, int phase) {
    if (character->mPrevEvent != character->mEvent) {
        func_ov074_02169540(character, FALSE);
    }
    switch (character->mEvent) {
    case 0:
        if (phase == 0) {
            mLight.SetState(2);
            character->SetState(0x117);
        }
        character->mReaction = 0;
        character->mWait = 0;
        break;
    case 1:
        if (phase == 0) {
            character->SetState(0x1d);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x1e);
        }
        break;
    case 2:
        if (phase == 0) {
            character->SetState(0x27);
            mLight.SetState(2);
        } else {
            character->SetState(0x28);
            character->mUnknown25D4 = 0xd100;
        }
        break;
    case 3:
        if (phase == 0) {
            character->SetState(0x29);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x2a);
        }
        break;
    case 4:
        if (phase == 0) {
            character->SetState(0x2e);
            mLight.SetState(2);
        } else {
            FloorPos pos;
            character->GetRandomEdgePos(&pos);
            character->SetFloorPos(pos.x, pos.z);
            u16 angle = func_ov074_02168dc4(character, FX_Atan2Idx(pos.z, pos.x));
            int state = Rand(2) + 0x2f;
            if (state == 0x2f) {
                character->SetTarget(pos.x, pos.z);
            } else {
                FloorPos next;
                func_ov074_02168d24(character, pos, &next);
                character->SetTarget(next.x, next.z);
                angle -= 0x4000;
            }
            Character_SetAngle(character, angle);
            character->SetState(state);
        }
        break;
    case 5:
        if (phase == 0) {
            character->SetState(0x34);
            mLight.SetState(2);
        } else {
            character->mModel.mPos.x = 0;
            character->mModel.mPos.z = -0x800;
            Character_SetAngle(character, 0x4000);
            character->SetState(Rand(3) + 0x35);
        }
        break;
    case 6:
        if (phase == 0) {
            character->SetState(0x3d);
            mLight.SetState(2);
        } else {
            character->SetState(0x3e);
            FloorPos pos;
            character->GetRandomCorner(&pos);
            character->mModel.mPos.x = pos.x;
            character->mModel.mPos.z = pos.z;
            Character_SetAngle(character, FX_Atan2Idx(pos.z, pos.x));
        }
        break;
    case 7:
        if (phase == 0) {
            character->SetState(0x40);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x41);
        }
        break;
    case 8:
        if (phase == 0) {
            character->SetState(0x44);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x45);
        }
        break;
    case 9:
        if (phase == 0) {
            character->SetState(0x48);
            mLight.SetState(2);
        } else {
            character->SetState(0x4c);
        }
        break;
    case 10:
        if (phase == 0) {
            character->SetState(0x22);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x23);
        }
        break;
    case 11:
        if (phase == 0) {
            character->SetState(0x4f);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x50);
        }
        break;
    case 12:
        if (phase == 0) {
            character->SetState(0x54);
            mLight.SetState(2);
        } else {
            character->SetState(0x55);
        }
        break;
    case 13:
        if (phase == 0) {
            character->SetState(0x58);
            mLight.SetState(2);
        } else {
            character->SetState(0x59);
        }
        break;
    case 14:
        if (phase == 0) {
            character->SetState(0x5b);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x5c);
        }
        break;
    case 15:
        if (phase == 0) {
            character->SetState(0x5e);
            mLight.SetState(2);
        } else {
            character->SetState(0x5f);
        }
        break;
    case 16:
        if (phase == 0) {
            character->SetState(0x61);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x62);
        }
        break;
    case 17:
        if (phase == 0) {
            character->SetState(0x65);
            mLight.SetState(2);
        } else {
            character->SetState(0x66);
        }
        break;
    case 18:
        if (phase == 0) {
            character->SetState(0x68);
            mLight.SetState(4);
            mFront.DrawWindow(character->mUnknown004F);
        } else {
            character->SetState(0x6a);
        }
        break;
    }
}

// Not matching yet: same size (0xee8) and stack frame, 15 instructions differ, all in case 0x2a: the original loads
// data_ov074_0216d404[1] after the four stores of the two places (interleaved with reading the first character's
// place back: pos.x, point.x, pos.z, point.z, in r6, r3, r5, r2), mine loads it between those stores, which also
// moves the place constants from r2/r3 to r5/r6. The original keeps the point's x and z across the first
// FX_Atan2Idx in a slot at sp+8, as this form does. Tried: the table not const (it is read again after the call,
// frame 8 bytes smaller), a named FloorPos copy in the case or at the top of the function (the copy lands at the top
// of the frame, not at sp+8), a const copy, a copy filled field by field, a by-value inline wrapper, a pointer and a
// reference to the entry, GetOffsetTo taking the point by value.
// @0x02150538
void RoomScene::StartPairEvent(int phase) {
    RoomCharacter *pair[2];
    for (int i = 0; i < 2; i++) {
        pair[i] = &mCharacters[i];
        if (pair[i]->mPrevEvent != pair[i]->mEvent) {
            func_ov074_02169540(pair[i], FALSE);
        }
    }
    RoomCharacter *second = pair[1];
    if (pair[0]->mEventPartner != second->mIndex) {
        return;
    }
    switch (pair[0]->mEvent) {
    case 0x18:
        if (phase == 0) {
            pair[0]->SetState(0x117);
            second->SetState(0x117);
            mLight.SetState(2);
        }
        pair[0]->mReaction = 0;
        second->mReaction = 0;
        pair[0]->mWait = 0;
        second->mWait = 0;
        break;
    case 0x1a:
        if (phase == 0) {
            pair[0]->SetState(0x70);
            second->SetState(0x70);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x199);
            int kind = Rand(100);
            if (Rand(100) < 30) {
                Character_SetAngle(pair[0], 0x8000);
                Character_SetAngle(second, 0);
                if (kind < 35) {
                    pair[0]->SetState(0x72);
                } else if (kind < 70) {
                    pair[0]->SetState(0x74);
                } else {
                    pair[0]->SetState(0x75);
                }
            } else {
                Character_SetAngle(pair[0], 0x4000);
                Character_SetAngle(second, 0x4000);
                if (kind < 40) {
                    pair[0]->SetState(0x71);
                } else if (kind < 75) {
                    pair[0]->SetState(0x73);
                } else {
                    pair[0]->SetState(0x75);
                }
            }
            second->SetState(pair[0]->mState);
        }
        break;
    case 0x1c:
        if (phase == 0) {
            pair[0]->SetState(0x7f);
            second->SetState(0x7f);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x199);
            Character_SetAngle(pair[0], 0x4400);
            Character_SetAngle(second, 0x3c00);
            int state = Rand(2) + 0x80;
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x1e:
        if (phase == 0) {
            pair[0]->SetState(0x8b);
            second->SetState(0x8b);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, pair[0]->mModel.mPos.z >> 1);
            second->SetFloorPos(-0x1666, second->mModel.mPos.z >> 1);
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                FloorPos pos = character->GetFloorPos();
                Character_SetAngle(character, FX_Atan2Idx(pos.z, pos.x) + 0x8000);
                int chance = Rand(100);
                if (chance < 70) {
                    character->SetState(0x8c);
                } else if (chance < 90) {
                    character->SetState(0x8d);
                } else {
                    character->SetState(0x8e);
                }
            }
        }
        break;
    case 0x20:
        if (phase == 0) {
            pair[0]->SetState(0x93);
            second->SetState(0x93);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x266);
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                func_ov074_02169144(character, &angle, 0x1000);
                Character_SetAngle(character, angle);
            }
            second->mUnknown25D6 = func_ov074_021691fc(second, pair[0]->GetFloorPos(), 0x3000);
            pair[0]->SetState(0x95);
            second->SetState(0x97);
        }
        break;
    case 0x22:
        if (phase == 0) {
            pair[0]->SetState(0x9f);
            second->SetState(0x9f);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1333, 0x266);
            second->SetFloorPos(-0x1333, -0x333);
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                func_ov074_02169144(character, &angle, 0xa00);
                Character_SetAngle(character, angle);
            }
            second->mUnknown25D6 = func_ov074_021691fc(second, pair[0]->GetFloorPos(), 0x3800);
            pair[0]->SetState(0xa0);
            second->SetState(0xa1);
        }
        break;
    case 0x24:
        if (phase == 0) {
            pair[0]->SetState(0xa8);
            second->SetState(0xa8);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1333, 0x266);
            second->SetFloorPos(-0x1333, -0x333);
            fx32 extra = Rand(16) << 9;
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                func_ov074_02169144(character, &angle, extra + 0x1000);
                Character_SetAngle(character, angle);
            }
            second->mUnknown25D6 = func_ov074_021691fc(second, pair[0]->GetFloorPos(), 0x3800);
            pair[0]->SetState(0xa9);
            second->SetState(0xab);
        }
        break;
    case 0x26:
        if (phase == 0) {
            pair[0]->SetState(0xb3);
            second->SetState(0xb3);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x266);
            fx32 distance = Rand(100) < 65 ? 0x1000 : 0x4000;
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                func_ov074_02169144(character, &angle, distance);
                Character_SetAngle(character, angle);
            }
            int state = Rand(100) < 55 ? 0xb4 : 0xb5;
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x28:
        if (phase == 0) {
            pair[0]->SetState(0xbf);
            second->SetState(0xbf);
            mLight.SetState(2);
        } else {
            u16 angle = Rand(256) << 8;
            second->SetFloorPos(FX_Mul(FX_CosIdx(angle), 0xf33), FX_Mul(FX_SinIdx(angle), 0xf33));
            angle = angle + 0x4000 + (Rand(256) << 7);
            pair[0]->SetFloorPos(FX_Mul(FX_CosIdx(angle), 0x24cc), FX_Mul(FX_SinIdx(angle), 0x24cc));
            second->mUnknown25D6 = func_ov074_021691fc(second, pair[0]->GetFloorPos(), 0x3800);
            pair[0]->SetState(Rand(100) < 50 ? 0xc0 : 0xc1);
            second->SetState(0xc2);
        }
        break;
    case 0x2a:
        if (phase == 0) {
            pair[0]->SetState(0xca);
            second->SetState(0xca);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x800);
            second->SetFloorPos(-0x1666, -0x8cc);
            Character_SetAngle(pair[0], Angle(pair[0]->GetOffsetTo(data_ov074_0216d404[1])));
            Character_SetAngle(second, Angle(second->GetOffsetTo(data_ov074_0216d404[1])));
            int state = Rand(3) + 0xcb;
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x2c:
        if (phase == 0) {
            pair[0]->SetState(0xe1);
            second->SetState(0xe1);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1451, 0xccc);
            second->SetFloorPos(-0x1451, 0xe66);
            u16 angles[2];
            int state;
            if (Rand(100) < 67) {
                angles[0] = Angle(pair[0]->GetOffsetTo(second->GetFloorPos()));
                angles[1] = Angle(second->GetOffsetTo(pair[0]->GetFloorPos()));
                state = Rand(100) < 70 ? 0xe2 : 0xe3;
            } else {
                func_ov074_02169144(pair[0], &angles[0], 0x8000);
                func_ov074_02169144(second, &angles[1], 0x8000);
                state = 0xe3;
            }
            Character_SetAngle(pair[0], angles[0]);
            Character_SetAngle(second, angles[1]);
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x2e:
        if (phase == 0) {
            pair[0]->SetState(0xe9);
            second->SetState(0xe9);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1451, 0xccc);
            second->SetFloorPos(-0x1451, 0xe66);
            u16 angle0 = Angle(pair[0]->GetOffsetTo(second->GetFloorPos()));
            u16 angle1 = Angle(second->GetOffsetTo(pair[0]->GetFloorPos()));
            Character_SetAngle(pair[0], angle0);
            Character_SetAngle(second, angle1);
            pair[0]->SetState(0xea);
            second->SetState(0xea);
        }
        break;
    }
}

// 0x02151420
int RoomScene::CountPresent() {
    int count = 0;
    for (int i = 0; i < 2; i++) {
        if (mCharacters[i].mPresent) {
            count++;
        }
    }
    return count;
}

// 0x02151454
int RoomScene::GetCharacterCountForDisplay() {
    if (mResidentCount == 0) {
        if (CountPresent() == 0) {
            return 0;
        }
        int index = mCharacters[0].mPresent ? 0 : 1;
        return mCharacters[index].mIndex != 1 ? 1 : 2;
    } else if (mResidentCount == 1) {
        return mResidents[0] != 1 ? 1 : 2;
    } else {
        if (CountPresent() == 0) {
            return mCharacters[0].mIndex;
        } else {
            return mCharacters[0].mEventPartner;
        }
    }
}

// 0x021514f0
void RoomScene::UpdateMeeting() {
    if (CountPresent() != 2) {
        return;
    }
    if (!AreCharactersNear(0, 1, 0x2333, 0x2666)) {
        return;
    }
    StartMeeting(0, 1);
}

// Not matching yet: same size (0x6a0), 75 instructions differ, of two kinds. (1) Stack: the original has the place
// that the first loop clamps at sp+0x78, above posFirst (0x70) and posSecond (0x68); here it is below them (as the
// local of ClampOwnPosToRect4E4, or as a local of the loop's body). Declared at the top of the function and assigned
// in the loop, it is where it should be but costs a third copy (frame 0xa8). (2) Registers: the original has
// first * 0x2d98 in r9 and the two characters in r7 and r8; mine r7, and r8 and r9.
// @0x02151554
void RoomScene::StartMeeting(int first, int second) {
    RoomCharacter *pair[2];
    FloorPos posFirst = mCharacters[first].GetFloorPos();
    FloorPos posSecond = mCharacters[second].GetFloorPos();
    fx32 distanceFirst = posFirst.LengthSq();
    fx32 distanceSecond = posSecond.LengthSq();
    if (distanceFirst - distanceSecond <= 0) {
        pair[0] = &mCharacters[first];
        pair[1] = &mCharacters[second];
    } else {
        pair[0] = &mCharacters[second];
        pair[1] = &mCharacters[first];
    }
    if (mCharacters[first].mState == 10 && mCharacters[second].mState == 10) {
        func_02091598(&mCharacters[first], mCharacters[second].GetFloorPos());
        func_02091598(&mCharacters[second], mCharacters[first].GetFloorPos());
        for (int i = 0; i < 2; i++) {
            RoomCharacter *character = pair[i];
            if (character->ClampOwnPosToRect4E4()) {
                character->SetState(1);
            }
        }
    } else if (mCharacters[first].mState == 10) {
        ApproachWaiting(first, second, distanceFirst - distanceSecond);
    } else if (mCharacters[second].mState == 10) {
        ApproachWaiting(second, first, distanceSecond - distanceFirst);
    } else if (mCharacters[first].IsState8Or9() && mCharacters[second].IsState8Or9()) {
        int chance;
        if (!mCharacters[first].ClampToRect4E4(NULL) && !mCharacters[second].ClampToRect4E4(NULL)) {
            chance = Rand(100);
        } else {
            chance = Rand(70);
        }
        if (chance < 40) {
            RoomCharacter *other = pair[1];
            func_0209171c(pair[0], other->GetFloorPos());
            int random = Rand(40);
            int patience = 200 - func_02098ec4(pair[0]);
            pair[0]->mWait = 100 - random + (patience >> 2);
            pair[0]->SetState(10);
            pair[0]->mReaction = 1;
            if (Rand(100) < 40) {
                other->SetState(1);
            } else {
                other->SetState(4);
            }
            other->mWait = 120;
            other->mReaction = 0;
        } else if (chance < 70) {
            RoomCharacter *other;
            if (Rand(100) < 70 && (other = pair[1], pair[0]->IsBeside(other, 0x1000)) &&
                other->IsBeside(pair[0], 0x1000)) {
                if (Rand(0) & 1) {
                    pair[0]->SetState(5);
                    other->SetState(3);
                } else {
                    other->SetState(5);
                    pair[0]->SetState(3);
                }
            } else {
                mCharacters[first].SetState(3);
                mCharacters[second].SetState(3);
            }
            mCharacters[first].mReaction = 0;
            mCharacters[second].mReaction = 0;
            int wait = Rand(30) + 60;
            pair[0]->mWait = wait;
            pair[1]->mWait = wait + 20;
        } else {
            func_02091598(&mCharacters[first], mCharacters[second].GetFloorPos());
            mCharacters[first].SetSavedTarget(mCharacters[first].mTarget);
            mCharacters[first].mWait = Rand(60) + 90;
            mCharacters[first].SetState(10);
            mCharacters[first].mReaction = 1;
            func_02091598(&mCharacters[second], mCharacters[first].GetFloorPos());
            mCharacters[second].SetSavedTarget(mCharacters[second].mTarget);
            mCharacters[second].mWait = Rand(60) + 90;
            mCharacters[second].SetState(10);
            mCharacters[second].mReaction = 1;
        }
    }
    if (mCharacters[first].IsState8Or9()) {
        AnswerWalker(first, second);
    } else if (mCharacters[second].IsState8Or9()) {
        AnswerWalker(second, first);
    }
}

// @0x02151bf4
void RoomScene::ApproachWaiting(int index, int other, int distanceDifference) {
    BOOL near = AreCharactersNear(index, other, 0x1800, 0x1b33);
    if (mCharacters[index].ClampToRect534(NULL)) {
        if (distanceDifference <= 0) {
            mCharacters[index].mTarget.x = 0;
            mCharacters[index].mTarget.z = 0;
        } else {
            mCharacters[index].SetState(11);
        }
    } else if (mCharacters[index].ClampToRect4E4(NULL)) {
        if (distanceDifference <= 0 || near) {
            func_02091598(&mCharacters[index], mCharacters[other].GetFloorPos());
        } else {
            func_0209171c(&mCharacters[index], mCharacters[other].GetFloorPos());
        }
    } else {
        if (near) {
            func_02091598(&mCharacters[index], mCharacters[other].GetFloorPos());
        } else {
            func_0209171c(&mCharacters[index], mCharacters[other].GetFloorPos());
        }
    }
}

// @0x02151d78
void RoomScene::AnswerWalker(int index, int walker) {
    int chance = Rand(100);
    BOOL facing;
    BOOL sitting;
    BOOL standing = mCharacters[walker].IsOneTwoOrFour(mCharacters[walker].mState);
    sitting = mCharacters[walker].IsState10To12();
    if (mCharacters[index].IsBeside(&mCharacters[walker], 0x1000) &&
        mCharacters[walker].IsBeside(&mCharacters[index], 0x1000)) {
        facing = TRUE;
        if (!standing) {
            chance -= 10;
        }
    } else {
        chance -= 10;
        facing = FALSE;
    }
    int prevState = mCharacters[index].mPrevState;
    if (prevState == 3 || prevState == 5 || prevState == 6 || chance < 50) {
        func_0209171c(&mCharacters[index], mCharacters[walker].GetFloorPos());
        int random = Rand(60);
        int patience = 200 - func_02098ec4(&mCharacters[index]);
        mCharacters[index].mWait = random + 90 + (patience >> 2);
        mCharacters[index].SetState(10);
        mCharacters[index].mReaction = 1;
        return;
    }
    if ((standing || sitting) && facing && Rand(100) < 50) {
        mCharacters[index].SetState(5);
        mCharacters[index].mReaction = 0;
        if (!sitting) {
            mCharacters[walker].SetState(3);
            mCharacters[walker].mReaction = 0;
        }
        mCharacters[index].mWait = Rand(60) + 60;
        mCharacters[walker].mWait = mCharacters[index].mWait + Rand(20);
    } else {
        mCharacters[index].mWait = 60;
        mCharacters[index].SetState(1);
        mCharacters[index].mReaction = 0;
    }
}

// 0x0215206c
BOOL RoomScene::AreCharactersNear(int first, int second, fx32 reach, fx32 maxReach) {
    if (mCharacters[first].IsWithinReach(&mCharacters[second], reach, maxReach) ||
        mCharacters[second].IsWithinReach(&mCharacters[first], reach, maxReach)) {
        return TRUE;
    }
    return FALSE;
}

// 0x02152124
void RoomScene::SetFadeState(int state) {
    mFadeState = state;
    mFadeStep = 0;
}

// 0x0215213c
void RoomScene::SaveCharacter(RoomCharacter *character) {
    if (character == NULL) {
        return;
    }
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c28c(&gGameData, index, character);
    }
}

// 0x0215217c
void RoomScene::SaveCharacters() {
    SaveCharacter(&mCharacters[0]);
    SaveCharacter(&mCharacters[1]);
}

// 0x021521a0
void RoomScene::LoadCharacter(RoomCharacter *character) {
    if (character == NULL) {
        return;
    }
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c264(&gGameData, index, character);
    } else {
        func_020979e0(character);
        func_02098c40(character, 40, index & 7);
        character->mIndex = index;
    }
}

// 0x02152204
void RoomScene::LoadCharacters() {
    LoadCharacter(&mCharacters[0]);
    LoadCharacter(&mCharacters[1]);
}

// 0x02152228
RoomCharacter::RoomCharacter() {
    func_ov074_02169890(this, 0);
    mUnknownClearedByConstructor = 0;
}

// 0x02152268
void RoomCharacter::Init(int slot, int param2, int param3) {
    func_02090fdc(this, slot, param2, param3);
    mState = mPrevState = 0;
    mSlot = slot;
    mWait = 0;
    mStep = 0;
    mPrevEvent = 0;
    mSubSubStep = 0;
    mSubStep = mSubSubStep;
    mKeepSound = FALSE;
    func_ov074_02169d5c(this);
    mSpeechText[0] = mSpeechTextDefault;
    mSpeechParam[0] = mSpeechDefault;
    for (int i = 0; i < 4; i++) {
        mSpeechText[i + 1] = mSpeechTextByMood[i];
        mSpeechParam[i + 1] = mSpeechByMood[i];
    }
}

// 0x02152308
void RoomCharacter::LoadHeadIcon(int param) {
    mHeadIcon.Load(param, data_ov074_0216d484[mMood]);
}

// 0x0215232c
void RoomCharacter::PlaceHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    PositionedNode *node = GetNode(0);
    mHeadIcon.SetPos(node->mPos.x, node->mPos.y, node->mPos.z);
}

// 0x02152370
void RoomCharacter::SetState(int state) {
    if (mState == 7 && func_0208dac4(mTtsPlayer->mEngine) == 1) {
        mTtsPlayer->Stop();
    }
    mPrevState = mState;
    mState = state;
    mStep = 0;
    mSubSubStep = 0;
    mSubStep = mSubSubStep;
    MI_CpuClear16(&mUnknown2BE4, 2);
    if (mKeepSound) {
        mKeepSound = FALSE;
        return;
    }
    gSoundManager.StopSeqArc(mSoundHandle, 0);
}

// 0x02152418
void RoomCharacter::SetStateWithPartner(int state, int partnerState) {
    SetState(state);
    mPartner->SetState(partnerState);
}

// 0x0215243c
void RoomCharacter::CopyName() {
    func_0207a80c(this, mNameText);
}

// 0x02152450
void RoomCharacter::SetEvent(u8 event) {
    mPrevEvent = mEvent;
    mEvent = event;
}

// 0x02152464
void RoomCharacter::ShowHeadIcon() {
    if (mMood == 0) {
        mHeadIcon.SetMode(1);
        return;
    }
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    s16 iconId = data_ov074_0216d484[mMood];
    if (mHeadIcon.mIconId != iconId) {
        mHeadIcon.Show();
        mHeadIcon.mIconId = iconId;
        mHeadIcon.Reload();
        mHeadIcon.SetMode(3);
    } else {
        mHeadIcon.Restart();
        mHeadIcon.SetMode(3);
    }
}

// 0x02152500
void RoomCharacter::Update() {
    func_ov074_021698a8(this);
    if (mPresent && mOverrideState == 0) {
        int state = mState;
        if (state != 0) {
            if (state <= 0x1c) {
                func_ov074_02152690(this);
            } else if (state <= 0x3f) {
                func_ov074_02154608(this);
            } else if (state <= 0x67) {
                func_ov074_021578a8(this);
            } else if (state <= 0x6f) {
                func_ov074_0215ac7c(this);
            } else if (state <= 0x92) {
                func_ov074_0215b45c(this);
            } else if (state <= 0xb2) {
                func_ov074_0215d1b4(this);
            } else if (state <= 0xc9) {
                func_ov074_0215f610(this);
            } else if (state <= 0xe0) {
                func_ov074_021616e8(this);
            } else if (state <= 0xf1) {
                func_ov074_021633d4(this);
            } else if (state <= 0x103) {
                func_ov074_0216420c(this);
            } else if (state <= 0x105) {
                func_ov074_0216526c(this);
            } else if (state <= 0x10b) {
                func_ov074_021655a4(this);
            } else if (state <= 0x112) {
                func_ov074_02166664(this);
            } else if (state <= 0x116) {
                func_ov074_02167384(this);
            } else {
                func_ov074_0216779c(this);
            }
        }
        if (mWait > 0) {
            mWait--;
        }
        func_020910a8(this);
    }
    mHeadIcon.Update();
}
