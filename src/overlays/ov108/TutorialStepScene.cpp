#include "TutorialStepScene.h"
#include "MainProgram.h"

// Overlay 108: the tutorial's step from one scene to the next. One source file: TutorialStepScene and its virtual
// table, with the scene's implicit destructor at the end of .text.

// Advances the tutorial by a step and asks for the scene that plays it (steps 1 to 23; step 0 cannot occur). At
// the last (24), the tutorial is over: the first two saved characters that exist are freed of what they were doing
// and of their troubles, saved character 0 is given trouble 0 of category 9 without end, and the game goes on to
// the island (scene 0x5F).
// @0x02149a40
void TutorialStepScene::StartLoading() {
    u8 step = gSceneMemory.mTutorialStep;
    if (step < 24) {
        gSceneMemory.mTutorialStep = ++step;
    }

    switch (step) {
    case 1:
        gSceneManager.RequestScene(0xAD);
        break;
    case 2:
        gSceneManager.RequestScene(0x6D);
        break;
    case 3:
        gSceneManager.RequestScene(0x72);
        break;
    case 4:
        gSceneManager.RequestScene(0x6E);
        break;
    case 5:
        gSceneManager.RequestScene(0x6F);
        break;
    case 6:
        gSceneManager.RequestScene(0x7E);
        break;
    case 7:
        gSceneManager.RequestScene(0x6D);
        break;
    case 8:
        gSceneManager.RequestScene(0x6B);
        break;
    case 9:
        gSceneManager.RequestScene(0x6C);
        break;
    case 10:
        gSceneManager.RequestScene(0x6F);
        break;
    case 11:
        gSceneManager.RequestScene(0x7E);
        break;
    case 12:
        gSceneManager.RequestScene(0x6C);
        break;
    case 13:
        gSceneManager.RequestScene(0x6B);
        break;
    case 14:
        gSceneManager.RequestScene(0x7D);
        break;
    case 15:
        gSceneManager.RequestScene(0x6D);
        break;
    case 16:
        gSceneManager.RequestScene(0x6B);
        break;
    case 17:
        gSceneManager.RequestScene(0x6C);
        break;
    case 18:
        gSceneManager.RequestScene(0x6D);
        break;
    case 19:
        gSceneManager.RequestScene(0x71);
        break;
    case 20:
        gSceneManager.RequestScene(0x6D);
        break;
    case 21:
        gSceneManager.RequestScene(0x6B);
        break;
    case 22:
        gSceneManager.RequestScene(0x6C);
        break;
    case 23:
        gSceneManager.RequestScene(0x6D);
        break;
    case 24: {
        int found = 0;
        for (int i = 0; i < 100 && found < 2; i++) {
            if (func_0202c3dc(&gGameData, i)) {
                SavedCharacter resident;
                func_0202c264(&gGameData, i, &resident);
                resident.ClearLongAct();
                resident.ClearTrouble();
                func_0202c28c(&gGameData, i, &resident);
                found++;
            }
        }

        SavedCharacter first;
        func_0202c264(&gGameData, 0, &first);
        first.ClearTrouble();
        first.mTroubleId = 0;
        first.mTroubleCategory = 9;
        first.mTroubleState = 1;
        first.mTroubleEnd = 0x7FFFFFFFFFFFFFFF;
        func_0202c28c(&gGameData, 0, &first);
        gSceneManager.RequestScene(0x5F);
        break;
    }
    }
}

// @0x02149d84
void TutorialStepScene::Init() {}

// @0x02149d88
void TutorialStepScene::Update() {}

// @0x02149d8c
void TutorialStepScene::Draw() {}

// @0x02149d90
void TutorialStepScene::Exit() {}
