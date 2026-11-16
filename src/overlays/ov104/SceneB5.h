#ifndef OV104_SCENEB5_H
#define OV104_SCENEB5_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "HeadIcon.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 104, scene 0xB5 (not decompiled). The scene factory (CreateScene) loads overlay 104 and allocates 0x1C390
// bytes; the constructor is the main program's (0x0204963C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214DCC4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Msg/TmdCommonMsgJP_LZ.bin", "Msg/TmdTroubleMsgJP_LZ.bin",
// "Msg/TmdEtcMsgJP_LZ.bin", "Scn/Scene/SceneMiiTouch_LZ.bin", "Mdl/Spl/SplRoomEffect_LZ.bin",
// "Mdl/Ica/ica_mii_event_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A character of the scene: the main program's Character with 0x44 bytes and a head icon after it. Its implicit constructor
// (CharacterBase's C2, CharacterModel's C2 for the second base, HeadIcon's C1 for the member) and destructor are the
// main program's copies after the scene's constructor, emitted for the scene's __cxa_vec_ctor.
//
// The name is descriptive, not the original.
class SceneB5Character : public Character {
public:
    /* 0x2B18 */ u8 mUnknown2B18[0x44];
    /* 0x2B5C */ HeadIcon mHeadIcon;
}; // 0x2B9C

class SceneB5 : public Scene {
public:
    SceneB5(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x18];
    /* 0x00028 */ Room mRoom;
    /* 0x00930 */ TtsPlayer mTtsPlayer;
    /* 0x15274 */ MessageText mText;
    /* 0x15864 */ u8 mUnknown15864[0x8];
    /* 0x1586C */ MessageText mTexts[1];
    /* 0x15E5C */ u8 mUnknown15E5C[0x14];
    /* 0x15E70 */ MessageFile mMessageFiles[3];
    /* 0x15EB8 */ u8 mUnknown15EB8[0xF8];
    /* 0x15FB0 */ ObjTextBox mTextBoxes[4];
    /* 0x166A0 */ u8 mUnknown166A0[0x4];
    /* 0x166A4 */ ObjTextBox mTextBoxes166A4[3];
    /* 0x16BD8 */ u8 mUnknown16BD8[0x3C];
    /* 0x16C14 */ SceneB5Character mCharacters[2];
    /* 0x1C34C */ ParticleSystem mParticles;
    /* 0x1C350 */ HeadIcon mHeadIcons[1];
}; // 0x1C390

#endif // OV104_SCENEB5_H
