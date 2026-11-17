#ifndef OV075_SCENE40TALK_H
#define OV075_SCENE40TALK_H

#include <nitro.h>

// What the character says when touched or when it has something to tell: tables of these by the personality's
// kind (cool, dry, nagomi, nori: the four TLM message files), searched for the entries whose conditions hold now,
// one of which is chosen at random. Each condition is a mask of the values allowed (0 for a day or the days since
// the last meeting: any), tested against Scene40TalkConditions.
//
// The names are descriptive: no name of these structures survives in the ROM.
struct Scene40TalkEntry {
    /* 0x0 */ u32 mMessage : 16;      // In the personality's talk messages
    /*     */ u32 mPersonalities : 16; // A bit for each personality (0 to 15)
    /* 0x4 */ u32 mSexes : 2;          // 1 male, 2 female
    /*     */ u32 mMoods : 5;          // A bit for each mood (0 to 4)
    /*     */ u32 mMonths : 12;        // A bit for each month
    /*     */ u32 mDay : 5;            // The day of the month, 0 for any
    /*     */ u32 mHours : 4;          // 1 morning (5 to 10), 2 day (11 to 17), 4 evening (18 to 23), 8 night (0 to 4)
    /*     */ u32 mNeedsCharacters : 1; // At least three saved characters
    /*     */ u32 mNeedsFriends : 1;   // At least two friends
    /*     */ u32 mNeedsLover : 1;
    /*     */ u32 mNeedsSpouse : 1;
    /* 0x8 */ u32 mWeekdays : 7;       // A bit for each day of the week
    /*     */ u32 mItem : 4;           // 0, or which of the tools (goods 236 to 243) it must have had
    /*     */ u32 mElapsed : 5;        // 0, or how long since the last meeting (a step of sElapsedDays)
    /*     */ u32 mArgType0 : 8;       // What the message's two arguments are (Scene40Character::GetTalkArguments)
    /*     */ u32 mArgType1 : 8;
}; // 0xC

// The values the entries' conditions are tested against, as Scene40Character::GetTalkConditions finds them: one
// bit each where the entry has a mask
struct Scene40TalkConditions {
    /* 0x00 */ int mSex;
    /* 0x04 */ int mMood;
    /* 0x08 */ int mPersonality;
    /* 0x0C */ int mMonth;
    /* 0x10 */ int mHour;
    /* 0x14 */ int mWeekday;
    /* 0x18 */ int mElapsed;
    /* 0x1C */ BOOL mHasCharacters;
    /* 0x20 */ BOOL mHasFriends;
    /* 0x24 */ BOOL mHasLover;
    /* 0x28 */ BOOL mHasSpouse;
}; // 0x2C

#endif // OV075_SCENE40TALK_H
