#ifndef OV056_TESTNG136SCENE_H
#define OV056_TESTNG136SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "shop/ShopWindow.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 56, scene 0x94 (not decompiled). The scene factory (CreateScene) loads overlay 56 and allocates 0x3570C
// bytes; the constructor is the main program's (0x02047DD0, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214C228). The members are as the constructor constructs them, by their classes; what lies between
// is not known.
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg136_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg136Scene : public Scene {
public:
    TestNg136Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1CD4];
    /* 0x07314 */ TtsPlayer mTtsPlayer;
    /* 0x1BC58 */ SpriteButton mButtons[1];
    /* 0x1BD4C */ u8 mUnknown1BD4C[0xC];
    /* 0x1BD58 */ MessageFile mMessageFiles[4];
    /* 0x1BDB8 */ u8 mUnknown1BDB8[0x8];
    /* 0x1BDC0 */ MessageText mTexts[1];
    /* 0x1C3B0 */ u8 mUnknown1C3B0[0x98];
    /* 0x1C448 */ MessageText mText;
    /* 0x1CA38 */ u8 mUnknown1CA38[0x94];
    /* 0x1CACC */ AnimModel mModel;
    /* 0x1CBAC */ AnimModel mModel1CBAC;
    /* 0x1CC8C */ u8 mUnknown1CC8C[0x14];
    /* 0x1CCA0 */ ItemModel mItemModels[4];
    /* 0x1D150 */ u8 mUnknown1D150[0x68];
    /* 0x1D1B8 */ Inventory mInventory;
    /* 0x1E9A4 */ u8 mUnknown1E9A4[0x4];
    /* 0x1E9A8 */ HalfPriceShopWindow mShopWindow;
    /* 0x2A070 */ MessageWindow mWindow2A070;
    /* 0x35014 */ u8 mUnknown35014[0x8];
    /* 0x3501C */ TextButton mTextButtons[1];
}; // 0x3570C

#endif // OV056_TESTNG136SCENE_H
