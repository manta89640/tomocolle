#ifndef OV001_NAMESORTTESTSCENE_H
#define OV001_NAMESORTTESTSCENE_H

#include <nitro.h>

#include "character/Character.h"
#include "Scene.h"

// The scene of overlay 1 (scene id 0x0C): a test of sorting eight saved characters by name (MATH_QSort with the
// main program's name comparators, also on a thread). The layout is what its constructor (in SceneManager.cpp)
// shows. The name is descriptive, not the original.
class NameSortTestScene : public Scene {
public:
    NameSortTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // Overlay 1 (src/overlays/ov001/NameSortTestScene.cpp): what is left of two debug prints
    void PrintText(const wchar_t *text, BOOL unknown);
    void PrintName(int line, const wchar_t *name, int length);

    // What mStep is
    enum Step {
        STEP_IDLE,    // Waiting for a button
        STEP_SORTING, // SortThread is sorting the characters by their Mii's names
    };

    /* 0x0010 */ int mStep;
    /* 0x0014 */ wchar_t mText[32];        // A test string, which A turns to upper case
    /* 0x0054 */ SavedCharacter mCharacters[8];
    /* 0x1654 */ u8 mSortStack[0x320];     // MATH_QSort's stack, for Y's sort (the thread's sort passes none)
    /* 0x1974 */ u8 mThreadStack[0xA00];
    /* 0x2374 */ OSThread mThread;         // Runs SortThread
}; // 0x2434

#endif // OV001_NAMESORTTESTSCENE_H
