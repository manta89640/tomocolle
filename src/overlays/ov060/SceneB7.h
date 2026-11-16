#ifndef OV060_SCENEB7_H
#define OV060_SCENEB7_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 60, scene 0xB7 (not decompiled). The scene factory (CreateScene) loads overlay 60 and allocates 0x1A840
// bytes; the constructor is the main program's (0x0204984C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A5C0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Mdl/Misc/fureai_3D_LZ.bin",
// "Scn/Scene/SceneBG_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class SceneB7 : public Scene {
public:
    SceneB7(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C4C];
    /* 0x04774 */ TtsPlayer mTtsPlayer;
    /* 0x190B8 */ SpriteButton mButtons[1];
    /* 0x191AC */ u8 mUnknown191AC[0xC];
    /* 0x191B8 */ MessageFile mMessageFiles[1];
    /* 0x191D0 */ u8 mUnknown191D0[0x8];
    /* 0x191D8 */ MessageText mTexts[1];
    /* 0x197C8 */ u8 mUnknown197C8[0x98];
    /* 0x19860 */ MessageText mText;
    /* 0x19E50 */ Room mRoom;
    /* 0x1A758 */ u8 mUnknown1A758[0x8];
    /* 0x1A760 */ AnimModel mModels[1];
}; // 0x1A840

#endif // OV060_SCENEB7_H
