#ifndef OV017_SCENE3C_H
#define OV017_SCENE3C_H

#include <nitro.h>

#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 17, scene 0x3C (not decompiled). The scene factory (CreateScene) loads overlay 17 and allocates 0x1F8B4
// bytes; the constructor is the main program's (0x020423E4, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AF90). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A model of the scene, of 0x144 bytes: the main program's AnimModel and something after it. Its constructor is the
// overlay's (0x0214A838); its destructor, which only destroys the model, is the main program's (after the scene's
// constructor), emitted for the scene's __cxa_vec_ctor. A member, not a base: the destructor stores only the model's
// tables.
//
// The name is descriptive, not the original.
class Scene3CModel {
public:
    Scene3CModel();

    /* 0x000 */ AnimModel mModel;
    /* 0x0E0 */ u8 mUnknown0E0[0x64];
}; // 0x144

class Scene3C : public Scene {
public:
    Scene3C(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1BBC];
    /* 0x071FC */ Scene3CModel mModels71FC[30];
    /* 0x097F4 */ u8 mUnknown097F4[0xC0];
    /* 0x098B4 */ TtsPlayer mTtsPlayer;
    /* 0x1E1F8 */ u8 mUnknown1E1F8[0x8];
    /* 0x1E200 */ SpriteButton mButtons[1];
    /* 0x1E2F4 */ u8 mUnknown1E2F4[0xC];
    /* 0x1E300 */ MessageFile mMessageFiles[1];
    /* 0x1E318 */ u8 mUnknown1E318[0x8];
    /* 0x1E320 */ MessageText mTexts[1];
    /* 0x1E910 */ u8 mUnknown1E910[0x98];
    /* 0x1E9A8 */ MessageText mText;
    /* 0x1EF98 */ u8 mUnknown1EF98[0xC];
    /* 0x1EFA4 */ Room mRoom;
    /* 0x1F8AC */ u8 mUnknown1F8AC[0x8];
}; // 0x1F8B4

#endif // OV017_SCENE3C_H
