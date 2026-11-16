#ifndef OV027_SCENE4F_H
#define OV027_SCENE4F_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 27, scene 0x4F (not decompiled). The scene factory (CreateScene) loads overlay 27 and allocates 0x317D4
// bytes; the constructor is the main program's (0x02043E80, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A804). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Scn/Scene/nagashisoumen_LZ.bin", "job/job010_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene4F : public Scene {
public:
    Scene4F(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ CharacterBase mCharacterBases[11];
    /* 0x1A0DC */ u8 mUnknown1A0DC[0x1DBC];
    /* 0x1BE98 */ AnimModel mModels[2];
    /* 0x1C058 */ u8 mUnknown1C058[0x84];
    /* 0x1C0DC */ TtsPlayer mTtsPlayer;
    /* 0x30A20 */ u8 mUnknown30A20[0x24];
    /* 0x30A44 */ MessageFile mMessageFiles[1];
    /* 0x30A5C */ u8 mUnknown30A5C[0x8];
    /* 0x30A64 */ MessageText mTexts[1];
    /* 0x31054 */ u8 mUnknown31054[0x98];
    /* 0x310EC */ MessageText mText;
    /* 0x316DC */ SpriteButton mButtons[1];
    /* 0x317D0 */ u8 mUnknown317D0[0x4];
}; // 0x317D4

#endif // OV027_SCENE4F_H
