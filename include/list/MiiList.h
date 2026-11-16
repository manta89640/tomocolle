#ifndef LIST_MIILIST_H
#define LIST_MIILIST_H

#include <nitro.h>
#include <nnsys.h>

#include "MessageWindow.h"
#include "ScrollBar.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/TextCanvas.h"

// The list of the saved characters that the Mii list scene and the wireless scenes show (main program 0x020a03e8
// to 0x020a3c68, src/list/MiiList.cpp): the characters in an order, grouped on tabs, with a panel of buttons that
// shows a page of them. The names are descriptive: no name of these classes survives in the ROM. The classes have
// no constructors of their own: the scenes' constructors build their members in place.

#define MII_LIST_SIZE 100
#define MII_LIST_GROUPS 17
#define MII_LIST_PAGE 8
#define MII_LIST_TABS 5

// The panel of the list's buttons (0x9478 bytes): a page of buttons with the characters' names, scroll buttons and
// a scroll bar. The buttons are one more than a page and are used round: the button of an entry is the entry modulo
// their count (GetButtonEntry), so scrolling by a row only writes the button that comes into view.
struct MiiListPanel {
    // mScrolling: an arrow button is scrolling the list a row, in steps of four pixels
    enum Scrolling {
        SCROLLING_NONE,
        SCROLLING_UP,
        SCROLLING_DOWN,
    };

    void Init(int pageSize);
    void SetBackground(int plane, int offset);
    void InitButtons(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size);
    void InitTexts(int plane, WindowPoint position, int *charOffset);
    void InitIcons(int resource, int sequence, WindowPoint position);
    void InitIcons2(int resource, int sequence, WindowPoint position);
    void InitUpButton(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size);
    void InitDownButton(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size);
    void InitScrollBar(int resource, int sequence, int knobSequence, int a, int b, int c, WindowPoint position,
                       ScreenRect rect);
    void SetUpScrollBar(int a, int b, int c);
    void Update();
    void PlaceButtons();
    void ShowEntry(int button, int entry);
    void ShowEntries();
    void ShowEntryAbove();
    void ShowEntryBelow();
    void Draw();
    void SetEntries(SavedCharacter *entries, int count);
    void SetMode(int mode);
    void Select(int entry);
    void ScrollTo(int scroll);
    void ScrollToSelection();
    void ClearSelection();
    void ResetScrollButtons();
    void SetScrollUnknown(int value);
    int GetButtonEntry(int button);
    BOOL CanScrollUp();
    BOOL CanScrollDown();

    /* 0x0000 */ int mScrolling;
    /* 0x0004 */ int mMode;            // 1 when the list is sorted by nickname: the buttons show the Mii's name
    /* 0x0008 */ SavedCharacter *mEntries; // What the panel shows
    /* 0x000C */ int mEntryCount;
    /* 0x0010 */ int mButtonCount;     // A page and one
    /* 0x0014 */ int mPageSize;        // How many entries a page shows
    /* 0x0018 */ int mBgPlane;         // The background that scrolls with the rows, if mBgScrolls
    /* 0x001C */ int mBgOffset;
    /* 0x0020 */ BOOL mBgScrolls;
    /* 0x0024 */ BOOL mTextDirty[MII_LIST_PAGE]; // The button's text is to be written again
    /* 0x0044 */ int mButtonEntries[MII_LIST_PAGE]; // mIndex of the character a button shows, -1 for none
    /* 0x0064 */ SpriteButton mButtons[MII_LIST_PAGE];
    // A mark beside each button, shown when the character's names, creator's name, birthday and blood type are all
    // filled in
    /* 0x0804 */ Sprite mIcons[MII_LIST_PAGE];
    /* 0x0D64 */ Sprite mIcons2[MII_LIST_PAGE]; // A mark shown when the character's mUnknown050Bit0 is set
    /* 0x12C4 */ ObjTextBox mTexts[MII_LIST_PAGE];
    /* 0x20A4 */ u8 mTextBuffers[MII_LIST_PAGE][0xE00];
    /* 0x90A4 */ SpriteButton mUpButton;
    /* 0x9198 */ SpriteButton mDownButton;
    /* 0x928C */ ScrollBar mScrollBar;
    /* 0x93C8 */ TouchScrollArea mTouchArea; // Over the rows
    // Where the first row's button, marks and text are; each row is 20 pixels below the one before
    /* 0x9424 */ WindowPoint mButtonPosition;
    /* 0x942C */ WindowPoint mIconPosition;
    /* 0x9434 */ WindowPoint mIcon2Position;
    /* 0x943C */ WindowPoint mTextPosition;
    /* 0x9444 */ ScreenRect mRect;     // Of a page of rows, for touches and for the touch scroll area
    /* 0x9454 */ BOOL mIconsShown;
    /* 0x9458 */ BOOL mIcons2Shown;
    /* 0x945C */ BOOL mTouchable;      // The rows take touches
    /* 0x9460 */ int mSelectedButton;  // -1 for none
    /* 0x9464 */ int mSelected;        // The entry selected, -1 for none
    /* 0x9468 */ BOOL mTouched;        // Update set it: a row was touched and selected
    /* 0x946C */ int mScroll;          // The first entry shown
    /* 0x9470 */ int mScrollOffset;    // And how many pixels of a row (20) it is scrolled past that
    /* 0x9474 */ int mScrollPosition;  // In pixels, as the scroll bar or a drag last gave it
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
    void Update();
    void Draw();
    void Load();
    BOOL RemoveSelected();
    void ShowTabs(int sortKey, int group);
    void Sort(int sortKey, BOOL selectFirst);
    void SortByName();
    void SortByNickname();
    void SortByPersonality();
    void SortByBirthYear();
    void SortByBirthday();
    void SortBySex();
    void SortByBloodType();
    void SwapRuns(int first, int firstCount, int second, int secondCount);
    BOOL SelectIndex(u16 index);
    void ClearSelection();

    /* 0x00000 */ SavedCharacter mResidents[MII_LIST_SIZE];
    /* 0x11300 */ int mCount;
    /* 0x11304 */ u16 mSelectedIndex; // The selected character's mIndex, 0xFFFF for none
    /* 0x11306 */ u16 mSortKey;       // 0 to 6: name, nickname, sex, year of birth, birthday, blood type, personality
    /* 0x11308 */ int mGroupCounts[MII_LIST_GROUPS];
    /* 0x1134C */ int mGroupStarts[MII_LIST_GROUPS];
    /* 0x11390 */ int mSelected;     // The selected entry of mResidents, -1 for none
    /* 0x11394 */ int mGroup;        // The group shown
    /* 0x11398 */ BOOL mChanged;     // Set by Update when the selection changed
    /* 0x1139C */ BOOL mSelectInGroup; // Showing a group (Sort, a tab) selects its first character
    /* 0x113A0 */ MiiListPanel *mPanel;
    /* 0x113A4 */ MiiListTabs *mTabs;
}; // 0x113A8

// What removing a saved character does to the rest of the saved game (src/list/MiiList.cpp): the other characters'
// relations, long acts and troubles with it (in "list", the loaded characters, or the saved game's when NULL), the
// news and the pairs that name it, the songs, and its own record.
void EraseCharacter(int index, int count, SavedCharacter *list);
void ForgetCharacter(int index, int count, SavedCharacter *list);
void RemoveNewsOf(int index);
void RemovePairsOf(int index);
void RemoveFromSongs(int index);
void ClearRecord(int index);

#endif // LIST_MIILIST_H
