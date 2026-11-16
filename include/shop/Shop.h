#ifndef SHOP_SHOP_H
#define SHOP_SHOP_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"
#include "ScrollBar.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "item/Mannequin.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// The classes of src/shop/Shop.cpp that scenes hold as members: the scrolling item list (ItemListView, virtual
// table at 0x021153B4; ShopItemListView, 0x02115398), and the shop counter with its staff and item panel. Their
// constructors are inline in the scenes' constructors (the list's below, as scene 0x1E's constructor at 0x0203F900
// shows them). The price window is in shop/ShopWindow.h.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// --- The list ---

// The scrolling list of items of the lower screen. Each of its rows shows the item at the scroll position plus the
// row, as a button with the item's name and, by the mode, a price or counts; the rows' cached state tells which need
// to be drawn again.
class ItemListView {
public:
    // What the rows show beside the names (mMode)
    enum Mode {
        MODE_NAME,  // Only the name
        MODE_PRICE, // The price, greyed when the money cannot pay for it
        MODE_COUNT, // "×" and the item's count, greyed when it has none of mUnknown0C
        MODE_BOTH,  // mUnknown0C " / " the count, greyed as in MODE_COUNT
    };

    ItemListView() {}

    virtual void Draw();
    virtual void SetVisible(BOOL visible);
    virtual void Refresh();
    virtual void DrawRow(int row, int item);
    virtual void ClearRow(int row);

    void Init(int rows);
    void Init();
    void SetBackground(int plane, int offset);
    void InitRows(int resource, u16 sequence, u16 pressedSequence, int *charOffset, ScreenPoint position);
    void InitRows(int resource, u16 sequence, u16 pressedSequence, int *charOffset);
    void InitPrices(int resource, int sequence, int *charOffset);
    void InitCounts(int *charOffset);
    void InitUpButton(int resource, u16 sequence, u16 pressedSequence, ScreenPoint position, ScreenPoint size);
    void InitUpButton(int resource, u16 sequence, u16 pressedSequence);
    void InitDownButton(int resource, u16 sequence, u16 pressedSequence, ScreenPoint position, ScreenPoint size);
    void InitDownButton(int resource, u16 sequence, u16 pressedSequence);
    void InitScrollBar(int resource, int sequence, int knobSequence, int a, int b, int c, ScreenPoint position,
                       ScreenRect knob);
    void InitScrollBar(int resource, int sequence, int knobSequence);
    void SetUpScrollBar(int a, int b, int c);
    void Update();
    void UpdateShownRows();
    void DrawDirtyRows();
    void UpdateRows();
    void SetMode(int mode);
    void SetScroll(int scroll);
    void UpdateRowPalette(int row, int item);
    void UpdateRowValue(int row, int item);
    void UpdateRowCount(int row, int item);
    void SetMessageFile(MessageFile *file);
    void SetItems(Item **items, int count);
    void Select(int index);
    void ScrollTo(int top);
    void ScrollToSelected();
    int FindItem(u16 id);
    void ResetArrowButtons();
    void SetUnknown308(BOOL value);
    int GetRowItem(int row);
    BOOL CanScrollUp();
    BOOL CanScrollDown();

    /* 0x00000 */ // The table
    /* 0x00004 */ int mScrolling;      // 1 or 2 while an arrow button scrolls the list up or down a row
    /* 0x00008 */ int mMode;
    /* 0x0000C */ Item **mItems;
    /* 0x00010 */ int mItemCount;
    /* 0x00014 */ int *mMoney;
    /* 0x00018 */ BOOL mTouched;       // An item was touched
    /* 0x0001C */ int mTouchedRow;
    /* 0x00020 */ int mSelected;       // The item touched, or -1
    /* 0x00024 */ BOOL mTouchable;     // The rows take touches
    /* 0x00028 */ int mScroll;         // In pixels; a row is 20
    /* 0x0002C */ ScreenRect mRect;    // Where the rows are, for the touch area
    /* 0x0003C */ SpriteButton mUpButton;
    /* 0x00130 */ SpriteButton mDownButton;
    /* 0x00224 */ ScrollBar mScrollBar;
    /* 0x00360 */ TouchScrollArea mTouchArea;
    /* 0x003BC */ BOOL mRowDirty[7];
    /* 0x003D8 */ int mRowCount;        // The rows drawn: one more than the visible ones
    /* 0x003DC */ int mVisibleRows;
    /* 0x003E0 */ int mBgPlane;         // The background that scrolls with the list
    /* 0x003E4 */ int mBgOffset;
    /* 0x003E8 */ ScreenPoint mPosition; // Of the first row
    /* 0x003F0 */ int mRowItemIds[7];   // The id of the item a row shows, -1 for none
    /* 0x0040C */ int mRowPalettes[7];  // 8, or 9 when greyed
    /* 0x00428 */ int mRowValues[7];
    /* 0x00444 */ int mRowCounts[7];
    /* 0x00460 */ SpriteButton mRowButtons[7];
    /* 0x00B0C */ MessageText mRowTexts[7];      // The items' names
    /* 0x0349C */ ObjTextCanvas mRowPrices[7];   // MODE_PRICE
    /* 0x040A4 */ Sprite mRowPriceIcons[7];      // MODE_PRICE
    /* 0x04558 */ ObjTextCanvas mRowCounts2[7];  // MODE_COUNT and MODE_BOTH
    /* 0x05160 */ u8 mRowTextBuffers[7][0x2000];
    /* 0x13160 */ u8 mRowPriceBuffers[7][0x100];
    /* 0x13860 */ u8 mRowCountBuffers[7][0x100];
    /* 0x13F60 */ int mUnknown13F60;
}; // 0x13F64

// The shops' list: ItemListView with a mark beside each row whose item the player has never had (virtual table at
// 0x02115398)
class ShopItemListView : public ItemListView {
public:
    ShopItemListView() {}

    virtual void Draw();
    virtual void SetVisible(BOOL visible);
    virtual void Refresh();
    virtual void DrawRow(int row, int item);
    virtual void ClearRow(int row);

    void InitMarks(int resource, int sequence);

    /* 0x13F64 */ Sprite mMarks[7];
}; // 0x14418

// --- The shop counter ---

// The panel beside the staff that shows the item being talked about: its name, and the number the player has
// ("所持数") or its price ("売値", "円"). It slides in and out.
class ShopItemPanel {
public:
    // mState
    enum State {
        STATE_SHOWN,
        STATE_SLIDING_IN,
        STATE_SLIDING_OUT,
        STATE_HIDDEN,
    };

    void Init(int resource, int sequence, u16 frame, MessageFile *file, int *charOffset);
    void Draw();
    void Show(Item *item, int count);
    void SetItem(int count);
    void Update();
    void SetState(int state);
    void SetKind(int kind);
    void SetY(int y);
    void SetShowSecondIcon(BOOL show);
    int GetCountMessage(int count);

    /* 0x000 */ Item *mItem;
    /* 0x004 */ u32 mState;
    /* 0x008 */ int mFrame;       // Of the slide
    /* 0x00C */ int mKind;        // 1: the price; 0, 2, 3: the number owned
    /* 0x010 */ int mMessageBase; // The first item's name in the message file, by the kind
    /* 0x014 */ BOOL mShowSecondIcon;
    /* 0x018 */ u8 mUnknown018[4];
    /* 0x01C */ MessageText mName;
    /* 0x60C */ Sprite mIcon;
    /* 0x6B8 */ Sprite mSecondIcon;
    /* 0x764 */ ObjTextCanvas mCount;
    /* 0x91C */ ObjTextCanvas mPrice;
    /* 0xAD4 */ ObjTextCanvas mCountLabel; // "所持数"
    /* 0xC8C */ ObjTextCanvas mPriceLabel; // "売値"
}; // 0xE44

// The shop's staff: the head staff (a mannequin wearing the shop's uniform, with the head staff's head), or a
// resident whose event places it in the shop, with what it says
class ShopStaff {
public:
    // mState
    enum State {
        STATE_IDLE,
        STATE_GREETING,
        STATE_TALKING,
        STATE_TURNING_BACK,
    };

    void Mount();
    void Unmount();
    void Load(int shop, VecFx32 position, u16 angle, u16 greetingAngle, TtsPlayer *player, SpeechWindow *window,
              MessageFile *file);
    void Update();
    void Draw();
    int GetUniform(int shop);
    void UpdateResident();
    void UpdateHeadStaff();
    void SetState(int state);
    BOOL IsIdle();
    void BowDown();
    void UpdateHeldModel();

    /* 0x0000 */ int mShop;          // 0 to 3
    /* 0x0004 */ int mResidentIndex;
    /* 0x0008 */ BOOL mResident;     // A resident works in the shop instead of the head staff
    /* 0x000C */ int mState;
    /* 0x0010 */ int mStep;
    /* 0x0014 */ BOOL mGreet;        // In STATE_IDLE: start STATE_GREETING
    /* 0x0018 */ BOOL mTalking;      // Say mMessages next
    /* 0x001C */ BOOL mAnimationDone;
    /* 0x0020 */ BOOL mSpeechDone;
    /* 0x0024 */ u32 mMessages[2];   // What the speech window shows, and what is spoken
    /* 0x002C */ VecFx32 mPosition;
    /* 0x0038 */ u16 mAngle;
    /* 0x003A */ u16 mGreetingAngle; // It turns to it to greet, and back to mAngle after
    /* 0x003C */ SceneCharacter mCharacter;
    /* 0x2B54 */ Mannequin mMannequin;
    /* 0x320C */ Model *mHeldModel;  // A model held in its right hand, or NULL
    /* 0x3210 */ TtsPlayer *mTtsPlayer;
    /* 0x3214 */ SpeechWindow *mSpeechWindow;
    /* 0x3218 */ MessageFile *mMessageFile;
}; // 0x321C

// The shop scenes' counter: the staff, its voice and speech window, the item panel and the 3D view they are in
class ShopCounter {
public:
    void Init(int *charOffset, MessageFile *file, VecFx32 cameraPosition, VecFx32 cameraTarget);
    void LoadStaff(int shop, VecFx32 position, u16 angle, u16 greetingAngle);
    void InitPanel(int kind, int resource, int sequence, u16 frame, int *charOffset, MessageFile *file);
    void Update();
    void Draw();
    void MountStaff();
    void UnmountStaff();
    void ShowPanel(Item *item, int count);
    void ClearPanelItem();
    void HidePanel();
    void SayMessagesCA();
    void SayMessagesCE();
    void SayMessagesD3();
    void SayMessagesD5();
    void GreetCustomer();
    BOOL IsStaffIdle();

    /* 0x00000 */ BOOL mPanelShown;
    /* 0x00004 */ MessageFile *mMessageFile;
    /* 0x00008 */ BOOL mTalking;
    /* 0x0000C */ TtsPlayer mTtsPlayer;
    /* 0x14950 */ SpeechWindow mSpeechWindow;
    /* 0x14F5C */ ShopStaff mStaff;
    /* 0x18178 */ ShopItemPanel mPanel;
}; // 0x18FBC

BOOL FindResidentAt(int place, BOOL *found, int *index);

#endif // SHOP_SHOP_H
