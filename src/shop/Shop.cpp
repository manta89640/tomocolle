#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"

// The shops' shared code: the scrolling item list of the lower screen ("LSL:/BG_list", "LTL:/Obj_list", ...), which
// the shops (overlays 56, 68, 83 to 85) and other scenes with lists of items (75, 86, 100, 110) use, the shops'
// price window, and the shop staff's model and speech.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

class MessageFile;

// An item as the list shows it (the item files' InventoryItem)
class InventoryItem {
public:
    virtual int GetPrice();
    virtual int GetSellPrice();

    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;  // Shown in a row's first number
    /* 0x10 */ int mCount;      // Shown in a row's second number
}; // 0x14

// A touch button of the main program (include/SpriteButton.h); only its size matters here
struct ListButton {
    /* 0x00 */ u8 mUnknown00[0xF4];
}; // 0xF4

// A row's text (the main program's MessageText, 0x5F0 bytes)
struct RowText {
    /* 0x000 */ u8 mUnknown000[0x154];
    /* 0x154 */ MessageFile *mMessageFile;
    /* 0x158 */ u8 mUnknown158[0x5F0 - 0x158];
}; // 0x5F0

// The scrolling list. Each of its rows shows the item at the scroll position plus the row; the rows' cached state
// tells which need to be drawn again.
class ItemListView {
public:
    virtual void Draw();
    virtual void SetVisible(BOOL visible);
    virtual void Refresh();
    virtual void DrawRow(int row, int item);
    virtual void ClearRow(int row);

    void Init(int rows);
    void Init();
    void SetBackground(int plane, int offset);
    void SetUpScrollBar(int a, int b, int c);
    void SetMode(int mode);
    void SetMessageFile(MessageFile *file);
    void DrawDirtyRows();
    void UpdateRows();
    void SetScroll(int scroll);
    void UpdateRowPalette(int row, int item);
    void UpdateRowValue(int row, int item);
    void UpdateRowCount(int row, int item);
    void SetItems(InventoryItem **items, int count);
    void Select(int index);
    void ScrollTo(int top);
    void ScrollToSelected();
    int FindItem(u16 id);
    void ResetArrowButtons();
    void SetUnknown308(BOOL value);
    int GetRowItem(int row);
    BOOL CanScrollUp();
    BOOL CanScrollDown();

    /* 0x0000 */ int mUnknown0004;
    /* 0x0008 */ int mMode;            // 1: rows the money cannot pay for are greyed; 2, 3: rows of items not had
    /* 0x000C */ InventoryItem **mItems;
    /* 0x0010 */ int mItemCount;
    /* 0x0014 */ int *mMoney;
    /* 0x0018 */ BOOL mTouched;        // An item was touched
    /* 0x001C */ int mTouchedRow;
    /* 0x0020 */ int mSelected;        // The item touched, or -1
    /* 0x0024 */ BOOL mUnknown0024;
    /* 0x0028 */ int mScroll;          // In pixels; a row is 20
    /* 0x002C */ int mRectX;
    /* 0x0030 */ int mRectY;
    /* 0x0034 */ int mRectWidth;
    /* 0x0038 */ int mRectHeight;
    /* 0x003C */ ListButton mUpButton;
    /* 0x0130 */ ListButton mDownButton;
    /* 0x0224 */ u8 mScrollBar[0x13C];
    /* 0x0360 */ u8 mTouchArea[0x5C];
    /* 0x03BC */ BOOL mRowDirty[7];
    /* 0x03D8 */ int mRowCount;        // The rows drawn: one more than the visible ones
    /* 0x03DC */ int mVisibleRows;
    /* 0x03E0 */ int mBgPlane;         // The background that scrolls with the list
    /* 0x03E4 */ int mBgOffset;
    /* 0x03E8 */ int mUnknown03E8;
    /* 0x03EC */ int mUnknown03EC;
    /* 0x03F0 */ int mRowItemIds[7];   // The id of the item a row shows, -1 for none
    /* 0x040C */ int mRowPalettes[7];  // 8, or 9 when greyed
    /* 0x0428 */ int mRowValues[7];
    /* 0x0444 */ int mRowCounts[7];
    /* 0x0460 */ ListButton mRowButtons[7];
    /* 0x0B0C */ RowText mRowTexts[7];
    /* 0x349C */ u8 mUnknown349C[0x13F60 - 0x349C];
    /* 0x13F60 */ int mUnknown13F60;
}; // 0x13F64

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
