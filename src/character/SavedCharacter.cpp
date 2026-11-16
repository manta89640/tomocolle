#include "character/Character.h"

#include "GameData.h"

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
extern MiiDataSetter data_020fbb7c[PARAM_COUNT];
extern MiiDataGetter data_020fbc2c[PARAM_COUNT];

// The clock (gap)
void Clock_GetDate(void *clock, RTCDate *date);
extern void *data_02123440;
extern MiiIdSource data_0212349c; // Where this console's Miis' create IDs come from

// The saved game (gap)
BOOL func_0202c3dc(GameData *game, int index); // Whether there is a saved character with that index
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
void func_0202c28c(GameData *game, int index, SavedCharacter *character); // Writes one back

BOOL func_0202c468(GameData *game, int index);
int func_0202c5dc(GameData *game, int index);
int func_0202c43c(GameData *game, int index, int other);
BOOL func_02039c00(void *flags, int flag);
extern u8 data_0212348c[];
extern const s8 gStr_AFKPUUA[24];  // By hour: the chance (of 100) that an idle character goes to sleep
extern const s8 data_020f8ca8[24]; // By hour: how many hours it then sleeps on top of the activity's length

extern MATHRandContext16 gRandom;
// The long activities (CharacterRecord::mLongActState)
struct LongActInfo {
    u32 minutes : 12;      // Its length
    u32 weight : 8;        // Its weight in the random choice (ChooseLongAct)
    u32 personalities : 4; // A bit for each kind of personality (data_020fba28) that may choose it
    u32 moods : 5;         // A bit for each mood that may choose it
    u32 unk29 : 3;
};
extern const int data_020fba28[16]; // The kind (0 to 3) of each personality

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78)
struct TroubleInfo {
    u32 kind : 8;
    u32 hours : 16;    // How long it lasts
    u32 unk0Bits24 : 8;
    u32 unk4Bits0 : 8;
    u32 unk4Bits8 : 8;
    u32 cooldownMinutes : 12; // Added to mTroubleEnd once it is over
    u32 unk4Bits28 : 4;
    u32 unk8;
}; // 0xC
extern TroubleInfo *data_02114d78[];
extern const u8 gStr_Parenthesis_2[]; // The starting progress of each trouble of category 0
extern const u8 data_020f8d8c[10];     // The weight of each trouble category in the random choice
extern const u32 gStr_LessThan_2[10];  // The number of troubles of each category
extern const LongActInfo data_020f8cc0[];
}

// The voice parameters' ranges and defaults (VoiceParam::defaultValue is what ResetVoice sets)
struct VoiceParam {
    /* 0x0 */ s16 unk0;
    /* 0x2 */ s16 min;
    /* 0x4 */ s16 max;
    /* 0x6 */ s16 defaultValue;
}; // 0x8
extern const VoiceParam data_020fb940[6];

// The scores of the personality questions' answers (index: the answer, 1 to 8; 0 unanswered)
static const s8 sScoresAnswer0[9] = {50, 10, 20, 30, 40, 60, 70, 80, 90};
static const s8 sScoresAnswer1[9] = {50, 5, 15, 25, 35, 65, 75, 85, 95};
static const s8 sScoresAnswer2[9] = {50, 10, 20, 30, 40, 60, 70, 80, 90};
static const s8 sScoresAnswer3[9] = {50, 5, 15, 25, 35, 65, 75, 85, 95};

// Keeps a field of the record within its range; FALSE when it was not. The name is the field's, for messages that
// the final build leaves out.
template <typename T> BOOL ClampField(T *value, int min, int max, const char *name) {
    if (*value > max) {
        *value = max;
        return FALSE;
    }
    if (*value < min) {
        *value = min;
        return FALSE;
    }
    return TRUE;
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
    MI_CpuClear32(mUnknown210, sizeof(mUnknown210));
    ResetRelations();
    ResetMood();
    ClearLongAct();
    ClearTrouble();
    func_0209c4a0();
    func_0209c5d8();
    func_0209c5f4();
    func_0209c698();
    mUnknown054Bit20 = 0;
    func_0209c6b4();
    func_0209c700();
    OwnClothes(mClothesId);
    func_0209c748();
    func_0209cb88();
    mUnknown296 = 0;
    mUnknown297 = 0;
    mRelationBlood = 0;
    func_0209d2c0();
    func_0209d2d4();
    func_0209c680();
    func_0209d310();
    MI_CpuClear32(mUnknown2A8, sizeof(mUnknown2A8));
    mUnknown2B4 = 0;
    mUnknown2B8 = 0;
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
        func_0209e01c();
    }
    ResetRelations();
    ResetMood();
    ClearLongAct();
    ClearTrouble();
    func_0209c5d8();
    func_0209c5f4();
    func_0209c698();
    mUnknown054Bit20 = 0;
    mRelationBlood = 0;
    func_0209d2c0();
    func_0209d2d4();
    func_0209c680();
    func_0209d310();
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
        if (!ClampField(&mVoice[i], data_020fb940[i].min, data_020fb940[i].max, "voice")) {
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
    if (!func_0209ccf0()) {
        func_0209cda4();
        valid = FALSE;
    }
    valid &= func_0209d1d0();
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
    Clock_GetDate(data_02123440, &date);
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
        number += (u16)(copy.data[0] << 12);
        number += copy.data[1] << 4;
        number += copy.data[2] >> 4;
    } else {
        number += (u16)(copy.data[3] << 14);
        number += copy.data[2] << 6;
        number += copy.data[3] >> 2;
    }
    return number;
}

void SavedCharacter::GetCreateIdParts(int *high, int *low) {
    u32 number = GetCreateIdNumber();
    *high = (number & 0xC000) >> 14;
    *low = number & 0x3FFF;
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
        data_020fbb7c[param](this, value);
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
    data_020fbb7c[param](this, value);
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
        SetVoice(i, data_020fb940[i].defaultValue);
    }
}

int SavedCharacter::GetParameter(int param) {
    u32 value = data_020fbc2c[param](this);
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
    return data_020fbc2c[param](this);
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
    return sScoresAnswer0[answer0] + sScoresAnswer1[answer1];
}

int SavedCharacter::GetAnswerScore23() {
    return ScoreAnswers23(mAnswers[2], mAnswers[3]);
}

int SavedCharacter::ScoreAnswers23(int answer2, int answer3) {
    return sScoresAnswer2[answer2] + sScoresAnswer3[answer3];
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

int SavedCharacter::FindOtherWithRank2478() {
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i) && i != mIndex && IsRank2478(mRelationRank[i])) {
            return i;
        }
    }
    return -1;
}

int SavedCharacter::FindOtherWithRank2Or4() {
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i) && i != mIndex && IsRank2Or4(mRelationRank[i])) {
            return i;
        }
    }
    return -1;
}

int SavedCharacter::FindOtherWithRank9() {
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i) && i != mIndex && mRelationRank[i] == 9) {
            return i;
        }
    }
    return -1;
}

BOOL SavedCharacter::IsRank2478(int rank) {
    switch (rank) {
    case 2:
    case 4:
    case 7:
    case 8:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsRank2Or4(int rank) {
    switch (rank) {
    case 2:
    case 4:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsRank678(int rank) {
    switch (rank) {
    case 6:
    case 7:
    case 8:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsRank124(int rank) {
    switch (rank) {
    case 1:
    case 2:
    case 4:
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsRank1249Or678(int rank) {
    if (IsRank678(rank)) {
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
            func_0202c264(&gGameData, partner, &other);
            other.mPersonality = other.GetPersonality();
            partnerCharacter = &other;
        }
        partnerCharacter->ClearLongAct();
        func_0202c28c(&gGameData, partner, partnerCharacter);
    }
    ClearLongAct();
    func_0209c5f4();
    func_0202c28c(&gGameData, mIndex, this);
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
    if ((1 << data_020fba28[mPersonality]) & info->personalities && info->moods & (1 << mMood) && info->weight != 0 &&
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
    int random = MATH_Rand16(&gRandom, GetTotalLongActWeight());
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

BOOL SavedCharacter::IsInTroubleWithoutUnknown4Bits8() {
    BOOL inTrouble = IsInTrouble();
    return inTrouble & (data_02114d78[mTroubleCategory][mTroubleId].unk4Bits8 == 0);
}

void SavedCharacter::SetTrouble(u8 state, u8 category, u8 id) {
    mTroubleState = state;
    mTroubleCategory = category;
    mTroubleId = id;
}

BOOL SavedCharacter::IsProgressingTrouble() {
    BOOL result = FALSE;
    if (data_02114d78[mTroubleCategory][mTroubleId].kind == 11) {
        BOOL active = FALSE;
        switch (mTroubleState) {
        case 1:
        case 2:
        case 4:
            active = TRUE;
            break;
        }
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
    if (!IsProgressingTrouble()) {
        mTroubleProgress = 0;
    } else {
        mTroubleProgress = gStr_Parenthesis_2[mTroubleId];
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

BOOL SavedCharacter::IsTroubleOfCategory3Kind8To10() {
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
    if (data_02114d78[category][mTroubleId].kind == 18) {
        return TRUE;
    }
    return FALSE;
}

BOOL SavedCharacter::IsTrouble3_4() {
    return IsTrouble3_4State(mTroubleState, mTroubleCategory, mTroubleId);
}

BOOL SavedCharacter::IsTrouble3_4State(int state, int category, int id) {
    if (state != 0 && category == 3 && id == 4) {
        return TRUE;
    }
    return FALSE;
}

int SavedCharacter::GetEncounterBonus(int base, int value) {
    int bonus = -base;
    if (value <= 1) {
        bonus = 0;
    } else if (value <= 30) {
        bonus += 2;
    } else if (value <= 70) {
        bonus += 4;
    } else {
        bonus += 6;
    }
    if (bonus <= 1) {
        bonus = 0;
    }
    return bonus;
}

void SavedCharacter::StartLongAct(int state, s64 now, RTCTime *time, SavedCharacter *first, SavedCharacter *second) {
    if (IsLongActAsPartnerState(state)) {
        u16 special[2];
        u16 candidates[100];
        int count;
        MI_CpuFill16(special, 0xFFFF, sizeof(special));
        count = 0;
        for (u16 i = 0; i < 100; i = (u16)(i + 1)) {
            if (func_0202c3dc(&gGameData, i) && !func_0202c468(&gGameData, i) && i != mIndex &&
                Is0Or1(func_0202c5dc(&gGameData, i))) {
                u8 rank = mRelationRank[i];
                if (!IsRank678(rank) && rank != 0 && rank != 3 && rank != 5) {
                    if (IsRank2478(rank)) {
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
                if (special[j] != 0xFFFF && 75 - j * 25 > MATH_Rand16(&gRandom, 100)) {
                    partner = special[j];
                    if (j == 0 && mRelationMeter[partner] > 50 && func_0202c43c(&gGameData, partner, mIndex) > 50) {
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
                    func_0202c264(&gGameData, partner, &other);
                    first = &other;
                }
            }
            int partnerState;
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
            func_0202c28c(&gGameData, partner, first);
        } else {
            SetLongAct(0, 0, now + GetLongActDuration(0, 3));
        }
    } else if (state == 0x13) {
        int days = MATH_Rand16(&gRandom, 4) + 2;
        SetLongAct(state, days, now + GetLongActDuration(state, 3));
    } else if (state == 0x14) {
        int choices[4];
        int count = 0;
        if (func_02039c00(data_0212348c, 3)) {
            choices[count] = 0;
            count++;
        }
        if (func_02039c00(data_0212348c, 10)) {
            choices[count] = 1;
            count++;
        }
        if (func_02039c00(data_0212348c, 11)) {
            choices[count] = 2;
            count++;
        }
        if (func_02039c00(data_0212348c, 8)) {
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
        if (time->hour < 24 && gStr_AFKPUUA[time->hour] > MATH_Rand16(&gRandom, 100)) {
            s64 end = now + GetLongActDuration(0x12, 3);
            SetLongAct(0x12, 0, end + data_020f8ca8[time->hour] * 3600);
            func_0209c600();
        } else {
            SetLongAct(0, 0, now + GetLongActDuration(0, 3));
        }
    } else if (state == 0xF) {
        SetLongAct(state, 0, now + GetLongActDuration(state, 3));
        func_0209c600();
    } else if (state == 0x11) {
        if (mTroubleState == 0) {
            SetLongAct(state, 0, now + GetLongActDuration(state, 3));
        } else {
            ClearLongActFrom(now);
        }
    } else {
        SetLongAct(state, 0, now + GetLongActDuration(state, 3));
    }
    func_0202c28c(&gGameData, mIndex, this);
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
        if (!func_02039c00(data_0212348c, 10)) {
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
        if (!func_02039c00(data_0212348c, 11)) {
            weight = 0;
        }
        break;
    }
    return weight;
}

int SavedCharacter::ChooseTrouble(int category) {
    int total = 0;
    u32 count = gStr_LessThan_2[category];
    for (u32 i = 0; i < count; i++) {
        total += data_02114d78[category][i].unk0Bits24;
    }
    int random = MATH_Rand16(&gRandom, total);
    u8 id = 0;
    int sum = 0;
    for (; id < gStr_LessThan_2[category]; id++) {
        sum += data_02114d78[category][id].unk0Bits24;
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
    func_0202c28c(&gGameData, mIndex, this);
}

void SavedCharacter::ClearTrouble() {
    mTroubleState = 0;
    mTroubleCategory = 0;
    mTroubleId = 0;
    mTroublePartner = 0;
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
