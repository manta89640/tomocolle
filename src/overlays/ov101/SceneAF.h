#ifndef OV101_SCENEAF_H
#define OV101_SCENEAF_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "HeadIcon.h"
#include "SpriteButton.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 101, scene 0xAF (not decompiled). The scene factory (CreateScene) loads overlay 101 and allocates 0x1BA80
// bytes; the constructor is the main program's (0x020489B4, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214D0C0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Msg/TmdCommonMsgJP_LZ.bin", "Msg/TmdTroubleMsgJP_LZ.bin",
// "Msg/TmdEtcMsgJP_LZ.bin", "Scn/Scene/SceneMiiTouch_LZ.bin", "Mdl/Spl/SplRoomEffect_LZ.bin",
// "Mdl/Ica/ica_mii_room_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A character of the scene: the main program's Character with 0x40 bytes and a head icon after it. Its implicit constructor
// (CharacterBase's C2, CharacterModel's C2 for the second base, HeadIcon's C1 for the member) and destructor are the
// main program's copies after the scene's constructor, emitted for the scene's __cxa_vec_ctor.
//
// The name is descriptive, not the original.
class SceneAFCharacter : public Character {
public:
    /* 0x2B18 */ u8 mUnknown2B18[0x40];
    /* 0x2B58 */ HeadIcon mHeadIcon;
}; // 0x2B98

class SceneAF : public Scene {
public:
    SceneAF(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x2C];
    /* 0x0003C */ Room mRoom;
    /* 0x00944 */ TtsPlayer mTtsPlayer;
    /* 0x15288 */ MessageText mText;
    /* 0x15878 */ u8 mUnknown15878[0x8];
    /* 0x15880 */ MessageText mTexts[1];
    /* 0x15E70 */ u8 mUnknown15E70[0x14];
    /* 0x15E84 */ MessageFile mMessageFiles[3];
    /* 0x15ECC */ SpriteButton mButton;
    /* 0x15FC0 */ SpriteButton mButton15FC0;
    /* 0x160B4 */ SpriteButton mButton160B4;
    /* 0x161A8 */ SpriteButton mButton161A8;
    /* 0x1629C */ u8 mUnknown1629C[0x68];
    /* 0x16304 */ SceneAFCharacter mCharacters[2];
    /* 0x1BA34 */ u8 mUnknown1BA34[0x8];
    /* 0x1BA3C */ ParticleSystem mParticles;
    /* 0x1BA40 */ HeadIcon mHeadIcons[1];
}; // 0x1BA80

#endif // OV101_SCENEAF_H
