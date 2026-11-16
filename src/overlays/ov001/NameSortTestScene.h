#ifndef OV001_NAMESORTTESTSCENE_H
#define OV001_NAMESORTTESTSCENE_H

#include <nitro.h>

#include "character/Character.h"
#include "Scene.h"

// The scene of overlay 1 (scene id 0x0C): a test of sorting eight saved characters by name (MATH_QSort with the
// main program's name comparators, also on a thread). The layout is what its constructor (in SceneManager.cpp)
// shows. The name is descriptive, not the original.
class NameSortTestScene : public Scene {
public:
    NameSortTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ u8 mUnknown0010[0x44];
    /* 0x0054 */ SavedCharacter mCharacters[8];
    /* 0x1654 */ u8 mUnknown1654[0xDE0];
}; // 0x2434

#endif // OV001_NAMESORTTESTSCENE_H
