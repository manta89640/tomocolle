#ifndef OV010_SCENE2B_H
#define OV010_SCENE2B_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 10, scene 0x2B (not decompiled). The scene factory (CreateScene) loads overlay 10 and allocates 0x1CC70
// bytes; the constructor is the main program's (0x02040E20, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B5C0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin", "Mdl/Misc/change_3D_LZ.bin",
// "Mdl/Spl/SplNgEffect_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene2B : public Scene {
public:
    Scene2B(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1BF8];
    /* 0x07238 */ AnimModel mModels[3];
    /* 0x074D8 */ u8 mUnknown074D8[0x88];
    /* 0x07560 */ TtsPlayer mTtsPlayer;
    /* 0x1BEA4 */ SpriteButton mButtons[1];
    /* 0x1BF98 */ u8 mUnknown1BF98[0xC];
    /* 0x1BFA4 */ MessageFile mMessageFiles[1];
    /* 0x1BFBC */ u8 mUnknown1BFBC[0x8];
    /* 0x1BFC4 */ MessageText mTexts[1];
    /* 0x1C5B4 */ u8 mUnknown1C5B4[0x98];
    /* 0x1C64C */ MessageText mText;
    /* 0x1CC3C */ u8 mUnknown1CC3C[0x34];
}; // 0x1CC70

#endif // OV010_SCENE2B_H
