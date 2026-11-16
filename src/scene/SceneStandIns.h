#ifndef SCENE_SCENESTANDINS_H
#define SCENE_SCENESTANDINS_H

#include "Scene.h"

// TEMPORARY: the scene classes whose headers are not written yet, as far as CreateScene needs them (the
// constructor, and the size it allocates). Each goes when its header (src/overlays/ovNNN/<Class>.h or
// src/scene/<Class>.h) is included by SceneManager.cpp instead (delete only its line: the blank lines keep
// deletions by different branches apart); the file goes when none is left.

#define SCENE_STAND_IN(name, size)          \
    class name : public Scene {             \
    public:                                 \
        name(int sceneId);                  \
        virtual void StartLoading();        \
        virtual void Init();                \
        virtual void Update();              \
        virtual void Draw();                \
        virtual void Exit();                \
        u8 mUnknown10[(size) - 0x10];       \
    }

















































#endif // SCENE_SCENESTANDINS_H
