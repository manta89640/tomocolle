#ifndef OV050_SCENE8D_H
#define OV050_SCENE8D_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 50, scene 0x8D (not decompiled). The scene factory (CreateScene) loads overlay 50 and allocates 0x1C534
// bytes; the constructor is the main program's (0x0204733C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A4A0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Mdl/Misc/road_3D_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A character of the scene: the main program's CharacterBase and 0x18 bytes more. Its implicit constructor and
// destructor (CharacterBase's C2 and D2, and nothing else) are the main program's copies after the scene's
// constructor (0x02047498, 0x020474AC), emitted for the scene's __cxa_vec_ctor.
//
// The name is descriptive, not the original.
class Scene8DCharacter : public CharacterBase {
public:
    /* 0x25E4 */ u8 mUnknown25E4[0x18];
}; // 0x25FC

class Scene8D : public Scene {
public:
    Scene8D(int sceneId);

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
    /* 0x19E4C */ Scene8DCharacter mCharacters19E4C[1];
    /* 0x1C448 */ u8 mUnknown1C448[0xC];
    /* 0x1C454 */ AnimModel mModel;
}; // 0x1C534

#endif // OV050_SCENE8D_H
