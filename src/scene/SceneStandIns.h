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







SCENE_STAND_IN(DrawManTestScene, 0x272C); // func_0203e320

SCENE_STAND_IN(HumanTestScene, 0x1D8); // func_0203e394

SCENE_STAND_IN(ParticleTestScene, 0x53C); // func_0203e430

SCENE_STAND_IN(G3dTestScene, 0x5C2C); // func_0203e470

SCENE_STAND_IN(BgTextTestScene, 0x1D4); // func_0203e524

SCENE_STAND_IN(MessageTestScene, 0x2D84); // func_0203e564

SCENE_STAND_IN(KeyboardTestScene, 0x6AF8); // func_0203e620


SCENE_STAND_IN(Scene0F, 0x6334); // func_0203e748

SCENE_STAND_IN(TtsTestScene, 0x6A858); // func_0203e9ec

SCENE_STAND_IN(EmptyScene, 0x10); // func_0203ea2c



SCENE_STAND_IN(RoomScene, 0x1D7C8); // _ZN9RoomSceneC1Ev

SCENE_STAND_IN(TutorialRoomScene, 0x134E8); // _ZN17TutorialRoomSceneC1Ev

SCENE_STAND_IN(Scene19, 0x1F7A8); // func_0203f220

SCENE_STAND_IN(FriendListScene, 0x7A3B8); // func_0203f350

SCENE_STAND_IN(WirelessTestScene, 0xCC); // func_0203f654

SCENE_STAND_IN(TestNg20Scene, 0x19FF8); // func_0203f694

SCENE_STAND_IN(ChangeClothesScene, 0xD070); // func_0203f784

SCENE_STAND_IN(Scene1E, 0x48F58); // func_0203f900

SCENE_STAND_IN(RightJobScene, 0x51730); // func_0203fd6c

SCENE_STAND_IN(RightJob2Scene, 0x34520); // func_0203ffc8

SCENE_STAND_IN(VoteScene, 0x3CD5C); // func_020400f0

SCENE_STAND_IN(MiiListMenuScene, 0x2C0C0); // func_02040288

SCENE_STAND_IN(G3SpriteTestScene, 0x48B4); // func_0204036c

SCENE_STAND_IN(MiiListScene, 0x2AA44); // func_020403e8

SCENE_STAND_IN(TushinScene, 0x3F0B4); // _ZN11TushinSceneC1Ei

SCENE_STAND_IN(Scene2F, 0x474B0); // func_02041124

SCENE_STAND_IN(Scene35, 0x46D4C); // func_02041a04

SCENE_STAND_IN(ChangeRoomScene, 0x17BF8); // func_02041f80

SCENE_STAND_IN(TestNg55Scene, 0x1C3D8); // func_02042254

SCENE_STAND_IN(TestNg63Scene, 0x1CEA8); // func_02042634

SCENE_STAND_IN(Scene40, 0x6F908); // func_020428e0

SCENE_STAND_IN(MiiNewsScene, 0x3F258); // func_02043800

SCENE_STAND_IN(WriteSongScene, 0x81224); // func_02044c80

SCENE_STAND_IN(Scene5D, 0x31AE0); // func_02045114

SCENE_STAND_IN(Scene67, 0x3D940); // func_02045504

SCENE_STAND_IN(Scene68, 0x47854); // func_02045720

SCENE_STAND_IN(Scene7F, 0x22B8); // func_0204657c

SCENE_STAND_IN(AffinityScene, 0x76A98); // func_02046750

SCENE_STAND_IN(TestNg129Scene, 0x56F0); // func_020474c0

SCENE_STAND_IN(WeddingScene, 0x1CEE0); // _ZN12WeddingSceneC1Ev

SCENE_STAND_IN(ItemTradeScene, 0x28E0C); // func_02048b78

SCENE_STAND_IN(SceneB4, 0x202E8); // func_020492e0


#endif // SCENE_SCENESTANDINS_H
