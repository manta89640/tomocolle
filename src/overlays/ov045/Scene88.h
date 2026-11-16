#ifndef OV045_SCENE88_H
#define OV045_SCENE88_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 45, scene 0x88 (not decompiled). The scene factory (CreateScene) loads overlay 45 and allocates 0x1A074
// bytes; the constructor is the main program's (0x02046D50, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AE80). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg105_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene88 : public Scene {
public:
    Scene88(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C58];
    /* 0x04780 */ TtsPlayer mTtsPlayer;
    /* 0x190C4 */ u8 mUnknown190C4[0x16C];
    /* 0x19230 */ SpriteButton mButtons[1];
    /* 0x19324 */ u8 mUnknown19324[0xC];
    /* 0x19330 */ MessageFile mMessageFiles[1];
    /* 0x19348 */ u8 mUnknown19348[0x8];
    /* 0x19350 */ MessageText mTexts[1];
    /* 0x19940 */ u8 mUnknown19940[0x98];
    /* 0x199D8 */ MessageText mText;
    /* 0x19FC8 */ u8 mUnknown19FC8[0xAC];
}; // 0x1A074

#endif // OV045_SCENE88_H
