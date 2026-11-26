#include "WriteSongScene.h"
#include "LyricList.h"
#include "MainProgram.h"

#include <wchar.h>

#include "text/TextUtil.h"

// --- The words ---

void LyricWord::Clear() {
    mSyllables = 0;
    mKind = 0;
    mId = 0;
    MI_CpuClear16(mText, sizeof(mText));
    MI_CpuClear16(mReading, sizeof(mReading));
}

// A word from a text, read as the speech engine reads it; FALSE when it is empty or longer than nine characters, or
// its reading is, or it has fewer than two or more than six syllables
BOOL LyricWord::SetFromText(u16 id, u8 kind, u16 source, const wchar_t *text) {
    wchar_t reading[128];
    MI_CpuClear16(reading, sizeof(reading));
    wcsncpy(reading, text, 9);
    MakeSpeechTextInPlace(reading, sizeof(reading));
    RemoveSpeechSymbols(reading);
    u32 length = wcslen(text);
    u32 readingLength = wcslen(reading);
    int syllables = (u8)CountSyllables(reading);
    if (length != 0 && length <= 9 && readingLength != 0 && readingLength <= 9 && syllables >= 2 && syllables <= 6) {
        mId = id;
        mKind = kind;
        mSource = source;
        wcsncpy(mText, text, 9);
        wcsncpy(mReading, reading, 9);
        mSyllables = syllables;
        UpdateShown();
        return TRUE;
    }
    return FALSE;
}

// The same with the reading given
BOOL LyricWord::SetFromTexts(u16 id, u8 kind, u16 source, const wchar_t *text, wchar_t *reading) {
    RemoveSpeechSymbols(reading);
    u32 length = wcslen(text);
    u32 readingLength = wcslen(reading);
    int syllables = (u8)CountSyllables(reading);
    if (length != 0 && length <= 9 && readingLength != 0 && readingLength <= 9 && syllables >= 2 && syllables <= 6) {
        mId = id;
        mKind = kind;
        mSource = source;
        wcsncpy(mText, text, 9);
        wcsncpy(mReading, reading, 9);
        mSyllables = syllables;
        UpdateShown();
        return TRUE;
    }
    return FALSE;
}

void LyricWord::UpdateShown() {
    mShowReading = FALSE;
    if (mKind != LYRIC_WORD_ITEM) {
        return;
    }
    mShowReading = TRUE;
    if (mSource == 0xEC || mSource == 0xEE || mSource == 0xF1) {
        mShowReading = FALSE;
    }
}

const wchar_t *LyricWord::GetShownText() {
    switch (mShowReading) {
    case 0:
        return mText;
    case 1:
        return mReading;
    }
    return NULL;
}

LyricWord &LyricWord::operator=(const LyricWord &other) {
    mId = other.mId;
    mSyllables = other.mSyllables;
    mKind = other.mKind;
    mSource = other.mSource;
    mShowReading = other.mShowReading;
    MI_CpuClear16(mText, sizeof(mText));
    MI_CpuClear16(mReading, sizeof(mReading));
    wcsncpy(mText, other.mText, 9);
    wcsncpy(mReading, other.mReading, 9);
    return *this;
}

// --- The list ---

void LyricWordList::MountArchives() {
    ArchiveManager_MountCompressed(&gArchiveManager, "WLS", "Msg/TmdSongMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "WLI", "Msg/TmdItemMsgJP_LZ.bin", 0);
}

void LyricWordList::Init(LyricWordPanel *panel, LyricWordTabs *tabs) {
    mPanel = panel;
    mTabs = tabs;
    for (int i = 0; i < LYRIC_LIST_SIZE; i++) {
        mEntries[i].Clear();
    }
}

void LyricWordList::Update() {
    mChanged = FALSE;
    if (mTabs->Update()) {
        int group = mTabs->mSelected;
        mGroup = group;
        mPanel->SetEntries(&mShown[mGroupStarts[group]], mGroupCounts[group]);
        mChanged = TRUE;
        mSelected = -1;
    }
    mPanel->Update();
    if (mPanel->mTouched && mPanel->mSelected != -1) {
        mChanged = TRUE;
        int start = mGroupStarts[mGroup];
        mSelected = start + mPanel->mSelected;
    }
}

void LyricWordList::Draw() {
    mPanel->Draw();
    mTabs->Draw();
}

void LyricWordList::UnmountArchives() {
    ArchiveManager_Unmount(&gArchiveManager, "WLS");
    ArchiveManager_Unmount(&gArchiveManager, "WLI");
}

#define SORT_BUFFER_SIZE 200

void LyricWordList::Load() {
    u32 buffer[SORT_BUFFER_SIZE];
    mCount = 0;
    AddSongWords();
    AddCharacterNames();
    AddItems();
    OS_GetVBlankCount();
    MATH_QSort(mEntries, mCount, sizeof(LyricWord), (MATHCompareFunc)Compare, buffer);
    OS_GetVBlankCount();
    // The words that start with no kana (group 10) sort before the kana: move them after
    int otherStart = -1;
    int otherCount = 0;
    int kanaStart = -1;
    int kanaCount = 0;
    for (int i = 0; i < mCount; i++) {
        int initial = CharacterSort::GetNameInitialGroup(mEntries[i].GetShownText());
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
    OS_GetVBlankCount();
}

void LyricWordList::AddSongWords() {
    MessageFile file;
    file.Open("WLS:tmd_song_msg_JP.bmg");
    for (int i = 0; i < 0x40F; i++) {
        if (mCount >= LYRIC_LIST_SIZE) {
            break;
        }
        wchar_t text[10];
        wchar_t reading[10];
        MI_CpuClear16(text, sizeof(text));
        MI_CpuClear16(reading, sizeof(reading));
        int message = i + 0x58;
        wcsncpy(text, file.GetText(message), 9);
        wcsncpy(reading, file.GetText(i + 0x467), 9);
        if (mEntries[mCount].SetFromTexts(mCount, LYRIC_WORD_SONG, message, text, reading)) {
            mCount++;
        }
    }
}

void LyricWordList::AddCharacterNames() {
    for (u16 i = 0; i < 100; i++) {
        if (!GameData_CharacterExists(&gGameData, i)) {
            continue;
        }
        if (mCount >= LYRIC_LIST_SIZE - 3) {
            return;
        }
        SavedCharacter character;
        GameData_ReadCharacter(&gGameData, i, &character);
        character.mIndex = i;
        wchar_t name[11];
        MiiData_GetName(&character, name);
        if (mEntries[mCount].SetFromText(mCount, LYRIC_WORD_MII_NAME, i, name)) {
            mCount++;
        }
        if (mEntries[mCount].SetFromText(mCount, LYRIC_WORD_FAMILY_NAME, i, character.mFamilyName)) {
            mCount++;
        }
        if (mEntries[mCount].SetFromText(mCount, LYRIC_WORD_NAME, i, character.mName)) {
            mCount++;
        }
        if (mEntries[mCount].SetFromText(mCount, LYRIC_WORD_MOTTO, i, character.mMotto)) {
            mCount++;
        }
    }
}

void LyricWordList::AddItems() {
    Inventory inventory;
    Item *items[INVENTORY_SIZE];
    MessageFile file;
    int count;
    inventory.Load();
    inventory.GetListed(items, &count);
    file.Open("WLI:tmd_item_msg_JP.bmg");
    for (int i = 0; i < count; i++) {
        if (mCount >= LYRIC_LIST_SIZE) {
            break;
        }
        wchar_t text[10];
        wchar_t reading[10];
        MI_CpuClear16(text, sizeof(text));
        MI_CpuClear16(reading, sizeof(reading));
        wcsncpy(text, file.GetText(items[i]->mId), 9);
        wcsncpy(reading, file.GetText(items[i]->mId + 0xFF), 9);
        if (mEntries[mCount].SetFromTexts(mCount, LYRIC_WORD_ITEM, items[i]->mId, text, reading)) {
            mCount++;
        }
    }
}

// Exchanges two runs of the list, the first run followed by the second, by following each entry's cycle of moves
void LyricWordList::SwapRuns(int first, int firstCount, int second, int secondCount) {
    s16 target[LYRIC_LIST_SIZE];
    u8 moved[LYRIC_LIST_SIZE];
    for (int i = 0; i < LYRIC_LIST_SIZE; i++) {
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
    LyricWord held[2];
    int h = 0;
    for (; first < firstEnd; first++) {
        int i = first;
        while (!moved[i]) {
            int to = target[i];
            held[h] = mEntries[to];
            mEntries[to] = mEntries[i];
            moved[i] = TRUE;
            i = to;
            h ^= 1;
            while (!moved[i]) {
                to = target[i];
                held[h] = mEntries[to];
                mEntries[to] = held[h ^ 1];
                moved[i] = TRUE;
                i = to;
                h ^= 1;
            }
        }
    }
}

// Keeps the words of a syllable count, and groups them by their initial
void LyricWordList::Filter(int syllables) {
    mShownCount = 0;
    for (int i = 0; i < mCount; i++) {
        if (syllables == mEntries[i].mSyllables) {
            mShown[mShownCount] = &mEntries[i];
            mShownCount++;
        }
    }
    for (int i = 0; i < LYRIC_LIST_GROUPS; i++) {
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    int group = 0;
    for (int i = 0; i < mShownCount; i++) {
        int key = CharacterSort::GetNameInitialGroup(mShown[i]->GetShownText());
        while (group < key) {
            group++;
            mGroupStarts[group] = i;
        }
        mGroupCounts[group]++;
    }
    mGroupCounts[10] += mGroupCounts[11];
    mGroupCounts[10] += mGroupCounts[12];
}

void LyricWordList::ShowFirstGroup() {
    int selected = mSelected;
    mGroup = 0;
    mPanel->SetEntries(&mShown[mGroupStarts[0]], mGroupCounts[0]);
    mTabs->Select(0);
    if (mGroupCounts[mGroup] != 0) {
        mSelected = mGroupStarts[mGroup];
        if (selected != mSelected) {
            mChanged = TRUE;
        }
    }
}

int LyricWordList::Compare(const LyricWord *a, const LyricWord *b) {
    wchar_t textA[10];
    wchar_t textB[10];
    MI_CpuClear16(textA, sizeof(textA));
    MI_CpuClear16(textB, sizeof(textB));
    wcscpy(textA, ((LyricWord *)a)->GetShownText());
    wcscpy(textB, ((LyricWord *)b)->GetShownText());
    return CharacterSort::CompareStrings(textA, textB);
}

// --- The panel ---

void LyricWordPanel::Init() {
    mScrolling = SCROLLING_NONE;
    mEntries = NULL;
    mEntryCount = 0;
    mTouched = FALSE;
    mSelectedButton = -1;
    mSelected = -1;
    mTouchable = FALSE;
    mScrollPosition = 0;
    mBgPlane = 0x100;
    mBgOffset = 0;
    mRect.x = 4;
    mRect.y = 0x26;
    mRect.width = 0xE5;
    mRect.height = 0x78;
}

void LyricWordPanel::SetBackground(int plane, int offset) {
    mBgPlane = plane;
    mBgOffset = offset;
}

void LyricWordPanel::InitButtons(int resource, u16 sequence, u16 pressedSequence, int *charOffset) {
    for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
        mTextDirty[i] = TRUE;
        mButtonEntries[i] = -1;
        Sprite_Init(&mButtons[i], resource, sequence, 0x80, 0x60, TRUE);
        SpriteButton_SetKind(&mButtons[i], 3);
        SpriteButton_SetSequences(&mButtons[i], sequence, pressedSequence);
        mButtons[i].SetPosition(FxVec2(0x77 << FX32_SHIFT, (i + 0x43) << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], 0xE4, 0x12);
        SpriteButton_Reset(&mButtons[i]);
        mButtons[i].mOverwritePalette = 3;
        mButtons[i].mVisible = FALSE;
        mButtons[i].mSounds.mSounds[0] = 0;
        mTexts[i].Init(0x1000, 0x14, 2, *charOffset, 0, 1, 0, 0, mTextBuffers[i]);
        mTexts[i].SetPositionFromVec(0xD << FX32_SHIFT, (i * 20 + 0x29) << FX32_SHIFT);
        mTexts[i].mColor = 8;
        mTexts[i].mSprite.mOverwritePalette = 3;
        *charOffset += mTexts[i].GetCharCount();
    }
    TouchScrollArea_Init(&mTouchArea, mRect);
    mTouchArea.mUnknown44 = TRUE;
}

void LyricWordPanel::InitUpButton(int resource, u16 sequence, u16 pressedSequence) {
    Sprite_Init(&mUpButton, resource, sequence, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mUpButton, 1);
    SpriteButton_SetSequences(&mUpButton, sequence, pressedSequence);
    mUpButton.SetPosition(FxVec2(0xF4 << FX32_SHIFT, 0x2C << FX32_SHIFT));
    SpriteButton_SetTouchRect(&mUpButton, 0xE, 0xC);
    SpriteButton_Reset(&mUpButton);
    mUpButton.mOverwritePalette = 2;
}

void LyricWordPanel::InitDownButton(int resource, u16 sequence, u16 pressedSequence) {
    Sprite_Init(&mDownButton, resource, sequence, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mDownButton, 1);
    SpriteButton_SetSequences(&mDownButton, sequence, pressedSequence);
    mDownButton.SetPosition(FxVec2(0xF4 << FX32_SHIFT, 0x96 << FX32_SHIFT));
    SpriteButton_SetTouchRect(&mDownButton, 0xE, 0xC);
    SpriteButton_Reset(&mDownButton);
    mDownButton.mOverwritePalette = 2;
}

void LyricWordPanel::InitScrollBar(int resource, int sequence, int knobSequence) {
    Sprite_Init(&mScrollBar, resource, sequence, 0x80, 0x60, TRUE);
    ScrollBar_Init(&mScrollBar, 0x61, 0x4A, 2, -8, -8, 0x10, 0x10);
    mScrollBar.SetPosition(FxVec2(0xF4 << FX32_SHIFT, 0x61 << FX32_SHIFT));
    func_02032aa4(&mScrollBar, sequence, knobSequence);
    mScrollBar.mOverwritePalette = 2;
    mScrollBar.mUnknownE0 = TRUE;
    mScrollBar.mUnknown124 = TRUE;
    func_02032ac0(&mScrollBar, 4, 5);
}

void LyricWordPanel::SetUpScrollBar(int a, int b, int c) {
    func_02032acc(&mScrollBar, a, b, c);
    func_02032f60(&mTouchArea, a, c);
}

void LyricWordPanel::Update() {
    BOOL moved;
    BOOL changed;
    mTouched = changed = moved = FALSE;
    BOOL up = SpriteButton_Update(&mUpButton);
    BOOL down = SpriteButton_Update(&mDownButton);
    int max = (mEntryCount - LYRIC_LIST_PAGE) * 20;
    if (max < 0) {
        max = 0;
    }
    switch (mScrolling) {
    case SCROLLING_NONE:
        ScrollBar_Update(&mScrollBar);
        TouchScrollArea_Update(&mTouchArea, mScrollPosition, 0, max);
        if (mScrollBar.mDragging) {
            mScrollPosition = ScrollBar_GetPosition(&mScrollBar, 0, max);
            changed = TRUE;
        } else if (mScrollBar.mReleased) {
            int position = ScrollBar_GetPosition(&mScrollBar, 0, max);
            moved = TRUE;
            mScrollPosition = (position + 10) / 20 * 20;
            changed = moved;
        } else if (mTouchArea.mDragging) {
            moved = TRUE;
            changed = TRUE;
            mScrollPosition = mTouchArea.mScroll;
        } else if (mTouchArea.mReleased) {
            moved = TRUE;
            mScrollPosition = (mTouchArea.mScroll + 10) / 20 * 20;
            changed = moved;
        } else if (up && mEntryCount > LYRIC_LIST_PAGE && GetScroll() > 0) {
            mScrolling = SCROLLING_UP;
            gSoundManager.StartSeqArc(1, 1, 8);
        } else if (down && CanScrollDown()) {
            mScrolling = SCROLLING_DOWN;
            gSoundManager.StartSeqArc(1, 1, 9);
        } else if (gTouchInput.mTp.touch && ScreenRect_Contains(&mRect, &gTouchInput.mPoint) && mTouchable) {
            for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
                if (SpriteButton_Update(&mButtons[i])) {
                    if (mSelectedButton != -1 && mSelectedButton != i) {
                        SpriteButton_SetSelected(&mButtons[mSelectedButton], FALSE);
                    }
                    mSelectedButton = i;
                    mSelected = GetButtonEntry(i);
                    mTouched = TRUE;
                }
            }
        }
        break;
    case SCROLLING_UP:
        mScrollPosition -= 4;
        if (mScrollPosition % 20 == 0) {
            if (up && mEntryCount > LYRIC_LIST_PAGE && GetScroll() > 0) {
                gSoundManager.StartSeqArc(1, 1, 8);
            } else {
                mScrolling = SCROLLING_NONE;
            }
        }
        moved = TRUE;
        changed = TRUE;
        break;
    case SCROLLING_DOWN:
        mScrollPosition += 4;
        if (mScrollPosition % 20 == 0) {
            if (down && CanScrollDown()) {
                gSoundManager.StartSeqArc(1, 1, 9);
            } else {
                mScrolling = SCROLLING_NONE;
            }
        }
        moved = TRUE;
        changed = TRUE;
        break;
    }
    if (moved) {
        ScrollBar_SetPosition(&mScrollBar, 0, max, mScrollPosition);
    }
    if (changed) {
        SetScroll(mScrollPosition);
        ShowEntries();
        PlaceButtons();
    }
}

void LyricWordPanel::Draw() {
    SpriteButton_Draw(&mUpButton);
    SpriteButton_Draw(&mDownButton);
    Sprite_Draw(&mScrollBar);
    for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
        mTexts[i].Draw();
        SpriteButton_Draw(&mButtons[i]);
    }
}

// Puts each button and its text at its row
void LyricWordPanel::PlaceButtons() {
    for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
        int y = GetButtonEntry(i) * 20 - mScrollPosition;
        mButtons[i].SetPosition(FxVec2(0x77 << FX32_SHIFT, (y + 0x2F) << FX32_SHIFT));
        mTexts[i].SetPositionFromVec(0xD << FX32_SHIFT, (y + 0x29) << FX32_SHIFT);
    }
    ScreenLoader_SetScrollY(&gScreenLoader, mBgPlane, mScrollPosition % 20 - mBgOffset);
}

// Writes the texts of the buttons whose word changed
void LyricWordPanel::ShowEntries() {
    for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
        if (!mTextDirty[i]) {
            continue;
        }
        int entry = GetButtonEntry(i);
        if (mButtonEntries[i] != -1) {
            mButtons[i].mVisible = TRUE;
            if (entry == mSelected) {
                mSelectedButton = i;
                SpriteButton_SetSelected(&mButtons[i], TRUE);
            } else {
                SpriteButton_SetSelected(&mButtons[i], FALSE);
            }
            wchar_t text[10];
            MI_CpuClear16(text, sizeof(text));
            wcsncpy(text, mEntries[entry]->mText, 9);
            NNS_G2dCharCanvasClear(&mTexts[i].mCanvas, 0);
            mTexts[i].SetText((const NNSG2dChar *)text);
            mTexts[i].Redraw();
            mTexts[i].Transfer();
        } else {
            NNS_G2dCharCanvasClear(&mTexts[i].mCanvas, 0);
            mTexts[i].Transfer();
            mButtons[i].mVisible = FALSE;
        }
        mTextDirty[i] = FALSE;
    }
}

// Marks every button's text to be written again
void LyricWordPanel::RefreshEntries() {
    for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
        mTextDirty[i] = TRUE;
        int entry = GetButtonEntry(i);
        if (entry >= 0 && entry < mEntryCount) {
            mButtonEntries[i] = mEntries[entry]->mId;
        } else {
            mButtonEntries[i] = -1;
        }
    }
}

// Scrolls to a position and marks the buttons whose word changes
void LyricWordPanel::SetScroll(int position) {
    mScrollPosition = position;
    for (int i = 0; i < LYRIC_LIST_BUTTONS; i++) {
        int entry = GetButtonEntry(i);
        if (entry >= 0 && entry < mEntryCount) {
            if (mButtonEntries[i] != mEntries[entry]->mId) {
                mTextDirty[i] = TRUE;
                mButtonEntries[i] = mEntries[entry]->mId;
            }
        } else if (mButtonEntries[i] != -1) {
            mTextDirty[i] = TRUE;
            mButtonEntries[i] = -1;
        }
    }
}

void LyricWordPanel::SetEntries(LyricWord **entries, int count) {
    mScrolling = SCROLLING_NONE;
    mSelectedButton = -1;
    mSelected = -1;
    mTouched = FALSE;
    mScrollPosition = 0;
    if (count != 0) {
        mEntries = entries;
        mEntryCount = count;
    } else {
        mEntries = NULL;
        mEntryCount = 0;
    }
    if (mEntryCount <= LYRIC_LIST_PAGE) {
        mUpButton.mVisible = FALSE;
        mDownButton.mVisible = FALSE;
        mScrollBar.mVisible = FALSE;
    } else {
        mUpButton.mVisible = TRUE;
        mDownButton.mVisible = TRUE;
        mScrollBar.mVisible = TRUE;
        ScrollBar_SetPosition(&mScrollBar, 0, (mEntryCount - LYRIC_LIST_PAGE) * 20, 0);
    }
    RefreshEntries();
    ShowEntries();
    PlaceButtons();
}

// --- The tabs ---

void LyricWordTabs::Init(int unknown) {
    mUnknown000 = unknown;
    mButtonCount = 0;
    mUnknown008 = 0;
}

void LyricWordTabs::AddButton(int resource, u16 sequence, u16 pressedSequence, WindowPoint position, WindowPoint size) {
    Sprite_Init(&mButtons[mButtonCount], resource, sequence, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mButtons[mButtonCount], 3);
    SpriteButton_SetSequences(&mButtons[mButtonCount], sequence, pressedSequence);
    mButtons[mButtonCount].SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mButtons[mButtonCount], size.x, size.y);
    SpriteButton_Reset(&mButtons[mButtonCount]);
    mButtons[mButtonCount].mUnknown5E = 2;
    mButtons[mButtonCount].mVisible = TRUE;
    mButtons[mButtonCount].mSounds.mSounds[0] = 3;
    mButtonCount++;
}

void LyricWordTabs::Reset() {
    mSelected = 0;
    SpriteButton_SetSelected(&mButtons[0], TRUE);
    mButtons[0].mUnknown5E = 1;
}

BOOL LyricWordTabs::Update() {
    for (int i = 0; i < mButtonCount; i++) {
        if (SpriteButton_Update(&mButtons[i])) {
            SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
            mButtons[mSelected].mUnknown5E = 2;
            mSelected = i;
            SpriteButton_SetSelected(&mButtons[mSelected], TRUE);
            mButtons[mSelected].mUnknown5E = 1;
            return TRUE;
        }
    }
    return FALSE;
}

void LyricWordTabs::Draw() {
    for (int i = 0; i < mButtonCount; i++) {
        SpriteButton_Draw(&mButtons[i]);
    }
}

void LyricWordTabs::Select(int button) {
    if (button >= mButtonCount) {
        return;
    }
    SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
    mButtons[mSelected].mUnknown5E = 2;
    mSelected = button;
    SpriteButton_SetSelected(&mButtons[mSelected], TRUE);
    mButtons[mSelected].mUnknown5E = 1;
}
