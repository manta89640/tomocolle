#ifndef OV044_TESTNG121SCENE_H
#define OV044_TESTNG121SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 44, scenes 0x82, 0x83, 0x84, 0x85, 0x86, 0x87 (not decompiled). The scene factory (CreateScene) loads
// overlay 44 and allocates 0x1ACF8 bytes; the constructor is the main program's (0x02046C48,
// src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214BD68). The members are as the
// constructor constructs them, by their classes; what lies between is not known. The scene's StartLoading mounts
// "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Scn/Scene/SceneTestNg121_LZ.bin", "Scn/Scene/SceneBG_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg121_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg121Scene : public Scene {
public:
    TestNg121Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ SavedCharacter mSavedCharacter;
    /* 0x049A4 */ u8 mUnknown049A4[0x94];
    /* 0x04A38 */ TtsPlayer mTtsPlayer;
    /* 0x1937C */ SpriteButton mButtons[1];
    /* 0x19470 */ u8 mUnknown19470[0xC];
    /* 0x1947C */ MessageFile mMessageFiles[1];
    /* 0x19494 */ u8 mUnknown19494[0x8];
    /* 0x1949C */ MessageText mTexts[1];
    /* 0x19A8C */ u8 mUnknown19A8C[0x98];
    /* 0x19B24 */ MessageText mText;
    /* 0x1A114 */ Room mRoom;
    /* 0x1AA1C */ u8 mUnknown1AA1C[0x2DC];
}; // 0x1ACF8

#endif // OV044_TESTNG121SCENE_H
