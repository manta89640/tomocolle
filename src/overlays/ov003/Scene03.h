#ifndef OV003_SCENE03_H
#define OV003_SCENE03_H

#include <nitro.h>

#include "Model.h"
#include "Scene.h"

// The scene of overlay 3 (scene id 0x03), a test that loads the body models Mdl/Body/body000 and body001. The
// overlay is not decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The name is the
// scene id's: no name of this class survives in the ROM.
class Scene03 : public Scene {
public:
    Scene03(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x010 */ Model mModel0;
    /* 0x0C4 */ Model mModel1;
    /* 0x178 */ u8 mUnknown178[0x44];
}; // 0x1BC

#endif // OV003_SCENE03_H
