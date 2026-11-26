#ifndef OV100_ALBUMSCENE_H
#define OV100_ALBUMSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "item/Item.h"
#include "item/Mannequin.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 100, scene 0xB4: the album. The scene factory (CreateScene) loads overlay 100 and allocates 0x202E8 bytes;
// the constructor is the main program's (0x020492E0, src/scene/SceneManager.cpp), which stores the overlay's virtual
// table (0x0214EC48). The scene mounts "Scn/Scene/SceneAlbum_LZ.bin", "Scn/List/List_AlbumList_LZ.bin", the
// mannequin's head "Mdl/Misc/headManikin_3D_LZ.bin" and the item, costume, room and explanation messages: a list of
// the goods, clothes and interiors under five tabs, with how many of the tab's there are and how many the player has
// had, and the one touched shown on the upper screen (on a mannequin, as a model or as a room) with its name, its
// explanation and its price. What the player has never had is listed under one message of its own.
//
// It is built as overlay 110's item trade (src/overlays/ov110) is: the same tabs, filters, preview models and room,
// with a list that draws a number beside each row instead of the counts.
//
// The names are after the scene's archive, "Scn/Scene/SceneAlbum_LZ.bin", which no other overlay mounts; they are not
// the original: no name of these classes survives in the ROM.
//
// The classes AlbumItems, AlbumListView and AlbumItemModel derive from the main program's without a virtual function
// of their own: each stores a table of its own after its base's constructor, emitted in SceneManager.cpp's data with
// the base's functions (0x02110ABC with 0x02110AE0 for the second base, 0x02110938, and 0x021109BC with 0x021109D8 for
// the item model). The item model's implicit D1 (0x02049600) follows the scene's constructor, its D0 (0x0204AEDC) the
// file's last function.

class AlbumItemModel;

// What is shown: the kind of thing and its number (AlbumList::mSelection)
class AlbumItems : public InventoryItem, public ClothesItem {
public:
    void Set(int kind, int id);

    /* 0x00 InventoryItem, 0x14 ClothesItem */
    /* 0x2C */ int mKind;  // 0: goods, 1: a costume, 2: an interior, 3 and 4 not seen
    /* 0x30 */ int mValue; // The goods' number, or a number by the kind (0xF7, 0xF9, 0xF4, 0xF5)
}; // 0x34

// The list: the shops' ItemListView with a number beside each row (its place in the list), and its own copies of
// the view's drawing, which put one message in place of the names of what was never had. Nothing of it is virtual
// (its table is ItemListView's functions): the functions below hide the view's or have names of their own.
class AlbumListView : public ItemListView {
public:
    AlbumListView() {}

    void Update();
    void SetMessages(MessageFile *file, u32 unknownMessage, BOOL zeroUnknown);
    void DrawNumberedRow(int row, int item);
    void DrawDirtyRows();
    void SetItems(Item **items, int count);
    void InitNumbers(int *charOffset);
    void DrawList();
    void PlaceRows();
    void SetNumbersVisible(BOOL visible);

    /* 0x13F64 */ u32 mUnknownMessage; // Shown in place of the name of what was never had
    /* 0x13F68 */ BOOL mZeroUnknown;   // Item 0 is shown so too
    /* 0x13F6C */ ObjTextCanvas mNumbers[7];
    /* 0x14B74 */ u8 mNumberBuffers[7][0x100];
}; // 0x15274

// The five tabs over the list: goods, costumes, interiors, and two more kinds of goods
class AlbumTabs {
public:
    // What mPressed becomes, by tab
    enum {
        TAB_GOODS = 0x01,
        TAB_COSTUMES = 0x02,
        TAB_INTERIORS = 0x04,
        TAB_GOODS_2 = 0x08,
        TAB_GOODS_3 = 0x10,
    };

    void Init(int plane, MessageFile *file, int *charOffset); // The file and the offset are not used
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int tab);
    void SetEnabled(int tabs);
    void OnOpen();

    /* 0x000 */ int mPressed;  // The tab pressed this frame, as TAB_*, or 0
    /* 0x004 */ int mSelected; // Its button
    /* 0x008 */ SpriteButton mButtons[5];
}; // 0x4CC

// The four buttons under the goods tab, which choose the goods it lists
class AlbumGoodsFilter {
public:
    void Init(int plane);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int button);
    int GetFilter();

    /* 0x000 */ int mPressed; // The button pressed this frame, or -1
    /* 0x004 */ int mSelected;
    /* 0x008 */ SpriteButton mButtons[4];
}; // 0x3D8

// The five buttons under the costumes tab, which choose the costumes it lists
class AlbumCostumeFilter {
public:
    void Init(int plane);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int button);
    int GetFilter();

    /* 0x000 */ int mPressed; // The button pressed this frame, or -1
    /* 0x004 */ int mSelected;
    /* 0x008 */ SpriteButton mButtons[5];
}; // 0x4CC

// The item model of the scene, which shows the goods chosen
class AlbumItemModel : public ItemModel {
public:
    void Load(int resourceId);
    void Reset();
    void ChangeItem(int item);

    /* 0x12C */ int mUnknown12C;
}; // 0x130

// What the scene gives its list: the player's things, the message files and the scene's objects (0x50 bytes at
// 0x1D778 of the scene)
struct AlbumContext {
    /* 0x00 */ ClothesList *mCostumes;
    /* 0x04 */ Inventory *mInventory;
    /* 0x08 */ InteriorList *mInteriors;
    /* 0x0C */ MessageFile *mCommonMessages;
    /* 0x10 */ MessageFile *mTroubleMessages;
    /* 0x14 */ MessageFile *mItemMessages;
    /* 0x18 */ MessageFile *mCostumeMessages;
    /* 0x1C */ MessageFile *mRoomMessages;
    /* 0x20 */ MessageFile *mEtcMessages;
    /* 0x24 */ Character *mPlayer;
    /* 0x28 */ AlbumItemModel *mItemModel;
    /* 0x2C */ Mannequin *mMannequin;
    /* 0x30 */ Room *mRoom;
    /* 0x34 */ MessageText *mExplainText; // The explanation of what is shown
    /* 0x38 */ MessageText *mNameText;    // Its name
    /* 0x3C */ MessageText *mPriceText;
    /* 0x40 */ MessageText *mCountText;   // How many things the tab has
    /* 0x44 */ MessageText *mOwnedText;   // How many of them the player has had
    /* 0x48 */ Sprite *mSprite1;
    /* 0x4C */ Sprite *mSprite2;
}; // 0x50

// The list of the lower screen with its tabs and buttons, and what it shows of the thing chosen on the upper screen
class AlbumList {
public:
    enum State {
        STATE_IDLE,
        STATE_OPEN,
        STATE_RUN,
        STATE_CLOSE_REQUEST,
        STATE_CLOSE,
    };

    // The upper screen's views of the thing chosen (mSelection.mKind)
    enum Preview {
        PREVIEW_ITEM,
        PREVIEW_COSTUME,
        PREVIEW_ROOM,
    };

    void Init(AlbumContext *context, int *charOffset);
    void SelectFirst();
    int Update();
    void UpdatePreviews();
    void Open();
    int Run();
    void Close();
    void Draw();
    void ShowCostume(int costume);
    void LoadMannequinHead();
    void ShowExplanation(int kind, int id);
    void ShowName(int kind, int id);
    void ShowUnknownName();
    void ShowPrice(int kind, int id);
    void ShowCounts(int tab);
    void SetRoomMode(int mode);
    void ShowRoom(u16 style);
    void StopRoom();
    void ResetLights();
    void UpdateRoom();
    void UpdateMannequin();

    /* 0x00000 */ int mLastState;
    /* 0x00004 */ int mState;
    /* 0x00008 */ int mMode;
    /* 0x0000C */ int mUnknown0C;
    /* 0x00010 */ int mUnknown10;
    /* 0x00014 */ AlbumItems mSelection;
    /* 0x00048 */ ClothesList *mCostumes;
    /* 0x0004C */ Inventory *mInventory;
    /* 0x00050 */ InteriorList *mInteriors;
    /* 0x00054 */ Item *mItems[300]; // What the list shows
    /* 0x00504 */ int mItemCount;
    /* 0x00508 */ AlbumListView mList;
    /* 0x1577C */ AlbumTabs mTabs;
    /* 0x15C48 */ AlbumGoodsFilter mGoodsFilter;
    /* 0x16020 */ AlbumCostumeFilter mCostumeFilter;
    /* 0x164EC */ SpriteButton mBackButton;
    /* 0x165E0 */ Mannequin *mMannequin;
    /* 0x165E4 */ AlbumItemModel *mItemModel;
    /* 0x165E8 */ Room *mRoom;
    /* 0x165EC */ AlbumContext *mContext;
    /* 0x165F0 */ u16 mRoomStyle;
    /* 0x165F2 */ u16 mRoomStep;
    /* 0x165F4 */ u16 mRoomAngle;
    /* 0x165F6 */ u16 mRoomTurn; // Added to mRoomAngle each frame: by the pad, to the left or the right
    /* 0x165F8 */ int mRoomMode;
    /* 0x165FC */ ScreenPoint mUnknown165FC[3];
    /* 0x16614 */ int mUnknown16614[3];
    /* 0x16620 */ int mUnknown16620[3];
    /* 0x1662C */ int mUnknown1662C;
    /* 0x16630 */ int mUnknown16630;
    /* 0x16634 */ int mTab;            // The tab shown, 0 to 4, as ShowCounts takes it
    /* 0x16638 */ BOOL mChosen;        // Something was chosen since the scene last looked
    /* 0x1663C */ BOOL mResetRoomFlags;
    /* 0x16640 */ int mCounts[5];      // By tab: how many things there are
    /* 0x16654 */ int mOwnedCounts[5]; // and how many of them the player has had
    /* 0x16668 */ BOOL mSelectFirst;   // Show the list's first thing
}; // 0x1666C

class AlbumScene : public Scene {
public:
    AlbumScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void Init3d();

    /* 0x00010 */ void *mBgChars[5]; // The upper screen's background of each tab
    /* 0x00024 */ void *mBgScreens[5];
    /* 0x00038 */ void *mBgPalettes[5];
    /* 0x0004C */ int mUnknown4C;
    /* 0x00050 */ AlbumList mList;
    /* 0x166BC */ Mannequin mMannequin;
    /* 0x16D74 */ AlbumItemModel mItemModel;
    /* 0x16EA4 */ Room mRoom;
    /* 0x177AC */ MessageText mExplainText;
    /* 0x17D9C */ MessageText mNameText;
    /* 0x1838C */ MessageText mPriceText;
    /* 0x1897C */ MessageText mCountText;
    /* 0x18F6C */ MessageText mOwnedText;
    /* 0x1955C */ Sprite mSprite1;
    /* 0x19608 */ Sprite mSprite2;
    /* 0x196B4 */ int mTextCharOffset;
    /* 0x196B8 */ int mListCharOffset;
    /* 0x196BC */ ClothesList mCostumes;
    /* 0x1B790 */ Inventory mInventory;
    /* 0x1CF7C */ InteriorList mInteriors;
    /* 0x1D6D0 */ MessageFile mMessages[7]; // Common, trouble, item, costume, room, etc and explain messages
    /* 0x1D778 */ AlbumContext mContext;
    /* 0x1D7C8 */ Character mPlayer;
    /* 0x202E0 */ void *mCamera;
    /* 0x202E4 */ int mBgShown; // The tab whose background the upper screen shows
}; // 0x202E8

#endif // OV100_ALBUMSCENE_H
