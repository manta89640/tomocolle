#ifndef OV022_SCENE45_H
#define OV022_SCENE45_H

#include <nitro.h>

#include "Scene.h"
#include "character/SceneCharacter.h"
#include "voice/TtsPlayer.h"

// Overlay 22, scene 0x45 (not decompiled). The scene factory (CreateScene) loads overlay 22 and allocates 0x1BBEC
// bytes; the constructor is the main program's (0x0204378C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A000). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene45 : public Scene {
public:
    Scene45(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1C4C];
    /* 0x0728C */ TtsPlayer mTtsPlayer;
    /* 0x1BBD0 */ u8 mUnknown1BBD0[0x1C];
}; // 0x1BBEC

#endif // OV022_SCENE45_H
