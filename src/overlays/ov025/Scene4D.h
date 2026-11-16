#ifndef OV025_SCENE4D_H
#define OV025_SCENE4D_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 25, scene 0x4D (not decompiled). The scene factory (CreateScene) loads overlay 25 and allocates 0x1CD50
// bytes; the constructor is the main program's (0x02043C60, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A7A0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Mdl/Misc/bungee_3D_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene4D : public Scene {
public:
    Scene4D(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1DD4];
    /* 0x07414 */ TtsPlayer mTtsPlayer;
    /* 0x1BD58 */ AnimModel mModel;
    /* 0x1BE38 */ SpriteButton mButtons[1];
    /* 0x1BF2C */ u8 mUnknown1BF2C[0xC];
    /* 0x1BF38 */ MessageFile mMessageFiles[1];
    /* 0x1BF50 */ u8 mUnknown1BF50[0x8];
    /* 0x1BF58 */ MessageText mTexts[1];
    /* 0x1C548 */ u8 mUnknown1C548[0x218];
    /* 0x1C760 */ MessageText mText;
}; // 0x1CD50

#endif // OV025_SCENE4D_H
