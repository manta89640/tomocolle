#ifndef OV053_SCENE91_H
#define OV053_SCENE91_H

#include <nitro.h>

#include "Scene.h"
#include "character/Character.h"
#include "item/Item.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 53, scene 0x91 (not decompiled). The scene factory (CreateScene) loads overlay 53 and allocates 0x1F33C
// bytes; the constructor is the main program's (0x020476F8, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AED8). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Mdl/Misc/zazen_3D_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A model of the scene with a character and an item model of its own: a class of overlay 53 derived from AnimModel
// (its table at 0x0214AEBC). Its destructor is the main program's copy (0x020478FC, after the scene's constructor),
// to which the overlay's table refers; the deleting destructor is the overlay's.
//
// The name is descriptive, not the original.
class Scene91Model : public AnimModel {
public:
    /* 0x00E0 */ u8 mUnknown00E0[8];
    /* 0x00E8 */ CharacterBase mCharacters[1];
    /* 0x26CC */ ItemModel mItemModels[1];
}; // 0x27F8

class Scene91 : public Scene {
public:
    Scene91(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[2];
    /* 0x05640 */ u8 mUnknown05640[0x1C58];
    /* 0x07298 */ TtsPlayer mTtsPlayer;
    /* 0x1BBDC */ SpriteButton mButtons[1];
    /* 0x1BCD0 */ u8 mUnknown1BCD0[0xC];
    /* 0x1BCDC */ MessageFile mMessageFiles[1];
    /* 0x1BCF4 */ u8 mUnknown1BCF4[0x8];
    /* 0x1BCFC */ MessageText mTexts[1];
    /* 0x1C2EC */ u8 mUnknown1C2EC[0x98];
    /* 0x1C384 */ MessageText mText;
    /* 0x1C974 */ AnimModel mModel;
    /* 0x1CA54 */ AnimModel mModel1CA54;
    /* 0x1CB34 */ Scene91Model mModel1CB34;
    /* 0x1F32C */ u8 mUnknown1F32C[0x10];
}; // 0x1F33C

#endif // OV053_SCENE91_H
