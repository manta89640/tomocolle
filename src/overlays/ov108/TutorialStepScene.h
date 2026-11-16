#ifndef OV108_TUTORIALSTEPSCENE_H
#define OV108_TUTORIALSTEPSCENE_H

#include <nitro.h>

#include "Scene.h"

// Overlay 108, scene 0x69: the tutorial's way from one step to the next. It has no state, archives or drawing of its
// own: StartLoading advances the tutorial's step (gSceneMemory.mTutorialStep, up to 24) and asks for the scene of
// the new step, and Init, Update, Draw and Exit do nothing. Each scene of the tutorial asks for scene 0x69 when its
// step is done: the first start's clock setting (overlay 98, scene 0xAD), the island's visits (overlay 72), the
// tutorial's apartment (overlay 73), the Mii editor (overlay 76), and PreStartScene (overlay 106) to begin. The
// scene factory (main CreateScene, case 0x69) loads overlay 108 and allocates 0x10 bytes; the constructor is the
// main program's (0x02045B44, src/scene/SceneManager.cpp).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class TutorialStepScene : public Scene {
public:
    TutorialStepScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
}; // 0x10

#endif // OV108_TUTORIALSTEPSCENE_H
