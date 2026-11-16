#ifndef OV004_SCENE04_H
#define OV004_SCENE04_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "Scene.h"

// The scene of overlay 4 (scene id 0x04), a test that mounts Scn/Scene/SceneTestG3d_3_LZ.bin (as scene 0x06 of the
// main program does) and draws a character. The overlay is not decompiled: this is the layout its constructor (in
// SceneManager.cpp) shows. The name is the scene id's: no name of this class survives in the ROM.
class Scene04 : public Scene {
public:
    Scene04(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ SceneCharacter mCharacter;
    /* 0x2B28 */ u8 mUnknown2B28[0x10];
}; // 0x2B38

#endif // OV004_SCENE04_H
