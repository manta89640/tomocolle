#ifndef OV105_NINTENDOLOGOSCENE_H
#define OV105_NINTENDOLOGOSCENE_H

#include <nitro.h>

#include "Scene.h"

// The scene of overlay 105 (scene ids 0x00, 0x0D, 0x28 and 0xB6; the scene manager starts the game with 0xB6): it
// mounts "Cmn/Nin/NintendoLogo_LZ.bin", shows its screen on a grey-white background, and 60 frames later asks
// for scene 0x39 (PreStartScene). The overlay is not decompiled; this is what the constructor (in the main
// program's SceneManager.cpp) and the overlay's code show. The names are descriptive, not the original.
class NintendoLogoScene : public Scene {
public:
    NintendoLogoScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state); // func_ov105_02149ba8: also clears the two fields after it

    /* 0x10 */ int mState; // Update counts frames only while it is 0
    /* 0x14 */ int mUnknown14;
    /* 0x18 */ int mFrame;
}; // 0x1C

#endif // OV105_NINTENDOLOGOSCENE_H
