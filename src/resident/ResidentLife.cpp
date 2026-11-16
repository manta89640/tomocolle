#include <nitro.h>

#include "GameData.h"
#include "resident/ResidentLife.h"

// What the residents do by themselves: the events that start and end with time, what came of the time the game was
// off (overlay 106's PreStartScene::UpdateResidents calls the functions in turn), and the encounters of two
// characters (EncounterJudge, which overlay 74's apartment scene holds and calls too).
//
// The names are descriptive, not the original: no name of these functions survives in the ROM.

// A saved character as these functions use it (the main program's class, 0x2C0 bytes; GameData_ReadCharacter copies the
// saved game's record into one, GameData_WriteCharacter back). The fields are named as the character headers name them.
struct SavedCharacter {
    SavedCharacter();
    // Its functions that these call, as include/character/Character.h names them
    void SetMoodFromResult(int result, int detail);
    void AddRelationMeter(int index, int change);
    void SetRelation(int index, u8 rank, u8 meter);
    static BOOL IsFightingRelation(int rank);
    BOOL IsFriendlyOrFightingRelation(int rank);
    void EndLongAct(SavedCharacter *first, SavedCharacter *second);
    BOOL IsLongActPaired();
    void StartLongAct(int state, s64 now, RTCTime *time, SavedCharacter *first, SavedCharacter *second);
    int ChooseLongAct();
    bool IsInTrouble();
    static bool IsTroubleStateActive(int state);
    static BOOL IsTroubleProgressFullAt(s64 now, s64 start, int progress);
    BOOL IsTroubleOfCategory34OrKind20();
    BOOL IsAngryAfterFight();
    static BOOL IsAngryAfterFightState(int state, int category, int id);
    static int GetFightsAllowed(int fights, int residents);
    void EndTrouble(s64 now);
    void ClearTrouble();
    BOOL StartRandomTrouble(s64 now, int force, SavedCharacter *partner, int unused);
    static int GetRelationBloodPairValue(int a, int b);
    void RandomizeUnknown24E();
    void ResetUnknown24E();
    void SetTrouble(u8 state, u8 category, u8 id);
    void ExpireTrouble(s64 now, SavedCharacter *partner);
    void BecomeAngryAt(SavedCharacter *other, s64 now, SavedCharacter *partner);
    int GetFullness(s64 now);
    void Eat(s64 now, u16 item);
    int GetClothesLiking(int clothes);

    /* 0x000 */ u8 mUnknown000[0x54];
    /* 0x054 */ u32 mUnknown054Bits0 : 11;
    /*       */ u32 mClothesId : 9;    // The clothes it wears (row of the clothes table)
    /*       */ u32 mUnknown054Bits20 : 12;
    /* 0x058 */ u8 mUnknown058[0xA4];
    /* 0x0FC */ u8 mRelationMeter[100];     // For each other character by index
    /* 0x160 */ u8 mRelationRank[100];     // For each other character by index
    /* 0x1C4 */ u8 mUnknown1C4[0x24];
    /* 0x1E8 */ u32 mOwnedClothes[10];  // A bit for each of the 300 clothes
    /* 0x210 */ u8 mUnknown210[0xC];
    /* 0x21C */ s64 mLongActEnd;         // A time in seconds; 0x7FFFFFFFFFFFFFFF for none
    /* 0x224 */ u8 mLongActState;
    /* 0x225 */ u8 mLongActPartner;      // mIndex of the character it is doing it with
    /* 0x226 */ u8 mTroubleId;       // Row and table of data_02114d78 for the time it is out
    /* 0x227 */ u8 mTroubleCategory;
    /* 0x228 */ s64 mTroubleStart;
    /* 0x230 */ s64 mTroubleEnd;
    /* 0x238 */ u16 mTroubleTarget;
    /* 0x23A */ u8 mTroubleState;        // While it is set, the character is out (mTroubleEnd, mTroubleStart count)
    /* 0x23B */ u8 mUnknown23B[0xA];
    /* 0x245 */ u8 mEatLastFood;        // aeb78 sets it to the item it picked
    /* 0x246 */ u8 mUnknown246[0x4F];
    /* 0x295 */ u8 mMood;              // 0..4
    /* 0x296 */ u8 mUnknown296[2];
    /* 0x298 */ u8 mRelationToPlayer;
    /* 0x299 */ u8 mUnknown299[0xB];
    /* 0x2A4 */ u16 mIndex;            // Which of the saved characters this is
    /* 0x2A6 */ u8 mUnknown2A6[0x1A];
}; // 0x2C0

// The game's clock (gGameClock points at it)
struct GameClock {
    /* 0x00 */ u8 mUnknown00[0x18];
    /* 0x18 */ s64 mSecondsAway;       // How long the game was off
    /* 0x20 */ u8 mUnknown20[0x14];
    /* 0x34 */ BOOL mOtherConsole;     // Not 0 when the console is another than the one last played on
};

struct OutingEntry {
    /* 0x0 */ u32 mUnknown0 : 8;
    /*     */ u32 mMinutes : 16;      // How long the outing lasts, one way
    /*     */ u32 mUnknown0Bits24 : 8;
    /* 0x4 */ u32 mUnknown4Bits0 : 16;
    /*     */ u32 mMinutes2 : 12;     // and the other
    /*     */ u32 mUnknown4Bits28 : 4;
    /* 0x8 */ int mUnknown8;
}; // 0xC

// What an event is (gEventTable, by event)
struct EventInfo {
    u32 mMinutes : 12; // How long it lasts
    u32 mUnknownBits12 : 20;
};

// What the item table (gGoodsTable, 0x14 bytes an item) says of an item, as far as is used here
struct ItemInfo {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ u32 mUnknown0CBits0 : 4;
    /*      */ u32 mKind : 3;
    /*      */ u32 mUnknown0CBits7 : 25;
    /* 0x10 */ u8 mUnknown10[4];
}; // 0x14


extern "C" {
extern GameClock *gGameClock;
extern MATHRandContext16 gRandom;
extern const EventInfo gEventTable[];    // By event
extern const s8 data_020f8ca8[];           // By month: an adjustment in hours
extern const u8 data_020f8f5c[];           // By number of residents
extern const OutingEntry *const data_02114d78[];
extern const s8 *const data_021135a0[];    // By relation: changes of affinity
extern const ItemInfo gGoodsTable[];
s64 Clock_GetSeconds(GameClock *clock);
s64 func_02035ae0(GameClock *clock);
BOOL GameData_CharacterExists(GameData *game, int index);
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character);
u8 GameData_GetCharacterEvent(GameData *game, int index);
u8 GameData_GetCharacterEventPartner(GameData *game, int index);
s64 GameData_GetCharacterEventEnd(GameData *game, int index);
s64 GameData_GetCharacterAwayUntil(GameData *game, int index);
s64 GameData_GetCharacterTroubleStart(GameData *game, int index);
u8 GameData_GetCharacterTroubleState(GameData *game, int index);
s64 GameData_GetCharacterTroubleProgress(GameData *game, int index);
u8 GameData_GetCharacterTroubleId(GameData *game, int index);
u8 GameData_GetCharacterTroubleCategory(GameData *game, int index);
u8 func_0202c748(GameData *game, int index);
BOOL GameData_IsCharacterSlotUsed(GameData *game, int index);
int MiiData_GetSex(SavedCharacter *character);
}

#define NO_TIME 0x7FFFFFFFFFFFFFFFLL

// The random number comes back as an int: the original truncates it to 16 bits a second time where it is stored in
// a u16 (MeetRandomPair)
static inline int Rand(u16 max) {
    return MATH_Rand16(&gRandom, max);
}

static inline void Swap(u16 &a, u16 &b) {
    u16 t = a;
    a = b;
    b = t;
}

int GetRandomEventCount(int *force);
int ListRandomEventCandidates(u16 *indices);
void ShuffleRandomEventCandidates(u16 *indices);
int GetRelationAfter(int result);

// Starts random events for as many of the residents as GetRandomEventCount says, in a random order
void StartRandomEvents(s64 now) {
    u16 indices[100];
    int force;
    int count = GetRandomEventCount(&force);
    if (count <= 0) {
        return;
    }
    ShuffleRandomEventCandidates(indices);
    for (int i = 0; i < count; i++) {
        SavedCharacter character;
        SavedCharacter partner;
        BOOL started;
        GameData_ReadCharacter(&gGameData, indices[i], &character);
        character.mIndex = indices[i];
        if (character.IsLongActPaired()) {
            GameData_ReadCharacter(&gGameData, character.mLongActPartner, &partner);
            partner.mIndex = character.mLongActPartner;
            started = character.StartRandomTrouble(now, force, &partner, TRUE);
            GameData_WriteCharacter(&gGameData, partner.mIndex, &partner);
        } else {
            started = character.StartRandomTrouble(now, force, NULL, TRUE);
        }
        if (started) {
            if (character.IsAngryAfterFight()) {
                SavedCharacter other;
                SavedCharacter otherPartner;
                int otherIndex = character.mTroubleTarget;
                GameData_ReadCharacter(&gGameData, otherIndex, &other);
                other.mIndex = otherIndex;
                if (other.IsLongActPaired()) {
                    if (other.mLongActPartner != character.mIndex) {
                        GameData_ReadCharacter(&gGameData, other.mLongActPartner, &otherPartner);
                        otherPartner.mIndex = other.mLongActPartner;
                        other.BecomeAngryAt(&character, now, &otherPartner);
                        GameData_WriteCharacter(&gGameData, otherPartner.mIndex, &otherPartner);
                    } else {
                        other.BecomeAngryAt(&character, now, NULL);
                    }
                } else {
                    other.BecomeAngryAt(&character, now, NULL);
                }
                u8 relation = GetRelationAfter(character.mRelationRank[other.mIndex]);
                character.mRelationRank[other.mIndex] = relation;
                other.mRelationRank[character.mIndex] = relation;
                GameData_WriteCharacter(&gGameData, other.mIndex, &other);
            }
            GameData_WriteCharacter(&gGameData, character.mIndex, &character);
        }
    }
}

// For every resident whose event has ended by now, the next one
void StartNextEvents(s64 now, RTCTime *time) {
    StartRandomEvents(now);
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i) && now >= GameData_GetCharacterEventEnd(&gGameData, i)) {
            SavedCharacter character;
            GameData_ReadCharacter(&gGameData, i, &character);
            character.mIndex = i;
            character.StartLongAct(character.ChooseLongAct(), now, time, NULL, NULL);
        }
    }
}

// Whether the outing of the resident (state 4) is over by now
BOOL IsOutingOver(int index, s64 now) {
    if (GameData_GetCharacterTroubleCategory(&gGameData, index) == 4) {
        s64 start;
        int length;
        u8 kind = GameData_GetCharacterTroubleState(&gGameData, index);
        BOOL check = FALSE;
        if (kind == 2) {
            start = GameData_GetCharacterTroubleStart(&gGameData, index);
            length = GameData_GetCharacterTroubleProgress(&gGameData, index);
            check = TRUE;
        } else if (kind == 1) {
            start = now;
            length = GameData_GetCharacterTroubleProgress(&gGameData, index);
            check = TRUE;
        }
        if (check && SavedCharacter::IsTroubleProgressFullAt(now, start, length)) {
            return TRUE;
        }
    }
    return FALSE;
}

void UpdateResident(int index, s64 now, RTCTime *time) {
    if (now >= GameData_GetCharacterEventEnd(&gGameData, index)) {
        SavedCharacter character;
        GameData_ReadCharacter(&gGameData, index, &character);
        character.mIndex = index;
        character.EndLongAct(NULL, NULL);
    }
    if (now >= GameData_GetCharacterAwayUntil(&gGameData, index)) {
        SavedCharacter character;
        GameData_ReadCharacter(&gGameData, index, &character);
        character.mIndex = index;
        character.ExpireTrouble(now, NULL);
        return;
    }
    if (!IsOutingOver(index, now)) {
        return;
    }
    SavedCharacter character;
    GameData_ReadCharacter(&gGameData, index, &character);
    character.mIndex = index;
    character.EndTrouble(now);
}

void UpdateResidents(s64 now, RTCTime *time) {
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            UpdateResident(i, now, time);
        }
    }
}

// Brings each resident's event end and return from an outing back within their lengths from now (after the
// clock was put back).
// 97%: the instructions are the original's, the registers a rotation: the original has gGameData's address in r5, i
// in r6 and the event's length (later the outing's table and limit) in r7/r8, where this gives the length r5. Tried:
// i declared at the top, a GameData pointer (plain and cast), an int or a separate minutes local for the length,
// nested ifs instead of continue, and the pragma sets (level 2, common subs, propagation, the eight passes,
// lifetimes, loop invariants, strength reduction).
void LimitEventTimes(s64 now) {
    RTCDate date;
    RTCTime time;
    RTC_ConvertSecondToDateTime(&date, &time, now);
    for (int i = 0; i < 100; i++) {
        if (!GameData_CharacterExists(&gGameData, i)) {
            continue;
        }
        s64 end = GameData_GetCharacterEventEnd(&gGameData, i);
        if (end != NO_TIME) {
            s64 length = gEventTable[GameData_GetCharacterEvent(&gGameData, i)].mMinutes * 60;
            if (now + length * 2 < end) {
                SavedCharacter character;
                GameData_ReadCharacter(&gGameData, i, &character);
                character.mIndex = (u8)i;
                s64 newEnd = now + length;
                if (character.mLongActState == 0x12) {
                    newEnd += (s64)data_020f8ca8[time.hour] * 3600;
                }
                character.mLongActEnd = newEnd;
                GameData_WriteCharacter(&gGameData, i, &character);
            }
        }
        if (GameData_GetCharacterTroubleState(&gGameData, i) == 0) {
            continue;
        }
        s64 away = GameData_GetCharacterAwayUntil(&gGameData, i);
        if (away == NO_TIME) {
            continue;
        }
        u8 table = GameData_GetCharacterTroubleCategory(&gGameData, i);
        u8 row = GameData_GetCharacterTroubleId(&gGameData, i);
        u32 minutes;
        if (SavedCharacter::IsTroubleStateActive(GameData_GetCharacterTroubleState(&gGameData, i))) {
            minutes = data_02114d78[table][row].mMinutes;
        } else {
            minutes = data_02114d78[table][row].mMinutes2;
        }
        s64 limit = now + (u64)minutes * 60;
        if (limit < away) {
            SavedCharacter character;
            GameData_ReadCharacter(&gGameData, i, &character);
            character.mTroubleEnd = limit;
            character.mTroubleStart = now;
            character.mIndex = (u8)i;
            GameData_WriteCharacter(&gGameData, i, &character);
        }
    }
}

#define CENTURY 3155760000LL // 100 years of 365.25 days, in seconds

// Moves each resident's event end and outing by the change of the game's clock offset (func_02035ae0), and keeps
// them within their lengths from now; on another console than the last one, only the latter
void ShiftEventTimes(s64 now) {
    RTCDate date;
    RTCTime time;
    s64 offset;
    RTC_ConvertSecondToDateTime(&date, &time, now);
    BOOL otherConsole = gGameClock->mOtherConsole;
    if (otherConsole) {
        offset = 0;
    } else {
        offset = func_02035ae0(gGameClock);
        s64 diff = now - offset;
        if (diff > CENTURY - 1) {
            if (offset < 0) {
                do {
                    offset += CENTURY;
                } while (offset < 0);
            }
        } else if (diff < 0 && offset > 0) {
            do {
                offset -= CENTURY - 1;
            } while (offset > 0);
        }
    }
    if (offset == 0 && !otherConsole) {
        return;
    }
    for (int i = 0; i < 100; i++) {
        if (!GameData_CharacterExists(&gGameData, i)) {
            continue;
        }
        SavedCharacter character;
        GameData_ReadCharacter(&gGameData, i, &character);
        character.mIndex = (u8)i;
        if (character.mLongActEnd != NO_TIME) {
            s64 end = offset + character.mLongActEnd;
            if (end < 0) {
                do {
                    end += CENTURY;
                } while (end < 0);
            }
            s64 length = gEventTable[character.mLongActState].mMinutes * 60;
            if (now + length * 2 < end || otherConsole) {
                end = now + length;
                if (character.mLongActState == 0x12) {
                    end += (s64)data_020f8ca8[time.hour] * 3600;
                }
            }
            character.mLongActEnd = end;
        }
        if (character.mTroubleState != 0 && character.mTroubleEnd != NO_TIME) {
            s64 start = offset + character.mTroubleStart;
            s64 away = offset + character.mTroubleEnd;
            if (away < 0) {
                do {
                    away += CENTURY;
                } while (away < 0);
            }
            u32 seconds;
            if (character.IsInTrouble()) {
                seconds = data_02114d78[character.mTroubleCategory][character.mTroubleId].mMinutes * 60;
            } else {
                seconds = data_02114d78[character.mTroubleCategory][character.mTroubleId].mMinutes2 * 60;
            }
            s64 limit = now + seconds;
            if (limit < away || otherConsole) {
                away = limit;
                start = now;
            }
            character.mTroubleEnd = away;
            character.mTroubleStart = start;
        }
        GameData_WriteCharacter(&gGameData, i, &character);
    }
}

// How many residents may start a random event now: a number by the count of residents (data_020f8f5c) less those
// that are out or have nothing planned; *force is set when there are none or when it is at least eight tenths
int GetRandomEventCount(int *force) {
    int residents = 0;
    int busy = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            residents++;
            if (GameData_GetCharacterTroubleState(&gGameData, i) != 0 || GameData_GetCharacterAwayUntil(&gGameData, i) == NO_TIME) {
                busy++;
            }
        }
    }
    int count = data_020f8f5c[residents] - busy;
    *force = FALSE;
    if (residents == 0) {
        *force = TRUE;
    } else if (count * 10 / residents >= 8) {
        *force = TRUE;
    }
    return count;
}

// Lists the residents that may start a random event, and returns how many
int ListRandomEventCandidates(u16 *indices) {
    int count = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i) && GameData_GetCharacterTroubleState(&gGameData, i) == 0 &&
            GameData_GetCharacterAwayUntil(&gGameData, i) != NO_TIME) {
            indices[count] = i;
            count++;
        }
    }
    return count;
}

void ShuffleRandomEventCandidates(u16 *indices) {
    u16 count = ListRandomEventCandidates(indices);
    for (int i = 0; i < 200; i++) {
        Swap(indices[Rand(count)], indices[Rand(count)]);
    }
}

// --- EncounterJudge ---------------------------------------------------------------------------------------------

// How often two of the residents meet while the game is off: a meeting every 3000 / pairs minutes
int EncounterJudge::GetMeetingInterval(int count) {
    int pairs = (count * (count - 1)) >> 1;
    if (pairs <= 0) {
        return 0;
    }
    if (pairs > 3000) {
        pairs = 3000;
    }
    return 3000 / pairs;
}

// How many meetings the time the game was off makes, up to 500
int EncounterJudge::GetMeetingsWhileAway(int count) {
    s64 minutes = gGameClock->mSecondsAway / 60;
    int interval = GetMeetingInterval(count);
    if (interval == 0) {
        return 0;
    }
    s64 meetings = minutes / interval;
    if (meetings > 500) {
        meetings = 500;
    }
    return meetings;
}

// The change of affinity a meeting brings: a random one of the relation's eight
int EncounterJudge::GetMeetingChange(SavedCharacter *first, SavedCharacter *second) {
    u8 relation = first->mRelationRank[second->mIndex];
    return data_021135a0[relation][Rand(8)] * 2;
}

// Counts the other residents by their relation to the two (counts[0..9] for the first, counts[10..19] for the
// second); *a counts those of whom SavedCharacter::IsAngryAfterFight/IsAngryAfterFightState hold, *b those both have a
// relation with that SavedCharacter::IsFriendlyOrFightingRelation holds for. Returns the number of residents.
int EncounterJudge::CountAcquaintances(SavedCharacter *first, SavedCharacter *second, u8 *counts, s16 *a, s16 *b) {
    MI_CpuFill8(counts, 0, 20);
    *a = 0;
    int residents = 0;
    *b = 0;
    for (int i = 0; i < 100; i++) {
        if (!GameData_CharacterExists(&gGameData, i)) {
            continue;
        }
        residents++;
        if (i != first->mIndex) {
            counts[first->mRelationRank[i]]++;
        }
        if (i != second->mIndex) {
            counts[10 + second->mRelationRank[i]]++;
        }
        if (i == first->mIndex) {
            if (first->IsAngryAfterFight()) {
                (*a)++;
            }
        } else if (i == second->mIndex) {
            if (second->IsAngryAfterFight()) {
                (*a)++;
            }
        } else {
            if (SavedCharacter::IsAngryAfterFightState(GameData_GetCharacterTroubleState(&gGameData, i), GameData_GetCharacterTroubleCategory(&gGameData, i),
                              GameData_GetCharacterTroubleId(&gGameData, i))) {
                (*a)++;
            }
            if (first->IsFriendlyOrFightingRelation(first->mRelationRank[i]) && second->IsFriendlyOrFightingRelation(second->mRelationRank[i])) {
                (*b)++;
            }
        }
    }
    return residents;
}

// What comes of the two meeting when they like each other (both affinities at least 200): a result by their
// relation, 10 when nothing
int EncounterJudge::Judge(SavedCharacter *first, SavedCharacter *second) {
    u8 counts[20];
    s16 a;
    s16 b;
    int firstIndex = first->mIndex;
    int secondIndex = second->mIndex;
    int result = 10;
    u8 relation = first->mRelationRank[secondIndex];
    if (firstIndex == secondIndex || first->mRelationMeter[secondIndex] < 200 || second->mRelationMeter[firstIndex] < 200) {
        return 10;
    }
    CountAcquaintances(first, second, counts, &a, &b);
    switch (relation) {
    case 0:
        if (b > 0) {
            result = 1;
        }
        break;
    case 1:
        int sex = MiiData_GetSex(second);
        int firstSex = MiiData_GetSex(first);
        if (firstSex == sex || counts[2] != 0 || counts[12] != 0 || counts[7] != 0 ||
            counts[17] != 0 || counts[4] != 0 || counts[14] != 0 || counts[8] != 0 || counts[18] != 0) {
            if (counts[9] == 0 && counts[19] == 0) {
                result = 9;
            }
        }
        break;
    case 2:
        int sex2 = MiiData_GetSex(second);
        int sex1 = MiiData_GetSex(first);
        if (sex1 == sex2) {
            break;
        }
        if (counts[4] != 0 || counts[14] != 0 || counts[8] != 0 || counts[18] != 0) {
            break;
        }
        if (first->IsTroubleOfCategory34OrKind20() || second->IsTroubleOfCategory34OrKind20()) {
            break;
        }
        if (SavedCharacter::GetRelationBloodPairValue(first->mRelationToPlayer, second->mRelationToPlayer)) {
            result = 4;
        }
        break;
    case 3:
        result = 1;
        break;
    case 5:
        result = 1;
        break;
    case 6:
        if (!first->IsTroubleOfCategory34OrKind20() && !second->IsTroubleOfCategory34OrKind20()) {
            result = 1;
        }
        break;
    case 7:
        if (!first->IsTroubleOfCategory34OrKind20() && !second->IsTroubleOfCategory34OrKind20()) {
            result = 2;
        }
        break;
    case 8:
        if (!first->IsTroubleOfCategory34OrKind20() && !second->IsTroubleOfCategory34OrKind20()) {
            result = 4;
        }
        break;
    case 9:
        if (MiiData_GetSex(first) == MiiData_GetSex(second)) {
        }
        break;
    }
    return result;
}

// What comes of the two meeting badly: 10 when nothing
int EncounterJudge::JudgeEnd(SavedCharacter *first, SavedCharacter *second, BOOL check) {
    u8 counts[20];
    s16 a;
    s16 b;
    u16 secondIndex = second->mIndex;
    u16 firstIndex = first->mIndex;
    u8 relation = first->mRelationRank[secondIndex];
    if (check) {
        if (firstIndex == secondIndex || first->mRelationMeter[second->mIndex] != 0 ||
            second->mRelationMeter[first->mIndex] != 0) {
            return 10;
        }
    }
    int result = GetRelationAfter(relation);
    if (SavedCharacter::IsFightingRelation(result)) {
        int residents = CountAcquaintances(first, second, counts, &a, &b);
        if (first->IsTroubleOfCategory34OrKind20() || second->IsTroubleOfCategory34OrKind20()) {
            result = 10;
        } else if (counts[6] != 0 || counts[16] != 0 || counts[7] != 0 || counts[17] != 0 || counts[8] != 0 ||
                   counts[18] != 0) {
            result = 10;
        } else if (SavedCharacter::GetFightsAllowed(a, residents) <= 1) {
            result = 10;
        }
    }
    return result;
}

// The relation that a result leads to (10: none)
int GetRelationAfter(int result) {
    int relation = 10;
    switch (result) {
    case 1:
        relation = 6;
        break;
    case 2:
        relation = 7;
        break;
    case 4:
        relation = 8;
        break;
    case 7:
        relation = 3;
        break;
    case 8:
        relation = 5;
        break;
    case 9:
        relation = 1;
        break;
    }
    return relation;
}

// The two meet: their affinities change, and a good or bad meeting may change their relation
void EncounterJudge::Meet(SavedCharacter *first, SavedCharacter *second, int change) {
    int firstIndex = first->mIndex;
    int secondIndex = second->mIndex;
    first->AddRelationMeter(secondIndex, change);
    second->AddRelationMeter(firstIndex, change);
    int result = 10;
    if (first->mMood == 3 || second->mMood == 3) {
        result = 10;
    } else if (change > 0) {
        result = Judge(first, second);
    } else if (change < 0) {
        result = JudgeEnd(first, second, TRUE);
    }
    if (result != 10) {
        u8 relation = first->mRelationRank[secondIndex];
        if (change > 0) {
            first->SetRelation(secondIndex, result, 100);
            second->SetRelation(firstIndex, result, 100);
            first->SetMoodFromResult(result, relation);
            second->SetMoodFromResult(result, relation);
            Apply(first, second, relation, Clock_GetSeconds(gGameClock));
        } else if (change < 0) {
            first->SetRelation(secondIndex, result, 100);
            second->SetRelation(firstIndex, result, 100);
            first->SetMoodFromResult(result, relation);
            second->SetMoodFromResult(result, relation);
            ApplyEnd(first, second, relation, Clock_GetSeconds(gGameClock), TRUE);
        }
    }
    if (change == 0 && result == 10) {
        return;
    }
    GameData_WriteCharacter(&gGameData, firstIndex, first);
    GameData_WriteCharacter(&gGameData, secondIndex, second);
}

// The two go out together (trouble state 1 of the category and id) for between three quarters and all of the
// trouble's length. Both are u8, as SetTrouble takes them: the u8 on the stack is then read from its slot at each
// call, as the original does.
void EncounterJudge::StartOuting(SavedCharacter *first, SavedCharacter *second, u8 category, u8 id, s64 now) {
    first->ClearTrouble();
    first->SetTrouble(1, category, id);
    first->mTroubleTarget = second->mIndex;
    int minutes = data_02114d78[first->mTroubleCategory][first->mTroubleId].mMinutes;
    int least = (minutes * 3) >> 2;
    int length = least + Rand(minutes - least) + 1;
    s64 away = now + length * 60;
    first->mTroubleEnd = away;
    second->ClearTrouble();
    second->SetTrouble(1, category, id);
    second->mTroubleTarget = first->mIndex;
    second->mTroubleEnd = away;
}

// Sets the end of the two's events, and of their partners' when asked
void EncounterJudge::SetEventEnds(SavedCharacter *first, SavedCharacter *second, s64 end, BOOL partners) {
    first->mLongActEnd = end;
    second->mLongActEnd = end;
    if (!partners) {
        return;
    }
    if (first->IsLongActPaired()) {
        u8 index = first->mLongActPartner;
        if (index != second->mIndex) {
            SavedCharacter partner;
            GameData_ReadCharacter(&gGameData, index, &partner);
            partner.mLongActEnd = end;
            GameData_WriteCharacter(&gGameData, index, &partner);
        }
    }
    if (!second->IsLongActPaired()) {
        return;
    }
    u8 index = second->mLongActPartner;
    if (index == first->mIndex) {
        return;
    }
    SavedCharacter partner;
    GameData_ReadCharacter(&gGameData, index, &partner);
    partner.mLongActEnd = end;
    GameData_WriteCharacter(&gGameData, index, &partner);
}

void EncounterJudge::Apply(SavedCharacter *first, SavedCharacter *second, int relation, s64 now) {
    if (first->mRelationRank[second->mIndex] != 4) {
        return;
    }
    if (relation == 8) {
        return;
    }
    StartOuting(first, second, 3, 2, now);
    first->mMood = 1;
    second->mMood = 1;
    if (!first->IsLongActPaired()) {
        first->mLongActEnd = now;
    }
    if (!second->IsLongActPaired()) {
        second->mLongActEnd = now;
    }
}

void EncounterJudge::ApplyEnd(SavedCharacter *first, SavedCharacter *second, int relation, s64 now, int param) {
    u8 current = first->mRelationRank[second->mIndex];
    if (current == 5) {
        first->ClearTrouble();
        second->ClearTrouble();
        first->mMood = 0;
        second->mMood = 0;
    } else if (current == 3) {
        first->ClearTrouble();
        second->ClearTrouble();
        first->mMood = 0;
        second->mMood = 0;
    } else if (SavedCharacter::IsFightingRelation(current)) {
        StartOuting(first, second, 3, 4, now);
        first->mMood = 2;
        second->mMood = 2;
    }
    SetEventEnds(first, second, now, param);
}

// Two different residents of the list chosen at random meet
void EncounterJudge::MeetRandomPair(int count, u8 *residents) {
    if (count < 2) {
        return;
    }
    u16 a;
    u16 b;
    do {
        a = Rand(count);
        b = Rand(count);
    } while (a == b);
    SavedCharacter characters[2];
    characters[0].mIndex = residents[a];
    characters[1].mIndex = residents[b];
    GameData_ReadCharacter(&gGameData, characters[0].mIndex, &characters[0]);
    GameData_ReadCharacter(&gGameData, characters[1].mIndex, &characters[1]);
    Meet(&characters[0], &characters[1], GetMeetingChange(&characters[0], &characters[1]));
}

// The meetings of the time the game was off
void EncounterJudge::MeetWhileAway(int count, u8 *residents) {
    int meetings = GetMeetingsWhileAway(count);
    for (int i = 0; i < meetings; i++) {
        MeetRandomPair(count, residents);
    }
}

// While the game was off, residents (half of them for every day, at most half) pick an item of kind 1
void EncounterJudge::PickItemsWhileAway(int count, u8 *residents, s64 now) {
    int items[0xFF];
    int half = count / 2;
    int times = half * (int)(gGameClock->mSecondsAway / 3600) / 24;
    times = MATH_CLAMP(times, 0, half);
    int itemCount = 0;
    for (int i = 0; i < 0xFF; i++) {
        if (gGoodsTable[(u16)i].mKind == 1) {
            items[itemCount] = i;
            itemCount++;
        }
    }
    for (int j = 0; j < times; j++) {
        PickItem(residents[Rand(count)], items, itemCount, now);
    }
}

void EncounterJudge::PickItem(int index, int *items, int itemCount, s64 now) {
    BOOL busy = FALSE;
    if (GameData_GetCharacterTroubleCategory(&gGameData, index) == 0 &&
        (GameData_GetCharacterTroubleState(&gGameData, index) == 1 || GameData_GetCharacterTroubleState(&gGameData, index) == 2)) {
        busy = TRUE;
    }
    if (busy) {
        return;
    }
    SavedCharacter character;
    GameData_ReadCharacter(&gGameData, index, &character);
    character.mIndex = index;
    if (character.GetFullness(now) > 0) {
        return;
    }
    int item = items[Rand(itemCount)];
    character.Eat(now - Rand(7200), item);
    character.mEatLastFood = item;
    GameData_WriteCharacter(&gGameData, index, &character);
}

// While the game was off, residents (a fifth of them for every day, at most a fifth) change their clothes
void EncounterJudge::ChangeClothesWhileAway(int count, u8 *residents) {
    int fifth = count / 5;
    int times = fifth * (int)(gGameClock->mSecondsAway / 3600) / 24;
    times = MATH_CLAMP(times, 0, fifth);
    for (int j = 0; j < times; j++) {
        ChangeClothes(residents[Rand(count)]);
    }
}

// The resident puts on clothes it has seen, chosen at random with three chances for those it likes most
// (GetClothesLiking at least 13), two for 10 and one for 6
void EncounterJudge::ChangeClothes(int index) {
    SavedCharacter character;
    u16 choices[1800];
    int count = 0;
    GameData_ReadCharacter(&gGameData, index, &character);
    character.mIndex = index;
    for (int i = 1; i < 300; i++) {
        if ((character.mOwnedClothes[i >> 5] >> (i & 31)) & 1) {
            if (character.GetClothesLiking(i) >= 13) {
                choices[count] = i;
                choices[count + 1] = i;
                choices[count + 2] = i;
                count += 3;
            } else if (character.GetClothesLiking(i) >= 10) {
                choices[count] = i;
                choices[count + 1] = i;
                count += 2;
            } else if (character.GetClothesLiking(i) >= 6) {
                choices[count] = i;
                count++;
            }
        }
    }
    if (count <= 0) {
        return;
    }
    character.mClothesId = choices[Rand(count)];
    GameData_WriteCharacter(&gGameData, index, &character);
}

// Clears byte 0x24E of every resident that has it set (SavedCharacter::ResetUnknown24E sets it to 0xFF)
void EncounterJudge::ClearUnknown24E() {
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i) && func_0202c748(&gGameData, i) != 0xFF) {
            SavedCharacter character;
            GameData_ReadCharacter(&gGameData, i, &character);
            character.mIndex = i;
            character.ResetUnknown24E();
            GameData_WriteCharacter(&gGameData, i, &character);
        }
    }
}

// Gives a fifth of the residents and one more a random byte 0x24E of 0 to 5 (SavedCharacter::RandomizeUnknown24E)
void EncounterJudge::SetRandomUnknown24E(int count, u8 *residents) {
    int times = MATH_CLAMP(count / 5 + 1, 0, count);
    for (int j = 0; j < times; j++) {
        int index = residents[Rand(count)];
        SavedCharacter character;
        GameData_ReadCharacter(&gGameData, index, &character);
        character.mIndex = index;
        character.RandomizeUnknown24E();
        GameData_WriteCharacter(&gGameData, index, &character);
    }
}
