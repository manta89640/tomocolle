#ifndef OV090_ROMANCESCENE_H
#define OV090_ROMANCESCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 90, scene 0x5A (not decompiled). The scene factory (CreateScene) loads overlay 90 and allocates 0x1AA58
// bytes; the constructor is the main program's (0x02044BA0, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x02156964). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "".
//
// The name is after the scene's archive, "Scn/Scene/SceneRomance_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class RomanceScene : public Scene {
public:
    RomanceScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0xC];
    /* 0x0001C */ SceneCharacter mCharacters[2];
    /* 0x0564C */ u8 mUnknown0564C[0x50];
    /* 0x0569C */ MessageFile mMessageFile;
    /* 0x056B4 */ TtsPlayer mTtsPlayer;
    /* 0x19FF8 */ u8 mUnknown19FF8[0x8];
    /* 0x1A000 */ MessageText mTexts[1];
    /* 0x1A5F0 */ u8 mUnknown1A5F0[0x14];
    /* 0x1A604 */ AnimModel mModel;
    /* 0x1A6E4 */ u8 mUnknown1A6E4[0x374];
}; // 0x1AA58

#endif // OV090_ROMANCESCENE_H
