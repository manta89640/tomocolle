#ifndef CHARACTER_CHARACTER_H
#define CHARACTER_CHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "character/CharacterModel.h"
#include "character/FloorPos.h"

// The main program's character classes (src/character/), from the record that the save data holds to the character
// that a scene draws:
//
//   CharacterRecord   0x2A4 bytes  what the save data holds of a character (func_0202c264 copies one out)
//   SavedCharacter    0x2C0        + its index and what the game derives from the record
//   CharacterBase     0x25E4       + the Mii's face, drawn with the Mii library (NFL) at the head
//   Character         0x2B18       + the body (CharacterModel, a second base), walking, turning, animations, sounds
//
// The scenes' own characters (SceneCharacter, RoomCharacter, ...) derive from Character. Its constructor and
// destructor are implicit: the scenes' constructors call the base constructors of CharacterBase and of
// CharacterModel (C2, 0x0208eb18: a model that is a member is built with C1, 0x0208eba0, as the G3D test scene's),
// and their destructors CharacterModel's inline destructor and CharacterBase's D2. The names are the project's own.

// The parameters of a Mii that SavedCharacter::GetParameter and SetParameter take: the Mii data's getters and setters
// by number (data_020fbc2c and data_020fbb7c, lib/MiiData). For a Mii not made on this console the birth month and day
// are the record's own (CharacterRecord::mOwnBirthMonth, mOwnBirthDay).
enum MiiParameter {
    PARAM_EYE_TYPE, // 0x00
    PARAM_EYE_COLOR,
    PARAM_EYE_SCALE,
    PARAM_EYE_ROTATION,
    PARAM_EYE_X, // 0x04
    PARAM_EYE_Y,
    PARAM_EYEBROW_TYPE,
    PARAM_EYEBROW_COLOR,
    PARAM_EYEBROW_SCALE, // 0x08
    PARAM_EYEBROW_ROTATION,
    PARAM_EYEBROW_X,
    PARAM_EYEBROW_Y,
    PARAM_MOUTH_TYPE, // 0x0C
    PARAM_MOUTH_COLOR,
    PARAM_MOUTH_SCALE,
    PARAM_MOUTH_Y,
    PARAM_BEARD_TYPE, // 0x10
    PARAM_BEARD_COLOR,
    PARAM_MUSTACHE_TYPE,
    PARAM_BEARD_SCALE,
    PARAM_BEARD_Y, // 0x14
    PARAM_MOLE_TYPE,
    PARAM_MOLE_SCALE,
    PARAM_MOLE_X,
    PARAM_MOLE_Y, // 0x18
    PARAM_HAIR_TYPE,
    PARAM_HAIR_COLOR,
    PARAM_HAIR_FLIP,
    PARAM_NOSE_TYPE, // 0x1C
    PARAM_NOSE_SCALE,
    PARAM_NOSE_Y,
    PARAM_GLASSES_TYPE,
    PARAM_GLASSES_COLOR, // 0x20
    PARAM_GLASSES_SCALE,
    PARAM_GLASSES_Y,
    PARAM_FACE_TYPE,
    PARAM_FACE_COLOR, // 0x24
    PARAM_FACE_TEXTURE,
    PARAM_BIRTH_MONTH,
    PARAM_BIRTH_DAY,
    PARAM_FAVORITE_COLOR, // 0x28
    PARAM_HEIGHT,
    PARAM_BUILD,
    PARAM_DAYS_IN_BIRTH_MONTH,
    PARAM_COUNT
};

// What a character is to another (CharacterRecord::mRelationRank), as the game's list of relations names them
enum Relation {
    RELATION_NONE,
    RELATION_FRIENDS,
    RELATION_LOVERS,
    RELATION_EX_LOVERS,
    RELATION_SPOUSES,
    RELATION_DIVORCED,
    RELATION_FRIENDS_FIGHTING,
    RELATION_LOVERS_FIGHTING,
    RELATION_SPOUSES_FIGHTING,
    RELATION_BEST_FRIENDS,
};

// A Mii's create ID (lib/NFL's NFLCreateId)
struct CreateId {
    u8 data[8];
};

// What the save data holds of a character: the 0x2A4 bytes that func_0202c264 copies out. Overlay 109's implicit
// copy assignment of it shows its members. The names in quotes are the game's own: SavedCharacter::Validate gives
// them with each field it checks.
struct CharacterRecord {
    /* 0x000 */ u8 mMii[0x4A];           // The Mii's data (0x4A bytes, lib/MiiData)
    /* 0x04A */ u8 mAnswers[5];          // "character": the personality questions' answers, 1 to 8 (0 unset)
    /* 0x04F */ u8 mRoomNumber;          // Number of its room in the building, 0xFF for none
    /* 0x050 */ u32 mUnknown050Bit0 : 1;
    /*       */ u32 mRegisteredYear : 7;  // The date it was registered: two-digit year, month and day
    /*       */ u32 mRegisteredMonth : 4;
    /*       */ u32 mRegisteredDay : 5;
    /*       */ u32 mBirthYear : 12;     // 1900 to 2099, 0xFFF for none
    /*       */ u32 mBloodType : 3;      // 0 to 4
    /* 0x054 */ u32 mUnknown054Bits0 : 4; // 0 to 8
    /*       */ u32 mInterior : 7;       // Which interior its room has (below 78)
    /*       */ u32 mClothesId : 9;      // The clothes it wears (row of the clothes table, below 300)
    /*       */ u32 mUnknown054Bit20 : 1;
    /*       */ u32 mOwnBirthMonth : 4;  // The birthday of a Mii from another console (0 unset)
    /*       */ u32 mOwnBirthDay : 5;
    /*       */ u32 mUnknown054Bits30 : 2;
    /* 0x058 */ wchar_t mName[11];
    /* 0x06E */ wchar_t mFamilyName[11];
    /* 0x084 */ wchar_t mMotto[17];      // "motto1": what it says (the speech texts)
    /* 0x0A6 */ wchar_t mMottos[4][9];   // "motto": the same by mood
    /* 0x0EE */ s16 mVoice[6];           // "voice": five parameters of the speech synthesizer and a choice of four
    /* 0x0FA */ u8 mSatisfyMeter;        // "SatisfyMeter", 0 to 99
    /* 0x0FB */ u8 mSatisfyLevel;        // "SatisfyLevel", 1 to 99
    /* 0x0FC */ u8 mRelationMeter[100];  // "RelationMeter": for each other character by index, 0 to 200
    /* 0x160 */ u8 mRelationRank[100];   // "RelationRank": for each other character by index, 0 to 10
    /* 0x1C4 */ u32 mUnknown1C4;
    /* 0x1C8 */ struct { u32 m[8]; } mUnknown1C8;
    /* 0x1E8 */ struct { u32 m[10]; } mOwnedClothes; // A bit for each row of the clothes table
    /* 0x210 */ u32 mOwnedInteriors[3];  // A bit for each row of the interiors table
    /* 0x21C */ s64 mLongActEnd;         // A time in seconds, as the clock's
    /* 0x224 */ u8 mLongActState;        // "LongActState", 0 to 0x2E: what it is doing for a long while
    /* 0x225 */ u8 mLongActPartner;      // mIndex of the character it is doing it with
    /* 0x226 */ u8 mTroubleId;           // "TroubleID", 0 to 0x6D
    /* 0x227 */ u8 mTroubleCategory;     // "TroubleCategory", 0 to 9
    /* 0x228 */ s64 mTroubleStart;       // When the trouble began (a time in seconds)
    /* 0x230 */ s64 mTroubleEnd;         // When it ends (the wedding sets it to the honeymoon's end)
    /* 0x238 */ u16 mTroubleTarget;      // What the trouble is about: mIndex of a character, or an item
    /* 0x23A */ u8 mTroubleState;        // "TroubleState", 0 to 5
    /* 0x23B */ u8 mTroubleProgress;     // 0 to 200
    /* 0x23C */ s64 mUnknown23C;
    /* 0x244 */ u8 mEatFullness;         // "EatFullness", 0 to 100
    /* 0x245 */ u8 mEatLastFood;         // "EatLastFood"
    /* 0x246 */ u8 mUnknown246;
    /* 0x247 */ u8 mMeetYear;            // "MeetYear", 0 to 99
    /* 0x248 */ u8 mMeetMonth;           // "MeetMonth", 1 to 12
    /* 0x249 */ u8 mMeetDay;             // "MeetDay", 1 to 31
    /* 0x24A */ u8 mUnknown24A;
    /* 0x24B */ u8 mTouchTalkMonth;      // "TouchTalkMonth", 1 to 12
    /* 0x24C */ u8 mTouchTalkDay;        // "TouchTalkDay", 1 to 31
    /* 0x24D */ u8 mTouchTalkHour;       // "TouchTalkHour", 0 to 23
    /* 0x24E */ u8 mUnknown24E;          // Below 6, or 0xFF
    /* 0x24F */ u8 mUnknown24F;
    /* 0x250 */ u16 mUnknown250[32];
    /* 0x290 */ u8 mFavoriteItem;        // GetLiking gives it the most liking, 0xFF for none
    /* 0x291 */ u8 mDislikedItem;        // and this none
    /* 0x292 */ u8 mUnknown292[3];
    /* 0x295 */ u8 mMood;                // 0 to 4
    /* 0x296 */ u8 mUnknown296;
    /* 0x297 */ u8 mUnknown297;
    /* 0x298 */ u8 mRelationBlood;       // "RelationBlood", 0 to 5
    /* 0x299 */ u8 mUnknown299;
    /* 0x29A */ u8 mSongSlots[6];        // Which of the saved songs each of the character's six is (below 100, or 0xFF)
    /* 0x2A0 */ u8 mPatYear;             // "PatYear", 0 to 99
    /* 0x2A1 */ u8 mPatMonth;            // "PatMonth", 0 to 12
    /* 0x2A2 */ u8 mPatDay;              // "PatDay", 0 to 31
    /* 0x2A3 */ u8 mUnknown2A3;          // At most 99, or 0xFF
}; // 0x2A4

// A saved character and what the game keeps beside it.
class TtsPlayer;

class SavedCharacter : public CharacterRecord {
public:
    SavedCharacter();

    void Reset();
    void MakeVisitor();
    void Register();
    BOOL Validate();
    static BOOL TerminateString(wchar_t *str, int length, const char *name);
    BOOL IsResident();
    void SetRegisteredDate(int year, int month, int day);
    void SetRegisteredDateToday();
    void AddUnknown1C4(int value);
    BOOL IsMiiFromThisConsole();
    void AssignCreateId();
    static void GetCreateId(CreateId *id, const CharacterRecord *character);
    u32 GetCreateIdNumber();
    void GetCreateIdParts(int *high, int *low);
    void GetParameterRange(int param, int *min, int *max);
    void CheckParameter(int param, int value);
    void SetParameter(int param, int value);
    void SetRawParameter(int param, int value);
    void SetSex(int sex);
    void SetLocalOnly(int value);
    void SetFavorite(int value);
    void SetBirthYear(u16 year);
    void SetOwnBirthMonth(int month);
    void SetOwnBirthDay(int day);
    void SetBloodType(int bloodType);
    void SetAnswer(int index, u8 answer);
    void SetVoice(int index, s16 value);
    void ResetVoice();
    int GetParameter(int param);
    int GetRawParameter(int param);
    GXRgb GetFaceColor();
    int RandomFromCreateId(int max);
    int GetAnswerScore01();
    int ScoreAnswers01(int answer0, int answer1);
    int GetAnswerScore23();
    int ScoreAnswers23(int answer2, int answer3);
    s8 GetPersonality();
    static BOOL Is0Or1(int value);
    void SetMoodFromResult(int result, int detail);
    void ResetMood();
    void AddRelationMeter(int index, int change);
    void SetRelation(int index, u8 rank, u8 meter);
    void ResetRelations();
    int FindOtherWithRank2478();
    int FindOtherWithRank2Or4();
    int FindOtherWithRank9();
    BOOL IsRank2478(int rank);
    BOOL IsRank2Or4(int rank);
    static BOOL IsRank678(int rank);
    static BOOL IsRank124(int rank);
    BOOL IsRank1249Or678(int rank);
    void SetLongAct(u8 state, u8 partner, s64 end);
    void ClearLongActFrom(s64 now);
    void ClearLongAct();
    void EndLongAct(SavedCharacter *first, SavedCharacter *second);
    void StartNextLongAct(s64 now, RTCTime *time, SavedCharacter *first, SavedCharacter *second);
    BOOL IsAway();
    static BOOL IsAwayState(int state);
    BOOL IsLongActPaired();
    static BOOL IsLongActPairedState(int state);
    BOOL IsLongActWithPartner();
    static BOOL IsLongActWithPartnerState(int state);
    BOOL IsLongActAsPartner();
    static BOOL IsLongActAsPartnerState(int state);
    static BOOL IsLongAct2BTo2E(int state);
    BOOL IsLongActIdle();
    static BOOL IsLongActIdleState(int state);
    s64 GetLongActDuration(int state, int shift);
    BOOL IsLongActAvailable(int state);
    int GetLongActWeight(int state);
    int GetTotalLongActWeight();
    int ChooseLongAct();
    bool IsInTrouble();
    static bool IsTroubleStateActive(int state);
    BOOL IsInTroubleWithoutUnknown4Bits8();
    void SetTrouble(u8 state, u8 category, u8 id);
    BOOL IsProgressingTrouble();
    BOOL IsProgressingTroubleInState2();
    void ResetTroubleProgress();
    void AddTroubleProgress(int value);
    static int GetTroubleTimeProgress(s64 now, s64 start);
    static u32 GetTroubleProgressAt(s64 now, s64 start, int progress);
    u32 GetTroubleProgress(s64 now);
    static BOOL IsTroubleProgressFullAt(s64 now, s64 start, int progress);
    BOOL IsTroubleProgressFull(s64 now);
    BOOL IsTroubleOfCategory34OrKind20();
    BOOL IsTroubleOfCategory3Kind8To10();
    BOOL IsTroubleOfCategory34OrKind18();
    BOOL IsTrouble3_4();
    static BOOL IsTrouble3_4State(int state, int category, int id);
    static int GetEncounterBonus(int base, int value);
    void StartLongAct(int state, s64 now, RTCTime *time, SavedCharacter *first, SavedCharacter *second);
    int ChooseTroubleCategory();
    int GetTroubleCategoryWeight(int category);
    int ChooseTrouble(int category);
    void EndTrouble(s64 now);
    void ClearTrouble();
    BOOL StartRandomTrouble(s64 now, int force, SavedCharacter *partner, int unused);
    BOOL AddSatisfaction(int value);
    void ResetSatisfaction();
    int GetFullness(s64 now);
    BOOL CanEat(s64 now, u16 item);
    void Eat(s64 now, u16 item);
    void ResetEating();
    void ClearUnknown246();
    void SetUnknown246ByChance();
    void SetMeetDate(const RTCDate *date);
    void GetMeetDate(RTCDate *date);
    void ResetMeetDate();
    void ResetTouchTalk();
    void ClearUnknown1C8();
    u32 CountOwnedClothes();
    void ClearOwnedClothes();
    u8 GetSongSlot(int slot);
    void SetSongSlot(int slot, u8 song);
    void ClearSongSlots();
    int GetTaste(int item);
    int GetLiking(int item);
    void SetUnknown292(int index, u8 value);
    u8 GetUnknown292(int index);
    void ResetItemPreferences();
    BOOL CheckUnknown292();
    void AddUnknown296(int value);
    void AddUnknown297(int value);
    static int GetRelationBloodPairValue(int a, int b);
    void ResetPatDate();
    void ResetUnknown2A3();
    void RandomizeUnknown24E();
    void ResetUnknown24E();
    int GetPersonalityPairValue(int a, int b);
    int GetClothesLiking(int clothes);
    int GetInteriorLiking(int interior);
    void ChooseFoodPreferences(u8 *favorite, u8 *disliked);
    BOOL AreFoodPreferencesValid();
    void ChooseNewFoodPreferences();
    int ScaleTalkWeightByAnswers(int kind, int weight);
    int ScaleTalkWeightByPersonalities(int kind, int weight, SavedCharacter *partner);
    int GetTalkReactionWeight(s8 relation, s8 partnerReaction, s8 reaction, SavedCharacter *partner);
    int GetTotalTalkReactionWeight(s8 relation, s8 partnerReaction, SavedCharacter *partner);
    int ChooseTalkReaction(s8 relation, s8 partnerReaction, SavedCharacter *partner);
    int ScaleReactionWeightByAnswers(int kind, int weight);
    int GetReactionWeight(s8 previous, s8 reaction);
    int GetTotalReactionWeight(s8 previous);
    int ChooseNextReaction(s8 previous);
    void SetVoiceOf(TtsPlayer *player);
    static int CompareRecords(const CharacterRecord *a, const CharacterRecord *b);
    BOOL IsLeapYear(int year);
    int GetDaysInYear(int year);
    int GetDaysInMonth(int month, int year);
    int GetDaysBetweenInYear(int month1, int day1, int month2, int day2, int year);
    int GetDaysBetween(int month1, int day1, int year1, int month2, int day2, int year2);
    int GetDaysBetween(RTCDate from, RTCDate to);
    void GetBirthday(RTCDate *date);
    int GetAgeInDays();
    int GetBiorhythm(int day);
    int GetBirthDayCycle();
    int GetPersonalityColumnValue(int row, const u8 *table);
    int GetPersonalityColumnOffset(int row);
    void UpdateBiorhythm();
    void AssignFreeRoom();
    u32 CountOwnedInteriors();
    void ExpireTrouble(s64 now, SavedCharacter *partner);
    void FailTrouble(s64 now, SavedCharacter *target);
    void EatRandomFood();
    void FailFightTrouble(s64 now, SavedCharacter *other);
    void BreakUp(s64 now, SavedCharacter *other);
    void Divorce(s64 now, SavedCharacter *other);
    BOOL StartTrouble(int category, int id, s64 now, BOOL shortened, SavedCharacter *partner, BOOL allowFight);
    s64 GetTroubleDuration(BOOL shortened);
    BOOL SetTroubleMood(int mood);
    int ChooseWantedFood();
    int ChooseWantedClothes(s64 now);
    int ChooseNewFriend();
    int ChooseFightOpponent();
    int ChooseLoverToLeave();
    int ChooseSpouseToLeave();
    void CountFights(int *residents, int *fights);
    void BecomeAngryAt(SavedCharacter *other, s64 now, SavedCharacter *partner);
    int ChooseLoveInterest();
    int ChooseCharacterByRelation();

    // The steps the trouble functions share
    void FinishTroubleUnsolved();
    void StartTroubleWith(int category, int id, SavedCharacter *other, s64 now);
    void StartHeartbreak(int id, SavedCharacter *other, s64 now);
    void BeginTrouble(int category, int id, u16 target, s64 now, BOOL shortened, SavedCharacter *partner);

    // Marks a row of the clothes table as owned
    void OwnClothes(int clothes) { mOwnedClothes.m[clothes >> 5] |= 1 << (clothes & 0x1F); }

    /* 0x2A4 */ u16 mIndex;              // Which of the saved characters this is
    /* 0x2A6 */ s16 mReaction;
    /* 0x2A8 */ int mBiorhythmCycles[3]; // Today's physical, intellectual and emotional cycles, 0 to 255
    /* 0x2B4 */ int mBiorhythm;          // Today's biorhythm, 0 to 1000 (UpdateBiorhythm)
    /* 0x2B8 */ int mBiorhythmYesterday;
    /* 0x2BC */ s8 mPersonality;         // 0..15
    /* 0x2BD */ u8 mUnknown2BD[3];
}; // 0x2C0

// What the Mii library's NFL_SetFaceDrawParam takes (lib/NFL's NFLDrawParam, which is C): the polygon attributes and
// material colours of a face.
struct FaceDrawParam {
    /* 0x00 */ int light;
    /* 0x04 */ GXPolygonMode polyMode;
    /* 0x08 */ int polygonID;
    /* 0x0C */ int alpha;
    /* 0x10 */ int misc;
    /* 0x14 */ GXRgb diffuse;
    /* 0x16 */ GXRgb ambient;
    /* 0x18 */ GXRgb specular;
    /* 0x1A */ GXRgb emission;
}; // 0x1C

// The character with its Mii face, which the Mii library (NFL) draws at the head's matrices.
class CharacterBase : public SavedCharacter {
public:
    // mFaceKind: which way the face moves by itself (UpdateFace)
    enum FaceKind {
        FACE_NONE,
        // 1 to 7: show expression 0 to 6 once
        FACE_BLINK = 8,          // 8, 9 and 13: blink, with eyes open in expression 0, 1 or 4
        FACE_LIP_SYNC_OPEN = 7,  // LipSync shows it while the mouth is open
    };

    CharacterBase();
    ~CharacterBase();

    void InitFace(u32 defaultMii);
    void ResetFace();
    void AddFace();
    void SetMiiParameter(int param, int value, BOOL makeFace);
    void MakeFace();
    void SetExpression(int expression);
    void SetDefaultMii(u32 index, BOOL makeFace);
    void SetRandomMii(int sex, int unk1, int unk2, BOOL makeFace);
    void SetHeadMatrices(const MtxFx43 *matrix, const MtxFx33 *rotation);
    BOOL HitTestFace();
    BOOL HitTestFace(const VecFx32 *origin, const VecFx32 *dir);
    BOOL HitTestFace(VecFx32 origin, VecFx32 dir) { return HitTestFace(&origin, &dir); }
    void DrawFace();
    void Draw();
    void SetFace(int kind);
    void UpdateBlink(int expression);
    void UpdateBlinkOrWink(int expression, fx32 closedTime);
    void UpdateRandomExpressions(int expression0, int expression1, int expression2, int expression3, fx32 time1,
                                 fx32 time2, int chance);
    void UpdateFace();
    void LipSync(const char *phoneme, int face);
    void UpdateHeadTurn();
    BOOL ClampHeadAngle(u16 *angle, int limit);
    static u16 StepAngle(u16 angle, u16 target, int shift);
    static fx32 GetAnimSpeedOf(int liveliness);

    /* 0x02C0 */ FaceDrawParam mFaceDrawParam;
    /* 0x02DC */ u8 mFace[0x2218];         // The library's face object (NFL_AddFace)
    /* 0x24F4 */ int mExpression;          // Given to NFL_SetExpression
    /* 0x24F8 */ int mFaceId;              // The face's id in the library, -1 for none
    /* 0x24FC */ BOOL mFaceVisible;
    /* 0x2500 */ int mFaceKind;            // How the face moves by itself (UpdateFace)
    /* 0x2504 */ fx32 mFaceTimerStep;      // What the face timer advances by in a frame
    /* 0x2508 */ fx32 mFaceTimer;
    /* 0x250C */ s16 mFaceStep;            // The step of the face's movement
    /* 0x2510 */ int mHeadTurn;            // Which of the head's angles turn (1: all three, 2, 3, 4: one), 0 none
    /* 0x2514 */ MtxFx33 mHeadRotation;    // The head joint's matrices, as the body last drew it
    /* 0x2538 */ MtxFx43 mHeadMatrix;
    /* 0x2568 */ VecFx32 mFaceOffset;
    /* 0x2574 */ VecFx32 mFaceScale;
    /* 0x2580 */ MtxFx33 mFaceRotation;
    /* 0x25A4 */ MtxFx43 mFaceMatrix;      // Where the face was drawn: the clip matrix
    /* 0x25D4 */ u16 mHeadPitchNow;        // Where the head is turned now
    /* 0x25D6 */ u16 mHeadYawNow;
    /* 0x25D8 */ u16 mHeadRollNow;
    /* 0x25DA */ u16 mHeadPitch;           // Where it turns to
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u16 mFaceRoll;            // Added to the face's roll
    /* 0x25E2 */ u8 mHeadTurnShift;        // The head turns by the distance shifted right by this, at least 0x20
    /* 0x25E3 */ u8 mUnknown25E3;
}; // 0x25E4

// A Mii head without a body: the face on a head model of its own (overlays 2, 77 and 79).
class CharacterHead : public CharacterBase {
public:
    // Implicit constructor; the destructor as the scenes' copies of it have it inlined (0x0203E064, 0x02040DC0)
    ~CharacterHead() { NNS_G3dRenderObjResetCallBack(&mHead.mRenderObj); }

    void Init(int resourceId, u32 defaultMii);
    void Update();
    void Draw();
    void SetAnimSpeed(fx32 speed);
    void SetAnimation(u32 resType, int id);
    void UpdateBlend();
    static void OnNode(NNSG3dRS *rs);
    static void InitCallbacks(NNSG3dRS *rs);

    /* 0x25E4 */ AnimModel mHead;
    /* 0x26C4 */ int mAnimId;              // The current 'BCA0' animation
    /* 0x26C8 */ int mPrevAnimId;
    /* 0x26CC */ fx32 mBlendFrames;
    /* 0x26D0 */ fx32 mAnimSpeed;
    /* 0x26D4 */ MtxFx33 mRootRotation;    // The matrices of the head model's node 0
    /* 0x26F8 */ MtxFx43 mRootMatrix;
}; // 0x2728

// The character with its body.
class Character : public CharacterBase, public CharacterModel {
public:
    void Init(int resourceId, int unk, u32 defaultMii);
    void SetArchive(const char *archiveName);
    void UpdateClothes();
    void Update();
    void UpdateSpeedScales();
    void UpdateWalkSpeedScale();
    void UpdateTurnSpeedScale();
    fx32 ScaleBySpeed(fx32 value);
    BOOL WalkToTarget(fx32 reach, fx32 speed, int turnShift);
    BOOL TurnToWalkTarget(int turnSpeed, int walkSpeed);
    BOOL TurnToTargetAngle(int turnSpeed, int walkSpeed);
    BOOL TurnTo(int turnSpeed, int walkSpeed, u16 angle);
    void MoveAlongAngle();
    void SetWalkTarget(VecFx32 target);
    void SetAngle(u16 angle);
    void Turn(u16 delta);
    void SetWalkTargetAwayFrom(FloorPos pos);
    void SetWalkTargetReflected(FloorPos pos);
    void Draw();
    void SetAnimSpeed(fx32 speed);
    void SetAnimation(u32 resType, int id);
    void BlendToAnimation(u32 resType, int id);
    void SetVisible(BOOL visible);
    void UpdateBodyScale();
    fx32 GetAnimSpeed();
    void UpdateAnimSpeed();
    void UpdateBodyColors();
    FloorPos GetPointInFront(fx32 distance);
    BOOL IsBeside(fx32 x, fx32 z, fx32 width);
    BOOL IsInFront(fx32 x, fx32 z);
    BOOL IsAhead(fx32 x, fx32 z, fx32 width);
    BOOL IsPointWithinReach(fx32 x, fx32 z, fx32 reach, fx32 maxReach);
    BOOL IsNearerThan(FloorPos pos, fx32 distanceSq);
    void SetLight(int light);
    void ResetSoundPhase(int slot);
    void ResetSoundPhases();
    int PlaySoundOnce(int player, int index, int seqArc);
    int PlaySoundAtAnimPhase(int slot, int player, int seqArc, int index, int quarters);

    // Where the character stands, on the floor. Filling a local and returning it is what leaves the dead copies of
    // the point on the callers' stacks.
    FloorPos GetFloorPos() const {
        FloorPos pos;
        pos.x = mPos.x;
        pos.z = mPos.z;
        return pos;
    }
    FloorPos GetTargetFloorPos() const {
        FloorPos pos;
        pos.x = mTarget.x;
        pos.z = mTarget.z;
        return pos;
    }

    /* 0x25E4 */ // CharacterModel
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mWalkSpeedScale;     // From the Mii's parameter 0x29 and mAnimSpeed
    /* 0x2AF4 */ fx32 mTurnSpeedScale;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to
    /* 0x2B04 */ int mTurnStep;            // How far it turned in the last step
    /* 0x2B08 */ u16 mMoveAngle;           // The way MoveAlongAngle moves it
    /* 0x2B0A */ u16 mAngle;               // The way the character faces
    /* 0x2B0C */ u16 mTargetAngle;         // TurnToTargetAngle turns mAngle towards it
    /* 0x2B0E */ u16 mWalkAngle;           // The direction to mTarget
    /* 0x2B10 */ s8 mSoundPhase[2];        // PlaySoundAtAnimPhase's progress through an animation, per slot
    /* 0x2B12 */ u8 mUnknown2B12[2];
    /* 0x2B14 */ fx32 mAnimSpeed;          // Given to the model when an animation starts
}; // 0x2B18

// The resident list's orders (src/character/CharacterSort.cpp). The name is the project's own.
class CharacterSort {
public:
    static void RemoveSpaces(wchar_t *str);
    static void KatakanaToHiragana(wchar_t *str);
    static void EnlargeSmallKana(wchar_t *str);
    static void RemoveVoicedMarks(wchar_t *str);
    static void ExpandLongVowelMarks(wchar_t *str);
    static void ExpandLigatures(wchar_t *str);
    static void ToUpperCase(wchar_t *str);
    static void ToUpperCaseWithoutAccents(wchar_t *str);
    static void ToCollationKeys(wchar_t *str);
    static int GetCollationKey(wchar_t c);
    static void MakeSortKeys(wchar_t *str);
    static int CompareNames(const SavedCharacter *a, const SavedCharacter *b, int field);
    static int CompareByName(const SavedCharacter *a, const SavedCharacter *b);
    static int CompareByFamilyName(const SavedCharacter *a, const SavedCharacter *b);
    static int CompareByMiiName(const SavedCharacter *a, const SavedCharacter *b);
    static int CompareStrings(const wchar_t *a, const wchar_t *b);
    static int CompareByPersonality(SavedCharacter *a, SavedCharacter *b);
    static int CompareByBirthday(SavedCharacter *a, SavedCharacter *b);
    static int CompareByBirthdayInYear(SavedCharacter *a, SavedCharacter *b);
    static int CompareBySex(SavedCharacter *a, SavedCharacter *b);
    static int CompareByBloodType(SavedCharacter *a, SavedCharacter *b);
    static int GetInitialGroup(wchar_t c);
    static int GetNameInitialGroup(const wchar_t *name);
};

#endif // CHARACTER_CHARACTER_H
