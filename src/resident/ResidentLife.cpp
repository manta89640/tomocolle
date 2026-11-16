#include <nitro.h>

#include "GameData.h"

// What the residents do by themselves: the events that start and end with time, what came of the time the game was
// off (overlay 106's PreStartScene::UpdateResidents calls the functions in turn), and the encounters of two
// characters (EncounterJudge, which overlay 74's apartment scene holds and calls too).
//
// The names are descriptive, not the original: no name of these functions survives in the ROM.

// A saved character as these functions use it (the main program's class, 0x2C0 bytes; func_0202c264 copies the
// saved game's record into one, func_0202c28c back). The fields are named as the character headers name them.
class SavedCharacter {
public:
    SavedCharacter();

    /* 0x000 */ u8 mUnknown000[0x54];
    /* 0x054 */ u32 mUnknown054Bits0 : 11;
    /*       */ u32 mClothesId : 9;    // The clothes it wears (row of the clothes table)
    /*       */ u32 mUnknown054Bits20 : 12;
    /* 0x058 */ u8 mUnknown058[0xA4];
    /* 0x0FC */ u8 mAffinity[100];     // For each other character by index
    /* 0x160 */ u8 mRelation[100];     // For each other character by index
    /* 0x1C4 */ u8 mUnknown1C4[0x24];
    /* 0x1E8 */ u32 mClothesSeen[10];  // A bit for each of the 300 clothes
    /* 0x210 */ u8 mUnknown210[0xC];
    /* 0x21C */ s64 mEventEnd;         // A time in seconds; 0x7FFFFFFFFFFFFFFF for none
    /* 0x224 */ u8 mEvent;
    /* 0x225 */ u8 mEventPartner;      // mIndex of the character it is doing it with
    /* 0x226 */ u8 mOutingIndex;       // Row and table of data_02114d78 for the time it is out
    /* 0x227 */ u8 mOutingTable;
    /* 0x228 */ s64 mOutingStart;
    /* 0x230 */ s64 mAwayUntil;
    /* 0x238 */ u16 mMoodPartner;
    /* 0x23A */ u8 mMoodLocked;        // While it is set, the character is out (mAwayUntil, mOutingStart count)
    /* 0x23B */ u8 mUnknown23B[0xA];
    /* 0x245 */ u8 mUnknown245;        // aeb78 sets it to the item it picked
    /* 0x246 */ u8 mUnknown246[0x4F];
    /* 0x295 */ u8 mMood;              // 0..4
    /* 0x296 */ u8 mUnknown296[2];
    /* 0x298 */ u8 mRelationToPlayer;
    /* 0x299 */ u8 mUnknown299[0xB];
    /* 0x2A4 */ u16 mIndex;            // Which of the saved characters this is
    /* 0x2A6 */ u8 mUnknown2A6[0x1A];
}; // 0x2C0

// The game's clock (data_02123440 points at it)
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

// What the item table (data_020f5f0c, 0x14 bytes an item) says of an item, as far as is used here
struct ItemInfo {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ u32 mUnknown0CBits0 : 4;
    /*      */ u32 mKind : 3;
    /*      */ u32 mUnknown0CBits7 : 25;
    /* 0x10 */ u8 mUnknown10[4];
}; // 0x14

class EncounterJudge {
public:
    int CountAcquaintances(SavedCharacter *first, SavedCharacter *second, u8 *counts, s16 *a, s16 *b);
    int Judge(SavedCharacter *first, SavedCharacter *second);
    int JudgeEnd(SavedCharacter *first, SavedCharacter *second, BOOL check);
    void Meet(SavedCharacter *first, SavedCharacter *second, int change);
    void StartOuting(SavedCharacter *first, SavedCharacter *second, int event, int kind, s64 now);
    void SetEventEnds(SavedCharacter *first, SavedCharacter *second, s64 end, BOOL partners);
    void Apply(SavedCharacter *first, SavedCharacter *second, int relation, s64 now);
    void ApplyEnd(SavedCharacter *first, SavedCharacter *second, int relation, s64 now, int param);
    int GetMeetingsWhileAway(int count);
    int GetMeetingChange(SavedCharacter *first, SavedCharacter *second);
    void MeetRandomPair(int count, u8 *residents);
    void MeetWhileAway(int count, u8 *residents);
    void PickItemsWhileAway(int count, u8 *residents, s64 now);
    void PickItem(int index, int *items, int itemCount, s64 now);
    void ChangeClothesWhileAway(int count, u8 *residents);
    void ChangeClothes(int index);
    void UpdateUnknownD310();
    void UpdateUnknownD2E0(int count, u8 *residents);

    u8 mUnknown[4];
};

extern "C" {
extern GameClock *data_02123440;
extern MATHRandContext16 gRandom;
extern const u32 data_020f8cc0[];          // By event: how long it lasts, in minutes (low 12 bits)
extern const s8 data_020f8ca8[];           // By month: an adjustment in hours
extern const u8 data_020f8f5c[];           // By number of residents
extern const OutingEntry *const data_02114d78[];
extern const s8 *const data_021135a0[];    // By relation: changes of affinity
extern const ItemInfo data_020f5f0c[];
s64 Clock_GetSeconds(GameClock *clock);
s64 func_02035ae0(GameClock *clock);
BOOL func_0202c3dc(GameData *game, int index);
void func_0202c264(GameData *game, int index, SavedCharacter *character);
void func_0202c28c(GameData *game, int index, SavedCharacter *character);
u8 func_0202c468(GameData *game, int index);
u8 func_0202c490(GameData *game, int index);
s64 func_0202c4b8(GameData *game, int index);
s64 func_0202c4e4(GameData *game, int index);
s64 func_0202c510(GameData *game, int index);
u8 func_0202c53c(GameData *game, int index);
s64 func_0202c564(GameData *game, int index);
u8 func_0202c58c(GameData *game, int index);
u8 func_0202c5b4(GameData *game, int index);
u8 func_0202c748(GameData *game, int index);
BOOL func_0202c354(GameData *game, int index);
int MiiData_GetSex(SavedCharacter *character);
void func_02098fe0(SavedCharacter *character, int result, int relation);
void func_02099088(SavedCharacter *character, int other, int change);
void func_020990b0(SavedCharacter *character, int other, u8 relation, int value);
BOOL func_0209926c(int relation);
BOOL func_020992ac(SavedCharacter *character, int relation);
void func_02099384(SavedCharacter *character, int a, int b);
void func_0209944c(SavedCharacter *character, int event, s64 now, RTCTime *time, int a, int b);
int func_02099cdc(SavedCharacter *character);
int func_0209a10c(SavedCharacter *character);
BOOL func_0209a190(SavedCharacter *character);
BOOL func_0209a1a0(int kind);
void func_0209a1fc(SavedCharacter *character, int a, int b, int c);
BOOL func_0209a3b4(s64 now, s64 start, int end);
BOOL func_0209a3ec(SavedCharacter *character);
BOOL func_0209a518(SavedCharacter *character);
BOOL func_0209a534(int a, int b, int c);
int func_0209a554(int a, int b);
void func_0209a588(SavedCharacter *character, s64 now, int a);
BOOL func_0209b14c(SavedCharacter *character, s64 now);
void func_0209b1d8(SavedCharacter *character);
BOOL func_0209b208(SavedCharacter *character, s64 now, int force, SavedCharacter *partner, BOOL a);
void func_0209be98(SavedCharacter *character, SavedCharacter *other, s64 now, SavedCharacter *partner);
s64 func_0209c4b4(SavedCharacter *character, s64 now);
void func_0209c56c(SavedCharacter *character, s64 time, u16 item);
int func_0209c89c(SavedCharacter *character, int clothes);
BOOL func_0209d27c(u8 a, u8 b);
void func_0209d2e0(SavedCharacter *character);
void func_0209d310(SavedCharacter *character);
}

#define NO_TIME 0x7FFFFFFFFFFFFFFFLL

static inline u16 Rand(u16 max) {
    return MATH_Rand16(&gRandom, max);
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
        func_0202c264(&gGameData, indices[i], &character);
        character.mIndex = indices[i];
        if (func_02099cdc(&character)) {
            func_0202c264(&gGameData, character.mEventPartner, &partner);
            partner.mIndex = character.mEventPartner;
            started = func_0209b208(&character, now, force, &partner, TRUE);
            func_0202c28c(&gGameData, partner.mIndex, &partner);
        } else {
            started = func_0209b208(&character, now, force, NULL, TRUE);
        }
        if (started) {
            if (func_0209a518(&character)) {
                SavedCharacter other;
                SavedCharacter otherPartner;
                int otherIndex = character.mMoodPartner;
                func_0202c264(&gGameData, otherIndex, &other);
                other.mIndex = otherIndex;
                if (func_02099cdc(&other)) {
                    if (other.mEventPartner != character.mIndex) {
                        func_0202c264(&gGameData, other.mEventPartner, &otherPartner);
                        otherPartner.mIndex = other.mEventPartner;
                        func_0209be98(&other, &character, now, &otherPartner);
                        func_0202c28c(&gGameData, otherPartner.mIndex, &otherPartner);
                    } else {
                        func_0209be98(&other, &character, now, NULL);
                    }
                } else {
                    func_0209be98(&other, &character, now, NULL);
                }
                u8 relation = GetRelationAfter(character.mRelation[other.mIndex]);
                character.mRelation[other.mIndex] = relation;
                other.mRelation[character.mIndex] = relation;
                func_0202c28c(&gGameData, other.mIndex, &other);
            }
            func_0202c28c(&gGameData, character.mIndex, &character);
        }
    }
}

// For every resident whose event has ended by now, the next one
void StartNextEvents(s64 now, RTCTime *time) {
    StartRandomEvents(now);
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i) && now >= func_0202c4b8(&gGameData, i)) {
            SavedCharacter character;
            func_0202c264(&gGameData, i, &character);
            character.mIndex = i;
            func_0209944c(&character, func_0209a10c(&character), now, time, 0, 0);
        }
    }
}

// Whether the outing of the resident (state 4) is over by now
BOOL IsOutingOver(int index, s64 now) {
    if (func_0202c5b4(&gGameData, index) == 4) {
        s64 start;
        int length;
        u8 kind = func_0202c53c(&gGameData, index);
        BOOL check = FALSE;
        if (kind == 2) {
            start = func_0202c510(&gGameData, index);
            length = func_0202c564(&gGameData, index);
            check = TRUE;
        } else if (kind == 1) {
            start = now;
            length = func_0202c564(&gGameData, index);
            check = TRUE;
        }
        if (check && func_0209a3b4(now, start, length)) {
            return TRUE;
        }
    }
    return FALSE;
}

void UpdateResident(int index, s64 now, RTCTime *time) {
    if (now >= func_0202c4b8(&gGameData, index)) {
        SavedCharacter character;
        func_0202c264(&gGameData, index, &character);
        character.mIndex = index;
        func_02099384(&character, 0, 0);
    }
    if (now >= func_0202c4e4(&gGameData, index)) {
        SavedCharacter character;
        func_0202c264(&gGameData, index, &character);
        character.mIndex = index;
        func_0209a588(&character, now, 0);
        return;
    }
    if (!IsOutingOver(index, now)) {
        return;
    }
    SavedCharacter character;
    func_0202c264(&gGameData, index, &character);
    character.mIndex = index;
    func_0209b14c(&character, now);
}

void UpdateResidents(s64 now, RTCTime *time) {
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            UpdateResident(i, now, time);
        }
    }
}
