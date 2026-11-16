#ifndef OV030_TESTNG94SCENE_H
#define OV030_TESTNG94SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "voice/TtsPlayer.h"

// Overlay 30, scene 0x52 (not decompiled). The scene factory (CreateScene) loads overlay 30 and allocates 0x19448
// bytes; the constructor is the main program's (0x02044220, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A5E0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Mdl/Misc/cosmos_3D_LZ.bin",
// "Scn/Scene/SceneTestNg94_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg94_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg94Scene : public Scene {
public:
    TestNg94Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C48];
    /* 0x04770 */ TtsPlayer mTtsPlayer;
    /* 0x190B4 */ u8 mUnknown190B4[0x28];
    /* 0x190DC */ AnimModel mModel;
    /* 0x191BC */ AnimModel mModel191BC;
    /* 0x1929C */ u8 mUnknown1929C[0xAC];
    /* 0x19348 */ SpriteButton mButtons[1];
    /* 0x1943C */ u8 mUnknown1943C[0xC];
}; // 0x19448

#endif // OV030_TESTNG94SCENE_H
