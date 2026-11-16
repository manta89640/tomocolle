#ifndef OV041_SCENE7B_H
#define OV041_SCENE7B_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "Model.h"
#include "SpriteButton.h"

// Overlay 41, scene 0x7B (not decompiled). The scene factory (CreateScene) loads overlay 41 and allocates 0x7690
// bytes; the constructor is the main program's (0x02046324, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A840). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Misc/mame_3D_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A character of the scene: the main program's CharacterBase and 0x18 bytes more. Its implicit constructor and
// destructor (CharacterBase's C2 and D2, and nothing else) are the main program's copies after the scene's
// constructor (0x0204640C, 0x02046420), emitted for the scene's __cxa_vec_ctor.
//
// The name is descriptive, not the original.
class Scene7BCharacter : public CharacterBase {
public:
    /* 0x25E4 */ u8 mUnknown25E4[0x18];
}; // 0x25FC

class Scene7B : public Scene {
public:
    Scene7B(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x8];
    /* 0x00018 */ AnimModel mModel;
    /* 0x000F8 */ Scene7BCharacter mCharacters[3];
    /* 0x072EC */ AnimModel mModels[3];
    /* 0x0758C */ u8 mUnknown0758C[0x4];
    /* 0x07590 */ SpriteButton mButtons[1];
    /* 0x07684 */ u8 mUnknown07684[0xC];
}; // 0x7690

#endif // OV041_SCENE7B_H
