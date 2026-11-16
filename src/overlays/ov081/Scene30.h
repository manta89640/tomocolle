#ifndef OV081_SCENE30_H
#define OV081_SCENE30_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"

// Overlay 81, scenes 0x30, 0x70 (not decompiled). The scene factory (CreateScene) loads overlay 81 and allocates
// 0x17EA8 bytes; the constructor is the main program's (0x0204156C, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x0214B3C8). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_contents_LZ.bin",
// "Mdl/Misc/fountain_box_3D_LZ.bin", "Msg/TmdCommonMsgJP_LZ.bin", "Msg/TmdEtcMsgJP_LZ.bin",
// "Scn/Scene/SceneCollectBills_LZ.bin", "Mdl/Spl/SplCollectBillsEffect_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene30 : public Scene {
public:
    Scene30(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x1BBC];
    /* 0x01BCC */ SceneCharacter mCharacters[4];
    /* 0x0C82C */ u8 mUnknown0C82C[0x4];
    /* 0x0C830 */ AnimModel mModel;
    /* 0x0C910 */ AnimModel mModel0C910;
    /* 0x0C9F0 */ u8 mUnknown0C9F0[0x4CC];
    /* 0x0CEBC */ MessageFile mMessageFile;
    /* 0x0CED4 */ MessageFile mMessageFile0CED4;
    /* 0x0CEEC */ MessageWindow mWindows[1];
    /* 0x17E90 */ u8 mUnknown17E90[0x18];
}; // 0x17EA8

#endif // OV081_SCENE30_H
