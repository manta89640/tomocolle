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
extern ScreenLoader gScreenLoader;
extern ArchiveLoader gArchiveManager;
extern u8 gScreenControl[];
extern u8 gVramManagerWork[];
extern u8 gCamera[];
extern Light gLights[];
extern GameData gGameData;
extern MATHRandContext16 gRandom;

// The message window's other functions
// Adds a button with a text: (number, resource, .., .., position, size, the text's message file and message, font,
// plane, character offset, the button's sounds, ..)
void MessageWindow_AddTextButtonEx(MessageWindow *window, int number, int resource, u8 a, u8 b, WindowPoint position,
                   WindowPoint size, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *sounds, int c);
// Adds a message text: (message file, message, font, plane, position, size, character offset)
void MessageWindow_AddTextInArea(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, WindowPoint size, int *charOffset);
void MessageWindow_AddSprite(MessageWindow *window, int resource, int sequence, WindowPoint position);
void MessageWindow_SetTextHAlign(MessageWindow *window, int a, int b);

void ScreenLoader_SetScrollY(ScreenLoader *loader, int plane, int y); // Scrolls a background
void ScreenLoader_SetPriority(ScreenLoader *loader, int a, int b);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void ResourceSet_SetArchive(int resourceSet, u32 resType, void *archive); // Gives a resource set an archive's animations
void GameData_ReadCharacter(GameData *game, int index, Character *character); // Reads a saved character
int MiiData_GetSex(Character *character);
void func_0209d898(Character *character, TtsPlayer *player); // Gives the speech player the character's voice
void ScreenControl_SetBlendAlpha(void *a, int b, int c, int d, int e);
void VramManagers_Reset(void *a);
void Camera_Reset(Camera *camera);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);

BOOL GameData_IsCharacterSlotUsed(GameData *game, int index);
u8 GameData_GetCharacterEvent(GameData *game, int index); // The character's event
u8 GameData_GetCharacterEventPartner(GameData *game, int index); // The place of its event
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

// --- The file's tables ---------------------------------------------------------------------------------------------

static const ScreenPoint sNamePosition = {0xC, 0xA0};
static const WindowPoint sTextSize = {0x53, 0x21};
static const ScreenPoint sShopPriceOffset = {0x34, -2};
static const ScreenPoint sTabSize = {0x2A, 0x18};
static const WindowPoint sTextPosition = {0xC, 2};
static const ScreenPoint sPriceLabelPosition = {0xC7, 0xA0};
static const WindowPoint sWindowButtonPositions[2] = {{0xB5, 0x95}, {0x4B, 0x95}};
static const ScreenPoint sUpButtonSize = {14, 12};
static const int sWindowButtonMessages[2] = {6, 5};
static const ScreenPoint sRowTouchSize = {0xE5, 18};
static const ScreenPoint sScrollBarPosition = {0xF4, 0x69};
static const ScreenPoint sUpButtonPosition = {0xF4, 0x3E};
static const ScreenPoint sShopPriceIconOffset = {0x66, 1};
static const ScreenPoint sPriceOffset = {0x34, -2};
static const WindowPoint sWindowButtonSizes[2] = {{0x5E, 0x1E}, {0x5E, 0x1E}};
static const ScreenPoint sDownButtonPosition = {0xF4, 0x94};
static const ScreenPoint sShopCountOffset = {0x2A, -2};
static const int sCanvasColors[3] = {8, 8, 1};
static const ScreenPoint sCanvasPositions[3] = {{0x2E, 0x42}, {0x7F, 0x42}, {0x43, 0x70}};
static const ScreenPoint sCanvasSizes[3] = {{4, 2}, {8, 2}, {8, 4}};
static const ScreenPoint sRowsPosition = {0x77, 0x41};
static const u8 sWindowButtonB[2] = {0x13, 0x13};
static const u8 sWindowButtonA[2] = {0x14, 0x14};
static const ScreenRect sShopWindowRect = {0x14, 0x14, 0xD8, 0x98};
static const WindowPoint sMinusButtonOffset = {0x38, 0x5D};
static const ButtonSounds sWindowButtonSounds[2] = {{-1, -1, 0, -1}, {0xC, -1, 0, -1}};
static const WindowPoint sPlusButtonOffset = {0x38, 0x33};
static const ScreenPoint sIconPosition2 = {0x80, 0x60};
static const ScreenPoint sCountPosition = {0xCD, 0xAC};
static const WindowPoint sIconPosition = {0x58, 0x76};
static const ScreenPoint sCountOffset = {0x2A, -2};
static const ScreenPoint sTextOffset = {-0x69, -6};
static const ScreenPoint sDownButtonSize = {14, 12};
static const ScreenPoint sPricePosition = {0xB2, 0xAC};
static const ScreenPoint sShopMarkOffset = {-0x6B, 0};
static GXRgb sCounterLightColors[2] = {GX_RGB(31, 31, 31), GX_RGB(31, 31, 31)};
static const VecFx32 sCounterLightDirection0 = {0x640, -0x258, -0xFA0};
static const VecFx32 sCounterLightDirection1 = {0, -0xFA0, 0x5DC};
static const int sTextMessage[1] = {0xC3}; // The window's question
static const ScreenPoint sPlusTouchSize = {18, 16};
static const ScreenPoint sMinusTouchSize = {14, 13};
static const ScreenPoint sPriceIconOffset = {0x66, 1};
static const ScreenPoint sCountLabelPosition = {0xC7, 0xA0};
static const u16 sTabSequences[5] = {0x17, 0x19, 0x1B, 0x1D, 0x1F};
static const u16 sTabPressedSequences[5] = {0x18, 0x1A, 0x1C, 0x1E, 0x20};
static const int sTabKinds[5] = {1, 2, 4, 8, 16};
static const ScreenPoint sTabPositions[5] = {{0x1A, 0x29}, {0x48, 0x29}, {0x76, 0x29}, {0xA4, 0x29}, {0xD2, 0x29}};
static const ScreenRect sScrollKnobRect = {-8, -8, 16, 16};
static const ScreenPoint sShopTextOffset = {-0x64, -6};

// --- The list -----------------------------------------------------------------------------------------------------

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
        SpriteButton_SetKind(&mRowButtons[i], 3);
        SpriteButton_SetSequences(&mRowButtons[i], sequence, pressedSequence);
        mRowButtons[i].SetPosition(FxVec2(sRowsPosition.x << FX32_SHIFT, (sRowsPosition.y + i * 20) << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mRowButtons[i], sRowTouchSize.x, sRowTouchSize.y);
        SpriteButton_Reset(&mRowButtons[i]);
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
    TouchScrollArea_Init(&mTouchArea, mRect);
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

void ItemListView::InitUpButton(int resource, u16 sequence, u16 pressedSequence, ScreenPoint position,
                                ScreenPoint size) {
    Sprite_Init(&mUpButton, resource, sequence, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mUpButton, 1);
    SpriteButton_SetSequences(&mUpButton, sequence, pressedSequence);
    mUpButton.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    SpriteButton_SetTouchRect(&mUpButton, size.x, size.y);
    SpriteButton_Reset(&mUpButton);
    mUpButton.mOverwritePalette = 2;
}

void ItemListView::InitUpButton(int resource, u16 sequence, u16 pressedSequence) {
    InitUpButton(resource, sequence, pressedSequence, sUpButtonPosition, sUpButtonSize);
}

void ItemListView::InitDownButton(int resource, u16 sequence, u16 pressedSequence, ScreenPoint position,
                                  ScreenPoint size) {
    Sprite_Init(&mDownButton, resource, sequence, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mDownButton, 1);
    SpriteButton_SetSequences(&mDownButton, sequence, pressedSequence);
    mDownButton.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
    SpriteButton_SetTouchRect(&mDownButton, size.x, size.y);
    SpriteButton_Reset(&mDownButton);
    mDownButton.mOverwritePalette = 2;
}

void ItemListView::InitDownButton(int resource, u16 sequence, u16 pressedSequence) {
    InitDownButton(resource, sequence, pressedSequence, sDownButtonPosition, sDownButtonSize);
}

void ItemListView::InitScrollBar(int resource, int sequence, int knobSequence, int a, int b, int c,
                                 ScreenPoint position, ScreenRect knob) {
    Sprite_Init(&mScrollBar, resource, sequence, 0x80, 0x60, TRUE);
    ScrollBar_Init(&mScrollBar, a, b, c, knob.x, knob.y, knob.width, knob.height);
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
    BOOL up = SpriteButton_Update(&mUpButton);
    BOOL down = SpriteButton_Update(&mDownButton);
    int max = (mItemCount - mVisibleRows) * 20;
    if (max < 0) {
        max = 0;
    }
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
        Refresh();
    }
}

void ItemListView::Draw() {
    SpriteButton_Draw(&mUpButton);
    SpriteButton_Draw(&mDownButton);
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
        SpriteButton_Draw(&mRowButtons[i]);
    }
}

static inline void PlaceRow(ItemListView *list, int i, ScreenPoint row) {
    SetSpritePosition(&list->mRowButtons[i], row);
    SetCanvasPosition(&list->mRowTexts[i], AddPoints(row, sTextOffset));
    SetCanvasPosition(&list->mRowPrices[i], AddPoints(row, sPriceOffset));
    SetSpritePosition(&list->mRowPriceIcons[i], AddPoints(row, sPriceIconOffset));
    SetCanvasPosition(&list->mRowCounts2[i], AddPoints(row, sCountOffset));
}

// Puts each row where its item is at the scroll position, and scrolls the background with them
void ItemListView::Refresh() {
    for (int i = 0; i < mRowCount; i++) {
        PlaceRow(this, i, AddPoints(mPosition, MakePoint(0, GetRowItem(i) * 20 - mScroll)));
    }
    ScreenLoader_SetScrollY(&gScreenLoader, mBgPlane, mScroll % 20 - mBgOffset);
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
        SpriteButton_SetSelected(&mRowButtons[row], TRUE);
    } else {
        SpriteButton_SetSelected(&mRowButtons[row], FALSE);
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
        ScrollBar_SetPosition(&mScrollBar, 0, (mItemCount - mVisibleRows) * 20, 0);
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
        SpriteButton_SetSelected(&mRowButtons[i], FALSE);
    }
    int top = mScroll / 20;
    if (top <= index && index < mRowCount + top) {
        int row = index % mRowCount;
        mTouchedRow = row;
        SpriteButton_SetSelected(&mRowButtons[row], TRUE);
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
    ScrollBar_SetPosition(&mScrollBar, 0, (mItemCount - mVisibleRows) * 20, mScroll);
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

void ShopItemListView::Refresh() {
    for (int i = 0; i < mRowCount; i++) {
        ScreenPoint row = AddPoints(mPosition, MakePoint(0, GetRowItem(i) * 20 - mScroll));
        SetSpritePosition(&mRowButtons[i], row);
        SetCanvasPosition(&mRowTexts[i], AddPoints(row, sShopTextOffset));
        SetCanvasPosition(&mRowPrices[i], AddPoints(row, sShopPriceOffset));
        SetSpritePosition(&mRowPriceIcons[i], AddPoints(row, sShopPriceIconOffset));
        SetCanvasPosition(&mRowCounts2[i], AddPoints(row, sShopCountOffset));
        SetSpritePosition(&mMarks[i], AddPoints(row, sShopMarkOffset));
    }
    ScreenLoader_SetScrollY(&gScreenLoader, mBgPlane, mScroll % 20 - mBgOffset);
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
    MessageWindow_Init(this, rect);
    mMode = MODE_BUY;
    mCount = 0;
}

// The plus and minus buttons and the three canvases
void ShopWindow::InitButtons(int resource, int *charOffset, MessageFile *file) {
    Sprite_Init(&mPlusButton, resource, 6, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mPlusButton, 1);
    SpriteButton_SetSequences(&mPlusButton, 6, 7);
    mPlusButton.SetPosition(FxVec2(56 << FX32_SHIFT, 51 << FX32_SHIFT));
    SpriteButton_SetTouchRect(&mPlusButton, sPlusTouchSize.x, sPlusTouchSize.y);
    mPlusButton.mUnknownE0 = 6;
    SpriteButton_Reset(&mPlusButton);
    mPlusButton.mOverwritePalette = 0;
    mPlusButton.mVisible = FALSE;
    mPlusButton.mUnknownDC = TRUE;
    Sprite_Init(&mMinusButton, resource, 8, 0x80, 0x60, TRUE);
    SpriteButton_SetKind(&mMinusButton, 1);
    SpriteButton_SetSequences(&mMinusButton, 8, 9);
    mMinusButton.SetPositionFromVec(56 << FX32_SHIFT, 93 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mMinusButton, sMinusTouchSize.x, sMinusTouchSize.y);
    mMinusButton.mUnknownE0 = 6;
    SpriteButton_Reset(&mMinusButton);
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
    MessageWindow_AddSprite(this, resource, 0x33, sIconPosition);
    mMessageFile = file;
}

void ShopWindow::Init(const char *screenName, int plane, int resource, int *charOffset, MessageFile *file,
                      MessageFile *buttonFile, int *money) {
    ScreenRect rect = sShopWindowRect;
    Init(rect);
    MessageWindow_SetScreen(this, screenName, plane);
    func_020331bc(this, 0x3F00, rect);
    InitButtons(resource, charOffset, file);
    for (int i = 0; i <= 1; i++) {
        MessageWindow_AddTextButtonEx(this, i, resource, sWindowButtonB[i], sWindowButtonA[i], sWindowButtonPositions[i], sWindowButtonSizes[i], buttonFile,
                      sWindowButtonMessages[i], &gFonts[2].mFont, 0x1000, charOffset, &sWindowButtonSounds[i], 1);
    }
    MessageWindow_AddTextInArea(this, file, sTextMessage[0], &gFonts[0].mFont, 0x1000, sTextPosition, sTextSize, charOffset);
    MessageWindow_SetTextHAlign(this, 0, 1);
    mMoney = money;
}

// The plus and minus buttons change the count, as far as the money, the item's count and the 99 the player can own
// allow
void ShopWindow::Update() {
    MessageWindow_Update(this);
    if (!(mUnknownAD50 & 1)) {
        return;
    }
    switch (mMode) {
    case MODE_BUY:
        if (SpriteButton_Update(&mPlusButton)) {
            int count = mCount + 1;
            if (count + mItem->mCount <= 99 && count + mItem->mUnknown0C >= 0 && GetBuyTotal(mItem, count) <= *mMoney) {
                mCount++;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 8);
                return;
            }
        }
        if (SpriteButton_Update(&mMinusButton)) {
            int count = mCount - 1;
            if (count > 0) {
                mCount = count;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 9);
            }
        }
        break;
    case MODE_SELL:
        if (SpriteButton_Update(&mPlusButton)) {
            int count = mCount + 1;
            if (mItem->mCount - count <= 99 && mItem->mUnknown0C - count >= 0) {
                mCount = count;
                DrawCounts();
                gSoundManager.StartSeqArc(0, 1, 8);
                return;
            }
        }
        if (SpriteButton_Update(&mMinusButton)) {
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
        SpriteButton_Draw(&mPlusButton);
        SpriteButton_Draw(&mMinusButton);
    }
    MessageWindow_Draw(this);
}

void ShopWindow::Open(int flags, int mode) {
    MessageWindow_Open(this, flags, 1);
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
    mIcon.SetPositionFromVec(sIconPosition2.x << FX32_SHIFT, sIconPosition2.y << FX32_SHIFT);
    mIcon.mOverwritePalette = 1;
    Sprite_Init(&mSecondIcon, resource, (u16)(sequence + 1), 0x80, 0x60, TRUE);
    mSecondIcon.SetPositionFromVec(sIconPosition2.x << FX32_SHIFT, sIconPosition2.y << FX32_SHIFT);
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
    mMannequin.SetArchive("3CM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacter.mArchiveName, mCharacter.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mMannequin.mArchiveName, mMannequin.mArchivePath,
                                   0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_contents_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3CH", "Mdl/Misc/headStaff_3D_LZ.bin", 0);
}

void ShopStaff::Unmount() {
    ModelResources_ReleaseModel(gModelResources, 28);
    ModelResources_ReleaseModel(gModelResources, 29);
    ModelResources_ReleaseModel(gModelResources, 30);
    ModelResources_ReleaseModel(gModelResources, 31);
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
    ModelResources_SetModel(gModelResources, 28, mCharacter.mModelPath);
    ModelResources_SetModel(gModelResources, 30, mMannequin.mModelPath);
    ModelResources_SetModel(gModelResources, 31, "3CH:/head.imd");
    void *animations = ModelResources_BindArchive(gModelResources, 28, "3HA", 0);
    mCharacter.Init(28, 0, 0);
    ResourceSet_SetArchive(mCharacter.mAnim.mUnknown04, RESOURCE_BCA0, animations);
    mCharacter.SetAnimation(RESOURCE_BCA0, 33);
    if (mResident) {
        GameData_ReadCharacter(&gGameData, mResidentIndex, &mCharacter);
        ModelResources_ReleaseModel(gModelResources, 28);
        ArchiveManager_Unmount(&gArchiveManager, mCharacter.mArchiveName);
        mCharacter.CharacterModel::SetArchive(mCharacter.mArchiveName, uniform, MiiData_GetSex(&mCharacter));
        ArchiveManager_MountCompressed(&gArchiveManager, mCharacter.mArchiveName, mCharacter.mArchivePath, 1);
        ModelResources_SetModel(gModelResources, 28, mCharacter.mModelPath);
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
        ModelResources_ReleaseModel(gModelResources, 30);
        ArchiveManager_Unmount(&gArchiveManager, "3CM");
        mMannequin.SetArchive("3CM", uniform, 0);
        ArchiveManager_MountCompressed(&gArchiveManager, "3CM", mMannequin.mArchivePath, 1);
        ModelResources_SetModel(gModelResources, 30, mMannequin.mModelPath);
        mMannequin.MakeModel(30, 0);
        mMannequin.LoadHead(31, 0);
        mMannequin.LoadTexturePalette();
        mMannequin.SetPaletteColor1(0x5FBE);
        void *headAnimations = ModelResources_BindArchive(gModelResources, 30, "3HA", 0);
        ResourceSet_SetArchive(mMannequin.mAnim.mUnknown04, RESOURCE_BCA0, headAnimations);
        mMannequin.SetAnimation(RESOURCE_BCA0, 33);
        mMannequin.mShadowVisible = TRUE;
        VecFx32 pos = mPosition;
        mMannequin.mPos.x = pos.x;
        mMannequin.mPos.y = pos.y;
        mMannequin.mPos.z = pos.z;
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
            if (mMannequin.mBlendFrames <= 0) {
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
            if (mMannequin.mBlendFrames <= 0) {
                mSpeechWindow->Show(mMessages[0], 0, 120);
                mTtsPlayer->Speak((const u16 *)mMessageFile->GetText(mMessages[1]), 0);
                mMannequin.BlendToAnimation(RESOURCE_BCA0, 29);
                AnimModel_FindAnimation(&mMannequin.mAnim, RESOURCE_BCA0, mMannequin.mAnimId)
                    ->mPlayOnce = TRUE;
                mStep++;
            }
            break;
        case 2:
            if (AnimModel_FindAnimation(&mMannequin.mAnim, RESOURCE_BCA0, mMannequin.mAnimId)
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
                AnimModel_FindAnimation(&mMannequin.mAnim, RESOURCE_BCA0, mMannequin.mAnimId)
                    ->mPlayOnce = TRUE;
            }
            break;
        }
        break;
    case STATE_TURNING_BACK:
        switch (mStep) {
        case 0:
            if (mMannequin.mBlendFrames <= 0) {
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
        MtxFx43 *matrices = mMannequin.mJointMatrices;
        mHeldModel->SetPosition(matrices[CharacterModel::JOINT_R_HAND]._30, matrices[CharacterModel::JOINT_R_HAND]._31,
                                matrices[CharacterModel::JOINT_R_HAND]._32);
        MtxFx33 *rotations = mMannequin.mJointRotations;
        MI_Copy36B(&rotations[CharacterModel::JOINT_R_HAND], &mHeldModel->mRotation);
        mHeldModel->mRotY = 0x4000;
    }
}

// --- The counter --------------------------------------------------------------------------------------------------

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
    ScreenControl_SetBlendAlpha(gScreenControl, 4, 17, 8, 8);
    VramManagers_Reset(gVramManagerWork);
    *(BOOL *)(gScreenControl + 0x48) = TRUE;
    *(BOOL *)(gScreenControl + 0xEC) = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    Camera *camera = (Camera *)gCamera;
    Camera_Reset(camera);
    camera->mUnknown00 = 0x281;
    camera->mUnknown04 = 0xFCE;
    camera->mUnknown08 = 0x1555;
    camera->mUnknown0C = FX32_ONE;
    camera->mUnknown10 = 0x50000;
    camera->mPosition = cameraPosition;
    camera->mTarget = cameraTarget;
    Light *light0 = (Light *)gLights;
    light0->mColor = sCounterLightColors[1];
    Light_SetDirectionByCamera(light0, camera, sCounterLightDirection0);
    Light *light1 = &gLights[1];
    light1->mColor = sCounterLightColors[0];
    Light_SetDirectionByCamera(light1, camera, sCounterLightDirection1);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 1);
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

// Not called, and stripped by the link: it keeps the tables of five tab buttons that the ROM has among the file's
// other tables (sTabSequences, sTabPressedSequences, sTabKinds, sTabPositions, sTabSize), which no code that is
// linked uses. What it does with them is a guess; only the tables are the original's.
void Shop_UnusedTabs(SpriteButton *buttons, int resource) {
    for (int i = 0; i < 5; i++) {
        Sprite_Init(&buttons[i], resource, sTabSequences[i], 0x80, 0x60, TRUE);
        SpriteButton_SetKind(&buttons[i], sTabKinds[i]);
        SpriteButton_SetSequences(&buttons[i], sTabSequences[i], sTabPressedSequences[i]);
        buttons[i].SetPosition(FxVec2(sTabPositions[i].x << FX32_SHIFT, sTabPositions[i].y << FX32_SHIFT));
        SpriteButton_SetTouchRect(&buttons[i], sTabSize.x, sTabSize.y);
    }
}

// Picks at random one of the residents whose event (0x14) is at the given place, for the staff to be
BOOL FindResidentAt(int place, BOOL *found, int *index) {
    int candidates[100];
    int count = 0;
    *found = FALSE;
    *index = 0;
    for (int i = 0; i < 100; i++) {
        if (GameData_IsCharacterSlotUsed(&gGameData, i) && GameData_GetCharacterEvent(&gGameData, i) == 0x14 &&
            place == GameData_GetCharacterEventPartner(&gGameData, i)) {
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
