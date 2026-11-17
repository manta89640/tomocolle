#ifndef OV075_SCENE40GIFTPAGE_H
#define OV075_SCENE40GIFTPAGE_H

#include <nitro.h>

#include "Scene40Character.h"
#include "Scene40Context.h"

class Scene40CharacterTouch;
class Scene40CounterDisplay;
class Scene40ItemModel;
class Scene40ProfilePage;

// The gift page (pages+0x1D94, the controller's page 2): the character's reaction to what it was given or shown
// (mReaction, which the controller sets: the main state, 0x02154BE0, switches over its 30 kinds), with the item on
// one of the scene's item models, the character's satisfaction and trouble, the counter and the profile's meters.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40GiftPage {
public:
    enum State {
        STATE_NONE = 0,
        STATE_REACT = 1,         // 0x02154BE0
        STATE_HOLD_ITEM = 2,
        STATE_3 = 3,             // 0x02155788
        STATE_4 = 4,             // 0x02155AE0
        STATE_5 = 5,             // 0x021560AC
        STATE_6 = 6,             // 0x021565A0
        STATE_7 = 7,             // 0x021564D4
        STATE_8 = 8,             // 0x02156A50
        STATE_9 = 9,
        STATE_WAIT_MESSAGES = 10, // Holds the item while the message chain runs (chain 12)
        STATE_LEAVE_A = 11,       // Message chain 2, the character's state 0xD0, then scene 0x69
        STATE_HOLD_THEN_B = 12,   // Message chain 5, then state 2
        STATE_LEAVE_B = 13,       // Message chain 6, then scene 0x69
        STATE_RESET = 14,
    };

    void Init(Scene40Context *context, Scene40CharacterTouch *touch, Scene40ProfilePage *profile,
              Scene40CounterDisplay *counter);
    void Update();
    void UpdateReact();
    void UpdateHoldItem();
    void Update3();
    void Update4();
    void Update5();
    void Update6();
    void Update7();
    void Update8();
    void UpdateWaitMessages();
    void UpdateLeaveA();
    void UpdateHoldThenB();
    void UpdateLeaveB();
    void Draw();
    void SetLucky(BOOL lucky);
    void SetLucky(BOOL lucky, int chance);
    void SetValue(int value);
    void SetItem(int item, int value);
    BOOL SolveTrouble(Scene40Gift *gift, int count);
    void StartCharacterReaction();
    void PlayReactionMusic();
    void PlaceItemModel();

    /* 0x00 */ int mState;
    /* 0x04 */ int mNextState;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mStep;
    /* 0x14 */ int mReaction; // What the character reacts to, as the controller sets it
    /* 0x18 */ BOOL mActive;
    /* 0x1C */ Scene40ItemModel *mItemModel;
    /* 0x20 */ Scene40Gift mGift;
    /* 0x58 */ int mItem;   // -1 for none
    /* 0x5C */ int mValue;
    /* 0x60 */ BOOL mLucky; // Set by SetLucky with a chance in a thousand
    /* 0x64 */ BOOL mGotTarget; // The gift was the very item the trouble was about (its target)
    /* 0x68 */ int mUnknown68;
    /* 0x6C */ int mUnknown6C;
    /* 0x70 */ int mUnknown70;
    /* 0x74 */ Scene40CharacterTouch *mTouch;
    /* 0x78 */ Scene40ProfilePage *mProfilePage;
    /* 0x7C */ Scene40CounterDisplay *mCounter;
    /* 0x80 */ Scene40Context *mContext;
    /* 0x84 */ Scene40Character *mCharacter;
}; // 0x88

#endif // OV075_SCENE40GIFTPAGE_H
