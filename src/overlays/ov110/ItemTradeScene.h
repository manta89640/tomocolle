#ifndef OV110_ITEMTRADESCENE_H
#define OV110_ITEMTRADESCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ScreenControl.h"
#include "character/Character.h"
#include "item/Item.h"
#include "item/Mannequin.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 110, scene 0xB1: wireless play, sending and receiving items ("たべもの・服・お宝など", etc message 0xF9).
// Overlay 109 (scene 0x29, the same archive "Scn/Scene/SceneTushin_LZ.bin") sends and receives Miis; this scene is
// the other branch of wireless play. Its classes are named with the prefix "ItemTrade": the program is linked as
// one, and overlay 109's classes have the plain names.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. What it uses of the
// main program that has no shared header is in MainProgram.h, beside it.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

struct Camera; // The main program's camera and lights (MainProgram.h)
struct Light;

// --- This overlay's classes ----------------------------------------------------------------------------------------

class ItemTradeItemModel;
class ItemTradeScene;

// What the scene gives its list: the player's things, the message files and the scene's objects (0x3C bytes at
// 0x1B1F0 of the scene).
struct ItemTradeContext {
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
    /* 0x28 */ ItemTradeItemModel *mItemModel;
    /* 0x2C */ Mannequin *mMannequin;
    /* 0x30 */ Room *mRoom;
    /* 0x34 */ MessageText *mNameText;
    /* 0x38 */ int *mSceneState;
}; // 0x3C

// What is chosen: the kind of thing, its number, and two values for it. Its first parts are an InventoryItem and a
// ClothesItem of classes the main program defines (virtual tables data_02110b0c and data_02110b30, which the
// scene's constructor stores).
// What is chosen to send: an item of either kind, as the main program's InventoryItem and ClothesItem together (two
// bases: the scene's constructor builds both, then stores this class's tables, which are SceneManager.cpp's,
// 0x02110B04 with the second at +0x2C)
class ItemTradeSelection : public InventoryItem, public ClothesItem {
public:
    void Set(int kind, int id);

    /* 0x00 InventoryItem, 0x14 ClothesItem */
    /* 0x2C */ int mKind; // 0: an item, 1: a costume, 2: a room interior, 3 and 4 not seen
    /* 0x30 */ int mCategory;
    /* 0x34 */ int mValue;
}; // 0x38

// The five tabs over the list: items, costumes, interiors, and two more kinds of items. Each is shown when there
// is something to show on it (mMasks).
class ItemTradeTabs {
public:
    // What mPressed becomes, by tab
    enum {
        TAB_ITEMS = 0x01,
        TAB_COSTUMES = 0x02,
        TAB_INTERIORS = 0x04,
        TAB_ITEMS_2 = 0x08,
        TAB_ITEMS_3 = 0x10,
    };

    void Init(int plane, MessageFile *file, int *charOffset);
    void SetMasks(Inventory *inventory, ClothesList *costumes, InteriorList *interiors);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int tab);
    void SetEnabled(int tabs);
    void OnOpen();

    /* 0x000 */ int mPressed;  // The tab pressed this frame, as TAB_*, or 0
    /* 0x004 */ int mSelected; // Its button
    /* 0x008 */ SpriteButton mButtons[5];
    /* 0x4CC */ int mMasks[5];
}; // 0x4E0

// The four buttons under the items tab, which choose the items it lists.
class ItemTradeItemFilter {
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

// The five buttons under the costumes tab, which choose the costumes it lists.
class ItemTradeCostumeFilter {
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

// The item model of the scene, which shows the item chosen.
class ItemTradeItemModel : public ItemModel {
public:
    // As the main program's Model has it (inline in the overlay's copy of the class)
    void SetVisible(BOOL visible) { mVisible = visible; }

    void Load(int resourceId);
    void Reset();
    void ChangeItem(int item);

    /* 0x12C */ int mUnknown12C;
}; // 0x130

// The list of the player's things on the lower screen, with its tabs and buttons, and what it shows of the thing
// chosen on the upper screen: an item's model, a costume on the mannequin, or a room interior.
class ItemTradeList {
public:
    enum State {
        STATE_IDLE,
        STATE_OPEN,
        STATE_RUN,
        STATE_CLOSE_REQUEST,
        STATE_CLOSE,
    };

    // What Run returns
    enum Result {
        RESULT_NONE,
        RESULT_OK,
        RESULT_BACK,
    };

    // The upper screen's views of the thing chosen
    enum Preview {
        PREVIEW_ITEM,
        PREVIEW_COSTUME,
        PREVIEW_ROOM,
    };

    void Init(ItemTradeContext *context, int *charOffset);
    int Update();
    void UpdateListMode();
    void Hide();
    void UpdatePreviews();
    void Open();
    int Run();
    void Close();
    void Draw();
    void ShowCostume(int costume);
    void LoadMannequinHead();
    void Save();
    void RemoveOne();
    void AddOne();
    BOOL Receive(int id);
    void RefreshList();
    void ShowName(int kind, int id);
    void SetRoomMode(int mode);
    void ShowRoom(u16 style);
    void UpdateRoom();
    void SetItemCamera();
    void SetRoomCamera();
    void ResetLights();
    void ResetListButtons();
    void UpdatePreviewStates();
    void ShowPending();

    /* 0x00000 */ int mLastState;
    /* 0x00004 */ int mState;
    /* 0x00008 */ int mMode;       // 0 and 1: what to send, all tabs; 2: costumes only
    /* 0x0000C */ int mChosenId;   // What OK chose
    /* 0x00010 */ BOOL mChosen;
    /* 0x00014 */ int mUnknown14;
    /* 0x00018 */ ItemTradeSelection mSelection;
    /* 0x00050 */ ClothesList *mCostumes;
    /* 0x00054 */ Inventory *mInventory;
    /* 0x00058 */ InteriorList *mInteriors;
    /* 0x0005C */ Item *mItems[300]; // What the list shows
    /* 0x0050C */ int mItemCount;
    /* 0x00510 */ ItemListView mList;
    /* 0x14474 */ ItemTradeTabs mTabs;
    /* 0x14954 */ ItemTradeItemFilter mItemFilter;
    /* 0x14D2C */ ItemTradeCostumeFilter mCostumeFilter;
    /* 0x151F8 */ TextButton mOkButton;
    /* 0x158E8 */ SpriteButton mBackButton;
    /* 0x159DC */ Mannequin *mMannequin;
    /* 0x159E0 */ ItemTradeItemModel *mItemModel;
    /* 0x159E4 */ Room *mRoom;
    /* 0x159E8 */ ItemTradeContext *mContext;
    /* 0x159EC */ u16 mRoomStyle;
    /* 0x159EE */ u16 mRoomStep;
    /* 0x159F0 */ u16 mRoomAngle;
    /* 0x159F4 */ int mRoomMode;
    /* 0x159F8 */ ScreenPoint mPreviewPositions[3];
    /* 0x15A10 */ int mPreviewOffsets[3];
    /* 0x15A1C */ int mPreviewStates[3];
    /* 0x15A28 */ int mPreviewTimers[3];
    /* 0x15A34 */ int mPendingId;    // What to show when the views are free again, or -1
    /* 0x15A38 */ int mPendingIndex; // Its place in the list, or -1
}; // 0x15A3C

// --- The scene -----------------------------------------------------------------------------------------------------

// What the scene shows of the save data's player character.
class ItemTradePlayer : public Character {
}; // 0x2B18

// Overlay 110, scene 0xB1: wireless play with items. One DS looks for a partner and the other is found; once they
// are connected, either can choose a thing from its list and send it. The scene factory (main CreateScene, case
// 177) loads overlay 110 and allocates 0x28E0C bytes; the constructor is the main program's (func_02048b78).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// list archive, "Scn/List/List_TushinItemList_LZ.bin".
class ItemTradeScene : public Scene {
public:
    ItemTradeScene(int sceneId); // 0x02048B78, the main program's (src/scene/SceneManager.cpp)

    // mState, by the etc message each shows
    enum State {
        STATE_CHOOSE,           // "Which one?": look for a partner, or be found
        STATE_RECRUITING,       // "Looking for a partner to come..." (the parent)
        STATE_SEARCHING,        // "Searching for a partner..." (the child)
        STATE_CHOOSE_PARTNER,   // "Partners found. Choose one"
        STATE_CONNECTING,       // "Connecting..."
        STATE_CONNECTED_PARENT, // "Connected with ..."
        STATE_CONNECTED_CHILD,
        STATE_LIST,             // The list: choose what to send (no window)
        STATE_ASK_SEND,         // "Send ...? All right?"
        STATE_SENDING,          // "Sending and saving..."
        STATE_RECEIVING,        // "Receiving..."
        STATE_SAVING,           // "Saving..."
        STATE_SENT,             // "Sent"
        STATE_PARTNER_FULL,     // "The partner has too many of it: it could not be sent"
        STATE_RECEIVED,         // "... has come"
        STATE_FULL,             // "There are too many of it: it could not be received"
        STATE_DISCONNECTED,     // "The connection was cut"
        STATE_ASK_DISCONNECT,   // "Disconnect?"
        STATE_WAIT,             // "Please wait..."
        STATE_ERROR,            // "An error occurred"
    };

    // What one DS sends the other besides an item (ItemTradeComm::mCommand)
    enum Command {
        COMMAND_ITEM,     // The item, four bytes
        COMMAND_1,
        COMMAND_SENDING,  // An item is about to come
        COMMAND_READY,    // Ready to receive it
        COMMAND_RECEIVED, // It came
        COMMAND_FULL,     // It came, but there are too many of it already
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetupQuestion(int *charOffset, MessageFile *file, u32 message);
    void SetupWindow(int state);
    void Init3d();
    void UpdateWireless();
    void UpdateMusic();
    void UpdateLinkIcon();
    void UpdateSend();
    void InitWirelessWhenIdle();
    void Send(int command);
    void SetEntry(BOOL entry);
    void ClearNames();
    void WriteSave();

    /* 0x00010 */ ItemTradeList mList;
    /* 0x15A4C */ Mannequin mMannequin;
    /* 0x16104 */ ItemTradeItemModel mItemModel;
    /* 0x16234 */ Room mRoom;
    /* 0x16B3C */ MessageText mNameText;
    /* 0x1712C */ int mNameCharOffset;
    /* 0x17130 */ int mListCharOffset;
    /* 0x17134 */ ClothesList mCostumes;
    /* 0x19208 */ Inventory mInventory;
    /* 0x1A9F4 */ InteriorList mInteriors;
    /* 0x1B148 */ MessageFile mMessages[7]; // Common, trouble, item, costume, room, etc and explain messages
    /* 0x1B1F0 */ ItemTradeContext mContext;
    /* 0x1B22C */ ItemTradePlayer mPlayer;
    /* 0x1DD44 */ Camera *mCamera;
    /* 0x1DD48 */ Sprite mLinkIcon;      // The strength of the link
    /* 0x1DDF4 */ MessageWindow mWindow;
    /* 0x28D98 */ int mWindowCharOffset;
    /* 0x28D9C */ int mState;
    /* 0x28DA0 */ int mNextState;
    /* 0x28DA4 */ int mScroll;           // Of the upper screen's background
    /* 0x28DA8 */ int mWait;
    /* 0x28DAC */ wchar_t mNames[3][11]; // Of the partners found
    /* 0x28DF0 */ BOOL mMusicPlaying;
    /* 0x28DF4 */ int mSendStep;      // 2: take the item away and save; 3: wait for the save; 1: send when ready
    /* 0x28DF8 */ BOOL mPartnerReady;  // COMMAND_READY came
    /* 0x28DFC */ BOOL mPartnerSending; // COMMAND_SENDING came
    /* 0x28E00 */ BOOL mIsParent;
    /* 0x28E04 */ BOOL mSaveAfterReturn; // The partner sent the item back: save once it is added again
    /* 0x28E08 */ BOOL mWindowDone;
}; // 0x28E0C

#endif // OV110_ITEMTRADESCENE_H
