#include "Scene40.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneMemory.h"
#include "Scene40Pool.h"

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

// Not decompiled yet
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
}

// Overlay 75: scenes 0x40 and 0x6C, the scene of touching a Mii ("Scn/Scene/SceneMiiTouch_LZ.bin"). One source
// file by its one static initialiser. Most of it is not decompiled yet.

// --- The scene -----------------------------------------------------------------------------------------------------

// For the first nine frames the room and the character are updated from here as well
void Scene40::Draw() {
    MessageWindow_Draw(&mWindows[0]);
    MessageWindow_Draw(&mWindows[1]);
    func_ov075_0214fc00(&mPages);
    Sprite_Draw(&mSprite013D0);

    switch (mDrawState) {
    case 0:
        mRoom.Update();
        mCharacter.Update(FALSE);
        mDrawFrames++;
        if (mDrawFrames >= 9) {
            mDrawState = 1;
        }
        break;
    case 1:
        break;
    }

    ParticleSystem_Draw(&mParticles, 0);
    mCharacter.Draw();
    mFace.Draw();
    mMannequin.Draw();
    mRoom.RoomModels::Draw();
    for (int i = 0; i < 3; i++) {
        mItemModels[i].Draw();
    }
    mSpeechWindow.Draw();
}

// NOT MATCHING (the copy of the camera's eye and target only): the original keeps the camera's address in r5 and
// loads the words in pairs before storing them (x, y; store x; z, target x; store y and z together; ...), where
// this loads and stores one at a time through r1. Whole-vector and field-by-field assignments compile alike; a
// local pointer to the camera is worse.
// Saves what the scene changed (the character, with the date of this meeting, and the lists of clothes, goods and
// interiors), keeps the camera for the next scene, and unmounts the scene's archives
void Scene40::Exit() {
    RTCDate date;
    date.year = mClock.mDate.year;
    date.month = mClock.mDate.month;
    date.day = mClock.mDate.day;
    date.week = mClock.mDate.week;
    mCharacter.SetMeetDate(&date);
    GameData_WriteCharacter(&gGameData, mCharacter.mIndex, &mCharacter);
    if ((int)gGameData.mUnknownB2[0] < (int)mCharacter.mSatisfyLevel) {
        gGameData.mUnknownB2[0] = mCharacter.mSatisfyLevel;
    }
    mClothesList.Save();
    mInventory.Save();
    mInteriors.Save();
    gGameData.SetCappedCounter(mUnknown309B4);

    gSceneMemory.mCameraPosition = gCamera.mEye;
    gSceneMemory.mCameraTarget = gCamera.mTarget;

    DestroyMainFrameHeap();
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "SMT");
    ArchiveManager_Unmount(&gArchiveManager, "LTL");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TLM");
    ArchiveManager_Unmount(&gArchiveManager, "TIM");
    ArchiveManager_Unmount(&gArchiveManager, "TDM");
    ArchiveManager_Unmount(&gArchiveManager, "TRM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ModelResources_ReleaseModel(gModelResources, 2);
    ModelResources_ReleaseModel(gModelResources, 4);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ArchiveManager_Unmount(&gArchiveManager, "3HB");
    ArchiveManager_Unmount(&gArchiveManager, "3HT");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ModelResources_ReleaseModel(gModelResources, 12);
    ModelResources_ReleaseModel(gModelResources, 13);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ModelResources_ReleaseModel(gModelResources, 17);
    ArchiveManager_Unmount(&gArchiveManager, "3PM");
    mRoom.ReleaseAll();
    ArchiveManager_Unmount(&gArchiveManager, "3HM");
    for (int i = 0; i < 3; i++) {
        ArchiveManager_Unmount(&gArchiveManager, mItemModels[i].mArchiveName);
        ModelResources_ReleaseModel(gModelResources, i + 14);
    }
}

// --- The scene's character ------------------------------------------------------------------------------------------

// With speaking set, the lips follow the scene's speech
void Scene40Character::Update(BOOL speaking) {
    Character::Update();
    PlaceHeadIcons();
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    mBalloon.Update();
    if (!speaking) {
        return;
    }
    UpdateState();
    if (mTtsPlayer->IsEngineIdle()) {
        return;
    }
    if (!mLipSync) {
        return;
    }
    LipSync(mTtsPlayer->GetPhonemeName(), 1);
}

// objdiff scores this 99.5% though its instructions and relocations equal the original's (checked with objdump
// against the delinked object; check_relocs lists nothing): the two functions it calls are byte for byte the same,
// and the report seems to pair one call with the other's.
void Scene40Character::Draw() {
    Character::Draw();
    mBalloon.Draw();
    mCover.Draw();
}

// --- The scene's list: the shops' item list with a mark beside each row ------------------------------------------

void Scene40ListView::Draw() {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Draw(&mMarks[i]);
    }
    ItemListView::Draw();
}

void Scene40ListView::SetVisible(BOOL visible) {
    ItemListView::SetVisible(visible);
    for (int i = 0; i < mRowCount; i++) {
        mMarks[i].mVisible = visible;
    }
}

// A row's mark shows that the character has had the row's item: a bit of its record for each
void Scene40ListView::DrawRow(int row, int item) {
    ItemListView::DrawRow(row, item);
    BOOL visible = TRUE;
    switch (mMarkKind) {
    case MARK_NONE:
        visible = FALSE;
        break;
    case MARK_ITEMS:
        visible = mCharacter->mUnknown1C8.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_CLOTHES:
        visible = mCharacter->mOwnedClothes.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_INTERIORS:
        visible = mCharacter->mOwnedInteriors[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    }
    mMarks[row].mVisible = visible;
}

void Scene40ListView::ClearRow(int row) {
    ItemListView::ClearRow(row);
    mMarks[row].mVisible = FALSE;
}


// --- ov075-e: the character's helpers, its balloon and cover, the scene's clock, input, camera, lights, effects ---

extern "C" {
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
void AnimModel_SetSpeed(AnimModel *model, fx32 speed);
void func_0202a5a4(AnimModel *model, fx32 frame);
BOOL LineMeetsSphere(const VecFx32 *origin, const VecFx32 *dir, fx32 radius);
extern VecFx32 gTouchLineDirection; // The direction of the line through the touched point
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void Camera_Reset(Camera *camera);
void Camera_UpdateEye(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirection(Light *light, int unk, u16 angle, int unk2, VecFx32 direction);
void ScaleVec(fx32 scale, const VecFx32 *in, VecFx32 *out);
void Model_SetAlpha(Model *model, int alpha);
}

#define RESOURCE_BTA0 0x30415442 // Texture SRT animation

// The character's state machine: a new state takes effect at the next update, which sees mEntered set
void Scene40Character::SetState(int state) {
    mPrevState = mState;
    mNextState = state;
    mStep = 0;
    mEntered = TRUE;
    mFrame = 0;
}

void Scene40Character::ForceState(int state) {
    mNextState = state;
    mState = state;
    mStep = 0;
    mEntered = TRUE;
    mFrame = 0;
}

void Scene40Character::ShowBalloon(int state) {
    mBalloon.ChooseContent();
    mBalloon.SetState(state);
}

BOOL Scene40Character::IsBalloonTouched() {
    return mBalloon.IsTouched();
}

void Scene40Character::UpdateCameraAngle() {
    u16 angle = mAngle;
    mCameraAngle = angle - GetAngleTo(&gCamera.mEye);
}

BOOL Scene40Character::IsFacingCameraLoosely() {
    BOOL facing = TRUE;
    if (mCameraAngle >= 0x4000 && mCameraAngle <= 0xC000) {
        facing = FALSE;
    }
    return facing;
}

BOOL Scene40Character::IsFacingCamera() {
    BOOL facing = TRUE;
    if (mCameraAngle >= 0x2800 && mCameraAngle <= 0xD800) {
        facing = FALSE;
    }
    return facing;
}

BOOL Scene40Character::IsFacingCameraClosely() {
    BOOL facing = TRUE;
    if (mCameraAngle >= 0x1000 && mCameraAngle <= 0xF000) {
        facing = FALSE;
    }
    return facing;
}

// Where a thing thrown from pos with a velocity lands on the floor
FloorPos GetLandingPoint(VecFx32 pos, VecFx32 velocity) {
    int frames = Scene40Physics::GetFallFrames(pos.y, velocity.y);
    FloorPos landing;
    landing.x = pos.x + velocity.x * frames;
    landing.z = pos.z + velocity.z * frames;
    return landing;
}

void Scene40Character::SetGift(Scene40Gift *gift) {
    mGift.Set(gift->mKind, gift->mItem.InventoryItem::mId);
}

// What the record's liking of an item (SavedCharacter::GetLiking and the like, 0 to 17) gives: a reaction, 0 (the
// least) to 4, and how much the gift satisfies, by the main program's tables. 18 and 19 are the goods it likes
// most (mFavoriteItem) and least (mDislikedItem).
static const int sGoodsReactions[18] = {19, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 18};
static const int sClothesReactions[16] = {9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5};
static const int sInteriorReactions[16] = {4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0};

extern "C" {
extern const s8 gGoodsSatisfactions[];    // By liking (0x020F8E78)
extern const s8 gClothesSatisfactions[];  // By liking (0x020F8E58)
extern const s8 gInteriorSatisfactions[]; // By liking (0x020F8E68)
}

// How much a gift of goods satisfies: the main program's value by liking, by the size of the food (half, 80% or
// all of it), and a hundredth of the price for goods it likes; at least 40 for its favourite, at most 70
int Scene40Character::GetGoodsSatisfaction(int item) {
    int liking = GetLiking(item);
    const ItemInfo *info = &gGoodsTable[(u16)item];
    int percent;
    int value = gGoodsSatisfactions[liking];
    switch (info->mFoodSize) {
    case 0:
        percent = 50;
        break;
    case 1:
        percent = 80;
        break;
    case 2:
        percent = 100;
        break;
    }
    int bonus = (int)info->mPrice / 100;
    if (liking >= 17) {
        value = bonus + value * percent / 100;
        if (value < 40) {
            value = 40;
        }
    } else if (liking > 6) {
        value = bonus + value * percent / 100;
    } else if (liking > 0) {
        value = value * percent / 100;
    }
    if (value > 70) {
        value = 70;
    } else if (value < -100) {
        value = -100;
    }
    return value;
}

int Scene40Character::GetGoodsReaction(int item) {
    return sGoodsReactions[GetLiking(item)];
}

int Scene40Character::GetClothesSatisfaction(int clothes) {
    return gClothesSatisfactions[GetClothesLiking(clothes)];
}

int Scene40Character::GetClothesReaction(int clothes) {
    return sClothesReactions[GetClothesLiking(clothes)];
}

int Scene40Character::GetInteriorReaction(int interior) {
    return sInteriorReactions[GetInteriorLiking(interior)];
}

int Scene40Character::GetInteriorSatisfaction(int interior) {
    return gInteriorSatisfactions[GetInteriorLiking(interior)];
}

// The record keeps the three goods it liked best of those given (mUnknown292, 0xFF for none), best first. An item
// it likes at least 11 (and that is not its favourite) takes its place among them; TRUE if it did.
BOOL Scene40Character::RankLikedItem(int item) {
    int liking = GetLiking(item);
    if (item == mFavoriteItem) {
        liking = 0;
    }
    BOOL ranked = FALSE;
    int slot = 0;
    if (liking >= 11) {
        for (int i = 0; i < 3; i++) {
            if (item == GetUnknown292(i)) {
                ranked = FALSE;
                break;
            }
            if (GetUnknown292(i) == 0xFF) {
                slot = i;
                ranked = TRUE;
                break;
            }
            if (GetLiking(GetUnknown292(i)) < liking) {
                slot = i;
                ranked = TRUE;
                break;
            }
        }
    }
    if (ranked) {
        for (int i = 2; i > slot; i--) {
            SetUnknown292(i, GetUnknown292(i - 1));
        }
        SetUnknown292(slot, item);
    }
    return ranked;
}

// The main program's table of troubles (by category, then id), as far as this file reads it
struct Scene40TroubleInfo {
    /* 0x0 */ u32 mKind : 8;
    /*     */ u32 mUnknown0Bits8 : 24;
    /* 0x4 */ u32 mUnknown4;
    /* 0x8 */ u32 mUnknown8;
}; // 0xC

extern "C" {
extern const Scene40TroubleInfo *const data_02114d78[];
}

// In a trouble of kind 1 while still full, mUnknown23C is put back by an hour for every tenth of fullness, and one
void Scene40Character::SetUnknown23CFromFullness(s64 now) {
    if (!IsInTrouble()) {
        return;
    }
    if (data_02114d78[mTroubleCategory][mTroubleId].mKind != 1) {
        return;
    }
    if (GetFullness(now) <= 0) {
        return;
    }
    mUnknown23C = now - (mEatFullness / 10 + 1) * 3600;
}

// Three things of the visit add to the record's mUnknown1C4 only once each
void Scene40Character::AddUnknown1C4Once0() {
    if (mUnknown1C4Added[0]) {
        return;
    }
    AddUnknown1C4(2);
    mUnknown1C4Added[0] = TRUE;
}

void Scene40Character::AddUnknown1C4Once1() {
    if (mUnknown1C4Added[1]) {
        return;
    }
    AddUnknown1C4(5);
    mUnknown1C4Added[1] = TRUE;
}

void Scene40Character::AddUnknown1C4Once2() {
    if (mUnknown1C4Added[2]) {
        return;
    }
    AddUnknown1C4(2);
    mUnknown1C4Added[2] = TRUE;
}

void Scene40Character::AddUnknown1C4ByTrouble() {
    if (IsInTrouble()) {
        AddUnknown1C4(1);
    } else {
        AddUnknown1C4(2);
    }
}

void Scene40Character::ClearMoodIfTroubleOfCategory4OrKind8() {
    if (mMood == 0) {
        return;
    }
    if ((IsInTrouble() && mTroubleCategory == 4) ||
        (IsInTrouble() && data_02114d78[mTroubleCategory][mTroubleId].mKind == 8)) {
        mMood = 0;
    }
}

void Scene40Character::ClearMoodIfTroubleOfKind8() {
    if (mMood == 0) {
        return;
    }
    if (!IsInTrouble()) {
        return;
    }
    if (data_02114d78[mTroubleCategory][mTroubleId].mKind == 8) {
        mMood = 0;
    }
}

// --- The character's talk ---------------------------------------------------------------------------------------

extern "C" {
// The personalities' kinds (0 to 3), by personality
extern const int gPersonalityKinds[];
int MiiData_GetSex(SavedCharacter *character);
}

// The tools an entry can require to have been had (Scene40TalkEntry::mItem: goods rows, by the record's bits at
// 0x1C8)
static const int sTalkItems[9] = {0, 236, 241, 243, 242, 239, 240, 237, 238};
static const int sPersonalityBits[16] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};
static const int sMonthBits[13] = {156, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};
static const int sHourBits[24] = {8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4};
static const int sWeekdayBits[7] = {1, 2, 4, 8, 16, 32, 64};
// The days since the last meeting at which each step of Scene40TalkConditions::mElapsed begins
static const u16 sElapsedDays[9] = {0, 0, 7, 14, 21, 28, 51, 81, 111};

// Whether an entry's conditions all hold
BOOL Scene40Character::MatchesTalk(const Scene40TalkEntry *entry, RTCDate *date, Scene40TalkConditions *conditions) {
    BOOL matches = TRUE;
    if (!(entry->mSexes & conditions->mSex)) {
        matches = FALSE;
    }
    if (!(entry->mPersonalities & conditions->mPersonality)) {
        matches = FALSE;
    }
    if (!(entry->mMoods & conditions->mMood)) {
        matches = FALSE;
    }
    if (!(entry->mMonths & conditions->mMonth)) {
        matches = FALSE;
    }
    if (!(entry->mHours & conditions->mHour)) {
        matches = FALSE;
    }
    if (!(entry->mWeekdays & conditions->mWeekday)) {
        matches = FALSE;
    }
    if (entry->mDay != 0 && entry->mDay != date->day) {
        matches = FALSE;
    }
    if (entry->mNeedsCharacters == 1 && conditions->mHasCharacters == FALSE) {
        matches = FALSE;
    }
    if (entry->mNeedsFriends == 1 && conditions->mHasFriends == FALSE) {
        matches = FALSE;
    }
    if (entry->mNeedsLover == 1 && conditions->mHasLover == FALSE) {
        matches = FALSE;
    }
    if (entry->mNeedsSpouse == 1 && conditions->mHasSpouse == FALSE) {
        matches = FALSE;
    }
    if (entry->mItem != 0) {
        int item = sTalkItems[entry->mItem];
        if (!((mUnknown1C8.m[item >> 5] >> (item & 31)) & 1)) {
            matches = FALSE;
        }
    }
    if (entry->mElapsed != 0 && entry->mElapsed != conditions->mElapsed) {
        matches = FALSE;
    }
    return matches;
}

static inline RTCTime Midnight() {
    RTCTime time = {0, 0, 0};
    return time;
}

// What the entries are tested against: the character's sex, mood and personality, the month, hour and day of the
// week, how long since the last meeting, and its relations
void Scene40Character::GetTalkConditions(RTCDate *date, RTCTime *time, Scene40TalkConditions *conditions) {
    mPersonality = GetPersonality();
    if (MiiData_GetSex(this) == 0) {
        conditions->mSex = 1;
    }
    if (MiiData_GetSex(this) == 1) {
        conditions->mSex = 2;
    }
    conditions->mPersonality = sPersonalityBits[mPersonality];
    switch (mMood) {
    case 0:
        conditions->mMood = 1;
        break;
    case 1:
        conditions->mMood = 2;
        break;
    case 2:
        conditions->mMood = 4;
        break;
    case 3:
        conditions->mMood = 8;
        break;
    case 4:
        conditions->mMood = 16;
        break;
    }
    conditions->mMonth = sMonthBits[date->month];
    conditions->mHour = sHourBits[time->hour];
    conditions->mWeekday = sWeekdayBits[date->week];
    RTCDate meet;
    RTCTime midnight = Midnight();
    GetMeetDate(&meet);
    s64 met = RTC_ConvertDateTimeToSecond(&meet, &midnight);
    int days = (RTC_ConvertDateTimeToSecond(date, time) - met) / 86400;
    if (days < 0) {
        days = 0;
    }
    for (int i = 8; i >= 0; i--) {
        if (sElapsedDays[i] <= days) {
            conditions->mElapsed = i;
            break;
        }
    }
    conditions->mHasCharacters = mCharacterCount >= 3;
    conditions->mHasFriends = mFriendCount >= 2;
    conditions->mHasLover = mLover != -1;
    conditions->mHasSpouse = mSpouse != -1;
}

extern "C" {
// INTERIM: the talk tables by personality kind, with their entry counts (the file's data, laid out at the end)
extern const Scene40TalkEntry *const sFirstTalks[4];   // The first time the player touches the character
extern const int sFirstTalkCounts[4];
extern const Scene40TalkEntry *const sTouchTalks[4];   // Being touched
extern const int sTouchTalkCounts[4];
extern const Scene40TalkEntry *const sRecentTalks[4];  // One time in ten, when they met within the week
extern const int sRecentTalkCounts[4];
extern const Scene40TalkEntry *const sNewsTalks1[4];   // Something to tell (record byte 0x24A) of kind 1
extern const int sNewsTalk1Counts[4];
extern const Scene40TalkEntry *const sNewsTalks2[4];   // of kind 2
extern const int sNewsTalk2Counts[4];
extern const Scene40TalkEntry *const sNewsTalksFE[4];  // of kind 0xFE (and 2 when its birthday is set)
extern const int sNewsTalkFECounts[4];
}

// A random number below max, compared as an int
static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// The two arguments of a message: up to two, while the entry has a type for them. Two friends are picked at
// random for types 16 and 17.
void Scene40Character::GetTalkArguments(const Scene40TalkEntry *entry, int *count, int *types, int *values) {
    int friends[2] = {0, 0};
    u16 foods[256];
    int n;
    int wanted = 2;
    for (int i = 0; i < mFriendCount; i++) {
        if (Rand(mFriendCount - i) < wanted) {
            friends[2 - wanted] = mFriends[i];
            wanted--;
        }
    }
    if (Rand(100) < 50) {
        int other = friends[0];
        friends[0] = friends[1];
        friends[1] = other;
    }
    for (*count = 0; (*count == 0 ? entry->mArgType0 : entry->mArgType1) != 0 && *count < 2; (*count)++) {
        types[*count] = *count == 0 ? entry->mArgType0 : entry->mArgType1;
        switch (types[*count]) {
        case 1:
        case 4:
        case 7:
        case 10:
        case 15:
            values[*count] = mIndex;
            break;
        case 14:
            values[*count] = 0;
            break;
        case 13:
            n = 0;
            for (int item = 0; item < 255; item++) {
                int kind = gGoodsTable[(u16)item].mKind;
                if (kind == 1 || kind == 4) {
                    foods[n++] = item;
                }
            }
            values[*count] = foods[Rand(n)];
            break;
        case 2:
        case 3:
        case 5:
        case 6:
        case 8:
        case 9:
        case 11:
        case 12:
            values[*count] = 0;
            break;
        case 16:
            values[*count] = friends[0];
            break;
        case 17:
            values[*count] = friends[1];
            break;
        case 18:
            values[*count] = mLover;
            break;
        case 19:
            values[*count] = mSpouse;
            break;
        default:
            values[*count] = 0;
            break;
        }
    }
}

// What the character says when touched: the first time, its first-meeting talk (and the record's bit 0x54.20 is
// set); one time in ten, when they met within the week, a talk of those; otherwise a touch talk. An entry whose
// conditions hold is chosen at random, or the first first-meeting talk if none does.
void Scene40Character::ChooseTouchTalk(u32 *message, int *count, int *types, int *values) {
    RTCDate date;
    RTCTime time;
    Scene40TalkConditions conditions;
    const Scene40TalkEntry *entry;
    Scene40Clock *clock = mContext->mClock;
    date.year = clock->mDate.year;
    date.month = clock->mDate.month;
    date.day = clock->mDate.day;
    date.week = clock->mDate.week;
    clock = mContext->mClock;
    time.hour = clock->mTime.hour;
    time.minute = clock->mTime.minute;
    time.second = clock->mTime.second;
    GetTalkConditions(&date, &time, &conditions);
    if (!mUnknown054Bit20) {
        int found[33];
        int n = 0;
        int total = sFirstTalkCounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sFirstTalks[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sFirstTalks[gPersonalityKinds[mPersonality]][found[MATH_Rand16(&gRandom, n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        mUnknown054Bit20++;
    } else if (MATH_Rand16(&gRandom, 10) != 0 || conditions.mElapsed != 1) {
        int found[65];
        int n = 0;
        int total = sTouchTalkCounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sTouchTalks[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sTouchTalks[gPersonalityKinds[mPersonality]][found[MATH_Rand16(&gRandom, n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
    } else {
        int found[48];
        int n = 0;
        int total = sRecentTalkCounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sRecentTalks[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sRecentTalks[gPersonalityKinds[mPersonality]][found[MATH_Rand16(&gRandom, n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
    }
    *message = entry->mMessage;
    GetTalkArguments(entry, count, types, values);
}

// Something to tell, of a kind (the record's byte 0x24A): kind 2 becomes 0xFE when the Mii has a birthday set
void Scene40Character::ChooseNewsTalk(int kind, u32 *message, int *count, int *types, int *values) {
    RTCDate date;
    RTCTime time;
    Scene40TalkConditions conditions;
    const Scene40TalkEntry *entry;
    int found[393];
    Scene40Clock *clock = mContext->mClock;
    date.year = clock->mDate.year;
    date.month = clock->mDate.month;
    date.day = clock->mDate.day;
    date.week = clock->mDate.week;
    clock = mContext->mClock;
    time.hour = clock->mTime.hour;
    time.minute = clock->mTime.minute;
    time.second = clock->mTime.second;
    GetTalkConditions(&date, &time, &conditions);
    int n = 0;
    if (kind == 2) {
        int month = GetParameter(PARAM_BIRTH_MONTH);
        int day = GetParameter(PARAM_BIRTH_DAY);
        if (month != 0 && day != 0) {
            kind = 0xFE;
        }
    }
    switch (kind) {
    case 1: {
        int total = sNewsTalk1Counts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sNewsTalks1[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sNewsTalks1[gPersonalityKinds[mPersonality]][found[Rand(n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        break;
    }
    case 2: {
        int total = sNewsTalk2Counts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sNewsTalks2[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sNewsTalks2[gPersonalityKinds[mPersonality]][found[Rand(n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        break;
    }
    case 0xFE: {
        int total = sNewsTalkFECounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sNewsTalksFE[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sNewsTalksFE[gPersonalityKinds[mPersonality]][found[Rand(n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        break;
    }
    }
    *message = entry->mMessage;
    GetTalkArguments(entry, count, types, values);
}

// Something to tell the player (the record's byte 0x24A: 1, 2 or 0xFE, the kind of talk; 0 or 0xFF for none)
BOOL Scene40Character::HasSomethingToTell() {
    BOOL has = FALSE;
    if (mUnknown24A != 0 && mUnknown24A != 0xFF) {
        has = TRUE;
    }
    return has;
}

// The record's byte 0x246, which the main program sets by chance as the character falls asleep (long acts 0x12
// and 0xF, SavedCharacter::SetUnknown246ByChance): the balloon then shows a dream
BOOL Scene40Character::IsDreaming() {
    return mUnknown246 != 0;
}

// Says what it has to tell (the speech's file 1), and has nothing to tell any more. Kind 1 keeps the year in
// the record's byte 0x2A3.
void Scene40Character::TellNews() {
    if (!HasSomethingToTell()) {
        return;
    }
    u32 message;
    int count;
    int types[2];
    int values[2];
    ChooseNewsTalk(mUnknown24A, &message, &count, types, values);
    Speak(1, message, count, types, values, 0);
    switch (mUnknown24A) {
    case 1:
        mUnknown2A3 = mContext->mClock->mDate.year;
        break;
    case 2:
    case 0xFE:
        break;
    }
    mUnknown24A = 0xFF;
}

// The things the character can propose to do (each a scene of its own, which overlay 75 requests), and how many
// saved characters each needs
struct Scene40Activity {
    /* 0x0 */ int mScene;
    /* 0x4 */ u8 mMinCharacters;
    /* 0x5 */ u8 mUnknown5;
    /* 0x6 */ u8 mUnknown6;
    /* 0x7 */ u8 mUnknown7;
    /* 0x8 */ u8 mUnknown8;
    /* 0x9 */ u8 mEnabled;
}; // 0xC

static const Scene40Activity sActivities[20] = {
    {24, 2, 0, 9, 0, 0, 1},    {28, 2, 0, 166, 0, 0, 1},  {43, 2, 1, 14, 1, 0, 1},   {50, 4, 0, 218, 0, 0, 1},
    {54, 1, 1, 24, 1, 0, 1},   {55, 3, 0, 169, 0, 0, 1},  {58, 1, 0, 35, 0, 0, 1},   {61, 2, 1, 104, 0, 0, 1},
    {76, 2, 0, 167, 0, 0, 1},  {77, 2, 0, 151, 0, 0, 1},  {80, 1, 0, 0, 0, 1, 1},    {82, 1, 0, 164, 0, 0, 1},
    {118, 1, 0, 125, 0, 0, 1}, {123, 3, 0, 220, 0, 0, 1}, {136, 1, 0, 219, 0, 0, 1}, {139, 1, 0, 189, 0, 0, 1},
    {141, 1, 0, 153, 0, 0, 1}, {143, 5, 1, 0, 0, 1, 1},   {144, 5, 1, 0, 0, 1, 1},   {145, 2, 0, 0, 0, 1, 1},
};

// One of the activities that are enabled and that there are enough characters for, at random
int Scene40Character::ChooseActivity() {
    int candidates[20];
    int count = 0;
    int i;
    for (i = 0; i < 20; i++) {
        if (sActivities[i].mEnabled == 1 && sActivities[i].mMinCharacters <= mCharacterCount) {
            candidates[count++] = i;
        }
    }
    return candidates[MATH_Rand16(&gRandom, count)];
}

// The game keeps a bit for each activity that was proposed (gGameData+0x110)
void MarkActivityProposed(int activity) {
    gGameData.mActivitiesProposed |= 1 << activity;
}

// --- The character's head ---------------------------------------------------------------------------------------

u16 Scene40Character::GetHeadYawTo(VecFx32 *target) {
    return mAngle - FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

u16 Scene40Character::GetAngleTo(VecFx32 *target) {
    return FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

// --- The balloon ------------------------------------------------------------------------------------------------

void Scene40Balloon::Init(int resourceId, int unk, CharacterBase *face, Scene40Character *character) {
    mModel.Load(resourceId, unk);
    PlayAnimation(RESOURCE_BCA0, "appear");
    PlayAnimation(RESOURCE_BTP0, "appear");
    mState = STATE_HIDDEN;
    mContent = CONTENT_NONE;
    mUnknown08 = FALSE;
    mCloseQueued = FALSE;
    mCharacter = character;
    mFace = face;
    SetState(STATE_HIDDEN);
}

void Scene40Balloon::Update() {
    switch (mState) {
    case STATE_HIDDEN:
        break;
    case STATE_SHOWN:
        if (mCloseQueued) {
            SetState(STATE_DISAPPEAR);
        }
        break;
    case STATE_APPEAR:
        if (FindAnimation(RESOURCE_BTP0, "appear")->mFinished) {
            SetState(STATE_SHOWN);
        }
        break;
    case STATE_DISAPPEAR:
        mCloseQueued = FALSE;
        if (FindAnimation(RESOURCE_BTP0, "disappear")->mFinished) {
            SetState(STATE_HIDDEN);
        }
        break;
    }
    UpdatePosition();
}

// The length of (x, z), by the hardware square root
static inline fx32 SqrtXZ(fx32 x, fx32 z) {
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

static const VecFx32 sBalloonUp = {0, FX32_ONE, 0};

static inline VecFx32 GetPos(Character *character) {
    return character->mPos;
}

// Over the character's head, beside it as seen from the camera (on the other side when it dreams, lying down),
// turned to the camera; the face in it looks at the camera
void Scene40Balloon::UpdatePosition() {
    if (mState == STATE_HIDDEN) {
        return;
    }
    VecFx32 offset;
    VecFx32 side;
    VecFx32 up = sBalloonUp;
    VecFx32 toCamera;
    VecFx32 head;
    Camera *camera = &gCamera;
    if (mContent == CONTENT_DREAM) {
        head = mCharacter->GetJointPosition(CharacterModel::JOINT_NECK);
        VEC_Subtract(&head, &camera->mEye, &toCamera);
        VEC_CrossProduct(&up, &toCamera, &side);
        VEC_Normalize(&side, &offset);
        ScaleVec(-0xCCC, &offset, &offset);
        offset.y += 0x1666;
        VEC_Add(&offset, &head, &mModel.mPosition);
        mModel.mRotY = FX_Atan2Idx(-toCamera.z, toCamera.x) + 0x4000;
    } else {
        head = GetPos(mCharacter);
        head.y = mCharacter->GetJointPosition(CharacterModel::JOINT_NECK).y;
        VEC_Subtract(&head, &camera->mEye, &toCamera);
        VEC_CrossProduct(&up, &toCamera, &side);
        VEC_Normalize(&side, &offset);
        ScaleVec(0x1666, &offset, &offset);
        offset.y += 0x14CC;
        VEC_Add(&offset, &head, &mModel.mPosition);
        mModel.mRotY = FX_Atan2Idx(-toCamera.z, toCamera.x) - 0x4000;
    }
    VecFx32 eye = camera->mEye;
    fx32 y = mModel.mPosition.y;
    u16 pitch = FX_Atan2Idx(eye.y - y, SqrtXZ(eye.x - mFace->mFaceOffset.x, eye.z - mFace->mFaceOffset.z));
    u16 yaw = -FX_Atan2Idx(eye.z - mFace->mFaceOffset.z, eye.x - mFace->mFaceOffset.x);
    mFace->mHeadPitchNow = -pitch;
    mFace->mHeadYawNow = yaw + 0x4000;
    MtxFx43 *matrix = &mFace->mHeadMatrix;
    matrix->_30 = mModel.mPosition.x;
    matrix->_31 = mModel.mPosition.y - 0x666;
    matrix->_32 = mModel.mPosition.z;
}

void Scene40Balloon::Draw() {
    mModel.Draw();
}

void Scene40Balloon::SetState(int state) {
    switch (state) {
    case STATE_HIDDEN:
        mState = STATE_HIDDEN;
        mModel.mVisible = FALSE;
        break;
    case STATE_SHOWN:
        mState = STATE_SHOWN;
        mModel.mVisible = TRUE;
        switch (mContent) {
        case CONTENT_NONE:
            break;
        case CONTENT_TROUBLE:
            PlayAnimation(RESOURCE_BCA0, "wait_etc");
            PlayAnimation(RESOURCE_BTP0, "wait_etc");
            FindAnimation(RESOURCE_BTP0, "wait_etc")->mPlayOnce = FALSE;
            break;
        case CONTENT_FACE:
            PlayAnimation(RESOURCE_BCA0, "wait_plain");
            PlayAnimation(RESOURCE_BTP0, "wait_plain");
            FindAnimation(RESOURCE_BTP0, "wait_plain")->mPlayOnce = FALSE;
            mFace->mFaceVisible = TRUE;
            break;
        case CONTENT_DOTS:
            PlayAnimation(RESOURCE_BCA0, "wait_dot");
            PlayAnimation(RESOURCE_BTP0, "wait_dot");
            FindAnimation(RESOURCE_BTP0, "wait_dot")->mPlayOnce = FALSE;
            break;
        case CONTENT_DREAM:
            PlayAnimation(RESOURCE_BCA0, "wait_dream");
            PlayAnimation(RESOURCE_BTP0, "wait_dream");
            FindAnimation(RESOURCE_BTP0, "wait_dream")->mPlayOnce = FALSE;
            break;
        }
        break;
    case STATE_APPEAR:
        if (mContent == CONTENT_NONE) {
            return;
        }
        if (mUnknown08) {
            return;
        }
        mState = STATE_APPEAR;
        mModel.mVisible = TRUE;
        PlayAnimation(RESOURCE_BCA0, "appear");
        PlayAnimation(RESOURCE_BTP0, "appear");
        FindAnimation(RESOURCE_BTP0, "appear")->mPlayOnce = TRUE;
        mModel.mScale.x = FX32_ONE;
        mModel.mScale.y = FX32_ONE;
        mModel.mScale.z = FX32_ONE;
        break;
    case STATE_DISAPPEAR:
        if (mState == STATE_SHOWN) {
            mState = STATE_DISAPPEAR;
            mModel.mVisible = TRUE;
            PlayAnimation(RESOURCE_BCA0, "disappear");
            PlayAnimation(RESOURCE_BTP0, "disappear");
            FindAnimation(RESOURCE_BTP0, "disappear")->mPlayOnce = TRUE;
            mFace->mFaceVisible = FALSE;
            mCloseQueued = FALSE;
        } else if (mState != STATE_HIDDEN) {
            mCloseQueued = TRUE;
        }
        break;
    }
}

// What the balloon shows, by what the character is doing and its trouble
void Scene40Balloon::ChooseContent() {
    BOOL asleep = mCharacter->mLongActState == 18;
    BOOL act15 = mCharacter->mLongActState == 15;
    BOOL inTrouble = mCharacter->IsInTrouble();
    BOOL hasNews = mCharacter->HasSomethingToTell();
    BOOL dreaming = mCharacter->IsDreaming();
    BOOL thinksOfSomeone = FALSE;
    if (inTrouble && mCharacter->mTroubleCategory == 4) {
        thinksOfSomeone = TRUE;
    }
    mContent = CONTENT_NONE;
    if (asleep || act15) {
        if (dreaming) {
            mContent = CONTENT_DREAM;
        }
        return;
    }
    if (thinksOfSomeone) {
        mContent = CONTENT_FACE;
        u16 target = mCharacter->mTroubleTarget;
        GameData_ReadCharacter(&gGameData, target, mFace);
        mFace->mIndex = target;
        mFace->MakeFace();
        return;
    }
    if (inTrouble) {
        mContent = CONTENT_TROUBLE;
        return;
    }
    if (hasNews) {
        mContent = CONTENT_DOTS;
    }
}

// Whether the line through the touched point passes within 0.5 of the balloon, while it is out
BOOL Scene40Balloon::IsTouched() {
    VecFx32 offset;
    VecFx32 pos = mModel.mPosition;
    VecFx32 dir = gTouchLineDirection;
    VEC_Subtract(&gCamera.mEye, &pos, &offset);
    return LineMeetsSphere(&offset, &dir, FX32_HALF) && mState != STATE_HIDDEN;
}

// --- The cover --------------------------------------------------------------------------------------------------

void Scene40Cover::Init(int kind) {
    mKind = kind;
    switch (kind) {
    case KIND_BED:
        mModel.Load(4, 0);
        mModel.mVisible = FALSE;
        break;
    case KIND_CURTAIN:
        mModel.Load(4, 0);
        PlayAnimation(RESOURCE_BCA0, "appear00");
        PlayAnimation(RESOURCE_BTA0, "appear00");
        mModel.mVisible = FALSE;
        mDisappearing = FALSE;
        break;
    }
}

void Scene40Cover::Draw() {
    mModel.Draw();
}

// Swaps the model: the bed or the curtain, with their archives. The curtain's path has no directory here, unlike
// the one StartLoading mounts ("Mdl/Misc/Tch/mii_tch_curtain_3D_LZ.bin"): so the ROM has it.
void Scene40Cover::SetKind(int kind) {
    if (mKind == kind) {
        return;
    }
    mKind = kind;
    switch (kind) {
    case KIND_BED:
        ModelResources_ReleaseModel(gModelResources, 4);
        ArchiveManager_Unmount(&gArchiveManager, "3HT");
        ArchiveManager_MountCompressed(&gArchiveManager, "3HT", "Mdl/Goods/gd10_3D_LZ.bin", 1);
        ModelResources_SetModel(gModelResources, 4, "3HT:/goods.imd");
        mModel.Load(4, 0);
        mModel.mVisible = FALSE;
        break;
    case KIND_CURTAIN:
        ModelResources_ReleaseModel(gModelResources, 4);
        ArchiveManager_Unmount(&gArchiveManager, "3HT");
        ArchiveManager_MountCompressed(&gArchiveManager, "3HT", "mii_tch_curtain_3D_LZ.bin", 1);
        ModelResources_SetModel(gModelResources, 4, "3HT:/curtain.imd");
        ModelResources_SetModel(gModelResources, 4, "3HT:/appear00.ica");
        ModelResources_SetModel(gModelResources, 4, "3HT:/appear00.ita");
        mModel.Load(4, 0);
        PlayAnimation(RESOURCE_BCA0, "appear00");
        PlayAnimation(RESOURCE_BTA0, "appear00");
        mModel.mVisible = FALSE;
        mDisappearing = FALSE;
        break;
    }
}

void Scene40Cover::SetPosition(VecFx32 pos, u16 angle) {
    switch (mKind) {
    case KIND_BED:
        mModel.SetPosition(pos);
        mModel.mRotY = angle;
        break;
    case KIND_CURTAIN:
        mModel.SetPosition(pos);
        mModel.mRotY = angle;
        break;
    }
}

void Scene40Cover::SetDiffuse(GXRgb color) {
    NNS_G3dMdlSetMdlDiff(mModel.mResMdl, 0, color);
}

// The curtain comes down around the character
void Scene40Cover::Appear() {
    if (mKind != KIND_CURTAIN) {
        return;
    }
    PlayAnimation(RESOURCE_BCA0, "appear00");
    PlayAnimation(RESOURCE_BTA0, "appear00");
    AnimModel_SetSpeed(&mModel, FX32_ONE);
    FindAnimation(RESOURCE_BCA0, "appear00")->mPlayOnce = TRUE;
    FindAnimation(RESOURCE_BTA0, "appear00")->mPlayOnce = TRUE;
    mModel.RestartAnims();
    mModel.mVisible = TRUE;
    mDisappearing = FALSE;
}

void Scene40Cover::PlayWait() {
    if (mKind != KIND_CURTAIN) {
        return;
    }
    PlayAnimation(RESOURCE_BCA0, "wait00");
    AnimModel_SetSpeed(&mModel, FX32_ONE);
    FindAnimation(RESOURCE_BCA0, "wait00")->mPlayOnce = FALSE;
}

// The curtain goes: "appear00" from its end, backwards
void Scene40Cover::Disappear() {
    if (mKind != KIND_CURTAIN) {
        return;
    }
    PlayAnimation(RESOURCE_BCA0, "appear00");
    PlayAnimation(RESOURCE_BTA0, "appear00");
    func_0202a5a4(&mModel, ((NNSG3dResAnmCommon *)FindAnimation(RESOURCE_BTA0, "appear00")->mAnmObj->resAnm)->numFrame * FX32_ONE);
    AnimModel_SetSpeed(&mModel, -FX32_ONE);
    FindAnimation(RESOURCE_BCA0, "appear00")->mPlayOnce = TRUE;
    FindAnimation(RESOURCE_BTA0, "appear00")->mPlayOnce = TRUE;
    mModel.RestartAnims();
    mModel.mVisible = TRUE;
    mDisappearing = TRUE;
}

BOOL Scene40Cover::HasAppeared() {
    if (mKind != KIND_CURTAIN) {
        return FALSE;
    }
    BOOL done = FALSE;
    if (FindAnimation(RESOURCE_BTA0, "appear00")->mFinished && !mDisappearing) {
        done = TRUE;
    }
    return done;
}

BOOL Scene40Cover::HasDisappeared() {
    if (mKind != KIND_CURTAIN) {
        return FALSE;
    }
    BOOL done = FALSE;
    if (FindAnimation(RESOURCE_BTA0, "appear00")->mFinished && mDisappearing) {
        done = TRUE;
    }
    return done;
}

// --- The scene's clock ------------------------------------------------------------------------------------------

Scene40Clock::Scene40Clock() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

void Scene40Clock::Update() {
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

void Scene40Clock::UpdateIfArchiveIdle() {
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

// --- The stylus -------------------------------------------------------------------------------------------------

static inline Scene40TouchPoint ZeroPoint() {
    Scene40TouchPoint point = {0, 0};
    return point;
}

void Scene40Touch::Init() {
    mPressed = FALSE;
    mHeld = FALSE;
    mReleased = FALSE;
    mSmooth = FALSE;
    Reset();
    mUnknownB4 = 0;
    mTaps = 0;
    mTapFrames = 0;
}

void Scene40Touch::Reset() {
    mPressPoint = ZeroPoint();
    mPoint = ZeroPoint();
    mReleasePoint = ZeroPoint();
    mDirection = ZeroPoint();
    mDistance = 0;
    mHeldFrames = 0;
    mVelocity = ZeroPoint();
    mMoveIndex = 0;
    for (int i = 0; i < 5; i++) {
        mMoves[i] = ZeroPoint();
    }
    if (!mSmooth) {
        return;
    }
    mPointCount = 0;
    mPointIndex = 0;
    for (int i = 0; i < 8; i++) {
        mPoints[i].x = 0;
        mPoints[i].y = 0;
    }
}

// The mean of the last five moves
Scene40TouchPoint Scene40Touch::GetVelocity() {
    Scene40TouchPoint sum = ZeroPoint();
    for (int i = 0; i < 5; i++) {
        sum.x += mMoves[i].x;
        sum.y += mMoves[i].y;
    }
    sum.x /= 5;
    sum.y /= 5;
    return sum;
}

// A tap counts when it comes 3 to 30 frames after the one before
void Scene40Touch::CountTap() {
    if (mTaps == 0) {
        mTaps++;
        mTapFrames = 0;
        return;
    }
    if (mTapFrames >= 3 && mTapFrames <= 30) {
        mTaps++;
    }
}

// --- The camera -------------------------------------------------------------------------------------------------

static const VecFx32 sCameraStartPosition = {0, 0x41D0, 0xC000};
static const VecFx32 sCameraStartTarget = {0, 0x1999, 0};
static const VecFx32 sCameraTarget = {0, 0x1999, 0};

static inline void CopyVec(VecFx32 *dst, const VecFx32 *src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

static inline void SetVec(VecFx32 *dst, VecFx32 src) {
    dst->x = src.x;
    dst->y = src.y;
    dst->z = src.z;
}

static inline void SetCameraPosition(Camera *camera, VecFx32 position) {
    camera->mPosition.x = position.x;
    camera->mPosition.y = position.y;
    camera->mPosition.z = position.z;
}

static inline void SetCameraTarget(Camera *camera, VecFx32 target) {
    camera->mTarget.x = target.x;
    camera->mTarget.y = target.y;
    camera->mTarget.z = target.z;
}

// The shift is unsigned, as overlay 96's
static inline VecFx32 ShiftVec(const VecFx32 &v, u32 shift) {
    VecFx32 result;
    result.x = v.x >> shift;
    result.y = v.y >> shift;
    result.z = v.z >> shift;
    return result;
}

// (x, z) made one long, as VEC_Normalize does it (overlay 96's NormalizeXZ)
static inline FloorPos NormalizeXZ(fx32 x, fx32 z) {
    FloorPos result;
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    s32 sqrt = (s32)CP_GetSqrtResult32();
    t = CP_GetDivResult64();
    t = t * sqrt;
    result.x = (fx32)((t * x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    result.z = (fx32)((t * z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    return result;
}

static inline fx32 ClampFx(fx32 value, fx32 min, fx32 max) {
    if (value > max) {
        value = max;
    } else if (value < min) {
        value = min;
    }
    return value;
}

void Scene40Camera::Init(Scene40Character *character) {
    mCamera = &gCamera;
    Camera_Reset(mCamera);
    SetCameraPosition(mCamera, sCameraStartPosition);
    SetCameraTarget(mCamera, sCameraStartTarget);
    mCamera->mAngleX = 0;
    mCamera->mAngleY = 0;
    mCamera->mAngleZ = 0;
    Camera_UpdateEye(mCamera);
    mCharacter = character;
    CopyVec(&mPosition, &mCamera->mPosition);
    CopyVec(&mTarget, &mCamera->mTarget);
    mState = 1;
}

// Coming back from another scene: the camera as that scene left it, put 14 in front of the target
void Scene40Camera::Reset(BOOL restore) {
    if (!restore) {
        return;
    }
    SetVec(&mPosition, gSceneMemory.mCameraPosition);
    SetVec(&mTarget, gSceneMemory.mCameraTarget);
    VEC_Subtract(&mPosition, &mTarget, &mPosition);
    VecFx32 target = sCameraTarget;
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
    PlaceAround(&mPosition, &mTarget, &mPosition);
    CopyVec(&mCamera->mPosition, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    Camera_UpdateEye(mCamera);
}

// Follows the character within the room, keeping the distance and the height
void Scene40Camera::Update() {
    switch (mState) {
    case 0:
        return;
    case 1:
        break;
    default:
        return;
    }
    VecFx32 middle;
    VecFx32 position;
    VecFx32 pos = mCharacter->mPos;
    middle = pos;
    middle.x = ClampFx(middle.x, -0x3000, 0x3000);
    middle.z = ClampFx(middle.z, -0x3000, 0x3000);
    middle.y = mTarget.y;
    mTarget = Approach(&middle, &mTarget, 5);
    PlaceAround(&mPosition, &middle, &position);
    mPosition = Approach(&position, &mPosition, 5);
    CopyVec(&mCamera->mTarget, &mTarget);
    CopyVec(&mCamera->mPosition, &mPosition);
}

void Scene40Camera::SetState(int state) {
    mState = state;
}

// result: 14 from the target, in the direction of position from it, at the height 0x41D0
void Scene40Camera::PlaceAround(VecFx32 *position, VecFx32 *target, VecFx32 *result) {
    FloorPos direction = NormalizeXZ(position->x - target->x, position->z - target->z);
    result->x = target->x + direction.x * 14;
    result->z = target->z + direction.z * 14;
    result->y = 0x41D0;
}

// The camera in front of the character, looking at it
void Scene40Camera::ViewFromFront() {
    VecFx32 pos = mCharacter->mPos;
    mTarget.x = pos.x;
    mTarget.y = pos.y;
    mTarget.z = pos.z;
    mTarget.x = ClampFx(mTarget.x, -0x3000, 0x3000);
    mTarget.z = ClampFx(mTarget.z, -0x3000, 0x3000);
    mTarget.y = 0x1999;
    u16 angle = mCharacter->mAngle;
    mPosition.x = FX_Mul(FX_CosIdx(angle), FX32_CONST(200));
    mPosition.z = FX_Mul(FX_SinIdx(angle), FX32_CONST(200));
    PlaceAround(&mPosition, &mTarget, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    CopyVec(&mCamera->mPosition, &mPosition);
    Camera_UpdateEye(mCamera);
}

void Scene40Camera::SetPosition(VecFx32 position) {
    mPosition.x = position.x;
    mPosition.y = position.y;
    mPosition.z = position.z;
    CopyVec(&mCamera->mPosition, &mPosition);
}

void Scene40Camera::SetTarget(VecFx32 target) {
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
    CopyVec(&mCamera->mTarget, &mTarget);
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 Scene40Camera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = ShiftVec(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

static inline FloorPos ToFloorPos(VecFx32 v) {
    FloorPos pos;
    pos.x = v.x;
    pos.z = v.z;
    return pos;
}

// Where the camera's eye is, on the floor
FloorPos Scene40Camera::GetEyeFloorPos() {
    VecFx32 eye = mCamera->mEye;
    return ToFloorPos(eye);
}

// --- The lights -------------------------------------------------------------------------------------------------

static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};

void Scene40Lights::Init(Camera *camera, Room *room) {
    VecFx32 view;
    mState = 0;
    mCamera = camera;
    mRoom = room;
    VEC_Subtract(&mCamera->mPosition, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        mLights[i] = &gLights[i];
        Light_Reset(&gLights[i]);
        mLights[i]->mColor = gInteriorTable[mRoom->mId].mLightColor;
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void Scene40Lights::Update() {
    VecFx32 view;
    switch (mState) {
    case 0: {
        VEC_Subtract(&mCamera->mEye, &mCamera->mTarget, &view);
        u16 angle = FX_Atan2Idx(view.x, view.z);
        for (int i = 0; i < 2; i++) {
            Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
        }
        break;
    }
    case 1:
        SetColor(mRoom->GetLightColor());
        mState = 0;
        break;
    }
}

void Scene40Lights::SetState(int state) {
    mState = state;
}

void Scene40Lights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// --- Falling and bouncing ---------------------------------------------------------------------------------------

void Scene40Physics::Init() {
    mCharacter = NULL;
    for (int i = 0; i < 10; i++) {
        mItems[i] = NULL;
    }
    mItemCount = 0;
}

void Scene40Physics::AddItem(Scene40ItemModel *item) {
    mItems[mItemCount++] = item;
}

void Scene40Physics::Update() {
    UpdateCharacter();
    UpdateItems();
}

void Scene40Motion::Reset() {
    VecFx32 zero = {0, 0, 0};
    mVelocity = zero;
    mAtRest = FALSE;
    mHeld = FALSE;
    mOffered = FALSE;
}

// At rest: on the floor and not moving
void Scene40Motion::UpdateAtRest(VecFx32 *pos, VecFx32 *velocity) {
    BOOL atRest = TRUE;
    if (pos->y != 0 || velocity->x != 0 || velocity->y != 0 || velocity->z != 0) {
        atRest = FALSE;
    }
    mAtRest = atRest;
}

// The item models that are (or are not) visible
int Scene40Physics::GetItems(Scene40ItemModel **items, int *count, BOOL visible) {
    *count = 0;
    for (int i = 0; i < mItemCount; i++) {
        if (visible == mItems[i]->mVisible) {
            items[(*count)++] = mItems[i];
        }
    }
    return *count;
}

// One item model no one uses yet, marked used: 1 if there was one
int Scene40Physics::TakeFreeItem(Scene40ItemModel **item) {
    int taken = 0;
    for (int i = 0; i < mItemCount; i++) {
        if (!mItems[i]->mInUse) {
            mItems[i]->mInUse = TRUE;
            taken++;
            *item = mItems[i];
            break;
        }
    }
    return taken;
}

// Frames until a thing at a height with an upward velocity reaches the floor, falling by 100 a frame
int Scene40Physics::GetFallFrames(fx32 height, fx32 velocity) {
    return FX_Div(-velocity - FX_Sqrt(FX_Mul(velocity, velocity) - FX_Mul(height * 2, -100)), -100) >> FX32_SHIFT;
}

// --- The item models --------------------------------------------------------------------------------------------

void Scene40ItemModel::Load(int resourceId) {
    LoadModel(resourceId);
    mMotion.Reset();
    mInUse = 0;
}

// Out of sight: transparent, at its normal size, and far away
void Scene40ItemModel::Hide() {
    mVisible = FALSE;
    mMotion.mHeld = FALSE;
    mMotion.mOffered = 0;
    mInUse = 0;
    Model_SetAlpha(this, 31);
    mPosition.x = 0x7FFFFFFF;
    mPosition.y = 0x7FFFFFFF;
    mPosition.z = 0x7FFFFFFF;
    mScale.x = FX32_ONE;
    mScale.y = FX32_ONE;
    mScale.z = FX32_ONE;
}

// Shows another item: lets go of the old one's files, and loads the new one's (as overlay 96's WeddingGiftModel)
void Scene40ItemModel::ChangeItem(int item) {
    ModelResources_ReleaseModel(gModelResources, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(gModelResources, mResourceId, mModelPath);
    LoadAnimation(mResourceId);
    ResetScale();
}

// Where a character looks at it: half a unit above it
VecFx32 Scene40ItemModel::GetLookPoint() {
    VecFx32 point = mPosition;
    point.y += FX32_HALF;
    return point;
}

// --- The character: the item in its hands -----------------------------------------------------------------------

static inline fx32 LengthSq(const VecFx32 *v) {
    return FX_Mul(v->x, v->x) + FX_Mul(v->y, v->y) + FX_Mul(v->z, v->z);
}

// Puts the held item in the right hand: the hand, or a little way from the elbow towards the hand if the hand is
// nearer than that (PlaceHeldItem with the left hand left out)
void Scene40Character::PlaceHeldItemInHand() {
    VecFx32 middle;
    VecFx32 rightHand;
    VecFx32 rightArm;
    VecFx32 rightOffset;
    VecFx32 rightReach;
    VecFx32 rightPoint;

    if (mHeldItem == NULL) {
        return;
    }
    mTrackLimbs = TRUE;
    rightHand = GetJointPosition(JOINT_R_HAND);
    rightArm = GetJointPosition(JOINT_R_ELBOW);

    VEC_Subtract(&rightHand, &rightArm, &rightOffset);
    VEC_Normalize(&rightOffset, &rightReach);
    ScaleVec(0x999, &rightReach, &rightReach);
    if (LengthSq(&rightReach) > LengthSq(&rightOffset)) {
        VEC_Add(&rightArm, &rightReach, &rightPoint);
        rightPoint.y = rightHand.y;
    } else {
        rightPoint = rightHand;
    }
    middle = rightPoint;
    mHeldItem->SetPosition(middle);
    mHeldItem->mMotion.mHeld = TRUE;
}

void Scene40Character::PlaceHeldItem() {
    PlaceHeldItem(FX32_ONE);
}

// Puts the held item between the hands, each hand's point found as above. Goods 247 hang lower, by 0xD99 scaled.
void Scene40Character::PlaceHeldItem(fx32 scale) {
    VecFx32 middle;
    VecFx32 rightHand;
    VecFx32 leftHand;
    VecFx32 rightArm;
    VecFx32 leftArm;
    VecFx32 rightOffset;
    VecFx32 leftOffset;
    VecFx32 rightReach;
    VecFx32 leftReach;
    VecFx32 rightPoint;
    VecFx32 leftPoint;

    if (mHeldItem == NULL) {
        return;
    }
    mTrackLimbs = TRUE;
    rightHand = GetJointPosition(JOINT_R_HAND);
    rightArm = GetJointPosition(JOINT_R_ELBOW);
    leftHand = GetJointPosition(JOINT_L_HAND);
    leftArm = GetJointPosition(JOINT_L_ELBOW);

    VEC_Subtract(&rightHand, &rightArm, &rightOffset);
    VEC_Normalize(&rightOffset, &rightReach);
    ScaleVec(0x999, &rightReach, &rightReach);
    if (LengthSq(&rightReach) > LengthSq(&rightOffset)) {
        VEC_Add(&rightArm, &rightReach, &rightPoint);
        rightPoint.y = rightHand.y;
    } else {
        rightPoint = rightHand;
    }

    VEC_Subtract(&leftHand, &leftArm, &leftOffset);
    VEC_Normalize(&leftOffset, &leftReach);
    ScaleVec(0x999, &leftReach, &leftReach);
    if (LengthSq(&leftReach) > LengthSq(&leftOffset)) {
        VEC_Add(&leftArm, &leftReach, &leftPoint);
        leftPoint.y = leftHand.y;
    } else {
        leftPoint = leftHand;
    }

    VEC_Add(&rightPoint, &leftPoint, &middle);
    middle.x /= 2;
    middle.y /= 2;
    middle.z /= 2;
    if (mHeldItem->mId == 247) {
        if (scale == FX32_ONE) {
            middle.y += -0xD99;
        } else {
            middle.y -= FX_Mul(scale, 0xD99);
        }
        middle.y = middle.y < 0 ? 0 : middle.y;
    }
    mHeldItem->SetPosition(middle);
    mHeldItem->mMotion.mHeld = TRUE;
}

// The cover between the hands, on the floor, turned as the body
void Scene40Character::PlaceCoverAtHands() {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(JOINT_R_HAND, &matrix, &rotation);
    fx32 rightX = matrix._30;
    fx32 rightZ = matrix._32;
    GetJointMatrices(JOINT_L_HAND, &matrix, &rotation);
    VecFx32 pos;
    pos.x = (rightX + matrix._30) / 2;
    pos.y = 0;
    pos.z = (rightZ + matrix._32) / 2;
    mCover.SetPosition(pos, mAnim.mRotY);
}

// The mouth open (face 7) from an eighth to half of the body's animation, closed (face 1) otherwise
void Scene40Character::UpdateMouthByAnimation() {
    ModelAnimation *anim = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId);
    fx32 frames = ((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame * FX32_ONE;
    fx32 frame = anim->mAnmObj->frame;
    if (frame >= frames >> 3 && frame <= frames >> 1) {
        SetFace(7);
    } else {
        SetFace(1);
    }
}

// --- The character: what it was taught, its head and its speech --------------------------------------------------

extern "C" {
wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n);
}

void Scene40Character::SetTaughtWord(const wchar_t *word) {
    MI_CpuClear16(mTaughtWord, sizeof(mTaughtWord));
    wcsncpy(mTaughtWord, word, 16);
}

void Scene40Character::SetTaughtText(const wchar_t *text) {
    MI_CpuClear16(mTaughtText, sizeof(mTaughtText));
    wcsncpy(mTaughtText, text, 64);
}

// Outside the middle of the room (+-1 each way) it walks back to the middle, turned to the camera
BOOL Scene40Character::WalkToMiddleIfAway() {
    BOOL away = FALSE;
    fx32 eyeX = gCamera.mEye.x;
    fx32 eyeZ = gCamera.mEye.z;
    if (mPos.x < -FX32_ONE || mPos.x > FX32_ONE || mPos.z < -FX32_ONE || mPos.z > FX32_ONE) {
        mUnknown2EA0.x = 0;
        mUnknown2EA0.z = 0;
        mUnknown2EA8.x = eyeX;
        mUnknown2EA8.z = eyeZ;
        SetTargetFloorPos(mUnknown2EA0);
        away = TRUE;
    }
    return away;
}

// Not turned to the camera: it walks towards it
BOOL Scene40Character::WalkToCameraIfNotFacing() {
    BOOL turning = FALSE;
    fx32 eyeX = gCamera.mEye.x;
    fx32 eyeZ = gCamera.mEye.z;
    if (!IsFacingCamera()) {
        mUnknown2EA8.x = eyeX;
        mUnknown2EA8.z = eyeZ;
        SetTargetFloorPos(mUnknown2EA8);
        turning = TRUE;
    }
    return turning;
}

// Takes the visible item the player holds out (the last of them), if any
BOOL Scene40Character::TakeOfferedItem() {
    Scene40ItemModel *items[10];
    int count;
    int index;
    BOOL found = FALSE;
    mContext->mPhysics->GetItems(items, &count, TRUE);
    for (int i = 0; i < count; i++) {
        if (items[i]->mMotion.mOffered) {
            index = i;
            found = TRUE;
        }
    }
    if (found) {
        mHeldItem = items[index];
    }
    return found;
}

// The head stops turning and looks ahead
void Scene40Character::StopHeadTurn() {
    mHeadTurn = 0;
    mHeadPitchNow = 0;
    mHeadYawNow = 0;
    mHeadRollNow = 0;
}

void Scene40Character::ResetHeadTurn() {
    mHeadTurn = 1;
    mHeadPitch = 0;
    mHeadYaw = 0;
    mHeadRoll = 0;
}

static inline u16 AngleTo(Character *character, FloorPos target) {
    FloorPos delta = target - character->GetFloorPos();
    return FX_Atan2Idx(delta.z, delta.x);
}

void Scene40Character::FaceCamera() {
    FloorPos camera;
    camera.x = gCamera.mEye.x;
    camera.z = gCamera.mEye.z;
    Character::SetAngle(AngleTo(this, camera));
}

// Turns the head towards a point, as far as the limits allow
void Scene40Character::TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = 0;
}

void Scene40Character::TurnHeadToCamera() {
    TurnHeadTo(&gCamera.mEye, 0x2000, 0x3F00);
}

void Scene40Character::TurnHeadToHeldItem(int pitchLimit, int yawLimit) {
    if (mHeldItem == NULL) {
        return;
    }
    VecFx32 point = mHeldItem->GetLookPoint();
    TurnHeadTo(&point, pitchLimit, yawLimit);
}

// The same, at once
void Scene40Character::SnapHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = 0;
    mHeadPitchNow = pitch;
    mHeadYawNow = yaw;
    mHeadRollNow = 0;
}

void Scene40Character::SnapHeadToCamera() {
    SnapHeadTo(&gCamera.mEye, 0x2000, 0x3F00);
}

u16 Scene40Character::GetHeadPitchTo(VecFx32 *target) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(0, &matrix, &rotation);
    fx32 dz = target->z - mPos.z;
    fx32 dx = target->x - mPos.x;
    fx32 eyes = matrix._31 + 0xCCC;
    return -FX_Atan2Idx(target->y - eyes, SqrtXZ(dx, dz));
}

void Scene40Character::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void Scene40Character::Speak(int file, u32 message, int mode) {
    Speak(file, message, 0, 0, mode);
}

// How old a character is on a date: -1 without a birth year
int Scene40Character::GetAge(SavedCharacter *character, RTCDate *date) {
    int year = date->year + 2000;
    int month = date->month;
    int day = date->day;
    int birthYear = character->mBirthYear;
    int birthMonth = character->GetParameter(PARAM_BIRTH_MONTH);
    int birthDay = character->GetParameter(PARAM_BIRTH_DAY);
    if (birthYear == 0xFFF) {
        return -1;
    }
    if (day < birthDay) {
        month--;
    }
    if (month < birthMonth) {
        year--;
    }
    int age = year - birthYear;
    if (age < 0) {
        age = -1;
    }
    return age;
}

// --- The file's static initialiser ------------------------------------------------------------------------------

// A text position of the profile page (read by 0x0215f180), a copy of a pool point: initialised at run time by the
// file's static initialiser, so it is in .data, zero in the ROM
// (not written yet)
