#ifndef LIST_MIILIST_H
#define LIST_MIILIST_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/TextCanvas.h"

// The list of the saved characters that the Mii list scene and the wireless scenes show (main program 0x020a03e8
// to 0x020a3c68, src/list/MiiList.cpp): the characters in an order, grouped on tabs, with a panel of buttons that
// shows a page of them. The names are descriptive: no name of these classes survives in the ROM. The classes have
// no constructors of their own: the scenes' constructors build their members in place.

struct WindowPoint; // A point or a size in pixels, by value (MessageWindow.h)

#define MII_LIST_SIZE 100
#define MII_LIST_GROUPS 17
#define MII_LIST_PAGE 8
#define MII_LIST_TABS 5

// The panel of the list's buttons (0x9478 bytes): a page of buttons with the characters' names, scroll buttons and
// a scroll bar.
struct MiiListPanel {
    void SetEntries(SavedCharacter *entries, int count);
    void SetMode(int mode);
    void Select(int entry);
    void ScrollTo(int scroll);
    void ScrollToSelection();
    void Draw();
    void ClearSelection();
    void ResetScrollButtons();
    void SetScrollUnknown(int value);

    /* 0x0000 */ int mUnknown0000;
    /* 0x0004 */ int mMode;            // 1 when the list is sorted by nickname
    /* 0x0008 */ SavedCharacter *mEntries; // What the panel shows
    /* 0x000C */ int mEntryCount;
    /* 0x0010 */ int mButtonCount;
    /* 0x0014 */ int mPageSize;        // How many entries a page shows
    /* 0x0018 */ int mUnknown0018;
    /* 0x001C */ int mUnknown001C;
    /* 0x0020 */ BOOL mUnknown0020;
    /* 0x0024 */ BOOL mTextDirty[MII_LIST_PAGE]; // The button's text is to be written again
    /* 0x0044 */ int mButtonEntries[MII_LIST_PAGE]; // -1 when a button shows nothing
    /* 0x0064 */ SpriteButton mButtons[MII_LIST_PAGE];
    /* 0x0804 */ Sprite mIcons[MII_LIST_PAGE];
    /* 0x0D64 */ Sprite mIcons2[MII_LIST_PAGE];
    /* 0x12C4 */ ObjTextBox mTexts[MII_LIST_PAGE];
    /* 0x20A4 */ u8 mTextBuffers[MII_LIST_PAGE][0xE00];
    /* 0x90A4 */ SpriteButton mUpButton;
    /* 0x9198 */ SpriteButton mDownButton;
    /* 0x928C */ u8 mScrollBar[0x13C];
    /* 0x93C8 */ u8 mUnknown93C8[0x5C];
    /* 0x9424 */ int mUnknown9424[8];
    /* 0x9444 */ int mUnknown9444[7];
    /* 0x9460 */ int mSelectedButton; // -1 for none
    /* 0x9464 */ int mSelected; // The entry selected, -1 for none
    /* 0x9468 */ BOOL mUnknown9468;
    /* 0x946C */ int mScroll;   // The first entry shown
    /* 0x9470 */ int mUnknown9470;
    /* 0x9474 */ int mUnknown9474;
}; // 0x9478

// One tab of the list: a button for each group.
struct MiiListTab {
    void Init(int tab);
    void AddButton(int resource, u8 sequence, u8 pressedSequence, WindowPoint position, WindowPoint size);
    void Reset();
    BOOL Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int button);

    /* 0x0000 */ int mTab;
    /* 0x0004 */ int mButtonCount;
    /* 0x0008 */ int mUnknown0008;
    /* 0x000C */ SpriteButton mButtons[MII_LIST_GROUPS];
    /* 0x1040 */ int mSelected; // The button selected
}; // 0x1044

// The tabs of the list (0x5158 bytes).
struct MiiListTabs {
    void InitTabs();
    void AddButtons(int tab, int resource, const u8 *sequences, const u8 *pressedSequences);
    void Reset();
    void Init(int resource, int sequence);
    BOOL Update();
    void Draw();
    void SetVisible(BOOL visible);
    void SetTab(int tab);

    /* 0x0000 */ int mCurrent; // The tab shown
    /* 0x0004 */ MiiListTab mTabs[MII_LIST_TABS];
}; // 0x5158

// The list (0x113A8 bytes): the saved characters, in the order of mSortKey, and in groups of that order.
struct MiiList {
    void Init(MiiListPanel *panel, MiiListTabs *tabs);
    void Draw();
    void Load();
    BOOL SelectIndex(u16 index);
    void ClearSelection();
    void ShowTabs(int sortKey, int group);
    void Sort(int sortKey, BOOL selectFirst);
    void SortByName();
    void SortByNickname();
    void SortByPersonality();
    void SortByBirthYear();
    void SortByBirthday();
    void SortBySex();
    void SortByBloodType();

    /* 0x00000 */ SavedCharacter mResidents[MII_LIST_SIZE];
    /* 0x11300 */ int mCount;
    /* 0x11304 */ u16 mSelectedIndex; // The selected character's mIndex, 0xFFFF for none
    /* 0x11306 */ u16 mSortKey;       // 0 to 6: name, nickname, sex, year of birth, birthday, blood type, personality
    /* 0x11308 */ int mGroupCounts[MII_LIST_GROUPS];
    /* 0x1134C */ int mGroupStarts[MII_LIST_GROUPS];
    /* 0x11390 */ int mSelected;     // The selected entry of mResidents, -1 for none
    /* 0x11394 */ int mGroup;        // The group shown
    /* 0x11398 */ BOOL mChanged;     // Set by Update when the selection changed
    /* 0x1139C */ BOOL mUnknown1139C;
    /* 0x113A0 */ MiiListPanel *mPanel;
    /* 0x113A4 */ MiiListTabs *mTabs;
}; // 0x113A8

#endif // LIST_MIILIST_H
