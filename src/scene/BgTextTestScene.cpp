#include "BgTextTestScene.h"
#include "MainProgram.h"

extern "C" {
// Sixteen colours: white, white, red, green, yellow, blue...
extern const u16 data_020b5abc[];
// Queues colours for copying to the palette of a plane: (loader, colours, plane, offset in bytes, size in bytes,
// unknown)
void PaletteLoader_QueueColors(PaletteLoader *loader, const void *colors, int plane, int offset, int size, int unknown);
}

void BgTextTestScene::StartLoading() {
}

void BgTextTestScene::Init() {
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_2D);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x18000);
    MI_CpuClearFast(G2_GetBG2CharPtr(), 0x8000);
    MI_CpuClearFast(G2_GetBG2ScrPtr(), 0x800);
    gScreenControl.Show(4);
    ScreenLoader_SetPriority(&gScreenLoader, 4, 0);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0, 0x101);
    PaletteLoader_QueueColors(&gPaletteLoader, data_020b5abc, 4, 0x1E0, 0x20, 0);
    mCanvas.InitText(&gFonts[2].mFont, 4, 25, 2, 2, 1, 1, 0, 1, 15, NULL, NNS_G2D_TEXT_BG_WIDTH_256,
                     NNS_G2D_CHARA_COLORMODE_16);
    mCanvas.mColor = 1;
}

void BgTextTestScene::Update() {
}

void BgTextTestScene::Draw() {
}

void BgTextTestScene::Exit() {
}
