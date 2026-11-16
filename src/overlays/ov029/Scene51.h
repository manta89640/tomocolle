#ifndef OV029_SCENE51_H
#define OV029_SCENE51_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 29, scene 0x51 (not decompiled). The scene factory (CreateScene) loads overlay 29 and allocates 0x1CA58
// bytes; the constructor is the main program's (0x020440F0, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A6E4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Mdl/Misc/beach_3D_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene51 : public Scene {
public:
    Scene51(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1C5C];
    /* 0x0729C */ TtsPlayer mTtsPlayer;
    /* 0x1BBE0 */ AnimModel mModel;
    /* 0x1BCC0 */ SpriteButton mButtons[1];
    /* 0x1BDB4 */ u8 mUnknown1BDB4[0xC];
    /* 0x1BDC0 */ MessageFile mMessageFiles[1];
    /* 0x1BDD8 */ u8 mUnknown1BDD8[0x8];
    /* 0x1BDE0 */ MessageText mTexts[1];
    /* 0x1C3D0 */ u8 mUnknown1C3D0[0x98];
    /* 0x1C468 */ MessageText mText;
}; // 0x1CA58

#endif // OV029_SCENE51_H
