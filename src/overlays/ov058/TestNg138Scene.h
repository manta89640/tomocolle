#ifndef OV058_TESTNG138SCENE_H
#define OV058_TESTNG138SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 58, scenes 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9 (not decompiled). The scene factory (CreateScene) loads
// overlay 58 and allocates 0x1DB00 bytes; the constructor is the main program's (0x0204830C,
// src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214B0DC). The members are as the
// constructor constructs them, by their classes; what lies between is not known. The scene's StartLoading mounts
// "Mdl/Ica/ica_mii_mini_contents2_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Mdl/Spl/SplNgEffect_LZ.bin", "Scn/Scene/SceneTestNg138_LZ.bin", "Scn/Scene/SceneBG_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg138_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg138Scene : public Scene {
public:
    TestNg138Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1C4C];
    /* 0x0728C */ TtsPlayer mTtsPlayer;
    /* 0x1BBD0 */ SpriteButton mButtons[1];
    /* 0x1BCC4 */ u8 mUnknown1BCC4[0xC];
    /* 0x1BCD0 */ MessageFile mMessageFiles[1];
    /* 0x1BCE8 */ u8 mUnknown1BCE8[0x8];
    /* 0x1BCF0 */ MessageText mTexts[1];
    /* 0x1C2E0 */ u8 mUnknown1C2E0[0x98];
    /* 0x1C378 */ MessageText mText;
    /* 0x1C968 */ Room mRoom;
    /* 0x1D270 */ u8 mUnknown1D270[0x8];
    /* 0x1D278 */ AnimModel mModel;
    /* 0x1D358 */ u8 mUnknown1D358[0xB8];
    /* 0x1D410 */ TextButton mTextButtons[1];
}; // 0x1DB00

#endif // OV058_TESTNG138SCENE_H
