#ifndef SHOP_SHOP_H
#define SHOP_SHOP_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "SpriteButton.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// The classes of src/shop/Shop.cpp that scenes hold as members: the scrolling item list (ItemListView, virtual
// table at 0x021153B4; ShopItemListView, 0x02115398), and the shop counter with its staff and item panel. Their
// constructors are inline in the scenes' constructors (the list's below, as scene 0x1E's constructor at 0x0203F900
// shows them); the members are what the decompiled functions and the constructors show, the rest is padding of the
// measured sizes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

class MessageFile;

// An item as the list shows it (the item files' InventoryItem). A stand-in where include/item/Item.h is not
// included: that header's InventoryItem::GetPrice returns u32, and the list compares the price with the money as
// signed (0x020A8F7C: movle), so Shop.cpp keeps this one until that is settled. A file that includes Item.h first
// (the scene constructors) gets the real class.
#ifndef ITEM_ITEM_H
class InventoryItem {
public:
    virtual int GetPrice();
    virtual int GetSellPrice();

    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;  // Shown in a row's first number
    /* 0x10 */ int mCount;      // Shown in a row's second number
}; // 0x14
#else
class InventoryItem;
#endif

// The scrolling list. Each of its rows shows the item at the scroll position plus the row; the rows' cached state
// tells which need to be drawn again.
class ItemListView {
public:
    ItemListView() {}

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
    /* 0x003C */ SpriteButton mUpButton;
    /* 0x0130 */ SpriteButton mDownButton;
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
    /* 0x0460 */ SpriteButton mRowButtons[7];
    /* 0x0B0C */ MessageText mRowTexts[7];
    /* 0x349C */ ObjTextCanvas mRowCanvases[7];
    /* 0x40A4 */ u8 mUnknown40A4[0x4558 - 0x40A4];
    /* 0x4558 */ ObjTextCanvas mRowCanvases2[7];
    /* 0x5160 */ u8 mUnknown5160[0x13F60 - 0x5160];
    /* 0x13F60 */ int mUnknown13F60;
}; // 0x13F64

// The shops' list: ItemListView with a mark beside each row whose item the player does not have yet (virtual table
// at 0x02115398)
class ShopItemListView : public ItemListView {
public:
    ShopItemListView() {}

    virtual void Draw();
    virtual void SetVisible(BOOL visible);
    virtual void Refresh();
    virtual void DrawRow(int row, int item);
    virtual void ClearRow(int row);

    /* 0x13F64 */ Sprite mMarks[7];
}; // 0x14418

// The panel that shows an item's number owned ("所持数") and price ("売値", "円") beside the staff
class ShopItemPanel {
public:
    void Init(int a, int b, u16 c, int d, int e); // 0x020ab09c
    void Draw();
    void Show(int kind, int item);
    void SetItem(int item);
    void Update();
    void SetState(int state);
    void SetUnknown(int value);
    void SetY(int y);
    void SetUnknown14(int value);

    /* 0x000 */ int mKind;
    /* 0x004 */ u32 mState;
    /* 0x008 */ u8 mUnknown008[0xC];
    /* 0x014 */ int mUnknown014;
    /* 0x018 */ u8 mUnknown018[0xB0];
    /* 0x0C8 */ fx32 mY0C8;
    /* 0x0CC */ u8 mUnknown0CC[0x5E8];
    /* 0x6B4 */ fx32 mY6B4;
    /* 0x6B8 */ u8 mUnknown6B8[0x60];
    /* 0x718 */ int mUnknown718;
    /* 0x71C */ u8 mUnknown71C[0x44];
    /* 0x760 */ fx32 mY760;
    /* 0x764 */ u8 mUnknown764[0xAC];
    /* 0x810 */ fx32 mY810;
    /* 0x814 */ u8 mUnknown814[0x1B4];
    /* 0x9C8 */ fx32 mY9C8;
    /* 0x9CC */ u8 mUnknown9CC[0x1B4];
    /* 0xB80 */ fx32 mYB80;
    /* 0xB84 */ u8 mUnknownB84[0x1B4];
    /* 0xD38 */ fx32 mYD38;
};

// The shop's staff: the head staff's model, or a resident who works in the shop, with its speech
class ShopStaff {
public:
    void Mount();       // 0x020abc4c: mounts the head staff's archives ("3HM", "3CM", "3HA", "3CH")
    void Unmount();     // 0x020abd04
    void Load(int a);   // 0x020abd84
    void Update();
    void Draw();
    int GetShopAnimation(int shop);
    void UpdateResident(); // 0x020ac20c
    void UpdateStaff();    // 0x020ac574
    void SetState(int state);
    BOOL IsIdle();

    /* 0x00 */ int mShop;          // 0 to 3
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ BOOL mResident;     // A resident works in the shop instead of the head staff
    /* 0x0C */ int mState;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ int mUnknown14;
    /* 0x18 */ BOOL mTalking;
    /* 0x1C */ int mUnknown1C;
    /* 0x20 */ int mUnknown20;
    /* 0x24 */ int mMessages[2];   // The two messages it says next
};

// The shop scenes' counter: the staff, its speech (speech player at 0xC, speech window at 0x14950) and the panel
class ShopCounter {
public:
    void MountStaff();
    void UnmountStaff();
    void ShowPanel(int kind, int item);
    void ClearPanelItem();
    void SayMessagesCA();
    void SayMessagesCE();
    void SayMessagesD3();
    void SayMessagesD5();
    void SetStaffUnknown14();
    BOOL IsStaffIdle();

    /* 0x00000 */ BOOL mPanelShown;
    /* 0x00004 */ int mUnknown00004;
    /* 0x00008 */ BOOL mTalking;
    /* 0x0000C */ u8 mUnknown0000C[0x14F5C - 0xC];
    /* 0x14F5C */ ShopStaff mStaff;
    /* 0x14F88 */ u8 mUnknown14F88[0x18178 - 0x14F88];
    /* 0x18178 */ ShopItemPanel mPanel;
};

#endif // SHOP_SHOP_H
