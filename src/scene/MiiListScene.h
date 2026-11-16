#ifndef SCENE_MIILISTSCENE_H
#define SCENE_MIILISTSCENE_H

#include "MainProgram.h"
#include "list/MiiList.h"

// Scene 0x26 (main program, no overlay): the list of Miis with its sort menu. The list (main program 0x020a03e8
// to 0x020a3b00) shows the saved characters on five tabs, and a button at the top right of the touch screen opens
// a menu of the seven orders it can be sorted in (name, nickname, sex, year of birth, birthday, blood type,
// personality: etc messages 0x7F to 0x85). The order is kept in the game data (GameData::mMiiListSortKey) when it
// has changed. The scene factory (main SceneManager::CreateScene, case 38) allocates 0x2AA44 bytes and calls the constructor,
// which is in the main program's file of scene constructors (func_020403e8).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The classes of the
// rest of the main program are STAND-INS: their sizes and the fields this scene touches.

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
