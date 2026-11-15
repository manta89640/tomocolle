#ifndef OV076_MIIEDITSCENE_H
#define OV076_MIIEDITSCENE_H

#include <nitro.h>

#include "Scene.h"

// Overlay 76, the Mii maker: scenes 0x42, 0x43, 0x44 and 0x6F. The scene factory (main CreateScene, cases 66 to 68
// and 111) loads overlay 76 (0x4C) and allocates 0x983F8 bytes; the constructor is the main program's
// (func_0204332c), which stores the overlay's virtual table (0x0215A120).
//
// WORK IN PROGRESS: no function is decompiled yet. The layout below is MEASURED from that constructor (what it
// constructs where, and how many) and from the overlay's destructor (0x02158618); the members are byte arrays of
// the classes' sizes until the code that uses them is written. The classes named are the main program's:
//   SpriteButton 0xF4 (func_0203117c), TextButton 0x6F0 (func_02031f1c), MessageWindow 0xAFA4 (vtable
//   data_021102e8), MessageText 0x5F0, ObjTextBox 0x1BC (ObjTextCanvas with TextCanvas as virtual base at +0x154),
//   MessageFile 0x18, CharacterBase 0x25E4, CharacterModel (to 0x2B18 with the character), TtsPlayer.
//
// The name is descriptive, not the original: it is after the scene's archive, "Scn/Scene/SceneMiiEdit2_LZ.bin".
class MiiEditScene : public Scene {
public:
    virtual void StartLoading(); // 0x02149BAC
    virtual void Init();         // 0x02149C98
    virtual void Update();       // 0x0214A9B0
    virtual void Draw();         // 0x0214ADE8
    virtual void Exit();         // 0x0214B060

    /* 0x00010 */ u8 mUnknown00010[0x148];      // Init gives +0x10 to func_02024b10
    /* 0x00158 */ u8 mButtons00158[6][0xF4];    // SpriteButton[6]
    /* 0x00710 */ u8 mCharacter[0x2B18];        // CharacterBase (C2) and its CharacterModel at +0x25E4: the Mii shown
    /* 0x03228 */ u8 mEditedCharacter[0x26F8];  // CharacterBase (C1, func_0208fd18), destroyed with its D1
    /* 0x05920 */ u8 mTtsPlayer[0x14A48];       // TtsPlayer: the voice is tried out with it
    /* 0x1A368 */ u8 mTextBoxes[9][0x1BC];      // ObjTextBox[9]
    /* 0x1B304 */ u8 mUnknown1B304[0x58];
    /* 0x1B35C */ u8 mText1B35C[0x5F0];         // MessageText
    /* 0x1B94C */ u8 mText1B94C[0x5F0];         // MessageText
    /* 0x1BF3C */ u8 mCharBox[0x1BC];           // ObjCharBox (ObjTextCanvas with its TextCanvas at +0x154)
    /* 0x1C0F8 */ u8 mMessageFiles[2][0x18];    // MessageFile[2]: etc messages ("TBM"), common messages ("T02")
    /* 0x1C128 */ u8 mUnknown1C128[0x10];
    /* 0x1C138 */ u8 mWindows1C138[2][0xAFA4];  // MessageWindow[2]
    /* 0x32080 */ u8 mUnknown32080[0x2C];
    /* 0x320AC */ u8 mButtons320AC[36][0xF4];   // SpriteButton[36]
    /* 0x342FC */ u8 mButtons342FC[12][0xF4];   // SpriteButton[12]
    /* 0x34E6C */ u8 mUnknown34E6C[0x648];
    /* 0x354B4 */ u8 mWindows354B4[6][0xAFA4];  // MessageWindow[6]
    /* 0x7728C */ u8 mButtons7728C[10][0xF4];   // SpriteButton[10]
    /* 0x77C14 */ u8 mUnknown77C14[0x158];
    /* 0x77D6C */ u8 mKeyboard[0x6014];         // The on-screen keyboard (func_02082eac constructs it)
    /* 0x7DD80 */ u8 mButton7DD80[0x100];       // SpriteButton
    /* 0x7DE80 */ u8 mButtons7DE80[22][0xF4];   // SpriteButton[22]
    /* 0x7F378 */ u8 mTextButtons[3][0x6F0];    // TextButton[3]
    /* 0x80848 */ u8 mUnknown80848[0xB94];
    /* 0x813DC */ u8 mText813DC[0x89C];         // MessageText
    /* 0x81C78 */ u8 mButtons81C78[5][0xF4];    // SpriteButton[5]
    /* 0x8213C */ u8 mUnknown8213C[0x368];
    /* 0x824A4 */ u8 mWindow824A4[0xAFA4];      // MessageWindow
    /* 0x8D448 */ u8 mUnknown8D448[0xC];
    /* 0x8D454 */ u8 mWindow8D454[0xAFA4];      // MessageWindow
}; // 0x983F8

#endif // OV076_MIIEDITSCENE_H
