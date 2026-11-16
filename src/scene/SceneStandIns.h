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



































SCENE_STAND_IN(Scene2F, 0x474B0); // 0x02041124

SCENE_STAND_IN(Scene35, 0x46D4C); // 0x02041a04






SCENE_STAND_IN(Scene68, 0x47854); // 0x02045720




SCENE_STAND_IN(ItemTradeScene, 0x28E0C); // 0x02048b78



#endif // SCENE_SCENESTANDINS_H
