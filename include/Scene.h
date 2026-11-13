#ifndef SCENE_H
#define SCENE_H

#include <nitro.h>

// The main program's scene base class (its constructor is inlined into each scene's; func_0204af20 runs a scene).
// Not decompiled: this is what the runner shows of it. The same definition as in overlay 74's RoomScene.h.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class Scene {
public:
    // What mPhase counts through
    enum Phase {
        PHASE_START_LOADING, // StartLoading() is called once
        PHASE_INIT,          // When the archives are loaded: Init(), Draw()
        PHASE_FADE_IN,       // Draw() while the screens fade in
        PHASE_RUN,           // Update(), Draw()
        PHASE_FADE_OUT,      // Draw() while the screens fade out (the scene manager sets this to leave the scene)
        PHASE_EXIT,          // When the archive loader is idle: Exit()
        PHASE_DONE,          // The scene manager deletes the scene
    };

    virtual void StartLoading() = 0;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Exit() = 0;
    virtual ~Scene() {}

    /* 0x04 */ int mSceneId;
    /* 0x08 */ int mPhase;
    /* 0x0C */ BOOL mSkipFade;
};

#endif // SCENE_H
