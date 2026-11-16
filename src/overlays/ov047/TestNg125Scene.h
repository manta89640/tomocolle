#ifndef OV047_TESTNG125SCENE_H
#define OV047_TESTNG125SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 47, scene 0x8A (not decompiled). The scene factory (CreateScene) loads overlay 47 and allocates 0x1B8EC
// bytes; the constructor is the main program's (0x02047018, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214C080). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg125_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg125_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg125Scene : public Scene {
public:
    TestNg125Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x8];
    /* 0x00018 */ SceneCharacter mCharacters[1];
    /* 0x02B30 */ u8 mUnknown02B30[0x1C40];
    /* 0x04770 */ TtsPlayer mTtsPlayer;
    /* 0x190B4 */ u8 mUnknown190B4[0x8];
    /* 0x190BC */ MessageText mTexts[1];
    /* 0x196AC */ u8 mUnknown196AC[0x98];
    /* 0x19744 */ MessageText mText;
    /* 0x19D34 */ u8 mUnknown19D34[0x988];
    /* 0x1A6BC */ MessageFile mMessageFiles[1];
    /* 0x1A6D4 */ MessageText mTexts1A6D4[1];
    /* 0x1ACC4 */ SpriteButton mButtons[1];
    /* 0x1ADB8 */ u8 mUnknown1ADB8[0x224];
    /* 0x1AFDC */ Room mRoom;
    /* 0x1B8E4 */ u8 mUnknown1B8E4[0x8];
}; // 0x1B8EC

#endif // OV047_TESTNG125SCENE_H
