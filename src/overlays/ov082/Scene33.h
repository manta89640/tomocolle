#ifndef OV082_SCENE33_H
#define OV082_SCENE33_H

#include <nitro.h>

#include "Scene.h"
#include "G3Sprite.h"
#include "MessageWindow.h"
#include "Model.h"
#include "text/MessageFile.h"

// Overlay 82, scenes 0x33, 0x73 (not decompiled). The scene factory (CreateScene) loads overlay 82 and allocates
// 0xEC2C bytes; the constructor is the main program's (0x020417DC, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x0214A940). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene's StartLoading mounts "Scn/Scene/SceneCollectBills_LZ.bin",
// "Mdl/Misc/fountain_box_3D_LZ.bin", "Msg/TmdCommonMsgJP_LZ.bin", "Msg/TmdEtcMsgJP_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene33 : public Scene {
public:
    Scene33(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ AnimModel mModel;
    /* 0x000F0 */ G3Sprite mG3Sprites[200];
    /* 0x03C50 */ u8 mUnknown03C50[0x4];
    /* 0x03C54 */ MessageFile mMessageFile;
    /* 0x03C6C */ MessageFile mMessageFile03C6C;
    /* 0x03C84 */ MessageWindow mWindows[1];
    /* 0x0EC28 */ u8 mUnknown0EC28[0x4];
}; // 0xEC2C

#endif // OV082_SCENE33_H
