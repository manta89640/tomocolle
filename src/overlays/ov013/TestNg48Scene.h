#ifndef OV013_TESTNG48SCENE_H
#define OV013_TESTNG48SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 13, scene 0x36 (not decompiled). The scene factory (CreateScene) loads overlay 13 and allocates 0x1A29C
// bytes; the constructor is the main program's (0x02041DF8, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A4C4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg48_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg48_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg48Scene : public Scene {
public:
    TestNg48Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1DC0];
    /* 0x048E8 */ TtsPlayer mTtsPlayer;
    /* 0x1922C */ u8 mUnknown1922C[0x158];
    /* 0x19384 */ MessageFile mMessageFiles[1];
    /* 0x1939C */ u8 mUnknown1939C[0x8];
    /* 0x193A4 */ MessageText mTexts[1];
    /* 0x19994 */ u8 mUnknown19994[0x218];
    /* 0x19BAC */ MessageText mText;
    /* 0x1A19C */ SpriteButton mButtons[1];
    /* 0x1A290 */ u8 mUnknown1A290[0xC];
}; // 0x1A29C

#endif // OV013_TESTNG48SCENE_H
