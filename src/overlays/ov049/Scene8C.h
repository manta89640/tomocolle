#ifndef OV049_SCENE8C_H
#define OV049_SCENE8C_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 49, scene 0x8C (not decompiled). The scene factory (CreateScene) loads overlay 49 and allocates 0x1A75C
// bytes; the constructor is the main program's (0x02047240, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A120). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene8C : public Scene {
public:
    Scene8C(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C48];
    /* 0x04770 */ TtsPlayer mTtsPlayer;
    /* 0x190B4 */ SpriteButton mButtons[1];
    /* 0x191A8 */ u8 mUnknown191A8[0xC];
    /* 0x191B4 */ MessageFile mMessageFiles[1];
    /* 0x191CC */ u8 mUnknown191CC[0x8];
    /* 0x191D4 */ MessageText mTexts[1];
    /* 0x197C4 */ u8 mUnknown197C4[0x98];
    /* 0x1985C */ MessageText mText;
    /* 0x19E4C */ Room mRoom;
    /* 0x1A754 */ u8 mUnknown1A754[0x8];
}; // 0x1A75C

#endif // OV049_SCENE8C_H
