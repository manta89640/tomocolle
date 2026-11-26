#include "AlbumScene.h"
#include "MainProgram.h"

#include <wchar.h>

// Overlay 100: the album (AlbumScene). One source file: the lists' helpers, the list view's drawing, the tabs and
// filters, the item model, the list of the lower screen with what it shows above, the scene, and the scene's
// implicit destructor, which the compiler emits with the scene's virtual table, at the end of .text.

// --- The file's constants ---

static const ScreenPoint sDownButtonSize = {14, 12};
static const u16 sGoodsFilterOnSequences[4] = {0x26, 0x28, 0x15, 0x17};
static const ScreenPoint sCountOffset = {0x2A, -2};
static const ScreenPoint sUpButtonSize = {14, 12};
static const ScreenPoint sOwnedTextPosition = {3, 0x25};
static const ScreenPoint sCostumeFilterTouchSize = {42, 24};
static const ScreenPoint sGoodsFilterTouchSize = {52, 24};
static const u16 sGoodsFilterSequences[4] = {0x25, 0x27, 0x14, 0x16};
static const ScreenPoint sUpButtonPosition = {0xF4, 0x45};
static const ScreenPoint sDownButtonPosition = {0xF4, 0xAF};
static const ScreenPoint sPriceTextPosition = {0x8C, 0x80};
static const ScreenPoint sRowsPosition = {0x77, 0x48};
static const ScreenPoint sTextOffset = {-0x50, -6};
static const ScreenPoint sScrollBarPosition = {0xF4, 0x69};
static const ScreenPoint sBackPosition = {0xF2, 0x0D};
static const ScreenPoint sNumberOffset = {-0x6D, -6};
static const ScreenPoint sExplainTextPosition = {0x35, 0xA0};
static const ScreenPoint sBackTouchSize = {24, 22};
static const ScreenPoint sPriceOffset = {0x34, -2};
static const ScreenPoint sTabTouchSize = {44, 22};
static const ScreenPoint sPriceIconOffset = {0x66, 1};
static const ScreenPoint sCountTextPosition = {3, 0x34};
static const ScreenPoint sNameTextPosition = {0x3F, 0x90};
static const u16 sTabOnSequences[5] = {0x2C, 0x2E, 0x30, 0x32, 0x2A};
static const u16 sCostumeFilterSequences[5] = {0x18, 0x1A, 0x1C, 0x1E, 0x20};
static const u16 sTabSequences[5] = {0x2B, 0x2D, 0x2F, 0x31, 0x29};
static const u16 sCostumeFilterOnSequences[5] = {0x19, 0x1B, 0x1D, 0x1F, 0x21};
static const VecFx32 sCameraTarget = {0, 0xE66, 0};
static const VecFx32 sLight0Direction = {0, -0xBB8, -0x2328};
static const VecFx32 sCameraPosition = {0, 0x1CCC, 0xD000};
static const VecFx32 sLight1Direction = {0x2710, -0x2710, 0};
static const VecFx32 sItemPosition = {0, 0x800, 0};
static const VecFx32 sZero = {0, 0, 0};
static const VecFx32 sRoomPosition = {0, 0x1000, 0};
static const int sGoodsFilters[4] = {1, 2, 8, 0x10};
static const ScreenRect sScrollKnobRect = {-8, -8, 16, 16};
static const int sCostumeFilters[5] = {0, 1, 2, 3, 4};
static const ScreenPoint sGoodsFilterPositions[4] = {{0x20, 0x2C}, {0x58, 0x2C}, {0x90, 0x2C}, {0xC8, 0x2C}};
static const ScreenPoint sTabPositions[5] = {{0x17, 0x0D}, {0x44, 0x0D}, {0x71, 0x0D}, {0x9E, 0x0D}, {0xCB, 0x0D}};
static const ScreenPoint sCostumeFilterPositions[5] = {
    {0x1A, 0x2C}, {0x48, 0x2C}, {0x76, 0x2C}, {0xA4, 0x2C}, {0xD2, 0x2C},
};

static GXRgb sInitLightColors[2] = {GX_RGB(31, 31, 31), GX_RGB(31, 31, 31)};
static GXRgb sLightColors[2] = {GX_RGB(31, 31, 31), GX_RGB(31, 31, 31)};

// The upper screen's background of each tab: goods, costumes, interiors, the second goods and the third
static const char *sBgCharNames[5] = {
    "SAB:/BG_food.ncg", "SAB:/BG_costume.ncg", "SAB:/BG_interior.ncg", "SAB:/BG_item.ncg", "SAB:/BG_souvenir.ncg",
};
static const char *sBgPaletteNames[5] = {
    "SAB:/BG_food.ncl", "SAB:/BG_costume.ncl", "SAB:/BG_interior.ncl", "SAB:/BG_item.ncl", "SAB:/BG_souvenior.ncl",
};
static const char *sBgScreenNames[5] = {
    "SAB:/BG_food.nsc", "SAB:/BG_costume.nsc", "SAB:/BG_interior.nsc", "SAB:/BG_item.nsc", "SAB:/BG_souvenir.nsc",
};

// --- The helpers -----------------------------------------------------------------------------------------------

static inline void ClearCanvas(TextCanvas &canvas, int color) { NNS_G2dCharCanvasClear(&canvas.mCanvas, color); }

// The sum of two points
template <class A, class B> static inline A AddPoints(A a, B b) {
    A p;
    p.x = a.x + b.x;
    p.y = a.y + b.y;
    return p;
}

static inline ScreenPoint MakePoint(int x, int y) {
    ScreenPoint p;
    p.x = x;
    p.y = y;
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

static inline void SetPixelPosition(Sprite *sprite, ScreenPoint position) {
    sprite->SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

// Plays the mannequin's body animation of that name
static inline void SetMannequinAnimation(Mannequin *mannequin, const char *name) {
    mannequin->SetAnimation(RESOURCE_BCA0,
                            ResourceSet_FindIndexByName((void *)mannequin->mAnim.mUnknown04, RESOURCE_BCA0, name));
}

// --- What the player has, by tab -------------------------------------------------------------------------------

// Counts the goods of a kind, and those of them the player has had
static void CountGoods(Inventory *inventory, int *count, int *owned, int kind) {
    for (int i = 0; i < inventory->mCount; i++) {
        if (kind == gGoodsTable[inventory->mList[i]->mId].mKind) {
            (*count)++;
            if (inventory->mList[i]->mOwned) {
                (*owned)++;
            }
        }
    }
}

// Lists the goods of a kind that pass the filter
static void GetGoods(Inventory *inventory, Item **list, int *count, int kind, u32 flags) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < inventory->mCount; i++) {
        InventoryItem *item = inventory->mList[i];
        if (kind == gGoodsTable[item->mId].mKind && (flags & gGoodsTable[item->mId].mFlags)) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

// Lists the goods of kind 3 (the second goods tab)
static void GetGoodsKind3(Inventory *inventory, Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < inventory->mCount; i++) {
        InventoryItem *item = inventory->mList[i];
        if (gGoodsTable[item->mId].mKind == 3) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

// Lists the goods of kinds 4 to 6 (the third goods tab)
static void GetGoodsKinds4To6(Inventory *inventory, Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < inventory->mCount; i++) {
        InventoryItem *item = inventory->mList[i];
        int kind = gGoodsTable[item->mId].mKind;
        if (kind >= 4 && kind <= 6) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

// Counts the clothes, and those the player has had (clothes 0 counts as had)
static void CountClothes(ClothesList *clothes, int *count, int *owned) {
    for (int i = 0; i < clothes->mCount; i++) {
        (*count)++;
        if (clothes->mItems[i].mOwned || clothes->mItems[i].mId == 0) {
            (*owned)++;
        }
    }
}

// Lists the clothes of a kind
static void GetClothes(ClothesList *clothes, Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < clothes->mCount; i++) {
        ClothesItem *item = clothes->mList[i];
        if (kind == gClothesTable[item->mId].mKind) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

// Counts the interiors of a kind other than 0, and those the player has had
static void CountInteriors(InteriorList *interiors, int *count, int *owned) {
    for (int i = 0; i < interiors->mCount; i++) {
        if (gInteriorTable[interiors->mItems[i].mId].mKind != 0) {
            (*count)++;
            if (interiors->mItems[i].mOwned) {
                (*owned)++;
            }
        }
    }
}

// Lists the interiors of a kind other than 0
static void GetInteriors(InteriorList *interiors, Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < interiors->mCount; i++) {
        InteriorItem *item = interiors->mList[i];
        if (gInteriorTable[item->mId].mKind != 0) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

// --- The list view ---------------------------------------------------------------------------------------------

// Scrolls by the arrow buttons, the scroll bar and dragging, and takes the touches of the rows
void AlbumListView::Update() {
    BOOL moveBar = FALSE;
    BOOL scrolled = moveBar;
    mTouched = scrolled;
    BOOL up = SpriteButton_Update(&mUpButton);
    BOOL down = SpriteButton_Update(&mDownButton);
    int max = (mItemCount - mVisibleRows) * 20;
    switch (mScrolling) {
    case 0:
        ScrollBar_Update(&mScrollBar);
        TouchScrollArea_Update(&mTouchArea, mScroll, 0, max);
        if (mScrollBar.mDragging) {
            mScroll = ScrollBar_GetPosition(&mScrollBar, 0, max);
            scrolled = TRUE;
        } else if (mScrollBar.mReleased) {
            mScroll = (ScrollBar_GetPosition(&mScrollBar, 0, max) + 10) / 20 * 20;
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
        } else if (gTouchInput.mTouching && ScreenRect_Contains(&mRect, &gTouchInput.mPoint) && mTouchable) {
            for (int i = 0; i < mRowCount; i++) {
                if (SpriteButton_Update(&mRowButtons[i])) {
                    if (mTouchedRow != -1 && mTouchedRow != i) {
                        SpriteButton_SetSelected(&mRowButtons[mTouchedRow], FALSE);
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
        ScrollBar_SetPosition(&mScrollBar, 0, max, mScroll);
    }
    if (scrolled) {
        SetScroll(mScroll);
        DrawDirtyRows();
        PlaceRows();
    }
}

// Sets the message file of the rows' names, and the message shown for what was never had
void AlbumListView::SetMessages(MessageFile *file, u32 unknownMessage, BOOL zeroUnknown) {
    for (int i = 0; i < mRowCount; i++) {
        mRowTexts[i].mMessageFile = file;
    }
    mUnknownMessage = unknownMessage;
    mZeroUnknown = zeroUnknown;
}

// ItemListView::DrawRow, with the row's number, and the unknown message for what was never had
void AlbumListView::DrawNumberedRow(int row, int item) {
    mRowButtons[row].mVisible = TRUE;
    if (item == mSelected) {
        mTouchedRow = row;
        SpriteButton_SetSelected(&mRowButtons[row], TRUE);
    } else {
        SpriteButton_SetSelected(&mRowButtons[row], FALSE);
    }
    ClearCanvas(mRowTexts[row], 0);
    mRowTexts[row].mColor = 8;
    if ((mZeroUnknown == TRUE && mRowItemIds[row] == 0) || mItems[item]->mOwned == TRUE) {
        mRowTexts[row].mMessage = mRowItemIds[row];
    } else {
        mRowTexts[row].mMessage = mUnknownMessage;
    }
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
        mRowCounts2[row].Transfer();
        break;
    }
    }
    wchar_t number[3];
    FormatNumber(item + 1, number, sizeof(number), 2, TRUE);
    mNumbers[row].mText = (const NNSG2dChar *)number;
    ClearCanvas(mNumbers[row], 0);
    mNumbers[row].DrawText();
    mNumbers[row].Transfer();
}

// Draws the rows that changed: with their item, or empty
void AlbumListView::DrawDirtyRows() {
    for (int i = 0; i < mRowCount; i++) {
        if (mRowDirty[i]) {
            if (mRowItemIds[i] != -1) {
                DrawNumberedRow(i, GetRowItem(i));
            } else {
                ClearRow(i);
            }
            mRowDirty[i] = FALSE;
        }
    }
}

// Shows a list of items from its top
void AlbumListView::SetItems(Item **items, int count) {
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
        ScrollBar_SetPosition(&mScrollBar, 0, (mItemCount - mVisibleRows) * 20, 0);
    }
    UpdateRows();
    DrawDirtyRows();
    PlaceRows();
}

// The rows' numbers, right-aligned
void AlbumListView::InitNumbers(int *charOffset) {
    for (int i = 0; i < mRowCount; i++) {
        mNumbers[i].mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
        mNumbers[i].mTextX = 0;
        mNumbers[i].InitText(&gFonts[0].mFont, 0x1000, 4, 2, *charOffset, 0, 1, 0, 0, 15, mNumberBuffers[i], 2,
                             NNS_G2D_CHARA_COLORMODE_16);
        mNumbers[i].mColor = 8;
        ClearCanvas(mNumbers[i], 0);
        mNumbers[i].SetPositionFromVec(0, 0);
        mNumbers[i].mSprite.mOverwritePalette = 3;
        *charOffset += mNumbers[i].GetCharCount();
    }
}

void AlbumListView::DrawList() {
    SpriteButton_Draw(&mUpButton);
    SpriteButton_Draw(&mDownButton);
    Sprite_Draw(&mScrollBar);
    for (int i = 0; i < mRowCount; i++) {
        mRowTexts[i].Draw();
        mNumbers[i].Draw();
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
        SpriteButton_Draw(&mRowButtons[i]);
    }
}

// Puts each row where its item is at the scroll position, and scrolls the background with them
void AlbumListView::PlaceRows() {
    for (int i = 0; i < mRowCount; i++) {
        ScreenPoint row = AddPoints(mPosition, MakePoint(0, GetRowItem(i) * 20 - mScroll));
        SetSpritePosition(&mRowButtons[i], row);
        SetCanvasPosition(&mRowTexts[i], AddPoints(row, sTextOffset));
        SetCanvasPosition(&mRowPrices[i], AddPoints(row, sPriceOffset));
        SetSpritePosition(&mRowPriceIcons[i], AddPoints(row, sPriceIconOffset));
        SetCanvasPosition(&mRowCounts2[i], AddPoints(row, sCountOffset));
        SetCanvasPosition(&mNumbers[i], AddPoints(row, sNumberOffset));
    }
    ScreenLoader_SetScrollY(&gScreenLoader, mBgPlane, mScroll % 20 - mBgOffset);
}

void AlbumListView::SetNumbersVisible(BOOL visible) {
    for (int i = 0; i < mRowCount; i++) {
        mNumbers[i].mSprite.mVisible = visible;
    }
}

// --- What is shown ---------------------------------------------------------------------------------------------

void AlbumItems::Set(int kind, int id) {
    mKind = kind;
    switch (kind) {
    case 0:
        InventoryItem::mId = id;
        mValue = id;
        break;
    case 1:
        InventoryItem::mId = id;
        mValue = 0xF7;
        break;
    case 2:
        InventoryItem::mId = id;
        mValue = 0xF9;
        break;
    case 3:
        InventoryItem::mId = id;
        mValue = 0xF4;
        break;
    case 4:
        InventoryItem::mId = id;
        mValue = 0xF5;
        break;
    }
}

// --- The tabs --------------------------------------------------------------------------------------------------

void AlbumTabs::Init(int plane, MessageFile *file, int *charOffset) {
    mPressed = 0;
    mSelected = 0;
    for (int i = 0; i < 5; i++) {
        Sprite_Init(&mButtons[i], plane, sTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mButtons[i], sTabSequences[i], sTabOnSequences[i]);
        mButtons[i].SetPosition(FxVec2(sTabPositions[i].x << FX32_SHIFT, sTabPositions[i].y << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], sTabTouchSize.x, sTabTouchSize.y);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = 1;
        mButtons[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
}

void AlbumTabs::Update() {
    mPressed = 0;
    if (SpriteButton_Update(&mButtons[0])) {
        mPressed = TAB_GOODS;
        SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
        mSelected = 0;
    } else if (SpriteButton_Update(&mButtons[3])) {
        mPressed = TAB_GOODS_2;
        SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
        mSelected = 3;
    } else if (SpriteButton_Update(&mButtons[1])) {
        mPressed = TAB_COSTUMES;
        SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
        mSelected = 1;
    } else if (SpriteButton_Update(&mButtons[2])) {
        mPressed = TAB_INTERIORS;
        SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
        mSelected = 2;
    } else if (SpriteButton_Update(&mButtons[4])) {
        mPressed = TAB_GOODS_3;
        SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
        mSelected = 4;
    }
}

void AlbumTabs::Draw() {
    for (int i = 0; i < 5; i++) {
        SpriteButton_Draw(&mButtons[i]);
    }
}

void AlbumTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < 5; i++) {
        mButtons[i].mVisible = visible;
    }
}

void AlbumTabs::Select(int tab) {
    switch (tab) {
    case TAB_GOODS:
        mSelected = 0;
        SpriteButton_SetSelected(&mButtons[0], TRUE);
        SpriteButton_SetSelected(&mButtons[3], FALSE);
        SpriteButton_SetSelected(&mButtons[1], FALSE);
        SpriteButton_SetSelected(&mButtons[2], FALSE);
        SpriteButton_SetSelected(&mButtons[4], FALSE);
        break;
    case TAB_GOODS_2:
        mSelected = 3;
        SpriteButton_SetSelected(&mButtons[0], FALSE);
        SpriteButton_SetSelected(&mButtons[3], TRUE);
        SpriteButton_SetSelected(&mButtons[1], FALSE);
        SpriteButton_SetSelected(&mButtons[2], FALSE);
        SpriteButton_SetSelected(&mButtons[4], FALSE);
        break;
    case TAB_COSTUMES:
        mSelected = 1;
        SpriteButton_SetSelected(&mButtons[0], FALSE);
        SpriteButton_SetSelected(&mButtons[3], FALSE);
        SpriteButton_SetSelected(&mButtons[1], TRUE);
        SpriteButton_SetSelected(&mButtons[2], FALSE);
        SpriteButton_SetSelected(&mButtons[4], FALSE);
        break;
    case TAB_INTERIORS:
        mSelected = 2;
        SpriteButton_SetSelected(&mButtons[0], FALSE);
        SpriteButton_SetSelected(&mButtons[3], FALSE);
        SpriteButton_SetSelected(&mButtons[1], FALSE);
        SpriteButton_SetSelected(&mButtons[2], TRUE);
        SpriteButton_SetSelected(&mButtons[4], FALSE);
        break;
    case TAB_GOODS_3:
        mSelected = 4;
        SpriteButton_SetSelected(&mButtons[0], FALSE);
        SpriteButton_SetSelected(&mButtons[3], FALSE);
        SpriteButton_SetSelected(&mButtons[1], FALSE);
        SpriteButton_SetSelected(&mButtons[2], FALSE);
        SpriteButton_SetSelected(&mButtons[4], TRUE);
        break;
    }
}

void AlbumTabs::SetEnabled(int tabs) {
    for (int i = 0; i < 5; i++) {
        SpriteButton_SetDisabled(&mButtons[i], TRUE);
    }
    if (tabs & TAB_GOODS) {
        SpriteButton_SetDisabled(&mButtons[0], FALSE);
    }
    if (tabs & TAB_GOODS_2) {
        SpriteButton_SetDisabled(&mButtons[3], FALSE);
    }
    if (tabs & TAB_COSTUMES) {
        SpriteButton_SetDisabled(&mButtons[1], FALSE);
    }
    if (tabs & TAB_INTERIORS) {
        SpriteButton_SetDisabled(&mButtons[2], FALSE);
    }
    if (tabs & TAB_GOODS_3) {
        SpriteButton_SetDisabled(&mButtons[4], FALSE);
    }
}

// Empty; called once, by AlbumList::Open, after the tabs are set up
void AlbumTabs::OnOpen() {
}

// --- The goods filter ------------------------------------------------------------------------------------------

void AlbumGoodsFilter::Init(int plane) {
    for (int i = 0; i < 4; i++) {
        Sprite_Init(&mButtons[i], plane, sGoodsFilterSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mButtons[i], sGoodsFilterSequences[i], sGoodsFilterOnSequences[i]);
        mButtons[i].SetPosition(
            FxVec2(sGoodsFilterPositions[i].x << FX32_SHIFT, sGoodsFilterPositions[i].y << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], sGoodsFilterTouchSize.x, sGoodsFilterTouchSize.y);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = 1;
        mButtons[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void AlbumGoodsFilter::Update() {
    mPressed = -1;
    for (int i = 0; i < 4; i++) {
        if (SpriteButton_Update(&mButtons[i])) {
            SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void AlbumGoodsFilter::Draw() {
    for (int i = 0; i < 4; i++) {
        SpriteButton_Draw(&mButtons[i]);
    }
}

void AlbumGoodsFilter::SetVisible(BOOL visible) {
    for (int i = 0; i < 4; i++) {
        mButtons[i].mVisible = visible;
    }
}

void AlbumGoodsFilter::Select(int button) {
    for (int i = 0; i < 4; i++) {
        SpriteButton_SetSelected(&mButtons[i], FALSE);
    }
    SpriteButton_SetSelected(&mButtons[button], TRUE);
    mSelected = button;
}

int AlbumGoodsFilter::GetFilter() {
    return sGoodsFilters[mSelected];
}

// --- The costume filter ----------------------------------------------------------------------------------------

void AlbumCostumeFilter::Init(int plane) {
    for (int i = 0; i < 5; i++) {
        Sprite_Init(&mButtons[i], plane, sCostumeFilterSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mButtons[i], sCostumeFilterSequences[i], sCostumeFilterOnSequences[i]);
        mButtons[i].SetPosition(
            FxVec2(sCostumeFilterPositions[i].x << FX32_SHIFT, sCostumeFilterPositions[i].y << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], sCostumeFilterTouchSize.x, sCostumeFilterTouchSize.y);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 3);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = 1;
        mButtons[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void AlbumCostumeFilter::Update() {
    mPressed = -1;
    for (int i = 0; i < 5; i++) {
        if (SpriteButton_Update(&mButtons[i])) {
            SpriteButton_SetSelected(&mButtons[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void AlbumCostumeFilter::Draw() {
    for (int i = 0; i < 5; i++) {
        SpriteButton_Draw(&mButtons[i]);
    }
}

void AlbumCostumeFilter::SetVisible(BOOL visible) {
    for (int i = 0; i < 5; i++) {
        mButtons[i].mVisible = visible;
    }
}

void AlbumCostumeFilter::Select(int button) {
    for (int i = 0; i < 5; i++) {
        SpriteButton_SetSelected(&mButtons[i], FALSE);
    }
    SpriteButton_SetSelected(&mButtons[button], TRUE);
    mSelected = button;
}

int AlbumCostumeFilter::GetFilter() {
    return sCostumeFilters[mSelected];
}

// --- The item model --------------------------------------------------------------------------------------------

void AlbumItemModel::Load(int resourceId) {
    LoadModel(resourceId);
    mUnknown12C = 0;
}

// Hides the model, far away
void AlbumItemModel::Reset() {
    mVisible = FALSE;
    mPosition.x = 0x7FFFFFFF;
    mPosition.y = 0x7FFFFFFF;
    mPosition.z = 0x7FFFFFFF;
    mScale.x = FX32_ONE;
    mScale.y = FX32_ONE;
    mScale.z = FX32_ONE;
    mUnknown12C = 0;
}

// Shows other goods: lets go of the old ones' files, and loads the new ones'
void AlbumItemModel::ChangeItem(int item) {
    ModelResources_ReleaseModel(gModelResources, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(gModelResources, mResourceId, mModelPath);
    LoadAnimation(mResourceId);
    ResetScale();
    SetScale(0x2800);
}

// --- The list --------------------------------------------------------------------------------------------------

void AlbumList::Init(AlbumContext *context, int *charOffset) {
    mContext = context;
    mCostumes = context->mCostumes;
    mInventory = context->mInventory;
    mInteriors = context->mInteriors;
    mMannequin = context->mMannequin;
    mItemModel = mContext->mItemModel;
    mRoom = context->mRoom;
    mSelection.Set(0, 0);

    mList.Init(6);
    mList.SetBackground(0x100, 0x3F);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x5FDF, 0x100);
    mList.InitRows(1, 8, 9, charOffset, sRowsPosition);
    mList.InitCounts(charOffset);
    mList.InitNumbers(charOffset);
    mList.InitUpButton(1, 2, 3, sUpButtonPosition, sUpButtonSize);
    mList.InitDownButton(1, 4, 5, sDownButtonPosition, sDownButtonSize);
    mList.InitScrollBar(1, 6, 7, 0x7A, 0x4C, 2, sScrollBarPosition, sScrollKnobRect);
    mList.SetUpScrollBar(2, 6, 7);
    mList.SetMessageFile(context->mItemMessages);
    mList.SetMode(ItemListView::MODE_NAME);
    mList.SetVisible(FALSE);
    mList.SetNumbersVisible(FALSE);

    mTabs.Init(1, context->mCommonMessages, charOffset);
    mGoodsFilter.Init(1);
    mCostumeFilter.Init(1);

    Sprite_Init(&mBackButton, 1, 0, 128, 96, TRUE);
    SpriteButton_SetSequences(&mBackButton, 0, 1);
    mBackButton.SetPosition(FxVec2(sBackPosition.x << FX32_SHIFT, sBackPosition.y << FX32_SHIFT));
    SpriteButton_SetTouchRect(&mBackButton, sBackTouchSize.x, sBackTouchSize.y);
    SpriteButton_Reset(&mBackButton);
    SpriteButton_SetKind(&mBackButton, 4);
    mBackButton.mUnknownDC = 1;
    mBackButton.mSounds.mSounds[0] = 13;
    mBackButton.mSounds.mSounds[2] = 0;
    mBackButton.mOverwritePalette = 1;
    mBackButton.mVisible = FALSE;

    mRoomAngle = 0x8000;
    mRoomTurn = 0;
    SetRoomMode(0);
    for (int i = 0; i < 3; i++) {
        mUnknown16614[i] = 0;
        mUnknown16620[i] = 0;
    }
    mUnknown1662C = -1;
    mTab = 0;
    mResetRoomFlags = FALSE;
    for (int i = 0; i < 5; i++) {
        mCounts[i] = 0;
        mOwnedCounts[i] = 0;
    }
    mSelectFirst = FALSE;
    CountGoods(context->mInventory, &mCounts[0], &mOwnedCounts[0], 1);
    CountClothes(context->mCostumes, &mCounts[1], &mOwnedCounts[1]);
    CountInteriors(context->mInteriors, &mCounts[2], &mOwnedCounts[2]);
    CountGoods(context->mInventory, &mCounts[3], &mOwnedCounts[3], 3);
    CountGoods(context->mInventory, &mCounts[4], &mOwnedCounts[4], 4);
    CountGoods(context->mInventory, &mCounts[4], &mOwnedCounts[4], 5);
    CountGoods(context->mInventory, &mCounts[4], &mOwnedCounts[4], 6);
}

// Selects the list's first thing, and has Run show it
void AlbumList::SelectFirst() {
    mList.Select(0);
    mSelectFirst = TRUE;
}

int AlbumList::Update() {
    int result = 0;
    int state = mState;
    mLastState = state;
    switch (state) {
    case STATE_IDLE:
        break;
    case STATE_OPEN:
        Open();
        break;
    case STATE_RUN:
        result = Run();
        break;
    case STATE_CLOSE_REQUEST:
        mState = STATE_CLOSE;
        break;
    case STATE_CLOSE:
        Close();
        break;
    }
    UpdatePreviews();
    return result;
}

void AlbumList::UpdatePreviews() {
    UpdateRoom();
    UpdateMannequin();
}

void AlbumList::Open() {
    mItemModel = mContext->mItemModel;
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x5FDF, 0x100);
    switch (mMode) {
    case 0:
    case 1: {
        mBackButton.mVisible = TRUE;
        mTabs.SetVisible(TRUE);
        // The second goods tab is always there; the others when the player has had anything of theirs
        int tabs = AlbumTabs::TAB_GOODS | AlbumTabs::TAB_GOODS_2;
        for (int i = 0; i < INVENTORY_SAVED; i++) {
            if (GameData_GetGoodsCount(&gGameData, i) != -1) {
                int kind = gGoodsTable[(u16)i].mKind;
                if (kind >= 4 && kind <= 6) {
                    tabs |= AlbumTabs::TAB_GOODS_3;
                    break;
                }
            }
        }
        for (int i = 0; i < 374; i++) {
            if (GameData_GetClothesCount(&gGameData, i) != -1) {
                tabs |= AlbumTabs::TAB_COSTUMES;
                break;
            }
        }
        for (int i = 0; i < INVENTORY_SAVED; i++) {
            if (GameData_GetInteriorCount(&gGameData, i) != -1) {
                tabs |= AlbumTabs::TAB_INTERIORS;
                break;
            }
        }
        mTabs.SetEnabled(tabs);
        mTabs.Select(AlbumTabs::TAB_GOODS);
        mTabs.OnOpen();
        mGoodsFilter.SetVisible(TRUE);
        mGoodsFilter.Select(0);
        mCostumeFilter.SetVisible(FALSE);
        mCostumeFilter.Select(0);
        GetGoods(mInventory, mItems, &mItemCount, 1, 1);
        mList.SetMode(ItemListView::MODE_COUNT);
        mList.SetMessages(mContext->mItemMessages, 0x1FE, FALSE);
        mList.SetVisible(TRUE);
        mList.SetNumbersVisible(TRUE);
        mList.SetItems(mItems, mItemCount);
        mList.mTouchable = TRUE;
        SelectFirst();
        mSelection.Set(0, 0);
        break;
    }
    case 2:
        mBackButton.mVisible = TRUE;
        mTabs.SetVisible(FALSE);
        mGoodsFilter.SetVisible(FALSE);
        mCostumeFilter.SetVisible(TRUE);
        mCostumeFilter.Select(0);
        mList.SetMode(ItemListView::MODE_NAME);
        mList.SetMessages(mContext->mCostumeMessages, 600, TRUE);
        mList.SetVisible(TRUE);
        mList.SetNumbersVisible(TRUE);
        mList.ItemListView::SetItems(mItems, mItemCount);
        mList.mTouchable = TRUE;
        mSelection.Set(1, 0);
        break;
    }
    mState = STATE_RUN;
}

int AlbumList::Run() {
    int result = 0;
    mTabs.Update();
    if (mTabs.mPressed != 0) {
        switch (mTabs.mPressed) {
        case AlbumTabs::TAB_GOODS:
            mGoodsFilter.SetVisible(TRUE);
            mGoodsFilter.Select(0);
            mCostumeFilter.SetVisible(FALSE);
            GetGoods(mInventory, mItems, &mItemCount, 1, 1);
            mList.SetMessages(mContext->mItemMessages, 0x1FE, FALSE);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(0, 0);
            mTab = 0;
            break;
        case AlbumTabs::TAB_GOODS_2:
            mGoodsFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            GetGoodsKind3(mInventory, mItems, &mItemCount);
            mList.SetMessages(mContext->mItemMessages, 0x1FE, FALSE);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(0, 0);
            mTab = 3;
            break;
        case AlbumTabs::TAB_COSTUMES:
            mGoodsFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(TRUE);
            mCostumeFilter.Select(0);
            GetClothes(mCostumes, mItems, &mItemCount, 0);
            mList.SetMessages(mContext->mCostumeMessages, 600, TRUE);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(1, 0);
            mTab = 1;
            break;
        case AlbumTabs::TAB_INTERIORS:
            mGoodsFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            GetInteriors(mInteriors, mItems, &mItemCount);
            mList.SetMessages(mContext->mRoomMessages, 100, FALSE);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(2, 0);
            mTab = 2;
            break;
        case AlbumTabs::TAB_GOODS_3:
            mGoodsFilter.SetVisible(FALSE);
            mCostumeFilter.SetVisible(FALSE);
            GetGoodsKinds4To6(mInventory, mItems, &mItemCount);
            mList.SetMessages(mContext->mItemMessages, 0x1FE, FALSE);
            mList.SetItems(mItems, mItemCount);
            mSelection.Set(0, 0);
            mTab = 4;
            break;
        }
        SelectFirst();
    }

    mGoodsFilter.Update();
    if (mGoodsFilter.mPressed != -1) {
        GetGoods(mInventory, mItems, &mItemCount, 1, mGoodsFilter.GetFilter());
        mList.SetItems(mItems, mItemCount);
        SelectFirst();
    }

    mCostumeFilter.Update();
    if (mCostumeFilter.mPressed != -1) {
        switch (mMode) {
        case 0:
        case 1:
            GetClothes(mCostumes, mItems, &mItemCount, mCostumeFilter.GetFilter());
            mList.SetItems(mItems, mItemCount);
            break;
        case 2:
            mList.SetItems(mItems, mItemCount);
            break;
        }
        SelectFirst();
    }

    mList.Update();
    if (mList.mTouched || mSelectFirst == TRUE) {
        mSelectFirst = FALSE;
        if (mList.mSelected != -1) {
            int id = mList.mItems[mList.mSelected]->mId;
            mChosen = TRUE;
            ShowCounts(-1);
            switch (mSelection.mKind) {
            case PREVIEW_ITEM:
                mItemModel->ChangeItem(id);
                mItemModel->mVisible = TRUE;
                mContext->mSprite1->mVisible = FALSE;
                mContext->mSprite2->mVisible = FALSE;
                if (mList.mItems[mList.mSelected]->mOwned != TRUE) {
                    mContext->mExplainText->mSprite.mVisible = FALSE;
                    ShowUnknownName();
                    mContext->mPriceText->mSprite.mVisible = FALSE;
                    Model_SetDiffuse(mItemModel, 0);
                    Model_SetAmbient(mItemModel, 0);
                    Model_SetSpecular(mItemModel, 0);
                    Model_SetEmission(mItemModel, 0);
                } else {
                    ShowExplanation(mSelection.mKind, id);
                    ShowName(mSelection.mKind, id);
                    ShowPrice(mSelection.mKind, id);
                    Model_SetDiffuse(mItemModel, GX_RGB(31, 31, 31));
                    Model_SetAmbient(mItemModel, GX_RGB(31, 31, 31));
                    Model_SetSpecular(mItemModel, GX_RGB(31, 31, 31));
                    Model_SetEmission(mItemModel, GX_RGB(31, 31, 31));
                }
                mMannequin->SetVisible(FALSE);
                mRoom->RoomModels::SetVisible(FALSE);
                mSelection.Set(0, id);
                ResetLights();
                StopRoom();
                break;
            case PREVIEW_COSTUME:
                ShowCostume(id);
                LoadMannequinHead();
                mMannequin->LoadHead(1, 0);
                mContext->mSprite1->mVisible = TRUE;
                mContext->mSprite2->mVisible = FALSE;
                if (mList.mItems[mList.mSelected]->mOwned != TRUE && mList.mItems[mList.mSelected]->mId != 0) {
                    mContext->mExplainText->mSprite.mVisible = FALSE;
                    ShowUnknownName();
                    mContext->mPriceText->mSprite.mVisible = FALSE;
                    Model_SetDiffuse(&mMannequin->mAnim, 0);
                    Model_SetAmbient(&mMannequin->mAnim, 0);
                    Model_SetSpecular(&mMannequin->mAnim, 0);
                    Model_SetEmission(&mMannequin->mAnim, 0);
                    Model_SetDiffuse(&mMannequin->mHead, 0);
                    Model_SetAmbient(&mMannequin->mHead, 0);
                    Model_SetSpecular(&mMannequin->mHead, 0);
                    Model_SetEmission(&mMannequin->mHead, 0);
                } else {
                    ShowExplanation(mSelection.mKind, id);
                    ShowName(mSelection.mKind, id);
                    ShowPrice(mSelection.mKind, id);
                }
                mMannequin->SetVisible(TRUE);
                mItemModel->mVisible = FALSE;
                mRoom->RoomModels::SetVisible(FALSE);
                mSelection.Set(1, id);
                ResetLights();
                StopRoom();
                break;
            case PREVIEW_ROOM:
                mItemModel->mVisible = FALSE;
                mMannequin->SetVisible(FALSE);
                mRoom->RoomModels::SetVisible(FALSE);
                mContext->mSprite1->mVisible = FALSE;
                mContext->mSprite2->mVisible = TRUE;
                ShowRoom(id);
                if (mList.mItems[mList.mSelected]->mOwned != TRUE) {
                    mContext->mExplainText->mSprite.mVisible = FALSE;
                    ShowUnknownName();
                    mContext->mPriceText->mSprite.mVisible = FALSE;
                    mResetRoomFlags = TRUE;
                } else {
                    ShowExplanation(mSelection.mKind, id);
                    ShowName(mSelection.mKind, id);
                    ShowPrice(mSelection.mKind, id);
                }
                mSelection.Set(2, id);
                break;
            }
        }
    }

    if (SpriteButton_Update(&mBackButton)) {
        result = 1;
    }
    return result;
}

void AlbumList::Close() {
    mItemModel->Reset();
    mItemModel = NULL;
    mMannequin->SetVisible(FALSE);
    mRoom->RoomModels::SetVisible(FALSE);
    mBackButton.mVisible = FALSE;
    mTabs.SetVisible(FALSE);
    mGoodsFilter.SetVisible(FALSE);
    mCostumeFilter.SetVisible(FALSE);
    mList.SetVisible(FALSE);
    mList.SetNumbersVisible(FALSE);
    mState = STATE_IDLE;
}

void AlbumList::Draw() {
    mList.DrawList();
    SpriteButton_Draw(&mBackButton);
    mTabs.Draw();
    mGoodsFilter.Draw();
    mCostumeFilter.Draw();
}

// Dresses the mannequin in a costume
void AlbumList::ShowCostume(int costume) {
    ModelResources_ReleaseModel(gModelResources, 0);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    mMannequin->SetArchive("3MM", costume, MiiData_GetSex(mContext->mPlayer));
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin->mArchivePath, 1);
    ModelResources_SetModel(gModelResources, 0, mMannequin->mModelPath);
    mMannequin->MakeModel(0, 0);
    mMannequin->SetPaletteColors(NFL_GetFavoriteColor(0), 0x5FBE);
    mMannequin->SetAnimFlags(TRUE, TRUE);
}

void AlbumList::LoadMannequinHead() {
    ModelResources_ReleaseModel(gModelResources, 1);
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ArchiveManager_MountCompressed(&gArchiveManager, "3MH", "Mdl/Misc/headManikin_3D_LZ.bin", 1);
    ModelResources_SetModel(gModelResources, 1, "3MH:/head.imd");
}

// The explanation of a thing, from the explanation messages: the goods', then the clothes', then the interiors'
void AlbumList::ShowExplanation(int kind, int id) {
    mContext->mExplainText->mSprite.mVisible = TRUE;
    switch (kind) {
    case 0:
        mContext->mExplainText->mMessage = id;
        break;
    case 1:
        mContext->mExplainText->mMessage = id + 0xFF;
        break;
    case 2:
        mContext->mExplainText->mMessage = id + 0x22B;
        break;
    default:
        return;
    }
    ClearCanvas(*mContext->mExplainText, 0);
    mContext->mExplainText->DrawAll();
}

// The name of a thing, from its kind's messages
void AlbumList::ShowName(int kind, int id) {
    mContext->mNameText->mSprite.mVisible = TRUE;
    switch (kind) {
    case 0:
        mContext->mNameText->mMessageFile = mContext->mItemMessages;
        mContext->mNameText->mMessage = id;
        break;
    case 1:
        mContext->mNameText->mMessageFile = mContext->mCostumeMessages;
        mContext->mNameText->mMessage = id;
        break;
    case 2:
        mContext->mNameText->mMessageFile = mContext->mRoomMessages;
        mContext->mNameText->mMessage = id;
        break;
    default:
        return;
    }
    ClearCanvas(*mContext->mNameText, 0);
    mContext->mNameText->DrawAll();
}

// The name of what was never had
void AlbumList::ShowUnknownName() {
    mContext->mNameText->mSprite.mVisible = TRUE;
    mContext->mNameText->mMessageFile = mContext->mItemMessages;
    mContext->mNameText->mMessage = 0x1FE;
    ClearCanvas(*mContext->mNameText, 0);
    mContext->mNameText->DrawAll();
}

// The price of a thing: none for goods of kinds 3 and 6 and interiors of kind 0, a message of its own for 0
void AlbumList::ShowPrice(int kind, int id) {
    wchar_t text[16];
    wchar_t number[16];
    MI_CpuClear16(text, sizeof(text));
    int price;
    switch (kind) {
    case 0: {
        const ItemInfo *info = &gGoodsTable[id];
        if (info->mKind == 3 || info->mKind == 6) {
            price = -1;
        } else {
            price = info->mPrice;
        }
        break;
    }
    case 1:
        price = gClothesTable[id].mPrice;
        break;
    case 2:
        switch (gInteriorTable[id].mKind) {
        case 0:
            price = -1;
            break;
        case 13:
            price = 0;
            break;
        default:
            price = gInteriorTable[id].mPrice;
            break;
        }
        break;
    default:
        return;
    }
    if (price < 0) {
        mContext->mPriceText->mSprite.mVisible = FALSE;
        return;
    }
    mContext->mPriceText->mSprite.mVisible = TRUE;
    mContext->mPriceText->mMessageFile = mContext->mEtcMessages;
    if (price == 0) {
        mContext->mPriceText->mMessage = 0x149;
    } else {
        FormatNumber(price, number, sizeof(number), 0, FALSE);
        wcsncpy(text, number, 16);
        mContext->mPriceText->SetStringArg(text, 0);
        mContext->mPriceText->mMessage = 0x14A;
    }
    ClearCanvas(*mContext->mPriceText, 0);
    mContext->mPriceText->DrawAll();
}

// How many things a tab has, and how many of them the player has had; -1 for the tab shown
void AlbumList::ShowCounts(int tab) {
    wchar_t number[16];
    wchar_t text[16];
    if (tab < 0) {
        tab = mTab;
    }
    mContext->mCountText->mMessageFile = mContext->mEtcMessages;
    mContext->mCountText->mMessage = 0x14B;
    FormatNumber(mCounts[tab], number, sizeof(number), 0, FALSE);
    wcsncpy(text, number, 16);
    mContext->mCountText->SetStringArg(text, 0);
    ClearCanvas(*mContext->mCountText, 0);
    mContext->mCountText->DrawAll();
    mContext->mOwnedText->mMessageFile = mContext->mEtcMessages;
    mContext->mOwnedText->mMessage = 0x14B;
    FormatNumber(mOwnedCounts[tab], number, sizeof(number), 0, FALSE);
    wcsncpy(text, number, 16);
    mContext->mOwnedText->SetStringArg(text, 0);
    ClearCanvas(*mContext->mOwnedText, 0);
    mContext->mOwnedText->DrawAll();
}

void AlbumList::SetRoomMode(int mode) {
    mRoomMode = mode;
    mRoomStep = 0;
}

void AlbumList::ShowRoom(u16 style) {
    mRoomStyle = style;
    SetRoomMode(1);
}

void AlbumList::StopRoom() {
    SetRoomMode(2);
}

void AlbumList::ResetLights() {
    data_021226e4.mLights[0].mColor = sLightColors[0];
    data_021226e4.mLights[1].mColor = sLightColors[1];
}

// Turns the room with the L and R buttons; loads another interior
void AlbumList::UpdateRoom() {
    switch (mRoomMode) {
    case 0:
        if (mRoom->mVisible) {
            if ((gPadInput.mHeld & PAD_BUTTON_R) && (gPadInput.mHeld & PAD_BUTTON_L)) {
                mRoomTurn = 0;
            } else if (gPadInput.mHeld & PAD_BUTTON_R) {
                mRoomTurn = 0x6D;
            } else if (gPadInput.mHeld & PAD_BUTTON_L) {
                mRoomTurn = -0x6D;
            } else {
                mRoomTurn = 0;
            }
            mRoomAngle += mRoomTurn;
            mRoom->mDoor.mModel.mRotX = 0xE38;
            mRoom->mDoor.mModel.mRotY = mRoomAngle;
        }
        break;
    case 1:
        switch (mRoomStep) {
        case 0:
            mRoomStep++;
            mRoom->RoomModels::SetVisible(FALSE);
            mRoom->ReleaseAll();
            break;
        case 1:
            mRoomStep++;
            mRoom->mId = mRoomStyle;
            mRoom->MountModel(0);
            mRoom->MountPartModels(0);
            break;
        case 2:
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mRoomStep++;
            }
            break;
        case 3:
            mRoomStep++;
            mRoom->Reload(11);
            break;
        case 4:
            mRoomStep++;
            mRoom->ReloadParts(12);
            break;
        case 5:
            SetRoomMode(0);
            mRoom->RoomModels::SetVisible(TRUE);
            if (mResetRoomFlags == TRUE) {
                mRoom->RoomModels::SetFlags(0);
            }
            mResetRoomFlags = FALSE;
            data_021226e4.mLights[0].mColor = gInteriorTable[mRoom->mId].mLightColor;
            data_021226e4.mLights[1].mColor = gInteriorTable[mRoom->mId].mLightColor;
            break;
        }
        break;
    case 2:
        SetRoomMode(0);
        mRoom->RoomModels::SetVisible(FALSE);
        break;
    }
}

// Turns the mannequin with the L and R buttons
void AlbumList::UpdateMannequin() {
    if (mMannequin->mHead.mVisible) {
        u16 turn;
        if ((gPadInput.mHeld & PAD_BUTTON_R) && (gPadInput.mHeld & PAD_BUTTON_L)) {
            turn = 0;
        } else if (gPadInput.mHeld & PAD_BUTTON_R) {
            turn = 0x200;
        } else if (gPadInput.mHeld & PAD_BUTTON_L) {
            turn = -0x200;
        } else {
            turn = 0;
        }
        mMannequin->mAnim.mRotY += turn;
    }
}

// --- The scene -------------------------------------------------------------------------------------------------

void AlbumScene::StartLoading() {
    gSoundManager.StopSeq(0, 0);
    gSoundManager.StartSeqIfNotPlaying(0, 3);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TIM", "Msg/TmdItemMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TDM", "Msg/TmdCostumeMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TRM", "Msg/TmdRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TXM", "Msg/TmdExplainMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SAB", "Scn/Scene/SceneAlbum_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "LTL", "Scn/List/List_AlbumList_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MH", "Mdl/Misc/headManikin_3D_LZ.bin", 0);
    mMannequin.SetArchive("3MM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin.mArchivePath, 0);

    GameData_ReadCharacter(&gGameData, 0, &mPlayer);
    mPlayer.mPersonality = mPlayer.GetPersonality();
    switch (gPersonalityKinds[mPlayer.mPersonality]) {
    case 0:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_cool_LZ.bin", 0);
        break;
    case 1:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_dry_LZ.bin", 0);
        break;
    case 2:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_nagomi_LZ.bin", 0);
        break;
    case 3:
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_nori_LZ.bin", 0);
        break;
    }

    char archiveName[] = "IM0";
    mItemModel.SetItem(archiveName, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mItemModel.mArchiveName, mItemModel.mArchivePath, 0);
    mRoom.mId = 0;
    mRoom.Mount("CO0", 0);
    mRoom.MountAll("RM?", 0);
}

// Takes a text's characters off the offset of the next
static inline void AddCharCount(int *charOffset, MessageText *text) {
    *charOffset += text->GetCharCount();
}

void AlbumScene::Init() {
    mMessages[0].Open("TCM:tmd_common_msg_JP.bmg");
    mMessages[1].Open("TTM:tmd_trouble_msg_JP.bmg");
    mMessages[2].Open("TIM:tmd_item_msg_JP.bmg");
    mMessages[3].Open("TDM:tmd_costume_msg_JP.bmg");
    mMessages[4].Open("TRM:tmd_room_msg_JP.bmg");
    mMessages[5].Open("TEM:tmd_etc_msg_JP.bmg");
    mMessages[6].Open("TXM:tmd_explain_msg_JP.bmg");

    for (int i = 4; i >= 0; i--) {
        mBgChars[i] = VramTransferQueue_LoadFile(&gVramTransferQueue, sBgCharNames[i], 2, 0, 0, 0, -1);
        mBgScreens[i] = ScreenLoader_LoadFile(&gScreenLoader, sBgScreenNames[i], 2, 0, 0, 0, 0, -1);
        mBgPalettes[i] = PaletteLoader_LoadFile(&gPaletteLoader, sBgPaletteNames[i], 2, -1, -1, -1);
    }
    PaletteLoader_LoadFile(&gPaletteLoader, "LTL:/BG_list.ncl", 0x2F00, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "LTL:/BG_list.ncg", 0x2F00, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_Album_frame.nsc", 0x200, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 2);
    ScreenLoader_SetScrollX(&gScreenLoader, 0x2F00, 0);
    ScreenLoader_SetScrollY(&gScreenLoader, 0x2F00, 0);
    ScreenLoader_SetScrollY(&gScreenLoader, 0x100, -0x3F);
    gScreenControl.Set(0x1313);
    PaletteLoader_LoadFile(&gPaletteLoader, "LTL:/Obj_list.ncl", 0x1000, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "LTL:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "LTL:/Obj_TushinEtc_list.nce", 0x1000, 1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SAB:/Obj_up.ncl", 0x10, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SAB:/Obj_up.ncg", 0x10, 0, 0, 0, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SAB:/Obj_up.nce", 0x10, 0);
    Init3d();

    mContext.mCostumes = &mCostumes;
    mContext.mInventory = &mInventory;
    mContext.mInteriors = &mInteriors;
    mContext.mCommonMessages = &mMessages[0];
    mContext.mTroubleMessages = &mMessages[1];
    mContext.mItemMessages = &mMessages[2];
    mContext.mCostumeMessages = &mMessages[3];
    mContext.mRoomMessages = &mMessages[4];
    mContext.mEtcMessages = &mMessages[5];
    mContext.mMannequin = &mMannequin;
    mContext.mPlayer = &mPlayer;
    mContext.mItemModel = &mItemModel;
    mContext.mExplainText = &mExplainText;
    mContext.mNameText = &mNameText;
    mContext.mPriceText = &mPriceText;
    mContext.mCountText = &mCountText;
    mContext.mOwnedText = &mOwnedText;
    mContext.mRoom = &mRoom;
    mContext.mSprite1 = &mSprite1;
    mContext.mSprite2 = &mSprite2;

    mTextCharOffset = 0x100;
    mListCharOffset = 0x190;
    mExplainText.InitObj(&gFonts[0].mFont, 0x10, 0x14, 4, mTextCharOffset, 0, 1, 0, 0, 15, NULL);
    mExplainText.mMessageFile = &mMessages[6];
    SetPixelPosition(&mExplainText.mSprite, sExplainTextPosition);
    mExplainText.mSprite.mOverwritePalette = 0;
    mExplainText.mHAlign = 0;
    mExplainText.mVAlign = 1;
    AddCharCount(&mTextCharOffset, &mExplainText);

    mNameText.InitObj(&gFonts[0].mFont, 0x10, 0x10, 2, mTextCharOffset, 0, 1, 0, 0, 15, NULL);
    mNameText.mMessageFile = &mMessages[2];
    SetPixelPosition(&mNameText.mSprite, sNameTextPosition);
    mNameText.mSprite.mOverwritePalette = 0;
    mNameText.mHAlign = 1;
    mNameText.mVAlign = 1;
    AddCharCount(&mTextCharOffset, &mNameText);

    mPriceText.InitObj(&gFonts[0].mFont, 0x10, 10, 2, mTextCharOffset, 0, 1, 0, 0, 15, NULL);
    mPriceText.mMessageFile = &mMessages[5];
    SetPixelPosition(&mPriceText.mSprite, sPriceTextPosition);
    mPriceText.mSprite.mOverwritePalette = 0;
    mPriceText.mHAlign = 2;
    mPriceText.mVAlign = 1;
    AddCharCount(&mTextCharOffset, &mPriceText);

    mCountText.InitObj(&gFonts[0].mFont, 0x10, 4, 2, mTextCharOffset, 0, 1, 0, 0, 15, NULL);
    mCountText.mMessageFile = &mMessages[5];
    SetPixelPosition(&mCountText.mSprite, sCountTextPosition);
    mCountText.mSprite.mOverwritePalette = 0;
    mCountText.mHAlign = 1;
    mCountText.mVAlign = 1;
    AddCharCount(&mTextCharOffset, &mCountText);

    mOwnedText.InitObj(&gFonts[0].mFont, 0x10, 4, 2, mTextCharOffset, 0, 1, 0, 0, 15, NULL);
    mOwnedText.mMessageFile = &mMessages[5];
    SetPixelPosition(&mOwnedText.mSprite, sOwnedTextPosition);
    mOwnedText.mSprite.mOverwritePalette = 0;
    mOwnedText.mHAlign = 1;
    mOwnedText.mVAlign = 1;
    AddCharCount(&mTextCharOffset, &mOwnedText);

    Sprite_Init(&mSprite1, 0, 0, 128, 96, TRUE);
    mSprite1.mUnknown5E = 0;
    mSprite1.mVisible = FALSE;
    Sprite_Init(&mSprite2, 0, 1, 128, 96, TRUE);
    mSprite2.mUnknown5E = 0;
    mSprite2.mVisible = FALSE;

    mCostumes.Load();
    mCostumes.Sort();
    mInventory.Load();
    mInventory.Sort();
    mInteriors.Load();
    mInteriors.Sort();
    mList.Init(&mContext, &mListCharOffset);
    mList.mMode = 0;
    mList.mState = AlbumList::STATE_OPEN;

    mRoom.Load(11);
    mRoom.LoadParts(12);
    mRoom.RoomModels::SetVisible(TRUE);
    VecFx32 roomPosition = sRoomPosition;
    mRoom.mDoor.mPosition.x = roomPosition.x;
    mRoom.mDoor.mPosition.y = roomPosition.y;
    mRoom.mDoor.mPosition.z = roomPosition.z;
    mRoom.SetScale(0x385);
    mRoom.mDoor.mModel.mRotX = 0xE38;
    mRoom.mDoor.mModel.mRotY = 0;
    mRoom.RoomModels::SetVisible(FALSE);
    mUnknown4C = 0;
    mBgShown = 0;
}

void AlbumScene::Init3d() {
    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = 1;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 1);

    mCamera = &gCamera;
    Camera_Reset((Camera *)mCamera);
    VEC_Set(&((Camera *)mCamera)->mPosition, sCameraPosition.x, sCameraPosition.y, sCameraPosition.z);
    VEC_Set(&((Camera *)mCamera)->mTarget, sCameraTarget.x, sCameraTarget.y, sCameraTarget.z);
    gLights[0].mColor = sInitLightColors[1];
    Light_SetDirectionByCamera(gLights, (Camera *)mCamera, sLight0Direction);
    gLight1[0].mColor = sInitLightColors[0];
    Light_SetDirectionByCamera(gLight1, (Camera *)mCamera, sLight1Direction);

    ModelResources_SetModel(gModelResources, 0, mMannequin.mModelPath);
    ModelResources_SetModel(gModelResources, 1, "3MH:/head.imd");
    mMannequin.Init(0, 0, 1, 0);
    void *animations = ModelResources_BindArchive(gModelResources, 0, "3HA", 0);
    ResourceSet_SetArchive((void *)mMannequin.mAnim.mUnknown04, RESOURCE_BCA0, animations);
    SetMannequinAnimation(&mMannequin, "idle00");
    mMannequin.SetAnimFlags(TRUE, TRUE);
    mMannequin.mShadowVisible = FALSE;
    mMannequin.SetVisible(FALSE);

    VecFx32 position = sZero;
    mMannequin.mPos.x = position.x;
    mMannequin.mPos.y = position.y;
    mMannequin.mPos.z = position.z;
    mMannequin.mScale.x = gMannequinBodyScale.x * 8 / 10;
    mMannequin.mInvScale.x = FX_Inv(mMannequin.mScale.x);
    mMannequin.mScale.y = gMannequinBodyScale.y * 8 / 10;
    mMannequin.mInvScale.y = FX_Inv(mMannequin.mScale.y);
    mMannequin.mScale.z = gMannequinBodyScale.z * 8 / 10;
    mMannequin.mInvScale.z = FX_Inv(mMannequin.mScale.z);
    mMannequin.mHeadScale.x = gMannequinHeadScale.x * 8 / 10;
    mMannequin.mHeadScale.y = gMannequinHeadScale.y * 8 / 10;
    mMannequin.mHeadScale.z = gMannequinHeadScale.z * 8 / 10;

    ModelResources_SetModel(gModelResources, 2, mItemModel.mModelPath);
    mItemModel.Load(2);
    mItemModel.mVisible = FALSE;
    VecFx32 itemPosition = sItemPosition;
    mItemModel.mPosition.x = itemPosition.x;
    mItemModel.mPosition.y = itemPosition.y;
    mItemModel.mPosition.z = itemPosition.z;
    mItemModel.SetScale(0x2800);
}

void AlbumScene::Update() {
    if (mList.mChosen) {
        mList.mChosen = FALSE;
        if (mBgShown != mList.mTab) {
            mBgShown = mList.mTab;
            VramTransferQueue_AddCharData(&gVramTransferQueue, mBgChars[mBgShown], 2, 0, 0x8000);
            ScreenLoader_QueueScreen(&gScreenLoader, mBgScreens[mBgShown], 2, 0, 0x800);
            PaletteLoader_QueueColors(&gPaletteLoader, mBgPalettes[mBgShown], 2, 0, 0x200, FALSE);
        }
    }
    if (mList.Update() == 1) {
        gSceneManager.RequestScene(0x5E);
    }
}

void AlbumScene::Draw() {
    mList.Draw();
    mMannequin.Draw();
    mItemModel.Draw();
    mRoom.RoomModels::Draw();
    mExplainText.ObjTextCanvas::Draw();
    mNameText.ObjTextCanvas::Draw();
    mPriceText.ObjTextCanvas::Draw();
    mCountText.ObjTextCanvas::Draw();
    mOwnedText.ObjTextCanvas::Draw();
    Sprite_Draw(&mSprite1);
    Sprite_Draw(&mSprite2);
}

void AlbumScene::Exit() {
    mCostumes.Save();
    mInventory.Save();
    mRoom.ReleaseAll();
    ArchiveManager_Unmount(&gArchiveManager, mItemModel.mArchiveName);
    ModelResources_ReleaseModel(gModelResources, 2);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ArchiveManager_Unmount(&gArchiveManager, "LTL");
    ArchiveManager_Unmount(&gArchiveManager, "SAB");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TIM");
    ArchiveManager_Unmount(&gArchiveManager, "TDM");
    ArchiveManager_Unmount(&gArchiveManager, "TRM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TXM");
}
