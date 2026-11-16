#ifndef OV085_SCENE68_H
#define OV085_SCENE68_H

#include <nitro.h>

#include "G3Sprite.h"
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

// Overlay 85, scenes 0x68 and 0x71 (not decompiled): the food shop ("Mdl/Misc/Shp/shop_food_A_3D_LZ.bin" to "D",
// "Mdl/Spl/SplShopFoodEffect_LZ.bin"). The scene factory (CreateScene) loads overlay 85 and allocates 0x47854 bytes;
// the constructor is the main program's (0x02045720, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table. The members are as the constructor constructs them, by their classes; what lies between is not
// known.
//
// The names are the scene id's: the shops share "Scn/Scene/SceneShop_LZ.bin", and no name of these classes survives
// in the ROM.

// The shop's counter and what the constructor builds from its address with it: an item model, a 3D sprite, a
// model, particles and two head icons. The name is descriptive, not the original.
struct Scene68Part {
    /* 0x00000 */ ShopCounter mCounter;
    /* 0x18FBC */ ItemModel mItemModel;
    /* 0x190E8 */ u8 mUnknown190E8[0x38];
    /* 0x19120 */ G3Sprite mSprite;
    /* 0x1916C */ Model mModel;
    /* 0x19220 */ ParticleSystem mParticles;
    /* 0x19224 */ HeadIcon mHeadIcons[2];
    /* 0x192A4 */ u8 mUnknown192A4[0x3C];
}; // 0x192E0

class Scene68 : public Scene {
public:
    Scene68(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x158];
    /* 0x00168 */ MessageFile mMessageFiles[4];
    /* 0x001C8 */ Scene68Part mPart;
    /* 0x194A8 */ Inventory mInventory;
    /* 0x1AC94 */ u8 mUnknown1AC94[0x408];
    /* 0x1B09C */ SpriteButton mButtons[4];
    /* 0x1B46C */ ShopItemListView mList;
    /* 0x2F884 */ ShopWindow mShopWindow;
    /* 0x3AF4C */ MessageWindow mWindow;
    /* 0x45EF0 */ SpriteButton mButtons2[2];
    /* 0x460D8 */ TextButton mTextButtons[2];
    /* 0x46EB8 */ TextButton mTextButton;
    /* 0x475A8 */ SpriteButton mButton;
    /* 0x4769C */ ObjTextCanvas mCanvas;
}; // 0x47854

#endif // OV085_SCENE68_H
