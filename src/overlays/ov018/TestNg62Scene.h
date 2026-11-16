#ifndef OV018_TESTNG62SCENE_H
#define OV018_TESTNG62SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 18, scene 0x3D (not decompiled). The scene factory (CreateScene) loads overlay 18 and allocates 0x1CA20
// bytes; the constructor is the main program's (0x02042544, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A5C0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg62_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg62_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg62Scene : public Scene {
public:
    TestNg62Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1C58];
    /* 0x07298 */ TtsPlayer mTtsPlayer;
    /* 0x1BBDC */ u8 mUnknown1BBDC[0xAC];
    /* 0x1BC88 */ SpriteButton mButtons[1];
    /* 0x1BD7C */ u8 mUnknown1BD7C[0xC];
    /* 0x1BD88 */ MessageFile mMessageFiles[1];
    /* 0x1BDA0 */ u8 mUnknown1BDA0[0x8];
    /* 0x1BDA8 */ MessageText mTexts[1];
    /* 0x1C398 */ u8 mUnknown1C398[0x98];
    /* 0x1C430 */ MessageText mText;
}; // 0x1CA20

#endif // OV018_TESTNG62SCENE_H
