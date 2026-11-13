#ifndef OV000_TEXTTESTSCENE_H
#define OV000_TEXTTESTSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "text/TextCanvas.h"

#include "Scene.h"

// The scene of overlay 0 (scene 1 of the main program's scene factory): a test of the text classes. It draws the
// string L"テスト12３" ("test 12 3") on a sprite text canvas and the character L'い' in a sprite character box, and
// does nothing else.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
//
// The size and the members are from the main program: the scene factory func_0203c050, for scene 1, loads overlay 0,
// allocates 0x5C8 bytes and calls the constructor func_0203dea0, which stores the table of Scene and then this
// class's (0x02149c68), constructs three BgTextCanvas (0x6C each) from 0x10, an ObjTextCanvas at 0x154 and, at
// 0x40C, an ObjTextCanvas base with the table of ObjCharBox.
class TextTestScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x010 */ BgTextCanvas mBgCanvases[3]; // Constructed and never used
    /* 0x154 */ ObjTextCanvas mText;         // Shows the test string
    /* 0x30C */ u8 mCharBuffer[0x100];       // What mCharBox draws in, before it is copied to VRAM
    /* 0x40C */ ObjCharBox mCharBox;         // Shows the test character
}; // 0x5C8

#endif // OV000_TEXTTESTSCENE_H
