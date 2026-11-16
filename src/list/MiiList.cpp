#include "list/MiiList.h"

#include <wchar.h>

#include "GameData.h"
#include "song/Song.h"
#include "sound/SoundManager.h"

// The list of saved characters (main program 0x020a03e8 to 0x020a3c68): the list itself, what removing one of
// them does to the saved game, the panel of the list's buttons and its tabs.

// The touch screen as the main program keeps it (gTouchInput), as far as the panel reads it
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x28];
    /* 0x28 */ TPData mTp;
    /* 0x30 */ ScreenPoint mLastPoint;
    /* 0x38 */ ScreenPoint mPoint;
};

// One of the eleven news items of the saved game (func_0202c8a8 copies one out, func_0202c8bc in), as far as
// removing a character reads it
struct NewsItem {
    /* 0x00 */ u8 mKind;          // An entry of data_020fa1d0
    /* 0x01 */ u8 mCharacters[3]; // mIndex of the characters it is about
    /* 0x04 */ u8 mUnknown04[8];
    /* 0x0C */ u32 mValid : 1;
    /*      */ u32 mUnknown0CBits1 : 31;
    /* 0x10 */ u8 mUnknown10[4];
}; // 0x14

// What a news kind is (data_020fa1d0, 0x20 bytes each)
struct NewsKind {
    /* 0x00 */ u8 mUnknown00[0x1C];
    /* 0x1C */ u32 mUnknown1CBits0 : 8;
    /*      */ u32 mCharacterCount : 4; // How many characters the news is about, less two
    /*      */ u32 mUnknown1CBits12 : 20;
}; // 0x20

// The two tables of 100 pairs of characters that the saved game keeps at gGameData + 0x155E4 and + 0x154B8
struct CharacterPair {
    /* 0x0 */ u8 mCharacters[2]; // mIndex of the two
    /* 0x2 */ s8 mState;         // -1 when the entry is empty
};

struct GameDataPairs {
    /* 0x00000 */ u8 mUnknown00000[0x154B8];
    /* 0x154B8 */ CharacterPair mPairsB[100];
    /* 0x155E4 */ CharacterPair mPairsA[100];
};

// SavedCharacter as the list copies it: the record in one block, then the class's own fields. The class's implicit
// assignment copies its base, the record, field by field; the original's copies are those of a structure that holds
// the record as a member.
struct SavedCharacterCopy {
    /* 0x000 */ CharacterRecord mRecord;
    /* 0x2A4 */ u16 mIndex;
    /* 0x2A6 */ s16 mReaction;
    /* 0x2A8 */ u32 mUnknown2A8[3];
    /* 0x2B4 */ int mUnknown2B4;
    /* 0x2B8 */ int mUnknown2B8;
    /* 0x2BC */ s8 mPersonality;
    /* 0x2BD */ u8 mUnknown2BD[3];
}; // 0x2C0

#define COPY_CHARACTER(dst, src) (*(SavedCharacterCopy *)&(dst) = *(SavedCharacterCopy *)&(src))

extern "C" {
extern TouchInput gTouchInput;
extern const NewsKind data_020fa1d0[];
extern u8 data_02121834[]; // The background screens' loader
BOOL func_0202c354(GameData *game, int index);                          // Whether there is a saved character
void func_0202c264(GameData *game, int index, CharacterRecord *record); // Reads a saved character
void func_0202c28c(GameData *game, int index, SavedCharacter *character); // Writes one back
void func_0202c2fc(GameData *game, int index);                          // Marks its slot free
BOOL func_0202c3dc(GameData *game, int index);                          // Whether the saved character exists
void func_0202c8a8(GameData *game, int index, NewsItem *item);
void func_0202c8bc(GameData *game, int index, NewsItem *item);
int func_0202ca34(GameData *game, int song, int *slot);
void func_02028154(void *screens, int plane, int offset); // Scrolls a background
int MiiData_GetSex(const CharacterRecord *character);
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *name);
void func_02031228(void *sprite);
}

// The tabs' buttons: how many each tab has, their size, and where they are
static const WindowPoint sTab3Positions[2] = {{0x24, 0x13}, {0x68, 0x13}};
static const int sTabButtonCounts[MII_LIST_TABS] = {11, 17, 13, 2, 5};
static const WindowPoint sTabButtonSizes[MII_LIST_TABS] = {{0x1A, 0x10}, {0x10, 0x10}, {0x1A, 0x10}, {0x40, 0x1A}, {0x20, 0x1A}};
static const WindowPoint sTab4Positions[5] = {{0x14, 0x13}, {0x36, 0x13}, {0x58, 0x13}, {0x7A, 0x13}, {0x9C, 0x13}};
static const WindowPoint sTab0Positions[11] = {
    {0x11, 0x0A}, {0x2D, 0x0A}, {0x49, 0x0A}, {0x65, 0x0A}, {0x81, 0x0A}, {0x11, 0x1C},
    {0x2D, 0x1C}, {0x49, 0x1C}, {0x65, 0x1C}, {0x81, 0x1C}, {0x9D, 0x1C},
};
static const WindowPoint sTab2Positions[13] = {
    {0x11, 0x0A}, {0x2D, 0x0A}, {0x49, 0x0A}, {0x65, 0x0A}, {0x81, 0x0A}, {0x9D, 0x0A}, {0x11, 0x1C},
    {0x2D, 0x1C}, {0x49, 0x1C}, {0x65, 0x1C}, {0x81, 0x1C}, {0x9D, 0x1C}, {0xB9, 0x1C},
};
static const WindowPoint sTab1Positions[17] = {
    {0x66, 0x1C}, {0x7A, 0x1C}, {0x90, 0x1C}, {0xA4, 0x1C}, {0x66, 0x0A}, {0x7A, 0x0A}, {0x90, 0x0A}, {0xA4, 0x0A},
    {0x12, 0x1C}, {0x26, 0x1C}, {0x3C, 0x1C}, {0x50, 0x1C}, {0x12, 0x0A}, {0x26, 0x0A}, {0x3C, 0x0A}, {0x50, 0x0A},
    {0xBA, 0x1C},
};
static const WindowPoint *sTabButtonPositions[MII_LIST_TABS] = {
    sTab0Positions, sTab1Positions, sTab2Positions, sTab3Positions, sTab4Positions,
};

#define SORT_BUFFER_SIZE 200

void MiiList::Init(MiiListPanel *panel, MiiListTabs *tabs) {
    mCount = 0;
    mSelectedIndex = 0xFFFF;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    mSelected = -1;
    mGroup = 0;
    mChanged = FALSE;
    mSelectInGroup = TRUE;
    mPanel = panel;
    mTabs = tabs;
}

void MiiList::Update() {
    mChanged = FALSE;
    if (mTabs->Update()) {
        int selected = mSelected;
        mGroup = mTabs->mTabs[mTabs->mCurrent].mSelected;
        mPanel->SetEntries(&mResidents[mGroupStarts[mGroup]], mGroupCounts[mGroup]);
        if (mGroupCounts[mGroup] != 0 && mSelectInGroup) {
            mPanel->Select(0);
            mPanel->ScrollToSelection();
            mSelected = mGroupStarts[mGroup];
            mSelectedIndex = mResidents[mSelected].mIndex;
            if (selected != mSelected) {
                mChanged = TRUE;
            }
        }
    }
    mPanel->Update();
    if (mPanel->mTouched && mPanel->mSelected != -1) {
        mChanged = TRUE;
        int start = mGroupStarts[mGroup];
        mSelected = start + mPanel->mSelected;
        mSelectedIndex = mResidents[mSelected].mIndex;
    }
}

void MiiList::Draw() {
    mTabs->Draw();
    mPanel->Draw();
}

void MiiList::Load() {
    mCount = 0;
    for (u16 i = 0; i < MII_LIST_SIZE; i++) {
        if (func_0202c354(&gGameData, i)) {
            func_0202c264(&gGameData, i, &mResidents[mCount]);
            mResidents[mCount].mIndex = i;
            mCount++;
        }
    }
}

// Removes the selected character from the list and from the saved game. The first saved character (the player's
// own) cannot be removed.
BOOL MiiList::RemoveSelected() {
    int next;
    int scroll;
    u16 i;
    u16 index = mSelectedIndex;
    int selected = mSelected;
    if (index == 0) {
        return FALSE;
    }
    int group = 0;
    int count;
    int last;
    for (;;) {
        count = mGroupCounts[group];
        last = mGroupStarts[group] + mGroupCounts[group] - 1;
        if (!(mGroupStarts[group] > selected || selected > last)) {
            break;
        }
        group++;
    }
    if (count == 1) {
        // The group empties: show the one before it, or after it if it is the first
        int step = -1;
        next = group - 1;
        if (next < 0) {
            next = group + 1;
            step = 1;
        }
        for (;;) {
            if (mGroupCounts[next] != 0) {
                break;
            }
            if (next == 0) {
                next = group;
                step = 1;
            }
            next += step;
        }
        mSelected = mGroupStarts[next];
        if (group < next) {
            mSelected--;
        }
        scroll = 0;
        mGroupCounts[group] = 0;
    } else {
        scroll = mPanel->mScroll;
        next = group;
        if (selected == last) {
            mSelected--;
        }
        mGroupCounts[group]--;
    }
    for (i = 0; i < mCount; i++) {
        if (selected < i) {
            COPY_CHARACTER((&mResidents[i])[-1], mResidents[i]);
        }
    }
    mCount--;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        if (group < i) {
            mGroupStarts[i]--;
        }
    }
    mGroup = next;
    mPanel->SetEntries(&mResidents[mGroupStarts[next]], mGroupCounts[next]);
    mPanel->Select(mSelected - mGroupStarts[next]);
    mPanel->ScrollTo(scroll);
    mSelectedIndex = mResidents[mSelected].mIndex;
    ShowTabs(mSortKey, mGroup);
    EraseCharacter(index, mCount, mResidents);
    return TRUE;
}

void EraseCharacter(int index, int count, SavedCharacter *list) {
    ForgetCharacter(index, count, list);
    RemoveNewsOf(index);
    RemovePairsOf(index);
    RemoveFromSongs(index);
    ClearRecord(index);
}

// Clears the other characters' relations with the character, and their long acts and troubles with it, in the
// saved game
void ForgetCharacter(int index, int count, SavedCharacter *list) {
    SavedCharacter loaded;
    SavedCharacter *characters = list != NULL ? list : &loaded;
    for (int i = 0; i < count; i++) {
        u16 other;
        int n;
        if (list != NULL) {
            n = i;
            other = characters[i].mIndex;
        } else {
            other = i;
            n = 0;
            if (!func_0202c3dc(&gGameData, other)) {
                continue;
            }
            func_0202c264(&gGameData, other, &loaded);
            loaded.mIndex = other;
        }
        if (other == index) {
            continue;
        }
        if (characters[n].mRelationMeter[index] != 100 || characters[n].mRelationRank[index] != 0) {
            characters[n].SetRelation(index, 0, 100);
            func_0202c28c(&gGameData, other, &characters[n]);
        }
        SavedCharacter *character = &characters[n];
        if (character->IsLongActPaired() && index == character->mLongActPartner) {
            character->ClearLongAct();
            func_0202c28c(&gGameData, other, character);
        }
        if (character->IsTroubleOfCategory34OrKind18() && index == character->mTroubleTarget) {
            if (character->IsLongActPaired()) {
                int partner = character->mLongActPartner;
                int found = -1;
                SavedCharacter partnerLoaded;
                SavedCharacter *p;
                if (list != NULL) {
                    for (int j = 0; j < count; j++) {
                        if (partner == characters[j].mIndex) {
                            found = j;
                            break;
                        }
                    }
                    p = &characters[found];
                } else {
                    if (func_0202c3dc(&gGameData, partner)) {
                        func_0202c264(&gGameData, partner, &partnerLoaded);
                        partnerLoaded.mIndex = partner;
                    }
                    p = &partnerLoaded;
                }
                p->ClearLongAct();
                func_0202c28c(&gGameData, p->mIndex, p);
            }
            characters[n].mMood = 0;
            characters[n].ClearLongAct();
            characters[n].ClearTrouble();
            func_0202c28c(&gGameData, other, &characters[n]);
        }
    }
}

// Drops the news items about the character, keeping the order of the others
void RemoveNewsOf(int index) {
    NewsItem items[11];
    for (int i = 0; i < 11; i++) {
        func_0202c8a8(&gGameData, i, &items[i]);
    }
    int n = 0;
    for (int i = 0; i < 11; i++) {
        BOOL about = FALSE;
        if (items[i].mValid == 1) {
            for (u32 j = 0; j < data_020fa1d0[items[i].mKind].mCharacterCount + 2; j++) {
                u8 c = items[i].mCharacters[j];
                if (c == index) {
                    about = TRUE;
                    break;
                }
            }
        }
        if (!about) {
            func_0202c8bc(&gGameData, n, &items[i]);
            n++;
        }
    }
    NewsItem empty;
    MI_CpuClear16(&empty, sizeof(empty));
    for (int i = n; i < 11; i++) {
        func_0202c8bc(&gGameData, n, &empty);
        n++;
    }
}

#define GAMEDATA_PAIRS ((GameDataPairs *)&gGameData)

static inline CharacterPair GetPairA(int i) {
    return GAMEDATA_PAIRS->mPairsA[i];
}

static inline void SetPairA(int i, CharacterPair pair) {
    GAMEDATA_PAIRS->mPairsA[i] = pair;
}

static inline CharacterPair GetPairB(int i) {
    return GAMEDATA_PAIRS->mPairsB[i];
}

static inline void SetPairB(int i, CharacterPair pair) {
    GAMEDATA_PAIRS->mPairsB[i] = pair;
}

// Drops the pairs the character is in from both tables, keeping the order of the others
void RemovePairsOf(int index) {
    CharacterPair pairs[100];
    CharacterPair empty;
    for (int i = 0; i < 100; i++) {
        pairs[i] = GetPairA(i);
    }
    int n = 0;
    for (int i = 0; i < 100; i++) {
        BOOL found = FALSE;
        if (pairs[i].mState != -1) {
            for (int j = 0; j < 2; j++) {
                u8 c = pairs[i].mCharacters[j];
                if (c == index) {
                    found = TRUE;
                    break;
                }
            }
        }
        if (!found) {
            SetPairA(n, pairs[i]);
            n++;
        }
    }
    empty.mState = -1;
    empty.mCharacters[0] = 0;
    empty.mCharacters[1] = 0;
    for (int i = n; i < 100; i++) {
        SetPairA(n, empty);
        n++;
    }
    for (int i = 0; i < 100; i++) {
        pairs[i] = GetPairB(i);
    }
    n = 0;
    for (int i = 0; i < 100; i++) {
        BOOL found = FALSE;
        if (pairs[i].mState != -1) {
            for (int j = 0; j < 2; j++) {
                u8 c = pairs[i].mCharacters[j];
                if (c == index) {
                    found = TRUE;
                    break;
                }
            }
        }
        if (!found) {
            SetPairB(n, pairs[i]);
            n++;
        }
    }
    for (int i = n; i < 100; i++) {
        SetPairB(n, empty);
        n++;
    }
}

// Changes the notes of the saved songs that are the character's
void RemoveFromSongs(int index) {
    int slot;
    for (int i = 0; i < 100; i++) {
        int owner = func_0202ca34(&gGameData, i, &slot);
        if (owner == 0xFF || owner == index) {
            continue;
        }
        SongEntry song;
        song.mSong = i;
        song.Load(song.mSong);
        if (song.SetNotesFromSlot(index, slot)) {
            song.Save(song.mSong);
        }
    }
}

// Writes an empty record over the character's and frees its slot
void ClearRecord(int index) {
    SavedCharacter character;
    character.Reset();
    func_0202c28c(&gGameData, index, &character);
    func_0202c2fc(&gGameData, index);
}

void MiiList::ShowTabs(int sortKey, int group) {
    switch (sortKey) {
    case 0:
        mTabs->SetVisible(TRUE);
        mTabs->SetTab(0);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    case 1:
        mTabs->SetVisible(TRUE);
        mTabs->SetTab(0);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    case 2:
        mTabs->SetVisible(TRUE);
        mTabs->SetTab(3);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    case 3:
        mTabs->SetVisible(FALSE);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    case 4:
        mTabs->SetVisible(TRUE);
        mTabs->SetTab(2);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    case 5:
        mTabs->SetVisible(TRUE);
        mTabs->SetTab(4);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    case 6:
        mTabs->SetVisible(TRUE);
        mTabs->SetTab(1);
        mTabs->mTabs[mTabs->mCurrent].Select(group);
        break;
    }
}

void MiiList::Sort(int sortKey, BOOL selectFirst) {
    int selected;
    switch (sortKey) {
    case 0:
        SortByName();
        break;
    case 1:
        SortByNickname();
        break;
    case 2:
        SortBySex();
        break;
    case 3:
        SortByBirthYear();
        break;
    case 4:
        SortByBirthday();
        break;
    case 5:
        SortByBloodType();
        break;
    case 6:
        SortByPersonality();
        break;
    }
    if (sortKey != 1) {
        mPanel->SetMode(0);
    } else {
        mPanel->SetMode(1);
    }
    if (selectFirst) {
        goto first;
    }
    selectFirst = FALSE;
    if (mSelectedIndex != 0xFFFF && mSelected != -1) {
        selectFirst = TRUE;
    }
    if (!selectFirst) {
    first:
        mSelectedIndex = mResidents[0].mIndex;
        selected = 0;
    } else {
        for (int i = 0; i < mCount; i++) {
            if (mSelectedIndex == mResidents[i].mIndex) {
                selected = i;
                break;
            }
        }
    }
    int group = 0;
    int last;
    for (;;) {
        last = mGroupStarts[group] + mGroupCounts[group] - 1;
        if (!(mGroupStarts[group] > selected || selected > last)) {
            break;
        }
        group++;
    }
    mGroup = group;
    mPanel->SetEntries(&mResidents[mGroupStarts[group]], mGroupCounts[group]);
    if (mSelectInGroup) {
        mSelected = selected;
        mSelectedIndex = mResidents[mSelected].mIndex;
        mPanel->Select(selected - mGroupStarts[group]);
        mPanel->ScrollToSelection();
    }
    ShowTabs(sortKey, mGroup);
}

void MiiList::SortByName() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 0;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByFamilyName, buffer);
    // The names that start with no kana (group 10) sort before the kana: move them after
    int otherStart = -1;
    int otherCount = 0;
    int kanaStart = -1;
    int kanaCount = 0;
    for (int i = 0; i < mCount; i++) {
        int initial = CharacterSort::GetNameInitialGroup(mResidents[i].mFamilyName);
        if (initial == 10) {
            if (otherStart == -1) {
                otherStart = i;
            }
            otherCount++;
        } else if (initial >= 0 && initial <= 9) {
            kanaCount++;
            if (kanaStart == -1) {
                kanaStart = i;
            }
        }
    }
    if (otherCount != 0 && kanaCount != 0) {
        SwapRuns(otherStart, otherCount, kanaStart, kanaCount);
    }
    int group = 0;
    for (int i = 0; i < mCount; i++) {
        int key = CharacterSort::GetNameInitialGroup(mResidents[i].mFamilyName);
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
    mGroupCounts[10] += mGroupCounts[11];
    mGroupCounts[10] += mGroupCounts[12];
}

void MiiList::SortByNickname() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 1;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByMiiName, buffer);
    int otherStart = -1;
    int otherCount = 0;
    int kanaStart = -1;
    int kanaCount = 0;
    for (int i = 0; i < mCount; i++) {
        wchar_t name[11];
        MiiData_GetName(&mResidents[i], name);
        int initial = CharacterSort::GetNameInitialGroup(name);
        if (initial == 10) {
            if (otherStart == -1) {
                otherStart = i;
            }
            otherCount++;
        } else if (initial >= 0 && initial <= 9) {
            kanaCount++;
            if (kanaStart == -1) {
                kanaStart = i;
            }
        }
    }
    if (otherCount != 0 && kanaCount != 0) {
        SwapRuns(otherStart, otherCount, kanaStart, kanaCount);
    }
    int group = 0;
    for (int i = 0; i < mCount; i++) {
        wchar_t name[11];
        MiiData_GetName(&mResidents[i], name);
        int key = CharacterSort::GetNameInitialGroup(name);
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
    mGroupCounts[10] += mGroupCounts[11];
    mGroupCounts[10] += mGroupCounts[12];
}

void MiiList::SortByPersonality() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 6;
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByPersonality, buffer);
    int group = 0;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    for (int i = 0; i < mCount; i++) {
        int key = mResidents[i].GetPersonality();
        if (mResidents[i].mUnknown050Bit0) {
            key = 16;
        }
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
}

void MiiList::SortByBirthYear() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 3;
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByBirthday, buffer);
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    mGroupCounts[0] = mCount;
    mGroupStarts[0] = 0;
}

void MiiList::SortByBirthday() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 4;
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByBirthdayInYear, buffer);
    u32 group = 0;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    for (int i = 0; i < mCount; i++) {
        u32 month = mResidents[i].GetParameter(0x26);
        u32 key;
        if (month >= 1 && month <= 12) {
            key = month - 1;
        } else {
            key = 12;
        }
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
}

void MiiList::SortBySex() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 2;
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareBySex, buffer);
    int group = 0;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    for (int i = 0; i < mCount; i++) {
        int key = MiiData_GetSex(&mResidents[i]);
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
}

void MiiList::SortByBloodType() {
    u32 buffer[SORT_BUFFER_SIZE];
    mSortKey = 5;
    MATH_QSort(mResidents, mCount, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByBloodType, buffer);
    u32 group = 0;
    for (int i = 0; i < MII_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    for (int i = 0; i < mCount; i++) {
        u32 key = mResidents[i].mBloodType;
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
}

// Exchanges two runs of the list, the first run followed by the second, by following each entry's cycle of moves
void MiiList::SwapRuns(int first, int firstCount, int second, int secondCount) {
    int target[MII_LIST_SIZE];
    BOOL moved[MII_LIST_SIZE];
    for (int i = 0; i < MII_LIST_SIZE; i++) {
        target[i] = -1;
        moved[i] = FALSE;
    }
    int firstEnd = first + firstCount;
    for (int i = first; i < firstEnd; i++) {
        target[i] = i + secondCount;
    }
    for (int i = second; i < second + secondCount; i++) {
        target[i] = first + (i - second);
    }
    SavedCharacter held[2];
    int h = 0;
    for (; first < firstEnd; first++) {
        int i = first;
        while (!moved[i]) {
            int to = target[i];
            COPY_CHARACTER(held[h], mResidents[to]);
            COPY_CHARACTER(mResidents[to], mResidents[i]);
            moved[i] = TRUE;
            i = to;
            h ^= 1;
            while (!moved[i]) {
                to = target[i];
                COPY_CHARACTER(held[h], mResidents[to]);
                COPY_CHARACTER(mResidents[to], held[h ^ 1]);
                moved[i] = TRUE;
                i = to;
                h ^= 1;
            }
        }
    }
}

BOOL MiiList::SelectIndex(u16 index) {
    for (int i = 0; i < mCount; i++) {
        if (index == mResidents[i].mIndex) {
            mSelected = i;
            mSelectedIndex = index;
            return TRUE;
        }
    }
    return FALSE;
}

void MiiList::ClearSelection() {
    mSelectedIndex = 0xFFFF;
    mSelected = -1;
    mPanel->ClearSelection();
}

void MiiListPanel::Init(int pageSize) {
    mScrolling = SCROLLING_NONE;
    mEntries = NULL;
    mEntryCount = 0;
    mSelectedButton = -1;
    mSelected = -1;
    mScroll = 0;
    mScrollOffset = 0;
    mScrollPosition = 0;
    mTouched = FALSE;
    WindowPoint buttonPosition = {0, 0};
    mButtonPosition = buttonPosition;
    WindowPoint iconPosition = {0, 0};
    mIconPosition = iconPosition;
    WindowPoint icon2Position = {0, 0};
    mIcon2Position = icon2Position;
    WindowPoint textPosition = {0, 0};
    mTextPosition = textPosition;
    ScreenRect rect = {0, 0, 0, 0};
    mRect = rect;
    mIconsShown = FALSE;
    mIcons2Shown = FALSE;
    mTouchable = TRUE;
    mButtonCount = pageSize + 1;
    mPageSize = pageSize;
    mBgPlane = 0x100;
    mBgOffset = 0;
    mBgScrolls = FALSE;
    for (int i = 0; i < mButtonCount; i++) {
        mButtonEntries[i] = -1;
    }
}

void MiiListPanel::SetBackground(int plane, int offset) {
    mBgPlane = plane;
    mBgOffset = offset;
    mBgScrolls = TRUE;
}

void MiiListPanel::InitButtons(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size) {
    mButtonPosition = position;
    mRect.x = position.x - (size.x >> 1);
    mRect.y = position.y - (size.y >> 1);
    mRect.width = size.x;
    mRect.height = mPageSize * 20;
    for (int i = 0; i < mButtonCount; i++) {
        Sprite_Init(&mButtons[i], resource, sequence, 0x80, 0x60, TRUE);
        func_02031260(&mButtons[i], 3);
        func_02031278(&mButtons[i], sequence, pressedSequence);
        mButtons[i].SetPosition(FxVec2(position.x << FX32_SHIFT, (position.y + i * 20) << FX32_SHIFT));
        func_02031eb0(&mButtons[i], size.x, size.y);
        func_0203119c(&mButtons[i]);
        mButtons[i].mOverwritePalette = 3;
        mButtons[i].mVisible = FALSE;
        mButtons[i].mUnknownDC = TRUE;
        mButtons[i].mSounds.mSounds[0] = 2;
    }
    func_02032c28(&mTouchArea, mRect);
    mTouchArea.mUnknown44 = TRUE;
}

void MiiListPanel::InitTexts(int plane, WindowPoint position, int *charOffset) {
    mTextPosition = position;
    for (int i = 0; i < mButtonCount; i++) {
        mTexts[i].Init(plane, 28, 4, *charOffset, 0, 1, 0, 0, mTextBuffers[i]);
        mTexts[i].SetPositionFromVec(position.x << FX32_SHIFT, (position.y + i * 20) << FX32_SHIFT);
        mTexts[i].mColor = 8;
        mTexts[i].mSprite.mOverwritePalette = 3;
        *charOffset += mTexts[i].GetCharCount();
        mTextDirty[i] = TRUE;
    }
}

void MiiListPanel::InitIcons(int resource, int sequence, WindowPoint position) {
    mIconPosition = position;
    mIconsShown = TRUE;
    for (int i = 0; i < mButtonCount; i++) {
        Sprite_Init(&mIcons[i], resource, sequence, 0x80, 0x60, TRUE);
        mIcons[i].SetPositionFromVec(position.x << FX32_SHIFT, (position.y + i * 20) << FX32_SHIFT);
        mIcons[i].mOverwritePalette = 3;
        mIcons[i].mVisible = FALSE;
    }
}

void MiiListPanel::InitIcons2(int resource, int sequence, WindowPoint position) {
    mIcon2Position = position;
    mIcons2Shown = TRUE;
    for (int i = 0; i < mButtonCount; i++) {
        Sprite_Init(&mIcons2[i], resource, sequence, 0x80, 0x60, TRUE);
        mIcons2[i].SetPositionFromVec(position.x << FX32_SHIFT, (position.y + i * 20) << FX32_SHIFT);
        mIcons2[i].mOverwritePalette = 3;
        mIcons2[i].mVisible = FALSE;
    }
}

void MiiListPanel::InitUpButton(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size) {
    Sprite_Init(&mUpButton, resource, sequence, 0x80, 0x60, TRUE);
    func_02031260(&mUpButton, 1);
    func_02031278(&mUpButton, sequence, pressedSequence);
    mUpButton.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    func_02031eb0(&mUpButton, size.x, size.y);
    func_0203119c(&mUpButton);
    mUpButton.mUnknown5E = 3;
}

void MiiListPanel::InitDownButton(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size) {
    Sprite_Init(&mDownButton, resource, sequence, 0x80, 0x60, TRUE);
    func_02031260(&mDownButton, 1);
    func_02031278(&mDownButton, sequence, pressedSequence);
    mDownButton.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    func_02031eb0(&mDownButton, size.x, size.y);
    func_0203119c(&mDownButton);
    mDownButton.mUnknown5E = 3;
}

void MiiListPanel::InitScrollBar(int resource, int sequence, int knobSequence, int a, int b, int c,
                                 WindowPoint position, ScreenRect rect) {
    int y = position.y;
    Sprite_Init(&mScrollBar, resource, sequence, 0x80, 0x60, TRUE);
    func_020325dc(&mScrollBar, a, b, c, rect.x, rect.y, rect.width, rect.height);
    mScrollBar.SetPosition(FxVec2(position.x << FX32_SHIFT, y << FX32_SHIFT));
    func_02032aa4(&mScrollBar, sequence, knobSequence);
    mScrollBar.mUnknown5E = 3;
    mScrollBar.mUnknownE0 = TRUE;
    mScrollBar.mUnknown124 = TRUE;
    func_02032ac0(&mScrollBar, 4, 5);
}

void MiiListPanel::SetUpScrollBar(int a, int b, int c) {
    func_02032acc(&mScrollBar, a, b, c);
    func_02032f60(&mTouchArea, a, c);
}

void MiiListPanel::Update() {
    mTouched = FALSE;
    BOOL up = func_0203133c(&mUpButton);
    BOOL down = func_0203133c(&mDownButton);
    int max = (mEntryCount - mPageSize) * 20;
    if (max < 0) {
        max = 0;
    }
    switch (mScrolling) {
    case SCROLLING_UP:
        mScrollOffset -= 4;
        if (mScrollOffset % 20 == 0) {
            if (up && CanScrollUp()) {
                ShowEntryAbove();
                mScroll--;
                mScrollOffset = 20;
                gSoundManager.StartSeqArc(0, 1, 8);
            } else {
                mScrolling = SCROLLING_NONE;
            }
        }
        PlaceButtons();
        func_02032a0c(&mScrollBar, 0, max, mScroll * 20 + mScrollOffset);
        break;
    case SCROLLING_DOWN:
        mScrollOffset += 4;
        if (mScrollOffset % 20 == 0) {
            mScroll++;
            mScrollOffset = 0;
            if (down && CanScrollDown()) {
                ShowEntryBelow();
                gSoundManager.StartSeqArc(0, 1, 9);
            } else {
                mScrolling = SCROLLING_NONE;
            }
        }
        PlaceButtons();
        func_02032a0c(&mScrollBar, 0, max, mScroll * 20 + mScrollOffset);
        break;
    case SCROLLING_NONE:
        func_02032678(&mScrollBar);
        func_02032cc0(&mTouchArea, mScroll * 20 + mScrollOffset, 0, max);
        if (mScrollBar.mDragging) {
            int position = func_020329d4(&mScrollBar, 0, max);
            if (position == mScrollPosition) {
                return;
            }
            mScroll = position / 20;
            mScrollOffset = position % 20;
            mScrollPosition = position;
            ShowEntries();
            PlaceButtons();
            return;
        }
        if (mScrollBar.mReleased) {
            int position = func_020329d4(&mScrollBar, 0, max);
            mScroll = (position + 10) / 20;
            mScrollOffset = 0;
            func_02032a0c(&mScrollBar, 0, max, mScroll * 20);
            ShowEntries();
            PlaceButtons();
            return;
        }
        if (mTouchArea.mDragging) {
            int position = mTouchArea.mScroll;
            if (position == mScrollPosition) {
                return;
            }
            mScroll = position / 20;
            mScrollOffset = position % 20;
            mScrollPosition = position;
            func_02032a0c(&mScrollBar, 0, max, position);
            ShowEntries();
            PlaceButtons();
            return;
        }
        if (mTouchArea.mReleased) {
            mScroll = (mTouchArea.mScroll + 10) / 20;
            mScrollOffset = 0;
            func_02032a0c(&mScrollBar, 0, max, mScroll * 20);
            ShowEntries();
            PlaceButtons();
            return;
        }
        if (up) {
            if (CanScrollUp()) {
                ShowEntryAbove();
                mScroll--;
                mScrollOffset = 20;
                mScrolling = SCROLLING_UP;
                gSoundManager.StartSeqArc(0, 1, 8);
            }
            return;
        }
        if (down) {
            if (CanScrollDown()) {
                ShowEntryBelow();
                mScrolling = SCROLLING_DOWN;
                gSoundManager.StartSeqArc(0, 1, 9);
            }
            return;
        }
        if (gTouchInput.mTp.touch && func_02028d88(&mRect, &gTouchInput.mPoint) && mTouchable) {
            for (int i = 0; i < mButtonCount; i++) {
                if (func_0203133c(&mButtons[i])) {
                    if (mSelectedButton != -1 && mSelectedButton != i) {
                        func_02031e28(&mButtons[mSelectedButton], FALSE);
                    }
                    mSelectedButton = i;
                    mSelected = GetButtonEntry(i);
                    mTouched = TRUE;
                }
            }
        }
        break;
    }
}

// Puts each button, with its marks and text, at its row
void MiiListPanel::PlaceButtons() {
    for (int i = 0; i < mButtonCount; i++) {
        int row = (i + mButtonCount - mScroll % mButtonCount) % mButtonCount;
        int y = row * 20 - mScrollOffset;
        int buttonY = y + mButtonPosition.y;
        int iconY = y + mIconPosition.y;
        int icon2Y = y + mIcon2Position.y;
        int textY = y + mTextPosition.y;
        mButtons[i].SetPositionFromVec(mButtonPosition.x << FX32_SHIFT, buttonY << FX32_SHIFT);
        mIcons[i].SetPositionFromVec(mIconPosition.x << FX32_SHIFT, iconY << FX32_SHIFT);
        mIcons2[i].SetPositionFromVec(mIcon2Position.x << FX32_SHIFT, icon2Y << FX32_SHIFT);
        mTexts[i].SetPositionFromVec(mTextPosition.x << FX32_SHIFT, textY << FX32_SHIFT);
    }
    if (mBgScrolls) {
        func_02028154(data_02121834, mBgPlane, mScrollOffset - mBgOffset);
    }
}

// Shows an entry on a button, or nothing when the entry is out of range; the text is written again only when it
// changed
void MiiListPanel::ShowEntry(int button, int entry) {
    BOOL complete;
    BOOL valid = (entry >= 0 && entry < mEntryCount) ? TRUE : FALSE;
    BOOL changed = TRUE;
    if (valid) {
        if (mButtonEntries[button] == mEntries[entry].mIndex && !mTextDirty[button]) {
            changed = FALSE;
        }
        mTextDirty[button] = FALSE;
    } else if (mButtonEntries[button] == -1) {
        changed = FALSE;
    }
    wchar_t text[22];
    MI_CpuClear16(text, sizeof(text));
    complete = TRUE;
    if (valid && changed) {
        mButtonEntries[button] = mEntries[entry].mIndex;
        switch (mMode) {
        case 0:
            if (wcslen(mEntries[entry].mFamilyName) != 0) {
                wcsncat(text, mEntries[entry].mFamilyName, 10);
            } else {
                wcscat(text, L"？？？");
                complete = FALSE;
            }
            wcscat(text, L"　");
            if (wcslen(mEntries[entry].mName) != 0) {
                wcsncat(text, mEntries[entry].mName, 10);
            } else {
                wcscat(text, L"？？？");
                complete = FALSE;
            }
            break;
        case 1:
            MiiData_GetName(&mEntries[entry], text);
            break;
        }
        mTexts[button].SetText((const NNSG2dChar *)text);
        mTexts[button].Redraw();
        mTexts[button].Transfer();
        mButtons[button].mVisible = TRUE;
        if (entry == mSelected) {
            mSelectedButton = button;
            func_02031e28(&mButtons[button], TRUE);
        } else {
            func_02031e28(&mButtons[button], FALSE);
        }
        wchar_t creator[11];
        MI_CpuClear16(creator, sizeof(creator));
        MiiData_GetCreatorName(&mEntries[entry], creator);
        if (wcslen(creator) == 0 || mEntries[entry].mBirthYear == 0xFFF ||
            mEntries[entry].GetParameter(PARAM_BIRTH_MONTH) == 0 || mEntries[entry].GetParameter(PARAM_BIRTH_DAY) == 0 ||
            mEntries[entry].mBloodType == 4) {
            complete = FALSE;
        }
        mIcons[button].mVisible = complete;
        BOOL marked = FALSE;
        if (mEntries[entry].mUnknown050Bit0) {
            marked = TRUE;
        }
        mIcons2[button].mVisible = marked;
    } else if (!valid && changed) {
        mButtonEntries[button] = -1;
        mTexts[button].SetText((const NNSG2dChar *)text);
        mTexts[button].Redraw();
        mTexts[button].Transfer();
        mButtons[button].mVisible = FALSE;
        mIcons[button].mVisible = FALSE;
        mIcons2[button].mVisible = FALSE;
    }
}

void MiiListPanel::ShowEntries() {
    for (int i = 0; i < mButtonCount; i++) {
        ShowEntry(i, GetButtonEntry(i));
    }
}

// The button that comes into view at the top as the list scrolls up a row
void MiiListPanel::ShowEntryAbove() {
    int button = (mScroll + mPageSize) % mButtonCount;
    int entry = GetButtonEntry(button) - mButtonCount;
    ShowEntry(button, entry);
}

// And at the bottom as it scrolls down
void MiiListPanel::ShowEntryBelow() {
    int button = (mScroll + mPageSize) % mButtonCount;
    ShowEntry(button, GetButtonEntry(button));
}

void MiiListPanel::Draw() {
    for (int i = 0; i < mButtonCount; i++) {
        if (mIconsShown) {
            Sprite_Draw(&mIcons[i]);
        }
        if (mIcons2Shown) {
            Sprite_Draw(&mIcons2[i]);
        }
        mTexts[i].Draw();
        func_02031d7c(&mButtons[i]);
    }
    func_02031d7c(&mUpButton);
    func_02031d7c(&mDownButton);
    Sprite_Draw(&mScrollBar);
}

void MiiListPanel::SetEntries(SavedCharacter *entries, int count) {
    mSelectedButton = -1;
    mSelected = -1;
    mTouched = FALSE;
    mScroll = 0;
    mScrollOffset = 0;
    if (count != 0) {
        mEntries = entries;
        mEntryCount = count;
    } else {
        mEntries = NULL;
        mEntryCount = 0;
    }
    if (mEntryCount <= mPageSize) {
        mUpButton.mVisible = FALSE;
        mDownButton.mVisible = FALSE;
        mScrollBar.mVisible = FALSE;
    } else {
        mUpButton.mVisible = TRUE;
        mDownButton.mVisible = TRUE;
        mScrollBar.mVisible = TRUE;
        func_02032a0c(&mScrollBar, 0, (mEntryCount - mPageSize) * 20, 0);
    }
    ShowEntries();
    PlaceButtons();
}

void MiiListPanel::SetMode(int mode) {
    if (mMode == mode) {
        return;
    }
    mMode = mode;
    for (int i = 0; i < mButtonCount; i++) {
        mTextDirty[i] = TRUE;
    }
}

void MiiListPanel::Select(int entry) {
    if (mEntryCount <= 0) {
        return;
    }
    if (entry < 0) {
        mSelected = -1;
        return;
    }
    if (entry > mEntryCount - 1) {
        entry = mEntryCount - 1;
    }
    mSelected = entry;
    for (int i = 0; i < mButtonCount; i++) {
        func_02031e28(&mButtons[i], FALSE);
    }
    if (mScroll <= entry && entry < mScroll + mButtonCount) {
        mSelectedButton = entry % mButtonCount;
        func_02031e28(&mButtons[mSelectedButton], TRUE);
        return;
    }
    mSelectedButton = -1;
}

void MiiListPanel::ScrollTo(int scroll) {
    if (mEntryCount - mPageSize < scroll) {
        scroll = mEntryCount - mPageSize;
    }
    if (scroll < 0) {
        scroll = 0;
    }
    mScroll = scroll;
    func_02032a0c(&mScrollBar, 0, (mEntryCount - mPageSize) * 20, mScroll * 20);
    PlaceButtons();
    ShowEntries();
}

void MiiListPanel::ScrollToSelection() {
    if (mEntryCount <= mPageSize) {
        mScroll = 0;
    } else if (mSelected + mPageSize <= mEntryCount) {
        mScroll = mSelected;
    } else {
        mScroll = mEntryCount - mPageSize;
    }
    func_02032a0c(&mScrollBar, 0, (mEntryCount - mPageSize) * 20, mScroll * 20);
    ShowEntries();
    PlaceButtons();
}

void MiiListPanel::ClearSelection() {
    func_02031e28(&mButtons[mSelectedButton], FALSE);
    mSelectedButton = -1;
    mSelected = -1;
}

void MiiListPanel::ResetScrollButtons() {
    func_02031228(&mUpButton);
    func_02031228(&mDownButton);
}

void MiiListPanel::SetScrollUnknown(int value) {
    mScrollBar.mUnknownE4 = value;
    mTouchArea.mUnknown24 = value;
}

// The entry a button shows: the one of the rows from mScroll on that is the button modulo the buttons' count
int MiiListPanel::GetButtonEntry(int button) {
    return button + mButtonCount * ((mScroll + mButtonCount - 1 - button) / mButtonCount);
}

BOOL MiiListPanel::CanScrollUp() {
    return mEntryCount > mPageSize && mScroll > 0;
}

BOOL MiiListPanel::CanScrollDown() {
    BOOL result = FALSE;
    if (mEntryCount > mPageSize && mScroll < mEntryCount - mPageSize) {
        result = TRUE;
    }
    return result;
}

void MiiListTabs::InitTabs() {
    for (int i = 0; i < MII_LIST_TABS; i++) {
        mTabs[i].Init(i);
    }
}

void MiiListTabs::AddButtons(int tab, int resource, const u8 *sequences, const u8 *pressedSequences) {
    int count = sTabButtonCounts[tab];
    for (int i = 0; i < count; i++) {
        WindowPoint size = sTabButtonSizes[tab];
        mTabs[tab].AddButton(resource, sequences[i], pressedSequences[i], sTabButtonPositions[tab][i], size);
    }
}

void MiiListTabs::Reset() {
    for (int i = 0; i < MII_LIST_TABS; i++) {
        mTabs[i].Reset();
    }
    mCurrent = 0;
    SetTab(0);
}

void MiiListTabs::Init(int resource, int sequence) {
    InitTabs();
    for (int tab = 0; tab < MII_LIST_TABS; tab++) {
        for (int i = 0; i < sTabButtonCounts[tab]; i++) {
            WindowPoint size = sTabButtonSizes[tab];
            mTabs[tab].AddButton(resource, sequence, sequence + 1, sTabButtonPositions[tab][i], size);
            sequence += 2;
        }
    }
    Reset();
}

BOOL MiiListTabs::Update() {
    return mTabs[mCurrent].Update();
}

void MiiListTabs::Draw() {
    mTabs[mCurrent].Draw();
}

void MiiListTabs::SetVisible(BOOL visible) {
    mTabs[mCurrent].SetVisible(visible);
}

void MiiListTabs::SetTab(int tab) {
    mTabs[mCurrent].SetVisible(FALSE);
    mTabs[tab].SetVisible(TRUE);
    mCurrent = tab;
}

void MiiListTab::Init(int tab) {
    mTab = tab;
    mButtonCount = 0;
    mUnknown0008 = 0;
}

void MiiListTab::AddButton(int resource, u8 sequence, u8 pressedSequence, WindowPoint position, WindowPoint size) {
    Sprite_Init(&mButtons[mButtonCount], resource, sequence, 0x80, 0x60, TRUE);
    func_02031260(&mButtons[mButtonCount], 3);
    func_02031278(&mButtons[mButtonCount], sequence, pressedSequence);
    mButtons[mButtonCount].SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    func_02031eb0(&mButtons[mButtonCount], size.x, size.y);
    func_0203119c(&mButtons[mButtonCount]);
    mButtons[mButtonCount].mUnknown5E = 2;
    mButtons[mButtonCount].mVisible = FALSE;
    mButtons[mButtonCount].mUnknownDC = TRUE;
    mButtons[mButtonCount].mSounds.mSounds[0] = 3;
    mButtonCount++;
}

void MiiListTab::Reset() {
    mSelected = 0;
    func_02031e28(&mButtons[0], TRUE);
    mButtons[0].mUnknown5E = 1;
}

BOOL MiiListTab::Update() {
    for (int i = 0; i < mButtonCount; i++) {
        if (func_0203133c(&mButtons[i])) {
            func_02031e28(&mButtons[mSelected], FALSE);
            mButtons[mSelected].mUnknown5E = 2;
            mSelected = i;
            func_02031e28(&mButtons[i], TRUE);
            mButtons[mSelected].mUnknown5E = 1;
            return TRUE;
        }
    }
    return FALSE;
}

void MiiListTab::Draw() {
    for (int i = 0; i < mButtonCount; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

void MiiListTab::SetVisible(BOOL visible) {
    for (int i = 0; i < mButtonCount; i++) {
        mButtons[i].mVisible = visible;
    }
}

void MiiListTab::Select(int button) {
    if (button >= mButtonCount) {
        return;
    }
    func_02031e28(&mButtons[mSelected], FALSE);
    mButtons[mSelected].mUnknown5E = 2;
    mSelected = button;
    func_02031e28(&mButtons[mSelected], TRUE);
    mButtons[mSelected].mUnknown5E = 1;
}
