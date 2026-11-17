#ifndef OV075_SCENE40CHOICELIST_H
#define OV075_SCENE40CHOICELIST_H

#include <nitro.h>

#include "SpriteButton.h"
#include "text/MessageFile.h"

// A set of answers the player can choose from (this file's tables): how many, and their messages
struct Scene40ChoiceSet {
    /* 0x00 */ int mCount;
    /* 0x04 */ u32 mMessages[4];
}; // 0x14

// The talk page's answers (talk page + 0x1C): up to four text buttons in a column on the lower screen, the lowest
// at the same place whatever their number. It opens after a delay, reports the button pressed, and closes.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40ChoiceList {
public:
    enum State {
        STATE_HIDDEN,
        STATE_CHOOSING,
        STATE_OPENING, // Waits, then shows the buttons
        STATE_CLOSING, // Hides them after a while
    };

    void Init(int *charOffset, MessageFile *file);
    void Update();
    void Draw();
    void SetChoices(int set);
    void SetChoicesForTrouble(int category, int id);
    void SetSpecialChoices(int index); // Of another table, as set 23
    void Open();
    void Close();

    /* 0x0000 */ int mState;
    /* 0x0004 */ TextButton mButtons[4];
    /* 0x1BC4 */ int mTroubleKind;  // The kind of trouble whose answers SetChoicesForTrouble shows
    /* 0x1BC8 */ int mSet;          // The choice set shown
    /* 0x1BCC */ int mCount;        // How many buttons it shows
    /* 0x1BD0 */ int mPressed;      // The button pressed in this frame, -1 for none
    /* 0x1BD4 */ int mFinished;     // The button whose press played out in this frame, -1 for none
    /* 0x1BD8 */ BOOL mOpened;      // Set in the frame the buttons appear
    /* 0x1BDC */ BOOL mClosed;      // and in the frame the list is hidden
    /* 0x1BE0 */ int mAnswer;       // The button pressed, as the talk page keeps it until the list has closed
    /* 0x1BE4 */ int mTimer;        // Frames since Open or Close
}; // 0x1BE8

#endif // OV075_SCENE40CHOICELIST_H
