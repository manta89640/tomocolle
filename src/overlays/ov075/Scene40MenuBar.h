#ifndef OV075_SCENE40MENUBAR_H
#define OV075_SCENE40MENUBAR_H

#include <nitro.h>

#include "Scene40Context.h"
#include "SpriteButton.h"
#include "voice/TtsPlayer.h"

class Scene40Character;

// The first button of the scene's menu: a sprite button that calls for attention. Once started, it waits two
// seconds and for the speech to end, then shows its calling sequence with a sound, sounds four more times a
// second apart and goes back to its sequence; touching it stops the call.
class Scene40CallButton : public SpriteButton {
public:
    enum CallState {
        CALL_NONE = 0,
        CALL_WAITING = 1, // Counting the two seconds before the call
        CALL_CALLING = 2, // Showing the calling sequence
    };

    void Reset();
    void SetSequences(u16 sequence, u16 pressedSequence, u16 callSequence);
    void StartCall();
    BOOL Update();
    void UpdateCall();
    void SetDisabled(BOOL disabled);

    /* 0x0F4 */ int mCallState;
    /* 0x0F8 */ int mCallFrames;
    /* 0x0FC */ u16 mCallSequence;
    /* 0x100 */ TtsPlayer *mTtsPlayer; // The scene's: the call waits for its speech to end
}; // 0x104

// A sprite that plays its animation once over a button as it is chosen, and hides when it ends
class Scene40ButtonFlash : public Sprite {
public:
    void Update();
    void Play();
}; // 0xAC

// The scene's menu, seven sprite buttons on the touch screen (0x0215D108 to 0x0215D818): five down the left edge, of
// which the first calls for attention, and two at the top right. The pages' controller updates it every frame and
// reads which button was touched and which pressed. A button is shown when what it offers is there (mAvailable).
class Scene40MenuBar {
public:
    enum {
        BUTTON_COUNT = 7,
    };

    void Init(Scene40Context *context);
    void Update();
    void SetEnabled(BOOL enabled);
    void Refresh();
    void UpdateAvailable();
    void Draw();
    void StartCall();
    void Choose(int button);
    void SetButtonVisible(int button, BOOL visible);
    void SetVisible(BOOL visible);
    void SetDisabled(BOOL disabled);
    void SetLeftDisabled(BOOL disabled);  // The five down the left edge
    void SetRightDisabled(BOOL disabled); // The two at the top right

    /* 0x000 */ int mUnknown000;
    /* 0x004 */ int mUnknown004;
    /* 0x008 */ int mTouchedButton; // -1, or the button the stylus came down on this frame
    /* 0x00C */ int mPressedButton; // -1, or the button pressed this frame
    /* 0x010 */ BOOL mEnabled;      // The buttons take touches only while it is set
    // Whether each button is shown: not in the tutorial's scene (0x6C); any of the character's bits at 0x299;
    // more than one of their clothes; a motto; more than one of their interiors; the last two always
    /* 0x014 */ BOOL mAvailable[BUTTON_COUNT];
    /* 0x030 */ Scene40Context *mContext;
    /* 0x034 */ Scene40Character *mCharacter;
    /* 0x038 */ Scene40CallButton mCallButton;          // The first button
    /* 0x13C */ SpriteButton mButtons[BUTTON_COUNT];   // The others, by their numbers (the first is unused)
    /* 0x7E8 */ SpriteButton *mButtonList[BUTTON_COUNT]; // All seven
    /* 0x804 */ Scene40ButtonFlash mFlash;
}; // 0x8B0

#endif // OV075_SCENE40MENUBAR_H
