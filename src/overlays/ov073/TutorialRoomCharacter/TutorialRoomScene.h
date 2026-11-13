#ifndef OV073_TUTORIALROOMSCENE_H
#define OV073_TUTORIALROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "RoomCharacter.h"
#include "TimedMessage.h"

// The tutorial's apartment scene (overlay 73, "Scn/Scene/ScenePlayRoomTut_LZ.bin"): overlay 74's RoomScene cut
// down to one room and one button. This is the part of it that the functions 0x0214bbe0 to 0x0214c1ac use; the
// layout is the one the scene's constructor (main program, func_0203efc0) builds.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// The touch screen (overlay 74's prelude.h)
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
};

// STAND-INS for the scene's members that this file touches, whose classes are group b2's.

// The cursor on the building front (overlay 74's RoomCursor, the first member of ApartmentFront).
struct RoomCursor {
    BOOL Contains(ScreenPoint point); // 0x0214f4ac

    /* 0x00 */ u8 mUnknown00[0xB8];
}; // 0xB8

// The building on the touch screen (overlay 74's ApartmentFront).
struct ApartmentFront {
    /* 0x000 */ RoomCursor mCursor;
    /* 0x0B8 */ u8 mUnknown0B8[0x148];
}; // 0x200

// The tutorial's touch button (0x02150264 constructs it): 0xC8 bytes, without overlay 74's mKind.
struct TouchButton {
    BOOL Contains(ScreenPoint point); // 0x02150458

    /* 0x00 */ u8 mUnknown00[0xC0];
    /* 0xC0 */ int mState; // 1: pressed, 2: not pressed, 3: let go on the button (overlay 74's TouchButton::State)
    /* 0xC4 */ MessageText *mLabel;
}; // 0xC8

class TutorialRoomScene {
public:
    BOOL UpdateTouch();
    void UpdateEnding();
    void ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode);
    BOOL AreCharactersNear(int first, int second);
    void UpdateIdle();
    void SaveCharacter(SavedCharacter *character);
    void SaveCharacters();
    void LoadCharacter(SavedCharacter *character);
    void LoadCharacters();

    // Group b0's
    void SetState(int state); // 0x0214bbb4: sets mState and zeroes mStep and mTimer

    /* 0x00000 */ u8 mUnknown00000[0x10];     // The base class Scene
    /* 0x00010 */ int mState;                 // What the tutorial is at (group b0)
    /* 0x00014 */ int mFadeState;
    /* 0x00018 */ RoomCharacter mCharacters[2];
    /* 0x05740 */ ApartmentRoom mRoom;
    /* 0x06060 */ RoomCamera mCamera;
    /* 0x060BC */ u8 mLight[0x1C];            // RoomLight
    /* 0x060D8 */ ApartmentFront mFront;
    /* 0x062D8 */ u8 mUnknown062D8[0xBCF4];
    /* 0x11FCC */ TouchButton mButtons[1];
    /* 0x12094 */ MessageText mButtonLabel[1];
    /* 0x12684 */ u8 mClock[0x24];            // Clock
    /* 0x126A8 */ u8 mClockDisplay[0x78C];    // ClockDisplay
    /* 0x12E34 */ TimedMessage mMessage;
    /* 0x1342C */ MessageFile mMessageFiles[3];
    /* 0x13474 */ s16 mStep;                  // Step inside mState
    /* 0x13476 */ s16 mTimer;                 // Frames of the step
    /* 0x13478 */ s16 mFadeStep;
    /* 0x1347A */ s16 mTouchStep;             // 0: waiting for a touch, 1: the stylus is down on the button
    /* 0x1347C */ s16 mPressedButton;         // Which of mButtons the stylus went down on
    /* 0x1347E */ s16 mResidentCount;         // Group b0: how many of the saved characters exist
};

#endif // OV073_TUTORIALROOMSCENE_H
