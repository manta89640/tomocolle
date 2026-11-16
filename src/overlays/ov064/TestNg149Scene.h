#ifndef OV064_TESTNG149SCENE_H
#define OV064_TESTNG149SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"

// Overlay 64, scene 0xBF (not decompiled). The scene factory (CreateScene) loads overlay 64 and allocates 0x128 bytes;
// the constructor is the main program's (0x02049CA8, src/scene/SceneManager.cpp), which stores the overlay's virtual
// table (0x02149ED8). The members are as the constructor constructs them, by their classes; what lies between is not
// known. The scene's StartLoading mounts "Scn/Scene/SceneTestNg149_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg149_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg149Scene : public Scene {
public:
    TestNg149Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SpriteButton mButtons[1];
    /* 0x00104 */ u8 mUnknown00104[0xC];
    /* 0x00110 */ MessageFile mMessageFiles[1];
}; // 0x128

#endif // OV064_TESTNG149SCENE_H
