#ifndef OV097_MARRIAGE2SCENE_H
#define OV097_MARRIAGE2SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 97, scenes 0xC0, 0xC1 (not decompiled). The scene factory (CreateScene) loads overlay 97 and allocates
// 0x35BF0 bytes; the constructor is the main program's (0x02049D28, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x0214B03C). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene's StartLoading mounts "Scn/Scene/SceneMarriage2_LZ.bin",
// "Msg/TmdStaffMsgJP_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneMarriage2_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class Marriage2Scene : public Scene {
public:
    Marriage2Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ CharacterBase mCharacterBases[11];
    /* 0x1A0DC */ u8 mUnknown1A0DC[0x1DBC];
    /* 0x1BE98 */ AnimModel mModels[2];
    /* 0x1C058 */ u8 mUnknown1C058[0xC];
    /* 0x1C064 */ SavedCharacter mSavedCharacter;
    /* 0x1C324 */ SpriteButton mButtons[1];
    /* 0x1C418 */ u8 mUnknown1C418[0x44];
    /* 0x1C45C */ MessageFile mMessageFiles[1];
    /* 0x1C474 */ MessageText mTexts[13];
    /* 0x211A4 */ u8 mUnknown211A4[0x104];
    /* 0x212A8 */ TtsPlayer mTtsPlayer;
    /* 0x35BEC */ u8 mUnknown35BEC[0x4];
}; // 0x35BF0

#endif // OV097_MARRIAGE2SCENE_H
