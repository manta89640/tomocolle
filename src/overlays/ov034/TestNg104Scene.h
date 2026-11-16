#ifndef OV034_TESTNG104SCENE_H
#define OV034_TESTNG104SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 34, scene 0x57 (not decompiled). The scene factory (CreateScene) loads overlay 34 and allocates 0x289E8
// bytes; the constructor is the main program's (0x02044804, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B140). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg104_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg104Scene : public Scene {
public:
    TestNg104Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1BBC];
    /* 0x071FC */ AnimModel mModels[2];
    /* 0x073BC */ u8 mUnknown073BC[0x94];
    /* 0x07450 */ TtsPlayer mTtsPlayer;
    /* 0x1BD94 */ MessageFile mMessageFiles[2];
    /* 0x1BDC4 */ u8 mUnknown1BDC4[0x8];
    /* 0x1BDCC */ MessageText mTexts[1];
    /* 0x1C3BC */ u8 mUnknown1C3BC[0x98];
    /* 0x1C454 */ MessageText mText;
    /* 0x1CA44 */ u8 mUnknown1CA44[0xF0C];
    /* 0x1D950 */ MessageWindow mWindow;
    /* 0x288F4 */ SpriteButton mButtons[1];
}; // 0x289E8

#endif // OV034_TESTNG104SCENE_H
