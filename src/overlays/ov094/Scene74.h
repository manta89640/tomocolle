#ifndef OV094_SCENE74_H
#define OV094_SCENE74_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// Overlay 94, scenes 0x74, 0x7E (not decompiled). The scene factory (CreateScene) loads overlay 94 and allocates
// 0x6D78 bytes; the constructor is the main program's (0x02045CC0, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x0214B020). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene's StartLoading mounts "Scn/Scene/SceneCharDistribution_LZ.bin",
// "Msg/TmdEtcMsgJP_LZ.bin", "Mdl/Ica/ica_mii_contents_LZ.bin", "Msg/TmdCommonMsgJP_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A part of the scene with its own members, which the constructor constructs from the part's address: a class of
// its own, constructed inline (what it is is not known).
struct Scene74Part {
    /* 0x0000 */ u8 mUnknown0000[0x174];
    /* 0x0174 */ MessageFile mMessageFile;
    /* 0x018C */ u8 mUnknown018C[0x4];
    /* 0x0190 */ MessageText mText0190;
    /* 0x0780 */ MessageText mText0780;
    /* 0x0D70 */ MessageText mText0D70;
    /* 0x1360 */ MessageText mText1360;
    /* 0x1950 */ u8 mUnknown1950[0x30];
}; // 0x1980

class Scene74 : public Scene {
public:
    Scene74(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x4];
    /* 0x00014 */ SceneCharacter mCharacters[1];
    /* 0x02B2C */ u8 mUnknown02B2C[0x1BBC];
    /* 0x046E8 */ MessageFile mMessageFiles[2];
    /* 0x04718 */ MessageText mTexts[1];
    /* 0x04D08 */ TextButton mTextButtons[1];
    /* 0x053F8 */ Scene74Part mPart;
}; // 0x6D78

#endif // OV094_SCENE74_H
