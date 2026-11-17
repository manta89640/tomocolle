#ifndef OV075_SCENE40LIGHTS_H
#define OV075_SCENE40LIGHTS_H

#include <nitro.h>

struct Camera;
struct Light;
class Room;

// The scene's two lights (scene+0x69E10): they follow the camera and take the room's colour, as overlay 96's
// WeddingLights.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40Lights {
public:
    void Init(Camera *camera, Room *room);
    void Update();
    void SetState(int state);
    void SetColor(u16 color);

    /* 0x00 */ int mState; // 0 following the camera; 1 takes the room's colour again
    /* 0x04 */ Light *mLights[2];
    /* 0x0C */ Camera *mCamera;
    /* 0x10 */ Room *mRoom;
}; // 0x14

#endif // OV075_SCENE40LIGHTS_H
