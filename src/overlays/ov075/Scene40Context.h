#ifndef OV075_SCENE40CONTEXT_H
#define OV075_SCENE40CONTEXT_H

#include <nitro.h>

class AnimModel;
class ClothesList;
class HeadIcon;
class InteriorList;
class Inventory;
class IslandProgress;
class Mannequin;
class MessageFile;
class MessageText;
class MessageWindow;
class Room;
class Scene40Camera;
class Scene40Character;
class Scene40Clock;
class Scene40ItemModel;
class Scene40Lights;
class Scene40MessageSequence;
class Scene40Physics;
class Scene40SpeechWindow;
class Scene40Touch;
class Sprite;
class TtsPlayer;

// The scene's members that its pages use (scene+0x30AB0): Scene40::Init fills it with their addresses and gives it
// to the pages' Init (0x0214CA84), which give it on to each page. The messages are those Init opens, in its order.
struct Scene40Context {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ ClothesList *mClothesList;
    /* 0x08 */ Inventory *mInventory;
    /* 0x0C */ InteriorList *mInteriors;
    /* 0x10 */ int *mMoney;      // gGameData's capped counter, as the scene keeps it
    /* 0x14 */ u8 mUnknown14[0x8]; // Not set by Init
    /* 0x1C */ MessageFile *mCommonMessages;   // TCM:tmd_common_msg_JP.bmg
    /* 0x20 */ MessageFile *mTroubleMessages;  // TTM:tmd_trouble_msg_JP.bmg
    /* 0x24 */ MessageFile *mTalkMessages;     // TLM:tmd_talk_<personality>_msg_JP.bmg
    /* 0x28 */ MessageFile *mItemMessages;     // TIM:tmd_item_msg_JP.bmg
    /* 0x2C */ MessageFile *mCostumeMessages;  // TDM:tmd_costume_msg_JP.bmg
    /* 0x30 */ MessageFile *mRoomMessages;     // TRM:tmd_room_msg_JP.bmg
    /* 0x34 */ MessageFile *mMiiTouchMessages; // TEM:tmd_miitouch_msg_JP.bmg
    /* 0x38 */ Sprite *mSprite;                // scene+0x13D0
    /* 0x3C */ Scene40SpeechWindow *mSpeechWindow;
    /* 0x40 */ TtsPlayer *mTtsPlayer;
    /* 0x44 */ MessageText *mText;             // scene+0x15DC0
    /* 0x48 */ IslandProgress *mIslandProgress;
    /* 0x4C */ Scene40Clock *mClock;
    /* 0x50 */ Scene40Touch *mTouch;
    /* 0x54 */ Scene40Camera *mCamera;
    /* 0x58 */ Scene40Lights *mLights;
    /* 0x5C */ Scene40Physics *mPhysics;
    /* 0x60 */ Scene40Character *mCharacter;
    /* 0x64 */ Scene40ItemModel *mItemModels; // The scene's three
    /* 0x68 */ Mannequin *mMannequin;
    /* 0x6C */ Room *mRoom;
    /* 0x70 */ AnimModel *mModels[3];
    /* 0x7C */ HeadIcon *mHeadIcon;
    /* 0x80 */ MessageWindow *mWindows[2];
    /* 0x88 */ Scene40MessageSequence *mMessageSequence;
    /* 0x8C */ BOOL mMenuAvailable[5]; // The menu's first five buttons, as the touch page copies them from it
}; // 0xA0

#endif // OV075_SCENE40CONTEXT_H
