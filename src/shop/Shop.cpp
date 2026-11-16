#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "shop/Shop.h"

// The shops' shared code: the scrolling item list of the lower screen ("LSL:/BG_list", "LTL:/Obj_list", ...), which
// the shops (overlays 56, 68, 83 to 85) and other scenes with lists of items (75, 86, 100, 110) use, the shops'
// price window, and the shop staff's model and speech.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

extern "C" {
void func_02031228(ListButton *button);
void func_02031e28(ListButton *button, BOOL pressed);
void func_02032a0c(void *scrollBar, int a, int range, int b);
void func_02032acc(void *scrollBar, int a, int b, int c);
void func_02032f60(void *area, int a, int b);
}

void ItemListView::Init(int rows) {
    mUnknown0004 = 0;
    mMode = 1;
    mItems = NULL;
    mItemCount = 0;
    mMoney = NULL;
    mTouched = FALSE;
    mTouchedRow = -1;
    mSelected = -1;
    mUnknown0024 = FALSE;
    mScroll = 0;
    mRowCount = rows + 1;
    mVisibleRows = rows;
    mBgPlane = 0x100;
    mBgOffset = 0;
    mRectX = 5;
    mRectY = 0x37;
    mRectWidth = 0xE5;
    mRectHeight = mVisibleRows * 20;
    mUnknown13F60 = 0;
}

void ItemListView::Init() {
    Init(5);
}

void ItemListView::SetBackground(int plane, int offset) {
    mBgPlane = plane;
    mBgOffset = offset;
}

// -- 0x020a818c to 0x020a8adc: the setting up of the rows, buttons and texts, and the update (not decompiled) --

void ItemListView::SetUpScrollBar(int a, int b, int c) {
    func_02032acc(mScrollBar, a, b, c);
    func_02032f60(mTouchArea, a, c);
}

// -- 0x020a8eb4 to 0x020a925c: drawing (not decompiled) --

// Draws the rows that changed: with their item, or empty
void ItemListView::DrawDirtyRows() {
    for (int i = 0; i < mRowCount; i++) {
        if (mRowDirty[i]) {
            if (mRowItemIds[i] != -1) {
                DrawRow(i, GetRowItem(i));
            } else {
                ClearRow(i);
            }
            mRowDirty[i] = FALSE;
        }
    }
}

// Marks every row to be drawn again and takes its item, number and palette
void ItemListView::UpdateRows() {
    for (int i = 0; i < mRowCount; i++) {
        mRowDirty[i] = TRUE;
        int item = GetRowItem(i);
        if (item >= 0 && item < mItemCount) {
            mRowItemIds[i] = mItems[item]->mId;
        } else {
            mRowItemIds[i] = -1;
        }
        UpdateRowPalette(i, item);
        UpdateRowValue(i, item);
        UpdateRowCount(i, item);
    }
}

void ItemListView::SetMode(int mode) {
    if (mode == mMode) {
        return;
    }
    mMode = mode;
    for (int i = 0; i < mRowCount; i++) {
        mRowDirty[i] = TRUE;
    }
}

void ItemListView::SetScroll(int scroll) {
    mScroll = scroll;
    for (int i = 0; i < mRowCount; i++) {
        int item = GetRowItem(i);
        if (item >= 0 && item < mItemCount) {
            if (mRowItemIds[i] != mItems[item]->mId) {
                mRowDirty[i] = TRUE;
                mRowItemIds[i] = mItems[item]->mId;
            }
        } else if (mRowItemIds[i] != -1) {
            mRowDirty[i] = TRUE;
            mRowItemIds[i] = -1;
        }
        UpdateRowValue(i, item);
        UpdateRowCount(i, item);
        UpdateRowPalette(i, item);
    }
}

// A row is greyed (palette 9) in mode 1 when the money cannot pay for its item, in modes 2 and 3 when the item is
// not had
void ItemListView::UpdateRowPalette(int row, int item) {
    int palette;
    if (item < 0 || mItemCount <= item) {
        mRowPalettes[row] = 8;
        if (mRowItemIds[row] != -1) {
            mRowDirty[row] = TRUE;
            mRowItemIds[row] = -1;
        }
        return;
    }
    switch (mMode) {
    case 0:
        palette = 8;
        break;
    case 1: {
        BOOL affordable = TRUE;
        if (mMoney != NULL) {
            affordable = mItems[item]->GetPrice() <= *mMoney ? TRUE : FALSE;
        }
        palette = affordable ? 8 : 9;
        break;
    }
    case 2:
    case 3:
        palette = mItems[item]->mUnknown0C ? 8 : 9;
        break;
    }
    if (palette != mRowPalettes[row]) {
        mRowDirty[row] = TRUE;
        mRowPalettes[row] = palette;
    }
}

void ItemListView::UpdateRowValue(int row, int item) {
    if (item < 0 || mItemCount <= item) {
        mRowValues[row] = 0x4D;
        if (mRowItemIds[row] != -1) {
            mRowDirty[row] = TRUE;
            mRowItemIds[row] = -1;
        }
        return;
    }
    if (mRowValues[row] != mItems[item]->mUnknown0C) {
        mRowDirty[row] = TRUE;
        mRowValues[row] = mItems[item]->mUnknown0C;
    }
}

void ItemListView::UpdateRowCount(int row, int item) {
    if (item < 0 || mItemCount <= item) {
        mRowCounts[row] = 0x4D;
        if (mRowItemIds[row] != -1) {
            mRowDirty[row] = TRUE;
            mRowItemIds[row] = -1;
        }
        return;
    }
    if (mRowCounts[row] != mItems[item]->mCount) {
        mRowDirty[row] = TRUE;
        mRowCounts[row] = mItems[item]->mCount;
    }
}

// Sets the message file that the rows take the items' names from
void ItemListView::SetMessageFile(MessageFile *file) {
    for (int i = 0; i < mRowCount; i++) {
        mRowTexts[i].mMessageFile = file;
    }
}

// Shows a list of items from its top
void ItemListView::SetItems(InventoryItem **items, int count) {
    mUnknown0004 = 0;
    mTouchedRow = -1;
    mSelected = -1;
    mTouched = FALSE;
    mScroll = 0;
    if (count != 0) {
        mItems = items;
        mItemCount = count;
    } else {
        mItems = NULL;
        mItemCount = 0;
    }
    if (mItemCount <= mVisibleRows) {
        *(BOOL *)((u8 *)&mUpButton + 0x60) = FALSE;
        *(BOOL *)((u8 *)&mDownButton + 0x60) = FALSE;
        *(BOOL *)(mScrollBar + 0x60) = FALSE;
    } else {
        *(BOOL *)((u8 *)&mUpButton + 0x60) = TRUE;
        *(BOOL *)((u8 *)&mDownButton + 0x60) = TRUE;
        *(BOOL *)(mScrollBar + 0x60) = TRUE;
        func_02032a0c(mScrollBar, 0, (mItemCount - mVisibleRows) * 20, 0);
    }
    UpdateRows();
    DrawDirtyRows();
    Refresh();
}

// Selects an item (clamped to the list; -1 below 0) and presses the button of its row if it is shown
void ItemListView::Select(int index) {
    if (mItemCount <= 0) {
        return;
    }
    if (index < 0) {
        mSelected = -1;
        return;
    }
    if (index > mItemCount - 1) {
        index = mItemCount - 1;
    }
    mSelected = index;
    for (int i = 0; i < mRowCount; i++) {
        func_02031e28(&mRowButtons[i], FALSE);
    }
    int top = mScroll / 20;
    if (top <= index && index < mRowCount + top) {
        int row = index % mRowCount;
        mTouchedRow = row;
        func_02031e28(&mRowButtons[row], TRUE);
        return;
    }
    mTouchedRow = -1;
}

// -- 0x020a9cc0: SetVisible (not decompiled) --

// Scrolls so that the given item is the top row, as far as the list goes
void ItemListView::ScrollTo(int top) {
    int max = mItemCount - mVisibleRows;
    if (max < 0) {
        max = 0;
    }
    top = MATH_CLAMP(top, 0, max);
    mScroll = top * 20;
    func_02032a0c(mScrollBar, 0, (mItemCount - mVisibleRows) * 20, mScroll);
    SetScroll(mScroll);
    DrawDirtyRows();
    Refresh();
}

void ItemListView::ScrollToSelected() {
    if (mItemCount <= mVisibleRows) {
        ScrollTo(0);
    } else if (mSelected + mVisibleRows <= mItemCount) {
        ScrollTo(mSelected);
    } else {
        ScrollTo(mItemCount - mVisibleRows);
    }
}

// Where the item with that id is in the list, or -1
int ItemListView::FindItem(u16 id) {
    int found = -1;
    for (int i = 0; i < mItemCount; i++) {
        if (id == mItems[i]->mId) {
            found = i;
            break;
        }
    }
    return found;
}

void ItemListView::ResetArrowButtons() {
    func_02031228(&mUpButton);
    func_02031228(&mDownButton);
}

void ItemListView::SetUnknown308(BOOL value) {
    *(BOOL *)(mScrollBar + 0xE4) = value;
    *(BOOL *)(mTouchArea + 0x24) = value;
}

// The item that a row shows: the rows are reused round, so row r shows the item at the scroll position that is r
// modulo the number of rows
int ItemListView::GetRowItem(int row) {
    int top = mScroll / 20;
    int first = mRowCount - 1 - row;
    return mRowCount * ((first + top) / mRowCount) + row;
}

BOOL ItemListView::CanScrollUp() {
    if (mItemCount > mVisibleRows && mScroll / 20 > 0) {
        return TRUE;
    }
    return FALSE;
}

BOOL ItemListView::CanScrollDown() {
    BOOL can = FALSE;
    if (mItemCount > mVisibleRows && mScroll / 20 < mItemCount - mVisibleRows) {
        can = TRUE;
    }
    return can;
}

// -- 0x020a9f54: ShopItemListView's set-up (not decompiled) --

void ShopItemListView::Draw() {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Draw(&mMarks[i]);
    }
    ItemListView::Draw();
}

void ShopItemListView::SetVisible(BOOL visible) {
    ItemListView::SetVisible(visible);
    for (int i = 0; i < mRowCount; i++) {
        mMarks[i].mVisible = visible;
    }
}

// -- 0x020aa08c: ShopItemListView::Refresh (not decompiled) --

void ShopItemListView::DrawRow(int row, int item) {
    ItemListView::DrawRow(row, item);
    mMarks[row].mVisible = mItems[item]->mOwned == FALSE ? TRUE : FALSE;
}

void ShopItemListView::ClearRow(int row) {
    ItemListView::ClearRow(row);
    mMarks[row].mVisible = FALSE;
}

// --- The shop staff and the item panel ------------------------------------------------------------------------

// The staff's animation for each of the four shops
int ShopStaff::GetShopAnimation(int shop) {
    int animation = 0;
    switch (shop) {
    case 0:
        animation = 0x87;
        break;
    case 1:
        animation = 0x73;
        break;
    case 2:
        animation = 0x97;
        break;
    case 3:
        animation = 0xC0;
        break;
    }
    return animation;
}

void ShopStaff::SetState(int state) {
    mState = state;
    mUnknown10 = 0;
    mUnknown1C = 0;
    mUnknown20 = 0;
}

BOOL ShopStaff::IsIdle() {
    BOOL idle = FALSE;
    if (!mTalking) {
        BOOL waiting = TRUE;
        if (mState != 0 && mState != 1) {
            waiting = FALSE;
        }
        if (waiting) {
            idle = TRUE;
        }
    }
    return idle;
}

void ShopItemPanel::SetY(int y) {
    fx32 y0 = (y + 0xA0) << FX32_SHIFT;
    fx32 y1 = (y + 0x60) << FX32_SHIFT;
    fx32 y2 = (y + 0xAC) << FX32_SHIFT;
    mY0C8 = y0;
    mY6B4 = y1;
    mY760 = y1;
    mY810 = y2;
    mYB80 = y0;
    mY9C8 = y2;
    mYD38 = y0;
}

void ShopItemPanel::SetUnknown14(int value) {
    mUnknown014 = value;
    if (mState <= 1) {
        mUnknown718 = mUnknown014;
    }
}

void ShopItemPanel::Show(int kind, int item) {
    mKind = kind;
    SetItem(item);
}

void ShopCounter::MountStaff() {
    mStaff.Mount();
}

void ShopCounter::UnmountStaff() {
    mStaff.Unmount();
}

void ShopCounter::ShowPanel(int kind, int item) {
    mPanelShown = TRUE;
    mPanel.Show(kind, item);
}

void ShopCounter::ClearPanelItem() {
    mPanel.SetItem(-1);
}

void ShopCounter::SayMessagesCA() {
    mStaff.mMessages[0] = 0xCA;
    mStaff.mMessages[1] = 0xCB;
    mStaff.mTalking = TRUE;
    mTalking = TRUE;
}

void ShopCounter::SayMessagesCE() {
    mStaff.mMessages[0] = 0xCE;
    mStaff.mMessages[1] = 0xCF;
    mStaff.mTalking = TRUE;
}

void ShopCounter::SayMessagesD3() {
    mStaff.mMessages[0] = 0xD3;
    mStaff.mMessages[1] = 0xD4;
    mStaff.mTalking = TRUE;
}

void ShopCounter::SayMessagesD5() {
    mStaff.mMessages[0] = 0xD5;
    mStaff.mMessages[1] = 0xD6;
    mStaff.mTalking = TRUE;
}

void ShopCounter::SetStaffUnknown14() {
    mStaff.mUnknown14 = TRUE;
}

BOOL ShopCounter::IsStaffIdle() {
    return mStaff.IsIdle();
}

extern "C" {
extern struct GameData gGameData;
extern MATHRandContext16 gRandom;
BOOL func_0202c354(GameData *game, int index);
u8 func_0202c468(GameData *game, int index); // The character's event
u8 func_0202c490(GameData *game, int index); // The place of its event
}

// Picks at random one of the residents whose event (0x14) is at the given place, for the staff to be
BOOL FindResidentAt(int place, BOOL *found, int *index) {
    int candidates[100];
    int count = 0;
    *found = FALSE;
    *index = 0;
    for (int i = 0; i < 100; i++) {
        if (func_0202c354(&gGameData, i) && func_0202c468(&gGameData, i) == 0x14 &&
            place == func_0202c490(&gGameData, i)) {
            candidates[count] = i;
            count++;
        }
    }
    if (count != 0) {
        *found = TRUE;
        *index = candidates[MATH_Rand16(&gRandom, count)];
    } else {
        *found = FALSE;
    }
    return *found;
}
