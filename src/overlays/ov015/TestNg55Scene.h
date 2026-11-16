#ifndef OV015_TESTNG55SCENE_H
#define OV015_TESTNG55SCENE_H

#include <nitro.h>

#include "DrawingCanvas.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"

// Overlay 15, scene 0x3A (not decompiled). The scene factory (CreateScene) loads overlay 15 and allocates 0x1C3D8
// bytes; the constructor is the main program's (0x02042254, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A5A0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Scn/Scene/SceneTestNg55_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg55_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.

// The scene's drawing canvas: a class of its own derived from the main program's DrawingCanvas, whose implicit
// constructor builds the base with DrawingCanvas's C2 (0x02025A9C), where a member is built with C1. The name is
// descriptive, not the original.
class TestNg55Canvas : public DrawingCanvas {
public:
    /* 0x2B4 */ u8 mUnknown2B4[0xB8];
}; // 0x36C

class TestNg55Scene : public Scene {
public:
    TestNg55Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ CharacterBase mCharacters[11];
    /* 0x1A0DC */ u8 mUnknown1A0DC[0x1E80];
    /* 0x1BF5C */ TestNg55Canvas mCanvas;
    /* 0x1C2C8 */ SpriteButton mButtons[1];
    /* 0x1C3BC */ u8 mUnknown1C3BC[0x1C];
}; // 0x1C3D8

#endif // OV015_TESTNG55SCENE_H
