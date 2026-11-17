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

    void Init(Scene40Touch *tracker, Scene40Character *character);
    void Update();
    void Clear();
    int HitTest(const ScreenPoint *point);

    /* 0x00 */ Scene40Touch *mTracker; // The scene's (scene+0x69D2C)
    /* 0x04 */ Scene40Character *mCharacter;
    /* 0x08 */ u8 mUnknown08[0xB4];
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
    void Init(Scene40Context *context, Scene40CharacterTouch *touch, Scene40MenuBar *menu);
    void Update();
    void StartCall();
    void SetMenuDisabled(BOOL disabled);
    void HideMenu(BOOL hide);
    void RefreshMenu();
    void UpdateMenu();
    void SetState(int state);
    void Draw();

    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mState;
    /* 0x08 */ int mUnknown08; // SetState clears these three
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ u8 mUnknown14[0x4];
    /* 0x18 */ int mUnknown18;
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
