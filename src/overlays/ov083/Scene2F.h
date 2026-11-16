#ifndef OV083_SCENE2F_H
#define OV083_SCENE2F_H

#include <nitro.h>

#include "HeadIcon.h"
#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "item/Item.h"
#include "shop/Shop.h"
#include "shop/ShopWindow.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 83, scene 0x2F (not decompiled): the goods shop ("Mdl/Misc/Shp/shop_item_3D_LZ.bin",
// "Mdl/Spl/SplShopItemEffect_LZ.bin"). The scene factory (CreateScene) loads overlay 83 and allocates 0x474B0 bytes;
// the constructor is the main program's (0x02041124, src/scene/SceneManager.cpp), which stores the overlay's virtual
// table. The members are as the constructor constructs them, by their classes; what lies between is not known.
//
// The names are the scene id's: the shops share "Scn/Scene/SceneShop_LZ.bin", and no name of these classes survives
// in the ROM.

// The shop's counter and what the constructor builds from its address with it: an item model, two models, particles
// and two head icons. The name is descriptive, not the original.
struct Scene2FPart {
    /* 0x00000 */ ShopCounter mCounter;
    /* 0x18FBC */ ItemModel mItemModel;
    /* 0x190E8 */ u8 mUnknown190E8[0x4];
    /* 0x190EC */ Model mModel1;
    /* 0x191A0 */ Model mModel2;
    /* 0x19254 */ ParticleSystem mParticles;
    /* 0x19258 */ HeadIcon mHeadIcons[2];
    /* 0x192D8 */ u8 mUnknown192D8[0x3C];
}; // 0x19314

class Scene2F : public Scene {
public:
    Scene2F(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x158];
    /* 0x00168 */ MessageFile mMessageFiles[4];
    /* 0x001C8 */ Scene2FPart mPart;
    /* 0x194DC */ Inventory mInventory;
    /* 0x1ACC8 */ u8 mUnknown1ACC8[0x400];
    /* 0x1B0C8 */ ShopItemListView mList;
    /* 0x2F4E0 */ ShopWindow mShopWindow;
    /* 0x3ABA8 */ MessageWindow mWindow;
    /* 0x45B4C */ SpriteButton mButtons[2];
    /* 0x45D34 */ TextButton mTextButtons[2];
    /* 0x46B14 */ TextButton mTextButton;
    /* 0x47204 */ SpriteButton mButton;
    /* 0x472F8 */ ObjTextCanvas mCanvas;
}; // 0x474B0

#endif // OV083_SCENE2F_H
