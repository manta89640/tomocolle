#ifndef OV052_SCENE8F_H
#define OV052_SCENE8F_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 52, scenes 0x8F, 0x90 (not decompiled). The scene factory (CreateScene) loads overlay 52 and allocates
// 0x24C74 bytes; the constructor is the main program's (0x020475C8, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x0214B4A0). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Mdl/Misc/ranger_3D_LZ.bin",
// "Mdl/Spl/SplNgEffect_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene8F : public Scene {
public:
    Scene8F(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[5];
    /* 0x0D788 */ u8 mUnknown0D788[0x1D10];
    /* 0x0F498 */ TtsPlayer mTtsPlayer;
    /* 0x23DDC */ SpriteButton mButtons[1];
    /* 0x23ED0 */ u8 mUnknown23ED0[0xC];
    /* 0x23EDC */ MessageFile mMessageFiles[1];
    /* 0x23EF4 */ u8 mUnknown23EF4[0x8];
    /* 0x23EFC */ MessageText mTexts[1];
    /* 0x244EC */ u8 mUnknown244EC[0x98];
    /* 0x24584 */ MessageText mText;
    /* 0x24B74 */ AnimModel mModel;
    /* 0x24C54 */ u8 mUnknown24C54[0x20];
}; // 0x24C74

#endif // OV052_SCENE8F_H
