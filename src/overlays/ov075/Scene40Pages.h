#ifndef OV075_SCENE40PAGES_H
#define OV075_SCENE40PAGES_H

// Included from Scene40.h, after the classes it holds.

#include "Scene40TalkPage.h"
#include "Scene40TouchPage.h"

// The scene's lower screen (scene+0x30B50 to scene+0x69D2C): ten pages, one shown at a time, and what they share.
// Init (0x0214CA84) initialises every page from the scene's context, Update (0x0214CC34) runs the current page,
// Draw (0x0214FC00) draws it. The scene switches the lower screen's graphics when a page asks for another layout
// (mRequestedLayout). Offsets are the object's, then the scene's in brackets. See scratchpad's ov075-design.md.
#include "Scene40ListPage.h"

class Scene40Pages {
public:
    // The pages (mPage), by their objects
    enum Page {
        PAGE_TOUCH = 0,  // mTouchPage
        PAGE_1 = 1,      // +0x180
        PAGE_2 = 2,      // +0x1D94
        PAGE_3 = 3,      // +0x1E1C
        PAGE_LIST_4 = 4, // mListPage, 4 to 7
        PAGE_LIST_5 = 5,
        PAGE_LIST_6 = 6,
        PAGE_LIST_7 = 7,
        PAGE_8 = 8,      // +0x1AAB8
        PAGE_9 = 9,      // +0x214B0
    };

    void Init(int *mainCharOffset, int *subCharOffset, Scene40Context *context);
    void Update();
    void UpdateTouchPage();
    void UpdatePage1();
    void UpdatePage2();
    void UpdatePage3();
    void UpdatePage4();
    void UpdatePage5();
    void UpdatePage6();
    void UpdatePage7();
    void UpdatePage8();
    void UpdatePage9();
    void Start(int entry);
    void SetActive(BOOL active);
    void HideMenu(BOOL hide);
    void Draw();

    /* 0x00000 [0x30B50] */ BOOL mWaitingForLayout; // The scene clears it when it has shown the layout asked for
    /* 0x00004 [0x30B54] */ BOOL mActive;           // Cleared while the scene changes the upper screen
    /* 0x00008 [0x30B58] */ int mRequestedLayout;   // 1 to 4 (Scene40::LoadLayout), or 0
    /* 0x0000C [0x30B5C] */ int mListFrame;         // Which frame the list's layout shows
    /* 0x00010 [0x30B60] */ int mKeyboardLayout;    // Which of five keyboards that layout shows
    /* 0x00014 [0x30B64] */ int mPage;
    /* 0x00018 [0x30B68] */ int mNextPage;
    /* 0x0001C [0x30B6C] */ Scene40Context *mContext;
    /* 0x00020 [0x30B70] */ Scene40Touch *mTracker;
    /* 0x00024 [0x30B74] */ Scene40Character *mCharacter;
    /* 0x00028 [0x30B78] */ Scene40SpeechWindow *mSpeechWindow;
    /* 0x0002C [0x30B7C] */ Scene40CharacterTouch mTouch;
    /* 0x000FC [0x30C4C] */ Scene40TouchPage mTouchPage;
    /* 0x00180 [0x30CD0] */ Scene40TalkPage mTalkPage;
    /* 0x01D94 [0x328E4] */ u8 mUnknown328E4[0x20];
    /* 0x01DB4 [0x32904] */ Scene40Items mItems2;
    /* 0x01DE0 [0x32930] */ u8 mUnknown32930[0x164];
    /* 0x01F44 [0x32A94] */ Scene40ListPage mListPage;
    /* 0x1AAB8 [0x4B608] */ u8 mUnknown4B608[0x14];
    /* 0x1AACC [0x4B61C] */ SoftKeyboard mKeyboard;
    /* 0x20AC4 [0x51614] */ ObjCharBox mCharBox;
    /* 0x20C80 [0x517D0] */ u8 mUnknown517D0[0x58];
    /* 0x20CD8 [0x51828] */ MessageText mText3;
    /* 0x212C8 [0x51E18] */ SpriteButton mButton2;
    /* 0x213BC [0x51F0C] */ SpriteButton mButton3;
    /* 0x214B0 [0x52000] */ u8 mUnknown52000[0x18];
    /* 0x214C8 [0x52018] */ Scene40Items mItems4;
    /* 0x214F4 [0x52044] */ u8 mUnknown52044[0x14];
    /* 0x21508 [0x52058] */ MessageText mText4;
    /* 0x21AF8 [0x52648] */ SpriteButton mButton4;
    /* 0x21BEC [0x5273C] */ Scene40Button mButtons4[4];
    /* 0x21FCC [0x52B1C] */ MessageText mTexts2[4];
    /* 0x2378C [0x542DC] */ Scene40Button mButtons5[6];
    /* 0x23D5C [0x548AC] */ MessageText mTexts3[6];
    /* 0x260FC [0x56C4C] */ Scene40Button mButtons6[8];
    /* 0x268BC [0x5740C] */ MessageText mTexts4[8];
    /* 0x2983C [0x5A38C] */ u8 mUnknown5A38C[0xC60];
    /* 0x2A49C [0x5AFEC] */ Scene40ProfilePage mProfilePage;
    /* 0x386B0 [0x69200] */ Scene40CounterDisplay mCounter;
    /* 0x3892C [0x6947C] */ Scene40MenuBar mMenuBar;
}; // 0x391DC

#endif // OV075_SCENE40PAGES_H
