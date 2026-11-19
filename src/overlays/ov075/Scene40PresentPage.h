#ifndef OV075_SCENE40PRESENTPAGE_H
#define OV075_SCENE40PRESENTPAGE_H

#include <nitro.h>

#include "Model.h"
#include "Sprite.h"
#include "Scene40Context.h"

class Scene40Character;
class Scene40CharacterTouch;
class Scene40ItemModel;

// The present page (pages+0x1E1C, the controller's page 3): the character offers three boxes (the scene's three
// models), each with a goods item of one group (ChooseItemInGroup(1, 4, box)); the player touches one, it opens,
// and the item is the player's (Item::Add).
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40PresentPage {
public:
    enum State {
        STATE_NONE = 0,
        STATE_START = 1,
        STATE_SHOW_BOXES = 2,
        STATE_CHOOSE = 3,
        STATE_OPEN = 4,
        STATE_RISE = 5,
        STATE_GIVE = 6,
        STATE_END = 7,
        STATE_WAIT_MESSAGES = 8,
        STATE_DONE = 9,
    };

    void Init(Scene40Context *context, Scene40CharacterTouch *touch);
    void Update();
    void Start();
    void UpdateStart();
    void UpdateShowBoxes();
    void UpdateChoose();
    void UpdateOpen();
    void UpdateRise();
    void UpdateGive();
    void UpdateEnd();
    void UpdateWaitMessages();
    void Draw();

    /* 0x000 */ int mState;
    /* 0x004 */ int mNextState;
    /* 0x008 */ int mStep;
    /* 0x00C */ int mFrames;
    /* 0x010 */ int mWaitFrames;
    /* 0x014 */ BOOL mActive;
    /* 0x018 */ AnimModel *mBoxes[3];        // The scene's three models
    /* 0x024 */ fx32 mBoxTouchRadii[3];
    /* 0x030 */ fx32 mScale;                 // By the boxes' distance from the camera
    /* 0x034 */ int mChosenBox;
    /* 0x038 */ int mOpenedBox;
    /* 0x03C */ Scene40Character *mCharacter;
    /* 0x040 */ Scene40ItemModel *mItemModels[3]; // The items, one in each box
    /* 0x04C */ int mItems[3];
    /* 0x058 */ ScreenPoint mItemPoints[3];  // Where on the screen the items rise
    /* 0x070 */ Sprite mSprite;
    /* 0x11C */ Scene40CharacterTouch *mTouch;
    /* 0x120 */ Scene40Context *mContext;
    /* 0x124 */ BOOL mUnknown124;
}; // 0x128

#endif // OV075_SCENE40PRESENTPAGE_H
