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

class Scene40ListPage {
public:
    /* 0x0000 [0x32A94] */ u8 mUnknown32A94[0x1C];
    /* 0x001C [0x32AB0] */ Scene40Items mItems3;
    /* 0x0048 [0x32ADC] */ u8 mUnknown32ADC[0x24];
    /* 0x006C [0x32B00] */ ClothesList mClothesList2;
    /* 0x2140 [0x34BD4] */ u8 mUnknown34BD4[0x4B4];
    /* 0x25F4 [0x35088] */ Scene40ListView mList;
    /* 0x16A14 [0x494A8] */ Scene40ListTabs mListTabs;
    /* 0x16E00 [0x49894] */ Scene40GoodsTabs mGoodsTabs;
    /* 0x171D8 [0x49C6C] */ Scene40ClothesTabs mClothesTabs;
    /* 0x176A4 [0x4A138] */ TextButton mTextButton;
    /* 0x17D94 [0x4A828] */ TextButton mTextButton2;
    /* 0x18484 [0x4AF18] */ SpriteButton mButton;
    /* 0x18578 [0x4B00C] */ MessageText mText2;
    /* 0x18B68 [0x4B5FC] */ u8 mUnknown4B5FC[0xC];
}; // 0x18B74

#endif // OV075_SCENE40LISTPAGE_H
