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
