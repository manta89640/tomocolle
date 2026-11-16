#ifndef RESIDENT_RESIDENTLIFE_H
#define RESIDENT_RESIDENTLIFE_H

#include <nitro.h>

// What the residents do by themselves (src/resident/ResidentLife.cpp): the events that start and end with time,
// what came of the time the game was off (overlay 106's PreStartScene::UpdateResidents calls the functions in
// turn), and the encounters of two characters (EncounterJudge, which overlay 74's apartment scene holds and calls).
//
// The names are descriptive, not the original: no name of these functions survives in the ROM.

struct SavedCharacter;

void StartRandomEvents(s64 now);
void StartNextEvents(s64 now, RTCTime *time);
BOOL IsOutingOver(int index, s64 now);
void UpdateResident(int index, s64 now, RTCTime *time);
void UpdateResidents(s64 now, RTCTime *time);
void LimitEventTimes(s64 now);
void ShiftEventTimes(s64 now);
int GetRandomEventCount(int *force);         // How many residents may start a random event now
int ListRandomEventCandidates(u16 *indices); // Lists them, and returns how many
void ShuffleRandomEventCandidates(u16 *indices);
int GetRelationAfter(int result);

// What decides the encounters of two characters (it has no data)
class EncounterJudge {
public:
    int CountAcquaintances(SavedCharacter *first, SavedCharacter *second, u8 *counts, s16 *a, s16 *b);
    int Judge(SavedCharacter *first, SavedCharacter *second);
    int JudgeEnd(SavedCharacter *first, SavedCharacter *second, BOOL check);
    void Meet(SavedCharacter *first, SavedCharacter *second, int change);
    void StartOuting(SavedCharacter *first, SavedCharacter *second, int event, u8 kind, s64 now);
    void SetEventEnds(SavedCharacter *first, SavedCharacter *second, s64 end, BOOL partners);
    void Apply(SavedCharacter *first, SavedCharacter *second, int relation, s64 now);
    void ApplyEnd(SavedCharacter *first, SavedCharacter *second, int relation, s64 now, int param);
    int GetMeetingInterval(int count);
    int GetMeetingsWhileAway(int count);
    int GetMeetingChange(SavedCharacter *first, SavedCharacter *second);
    void MeetRandomPair(int count, u8 *residents);
    void MeetWhileAway(int count, u8 *residents);
    void PickItemsWhileAway(int count, u8 *residents, s64 now);
    void PickItem(int index, int *items, int itemCount, s64 now);
    void ChangeClothesWhileAway(int count, u8 *residents);
    void ChangeClothes(int index);
    void ClearUnknown24E();
    void SetRandomUnknown24E(int count, u8 *residents);

    u8 mUnknown[4];
};

#endif // RESIDENT_RESIDENTLIFE_H
