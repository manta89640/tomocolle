#ifndef OV048_SCENE8B_H
#define OV048_SCENE8B_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"

// Overlay 48, scene 0x8B (not decompiled). The scene factory (CreateScene) loads overlay 48 and allocates 0x52CC
// bytes; the constructor is the main program's (0x02047130, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A540). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Mdl/Misc/futon_3D_LZ.bin",
// "Mdl/Misc/futon_shadow_3D_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene8B : public Scene {
public:
    Scene8B(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BC4];
    /* 0x046EC */ SpriteButton mButtons[1];
    /* 0x047E0 */ u8 mUnknown047E0[0xC];
    /* 0x047EC */ Room mRoom;
    /* 0x050F4 */ u8 mUnknown050F4[0x8];
    /* 0x050FC */ AnimModel mModel;
    /* 0x051DC */ AnimModel mModel051DC;
    /* 0x052BC */ u8 mUnknown052BC[0x10];
}; // 0x52CC

#endif // OV048_SCENE8B_H
