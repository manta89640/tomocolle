#ifndef OV059_TESTNG139SCENE_H
#define OV059_TESTNG139SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 59, scene 0xAA (not decompiled). The scene factory (CreateScene) loads overlay 59 and allocates 0x1D3A8
// bytes; the constructor is the main program's (0x0204846C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BDE8). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Mdl/Misc/frying_pan_3D_LZ.bin", "Scn/Scene/SceneTestNg139_LZ.bin", "Mdl/Spl/SplNgEffect_LZ.bin",
// "Scn/Scene/SceneBG_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg139_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg139Scene : public Scene {
public:
    TestNg139Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C4C];
    /* 0x04774 */ TtsPlayer mTtsPlayer;
    /* 0x190B8 */ SpriteButton mButtons[1];
    /* 0x191AC */ u8 mUnknown191AC[0xC];
    /* 0x191B8 */ MessageFile mMessageFiles[2];
    /* 0x191E8 */ u8 mUnknown191E8[0x8];
    /* 0x191F0 */ MessageText mTexts[1];
    /* 0x197E0 */ u8 mUnknown197E0[0x98];
    /* 0x19878 */ MessageText mText;
    /* 0x19E68 */ u8 mUnknown19E68[0x328];
    /* 0x1A190 */ Room mRoom;
    /* 0x1AA98 */ u8 mUnknown1AA98[0x8];
    /* 0x1AAA0 */ AnimModel mModels[3];
    /* 0x1AD40 */ ItemModel mItemModels[1];
    /* 0x1AE6C */ u8 mUnknown1AE6C[0x6C];
    /* 0x1AED8 */ Inventory mInventory;
    /* 0x1C6C4 */ u8 mUnknown1C6C4[0xC0];
    /* 0x1C784 */ MessageText mTexts1C784[2];
    /* 0x1D364 */ u8 mUnknown1D364[0x44];
}; // 0x1D3A8

#endif // OV059_TESTNG139SCENE_H
