#include <wchar.h>

#include "Scene40.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneMemory.h"
#include "sound/SoundManager.h"
#include "text/TextUtil.h"

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

BOOL func_02031ccc(SpriteButton *button); // Whether the button's press has played out
void TextButton_SetOverwritePalette(TextButton *button, int palette);

// Not decompiled yet
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
void func_ov075_0217aea8(void *object);
void func_ov075_021782cc(Scene40CharacterPartA *balloon, int resourceId, int unk, CharacterBase *face,
                         Scene40Character *character);
void func_ov075_02178c34(Scene40CharacterPartB *cover, int kind);
void func_ov075_02178e94(Scene40CharacterPartB *cover, VecFx32 position, u16 angle);
void func_ov075_02178f38(Scene40CharacterPartB *cover, GXRgb color);
void func_ov075_02178364(Scene40CharacterPartA *part);
}

// The head icon of each mood
extern const s16 sMoodHeadIcons[5];

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78), as src/character/SavedCharacter.cpp
// has it
struct TroubleInfo {
    u32 kind : 8;
    u32 minutes : 16;
    u32 weight : 8;
    u32 unk4Bits0 : 8;
    u32 mood : 8;
    u32 cooldownMinutes : 12;
    u32 hasTarget : 4;
    u32 flags;
}; // 0xC
extern "C" const TroubleInfo *const data_02114d78[];

// The answers of the talk page
extern const Scene40ChoiceSet sChoiceSets[];        // By set
extern const int sTroubleChoiceSets[];              // The set of each kind of trouble
extern const Scene40ChoiceSet sTroubleChoices[];    // Set 10's answers, by trouble id
extern const Scene40ChoiceSet sSpecialChoiceSets[];

// A text button's text: cleared, and drawn again
static inline void ClearButtonText(TextButton *button) { NNS_G2dCharCanvasClear(&button->mText.mCanvas, 0); }
static inline void DrawButtonText(TextButton *button) { button->mText.DrawAll(); }

// A model's scale, given by value. The copy of the parameter is the original's: ShowBed has the call's result,
// the parameter and this copy on its stack.
static inline void SetModelScale(Model *model, VecFx32 scale) {
    VecFx32 s = scale;
    model->mScale.x = s.x;
    model->mScale.y = s.y;
    model->mScale.z = s.z;
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
    date.year = mUnknown30A8C.mDate.year;
    date.month = mUnknown30A8C.mDate.month;
    date.day = mUnknown30A8C.mDate.day;
    date.week = mUnknown30A8C.mDate.week;
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

// --- The talk page's answers --------------------------------------------------------------------------------------

void Scene40ChoiceList::Init(int *charOffset, MessageFile *file) {
    mState = STATE_HIDDEN;
    mCount = 0;
    mSet = 0;
    mPressed = -1;
    mFinished = -1;
    mOpened = FALSE;
    mClosed = FALSE;
    mTimer = 0;
    for (int i = 0; i < 4; i++) {
        Sprite_Init(&mButtons[i], 0, 15, 0x80, 0x60, TRUE);
        SpriteButton_SetSequences(&mButtons[i], 15, 16);
        mButtons[i].SetPosition(FxVec2(208 << FX32_SHIFT, (i * 24 + 64) << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], 96, 20);
        *charOffset += TextButton_SetMessage(&mButtons[i], file, 480, &gFonts[0].mFont, 16, *charOffset);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 4);
        TextButton_SetOverwritePalette(&mButtons[i], 1);
        mButtons[i].mVisible = FALSE;
        mButtons[i].mUnknownDC = TRUE;
        mButtons[i].mSounds.mSounds[0] = 10;
        mButtons[i].mSounds.mSounds[2] = 0;
    }
}

void Scene40ChoiceList::Update() {
    mPressed = -1;
    mFinished = -1;
    mOpened = FALSE;
    mClosed = FALSE;
    switch (mState) {
    case STATE_HIDDEN:
        break;
    case STATE_CHOOSING:
        for (int i = 0; i < mCount; i++) {
            if (SpriteButton_Update(&mButtons[i])) {
                mPressed = i;
            }
            if (func_02031ccc(&mButtons[i])) {
                mFinished = i;
            }
        }
        break;
    case STATE_OPENING:
        if (mTimer >= 100) {
            mState = STATE_CHOOSING;
            mOpened = TRUE;
            for (int i = 0; i < mCount; i++) {
                SpriteButton_Reset(&mButtons[i]);
                mButtons[i].mVisible = TRUE;
            }
        }
        mTimer++;
        break;
    case STATE_CLOSING:
        if (mTimer == 15) {
            for (int i = 0; i < mCount; i++) {
                mButtons[i].mVisible = FALSE;
            }
        } else if (mTimer > 15) {
            mState = STATE_HIDDEN;
            mClosed = TRUE;
        }
        mTimer++;
        break;
    }
}

void Scene40ChoiceList::Draw() {
    for (int i = 0; i < mCount; i++) {
        TextButton_Draw(&mButtons[i]);
    }
}

// The buttons are 24 pixels apart, the last at y 136 however many there are. The three functions that place them
// write the position each in its own way (SetPosition of an FxVec2 or SetPositionFromVec): each form leaves the
// position's dead copies in other stack slots, and these are the forms that give the original's.
void Scene40ChoiceList::SetChoices(int set) {
    mSet = set;
    mCount = sChoiceSets[set].mCount;
    for (int i = 0; i < mCount; i++) {
        ClearButtonText(&mButtons[i]);
        mButtons[i].mText.mMessage = sChoiceSets[mSet].mMessages[i];
        DrawButtonText(&mButtons[i]);
        mButtons[i].SetPosition(FxVec2(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT));
    }
}

// The answers to a trouble: the set of its kind; set 10 has answers of the trouble's own
void Scene40ChoiceList::SetChoicesForTrouble(int category, int id) {
    mTroubleKind = data_02114d78[category][id].kind;
    mSet = sTroubleChoiceSets[mTroubleKind];
    mCount = sChoiceSets[mSet].mCount;
    if (mSet == 10) {
        for (int i = 0; i < mCount; i++) {
            ClearButtonText(&mButtons[i]);
            mButtons[i].mText.mMessage = sTroubleChoices[id].mMessages[i];
            DrawButtonText(&mButtons[i]);
            mButtons[i].SetPositionFromVec(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT);
        }
    } else {
        for (int i = 0; i < mCount; i++) {
            ClearButtonText(&mButtons[i]);
            mButtons[i].mText.mMessage = sChoiceSets[mSet].mMessages[i];
            DrawButtonText(&mButtons[i]);
            mButtons[i].SetPositionFromVec(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT);
        }
    }
}

void Scene40ChoiceList::SetSpecialChoices(int index) {
    mSet = 23;
    mCount = sSpecialChoiceSets[index].mCount;
    for (int i = 0; i < mCount; i++) {
        ClearButtonText(&mButtons[i]);
        mButtons[i].mText.mMessage = sSpecialChoiceSets[index].mMessages[i];
        DrawButtonText(&mButtons[i]);
        mButtons[i].SetPosition(FxVec2(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT));
    }
}

void Scene40ChoiceList::Open() {
    mTimer = 0;
    mState = STATE_OPENING;
}

void Scene40ChoiceList::Close() {
    mTimer = 0;
    mState = STATE_CLOSING;
}

// --- The scene's list: the shops' item list with a mark beside each row ------------------------------------------

void Scene40ListView::Init() {
    ItemListView::Init();
    mMarkKind = MARK_NONE;
}

void Scene40ListView::InitMarks(int resource, int sequence) {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Init(&mMarks[i], resource, sequence, 0x80, 0x60, TRUE);
        mMarks[i].SetPosition(FxVec2(0, 0));
        mMarks[i].mOverwritePalette = 3;
    }
}

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

// --- The clothes the character owns, for the list page -------------------------------------------------------------

void Scene40ClothesList::LoadOwnedBy(SavedCharacter *character) {
    int i;
    mCount = 0;
    for (i = 0; i < CLOTHES_SIZE; i++) {
        mItems[i].Clear();
    }
    mCount = CLOTHES_SIZE;
    for (i = 0; i < mCount; i++) {
        mItems[i].mId = i;
        if ((character->mOwnedClothes.m[i >> 5] >> (i & 31)) & 1) {
            mItems[i].mOwned = TRUE;
            mItems[i].mUnknown0C = 1;
            mItems[i].mCount = 1;
        }
        mList[i] = &mItems[i];
    }
}

void Scene40ClothesList::GetOwned(Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        ClothesItem *item = mList[i];
        if (item->mOwned && kind == gClothesTable[item->mId].mKind) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

static s32 CompareClothesSortKeys(void *a, void *b);

void Scene40ClothesList::Sort() {
    u32 buffer[200];
    MATH_QSort(mList, mCount, sizeof(ClothesItem *), CompareClothesSortKeys, buffer);
}

static s32 CompareClothesSortKeys(void *a, void *b) {
    return gClothesTable[(*(ClothesItem **)a)->mId].mSortKey - gClothesTable[(*(ClothesItem **)b)->mId].mSortKey;
}

// --- The meters of the character's status page -------------------------------------------------------------------

void Scene40Meter::Init() {
    mTarget = 0;
    mValue = 0;
    mFull = FALSE;
    mState = STATE_IDLE;
    mRiseStep = 1;
    mFallStep = 1;
    mSeqArc = 27;
    mRiseSound = 0;
    mFallSound = 1;
    mRiseEndSound = 2;
    mFallEndSound = 3;
}

// One frame of moving the value to the target
void Scene40Meter::Update() {
    mFull = FALSE;
    mEmpty = FALSE;
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_START:
        if (mValue < mTarget) {
            gSoundManager.StartSeqArc(1, mSeqArc, mRiseSound);
            mState = STATE_MOVING;
            mFalling = FALSE;
        } else if (mValue <= mTarget) {
            mState = STATE_ARRIVED;
        } else {
            gSoundManager.StartSeqArc(1, mSeqArc, mFallSound);
            mState = STATE_MOVING;
            mFalling = TRUE;
        }
        break;
    case STATE_MOVING:
        if (mValue == mTarget) {
            gSoundManager.StopSeqArc(1, 0);
            if (!mFalling) {
                gSoundManager.StartSeqArc(1, mSeqArc, mRiseEndSound);
            } else {
                gSoundManager.StartSeqArc(1, mSeqArc, mFallEndSound);
            }
            mState = STATE_ARRIVED;
            if (mValue == 100) {
                mFull = TRUE;
            }
            if (mValue == 0) {
                mEmpty = TRUE;
            }
        } else if (mValue < mTarget) {
            mValue += mRiseStep;
            if (mValue > mTarget) {
                mValue = mTarget;
            }
            Show(mValue);
            mFalling = FALSE;
        } else if (mTarget < mValue) {
            mValue -= mFallStep;
            if (mValue < mTarget) {
                mValue = mTarget;
            }
            Show(mValue);
            mFalling = TRUE;
        }
        break;
    case STATE_ARRIVED:
        mState = STATE_IDLE;
        break;
    }
}

void Scene40Meter::SetMoveSounds(int seqArc, int riseSound, int fallSound) {
    mSeqArc = seqArc;
    mRiseSound = riseSound;
    mFallSound = fallSound;
}

void Scene40Meter::SetArriveSounds(int seqArc, int riseSound, int fallSound) {
    mSeqArc = seqArc;
    mRiseEndSound = riseSound;
    mFallEndSound = fallSound;
}

void Scene40Meter::SetSteps(int riseStep, int fallStep) {
    mRiseStep = riseStep;
    mFallStep = fallStep;
}

// Shows a value at once
void Scene40Meter::SetValue(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    mTarget = mValue = value;
    Show(value);
}

// Where Update moves the value to
void Scene40Meter::SetTarget(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    mTarget = value;
}

void Scene40SegmentMeter::Draw() {
    for (int i = 0; i < 10; i++) {
        Sprite_Draw(&mSegments[i]);
    }
}

void Scene40SegmentMeter::SetPosition(ScreenPoint position) {
    for (int i = 0; i < 10; i++) {
        mSegments[i].SetPosition(FxVec2((position.x + mSpacing * (i * 10 - 50)) << FX32_SHIFT, position.y << FX32_SHIFT));
    }
}

// Each segment shows up to 10 of the value, as its frame
void Scene40SegmentMeter::Show(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    for (int i = 0; i < 10; i++) {
        int frame = value - i * 10;
        if (frame > 10) {
            frame = 10;
        } else if (frame < 0) {
            frame = 0;
        }
        Sprite_SetFrame(&mSegments[i], (u16)frame);
    }
}

void Scene40SegmentMeter::SetVisible(BOOL visible) {
    for (int i = 0; i < 10; i++) {
        mSegments[i].mVisible = visible;
    }
}

// When the points arrive at the target, a full meter goes on to the next level
void Scene40LevelMeter::Update() {
    mLevelChanged = FALSE;
    Scene40Meter::Update();
    if (mState != STATE_ARRIVED) {
        return;
    }
    mPoints = mTarget;
    mTotal = mLevel * 100 + mPoints;
    if (mFull) {
        mLevelChanged = TRUE;
        mLevel++;
        ShowLevel(mLevel);
    }
}

void Scene40LevelMeter::Draw() {
    Scene40SegmentMeter::Draw();
    mLevelText.Draw();
}

// Shows a level and its points at once
void Scene40LevelMeter::SetLevel(int level, int points) {
    mLevel = mTargetLevel = level;
    mPoints = mTargetPoints = points;
    mTotal = mTargetTotal = mLevel * 100 + points;
    mDistance = 0;
    ShowLevel(mLevel);
    SetValue(points);
}

// Sets where the meter goes: to the end of the meter if the target is in another level, else to its points. With
// beyondTop, a character at level 99 may go on to 100.
void Scene40LevelMeter::SetTargetLevel(int level, int points, BOOL beyondTop) {
    mTargetLevel = level;
    mTargetPoints = points;
    mTargetTotal = mTargetLevel * 100 + points;
    if (beyondTop && mTargetLevel >= 99 && mLevel == 99) {
        mTargetLevel++;
        mTargetTotal += 100;
    }
    mDistance = mTargetTotal - mTotal;
    if (mDistance > 0) {
        if (mTargetPoints < mPoints || mTargetTotal - mTotal >= 100) {
            SetTarget(100);
        } else {
            SetTarget(mTargetPoints);
        }
    } else if (mDistance < 0) {
        if (mPoints < mTargetPoints || mTotal - mTargetTotal >= 100) {
            SetTarget(0);
        } else {
            SetTarget(mTargetPoints);
        }
    }
}

// Starts the meter moving; after a level went up it starts from empty
void Scene40LevelMeter::Start() {
    if (mTotal <= mTargetTotal) {
        SetValue(mPoints = 0);
        if (mTargetTotal - mTotal >= 100) {
            SetTarget(100);
        } else {
            SetTarget(mTargetPoints);
        }
    }
    mState = STATE_START;
}

// "Ｌｖ" and the level, at most 99
void Scene40LevelMeter::ShowLevel(int level) {
    wchar_t text[10] = L"Ｌｖ";
    wchar_t number[10];
    if (level > 99) {
        level = 99;
    } else if (level < 0) {
        level = 0;
    }
    FormatNumber(level, number, sizeof(number), 2, TRUE);
    wcscat(text, number);
    mLevelText.SetText((const NNSG2dChar *)text);
    mLevelText.Redraw();
}

void Scene40LevelMeter::SetVisible(BOOL visible) {
    Scene40SegmentMeter::SetVisible(visible);
    mLevelText.mSprite.mVisible = visible;
}

// Shows the frame of the value: the sprite has 33 of them
void Scene40SpriteMeter::Init(int resource, int sequence) {
    Sprite_Init(&mSprite, resource, sequence, 0x80, 0x60, TRUE);
    mSprite.mOverwritePalette = 1;
    Sprite_SetFrame(&mSprite, 0);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mSprite.mAnim));
}

void Scene40SpriteMeter::Draw() {
    Sprite_Draw(&mSprite);
}

void Scene40SpriteMeter::SetPosition(ScreenPoint position) {
    mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

void Scene40SpriteMeter::Show(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    int frame = value / 3;
    if (frame > 32) {
        frame = 32;
    } else if (frame < 0) {
        frame = 0;
    }
    Sprite_SetFrame(&mSprite, (u16)frame);
}

void Scene40SpriteMeter::SetVisible(BOOL visible) {
    mSprite.mVisible = visible;
}

// --- The present the character receives -------------------------------------------------------------------------

void Scene40Gift::Set(int kind, int id) {
    mKind = kind;
    switch (kind) {
    case 0:
        mItem.InventoryItem::mId = id;
        mMessage = id;
        switch (gGoodsTable[mItem.InventoryItem::mId].mKind) {
        case 1:
            mGiftKind = 0;
            return;
        case 2:
            mGiftKind = 4;
            return;
        case 3:
            switch (mItem.InventoryItem::mId) {
            case 206:
                mGiftKind = 6;
                return;
            case 212:
                mGiftKind = 7;
                return;
            case 205:
                mGiftKind = 8;
                return;
            case 204:
                mGiftKind = 9;
                return;
            case 214:
                mGiftKind = 10;
                return;
            case 213:
                mGiftKind = 11;
                return;
            case 207:
                mGiftKind = 12;
                return;
            case 137:
                mGiftKind = 13;
                return;
            case 215:
                mGiftKind = 14;
                return;
            case 208:
                mGiftKind = 15;
                return;
            default:
                mGiftKind = 16;
                return;
            }
        default:
            mGiftKind = 16;
            return;
        }
    case 1:
        mItem.InventoryItem::mId = id;
        mGiftKind = 1;
        mMessage = 0xF7;
        return;
    case 2:
        mItem.InventoryItem::mId = id;
        mGiftKind = 2;
        mMessage = 0xF9;
        return;
    case 3:
        mItem.InventoryItem::mId = id;
        mGiftKind = 3;
        mMessage = 0xF4;
        return;
    case 4:
        mItem.InventoryItem::mId = id;
        mGiftKind = 5;
        mMessage = 0xF5;
        return;
    }
}

// --- The scene's character ------------------------------------------------------------------------------------------

void Scene40Character::Init(int resourceId, int unk, u32 defaultMii) {
    Character::Init(resourceId, unk, defaultMii);
    func_ov075_0217aea8(mUnknown2B18);
    mState = mNextState = 0;
    mEntered = TRUE;
    mHeldItem = NULL;
    mLipSync = TRUE;
    mUnknown30A0 = 0;
    mUnknown2ED0[0] = 0;
    mUnknown2ED0[1] = 0;
    mUnknown2ED0[2] = 0;
    mUnknown2ED0[3] = 0;
    mUnknown1C4Added[0] = FALSE;
    mUnknown1C4Added[1] = FALSE;
    mUnknown1C4Added[2] = FALSE;
    mUnknown2EE0[0] = 0;
    mUnknown2EE0[1] = 0;
    mUnknown2EE0[2] = 0;
    mUnknown3094 = TRUE;
}

// Both head icons show the icon of the mood, over the head
void Scene40Character::LoadHeadIcons(ParticleSystem *particles) {
    for (int i = 0; i < 2; i++) {
        mHeadIcons[i].Load(particles, sMoodHeadIcons[mMood]);
        mHeadIconPlaces[i] = ICON_AT_HEAD;
    }
}

void Scene40Character::PlaceHeadIcons() {
    for (int i = 0; i < 2; i++) {
        switch (mHeadIconPlaces[i]) {
        case ICON_AT_HEAD:
            PlaceHeadIconAtHead(i);
            break;
        case ICON_AT_BODY:
            PlaceHeadIconAtBody(i);
            break;
        case ICON_AT_FEET:
            PlaceHeadIconAtFeet(i);
            break;
        case ICON_AT_HAND:
            PlaceHeadIconAtHand(i);
            break;
        }
    }
}

void Scene40Character::PlaceHeadIconAtHead(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle + mHeadYawNow;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcons[icon].SetPos(matrices[JOINT_NECK]._30, matrices[JOINT_NECK]._31, matrices[JOINT_NECK]._32);
}

void Scene40Character::PlaceHeadIconAtBody(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcons[icon].SetPos(matrices[JOINT_BODY]._30, matrices[JOINT_BODY]._31, matrices[JOINT_BODY]._32);
}

void Scene40Character::PlaceHeadIconAtFeet(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle;
    VecFx32 *pos = &mPos;
    mHeadIcons[icon].SetPos(pos->x, pos->y, pos->z);
}

void Scene40Character::PlaceHeadIconAtHand(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcons[icon].SetPos(matrices[JOINT_R_HAND]._30, matrices[JOINT_R_HAND]._31, matrices[JOINT_R_HAND]._32);
}

// The first head icon shows the mood's icon over the head, from its start; none in the first mood
void Scene40Character::ShowMoodHeadIcon() {
    mHeadIconPlaces[0] = ICON_AT_HEAD;
    if (mMood == 0) {
        mHeadIcons[0].SetMode(1);
        return;
    }
    mHeadIcons[0].mAngle = 0x4000 - mAngle + mHeadYawNow;
    s16 id = sMoodHeadIcons[mMood];
    if (mHeadIcons[0].mIconId != id) {
        mHeadIcons[0].Show();
        mHeadIcons[0].mIconId = id;
        mHeadIcons[0].Reload();
        mHeadIcons[0].SetMode(3);
    } else {
        mHeadIcons[0].Restart();
        mHeadIcons[0].SetMode(3);
    }
}

void Scene40Character::InitBalloon(int resourceId, int unk, CharacterBase *face) {
    func_ov075_021782cc(&mPartA, resourceId, unk, face, this);
}

// 0: the bed, 1: the dressing curtain
void Scene40Character::InitCover(int kind) {
    func_ov075_02178c34(&mPartB, kind);
}

// Puts the bed where the character lies down, at the character's scale
void Scene40Character::ShowBed(VecFx32 position, u16 angle, GXRgb color) {
    if (*(int *)mPartB.mUnknown00 != 0) {
        return;
    }
    func_ov075_02178e94(&mPartB, position, angle);
    func_ov075_02178f38(&mPartB, color);
    mPartB.mModel.mVisible = TRUE;
    SetModelScale(&mPartB.mModel, GetHeldItemScale());
}

void Scene40Character::SetContext(Scene40Context *context) {
    mContext = context;
    mTtsPlayer = mContext->mTtsPlayer;
    mSpeechWindow = (Scene40SpeechWindow *)mContext->mSpeechWindow;
}

// With speaking set, the lips follow the scene's speech
void Scene40Character::Update(BOOL speaking) {
    Character::Update();
    PlaceHeadIcons();
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    func_ov075_02178364(&mPartA);
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
    mPartA.Draw();
    mPartB.Draw();
}

// Runs the group function of the state's range
void Scene40Character::UpdateState() {
    mState = mNextState;
    if (mState <= 0x19) {
        UpdateStates00To19();
    } else if (mState <= 0x1C) {
        UpdateStates1ATo1C();
    } else if (mState <= 0x27) {
        UpdateStates1DTo27();
    } else if (mState <= 0x2D) {
        UpdateStates28To2D();
    } else if (mState <= 0x43) {
        UpdateStates2ETo43();
    } else if (mState <= 0x47) {
        UpdateStates44To47();
    } else if (mState <= 0x4E) {
        UpdateStates48To4E();
    } else if (mState <= 0x66) {
        UpdateStates4FTo66();
    } else if (mState <= 0x85) {
        UpdateStates67To85();
    } else if (mState <= 0xB2) {
        UpdateStates86ToB2();
    } else if (mState <= 0xBC) {
        UpdateStatesB3ToBC();
    } else if (mState <= 0xC5) {
        UpdateStatesBDToC5();
    } else if (mState <= 0xD1) {
        UpdateStatesC6ToD1();
    }
    mFrame++;
}

void Scene40CharacterPartA::Draw() {
    mModel.Draw();
}

void Scene40CharacterPartB::Draw() {
    mModel.Draw();
}

// --- The scene's clock, at 0x30A8C --------------------------------------------------------------------------------

Scene40Unknown30A8C::Scene40Unknown30A8C() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

void Scene40Unknown30A8C::Read() {
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

void Scene40Unknown30A8C::ReadIfIdle() {
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}
