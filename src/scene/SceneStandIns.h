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

SCENE_STAND_IN(Scene00, 0x1C); // func_0203de78


SCENE_STAND_IN(Scene02, 0x4DDC); // func_0203df24

SCENE_STAND_IN(Scene03, 0x1BC); // func_0203e0c4

SCENE_STAND_IN(Scene04, 0x2B38); // func_0203e11c

SCENE_STAND_IN(TestDrawMiiScene, 0x28F8); // func_0203e1e8

SCENE_STAND_IN(DrawManTestScene, 0x272C); // func_0203e320

SCENE_STAND_IN(HumanTestScene, 0x1D8); // func_0203e394

SCENE_STAND_IN(ParticleTestScene, 0x53C); // func_0203e430

SCENE_STAND_IN(G3dTestScene, 0x5C2C); // func_0203e470

SCENE_STAND_IN(BgTextTestScene, 0x1D4); // func_0203e524

SCENE_STAND_IN(MessageTestScene, 0x2D84); // func_0203e564

SCENE_STAND_IN(KeyboardTestScene, 0x6AF8); // func_0203e620

SCENE_STAND_IN(Scene0C, 0x2434); // func_0203e6f0

SCENE_STAND_IN(Scene0F, 0x6334); // func_0203e748

SCENE_STAND_IN(TtsTestScene, 0x6A858); // func_0203e9ec

SCENE_STAND_IN(EmptyScene, 0x10); // func_0203ea2c

SCENE_STAND_IN(Scene17, 0x39A8C); // func_0203ea54

SCENE_STAND_IN(Scene18, 0x1AA50); // func_0203ebf8

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

SCENE_STAND_IN(RankingScene, 0x2AC10); // func_020408a4

SCENE_STAND_IN(Scene2E, 0x924); // func_020410d4

SCENE_STAND_IN(Scene2F, 0x474B0); // func_02041124

SCENE_STAND_IN(Scene31, 0x1DC1C); // func_020416e4

SCENE_STAND_IN(Scene34, 0x1D5A0); // func_02041908

SCENE_STAND_IN(Scene35, 0x46D4C); // func_02041a04

SCENE_STAND_IN(ChangeRoomScene, 0x17BF8); // func_02041f80

SCENE_STAND_IN(TestNg55Scene, 0x1C3D8); // func_02042254

SCENE_STAND_IN(Scene3B, 0x1B0C4); // func_020422ec

SCENE_STAND_IN(Scene3C, 0x1F8B4); // func_020423e4

SCENE_STAND_IN(TestNg63Scene, 0x1CEA8); // func_02042634

SCENE_STAND_IN(Scene40, 0x6F908); // func_020428e0

SCENE_STAND_IN(MiiNewsScene, 0x3F258); // func_02043800

SCENE_STAND_IN(Scene4C, 0x1D32C); // func_02043b64

SCENE_STAND_IN(Scene50, 0x1A094); // func_02043f98

SCENE_STAND_IN(TestNg95Scene, 0x277C0); // func_02044330

SCENE_STAND_IN(Scene54, 0x1A330); // func_02044520

SCENE_STAND_IN(TestNg100Scene, 0x27880); // func_02044634

SCENE_STAND_IN(TestNg107Scene, 0x26588); // func_02044a20

SCENE_STAND_IN(WriteSongScene, 0x81224); // func_02044c80

SCENE_STAND_IN(Scene5D, 0x31AE0); // func_02045114

SCENE_STAND_IN(Scene67, 0x3D940); // func_02045504

SCENE_STAND_IN(Scene68, 0x47854); // func_02045720

SCENE_STAND_IN(Scene69, 0x10); // func_02045b44

SCENE_STAND_IN(Scene75, 0x27258); // func_02045dbc

SCENE_STAND_IN(TestNg111Scene, 0x19A24); // func_02046000

SCENE_STAND_IN(Scene7A, 0x25D90); // func_020461d4

SCENE_STAND_IN(TestNg119Scene, 0x1BE88); // func_02046434

SCENE_STAND_IN(Scene7F, 0x22B8); // func_0204657c

SCENE_STAND_IN(AffinityScene, 0x76A98); // func_02046750

SCENE_STAND_IN(TestNg121Scene, 0x1ACF8); // func_02046c48

SCENE_STAND_IN(Scene89, 0x267D4); // func_02046e40

SCENE_STAND_IN(TestNg125Scene, 0x1B8EC); // func_02047018

SCENE_STAND_IN(Scene8B, 0x52CC); // func_02047130

SCENE_STAND_IN(Scene8C, 0x1A75C); // func_02047240

SCENE_STAND_IN(Scene8D, 0x1C534); // func_0204733c

SCENE_STAND_IN(TestNg129Scene, 0x56F0); // func_020474c0

SCENE_STAND_IN(Scene91, 0x1F33C); // func_020476f8

SCENE_STAND_IN(TestNg133Scene, 0x26EBC); // func_02047974

SCENE_STAND_IN(TestNg134Scene, 0x36640); // func_02047ae8

SCENE_STAND_IN(TestNg136Scene, 0x3570C); // func_02047dd0

SCENE_STAND_IN(TestNg137Scene, 0x2E3D8); // func_020480d8

SCENE_STAND_IN(SceneA3, 0x1DB00); // func_0204830c

SCENE_STAND_IN(SceneAA, 0x1D3A8); // func_0204846c

SCENE_STAND_IN(WeddingScene, 0x1CEE0); // _ZN12WeddingSceneC1Ev

SCENE_STAND_IN(OptionScene, 0x26394); // func_020487e4

SCENE_STAND_IN(InformationScene, 0x51BC); // func_02048918

SCENE_STAND_IN(SceneAF, 0x1BA80); // func_020489b4

SCENE_STAND_IN(ItemTradeScene, 0x28E0C); // func_02048b78

SCENE_STAND_IN(SceneB2, 0x1BA84); // func_02048edc

SCENE_STAND_IN(SceneB3, 0x1BBE8); // func_0204909c

SCENE_STAND_IN(SceneB4, 0x202E8); // func_020492e0

SCENE_STAND_IN(SceneB5, 0x1C390); // func_0204963c

SCENE_STAND_IN(SceneB7, 0x1A840); // func_0204984c

SCENE_STAND_IN(SceneB8, 0x1A99C); // func_02049970

SCENE_STAND_IN(SceneB9, 0x1A760); // func_02049a94

SCENE_STAND_IN(SceneBA, 0x1A210); // func_02049b90

SCENE_STAND_IN(SceneBF, 0x128); // func_02049ca8

SCENE_STAND_IN(SceneC0, 0x35BF0); // func_02049d28

SCENE_STAND_IN(SaveErrorScene, 0xAFEC); // _ZN14SaveErrorSceneC1Ei


#endif // SCENE_SCENESTANDINS_H
