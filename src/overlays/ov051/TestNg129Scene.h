#ifndef OV051_TESTNG129SCENE_H
#define OV051_TESTNG129SCENE_H

#include <nitro.h>

#include "DrawingCanvas.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"

// Overlay 51, scene 0x8E (not decompiled). The scene factory (CreateScene) loads overlay 51 and allocates 0x56F0
// bytes; the constructor is the main program's (0x020474C0, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A9A4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Mdl/Goods/gd10_3D_LZ.bin",
// "Scn/Scene/SceneTestNg129_LZ.bin", "Scn/Scene/SceneBG_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg129_LZ.bin", which no other overlay mounts; it is not
// the original: no name of this class survives in the ROM.
class TestNg129Scene : public Scene {
public:
    TestNg129Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ SceneCharacter mCharacters[1];
    /* 0x2B28 */ u8 mUnknown2B28[0x1BC4];
    /* 0x46EC */ SpriteButton mButtons[1];
    /* 0x47E0 */ u8 mUnknown47E0[0xC];
    /* 0x47EC */ Room mRoom;
    /* 0x50F4 */ u8 mUnknown50F4[0x8];
    /* 0x50FC */ AnimModel mModel;
    /* 0x51DC */ u8 mUnknown51DC[0xC];
    /* 0x51E8 */ DrawingCanvas mCanvas;
    /* 0x549C */ u8 mUnknown549C[0x154];
    /* 0x55F0 */ SpriteButton mButtons2[1];
    /* 0x56E4 */ u8 mUnknown56E4[0xC];
}; // 0x56F0

#endif // OV051_TESTNG129SCENE_H
