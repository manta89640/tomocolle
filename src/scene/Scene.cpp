#include "MainProgram.h"

// The scene base class's one function of its own: the scene manager calls it once a frame for the current scene,
// and it runs the scene through its phases.

extern "C" {
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader); // Whether no archive is being loaded
BOOL ScreenControl_FadeIn(ScreenControl *screens, int planes); // Fades the screens in: TRUE when done
BOOL ScreenControl_FadeOut(ScreenControl *screens, int planes); // Fades the screens out: TRUE when done
}

void Scene::Run() {
    switch (mPhase) {
    case PHASE_START_LOADING:
        StartLoading();
        mPhase++;
        break;
    case PHASE_INIT:
        if (!ArchiveManager_IsIdle(&gArchiveManager)) {
            return;
        }
        Init();
        Draw();
        mPhase++;
        break;
    case PHASE_FADE_IN:
        if (mSkipFade) {
            mPhase++;
        } else if (ScreenControl_FadeIn(&gScreenControl, 0x3F3F)) {
            mPhase++;
        }
        Draw();
        break;
    case PHASE_RUN:
        Update();
        Draw();
        break;
    case PHASE_FADE_OUT:
        if (mSkipFade) {
            mPhase++;
        } else if (ScreenControl_FadeOut(&gScreenControl, 0x3F3F)) {
            mPhase++;
        }
        Draw();
        break;
    case PHASE_EXIT:
        if (!ArchiveManager_IsIdle(&gArchiveManager)) {
            return;
        }
        Exit();
        mPhase++;
        break;
    }
}
