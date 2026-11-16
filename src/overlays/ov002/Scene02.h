#ifndef OV002_SCENE02_H
#define OV002_SCENE02_H

#include <nitro.h>

#include "character/AnimModelCharacter.h"
#include "Model.h"
#include "Scene.h"

// The scene of overlay 2 (scene id 0x02), a test that mounts Scn/Scene/SceneTestG3d_LZ.bin (as scene 0x10 of the
// main program does) with a glass model and a head with its animation (TGH:/head.imd, wait00.ica). The overlay is not decompiled: this is
// the layout its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name of this class
// survives in the ROM.
class Scene02 : public Scene {
public:
    Scene02(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ Model mModel;
    /* 0x00C4 */ u8 mUnknown00C4[4];
    /* 0x00C8 */ CharacterBase mCharacter;
    /* 0x26AC */ AnimModelCharacter mAnimCharacter;
    /* 0x4DD4 */ u8 mUnknown4DD4[8];
}; // 0x4DDC

#endif // OV002_SCENE02_H
