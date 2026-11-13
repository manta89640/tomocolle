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

// --- Group a1's own functions that are NOT WRITTEN yet (declared so that the written ones can call them) ---
// 0x0214ee90: the same as ChooseReactionsQuiet with more reactions (talking to the partner).
// 0x0214fd84 StartEvent(RoomCharacter *, int phase): switch on mEvent 0..18.
// 0x02150538 StartPairEvent(int phase): switch on mEvent 0x18..0x2e of two characters (the large one).
// 0x02151554 StartMeeting(int, int), 0x02151bf4, 0x02151d78: what two characters do when they come near.

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

// Not matching yet: 0x474 bytes against 0x468, same stack frame and same order of work. What differs:
// - one character present: the original keeps `character + 0x2000` in a register from reading the place to writing
//   mTarget (mine computes it again), with the z constant loaded between the two arms of the x choice;
// - two characters, third walk-target case: the original shares the -0x2000 of the comparison with mTarget.z;
// - last case: the two places are read in the order character 1, character 0, but character 0's copies are the
//   higher on the stack (0x30/0x08 against 0x28/0x00); mine has them the other way round.
// Tried: SetTarget() inline helpers, if/else instead of ?:, an inline member doing the whole first case, a free
// ToFloorPos(const VecFx32 &), both declaration orders and assignment instead of initialisation in the last case.
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
                    character->mTarget.x = pos.x < 0 ? -0x2666 : 0x2666;
                    character->mTarget.z = -0x1999;
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
                    mCharacters[0].mTarget.x = pos1.x < 0 ? 0x2666 : -0x2666;
                    mCharacters[0].mTarget.z = -0x2000;
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
            FloorPos pos1 = mCharacters[1].GetFloorPos();
            FloorPos pos0 = mCharacters[0].GetFloorPos();
            int nearer;
            int farther;
            if (pos0.LengthSq() > pos1.LengthSq()) {
                nearer = 1;
                farther = 0;
            } else {
                nearer = 0;
                farther = 1;
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

// Not matching yet: same size, same instructions; the registers of `partner` and `partnerIndex` are exchanged
// (original r6 and r5, mine r5 and r6), which also moves two instructions. Tried: a `character` local declared
// first and declared last, and no local at all (this form).
// 0x0214fad4
void RoomScene::ChooseTalkReaction(int index) {
    RoomCharacter *partner = mCharacters[index].mPartner;
    u16 partnerIndex = partner->mIndex;
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

// Not matching yet: 0x3c bytes against 0x44. The original takes the address of mHeadNode (this + 0x2860) and
// reads the position through it as z, y, x at +0x2c, +0x28, +0x24 before storing x, y, z; mine reads this + 0x2884
// directly. Tried: structure assignment, SetPos(VecFx32) by value with a GetPos() accessor, SetPos(x, y, z).
// 0x0215232c
void RoomCharacter::PlaceHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    mHeadIcon.SetPos(mHeadNode.mPos.x, mHeadNode.mPos.y, mHeadNode.mPos.z);
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
