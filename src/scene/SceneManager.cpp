// The scene manager (gSceneManager) and the scene factory, CreateScene, which constructs each scene by its id.
//
// The constructors of the scenes are in this file, after CreateScene, in the order of the scene ids, each followed
// by the implicit destructors of its members that it needs: this file holds every scene class's constructor,
// including those of the scenes whose other functions are in an overlay. The classes are declared in the headers
// beside the code of each scene (src/overlays/ovNNN/, src/scene/), against the main program's classes in include/.

#include <nitro.h>

#include "wireless/WirelessSession.h"

// ---- The scene manager and CreateScene (gap38-a) ----------------------------------------------------------------

WirelessSession *gWirelessSession;

// ---- Constructors 0x0203de78..0x02040578: scenes up to 0x26 (gap38-a) --------------------------------------------

// ---- Constructors 0x02040578..0x0204830c (gap38-b) ---------------------------------------------------------------

#include "MapModelsScene.h"
#include "../overlays/ov010/Scene2B.h"
#include "../overlays/ov069/SavePlayScene.h"
#include "../overlays/ov081/Scene30.h"
#include "../overlays/ov082/Scene33.h"
#include "../overlays/ov013/TestNg48Scene.h"
#include "../overlays/ov014/TestNg50Scene.h"
#include "../overlays/ov106/PreStartScene.h"
#include "../overlays/ov018/TestNg62Scene.h"
#include "../overlays/ov020/Scene3F.h"
#include "../overlays/ov076/MiiEditScene.h"
#include "../overlays/ov022/Scene45.h"
#include "../overlays/ov023/Scene4B.h"
#include "../overlays/ov025/Scene4D.h"
#include "../overlays/ov026/TestNg87Scene.h"
#include "../overlays/ov027/Scene4F.h"
#include "../overlays/ov029/Scene51.h"
#include "../overlays/ov030/TestNg94Scene.h"
#include "../overlays/ov089/Scene56.h"
#include "../overlays/ov034/TestNg104Scene.h"
#include "../overlays/ov035/Scene58.h"
#include "../overlays/ov090/RomanceScene.h"
#include "../overlays/ov072/MapScene.h"
#include "../overlays/ov093/Scene72.h"
#include "../overlays/ov094/Scene74.h"
#include "../overlays/ov039/TestNg116Scene.h"
#include "../overlays/ov043/Scene81.h"
#include "../overlays/ov045/Scene88.h"
#include "../overlays/ov052/Scene8F.h"

// Scene 0x27
MapModelsScene::MapModelsScene(int sceneId) : Scene(sceneId) {}

// Scene 0x2B, overlay 10
Scene2B::Scene2B(int sceneId) : Scene(sceneId) {}

// Scenes 0x2C and 0x2D, overlay 69
SavePlayScene::SavePlayScene(int sceneId) : Scene(sceneId) {
    mAnimationCount = 0;
    mResidentCount = 0;
}

SavePlayCharacter::SavePlayCharacter() {}

SavePlayCharacter::~SavePlayCharacter() {}

// Scenes 0x30 and 0x70, overlay 81
Scene30::Scene30(int sceneId) : Scene(sceneId) {}

// Scenes 0x33 and 0x73, overlay 82
Scene33::Scene33(int sceneId) : Scene(sceneId) {}

// Scene 0x36, overlay 13
TestNg48Scene::TestNg48Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x37, overlay 14
TestNg50Scene::TestNg50Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x39, overlay 106
PreStartScene::PreStartScene(int sceneId) : Scene(sceneId) {
    mResidentCount = 0;
}

// Scene 0x3D, overlay 18
TestNg62Scene::TestNg62Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x3F, overlay 20
Scene3F::Scene3F(int sceneId) : Scene(sceneId) {}

// Scenes 0x42, 0x43, 0x44 and 0x6F, overlay 76
MiiEditScene::MiiEditScene(int sceneId) : Scene(sceneId) {}

// Scene 0x45, overlay 22
Scene45::Scene45(int sceneId) : Scene(sceneId) {}

// Scene 0x4B, overlay 23
Scene4B::Scene4B(int sceneId) : Scene(sceneId) {}

// Scene 0x4D, overlay 25
Scene4D::Scene4D(int sceneId) : Scene(sceneId) {}

// Scene 0x4E, overlay 26
TestNg87Scene::TestNg87Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x4F, overlay 27
Scene4F::Scene4F(int sceneId) : Scene(sceneId) {}

// Scene 0x51, overlay 29
Scene51::Scene51(int sceneId) : Scene(sceneId) {}

// Scene 0x52, overlay 30
TestNg94Scene::TestNg94Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x56, overlay 89
Scene56::Scene56(int sceneId) : Scene(sceneId) {}

// Scene 0x57, overlay 34
TestNg104Scene::TestNg104Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x58, overlay 35
Scene58::Scene58(int sceneId) : Scene(sceneId) {}

// Scene 0x5A, overlay 90
RomanceScene::RomanceScene(int sceneId) : Scene(sceneId) {}

// Scenes 0x5F to 0x66, 0x6A, 0x6D and 0x6E, overlay 72
MapScene::MapScene(int sceneId) : Scene(sceneId) {}

// Scene 0x72, overlay 93
Scene72::Scene72(int sceneId) : Scene(sceneId) {}

// Scenes 0x74 and 0x7E, overlay 94
Scene74::Scene74(int sceneId) : Scene(sceneId) {}

// Scenes 0x77, 0x78 and 0x79, overlay 39
TestNg116Scene::TestNg116Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x81, overlay 43
Scene81::Scene81(int sceneId) : Scene(sceneId) {}

// Scene 0x88, overlay 45
Scene88::Scene88(int sceneId) : Scene(sceneId) {}

// Scenes 0x8F and 0x90, overlay 52
Scene8F::Scene8F(int sceneId) : Scene(sceneId) {}

// ---- Constructors and the rest, 0x0204830c..0x0204af20 (gap38-c) -------------------------------------------------
