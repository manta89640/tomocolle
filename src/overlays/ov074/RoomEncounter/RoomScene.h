#ifndef OV074_ROOMSCENE_H
#define OV074_ROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "text/MessageText.h"
#include "RoomCharacter.h"

// The apartment scene (overlay 74, "ScenePlayRoom2"): a room with up to two characters in it. This is the part of
// it that the functions 0x0214e5f8 to 0x02152228 use, which decide what the characters do.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// What the main program's func_020adfbc, func_020ae1f8, func_020ae75c and func_020ae80c work on: they decide what
// comes of two characters meeting. Main-program class, not decompiled.
struct EncounterJudge {
    int Judge(RoomCharacter *first, RoomCharacter *second);                // func_020adfbc: 10 when nothing comes of it
    int JudgeEnd(RoomCharacter *first, RoomCharacter *second, BOOL param); // func_020ae1f8
    void Apply(RoomCharacter *first, RoomCharacter *second, int relation, s64 time); // func_020ae75c
    void ApplyEnd(RoomCharacter *first, RoomCharacter *second, int relation, s64 time, int param);                                           // func_020ae80c

    u8 mUnknown[4];
};

// The room's light and the building front on the touch screen (unit RoomObjects): only what this unit calls.
class RoomLight {
public:
    void SetState(int state); // 0x0216b8a8

    u8 mUnknown[0x1C];
};

class ApartmentFront {
public:
    void DrawWindow(int room); // 0x0216c1f0

    u8 mUnknown[0x200];
};

class RoomScene {
public:
    void ChooseEncounter();
    void ChooseReactionsQuiet();
    void ChooseReactions();
    void ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode);
    void ShowEventMessage();
    void ApplyEncounterResult(RoomCharacter *first, RoomCharacter *second, int result, int kind);
    int JudgeEncounterEnd(RoomCharacter *first, RoomCharacter *second);
    void StartEncounter(int nearer, int farther);
    void UpdateEncounter();
    void FinishEncounter();
    void ChooseTalkReaction(int index);
    void StartTalkReaction(int index, int reaction);
    void StartEvent(RoomCharacter *character, int phase);
    void StartPairEvent(int phase);
    int CountPresent();
    int GetCharacterCountForDisplay();
    void UpdateMeeting();
    void StartMeeting(int first, int second);
    void ApproachWaiting(int index, int other, int distanceDifference);
    void AnswerWalker(int index, int walker);
    BOOL AreCharactersNear(int first, int second, fx32 reach, fx32 maxReach);
    void SetFadeState(int state);
    void SaveCharacter(RoomCharacter *character);
    void SaveCharacters();
    void LoadCharacter(RoomCharacter *character);
    void LoadCharacters();

    // Group a0's (0x0214bdac, 0x0214bdc4)
    void SetEventState(int state);
    void SetEventState(int state, int timer);

    /* 0x00000 */ u8 mUnknown00000[0x14];
    /* 0x00014 */ int mEventState;
    /* 0x00018 */ int mFadeState;
    /* 0x0001C */ int mEventTimer;
    /* 0x00020 */ RoomCharacter mCharacters[2];
    /* 0x05B50 */ RoomCharacter *mActor;      // The two characters of what is going on, in the order the
    /* 0x05B54 */ RoomCharacter *mActorOther; // encounter functions take them
    /* 0x05B58 */ u8 mUnknown05B58[0xDD4];
    /* 0x0692C */ RoomLight mLight;
    /* 0x06948 */ ApartmentFront mFront;
    /* 0x06B48 */ u8 mUnknown06B48[0x14D8];
    /* 0x08020 */ s64 mTime;                  // The clock (group a0): its first field
    /* 0x08028 */ u8 mUnknown08028[0x7D8];
    /* 0x08800 */ MessageText mMessage;
    /* 0x08DF0 */ u8 mUnknown08DF0[8];
    /* 0x08DF8 */ EncounterJudge mEncounterJudge;
    /* 0x08DFC */ u8 mUnknown08DFC[0x14954];
    /* 0x1D750 */ BOOL mEncounterPending;     // FinishEncounter judges the encounter while this is set
    /* 0x1D754 */ s16 mResidentCount;         // The names from here on are group a0's
    /* 0x1D756 */ s16 mScanIndex;
    /* 0x1D758 */ s16 mInputStep;
    /* 0x1D75A */ s16 mEventStep;
    /* 0x1D75C */ s16 mFadeStep;
    /* 0x1D75E */ u16 mIdleFrames;
    /* 0x1D760 */ u16 mEventFrames;
    /* 0x1D762 */ u16 mUnknown1D762;
    /* 0x1D764 */ u8 mResidents[100];         // Indices of the residents that exist
};

#endif // OV074_ROOMSCENE_H
