#ifndef OV011_SCENE31_H
#define OV011_SCENE31_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 11, scene 0x31 (not decompiled). The scene factory (CreateScene) loads overlay 11 and allocates 0x1DC1C
// bytes; the constructor is the main program's (0x020416E4, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B2C0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg44_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin",
// "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene31 : public Scene {
public:
    Scene31(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1CC0];
    /* 0x07300 */ TtsPlayer mTtsPlayer;
    /* 0x1BC44 */ u8 mUnknown1BC44[0xB0];
    /* 0x1BCF4 */ SpriteButton mButtons[1];
    /* 0x1BDE8 */ u8 mUnknown1BDE8[0xC];
    /* 0x1BDF4 */ MessageFile mMessageFiles[2];
    /* 0x1BE24 */ u8 mUnknown1BE24[0x8];
    /* 0x1BE2C */ MessageText mTexts[1];
    /* 0x1C41C */ u8 mUnknown1C41C[0x108];
    /* 0x1C524 */ MessageText mText;
    /* 0x1CB14 */ u8 mUnknown1CB14[0x7F8];
    /* 0x1D30C */ Room mRoom;
    /* 0x1DC14 */ u8 mUnknown1DC14[0x8];
}; // 0x1DC1C

#endif // OV011_SCENE31_H
