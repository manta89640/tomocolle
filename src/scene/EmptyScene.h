#ifndef SCENE_EMPTYSCENE_H
#define SCENE_EMPTYSCENE_H

#include "Scene.h"

// Scene 22 (main program, no overlay): a scene that does nothing; all its functions are empty. The scene factory
// (main func_0203c050, case 22) allocates 0x10 bytes.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class EmptyScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
}; // 0x10

#endif // SCENE_EMPTYSCENE_H
