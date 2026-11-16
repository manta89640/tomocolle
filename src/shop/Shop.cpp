#include <nitro.h>
#include <nnsys.h>
#include <wchar.h>

#include "GameData.h"
#include "shop/Shop.h"
#include "shop/ShopWindow.h"
#include "sound/SoundManager.h"
#include "text/TextUtil.h"

// The shops' shared code: the scrolling item list of the lower screen, which the shops (overlays 56, 68, 83 to 85)
// and other scenes with lists of items (75, 86, 100, 110) use, the shops' price window, and the shop counter: the
// staff with its speech and the panel of the item talked about.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
};

// The main program's camera and lights
struct Camera {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ fx32 mUnknown0C;
    /* 0x10 */ fx32 mUnknown10;
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
};

struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

struct ScreenLoader;
struct ArchiveLoader;

extern "C" {
void func_02031228(SpriteButton *button);
extern TouchInput gTouchInput;
extern ScreenLoader data_02121834;
extern ArchiveLoader gArchiveManager;
extern u8 data_0211d604[];
extern u8 data_02122824[];
extern u8 data_02122764[];
extern Light data_021227cc[];
extern GameData gGameData;
extern MATHRandContext16 gRandom;

// The message window's other functions
// Adds a button with a text: (number, resource, .., .., position, size, the text's message file and message, font,
// plane, character offset, the button's sounds, ..)
void func_02033844(MessageWindow *window, int number, int resource, u8 a, u8 b, WindowPoint position,
                   WindowPoint size, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *sounds, int c);
// Adds a message text: (message file, message, font, plane, position, size, character offset)
void func_020338d0(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, WindowPoint size, int *charOffset);
void func_02033bb4(MessageWindow *window, int resource, int sequence, WindowPoint position);
void func_02034f80(MessageWindow *window, int a, int b);

void func_02028154(ScreenLoader *loader, int plane, int y); // Scrolls a background
void func_02028280(ScreenLoader *loader, int a, int b);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void func_02029660(int resourceSet, u32 resType, void *archive); // Gives a resource set an archive's animations
void func_0202c264(GameData *game, int index, Character *character); // Reads a saved character
int MiiData_GetSex(Character *character);
void func_0209d898(Character *character, TtsPlayer *player); // Gives the speech player the character's voice
void func_02023ee0(void *a, int b, int c, int d, int e);
void func_0202b0e8(void *a);
void func_0202a640(Camera *camera);
void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);

BOOL func_0202c354(GameData *game, int index);
u8 func_0202c468(GameData *game, int index); // The character's event
u8 func_0202c490(GameData *game, int index); // The place of its event
}

#define RESOURCE_BCA0 0x30414342 // Joint animations

static inline void ClearCanvas(TextCanvas &canvas, int color) { NNS_G2dCharCanvasClear(&canvas.mCanvas, color); }

// The sum of two points
template <class A, class B> static inline A AddPoints(A a, B b) {
    A p;
    p.x = a.x + b.x;
    p.y = a.y + b.y;
    return p;
}

// Puts a canvas at a point in pixels
template <class P> static inline void SetCanvasPosition(ObjTextCanvas *canvas, P position) {
    canvas->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

// Puts a sprite at a point in pixels
template <class P> static inline void SetSpritePosition(Sprite *sprite, const P &position) {
    sprite->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

// --- The list -----------------------------------------------------------------------------------------------------

static const ScreenPoint sRowsPosition = {0x77, 0x41};

void ItemListView::Init(int rows) {
    mScrolling = 0;
    mMode = MODE_PRICE;
    mItems = NULL;
    mItemCount = 0;
    mMoney = NULL;
    mTouched = FALSE;
    mTouchedRow = -1;
    mSelected = -1;
    mTouchable = FALSE;
    mScroll = 0;
    mRowCount = rows + 1;
    mVisibleRows = rows;
    mBgPlane = 0x100;
    mBgOffset = 0;
    mRect.x = 5;
    mRect.y = 0x37;
    mRect.width = 0xE5;
    mRect.height = mVisibleRows * 20;
    mUnknown13F60 = 0;
}

void ItemListView::Init() {
    Init(5);
}

void ItemListView::SetBackground(int plane, int offset) {
    mBgPlane = plane;
    mBgOffset = offset;
}

// The rows: their buttons and the texts of the items' names
void ItemListView::InitRows(int resource, u16 sequence, u16 pressedSequence, int *charOffset, ScreenPoint position) {
    for (int i = 0; i < mRowCount; i++) {
        mRowDirty[i] = TRUE;
        mRowItemIds[i] = -1;
        mRowPalettes[i] = 8;
        mRowValues[i] = 0x4D;
        mRowCounts[i] = 0x4D;
        mPosition = position;
        Sprite_Init(&mRowButtons[i], resource, sequence, 0x80, 0x60, TRUE);
        func_02031260(&mRowButtons[i], 3);
        func_02031278(&mRowButtons[i], sequence, pressedSequence);
        mRowButtons[i].SetPosition(FxVec2(sRowsPosition.x << FX32_SHIFT, (sRowsPosition.y + i * 20) << FX32_SHIFT));
        func_02031eb0(&mRowButtons[i], 0xE5, 18);
        func_0203119c(&mRowButtons[i]);
        mRowButtons[i].mOverwritePalette = 3;
        mRowButtons[i].mVisible = FALSE;
        mRowButtons[i].mUnknownDC = TRUE;
        mRowButtons[i].mSounds.mSounds[0] = 2;
        mRowTexts[i].InitObj(&gFonts[0].mFont, 0x1000, 16, 2, *charOffset, 0, 1, 0, 0, 15, mRowTextBuffers[i]);
        mRowTexts[i].SetPositionFromVec(0, 0);
        mRowTexts[i].mMessageFile = NULL;
        mRowTexts[i].mColor = 8;
        mRowTexts[i].mSprite.mOverwritePalette = 3;
        *charOffset += mRowTexts[i].GetCharCount();
    }
    mRect.x = position.x - 0x72;
    mRect.y = position.y - 10;
    mRect.width = 0xE5;
    mRect.height = mVisibleRows * 20;
    func_02032c28(&mTouchArea, mRect);
    mTouchArea.mUnknown44 = TRUE;
}

void ItemListView::InitRows(int resource, u16 sequence, u16 pressedSequence, int *charOffset) {
    InitRows(resource, sequence, pressedSequence, charOffset, sRowsPosition);
}

// The rows' prices, with an icon each
void ItemListView::InitPrices(int resource, int sequence, int *charOffset) {
    for (int i = 0; i < mRowCount; i++) {
        mRowPrices[i].InitText(&gFonts[1].mFont, 0x1000, 8, 1, *charOffset, -2, 1, 0, 0, 15, mRowPriceBuffers[i], 2,
                               NNS_G2D_CHARA_COLORMODE_16);
        mRowPrices[i].mColor = 8;
        ClearCanvas(mRowPrices[i], 0);
        mRowPrices[i].SetPositionFromVec(0, 0);
        mRowPrices[i].mSprite.mOverwritePalette = 3;
        *charOffset += mRowPrices[i].GetCharCount();
        Sprite_Init(&mRowPriceIcons[i], resource, sequence, 0x80, 0x60, TRUE);
        mRowPriceIcons[i].SetPositionFromVec(0, 0);
        mRowPriceIcons[i].mOverwritePalette = 3;
    }
}

// The rows' counts, right-aligned
void ItemListView::InitCounts(int *charOffset) {
    for (int i = 0; i < mRowCount; i++) {
        mRowCounts2[i].mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
        mRowCounts2[i].mTextX = 0x40;
        mRowCounts2[i].InitText(&gFonts[1].mFont, 0x1000, 8, 1, *charOffset, -2, 1, 0, 0, 15,
                                mRowCountBuffers[i], 2, NNS_G2D_CHARA_COLORMODE_16);
        mRowCounts2[i].mColor = 8;
        ClearCanvas(mRowCounts2[i], 0);
        mRowCounts2[i].SetPositionFromVec(0, 0);
        mRowCounts2[i].mSprite.mOverwritePalette = 3;
        *charOffset += mRowCounts2[i].GetCharCount();
    }
}

static const ScreenPoint sUpButtonPosition = {0xF4, 0x3E};
static const ScreenPoint sUpButtonSize = {14, 12};

void ItemListView::InitUpButton(int resource, u16 sequence, u16 pressedSequence, ScreenPoint position,
                                ScreenPoint size) {
    Sprite_Init(&mUpButton, resource, sequence, 0x80, 0x60, TRUE);
    func_02031260(&mUpButton, 1);
    func_02031278(&mUpButton, sequence, pressedSequence);
    mUpButton.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    func_02031eb0(&mUpButton, size.x, size.y);
    func_0203119c(&mUpButton);
    mUpButton.mOverwritePalette = 2;
}

void ItemListView::InitUpButton(int resource, u16 sequence, u16 pressedSequence) {
    InitUpButton(resource, sequence, pressedSequence, sUpButtonPosition, sUpButtonSize);
}

static const ScreenPoint sDownButtonPosition = {0xF4, 0x94};
static const ScreenPoint sDownButtonSize = {14, 12};

void ItemListView::InitDownButton(int resource, u16 sequence, u16 pressedSequence, ScreenPoint position,
                                  ScreenPoint size) {
    Sprite_Init(&mDownButton, resource, sequence, 0x80, 0x60, TRUE);
    func_02031260(&mDownButton, 1);
    func_02031278(&mDownButton, sequence, pressedSequence);
    mDownButton.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    func_02031eb0(&mDownButton, size.x, size.y);
    func_0203119c(&mDownButton);
    mDownButton.mOverwritePalette = 2;
}

void ItemListView::InitDownButton(int resource, u16 sequence, u16 pressedSequence) {
    InitDownButton(resource, sequence, pressedSequence, sDownButtonPosition, sDownButtonSize);
}

static const ScreenPoint sScrollBarPosition = {0xF4, 0x69};
static const ScreenRect sScrollKnobRect = {-8, -8, 16, 16};

void ItemListView::InitScrollBar(int resource, int sequence, int knobSequence, int a, int b, int c,
                                 ScreenPoint position, ScreenRect knob) {
    Sprite_Init(&mScrollBar, resource, sequence, 0x80, 0x60, TRUE);
    func_020325dc(&mScrollBar, a, b, c, knob.x, knob.y, knob.width, knob.height);
    mScrollBar.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    func_02032aa4(&mScrollBar, sequence, knobSequence);
    mScrollBar.mOverwritePalette = 2;
    mScrollBar.mUnknownE0 = TRUE;
    mScrollBar.mUnknown124 = TRUE;
    func_02032ac0(&mScrollBar, 4, 5);
}

void ItemListView::InitScrollBar(int resource, int sequence, int knobSequence) {
    InitScrollBar(resource, sequence, knobSequence, 0x69, 0x38, 2, sScrollBarPosition, sScrollKnobRect);
}

void ItemListView::SetUpScrollBar(int a, int b, int c) {
    func_02032acc(&mScrollBar, a, b, c);
    func_02032f60(&mTouchArea, a, c);
}

// Scrolls by the arrow buttons, the scroll bar and dragging, and takes the touches of the rows
void ItemListView::Update() {
    BOOL moveBar = FALSE;
    BOOL scrolled = moveBar;
    mTouched = scrolled;
    BOOL up = func_0203133c(&mUpButton);
    BOOL down = func_0203133c(&mDownButton);
    int max = (mItemCount - mVisibleRows) * 20;
    if (max < 0) {
        max = 0;
    }
    switch (mScrolling) {
    case 0:
        func_02032678(&mScrollBar);
        func_02032cc0(&mTouchArea, mScroll, 0, max);
        if (mScrollBar.mDragging) {
            mScroll = func_020329d4(&mScrollBar, 0, max);
            scrolled = TRUE;
        } else if (mScrollBar.mReleased) {
            mScroll = (func_020329d4(&mScrollBar, 0, max) + 10) / 20 * 20;
            moveBar = TRUE;
            scrolled = moveBar;
        } else if (mTouchArea.mDragging) {
            mScroll = mTouchArea.mScroll;
            moveBar = TRUE;
            scrolled = moveBar;
        } else if (mTouchArea.mReleased) {
            mScroll = (mTouchArea.mScroll + 10) / 20 * 20;
            moveBar = TRUE;
            scrolled = moveBar;
        } else if (up && CanScrollUp()) {
            mScrolling = 1;
            gSoundManager.StartSeqArc(0, 1, 8);
        } else if (down && CanScrollDown()) {
            mScrolling = 2;
            gSoundManager.StartSeqArc(0, 1, 9);
        } else if (gTouchInput.mTouching && func_02028d88(&mRect, &gTouchInput.mPoint) && mTouchable) {
            for (int i = 0; i < mRowCount; i++) {
                if (func_0203133c(&mRowButtons[i])) {
                    if (mTouchedRow != -1 && mTouchedRow != i) {
                        func_02031e28(&mRowButtons[mTouchedRow], FALSE);
                    }
                    mTouchedRow = i;
                    mSelected = GetRowItem(i);
                    mTouched = TRUE;
                }
            }
        }
        break;
    case 1:
        mScroll -= 4;
        if (mScroll % 20 == 0) {
            if (up && CanScrollUp()) {
                gSoundManager.StartSeqArc(0, 1, 8);
            } else {
                mScrolling = 0;
            }
        }
        moveBar = TRUE;
        scrolled = moveBar;
        break;
    case 2:
        mScroll += 4;
        if (mScroll % 20 == 0) {
            if (down && CanScrollDown()) {
                gSoundManager.StartSeqArc(0, 1, 9);
            } else {
                mScrolling = 0;
            }
        }
        moveBar = TRUE;
        scrolled = moveBar;
        break;
    }
    if (moveBar) {
        func_02032a0c(&mScrollBar, 0, max, mScroll);
    }
    if (scrolled) {
        SetScroll(mScroll);
        DrawDirtyRows();
        Refresh();
    }
}

void ItemListView::Draw() {
    func_02031d7c(&mUpButton);
    func_02031d7c(&mDownButton);
    Sprite_Draw(&mScrollBar);
    for (int i = 0; i < mRowCount; i++) {
        mRowTexts[i].Draw();
        switch (mMode) {
        case MODE_NAME:
            break;
        case MODE_PRICE:
            mRowPrices[i].Draw();
            Sprite_Draw(&mRowPriceIcons[i]);
            break;
        case MODE_COUNT:
        case MODE_BOTH:
            mRowCounts2[i].Draw();
            break;
        }
        func_02031d7c(&mRowButtons[i]);
    }
}

static const ScreenPoint sCountOffset = {0x2A, -2};
static const ScreenPoint sPriceIconOffset = {0x66, 1};
static const ScreenPoint sPriceOffset = {0x34, -2};
static const ScreenPoint sTextOffset = {-0x69, -6};


// Puts each row where its item is at the scroll position, and scrolls the background with them
void ItemListView::Refresh() {
    for (int i = 0; i < mRowCount; i++) {
        ScreenPoint offset;
        offset.x = 0;
        offset.y = GetRowItem(i) * 20 - mScroll;
        ScreenPoint row = AddPoints(mPosition, offset);
        SetSpritePosition(&mRowButtons[i], row);
        SetCanvasPosition(&mRowTexts[i], AddPoints(row, sTextOffset));
        SetCanvasPosition(&mRowPrices[i], AddPoints(row, sPriceOffset));
        SetSpritePosition(&mRowPriceIcons[i], AddPoints(row, sPriceIconOffset));
        SetCanvasPosition(&mRowCounts2[i], AddPoints(row, sCountOffset));
    }
    func_02028154(&data_02121834, mBgPlane, mScroll % 20 - mBgOffset);
}

// Takes the palette, value and count of the rows that show an item again, and draws those that changed
void ItemListView::UpdateShownRows() {
    for (int i = 0; i < mRowCount; i++) {
        if (mRowItemIds[i] != -1) {
            int item = GetRowItem(i);
            UpdateRowPalette(i, item);
            UpdateRowValue(i, item);
            UpdateRowCount(i, item);
        }
    }
    DrawDirtyRows();
}

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

void ItemListView::DrawRow(int row, int item) {
    mRowButtons[row].mVisible = TRUE;
    if (item == mSelected) {
        mTouchedRow = row;
        func_02031e28(&mRowButtons[row], TRUE);
    } else {
        func_02031e28(&mRowButtons[row], FALSE);
    }
    ClearCanvas(mRowTexts[row], 0);
    mRowTexts[row].mColor = mRowPalettes[row];
    mRowTexts[row].mMessage = mRowItemIds[row];
    mRowTexts[row].DrawAll();
    mRowTexts[row].ObjTextCanvas::Transfer();
    switch (mMode) {
    case MODE_NAME:
        break;
    case MODE_PRICE: {
        wchar_t price[8];
        FormatNumber(mItems[item]->GetPrice(), price, sizeof(price), 7, FALSE);
        mRowPrices[row].mText = (const NNSG2dChar *)price;
        ClearCanvas(mRowPrices[row], 0);
        mRowPrices[row].DrawText();
        mRowPrices[row].Transfer();
        mRowPriceIcons[row].mVisible = TRUE;
        break;
    }
    case MODE_COUNT: {
        wchar_t count[3];
        wchar_t text[5];
        FormatNumber(mRowCounts[row], count, sizeof(count), 2, FALSE);
        wcscpy(text, L"ｘ ");
        wcsncat(text, count, 2);
        mRowCounts2[row].mText = (const NNSG2dChar *)text;
        ClearCanvas(mRowCounts2[row], 0);
        mRowCounts2[row].DrawText();
        mRowCounts2[row].Transfer();
        break;
    }
    case MODE_BOTH: {
        wchar_t text[8];
        wchar_t value[3];
        wchar_t count[3];
        MI_CpuClear16(text, sizeof(text));
        FormatNumber(mRowValues[row], value, sizeof(value), 0, FALSE);
        FormatNumber(mRowCounts[row], count, sizeof(count), 0, FALSE);
        wcsncpy(text, value, 2);
        wcscat(text, L" / ");
        wcsncat(text, count, 2);
        mRowCounts2[row].mText = (const NNSG2dChar *)text;
        ClearCanvas(mRowCounts2[row], 0);
        mRowCounts2[row].DrawText();
        mRowCounts2[row].Transfer();
        break;
    }
    }
}

void ItemListView::ClearRow(int row) {
    ClearCanvas(mRowTexts[row], 0);
    mRowTexts[row].ObjTextCanvas::Transfer();
    mRowButtons[row].mVisible = FALSE;
    switch (mMode) {
    case MODE_NAME:
        break;
    case MODE_PRICE:
        ClearCanvas(mRowPrices[row], 0);
        mRowPrices[row].Transfer();
        mRowPriceIcons[row].mVisible = FALSE;
        break;
    case MODE_COUNT:
    case MODE_BOTH:
        ClearCanvas(mRowCounts2[row], 0);
        mRowCounts2[row].Transfer();
        break;
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

// A row is greyed (palette 9) in MODE_PRICE when the money cannot pay for its item, in the count modes when the
// item has none of mUnknown0C
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
    case MODE_NAME:
        palette = 8;
        break;
    case MODE_PRICE: {
        BOOL affordable = TRUE;
        if (mMoney != NULL) {
            affordable = mItems[item]->GetPrice() <= *mMoney ? TRUE : FALSE;
        }
        palette = affordable ? 8 : 9;
        break;
    }
    case MODE_COUNT:
    case MODE_BOTH:
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
void ItemListView::SetItems(Item **items, int count) {
    mScrolling = 0;
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
        mUpButton.mVisible = FALSE;
        mDownButton.mVisible = FALSE;
        mScrollBar.mVisible = FALSE;
    } else {
        mUpButton.mVisible = TRUE;
        mDownButton.mVisible = TRUE;
        mScrollBar.mVisible = TRUE;
        func_02032a0c(&mScrollBar, 0, (mItemCount - mVisibleRows) * 20, 0);
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

void ItemListView::SetVisible(BOOL visible) {
    for (int i = 0; i < mRowCount; i++) {
        mRowButtons[i].mVisible = visible;
        mRowTexts[i].mSprite.mVisible = visible;
        mRowPrices[i].mSprite.mVisible = FALSE;
        mRowPriceIcons[i].mVisible = FALSE;
        mRowCounts2[i].mSprite.mVisible = FALSE;
        switch (mMode) {
        case MODE_NAME:
            break;
        case MODE_PRICE:
            mRowPrices[i].mSprite.mVisible = visible;
            mRowPriceIcons[i].mVisible = visible;
            break;
        case MODE_COUNT:
        case MODE_BOTH:
            mRowCounts2[i].mSprite.mVisible = visible;
            break;
        }
    }
    mUpButton.mVisible = visible;
    mDownButton.mVisible = visible;
    mScrollBar.mVisible = visible;
}

// Scrolls so that the given item is the top row, as far as the list goes
void ItemListView::ScrollTo(int top) {
    int max = mItemCount - mVisibleRows;
    if (max < 0) {
        max = 0;
    }
    top = MATH_CLAMP(top, 0, max);
    mScroll = top * 20;
    func_02032a0c(&mScrollBar, 0, (mItemCount - mVisibleRows) * 20, mScroll);
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
    mScrollBar.mUnknownE4 = value;
    mTouchArea.mUnknown24 = value;
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

// --- The shops' list ----------------------------------------------------------------------------------------------

void ShopItemListView::InitMarks(int resource, int sequence) {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Init(&mMarks[i], resource, sequence, 0x80, 0x60, TRUE);
        mMarks[i].SetPosition(FxVec2(0, 0));
        mMarks[i].mOverwritePalette = 3;
    }
}

void ShopItemListView::Draw() {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Draw(&mMarks[i]);
    }
    ItemListView::Draw();
}

void ShopItemListView::SetVisible(BOOL visible) {
    ItemListView::SetVisible(visible);
    for (int i = 0; i < mRowCount; i++) {
        mMarks[i].mVisible = visible;
    }
}

static const ScreenPoint sShopCountOffset = {0x2A, -2};
static const ScreenPoint sShopPriceIconOffset = {0x66, 1};
static const ScreenPoint sShopPriceOffset = {0x34, -2};
static const ScreenPoint sShopTextOffset = {-0x64, -6};
static const ScreenPoint sShopMarkOffset = {-0x6B, 0};

void ShopItemListView::Refresh() {
    for (int i = 0; i < mRowCount; i++) {
        ScreenPoint offset;
        offset.x = 0;
        offset.y = GetRowItem(i) * 20 - mScroll;
        ScreenPoint row = AddPoints(mPosition, offset);
        mRowButtons[i].SetPosition(FxVec2(row.x << FX32_SHIFT, row.y << FX32_SHIFT));
        ScreenPoint text = AddPoints(row, sShopTextOffset);
        mRowTexts[i].SetPositionFromVec(text.x << FX32_SHIFT, text.y << FX32_SHIFT);
        ScreenPoint price = AddPoints(row, sShopPriceOffset);
        mRowPrices[i].SetPositionFromVec(price.x << FX32_SHIFT, price.y << FX32_SHIFT);
        ScreenPoint icon = AddPoints(row, sShopPriceIconOffset);
        mRowPriceIcons[i].SetPosition(FxVec2(icon.x << FX32_SHIFT, icon.y << FX32_SHIFT));
        ScreenPoint count = AddPoints(row, sShopCountOffset);
        mRowCounts2[i].SetPositionFromVec(count.x << FX32_SHIFT, count.y << FX32_SHIFT);
        ScreenPoint mark = AddPoints(row, sShopMarkOffset);
        mMarks[i].SetPosition(FxVec2(mark.x << FX32_SHIFT, mark.y << FX32_SHIFT));
    }
    func_02028154(&data_02121834, mBgPlane, mScroll % 20 - mBgOffset);
}

void ShopItemListView::DrawRow(int row, int item) {
    ItemListView::DrawRow(row, item);
    mMarks[row].mVisible = mItems[item]->mOwned == FALSE ? TRUE : FALSE;
}

void ShopItemListView::ClearRow(int row) {
    ItemListView::ClearRow(row);
    mMarks[row].mVisible = FALSE;
}

// --- The price window ---------------------------------------------------------------------------------------------

void ShopWindow::Init(ScreenRect rect) {
    func_02032fb8(this, rect);
    mMode = MODE_BUY;
    mCount = 0;
}

static const int sCanvasColors[3] = {8, 8, 1};
static const ScreenPoint sCanvasSizes[3] = {{4, 2}, {8, 2}, {8, 4}};
static const ScreenPoint sCanvasPositions[3] = {{0x2E, 0x42}, {0x7F, 0x42}, {0x43, 0x70}};
static const WindowPoint sIconPosition = {0x58, 0x76};

// The plus and minus buttons and the three canvases
void ShopWindow::InitButtons(int resource, int *charOffset, MessageFile *file) {
    Sprite_Init(&mPlusButton, resource, 6, 0x80, 0x60, TRUE);
    func_02031260(&mPlusButton, 1);
    func_02031278(&mPlusButton, 6, 7);
    mPlusButton.SetPosition(FxVec2(56 << FX32_SHIFT, 51 << FX32_SHIFT));
    func_02031eb0(&mPlusButton, 18, 16);
    mPlusButton.mUnknownE0 = 6;
    func_0203119c(&mPlusButton);
    mPlusButton.mOverwritePalette = 0;
    mPlusButton.mVisible = FALSE;
    mPlusButton.mUnknownDC = TRUE;
    Sprite_Init(&mMinusButton, resource, 8, 0x80, 0x60, TRUE);
    func_02031260(&mMinusButton, 1);
    func_02031278(&mMinusButton, 8, 9);
    mMinusButton.SetPositionFromVec(56 << FX32_SHIFT, 93 << FX32_SHIFT);
    func_02031eb0(&mMinusButton, 14, 13);
    mMinusButton.mUnknownE0 = 6;
    func_0203119c(&mMinusButton);
    mMinusButton.mOverwritePalette = 0;
    mMinusButton.mVisible = FALSE;
    mMinusButton.mUnknownDC = TRUE;
    mTextCanvases[1].mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mTextCanvases[2].mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mTextCanvases[1].mTextX = 0x40;
    mTextCanvases[2].mTextX = 0x40;
    for (int i = 0; i < 3; i++) {
        mTextCanvases[i].InitText(&gFonts[0].mFont, 0x1000, sCanvasSizes[i].x, sCanvasSizes[i].y, *charOffset, -2,
                                  1, 0, 0, 15, NULL, 2, NNS_G2D_CHARA_COLORMODE_16);
        mTextCanvases[i].mColor = sCanvasColors[i];
        ClearCanvas(mTextCanvases[i], 0);
        SetCanvasPosition(&mTextCanvases[i], sCanvasPositions[i]);
        mTextCanvases[i].mSprite.mOverwritePalette = 0;
        *charOffset += mTextCanvases[i].GetCharCount();
    }
    func_02033bb4(this, resource, 0x33, sIconPosition);
    mMessageFile = file;
}

static const ScreenRect sWindowRect = {0x14, 0x14, 0xD8, 0x98};
static const u8 sButtonA[2] = {0x14, 0x14};
static const u8 sButtonB[2] = {0x13, 0x13};
static const WindowPoint sButtonPositions[2] = {{0xB5, 0x95}, {0x4B, 0x95}};
static const WindowPoint sButtonSizes[2] = {{0x5E, 0x1E}, {0x5E, 0x1E}};
static const int sButtonMessages[2] = {6, 5};
static const ButtonSounds sButtonSounds[2] = {{-1, -1, 0, -1}, {0xC, -1, 0, -1}};
static const WindowPoint sTextPosition = {0xC, 2};
static const WindowPoint sTextSize = {0x53, 0x21};

void ShopWindow::Init(const char *screenName, int plane, int resource, int *charOffset, MessageFile *file,
                      MessageFile *buttonFile, int *money) {
    ScreenRect rect = sWindowRect;
    Init(rect);
    func_02033174(this, screenName, plane);
    func_020331bc(this, 0x3F00, rect);
    InitButtons(resource, charOffset, file);
    for (int i = 0; i <= 1; i++) {
        func_02033844(this, i, resource, sButtonB[i], sButtonA[i], sButtonPositions[i], sButtonSizes[i], buttonFile,
                      sButtonMessages[i], &gFonts[2].mFont, 0x1000, charOffset, &sButtonSounds[i], 1);
    }
    func_020338d0(this, file, 0xC3, &gFonts[0].mFont, 0x1000, sTextPosition, sTextSize, charOffset);
    func_02034f80(this, 0, 1);
    mMoney = money;
}

// The plus and minus buttons change the count, as far as the money, the item's count and the 99 the player can own
// allow
void ShopWindow::Update() {
    func_020341d4(this);
    if (!(mUnknownAD50 & 1)) {
        return;
    }
    switch (mMode) {
    case MODE_BUY:
        if (func_0203133c(&mPlusButton)) {
            int count = mCount + 1;
            if (count + mItem->mCount <= 99 && count + mItem->mUnknown0C >= 0 && GetBuyTotal(mItem, count) <= *mMoney) {
                mCount++;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 8);
                return;
            }
        }
        if (func_0203133c(&mMinusButton)) {
            int count = mCount - 1;
            if (count > 0) {
                mCount = count;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 9);
            }
        }
        break;
    case MODE_SELL:
        if (func_0203133c(&mPlusButton)) {
            int count = mCount + 1;
            if (mItem->mCount - count <= 99 && mItem->mUnknown0C - count >= 0) {
                mCount = count;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 8);
                return;
            }
        }
        if (func_0203133c(&mMinusButton)) {
            int count = mCount - 1;
            if (count > 0) {
                mCount = count;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 9);
            }
        }
        break;
    }
}

int ShopWindow::GetBuyTotal(Item *item, int count) {
    return item->GetPrice() * count;
}

void ShopWindow::Draw() {
    if (mUnknownAD54) {
        for (int i = 0; i < 3; i++) {
            mTextCanvases[i].Draw();
        }
        func_02031d7c(&mPlusButton);
        func_02031d7c(&mMinusButton);
    }
    func_020345d8(this);
}

void ShopWindow::Open(int flags, int mode) {
    func_02034720(this, flags, 1);
    mCount = 1;
    switch (mode) {
    case MODE_BUY:
        mMode = MODE_BUY;
        mTexts[0].mMessage = 0xC3;
        ClearCanvas(mTexts[0], 0);
        mTexts[0].DrawAll();
        break;
    case MODE_SELL:
        mMode = MODE_SELL;
        mTexts[0].mMessage = 0xC7;
        ClearCanvas(mTexts[0], 0);
        mTexts[0].DrawAll();
        break;
    }
}

int ShopWindow::GetTotal() {
    switch (mMode) {
    case MODE_BUY:
        return GetBuyTotal(mItem, mCount);
    case MODE_SELL:
        return GetSellTotal(mItem, mCount);
    }
}

int ShopWindow::GetSellTotal(Item *item, int count) {
    return item->GetPrice2() * count;
}

// Draws the count, the total and the money
void ShopWindow::DrawCounts() {
    wchar_t count[3];
    wchar_t total[10];
    wchar_t money[10];
    FormatNumber(mCount, count, sizeof(count), 2, FALSE);
    mTextCanvases[0].mText = (const NNSG2dChar *)count;
    ClearCanvas(mTextCanvases[0], 0);
    mTextCanvases[0].DrawText();
    FormatNumberWithCommas(GetTotal(), total, sizeof(total), 0, FALSE);
    mTextCanvases[1].mText = (const NNSG2dChar *)total;
    ClearCanvas(mTextCanvases[1], 0);
    mTextCanvases[1].DrawText();
    FormatNumberWithCommas(*mMoney, money, sizeof(money), 0, FALSE);
    mTextCanvases[2].mText = (const NNSG2dChar *)money;
    ClearCanvas(mTextCanvases[2], 0);
    mTextCanvases[2].DrawText();
}

static const WindowPoint sPlusButtonOffset = {0x38, 0x33};
static const WindowPoint sMinusButtonOffset = {0x38, 0x5D};

void ShopWindow::SetPosition(WindowPoint position) {
    MessageWindow::SetPosition(position);
    SetSpritePosition(&mPlusButton, AddPoints(position, sPlusButtonOffset));
    SetSpritePosition(&mMinusButton, AddPoints(position, sMinusButtonOffset));
    for (int i = 0; i < 3; i++) {
        SetCanvasPosition(&mTextCanvases[i], AddPoints(position, sCanvasPositions[i]));
    }
}

void ShopWindow::SetVisible(BOOL visible) {
    MessageWindow::SetVisible(visible);
    mPlusButton.mVisible = visible;
    mMinusButton.mVisible = visible;
    for (int i = 0; i < 3; i++) {
        mTextCanvases[i].mSprite.mVisible = visible;
    }
}

int HalfPriceShopWindow::GetBuyTotal(Item *item, int count) {
    return item->GetPrice() / 2 * count;
}

int HalfPriceShopWindow::GetSellTotal(Item *item, int count) {
    return item->GetPrice2() * 11 / 10 * count;
}

// --- The item panel -----------------------------------------------------------------------------------------------

static const ScreenPoint sNamePosition = {0xC, 0xA0};
static const ScreenPoint sCountPosition = {0xCD, 0xAC};
static const ScreenPoint sPricePosition = {0xB2, 0xAC};
static const ScreenPoint sCountLabelPosition = {0xC7, 0xA0};
static const ScreenPoint sPriceLabelPosition = {0xC7, 0xA0};

void ShopItemPanel::Init(int resource, int sequence, u16 frame, MessageFile *file, int *charOffset) {
    mState = STATE_SHOWN;
    mFrame = 0;
    mKind = 0;
    mMessageBase = 0;
    mShowSecondIcon = FALSE;
    mName.InitObj(&gFonts[0].mFont, 0x10, 20, 4, *charOffset, 0, 1, 0, 0, 15, NULL);
    mName.mMessageFile = file;
    SetCanvasPosition(&mName, sNamePosition);
    mName.mSprite.mOverwritePalette = 0;
    *charOffset += mName.GetCharCount();
    Sprite_Init(&mIcon, resource, sequence, 0x80, 0x60, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mIcon.mAnim));
    Sprite_SetFrame(&mIcon, frame);
    mIcon.SetPositionFromVec(0x80 << FX32_SHIFT, 0x60 << FX32_SHIFT);
    mIcon.mOverwritePalette = 1;
    Sprite_Init(&mSecondIcon, resource, (u16)(sequence + 1), 0x80, 0x60, TRUE);
    mSecondIcon.SetPositionFromVec(0x80 << FX32_SHIFT, 0x60 << FX32_SHIFT);
    mSecondIcon.mOverwritePalette = 1;
    mSecondIcon.mVisible = FALSE;
    mCount.mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mCount.InitText(&gFonts[0].mFont, 0x10, 4, 2, *charOffset, -1, 1, 0, 0, 15, NULL, 2, NNS_G2D_CHARA_COLORMODE_16);
    mCount.mColor = 8;
    ClearCanvas(mCount, 0);
    SetCanvasPosition(&mCount, sCountPosition);
    mCount.mSprite.mOverwritePalette = 0;
    mCount.mSprite.mVisible = FALSE;
    *charOffset += mCount.GetCharCount();
    mPrice.mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mPrice.mTextX = 0x40;
    mPrice.InitText(&gFonts[0].mFont, 0x10, 8, 2, *charOffset, -2, 1, 0, 0, 15, NULL, 2, NNS_G2D_CHARA_COLORMODE_16);
    mPrice.mColor = 8;
    ClearCanvas(mPrice, 2);
    SetCanvasPosition(&mPrice, sPricePosition);
    mPrice.mSprite.mOverwritePalette = 0;
    mPrice.mSprite.mVisible = FALSE;
    *charOffset += mPrice.GetCharCount();
    mCountLabel.InitText(&gFonts[0].mFont, 0x10, 4, 2, *charOffset, 0, 1, 0, 0, 15, NULL, 2,
                         NNS_G2D_CHARA_COLORMODE_16);
    mCountLabel.mColor = 8;
    ClearCanvas(mCountLabel, 0);
    SetCanvasPosition(&mCountLabel, sCountLabelPosition);
    mCountLabel.mSprite.mOverwritePalette = 0;
    mCountLabel.mSprite.mVisible = FALSE;
    *charOffset += mCountLabel.GetCharCount();
    mCountLabel.mText = (const NNSG2dChar *)L"所持数";
    mCountLabel.DrawText();
    mPriceLabel.InitText(&gFonts[0].mFont, 0x10, 4, 2, *charOffset, 0, 1, 0, 0, 15, NULL, 2,
                         NNS_G2D_CHARA_COLORMODE_16);
    mPriceLabel.mColor = 8;
    ClearCanvas(mPriceLabel, 0);
    SetCanvasPosition(&mPriceLabel, sPriceLabelPosition);
    mPriceLabel.mSprite.mOverwritePalette = 0;
    mPriceLabel.mSprite.mVisible = FALSE;
    *charOffset += mPriceLabel.GetCharCount();
    mPriceLabel.mText = (const NNSG2dChar *)L"売値";
    mPriceLabel.DrawText();
}

void ShopItemPanel::Draw() {
    mName.Draw();
    Sprite_Draw(&mIcon);
    Sprite_Draw(&mSecondIcon);
    mCount.Draw();
    mPrice.Draw();
    mCountLabel.Draw();
    mPriceLabel.Draw();
}

void ShopItemPanel::Show(Item *item, int count) {
    mItem = item;
    SetItem(count);
}

// Shows the item's name (count -1) or a message of a count, and the number owned or the price
void ShopItemPanel::SetItem(int count) {
    if (count == -1) {
        mName.mSprite.mVisible = TRUE;
        mName.mMessage = mMessageBase + mItem->mId;
        ClearCanvas(mName, 0);
        mName.DrawAll();
    } else {
        mName.mSprite.mVisible = TRUE;
        mName.mMessage = GetCountMessage(count);
        ClearCanvas(mName, 0);
        mName.DrawAll();
    }
    switch (mKind) {
    case 0:
    case 2:
    case 3: {
        wchar_t text[8];
        MI_CpuClear16(text, sizeof(text));
        FormatNumber(mItem->mCount, text, sizeof(text), 2, FALSE);
        mCount.mText = (const NNSG2dChar *)text;
        ClearCanvas(mCount, 0);
        mCount.DrawText();
        break;
    }
    case 1: {
        wchar_t text[16];
        wchar_t price[16];
        MI_CpuClear16(text, sizeof(text));
        FormatNumberWithCommas(mItem->GetPrice2(), price, sizeof(price), 5, FALSE);
        wcsncpy(text, price, 16);
        wcscat(text, L"円");
        mPrice.mText = (const NNSG2dChar *)text;
        ClearCanvas(mPrice, 0);
        mPrice.DrawText();
        break;
    }
    }
}

// Slides the panel in and out
void ShopItemPanel::Update() {
    switch (mState) {
    case STATE_SHOWN:
        break;
    case STATE_SLIDING_IN: {
        int y = (mFrame - 10) * 8;
        if (y > 56) {
            y = 56;
        } else if (y < 0) {
            y = 0;
        }
        SetY(56 - y);
        if (y >= 56) {
            SetState(STATE_SHOWN);
        }
        mFrame++;
        break;
    }
    case STATE_SLIDING_OUT: {
        int y = (10 - mFrame) * 8;
        if (y > 56) {
            y = 56;
        } else if (y < 0) {
            y = 0;
        }
        SetY(56 - y);
        if (y >= 56) {
            SetState(STATE_HIDDEN);
        }
        mFrame++;
        break;
    }
    case STATE_HIDDEN:
        break;
    }
}

void ShopItemPanel::SetState(int state) {
    mState = state;
    mFrame = 0;
    switch (state) {
    case STATE_SHOWN:
        break;
    case STATE_SLIDING_IN:
        mName.mSprite.mVisible = TRUE;
        mIcon.mVisible = TRUE;
        mSecondIcon.mVisible = mShowSecondIcon;
        switch (mKind) {
        case 0:
        case 2:
        case 3:
            mCount.mSprite.mVisible = TRUE;
            mCountLabel.mSprite.mVisible = TRUE;
            break;
        case 1:
            mPrice.mSprite.mVisible = TRUE;
            mPriceLabel.mSprite.mVisible = TRUE;
            break;
        }
        SetY(56);
        break;
    case STATE_SLIDING_OUT:
        break;
    case STATE_HIDDEN:
        mName.mSprite.mVisible = FALSE;
        mIcon.mVisible = FALSE;
        mSecondIcon.mVisible = FALSE;
        mCount.mSprite.mVisible = FALSE;
        mCountLabel.mSprite.mVisible = FALSE;
        mPrice.mSprite.mVisible = FALSE;
        mPriceLabel.mSprite.mVisible = FALSE;
        SetY(56);
        break;
    }
}

// What the panel shows: the number owned (kinds 0, 2 and 3) or the price (1), and where the names are
void ShopItemPanel::SetKind(int kind) {
    mKind = kind;
    switch (kind) {
    case 0:
    case 2:
    case 3:
        mCount.mSprite.mVisible = TRUE;
        mCountLabel.mSprite.mVisible = TRUE;
        mPrice.mSprite.mVisible = FALSE;
        mPriceLabel.mSprite.mVisible = FALSE;
        break;
    case 1:
        mCount.mSprite.mVisible = FALSE;
        mCountLabel.mSprite.mVisible = FALSE;
        mPrice.mSprite.mVisible = TRUE;
        mPriceLabel.mSprite.mVisible = TRUE;
        break;
    }
    const int messageBases[4] = {0, 0, 0xFF, 0x22B};
    mMessageBase = messageBases[mKind];
}

void ShopItemPanel::SetY(int y) {
    fx32 y0 = (y + 0xA0) << FX32_SHIFT;
    fx32 y1 = (y + 0x60) << FX32_SHIFT;
    fx32 y2 = (y + 0xAC) << FX32_SHIFT;
    mName.mSprite.mPosition.y = y0;
    mIcon.mPosition.y = y1;
    mSecondIcon.mPosition.y = y1;
    mCount.mSprite.mPosition.y = y2;
    mCountLabel.mSprite.mPosition.y = y0;
    mPrice.mSprite.mPosition.y = y2;
    mPriceLabel.mSprite.mPosition.y = y0;
}

void ShopItemPanel::SetShowSecondIcon(BOOL show) {
    mShowSecondIcon = show;
    if (mState <= STATE_SLIDING_IN) {
        mSecondIcon.mVisible = mShowSecondIcon;
    }
}

// The message for a count of 5 to 30
int ShopItemPanel::GetCountMessage(int count) {
    switch (count) {
    case 5:
        return 0x279;
    case 10:
        return 0x27A;
    case 15:
        return 0x27B;
    case 20:
        return 0x27C;
    case 25:
        return 0x27D;
    case 30:
        return 0x27E;
    default:
        return 0x279;
    }
}

// --- The staff ----------------------------------------------------------------------------------------------------

// Mounts the head staff's archives: its model, its clothes, its animations and its head
void ShopStaff::Mount() {
    mCharacter.CharacterModel::SetArchive("3HM", 0, 0);
    mMannequin.mBody.SetArchive("3CM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacter.mArchiveName, mCharacter.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mMannequin.mBody.mArchiveName, mMannequin.mBody.mArchivePath,
                                   0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_contents_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3CH", "Mdl/Misc/headStaff_3D_LZ.bin", 0);
}

void ShopStaff::Unmount() {
    func_0202aecc(data_021223e4, 28);
    func_0202aecc(data_021223e4, 29);
    func_0202aecc(data_021223e4, 30);
    func_0202aecc(data_021223e4, 31);
    ArchiveManager_Unmount(&gArchiveManager, "3HM");
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ArchiveManager_Unmount(&gArchiveManager, "3CM");
    ArchiveManager_Unmount(&gArchiveManager, "3CH");
}

// Loads the resident who works in the shop if there is one, or else the head staff, in the shop's uniform
void ShopStaff::Load(int shop, VecFx32 position, u16 angle, u16 greetingAngle, TtsPlayer *player,
                     SpeechWindow *window, MessageFile *file) {
    mShop = shop;
    mPosition = position;
    mAngle = angle;
    mGreetingAngle = greetingAngle;
    mTtsPlayer = player;
    mSpeechWindow = window;
    mMessageFile = file;
    mHeldModel = NULL;
    int uniform = GetUniform(shop);
    FindResidentAt(shop, &mResident, &mResidentIndex);
    if (!mResident) {
        mResidentIndex = 0;
    }
    mGreet = FALSE;
    mTalking = FALSE;
    ModelResources_SetModel(data_021223e4, 28, mCharacter.mModelPath);
    ModelResources_SetModel(data_021223e4, 30, mMannequin.mBody.mModelPath);
    ModelResources_SetModel(data_021223e4, 31, "3CH:/head.imd");
    void *animations = func_0202ad04(data_021223e4, 28, "3HA", 0);
    mCharacter.Init(28, 0, 0);
    func_02029660(mCharacter.mAnim.mUnknown04, RESOURCE_BCA0, animations);
    mCharacter.SetAnimation(RESOURCE_BCA0, 33);
    if (mResident) {
        func_0202c264(&gGameData, mResidentIndex, &mCharacter);
        func_0202aecc(data_021223e4, 28);
        ArchiveManager_Unmount(&gArchiveManager, mCharacter.mArchiveName);
        mCharacter.CharacterModel::SetArchive(mCharacter.mArchiveName, uniform, MiiData_GetSex(&mCharacter));
        ArchiveManager_MountCompressed(&gArchiveManager, mCharacter.mArchiveName, mCharacter.mArchivePath, 1);
        ModelResources_SetModel(data_021223e4, 28, mCharacter.mModelPath);
        mCharacter.MakeModel(28, 0);
        mCharacter.MakeFace();
        mCharacter.UpdateBodyColors();
        mCharacter.UpdateBodyScale();
        mCharacter.UpdateAnimSpeed();
        mCharacter.SetVisible(TRUE);
        mCharacter.mShadowVisible = TRUE;
        VecFx32 pos = mPosition;
        mCharacter.mPos.x = pos.x;
        mCharacter.mPos.y = pos.y;
        mCharacter.mPos.z = pos.z;
        mCharacter.SetAngle(mAngle);
        mCharacter.SetFace(8);
        mCharacter.Update();
        func_0209d898(&mCharacter, player);
    } else {
        mMannequin.Init(30, 0, 31, 0);
        func_0202aecc(data_021223e4, 30);
        ArchiveManager_Unmount(&gArchiveManager, "3CM");
        mMannequin.mBody.SetArchive("3CM", uniform, 0);
        ArchiveManager_MountCompressed(&gArchiveManager, "3CM", mMannequin.mBody.mArchivePath, 1);
        ModelResources_SetModel(data_021223e4, 30, mMannequin.mBody.mModelPath);
        mMannequin.mBody.MakeModel(30, 0);
        mMannequin.LoadHead(31, 0);
        mMannequin.mBody.LoadTexturePalette();
        mMannequin.mBody.SetPaletteColor1(0x5FBE);
        void *headAnimations = func_0202ad04(data_021223e4, 30, "3HA", 0);
        func_02029660(mMannequin.mBody.mAnim.mUnknown04, RESOURCE_BCA0, headAnimations);
        mMannequin.SetAnimation(RESOURCE_BCA0, 33);
        mMannequin.mBody.mShadowVisible = TRUE;
        VecFx32 pos = mPosition;
        mMannequin.mBody.mPos.x = pos.x;
        mMannequin.mBody.mPos.y = pos.y;
        mMannequin.mBody.mPos.z = pos.z;
        mMannequin.SetAngle(mAngle);
        mMannequin.ApplyAngle();
        mMannequin.Draw();
    }
    SetState(STATE_IDLE);
}

void ShopStaff::Update() {
    if (mResident) {
        UpdateResident();
    } else {
        UpdateHeadStaff();
    }
    UpdateHeldModel();
}

void ShopStaff::Draw() {
    if (mResident) {
        mCharacter.Draw();
    } else {
        mMannequin.Draw();
    }
}

// The clothes the staff wears in each of the four shops
int ShopStaff::GetUniform(int shop) {
    int clothes = 0;
    switch (shop) {
    case 0:
        clothes = 0x87;
        break;
    case 1:
        clothes = 0x73;
        break;
    case 2:
        clothes = 0x97;
        break;
    case 3:
        clothes = 0xC0;
        break;
    }
    return clothes;
}

void ShopStaff::UpdateResident() {
    switch (mState) {
    case STATE_IDLE:
        if (mTalking) {
            SetState(STATE_TALKING);
        } else if (mGreet) {
            SetState(STATE_GREETING);
        }
        break;
    case STATE_GREETING:
        switch (mStep) {
        case 0:
            if (mCharacter.mBlendFrames <= 0) {
                BowDown();
                mStep++;
            }
            break;
        case 1:
            mCharacter.Turn(0xFF1C);
            if (mCharacter.mAngle < mGreetingAngle) {
                mCharacter.SetAngle(mGreetingAngle);
                mStep++;
            }
            break;
        case 2:
            if (mTalking || !mGreet) {
                SetState(STATE_TURNING_BACK);
            }
            break;
        }
        break;
    case STATE_TALKING:
        switch (mStep) {
        case 0:
            mTalking = FALSE;
            mStep++;
            break;
        case 1:
            if (mCharacter.mBlendFrames <= 0) {
                mSpeechWindow->Show(mMessages[0], 0, 120);
                mTtsPlayer->Speak((const u16 *)mMessageFile->GetText(mMessages[1]), 0);
                mCharacter.BlendToAnimation(RESOURCE_BCA0, 29);
                AnimModel_FindAnimation(&mCharacter.mAnim, RESOURCE_BCA0, mCharacter.mAnimId)->mPlayOnce = TRUE;
                mStep++;
            }
            break;
        case 2:
            if (AnimModel_FindAnimation(&mCharacter.mAnim, RESOURCE_BCA0, mCharacter.mAnimId)->mFinished) {
                mCharacter.BlendToAnimation(RESOURCE_BCA0, 33);
                mAnimationDone = TRUE;
            }
            if (mTtsPlayer->IsEngineIdle()) {
                mSpeechDone = TRUE;
            }
            if (mAnimationDone && mSpeechDone) {
                SetState(STATE_IDLE);
            }
            if (mTalking) {
                mTalking = FALSE;
                mStep = 2;
                mSpeechWindow->Show(mMessages[0], 0, 120);
                mTtsPlayer->Speak((const u16 *)mMessageFile->GetText(mMessages[1]), 0);
                mCharacter.SetAnimation(RESOURCE_BCA0, 29);
                AnimModel_FindAnimation(&mCharacter.mAnim, RESOURCE_BCA0, mCharacter.mAnimId)->mPlayOnce = TRUE;
            }
            break;
        }
        break;
    case STATE_TURNING_BACK:
        switch (mStep) {
        case 0:
            if (mCharacter.mBlendFrames <= 0) {
                mCharacter.BlendToAnimation(RESOURCE_BCA0, 33);
                mStep++;
            }
            break;
        case 1:
            mCharacter.Turn(0xE4);
            if (mAngle < mCharacter.mAngle) {
                mCharacter.SetAngle(mAngle);
                SetState(STATE_IDLE);
            }
            break;
        }
        break;
    }
    mCharacter.LipSync(mTtsPlayer->GetPhonemeName(), 1);
    mCharacter.Update();
}

void ShopStaff::UpdateHeadStaff() {
    switch (mState) {
    case STATE_IDLE:
        if (mTalking) {
            SetState(STATE_TALKING);
        } else if (mGreet) {
            SetState(STATE_GREETING);
        }
        break;
    case STATE_GREETING:
        switch (mStep) {
        case 0:
            if (mMannequin.mBody.mBlendFrames <= 0) {
                BowDown();
                mStep++;
            }
            break;
        case 1:
            mMannequin.Turn(0xFF1C);
            if (mMannequin.mTargetAngle < mGreetingAngle) {
                mMannequin.SetAngle(mGreetingAngle);
                mStep++;
            }
            break;
        case 2:
            if (mTalking || !mGreet) {
                SetState(STATE_TURNING_BACK);
            }
            break;
        }
        break;
    case STATE_TALKING:
        switch (mStep) {
        case 0:
            mTalking = FALSE;
            mStep++;
            break;
        case 1:
            if (mMannequin.mBody.mBlendFrames <= 0) {
                mSpeechWindow->Show(mMessages[0], 0, 120);
                mTtsPlayer->Speak((const u16 *)mMessageFile->GetText(mMessages[1]), 0);
                mMannequin.BlendToAnimation(RESOURCE_BCA0, 29);
                AnimModel_FindAnimation(&mMannequin.mBody.mAnim, RESOURCE_BCA0, mMannequin.mBody.mAnimId)
                    ->mPlayOnce = TRUE;
                mStep++;
            }
            break;
        case 2:
            if (AnimModel_FindAnimation(&mMannequin.mBody.mAnim, RESOURCE_BCA0, mMannequin.mBody.mAnimId)
                    ->mFinished) {
                mMannequin.BlendToAnimation(RESOURCE_BCA0, 33);
                mAnimationDone = TRUE;
            }
            if (mTtsPlayer->IsEngineIdle()) {
                mSpeechDone = TRUE;
            }
            if (mAnimationDone && mSpeechDone) {
                SetState(STATE_IDLE);
            }
            if (mTalking) {
                mTalking = FALSE;
                mStep = 2;
                mSpeechWindow->Show(mMessages[0], 0, 120);
                mTtsPlayer->Speak((const u16 *)mMessageFile->GetText(mMessages[1]), 0);
                mMannequin.SetAnimation(RESOURCE_BCA0, 29);
                AnimModel_FindAnimation(&mMannequin.mBody.mAnim, RESOURCE_BCA0, mMannequin.mBody.mAnimId)
                    ->mPlayOnce = TRUE;
            }
            break;
        }
        break;
    case STATE_TURNING_BACK:
        switch (mStep) {
        case 0:
            if (mMannequin.mBody.mBlendFrames <= 0) {
                mMannequin.BlendToAnimation(RESOURCE_BCA0, 33);
                mStep++;
            }
            break;
        case 1:
            mMannequin.Turn(0xE4);
            if (mAngle < mMannequin.mTargetAngle) {
                mMannequin.SetAngle(mAngle);
                SetState(STATE_IDLE);
            }
            break;
        }
        break;
    }
    mMannequin.ApplyAngle();
}

void ShopStaff::SetState(int state) {
    mState = state;
    mStep = 0;
    mAnimationDone = FALSE;
    mSpeechDone = FALSE;
}

BOOL ShopStaff::IsIdle() {
    BOOL idle = FALSE;
    if (!mTalking) {
        BOOL waiting = TRUE;
        if (mState != STATE_IDLE && mState != STATE_GREETING) {
            waiting = FALSE;
        }
        if (waiting) {
            idle = TRUE;
        }
    }
    return idle;
}

// The bow of the greeting: the head staff's shop has a bow of its own
void ShopStaff::BowDown() {
    int animation;
    switch (mShop) {
    case 0:
    case 1:
    case 2:
        animation = 0x22;
        break;
    case 3:
        animation = 0x2A;
        break;
    }
    if (mResident) {
        mCharacter.BlendToAnimation(RESOURCE_BCA0, animation);
    } else {
        mMannequin.BlendToAnimation(RESOURCE_BCA0, animation);
    }
}

// Puts the held model in the right hand
void ShopStaff::UpdateHeldModel() {
    if (mHeldModel == NULL) {
        return;
    }
    if (mResident) {
        MtxFx43 *matrices = mCharacter.mJointMatrices;
        mHeldModel->SetPosition(matrices[CharacterModel::JOINT_R_HAND]._30, matrices[CharacterModel::JOINT_R_HAND]._31,
                                matrices[CharacterModel::JOINT_R_HAND]._32);
        MtxFx33 *rotations = mCharacter.mJointRotations;
        MI_Copy36B(&rotations[CharacterModel::JOINT_R_HAND], &mHeldModel->mRotation);
        mHeldModel->mRotY = 0x4000;
    } else {
        MtxFx43 *matrices = mMannequin.mBody.mJointMatrices;
        mHeldModel->SetPosition(matrices[CharacterModel::JOINT_R_HAND]._30, matrices[CharacterModel::JOINT_R_HAND]._31,
                                matrices[CharacterModel::JOINT_R_HAND]._32);
        MtxFx33 *rotations = mMannequin.mBody.mJointRotations;
        MI_Copy36B(&rotations[CharacterModel::JOINT_R_HAND], &mHeldModel->mRotation);
        mHeldModel->mRotY = 0x4000;
    }
}

// --- The counter --------------------------------------------------------------------------------------------------

static inline void SetUpLight(Light *light, Camera *camera, GXRgb color, VecFx32 direction) {
    light->mColor = color;
    func_0202aa28(light, camera, direction);
}

static inline void SetUpLight2(Light *light, Camera *camera, GXRgb color, const VecFx32 &direction) {
    light->mColor = color;
    func_0202aa28(light, camera, direction);
}

static GXRgb sLightColors[2] = {GX_RGB(31, 31, 31), GX_RGB(31, 31, 31)};
static const VecFx32 sLightDirection0 = {0x640, -0x258, -0xFA0};
static const VecFx32 sLightDirection1 = {0, -0xFA0, 0x5DC};

void ShopCounter::MountStaff() {
    mStaff.Mount();
}

// The speech window, and the 3D view: camera, lights and clear colour
void ShopCounter::Init(int *charOffset, MessageFile *file, VecFx32 cameraPosition, VecFx32 cameraTarget) {
    mPanelShown = FALSE;
    mTalking = FALSE;
    mTtsPlayer.InitEngine();
    mMessageFile = file;
    *charOffset += mSpeechWindow.Init(*charOffset, file, 4, 0x10, &mTtsPlayer, FALSE);
    mSpeechWindow.mText[0].mVAlign = 1;
    mSpeechWindow.Hide();
    func_02023ee0(data_0211d604, 4, 17, 8, 8);
    func_0202b0e8(data_02122824);
    *(BOOL *)(data_0211d604 + 0x48) = TRUE;
    *(BOOL *)(data_0211d604 + 0xEC) = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    Camera *camera = (Camera *)data_02122764;
    func_0202a640(camera);
    camera->mUnknown00 = 0x281;
    camera->mUnknown04 = 0xFCE;
    camera->mUnknown08 = 0x1555;
    camera->mUnknown0C = FX32_ONE;
    camera->mUnknown10 = 0x50000;
    camera->mPosition = cameraPosition;
    camera->mTarget = cameraTarget;
    Light *light0 = &data_021227cc[0];
    light0->mColor = sLightColors[1];
    func_0202aa28(light0, camera, sLightDirection0);
    Light *light1 = &data_021227cc[1];
    light1->mColor = sLightColors[0];
    func_0202aa28(light1, camera, sLightDirection1);
    func_02028280(&data_02121834, 1, 1);
}

void ShopCounter::LoadStaff(int shop, VecFx32 position, u16 angle, u16 greetingAngle) {
    mStaff.Load(shop, position, angle, greetingAngle, &mTtsPlayer, &mSpeechWindow, mMessageFile);
}

void ShopCounter::InitPanel(int kind, int resource, int sequence, u16 frame, int *charOffset, MessageFile *file) {
    mPanel.Init(resource, sequence, frame, file, charOffset);
    mPanel.SetState(ShopItemPanel::STATE_HIDDEN);
    mPanel.SetKind(kind);
}

// The panel hides while the speech window is shown, and comes back after
void ShopCounter::Update() {
    BOOL shown = mSpeechWindow.mShown;
    mStaff.Update();
    mTtsPlayer.Update();
    mSpeechWindow.Update();
    mPanel.Update();
    if (shown != mSpeechWindow.mShown && mSpeechWindow.mShown) {
        mPanel.SetState(ShopItemPanel::STATE_HIDDEN);
    }
    if (mSpeechWindow.mShown) {
        return;
    }
    if (!mPanelShown) {
        return;
    }
    if (mPanel.mState != ShopItemPanel::STATE_HIDDEN) {
        return;
    }
    mPanel.SetState(ShopItemPanel::STATE_SLIDING_IN);
}

void ShopCounter::Draw() {
    mSpeechWindow.Draw();
    mPanel.Draw();
    mStaff.Draw();
}

void ShopCounter::UnmountStaff() {
    mStaff.Unmount();
}

void ShopCounter::ShowPanel(Item *item, int count) {
    mPanelShown = TRUE;
    mPanel.Show(item, count);
}

void ShopCounter::ClearPanelItem() {
    mPanel.SetItem(-1);
}

void ShopCounter::HidePanel() {
    mStaff.mGreet = FALSE;
    mSpeechWindow.Hide();
    mPanelShown = FALSE;
}

void ShopCounter::SayMessagesCA() {
    mStaff.mMessages[0] = 0xCA;
    mStaff.mMessages[1] = 0xCB;
    mStaff.mTalking = TRUE;
    mTalking = TRUE;
}

void ShopCounter::SayMessagesCE() {
    mStaff.mMessages[0] = 0xCE;
    mStaff.mMessages[1] = 0xCF;
    mStaff.mTalking = TRUE;
}

void ShopCounter::SayMessagesD3() {
    mStaff.mMessages[0] = 0xD3;
    mStaff.mMessages[1] = 0xD4;
    mStaff.mTalking = TRUE;
}

void ShopCounter::SayMessagesD5() {
    mStaff.mMessages[0] = 0xD5;
    mStaff.mMessages[1] = 0xD6;
    mStaff.mTalking = TRUE;
}

void ShopCounter::GreetCustomer() {
    mStaff.mGreet = TRUE;
}

BOOL ShopCounter::IsStaffIdle() {
    return mStaff.IsIdle();
}

// Picks at random one of the residents whose event (0x14) is at the given place, for the staff to be
BOOL FindResidentAt(int place, BOOL *found, int *index) {
    int candidates[100];
    int count = 0;
    *found = FALSE;
    *index = 0;
    for (int i = 0; i < 100; i++) {
        if (func_0202c354(&gGameData, i) && func_0202c468(&gGameData, i) == 0x14 &&
            place == func_0202c490(&gGameData, i)) {
            candidates[count] = i;
            count++;
        }
    }
    if (count != 0) {
        *found = TRUE;
        *index = candidates[MATH_Rand16(&gRandom, count)];
    } else {
        *found = FALSE;
    }
    return *found;
}
