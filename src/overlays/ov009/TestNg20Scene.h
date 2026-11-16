#ifndef OV009_TESTNG20SCENE_H
#define OV009_TESTNG20SCENE_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 9 (scene id 0x1C), which mounts Scn/Scene/SceneTestNg20_LZ.bin and NgCaption_LZ.bin: a
// speech player, message texts, a button and a character. The overlay is not decompiled: this is the layout its
// constructor (in SceneManager.cpp) shows. The name is after its archive.
class TestNg20Scene : public Scene {
public:
    TestNg20Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x1A0];
    /* 0x001B0 */ TtsPlayer mTtsPlayer;
    /* 0x14AF4 */ u8 mUnknown14AF4[0x98];
    /* 0x14B8C */ SpriteButton mButtons[1];
    /* 0x14C80 */ u8 mUnknown14C80[0xC];
    /* 0x14C8C */ MessageFile mMessageFiles[1];
    /* 0x14CA4 */ u8 mUnknown14CA4[8];
    /* 0x14CAC */ MessageText mTexts[1];
    /* 0x1529C */ u8 mUnknown1529C[0x98];
    /* 0x15334 */ MessageText mText;
    /* 0x15924 */ SceneCharacter mCharacters[1];
    /* 0x1843C */ u8 mUnknown1843C[0x1BBC];
}; // 0x19FF8

#endif // OV009_TESTNG20SCENE_H
