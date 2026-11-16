#ifndef OV003_BODYPALETTETESTSCENE_H
#define OV003_BODYPALETTETESTSCENE_H

#include <nitro.h>

#include "Model.h"
#include "Scene.h"

struct Camera;

// The scene of overlay 3 (scene id 0x03), a test of the body models' palettes (Mdl/Body/body000 and body001). The
// layout is what its constructor (in SceneManager.cpp) shows; the name is from the overlay's code as its agent
// read it. The name is descriptive, not the original.
class BodyPaletteTestScene : public Scene {
public:
    BodyPaletteTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x010 */ Model mBody;       // Resource 0: TG3's body.imd; its palette is changed and its model replaced
    /* 0x0C4 */ Model mSecondBody; // Resource 1: TG4's body.imd, the same model, left as loaded beside the first
    /* 0x178 */ Camera *mCamera;
    /* 0x17C */ u8 mUnknown17C[0x40];
}; // 0x1BC

#endif // OV003_BODYPALETTETESTSCENE_H
