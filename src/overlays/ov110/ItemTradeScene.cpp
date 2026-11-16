#include "ItemTradeScene.h"
#include "MainProgram.h"

// Overlay 110: wireless play with items (ItemTradeScene). One source file: the list of the player's things with its
// tabs and buttons, the scene, and the scene's implicit destructor, which the compiler emits with the scene's
// virtual table, at the end of .text.

// --- The file's constants ---
//
// The compiler pools a file's constants under 64 bytes into one .rodata section sorted by size, and gives objects
// of one size their places by where they are declared; in this order they come out at the ROM's addresses (the
// 1-aligned arrays and the tables of 64 bytes and more get sections of their own, also in the ROM's order). Reads
// of these with a constant index are folded into the code as immediates: the touch sizes, the buttons' positions
// and the camera vectors are in the ROM though no instruction loads them.
//
// What they are: the tabs' and filters' animation sequences (off and on), positions and touch sizes; the upper
// screen's views, where each rests and where it comes in from; the OK and back buttons; the item and room cameras;
// and the message window's rectangles, buttons, texts and sounds (used by ItemTradeScene::SetupWindow).

static const u16 sTabSequences[5] = { 0x2B, 0x2D, 0x2F, 0x31, 0x29 };
static const u16 sTabOnSequences[5] = { 0x2C, 0x2E, 0x30, 0x32, 0x2A };
static const WindowPoint sLinkIconPosition = { 244, 182 };
static const ScreenPoint sTabPositions[5] = { { 23, 13 }, { 68, 13 }, { 113, 13 }, { 158, 13 }, { 203, 13 } };
static const WindowPoint sTabTouchSize = { 44, 22 };
static const WindowPoint sBackPosition = { 242, 13 };
static const WindowPoint sThreeButtonPositions[4] = { { 128, 72 }, { 128, 96 }, { 128, 120 }, { 128, 149 } };
static const int sButtonSounds[4] = { 10, -1, 0, -1 };
static const u16 sCostumeFilterOnSequences[5] = { 0x19, 0x1B, 0x1D, 0x1F, 0x21 };
static const u16 sCostumeFilterSequences[5] = { 0x18, 0x1A, 0x1C, 0x1E, 0x20 };
static const ScreenPoint sCostumeFilterPositions[5] = { { 26, 41 }, { 72, 41 }, { 118, 41 }, { 164, 41 }, { 210, 41 } };
static const int sCostumeFilters[5] = { 0, 1, 2, 3, 4 };
static const WindowPoint sButtonTextPositions[3] = { { 38, 68 }, { 38, 100 }, { 38, 118 } };
static const ScreenPoint sPreviewPositions[3] = { { 128, 134 }, { 190, 172 }, { 128, 128 } };
static const WindowPoint sItemFilterTouchSize = { 42, 24 };
static const u16 sItemFilterOnSequences[4] = { 0x26, 0x28, 0x15, 0x17 };
static const WindowPoint sOkSize = { 94, 30 };
static const WindowPoint sTextArea = { 24, 5 };
static const WindowPoint sOkPosition = { 128, 173 };
static const WindowPoint sButtonSize = { 190, 26 };
static const VecFx32 sItemCameraTarget = { 0, 0x599, 0 };
static const VecFx32 sRoomCameraPosition = { 0, 0x2666, 0x5000 };
static const VecFx32 sRoomCameraTarget = { 0, 0x4CC, 0 };
static const VecFx32 sItemCameraPosition = { 0, 0x599, 0x4666 };
static const VecFx32 sZero = { 0, 0, 0 };
static const WindowPoint sAnswerSizes[2] = { { 94, 30 }, { 94, 30 } };
static const ScreenRect sQuestionRect = { 20, 20, 216, 152 };
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
static const WindowPoint sBackTouchSize = { 24, 22 };
static const WindowPoint sTextPositions[3] = { { 32, 33 }, { 32, 78 }, { 32, 58 } }; // Buttons, none, OK alone
static const char sWindowScreenName[] = "LTL:/Window_common.nsc";
static const WindowPoint sButtonPositions[4] = { { 128, 74 }, { 128, 106 }, { 128, 124 }, { 128, 149 } };
static const WindowPoint sOkTouchSize = { 72, 30 };
static const WindowPoint sThreeButtonTextPositions[3] = { { 38, 66 }, { 38, 90 }, { 38, 114 } };
static const ScreenPoint sItemFilterPositions[4] = { { 28, 41 }, { 88, 41 }, { 148, 41 }, { 208, 41 } };
static const WindowPoint sNameTextPosition = { 8, 8 };
static const ScreenPoint sPreviewStarts[3] = { { 128, 0 }, { 190, 0 }, { 128, 0 } };
static const WindowPoint sCostumeFilterTouchSize = { 42, 24 };
static const u8 sAnswerOnSequences[2] = { 11, 11 };
static const u8 sAnswerSequences[2] = { 10, 10 };
static const int sItemFilters[4] = { 1, 2, 8, 0x10 };
static const WindowPoint sAnswerPositions[2] = { { 75, 149 }, { 181, 149 } };
static const WindowPoint sThreeButtonSize = { 190, 20 };
static const int sAnswerSounds[2][4] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 } };
static const u8 sLinkSequences[4] = { 0, 1, 2, 3 };
static const int sAnswerMessages[2] = { 2, 3 };
static const u16 sItemFilterSequences[4] = { 0x25, 0x27, 0x14, 0x16 };
static const VecFx32 sRoomPosition = { 0, 0x333, 0 };
static const VecFx32 sFarAway = { 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF };
// The lights' colours for an item or a costume (a room has its own)
static GXRgb sLightColors[2] = { GX_RGB(31, 31, 31), GX_RGB(31, 31, 31) };

// What the two DS send each other. The buffers WH sends from are 32-byte aligned.
static u8 sDisconnected;     // The receiver was called without data: the partner has gone
static u16 sCommand ATTRIBUTE_ALIGN(32); // What Send sends with a command
static u16 sReceived;        // An item has come
static u16 sSending;         // An item is being sent
static u16 sReceivedCommand; // The command the other DS sent
static int sItem ATTRIBUTE_ALIGN(32); // The item sent or received: an item, costume + 0x800, or interior + 0x1000
static BOOL sReceivedNew;
static BOOL sSent;
// The partners found, and how long each is still shown
static int sPartnerTimers[3];
static u8 sPartners[0x240];



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
        InventoryItem::mId = id;
        mValue = id;
        {
            u16 item = InventoryItem::mId;
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
        InventoryItem::mId = id;
        mCategory = 1;
        mValue = 0xF7;
        break;
    case 2:
        InventoryItem::mId = id;
        mCategory = 2;
        mValue = 0xF9;
        break;
    case 3:
        InventoryItem::mId = id;
        mCategory = 3;
        mValue = 0xF4;
        break;
    case 4:
        InventoryItem::mId = id;
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
        func_02031eb0(&mButtons[i], sTabTouchSize.x, sTabTouchSize.y);
        func_0203119c(&mButtons[i]);
        func_02031260(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = 1;
        mButtons[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
}

// @0x02149c88
void ItemTradeTabs::SetMasks(Inventory *inventory, ClothesList *costumes, InteriorList *interiors) {
    mMasks[0] = inventory->HasAnyOwnedKind1();
    mMasks[3] = inventory->HasAnyOwnedKind3();
    mMasks[1] = costumes->HasAnyOwned();
    mMasks[2] = interiors->HasAnyOwned();
    mMasks[4] = inventory->HasAnyOwnedKinds4And5();
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

// Empty; called once, by ItemTradeList::Open, after the tabs are set up
// @0x0214a0bc
void ItemTradeTabs::OnOpen() {
}

// --- The item filter ---

// @0x0214a0c0
void ItemTradeItemFilter::Init(int plane) {
    for (int i = 0; i < 4; i++) {
        Sprite_Init(&mButtons[i], plane, sItemFilterSequences[i], 128, 96, TRUE);
        func_02031278(&mButtons[i], sItemFilterSequences[i], sItemFilterOnSequences[i]);
        mButtons[i].SetPosition(
            FxVec2(sItemFilterPositions[i].x << FX32_SHIFT, sItemFilterPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], sItemFilterTouchSize.x, sItemFilterTouchSize.y);
        func_0203119c(&mButtons[i]);
        func_02031260(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = 1;
        mButtons[i].mSounds.mSounds[0] = 3;
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
        func_02031eb0(&mButtons[i], sCostumeFilterTouchSize.x, sCostumeFilterTouchSize.y);
        func_0203119c(&mButtons[i]);
        func_02031260(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = 1;
        mButtons[i].mSounds.mSounds[0] = 3;
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

    mList.Init();
    mList.SetBackground(0x100, 0x38);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x5FDF, 0x100);
    mList.InitRows(1, 8, 9, charOffset);
    mList.InitCounts(charOffset);
    mList.InitUpButton(1, 2, 3);
    mList.InitDownButton(1, 4, 5);
    mList.InitScrollBar(1, 6, 7);
    mList.SetUpScrollBar(2, 6, 7);
    mList.SetMessageFile(context->mItemMessages);
    mList.SetMode(2);
    mList.SetVisible(FALSE);

    mTabs.Init(1, context->mCommonMessages, charOffset);
    mItemFilter.Init(1);
    mCostumeFilter.Init(1);

    Sprite_Init(&mOkButton, 1, 10, 128, 96, TRUE);
    func_02031278(&mOkButton, 10, 11);
    mOkButton.SetPositionFromVec(sOkPosition.x << FX32_SHIFT, sOkPosition.y << FX32_SHIFT);
    func_02031eb0(&mOkButton, sOkTouchSize.x, sOkTouchSize.y);
    *charOffset += func_02031f58(&mOkButton, context->mTroubleMessages, 0x1DC, &gFonts[2].mFont, 0x1000, *charOffset);
    func_0203119c(&mOkButton);
    func_02031260(&mOkButton, 4);
    mOkButton.mUnknownDC = 1;
    mOkButton.mSounds.mSounds[0] = 10;
    mOkButton.mSounds.mSounds[2] = 0;
    func_02032134(&mOkButton, 1);
    mOkButton.mVisible = FALSE;

    Sprite_Init(&mBackButton, 1, 0, 128, 96, TRUE);
    func_02031278(&mBackButton, 0, 1);
    mBackButton.SetPositionFromVec(sBackPosition.x << FX32_SHIFT, sBackPosition.y << FX32_SHIFT);
    func_02031eb0(&mBackButton, sBackTouchSize.x, sBackTouchSize.y);
    func_0203119c(&mBackButton);
    func_02031260(&mBackButton, 4);
    mBackButton.mUnknownDC = 1;
    mBackButton.mSounds.mSounds[0] = 12;
    mBackButton.mSounds.mSounds[2] = 0;
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
        mList.SetUnknown308(TRUE);
    } else {
        mList.SetUnknown308(FALSE);
    }
}

// @0x0214a904
void ItemTradeList::Hide() {
    mList.SetUnknown308(FALSE);
    mList.Update();
}

// @0x0214a924
void ItemTradeList::UpdatePreviews() {
    UpdateRoom();
    UpdatePreviewStates();
}

// @0x0214a93c
void ItemTradeList::Open() {
    mItemModel = mContext->mItemModel;
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x5FDF, 0x100);
    switch (mMode) {
    case 0:
    case 1:
        mBackButton.mVisible = TRUE;
        mTabs.SetMasks(mInventory, mCostumes, mInteriors);
        mTabs.SetVisible(TRUE);
        mTabs.SetEnabled(0x1F);
        mTabs.Select(ItemTradeTabs::TAB_ITEMS);
        mTabs.OnOpen();
        mItemFilter.SetVisible(TRUE);
        mItemFilter.Select(0);
        mCostumeFilter.SetVisible(FALSE);
        mCostumeFilter.Select(0);
        mInventory->GetOwned(mItems, &mItemCount, 1, 1);
        mOkButton.mVisible = TRUE;
        func_02031e88(&mOkButton, TRUE);
        ClearText(&mOkButton.mText);
        mOkButton.mText.mMessage = 0x1DC;
        mOkButton.mText.DrawAll();
        mList.SetMode(2);
        mList.SetMessageFile(mContext->mItemMessages);
        mList.SetVisible(TRUE);
        mList.SetItems(mItems, mItemCount);
        mList.mTouchable = TRUE;
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
        mList.SetMode(0);
        mList.SetMessageFile(mContext->mCostumeMessages);
        mList.SetVisible(TRUE);
        mList.SetItems(mItems, mItemCount);
        mList.mTouchable = TRUE;
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
            mInventory->GetOwned(mItems, &mItemCount, 1, 1);
            mList.SetMessageFile(mContext->mItemMessages);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(0, 0);
            break;
        case ItemTradeTabs::TAB_ITEMS_2:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            mInventory->GetOwnedKind3(mItems, &mItemCount);
            mList.SetMessageFile(mContext->mItemMessages);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(0, 0);
            break;
        case ItemTradeTabs::TAB_COSTUMES:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(TRUE);
            mCostumeFilter.Select(0);
            mCostumes->GetOwned(mItems, &mItemCount, 0);
            mList.SetMessageFile(mContext->mCostumeMessages);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(1, 0);
            break;
        case ItemTradeTabs::TAB_INTERIORS:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            mInteriors->GetOwned(mItems, &mItemCount);
            mList.SetMessageFile(mContext->mRoomMessages);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(2, 0);
            break;
        case ItemTradeTabs::TAB_ITEMS_3:
            mItemFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            mInventory->GetOwnedKinds4And5Again(mItems, &mItemCount);
            mList.SetMessageFile(mContext->mItemMessages);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(0, 0);
            break;
        }
        mPendingIndex = -1;
        func_02031e88(&mOkButton, TRUE);
    }

    mItemFilter.Update();
    if (mItemFilter.mPressed != -1) {
        mInventory->GetOwned(mItems, &mItemCount, 1, mItemFilter.GetFilter());
        mList.SetItems(mItems, mItemCount);
        mPendingIndex = -1;
        func_02031e88(&mOkButton, TRUE);
    }

    mCostumeFilter.Update();
    if (mCostumeFilter.mPressed != -1) {
        switch (mMode) {
        case 0:
        case 1:
            mCostumes->GetOwned(mItems, &mItemCount, mCostumeFilter.GetFilter());
            mList.SetItems(mItems, mItemCount);
            break;
        case 2:
            mList.SetItems(mItems, mItemCount);
            break;
        }
        mPendingIndex = -1;
        func_02031e88(&mOkButton, TRUE);
    }

    mList.Update();
    if (mList.mTouched && mList.mSelected != -1) {
        Item *item = mList.mItems[mList.mSelected];
        int id = item->mId;
        func_02031e88(&mOkButton, item->mCount <= 0);
        mPendingIndex = mList.mSelected;
        switch (mSelection.mKind) {
        case PREVIEW_ITEM:
            SetItemCamera();
            mItemModel->ChangeItem(id);
            mItemModel->mVisible = TRUE;
            ShowName(mSelection.mKind, id);
            mMannequin->SetVisible(FALSE);
            mRoom->RoomModels::SetVisible(FALSE);
            mSelection.Set(0, id);
            ResetLights();
            break;
        case PREVIEW_COSTUME:
            SetItemCamera();
            ShowCostume(id);
            LoadMannequinHead();
            ShowName(mSelection.mKind, id);
            mMannequin->SetVisible(TRUE);
            mItemModel->mVisible = FALSE;
            mRoom->RoomModels::SetVisible(FALSE);
            mSelection.Set(1, id);
            ResetLights();
            break;
        case PREVIEW_ROOM:
            SetRoomCamera();
            mItemModel->mVisible = FALSE;
            mMannequin->SetVisible(FALSE);
            mRoom->RoomModels::SetVisible(FALSE);
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
    mMannequin->SetVisible(FALSE);
    mRoom->RoomModels::SetVisible(FALSE);
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
    ModelResources_ReleaseModel(gModelResources, 0);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    mMannequin->SetArchive("3MM", costume, MiiData_GetSex(mContext->mPlayer));
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin->mArchivePath, 1);
    ModelResources_SetModel(gModelResources, 0, mMannequin->mModelPath);
    mMannequin->MakeModel(0, 0);
    mMannequin->SetPaletteColors(NFL_GetFavoriteColor(MiiData_GetFavoriteColor(mContext->mPlayer)), 0x5FBE);
    mMannequin->SetAnimFlags(TRUE, TRUE);
}

// @0x0214b414
void ItemTradeList::LoadMannequinHead() {
    ModelResources_ReleaseModel(gModelResources, 1);
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ArchiveManager_MountCompressed(&gArchiveManager, "3MH", "Mdl/Misc/headManikin_3D_LZ.bin", 1);
    ModelResources_SetModel(gModelResources, 1, "3MH:/head.imd");
}

// @0x0214b470
void ItemTradeList::Save() {
    mInventory->Save();
    mCostumes->Save();
    mInteriors->Save();
}

// Takes one of the item chosen away (it has been sent)
// @0x0214b494
void ItemTradeList::RemoveOne() {
    mList.mItems[mList.mSelected]->Add(-1);
    RefreshList();
    mList.Select(mPendingIndex);
    mList.ScrollToSelected();
    func_02031e88(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
}

// Gives one of the item chosen back
// @0x0214b4fc
void ItemTradeList::AddOne() {
    mList.mItems[mList.mSelected]->Add(1);
    RefreshList();
    mList.Select(mPendingIndex);
    mList.ScrollToSelected();
    func_02031e88(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
}

// Adds what came: FALSE when there are 99 of it already
// @0x0214b564
BOOL ItemTradeList::Receive(int id) {
    BOOL received = TRUE;
    if (id >= 0x1000) {
        Item *item = &mInteriors->mItems[id & 0xFFF];
        if (item->mCount >= 99) {
            received = FALSE;
        } else {
            item->Add(1);
        }
    } else if (id >= 0x800) {
        Item *item = &mCostumes->mItems[id & 0x7FF];
        if (item->mCount >= 99) {
            received = FALSE;
        } else {
            item->Add(1);
        }
    } else {
        Item *item = &mInventory->mItems[id];
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
        mPendingIndex = mList.FindItem(selected);
        mList.Select(mPendingIndex);
        mList.ScrollToSelected();
        func_02031e88(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
    }
    return received;
}

// @0x0214b6ac
void ItemTradeList::RefreshList() {
    switch (mTabs.mSelected) {
    case 0:
        mInventory->GetOwned(mItems, &mItemCount, 1, mItemFilter.GetFilter());
        break;
    case 1:
        mCostumes->GetOwned(mItems, &mItemCount, mCostumeFilter.GetFilter());
        break;
    case 2:
        mInteriors->GetOwned(mItems, &mItemCount);
        break;
    case 3:
        mInventory->GetOwnedKind3(mItems, &mItemCount);
        break;
    case 4:
        mInventory->GetOwnedKinds4And5Again(mItems, &mItemCount);
        break;
    }
    mList.SetItems(mItems, mItemCount);
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
        mRoom->mDoor.mModel.mRotX = 0;
        mRoom->mDoor.mModel.mRotY = mRoomAngle;
        break;
    case 1:
        switch (mRoomStep) {
        case 0:
            mRoomStep++;
            mRoom->RoomModels::SetVisible(FALSE);
            mRoom->ReleaseAll();
            break;
        case 1:
            mRoomStep++;
            mRoom->mId = mRoomStyle;
            mRoom->MountModel(0);
            mRoom->MountPartModels(0);
            break;
        case 2:
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mRoomStep++;
            }
            break;
        case 3:
            mRoomStep++;
            mRoom->Reload(1);
            break;
        case 4:
            mRoomStep++;
            mRoom->ReloadParts(2);
            break;
        case 5:
            SetRoomMode(0);
            mRoom->RoomModels::SetVisible(TRUE);
            data_021226e4.mLights[0].mColor = data_020f72f8[mRoom->mId].mLightColor;
            data_021226e4.mLights[1].mColor = data_020f72f8[mRoom->mId].mLightColor;
            break;
        }
        break;
    }
}

// The two camera setters are compiled without propagation (`#pragma opt_propagation off`): with it, the camera's
// address is reloaded from the literal pool after Camera_Reset instead of kept in r4, and loaded before the
// constants instead of after them.
#pragma opt_propagation off
// @0x0214ba40
void ItemTradeList::SetItemCamera() {
    Camera *camera = &gCamera;
    Camera_Reset(camera);
    camera->mPosition = sItemCameraPosition;
    camera->mTarget = sItemCameraTarget;
    Camera_UpdateEye(camera);
}

// @0x0214ba8c
void ItemTradeList::SetRoomCamera() {
    Camera *camera = &gCamera;
    camera->mPosition = sRoomCameraPosition;
    camera->mTarget = sRoomCameraTarget;
}
#pragma opt_propagation reset

// @0x0214bac4
void ItemTradeList::ResetLights() {
    data_021226e4.mLights[0].mColor = sLightColors[0];
    data_021226e4.mLights[1].mColor = sLightColors[1];
}

// Resets the list view's two buttons (func_020a9e6c: func_02031228, the main program's button reset, on the
// buttons at +0x3C and +0x130), when the partner starts sending
// @0x0214bae8
void ItemTradeList::ResetListButtons() {
    mList.ResetArrowButtons();
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
    // Emitted as objects though every use is folded; declared in this order they come out in the ROM's
    const fx32 unknown2 = 0x4000;
    const fx32 unknown1 = 0x3000;
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
        if (sItem >= 0x1000 && mPreviewStates[PREVIEW_ROOM] == 1) {
            SetRoomCamera();
            mRoom->RoomModels::SetVisible(TRUE);
            mPreviewStates[PREVIEW_ROOM] = 3;
            mPreviewStates[PREVIEW_ITEM] = 2;
            mPreviewStates[PREVIEW_COSTUME] = 2;
            mPreviewPositions[PREVIEW_ROOM].y = sPreviewStarts[PREVIEW_ROOM].y;
            mPreviewOffsets[PREVIEW_ROOM] = 0x1CCC;
            int style = sItem - 0x1000;
            ShowRoom(style);
            UpdateRoom();
            ShowName(PREVIEW_ROOM, style);
        } else if (sItem >= 0x800 && mPreviewStates[PREVIEW_COSTUME] == 1) {
            SetItemCamera();
            mMannequin->SetVisible(TRUE);
            mPreviewStates[PREVIEW_COSTUME] = 3;
            mPreviewStates[PREVIEW_ITEM] = 2;
            mPreviewStates[PREVIEW_ROOM] = 2;
            mPreviewPositions[PREVIEW_COSTUME].y = sPreviewStarts[PREVIEW_COSTUME].y;
            mPreviewOffsets[PREVIEW_COSTUME] = 0x1CCC;
            int costume = sItem - 0x800;
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
            mItemModel->ChangeItem(sItem);
            ShowName(PREVIEW_ITEM, sItem);
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
                mMannequin->SetAlpha(31 - mPreviewTimers[i] * 2);
                break;
            case PREVIEW_ROOM:
                mRoom->RoomModels::SetAlpha(31 - mPreviewTimers[i] * 2);
                mRoom->Update();
                break;
            }
            if (mPreviewTimers[i] >= 15) {
                mRoom->RoomModels::SetVisible(FALSE);
                mMannequin->SetVisible(FALSE);
                mItemModel->SetVisible(FALSE);
                mRoom->RoomModels::SetAlpha(31);
                mMannequin->SetAlpha(31);
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
    mMannequin->mPos.y = mPreviewOffsets[PREVIEW_COSTUME] - 0x599;
    mRoom->mDoor.mPosition.y = mPreviewOffsets[PREVIEW_ROOM] + 0xCC;
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
        mRoom->RoomModels::SetVisible(TRUE);
        ShowRoom(mPendingId);
        UpdateRoom();
        mList.Select(mPendingIndex);
        break;
    case PREVIEW_COSTUME:
        if (mPendingId == -1) {
            return;
        }
        SetItemCamera();
        mMannequin->SetVisible(TRUE);
        ShowCostume(mPendingId);
        LoadMannequinHead();
        mList.Select(mPendingIndex);
        break;
    case PREVIEW_ITEM:
        if (mPendingId == -1) {
            return;
        }
        SetItemCamera();
        mItemModel->mVisible = TRUE;
        mItemModel->ChangeItem(mPendingId);
        mList.Select(mPendingIndex);
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
    const VecFx32 unknown = { 0, FX32_ONE, 0 };
    mVisible = FALSE;
    mPosition.x = sFarAway.x;
    mPosition.y = sFarAway.y;
    mPosition.z = sFarAway.z;
    mScale.x = FX32_ONE;
    mScale.y = FX32_ONE;
    mScale.z = FX32_ONE;
    mUnknown12C = 0;
}

// Shows another item: lets go of the old one's files, and loads the new one's
// @0x0214c384
void ItemTradeItemModel::ChangeItem(int item) {
    ModelResources_ReleaseModel(gModelResources, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(gModelResources, mResourceId, mModelPath);
    LoadAnimation(mResourceId);
    ResetScale();
}

// --- The scene ---

// The window of each state: 0 a question with two answers and OK, 1 a message with OK, 2 three buttons and OK
// (the partners found), 3 yes and no, 4 a message alone, 5 a message with OK; -1 no window (the list)
static const int sWindowKinds[20] = { 0, 1, 1, 2, 4, 5, 5, -1, 3, 4, 4, 4, 5, 5, 5, 5, 5, 3, 4, 5 };
// The etc message of each state
static const u32 sStateMessages[20] = { 0xFD,  0x100, 0x101, 0x103, 0x113, 0x104, 0x104, 0x108, 0x107, 0x109,
                                        0x10B, 0x10D, 0x10A, 0x127, 0x110, 0x126, 0x111, 0x11E, 0x11D, 0x112 };

// The OK button's sounds, by window kind
static const int sOkSounds[6][4] = {
    { 12, -1, 0, -1 }, { 12, -1, 0, -1 }, { 12, -1, 0, -1 }, { 10, -1, 0, -1 }, { 10, -1, 0, -1 }, { 10, -1, 0, -1 },
};
// By window kind: which of the three buttons there are, and whether there is an OK button
struct WindowButtons {
    /* 0x0 */ int mButtons[3];
    /* 0xC */ int mOk;
};
static const WindowButtons sWindowButtons[6] = {
    { { 1, 1, 0 }, 1 }, { { 0, 0, 0 }, 1 }, { { 1, 1, 1 }, 1 }, { { 0, 0, 0 }, 1 }, { { 0, 0, 0 }, 0 }, { { 0, 0, 0 }, 1 },
};
// and their messages: etc messages for the three ("Look for a partner", "Be found", "Food, clothes, treasure..."),
// a common message for OK
static const WindowButtons sWindowButtonMessages[6] = {
    { { 0xFE, 0xFF, 0xF9 }, 9 },        { { 0xFE, 0xFF, 0xF9 }, 5 }, { { 0x102, 0x102, 0x102 }, 5 },
    { { 0xFE, 0xFF, 0xF9 }, 4 },        { { 0xFE, 0xFF, 0xF9 }, 4 }, { { 0xFE, 0xFF, 0xF9 }, 4 },
};

#define GetWindowButton(window, i) ((SpriteButton *)&(window)->mSprites[i])

static inline void SetPixelPosition(Sprite *sprite, WindowPoint position) {
    sprite->SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

static inline BOOL IsWindowOpen(MessageWindow *window) {
    return (window->mUnknownAD50 & 1) ? TRUE : FALSE;
}

// The yes and no question
// @0x0214c400
void ItemTradeScene::SetupQuestion(int *charOffset, MessageFile *file, u32 message) {
    ScreenRect rect = sQuestionRect;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, sWindowScreenName, 0x400);
    func_020331bc(&mWindow, 0x3F00, rect);
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow, i, 1, sAnswerSequences[i], sAnswerOnSequences[i], sAnswerPositions[i],
                      sAnswerSizes[i], &mMessages[0], sAnswerMessages[i], &gFonts[2].mFont, 0x1000, charOffset,
                      sAnswerSounds[i], 1);
    }
    func_020338d0(&mWindow, file, message, &gFonts[0].mFont, 0x1000, sTextArea, sTextPositions[2], charOffset);
    mWindow.mTexts[2].mHAlign = mWindow.mTexts[2].mMessageFile->GetAlignment(mWindow.mTexts[2].mMessage);
}

// Sets the window up for a state. The text's vertical alignment is an if/else and the buttons read their tables
// directly: with a ternary, or the positions through a local pointer, the compiler keeps `this` in r11 and loads
// the positions' address at each use, where the original spills `this` and keeps the positions in r11.
// @0x0214c5b8
void ItemTradeScene::SetupWindow(int state) {
    int i;
    int charOffset = mWindowCharOffset;
    int kind = sWindowKinds[state];
    if (kind == 3) {
        SetupQuestion(&charOffset, &mMessages[5], sStateMessages[state]);
        return;
    }

    ScreenRect rect = sWindowRect;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, sWindowScreenName, 0x400);
    func_020331bc(&mWindow, 0x3F00, rect);

    WindowPoint textPosition;
    int buttons = sWindowButtons[kind].mButtons[0];
    if (buttons != 0) {
        textPosition = sTextPositions[0];
    } else if (sWindowButtons[kind].mOk != 0) {
        textPosition = sTextPositions[2];
    } else {
        textPosition = sTextPositions[1];
    }
    func_020338d0(&mWindow, &mMessages[5], sStateMessages[state], &gFonts[0].mFont, 0x1000, sTextArea, textPosition,
                  &charOffset);
    mWindow.mTexts[0].mHAlign = mWindow.mTexts[0].mMessageFile->GetAlignment(mWindow.mTexts[0].mMessage);
    if (buttons == 0) {
        mWindow.mTexts[0].mVAlign = 1;
    } else {
        mWindow.mTexts[0].mVAlign = 0;
    }

    if (sWindowButtons[kind].mButtons[2]) {
        for (i = 0; i < 3; i++) {
            if (sWindowButtons[kind].mButtons[i]) {
                func_02033448(&mWindow, i, 1, 0x37, 0x38, sThreeButtonPositions[i], sThreeButtonSize, sButtonSounds,
                              1);
                func_02033a58(&mWindow, &mMessages[5], sWindowButtonMessages[kind].mButtons[i], &gFonts[0].mFont,
                              0x1000, sThreeButtonTextPositions[i], &charOffset);
            }
        }
    } else {
        for (i = 0; i < 3; i++) {
            if (sWindowButtons[kind].mButtons[i]) {
                func_02033448(&mWindow, i, 1, 0x35, 0x36, sButtonPositions[i], sButtonSize, sButtonSounds, 1);
                func_02033a58(&mWindow, &mMessages[5], sWindowButtonMessages[kind].mButtons[i], &gFonts[0].mFont,
                              0x1000, sButtonTextPositions[i], &charOffset);
            }
        }
    }

    if (sWindowButtons[kind].mOk) {
        func_02033844(&mWindow, 3, 1, 10, 11, sButtonPositions[3], sOkSize, &mMessages[0],
                      sWindowButtonMessages[kind].mOk, &gFonts[2].mFont, 0x1000, &charOffset, sOkSounds[kind], 1);
    }
}

// @0x0214ca20
void ItemTradeScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TIM", "Msg/TmdItemMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TDM", "Msg/TmdCostumeMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TRM", "Msg/TmdRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TXM", "Msg/TmdExplainMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "STS", "Scn/Scene/SceneTushin_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "LTL", "Scn/List/List_TushinItemList_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MH", "Mdl/Misc/headManikin_3D_LZ.bin", 0);
    mMannequin.SetArchive("3MM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin.mArchivePath, 0);

    func_0202c264(&gGameData, 0, &mPlayer);
    mPlayer.mPersonality = mPlayer.GetPersonality();
    switch (gPersonalityKinds[mPlayer.mPersonality]) {
    case 0:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_cool_LZ.bin", 0);
        break;
    case 1:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_dry_LZ.bin", 0);
        break;
    case 2:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_nagomi_LZ.bin", 0);
        break;
    case 3:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_nori_LZ.bin", 0);
        break;
    }

    char archiveName[] = "IM0";
    mItemModel.SetItem(archiveName, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mItemModel.mArchiveName, mItemModel.mArchivePath, 0);
    mRoom.mId = 0;
    mRoom.Mount("CO0", 0);
    mRoom.MountAll("RM?", 0);
}

// @0x0214ccf4
void ItemTradeScene::Init() {
    mMessages[0].Open("TCM:tmd_common_msg_JP.bmg");
    mMessages[1].Open("TTM:tmd_trouble_msg_JP.bmg");
    mMessages[2].Open("TIM:tmd_item_msg_JP.bmg");
    mMessages[3].Open("TDM:tmd_costume_msg_JP.bmg");
    mMessages[4].Open("TRM:tmd_room_msg_JP.bmg");
    mMessages[5].Open("TEM:tmd_etc_msg_JP.bmg");
    mMessages[6].Open("TXM:tmd_explain_msg_JP.bmg");

    func_02028f24(&gVramTransferQueue, "STS:/BG_up.ncg", 2, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "STS:/BG_up.ncl", 2, -1, -1, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "STS:/BG_up.nsc", 2, 0, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "LTL:/BG_list.ncl", 0x2F00, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "LTL:/BG_list.ncg", 0x2F00, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_TushinEtc_frame.nsc", 0x200, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 2);
    ScreenLoader_SetScrollX(&gScreenLoader, 0x2F00, 0);
    ScreenLoader_SetScrollY(&gScreenLoader, 0x2F00, 0);
    ScreenLoader_SetScrollY(&gScreenLoader, 0x100, -0x38);
    gScreenControl.Set(0x1313);
    gScreenControl.Set(0x1313);
    ScreenControl_SetBlendAlpha(&gScreenControl, 1, 0x12, 8, 8);
    PaletteLoader_LoadFile(&gPaletteLoader, "LTL:/Obj_list.ncl", 0x1000, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "LTL:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "LTL:/Obj_TushinEtc_list.nce", 0x1000, 1);
    PaletteLoader_LoadFile(&gPaletteLoader, "STS:/Obj_up.ncl", 0x10, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "STS:/Obj_up.ncg", 0x10, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "STS:/Obj_up.nce", 0x10, 0);
    Init3d();

    Sprite_Init(&mLinkIcon, 0, 0, 128, 96, TRUE);
    mLinkIcon.SetPosition(FxVec2(sLinkIconPosition.x << FX32_SHIFT, sLinkIconPosition.y << FX32_SHIFT));
    mLinkIcon.mVisible = FALSE;

    mContext.mCostumes = &mCostumes;
    mContext.mInventory = &mInventory;
    mContext.mInteriors = &mInteriors;
    mContext.mCommonMessages = &mMessages[0];
    mContext.mTroubleMessages = &mMessages[1];
    mContext.mItemMessages = &mMessages[2];
    mContext.mCostumeMessages = &mMessages[3];
    mContext.mRoomMessages = &mMessages[4];
    mContext.mEtcMessages = &mMessages[5];
    mContext.mMannequin = &mMannequin;
    mContext.mPlayer = &mPlayer;
    mContext.mItemModel = &mItemModel;
    mContext.mNameText = &mNameText;
    mContext.mRoom = &mRoom;
    mContext.mSceneState = &mState;

    mNameCharOffset = 0x100;
    mListCharOffset = 0x190;
    mScroll = 0;
    mWait = 0;
    mNameText.InitObj(&gFonts[0].mFont, 0x10, 0x14, 4, mNameCharOffset, 0, 1, 0, 0, 15, NULL);
    mNameText.mMessageFile = &mMessages[6];
    SetPixelPosition(&mNameText.mSprite, sNameTextPosition);
    mNameText.mSprite.mOverwritePalette = 0;
    mNameCharOffset += ((mNameText.mCanvas.areaWidth * mNameText.mCanvas.areaHeight - 1) >> mNameText.mVramMode) + 1;

    gWirelessSession->SetParentBuffer((WMBssDesc *)sPartners, 3);
    gWirelessSession->SetTimers(sPartnerTimers);
    gWirelessSession->Reset(1);
    MI_CpuClear8(sPartners, sizeof(sPartners));
    MI_CpuClear8(sPartnerTimers, sizeof(sPartnerTimers));
    mMusicPlaying = FALSE;
    mSendStep = 0;
    mPartnerReady = FALSE;
    mPartnerSending = FALSE;
    mIsParent = FALSE;
    mSaveAfterReturn = FALSE;
    mWindowDone = FALSE;

    mCostumes.Load();
    mCostumes.Sort();
    mInventory.Load();
    mInventory.Sort();
    mInteriors.Load();
    mInteriors.Sort();
    mList.Init(&mContext, &mListCharOffset);
    mList.mMode = 0;
    mList.mState = ItemTradeList::STATE_OPEN;

    mState = STATE_CHOOSE;
    mNextState = STATE_CHOOSE;
    mWindowCharOffset = mListCharOffset;
    SetupWindow(mState);
    func_02034f38(&mWindow, 0, sStateMessages[mState]);
    func_02034720(&mWindow, 0x12, 1);
    sReceivedCommand = 0;
    sSending = 0;
    sReceived = 0;
    sDisconnected = 0;

    mRoom.Load(1);
    mRoom.LoadParts(2);
    mRoom.RoomModels::SetVisible(TRUE);
    VecFx32 roomPosition = sRoomPosition;
    mRoom.mDoor.mPosition.x = roomPosition.x;
    mRoom.mDoor.mPosition.y = roomPosition.y;
    mRoom.mDoor.mPosition.z = roomPosition.z;
    mRoom.SetScale(0x1C2);
    mRoom.mDoor.mModel.mRotX = 0;
    mRoom.mDoor.mModel.mRotY = 0;
    mRoom.RoomModels::SetVisible(FALSE);
    gSoundManager.LoadGroup(0x5D);
    WH_Initialize();
}

// @0x0214d418
void ItemTradeScene::Init3d() {
    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = 1;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 1);

    mCamera = &gCamera;
    Camera_Reset(mCamera);
    VEC_Set(&mCamera->mPosition, 0, 0x599, 0x4666);
    VEC_Set(&mCamera->mTarget, 0, 0x599, 0);
    Light_Reset(gLights);
    Light_Reset(gLight1);
    Light_SetRotatedDirection(gLights, mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0x640, -0x258, -0xFA0);
    Light_SetRotatedDirection(gLight1, mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0, -0xFA0, 0x5DC);

    ModelResources_SetModel(gModelResources, 0, mMannequin.mModelPath);
    ModelResources_SetModel(gModelResources, 1, "3MH:/head.imd");
    mMannequin.Init(0, 0, 1, 0);
    void *animations = ModelResources_BindArchive(gModelResources, 0, "3HA", 0);
    func_02029660((void *)mMannequin.mAnim.mUnknown04, RESOURCE_BCA0, animations);
    SetMannequinAnimation(&mMannequin, "idle00");
    mMannequin.SetAnimFlags(TRUE, TRUE);
    mMannequin.mShadowVisible = FALSE;
    mMannequin.SetVisible(FALSE);

    VecFx32 zero = sZero;
    VecFx32 position = zero;
    mMannequin.mPos.x = position.x;
    mMannequin.mPos.y = position.y;
    mMannequin.mPos.z = position.z;
    mMannequin.mScale.x = gMannequinBodyScale.x * 4 / 10;
    mMannequin.mInvScale.x = FX_Inv(mMannequin.mScale.x);
    mMannequin.mScale.y = gMannequinBodyScale.y * 4 / 10;
    mMannequin.mInvScale.y = FX_Inv(mMannequin.mScale.y);
    mMannequin.mScale.z = gMannequinBodyScale.z * 4 / 10;
    mMannequin.mInvScale.z = FX_Inv(mMannequin.mScale.z);
    mMannequin.mHeadScale.x = gMannequinHeadScale.x * 4 / 10;
    mMannequin.mHeadScale.y = gMannequinHeadScale.y * 4 / 10;
    mMannequin.mHeadScale.z = gMannequinHeadScale.z * 4 / 10;

    ModelResources_SetModel(gModelResources, 2, mItemModel.mModelPath);
    mItemModel.Load(2);
    VecFx32 itemPosition = zero;
    mItemModel.mPosition.x = itemPosition.x;
    mItemModel.mPosition.y = itemPosition.y;
    mItemModel.mPosition.z = itemPosition.z;
    mItemModel.mVisible = FALSE;
}

static void EmptyCallback(void *arg);
static void SendCallback(BOOL result);
static void Receive(u16 aid, u16 *data, u16 size);

// @0x0214d7a8
void ItemTradeScene::UpdateWireless() {
    if (mWait > 0) {
        return;
    }
    switch (WH_GetSystemState()) {
    case WH_SYSSTATE_IDLE:
        gWirelessSession->SetUserGameInfo();
        gWirelessSession->UpdateScan();
        gWirelessSession->UpdateMeasureChannel();
        if (!gWirelessSession->UpdateConnect()) {
            mNextState = STATE_ERROR;
            WH_Reset();
        }
        break;
    case WH_SYSSTATE_CONNECTED: {
        BOOL connected = FALSE;
        int next = 20;
        switch (mState) {
        case STATE_RECRUITING:
            next = STATE_CONNECTED_PARENT;
            if (WH_GetBitmap() != WH_BITMAP_EMPTY) {
                connected = TRUE;
            }
            break;
        case STATE_CONNECTING:
            next = STATE_CONNECTED_CHILD;
            if (mWindow.mUnknownAD50 & 1) {
                connected = TRUE;
            }
            break;
        }
        if (connected && mNextState != next) {
            mNextState = next;
            if (next == STATE_CONNECTED_PARENT) {
                SetEntry(FALSE);
            }
            func_020349cc(&mWindow, 0x1C);
        }
        break;
    }
    case WH_SYSSTATE_MEASURECHANNEL: {
        u16 tgid = WM_GetNextTgid();
        WH_ParentConnect(0, tgid, WH_GetMeasureChannel());
        break;
    }
    case WH_SYSSTATE_SCANNING:
        if (gWirelessSession->IsParentFound()) {
            gWirelessSession->ClearParentFound();
            WirelessSession *session = gWirelessSession;
            session->GetFoundParentName(mNames[session->GetFoundParent()]);
        }
        if (mState == STATE_CHOOSE_PARTNER) {
            for (int i = 0; i < 3; i++) {
                if (sPartnerTimers[i] <= 0) {
                    continue;
                }
                if (sPartnerTimers[i] == 600) {
                    mWindow.mTexts[i + 1].SetStringArg(mNames[i], 0);
                    ClearText(&mWindow.mTexts[i + 1]);
                    mWindow.mTexts[i + 1].DrawAll();
                    func_02031e88(GetWindowButton(&mWindow, i), FALSE);
                }
                if (--sPartnerTimers[i] == 0) {
                    mWindow.mTexts[i + 1].SetStringArg(L"", 0);
                    ClearText(&mWindow.mTexts[i + 1]);
                    mWindow.mTexts[i + 1].DrawAll();
                    func_02031e88(GetWindowButton(&mWindow, i), TRUE);
                }
            }
        }
        break;
    case WH_SYSSTATE_CONNECT_FAIL:
        if (mState == STATE_ERROR) {
            return;
        }
        mNextState = STATE_ERROR;
        if (mWindow.mUnknownAD50 & 1) {
            func_020349cc(&mWindow, 0x1C);
        }
        WH_Reset();
        break;
    }
}

// @0x0214da84
void ItemTradeScene::UpdateMusic() {
    switch (mState) {
    case STATE_RECRUITING:
    case STATE_SEARCHING:
    case STATE_CONNECTING:
    case STATE_SENDING:
    case STATE_RECEIVING:
        if (!mMusicPlaying) {
            mMusicPlaying = TRUE;
            PlaySound(3, 9);
        }
        break;
    default:
        if (mMusicPlaying == TRUE) {
            mMusicPlaying = FALSE;
            StopSound(3);
        }
        break;
    }
}

// @0x0214db10
void ItemTradeScene::UpdateLinkIcon() {
    switch (mState) {
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 17:
    case 18:
    case 19: {
        mLinkIcon.mVisible = TRUE;
        int level = WH_GetLinkLevel();
        if (level > 3) {
            level = 3;
        } else if (level < 0) {
            level = 0;
        }
        Sprite_SetSequence(&mLinkIcon, sLinkSequences[level], TRUE);
        break;
    }
    default:
        mLinkIcon.mVisible = FALSE;
        break;
    }
}

// @0x0214dbd0
void ItemTradeScene::UpdateSend() {
    if (mSendStep == 1 && mPartnerReady) {
        mSendStep = 0;
        mPartnerReady = FALSE;
        Send(COMMAND_ITEM);
    }
}

// @0x0214dc04
void ItemTradeScene::InitWirelessWhenIdle() {
    if (mState == STATE_CHOOSE) {
        WH_Initialize();
    }
}

// Where the connection is lost, the two ways (with a window and without) are an if and its else that return
// after: with a return in each, the window's address is computed after mState is loaded, not before.
// @0x0214dc20
void ItemTradeScene::Update() {
    gWirelessSession->UpdateEnd();
    UpdateMusic();
    UpdateLinkIcon();
    func_020341d4(&mWindow);
    if (mWindow.mTransitionDone) {
        mWindowDone = TRUE;
    }
    mList.UpdateListMode();
    if (mWait > 0) {
        mWait--;
        return;
    }

    switch (sReceivedCommand) {
    case 0:
        break;
    case COMMAND_SENDING:
        mPartnerSending = TRUE;
        sReceivedCommand = 0;
        return;
    case COMMAND_READY:
        mPartnerReady = TRUE;
        sReceivedCommand = 0;
        return;
    }

    if (mPartnerSending == TRUE && mState == STATE_LIST) {
        mPartnerSending = FALSE;
        mState = STATE_RECEIVING;
        mList.Hide();
        mList.ResetListButtons();
        Send(COMMAND_READY);
        sReceivedNew = FALSE;
        SetupWindow(mState);
        func_02034f38(&mWindow, 0, sStateMessages[mState]);
        func_02034720(&mWindow, 0x12, 1);
    }

    int kind = sWindowKinds[mState];
    BOOL failed = FALSE;
    int next = STATE_DISCONNECTED;
    if (sDisconnected ||
        ((WH_GetSystemState() == WH_SYSSTATE_ERROR || WH_GetSystemState() == WH_SYSSTATE_FATAL) &&
         mNextState != STATE_DISCONNECTED)) {
        if (WH_GetSystemState() != WH_SYSSTATE_STOP) {
            WH_Finalize();
            gWirelessSession->RequestEnd(TRUE);
        }
        sDisconnected = 0;
        failed = TRUE;
        next = STATE_DISCONNECTED;
    }
    if (failed) {
        mNextState = next;
        mList.Hide();
        if (kind != -1) {
            mWindow.mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindow, 0x1C);
            PlaySound(13);
        } else {
            mState = next;
            SetupWindow(mState);
            func_02034f38(&mWindow, 0, sStateMessages[mState]);
            func_02034720(&mWindow, 0x12, 1);
            PlaySound(12);
        }
        return;
    }

    if (kind != -1) {
        mList.UpdatePreviews();
        int result = mWindow.mResult;
        if (result != -1) {
            mWindow.mUnknownSetBeforeClose = 15;
            int flags = 0x1C;
            switch (mState) {
            case STATE_CHOOSE:
                switch (result) {
                case 0:
                    mNextState = STATE_RECRUITING;
                    gWirelessSession->Initialize();
                    WH_SetReceiver(Receive);
                    break;
                case 1:
                    mNextState = STATE_SEARCHING;
                    for (int i = 0; i < 3; i++) {
                        sPartnerTimers[i] = 0;
                    }
                    ClearNames();
                    gWirelessSession->Initialize();
                    gWirelessSession->SetSsid();
                    WH_SetReceiver(Receive);
                    break;
                case 3:
                    gSceneManager.RequestScene(0x29);
                    flags = 0x2C;
                    break;
                }
                break;
            case STATE_RECRUITING:
                if (result == 3) {
                    mNextState = STATE_CHOOSE;
                    flags = 0x2C;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(TRUE);
                }
                break;
            case STATE_SEARCHING:
                if (result == 3) {
                    mNextState = STATE_CHOOSE;
                    flags = 0x2C;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(TRUE);
                }
                break;
            case STATE_CHOOSE_PARTNER:
                if (result == 3) {
                    mNextState = STATE_CHOOSE;
                    flags = 0x2C;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(TRUE);
                } else if (result >= 0 && result < 3 && sPartnerTimers[result] > 0) {
                    gWirelessSession->EndScan(result);
                    mNextState = STATE_CONNECTING;
                }
                break;
            case STATE_CONNECTED_PARENT:
            case STATE_CONNECTED_CHILD:
                mNextState = STATE_LIST;
                flags = 0x14;
                break;
            case STATE_ASK_SEND:
                if (result == 0) {
                    mNextState = STATE_SENDING;
                    Send(COMMAND_SENDING);
                } else {
                    mNextState = STATE_LIST;
                    flags = 0x14;
                }
                break;
            case STATE_SENT:
            case STATE_PARTNER_FULL:
            case STATE_RECEIVED:
            case STATE_FULL:
                mNextState = STATE_LIST;
                flags = 0x14;
                break;
            case STATE_DISCONNECTED:
                gSceneManager.RequestScene(0x29);
                break;
            case STATE_ASK_DISCONNECT:
                if (result == 0) {
                    mNextState = STATE_WAIT;
                } else {
                    mNextState = STATE_LIST;
                    flags = 0x14;
                }
                break;
            case STATE_ERROR:
                mNextState = STATE_CHOOSE;
                gSceneManager.RequestScene(0x29);
                break;
            }
            func_020349cc(&mWindow, flags);
        } else {
            switch (mState) {
            case STATE_SEARCHING:
                if (WH_GetSystemState() == WH_SYSSTATE_SCANNING && IsWindowOpen(&mWindow) &&
                    mNextState != STATE_CHOOSE_PARTNER && gWirelessSession->CountParents() > 0) {
                    mNextState = STATE_CHOOSE_PARTNER;
                    mWindow.mUnknownSetBeforeClose = 15;
                    func_020349cc(&mWindow, 0x1C);
                }
                break;
            case STATE_ASK_SEND:
            case STATE_ASK_DISCONNECT:
                if (mPartnerSending == TRUE && mNextState != STATE_SENDING) {
                    mNextState = STATE_RECEIVING;
                    mWindow.mUnknownSetBeforeClose = 15;
                    func_020349cc(&mWindow, 0x1C);
                }
                break;
            case STATE_SENDING:
                switch (mSendStep) {
                case 2:
                    mList.RemoveOne();
                    mList.Save();
                    WriteSave();
                    mSendStep = 3;
                    break;
                case 3:
                    if (!func_0202cd38(&gGameData)) {
                        mSendStep = 1;
                    }
                    break;
                }
                if (mSendStep == 0 && IsWindowOpen(&mWindow) && sSending && sSent) {
                    BOOL done = FALSE;
                    switch (sReceivedCommand) {
                    case COMMAND_RECEIVED:
                        mNextState = STATE_SENT;
                        done = TRUE;
                        break;
                    case COMMAND_FULL:
                        mList.AddOne();
                        mList.Save();
                        WriteSave();
                        mSaveAfterReturn = TRUE;
                        break;
                    }
                    sReceivedCommand = 0;
                    if (mSaveAfterReturn == TRUE && !func_0202cd38(&gGameData)) {
                        mNextState = STATE_PARTNER_FULL;
                        mSaveAfterReturn = FALSE;
                        done = TRUE;
                    }
                    if (done == TRUE) {
                        sSent = FALSE;
                        sSending = 0;
                        mWindow.mUnknownSetBeforeClose = 15;
                        func_020349cc(&mWindow, 0x1C);
                    }
                }
                break;
            case STATE_RECEIVING:
                if (IsWindowOpen(&mWindow) && sReceived && sReceivedNew == TRUE) {
                    if (mList.Receive(sItem) == TRUE) {
                        mList.Save();
                        Send(COMMAND_RECEIVED);
                        mNextState = STATE_SAVING;
                        mWindow.mUnknownSetBeforeClose = 15;
                        func_020349cc(&mWindow, 0x1C);
                    } else {
                        Send(COMMAND_FULL);
                        sReceived = 0;
                        mNextState = STATE_FULL;
                        mWindow.mUnknownSetBeforeClose = 15;
                        func_020349cc(&mWindow, 0x1C);
                    }
                    sReceivedNew = FALSE;
                }
                break;
            case STATE_SAVING:
                if (IsWindowOpen(&mWindow) && !func_0202cd38(&gGameData)) {
                    if (sReceived) {
                        WriteSave();
                        sReceived = 0;
                    } else {
                        mNextState = STATE_RECEIVED;
                        mWindow.mUnknownSetBeforeClose = 15;
                        func_020349cc(&mWindow, 0x1C);
                    }
                }
                break;
            case STATE_WAIT:
                if (WH_GetSystemState() == WH_SYSSTATE_STOP || WH_GetSystemState() == WH_SYSSTATE_IDLE) {
                    gSceneManager.RequestScene(0x29);
                }
                break;
            }
        }

        if (mWindowDone == TRUE) {
            int nextKind = sWindowKinds[mNextState];
            if (nextKind >= 0) {
                SetupWindow(mNextState);
                if (nextKind == 2) {
                    for (int i = 0; i < 3; i++) {
                        if (sPartnerTimers[i] > 0) {
                            func_02031e88(GetWindowButton(&mWindow, i), FALSE);
                            mWindow.mTexts[i + 1].SetStringArg(mNames[i], 0);
                        } else {
                            func_02031e88(GetWindowButton(&mWindow, i), TRUE);
                        }
                    }
                }
                switch (mNextState) {
                case STATE_RECRUITING:
                    gWirelessSession->RequestMeasureChannel(TRUE);
                    break;
                case STATE_SEARCHING:
                    gWirelessSession->RequestScan(TRUE);
                    break;
                case STATE_CHOOSE_PARTNER:
                    PlaySound(10);
                    break;
                case STATE_CONNECTING:
                    gWirelessSession->RequestConnect(TRUE);
                    mWait = 60;
                    break;
                case STATE_CONNECTED_PARENT:
                    mWindow.mTexts[0].SetStringArg((const wchar_t *)gWHChildSsid, 0);
                    PlaySound(11);
                    mIsParent = TRUE;
                    break;
                case STATE_CONNECTED_CHILD:
                    int partner = gWirelessSession->mParent;
                    mWindow.mTexts[0].SetStringArg(mNames[partner], 0);
                    PlaySound(11);
                    mIsParent = FALSE;
                    break;
                case STATE_SENDING:
                    if (mPartnerSending == TRUE) {
                        mPartnerSending = FALSE;
                        if (!mIsParent) {
                            mNextState = STATE_RECEIVING;
                            Send(COMMAND_READY);
                            sReceivedNew = FALSE;
                            break;
                        }
                    }
                    mSendStep = 2;
                    mSaveAfterReturn = FALSE;
                    break;
                case STATE_RECEIVING:
                    sReceivedNew = FALSE;
                    if ((mState == STATE_ASK_SEND || mState == STATE_ASK_DISCONNECT) && mPartnerSending == TRUE) {
                        mPartnerSending = FALSE;
                        Send(COMMAND_READY);
                    }
                    break;
                case STATE_SAVING:
                    mWait = 60;
                    break;
                case STATE_SENT:
                    PlaySound(11);
                    break;
                case STATE_PARTNER_FULL:
                case STATE_FULL:
                    PlaySound(12);
                    break;
                case STATE_RECEIVED: {
                    const wchar_t *name;
                    if (sItem >= 0x1000) {
                        name = mContext.mRoomMessages->GetText((u16)(sItem & 0xFFF));
                    } else if (sItem >= 0x800) {
                        name = mContext.mCostumeMessages->GetText((u16)(sItem & 0x7FF));
                    } else {
                        name = mContext.mItemMessages->GetText((u16)sItem);
                    }
                    mWindow.mTexts[0].SetStringArg(name, 0);
                    PlaySound(11);
                    break;
                }
                case STATE_WAIT:
                    WH_Finalize();
                    gWirelessSession->RequestEnd(TRUE);
                    mWait = 60;
                    break;
                case STATE_ERROR:
                    gWirelessSession->RequestEnd(TRUE);
                    PlaySound(13);
                    break;
                }
                func_02034f38(&mWindow, 0, sStateMessages[mNextState]);
                if (mState > mNextState) {
                    func_02034720(&mWindow, 0x2A, 1);
                } else {
                    func_02034720(&mWindow, 0x1A, 1);
                }
                if (nextKind == 2) {
                    for (int i = 0; i < 3; i++) {
                        if (sPartnerTimers[i] > 0) {
                            func_02031e88(GetWindowButton(&mWindow, i), FALSE);
                        } else {
                            func_02031e88(GetWindowButton(&mWindow, i), TRUE);
                        }
                    }
                }
            }
            mState = mNextState;
            InitWirelessWhenIdle();
        }
        mWindowDone = FALSE;
    } else {
        int result = mList.Update();
        if (sReceived) {
            mState = STATE_RECEIVING;
            sReceivedNew = FALSE;
            SetupWindow(mState);
            func_02034f38(&mWindow, 0, sStateMessages[mState]);
            func_02034720(&mWindow, 0x12, 1);
        } else if (mState == STATE_LIST) {
            switch (result) {
            case ItemTradeList::RESULT_OK: {
                mState = STATE_ASK_SEND;
                mList.Hide();
                SetupWindow(mState);
                const wchar_t *name;
                switch (mList.mSelection.mKind) {
                case 0:
                    name = mContext.mItemMessages->GetText((u16)mList.mChosenId);
                    break;
                case 1:
                    name = mContext.mCostumeMessages->GetText((u16)mList.mChosenId);
                    break;
                case 2:
                    name = mContext.mRoomMessages->GetText((u16)mList.mChosenId);
                    break;
                default:
                    name = mContext.mCostumeMessages->GetText((u16)mList.mChosenId);
                    break;
                }
                mWindow.mTexts[0].SetStringArg(name, 0);
                func_02034f38(&mWindow, 0, sStateMessages[mState]);
                func_02034720(&mWindow, 0x12, 1);
                break;
            }
            case ItemTradeList::RESULT_BACK:
                mState = STATE_ASK_DISCONNECT;
                mList.Hide();
                SetupWindow(mState);
                func_02034f38(&mWindow, 0, sStateMessages[mState]);
                func_02034720(&mWindow, 0x12, 1);
                break;
            }
        }
    }
    UpdateWireless();
    UpdateSend();
}

// @0x0214eb68
void ItemTradeScene::Draw() {
    Sprite_Draw(&mLinkIcon);
    mList.Draw();
    func_020345d8(&mWindow);
    mMannequin.Draw();
    mItemModel.Draw();
    mRoom.RoomModels::Draw();
    mScroll += 0x4CC;
    ScreenLoader_SetScrollX(&gScreenLoader, 2, mScroll >> FX32_SHIFT);
}

// @0x0214ebe8
void ItemTradeScene::Exit() {
    if (WH_GetSystemState() != WH_SYSSTATE_STOP && !gWirelessSession->mEndRequested) {
        WH_Finalize();
        gWirelessSession->RequestEnd(TRUE);
    }
    mCostumes.Save();
    mInventory.Save();
    mRoom.ReleaseAll();
    ArchiveManager_Unmount(&gArchiveManager, mItemModel.mArchiveName);
    ModelResources_ReleaseModel(gModelResources, 2);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ArchiveManager_Unmount(&gArchiveManager, "LTL");
    ArchiveManager_Unmount(&gArchiveManager, "STS");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TIM");
    ArchiveManager_Unmount(&gArchiveManager, "TDM");
    ArchiveManager_Unmount(&gArchiveManager, "TRM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TXM");
    gWirelessSession->RequestEnd(TRUE);
    while (gWirelessSession->UpdateEnd() == TRUE) {
    }
}

// Sends the item chosen, or a command
// @0x0214ed70
void ItemTradeScene::Send(int command) {
    int state = WH_GetSystemState();
    if (state == WH_SYSSTATE_STOP || state == WH_SYSSTATE_ERROR || state == WH_SYSSTATE_FATAL) {
        sSent = FALSE;
        return;
    }
    sCommand = command;
    switch (command) {
    case COMMAND_ITEM: {
        if (sSending) {
            return;
        }
        sSending = 1;
        int id = mList.mChosenId;
        sItem = id;
        switch (mList.mSelection.mKind) {
        case 0:
            break;
        case 1:
            sItem = id + 0x800;
            break;
        case 2:
            sItem = id + 0x1000;
            break;
        }
        WH_SendData(&sItem, 4, SendCallback);
        sSent = FALSE;
        break;
    }
    case COMMAND_1:
    case COMMAND_SENDING:
    case COMMAND_READY:
    case COMMAND_RECEIVED:
    case COMMAND_FULL:
    default:
        WH_SendData(&sCommand, 2, SendCallback);
        break;
    }
}

// @0x0214ee58
void ItemTradeScene::SetEntry(BOOL entry) {
    WM_SetEntry(EmptyCallback, entry);
}

// @0x0214ee6c
void ItemTradeScene::ClearNames() {
    MI_CpuClear8(mNames, sizeof(mNames));
}

// Starts writing the save data, or asks for the scene that says it could not be written
// @0x0214ee88
void ItemTradeScene::WriteSave() {
    if (!func_0202cd48(&gGameData)) {
        gSceneManager.RequestScene(0xC2);
    }
}

// @0x0214eeb4
static void SendCallback(BOOL result) {
    switch (WH_GetSystemState()) {
    case WH_SYSSTATE_STOP:
    case WH_SYSSTATE_ERROR:
    case WH_SYSSTATE_FATAL:
        return;
    }
    sSent = TRUE;
}

// WH's receiver: no data when the partner has gone, two bytes for a command, four for an item
// @0x0214eee0
static void Receive(u16 aid, u16 *data, u16 size) {
    switch (size) {
    case 0:
        sDisconnected = 1;
        break;
    case 2:
        sReceivedCommand = *data;
        break;
    default:
        MI_CpuCopy8(data, &sItem, 4);
        sReceived = 1;
        sReceivedNew = TRUE;
        break;
    }
}

// @0x0214ef44
static void EmptyCallback(void *arg) {
}
