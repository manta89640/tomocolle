#include "character/Character.h"

#include <string.h>

#include "GameData.h"
#include "save/SaveBlocks.h"
#include "island/Apartment.h"
#include "room/Room.h"
#include "voice/TtsPlayer.h"

struct MiiIdSource;

// The part of NFL_GetCharInfo's result that the character reads (lib/NFL's NFLCharInfo)
struct CharInfo {
    /* 0x00 */ u16 name[11];
    /* 0x16 */ u16 creatorName[11];
    /* 0x2C */ CreateId createId;
    /* 0x34 */ u32 bits;
    /* 0x38 */ GXRgb faceColor;
}; // 0x3C

extern "C" {
// The Mii library (lib/NFL)
int NFL_CopyDefaultMii(CharacterRecord *mii, u32 index);
int NFL_GetCharInfo(CharInfo *info, const CharacterRecord *mii);
BOOL NFL_IsSameMiiCreateIdSource(const MiiIdSource *source, const CharacterRecord *mii);
u32 NFL_GetMiiCreateIdBit6(const CharacterRecord *mii);

// The Mii data (lib/MiiData)
void MiiData_SetName(CharacterRecord *mii, const wchar_t *name);
void MiiData_SetCreatorName(CharacterRecord *mii, const wchar_t *name);
void MiiData_SetSex(CharacterRecord *mii, int value);
void MiiData_SetLocalOnly(CharacterRecord *mii, int value);
void MiiData_SetFavorite(CharacterRecord *mii, int value);
BOOL MiiData_AssignCreateId(MiiIdSource *source, CharacterRecord *mii);
typedef void (*MiiDataSetter)(CharacterRecord *mii, int value);
typedef int (*MiiDataGetter)(CharacterRecord *mii);
void MiiData_SetBeardColor(CharacterRecord *mii, int value);
void MiiData_SetBeardScale(CharacterRecord *mii, int value);
void MiiData_SetBeardType(CharacterRecord *mii, int value);
void MiiData_SetBeardY(CharacterRecord *mii, int value);
void MiiData_SetBirthDay(CharacterRecord *mii, int value);
void MiiData_SetBirthMonth(CharacterRecord *mii, int value);
void MiiData_SetBuild(CharacterRecord *mii, int value);
void MiiData_SetEyeColor(CharacterRecord *mii, int value);
void MiiData_SetEyeRotation(CharacterRecord *mii, int value);
void MiiData_SetEyeScale(CharacterRecord *mii, int value);
void MiiData_SetEyeType(CharacterRecord *mii, int value);
void MiiData_SetEyeX(CharacterRecord *mii, int value);
void MiiData_SetEyeY(CharacterRecord *mii, int value);
void MiiData_SetEyebrowColor(CharacterRecord *mii, int value);
void MiiData_SetEyebrowRotation(CharacterRecord *mii, int value);
void MiiData_SetEyebrowScale(CharacterRecord *mii, int value);
void MiiData_SetEyebrowType(CharacterRecord *mii, int value);
void MiiData_SetEyebrowX(CharacterRecord *mii, int value);
void MiiData_SetEyebrowY(CharacterRecord *mii, int value);
void MiiData_SetFaceColor(CharacterRecord *mii, int value);
void MiiData_SetFaceTexture(CharacterRecord *mii, int value);
void MiiData_SetFaceType(CharacterRecord *mii, int value);
void MiiData_SetFavoriteColor(CharacterRecord *mii, int value);
void MiiData_SetGlassesColor(CharacterRecord *mii, int value);
void MiiData_SetGlassesScale(CharacterRecord *mii, int value);
void MiiData_SetGlassesType(CharacterRecord *mii, int value);
void MiiData_SetGlassesY(CharacterRecord *mii, int value);
void MiiData_SetHairColor(CharacterRecord *mii, int value);
void MiiData_SetHairFlip(CharacterRecord *mii, int value);
void MiiData_SetHairType(CharacterRecord *mii, int value);
void MiiData_SetHeight(CharacterRecord *mii, int value);
void MiiData_SetMoleScale(CharacterRecord *mii, int value);
void MiiData_SetMoleType(CharacterRecord *mii, int value);
void MiiData_SetMoleX(CharacterRecord *mii, int value);
void MiiData_SetMoleY(CharacterRecord *mii, int value);
void MiiData_SetMouthColor(CharacterRecord *mii, int value);
void MiiData_SetMouthScale(CharacterRecord *mii, int value);
void MiiData_SetMouthType(CharacterRecord *mii, int value);
void MiiData_SetMouthY(CharacterRecord *mii, int value);
void MiiData_SetMustacheType(CharacterRecord *mii, int value);
void MiiData_SetNoseScale(CharacterRecord *mii, int value);
void MiiData_SetNoseType(CharacterRecord *mii, int value);
void MiiData_SetNoseY(CharacterRecord *mii, int value);
int MiiData_GetBeardColor(CharacterRecord *mii);
int MiiData_GetBeardScale(CharacterRecord *mii);
int MiiData_GetBeardType(CharacterRecord *mii);
int MiiData_GetBeardY(CharacterRecord *mii);
int MiiData_GetBirthDay(CharacterRecord *mii);
int MiiData_GetBirthMonth(CharacterRecord *mii);
int MiiData_GetBuild(CharacterRecord *mii);
int MiiData_GetDaysInBirthMonth(CharacterRecord *mii);
int MiiData_GetEyeColor(CharacterRecord *mii);
int MiiData_GetEyeRotation(CharacterRecord *mii);
int MiiData_GetEyeScale(CharacterRecord *mii);
int MiiData_GetEyeType(CharacterRecord *mii);
int MiiData_GetEyeX(CharacterRecord *mii);
int MiiData_GetEyeY(CharacterRecord *mii);
int MiiData_GetEyebrowColor(CharacterRecord *mii);
int MiiData_GetEyebrowRotation(CharacterRecord *mii);
int MiiData_GetEyebrowScale(CharacterRecord *mii);
int MiiData_GetEyebrowType(CharacterRecord *mii);
int MiiData_GetEyebrowX(CharacterRecord *mii);
int MiiData_GetEyebrowY(CharacterRecord *mii);
int MiiData_GetFaceColor(CharacterRecord *mii);
int MiiData_GetFaceTexture(CharacterRecord *mii);
int MiiData_GetFaceType(CharacterRecord *mii);
int MiiData_GetFavoriteColor(CharacterRecord *mii);
int MiiData_GetGlassesColor(CharacterRecord *mii);
int MiiData_GetGlassesScale(CharacterRecord *mii);
int MiiData_GetGlassesType(CharacterRecord *mii);
int MiiData_GetGlassesY(CharacterRecord *mii);
int MiiData_GetHairColor(CharacterRecord *mii);
int MiiData_GetHairFlip(CharacterRecord *mii);
int MiiData_GetHairType(CharacterRecord *mii);
int MiiData_GetHeight(CharacterRecord *mii);
int MiiData_GetMoleScale(CharacterRecord *mii);
int MiiData_GetMoleType(CharacterRecord *mii);
int MiiData_GetMoleX(CharacterRecord *mii);
int MiiData_GetMoleY(CharacterRecord *mii);
int MiiData_GetMouthColor(CharacterRecord *mii);
int MiiData_GetMouthScale(CharacterRecord *mii);
int MiiData_GetMouthType(CharacterRecord *mii);
int MiiData_GetMouthY(CharacterRecord *mii);
int MiiData_GetMustacheType(CharacterRecord *mii);
int MiiData_GetNoseScale(CharacterRecord *mii);
int MiiData_GetNoseType(CharacterRecord *mii);
int MiiData_GetNoseY(CharacterRecord *mii);

// The clock (gap)
void Clock_GetDate(void *clock, RTCDate *date);
extern void *gGameClock;
extern MiiIdSource data_0212349c; // Where this console's Miis' create IDs come from

// The saved game (gap)
BOOL GameData_CharacterExists(GameData *game, int index); // Whether there is a saved character with that index
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character); // Reads one
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes one back

BOOL GameData_GetCharacterEvent(GameData *game, int index);
u8 GameData_GetCharacterTroubleState(GameData *game, int index); // The saved character's mTroubleState
u8 GameData_GetCharacterTroubleId(GameData *game, int index); // Its mTroubleId
u8 GameData_GetCharacterTroubleCategory(GameData *game, int index); // Its mTroubleCategory
int GameData_GetCharacterSex(GameData *game, int index); // Its sex
int GameData_GetCharacterRelationToPlayer(GameData *game, int index); // Its mRelationBlood
int MiiData_GetSex(const CharacterRecord *mii);
extern const u8 data_020f8d7c[6]; // ChooseCharacterByRelation's weights of its groups
BOOL GameData_IsCharacterSlotUsed(GameData *game, int index); // Whether there is a saved character with that index
int GameData_GetCharacterRoomNumber(GameData *game, int index);  // Its mRoomNumber

// The reactions in a talk (ChooseTalkReaction) and by themselves (ChooseNextReaction)
struct TalkReactionInfo {
    u8 answerScaling;  // ScaleTalkWeightByAnswers's kind
    u8 personalities;  // A bit for each kind of personality that may react so
    u8 moods;          // A bit for each mood that may
    u8 pairScaling;    // ScaleTalkWeightByPersonalities's kind
};
struct ReactionInfo {
    u8 answerScaling;
    u8 personalities;
    u8 moods;
};
extern const TalkReactionInfo data_020f5828[12];
extern const s8 *const data_021135f0[];    // By relation: the weights of the reactions by the partner's reaction
extern const ReactionInfo data_020f5df8[15];
extern const u8 data_020f5e28[][15];       // By the reaction before: the weights of the next
int GameData_GetCharacterMood(GameData *game, int index);
u8 GameData_GetCharacterAffinity(GameData *game, int index, int other); // Its mRelationMeter for another
extern u8 gIslandState[];
extern const s8 gStr_AFKPUUA[24];  // By hour: the chance (of 100) that an idle character goes to sleep
extern const s8 data_020f8ca8[24]; // By hour: how many hours it then sleeps on top of the activity's length

extern MATHRandContext16 gRandom;
// The long activities (CharacterRecord::mLongActState)
struct LongActInfo {
    u32 minutes : 12;      // Its length
    u32 weight : 8;        // Its weight in the random choice (ChooseLongAct)
    u32 personalities : 4; // A bit for each kind of personality (gPersonalityKinds) that may choose it
    u32 moods : 5;         // A bit for each mood that may choose it
    u32 unk29 : 3;
};

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78)
struct TroubleInfo {
    u32 kind : 8;
    u32 minutes : 16;         // How long it lasts at most
    u32 weight : 8;           // Its weight in the random choice (ChooseTrouble)
    u32 unk4Bits0 : 8;
    u32 mood : 8;             // The mood it puts the character in (SetTroubleMood), 0 to 4
    u32 cooldownMinutes : 12; // Added to mTroubleEnd once it is over
    u32 hasTarget : 4;        // Not 0: about a character or an item, which StartTrouble chooses (mTroubleTarget)
    u32 flags;                // Food troubles: the kinds of food; clothes troubles: the kinds of clothes
}; // 0xC
extern TroubleInfo *data_02114d78[];
extern const u8 gStr_Parenthesis_2[]; // The starting progress of each trouble of category 0
extern const u8 data_020f8d8c[10];
extern const u32 gStr_LessThan_2[10];  // The number of troubles of each category
extern const LongActInfo data_020f8cc0[];
}

// The voice parameters' ranges and defaults (VoiceParam::defaultValue is what ResetVoice sets)
struct VoiceParam {
    /* 0x0 */ s16 param; // The speech engine's parameter (TtsPlayer::SetEngineParam)
    /* 0x2 */ s16 min;
    /* 0x4 */ s16 max;
    /* 0x6 */ s16 defaultValue;
}; // 0x8

extern const VoiceParam gVoiceParams[6];
extern const int gPersonalityKinds[16];
extern const s16 gVoicePresets[6][6];

// How much eating each kind of food fills (ItemInfo::mFoodSize)
static const u8 sFoodSizes[4] = {35, 50, 75, 0};
// By kind of personality: its column of a table of four (GetPersonalityColumnValue, GetPersonalityColumnOffset)
static const int sPersonalityColumnOffsets[4] = {3, 1, 2, 0};
static const int sPersonalityColumns[4] = {3, 1, 2, 0};
// The scores of the personality questions' answers (index: the answer, 1 to 8; 0 unanswered)
// (the fifth is read by nothing the game keeps)
static const s8 sAnswerScores[5][9] = {
    {50, 10, 20, 30, 40, 60, 70, 80, 90},
    {50, 5, 15, 25, 35, 65, 75, 85, 95},
    {50, 10, 20, 30, 40, 60, 70, 80, 90},
    {50, 5, 15, 25, 35, 65, 75, 85, 95},
    {50, 10, 20, 30, 40, 60, 70, 80, 90},
};
// The biorhythm's cycles, by day of the cycle
static const s16 sPhysicalCycle[23] = {-5000, 20805, 24897, 28359, 30932, 32426, 32767, 31823, 29770, 26724, 22912, 18615, -7000, 9857, 6045, 2999, 946, 0, 343, 1837, 4410, 7872, 11964};
// The voice parameters: the speech engine's parameter, range and default (ResetVoice)
const VoiceParam gVoiceParams[6] = {{14, 0, 400, 100}, {15, 0, 400, 100}, {6, 80, 130, 100}, {4, 3500, 7000, 5058}, {3, 50, 180, 100}, {12, 0, 3, 0}};
// The days of each month (1 to 12), and in a leap year
static const int sDaysInMonth[13] = {31, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const int sDaysInMonthLeap[13] = {31, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const s16 sEmotionalCycle[28] = {-5000, 20030, 23493, 26600, 29194, 31146, 32358, 32767, 32358, 31146, 29194, 26600, 23493, 20030, -7000, 12739, 9276, 6169, 3575, 1623, 411, 0, 411, 1623, 3575, 6169, 9276, 12739};
// Whether two RelationBlood values (0 to 5) allow love, a triangle of 6
static const s8 sRelationBloodPairs[21] = {1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 1, 0};
// The kind (0 to 3) of each personality (mPersonality)
const int gPersonalityKinds[16] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3};
static const s16 sIntellectualCycle[34] = {-5000, 19485, 22474, 25242, 27691, 29731, 31288, 32307, 32767, 32602, 31867, 30573, 28767, 26512, 23892, 21000, 17942, -7000, 11769, 8877, 6257, 4002, 2196, 902, 167, 0, 462, 1481, 3038, 5078, 7527, 10295, 13284};
// The voices overlay 76 offers to choose from
const s16 gVoicePresets[6][6] = {{100, 100, 107, 5425, 90, 0}, {100, 100, 115, 5600, 76, 0}, {100, 100, 100, 5075, 104, 0}, {100, 100, 106, 5643, 118, 0}, {100, 100, 85, 4375, 118, 0}, {170, 50, 111, 5250, 151, 0}};
// How well two personalities go together (0 to 15), a triangle of 16
static const s8 sPersonalityPairs[136] = {100, 90, 70, 60, 90, 80, 70, 60, 70, 60, 40, 30, 60, 50, 30, 20, 100, 80, 70, 80, 90, 70, 60, 60, 70, 50, 40, 50, 60, 40, 30, 100, 90, 60, 70, 90, 80, 40, 50, 70, 60, 30, 40, 60, 50, 100, 50, 60, 80, 90, 30, 40, 60, 70, 20, 30, 50, 60, 100, 90, 70, 60, 80, 70, 50, 40, 70, 60, 40, 30, 100, 80, 70, 70, 80, 60, 50, 60, 70, 50, 40, 100, 90, 50, 60, 80, 70, 40, 50, 70, 60, 100, 40, 50, 70, 80, 30, 40, 60, 70, 100, 90, 70, 60, 90, 80, 60, 50, 100, 80, 70, 80, 90, 70, 60, 100, 90, 60, 70, 90, 80, 100, 50, 60, 80, 90, 100, 90, 70, 60, 100, 80, 70, 100, 90, 100};
// The Mii data's setters and getters by MiiParameter
static MiiDataSetter const sMiiSetters[PARAM_COUNT] = {
    MiiData_SetEyeType,
    MiiData_SetEyeColor,
    MiiData_SetEyeScale,
    MiiData_SetEyeRotation,
    MiiData_SetEyeX,
    MiiData_SetEyeY,
    MiiData_SetEyebrowType,
    MiiData_SetEyebrowColor,
    MiiData_SetEyebrowScale,
    MiiData_SetEyebrowRotation,
    MiiData_SetEyebrowX,
    MiiData_SetEyebrowY,
    MiiData_SetMouthType,
    MiiData_SetMouthColor,
    MiiData_SetMouthScale,
    MiiData_SetMouthY,
    MiiData_SetBeardType,
    MiiData_SetBeardColor,
    MiiData_SetMustacheType,
    MiiData_SetBeardScale,
    MiiData_SetBeardY,
    MiiData_SetMoleType,
    MiiData_SetMoleScale,
    MiiData_SetMoleX,
    MiiData_SetMoleY,
    MiiData_SetHairType,
    MiiData_SetHairColor,
    MiiData_SetHairFlip,
    MiiData_SetNoseType,
    MiiData_SetNoseScale,
    MiiData_SetNoseY,
    MiiData_SetGlassesType,
    MiiData_SetGlassesColor,
    MiiData_SetGlassesScale,
    MiiData_SetGlassesY,
    MiiData_SetFaceType,
    MiiData_SetFaceColor,
    MiiData_SetFaceTexture,
    MiiData_SetBirthMonth,
    MiiData_SetBirthDay,
    MiiData_SetFavoriteColor,
    MiiData_SetHeight,
    MiiData_SetBuild,
    MiiData_SetBirthDay,
};
static MiiDataGetter const sMiiGetters[PARAM_COUNT] = {
    MiiData_GetEyeType,
    MiiData_GetEyeColor,
    MiiData_GetEyeScale,
    MiiData_GetEyeRotation,
    MiiData_GetEyeX,
    MiiData_GetEyeY,
    MiiData_GetEyebrowType,
    MiiData_GetEyebrowColor,
    MiiData_GetEyebrowScale,
    MiiData_GetEyebrowRotation,
    MiiData_GetEyebrowX,
    MiiData_GetEyebrowY,
    MiiData_GetMouthType,
    MiiData_GetMouthColor,
    MiiData_GetMouthScale,
    MiiData_GetMouthY,
    MiiData_GetBeardType,
    MiiData_GetBeardColor,
    MiiData_GetMustacheType,
    MiiData_GetBeardScale,
    MiiData_GetBeardY,
    MiiData_GetMoleType,
    MiiData_GetMoleScale,
    MiiData_GetMoleX,
    MiiData_GetMoleY,
    MiiData_GetHairType,
    MiiData_GetHairColor,
    MiiData_GetHairFlip,
    MiiData_GetNoseType,
    MiiData_GetNoseScale,
    MiiData_GetNoseY,
    MiiData_GetGlassesType,
    MiiData_GetGlassesColor,
    MiiData_GetGlassesScale,
    MiiData_GetGlassesY,
    MiiData_GetFaceType,
    MiiData_GetFaceColor,
    MiiData_GetFaceTexture,
    MiiData_GetBirthMonth,
    MiiData_GetBirthDay,
    MiiData_GetFavoriteColor,
    MiiData_GetHeight,
    MiiData_GetBuild,
    MiiData_GetDaysInBirthMonth,
};

// Keeps a field of the record within its range; FALSE when it was not. The name is the field's, for messages that
// the final build leaves out.
template <typename T> BOOL ClampField(T *value, int min, int max, const char *name) {
    if (*value > max) {
        *value = max;
    } else if (*value < min) {
        *value = min;
    } else {
        return TRUE;
    }
    return FALSE;
}

// A random number from 0 to max - 1
static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

SavedCharacter::SavedCharacter() {
    mName[10] = 0;
    mFamilyName[10] = 0;
    mIndex = 0;
    mReaction = 0;
    mPersonality = 0;
}

void SavedCharacter::Reset() {
    NFL_CopyDefaultMii(this, 2);
    MiiData_SetName(this, L"");
    MiiData_SetCreatorName(this, L"");
    SetSex(0);
    SetRawParameter(PARAM_BIRTH_MONTH, 0);
    SetRawParameter(PARAM_BIRTH_DAY, 0);
    SetRawParameter(PARAM_FAVORITE_COLOR, 0);
    SetFavorite(0);
    SetRawParameter(PARAM_HEIGHT, 64);
    SetRawParameter(PARAM_BUILD, 64);
    SetLocalOnly(1);
    mUnknown050Bit0 = 0;
    SetRegisteredDate(0, 1, 1);
    mUnknown1C4 = 0;
    MI_CpuClear16(mName, sizeof(mName));
    MI_CpuClear16(mFamilyName, sizeof(mFamilyName));
    SetBirthYear(0xFFF);
    SetBloodType(4);
    SetOwnBirthMonth(0);
    SetOwnBirthDay(0);
    MI_CpuFill8(mAnswers, 0, sizeof(mAnswers));
    ResetVoice();
    mUnknown054Bits0 = 8;
    MI_CpuClear16(mMotto, sizeof(mMotto));
    MI_CpuClear16(mMottos, sizeof(mMottos));
    mClothesId = 0;
    mInterior = 0;
    mRoomNumber = 0xFF;
    MI_CpuClear32(mOwnedInteriors, sizeof(mOwnedInteriors));
    ResetRelations();
    ResetMood();
    ClearLongAct();
    ClearTrouble();
    ResetSatisfaction();
    ResetEating();
    ClearUnknown246();
    ResetTouchTalk();
    mUnknown054Bit20 = 0;
    ClearUnknown1C8();
    ClearOwnedClothes();
    OwnClothes(mClothesId);
    ClearSongSlots();
    ResetItemPreferences();
    mUnknown296 = 0;
    mUnknown297 = 0;
    mRelationBlood = 0;
    ResetPatDate();
    ResetUnknown2A3();
    ResetMeetDate();
    ResetUnknown24E();
    MI_CpuClear32(mBiorhythmCycles, sizeof(mBiorhythmCycles));
    mBiorhythm = 0;
    mBiorhythmYesterday = 0;
    mReaction = 0;
    mPersonality = 0;
}

void SavedCharacter::MakeVisitor() {
    mUnknown050Bit0 = 1;
    SetRegisteredDateToday();
    mClothesId = 0;
    mInterior = 0;
    OwnClothes(mClothesId);
}

void SavedCharacter::Register() {
    SetRegisteredDateToday();
    mUnknown1C4 = 0;
    SetBirthYear(0xFFF);
    SetOwnBirthMonth(0);
    SetOwnBirthDay(0);
    if (mUnknown050Bit0) {
        mRoomNumber = 0xFF;
    } else {
        AssignFreeRoom();
    }
    ResetRelations();
    ResetMood();
    ClearLongAct();
    ClearTrouble();
    ResetEating();
    ClearUnknown246();
    ResetTouchTalk();
    mUnknown054Bit20 = 0;
    mRelationBlood = 0;
    ResetPatDate();
    ResetUnknown2A3();
    ResetMeetDate();
    ResetUnknown24E();
}

// Checks every field of the record and puts the ones out of range back in it; FALSE when any was out
BOOL SavedCharacter::Validate() {
    BOOL valid = TRUE;

    if (mRegisteredYear > 99) {
        mRegisteredYear = 99;
    }
    if (mRegisteredMonth == 0 || mRegisteredMonth > 12) {
        mRegisteredMonth = 1;
    }
    if (mRegisteredDay == 0 || mRegisteredDay > 31) {
        mRegisteredDay = 1;
    }
    if (mBirthYear != 0xFFF) {
        if (mBirthYear < 1900) {
            valid = FALSE;
            mBirthYear = 1900;
        } else if (mBirthYear > 2099) {
            valid = FALSE;
            mBirthYear = 2099;
        }
    }
    if (mOwnBirthMonth > 12) {
        mOwnBirthMonth = 0;
        valid = FALSE;
    }
    if (mOwnBirthDay > 31) {
        mOwnBirthDay = 0;
        valid = FALSE;
    }
    if (mBloodType > 4) {
        mBloodType = 4;
        valid = FALSE;
    }
    for (int i = 0; i < 5; i++) {
        if (!ClampField(&mAnswers[i], 0, 9, "character")) {
            valid = FALSE;
        }
    }
    if (mUnknown054Bits0 > 8) {
        mUnknown054Bits0 = 8;
        valid = FALSE;
    }
    if (mInterior >= 78) {
        mInterior = 0;
        valid = FALSE;
    }
    for (int i = 0; i < 6; i++) {
        if (!ClampField(&mVoice[i], gVoiceParams[i].min, gVoiceParams[i].max, "voice")) {
            valid = FALSE;
        }
    }
    if (!TerminateString(mMotto, 17, "motto1")) {
        valid = FALSE;
    }
    for (int i = 0; i < 4; i++) {
        if (!TerminateString(mMottos[i], 9, "motto")) {
            valid = FALSE;
        }
    }
    if (mClothesId >= 300) {
        valid = FALSE;
        mClothesId = 0;
    }
    if (mRoomNumber >= 100) {
        valid = FALSE;
        mRoomNumber = 0;
    }
    if (mMood >= 5) {
        valid = FALSE;
        mMood = 0;
    }
    for (int i = 0; i < 100; i++) {
        if (!ClampField(&mRelationMeter[i], 0, 200, "RelationMeter")) {
            valid = FALSE;
        }
    }
    for (int i = 0; i < 100; i++) {
        if (!ClampField(&mRelationRank[i], 0, 10, "RelationRank")) {
            valid = FALSE;
        }
    }
    if (!ClampField(&mLongActState, 0, 0x2E, "LongActState")) {
        valid = FALSE;
    }
    if (mLongActEnd < 0) {
        mLongActEnd = 0;
        valid = FALSE;
    }
    if (!ClampField(&mTroubleState, 0, 5, "TroubleState")) {
        valid = FALSE;
    }
    if (!ClampField(&mTroubleId, 0, 0x6D, "TroubleID")) {
        valid = FALSE;
    }
    if (!ClampField(&mTroubleCategory, 0, 9, "TroubleCategory")) {
        valid = FALSE;
    }
    if (mTroubleEnd < 0) {
        mTroubleEnd = 0;
        valid = FALSE;
    }
    if (mTroubleStart < 0) {
        mTroubleStart = 0;
        valid = FALSE;
    }
    if (!ClampField(&mSatisfyMeter, 0, 99, "SatisfyMeter")) {
        valid = FALSE;
    }
    if (!ClampField(&mSatisfyLevel, 1, 99, "SatisfyLevel")) {
        valid = FALSE;
    }
    if (mUnknown23C < 0) {
        mUnknown23C = 0;
        valid = FALSE;
    }
    if (!ClampField(&mEatFullness, 0, 100, "EatFullness")) {
        valid = FALSE;
    }
    if (!ClampField(&mEatLastFood, 0, 0xFF, "EatLastFood")) {
        valid = FALSE;
    }
    if (!ClampField(&mMeetYear, 0, 99, "MeetYear")) {
        valid = FALSE;
    }
    if (!ClampField(&mMeetMonth, 1, 12, "MeetMonth")) {
        valid = FALSE;
    }
    if (!ClampField(&mMeetDay, 1, 31, "MeetDay")) {
        valid = FALSE;
    }
    if (!ClampField(&mPatYear, 0, 99, "PatYear")) {
        valid = FALSE;
    }
    if (!ClampField(&mPatMonth, 0, 12, "PatMonth")) {
        valid = FALSE;
    }
    if (!ClampField(&mPatDay, 0, 31, "PatDay")) {
        valid = FALSE;
    }
    if (!ClampField(&mTouchTalkMonth, 1, 12, "TouchTalkMonth")) {
        valid = FALSE;
    }
    if (!ClampField(&mTouchTalkDay, 1, 31, "TouchTalkDay")) {
        valid = FALSE;
    }
    if (!ClampField(&mTouchTalkHour, 0, 23, "TouchTalkHour")) {
        valid = FALSE;
    }
    if (mUnknown2A3 > 99 && mUnknown2A3 != 0xFF) {
        mUnknown2A3 = 0xFF;
        valid = FALSE;
    }
    if (mUnknown054Bit20 > 1) {
        mUnknown054Bit20 = 0;
        valid = FALSE;
    }
    int i;
    for (i = 0; i < 6; i++) {
        if (mSongSlots[i] >= 100 && mSongSlots[i] != 0xFF) {
            mSongSlots[i] = 0xFF;
            valid = FALSE;
        }
    }
    if (!AreFoodPreferencesValid()) {
        ChooseNewFoodPreferences();
        valid = FALSE;
    }
    valid &= CheckUnknown292();
    if (!ClampField(&mRelationBlood, 0, 5, "RelationBlood")) {
        valid = FALSE;
    }
    if (mUnknown24E >= 6 && mUnknown24E < 0xFF) {
        mUnknown24E = 0xFF;
        valid = FALSE;
    }
    return valid;
}

BOOL SavedCharacter::TerminateString(wchar_t *str, int length, const char *name) {
    BOOL terminated = FALSE;
    for (int i = 0; i < length; i++) {
        if (str[i] == 0) {
            terminated = TRUE;
            break;
        }
    }
    if (!terminated) {
        str[length - 1] = 0;
    }
    return terminated;
}

BOOL SavedCharacter::IsResident() {
    if (mIndex < 100 && !mUnknown050Bit0) {
        return TRUE;
    }
    return FALSE;
}

void SavedCharacter::SetRegisteredDate(int year, int month, int day) {
    mRegisteredYear = year;
    mRegisteredMonth = month;
    mRegisteredDay = day;
}

void SavedCharacter::SetRegisteredDateToday() {
    RTCDate date;
    Clock_GetDate(gGameClock, &date);
    SetRegisteredDate(date.year, date.month, date.day);
}

void SavedCharacter::AddUnknown1C4(int value) {
    s64 sum = value + mUnknown1C4;
    if (sum < 0) {
        sum = 0;
    } else if (sum > 0xFFFFFFFF) {
        sum = 0xFFFFFFFF;
    }
    mUnknown1C4 = sum;
}

BOOL SavedCharacter::IsMiiFromThisConsole() {
    return NFL_IsSameMiiCreateIdSource(&data_0212349c, this);
}

void SavedCharacter::AssignCreateId() {
    MiiData_AssignCreateId(&data_0212349c, this);
}

void SavedCharacter::GetCreateId(CreateId *id, const CharacterRecord *character) {
    CharInfo info;
    NFL_GetCharInfo(&info, character);
    *id = info.createId;
}

u32 SavedCharacter::GetCreateIdNumber() {
    CreateId copy;
    CreateId id;
    GetCreateId(&id, this);
    copy = id;
    u32 number = 0;
    if (NFL_GetMiiCreateIdBit6(this)) {
        number += (copy.data[0] & 0x0F) << 12;
        number += copy.data[1] << 4;
        number += copy.data[2] >> 4;
    } else {
        number += (copy.data[3] & 0x03) << 14;
        number += copy.data[2] << 6;
        number += copy.data[3] >> 2;
    }
    return number;
}

u32 SavedCharacter::GetCreateIdParts(int *high, int *low) {
    u32 number = GetCreateIdNumber();
    *high = (number & 0xC000) >> 14;
    *low = number & 0x3FFF;
    return number;
}

void SavedCharacter::GetParameterRange(int param, int *min, int *max) {
    switch (param) {
    case PARAM_EYE_TYPE:
        *min = 0;
        *max = 47;
        break;
    case PARAM_EYE_COLOR:
        *min = 0;
        *max = 5;
        break;
    case PARAM_EYE_SCALE:
        *min = 0;
        *max = 7;
        break;
    case PARAM_EYE_ROTATION:
        *min = 0;
        *max = 7;
        break;
    case PARAM_EYE_X:
        *min = 0;
        *max = 12;
        break;
    case PARAM_EYE_Y:
        *min = 0;
        *max = 18;
        break;
    case PARAM_EYEBROW_TYPE:
        *min = 0;
        *max = 23;
        break;
    case PARAM_EYEBROW_COLOR:
        *min = 0;
        *max = 7;
        break;
    case PARAM_EYEBROW_SCALE:
        *min = 0;
        *max = 8;
        break;
    case PARAM_EYEBROW_ROTATION:
        *min = 0;
        *max = 11;
        break;
    case PARAM_EYEBROW_X:
        *min = 0;
        *max = 12;
        break;
    case PARAM_EYEBROW_Y:
        *min = 3;
        *max = 18;
        break;
    case PARAM_MOUTH_TYPE:
        *min = 0;
        *max = 23;
        break;
    case PARAM_MOUTH_COLOR:
        *min = 0;
        *max = 2;
        break;
    case PARAM_MOUTH_SCALE:
        *min = 0;
        *max = 8;
        break;
    case PARAM_MOUTH_Y:
        *min = 0;
        *max = 18;
        break;
    case PARAM_BEARD_TYPE:
        *min = 0;
        *max = 3;
        break;
    case PARAM_BEARD_COLOR:
        *min = 0;
        *max = 7;
        break;
    case PARAM_MUSTACHE_TYPE:
        *min = 0;
        *max = 3;
        break;
    case PARAM_BEARD_SCALE:
        *min = 0;
        *max = 8;
        break;
    case PARAM_BEARD_Y:
        *min = 0;
        *max = 16;
        break;
    case PARAM_MOLE_TYPE:
        *min = 0;
        *max = 1;
        break;
    case PARAM_MOLE_SCALE:
        *min = 0;
        *max = 8;
        break;
    case PARAM_MOLE_X:
        *min = 0;
        *max = 16;
        break;
    case PARAM_MOLE_Y:
        *min = 0;
        *max = 30;
        break;
    case PARAM_HAIR_TYPE:
        *min = 0;
        *max = 71;
        break;
    case PARAM_HAIR_COLOR:
        *min = 0;
        *max = 7;
        break;
    case PARAM_HAIR_FLIP:
        *min = 0;
        *max = 1;
        break;
    case PARAM_NOSE_TYPE:
        *min = 0;
        *max = 11;
        break;
    case PARAM_NOSE_SCALE:
        *min = 0;
        *max = 8;
        break;
    case PARAM_NOSE_Y:
        *min = 0;
        *max = 18;
        break;
    case PARAM_GLASSES_TYPE:
        *min = 0;
        *max = 8;
        break;
    case PARAM_GLASSES_COLOR:
        *min = 0;
        *max = 5;
        break;
    case PARAM_GLASSES_SCALE:
        *min = 0;
        *max = 7;
        break;
    case PARAM_GLASSES_Y:
        *min = 0;
        *max = 20;
        break;
    case PARAM_FACE_TYPE:
        *min = 0;
        *max = 7;
        break;
    case PARAM_FACE_COLOR:
        *min = 0;
        *max = 5;
        break;
    case PARAM_FACE_TEXTURE:
        *min = 0;
        *max = 11;
        break;
    case PARAM_BIRTH_MONTH:
        *min = 0;
        *max = 12;
        break;
    case PARAM_BIRTH_DAY:
        *min = 0;
        switch (GetParameter(PARAM_BIRTH_MONTH)) {
        case 1:
            *max = 31;
            break;
        case 2: {
            int year = mBirthYear;
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
                *max = 29;
            } else {
                *max = 28;
            }
            break;
        }
        case 3:
            *max = 31;
            break;
        case 4:
            *max = 30;
            break;
        case 5:
            *max = 31;
            break;
        case 6:
            *max = 30;
            break;
        case 7:
            *max = 31;
            break;
        case 8:
            *max = 31;
            break;
        case 9:
            *max = 30;
            break;
        case 10:
            *max = 31;
            break;
        case 11:
            *max = 30;
            break;
        case 12:
            *max = 31;
            break;
        default:
            *max = 0;
            break;
        }
        break;
    case PARAM_FAVORITE_COLOR:
        *min = 0;
        *max = 11;
        break;
    case PARAM_HEIGHT:
        *min = 0;
        *max = 127;
        break;
    case PARAM_BUILD:
        *min = 0;
        *max = 127;
        break;
    }
}

void SavedCharacter::CheckParameter(int param, int value) {
    int min;
    int max;
    GetParameterRange(param, &min, &max);
}

void SavedCharacter::SetParameter(int param, int value) {
    CheckParameter(param, value);
    if (IsMiiFromThisConsole()) {
        sMiiSetters[param](this, value);
        return;
    }
    switch (param) {
    case PARAM_BIRTH_MONTH:
        SetOwnBirthMonth(value);
        break;
    case PARAM_BIRTH_DAY:
        SetOwnBirthDay(value);
        break;
    }
}

void SavedCharacter::SetRawParameter(int param, int value) {
    sMiiSetters[param](this, value);
}

void SavedCharacter::SetSex(int sex) {
    MiiData_SetSex(this, sex);
}

void SavedCharacter::SetLocalOnly(int value) {
    MiiData_SetLocalOnly(this, value);
}

void SavedCharacter::SetFavorite(int value) {
    MiiData_SetFavorite(this, value);
}

void SavedCharacter::SetBirthYear(u16 year) {
    mBirthYear = year;
}

void SavedCharacter::SetOwnBirthMonth(int month) {
    mOwnBirthMonth = month;
}

void SavedCharacter::SetOwnBirthDay(int day) {
    mOwnBirthDay = day;
}

void SavedCharacter::SetBloodType(int bloodType) {
    mBloodType = (u8)bloodType;
}

void SavedCharacter::SetAnswer(int index, u8 answer) {
    mAnswers[index] = answer;
}

void SavedCharacter::SetVoice(int index, s16 value) {
    mVoice[index] = value;
}

void SavedCharacter::ResetVoice() {
    for (int i = 0; i < 6; i++) {
        SetVoice(i, gVoiceParams[i].defaultValue);
    }
}

int SavedCharacter::GetParameter(int param) {
    u32 value = sMiiGetters[param](this);
    switch (param) {
    case PARAM_BIRTH_MONTH:
        if (value == 0 && !IsMiiFromThisConsole()) {
            value = mOwnBirthMonth;
        }
        break;
    case PARAM_BIRTH_DAY:
        if (value == 0 && !IsMiiFromThisConsole()) {
            value = mOwnBirthDay;
        }
        break;
    case PARAM_HEIGHT:
    case PARAM_BUILD:
        if (value >= 128) {
            value = 127;
        }
        break;
    }
    return value;
}

int SavedCharacter::GetRawParameter(int param) {
    return sMiiGetters[param](this);
}

GXRgb SavedCharacter::GetFaceColor() {
    CharInfo info;
    NFL_GetCharInfo(&info, this);
    return info.faceColor;
}

int SavedCharacter::RandomFromCreateId(int max) {
    u32 x = GetCreateIdNumber() * 0x5D588B65 + 0x269EC3;
    if (max != 0) {
        x = (x >> 16) * max;
    }
    return (u16)(x >> 16);
}

int SavedCharacter::GetAnswerScore01() {
    return ScoreAnswers01(mAnswers[0], mAnswers[1]);
}

int SavedCharacter::ScoreAnswers01(int answer0, int answer1) {
    return sAnswerScores[0][answer0] + sAnswerScores[1][answer1];
}

int SavedCharacter::GetAnswerScore23() {
    return ScoreAnswers23(mAnswers[2], mAnswers[3]);
}

int SavedCharacter::ScoreAnswers23(int answer2, int answer3) {
    return sAnswerScores[2][answer2] + sAnswerScores[3][answer3];
}

s8 SavedCharacter::GetPersonality() {
    int personality = 0;
    int score01 = GetAnswerScore01();
    int score23 = GetAnswerScore23();
    if (score01 >= 51 && score01 <= 100) {
        personality += 1;
    } else if (score01 >= 101 && score01 <= 150) {
        personality += 2;
    } else if (score01 >= 151 && score01 <= 200) {
        personality += 3;
    }
    if (score23 >= 51 && score23 <= 100) {
        personality += 4;
    } else if (score23 >= 101 && score23 <= 150) {
        personality += 8;
    } else if (score23 >= 151 && score23 <= 200) {
        personality += 12;
    }
    return personality;
}

BOOL SavedCharacter::Is0Or1(int value) {
    switch (value) {
    case 0:
    case 1:
        return TRUE;
    }
    return FALSE;
}

void SavedCharacter::SetMoodFromResult(int result, int detail) {
    int mood = 5;
    switch (result) {
    case 0:
        mood = 0;
        break;
    case 1:
        mood = detail == 0 ? 1 : 0;
        break;
    case 2:
        mood = detail != 7 ? 1 : 0;
        break;
    case 3:
        mood = 3;
        break;
    case 4:
        mood = 1;
        break;
    case 5:
        mood = 3;
        break;
    case 6:
    case 7:
    case 8:
        mood = 2;
        break;
    case 9:
        mood = 1;
        break;
    }
    if (mood == 5) {
        return;
    }
    if (mTroubleState == 0) {
        mMood = mood;
    }
}

void SavedCharacter::ResetMood() {
    mMood = 0;
}

void SavedCharacter::AddRelationMeter(int index, int change) {
    int value = mRelationMeter[index] + change;
    if (value > 200) {
        value = 200;
    } else if (value < 0) {
        value = 0;
    }
    mRelationMeter[index] = value;
}

void SavedCharacter::SetRelation(int index, u8 rank, u8 meter) {
    mRelationMeter[index] = meter;
    mRelationRank[index] = rank;
}

void SavedCharacter::ResetRelations() {
    MI_CpuFill8(mRelationMeter, 100, sizeof(mRelationMeter));
    MI_CpuFill8(mRelationRank, 0, sizeof(mRelationRank));
}

int SavedCharacter::FindCouplePartner() {
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && IsCoupleRelation(mRelationRank[i])) {
            return i;
        }
    }
    return -1;
}

int SavedCharacter::FindLoverOrSpouse() {
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && IsLoversOrSpouses(mRelationRank[i])) {
            return i;
        }
    }
    return -1;
}

int SavedCharacter::FindBestFriend() {
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && mRelationRank[i] == 9) {
            return i;
        }
    }
    return -1;
}

BOOL SavedCharacter::IsCoupleRelation(int rank) {
    switch (rank) {
    case 2:
    case 4:
    case 7:
    case 8:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsLoversOrSpouses(int rank) {
    switch (rank) {
    case 2:
    case 4:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsFightingRelation(int rank) {
    switch (rank) {
    case 6:
    case 7:
    case 8:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsFriendsLoversOrSpouses(int rank) {
    switch (rank) {
    case 1:
    case 2:
    case 4:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsFriendlyOrFightingRelation(int rank) {
    if (IsFightingRelation(rank)) {
        return TRUE;
    }
    switch (rank) {
    case 1:
    case 2:
    case 4:
    case 9:
        return TRUE;
    }
    return FALSE;
}

void SavedCharacter::SetLongAct(u8 state, u8 partner, s64 end) {
    mLongActState = state;
    mLongActPartner = partner;
    mLongActEnd = end;
}

void SavedCharacter::ClearLongActFrom(s64 now) {
    SetLongAct(0, 0, now + GetLongActDuration(0, 3));
}

void SavedCharacter::ClearLongAct() {
    SetLongAct(0, 0, 0);
}

void SavedCharacter::EndLongAct(SavedCharacter *first, SavedCharacter *second) {
    if (IsLongActPaired()) {
        u8 partner = mLongActPartner;
        SavedCharacter other;
        SavedCharacter *partnerCharacter;
        if (first != NULL && partner == first->mIndex) {
            partnerCharacter = first;
        } else if (second != NULL && partner == second->mIndex) {
            partnerCharacter = second;
        } else {
            GameData_ReadCharacter(&gGameData, partner, &other);
            other.mPersonality = other.GetPersonality();
            partnerCharacter = &other;
        }
        partnerCharacter->ClearLongAct();
        GameData_WriteCharacter(&gGameData, partner, partnerCharacter);
    }
    ClearLongAct();
    ClearUnknown246();
    GameData_WriteCharacter(&gGameData, mIndex, this);
}

void SavedCharacter::StartNextLongAct(s64 now, RTCTime *time, SavedCharacter *first, SavedCharacter *second) {
    EndLongAct(first, second);
    StartLongAct(ChooseLongAct(), now, time, first, second);
}

BOOL SavedCharacter::IsAway() {
    return IsAwayState(mLongActState);
}

BOOL SavedCharacter::IsAwayState(int state) {
    if (IsLongActWithPartnerState(state)) {
        return TRUE;
    }
    switch (state) {
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsLongActPaired() {
    return IsLongActPairedState(mLongActState);
}

BOOL SavedCharacter::IsLongActPairedState(int state) {
    if (IsLongActWithPartnerState(state)) {
        return TRUE;
    }
    if (IsLongActAsPartnerState(state)) {
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsLongActWithPartner() {
    return IsLongActWithPartnerState(mLongActState);
}

BOOL SavedCharacter::IsLongActWithPartnerState(int state) {
    switch (state) {
    case 0x17:
    case 0x19:
    case 0x1B:
    case 0x1D:
    case 0x1F:
    case 0x21:
    case 0x23:
    case 0x25:
    case 0x27:
    case 0x29:
    case 0x2B:
    case 0x2D:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsLongActAsPartner() {
    return IsLongActAsPartnerState(mLongActState);
}

BOOL SavedCharacter::IsLongActAsPartnerState(int state) {
    switch (state) {
    case 0x18:
    case 0x1A:
    case 0x1C:
    case 0x1E:
    case 0x20:
    case 0x22:
    case 0x24:
    case 0x26:
    case 0x28:
    case 0x2A:
    case 0x2C:
    case 0x2E:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsLongAct2BTo2E(int state) {
    switch (state) {
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsLongActIdle() {
    return IsLongActIdleState(mLongActState);
}

BOOL SavedCharacter::IsLongActIdleState(int state) {
    switch (state) {
    case 0:
    case 0x18:
        return TRUE;
    }
    return FALSE;
}

s64 SavedCharacter::GetLongActDuration(int state, int shift) {
    int minutes = data_020f8cc0[state].minutes * 60;
    int random = minutes >> shift;
    if (random > 0x7FFF) {
        random = 0x7FFF;
    } else if (random < 0) {
        random = 0;
    }
    return minutes + MATH_Rand16(&gRandom, random);
}

BOOL SavedCharacter::IsLongActAvailable(int state) {
    switch (state) {
    case 1:
    case 34:
        return (mUnknown1C8.m[7] >> 16) & 1;
    case 4:
        return (mUnknown1C8.m[7] >> 19) & 1;
    case 5:
    case 42:
        return (mUnknown1C8.m[7] >> 12) & 1;
    case 8:
    case 38:
        return (mUnknown1C8.m[7] >> 14) & 1;
    case 9:
    case 32:
        return (mUnknown1C8.m[7] >> 18) & 1;
    case 10:
    case 30:
        return (mUnknown1C8.m[7] >> 17) & 1;
    case 12:
    case 36:
        return (mUnknown1C8.m[7] >> 15) & 1;
    case 16:
    case 40:
        return (mUnknown1C8.m[7] >> 13) & 1;
    case 29:
    case 31:
    case 33:
    case 35:
    case 37:
    case 39:
    case 41:
        return FALSE;
    }
    return TRUE;
}

int SavedCharacter::GetLongActWeight(int state) {
    const LongActInfo *info = &data_020f8cc0[state];
    if ((1 << gPersonalityKinds[mPersonality]) & info->personalities && info->moods & (1 << mMood) && info->weight != 0 &&
        IsLongActAvailable(state)) {
        return data_020f8cc0[state].weight;
    }
    return 0;
}

int SavedCharacter::GetTotalLongActWeight() {
    int total = 0;
    for (u32 i = 0; i < 47; i++) {
        total += GetLongActWeight(i);
    }
    return total;
}

int SavedCharacter::ChooseLongAct() {
    mPersonality = GetPersonality();
    int random = Rand(GetTotalLongActWeight());
    int sum = 0;
    u32 i;
    for (i = 0; i < 47; i++) {
        sum += GetLongActWeight(i);
        if (sum > random) {
            break;
        }
    }
    if (i >= 47) {
        i = 0;
    }
    return i;
}

bool SavedCharacter::IsInTrouble() {
    return IsTroubleStateActive(mTroubleState);
}

bool SavedCharacter::IsTroubleStateActive(int state) {
    return state == 1 || state == 2;
}

BOOL SavedCharacter::IsInTroubleWithoutMood() {
    BOOL inTrouble = IsInTrouble() & 1;
    return inTrouble & (data_02114d78[mTroubleCategory][mTroubleId].mood == 0);
}

void SavedCharacter::SetTrouble(u8 state, u8 category, u8 id) {
    mTroubleState = state;
    mTroubleCategory = category;
    mTroubleId = id;
}

BOOL SavedCharacter::IsProgressingTrouble() {
    BOOL result = FALSE;
    if (data_02114d78[mTroubleCategory][mTroubleId].kind == 11) {
        int state = mTroubleState;
        BOOL active = state == 1 || state == 2 || state == 4;
        if (active) {
            result = TRUE;
        }
    }
    return result;
}

BOOL SavedCharacter::IsProgressingTroubleInState2() {
    return data_02114d78[mTroubleCategory][mTroubleId].kind == 11 && mTroubleState == 2;
}

void SavedCharacter::ResetTroubleProgress() {
    if (IsProgressingTrouble()) {
        mTroubleProgress = gStr_Parenthesis_2[mTroubleId];
    } else {
        mTroubleProgress = 0;
    }
}

void SavedCharacter::AddTroubleProgress(int value) {
    if (!IsProgressingTrouble()) {
        return;
    }
    if (value < 0) {
        value = 0;
    }
    int progress = mTroubleProgress + value;
    if (progress > 200) {
        progress = 200;
    } else if (progress < 0) {
        progress = 0;
    }
    mTroubleProgress = progress;
}

int SavedCharacter::GetTroubleTimeProgress(s64 now, s64 start) {
    s64 elapsed = now - start;
    if (elapsed < 0) {
        elapsed = 0;
    }
    s64 progress = elapsed >> 11;
    int result = progress;
    if (progress > 150) {
        result = 150;
    } else if (progress < 0) {
        result = 0;
    }
    return result;
}

u32 SavedCharacter::GetTroubleProgressAt(s64 now, s64 start, int progress) {
    return GetTroubleTimeProgress(now, start) + progress;
}

u32 SavedCharacter::GetTroubleProgress(s64 now) {
    return GetTroubleProgressAt(now, mTroubleStart, mTroubleProgress);
}

BOOL SavedCharacter::IsTroubleProgressFullAt(s64 now, s64 start, int progress) {
    return GetTroubleProgressAt(now, start, progress) >= 200;
}

BOOL SavedCharacter::IsTroubleProgressFull(s64 now) {
    return GetTroubleProgress(now) >= 200;
}

BOOL SavedCharacter::IsTroubleOfCategory34OrKind20() {
    if (mTroubleState != 0) {
        u8 category = mTroubleCategory;
        if (category == 3 || category == 4 || data_02114d78[category][mTroubleId].kind == 20) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL SavedCharacter::IsInFightOrSeparationTrouble() {
    if (!IsInTrouble()) {
        return FALSE;
    }
    u8 category = mTroubleCategory;
    if (category == 3) {
        switch (data_02114d78[category][mTroubleId].kind) {
        case 8:
        case 9:
        case 10:
            return TRUE;
        }
    }
    return FALSE;
}

BOOL SavedCharacter::IsTroubleOfCategory34OrKind18() {
    if (!IsInTrouble()) {
        return FALSE;
    }
    u8 category = mTroubleCategory;
    switch (category) {
    case 3:
    case 4:
        return TRUE;
    }
    switch (data_02114d78[category][mTroubleId].kind) {
    case 18:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsAngryAfterFight() {
    return IsAngryAfterFightState(mTroubleState, mTroubleCategory, mTroubleId);
}

BOOL SavedCharacter::IsAngryAfterFightState(int state, int category, int id) {
    if (state != 0 && category == 3 && id == 4) {
        return TRUE;
    }
    return FALSE;
}

// How many more fights the island allows: none up to one resident, then 2, 4 or 6 (above 30 and 70 residents),
// less the fights going on; 0 when that is not more than one
int SavedCharacter::GetFightsAllowed(int fights, int residents) {
    int allowed = -fights;
    if (residents <= 1) {
        allowed = 0;
    } else if (residents <= 30) {
        allowed += 2;
    } else if (residents <= 70) {
        allowed += 4;
    } else {
        allowed += 6;
    }
    if (allowed <= 1) {
        allowed = 0;
    }
    return allowed;
}

inline int SavedCharacter::GetDaysBetween(RTCDate from, RTCDate to) {
    return GetDaysBetween(from.month, from.day, from.year, to.month, to.day, to.year);
}

inline void SavedCharacter::FinishTroubleUnsolved() {
    mTroubleEnd += data_02114d78[mTroubleCategory][mTroubleId].cooldownMinutes * 60;
    mTroubleState = 5;
}

inline void SavedCharacter::StartTroubleWith(int category, int id, SavedCharacter *other, s64 now) {
    ClearTrouble();
    SetTrouble(1, category, id);
    mTroubleTarget = other->mIndex;
    mTroubleEnd = now + data_02114d78[mTroubleCategory][mTroubleId].minutes * 60;
    mMood = 0;
}

// Heartbroken (category 4) by the other, for the trouble's whole time
inline void SavedCharacter::StartHeartbreak(int id, SavedCharacter *other, s64 now) {
    ClearTrouble();
    mTroubleCategory = 4;
    mTroubleId = id;
    mTroubleState = 1;
    mTroubleTarget = other->mIndex;
    mTroubleEnd = now + data_02114d78[mTroubleCategory][mTroubleId].minutes * 60;
    mMood = 3;
    ResetTroubleProgress();
    mSatisfyMeter = 0;
    mLongActEnd = now;
}

inline void SavedCharacter::BeginTrouble(int category, int id, u16 target, s64 now, BOOL shortened,
                                         SavedCharacter *partner) {
    mTroubleState = 1;
    mTroubleId = id;
    mTroubleCategory = category;
    mTroubleTarget = target;
    mTroubleEnd = now + GetTroubleDuration(shortened);
    SetTroubleMood(data_02114d78[mTroubleCategory][mTroubleId].mood);
    int mood = data_02114d78[mTroubleCategory][mTroubleId].mood;
    switch (mood) {
    case 2:
    case 3:
    case 4:
        mLongActEnd = now;
        if (partner != NULL) {
            partner->mLongActEnd = now;
        }
        break;
    }
}
// When the trouble's time is over: one still open fails (FailTrouble), one that was solved or failed is cleared
void SavedCharacter::ExpireTrouble(s64 now, SavedCharacter *partner) {
    BOOL changed = FALSE;
    switch (data_02114d78[mTroubleCategory][mTroubleId].mood) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        if (mMood != 0) {
            mMood = 0;
            changed = TRUE;
            switch (mLongActState) {
            case 6:
            case 13:
            case 14:
                mLongActEnd = now;
                break;
            }
        }
        break;
    }
    switch (mTroubleState) {
    case 0:
        break;
    case 1:
    case 2:
        FailTrouble(now, partner);
        changed = TRUE;
        break;
    case 3:
    case 4:
    case 5:
        ClearTrouble();
        changed = TRUE;
        break;
    }
    if (changed) {
        GameData_WriteCharacter(&gGameData, mIndex, this);
    }
}

// What comes of a trouble that nobody solved in time (now -1: at its end)
void SavedCharacter::FailTrouble(s64 now, SavedCharacter *target) {
    if (now == -1) {
        now = mTroubleEnd;
    }
    SavedCharacter other;
    if (IsInFightOrSeparationTrouble() && target == NULL) {
        u16 index = mTroubleTarget;
        target = &other;
        GameData_ReadCharacter(&gGameData, index, target);
        other.mIndex = index;
    }
    const TroubleInfo *info = &data_02114d78[mTroubleCategory][mTroubleId];
    switch (info->kind) {
    case 1:
        EatRandomFood();
        return;
    case 8:
        FailFightTrouble(now, target);
        GameData_WriteCharacter(&gGameData, target->mIndex, target);
        return;
    case 9:
        BreakUp(now, target);
        GameData_WriteCharacter(&gGameData, target->mIndex, target);
        return;
    case 10:
        Divorce(now, target);
        GameData_WriteCharacter(&gGameData, target->mIndex, target);
        return;
    default:
        mTroubleEnd += info->cooldownMinutes * 60;
        mTroubleState = 5;
        return;
    }
}

// A food trouble nobody solved: the character eats some food of the goods table by itself
void SavedCharacter::EatRandomFood() {
    int food;
    int foods[255];
    int count = 0;
    for (int i = 0; i < 255; i++) {
        if (gGoodsTable[(u16)i].mKind == 1) {
            foods[count] = i;
            count++;
        }
    }
    food = foods[MATH_Rand16(&gRandom, count)];
    if (CanEat(mTroubleEnd, food)) {
        Eat(mTroubleEnd, food);
        mEatLastFood = food;
    }
    FinishTroubleUnsolved();
}

// An angry trouble after a fight nobody settled: friends part, lovers and spouses each want to leave the other
void SavedCharacter::FailFightTrouble(s64 now, SavedCharacter *other) {
    switch (mRelationRank[other->mIndex]) {
    case RELATION_FRIENDS_FIGHTING:
        SetRelation(other->mIndex, RELATION_NONE, 100);
        other->SetRelation(mIndex, RELATION_NONE, 100);
        mMood = 0;
        FinishTroubleUnsolved();
        other->mMood = 0;
        other->FinishTroubleUnsolved();
        break;
    case RELATION_LOVERS_FIGHTING:
        StartTroubleWith(3, 5, other, now);
        other->StartTroubleWith(3, 5, this, now);
        break;
    case RELATION_SPOUSES_FIGHTING:
        StartTroubleWith(3, 6, other, now);
        other->StartTroubleWith(3, 6, this, now);
        break;
    }
}

// A wish to break up nobody talked out of: the two are former lovers, and the other one is heartbroken
void SavedCharacter::BreakUp(s64 now, SavedCharacter *other) {
    SetRelation(other->mIndex, RELATION_EX_LOVERS, 100);
    other->SetRelation(mIndex, RELATION_EX_LOVERS, 100);
    FinishTroubleUnsolved();
    if (!other->IsTroubleOfCategory34OrKind20() || other->mTroubleCategory == 4 ||
        data_02114d78[other->mTroubleCategory][other->mTroubleId].kind == 4) {
        if (!IsLongActPaired()) {
            other->StartHeartbreak(1, this, now);
            return;
        }
    }
    if (data_02114d78[other->mTroubleCategory][other->mTroubleId].kind == 9) {
        other->ClearTrouble();
    }
}

// The same for a divorce
void SavedCharacter::Divorce(s64 now, SavedCharacter *other) {
    SetRelation(other->mIndex, RELATION_DIVORCED, 100);
    other->SetRelation(mIndex, RELATION_DIVORCED, 100);
    FinishTroubleUnsolved();
    if (!other->IsTroubleOfCategory34OrKind20() || other->mTroubleCategory == 4 ||
        data_02114d78[other->mTroubleCategory][other->mTroubleId].kind == 4) {
        if (!IsLongActPaired()) {
            other->StartHeartbreak(2, this, now);
            return;
        }
    }
    if (data_02114d78[other->mTroubleCategory][other->mTroubleId].kind == 10) {
        other->ClearTrouble();
    }
}

void SavedCharacter::StartLongAct(int state, s64 now, RTCTime *time, SavedCharacter *first, SavedCharacter *second) {
    if (IsLongActAsPartnerState(state)) {
        u16 special[2];
        u16 candidates[100];
        int count = 0;
        MI_CpuFill16(special, 0xFFFF, sizeof(special));
        for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
            if (GameData_CharacterExists(&gGameData, i) && !GameData_GetCharacterEvent(&gGameData, i) && i != mIndex &&
                Is0Or1(GameData_GetCharacterMood(&gGameData, i))) {
                u8 rank = mRelationRank[i];
                if (!IsFightingRelation(rank) && rank != 0 && rank != 3 && rank != 5) {
                    if (IsCoupleRelation(rank)) {
                        special[0] = i;
                    }
                    if (rank == 9) {
                        special[1] = i;
                    }
                    candidates[count] = i;
                    count++;
                }
            }
        }
        if (count >= 1) {
            int partner = -1;
            int j;
            for (j = 0; j < 2; j++) {
                if (special[j] != 0xFFFF && MATH_Rand16(&gRandom, 100) < 75 - j * 25) {
                    partner = special[j];
                    if (j == 0 && mRelationMeter[partner] > 50 && GameData_GetCharacterAffinity(&gGameData, partner, mIndex) > 50) {
                        int random = MATH_Rand16(&gRandom, 100);
                        if (random < 30) {
                            state = 0x2C;
                        } else if (random < 50) {
                            state = 0x2E;
                        }
                    }
                    break;
                }
            }
            if (partner == -1) {
                partner = candidates[MATH_Rand16(&gRandom, count)];
            }
            s64 end = now + GetLongActDuration(state, 3);
            SetLongAct(state, partner, end);
            SavedCharacter other;
            if (first == NULL || partner != first->mIndex) {
                if (second != NULL && partner == second->mIndex) {
                    first = second;
                } else {
                    GameData_ReadCharacter(&gGameData, partner, &other);
                    first = &other;
                }
            }
            u8 partnerState;
            if (state == 0x18) {
                partnerState = 0x17;
            } else if (state == 0x1A) {
                partnerState = 0x19;
            } else if (state == 0x1C) {
                partnerState = 0x1B;
            } else if (state == 0x1E) {
                partnerState = 0x1D;
            } else if (state == 0x20) {
                partnerState = 0x1F;
            } else if (state == 0x22) {
                partnerState = 0x21;
            } else if (state == 0x24) {
                partnerState = 0x23;
            } else if (state == 0x26) {
                partnerState = 0x25;
            } else if (state == 0x28) {
                partnerState = 0x27;
            } else if (state == 0x2A) {
                partnerState = 0x29;
            } else if (state == 0x2C) {
                partnerState = 0x2B;
            } else if (state == 0x2E) {
                partnerState = 0x2D;
            }
            first->SetLongAct(partnerState, mIndex, end);
            GameData_WriteCharacter(&gGameData, partner, first);
        } else {
            SetLongAct(0, 0, now + GetLongActDuration(0, 3));
        }
    } else if (state == 0x13) {
        int days = MATH_Rand16(&gRandom, 4) + 2;
        SetLongAct(state, days, now + GetLongActDuration(state, 3));
    } else if (state == 0x14) {
        int choices[4];
        int count = 0;
        if (((SaveHeader *)gIslandState)->GetUnknown44(3)) {
            choices[count] = 0;
            count++;
        }
        if (((SaveHeader *)gIslandState)->GetUnknown44(10)) {
            choices[count] = 1;
            count++;
        }
        if (((SaveHeader *)gIslandState)->GetUnknown44(11)) {
            choices[count] = 2;
            count++;
        }
        if (((SaveHeader *)gIslandState)->GetUnknown44(8)) {
            choices[count] = 3;
            count++;
        }
        if (count <= 0 || count > 4) {
            ClearLongActFrom(now);
        } else {
            int choice = MATH_Rand16(&gRandom, count);
            SetLongAct(state, choices[choice], now + GetLongActDuration(state, 3));
        }
    } else if (state == 0x15 || state == 0x16) {
        SetLongAct(state, 0, now + GetLongActDuration(state, 4));
    } else if (state == 0 || state == 0x17) {
        if (time->hour < 24 && MATH_Rand16(&gRandom, 100) < gStr_AFKPUUA[time->hour]) {
            s64 end = now + GetLongActDuration(0x12, 3);
            SetLongAct(0x12, 0, end + (s64)data_020f8ca8[time->hour] * 3600);
            SetUnknown246ByChance();
        } else {
            SetLongAct(0, 0, now + GetLongActDuration(0, 3));
        }
    } else if (state == 0xF) {
        SetLongAct(state, 0, now + GetLongActDuration(state, 3));
        SetUnknown246ByChance();
    } else if (state == 0x11) {
        if (mTroubleState == 0) {
            SetLongAct(state, 0, now + GetLongActDuration(state, 3));
        } else {
            ClearLongActFrom(now);
        }
    } else {
        SetLongAct(state, 0, now + GetLongActDuration(state, 3));
    }
    GameData_WriteCharacter(&gGameData, mIndex, this);
}

int SavedCharacter::ChooseTroubleCategory() {
    int total = 0;
    for (int i = 0; i < 10; i++) {
        total += GetTroubleCategoryWeight(i);
    }
    int random = MATH_Rand16(&gRandom, total);
    u8 category = 0;
    int sum = 0;
    for (; category < 10; category++) {
        sum += GetTroubleCategoryWeight(category);
        if (random < sum) {
            break;
        }
    }
    if (category >= 10) {
        category = 0;
    }
    return category;
}

int SavedCharacter::GetTroubleCategoryWeight(int category) {
    u8 weight = data_020f8d8c[category];
    switch (category) {
    case 0:
        break;
    case 1:
        if (((u8 *)&gGameData)[0xB2] < 5) {
            weight = 0;
        }
        break;
    case 2:
        if (!((SaveHeader *)gIslandState)->GetUnknown44(10)) {
            weight = 0;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    case 8:
        if (!((SaveHeader *)gIslandState)->GetUnknown44(11)) {
            weight = 0;
        }
        break;
    }
    return weight;
}

static inline const TroubleInfo *GetTroubleInfo(int category, int id) {
    return &data_02114d78[category][id];
}

int SavedCharacter::ChooseTrouble(int category) {
    int total = 0;
    u32 count = gStr_LessThan_2[category];
    for (u32 i = 0; i < count; i++) {
        total += GetTroubleInfo(category, i)->weight;
    }
    int random = MATH_Rand16(&gRandom, total);
    u8 id = 0;
    int sum = 0;
    for (; id < gStr_LessThan_2[category]; id++) {
        sum += GetTroubleInfo(category, id)->weight;
        if (random < sum) {
            break;
        }
    }
    if (id >= count) {
        id = 0;
    }
    return id;
}

void SavedCharacter::EndTrouble(s64 now) {
    mTroubleState = 4;
    mTroubleEnd = now + data_02114d78[mTroubleCategory][mTroubleId].cooldownMinutes * 60;
    mMood = 0;
    if (!IsLongActPaired()) {
        mLongActEnd = now;
    }
    GameData_WriteCharacter(&gGameData, mIndex, this);
}

void SavedCharacter::ClearTrouble() {
    mTroubleState = 0;
    mTroubleCategory = 0;
    mTroubleId = 0;
    mTroubleTarget = 0;
    mTroubleProgress = 0;
    mTroubleEnd = 0;
    mTroubleStart = 0;
}

BOOL SavedCharacter::StartRandomTrouble(s64 now, int force, SavedCharacter *partner, int unused) {
    if (mTroubleState != 0) {
        return FALSE;
    }
    int category = ChooseTroubleCategory();
    return StartTrouble(category, ChooseTrouble(category), now, force, partner, unused);
}

BOOL SavedCharacter::StartTrouble(int category, int id, s64 now, BOOL shortened, SavedCharacter *partner,
                                  BOOL allowFight) {
    const TroubleInfo *info = &data_02114d78[category][id];
    if (info->hasTarget == 0) {
        int kind = info->kind;
        if (kind == 1) {
            if (GetFullness(now) > 0) {
                return FALSE;
            }
        } else if (kind == 3) {
            if (CountOwnedClothes() >= CLOTHES_SIZE) {
                return FALSE;
            }
            if (MiiData_GetSex(this) == 0) {
                u32 flags = data_02114d78[category][id].flags;
                if (flags & 0x40 || flags & 0x80 || flags & 0x1000) {
                    return FALSE;
                }
            }
            u32 flags = data_02114d78[category][id].flags;
            if (flags & 1) {
                RTCDate date;
                RTCTime time;
                RTC_ConvertSecondToDateTime(&date, &time, now);
                if (date.month >= 4 && date.month <= 9) {
                    return FALSE;
                }
            } else if (flags & 2) {
                RTCDate date;
                RTCTime time;
                RTC_ConvertSecondToDateTime(&date, &time, now);
                if (date.month <= 3 || date.month >= 10) {
                    return FALSE;
                }
            }
        } else if (kind == 19) {
            if (CountOwnedInteriors() >= 71) {
                return FALSE;
            }
        }
        BeginTrouble(category, id, 0, now, shortened, partner);
        return TRUE;
    }
    int kind = info->kind;
    u16 target;
    BOOL ok = TRUE;
    if (kind == 1) {
        if (GetFullness(now) > 0) {
            ok = FALSE;
        } else {
            int food = ChooseWantedFood();
            if (food != -1) {
                target = food;
            } else {
                ok = FALSE;
            }
        }
    } else if (kind == 3) {
        if (CountOwnedClothes() >= CLOTHES_SIZE) {
            ok = FALSE;
        } else {
            int clothes = ChooseWantedClothes(now);
            if (clothes != -1) {
                target = clothes;
            } else {
                ok = FALSE;
            }
        }
    } else if (kind == 4) {
        int other = ChooseNewFriend();
        if (other != -1) {
            target = other;
        } else {
            ok = FALSE;
        }
    } else if (kind == 5) {
        int other = ChooseLoveInterest();
        if (other != -1) {
            target = other;
        } else {
            ok = FALSE;
        }
    } else if (kind == 6 || kind == 7) {
    } else if (kind == 8) {
        int residents;
        int fights;
        CountFights(&residents, &fights);
        if (!allowFight || GetFightsAllowed(fights, residents) <= 1) {
            ok = FALSE;
        } else {
            int other = ChooseFightOpponent();
            if (other != -1) {
                target = other;
            } else {
                ok = FALSE;
            }
        }
    } else if (kind == 9) {
        int other = ChooseLoverToLeave();
        if (other != -1) {
            target = other;
        } else {
            ok = FALSE;
        }
    } else if (kind == 10) {
        int other = ChooseSpouseToLeave();
        if (other != -1) {
            target = other;
        } else {
            ok = FALSE;
        }
    } else if (kind == 18) {
        int other = ChooseCharacterByRelation();
        if (other != -1) {
            target = other;
        } else {
            ok = FALSE;
        }
    }
    if (ok) {
        BeginTrouble(category, id, target, now, shortened, partner);
        return TRUE;
    }
    return FALSE;
}

// How long a new trouble lasts: from a half (or, shortened, a quarter) of its table time to all of it, at random
s64 SavedCharacter::GetTroubleDuration(BOOL shortened) {
    int seconds = data_02114d78[mTroubleCategory][mTroubleId].minutes * 60;
    if (shortened) {
        return (u16)MATH_Rand16(&gRandom, seconds - (seconds >> 2)) + (seconds >> 2);
    }
    return (u16)MATH_Rand16(&gRandom, seconds - (seconds >> 1)) + (seconds >> 1);
}

BOOL SavedCharacter::SetTroubleMood(int mood) {
    switch (mood) {
    case 0:
        mMood = 0;
        break;
    case 1:
        mMood = 1;
        break;
    case 2:
        mMood = 2;
        break;
    case 3:
        mMood = 3;
        break;
    case 4:
        mMood = 4;
        break;
    }
    return TRUE;
}

// A food trouble's wish: a food of the goods table that is not one of the bad ones, at random; -1 for none
int SavedCharacter::ChooseWantedFood() {
    u16 foods[255];
    int count = 0;
    for (u16 i = 0; i < 255; i = (u16)(i + 1)) {
        const ItemInfo *item = &gGoodsTable[i];
        if (item->mKind == 1 && !(item->mFlags & ITEM_BAD_FOOD)) {
            foods[count] = i;
            count++;
        }
    }
    if (count > 0) {
        return foods[MATH_Rand16(&gRandom, count)];
    }
    return -1;
}

static inline BOOL IsSummer(int month) {
    BOOL summer = FALSE;
    if (month >= 4 && month <= 9) {
        summer = TRUE;
    }
    return summer;
}

static inline BOOL IsWinter(int month) {
    BOOL winter = TRUE;
    if (month > 3 && month < 10) {
        winter = FALSE;
    }
    return winter;
}

// A clothes trouble's wish: clothes it does not own, for the season (by the month of now) and its sex; -1 for none
int SavedCharacter::ChooseWantedClothes(s64 now) {
    RTCDate date;
    RTCTime time;
    u16 clothes[CLOTHES_SIZE];
    int count = 0;
    RTC_ConvertSecondToDateTime(&date, &time, now);
    BOOL summer = FALSE;
    if (date.month >= 4 && date.month <= 9) {
        summer = TRUE;
    }
    BOOL winter = TRUE;
    if (date.month > 3 && date.month < 10) {
        winter = FALSE;
    }
    for (u16 i = 0; i < CLOTHES_SIZE; i = (u16)(i + 1)) {
        if ((mOwnedClothes.m[i >> 5] >> (i & 0x1F)) & 1) {
            continue;
        }
        if (summer && gClothesTable[i].mStyles & CLOTHES_WARM) {
            continue;
        }
        if (winter && gClothesTable[i].mStyles & CLOTHES_COOL) {
            continue;
        }
        if (MiiData_GetSex(this) == 0 && gClothesTable[i].mSex == 2) {
            continue;
        }
        if (MiiData_GetSex(this) == 1 && gClothesTable[i].mSex == 1) {
            continue;
        }
        clothes[count] = i;
        count++;
    }
    if (count > 0) {
        return clothes[MATH_Rand16(&gRandom, count)];
    }
    return -1;
}

// Whom it wants to make friends with: another resident it has no relation to; -1 for none, and half of the time
// when it has more than five friends
int SavedCharacter::ChooseNewFriend() {
    u16 others[100];
    int count = 0;
    int friends = 0;
    for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex) {
            u8 rank = mRelationRank[i];
            if (rank == RELATION_NONE) {
                others[count] = i;
                count++;
            } else if (rank == RELATION_FRIENDS) {
                friends++;
            }
        }
    }
    int other = -1;
    if (count > 0) {
        other = others[MATH_Rand16(&gRandom, count)];
    }
    if (friends > 5 && Rand(100) < 50) {
        other = -1;
    }
    return other;
}

// Whom it fights with: a friend, lover or spouse who is not in trouble; the last lover or spouse found a fifth of
// the time
int SavedCharacter::ChooseFightOpponent() {
    u16 others[100];
    int count = 0;
    int partners = 0;
    int partner;
    for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && !GameData_GetCharacterTroubleState(&gGameData, i) &&
            IsFriendsLoversOrSpouses(mRelationRank[i])) {
            others[count] = i;
            count++;
            if (mRelationRank[i] == RELATION_LOVERS || mRelationRank[i] == RELATION_SPOUSES) {
                partner = i;
                partners++;
            }
        }
    }
    int result = -1;
    if (partners > 0 && Rand(100) < 20) {
        return partner;
    }
    if (count > 0) {
        result = others[MATH_Rand16(&gRandom, count)];
    }
    return result;
}

// Whom it wants to break up with: a lover it likes no more than 50, or one it is fighting with; -1 for none
int SavedCharacter::ChooseLoverToLeave() {
    int other = -1;
    for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
        u8 rank = mRelationRank[i];
        BOOL cooled = FALSE;
        if (rank == RELATION_LOVERS && mRelationMeter[i] <= 50) {
            cooled = TRUE;
        }
        BOOL fighting = rank == RELATION_LOVERS_FIGHTING ? TRUE : FALSE;
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && (fighting || cooled)) {
            other = i;
            break;
        }
    }
    return other;
}

// The same for a spouse
int SavedCharacter::ChooseSpouseToLeave() {
    int other = -1;
    for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
        u8 rank = mRelationRank[i];
        BOOL cooled = FALSE;
        if (rank == RELATION_SPOUSES && mRelationMeter[i] <= 50) {
            cooled = TRUE;
        }
        BOOL fighting = rank == RELATION_SPOUSES_FIGHTING ? TRUE : FALSE;
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && (fighting || cooled)) {
            other = i;
            break;
        }
    }
    return other;
}

// The number of residents, and of those angry after a fight
void SavedCharacter::CountFights(int *residents, int *fights) {
    *residents = 0;
    *fights = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_CharacterExists(&gGameData, i)) {
            (*residents)++;
            if (IsAngryAfterFightState(GameData_GetCharacterTroubleState(&gGameData, i), GameData_GetCharacterTroubleCategory(&gGameData, i),
                                  GameData_GetCharacterTroubleId(&gGameData, i))) {
                (*fights)++;
            }
        }
    }
}

// Meeting one who is angry at it after a fight, the character is angry back until the other's trouble ends
void SavedCharacter::BecomeAngryAt(SavedCharacter *other, s64 now, SavedCharacter *partner) {
    if (!other->IsAngryAfterFight()) {
        return;
    }
    s64 end = other->mTroubleEnd;
    ClearTrouble();
    SetTrouble(1, 3, 4);
    mTroubleTarget = other->mIndex;
    mTroubleEnd = end;
    mMood = 2;
    mLongActEnd = now;
    if (partner != NULL) {
        partner->mLongActEnd = now;
    }
}

// Whom it is in love with: a friend or best friend of the other sex it likes 140 or more, that the blood relations
// allow; -1 for none, and when it has a lover or spouse already
int SavedCharacter::ChooseLoveInterest() {
    u16 others[100];
    int count = 0;
    if (FindCouplePartner() != -1) {
        return -1;
    }
    for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
        if (GameData_CharacterExists(&gGameData, i) && i != mIndex && MiiData_GetSex(this) != GameData_GetCharacterSex(&gGameData, i) &&
            GetRelationBloodPairValue(mRelationBlood, GameData_GetCharacterRelationToPlayer(&gGameData, i)) != 0) {
            u8 rank = mRelationRank[i];
            if ((rank == RELATION_FRIENDS || rank == RELATION_BEST_FRIENDS) && mRelationMeter[i] >= 140) {
                others[count] = i;
                count++;
            }
        }
    }
    if (count > 0) {
        return others[MATH_Rand16(&gRandom, count)];
    }
    return -1;
}

// Whom a trouble of kind 18 is about: first a group by the weights of data_020f8d7c (its lover or spouse, its best
// friend, friends of the other sex, of its own, others of the other sex, of its own), then one of the group
int SavedCharacter::ChooseCharacterByRelation() {
    u8 groups[4][100];
    int counts[4] = {0, 0, 0, 0};
    int partner = -1;
    int bestFriend = -1;
    int result;
    for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
        if (!GameData_CharacterExists(&gGameData, i) || i == mIndex) {
            continue;
        }
        if (IsLoversOrSpouses(mRelationRank[i])) {
            partner = i;
        } else if (mRelationRank[i] == RELATION_BEST_FRIENDS) {
            bestFriend = i;
        } else if (mRelationRank[i] == RELATION_FRIENDS) {
            if (MiiData_GetSex(this) != GameData_GetCharacterSex(&gGameData, i)) {
                groups[0][counts[0]] = i;
                counts[0]++;
            } else {
                groups[1][counts[1]] = i;
                counts[1]++;
            }
        } else if (mRelationRank[i] == RELATION_NONE) {
            if (MiiData_GetSex(this) != GameData_GetCharacterSex(&gGameData, i)) {
                groups[2][counts[2]] = i;
                counts[2]++;
            } else {
                groups[3][counts[3]] = i;
                counts[3]++;
            }
        }
    }
    int total = 0;
    int sum = 0;
    for (int j = 0; j < 6; j++) {
        int weight = data_020f8d7c[j];
        switch (j) {
        case 0:
            if (partner == -1) {
                weight = 0;
            }
            break;
        case 1:
            if (bestFriend == -1) {
                weight = 0;
            }
            break;
        case 2:
            if (counts[0] < 1) {
                weight = 0;
            }
            break;
        case 3:
            if (counts[1] < 1) {
                weight = 0;
            }
            break;
        case 4:
            if (counts[2] < 1) {
                weight = 0;
            }
            break;
        case 5:
            if (counts[3] < 1) {
                weight = 0;
            }
            break;
        }
        total += weight;
    }
    if (total <= 0) {
        return -1;
    }
    int random = MATH_Rand16(&gRandom, total);
    int j;
    for (j = 0; j < 6; j++) {
        int weight = data_020f8d7c[j];
        switch (j) {
        case 0:
            if (partner == -1) {
                weight = 0;
            }
            break;
        case 1:
            if (bestFriend == -1) {
                weight = 0;
            }
            break;
        case 2:
            if (counts[0] < 1) {
                weight = 0;
            }
            break;
        case 3:
            if (counts[1] < 1) {
                weight = 0;
            }
            break;
        case 4:
            if (counts[2] < 1) {
                weight = 0;
            }
            break;
        case 5:
            if (counts[3] < 1) {
                weight = 0;
            }
            break;
        }
        sum += weight;
        if (random < sum) {
            break;
        }
    }
    if (j >= 6) {
        return -1;
    }
    switch (j) {
    case 0:
        result = partner;
        break;
    case 1:
        result = bestFriend;
        break;
    case 2:
        result = groups[0][Rand(counts[0])];
        break;
    case 3:
        result = groups[1][Rand(counts[1])];
        break;
    case 4:
        result = groups[2][Rand(counts[2])];
        break;
    case 5:
        result = groups[3][Rand(counts[3])];
        break;
    }
    return result;
}

BOOL SavedCharacter::AddSatisfaction(int value) {
    BOOL levelUp = FALSE;
    int meter = value + mSatisfyMeter;
    if (meter >= 100) {
        levelUp = TRUE;
        int level = mSatisfyLevel;
        if (level >= 99) {
            mSatisfyMeter = meter - 100;
        } else {
            mSatisfyMeter = meter - 100;
            mSatisfyLevel++;
        }
    } else {
        if (meter < 0) {
            meter = levelUp;
        }
        mSatisfyMeter = meter;
    }
    return levelUp;
}

void SavedCharacter::ResetSatisfaction() {
    mSatisfyMeter = 0;
    mSatisfyLevel = 1;
}

int SavedCharacter::GetFullness(s64 now) {
    s64 elapsed = now - mUnknown23C;
    if (elapsed < 0) {
        mUnknown23C = now;
        elapsed = 0;
    }
    int digested = elapsed / 3600 * 10; // Ten an hour
    int fullness = mEatFullness - digested;
    if (fullness > 100) {
        return 100;
    }
    if (fullness < 0) {
        return 0;
    }
    return fullness;
}

// How much eating the food fills
static inline int GetFoodSize(u16 item) {
    return sFoodSizes[gGoodsTable[item].mFoodSize];
}

BOOL SavedCharacter::CanEat(s64 now, u16 item) {
    return GetFullness(now) + GetFoodSize(item) < 130;
}

void SavedCharacter::Eat(s64 now, u16 item) {
    int fullness = GetFullness(now) + GetFoodSize(item);
    if (fullness >= 130) {
        return;
    }
    if (fullness > 100) {
        fullness = 100;
    } else if (fullness < 0) {
        fullness = 0;
    }
    mEatFullness = fullness;
    mUnknown23C = now;
}

void SavedCharacter::ResetEating() {
    mEatFullness = 0;
    mUnknown23C = 0;
    mEatLastFood = 0xFF;
}

void SavedCharacter::ClearUnknown246() {
    mUnknown246 = 0;
}

void SavedCharacter::SetUnknown246ByChance() {
    if (Rand(100) < 50) {
        mUnknown246 = 1;
    }
}

void SavedCharacter::SetMeetDate(const RTCDate *date) {
    mMeetYear = date->year;
    mMeetMonth = date->month;
    mMeetDay = date->day;
}

void SavedCharacter::GetMeetDate(RTCDate *date) {
    date->year = mMeetYear;
    date->month = mMeetMonth;
    date->day = mMeetDay;
    date->week = RTC_WEEK_SUNDAY;
}

void SavedCharacter::ResetMeetDate() {
    mMeetYear = 0;
    mMeetMonth = 1;
    mMeetDay = 1;
}

void SavedCharacter::ResetTouchTalk() {
    mUnknown24A = 0;
    mTouchTalkMonth = 1;
    mTouchTalkDay = 1;
    mTouchTalkHour = 0;
}

void SavedCharacter::ClearUnknown1C8() {
    MI_CpuClear32(&mUnknown1C8, sizeof(mUnknown1C8));
}

u32 SavedCharacter::CountOwnedClothes() {
    int count = 0;
    for (u32 i = 0; i < 10; i++) {
        count += MATH_CountPopulation(mOwnedClothes.m[i]);
    }
    return count;
}

void SavedCharacter::ClearOwnedClothes() {
    MI_CpuClear32(&mOwnedClothes, sizeof(mOwnedClothes));
}

u8 SavedCharacter::GetSongSlot(int slot) {
    u8 song = mSongSlots[slot];
    if (!((mUnknown299 >> (slot & 0x1F)) & 1) || song == 0xFF) {
        return 0xFF;
    }
    return song;
}

void SavedCharacter::SetSongSlot(int slot, u8 song) {
    mSongSlots[slot] = song;
}

void SavedCharacter::ClearSongSlots() {
    MI_CpuFill8(mSongSlots, 0xFF, sizeof(mSongSlots));
    mUnknown299 = 0;
}

int SavedCharacter::GetTaste(int item) {
    int kind;
    int offset;
    int taste;
    GetCreateIdParts(&kind, &offset);
    switch (kind) {
    case 0:
        taste = gGoodsTable[item].mTaste0;
        break;
    case 1:
        taste = gGoodsTable[item].mTaste1;
        break;
    case 2:
        taste = gGoodsTable[item].mTaste2;
        break;
    case 3:
        taste = gGoodsTable[item].mTaste3;
        break;
    }
    taste += (u32)(item + offset) % 7 - 3;
    if (taste > 15) {
        return 15;
    }
    if (taste < 0) {
        return 0;
    }
    return taste;
}

int SavedCharacter::GetLiking(int item) {
    int liking = GetTaste(item) + 1;
    if (item == mFavoriteItem) {
        liking = 17;
    }
    if (item == mDislikedItem) {
        liking = 0;
    }
    return liking;
}

// How much the character likes clothes, 0 to 15: their taste for its kind of personality, give or take three by the
// create ID, halved for a man in women's clothes and two less for a woman in men's
int SavedCharacter::GetClothesLiking(int clothes) {
    int high;
    int low;
    int liking;
    GetCreateIdParts(&high, &low);
    switch (gPersonalityKinds[mPersonality]) {
    case 3:
        liking = gClothesTable[clothes].mTaste3;
        break;
    case 1:
        liking = gClothesTable[clothes].mTaste1;
        break;
    case 2:
        liking = gClothesTable[clothes].mTaste2;
        break;
    case 0:
        liking = gClothesTable[clothes].mTaste0;
        break;
    }
    liking += (u32)(clothes + low) % 7 - 3;
    if (MiiData_GetSex(this) == 0 && gClothesTable[clothes].mSex == 2) {
        liking >>= 1;
    } else if (MiiData_GetSex(this) == 1 && gClothesTable[clothes].mSex == 1) {
        liking -= 2;
    }
    if (liking > 15) {
        liking = 15;
    } else if (liking < 0) {
        liking = 0;
    }
    return liking;
}

// The same for a room's interior
int SavedCharacter::GetInteriorLiking(int interior) {
    int high;
    int low;
    int liking;
    GetCreateIdParts(&high, &low);
    switch (gPersonalityKinds[mPersonality]) {
    case 3:
        liking = gInteriorTable[interior].mTaste3;
        break;
    case 1:
        liking = gInteriorTable[interior].mTaste1;
        break;
    case 2:
        liking = gInteriorTable[interior].mTaste2;
        break;
    case 0:
        liking = gInteriorTable[interior].mTaste0;
        break;
    }
    liking += (u32)(interior + low) % 7 - 3;
    if (MiiData_GetSex(this) == 0 && gInteriorTable[interior].mSex == 2) {
        liking >>= 1;
    } else if (MiiData_GetSex(this) == 1 && gInteriorTable[interior].mSex == 1) {
        liking -= 2;
    }
    if (liking > 15) {
        liking = 15;
    } else if (liking < 0) {
        liking = 0;
    }
    return liking;
}

void SavedCharacter::SetUnknown292(int index, u8 value) {
    mUnknown292[index] = value;
}

u8 SavedCharacter::GetUnknown292(int index) {
    return mUnknown292[index];
}

void SavedCharacter::ResetItemPreferences() {
    mFavoriteItem = 0xFF;
    mDislikedItem = 0xFF;
    MI_CpuFill8(mUnknown292, 0xFF, sizeof(mUnknown292));
}

// A favourite food among the foods it finds tasty (7 or more) and a disliked one among all foods, at random; bad food
// and five particular foods are left out
void SavedCharacter::ChooseFoodPreferences(u8 *favorite, u8 *disliked) {
    u8 tasty[255];
    u8 foods[255];
    int tastyCount = 0;
    u8 *nextFood = foods;
    u8 *nextTasty = tasty;
    int foodCount = 0;
    for (int i = 0; i < 255; i++) {
        const ItemInfo *item = &gGoodsTable[(u16)i];
        if (item->mKind != 1 || item->mFlags & ITEM_BAD_FOOD) {
            continue;
        }
        int kind = item->mUnknown04Bits24;
        if (kind == 66 || kind == 125 || kind == 54 || kind == 23 || kind == 53) {
            continue;
        }
        *nextFood++ = i;
        foodCount++;
        if (GetTaste(i) >= 7) {
            *nextTasty++ = i;
            tastyCount++;
        }
    }
    u8 favorite_ = tasty[MATH_Rand16(&gRandom, tastyCount)];
    u8 disliked_;
    do {
        disliked_ = foods[MATH_Rand16(&gRandom, foodCount)];
    } while (disliked_ == favorite_);
    *favorite = favorite_;
    *disliked = disliked_;
}

// Whether the favourite and disliked foods are two foods, and the recent ones foods
BOOL SavedCharacter::AreFoodPreferencesValid() {
    BOOL valid = TRUE;
    if (mFavoriteItem == 0xFF || mDislikedItem == 0xFF) {
        valid = FALSE;
    } else if (gGoodsTable[mFavoriteItem].mKind != 1 || gGoodsTable[mDislikedItem].mKind != 1 ||
               mFavoriteItem == mDislikedItem) {
        valid = FALSE;
    }
    for (int i = 0; i < 3; i++) {
        u8 item = mUnknown292[i];
        if (item != 0xFF && gGoodsTable[item].mKind != 1) {
            valid = FALSE;
            break;
        }
    }
    return valid;
}

// New favourite and disliked foods (chosen on a copy of the record)
void SavedCharacter::ChooseNewFoodPreferences() {
    SavedCharacter copy;
    (CharacterRecord &)copy = *this;
    u8 favorite;
    u8 disliked;
    copy.ChooseFoodPreferences(&favorite, &disliked);
    mFavoriteItem = favorite;
    mDislikedItem = disliked;
    MI_CpuFill8(mUnknown292, 0xFF, sizeof(mUnknown292));
}

BOOL SavedCharacter::CheckUnknown292() {
    BOOL valid = TRUE;
    if (mUnknown292[0] == mFavoriteItem) {
        for (int i = 0; i < 2; i++) {
            mUnknown292[i] = mUnknown292[i + 1];
        }
        mUnknown292[2] = 0xFF;
        valid = FALSE;
    }
    return valid;
}

void SavedCharacter::AddUnknown296(int value) {
    s64 sum = value + mUnknown296;
    if (sum > 0xFF) {
        sum = 0xFF;
    } else if (sum < 0) {
        sum = 0;
    }
    mUnknown296 = sum;
}

void SavedCharacter::AddUnknown297(int value) {
    s64 sum = value + mUnknown297;
    if (sum > 0xFF) {
        sum = 0xFF;
    } else if (sum < 0) {
        sum = 0;
    }
    mUnknown297 = sum;
}

int SavedCharacter::GetRelationBloodPairValue(int a, int b) {
    if (a >= 6 || b >= 6) {
        return 0;
    }
    if (a < b) {
        int t = a;
        a = b;
        b = t;
    }
    return sRelationBloodPairs[b * 6 + a - ((b * (b + 1)) >> 1)];
}

void SavedCharacter::ResetPatDate() {
    mPatYear = 0;
    mPatMonth = 0;
    mPatDay = 0;
}

void SavedCharacter::ResetUnknown2A3() {
    mUnknown2A3 = 0xFF;
}

void SavedCharacter::RandomizeUnknown24E() {
    mUnknown24E = MATH_Rand16(&gRandom, 6);
}

void SavedCharacter::ResetUnknown24E() {
    mUnknown24E = 0xFF;
}

int SavedCharacter::GetPersonalityPairValue(int a, int b) {
    if (a >= 16 || b >= 16) {
        return 50;
    }
    if (a < b) {
        int t = a;
        a = b;
        b = t;
    }
    return sPersonalityPairs[a + b * 16 - ((b * (b + 1)) >> 1)];
}

// A reaction's weight scaled by the personality answers (kind 1 to 4: the first two answers' score, its complement,
// the last two answers' score, its complement), in 128ths
int SavedCharacter::ScaleTalkWeightByAnswers(int kind, int weight) {
    switch (kind) {
    case 0:
        weight = 0;
        break;
    case 1:
        weight = weight * GetAnswerScore01() >> 7;
        break;
    case 2:
        weight = weight * (ScoreAnswers01(8, 8) + ScoreAnswers01(1, 1) - GetAnswerScore01()) >> 7;
        break;
    case 3:
        weight = weight * GetAnswerScore23() >> 7;
        break;
    case 4:
        weight = weight * (ScoreAnswers23(8, 8) + ScoreAnswers23(1, 1) - GetAnswerScore23()) >> 7;
        break;
    default:
        weight = 0;
        break;
    }
    return weight;
}

// The same by how well the two personalities go together (kind 1) or not (kind 2)
int SavedCharacter::ScaleTalkWeightByPersonalities(int kind, int weight, SavedCharacter *partner) {
    switch (kind) {
    case 0:
        weight = 0;
        break;
    case 1:
        weight = weight * GetPersonalityPairValue(partner->mPersonality, mPersonality) >> 7;
        break;
    case 2:
        weight = weight * (120 - GetPersonalityPairValue(partner->mPersonality, mPersonality)) >> 7;
        break;
    default:
        weight = 0;
        break;
    }
    return weight;
}

// The weight of one of the twelve reactions to what the partner did in a talk: by the relation and the partner's
// reaction, for the character's kind of personality and mood only, plus its scalings
int SavedCharacter::GetTalkReactionWeight(s8 relation, s8 partnerReaction, s8 reaction, SavedCharacter *partner) {
    const TalkReactionInfo *info = &data_020f5828[reaction];
    int weight = data_021135f0[relation][partnerReaction * 12 + reaction];
    if ((info->personalities & (1 << gPersonalityKinds[mPersonality])) && (info->moods & (1 << mMood)) && weight != 0) {
        int byAnswers = ScaleTalkWeightByAnswers(info->answerScaling, weight);
        weight += byAnswers + ScaleTalkWeightByPersonalities(info->pairScaling, weight, partner);
    } else {
        weight = 0;
    }
    return weight;
}

int SavedCharacter::GetTotalTalkReactionWeight(s8 relation, s8 partnerReaction, SavedCharacter *partner) {
    int total = 0;
    for (s8 i = 0; i < 12; i++) {
        total += GetTalkReactionWeight(relation, partnerReaction, i, partner);
    }
    return total;
}

// How the character reacts in a talk (overlay 74), 0 to 11, by the weights
int SavedCharacter::ChooseTalkReaction(s8 relation, s8 partnerReaction, SavedCharacter *partner) {
    mPersonality = GetPersonality();
    int random = Rand(GetTotalTalkReactionWeight(relation, partnerReaction, partner));
    int sum = 0;
    int i;
    for (i = 0; i < 12; i++) {
        sum += GetTalkReactionWeight(relation, partnerReaction, i, partner);
        if (sum > random) {
            break;
        }
    }
    if (i >= 12) {
        i = 0;
    }
    return i;
}

// As ScaleTalkWeightByAnswers, for the reactions by themselves
int SavedCharacter::ScaleReactionWeightByAnswers(int kind, int weight) {
    switch (kind) {
    case 0:
        weight = 0;
        break;
    case 1:
        weight = weight * GetAnswerScore01() >> 7;
        break;
    case 2:
        weight = weight * (ScoreAnswers01(8, 8) + ScoreAnswers01(1, 1) - GetAnswerScore01()) >> 7;
        break;
    case 3:
        weight = weight * GetAnswerScore23() >> 7;
        break;
    case 4:
        weight = weight * (ScoreAnswers23(8, 8) + ScoreAnswers23(1, 1) - GetAnswerScore23()) >> 7;
        break;
    default:
        weight = 0;
        break;
    }
    return weight;
}

// The weight of one of the fifteen reactions that follow the one before
int SavedCharacter::GetReactionWeight(s8 previous, s8 reaction) {
    int weight = data_020f5e28[previous][reaction];
    const ReactionInfo *info = &data_020f5df8[reaction];
    if ((info->personalities & (1 << gPersonalityKinds[mPersonality])) && (info->moods & (1 << mMood)) && weight != 0) {
        weight += ScaleReactionWeightByAnswers(info->answerScaling, weight);
    } else {
        weight = 0;
    }
    return weight;
}

int SavedCharacter::GetTotalReactionWeight(s8 previous) {
    int total = 0;
    for (s8 i = 0; i < 15; i++) {
        total += GetReactionWeight(previous, i);
    }
    return total;
}

// The reaction that follows the one before (overlay 74's characters), 0 to 14, by the weights
int SavedCharacter::ChooseNextReaction(s8 previous) {
    mPersonality = GetPersonality();
    int random = Rand(GetTotalReactionWeight(previous));
    int sum = 0;
    int i;
    for (i = 0; i < 15; i++) {
        sum += GetReactionWeight(previous, i);
        if (sum > random) {
            break;
        }
    }
    if (i >= 15) {
        i = 0;
    }
    return i;
}

// Gives the speech synthesizer the character's voice, each parameter within its range
void SavedCharacter::SetVoiceOf(TtsPlayer *player) {
    player->StopEngine();
    for (int i = 0; i < 6; i++) {
        int value = mVoice[i];
        if (value < gVoiceParams[i].min) {
            value = gVoiceParams[i].min;
        }
        if (value > gVoiceParams[i].max) {
            value = gVoiceParams[i].max;
        }
        player->SetEngineParam(gVoiceParams[i].param, value);
    }
}

// 0 when the two records are the same
int SavedCharacter::CompareRecords(const CharacterRecord *a, const CharacterRecord *b) {
    return memcmp(a, b, sizeof(CharacterRecord));
}

BOOL SavedCharacter::IsLeapYear(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
        return TRUE;
    }
    return FALSE;
}

int SavedCharacter::GetDaysInYear(int year) {
    return IsLeapYear(year) ? 366 : 365;
}

int SavedCharacter::GetDaysInMonth(int month, int year) {
    return IsLeapYear(year) ? sDaysInMonthLeap[month] : sDaysInMonth[month];
}

// The days from one date to a later one of the same year, 0 if it is earlier
int SavedCharacter::GetDaysBetweenInYear(int month1, int day1, int month2, int day2, int year) {
    int days;
    if (month2 - month1 < 0) {
        return 0;
    }
    if (month2 - month1 == 0) {
        days = day2 - day1;
        if (days < 0) {
            days = 0;
        }
        return days;
    }
    days = GetDaysInMonth(month1, year) - day1;
    for (int month = month1 + 1; month < month2; month++) {
        days += GetDaysInMonth(month, year);
    }
    return days + day2;
}

// The days from one date to a later one, 0 if it is earlier
int SavedCharacter::GetDaysBetween(int month1, int day1, int year1, int month2, int day2, int year2) {
    int year;
    int days;
    if (year1 > year2) {
        return 0;
    }
    if (year1 == year2) {
        if (month2 - month1 < 0) {
            return 0;
        }
        return GetDaysBetweenInYear(month1, day1, month2, day2, year1);
    }
    days = GetDaysBetweenInYear(month1, day1, 12, 31, year1);
    for (year = year1 + 1; year < year2; year++) {
        days += GetDaysInYear(year);
    }
    return days + (GetDaysBetweenInYear(1, 1, month2, day2, year2) + 1);
}

// The birthday; without a year of birth, a date of the 20th century made from the create ID
void SavedCharacter::GetBirthday(RTCDate *date) {
    date->year = mBirthYear;
    if (date->year == 0xFFF) {
        CharInfo info;
        NFL_GetCharInfo(&info, this);
        CreateId id = info.createId;
        date->year = id.data[0] % 99 + 1900;
        date->month = id.data[1] % 12 + 1;
        date->day = id.data[2] % 28 + 1;
    } else {
        date->month = GetParameter(PARAM_BIRTH_MONTH);
        date->day = GetParameter(PARAM_BIRTH_DAY);
    }
}

// The character's age in days
int SavedCharacter::GetAgeInDays() {
    RTCDate birthday;
    GetBirthday(&birthday);
    RTCDate today;
    Clock_GetDate(gGameClock, &today);
    today.year += 2000;
    return GetDaysBetween(birthday, today);
}

// The biorhythm on a day of life: the physical (23 days), intellectual (33) and emotional (28) cycles added up, 0 to
// 1000
int SavedCharacter::GetBiorhythm(int day) {
    if (day < 0) {
        day = 0;
    }
    int cycles[3];
    cycles[0] = sPhysicalCycle[day % 23];
    cycles[1] = sIntellectualCycle[day % 33];
    cycles[2] = sEmotionalCycle[day % 28];
    int sum = 0;
    for (int i = 0; i < 3; i++) {
        sum += cycles[i];
    }
    sum *= 1000;
    sum /= 32767 * 3;
    if (sum < 0) {
        sum = 0;
    }
    if (sum > 1000) {
        sum = 1000;
    }
    return sum;
}

// The day of the 60-day cycle of the calendar the character was born on, times two, plus one for a woman
int SavedCharacter::GetBirthDayCycle() {
    RTCDate birthday;
    GetBirthday(&birthday);
    RTCDate origin;
    origin.year = 1900;
    origin.month = 1;
    origin.day = 1;
    int days = GetDaysBetween(origin, birthday);
    int cycle = (days + 10) % 60 * 2;
    if (MiiData_GetSex(this) == 1) {
        cycle++;
    }
    return cycle;
}

// A value of the row of a table of four columns that is the column of the character's kind of personality
int SavedCharacter::GetPersonalityColumnValue(int row, const u8 *table) {
    return (table + row * 4)[sPersonalityColumns[gPersonalityKinds[GetPersonality()]]];
}

// The offset of that value
int SavedCharacter::GetPersonalityColumnOffset(int row) {
    return row * 4 + sPersonalityColumnOffsets[gPersonalityKinds[GetPersonality()]];
}

// Today's biorhythm, its three cycles (0 to 255) and yesterday's
void SavedCharacter::UpdateBiorhythm() {
    int day = GetAgeInDays();
    mBiorhythmYesterday = GetBiorhythm(day - 1);
    mBiorhythmCycles[0] = sPhysicalCycle[day % 23];
    mBiorhythmCycles[1] = sIntellectualCycle[day % 33];
    mBiorhythmCycles[2] = sEmotionalCycle[day % 28];
    mBiorhythm = 0;
    for (int i = 0; i < 3; i++) {
        mBiorhythm += mBiorhythmCycles[i];
        if (mBiorhythmCycles[i] < 0) {
            mBiorhythmCycles[i] = 0;
        }
        mBiorhythmCycles[i] >>= 7;
        if (mBiorhythmCycles[i] > 255) {
            mBiorhythmCycles[i] = 255;
        }
    }
    mBiorhythm *= 1000;
    mBiorhythm /= 32767 * 3;
    if (mBiorhythm < 0) {
        mBiorhythm = 0;
    }
    if (mBiorhythm > 1000) {
        mBiorhythm = 1000;
    }
}

// The first room of the building no saved character lives in
void SavedCharacter::AssignFreeRoom() {
    u8 rooms[100];
    MI_CpuFill8(rooms, 0xFF, sizeof(rooms));
    for (int i = 0; i < 100; i++) {
        if (GameData_IsCharacterSlotUsed(&gGameData, i)) {
            rooms[GameData_GetCharacterRoomNumber(&gGameData, i)] = i;
        }
    }
    GetApartmentRooms();
    u8 room = 0xFF;
    for (int i = 0; i < 100; i++) {
        if (rooms[i] == 0xFF) {
            room = i;
            break;
        }
    }
    mRoomNumber = room;
}

u32 SavedCharacter::CountOwnedInteriors() {
    int count = 0;
    for (u32 i = 0; i < 3; i++) {
        count += MATH_CountPopulation(mOwnedInteriors[i]);
    }
    return count;
}
