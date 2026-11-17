#ifndef OV075_SCENE40LISTPAGE_H
#define OV075_SCENE40LISTPAGE_H

// Included from Scene40Pages.h, after the classes it holds.

// The tabs over the list page's list: one for each of the four lists (goods of kind 1, goods of kind 3, clothes,
// interiors), each shown when the player has some of it or the island has its shop
class Scene40ListTabs {
public:
    enum {
        TAB_COUNT = 4,
    };

    void Init(int resource);
    void UpdateAvailable(Scene40Context *context);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int tab);

    /* 0x000 */ int mUnknown000;
    /* 0x004 */ u32 mPressed;  // A bit for the tab pressed this frame (1 << tab), 0 for none
    /* 0x008 */ int mSelected;
    /* 0x00C */ SpriteButton mTabs[TAB_COUNT];
    /* 0x3DC */ BOOL mAvailable[TAB_COUNT];
}; // 0x3EC

// The tabs over the list of goods of kind 1: each a filter of the goods' flags (Inventory::GetOwned)
class Scene40GoodsTabs {
public:
    enum {
        TAB_COUNT = 4,
    };

    void Init(int resource);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int tab);
    u32 GetFilter();
    static int FindTab(u32 filter);

    /* 0x000 */ int mPressed; // The tab pressed this frame, -1 for none
    /* 0x004 */ int mSelected;
    /* 0x008 */ SpriteButton mTabs[TAB_COUNT];
}; // 0x3D8

// The tabs over the list of clothes: each a kind of clothes (ClothesList::GetOwned)
class Scene40ClothesTabs {
public:
    enum {
        TAB_COUNT = 5,
    };

    void Init(int resource);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int tab);
    int GetKind();
    static int FindTab(int kind);

    /* 0x000 */ int mPressed; // The tab pressed this frame, -1 for none
    /* 0x004 */ int mSelected;
    /* 0x008 */ SpriteButton mTabs[TAB_COUNT];
}; // 0x4CC

// The page with a list of what the player owns (pages+0x1F44, the controller's pages 4 to 7): the goods, clothes and
// interiors to give the character, under the four tabs, or the character's own clothes to dress it in. A goods item
// is shown on an item model, clothes on the mannequin. A button takes the player to the shop of the tab's list.
class Scene40ListPage {
public:
    // mMode: what the list is for (the controller's page less 4)
    enum Mode {
        MODE_GIVE = 0,            // Something to give the character
        MODE_GIVE_FROM_SHOP = 1,  // The same, back from a shop: the item bought (gSceneScratch) is chosen
        MODE_DRESS = 2,           // Clothes of the character's to dress it in (its state 0x55 then)
        MODE_DRESS_2 = 3,         // The same, its state 0x9A then
    };
    // mState, mNextState
    enum State {
        STATE_NONE = 0,
        STATE_OPEN = 1,
        STATE_CHOOSE = 2,
        STATE_CHOSEN = 3, // The controller takes the result and closes the page
        STATE_CLOSE = 4,
    };

    void Init(Scene40Context *context, int *charOffset);
    void Update();
    void Open();
    void Choose();
    void Close();
    void Draw();
    void ShowList(int tab, int filterTab);
    void ShowItem(u16 id);
    void DressMannequin(u16 clothes);
    void PlaceMannequin();
    void PlaceItemModel();
    void GetGift(Scene40Gift *gift);

    /* 0x0000 [0x32A94] */ int mState;
    /* 0x0004 [0x32A98] */ int mNextState;
    /* 0x0008 [0x32A9C] */ int mMode;
    /* 0x000C [0x32AA0] */ int mTab;     // The list shown first
    /* 0x0010 [0x32AA4] */ BOOL mActive; // The page takes touches (the controller's SetActive)
    /* 0x0014 [0x32AA8] */ int mChosenId;
    /* 0x0018 [0x32AAC] */ BOOL mChosen; // The page closed with an item chosen, not with its close button
    /* 0x001C [0x32AB0] */ Scene40Gift mGift; // What the chosen item would be as a present
    /* 0x0054 [0x32AE8] */ BOOL mSortGoods;     // Sort the lists again (an item was given away) when the page opens
    /* 0x0058 [0x32AEC] */ BOOL mSortClothes;
    /* 0x005C [0x32AF0] */ BOOL mSortInteriors;
    /* 0x0060 [0x32AF4] */ ClothesList *mClothesList;
    /* 0x0064 [0x32AF8] */ Inventory *mInventory;
    /* 0x0068 [0x32AFC] */ InteriorList *mInteriors;
    /* 0x006C [0x32B00] */ Scene40ClothesList mOwnClothes; // The character's own clothes (MODE_DRESS)
    /* 0x2140 [0x34BD4] */ Item *mItems[CLOTHES_SIZE];  // What the list shows
    /* 0x25F0 [0x35084] */ int mItemCount;
    /* 0x25F4 [0x35088] */ Scene40ListView mList;
    /* 0x16A14 [0x494A8] */ Scene40ListTabs mListTabs;
    /* 0x16E00 [0x49894] */ Scene40GoodsTabs mGoodsTabs;
    /* 0x171D8 [0x49C6C] */ Scene40ClothesTabs mClothesTabs;
    /* 0x176A4 [0x4A138] */ TextButton mOkButton;   // Gives or dresses (its message 476, 479 for clothes)
    /* 0x17D94 [0x4A828] */ TextButton mShopButton; // Shown while the island has the tab's shop
    /* 0x18484 [0x4AF18] */ SpriteButton mCloseButton;
    /* 0x18578 [0x4B00C] */ MessageText mTitle;
    /* 0x18B68 [0x4B5FC] */ Mannequin *mMannequin;
    /* 0x18B6C [0x4B600] */ Scene40ItemModel *mItemModel; // One of the scene's three, while a goods item is shown
    /* 0x18B70 [0x4B604] */ Scene40Context *mContext;
}; // 0x18B74

#endif // OV075_SCENE40LISTPAGE_H
