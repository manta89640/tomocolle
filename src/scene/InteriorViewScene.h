#ifndef SCENE_INTERIORVIEWSCENE_H
#define SCENE_INTERIORVIEWSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "room/Room.h"

struct Camera;

// Scene 0x2E, of the main program (functions 0x02051DAC to 0x02052244, virtual table 0x02111D08; not decompiled): it
// shows a room (mounted as "CO0" and "RM" by StartLoading) and steps it through the 78 interiors when its state asks
// for the next one (the interior's number wraps at 78 and is given to the room), while the pad turns the camera.
// The scene factory allocates 0x924 bytes and loads no overlay; the constructor is in src/scene/SceneManager.cpp.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class InteriorViewScene : public Scene {
public:
    InteriorViewScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state); // 0x020521D4: sets mState and clears mStep

    /* 0x10 */ Camera *mCamera;     // The pad's directions turn its angles (+0x44, +0x46)
    /* 0x14 */ int mState;
    /* 0x18 */ u16 mStep;
    /* 0x1A */ u16 mInterior;       // The interior shown, 0 to 77
    /* 0x1C */ Room mRoom;
}; // 0x924

#endif // SCENE_INTERIORVIEWSCENE_H
