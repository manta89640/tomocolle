#ifndef OV004_CHARACTERBODYTESTSCENE_H
#define OV004_CHARACTERBODYTESTSCENE_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "Scene.h"

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

    /* 0x0010 */ SceneCharacter mCharacter;
    /* 0x2B28 */ u8 mUnknown2B28[0x10];
}; // 0x2B38

#endif // OV004_CHARACTERBODYTESTSCENE_H
