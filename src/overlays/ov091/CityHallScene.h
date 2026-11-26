#ifndef OV091_CITYHALLSCENE_H
#define OV091_CITYHALLSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// Overlay 91, scene 0x5E: the city hall. A menu of seven buttons over a model of the building, each leading to
// another scene (one of them only once a feature of the island is open, one only while the island has room for
// another resident), a button back, and a window that says the island is full. The scene factory (CreateScene)
// loads overlay 91 and allocates 0xEDF0 bytes; the constructor is the main program's (0x020451E8,
// src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214A980).
//
// The name is after the scene's archive, "Scn/Scene/SceneCityHall_LZ.bin", which no other overlay mounts, and its
// model "M00:/cityhall.imd"; it is not the original: no name of this class survives in the ROM.
class CityHallScene : public Scene {
public:
    // What the scene is doing (SetState)
    enum State {
        STATE_MENU,         // Waiting for a button
        STATE_WINDOW_OPEN,  // The window that says the island is full is open
        STATE_WINDOW_CLOSE, // It is closing
    };

    CityHallScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
    virtual ~CityHallScene();

    void SetState(int);

    /* 0x00010 */ MessageFile mMessageFiles[2]; // "TET:/tmd_etc_msg_JP.bmg", "TEC:/tmd_common_msg_JP.bmg"
    /* 0x00040 */ SpriteButton mButtons[7];
    /* 0x006EC */ TextButton mTextButtons[1]; // The way back
    /* 0x00DDC */ AnimModel mModel;          // The building, "M00:/cityhall.imd"
    /* 0x00EBC */ MessageText mTexts[8];
    /* 0x03E3C */ fx32 mScrollX;             // The upper screen's background scrolls by it
    /* 0x03E40 */ int mUnknown03E40;
    /* 0x03E44 */ MessageWindow mWindow;
    /* 0x0EDE8 */ int mState;
    /* 0x0EDEC */ int mUnknown0EDEC;        // SetState clears it; nothing in the overlay reads it
}; // 0xEDF0

#endif // OV091_CITYHALLSCENE_H
