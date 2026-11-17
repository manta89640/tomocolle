#ifndef OV075_SCENE40OPTIONPAGE_H
#define OV075_SCENE40OPTIONPAGE_H

#include <nitro.h>

#include "SpriteButton.h"
#include "text/MessageText.h"
#include "Scene40Character.h"
#include "Scene40Context.h"

// Page 9 of the pages (pages+0x214B0): three panels of options under a title (messages 137, 142, 156 of the scene's
// own file): four, six and eight buttons, each with a text and a sprite, shown by the panels' mAvailable. Choosing
// in the second panel makes a gift of kind 3 (its index), in the third one of kind 0 (an item of a table); another
// state goes to scene 0x38 with the character's interior marked owned. Not decompiled yet: the fields are those its
// functions and the controller show.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40OptionPage {
public:
    void Init(Scene40Context *context, int *charOffset); // 0x0215AF4C
    void Update();                                       // 0x0215B984
    void Draw();                                         // 0x0215BAC8
    void ShowPanel(int panel);                           // 0x0215C3B0
    void FindTrouble();                                  // 0x0215C6B4
    BOOL IsAnyAvailable();                               // 0x0215C7F0: any of the first panel's options

    /* 0x0000 */ int mState;
    /* 0x0004 */ int mNextState;
    /* 0x0008 */ int mFrames;
    /* 0x000C */ int mUnknown000C;
    /* 0x0010 */ int mResult;   // For the controller: what was chosen (0, 1, 3)
    /* 0x0014 */ Scene40Context *mContext;
    /* 0x0018 */ Scene40Gift mGift;
    /* 0x0050 */ int mNextPanel;
    /* 0x0054 */ BOOL mActive;
    /* 0x0058 */ MessageText mTitle;
    /* 0x0648 */ SpriteButton mBackButton;
    /* 0x073C */ Scene40Button mButtonsA[4];
    /* 0x0B1C */ MessageText mTextsA[4];
    /* 0x22DC */ Scene40Button mButtonsB[6];
    /* 0x28AC */ MessageText mTextsB[6];
    /* 0x4C4C */ Scene40Button mButtonsC[8];
    /* 0x540C */ MessageText mTextsC[8];
    /* 0x838C */ Sprite mSpritesA[4];
    /* 0x863C */ Sprite mSpritesB[6];
    /* 0x8A44 */ Sprite mSpritesC[8];
    /* 0x8FA4 */ BOOL mAvailableA[4];
    /* 0x8FB4 */ BOOL mAvailableB[6];
    /* 0x8FCC */ BOOL mAvailableC[8];
}; // 0x8FEC

#endif // OV075_SCENE40OPTIONPAGE_H
