#ifndef OV107_SAVEERRORSCENE_H
#define OV107_SAVEERRORSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "MessageWindow.h"
#include "Scene.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// --- The scene ----------------------------------------------------------------------------------------------------

// Overlay 107, scene 0xC2: the message that the save data could not be written ("データを書き込めませんでした。
// 電源を切って、カードを差込みなおしてください。", message 0x1A6 of the etc messages), in a message window on the
// lower screen, and nothing after it: the player has to turn the console off. The scenes that save (overlays 69 and
// 106) ask for scene 0xC2 when the main program's save function fails. The scene factory (main CreateScene, case
// 194) loads overlay 107, allocates 0xAFEC bytes and calls the constructor, which is in the main program
// (func_02049e40).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SaveErrorScene : public Scene {
public:
    SaveErrorScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    void UpdateWindow();
    BOOL ShowMessage(u32 message);

    /* 0x0010 */ int mState;
    /* 0x0014 */ MessageWindow mWindow[1];
    /* 0xAFB8 */ MessageFile mEtcMessages;
    /* 0xAFD0 */ MessageFile mCommonMessages;
    /* 0xAFE8 */ s8 mSubStep; // Of ShowMessage
    /* 0xAFE9 */ s8 mStep;
}; // 0xAFEC

#endif // OV107_SAVEERRORSCENE_H
