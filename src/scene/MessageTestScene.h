#ifndef SCENE_MESSAGETESTSCENE_H
#define SCENE_MESSAGETESTSCENE_H

#include "MainProgram3d.h"
#include "text/MessageText.h"

// Scene 10 (main program, no overlay): a test of the message texts, with the etc and item messages
// ("Msg/TmdEtcMsgJP_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin"). It shows a line of text on a sprite canvas, an etc
// message on sprites and two on background 2, one of them with a string and two item messages put in its tags.
// A shows the next etc message in all three, drawn a character at a time on sprites, at once and a character
// at a time on the background; Y draws the rest of the last at once. The scene factory (main func_0203c050, case 10)
// allocates 0x2D84 bytes.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class MessageTestScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ MessageFile mFiles[2];        // The etc messages, the item messages
    /* 0x0040 */ ObjTextCanvas mCanvases[2];   // Only the first is used
    /* 0x03B0 */ u32 mMessage;                 // The etc message A shows next
    /* 0x03B4 */ MessageText mObjTexts[1];
    /* 0x09A4 */ MessageText mBgTexts[2];
    /* 0x1584 */ u8 mCharBuffer[0x1800];       // Where the first background text is drawn
}; // 0x2D84

#endif // SCENE_MESSAGETESTSCENE_H
