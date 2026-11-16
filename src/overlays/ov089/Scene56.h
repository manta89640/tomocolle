#ifndef OV089_SCENE56_H
#define OV089_SCENE56_H

#include <nitro.h>

#include "Scene.h"
#include "character/SceneCharacter.h"

// Overlay 89, scene 0x56 (not decompiled). The scene factory (CreateScene) loads overlay 89 and allocates 0x8178
// bytes; the constructor is the main program's (0x0204479C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AAC4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_room_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene56 : public Scene {
public:
    Scene56(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x8];
    /* 0x00018 */ SceneCharacter mCharacters[3];
    /* 0x08160 */ u8 mUnknown08160[0x18];
}; // 0x8178

#endif // OV089_SCENE56_H
