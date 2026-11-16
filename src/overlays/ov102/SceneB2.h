#ifndef OV102_SCENEB2_H
#define OV102_SCENEB2_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "HeadIcon.h"
#include "SpriteButton.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 102, scene 0xB2 (not decompiled). The scene factory (CreateScene) loads overlay 102 and allocates 0x1BA84
// bytes; the constructor is the main program's (0x02048EDC, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214CE20). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Msg/TmdCommonMsgJP_LZ.bin", "Msg/TmdTroubleMsgJP_LZ.bin",
// "Msg/TmdEtcMsgJP_LZ.bin", "Scn/Scene/SceneMiiTouch_LZ.bin", "Mdl/Spl/SplRoomEffect_LZ.bin",
// "Mdl/Ica/ica_mii_room_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A character of the scene: the main program's Character with 0x44 bytes and a head icon after it. Its implicit constructor
// (CharacterBase's C2, CharacterModel's C2 for the second base, HeadIcon's C1 for the member) and destructor are the
// main program's copies after the scene's constructor, emitted for the scene's __cxa_vec_ctor.
//
// The name is descriptive, not the original.
class SceneB2Character : public Character {
public:
    /* 0x2B18 */ u8 mUnknown2B18[0x44];
    /* 0x2B5C */ HeadIcon mHeadIcon;
}; // 0x2B9C

class SceneB2 : public Scene {
public:
    SceneB2(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x28];
    /* 0x00038 */ Room mRoom;
    /* 0x00940 */ TtsPlayer mTtsPlayer;
    /* 0x15284 */ MessageText mText;
    /* 0x15874 */ u8 mUnknown15874[0x8];
    /* 0x1587C */ MessageText mTexts[1];
    /* 0x15E6C */ u8 mUnknown15E6C[0x14];
    /* 0x15E80 */ MessageFile mMessageFiles[3];
    /* 0x15EC8 */ SpriteButton mButton;
    /* 0x15FBC */ SpriteButton mButton15FBC;
    /* 0x160B0 */ SpriteButton mButton160B0;
    /* 0x161A4 */ SpriteButton mButton161A4;
    /* 0x16298 */ u8 mUnknown16298[0x70];
    /* 0x16308 */ SceneB2Character mCharacters[2];
    /* 0x1BA40 */ ParticleSystem mParticles;
    /* 0x1BA44 */ HeadIcon mHeadIcons[1];
}; // 0x1BA84

#endif // OV102_SCENEB2_H
