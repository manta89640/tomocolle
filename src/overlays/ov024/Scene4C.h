#ifndef OV024_SCENE4C_H
#define OV024_SCENE4C_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 24, scene 0x4C (not decompiled). The scene factory (CreateScene) loads overlay 24 and allocates 0x1D32C
// bytes; the constructor is the main program's (0x02043B64, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A840). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg44_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene4C : public Scene {
public:
    Scene4C(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1C50];
    /* 0x07290 */ TtsPlayer mTtsPlayer;
    /* 0x1BBD4 */ u8 mUnknown1BBD4[0xB0];
    /* 0x1BC84 */ SpriteButton mButtons[1];
    /* 0x1BD78 */ u8 mUnknown1BD78[0xC];
    /* 0x1BD84 */ MessageFile mMessageFiles[1];
    /* 0x1BD9C */ u8 mUnknown1BD9C[0x8];
    /* 0x1BDA4 */ MessageText mTexts[1];
    /* 0x1C394 */ u8 mUnknown1C394[0x98];
    /* 0x1C42C */ MessageText mText;
    /* 0x1CA1C */ Room mRoom;
    /* 0x1D324 */ u8 mUnknown1D324[0x8];
}; // 0x1D32C

#endif // OV024_SCENE4C_H
