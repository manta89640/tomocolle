#include "BgTextTestScene.h"
#include "MainProgram.h"

extern "C" {
// Sixteen colours: white, white, red, green, yellow, blue...
extern const u16 data_020b5abc[];
// Queues colours for copying to the palette of a plane: (loader, colours, plane, offset in bytes, size in bytes,
// unknown)
void func_020277dc(PaletteLoader *loader, const void *colors, int plane, int offset, int size, int unknown);
}

void BgTextTestScene::StartLoading() {
}

void BgTextTestScene::Init() {
    GX_SetGraphicsMode(GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BG0_AS_2D);
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x18000);
    MI_CpuClearFast(G2_GetBG2CharPtr(), 0x8000);
    MI_CpuClearFast(G2_GetBG2ScrPtr(), 0x800);
    data_0211d604.Show(4);
    func_02028280(&data_02121834, 4, 0);
    func_020278f8(&data_021213b8, 0, 0x101);
    func_020277dc(&data_021213b8, data_020b5abc, 4, 0x1E0, 0x20, 0);
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
