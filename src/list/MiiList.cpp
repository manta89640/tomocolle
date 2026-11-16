#include "list/MiiList.h"

#include "MessageWindow.h"

#include "GameData.h"

// The list of saved characters (main program 0x020a03e8 to 0x020a3c68): the list itself, the panel of its
// buttons and its tabs.

extern "C" {
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

extern "C" {
BOOL func_0202c354(GameData *game, int index);                          // Whether there is a saved character
void func_0202c264(GameData *game, int index, CharacterRecord *record); // Reads a saved character
}

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
    mUnknown1139C = TRUE;
    mPanel = panel;
    mTabs = tabs;
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
    Sprite_Draw((Sprite *)mScrollBar);
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
    *(int *)(mScrollBar + 0xE4) = value;
    *(int *)(mUnknown93C8 + 0x24) = value;
}

extern "C" {
int MiiData_GetSex(const CharacterRecord *character);
void func_020a31cc(MiiListPanel *panel);
void func_020a2d38(MiiListPanel *panel);
void func_02032a0c(void *scrollBar, int unk, int range, int position);
}

#define SORT_BUFFER_SIZE 200

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
    while (mGroupStarts[group] > selected || selected > mGroupStarts[group] + mGroupCounts[group] - 1) {
        group++;
    }
    mGroup = group;
    mPanel->SetEntries(&mResidents[mGroupStarts[group]], mGroupCounts[group]);
    if (mUnknown1139C) {
        mSelected = selected;
        mSelectedIndex = mResidents[selected].mIndex;
        mPanel->Select(selected - mGroupStarts[group]);
        mPanel->ScrollToSelection();
    }
    ShowTabs(sortKey, mGroup);
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

void MiiListPanel::SetEntries(SavedCharacter *entries, int count) {
    mSelectedButton = -1;
    mSelected = -1;
    mUnknown9468 = FALSE;
    mScroll = 0;
    mUnknown9470 = 0;
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
        ((Sprite *)mScrollBar)->mVisible = FALSE;
    } else {
        mUpButton.mVisible = TRUE;
        mDownButton.mVisible = TRUE;
        ((Sprite *)mScrollBar)->mVisible = TRUE;
        func_02032a0c(mScrollBar, 0, (mEntryCount - mPageSize) * 20, 0);
    }
    func_020a31cc(this);
    func_020a2d38(this);
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
    func_02032a0c(mScrollBar, 0, (mEntryCount - mPageSize) * 20, scroll * 20);
    func_020a2d38(this);
    func_020a31cc(this);
}

void MiiListPanel::ScrollToSelection() {
    if (mEntryCount <= mPageSize) {
        mScroll = 0;
    } else if (mSelected + mPageSize <= mEntryCount) {
        mScroll = mSelected;
    } else {
        mScroll = mEntryCount - mPageSize;
    }
    func_02032a0c(mScrollBar, 0, (mEntryCount - mPageSize) * 20, mScroll * 20);
    func_020a31cc(this);
    func_020a2d38(this);
}

void MiiListPanel::InitIcons(int resource, int sequence, WindowPoint position) {
    mUnknown9424[2] = position.x;
    mUnknown9424[3] = position.y;
    mIconsShown = TRUE;
    for (int i = 0; i < mButtonCount; i++) {
        Sprite_Init(&mIcons[i], resource, sequence, 0x80, 0x60, TRUE);
        mIcons[i].SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
        mIcons[i].mOverwritePalette = 3;
        mIcons[i].mVisible = FALSE;
        position.y += 20;
    }
}

void MiiListPanel::InitIcons2(int resource, int sequence, WindowPoint position) {
    mUnknown9424[4] = position.x;
    mUnknown9424[5] = position.y;
    mIcons2Shown = TRUE;
    for (int i = 0; i < mButtonCount; i++) {
        Sprite_Init(&mIcons2[i], resource, sequence, 0x80, 0x60, TRUE);
        mIcons2[i].SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
        mIcons2[i].mOverwritePalette = 3;
        mIcons2[i].mVisible = FALSE;
        position.y += 20;
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
