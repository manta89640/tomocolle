#ifndef OV033_TESTNG100SCENE_H
#define OV033_TESTNG100SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 33, scene 0x55 (not decompiled). The scene factory (CreateScene) loads overlay 33 and allocates 0x27880
// bytes; the constructor is the main program's (0x02044634, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BDA0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg100_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Mdl/Spl/SplNgEffect_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg100_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg100Scene : public Scene {
public:
    TestNg100Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0xC38];
    /* 0x00C48 */ SpriteButton mButtons[20];
    /* 0x01F58 */ u8 mUnknown01F58[0x50];
    /* 0x01FA8 */ SceneCharacter mCharacters[1];
    /* 0x04AC0 */ u8 mUnknown04AC0[0x1C40];
    /* 0x06700 */ TtsPlayer mTtsPlayer;
    /* 0x1B044 */ u8 mUnknown1B044[0x2E0];
    /* 0x1B324 */ MessageFile mMessageFiles[1];
    /* 0x1B33C */ u8 mUnknown1B33C[0x8];
    /* 0x1B344 */ MessageText mTexts[1];
    /* 0x1B934 */ u8 mUnknown1B934[0x98];
    /* 0x1B9CC */ MessageText mText;
    /* 0x1BFBC */ Room mRoom;
    /* 0x1C8C4 */ u8 mUnknown1C8C4[0xC];
    /* 0x1C8D0 */ MessageWindow mWindow;
    /* 0x27874 */ u8 mUnknown27874[0xC];
}; // 0x27880

#endif // OV033_TESTNG100SCENE_H
