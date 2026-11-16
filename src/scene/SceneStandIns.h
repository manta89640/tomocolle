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







SCENE_STAND_IN(DrawManTestScene, 0x272C); // 0x0203e320

SCENE_STAND_IN(HumanTestScene, 0x1D8); // 0x0203e394

SCENE_STAND_IN(ParticleTestScene, 0x53C); // 0x0203e430

SCENE_STAND_IN(G3dTestScene, 0x5C2C); // 0x0203e470

SCENE_STAND_IN(BgTextTestScene, 0x1D4); // 0x0203e524

SCENE_STAND_IN(MessageTestScene, 0x2D84); // 0x0203e564

SCENE_STAND_IN(KeyboardTestScene, 0x6AF8); // 0x0203e620



SCENE_STAND_IN(TtsTestScene, 0x6A858); // 0x0203e9ec

SCENE_STAND_IN(EmptyScene, 0x10); // 0x0203ea2c



SCENE_STAND_IN(RoomScene, 0x1D7C8); // _ZN9RoomSceneC1Ei

SCENE_STAND_IN(TutorialRoomScene, 0x134E8); // _ZN17TutorialRoomSceneC1Ei



SCENE_STAND_IN(WirelessTestScene, 0xCC); // 0x0203f654






SCENE_STAND_IN(VoteScene, 0x3CD5C); // 0x020400f0




SCENE_STAND_IN(TushinScene, 0x3F0B4); // _ZN11TushinSceneC1Ei

SCENE_STAND_IN(Scene2F, 0x474B0); // 0x02041124

SCENE_STAND_IN(Scene35, 0x46D4C); // 0x02041a04



SCENE_STAND_IN(Scene40, 0x6F908); // 0x020428e0



SCENE_STAND_IN(Scene68, 0x47854); // 0x02045720



SCENE_STAND_IN(WeddingScene, 0x1CEE0); // _ZN12WeddingSceneC1Ei

SCENE_STAND_IN(ItemTradeScene, 0x28E0C); // 0x02048b78

SCENE_STAND_IN(SceneB4, 0x202E8); // 0x020492e0


#endif // SCENE_SCENESTANDINS_H
