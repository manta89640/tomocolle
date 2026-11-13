#include "TextTestScene.h"

extern "C" {
// --- The main program's, not named yet ---
// The palette loader (as overlay 74 has it)
struct PaletteLoader;
extern PaletteLoader data_021213b8;
// Sixteen colours: white, white, red, green, yellow, blue...
extern const u16 data_020b5abc[];
// Queues colours for copying to the palette of a plane: (loader, colours, plane, offset in bytes, size in bytes,
// unknown)
void func_020277dc(PaletteLoader *loader, const void *colors, int plane, int offset, int size, int unknown);
}

// The library's character type is u16, which is not this compiler's wchar_t: a wide string literal is given to the
// text classes by way of this. (Init only matches with the conversion in an inline function: with a cast in place,
// the string's address is loaded after the canvas's instead of before.)
static inline const NNSG2dChar *ToG2dChars(const wchar_t *text) {
    return (const NNSG2dChar *)text;
}

// @0x02149a40
void TextTestScene::StartLoading() {
}

// @0x02149a44
void TextTestScene::Init() {
    func_020277dc(&data_021213b8, data_020b5abc, 0x10, 0x1E0, 0x20, 0);
    MI_CpuClearFast(GraphicsManager_GetCharBase(gGraphicsManager, 0x10), 0x800);

    mText.mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT;
    mText.mTextX = 128;
    mText.InitText(&gFonts[0].mFont, 0x10, 16, 4, 2, 1, 1, 0, 0, 15, NULL, 2, NNS_G2D_CHARA_COLORMODE_16);
    mText.mText = ToG2dChars(L"テスト12３");
    NNS_G2dCharCanvasClear(&mText.mCanvas, 2);
    mText.DrawText();

    MI_CpuClearFast(mCharBuffer, sizeof(mCharBuffer));
    mCharBox.Init(0x10, 4, 2, 100, 0, 0, mCharBuffer);
    mCharBox.mColor = 1;
    mCharBox.SetChar(L'い');
    mCharBox.SetPositionFromVec(100 * FX32_ONE, 100 * FX32_ONE);
    mCharBox.Redraw();
    mCharBox.Transfer();
}

// @0x02149c0c
void TextTestScene::Update() {
}

// @0x02149c10
void TextTestScene::Draw() {
    mText.Draw();
    mCharBox.Update(FALSE);
}

// @0x02149c34
void TextTestScene::Exit() {
}
