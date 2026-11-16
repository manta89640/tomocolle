#ifndef SCENE_BGTEXTTESTSCENE_H
#define SCENE_BGTEXTTESTSCENE_H

#include "MainProgram.h"

// Scene 9 (main program, no overlay): sets up a text canvas on background 2 of the main screen, in the 12-pixel
// font, and does nothing else: Update, Draw and Exit are empty, and nothing is drawn on the canvas. The scene
// factory (main func_0203c050, case 9) allocates 0x1D4 bytes; only the canvas is used of them.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class BgTextTestScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x010 */ BgTextCanvas mCanvas;
    /* 0x07C */ u8 mUnknown07C[0x158];
}; // 0x1D4

extern "C" {
// Sixteen colours: white, white, red, green, yellow, blue...
extern const u16 data_020b5abc[];
// Queues colours for copying to the palette of a plane: (loader, colours, plane, offset in bytes, size in bytes,
// unknown)
void func_020277dc(PaletteLoader *loader, const void *colors, int plane, int offset, int size, int unknown);
}

#endif // SCENE_BGTEXTTESTSCENE_H
