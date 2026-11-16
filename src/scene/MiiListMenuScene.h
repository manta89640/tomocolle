#ifndef SCENE_MIILISTMENUSCENE_H
#define SCENE_MIILISTMENUSCENE_H

#include "MainProgram.h"

// Scene 0x24 (main program, no overlay): the two menus of the list of Miis, without the list. Two buttons at the
// top of the touch screen open them: the left one a menu of what to do with a Mii (edit it, change its clothes, or
// make it move out, which asks for confirmation and then says that it has gone), the right one a menu of the seven
// orders the list can be sorted in (name, nickname, sex, year of birth, birthday, blood type, personality). Nothing
// is done with the choices. The scene factory (main SceneManager::CreateScene, case 36) allocates 0x2C0C0 bytes and calls the
// constructor, which is in the main program's file of scene constructors (func_02040288).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class MiiListMenuScene : public Scene {
public:
    MiiListMenuScene(int sceneId);

    enum State {
        STATE_IDLE,         // Waits for one of the two buttons
        STATE_MII_MENU,     // The menu of what to do with a Mii (window 0)
        STATE_SORT_MENU,    // The menu of the orders (window 1)
        STATE_CONFIRM,      // "Move this Mii out?" (window 2)
        STATE_MOVED_OUT,    // "... has moved away" (window 3)
    };

    // What a window's mUnknownAF98 is set to when it closes: what the scene does next
    enum {
        NEXT_CONFIRM = 2, // Window 0: the Mii is to move out
        NEXT_IDLE = 3,    // Window 0: cancelled
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void UpdateIdle();
    void UpdateMiiMenu();
    void UpdateSortMenu();
    void UpdateConfirm();
    void UpdateMovedOut();

    /* 0x00010 */ int mState;
    /* 0x00014 */ int mCharOffset;           // The first character of the touch screen's sprites that is free
    /* 0x00018 */ SpriteButton mButtons[2];  // The Mii menu's and the sort menu's
    /* 0x00200 */ MessageFile mEtcMessages;
    /* 0x00218 */ MessageFile mCommonMessages;
    /* 0x00230 */ MessageWindow mWindows[4]; // The two menus, the confirmation and the farewell
}; // 0x2C0C0

#endif // SCENE_MIILISTMENUSCENE_H
