#ifndef OV043_SCENE81_H
#define OV043_SCENE81_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "keyboard/SoftKeyboard.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 43, scene 0x81 (not decompiled). The scene factory (CreateScene) loads overlay 43 and allocates 0x29594
// bytes; the constructor is the main program's (0x02046ABC, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BA60). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Msg/TmdCommonMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Cmn/Kbd/SoftKeyboard_LZ.bin", "Mdl/Misc/question_3D_LZ.bin", "Mdl/Spl/SplCollectBillsEffect_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene81 : public Scene {
public:
    Scene81(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[4];
    /* 0x0AC70 */ u8 mUnknown0AC70[0x1BD4];
    /* 0x0C844 */ TtsPlayer mTtsPlayer;
    /* 0x21188 */ u8 mUnknown21188[0x158];
    /* 0x212E0 */ SpriteButton mButtons[1];
    /* 0x213D4 */ u8 mUnknown213D4[0xC];
    /* 0x213E0 */ MessageFile mMessageFiles[2];
    /* 0x21410 */ u8 mUnknown21410[0x8];
    /* 0x21418 */ MessageText mTexts[1];
    /* 0x21A08 */ u8 mUnknown21A08[0x14];
    /* 0x21A1C */ MessageText mText;
    /* 0x2200C */ u8 mUnknown2200C[0x104];
    /* 0x22110 */ SoftKeyboard mKeyboard;
    /* 0x28108 */ u8 mUnknown28108[0x4];
    /* 0x2810C */ MessageText mTexts2810C[3];
    /* 0x292DC */ ObjCharBox mCharBox;
    /* 0x29498 */ u8 mUnknown29498[0x1C];
    /* 0x294B4 */ AnimModel mModel;
}; // 0x29594

#endif // OV043_SCENE81_H
