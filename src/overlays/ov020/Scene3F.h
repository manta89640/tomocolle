#ifndef OV020_SCENE3F_H
#define OV020_SCENE3F_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 20, scene 0x3F (not decompiled). The scene factory (CreateScene) loads overlay 20 and allocates 0x236F0
// bytes; the constructor is the main program's (0x020427BC, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B300). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Mdl/Misc/fountain_school_3D_LZ.bin",
// "Mdl/Spl/SplCollectBillsEffect_LZ.bin", "Scn/Scene/Sky_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene3F : public Scene {
public:
    Scene3F(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[4];
    /* 0x0AC70 */ u8 mUnknown0AC70[0x1BBC];
    /* 0x0C82C */ AnimModel mModels[2];
    /* 0x0C9EC */ u8 mUnknown0C9EC[0x50];
    /* 0x0CA3C */ SavedCharacter mSavedCharacter;
    /* 0x0CCFC */ u8 mUnknown0CCFC[0x39C];
    /* 0x0D098 */ TtsPlayer mTtsPlayer;
    /* 0x219DC */ MessageFile mMessageFiles[1];
    /* 0x219F4 */ u8 mUnknown219F4[0x8];
    /* 0x219FC */ MessageText mTexts[1];
    /* 0x21FEC */ u8 mUnknown21FEC[0x118];
    /* 0x22104 */ MessageText mText;
    /* 0x226F4 */ SpriteButton mButtons[1];
    /* 0x227E8 */ u8 mUnknown227E8[0xF08];
}; // 0x236F0

#endif // OV020_SCENE3F_H
