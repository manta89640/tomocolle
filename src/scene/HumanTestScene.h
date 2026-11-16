#ifndef SCENE_HUMANTESTSCENE_H
#define SCENE_HUMANTESTSCENE_H

#include "MainProgram3d.h"

// Scene 7 (main program, no overlay): a test of a head model and a body model with textures loaded later, with the
// archives "Scn/Scene/TestHuman00Itp_LZ.bin" (HP0), "Scn/Scene/TestHuman00Imd_LZ.bin" (HM0) and
// "Scn/Scene/TestBody00Imd_LZ.bin" (BM0). A loads the textures from two more archives (HT0, BT0) and starts the
// head's texture pattern animation; then B loads the palette "style01.ntfp" over the body's, its first colour
// replaced. The pad turns the camera. The scene factory (main func_0203c050, case 7) allocates 0x1D8 bytes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.
class HumanTestScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x010 */ Camera *mCamera;
    /* 0x014 */ AnimModel mHead;
    /* 0x0F4 */ AnimModel mBody;
    /* 0x1D4 */ BOOL mTexturesLoaded;
}; // 0x1D8

#endif // SCENE_HUMANTESTSCENE_H
