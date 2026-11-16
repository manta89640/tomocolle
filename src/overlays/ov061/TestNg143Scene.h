#ifndef OV061_TESTNG143SCENE_H
#define OV061_TESTNG143SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 61, scene 0xB8 (not decompiled). The scene factory (CreateScene) loads overlay 61 and allocates 0x1A99C
// bytes; the constructor is the main program's (0x02049970, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A8C0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Scn/Scene/SceneTestNg143_LZ.bin",
// "Mdl/Misc/fureai_3D_LZ.bin", "Scn/Scene/SceneBG_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg143_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg143Scene : public Scene {
public:
    TestNg143Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C50];
    /* 0x04778 */ TtsPlayer mTtsPlayer;
    /* 0x190BC */ SpriteButton mButtons[1];
    /* 0x191B0 */ u8 mUnknown191B0[0xC];
    /* 0x191BC */ MessageFile mMessageFiles[1];
    /* 0x191D4 */ u8 mUnknown191D4[0x8];
    /* 0x191DC */ MessageText mTexts[1];
    /* 0x197CC */ u8 mUnknown197CC[0x98];
    /* 0x19864 */ MessageText mText;
    /* 0x19E54 */ Room mRoom;
    /* 0x1A75C */ u8 mUnknown1A75C[0x160];
    /* 0x1A8BC */ AnimModel mModels[1];
}; // 0x1A99C

#endif // OV061_TESTNG143SCENE_H
