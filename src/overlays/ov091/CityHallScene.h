#ifndef OV091_CITYHALLSCENE_H
#define OV091_CITYHALLSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// Overlay 91, scene 0x5E (not decompiled). The scene factory (CreateScene) loads overlay 91 and allocates 0xEDF0
// bytes; the constructor is the main program's (0x020451E8, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A980). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Scn/Scene/SceneCityHall_LZ.bin", "Msg/TmdEtcMsgJP_LZ.bin",
// "Msg/TmdCommonMsgJP_LZ.bin", "Mdl/Misc/cityhall_3D_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneCityHall_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class CityHallScene : public Scene {
public:
    CityHallScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ MessageFile mMessageFiles[2];
    /* 0x00040 */ SpriteButton mButtons[7];
    /* 0x006EC */ TextButton mTextButtons[1];
    /* 0x00DDC */ AnimModel mModel;
    /* 0x00EBC */ MessageText mTexts[8];
    /* 0x03E3C */ u8 mUnknown03E3C[0x8];
    /* 0x03E44 */ MessageWindow mWindow;
    /* 0x0EDE8 */ u8 mUnknown0EDE8[0x8];
}; // 0xEDF0

#endif // OV091_CITYHALLSCENE_H
