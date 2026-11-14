#ifndef OV073_TUTORIALROOMLIGHT_H
#define OV073_TUTORIALROOMLIGHT_H

#include <nitro.h>
#include <nnsys.h>

#include "MainProgram.h"

class TutorialRoomCamera;
class TutorialApartmentRoom;

// The lighting of the apartment room: two lights that keep their direction relative to the camera, and whose
// colour fades between the room style's own and the room's current one.
//
// The name is descriptive, not the original: no name of this class survives in the ROM. Overlay 74 has the same
// class without the prefix, which tells the two apart: the program is linked as one, so they cannot have one name.
class TutorialRoomLight {
public:
    enum State {
        STATE_IDLE,
        STATE_SET_STYLE_COLOR,  // Take the colour of the room's style at once
        STATE_FADE_STYLE_COLOR, // Fade to it
        STATE_SET_ROOM_COLOR,   // Take the colour the room gives (func_020a4d90) at once
        STATE_FADE_ROOM_COLOR,  // Fade to it
    };

    TutorialRoomLight();
    void Init(TutorialRoomCamera *camera, TutorialApartmentRoom *room);
    void Update();
    void SetState(int state);
    void SetColor(u16 color);
    void BlendColor(int *targetR, int *targetG, int *targetB, int *r, int *g, int *b);

    /* 0x00 */ Light *mLights[2];
    /* 0x08 */ TutorialRoomCamera *mCamera;
    /* 0x0C */ TutorialApartmentRoom *mRoom;
    /* 0x10 */ int mState;
    /* 0x14 */ fx32 mFade; // 0 to 1, squared to weigh the target colour
    /* 0x18 */ s16 mStep;
}; // 0x1C

#endif // OV073_TUTORIALROOMLIGHT_H
