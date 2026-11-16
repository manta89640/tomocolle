#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <nitro.h>

#include "Scene.h"

// The scene manager, gSceneManager (src/scene/SceneManager.cpp): it runs the current scene each frame, and when
// the scene has finished leaving (Scene::PHASE_DONE) it deletes it, unloads its overlay, constructs the scene
// asked for (CreateScene, which loads that scene's overlay and sound group) and puts the graphics managers back
// in the state Init recorded. A scene asks for the next one with RequestScene.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class SceneManager {
public:
    SceneManager() { mSoundGroup = -1; }
    ~SceneManager();

    void Init();
    void Update();
    void RequestScene(int sceneId);
    Scene *CreateScene(int sceneId);
    void UnloadSceneOverlay(int sceneId);
    // Loads a sound group into the sound heap, keeping it if it is the one loaded: -1 unloads, -2 keeps what is
    // loaded
    void SetSoundGroup(int group);

    /* 0x000 */ Scene *mScene;
    /* 0x004 */ int mNextSceneId;     // What RequestScene asked for
    /* 0x008 */ int mPreviousSceneId; // The scene being left, whose overlay Update unloads
    // The state of three graphics managers as Init leaves them, which each new scene starts from
    /* 0x00C */ u8 mGraphicsState[0x140];      // gGraphicsManager's (func_02024b10 records, func_02024c14 restores)
    /* 0x14C */ u8 mScreenLoaderState[0x140];  // data_02121834's (func_02027ad0, func_02027c0c)
    /* 0x28C */ u8 mScreenControlState[0xD4];  // gScreenControl's (ScreenControl_SaveState, ScreenControl_RestoreState)
    /* 0x360 */ int mSoundGroup;               // The sound group loaded, -1 for none
}; // 0x364

extern SceneManager gSceneManager;

#endif // SCENEMANAGER_H
