#ifndef OV084_SCENE35_H
#define OV084_SCENE35_H

#include <nitro.h>

#include "G3Sprite.h"
#include "HeadIcon.h"
#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "shop/ShopWindow.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 84, scene 0x35 (not decompiled): the interior shop ("Mdl/Misc/Shp/shop_reform_3D_LZ.bin",
// "Mdl/Spl/SplShopRoomEffect_LZ.bin", the room messages). The scene factory (CreateScene) loads overlay 84 and
// allocates 0x46D4C bytes; the constructor is the main program's (0x02041A04, src/scene/SceneManager.cpp), which
// stores the overlay's virtual table (0x0214C2E4). The members are as the constructor constructs them, by their
// classes; what lies between is not known.
//
// The names are the scene id's: the shops share "Scn/Scene/SceneShop_LZ.bin", and no name of these classes survives
// in the ROM.

// The shop's counter and what the constructor builds from its address with it: two models, a 3D sprite, the room
// on show, particles and a head icon. The name is descriptive, not the original.
struct Scene35Part {
    /* 0x00000 */ ShopCounter mCounter;
    /* 0x18FBC */ u8 mUnknown18FBC[0x4];
    /* 0x18FC0 */ Model mModel1;
    /* 0x19074 */ Model mModel2;
    /* 0x19128 */ u8 mUnknown19128[0x34];
    /* 0x1915C */ G3Sprite mSprite;
    /* 0x191A8 */ u8 mUnknown191A8[0x8];
    /* 0x191B0 */ Room mRoom;
    /* 0x19AB8 */ u8 mUnknown19AB8[0x8];
    /* 0x19AC0 */ ParticleSystem mParticles;
    /* 0x19AC4 */ HeadIcon mHeadIcons[1];
    /* 0x19B04 */ u8 mUnknown19B04[0x40];
}; // 0x19B44

// The shop's list of interiors: the shops' ShopItemListView with its own DrawRow (the overlay's 0x0214B22C, table
// 0x0214C2C8). Its constructor is inline.
class Scene35ListView : public ShopItemListView {
public:
    Scene35ListView() {}

    virtual void DrawRow(int row, int item);
}; // 0x14418

class Scene35 : public Scene {
public:
    Scene35(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x10];
    /* 0x00020 */ MessageFile mMessageFiles[4];
    /* 0x00080 */ Scene35Part mPart;
    /* 0x19BC4 */ InteriorList mInteriors;
    /* 0x1A318 */ u8 mUnknown1A318[0x140];
    /* 0x1A458 */ Scene35ListView mList;
    /* 0x2E870 */ u8 mUnknown2E870[0x4];
    /* 0x2E874 */ ShopWindow mShopWindow;
    /* 0x39F3C */ MessageWindow mWindow;
    /* 0x44EE0 */ TextButton mTextButtons[3];
    /* 0x463B0 */ TextButton mTextButton;
    /* 0x46AA0 */ SpriteButton mButton;
    /* 0x46B94 */ ObjTextCanvas mCanvas;
}; // 0x46D4C

#endif // OV084_SCENE35_H
