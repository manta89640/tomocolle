#ifndef OV046_SCENE89_H
#define OV046_SCENE89_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 46, scene 0x89 (not decompiled). The scene factory (CreateScene) loads overlay 46 and allocates 0x267D4
// bytes; the constructor is the main program's (0x02046E40, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B900). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg117_LZ.bin", "Mdl/Misc/acchi_3D_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin",
// "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene89 : public Scene {
public:
    Scene89(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ SavedCharacter mSavedCharacter;
    /* 0x049A4 */ u8 mUnknown049A4[0xA0];
    /* 0x04A44 */ TtsPlayer mTtsPlayer;
    /* 0x19388 */ u8 mUnknown19388[0x610];
    /* 0x19998 */ TextButton mTextButtons[1];
    /* 0x1A088 */ u8 mUnknown1A088[0x8];
    /* 0x1A090 */ MessageFile mMessageFiles[1];
    /* 0x1A0A8 */ u8 mUnknown1A0A8[0x4];
    /* 0x1A0AC */ MessageWindow mWindow;
    /* 0x25050 */ u8 mUnknown25050[0x8];
    /* 0x25058 */ MessageText mTexts[1];
    /* 0x25648 */ u8 mUnknown25648[0x98];
    /* 0x256E0 */ MessageText mText;
    /* 0x25CD0 */ Room mRoom;
    /* 0x265D8 */ u8 mUnknown265D8[0x8];
    /* 0x265E0 */ SpriteButton mButtons[1];
    /* 0x266D4 */ u8 mUnknown266D4[0xC];
    /* 0x266E0 */ AnimModel mModel;
    /* 0x267C0 */ u8 mUnknown267C0[0x14];
}; // 0x267D4

#endif // OV046_SCENE89_H
