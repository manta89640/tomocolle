#ifndef OV038_TESTNG111SCENE_H
#define OV038_TESTNG111SCENE_H

#include <nitro.h>

#include "HeadIcon.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "voice/TtsPlayer.h"

// Overlay 38, scene 0x76 (not decompiled). The scene factory (CreateScene) loads overlay 38 and allocates 0x19A24
// bytes; the constructor is the main program's (0x02046000, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A680). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_room_LZ.bin", "Scn/Scene/SceneTestNg111_LZ.bin",
// "Mdl/Spl/SplRoomEffect_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg111_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg111Scene : public Scene {
public:
    TestNg111Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C54];
    /* 0x0477C */ TtsPlayer mTtsPlayer;
    /* 0x190C0 */ u8 mUnknown190C0[0x820];
    /* 0x198E0 */ SpriteButton mButtons[1];
    /* 0x199D4 */ u8 mUnknown199D4[0xC];
    /* 0x199E0 */ ParticleSystem mParticles;
    /* 0x199E4 */ HeadIcon mHeadIcon;
}; // 0x19A24

#endif // OV038_TESTNG111SCENE_H
