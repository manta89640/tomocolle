#ifndef OV075_SCENE40TALKPAGE_H
#define OV075_SCENE40TALKPAGE_H

#include <nitro.h>

#include "Scene40ChoiceList.h"
#include "Scene40Context.h"

class Scene40MenuBar;
class Scene40ProfilePage;

// The talk page (pages+0x180, the controller's page 1): the character says something and the player answers with
// the choice list. mTopic says what the talk is (Start, 0x02153644, switches on it); each state waits for an answer,
// acts on it, and ends with mResult for the controller (STATE_DONE).
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40TalkPage {
public:
    enum State {
        STATE_NONE = 0,
        STATE_START = 1,             // 0x02153644
        STATE_ANSWER = 2,            // 0x021538D4
        STATE_SECOND_ANSWER = 3,     // Kept for the next scene (its second answer byte)
        STATE_FIRST_OF_TWO = 4,      // Kept for the next scene, then the second question
        STATE_SECOND_OF_TWO = 5,
        STATE_YES_NO = 6,
        STATE_TROUBLE_ANSWER = 7,    // The answer ends the trouble and starts another
        STATE_WAIT_QUESTION = 8,     // Waits for the character's state 0xAC, then asks
        STATE_RELATION_ANSWER = 9,   // The answer changes the character's relation with the trouble's target
        STATE_SOLVE_OR_NOT = 10,
        STATE_PLAY_OR_NOT = 11,      // Goes to a game scene
        STATE_FOUR_ANSWERS = 12,
        STATE_WAIT_MESSAGES = 13,    // Waits for the scene's message chain
        STATE_WAIT_CHARACTER = 14,   // Waits for the character's state 0x33
        STATE_LEAVE_A = 15,          // Message chain 15, then scene 0x69
        STATE_LEAVE_B = 16,          // Message chain 16, then scene 0x69
        STATE_DONE = 17,
    };

    void Init(Scene40Context *context, int *charOffset, Scene40MenuBar *menuBar, Scene40ProfilePage *profilePage);
    void Update();
    void UpdateStart();
    void UpdateAnswer();
    void UpdateSecondAnswer();
    void UpdateFirstOfTwo();
    void UpdateSecondOfTwo();
    void UpdateYesNo();
    void UpdateTroubleAnswer();
    void UpdateWaitQuestion();
    void UpdateRelationAnswer();
    void UpdateSolveOrNot();
    void UpdatePlayOrNot();
    void UpdateFourAnswers();
    void UpdateWaitMessages();
    void UpdateWaitCharacter();
    void UpdateLeaveA();
    void UpdateLeaveB();
    void Draw();

    /* 0x0000 */ int mState;
    /* 0x0004 */ int mNextState;
    /* 0x0008 */ int mTopic;  // What the talk is about, as the controller sets it
    /* 0x000C */ int mResult; // For the controller when the page is done
    /* 0x0010 */ int mStep;
    /* 0x0014 */ int mChoice; // The last button pressed
    /* 0x0018 */ BOOL mActive;
    /* 0x001C */ Scene40ChoiceList mChoices;
    /* 0x1C04 */ Scene40Context *mContext;
    /* 0x1C08 */ Scene40MenuBar *mMenuBar;
    /* 0x1C0C */ Scene40ProfilePage *mProfilePage;
    /* 0x1C10 */ int mUnknown1C10;
}; // 0x1C14

#endif // OV075_SCENE40TALKPAGE_H
