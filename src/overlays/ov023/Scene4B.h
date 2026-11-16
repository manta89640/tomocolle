#ifndef OV023_SCENE4B_H
#define OV023_SCENE4B_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "voice/TtsPlayer.h"

// Overlay 23, scene 0x4B (not decompiled). The scene factory (CreateScene) loads overlay 23 and allocates 0x3AB2C
// bytes; the constructor is the main program's (0x02043A54, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A960). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Mdl/Misc/fashion_show_3D_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene4B : public Scene {
public:
    Scene4B(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x128];
    /* 0x00138 */ SceneCharacter mCharacters[3];
    /* 0x08280 */ u8 mUnknown08280[0x1BBC];
    /* 0x09E3C */ CharacterBase mCharacterBases[11];
    /* 0x23F08 */ u8 mUnknown23F08[0x1DBC];
    /* 0x25CC4 */ AnimModel mModel;
    /* 0x25DA4 */ u8 mUnknown25DA4[0x84];
    /* 0x25E28 */ TtsPlayer mTtsPlayer;
    /* 0x3A76C */ SavedCharacter mSavedCharacter;
    /* 0x3AA2C */ SpriteButton mButtons[1];
    /* 0x3AB20 */ u8 mUnknown3AB20[0xC];
}; // 0x3AB2C

#endif // OV023_SCENE4B_H
