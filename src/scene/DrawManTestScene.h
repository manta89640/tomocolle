#ifndef SCENE_DRAWMANTESTSCENE_H
#define SCENE_DRAWMANTESTSCENE_H

#include "MainProgram3d.h"

// Scene 6 (main program, no overlay): a test of drawing a character's head model, with the archives
// "Scn/Scene/SceneTestDrawMan_LZ.bin" (DMN) and "Scn/Scene/SceneTestG3d_3_LZ.bin" (CA3). It shows the model
// "HUMAN00_.imd" with the textures "HUMAN00.itx" and a character, and keeps the matrix of the model's joint
// "head_h2" as it is drawn, which the character is drawn with. The pad turns the camera; with A held, left and
// right move the model; B binds another palette to the model and Y loads its textures again. The scene factory
// (main func_0203c050, case 6) allocates 0x272C bytes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.
class DrawManTestScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    static void HeadCallback(NNSG3dRS *rs);

    /* 0x0010 */ Camera *mCamera;
    /* 0x0014 */ AnimModel mModel;
    /* 0x00F4 */ CharacterBase mCharacter;
    /* 0x26D8 */ MtxFx43 mHeadMatrix;   // The head joint's matrix, as the model was last drawn
    /* 0x2708 */ MtxFx33 mHeadRotation;
}; // 0x272C

#endif // SCENE_DRAWMANTESTSCENE_H
