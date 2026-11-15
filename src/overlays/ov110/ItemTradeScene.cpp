#include "ItemTradeScene.h"

// Overlay 110: wireless play with items (ItemTradeScene). One source file: the list of the player's things with its
// tabs and buttons, the scene, and the scene's implicit destructor, which the compiler emits with the scene's
// virtual table, at the end of .text.

// --- The file's constants ---

// The tabs' sequences, off and on, and where they are
static const u16 sTabSequences[5] = { 0x2B, 0x2D, 0x2F, 0x31, 0x29 };
static const u16 sTabOnSequences[5] = { 0x2C, 0x2E, 0x30, 0x32, 0x2A };
static const ScreenPoint sTabPositions[5] = { { 23, 13 }, { 68, 13 }, { 113, 13 }, { 158, 13 }, { 203, 13 } };
// The item filter's
static const u16 sItemFilterSequences[4] = { 0x25, 0x27, 0x14, 0x16 };
static const u16 sItemFilterOnSequences[4] = { 0x26, 0x28, 0x15, 0x17 };
static const ScreenPoint sItemFilterPositions[4] = { { 28, 41 }, { 88, 41 }, { 148, 41 }, { 208, 41 } };
static const int sItemFilters[4] = { 1, 2, 8, 0x10 };
// The costume filter's
static const u16 sCostumeFilterSequences[5] = { 0x18, 0x1A, 0x1C, 0x1E, 0x20 };
static const u16 sCostumeFilterOnSequences[5] = { 0x19, 0x1B, 0x1D, 0x1F, 0x21 };
static const ScreenPoint sCostumeFilterPositions[5] = { { 26, 41 }, { 72, 41 }, { 118, 41 }, { 164, 41 }, { 210, 41 } };
static const int sCostumeFilters[5] = { 0, 1, 2, 3, 4 };
// Where each view of the upper screen rests, and where it comes in from
static const ScreenPoint sPreviewPositions[3] = { { 128, 134 }, { 190, 172 }, { 128, 128 } };
static const ScreenPoint sPreviewStarts[3] = { { 128, 0 }, { 190, 0 }, { 128, 0 } };

// The lights' colours for an item or a costume (a room has its own)
static GXRgb sLightColors[2] = { GX_RGB(31, 31, 31), GX_RGB(31, 31, 31) };

// What the two DS send each other. The buffers are WH's: 32-byte aligned.
struct ItemTradeComm {
    /* 0x00 */ u8 mConnected;        // The receiver was called without data
    /* 0x02 */ u16 mReceived;        // An item has come
    /* 0x04 */ u8 mUnknown04[0x1C];
    /* 0x20 */ u16 mCommand;         // What Send sends with a command
    /* 0x22 */ u16 mSending;         // An item is being sent
    /* 0x24 */ u16 mReceivedCommand; // The command the other DS sent
    /* 0x28 */ BOOL mReceivedNew;
    /* 0x2C */ u8 mUnknown2C[0x14];
    /* 0x40 */ int mItem;            // The item sent or received: an item, costume + 0x800, or interior + 0x1000
    /* 0x44 */ BOOL mSent;
}; // 0x48

static ItemTradeComm sComm ATTRIBUTE_ALIGN(32);

// --- Sounds ---

// @0x02149a40
static void PlaySound(int index) {
    gSoundManager.StartSeqArc(1, 0x19, index);
}

// @0x02149a60
static void PlaySound(int handle, int index) {
    gSoundManager.StartSeqArc(handle, 0x19, index);
}

// @0x02149a84
static void StopSound(int handle) {
    gSoundManager.StopSeqArc(handle, 0);
}

// --- What is chosen ---

// @0x02149aa0
void ItemTradeSelection::Set(int kind, int id) {
    mKind = kind;
    switch (kind) {
    case 0:
        mId = id;
        mValue = id;
        {
            u16 item = mId;
            int itemKind = data_020f5f0c[item].mKind;
            if (itemKind == 1) {
                mCategory = 0;
            } else if (itemKind == 2) {
                mCategory = 4;
            } else if (item == 0x81) {
                mCategory = 6;
            } else {
                mCategory = 7;
            }
        }
        break;
    case 1:
        mId = id;
        mCategory = 1;
        mValue = 0xF7;
        break;
    case 2:
        mId = id;
        mCategory = 2;
        mValue = 0xF9;
        break;
    case 3:
        mId = id;
        mCategory = 3;
        mValue = 0xF4;
        break;
    case 4:
        mId = id;
        mCategory = 5;
        mValue = 0xF5;
        break;
    }
}

// --- The tabs ---

// @0x02149b84
void ItemTradeTabs::Init(int plane, MessageFile *file, int *charOffset) {
    mPressed = 0;
    mSelected = 0;
    for (int i = 0; i < 5; i++) {
        Sprite_Init(&mButtons[i], plane, sTabSequences[i], 128, 96, TRUE);
        func_02031278(&mButtons[i], sTabSequences[i], sTabOnSequences[i]);
        mButtons[i].SetPosition(FxVec2(sTabPositions[i].x << FX32_SHIFT, sTabPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], 44, 22);
        func_0203119c(&mButtons[i]);
        func_02031260(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mSoundArc = 1;
        mButtons[i].mSounds[0] = 3;
    }
    SetVisible(FALSE);
}

// @0x02149c88
void ItemTradeTabs::SetMasks(Inventory *inventory, CostumeList *costumes, InteriorList *interiors) {
    mMasks[0] = func_0209f4b4(inventory);
    mMasks[3] = func_0209f51c(inventory);
    mMasks[1] = func_0209e7dc(costumes);
    mMasks[2] = func_020a53c8(interiors);
    mMasks[4] = func_0209f584(inventory);
}

// @0x02149cdc
void ItemTradeTabs::Update() {
    mPressed = 0;
    if (func_0203133c(&mButtons[0])) {
        mPressed = TAB_ITEMS;
        func_02031e28(&mButtons[mSelected], FALSE);
        mSelected = 0;
    } else if (func_0203133c(&mButtons[3])) {
        mPressed = TAB_ITEMS_2;
        func_02031e28(&mButtons[mSelected], FALSE);
        mSelected = 3;
    } else if (func_0203133c(&mButtons[1])) {
        mPressed = TAB_COSTUMES;
        func_02031e28(&mButtons[mSelected], FALSE);
        mSelected = 1;
    } else if (func_0203133c(&mButtons[2])) {
        mPressed = TAB_INTERIORS;
        func_02031e28(&mButtons[mSelected], FALSE);
        mSelected = 2;
    } else if (func_0203133c(&mButtons[4])) {
        mPressed = TAB_ITEMS_3;
        func_02031e28(&mButtons[mSelected], FALSE);
        mSelected = 4;
    }
}

// @0x02149e18
void ItemTradeTabs::Draw() {
    for (int i = 0; i < 5; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

// @0x02149e40
void ItemTradeTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < 5; i++) {
        mButtons[i].mVisible = visible & mMasks[i];
    }
}

// @0x02149e6c
void ItemTradeTabs::Select(int tab) {
    switch (tab) {
    case TAB_ITEMS:
        mSelected = 0;
        func_02031e28(&mButtons[0], TRUE);
        func_02031e28(&mButtons[3], FALSE);
        func_02031e28(&mButtons[1], FALSE);
        func_02031e28(&mButtons[2], FALSE);
        func_02031e28(&mButtons[4], FALSE);
        break;
    case TAB_ITEMS_2:
        mSelected = 3;
        func_02031e28(&mButtons[0], FALSE);
        func_02031e28(&mButtons[3], TRUE);
        func_02031e28(&mButtons[1], FALSE);
        func_02031e28(&mButtons[2], FALSE);
        func_02031e28(&mButtons[4], FALSE);
        break;
    case TAB_COSTUMES:
        mSelected = 1;
        func_02031e28(&mButtons[0], FALSE);
        func_02031e28(&mButtons[3], FALSE);
        func_02031e28(&mButtons[1], TRUE);
        func_02031e28(&mButtons[2], FALSE);
        func_02031e28(&mButtons[4], FALSE);
        break;
    case TAB_INTERIORS:
        mSelected = 2;
        func_02031e28(&mButtons[0], FALSE);
        func_02031e28(&mButtons[3], FALSE);
        func_02031e28(&mButtons[1], FALSE);
        func_02031e28(&mButtons[2], TRUE);
        func_02031e28(&mButtons[4], FALSE);
        break;
    case TAB_ITEMS_3:
        mSelected = 4;
        func_02031e28(&mButtons[0], FALSE);
        func_02031e28(&mButtons[3], FALSE);
        func_02031e28(&mButtons[1], FALSE);
        func_02031e28(&mButtons[2], FALSE);
        func_02031e28(&mButtons[4], TRUE);
        break;
    }
}

// @0x0214a020
void ItemTradeTabs::SetEnabled(int tabs) {
    for (int i = 0; i < 5; i++) {
        func_02031e88(&mButtons[i], TRUE);
    }
    if (tabs & TAB_ITEMS) {
        func_02031e88(&mButtons[0], FALSE);
    }
    if (tabs & TAB_ITEMS_2) {
        func_02031e88(&mButtons[3], FALSE);
    }
    if (tabs & TAB_COSTUMES) {
        func_02031e88(&mButtons[1], FALSE);
    }
    if (tabs & TAB_INTERIORS) {
        func_02031e88(&mButtons[2], FALSE);
    }
    if (tabs & TAB_ITEMS_3) {
        func_02031e88(&mButtons[4], FALSE);
    }
}

// @0x0214a0bc
void ItemTradeTabs::func_ov110_0214a0bc() {
}

// --- The item filter ---

// @0x0214a0c0
void ItemTradeItemFilter::Init(int plane) {
    for (int i = 0; i < 4; i++) {
        Sprite_Init(&mButtons[i], plane, sItemFilterSequences[i], 128, 96, TRUE);
        func_02031278(&mButtons[i], sItemFilterSequences[i], sItemFilterOnSequences[i]);
        mButtons[i].SetPosition(
            FxVec2(sItemFilterPositions[i].x << FX32_SHIFT, sItemFilterPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], 42, 24);
        func_0203119c(&mButtons[i]);
        func_02031260(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mSoundArc = 1;
        mButtons[i].mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

// @0x0214a1c8
void ItemTradeItemFilter::Update() {
    mPressed = -1;
    for (int i = 0; i < 4; i++) {
        if (func_0203133c(&mButtons[i])) {
            func_02031e28(&mButtons[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

// @0x0214a228
void ItemTradeItemFilter::Draw() {
    for (int i = 0; i < 4; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

// @0x0214a250
void ItemTradeItemFilter::SetVisible(BOOL visible) {
    for (int i = 0; i < 4; i++) {
        mButtons[i].mVisible = visible;
    }
}

// @0x0214a26c
void ItemTradeItemFilter::Select(int button) {
    for (int i = 0; i < 4; i++) {
        func_02031e28(&mButtons[i], FALSE);
    }
    func_02031e28(&mButtons[button], TRUE);
    mSelected = button;
}

// @0x0214a2bc
int ItemTradeItemFilter::GetFilter() {
    return sItemFilters[mSelected];
}

// --- The costume filter ---

// @0x0214a2d0
void ItemTradeCostumeFilter::Init(int plane) {
    for (int i = 0; i < 5; i++) {
        Sprite_Init(&mButtons[i], plane, sCostumeFilterSequences[i], 128, 96, TRUE);
        func_02031278(&mButtons[i], sCostumeFilterSequences[i], sCostumeFilterOnSequences[i]);
        mButtons[i].SetPosition(
            FxVec2(sCostumeFilterPositions[i].x << FX32_SHIFT, sCostumeFilterPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], 42, 24);
        func_0203119c(&mButtons[i]);
        func_02031260(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mSoundArc = 1;
        mButtons[i].mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

// @0x0214a3d8
void ItemTradeCostumeFilter::Update() {
    mPressed = -1;
    for (int i = 0; i < 5; i++) {
        if (func_0203133c(&mButtons[i])) {
            func_02031e28(&mButtons[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

// @0x0214a438
void ItemTradeCostumeFilter::Draw() {
    for (int i = 0; i < 5; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

// @0x0214a460
void ItemTradeCostumeFilter::SetVisible(BOOL visible) {
    for (int i = 0; i < 5; i++) {
        mButtons[i].mVisible = visible;
    }
}

// @0x0214a47c
void ItemTradeCostumeFilter::Select(int button) {
    for (int i = 0; i < 5; i++) {
        func_02031e28(&mButtons[i], FALSE);
    }
    func_02031e28(&mButtons[button], TRUE);
    mSelected = button;
}

// @0x0214a4cc
int ItemTradeCostumeFilter::GetFilter() {
    return sCostumeFilters[mSelected];
}

// --- The list ---

static inline void ClearText(MessageText *text) {
    NNS_G2dCharCanvasClear(&text->mCanvas, 0);
}

// @0x0214a4e0
void ItemTradeList::Init(ItemTradeContext *context, int *charOffset) {
    mContext = context;
    mCostumes = context->mCostumes;
    mInventory = context->mInventory;
    mInteriors = context->mInteriors;
    mMannequin = context->mMannequin;
    mItemModel = mContext->mItemModel;
    mRoom = context->mRoom;
    mSelection.Set(0, 0);

    func_020a8170(&mList);
    func_020a8180(&mList, 0x100, 0x38);
    func_020278f8(&data_021213b8, 0x5FDF, 0x100);
    func_020a842c(&mList, 1, 8, 9, charOffset);
    func_020a8648(&mList, charOffset);
    func_020a8864(&mList, 1, 2, 3);
    func_020a8938(&mList, 1, 4, 5);
    func_020a8a38(&mList, 1, 6, 7);
    func_020a8ab0(&mList, 2, 6, 7);
    func_020a9b10(&mList, context->mItemMessages);
    func_020a9810(&mList, 2);
    mList.SetVisible(FALSE);

    mTabs.Init(1, context->mCommonMessages, charOffset);
    mItemFilter.Init(1);
    mCostumeFilter.Init(1);

    Sprite_Init(&mOkButton, 1, 10, 128, 96, TRUE);
    func_02031278(&mOkButton, 10, 11);
    mOkButton.SetPositionFromVec(0x80000, 0xAD000);
    func_02031eb0(&mOkButton, 72, 30);
    *charOffset += func_02031f58(&mOkButton, context->mTroubleMessages, 0x1DC, &gFonts[2].mFont, 0x1000, *charOffset);
    func_0203119c(&mOkButton);
    func_02031260(&mOkButton, 4);
    mOkButton.mSoundArc = 1;
    mOkButton.mSounds[0] = 10;
    mOkButton.mSounds[2] = 0;
    func_02032134(&mOkButton, 1);
    mOkButton.mVisible = FALSE;

    Sprite_Init(&mBackButton, 1, 0, 128, 96, TRUE);
    func_02031278(&mBackButton, 0, 1);
    mBackButton.SetPositionFromVec(0xF2000, 0xD000);
    func_02031eb0(&mBackButton, 24, 22);
    func_0203119c(&mBackButton);
    func_02031260(&mBackButton, 4);
    mBackButton.mSoundArc = 1;
    mBackButton.mSounds[0] = 12;
    mBackButton.mSounds[2] = 0;
    mBackButton.mOverwritePalette = 1;
    mBackButton.mVisible = FALSE;

    mRoomAngle = 0;
    SetRoomMode(0);
    for (int i = 0; i < 3; i++) {
        mPreviewPositions[i] = sPreviewPositions[i];
        mPreviewOffsets[i] = 0;
        mPreviewStates[i] = 0;
        mPreviewTimers[i] = 0;
    }
    mPendingId = -1;
    mPendingIndex = -1;
}

// @0x0214a864
int ItemTradeList::Update() {
    int result = RESULT_NONE;
    int state = mState;
    mLastState = state;
    switch (state) {
    case STATE_IDLE:
        break;
    case STATE_OPEN:
        Open();
        break;
    case STATE_RUN:
        result = Run();
        break;
    case STATE_CLOSE_REQUEST:
        mState = STATE_CLOSE;
        break;
    case STATE_CLOSE:
        Close();
        break;
    }
    UpdatePreviews();
    return result;
}

// @0x0214a8cc
void ItemTradeList::UpdateListMode() {
    if (*mContext->mSceneState == 7) {
        func_020a9e88(&mList, TRUE);
    } else {
        func_020a9e88(&mList, FALSE);
    }
}

// @0x0214a904
void ItemTradeList::Hide() {
    func_020a9e88(&mList, FALSE);
    func_020a8adc(&mList);
}

// @0x0214a924
void ItemTradeList::UpdatePreviews() {
    UpdateRoom();
    UpdatePreviewStates();
}

// @0x0214a93c
void ItemTradeList::Open() {
    mItemModel = mContext->mItemModel;
    func_020278f8(&data_021213b8, 0x5FDF, 0x100);
    switch (mMode) {
    case 0:
    case 1:
        mBackButton.mVisible = TRUE;
        mTabs.SetMasks(mInventory, mCostumes, mInteriors);
        mTabs.SetVisible(TRUE);
        mTabs.SetEnabled(0x1F);
        mTabs.Select(ItemTradeTabs::TAB_ITEMS);
        mTabs.func_ov110_0214a0bc();
        mItemFilter.SetVisible(TRUE);
        mItemFilter.Select(0);
        mCostumeFilter.SetVisible(FALSE);
        mCostumeFilter.Select(0);
        func_0209f0f0(mInventory, mItems, &mItemCount, 1, 1);
        mOkButton.mVisible = TRUE;
        func_02031e88(&mOkButton, TRUE);
        ClearText(&mOkButton.mText);
        mOkButton.mText.mMessage = 0x1DC;
        mOkButton.mText.DrawAll();
        func_020a9810(&mList, 2);
        func_020a9b10(&mList, mContext->mItemMessages);
        mList.SetVisible(TRUE);
        func_020a9b40(&mList, mItems, mItemCount);
        mList.mUnknown024 = TRUE;
        mSelection.Set(0, 0);
        break;
    case 2:
        mBackButton.mVisible = TRUE;
        mTabs.SetVisible(FALSE);
        mItemFilter.SetVisible(FALSE);
        mCostumeFilter.SetVisible(TRUE);
        mCostumeFilter.Select(0);
        mOkButton.mVisible = TRUE;
        func_02031e88(&mOkButton, TRUE);
        ClearText(&mOkButton.mText);
        mOkButton.mText.mMessage = 0x1DF;
        mOkButton.mText.DrawAll();
        func_020a9810(&mList, 0);
        func_020a9b10(&mList, mContext->mCostumeMessages);
        mList.SetVisible(TRUE);
        func_020a9b40(&mList, mItems, mItemCount);
        mList.mUnknown024 = TRUE;
        mSelection.Set(1, 0);
        break;
    }
    mState = STATE_RUN;
}

// @0x0214ac10
int ItemTradeList::Run() {
    int result = RESULT_NONE;
    mTabs.Update();
    if (mTabs.mPressed != 0) {
        switch (mTabs.mPressed) {
        case ItemTradeTabs::TAB_ITEMS:
            mItemFilter.SetVisible(TRUE);
            mItemFilter.Select(0);
            mCostumeFilter.SetVisible(FALSE);
            func_0209f0f0(mInventory, mItems, &mItemCount, 1, 1);
            func_020a9b10(&mList, mContext->mItemMessages);
            func_020a9b40(&mList, mItems, mItemCount);
            mSelection.Set(0, 0);
            break;
        case ItemTradeTabs::TAB_ITEMS_2:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            func_0209f208(mInventory, mItems, &mItemCount);
            func_020a9b10(&mList, mContext->mItemMessages);
            func_020a9b40(&mList, mItems, mItemCount);
            mSelection.Set(0, 0);
            break;
        case ItemTradeTabs::TAB_COSTUMES:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(TRUE);
            mCostumeFilter.Select(0);
            func_0209e4ac(mCostumes, mItems, &mItemCount, 0);
            func_020a9b10(&mList, mContext->mCostumeMessages);
            func_020a9b40(&mList, mItems, mItemCount);
            mSelection.Set(1, 0);
            break;
        case ItemTradeTabs::TAB_INTERIORS:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            func_020a5174(mInteriors, mItems, &mItemCount);
            func_020a9b10(&mList, mContext->mRoomMessages);
            func_020a9b40(&mList, mItems, mItemCount);
            mSelection.Set(2, 0);
            break;
        case ItemTradeTabs::TAB_ITEMS_3:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            func_0209f28c(mInventory, mItems, &mItemCount);
            func_020a9b10(&mList, mContext->mItemMessages);
            func_020a9b40(&mList, mItems, mItemCount);
            mSelection.Set(0, 0);
            break;
        }
        mPendingIndex = -1;
        func_02031e88(&mOkButton, TRUE);
    }

    mItemFilter.Update();
    if (mItemFilter.mPressed != -1) {
        func_0209f0f0(mInventory, mItems, &mItemCount, 1, mItemFilter.GetFilter());
        func_020a9b40(&mList, mItems, mItemCount);
        mPendingIndex = -1;
        func_02031e88(&mOkButton, TRUE);
    }

    mCostumeFilter.Update();
    if (mCostumeFilter.mPressed != -1) {
        switch (mMode) {
        case 0:
        case 1:
            func_0209e4ac(mCostumes, mItems, &mItemCount, mCostumeFilter.GetFilter());
            func_020a9b40(&mList, mItems, mItemCount);
            break;
        case 2:
            func_020a9b40(&mList, mItems, mItemCount);
            break;
        }
        mPendingIndex = -1;
        func_02031e88(&mOkButton, TRUE);
    }

    func_020a8adc(&mList);
    if (mList.mTouched && mList.mSelected != -1) {
        InventoryItem *item = mList.mItems[mList.mSelected];
        int id = item->mId;
        func_02031e88(&mOkButton, item->mCount <= 0);
        mPendingIndex = mList.mSelected;
        switch (mSelection.mKind) {
        case PREVIEW_ITEM:
            SetItemCamera();
            mItemModel->ChangeItem(id);
            mItemModel->mVisible = TRUE;
            ShowName(mSelection.mKind, id);
            func_020a03bc(mMannequin, FALSE);
            func_020a4b74(&mRoom->mParts, FALSE);
            mSelection.Set(0, id);
            ResetLights();
            break;
        case PREVIEW_COSTUME:
            SetItemCamera();
            ShowCostume(id);
            LoadMannequinHead();
            ShowName(mSelection.mKind, id);
            func_020a03bc(mMannequin, TRUE);
            mItemModel->mVisible = FALSE;
            func_020a4b74(&mRoom->mParts, FALSE);
            mSelection.Set(1, id);
            ResetLights();
            break;
        case PREVIEW_ROOM:
            SetRoomCamera();
            mItemModel->mVisible = FALSE;
            func_020a03bc(mMannequin, FALSE);
            func_020a4b74(&mRoom->mParts, FALSE);
            ShowRoom(id);
            mSelection.Set(2, id);
            ShowName(mSelection.mKind, id);
            break;
        }
    }

    if (func_0203133c(&mOkButton)) {
        if (mList.mSelected == -1) {
            mChosen = FALSE;
        } else {
            result = RESULT_OK;
            mChosenId = mList.mItems[mList.mSelected]->mId;
            mChosen = TRUE;
        }
    }
    if (func_0203133c(&mBackButton)) {
        result = RESULT_BACK;
        mChosen = FALSE;
    }
    return result;
}

// @0x0214b224
void ItemTradeList::Close() {
    mItemModel->Reset();
    mItemModel = NULL;
    func_020a03bc(mMannequin, FALSE);
    func_020a4b74(&mRoom->mParts, FALSE);
    mOkButton.mVisible = FALSE;
    mBackButton.mVisible = FALSE;
    mTabs.SetVisible(FALSE);
    mItemFilter.SetVisible(FALSE);
    mCostumeFilter.SetVisible(FALSE);
    mList.SetVisible(FALSE);
    mState = STATE_IDLE;
}

// @0x0214b2bc
void ItemTradeList::Draw() {
    mList.Draw();
    func_0203206c(&mOkButton);
    func_02031d7c(&mBackButton);
    mTabs.Draw();
    mItemFilter.Draw();
    mCostumeFilter.Draw();
}

// Dresses the mannequin in a costume
// @0x0214b314
void ItemTradeList::ShowCostume(int costume) {
    func_0202aecc(data_021223e4, 0);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    func_0208ee18(&mMannequin->mModel, "3MM", costume, func_0207a710(mContext->mPlayer));
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin->mArchivePath, 1);
    ModelResources_SetModel(data_021223e4, 0, mMannequin->mModelPath);
    func_0208ef5c(&mMannequin->mModel, 0, 0);
    func_0208f19c(&mMannequin->mModel, func_02073514(func_0207a6c0(mContext->mPlayer)), 0x5FBE);
    func_0208ef00(&mMannequin->mModel, TRUE, TRUE);
}

// @0x0214b414
void ItemTradeList::LoadMannequinHead() {
    func_0202aecc(data_021223e4, 1);
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ArchiveManager_MountCompressed(&gArchiveManager, "3MH", "Mdl/Misc/headManikin_3D_LZ.bin", 1);
    ModelResources_SetModel(data_021223e4, 1, "3MH:/head.imd");
}

// @0x0214b470
void ItemTradeList::Save() {
    mInventory->Save();
    func_0209e310(mCostumes);
    func_020a5008(mInteriors);
}

// Takes one of the item chosen away (it has been sent)
// @0x0214b494
void ItemTradeList::RemoveOne() {
    mList.mItems[mList.mSelected]->Add(-1);
    RefreshList();
    func_020a9bf4(&mList, mPendingIndex);
    func_020a9de4(&mList);
    func_02031e88(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
}

// Gives one of the item chosen back
// @0x0214b4fc
void ItemTradeList::AddOne() {
    mList.mItems[mList.mSelected]->Add(1);
    RefreshList();
    func_020a9bf4(&mList, mPendingIndex);
    func_020a9de4(&mList);
    func_02031e88(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
}

// Adds what came: FALSE when there are 99 of it already
// @0x0214b564
BOOL ItemTradeList::Receive(int id) {
    BOOL received = TRUE;
    if (id >= 0x1000) {
        InventoryItem *item = &mInteriors->mItems[id & 0xFFF];
        if (item->mCount >= 99) {
            received = FALSE;
        } else {
            item->Add(1);
        }
    } else if (id >= 0x800) {
        InventoryItem *item = &mCostumes->mItems[id & 0x7FF];
        if (item->mCount >= 99) {
            received = FALSE;
        } else {
            item->Add(1);
        }
    } else {
        InventoryItem *item = &mInventory->mItems[id];
        if (item->mCount >= 99) {
            received = FALSE;
        } else {
            item->Add(1);
        }
    }

    u16 selected = 0;
    if (mPendingIndex != -1) {
        selected = mList.mItems[mList.mSelected]->mId;
    }
    RefreshList();
    if (mPendingIndex != -1) {
        mPendingIndex = func_020a9e28(&mList, selected);
        func_020a9bf4(&mList, mPendingIndex);
        func_020a9de4(&mList);
        func_02031e88(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
    }
    return received;
}

// @0x0214b6ac
void ItemTradeList::RefreshList() {
    switch (mTabs.mSelected) {
    case 0:
        func_0209f0f0(mInventory, mItems, &mItemCount, 1, mItemFilter.GetFilter());
        break;
    case 1:
        func_0209e4ac(mCostumes, mItems, &mItemCount, mCostumeFilter.GetFilter());
        break;
    case 2:
        func_020a5174(mInteriors, mItems, &mItemCount);
        break;
    case 3:
        func_0209f208(mInventory, mItems, &mItemCount);
        break;
    case 4:
        func_0209f28c(mInventory, mItems, &mItemCount);
        break;
    }
    func_020a9b40(&mList, mItems, mItemCount);
    mTabs.SetMasks(mInventory, mCostumes, mInteriors);
    mTabs.SetVisible(TRUE);
}

// Shows the name of the thing chosen on the upper screen
// @0x0214b7b8
void ItemTradeList::ShowName(int kind, int id) {
    mContext->mNameText->mSprite.mVisible = TRUE;
    switch (kind) {
    case PREVIEW_ITEM:
        mContext->mNameText->mMessage = id;
        break;
    case PREVIEW_COSTUME:
        mContext->mNameText->mMessage = id + 0xFF;
        break;
    case PREVIEW_ROOM:
        mContext->mNameText->mMessage = id + 0x22B;
        break;
    default:
        return;
    }
    ClearText(mContext->mNameText);
    mContext->mNameText->DrawAll();
}

// @0x0214b864
void ItemTradeList::SetRoomMode(int mode) {
    mRoomMode = mode;
    mRoomStep = 0;
}

// @0x0214b880
void ItemTradeList::ShowRoom(u16 style) {
    mRoomStyle = style;
    SetRoomMode(1);
}

// @0x0214b89c
void ItemTradeList::UpdateRoom() {
    switch (mRoomMode) {
    case 0:
        mRoomAngle += 0x6D;
        mRoom->mParts.mDoor.mModel.mRotX = 0;
        mRoom->mParts.mDoor.mModel.mRotY = mRoomAngle;
        break;
    case 1:
        switch (mRoomStep) {
        case 0:
            mRoomStep++;
            func_020a4b74(&mRoom->mParts, FALSE);
            func_020a5948(mRoom);
            break;
        case 1:
            mRoomStep++;
            mRoom->mStyle = mRoomStyle;
            func_020a54e4(mRoom, 0);
            func_020a56dc(mRoom, 0);
            break;
        case 2:
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mRoomStep++;
            }
            break;
        case 3:
            mRoomStep++;
            func_020a5530(mRoom, 1);
            break;
        case 4:
            mRoomStep++;
            func_020a5854(mRoom, 2);
            break;
        case 5:
            SetRoomMode(0);
            func_020a4b74(&mRoom->mParts, TRUE);
            data_021226e4.mLights[0].mColor = data_020f72f8[mRoom->mStyle].mLightColor;
            data_021226e4.mLights[1].mColor = data_020f72f8[mRoom->mStyle].mLightColor;
            break;
        }
        break;
    }
}

static inline void ResetCamera(Camera *camera) {
    func_0202a640(camera);
}

static inline void LookAt(Camera *camera, fx32 y, fx32 z, fx32 targetY) {
    VEC_Set(&camera->mPosition, 0, y, z);
    VEC_Set(&camera->mTarget, 0, targetY, 0);
}

// @0x0214ba40
void ItemTradeList::SetItemCamera() {
    Camera *camera = &data_02122764;
    func_0202a640(camera);
    LookAt(camera, 0x599, 0x4666, 0x599);
    func_0202a748(camera);
}

// @0x0214ba8c
void ItemTradeList::SetRoomCamera() {
    LookAt(&data_02122764, 0x2666, 0x5000, 0x4CC);
}

// @0x0214bac4
void ItemTradeList::ResetLights() {
    data_021226e4.mLights[0].mColor = sLightColors[0];
    data_021226e4.mLights[1].mColor = sLightColors[1];
}

// @0x0214bae8
void ItemTradeList::func_ov110_0214bae8() {
    func_020a9e6c(&mList);
}

static inline void SetTextVisible(MessageText *text, BOOL visible) {
    text->mSprite.mVisible = visible;
}

static inline BOOL HasSelection(ItemListView *list) {
    return list->mSelected != -1;
}

// The three views of the upper screen: each slides in when it is chosen, bounces, and fades out when another is
// @0x0214baf8
void ItemTradeList::UpdatePreviewStates() {
    int i;
    switch (*mContext->mSceneState) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        break;
    case 10:
    case 11: {
        BOOL hidden = FALSE;
        for (i = 0; i < 3; i++) {
            if (mPreviewStates[i] == 0 || mPreviewStates[i] == 2 || mPreviewStates[i] == 6) {
                hidden = TRUE;
                mPreviewStates[i] = 1;
            }
        }
        if (hidden == TRUE) {
            if (HasSelection(&mList)) {
                mPendingId = mList.mItems[mList.mSelected]->mId;
                mPendingIndex = mList.mSelected;
                mContext->mNameText->mSprite.mVisible = FALSE;
            } else {
                mPendingId = -1;
            }
        }
        break;
    }
    case 14:
        if (sComm.mItem >= 0x1000 && mPreviewStates[PREVIEW_ROOM] == 1) {
            SetRoomCamera();
            func_020a4b74(&mRoom->mParts, TRUE);
            mPreviewStates[PREVIEW_ROOM] = 3;
            mPreviewStates[PREVIEW_ITEM] = 2;
            mPreviewStates[PREVIEW_COSTUME] = 2;
            mPreviewPositions[PREVIEW_ROOM].y = sPreviewStarts[PREVIEW_ROOM].y;
            mPreviewOffsets[PREVIEW_ROOM] = 0x1CCC;
            int style = sComm.mItem - 0x1000;
            ShowRoom(style);
            UpdateRoom();
            ShowName(PREVIEW_ROOM, style);
        } else if (sComm.mItem >= 0x800 && mPreviewStates[PREVIEW_COSTUME] == 1) {
            SetItemCamera();
            func_020a03bc(mMannequin, TRUE);
            mPreviewStates[PREVIEW_COSTUME] = 3;
            mPreviewStates[PREVIEW_ITEM] = 2;
            mPreviewStates[PREVIEW_ROOM] = 2;
            mPreviewPositions[PREVIEW_COSTUME].y = sPreviewStarts[PREVIEW_COSTUME].y;
            mPreviewOffsets[PREVIEW_COSTUME] = 0x1CCC;
            int costume = sComm.mItem - 0x800;
            ShowCostume(costume);
            LoadMannequinHead();
            ShowName(PREVIEW_COSTUME, costume);
        } else if (mPreviewStates[PREVIEW_ITEM] == 1) {
            SetItemCamera();
            mItemModel->SetVisible(TRUE);
            mPreviewStates[PREVIEW_ITEM] = 3;
            mPreviewStates[PREVIEW_ROOM] = 2;
            mPreviewStates[PREVIEW_COSTUME] = 2;
            mPreviewPositions[PREVIEW_ITEM].y = sPreviewStarts[PREVIEW_ITEM].y;
            mPreviewOffsets[PREVIEW_ITEM] = 0x1CCC;
            mItemModel->ChangeItem(sComm.mItem);
            ShowName(PREVIEW_ITEM, sComm.mItem);
        }
        break;
    case 15:
        for (i = 0; i < 3; i++) {
            mPreviewStates[i] = 6;
        }
        break;
    }

    if (mRoomMode == 1) {
        return;
    }

    for (i = 0; i < 3; i++) {
        switch (mPreviewStates[i]) {
        case 0:
            if (mPreviewOffsets[i] < 0) {
                mPreviewOffsets[i] += 0x199;
            }
            if (mPreviewOffsets[i] >= 0) {
                mPreviewOffsets[i] = 0;
            }
            break;
        case 1:
        case 2:
            if (i == PREVIEW_ROOM) {
                if (mPreviewOffsets[i] <= -0x34CC) {
                    mPreviewOffsets[i] = -0x34CC;
                } else {
                    mPreviewOffsets[i] -= 0x199;
                }
            } else {
                if (mPreviewOffsets[i] <= -0x1CCC) {
                    mPreviewOffsets[i] = -0x1CCC;
                } else {
                    mPreviewOffsets[i] -= 0x199;
                }
            }
            if (*mContext->mSceneState == 7) {
                int j;
                BOOL busy = FALSE;
                for (j = 0; j < 3; j++) {
                    if (i != j && (mPreviewStates[j] == 5 || mPreviewStates[j] == 4)) {
                        busy = TRUE;
                        break;
                    }
                }
                if (!busy) {
                    mPreviewStates[i] = 0;
                }
            }
            break;
        case 3:
            if (mPreviewOffsets[i] > 0) {
                mPreviewOffsets[i] -= 0x199;
            }
            if (mPreviewOffsets[i] <= 0) {
                mPreviewOffsets[i] = 0;
                mPreviewStates[i] = 4;
                mPreviewTimers[i] = 0;
                PlaySound(8);
            }
            break;
        case 4:
            if (mPreviewTimers[i] < 30) {
                mPreviewOffsets[i] -= (mPreviewTimers[i] - 15) << 4;
                mPreviewTimers[i]++;
                if (mPreviewTimers[i] == 30) {
                    PlaySound(8);
                }
            } else {
                mPreviewOffsets[i] = 0;
                if (*mContext->mSceneState != 14) {
                    mPreviewStates[i] = 5;
                    mPreviewTimers[i] = 0;
                }
            }
            break;
        case 5:
            mPreviewTimers[i] = 15;
            switch (i) {
            case PREVIEW_ITEM:
                func_02029f50(mItemModel, 31 - mPreviewTimers[i] * 2);
                break;
            case PREVIEW_COSTUME:
                func_020a03c8(mMannequin, 31 - mPreviewTimers[i] * 2);
                break;
            case PREVIEW_ROOM:
                func_020a4bc4(&mRoom->mParts, 31 - mPreviewTimers[i] * 2);
                func_020a5960(mRoom);
                break;
            }
            if (mPreviewTimers[i] >= 15) {
                func_020a4b74(&mRoom->mParts, FALSE);
                func_020a03bc(mMannequin, FALSE);
                mItemModel->SetVisible(FALSE);
                func_020a4bc4(&mRoom->mParts, 31);
                func_020a03c8(mMannequin, 31);
                func_02029f50(mItemModel, 31);
                mPreviewStates[i] = 0;
                if (i == PREVIEW_ROOM) {
                    mPreviewOffsets[i] = -0x34CC;
                } else {
                    mPreviewOffsets[i] = -0x1CCC;
                }
                if (mPendingId != -1) {
                    ShowName(mSelection.mKind, mPendingId);
                } else {
                    SetTextVisible(mContext->mNameText, FALSE);
                }
                if (*mContext->mSceneState == 10) {
                    mPreviewStates[i] = 1;
                } else {
                    ShowPending();
                }
            }
            break;
        case 6:
            if (*mContext->mSceneState == 7) {
                mPreviewStates[i] = 0;
                if (i == 0) {
                    ShowPending();
                }
            }
            break;
        }
    }

    mItemModel->mPosition.y = mPreviewOffsets[PREVIEW_ITEM];
    mMannequin->mModel.mPos.y = mPreviewOffsets[PREVIEW_COSTUME] - 0x599;
    mRoom->mParts.mUnknown008 = mPreviewOffsets[PREVIEW_ROOM] + 0xCC;
}

// Shows what was chosen while the views were busy
// @0x0214c21c
void ItemTradeList::ShowPending() {
    switch (mSelection.mKind) {
    case PREVIEW_ROOM:
        if (mPendingId == -1) {
            return;
        }
        SetRoomCamera();
        func_020a4b74(&mRoom->mParts, TRUE);
        ShowRoom(mPendingId);
        UpdateRoom();
        func_020a9bf4(&mList, mPendingIndex);
        break;
    case PREVIEW_COSTUME:
        if (mPendingId == -1) {
            return;
        }
        SetItemCamera();
        func_020a03bc(mMannequin, TRUE);
        ShowCostume(mPendingId);
        LoadMannequinHead();
        func_020a9bf4(&mList, mPendingIndex);
        break;
    case PREVIEW_ITEM:
        if (mPendingId == -1) {
            return;
        }
        SetItemCamera();
        mItemModel->mVisible = TRUE;
        mItemModel->ChangeItem(mPendingId);
        func_020a9bf4(&mList, mPendingIndex);
        break;
    }
}

// --- The item model ---

// @0x0214c33c
void ItemTradeItemModel::Load(int resourceId) {
    LoadModel(resourceId);
    mUnknown12C = 0;
}

// @0x0214c354
void ItemTradeItemModel::Reset() {
    mVisible = FALSE;
    mPosition.x = 0x7FFFFFFF;
    mPosition.y = 0x7FFFFFFF;
    mPosition.z = 0x7FFFFFFF;
    mScale.x = FX32_ONE;
    mScale.y = FX32_ONE;
    mScale.z = FX32_ONE;
    mUnknown12C = 0;
}

// Shows another item: lets go of the old one's files, and loads the new one's
// @0x0214c384
void ItemTradeItemModel::ChangeItem(int item) {
    func_0202aecc(data_021223e4, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(data_021223e4, mResourceId, mModelPath);
    LoadAnimation(mResourceId);
    ResetScale();
}
