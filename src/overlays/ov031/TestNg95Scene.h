#ifndef OV031_TESTNG95SCENE_H
#define OV031_TESTNG95SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 31, scene 0x53 (not decompiled). The scene factory (CreateScene) loads overlay 31 and allocates 0x277C0
// bytes; the constructor is the main program's (0x02044330, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BA10). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Msg/TmdItemMsgJP_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin",
// "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Scn/Scene/SceneTestNg95_LZ.bin",
// "Mdl/Misc/silhouette_3D_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg95_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg95Scene : public Scene {
public:
    TestNg95Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ MessageFile mMessageFiles[2];
    /* 0x00040 */ u8 mUnknown00040[0x84];
    /* 0x000C4 */ TtsPlayer mTtsPlayer;
    /* 0x14A08 */ u8 mUnknown14A08[0x24];
    /* 0x14A2C */ SceneCharacter mCharacters[1];
    /* 0x17544 */ u8 mUnknown17544[0x1BBC];
    /* 0x19100 */ ItemModel mItemModels[1];
    /* 0x1922C */ u8 mUnknown1922C[0xC];
    /* 0x19238 */ AnimModel mModel;
    /* 0x19318 */ SpriteButton mButtons[1];
    /* 0x1940C */ u8 mUnknown1940C[0x14];
    /* 0x19420 */ MessageText mTexts[1];
    /* 0x19A10 */ u8 mUnknown19A10[0x98];
    /* 0x19AA8 */ MessageText mText;
    /* 0x1A098 */ Room mRoom;
    /* 0x1A9A0 */ u8 mUnknown1A9A0[0x2B8];
    /* 0x1AC58 */ TextButton mTextButtons[4];
    /* 0x1C818 */ u8 mUnknown1C818[0x4];
    /* 0x1C81C */ MessageWindow mWindow;
}; // 0x277C0

#endif // OV031_TESTNG95SCENE_H
