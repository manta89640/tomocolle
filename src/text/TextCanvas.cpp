#include "text/TextCanvas.h"

#include "text/TextObjResource.h"

extern "C" {

// --- Not in this file, and not named yet ---

// Returns the file of that name in an archive (it calls NNS_FndGetArchiveFileByName with the name)
void *ArchiveManager_GetFileByName(void *archive, const char *fileName);
extern u8 gArchiveManager[];

// The graphics manager: where a plane's character data is, and where a background's screen data is
void *GraphicsManager_GetScreenBase(void *manager, int plane);

// Queues a copy of character data to a plane's VRAM
void VramTransferQueue_AddCharData(void *queue, void *source, int plane, int offset, int size);
extern u8 gVramTransferQueue[];

// The sprite class: set up with an animation resource, and draw
void Sprite_Init(Sprite *sprite, int resource, int unk2, int x, int y, int unk5);
void Sprite_Draw(Sprite *sprite);

// Loads a sprite cell file and its character file for a screen under a resource number
void ObjResourceManager_Load(void *manager, const char *cellPath, const char *charPath, int screen, int resource, int unk);
extern u8 gObjResourceManager[];

} // extern "C"

TextObjResource gTextObjResource;
static GXOamAttr sOamBuffer[24]; // Where the library arranges a canvas's sprites

CharCanvas::CharCanvas() {
    mCharY = 0;
    mCharX = 0;
    mColor = 8;
    mText = NULL;
    mBuffer = NULL;
    mShadowDirections = 0;
    mShadowColor = 1;
}

int CharCanvas::DrawChar(u16 character) {
    if (mShadowDirections != 0) {
        for (int direction = 1; direction < 0xFF; direction <<= 1) {
            if (direction & mShadowDirections) {
                int x;
                int y;
                GetShadowOffset(direction & mShadowDirections, &x, &y);
                NNS_G2dCharCanvasDrawChar(&mCanvas, mFont, mCharX + x, mCharY + y, mShadowColor, character);
            }
        }
    }
    return NNS_G2dCharCanvasDrawChar(&mCanvas, mFont, mCharX, mCharY, mColor, character);
}

void CharCanvas::DrawGlyph(const NNSG2dGlyph *glyph) {
    if (mShadowDirections != 0) {
        for (int direction = 1; direction < 0xFF; direction <<= 1) {
            if (direction & mShadowDirections) {
                int x;
                int y;
                GetShadowOffset(direction & mShadowDirections, &x, &y);
                NNS_G2dCharCanvasDrawGlyph(&mCanvas, mFont, mCharX + x, mCharY + y, mShadowColor, glyph);
            }
        }
    }
    NNS_G2dCharCanvasDrawGlyph(&mCanvas, mFont, mCharX, mCharY, mColor, glyph);
}

void CharCanvas::GetShadowOffset(int direction, int *x, int *y) {
    switch (direction) {
    case 0:
        *y = 0;
        *x = 0;
        break;
    case SHADOW_RIGHT_UP:
        *x = 1;
        *y = -1;
        break;
    case SHADOW_RIGHT:
        *x = 1;
        *y = 0;
        break;
    case SHADOW_RIGHT_DOWN:
        *x = 1;
        *y = 1;
        break;
    case SHADOW_DOWN:
        *x = 0;
        *y = 1;
        break;
    case SHADOW_LEFT_DOWN:
        *x = -1;
        *y = 1;
        break;
    case SHADOW_LEFT:
        *x = -1;
        *y = 0;
        break;
    case SHADOW_LEFT_UP:
        *x = -1;
        *y = -1;
        break;
    case SHADOW_UP:
        *x = 0;
        *y = -1;
        break;
    default:
        *y = 0;
        *x = 0;
        break;
    }
}

TextCanvas::TextCanvas() {
    mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_LEFT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mTextX = 0;
    mTextY = 0;
    mUnknownClearedByConstructor60 = 0;
    mUnknownClearedByConstructor64 = 0;
}

void TextCanvas::InitTextCanvas(int hSpace, int vSpace) {
    NNS_G2dTextCanvasInit(&mTextCanvas, &mCanvas, mFont, hSpace, vSpace);
}

void TextCanvas::DrawText() {
    if (mShadowDirections != 0) {
        for (int direction = 1; direction < 0xFF; direction <<= 1) {
            if (direction & mShadowDirections) {
                int x;
                int y;
                GetShadowOffset(direction & mShadowDirections, &x, &y);
                NNS_G2dTextCanvasDrawText(&mTextCanvas, mTextX + x, mTextY + y, mShadowColor, mDrawFlags, mText);
            }
        }
    }
    NNS_G2dTextCanvasDrawText(&mTextCanvas, mTextX, mTextY, mColor, mDrawFlags, mText);
}

void TextCanvas::SetFont(const NNSG2dFont *font) {
    mFont = font;
    mTextCanvas.pFont = font;
}

BgTextCanvas::BgTextCanvas() {
}

void BgTextCanvas::InitCanvas(int plane, int width, int height, int charOffset, void *buffer,
                              NNSG2dCharaColorMode colorMode) {
    void *charBase;

    mPlane = plane;
    mCharOffset = charOffset;
    if (buffer != NULL) {
        mBuffer = buffer;
        charBase = buffer;
        charOffset = 0;
    } else {
        charBase = GetCharBase();
    }
    NNS_G2dCharCanvasInitForBG(&mCanvas, (u8 *)charBase + charOffset * 32, width, height, colorMode);
    if (buffer != NULL) {
        u8 *vram = (u8 *)GetCharBase();
        vram += mCharOffset * 32;
        MI_CpuClearFast(vram, mCanvas.areaWidth * mCanvas.areaHeight * 32);
    }
}

void BgTextCanvas::Init(const NNSG2dFont *font, int plane, int width, int height, int left, int top, int charOffset,
                        int palette, void *buffer, NNSG2dTextBGWidth screenWidth, NNSG2dCharaColorMode colorMode) {
    mFont = font;
    InitCanvas(plane, width, height, charOffset, buffer, colorMode);
    MapScreen(left, top, palette, screenWidth);
}

void BgTextCanvas::MapScreen(int left, int top, int palette, NNSG2dTextBGWidth screenWidth) {
    NNS_G2dMapScrToCharText(GraphicsManager_GetScreenBase(gGraphicsManager, mPlane), mCanvas.areaWidth, mCanvas.areaHeight, left, top,
                            screenWidth, mCharOffset, palette);
}

void BgTextCanvas::InitText(const NNSG2dFont *font, int plane, int width, int height, int left, int top,
                            int charOffset, int hSpace, int vSpace, int palette, void *buffer,
                            NNSG2dTextBGWidth screenWidth, NNSG2dCharaColorMode colorMode) {
    Init(font, plane, width, height, left, top, charOffset, palette, buffer, screenWidth, colorMode);
    InitTextCanvas(hSpace, vSpace);
}

void BgTextCanvas::Transfer() {
    if (mBuffer != NULL) {
        int offset = mCharOffset << 5;
        gGraphicsManager[6] |= mPlane;
        VramTransferQueue_AddCharData(gVramTransferQueue, mBuffer, mPlane, offset, mCanvas.areaWidth * mCanvas.areaHeight << 5);
    }
}

ObjTextCanvas::ObjTextCanvas() {
    mSprite.ResetPosition();
    mVramMode = NNS_G2D_OBJVRAMMODE_128K;
}

void ObjTextCanvas::InitCanvas(int plane, int width, int height, int charOffset, void *buffer, int vramMode,
                               NNSG2dCharaColorMode colorMode) {
    void *charBase;

    mPlane = plane;
    mCharOffset = charOffset;
    mVramMode = vramMode;
    if (buffer != NULL) {
        mBuffer = buffer;
        charBase = buffer;
        charOffset = 0;
    } else {
        charBase = GetCharBase();
    }
    NNS_G2dCharCanvasInitForOBJ1D(&mCanvas, (u8 *)charBase + (charOffset << vramMode) * 32, width, height, colorMode);
    if (buffer != NULL) {
        u8 *vram = (u8 *)GetCharBase();
        vram += (mCharOffset << mVramMode) * 32;
        MI_CpuClearFast(vram, mCanvas.areaWidth * mCanvas.areaHeight * 32);
    }
    if (plane == 0x10) {
        Sprite_Init(&mSprite, 0x47, 0, 0, 0, 1);
    } else {
        Sprite_Init(&mSprite, 0x46, 0, 0, 0, 1);
    }
}

void ObjTextCanvas::Init(const NNSG2dFont *font, int plane, int width, int height, int charOffset, int x, int y,
                         int palette, void *buffer, int vramMode, NNSG2dCharaColorMode colorMode) {
    mFont = font;
    InitCanvas(plane, width, height, charOffset, buffer, vramMode, colorMode);
    Arrange(x, y, palette, vramMode);
}

void ObjTextCanvas::Arrange(int x, int y, int palette, int vramMode) {
    int count = NNS_G2dArrangeOBJ1D(sOamBuffer, mCanvas.areaWidth, mCanvas.areaHeight, x, y, GX_OAM_COLORMODE_16,
                                    mCharOffset, (NNSG2dOBJVramMode)vramMode);
    const GXOamAttr *from = sOamBuffer;
    NNSG2dCellOAMAttrData *to = mOam;
    for (int i = 0; i < count; i++) {
        to->attr0 = from->attr0;
        to->attr1 = from->attr1;
        to->attr2 = from->attr2;
        from++;
        to++;
    }
    mCell.numOAMAttrs = count;
    mCell.cellAttr = mSprite.mAnim.pCurrentCell->cellAttr;
    mCell.pOamAttrArray = mOam;
    mSprite.mAnim.pCurrentCell = &mCell;
    mSprite.mPaletteOffset = palette;
}

void ObjTextCanvas::InitText(const NNSG2dFont *font, int plane, int width, int height, int charOffset, int hSpace,
                             int vSpace, int x, int y, int palette, void *buffer, int vramMode,
                             NNSG2dCharaColorMode colorMode) {
    Init(font, plane, width, height, charOffset, x, y, palette, buffer, vramMode, colorMode);
    InitTextCanvas(hSpace, vSpace);
}

void ObjTextCanvas::Draw() {
    mSprite.mAnim.pCurrentCell = &mCell;
    Sprite_Draw(&mSprite);
}

void ObjTextCanvas::Transfer() {
    if (mBuffer != NULL) {
        VramTransferQueue_AddCharData(gVramTransferQueue, mBuffer, mPlane, (mCharOffset << mVramMode) << 5,
                      mCanvas.areaWidth * mCanvas.areaHeight << 5);
    }
}

void ObjCharBox::Init(int plane, int width, int height, int charOffset, int x, int y, void *buffer) {
    ObjTextCanvas::Init(&gFonts[0].mFont, plane, width, height, charOffset, x, y, 15, buffer, NNS_G2D_OBJVRAMMODE_128K,
                        NNS_G2D_CHARA_COLORMODE_16);
    mColor = 8;
    NNS_G2dCharCanvasClear(&mCanvas, 0);
    mSprite.SetPosition(0, 0);
    mSprite.mOverwritePalette = 0;
    mChanged = FALSE;
}

void ObjCharBox::SetChar(u16 character) {
    mChar = character;
    mChanged = TRUE;
}

int ObjCharBox::Redraw() {
    int result = 0;
    if (mChanged) {
        result = DrawChar(mChar);
        mChanged = FALSE;
    }
    return result;
}

void ObjCharBox::Update(BOOL redraw) {
    if (redraw) {
        Redraw();
    }
    Draw();
}

void ObjTextBox::Init(int plane, int width, int height, int charOffset, int hSpace, int vSpace, int x, int y,
                      void *buffer) {
    InitText(&gFonts[0].mFont, plane, width, height, charOffset, hSpace, vSpace, x, y, 15, buffer,
             NNS_G2D_OBJVRAMMODE_128K, NNS_G2D_CHARA_COLORMODE_16);
    mColor = 8;
    NNS_G2dCharCanvasClear(&mCanvas, 0);
    mSprite.SetPosition(0, 0);
    mSprite.mOverwritePalette = 0;
    mChanged = FALSE;
}

void ObjTextBox::SetText(const NNSG2dChar *text) {
    mText = text;
    mChanged = TRUE;
}

void ObjTextBox::Redraw() {
    if (!mChanged) {
        return;
    }
    NNS_G2dCharCanvasClear(&mCanvas, 0);
    DrawText();
    mChanged = FALSE;
}

void ObjTextBox::Update(BOOL redraw) {
    if (redraw) {
        Redraw();
    }
    Draw();
}

void TextObjResource::Load(const char *cellPath, const char *charPath, int screen, int resource, int unk) {
    mScreen = screen;
    mResource = resource;
    mUnknown = unk;
    ObjResourceManager_Load(gObjResourceManager, cellPath, charPath, screen, resource, unk);
}
