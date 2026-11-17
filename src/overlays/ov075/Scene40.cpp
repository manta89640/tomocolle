#include "Scene40.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "save/SaveBlocks.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "sound/SoundManager.h"

#include <wchar.h>

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

BOOL SpriteButton_IsTouched(SpriteButton *button);                // Whether the stylus came down on it
void SpriteButton_SetDisabled(SpriteButton *button, BOOL disabled);
BOOL func_02031d00(SpriteButton *button);
void TextButton_SetOverwritePalette(TextButton *button, int palette); // Of the button and its text
int MiiData_GetSex(const CharacterRecord *mii);
void MiiData_GetName(const CharacterRecord *mii, wchar_t *name);
int MiiData_GetFavoriteColor(const CharacterRecord *mii);
GXRgb NFL_GetFavoriteColor(int index);
void Camera_UpdateEye(Camera *camera);                         // Whether it is selected (mUnknownB0)

// Not decompiled yet
// Of ov075-c
// Of ov075-e
int func_ov075_02176454(Scene40Character *character); // Chooses one of gStr_S_ov075
void func_ov075_021762ac(Scene40Character *character);
BOOL func_ov075_0217ade4(void *models, Scene40ItemModel **model);
void func_ov075_0217af40(Scene40ItemModel *model);
void func_ov075_0217af88(Scene40ItemModel *model, int item);
void func_ov075_021750dc(Scene40Character *character);
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
void func_ov075_02161988(Scene40Character *character);
void func_ov075_02161d94(Scene40Character *character);
void func_ov075_02178364(Scene40CharacterPartA *part);
}

// What this scene keeps in the main program's gSceneScratch (an array of bytes there) for the scenes it goes to
struct Scene40Scratch {
    /* 0x00 */ u16 mShopItem; // The item bought in the shop the list page's button went to, 0xFFFF for none
    /* 0x02 */ u8 mUnknown02[2];
    /* 0x04 */ BOOL mMenuAvailable[5]; // The menu's first five buttons, as Scene40MenuBar::Refresh takes them back
    /* 0x18 */ u8 mUnknown18[0x58];
}; // 0x70

extern Scene40Scratch gSceneScratch;

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78), as src/character/SavedCharacter.cpp
// has it
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

// The bytes after gSceneMemory's 0x3C: the first of gSceneScratch, which this file reaches through gSceneMemory as
// well (the literal is gSceneMemory's address and the offset is in the instruction)
struct Scene40SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacters[12];
    /* 0x2C */ u16 mUnknown2C[8];
    /* 0x3C */ u8 mTalkAnswers[2]; // The talk page's answers, for the scene it goes to
}; // 0x3E

static inline Scene40SceneMemory &GetSceneMemory() {
    return (Scene40SceneMemory &)gSceneMemory;
}

// A game the talk page can go to (gStr_S_ov075, 0x0217BBC0; 0x02176454 chooses one)
struct Scene40Game {
    /* 0x0 */ int mSceneId;
    /* 0x4 */ u8 mUnknown4[4];
}; // 0x8
extern "C" const Scene40Game gStr_S_ov075[];

// Of ov075-a: static functions of this file (remove these declarations at the merge)
void ProjectTouchBetween(const ScreenPoint *point, VecFx32 *out, fx32 targetWeight, fx32 eyeWeight);
fx32 GetCameraDistanceScale(const VecFx32 *pos);
void MakeFacingMatrixX(MtxFx33 *mtx, fx32 sin, fx32 cos);
void MakeFacingMatrixY(MtxFx33 *mtx, fx32 sin, fx32 cos);
void SetGameFlags(u32 flags, BOOL set);
u32 GetGameFlags(u32 flags);
void AddTroublesSolved(int count);
extern SaveHeader gIslandState; // The header of the save data

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

static inline const ItemInfo *GetGoodsInfo(u16 id) {
    return &gGoodsTable[id];
}

static inline const ClothesInfo *GetClothesInfo(u16 id) {
    return &gClothesTable[id];
}

// The body's place and size. The mannequin comes last: an inlined call binds its arguments last to first, and the
// original reads the pointer before it computes the size (as a member function, `this` binds last and does not match)
static inline void SetBodyPosition(Mannequin *body, VecFx32 position) {
    body->mPos.x = position.x;
    body->mPos.y = position.y;
    body->mPos.z = position.z;
}

static inline void SetBodyScaleX(fx32 x, Mannequin *body) {
    body->mScale.x = x;
    body->mInvScale.x = FX_Inv(x);
}

static inline void SetBodyScaleY(fx32 y, Mannequin *body) {
    body->mScale.y = y;
    body->mInvScale.y = FX_Inv(y);
}

static inline void SetBodyScaleZ(fx32 z, Mannequin *body) {
    body->mScale.z = z;
    body->mInvScale.z = FX_Inv(z);
}

// --- The list page (pages+0x1F44) ----------------------------------------------------------------------------------

// Where on the screen the mannequin and an item model are shown
static const ScreenPoint sMannequinScreenPoint = {190, 172};
static const ScreenPoint sItemScreenPoint = {190, 134};

void Scene40ListPage::Init(Scene40Context *context, int *charOffset) {
    mContext = context;
    mNextState = STATE_NONE;
    mState = STATE_NONE;
    mMode = MODE_GIVE;
    mTab = 0;
    mActive = TRUE;
    mChosenId = 0;
    mChosen = FALSE;
    mItemCount = 0;
    mSortGoods = FALSE;
    mSortClothes = FALSE;
    mSortInteriors = FALSE;
    mClothesList = context->mClothesList;
    mInventory = context->mInventory;
    mInteriors = context->mInteriors;
    mMannequin = context->mMannequin;
    mGift.Set(0, 0);

    mTitle.InitObjForMessage(mContext->mMiiTouchMessages, 188, &gFonts[0].mFont, 0x1000, *charOffset, 0, 1, 0, 0, 15,
                             NULL);
    mTitle.mMessageFile = mContext->mMiiTouchMessages;
    mTitle.SetPositionFromVec((128 - mTitle.mCanvas.areaWidth * 8 / 2) << FX32_SHIFT,
                              (15 - mTitle.mCanvas.areaHeight * 8 / 2) << FX32_SHIFT);
    mTitle.mSprite.mOverwritePalette = 1;
    mTitle.mHAlign = mTitle.mMessageFile->GetAlignment(mTitle.mMessage);
    *charOffset += mTitle.GetCharCount();
    mTitle.mSprite.mVisible = FALSE;

    mList.Init();
    mList.SetBackground(0x100, 56);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x77DC, 0x100);
    mList.InitRows(2, 8, 9, charOffset);
    mList.InitCounts(charOffset);
    mList.InitMarks(2, 36);
    mList.InitUpButton(2, 2, 3);
    mList.InitDownButton(2, 4, 5);
    mList.InitScrollBar(2, 6, 7);
    mList.SetUpScrollBar(2, 6, 7);
    mList.mCharacter = context->mCharacter;
    mList.SetMessageFile(context->mItemMessages);
    mList.SetMode(ItemListView::MODE_COUNT);
    mList.mMarkKind = Scene40ListView::MARK_NONE;
    mList.SetVisible(FALSE);

    mListTabs.Init(2);
    mGoodsTabs.Init(2);
    mClothesTabs.Init(2);

    Sprite_Init(&mOkButton, 2, 10, 128, 96, TRUE);
    SpriteButton_SetSequences(&mOkButton, 10, 11);
    mOkButton.SetPositionFromVec(192 << FX32_SHIFT, 173 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mOkButton, 72, 30);
    *charOffset += TextButton_SetMessage(&mOkButton, context->mTroubleMessages, 476, &gFonts[2].mFont, 0x1000, *charOffset);
    SpriteButton_Reset(&mOkButton);
    SpriteButton_SetKind(&mOkButton, 4);
    mOkButton.mUnknownDC = TRUE;
    mOkButton.mSounds.mSounds[0] = 10;
    mOkButton.mSounds.mSounds[2] = 0;
    TextButton_SetOverwritePalette(&mOkButton, 1);
    mOkButton.mVisible = FALSE;

    Sprite_Init(&mShopButton, 2, 10, 128, 96, TRUE);
    SpriteButton_SetSequences(&mShopButton, 10, 11);
    mShopButton.SetPositionFromVec(64 << FX32_SHIFT, 173 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mShopButton, 72, 30);
    *charOffset += TextButton_SetMessage(&mShopButton, context->mTroubleMessages, 478, &gFonts[2].mFont, 0x1000, *charOffset);
    SpriteButton_Reset(&mShopButton);
    SpriteButton_SetKind(&mShopButton, 4);
    mShopButton.mUnknownDC = TRUE;
    mShopButton.mSounds.mSounds[0] = 10;
    mShopButton.mSounds.mSounds[2] = 0;
    TextButton_SetOverwritePalette(&mShopButton, 1);
    mShopButton.mVisible = FALSE;

    Sprite_Init(&mCloseButton, 2, 0, 128, 96, TRUE);
    SpriteButton_SetSequences(&mCloseButton, 0, 1);
    mCloseButton.SetPositionFromVec(242 << FX32_SHIFT, 13 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mCloseButton, 24, 22);
    SpriteButton_Reset(&mCloseButton);
    SpriteButton_SetKind(&mCloseButton, 4);
    mCloseButton.mUnknownDC = TRUE;
    mCloseButton.mSounds.mSounds[0] = 12;
    mCloseButton.mSounds.mSounds[2] = 0;
    mCloseButton.mOverwritePalette = 1;
    mCloseButton.mVisible = FALSE;
}

void Scene40ListPage::Update() {
    switch (mState = mNextState) {
    case STATE_NONE:
        break;
    case STATE_OPEN:
        Open();
        break;
    case STATE_CHOOSE:
        Choose();
        break;
    case STATE_CHOSEN:
        mNextState = STATE_CLOSE;
        break;
    case STATE_CLOSE:
        Close();
        break;
    }
}

void Scene40ListPage::Open() {
    func_ov075_0217ade4(mContext->mPhysics, &mItemModel);
    mContext->mSpeechWindow->Hide();
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x77DC, 0x100);
    if (mSortClothes) {
        mClothesList->SortHeldFirst();
        mSortClothes = FALSE;
    }
    if (mSortGoods) {
        mInventory->SortHeldFirst();
        mSortGoods = FALSE;
    }
    if (mSortInteriors) {
        mInteriors->SortHeldFirst();
        mSortInteriors = FALSE;
    }

    switch (mMode) {
    case MODE_GIVE:
    case MODE_GIVE_FROM_SHOP: {
        mTitle.mSprite.mVisible = FALSE;
        mCloseButton.mVisible = TRUE;
        mListTabs.UpdateAvailable(mContext);
        mListTabs.SetVisible(TRUE);
        mListTabs.Select(mTab);
        mOkButton.mVisible = TRUE;
        SpriteButton_SetDisabled(&mOkButton, TRUE);
        NNS_G2dCharCanvasClear(&mOkButton.mText.mCanvas, 0);
        mOkButton.mText.mMessage = 476;
        mOkButton.mText.DrawAll();
        NNS_G2dCharCanvasClear(&mShopButton.mText.mCanvas, 0);
        mShopButton.mText.DrawAll();
        mList.SetMode(ItemListView::MODE_COUNT);
        mList.SetVisible(TRUE);
        mList.mTouchable = TRUE;
        int tab = 0;
        int filterTab = 0;
        if (mMode == MODE_GIVE_FROM_SHOP) {
            mContext->mCharacter->SetState(0);
            u16 *bought = &gSceneScratch.mShopItem;
            int item = *bought;
            int shownTab = mTab;
            if (item != 0xFFFF) {
                switch (shownTab) {
                case 0:
                    filterTab = Scene40GoodsTabs::FindTab(GetGoodsInfo(item)->mFlags);
                    break;
                case 1:
                    break;
                case 2:
                    filterTab = Scene40ClothesTabs::FindTab(GetClothesInfo(item)->mKind);
                    break;
                case 3:
                    break;
                }
            }
            ShowList(shownTab, filterTab);
            if (item != 0xFFFF) {
                int index = mList.FindItem(item);
                if (index != -1) {
                    mList.Select(index);
                    mList.ScrollToSelected();
                    ShowItem(*bought);
                }
            }
        } else {
            Scene40Character *character = mContext->mCharacter;
            if (character->mTroubleState == 2) {
                switch (character->mTroubleCategory) {
                case 0:
                    break;
                case 1:
                    tab = 1;
                    break;
                case 2:
                    tab = 2;
                    if (MiiData_GetSex(character) != 0) {
                        filterTab = 1;
                    }
                    break;
                case 8:
                    tab = 3;
                    break;
                }
            }
            mTab = tab;
            mListTabs.Select(tab);
            ShowList(tab, filterTab);
        }
        break;
    }
    case MODE_DRESS:
    case MODE_DRESS_2: {
        wchar_t name[12];
        MiiData_GetName(mContext->mCharacter, name);
        mTitle.SetStringArg(name, 0);
        mTitle.mSprite.mVisible = TRUE;
        NNS_G2dCharCanvasClear(&mTitle.mCanvas, 0);
        mTitle.DrawAll();
        mShopButton.mVisible = FALSE;
        mCloseButton.mVisible = mMode == MODE_DRESS ? TRUE : FALSE;
        mListTabs.SetVisible(FALSE);
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(TRUE);
        mOwnClothes.LoadOwnedBy(mContext->mCharacter);
        mOwnClothes.Sort();
        if (MiiData_GetSex(mContext->mCharacter) == 0) {
            mClothesTabs.Select(0);
            mOwnClothes.GetOwned(mItems, &mItemCount, 0);
        } else {
            mClothesTabs.Select(1);
            mOwnClothes.GetOwned(mItems, &mItemCount, 1);
        }
        mOkButton.mVisible = TRUE;
        SpriteButton_SetDisabled(&mOkButton, TRUE);
        NNS_G2dCharCanvasClear(&mOkButton.mText.mCanvas, 0);
        mOkButton.mText.mMessage = 479;
        mOkButton.mText.DrawAll();
        mList.SetMode(ItemListView::MODE_NAME);
        mList.SetMessageFile(mContext->mCostumeMessages);
        mList.mMarkKind = Scene40ListView::MARK_NONE;
        mList.SetVisible(TRUE);
        mList.SetItems(mItems, mItemCount);
        mList.mTouchable = TRUE;
        mGift.Set(1, 0);
        break;
    }
    }
    mNextState = STATE_CHOOSE;
}

void Scene40ListPage::Choose() {
    mListTabs.Update();
    if (mListTabs.mPressed != 0) {
        ShowList(mListTabs.mSelected, -1);
        SpriteButton_SetDisabled(&mOkButton, TRUE);
    }
    mGoodsTabs.Update();
    if (mGoodsTabs.mPressed != -1) {
        mInventory->GetOwned(mItems, &mItemCount, 1, mGoodsTabs.GetFilter());
        mList.SetItems(mItems, mItemCount);
        SpriteButton_SetDisabled(&mOkButton, TRUE);
    }
    mClothesTabs.Update();
    if (mClothesTabs.mPressed != -1) {
        switch (mMode) {
        case MODE_GIVE:
        case MODE_GIVE_FROM_SHOP:
            mClothesList->GetOwned(mItems, &mItemCount, mClothesTabs.GetKind());
            mList.SetItems(mItems, mItemCount);
            break;
        case MODE_DRESS:
        case MODE_DRESS_2:
            mOwnClothes.GetOwned(mItems, &mItemCount, mClothesTabs.GetKind());
            mList.SetItems(mItems, mItemCount);
            break;
        }
        SpriteButton_SetDisabled(&mOkButton, TRUE);
    }
    mList.Update();
    if (mList.mTouched && mList.mSelected != -1) {
        ShowItem(mList.mItems[mList.mSelected]->mId);
    }

    if (mActive && SpriteButton_Update(&mOkButton)) {
        if (mList.mSelected != -1) {
            Item *item = mList.mItems[mList.mSelected];
            mChosenId = item->mId;
            switch (mMode) {
            case MODE_GIVE:
            case MODE_GIVE_FROM_SHOP:
                item->Add(-1);
                if (item->mCount <= 0) {
                    switch (mListTabs.mSelected) {
                    case 0:
                    case 1:
                        mSortGoods = TRUE;
                        break;
                    case 2:
                        mSortClothes = TRUE;
                        break;
                    case 3:
                        mSortInteriors = TRUE;
                        break;
                    }
                }
                break;
            case MODE_DRESS:
                mContext->mCharacter->mClothesId = (u16)mChosenId;
                func_ov075_021750dc(mContext->mCharacter);
                mContext->mCharacter->SetState(0x55);
                break;
            case MODE_DRESS_2:
                mContext->mCharacter->mClothesId = (u16)mChosenId;
                func_ov075_021750dc(mContext->mCharacter);
                mContext->mCharacter->SetState(0x9A);
                break;
            }
            mChosen = TRUE;
        } else {
            mChosen = FALSE;
        }
        mNextState = STATE_CHOSEN;
    }
    if (mActive && SpriteButton_Update(&mCloseButton)) {
        mChosen = FALSE;
        mNextState = STATE_CHOSEN;
    }
    if (mActive && SpriteButton_Update(&mShopButton)) {
        gSceneMemory.mUnknown2C[0] = 0xFFFF;
        if (mList.mSelected != -1) {
            gSceneMemory.mUnknown2C[0] = mList.mItems[mList.mSelected]->mId;
        }
        switch (mListTabs.mSelected) {
        case 0:
            if (gIslandState.GetUnknown44(3) == 2) {
                gIslandState.SetUnknown44(3, 1);
            }
            gSceneManager.RequestScene(0x68);
            break;
        case 1:
            if (gIslandState.GetUnknown44(8) == 2) {
                gIslandState.SetUnknown44(8, 1);
            }
            gSceneManager.RequestScene(0x2F);
            break;
        case 2:
            if (gIslandState.GetUnknown44(10) == 2) {
                gIslandState.SetUnknown44(10, 1);
            }
            gSceneManager.RequestScene(0x1E);
            break;
        case 3:
            if (gIslandState.GetUnknown44(11) == 2) {
                gIslandState.SetUnknown44(11, 1);
            }
            gSceneManager.RequestScene(0x35);
            break;
        }
    }
    if (mMannequin->mHead.mVisible) {
        PlaceMannequin();
    }
    if (mItemModel->mVisible) {
        PlaceItemModel();
    }
}

void Scene40ListPage::Close() {
    func_ov075_0217af40(mItemModel);
    mItemModel = NULL;
    mMannequin->SetVisible(FALSE);
    mOkButton.mVisible = FALSE;
    mShopButton.mVisible = FALSE;
    mCloseButton.mVisible = FALSE;
    mListTabs.SetVisible(FALSE);
    mGoodsTabs.SetVisible(FALSE);
    mClothesTabs.SetVisible(FALSE);
    mList.SetVisible(FALSE);
    mContext->mSpeechWindow->Hide();
    mNextState = STATE_NONE;
}

void Scene40ListPage::Draw() {
    mTitle.Draw();
    mList.Draw();
    TextButton_Draw(&mOkButton);
    TextButton_Draw(&mShopButton);
    SpriteButton_Draw(&mCloseButton);
    mListTabs.Draw();
    mGoodsTabs.Draw();
    mClothesTabs.Draw();
}

// The list of a tab: filterTab chooses the goods' filter or the kind of clothes, -1 for the first (for clothes, the
// character's sex's)
void Scene40ListPage::ShowList(int tab, int filterTab) {
    if (filterTab == -1) {
        filterTab = 0;
        if (tab == 2 && MiiData_GetSex(mContext->mCharacter) != 0) {
            filterTab = 1;
        }
    }
    switch (tab) {
    case 0:
        mGoodsTabs.SetVisible(TRUE);
        mGoodsTabs.Select(filterTab);
        mClothesTabs.SetVisible(FALSE);
        mInventory->GetOwned(mItems, &mItemCount, 1, mGoodsTabs.GetFilter());
        mList.SetMessageFile(mContext->mItemMessages);
        mList.mMarkKind = Scene40ListView::MARK_ITEMS;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = mContext->mSceneId == 0x40 ? TRUE : FALSE;
        mGift.Set(0, 0);
        break;
    case 1:
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(FALSE);
        mInventory->GetOwned(mItems, &mItemCount, 3);
        mList.SetMessageFile(mContext->mItemMessages);
        mList.mMarkKind = Scene40ListView::MARK_ITEMS;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = FALSE;
        mGift.Set(0, 0);
        break;
    case 2:
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(TRUE);
        mClothesTabs.Select(filterTab);
        mClothesList->GetOwned(mItems, &mItemCount, mClothesTabs.GetKind());
        mList.SetMessageFile(mContext->mCostumeMessages);
        mList.mMarkKind = Scene40ListView::MARK_CLOTHES;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = gIslandState.GetUnknown44(10) ? TRUE : FALSE;
        mGift.Set(1, 0);
        break;
    case 3:
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(FALSE);
        mInteriors->GetOwnedFrom8(mItems, &mItemCount);
        mList.SetMessageFile(mContext->mRoomMessages);
        mList.mMarkKind = Scene40ListView::MARK_INTERIORS;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = gIslandState.GetUnknown44(11) ? TRUE : FALSE;
        mGift.Set(2, 0);
        break;
    }
}

// Shows the item touched: goods on an item model, clothes on the mannequin
void Scene40ListPage::ShowItem(u16 id) {
    SpriteButton_SetDisabled(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
    switch (mGift.mKind) {
    case 0:
        func_ov075_0217af88(mItemModel, id);
        mItemModel->mVisible = TRUE;
        *(BOOL *)&mItemModel->mUnknown12C[0x10] = TRUE; // TODO: ov075-e's field
        mMannequin->SetVisible(FALSE);
        mGift.Set(0, id);
        break;
    case 1:
        DressMannequin(id);
        mMannequin->SetVisible(TRUE);
        mItemModel->mVisible = FALSE;
        mGift.Set(1, id);
        break;
    case 2:
        mItemModel->mVisible = FALSE;
        mMannequin->SetVisible(FALSE);
        mGift.Set(2, id);
        break;
    }
}

void Scene40ListPage::DressMannequin(u16 clothes) {
    ModelResources_ReleaseModel(gModelResources, 12);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    mMannequin->SetArchive("3MM", clothes, MiiData_GetSex(mContext->mCharacter));
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin->mArchivePath, TRUE);
    ModelResources_SetModel(gModelResources, 12, mMannequin->mModelPath);
    mMannequin->MakeModel(12, 0);
    mMannequin->SetPaletteColors(NFL_GetFavoriteColor(MiiData_GetFavoriteColor(mContext->mCharacter)), 0x5FBE);
    mMannequin->SetAnimFlags(TRUE, TRUE);
}

// Puts the mannequin where the screen point shows, turned a little and sized by its distance from the camera
void Scene40ListPage::PlaceMannequin() {
    VecFx32 position;
    ProjectTouchBetween(&sMannequinScreenPoint, &position, 0x3000, 0x4000);
    fx32 distance = GetCameraDistanceScale(&position);
    MtxFx33 rotation;
    MakeFacingMatrixY(&rotation, -0x1F3, 0xFE1);
    MI_Copy36B(&rotation, &mMannequin->mAnim.mRotation);
    fx32 scale = FX_Mul(distance, 0xF33);
    SetBodyPosition(mMannequin, position);
    SetBodyScaleX(FX_Mul(scale, gMannequinBodyScale.x), mMannequin);
    SetBodyScaleY(FX_Mul(scale, gMannequinBodyScale.y), mMannequin);
    SetBodyScaleZ(FX_Mul(scale, gMannequinBodyScale.z), mMannequin);
    mMannequin->mHeadScale.x = FX_Mul(scale, gMannequinHeadScale.x);
    mMannequin->mHeadScale.y = FX_Mul(scale, gMannequinHeadScale.y);
    mMannequin->mHeadScale.z = FX_Mul(scale, gMannequinHeadScale.z);
}

void Scene40ListPage::PlaceItemModel() {
    Camera_UpdateEye(&gCamera);
    VecFx32 position;
    ProjectTouchBetween(&sItemScreenPoint, &position, 0x3000, 0x4000);
    mItemModel->SetPosition(position);
}

void Scene40ListPage::GetGift(Scene40Gift *gift) {
    gift->Set(mGift.mKind, mGift.mItem.InventoryItem::mId);
}

// --- The list page's tabs ------------------------------------------------------------------------------------------

static const u16 sListTabSequences[Scene40ListTabs::TAB_COUNT] = {41, 47, 43, 45};
static const u16 sListTabPressedSequences[Scene40ListTabs::TAB_COUNT] = {42, 48, 44, 46};
static const ScreenPoint sListTabPositions[Scene40ListTabs::TAB_COUNT] = {{23, 13}, {158, 13}, {68, 13}, {113, 13}};

void Scene40ListTabs::Init(int resource) {
    mPressed = 0;
    mSelected = 0;
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sListTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sListTabSequences[i], sListTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sListTabPositions[i].x << FX32_SHIFT, sListTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 44, 22);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
}

// A list's tab is there when the player owns some of it or the island has the shop that sells it
void Scene40ListTabs::UpdateAvailable(Scene40Context *context) {
    u8 built = gIslandState.GetUnknown44(3);
    BOOL owned = context->mInventory->HasAnyOwnedKind1();
    mAvailable[0] = owned | (built != 0);
    u8 level = gGameData.mUnknownB2[0];
    owned = context->mInventory->HasAnyOwnedKind3();
    mAvailable[1] = owned | (level >= 5);
    built = gIslandState.GetUnknown44(10);
    owned = context->mClothesList->HasAnyOwned();
    mAvailable[2] = owned | (built != 0);
    built = gIslandState.GetUnknown44(11);
    owned = context->mInteriors->HasAnyOwned();
    mAvailable[3] = owned | (built != 0);
}

void Scene40ListTabs::Update() {
    mPressed = 0;
    if (SpriteButton_Update(&mTabs[0])) {
        mPressed = 1 << 0;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 0;
    } else if (SpriteButton_Update(&mTabs[1])) {
        mPressed = 1 << 1;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 1;
    } else if (SpriteButton_Update(&mTabs[2])) {
        mPressed = 1 << 2;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 2;
    } else if (SpriteButton_Update(&mTabs[3])) {
        mPressed = 1 << 3;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 3;
    }
}

void Scene40ListTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40ListTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible & mAvailable[i];
    }
}

void Scene40ListTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    mSelected = tab;
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
}

static const u16 sGoodsTabSequences[Scene40GoodsTabs::TAB_COUNT] = {37, 39, 20, 22};
static const u16 sGoodsTabPressedSequences[Scene40GoodsTabs::TAB_COUNT] = {38, 40, 21, 23};
static const ScreenPoint sGoodsTabPositions[Scene40GoodsTabs::TAB_COUNT] = {{32, 41}, {88, 41}, {144, 41}, {200, 41}};
static const u32 sGoodsTabFilters[Scene40GoodsTabs::TAB_COUNT] = {1, 2, 8, 16};

void Scene40GoodsTabs::Init(int resource) {
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sGoodsTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sGoodsTabSequences[i], sGoodsTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sGoodsTabPositions[i].x << FX32_SHIFT, sGoodsTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 52, 24);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void Scene40GoodsTabs::Update() {
    mPressed = -1;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (SpriteButton_Update(&mTabs[i])) {
            SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void Scene40GoodsTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40GoodsTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible;
    }
}

void Scene40GoodsTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
    mSelected = tab;
}

u32 Scene40GoodsTabs::GetFilter() {
    return sGoodsTabFilters[mSelected];
}

int Scene40GoodsTabs::FindTab(u32 filter) {
    int tab = 0;
    switch (filter & 0x1F) {
    case 1:
        tab = 0;
        break;
    case 2:
        tab = 1;
        break;
    case 8:
        tab = 2;
        break;
    case 16:
        tab = 3;
        break;
    }
    return tab;
}

static const u16 sClothesTabSequences[Scene40ClothesTabs::TAB_COUNT] = {24, 26, 28, 30, 32};
static const u16 sClothesTabPressedSequences[Scene40ClothesTabs::TAB_COUNT] = {25, 27, 29, 31, 33};
static const ScreenPoint sClothesTabPositions[Scene40ClothesTabs::TAB_COUNT] = {
    {26, 41}, {72, 41}, {118, 41}, {164, 41}, {210, 41},
};
static const int sClothesTabKinds[Scene40ClothesTabs::TAB_COUNT] = {0, 1, 2, 3, 4};

void Scene40ClothesTabs::Init(int resource) {
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sClothesTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sClothesTabSequences[i], sClothesTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sClothesTabPositions[i].x << FX32_SHIFT, sClothesTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 42, 24);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void Scene40ClothesTabs::Update() {
    mPressed = -1;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (SpriteButton_Update(&mTabs[i])) {
            SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void Scene40ClothesTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40ClothesTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible;
    }
}

void Scene40ClothesTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
    mSelected = tab;
}

int Scene40ClothesTabs::GetKind() {
    return sClothesTabKinds[mSelected];
}

int Scene40ClothesTabs::FindTab(int kind) {
    int tab = 0;
    switch (kind) {
    case 0:
        tab = 0;
        break;
    case 1:
        tab = 1;
        break;
    case 2:
        tab = 2;
        break;
    case 3:
        tab = 3;
        break;
    case 4:
        tab = 4;
        break;
    }
    return tab;
}

// --- The scene's speech window (scene+0x163B0) ---------------------------------------------------------------------

void Scene40SpeechWindow::Update() {
    SpeechWindow::Update();
    if (mEnabled && gScreenControl.mUnknown44 == 0) {
        return;
    }
    Hide();
}

void Scene40SpeechWindow::Show(u32 message, int mode, int frames) {
    if (!mEnabled) {
        return;
    }
    if (gScreenControl.mUnknown44 != 0) {
        return;
    }
    SpeechWindow::Show(message, mode, frames);
}

// --- The talk page (pages+0x180) ----------------------------------------------------------------------------------

// The trouble the character has
static inline const TroubleInfo *GetTrouble(SavedCharacter *character) {
    return &data_02114d78[character->mTroubleCategory][character->mTroubleId];
}

void Scene40TalkPage::UpdateSecondAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        MI_CpuFill8(&gSceneScratch, 0, 100);
        GetSceneMemory().mTalkAnswers[1] = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0x92);
    mResult = 3;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateFirstOfTwo() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        MI_CpuFill8(&gSceneScratch, 0, 100);
        GetSceneMemory().mTalkAnswers[0] = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0x96);
    mChoices.SetChoices(14);
    mChoices.Open();
    mNextState = STATE_SECOND_OF_TWO;
}

void Scene40TalkPage::UpdateSecondOfTwo() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        GetSceneMemory().mTalkAnswers[1] = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0x97);
    mChoices.SetChoices(15);
    mChoices.Open();
    mNextState = STATE_YES_NO;
}

void Scene40TalkPage::UpdateYesNo() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0:
        mResult = 5;
        mNextState = STATE_DONE;
        break;
    case 1:
        mContext->mCharacter->SetState(0x98);
        mResult = 4;
        mNextState = STATE_DONE;
        break;
    }
    mProfilePage->SetSwitchable(FALSE);
}

void Scene40TalkPage::UpdateTroubleAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    int state = 0x9D;
    switch (mChoices.mAnswer) {
    case 0:
        state = 0x9D;
        break;
    case 1:
        state = 0x9E;
        break;
    case 2:
        state = 0x9F;
        break;
    case 3:
        state = 0xA0;
        break;
    }
    mContext->mCharacter->SetState(state);
    s64 now = mContext->mClock->mSeconds;
    u16 target = mContext->mCharacter->mTroubleTarget;
    mContext->mCharacter->ClearTrouble();
    mContext->mCharacter->mTroubleCategory = 4;
    mContext->mCharacter->mTroubleId = 3;
    mContext->mCharacter->mTroubleState = 1;
    mContext->mCharacter->mTroubleTarget = target;
    Scene40Character *character = mContext->mCharacter;
    character->mTroubleEnd = now + GetTrouble(character)->minutes * 60;
    mContext->mCharacter->mMood = 3;
    mContext->mCharacter->ResetTroubleProgress();
    func_ov075_021762ac(mContext->mCharacter);
    mContext->mUnknown18 = -100;
    mResult = 9;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateWaitQuestion() {
    if (mContext->mCharacter->mState != 0xAC) {
        return;
    }
    mContext->mCharacter->SetState(0xAD);
    mChoices.SetSpecialChoices(mContext->mCharacter->mUnknown2EE0[0]);
    mChoices.Open();
    mNextState = STATE_RELATION_ANSWER;
}

// The answer moves the character's relation meter with the trouble's target up or down by 40
void Scene40TalkPage::UpdateRelationAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        mContext->mCharacter->mUnknown2EE0[2] = mChoices.mPressed;
        Scene40Character *character = mContext->mCharacter;
        int answer = mChoices.mPressed;
        u16 target = character->mTroubleTarget;
        int meter = character->mRelationMeter[target];
        int value;
        switch (answer) {
        case 0:
            value = meter + 40;
            break;
        case 1:
            value = meter;
            break;
        case 2:
            value = meter - 40;
            break;
        }
        character->mRelationMeter[target] = value > 200 ? 200 : value < 0 ? 0 : value;
        mProfilePage->mRelationsChanged = TRUE;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0xAE);
    Scene40Character *character = mContext->mCharacter;
    character->mTroubleEnd = mContext->mClock->mSeconds + GetTrouble(character)->cooldownMinutes * 60;
    mContext->mCharacter->mTroubleState = 3;
    AddTroublesSolved(1);
    mContext->mUnknown18 = GetTrouble(mContext->mCharacter)->unk4Bits0;
    mNextState = STATE_WAIT_CHARACTER;
}

void Scene40TalkPage::UpdateSolveOrNot() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0: {
        mContext->mCharacter->SetState(0xB0);
        Scene40Character *character = mContext->mCharacter;
        character->mTroubleEnd = mContext->mClock->mSeconds + GetTrouble(character)->cooldownMinutes * 60;
        mContext->mCharacter->mTroubleState = 3;
        AddTroublesSolved(1);
        mContext->mUnknown18 = GetTrouble(mContext->mCharacter)->unk4Bits0;
        mNextState = STATE_WAIT_CHARACTER;
        break;
    }
    case 1:
        mContext->mSpeechWindow->Hide();
        mResult = 2;
        mNextState = STATE_DONE;
        break;
    case 2:
        mContext->mCharacter->SetState(0);
        mContext->mSpeechWindow->Hide();
        mResult = 0;
        mNextState = STATE_DONE;
        break;
    }
}

// Yes goes to one of the games, chosen by the character, with the character for it
void Scene40TalkPage::UpdatePlayOrNot() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0:
        MI_CpuFill8(&gSceneScratch, 0, 100);
        GetSceneMemory().mTalkAnswers[0] = 2;
        GetSceneMemory().mCharacters[0] = mContext->mCharacter->mIndex;
        gSceneManager.RequestScene(gStr_S_ov075[func_ov075_02176454(mContext->mCharacter)].mSceneId);
        break;
    case 1:
        mContext->mCharacter->SetState(0);
        mContext->mSpeechWindow->Hide();
        mResult = 0;
        mNextState = STATE_DONE;
        break;
    }
}

void Scene40TalkPage::UpdateFourAnswers() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0:
        mContext->mCharacter->SetState(0x69);
        break;
    case 1:
        mContext->mCharacter->SetState(0x6A);
        break;
    case 2:
        mContext->mCharacter->SetState(0x6B);
        break;
    case 3:
        mContext->mCharacter->SetState(0x6C);
        break;
    }
    Scene40Character *character = mContext->mCharacter;
    if (character->IsInTrouble() && GetTrouble(character)->kind == 13) {
        character = mContext->mCharacter;
        character->mTroubleEnd = mContext->mClock->mSeconds + GetTrouble(character)->cooldownMinutes * 60;
        mContext->mCharacter->mTroubleState = 3;
        mContext->mUnknown18 = GetTrouble(mContext->mCharacter)->unk4Bits0;
        AddTroublesSolved(1);
    } else {
        mContext->mUnknown18 = 30;
    }
    mResult = 7;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateWaitMessages() {
    Scene40MessageSequence *sequence = mContext->mMessageSequence;
    if (sequence->mLastState == Scene40MessageSequence::STATE_DONE) {
        mNextState = *(int *)sequence->mUnknown30;
    }
}

void Scene40TalkPage::UpdateWaitCharacter() {
    if (mContext->mCharacter->mState != 0x33) {
        return;
    }
    mResult = 8;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateLeaveA() {
    switch (mStep) {
    case 0:
        if (mContext->mCharacter->mState != 0xC9) {
            return;
        }
        mContext->mCharacter->SetState(0);
        *(int *)mContext->mMessageSequence->mUnknown30 = STATE_LEAVE_A;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 1;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        gSceneManager.RequestScene(0x69);
        break;
    }
}

void Scene40TalkPage::UpdateLeaveB() {
    switch (mStep) {
    case 0:
        if (mContext->mCharacter->mState != 0xCD) {
            return;
        }
        mContext->mCharacter->SetState(0);
        *(int *)mContext->mMessageSequence->mUnknown30 = STATE_LEAVE_B;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 3;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        gSceneManager.RequestScene(0x69);
        break;
    }
}

void Scene40TalkPage::Draw() {
    mChoices.Draw();
}

// --- The scene's menu (scene+0x6947C) ------------------------------------------------------------------------------

// Each button's sequence, pressed sequence, place and touch rectangle in pixels and sound; the first button's
// values are written out in Init as well
static const u16 sMenuSequences[Scene40MenuBar::BUTTON_COUNT] = {4, 7, 2, 11, 9, 28, 0};
static const u16 sMenuPressedSequences[Scene40MenuBar::BUTTON_COUNT] = {6, 8, 3, 12, 10, 29, 1};
static const ScreenPoint sMenuPositions[Scene40MenuBar::BUTTON_COUNT] = {
    {18, 13}, {18, 109}, {18, 37}, {18, 85}, {18, 61}, {207, 13}, {242, 13},
};
static const ScreenPoint sMenuTouchSizes[Scene40MenuBar::BUTTON_COUNT] = {
    {32, 22}, {32, 22}, {32, 22}, {32, 22}, {32, 22}, {32, 22}, {24, 22},
};
static const int sMenuSounds[Scene40MenuBar::BUTTON_COUNT] = {10, 10, 10, 10, 10, 10, 12};

void Scene40CallButton::Reset() {
    SpriteButton_Reset(this);
    mCallState = CALL_NONE;
    mCallFrames = 0;
}

void Scene40CallButton::SetSequences(u16 sequence, u16 pressedSequence, u16 callSequence) {
    SpriteButton_SetSequences(this, sequence, pressedSequence);
    mCallSequence = callSequence;
}

void Scene40CallButton::StartCall() {
    if (!mVisible) {
        return;
    }
    mCallState = CALL_WAITING;
    mCallFrames = 0;
}

// Whether it was pressed; a touch stops the call
BOOL Scene40CallButton::Update() {
    BOOL pressed = SpriteButton_Update(this);
    BOOL selected = func_02031d00(this);
    if (pressed | selected) {
        mCallState = CALL_NONE;
    }
    return pressed;
}

void Scene40CallButton::UpdateCall() {
    switch (mCallState) {
    case CALL_NONE:
        break;
    case CALL_WAITING:
        if (++mCallFrames < 120) {
            break;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            break;
        }
        Sprite_SetSequence(this, mCallSequence, TRUE);
        gSoundManager.StartSeqArc(1, 10, 14);
        mCallState = CALL_CALLING;
        mCallFrames = 0;
        break;
    case CALL_CALLING:
        if (++mCallFrames < 160) {
            if (mCallFrames % 20 == 0 && (mCallFrames / 20) % 2 == 0) {
                gSoundManager.StartSeqArc(1, 10, 15);
            }
        } else if (mCallFrames >= 160) {
            Sprite_SetSequence(this, mSequence, TRUE);
            mCallState = CALL_NONE;
            mCallFrames = 0;
        }
        break;
    }
}

void Scene40CallButton::SetDisabled(BOOL disabled) {
    SpriteButton_SetDisabled(this, disabled);
    if (disabled) {
        mCallState = CALL_NONE;
    }
}

void Scene40ButtonFlash::Update() {
    if (!mVisible) {
        return;
    }
    if (!mAnim.animCtrl.bActive) {
        mVisible = FALSE;
    }
}

void Scene40ButtonFlash::Play() {
    if (mVisible) {
        return;
    }
    mVisible = TRUE;
    Sprite_SetPlaying(this, TRUE);
}

void Scene40MenuBar::Init(Scene40Context *context) {
    mUnknown000 = 0;
    mUnknown004 = 0;
    mTouchedButton = -1;
    mPressedButton = -1;
    mContext = context;
    mCharacter = context->mCharacter;
    mEnabled = FALSE;

    Sprite_Init(&mCallButton, 0, 4, 128, 96, TRUE);
    mCallButton.SetSequences(4, 6, 5);
    mCallButton.mTtsPlayer = mContext->mTtsPlayer;
    mCallButton.SetPositionFromVec(18 << FX32_SHIFT, 13 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mCallButton, 32, 22);
    mCallButton.Reset();
    SpriteButton_SetKind(&mCallButton, 4);
    mCallButton.mOverwritePalette = 1;
    mCallButton.mUnknownDC = TRUE;
    mCallButton.mSounds.mSounds[0] = 10;
    mCallButton.mSounds.mSounds[2] = 0;
    mButtonList[0] = &mCallButton;

    for (int i = 1; i < BUTTON_COUNT; i++) {
        Sprite_Init(&mButtons[i], 0, sMenuSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mButtons[i], sMenuSequences[i], sMenuPressedSequences[i]);
        mButtons[i].SetPositionFromVec(sMenuPositions[i].x << FX32_SHIFT, sMenuPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mButtons[i], sMenuTouchSizes[i].x, sMenuTouchSizes[i].y);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 4);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = TRUE;
        mButtons[i].mSounds.mSounds[0] = sMenuSounds[i];
        mButtons[i].mSounds.mSounds[2] = 0;
        mButtonList[i] = &mButtons[i];
    }
    for (int i = 0; i < 5; i++) {
        mButtonList[i]->mVisible = FALSE;
    }

    Sprite_Init(&mFlash, 0, 31, 128, 96, TRUE);
    mFlash.mOverwritePalette = 1;
    mFlash.mVisible = FALSE;
}

void Scene40MenuBar::Update() {
    mTouchedButton = -1;
    mPressedButton = -1;
    mFlash.Update();
    mCallButton.UpdateCall();
    if (mEnabled && SpriteButton_IsTouched(&mCallButton)) {
        mTouchedButton = 0;
    }
    if (mEnabled && mCallButton.Update()) {
        mPressedButton = 0;
    }
    for (int i = 1; i < BUTTON_COUNT; i++) {
        if (mEnabled && SpriteButton_IsTouched(mButtonList[i])) {
            mTouchedButton = i;
        }
        if (mEnabled && SpriteButton_Update(mButtonList[i])) {
            mPressedButton = i;
        }
    }
}

void Scene40MenuBar::SetEnabled(BOOL enabled) {
    if (enabled != mEnabled) {
        mEnabled = enabled;
    }
}

// Shows the buttons whose offer is there; coming back from scene 0x38 or 0x40, the first five are as that scene
// left them
void Scene40MenuBar::Refresh() {
    UpdateAvailable();
    if (gSceneManager.mPreviousSceneId == 0x38) {
        for (int i = 0; i < 5; i++) {
            mAvailable[i] = gSceneScratch.mMenuAvailable[i];
        }
    } else if (gSceneManager.mPreviousSceneId == 0x40) {
        for (int i = 0; i < 5; i++) {
            mAvailable[i] = gSceneScratch.mMenuAvailable[i];
        }
    }
    for (int i = 0; i < BUTTON_COUNT; i++) {
        mButtonList[i]->mVisible = mAvailable[i] ? TRUE : FALSE;
    }
}

void Scene40MenuBar::UpdateAvailable() {
    mAvailable[0] = mContext->mSceneId != 0x6C;
    mAvailable[1] = MATH_CountPopulation(mCharacter->mUnknown299) != 0;
    mAvailable[2] = mCharacter->CountOwnedClothes() > 1;
    mAvailable[3] = wcslen(mCharacter->mMotto) != 0;
    mAvailable[4] = mCharacter->CountOwnedInteriors() > 1;
    mAvailable[5] = TRUE;
    mAvailable[6] = TRUE;
}

void Scene40MenuBar::Draw() {
    Sprite_Draw(&mFlash);
    for (int i = 0; i < BUTTON_COUNT; i++) {
        SpriteButton_Draw(mButtonList[i]);
    }
}

void Scene40MenuBar::StartCall() {
    mCallButton.StartCall();
}

// Shows the button chosen and plays the flash over it
void Scene40MenuBar::Choose(int button) {
    SetButtonVisible(button, TRUE);
    ScreenPoint position = mButtonList[button]->GetPixelPosition();
    mFlash.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    mFlash.Play();
    gSoundManager.StartSeqArc(1, 10, 17);
}

void Scene40MenuBar::SetButtonVisible(int button, BOOL visible) {
    mButtonList[button]->mVisible = visible;
}

void Scene40MenuBar::SetVisible(BOOL visible) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        mButtonList[i]->mVisible = visible;
    }
}

void Scene40MenuBar::SetDisabled(BOOL disabled) {
    mCallButton.SetDisabled(disabled);
    for (int i = 1; i < BUTTON_COUNT; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

void Scene40MenuBar::SetLeftDisabled(BOOL disabled) {
    mCallButton.SetDisabled(disabled);
    for (int i = 1; i < 5; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

void Scene40MenuBar::SetRightDisabled(BOOL disabled) {
    for (int i = 5; i <= 6; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

// --- The scene's character ------------------------------------------------------------------------------------------

// With speaking set, the lips follow the scene's speech
void Scene40Character::Update(BOOL speaking) {
    Character::Update();
    func_ov075_02161988(this);
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    func_ov075_02178364(&mPartA);
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
    mPartA.Draw();
    mPartB.Draw();
}

void Scene40CharacterPartA::Draw() {
    mModel.Draw();
}

void Scene40CharacterPartB::Draw() {
    mModel.Draw();
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

// --- The scene's clock, at 0x30A8C --------------------------------------------------------------------------------

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
