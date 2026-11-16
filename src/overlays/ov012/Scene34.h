#ifndef OV012_SCENE34_H
#define OV012_SCENE34_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 12, scene 0x34 (not decompiled). The scene factory (CreateScene) loads overlay 12 and allocates 0x1D5A0
// bytes; the constructor is the main program's (0x02041908, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AA20). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene34 : public Scene {
public:
    Scene34(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x4];
    /* 0x00014 */ SceneCharacter mCharacters[2];
    /* 0x05644 */ u8 mUnknown05644[0x1DD8];
    /* 0x0741C */ TtsPlayer mTtsPlayer;
    /* 0x1BD60 */ SpriteButton mButtons[1];
    /* 0x1BE54 */ u8 mUnknown1BE54[0xC];
    /* 0x1BE60 */ MessageFile mMessageFiles[2];
    /* 0x1BE90 */ u8 mUnknown1BE90[0x8];
    /* 0x1BE98 */ MessageText mTexts[1];
    /* 0x1C488 */ u8 mUnknown1C488[0x218];
    /* 0x1C6A0 */ MessageText mText;
    /* 0x1CC90 */ Room mRoom;
    /* 0x1D598 */ u8 mUnknown1D598[0x8];
}; // 0x1D5A0

#endif // OV012_SCENE34_H
