#ifndef OV092_LYRICLIST_H
#define OV092_LYRICLIST_H

#include <nitro.h>

#include "ScrollBar.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "text/TextCanvas.h"

// The list of words that the song writing scene offers for a line of a song (overlay 92, 0x02149A40 to 0x0214B794):
// the song words of the song messages ("WLS:tmd_song_msg_JP.bmg", messages 0x58 on, with their readings 0x467 on),
// the names of the saved characters, and the items listed in the inventory ("WLI:tmd_item_msg_JP.bmg"). The list
// keeps those of the syllable count the line asks for, grouped by their kana initial, with a panel of buttons and a
// row of tabs: the same design as the main program's list of characters (include/list/MiiList.h), with the words
// in place of the characters.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// What a word is
enum LyricWordKind {
    LYRIC_WORD_SONG,        // A word of the song messages
    LYRIC_WORD_FAMILY_NAME, // A saved character's mFamilyName
    LYRIC_WORD_NAME,        // Its mName
    LYRIC_WORD_MII_NAME,    // Its Mii's name
    LYRIC_WORD_MOTTO,       // Its mMotto
    LYRIC_WORD_ITEM,        // An item's name
};

// One word of the list (0x30 bytes)
struct LyricWord {
    void Clear();
    BOOL SetFromText(u16 id, u8 kind, u16 source, const wchar_t *text);
    BOOL SetFromTexts(u16 id, u8 kind, u16 source, const wchar_t *text, wchar_t *reading);
    void UpdateShown();
    const wchar_t *GetShownText();
    LyricWord &operator=(const LyricWord &other);

    /* 0x00 */ u16 mId;          // Its place in the list as it was built
    /* 0x02 */ u8 mSyllables;    // 2 to 6
    /* 0x03 */ u8 mKind;         // LyricWordKind
    /* 0x04 */ wchar_t mText[10];
    /* 0x18 */ wchar_t mReading[10]; // In katakana, as the speech engine reads it
    /* 0x2C */ u16 mSource;      // The message, character or item it was made from
    /* 0x2E */ u8 mShowReading;  // GetShownText gives the reading: the items, but three of them
}; // 0x30

#define LYRIC_LIST_SIZE 2000
#define LYRIC_LIST_GROUPS 12
#define LYRIC_LIST_BUTTONS 7
#define LYRIC_LIST_PAGE 6
#define LYRIC_LIST_TABS 16

// The panel of the list's buttons: seven buttons used round, scroll buttons and a scroll bar (as MiiListPanel)
struct LyricWordPanel {
    enum Scrolling {
        SCROLLING_NONE,
        SCROLLING_UP,
        SCROLLING_DOWN,
    };

    void Init();
    void SetBackground(int plane, int offset);
    void InitButtons(int resource, u16 sequence, u16 pressedSequence, int *charOffset);
    void InitUpButton(int resource, u16 sequence, u16 pressedSequence);
    void InitDownButton(int resource, u16 sequence, u16 pressedSequence);
    void InitScrollBar(int resource, int sequence, int knobSequence);
    void SetUpScrollBar(int a, int b, int c);
    void Update();
    void Draw();
    void PlaceButtons();
    void ShowEntries();
    void RefreshEntries();
    void SetScroll(int position);
    void SetEntries(LyricWord **entries, int count);

    // The first row shown
    int GetScroll() { return mScrollPosition / 20; }
    // The entry a button shows: the one of the rows from the first shown that is the button modulo the buttons'
    // count
    int GetButtonEntry(int button) {
        return button + LYRIC_LIST_BUTTONS * ((LYRIC_LIST_BUTTONS - 1 - button + GetScroll()) / LYRIC_LIST_BUTTONS);
    }
    BOOL CanScrollDown() {
        BOOL result = FALSE;
        if (mEntryCount > LYRIC_LIST_PAGE && GetScroll() < mEntryCount - LYRIC_LIST_PAGE) {
            result = TRUE;
        }
        return result;
    }

    /* 0x0000 */ int mScrolling;
    /* 0x0004 */ LyricWord **mEntries;
    /* 0x0008 */ int mEntryCount;
    /* 0x000C */ int mBgPlane;
    /* 0x0010 */ int mBgOffset;
    /* 0x0014 */ BOOL mTouched;
    /* 0x0018 */ int mSelectedButton;
    /* 0x001C */ int mSelected;
    /* 0x0020 */ BOOL mTouchable;
    /* 0x0024 */ int mScrollPosition; // In pixels, 20 a row
    /* 0x0028 */ ScreenRect mRect;
    /* 0x0038 */ SpriteButton mUpButton;
    /* 0x012C */ SpriteButton mDownButton;
    /* 0x0220 */ ScrollBar mScrollBar;
    /* 0x035C */ TouchScrollArea mTouchArea;
    /* 0x03B8 */ BOOL mTextDirty[LYRIC_LIST_BUTTONS];
    /* 0x03D4 */ int mButtonEntries[LYRIC_LIST_BUTTONS]; // mId of the word a button shows, -1 for none
    /* 0x03F0 */ SpriteButton mButtons[LYRIC_LIST_BUTTONS];
    /* 0x0A9C */ ObjTextBox mTexts[LYRIC_LIST_BUTTONS];
    /* 0x16C0 */ u8 mTextBuffers[LYRIC_LIST_BUTTONS][0x3200];
}; // 0x174C0

// The tabs of the list: a button for each group
struct LyricWordTabs {
    void Init(int unknown);
    void AddButton(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size);
    void Reset();
    BOOL Update();
    void Draw();
    void Select(int button);

    /* 0x000 */ int mUnknown000;
    /* 0x004 */ int mButtonCount;
    /* 0x008 */ int mUnknown008;
    /* 0x00C */ SpriteButton mButtons[LYRIC_LIST_TABS];
    /* 0xF4C */ int mSelected;
}; // 0xF50

// The list
struct LyricWordList {
    static void MountArchives();
    void Init(LyricWordPanel *panel, LyricWordTabs *tabs);
    void Update();
    void Draw();
    static void UnmountArchives();
    void Load();
    void AddSongWords();
    void AddCharacterNames();
    void AddItems();
    void SwapRuns(int first, int firstCount, int second, int secondCount);
    void Filter(int syllables);
    void ShowFirstGroup();
    static int Compare(const LyricWord *a, const LyricWord *b);

    /* 0x00000 */ LyricWord mEntries[LYRIC_LIST_SIZE];
    /* 0x17700 */ LyricWord *mShown[LYRIC_LIST_SIZE]; // The words of the syllable count asked for
    /* 0x19640 */ int mCount;
    /* 0x19644 */ int mShownCount;
    /* 0x19648 */ int mGroupCounts[LYRIC_LIST_GROUPS];
    /* 0x19678 */ int mGroupStarts[LYRIC_LIST_GROUPS];
    /* 0x196A8 */ int mSelected; // In mShown, -1 for none
    /* 0x196AC */ int mGroup;
    /* 0x196B0 */ BOOL mChanged;
    /* 0x196B4 */ LyricWordPanel *mPanel;
    /* 0x196B8 */ LyricWordTabs *mTabs;
}; // 0x196BC

#endif // OV092_LYRICLIST_H
