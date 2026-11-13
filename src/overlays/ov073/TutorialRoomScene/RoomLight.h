#ifndef ROOMLIGHT_H
#define ROOMLIGHT_H

#include <nitro.h>
#include <nnsys.h>

class RoomCamera;
class ApartmentRoom;

// One of the main program's 3D lights (data_021227cc is an array of them; func_0202a96c resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

// The lighting of the apartment room: two lights that keep their direction relative to the camera, and whose
// colour fades between the room style's own and the room's current one.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class RoomLight {
public:
    enum State {
        STATE_IDLE,
        STATE_SET_STYLE_COLOR,  // Take the colour of the room's style at once
        STATE_FADE_STYLE_COLOR, // Fade to it
        STATE_SET_ROOM_COLOR,   // Take the colour the room gives (func_020a4d90) at once
        STATE_FADE_ROOM_COLOR,  // Fade to it
    };

    RoomLight();
    void Init(RoomCamera *camera, ApartmentRoom *room);
    void Update();
    void SetState(int state);
    void SetColor(u16 color);
    void BlendColor(int *targetR, int *targetG, int *targetB, int *r, int *g, int *b);

    /* 0x00 */ Light *mLights[2];
    /* 0x08 */ RoomCamera *mCamera;
    /* 0x0C */ ApartmentRoom *mRoom;
    /* 0x10 */ int mState;
    /* 0x14 */ fx32 mFade; // 0 to 1, squared to weigh the target colour
    /* 0x18 */ s16 mStep;
}; // 0x1C

#endif // ROOMLIGHT_H
