#ifndef OV108_SCENE69_H
#define OV108_SCENE69_H

#include <nitro.h>

#include "Scene.h"

// Overlay 108, scene 0x69 (not decompiled). The scene factory (CreateScene) loads overlay 108 and allocates 0x10
// bytes; the constructor is the main program's (0x02045B44, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x02149DC0). The members are as the constructor constructs them, by their classes; what lies between
// is not known.
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene69 : public Scene {
public:
    Scene69(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
}; // 0x10

#endif // OV108_SCENE69_H
