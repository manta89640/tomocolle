#ifndef OV002_FACETESTSCENE_H
#define OV002_FACETESTSCENE_H

#include <nitro.h>

#include "character/AnimModelCharacter.h"
#include "Model.h"
#include "Scene.h"

struct Camera;

// The scene of overlay 2 (scene id 0x02), which mounts Scn/Scene/SceneTestG3d_LZ.bin (as scene 0x10 of the main
// program does): a test of a Mii's face on its own, with a glass model and a head with its animation
// (TGH:/head.imd, wait00.ica). The layout is what its constructor (in SceneManager.cpp) shows; the member names are
// from the overlay's code as its agent read it. The names are descriptive, not the original.
class FaceTestScene : public Scene {
public:
    FaceTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ Model mGlasses;             // TG3:/Glass6.imd, resource 0
    /* 0x00C4 */ Camera *mCamera;            // &data_02122764
    /* 0x00C8 */ CharacterBase mFace;        // A face with no head model
    /* 0x26AC */ AnimModelCharacter mHead;   // Resource 1: TGH:/head.imd with wait00.ica
    /* 0x4DD4 */ int mFaceKind;              // X with up and down: 0 to 26
    /* 0x4DD8 */ int mHeadTurn;              // L with up and down: 0 to 7
}; // 0x4DDC

#endif // OV002_FACETESTSCENE_H
