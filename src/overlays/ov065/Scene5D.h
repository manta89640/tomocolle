#ifndef OV065_SCENE5D_H
#define OV065_SCENE5D_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "text/TextCanvas.h"

// Overlay 65, scene 0x5D (not decompiled). The scene factory (CreateScene) loads overlay 65 and allocates 0x31AE0
// bytes; the constructor is the main program's (0x02045114, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BC20). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Scn/List/MiiList_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene5D : public Scene {
public:
    Scene5D(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x196F8];
    /* 0x19708 */ SpriteButton mButton;
    /* 0x197FC */ SpriteButton mButton197FC;
    /* 0x198F0 */ u8 mUnknown198F0[0x1D0];
    /* 0x19AC0 */ SpriteButton mButtons[7];
    /* 0x1A16C */ ObjTextBox mTextBoxes[7];
    /* 0x1AD90 */ u8 mUnknown1AD90[0x15E0C];
    /* 0x30B9C */ SpriteButton mButtons30B9C[16];
    /* 0x31ADC */ u8 mUnknown31ADC[0x4];
}; // 0x31AE0

#endif // OV065_SCENE5D_H
