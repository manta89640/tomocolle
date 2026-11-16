#ifndef OV103_SCENEB3_H
#define OV103_SCENEB3_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "G3Sprite.h"
#include "HeadIcon.h"
#include "Model.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 103, scene 0xB3 (not decompiled). The scene factory (CreateScene) loads overlay 103 and allocates 0x1BBE8
// bytes; the constructor is the main program's (0x0204909C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214DDA4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Misc/divo_3D_LZ.bin", "Msg/TmdCommonMsgJP_LZ.bin",
// "Msg/TmdTroubleMsgJP_LZ.bin", "Msg/TmdEtcMsgJP_LZ.bin", "Scn/Scene/SceneMiiTouch_LZ.bin", "Scn/Scene/Sky_LZ.bin",
// "Mdl/Spl/SplRoomEffect_LZ.bin", "Mdl/Ica/ica_mii_event_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A character of the scene: the main program's Character with 0x44 bytes and a head icon after it. Its implicit constructor
// (CharacterBase's C2, CharacterModel's C2 for the second base, HeadIcon's C1 for the member) and destructor are the
// main program's copies after the scene's constructor, emitted for the scene's __cxa_vec_ctor.
//
// The name is descriptive, not the original.
class SceneB3Character : public Character {
public:
    /* 0x2B18 */ u8 mUnknown2B18[0x44];
    /* 0x2B5C */ HeadIcon mHeadIcon;
}; // 0x2B9C

class SceneB3 : public Scene {
public:
    SceneB3(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x18];
    /* 0x00028 */ AnimModel mModel;
    /* 0x00108 */ u8 mUnknown00108[0x34];
    /* 0x0013C */ G3Sprite mG3Sprite;
    /* 0x00188 */ TtsPlayer mTtsPlayer;
    /* 0x14ACC */ MessageText mText;
    /* 0x150BC */ u8 mUnknown150BC[0x8];
    /* 0x150C4 */ MessageText mTexts[1];
    /* 0x156B4 */ u8 mUnknown156B4[0x14];
    /* 0x156C8 */ MessageFile mMessageFiles[3];
    /* 0x15710 */ u8 mUnknown15710[0xF8];
    /* 0x15808 */ ObjTextBox mTextBoxes[4];
    /* 0x15EF8 */ u8 mUnknown15EF8[0x4];
    /* 0x15EFC */ ObjTextBox mTextBoxes15EFC[3];
    /* 0x16430 */ u8 mUnknown16430[0x3C];
    /* 0x1646C */ SceneB3Character mCharacters[2];
    /* 0x1BBA4 */ ParticleSystem mParticles;
    /* 0x1BBA8 */ HeadIcon mHeadIcons[1];
}; // 0x1BBE8

#endif // OV103_SCENEB3_H
