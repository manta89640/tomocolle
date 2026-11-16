#ifndef OV105_NINTENDOLOGOSCENE_H
#define OV105_NINTENDOLOGOSCENE_H

#include <nitro.h>

#include "Scene.h"

// Overlay 105, scenes 0x00, 0x0D, 0x28 and 0xB6 (the scene manager starts the game with 0xB6): the Nintendo logo. It
// mounts "Cmn/Nin/NintendoLogo_LZ.bin" as "MNU", shows its screen "MNU:/Gray_White.nsc" on one plane of the main
// engine, with the screens fading in and out at a tenth of full brightness a frame, and 60 frames after it starts
// asks for scene 0x39 (PreStartScene, overlay 106). The four scene ids construct the same scene and it never reads
// its id: they differ only in who asks for them (docs/binary.md). The scene factory (main CreateScene) loads overlay
// 105 and allocates 0x1C bytes; the constructor is the main program's (src/scene/SceneManager.cpp).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class NintendoLogoScene : public Scene {
public:
    // mState: the scene has only the one
    enum State {
        STATE_SHOW, // Count the frames, and leave after 60
    };

    NintendoLogoScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);

    /* 0x10 */ int mState;
    /* 0x14 */ int mUnknown14; // SetState clears it, and nothing reads it
    /* 0x18 */ int mFrame;     // The frames counted in the state, from 0 (SetState)
}; // 0x1C

#endif // OV105_NINTENDOLOGOSCENE_H
