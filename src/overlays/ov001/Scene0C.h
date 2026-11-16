#ifndef OV001_SCENE0C_H
#define OV001_SCENE0C_H

#include <nitro.h>

#include "character/Character.h"
#include "Scene.h"

// The scene of overlay 1 (scene id 0x0C), a test with eight saved characters. The overlay is not decompiled: this
// is the layout its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name of this class
// survives in the ROM.
class Scene0C : public Scene {
public:
    Scene0C(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ u8 mUnknown0010[0x44];
    /* 0x0054 */ SavedCharacter mCharacters[8];
    /* 0x1654 */ u8 mUnknown1654[0xDE0];
}; // 0x2434

#endif // OV001_SCENE0C_H
