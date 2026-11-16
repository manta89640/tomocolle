#ifndef SCENE_G3DTESTSCENE_H
#define SCENE_G3DTESTSCENE_H

#include "Scene.h"
#include "Model.h"
#include "character/Character.h"

struct Camera;

// Scene 16 (main program, no overlay): a test of the 3D engine with two characters, a body model and a pair of
// glasses, with the archives "Mdl/Ica/ica_mii_room_LZ.bin" (BIC, the room animations), "Mdl/Body/body000_3D_LZ.bin"
// (THM) and "Scn/Scene/SceneTestG3d_LZ.bin" (TG3, "Glass6.imd"). Edge marking is on, and each model gets its own
// polygon id. The glasses follow the first character's left hand. With B held the pad turns the first character's
// model; X starts a room animation (by the direction held, or at random), Y another face, and with R held the pad
// turns the camera. The scene factory (main func_0203c050, case 16) allocates 0x5C2C bytes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.
class G3dTestScene : public Scene {
public:
    G3dTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ Camera *mCamera;
    /* 0x0014 */ Character mCharacter;
    /* 0x2B2C */ Character mSecondCharacter;
    /* 0x5644 */ CharacterModel mBody;
    /* 0x5B4C */ AnimModel mGlasses;
}; // 0x5C2C

#endif // SCENE_G3DTESTSCENE_H
