#ifndef OV005_TESTDRAWMIISCENE_H
#define OV005_TESTDRAWMIISCENE_H

#include <nitro.h>

#include "character/Character.h"
#include "Model.h"
#include "Scene.h"

struct Camera;

// A model of the scene (overlay 5's class: its constructor, func_ov005_02149f10, and its deleting destructor are
// the overlay's; its table is at 0x0214A4E0). Its complete-object destructor is the implicit one, which the main
// program's copy (0x0203E2A8, emitted with the scene's constructor) serves for both.
// The scene has two, which fade out while they stand between the camera and the Mii (TestDrawMiiScene::
// UpdateModelFades) and back in when they do not.
class TestDrawMiiModel : public AnimModel {
public:
    // mFade
    enum Fade {
        FADE_IN,  // Its alpha steps up to 31
        FADE_OUT, // Its alpha steps down to 0
    };

    TestDrawMiiModel();

    void Init();
    void Update();

    /* 0xE0 */ VecFx32 mHome;  // Where it stands: copied to its position at each update
    /* 0xEC */ int mFade;      // 0 at first
    /* 0xF0 */ int mAlpha;     // 31 at first
    /* 0xF4 */ u16 mHomeRotY;  // Copied to its rotation about y at each update
    /* 0xF6 */ u8 mUnknownF6[2];
}; // 0xF8

// An animated model with nothing of its own, but a table of its own (0x02110954, in the main program, with its
// implicit destructors 0x0203E2E4 and 0x0204AD88): only this scene has one.
class TestDrawMiiAnimModel : public AnimModel {};

// The scene's camera: it circles a point at the angles mAngles, at the distances mRadius (overlay 5's
// functions 0x0214A010 to 0x0214A2A8; no constructor, no table).
class TestDrawMiiCamera {
public:
    void Init();
    void Update();
    VecFx32 GetEye(VecFx32 radius, VecFx32 center);
    void SetRandomRadius();
    void SetRandomTarget();

    /* 0x00 */ Camera *mCamera; // &gCamera
    /* 0x04 */ VecFx32 mEye;    // Where it looks from, as GetEye last gave it
    /* 0x10 */ VecFx32 mAngles; // fx32 sine table indices: x the height (pitch), y the turn about the point (yaw)
    /* 0x1C */ VecFx32 mRadius; // The distance along each axis: z with the yaw's sine on x, x with its cosine on z
    /* 0x28 */ VecFx32 mTarget; // What it looks at: one eighth of it is the centre of the circle
}; // 0x34

// The scene of overlay 5 (scene id 0x05), mounting Scn/Scene/SceneTestDrawMii_LZ.bin: a test that draws a Mii's
// face (a CharacterBase) with two models and the room model SME:/room.imd. The layout is what its constructor (in
// SceneManager.cpp) and the overlay's code show. The names are descriptive, not the original.
class TestDrawMiiScene : public Scene {
public:
    TestDrawMiiScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void UpdateModelFades();

    /* 0x0010 */ CharacterBase mCharacter;
    /* 0x25F4 */ TestDrawMiiModel mModels[2];
    /* 0x27E4 */ TestDrawMiiAnimModel mAnimModel; // The room: SME:/room.imd, resource 0
    /* 0x28C4 */ TestDrawMiiCamera mCamera;
}; // 0x28F8

#endif // OV005_TESTDRAWMIISCENE_H
