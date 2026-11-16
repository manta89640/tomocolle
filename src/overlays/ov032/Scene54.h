#ifndef OV032_SCENE54_H
#define OV032_SCENE54_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 32, scene 0x54 (not decompiled). The scene factory (CreateScene) loads overlay 32 and allocates 0x1A330
// bytes; the constructor is the main program's (0x02044520, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A580). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Mdl/Misc/fan_3D_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/SceneBG_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene54 : public Scene {
public:
    Scene54(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ MessageFile mMessageFiles[1];
    /* 0x00028 */ u8 mUnknown00028[0x204];
    /* 0x0022C */ TtsPlayer mTtsPlayer;
    /* 0x14B70 */ MessageText mText;
    /* 0x15160 */ SceneCharacter mCharacters[1];
    /* 0x17C78 */ u8 mUnknown17C78[0x1BBC];
    /* 0x19834 */ AnimModel mModel;
    /* 0x19914 */ u8 mUnknown19914[0xC];
    /* 0x19920 */ SpriteButton mButtons[1];
    /* 0x19A14 */ u8 mUnknown19A14[0xC];
    /* 0x19A20 */ Room mRoom;
    /* 0x1A328 */ u8 mUnknown1A328[0x8];
}; // 0x1A330

#endif // OV032_SCENE54_H
