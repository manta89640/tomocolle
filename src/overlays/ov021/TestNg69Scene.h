#ifndef OV021_TESTNG69SCENE_H
#define OV021_TESTNG69SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "voice/TtsPlayer.h"

// Overlay 21, scene 0x41 (not decompiled). The scene factory (CreateScene) loads overlay 21 and allocates 0x19BD4
// bytes; the constructor is the main program's (0x02043208, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A5CC). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Scn/Scene/SceneTestNg69_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg69_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
// A model of the scene, of 0xE8 bytes: the main program's AnimModel and something after it. Its constructor is the
// overlay's (0x0214A194); its destructor, which only destroys the model, is the main program's (0x020432F0, after the
// scene's constructor), emitted for the scene's __cxa_vec_ctor. A member, not a base: the destructor stores only the
// model's tables.
//
// The name is descriptive, not the original.
class TestNg69Model {
public:
    TestNg69Model();

    /* 0x00 */ AnimModel mModel;
    /* 0xE0 */ u8 mUnknownE0[8];
}; // 0xE8

class TestNg69Scene : public Scene {
public:
    TestNg69Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ TestNg69Model mModels[10];
    /* 0x04FF4 */ u8 mUnknown04FF4[0xBC];
    /* 0x050B0 */ TtsPlayer mTtsPlayer;
    /* 0x199F4 */ SpriteButton mButtons[1];
    /* 0x19AE8 */ u8 mUnknown19AE8[0xC];
    /* 0x19AF4 */ AnimModel mModels19AF4[1];
}; // 0x19BD4

#endif // OV021_TESTNG69SCENE_H
