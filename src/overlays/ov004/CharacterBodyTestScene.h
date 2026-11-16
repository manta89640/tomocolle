#ifndef OV004_CHARACTERBODYTESTSCENE_H
#define OV004_CHARACTERBODYTESTSCENE_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "Scene.h"

struct Camera;

// The scene of overlay 4 (scene id 0x04), which mounts Scn/Scene/SceneTestG3d_3_LZ.bin (as scene 0x06 of the main
// program does): a test of a character's body (its models, palette, scale, turning and joints). The layout is what
// its constructor (in SceneManager.cpp) shows; the name is from the overlay's code as its agent read it. The name
// is descriptive, not the original.
class CharacterBodyTestScene : public Scene {
public:
    CharacterBodyTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // Overlay 4 (src/overlays/ov004/CharacterBodyTestScene.cpp)
    void UpdateTurnTest();

    // What mTurnTest counts through: L starts it, and the character turns to face sTurnTarget
    enum TurnTest {
        TURN_TEST_IDLE,
        TURN_TEST_START, // Waits a frame
        TURN_TEST_AIM,   // Sets the character's target angle to the direction of sTurnTarget
        TURN_TEST_TURN,  // Turns it a step a frame until it faces that way
        TURN_TEST_DONE,
    };

    /* 0x0010 */ SceneCharacter mCharacter;
    /* 0x2B28 */ int mBodyStep;    // B steps it: which body model or palette the character is given next
    /* 0x2B2C */ int mUnknown2B2C; // Only cleared
    /* 0x2B30 */ Camera *mCamera;  // &data_02122764
    /* 0x2B34 */ int mTurnTest;    // TurnTest
}; // 0x2B38

// --- What the scene uses of the main program (not decompiled) ----------------------------------------------------

extern "C" {
// Moves the camera, its position and its target alike
void Camera_Translate(Camera *camera, fx32 x, fx32 y, fx32 z);
// Sets the line from the camera's eye through the touched point, its direction at data_02122818
int ModelResources_SetTouchLine(void *resources, int x, int y);
extern VecFx32 data_02122818;
}

#endif // OV004_CHARACTERBODYTESTSCENE_H
