#ifndef OV014_TESTNG50SCENE_H
#define OV014_TESTNG50SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "voice/TtsPlayer.h"

// Overlay 14, scene 0x37 (not decompiled). The scene factory (CreateScene) loads overlay 14 and allocates 0x30C54
// bytes; the constructor is the main program's (0x02041EE8, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A220). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Scn/Scene/SceneTestNg50_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg50_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg50Scene : public Scene {
public:
    TestNg50Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ CharacterBase mCharacterBases[11];
    /* 0x1A0DC */ u8 mUnknown1A0DC[0x1FD8];
    /* 0x1C0B4 */ TtsPlayer mTtsPlayer;
    /* 0x309F8 */ u8 mUnknown309F8[0x15C];
    /* 0x30B54 */ SpriteButton mButtons[1];
    /* 0x30C48 */ u8 mUnknown30C48[0xC];
}; // 0x30C54

#endif // OV014_TESTNG50SCENE_H
