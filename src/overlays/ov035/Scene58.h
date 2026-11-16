#ifndef OV035_SCENE58_H
#define OV035_SCENE58_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "voice/TtsPlayer.h"

// Overlay 35, scene 0x58 (not decompiled). The scene factory (CreateScene) loads overlay 35 and allocates 0x19498
// bytes; the constructor is the main program's (0x02044988, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A710). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg105_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene58 : public Scene {
public:
    Scene58(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C50];
    /* 0x04778 */ TtsPlayer mTtsPlayer;
    /* 0x190BC */ u8 mUnknown190BC[0x2DC];
    /* 0x19398 */ SpriteButton mButtons[1];
    /* 0x1948C */ u8 mUnknown1948C[0xC];
}; // 0x19498

#endif // OV035_SCENE58_H
