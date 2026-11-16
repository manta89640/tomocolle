#ifndef OV100_ALBUMSCENE_H
#define OV100_ALBUMSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "item/Item.h"
#include "item/Mannequin.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 100, scene 0xB4 (not decompiled). The scene factory (CreateScene) loads overlay 100 and allocates 0x202E8
// bytes; the constructor is the main program's (0x020492E0, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214EC48). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene mounts "Scn/Scene/SceneAlbum_LZ.bin", "Scn/List/List_AlbumList_LZ.bin", the mannequin's
// head "Mdl/Misc/headManikin_3D_LZ.bin" and the item, costume, room and explanation messages: a list of the goods,
// clothes and interiors that the player owns, shown on a mannequin, as a model or as a room.
//
// The names are after the scene's archive, "Scn/Scene/SceneAlbum_LZ.bin", which no other overlay mounts; they are not
// the original: no name of these classes survives in the ROM.
//
// The four classes below derive from the main program's without a virtual function of their own: each stores a
// table of its own after its base's constructor, emitted in SceneManager.cpp's data with the base's functions
// (0x02110ABC with 0x02110AE0 for the second base, 0x02110938, and 0x021109BC with 0x021109D8 for the item model). The item model's implicit
// D1 (0x02049600) follows the scene's constructor, its D0 (0x0204AEDC) the file's last function.
class AlbumItems : public InventoryItem, public ClothesItem {}; // 0x2C

class AlbumListView : public ItemListView {
public:
    AlbumListView() {}
}; // 0x13F64

class AlbumItemModel : public ItemModel {}; // 0x12C

class AlbumScene : public Scene {
public:
    AlbumScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x54];
    /* 0x00064 */ AlbumItems mItems;
    /* 0x00090 */ u8 mUnknown00090[0x4C8];
    /* 0x00558 */ AlbumListView mList;
    /* 0x144BC */ u8 mUnknown144BC[0x8];
    /* 0x144C4 */ ObjTextCanvas mCanvases[7];
    /* 0x150CC */ u8 mUnknown150CC[0x708];
    /* 0x157D4 */ SpriteButton mButtons[5];
    /* 0x15C98 */ u8 mUnknown15C98[0x8];
    /* 0x15CA0 */ SpriteButton mButtons2[4];
    /* 0x16070 */ u8 mUnknown16070[0x8];
    /* 0x16078 */ SpriteButton mButtons3[5];
    /* 0x1653C */ SpriteButton mButton;
    /* 0x16630 */ u8 mUnknown16630[0x8C];
    /* 0x166BC */ Mannequin mMannequin;
    /* 0x16D74 */ AlbumItemModel mItemModel;
    /* 0x16EA0 */ u8 mUnknown16EA0[0x4];
    /* 0x16EA4 */ Room mRoom;
    /* 0x177AC */ MessageText mText1;
    /* 0x17D9C */ MessageText mText2;
    /* 0x1838C */ MessageText mText3;
    /* 0x1897C */ MessageText mText4;
    /* 0x18F6C */ MessageText mText5;
    /* 0x1955C */ u8 mUnknown1955C[0x160];
    /* 0x196BC */ ClothesList mClothesList;
    /* 0x1B790 */ Inventory mInventory;
    /* 0x1CF7C */ InteriorList mInteriors;
    /* 0x1D6D0 */ MessageFile mMessageFiles[7];
    /* 0x1D778 */ u8 mUnknown1D778[0x50];
    /* 0x1D7C8 */ Character mCharacter;
    /* 0x202E0 */ u8 mUnknown202E0[0x8];
}; // 0x202E8

#endif // OV100_ALBUMSCENE_H
