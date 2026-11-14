#ifndef TEXT_TEXTCANVAS_H
#define TEXT_TEXTCANVAS_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"

// The game's text drawing: a font, a canvas of characters that text is drawn on, the two places such a canvas can
// be shown (a background layer, a group of sprites), and boxes that redraw their content when it changes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

extern "C" {
// --- Not in this file, and not named yet ---
// The graphics manager: where a plane's character data is
void *GraphicsManager_GetCharBase(void *manager, int plane);
extern u32 gGraphicsManager[];
}

// A font of the library with the file it was loaded from.
class Font {
public:
    Font();
    void Init(const char *fileName);

    /* 0x0 */ NNSG2dFont mFont;
    /* 0x8 */ void *mResource; // The font file (NFTR), as found in the archive
};

// A canvas of the library and the state for drawing single characters on it with a drop shadow.
class CharCanvas {
public:
    // Bits of mShadowDirections: where a copy of the character in the shadow colour goes, relative to the character
    enum ShadowDirection {
        SHADOW_RIGHT_UP = 0x01,
        SHADOW_RIGHT = 0x02,
        SHADOW_RIGHT_DOWN = 0x04,
        SHADOW_DOWN = 0x08,
        SHADOW_LEFT_DOWN = 0x10,
        SHADOW_LEFT = 0x20,
        SHADOW_LEFT_UP = 0x40,
        SHADOW_UP = 0x80,
    };

    CharCanvas();
    int DrawChar(u16 character);
    void DrawGlyph(const NNSG2dGlyph *glyph);
    void GetShadowOffset(int direction, int *x, int *y);
    void *GetCharBase() { return GraphicsManager_GetCharBase(gGraphicsManager, mPlane); }
    void SetCharX(int x) { mCharX = x; }
    void AddCharX(int x) { mCharX += x; }
    void AddCharXU(u32 x) { mCharX += x; }
    void SetCharPos(int x, int y) {
        mCharX = x;
        mCharY = y;
    }

    /* 0x00 */ NNSG2dCharCanvas mCanvas;
    /* 0x18 */ const NNSG2dFont *mFont;
    /* 0x1C */ int mCharOffset;       // First character (tile) of the canvas in its VRAM
    /* 0x20 */ int mPlane;            // Which background or sprite plane the canvas is on, as the graphics manager's mask
    /* 0x24 */ int mCharX;            // Where DrawChar and DrawGlyph draw
    /* 0x28 */ int mCharY;
    /* 0x2C */ int mColor;
    /* 0x30 */ const NNSG2dChar *mText; // What TextCanvas::DrawText draws
    /* 0x34 */ void *mBuffer;         // If not NULL, the canvas draws here and Transfer() copies it to VRAM
    /* 0x38 */ int mShadowDirections;
    /* 0x3C */ int mShadowColor;
    /* 0x40 */ u16 mChar;             // What ObjCharBox draws
};

// Adds the library's text canvas, for drawing strings.
class TextCanvas : public CharCanvas {
public:
    TextCanvas();
    void InitTextCanvas(int hSpace, int vSpace);
    void DrawText();
    void SetFont(const NNSG2dFont *font);
    // These return unsigned though the fields are signed: MessageText's code only matches when the two differ
    u32 GetTextX() { return mTextX; }
    u32 GetTextY() { return mTextY; }

    /* 0x44 */ NNSG2dTextCanvas mTextCanvas;
    /* 0x54 */ u32 mDrawFlags; // NNS_G2D_VERTICALORIGIN_*, HORIZONTALORIGIN_*, HORIZONTALALIGN_*
    /* 0x58 */ int mTextX;
    /* 0x5C */ int mTextY;
    /* 0x60 */ int mUnknownClearedByConstructor60;
    /* 0x64 */ int mUnknownClearedByConstructor64;
};

// The two canvases with TextCanvas as a virtual base are declared in this order, ObjTextCanvas first, because the
// compiler emits their virtual tables in the reverse order of the classes' declarations, and BgTextCanvas's comes first
// in the ROM.

// A text canvas on sprites: the library arranges sprites over the canvas, and they are drawn as one cell.
class ObjTextCanvas : public virtual TextCanvas {
public:
    ObjTextCanvas();
    void InitCanvas(int plane, int width, int height, int charOffset, void *buffer, int vramMode,
                    NNSG2dCharaColorMode colorMode);
    void Init(const NNSG2dFont *font, int plane, int width, int height, int charOffset, int x, int y, int palette,
              void *buffer, int vramMode, NNSG2dCharaColorMode colorMode);
    void Arrange(int x, int y, int palette, int vramMode);
    void InitText(const NNSG2dFont *font, int plane, int width, int height, int charOffset, int hSpace, int vSpace,
                  int x, int y, int palette, void *buffer, int vramMode, NNSG2dCharaColorMode colorMode);
    void Draw();
    void Transfer();
    // Where the canvas is on the screen
    void SetPosition(fx32 x, fx32 y) { mSprite.SetPosition(x, y); }
    void SetPositionFromVec(fx32 x, fx32 y) { mSprite.SetPosition(Sprite::MakeVec(x, y)); }
    // How many characters (tiles) of VRAM the canvas takes
    int GetCharCount() { return ((mCanvas.areaWidth * mCanvas.areaHeight - 1) >> mVramMode) + 1; }

    /* 0x004 */ Sprite mSprite;
    /* 0x0B0 */ u8 mUnknownB0[4];
    /* 0x0B4 */ NNSG2dCellOAMAttrData mOam[24];
    /* 0x144 */ NNSG2dCellData mCell;
    /* 0x14C */ int mVramMode; // NNSG2dOBJVramMode: also how far a character number is shifted
};

// A text canvas on a background layer.
class BgTextCanvas : public virtual TextCanvas {
public:
    BgTextCanvas();
    void InitCanvas(int plane, int width, int height, int charOffset, void *buffer, NNSG2dCharaColorMode colorMode);
    void Init(const NNSG2dFont *font, int plane, int width, int height, int left, int top, int charOffset,
              int palette, void *buffer, NNSG2dTextBGWidth screenWidth, NNSG2dCharaColorMode colorMode);
    void MapScreen(int left, int top, int palette, NNSG2dTextBGWidth screenWidth);
    void InitText(const NNSG2dFont *font, int plane, int width, int height, int left, int top, int charOffset,
                  int hSpace, int vSpace, int palette, void *buffer, NNSG2dTextBGWidth screenWidth,
                  NNSG2dCharaColorMode colorMode);
    void Transfer();
};

// A sprite canvas showing one character.
class ObjCharBox : public ObjTextCanvas {
public:
    void Init(int plane, int width, int height, int charOffset, int x, int y, void *buffer);
    void SetChar(u16 character);
    int Redraw();
    void Update(BOOL redraw);

    /* 0x150 */ BOOL mChanged;
};

// A sprite canvas showing a string.
class ObjTextBox : public ObjTextCanvas {
public:
    void Init(int plane, int width, int height, int charOffset, int hSpace, int vSpace, int x, int y, void *buffer);
    void SetText(const NNSG2dChar *text);
    void Redraw();
    void Update(BOOL redraw);

    /* 0x150 */ BOOL mChanged;
};

// The game's three fonts, in the order 10, 8 and 12 pixels (the main program's start-up code loads
// "L10:/LD10UTF16.NFTR", "L08:/LD08UTF16.NFTR" and "L12:/LD12UTF16.NFTR" into them)
extern Font gFonts[3];

#endif // TEXT_TEXTCANVAS_H
