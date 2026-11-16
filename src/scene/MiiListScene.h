#ifndef SCENE_MIILISTSCENE_H
#define SCENE_MIILISTSCENE_H

#include "MainProgram.h"
#include "list/MiiList.h"

// Scene 0x26 (main program, no overlay): the list of Miis with its sort menu. The list (main program 0x020a03e8
// to 0x020a3b00) shows the saved characters on five tabs, and a button at the top right of the touch screen opens
// a menu of the seven orders it can be sorted in (name, nickname, sex, year of birth, birthday, blood type,
// personality: etc messages 0x7F to 0x85). The order is kept in the game data (GameData::mMiiListSortKey) when it
// has changed. The scene factory (main func_0203c050, case 38) allocates 0x2AA44 bytes and calls the constructor,
// which is in the main program's file of scene constructors (func_020403e8).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The classes of the
// rest of the main program are STAND-INS: their sizes and the fields this scene touches.

extern "C" {
void func_020a03e8(MiiList *list, MiiListPanel *panel, MiiListTabs *tabs);
void func_020a0454(MiiList *list); // Updates it
void func_020a05b8(MiiList *list); // Draws it
void func_020a05dc(MiiList *list);
void func_020a123c(MiiList *list, int sortKey, BOOL unknown); // Sorts it

void func_020a1fe0(MiiListPanel *panel, int unknown);
void func_020a2104(MiiListPanel *panel, int plane, int unknown);
void func_020a2118(MiiListPanel *panel, int a, int b, int c, WindowPoint position, WindowPoint size);
void func_020a2280(MiiListPanel *panel, int plane, WindowPoint position, int *charOffset);
void func_020a258c(MiiListPanel *panel, int a, int b, int c, WindowPoint position, WindowPoint size);
void func_020a2644(MiiListPanel *panel, int a, int b, int c, WindowPoint position, WindowPoint size);
void func_020a26fc(MiiListPanel *panel, int a, int b, int c, int d, int e, int f, WindowPoint position,
                   ScreenRect rect);

void func_020a36ec(MiiListTabs *tabs);
// The animation sequences of a tab's buttons, released and pressed (data_020fbe28 has how many there are)
void func_020a371c(MiiListTabs *tabs, int tab, int a, const u8 *sequences, const u8 *pressedSequences);
void func_020a37cc(MiiListTabs *tabs);
}

// The scene.
class MiiListScene : public Scene {
public:
    MiiListScene(int sceneId);

    enum State {
        STATE_IDLE,      // The list, and the sort button
        STATE_SORT_MENU, // The sort menu
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void UpdateIdle();
    void UpdateSortMenu();

    /* 0x00010 */ int mState;
    /* 0x00014 */ int mCharOffset;         // The first character of the touch screen's sprites that is free
    /* 0x00018 */ u8 mSortKey;             // The order at the start: Exit saves the list's if it differs
    /* 0x0001C */ MessageFile mEtcMessages;
    /* 0x00034 */ SpriteButton mSortButton;
    /* 0x00128 */ MiiList mList;
    /* 0x114D0 */ MiiListPanel mPanel;
    /* 0x1A948 */ MiiListTabs mTabs;
    /* 0x1FAA0 */ MessageWindow mSortMenu;
}; // 0x2AA44

#endif // SCENE_MIILISTSCENE_H
