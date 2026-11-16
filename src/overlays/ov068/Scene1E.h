#ifndef OV068_SCENE1E_H
#define OV068_SCENE1E_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "HeadIcon.h"
#include "item/Item.h"
#include "item/Mannequin.h"
#include "MessageWindow.h"
#include "Model.h"
#include "shop/Shop.h"
#include "shop/ShopWindow.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 68 (scene id 0x1E): a shop, with the shops' item list (ShopItemListView) and price window
// (ShopWindow), three hundred clothes items, a speech player, a character, models and texts. The overlay is not
// decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name
// of this class survives in the ROM.
// The shop's counter and what the constructor builds from its address with it: the mannequin, two models,
// particles and a head icon. The name is descriptive, not the original.
struct Scene1EPart {
    /* 0x00000 */ ShopCounter mCounter;
    /* 0x18FBC */ u8 mUnknown18FBC[8];
    /* 0x18FC4 */ Mannequin mMannequin;
    /* 0x1967C */ Model mModel1;
    /* 0x19730 */ Model mModel2;
    /* 0x197E4 */ ParticleSystem mParticles;
    /* 0x197E8 */ HeadIcon mHeadIcons[1];
    /* 0x19828 */ u8 mUnknown19828[0x40];
}; // 0x19868

class Scene1E : public Scene {
public:
    Scene1E(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x158];
    /* 0x00168 */ MessageFile mMessageFiles[4];
    /* 0x001C8 */ Scene1EPart mPart;
    /* 0x19A30 */ ClothesItem mItems[300];
    /* 0x1B650 */ u8 mUnknown1B650[0x96C];
    /* 0x1BFBC */ SpriteButton mButtons[5];
    /* 0x1C480 */ ShopItemListView mList;
    /* 0x30898 */ ShopWindow mShopWindow;
    /* 0x3BF60 */ MessageWindow mWindow;
    /* 0x46F04 */ SpriteButton mButtons2[2];
    /* 0x470EC */ TextButton mTextButtons[3];
    /* 0x485BC */ TextButton mTextButton;
    /* 0x48CAC */ SpriteButton mButton;
    /* 0x48DA0 */ ObjTextCanvas mCanvas;
}; // 0x48F58

#endif // OV068_SCENE1E_H
