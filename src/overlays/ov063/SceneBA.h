#ifndef OV063_SCENEBA_H
#define OV063_SCENEBA_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 63, scenes 0xBA, 0xBB, 0xBC, 0xBD, 0xBE (not decompiled). The scene factory (CreateScene) loads overlay 63
// and allocates 0x1A210 bytes; the constructor is the main program's (0x02049B90, src/scene/SceneManager.cpp), which
// stores the overlay's virtual table (0x0214A904). The members are as the constructor constructs them, by their
// classes; what lies between is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Mdl/Misc/commendation_3D_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class SceneBA : public Scene {
public:
    SceneBA(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1CCC];
    /* 0x047F4 */ TtsPlayer mTtsPlayer;
    /* 0x19138 */ MessageFile mMessageFiles[2];
    /* 0x19168 */ u8 mUnknown19168[0x8];
    /* 0x19170 */ MessageText mTexts[1];
    /* 0x19760 */ u8 mUnknown19760[0x118];
    /* 0x19878 */ MessageText mText;
    /* 0x19E68 */ u8 mUnknown19E68[0xE0];
    /* 0x19F48 */ AnimModel mModels[2];
    /* 0x1A108 */ SpriteButton mButtons[1];
    /* 0x1A1FC */ u8 mUnknown1A1FC[0x14];
}; // 0x1A210

#endif // OV063_SCENEBA_H
