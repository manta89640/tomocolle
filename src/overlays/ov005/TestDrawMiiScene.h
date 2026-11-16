#ifndef OV005_TESTDRAWMIISCENE_H
#define OV005_TESTDRAWMIISCENE_H

#include <nitro.h>

#include "character/Character.h"
#include "Model.h"
#include "Scene.h"

// A model of the scene (overlay 5's class: its constructor, func_ov005_02149f10, and its deleting destructor are
// the overlay's; its table is at 0x0214A4E0). Its complete-object destructor is the implicit one, which the main
// program's copy (0x0203E2A8, emitted with the scene's constructor) serves for both.
class TestDrawMiiModel : public AnimModel {
public:
    TestDrawMiiModel();

    /* 0xE0 */ u8 mUnknownE0[0xC];
    /* 0xEC */ int mUnknownEC; // 0 at first
    /* 0xF0 */ int mUnknownF0; // 31 at first
    /* 0xF4 */ u8 mUnknownF4[4];
}; // 0xF8

// An animated model with nothing of its own, but a table of its own (0x02110954, in the main program, with its
// implicit destructors 0x0203E2E4 and 0x0204AD88): only this scene has one.
class TestDrawMiiAnimModel : public AnimModel {};

// The scene of overlay 5 (scene id 0x05), mounting Scn/Scene/SceneTestDrawMii_LZ.bin: a test that draws a Mii's
// face (a CharacterBase) with two models and the room model SME:/room.imd. The overlay is not decompiled: this
// is the layout its constructor (in SceneManager.cpp) shows. The names are descriptive, not the original.
class TestDrawMiiScene : public Scene {
public:
    TestDrawMiiScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ CharacterBase mCharacter;
    /* 0x25F4 */ TestDrawMiiModel mModels[2];
    /* 0x27E4 */ TestDrawMiiAnimModel mAnimModel;
    /* 0x28C4 */ u8 mUnknown28C4[0x34];
}; // 0x28F8

#endif // OV005_TESTDRAWMIISCENE_H
