#include "Scene40.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneMemory.h"

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

// Not decompiled yet
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
void func_ov075_02161988(Scene40Character *character);
void func_ov075_02161d94(Scene40Character *character);
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
    func_ov075_02161988(this);
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    mBalloon.Update();
    if (!speaking) {
        return;
    }
    func_ov075_02161d94(this);
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
    for (int i = 0; i < 20; i++) {
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
