#ifndef OV039_TESTNG116SCENE_H
#define OV039_TESTNG116SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 39, scenes 0x77, 0x78, 0x79 (not decompiled). The scene factory (CreateScene) loads overlay 39 and allocates
// 0x1EE84 bytes; the constructor is the main program's (0x020460B0, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x0214BA70). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_travel_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/SceneTestNg116_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg116_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg116Scene : public Scene {
public:
    TestNg116Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x8];
    /* 0x00018 */ SceneCharacter mCharacters[2];
    /* 0x05648 */ u8 mUnknown05648[0x1C5C];
    /* 0x072A4 */ TtsPlayer mTtsPlayer;
    /* 0x1BBE8 */ u8 mUnknown1BBE8[0x360];
    /* 0x1BF48 */ SpriteButton mButtons[1];
    /* 0x1C03C */ TextButton mTextButtons[4];
    /* 0x1DBFC */ MessageFile mMessageFiles[1];
    /* 0x1DC14 */ u8 mUnknown1DC14[0x8];
    /* 0x1DC1C */ MessageText mTexts[1];
    /* 0x1E20C */ u8 mUnknown1E20C[0x98];
    /* 0x1E2A4 */ MessageText mText;
    /* 0x1E894 */ MessageText mTexts1E894[1];
}; // 0x1EE84

#endif // OV039_TESTNG116SCENE_H
