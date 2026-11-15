#include "ItemTradeScene.h"

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
    func_0208ee18(&mMannequin->mModel, "3MM", costume, MiiData_GetSex(mContext->mPlayer));
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin->mArchivePath, 1);
    ModelResources_SetModel(data_021223e4, 0, mMannequin->mModelPath);
    func_0208ef5c(&mMannequin->mModel, 0, 0);
    func_0208f19c(&mMannequin->mModel, func_02073514(MiiData_GetFavoriteColor(mContext->mPlayer)), 0x5FBE);
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

// The two camera setters are compiled without propagation (`#pragma opt_propagation off`): with it, the camera's
// address is reloaded from the literal pool after func_0202a640 instead of kept in r4, and loaded before the
// constants instead of after them.
#pragma opt_propagation off
// @0x0214ba40
void ItemTradeList::SetItemCamera() {
    Camera *camera = &data_02122764;
    func_0202a640(camera);
    camera->mPosition = sItemCameraPosition;
    camera->mTarget = sItemCameraTarget;
    func_0202a748(camera);
}

// @0x0214ba8c
void ItemTradeList::SetRoomCamera() {
    Camera *camera = &data_02122764;
    camera->mPosition = sRoomCameraPosition;
    camera->mTarget = sRoomCameraTarget;
}
#pragma opt_propagation reset

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
    const fx32 unknown1 = 0x3000;
    const fx32 unknown2 = 0x4000;
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
            func_020a4b74(&mRoom->mParts, TRUE);
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
            func_020a03bc(mMannequin, TRUE);
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
    mRoom->mParts.mPosition.y = mPreviewOffsets[PREVIEW_ROOM] + 0xCC;
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
    func_0202aecc(data_021223e4, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(data_021223e4, mResourceId, mModelPath);
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

// Sets the window up for a state
// NOT MATCHING: the original keeps the base of the button positions (sThreeButtonPositions, sButtonPositions) in
// r11 through each loop and so keeps this and kind on the stack for the whole function; this reloads the
// table's address each time and keeps this in r11 (0x20 bytes shorter). The calls and their arguments are the
// same. Tried: a local pointer to the table, the loops as one inline helper, the button and message rows as
// locals.
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
    mWindow.mTexts[0].mVAlign = buttons == 0 ? 1 : 0;

    if (sWindowButtons[kind].mButtons[2]) {
        WindowPoint size = sThreeButtonSize;
        const WindowPoint *positions = sThreeButtonPositions;
        for (i = 0; i < 3; i++) {
            if (sWindowButtons[kind].mButtons[i]) {
                func_02033448(&mWindow, i, 1, 0x37, 0x38, positions[i], size, sButtonSounds, 1);
                func_02033a58(&mWindow, &mMessages[5], sWindowButtonMessages[kind].mButtons[i], &gFonts[0].mFont,
                              0x1000, sThreeButtonTextPositions[i], &charOffset);
            }
        }
    } else {
        WindowPoint size = sButtonSize;
        const WindowPoint *positions = sButtonPositions;
        for (i = 0; i < 3; i++) {
            if (sWindowButtons[kind].mButtons[i]) {
                func_02033448(&mWindow, i, 1, 0x35, 0x36, positions[i], size, sButtonSounds, 1);
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
    func_0208ee18(&mMannequin.mModel, "3MM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin.mArchivePath, 0);

    func_0202c264(&gGameData, 0, &mPlayer);
    mPlayer.mPersonality = func_02098f2c(&mPlayer);
    switch (data_020fba28[mPlayer.mPersonality]) {
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
    mRoom.mStyle = 0;
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
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
    func_02027534(&data_021213b8, "STS:/BG_up.ncl", 2, -1, -1, -1);
    func_02027d7c(&data_02121834, "STS:/BG_up.nsc", 2, 0, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "LTL:/BG_list.ncl", 0x2F00, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "LTL:/BG_list.ncg", 0x2F00, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "LTL:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "LTL:/BG_TushinEtc_frame.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 0x100, 3);
    func_02028280(&data_02121834, 0x200, 2);
    func_02028108(&data_02121834, 0x2F00, 0);
    func_02028154(&data_02121834, 0x2F00, 0);
    func_02028154(&data_02121834, 0x100, -0x38);
    data_0211d604.Set(0x1313);
    data_0211d604.Set(0x1313);
    func_02023ee0(&data_0211d604, 1, 0x12, 8, 8);
    func_02027534(&data_021213b8, "LTL:/Obj_list.ncl", 0x1000, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "LTL:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "LTL:/Obj_TushinEtc_list.nce", 0x1000, 1);
    func_02027534(&data_021213b8, "STS:/Obj_up.ncl", 0x10, -1, -1, -1);
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

    func_0203b7d4(data_0213b700, sPartners, 3);
    func_0203b7e0(data_0213b700, sPartnerTimers);
    func_0203b76c(data_0213b700, 1);
    MI_CpuClear8(sPartners, sizeof(sPartners));
    MI_CpuClear8(sPartnerTimers, sizeof(sPartnerTimers));
    mMusicPlaying = FALSE;
    mSendStep = 0;
    mPartnerReady = FALSE;
    mPartnerSending = FALSE;
    mIsParent = FALSE;
    mSaveAfterReturn = FALSE;
    mWindowDone = FALSE;

    func_0209e248(&mCostumes);
    func_0209e3c8(&mCostumes);
    mInventory.Load();
    func_0209ef8c(&mInventory);
    func_020a4f58(&mInteriors);
    func_020a5090(&mInteriors);
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

    func_020a5514(&mRoom, 1);
    func_020a5760(&mRoom, 2);
    func_020a4b74(&mRoom.mParts, TRUE);
    VecFx32 roomPosition = sRoomPosition;
    mRoom.mParts.mPosition.x = roomPosition.x;
    mRoom.mParts.mPosition.y = roomPosition.y;
    mRoom.mParts.mPosition.z = roomPosition.z;
    func_020a5970(&mRoom, 0x1C2);
    mRoom.mParts.mDoor.mModel.mRotX = 0;
    mRoom.mParts.mDoor.mModel.mRotY = 0;
    func_020a4b74(&mRoom.mParts, FALSE);
    gSoundManager.LoadGroup(0x5D);
    WH_Initialize();
}

// @0x0214d418
void ItemTradeScene::Init3d() {
    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    func_02028280(&data_02121834, 1, 0);
    func_02028280(&data_02121834, 2, 1);

    mCamera = &data_02122764;
    func_0202a640(mCamera);
    VEC_Set(&mCamera->mPosition, 0, 0x599, 0x4666);
    VEC_Set(&mCamera->mTarget, 0, 0x599, 0);
    func_0202a96c(data_021227cc);
    func_0202a96c(data_021227d8);
    func_0202ab2c(data_021227cc, mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0x640, -0x258, -0xFA0);
    func_0202ab2c(data_021227d8, mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0, -0xFA0, 0x5DC);

    ModelResources_SetModel(data_021223e4, 0, mMannequin.mModelPath);
    ModelResources_SetModel(data_021223e4, 1, "3MH:/head.imd");
    func_020a02c0(&mMannequin, 0, 0, 1, 0);
    void *animations = func_0202ad04(data_021223e4, 0, "3HA", 0);
    func_02029660(mMannequin.mModel.mAnim.mResourceSet, RESOURCE_BCA0, animations);
    mMannequin.SetAnimation("idle00");
    func_0208ef00(&mMannequin.mModel, TRUE, TRUE);
    mMannequin.mModel.mUnknown4A4 = FALSE;
    func_020a03bc(&mMannequin, FALSE);

    VecFx32 zero = sZero;
    VecFx32 position = zero;
    mMannequin.mModel.mPos.x = position.x;
    mMannequin.mModel.mPos.y = position.y;
    mMannequin.mModel.mPos.z = position.z;
    mMannequin.mModel.mScale.x = data_020fbe0c.x * 4 / 10;
    mMannequin.mModel.mInvScale.x = FX_Inv(mMannequin.mModel.mScale.x);
    mMannequin.mModel.mScale.y = data_020fbe0c.y * 4 / 10;
    mMannequin.mModel.mInvScale.y = FX_Inv(mMannequin.mModel.mScale.y);
    mMannequin.mModel.mScale.z = data_020fbe0c.z * 4 / 10;
    mMannequin.mModel.mInvScale.z = FX_Inv(mMannequin.mModel.mScale.z);
    mMannequin.mUnknown140.x = data_020fbe00.x * 4 / 10;
    mMannequin.mUnknown140.y = data_020fbe00.y * 4 / 10;
    mMannequin.mUnknown140.z = data_020fbe00.z * 4 / 10;

    ModelResources_SetModel(data_021223e4, 2, mItemModel.mModelPath);
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
        func_0203b898(data_0213b700);
        func_0203b908(data_0213b700);
        func_0203b948(data_0213b700);
        if (!func_0203b970(data_0213b700)) {
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
        if (func_0203b9b4(data_0213b700)) {
            func_0203b9fc(data_0213b700);
            WirelessSession *session = data_0213b700;
            func_0203ba20(session, mNames[func_0203ba10(session)]);
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

// NOT MATCHING, by one instruction's place: where the connection is lost and there is no window, the original
// computes the window's address (add r0, r9, #500) before it loads mState for sStateMessages, this after.
// @0x0214dc20
void ItemTradeScene::Update() {
    func_0203b800(data_0213b700);
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
        mList.func_ov110_0214bae8();
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
            func_0203b8b4(data_0213b700, TRUE);
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
            return;
        }
        mState = next;
        SetupWindow(mState);
        func_02034f38(&mWindow, 0, sStateMessages[mState]);
        func_02034720(&mWindow, 0x12, 1);
        PlaySound(12);
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
                    func_0203b7e8(data_0213b700);
                    WH_SetReceiver(Receive);
                    break;
                case 1:
                    mNextState = STATE_SEARCHING;
                    for (int i = 0; i < 3; i++) {
                        sPartnerTimers[i] = 0;
                    }
                    ClearNames();
                    func_0203b7e8(data_0213b700);
                    func_0203b87c(data_0213b700);
                    WH_SetReceiver(Receive);
                    break;
                case 3:
                    func_0203c02c(&gSceneManager, 0x29);
                    flags = 0x2C;
                    break;
                }
                break;
            case STATE_RECRUITING:
                if (result == 3) {
                    mNextState = STATE_CHOOSE;
                    flags = 0x2C;
                    WH_Finalize();
                    func_0203b8b4(data_0213b700, TRUE);
                }
                break;
            case STATE_SEARCHING:
                if (result == 3) {
                    mNextState = STATE_CHOOSE;
                    flags = 0x2C;
                    WH_Finalize();
                    func_0203b8b4(data_0213b700, TRUE);
                }
                break;
            case STATE_CHOOSE_PARTNER:
                if (result == 3) {
                    mNextState = STATE_CHOOSE;
                    flags = 0x2C;
                    WH_Finalize();
                    func_0203b8b4(data_0213b700, TRUE);
                } else if (result >= 0 && result < 3 && sPartnerTimers[result] > 0) {
                    func_0203bac4(data_0213b700, result);
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
                func_0203c02c(&gSceneManager, 0x29);
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
                func_0203c02c(&gSceneManager, 0x29);
                break;
            }
            func_020349cc(&mWindow, flags);
        } else {
            switch (mState) {
            case STATE_SEARCHING:
                if (WH_GetSystemState() == WH_SYSSTATE_SCANNING && IsWindowOpen(&mWindow) &&
                    mNextState != STATE_CHOOSE_PARTNER && func_0203b9c4(data_0213b700) > 0) {
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
                    func_0203c02c(&gSceneManager, 0x29);
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
                    func_0203b940(data_0213b700, TRUE);
                    break;
                case STATE_SEARCHING:
                    func_0203b900(data_0213b700, TRUE);
                    break;
                case STATE_CHOOSE_PARTNER:
                    PlaySound(10);
                    break;
                case STATE_CONNECTING:
                    func_0203b968(data_0213b700, TRUE);
                    mWait = 60;
                    break;
                case STATE_CONNECTED_PARENT:
                    mWindow.mTexts[0].SetStringArg((const wchar_t *)gWHChildSsid, 0);
                    PlaySound(11);
                    mIsParent = TRUE;
                    break;
                case STATE_CONNECTED_CHILD:
                    int partner = data_0213b700->mPartner;
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
                    func_0203b8b4(data_0213b700, TRUE);
                    mWait = 60;
                    break;
                case STATE_ERROR:
                    func_0203b8b4(data_0213b700, TRUE);
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
    func_020a034c(&mMannequin);
    mItemModel.Draw();
    func_020a4a50(&mRoom.mParts);
    mScroll += 0x4CC;
    func_02028108(&data_02121834, 2, mScroll >> FX32_SHIFT);
}

// @0x0214ebe8
void ItemTradeScene::Exit() {
    if (WH_GetSystemState() != WH_SYSSTATE_STOP && !data_0213b700->mActive) {
        WH_Finalize();
        func_0203b8b4(data_0213b700, TRUE);
    }
    func_0209e310(&mCostumes);
    mInventory.Save();
    func_020a5948(&mRoom);
    ArchiveManager_Unmount(&gArchiveManager, mItemModel.mArchiveName);
    func_0202aecc(data_021223e4, 2);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
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
    func_0203b8b4(data_0213b700, TRUE);
    while (func_0203b800(data_0213b700) == TRUE) {
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
        func_0203c02c(&gSceneManager, 0xC2);
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
