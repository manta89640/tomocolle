#ifndef OV075_SCENE40TOUCHPAGE_H
#define OV075_SCENE40TOUCHPAGE_H

// Included from Scene40Pages.h.

class Scene40MenuBar;

// What the stylus does to the character (the pages' +0x2C, scene+0x30B7C): which of its parts it touched as it came
// down, as it moves and in all, and the way a stroke went (0x0214FCC8 to 0x021504E8). The touch itself comes from the
// scene's touch tracker (scene+0x69D2C).
class Scene40CharacterTouch {
public:
    // What a touch hit (a set of these, mPartAtPress and the others)
    enum Part {
        PART_FACE = 1 << 0, // CharacterBase::HitTestFace
        PART_HEAD = 1 << 1, // The body's joint 3
        PART_BODY = 1 << 2, // 0x02173F04
        PART_NONE = 1 << 3, // None of them
    };

    // Where the stylus is in the room, at three depths: on the plane through the character, then between the
    // camera's eye and target at 4:3 and at 1:2 of the way from the target
    struct Depth {
        /* 0x00 */ VecFx32 mOrigin;  // The plane's point (the character's position, for the first)
        /* 0x0C */ VecFx32 mPress;   // Where the stylus came down
        /* 0x18 */ VecFx32 mHold;    // Where it is
        /* 0x24 */ VecFx32 mRelease; // Where it went up
        /* 0x30 */ VecFx32 mFlick;   // Which way it was flicked, and how fast
    }; // 0x3C

    void Init(Scene40Touch *tracker, Scene40Character *character);
    void Update();
    void Clear();
    int HitTest(const ScreenPoint *point);

    /* 0x00 */ Scene40Touch *mTracker; // The scene's (scene+0x69D2C)
    /* 0x04 */ Scene40Character *mCharacter;
    /* 0x08 */ Depth mDepths[3];
    /* 0xBC */ int mPartAtPress;  // What the stylus came down on
    /* 0xC0 */ int mPart;         // What it is on
    /* 0xC4 */ int mPartsTouched; // All it has been on since it came down
    /* 0xC8 */ int mStroke;       // The way it moved, 1 to 8 by eighths of a turn, or 0
    /* 0xCC */ u8 mUnknownCC[0x4];
}; // 0xD0

// The page the scene starts on (page 0, the pages' +0xFC, scene+0x30C4C): the character in its room, touched and
// talked to, with the menu (0x02150930 to 0x02153508).
class Scene40TouchPage {
public:
    enum State {
        STATE_START = 1,          // By mEntry (UpdateStart)
        STATE_IDLE = 2,           // The menu, and the stylus on the character
        STATE_3 = 3,
        STATE_4 = 4,
        STATE_5 = 5,
        STATE_WAIT_IDLE = 6,      // Waits for the character's state 0, then the menu again
        STATE_7 = 7,
        STATE_8 = 8,
        STATE_WAIT_9 = 9,         // Waits for the character's state 17
        STATE_10 = 10,
        STATE_11 = 11,
        STATE_12 = 12,
        STATE_13 = 13,
        STATE_14 = 14,
        STATE_WAIT_MESSAGES = 15, // Waits for the scene's message chain, then its return state
        STATE_MESSAGES_0 = 16,    // Message chain 0 once the character is idle, then the idle state
        STATE_MESSAGES_4 = 17,    // The same with chain 4, after choosing the menu's first button
        STATE_DONE = 18,          // mResult is ready
    };

    void Init(Scene40Context *context, Scene40CharacterTouch *touch, Scene40MenuBar *menu);
    void Update();
    void StartCall();
    void SetMenuDisabled(BOOL disabled);
    void HideMenu(BOOL hide);
    void RefreshMenu();
    void UpdateMenu();
    void UpdateStart();
    void UpdateIdle();
    void Unused02151ab0();
    void Update3();
    void Update4();
    void Update5();
    void UpdateWaitIdle();
    void Update7();
    void Update8();
    void UpdateWait9();
    void Update10();
    void Update11();
    void Update12();
    void Update13();
    void Update14();
    void UpdateWaitMessages();
    void UpdateMessages0();
    void UpdateMessages4();
    void SetState(int state);
    void PlaceGift();
    BOOL IsCharacterMood0Or1();
    BOOL IsCharacterNotIn2Or24Or25();
    void PlayGame();
    void PlayGame3B();
    void PlayGame87();
    int FindPresentScene(int sceneId);
    void Draw();

    /* 0x00 */ int mLastState; // mState as Update began: the controller reads it (STATE_DONE: mResult is ready)
    /* 0x04 */ int mState;
    /* 0x08 */ int mStep;     // SetState clears these three
    /* 0x0C */ int mFrames;   // Frames in the state
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ int mResult;   // What the controller does next (UpdateTouchPage)
    /* 0x18 */ int mEntry;    // How the page starts (STATE_START): what the controller sets
    /* 0x1C */ BOOL mActive; // The pages' SetActive sets it
    /* 0x20 */ int mUnknown20;
    /* 0x24 */ BOOL mMenuHidden;
    /* 0x28 */ Scene40Items mItems;
    /* 0x54 */ u8 mUnknown54[0xC];
    /* 0x60 */ int mUnknown60;
    /* 0x64 */ int mUnknown64;
    /* 0x68 */ int mUnknown68;
    /* 0x6C */ u8 mUnknown6C[0x4];
    /* 0x70 */ Scene40MenuBar *mMenu;
    /* 0x74 */ Scene40Touch *mTracker;
    /* 0x78 */ Scene40CharacterTouch *mTouch;
    /* 0x7C */ Scene40Context *mContext;
    /* 0x80 */ Scene40Character *mCharacter;
}; // 0x84

#endif // OV075_SCENE40TOUCHPAGE_H
