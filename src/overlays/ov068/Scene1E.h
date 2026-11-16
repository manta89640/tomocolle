#ifndef OV068_SCENE1E_H
#define OV068_SCENE1E_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "HeadIcon.h"
#include "item/Item.h"
#include "MessageWindow.h"
#include "Model.h"
#include "shop/Shop.h"
#include "shop/ShopWindow.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 68 (scene id 0x1E): a shop, with the shops' item list (ShopItemListView) and price window
// (ShopWindow), three hundred clothes items, a speech player, a character, models and texts. The overlay is not
// decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name
// of this class survives in the ROM.
class Scene1E : public Scene {
public:
    Scene1E(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // The main program's mannequin (main's include/item/Mannequin.h, 0x6B8 bytes: a head model and a body), as these
    // constructors show it: the body is built with CharacterModel's C2 (0x0208EB18) here and in the other shops'
    // constructors (0x020411B0, 0x02041AD4, 0x020457F0), so it is a base class, after a part that holds the head.
    struct MannequinHead {
        /* 0x000 */ AnimModel mHead;
        /* 0x0E0 */ u8 mUnknown0E0[0xC8];
    };
    struct ModelPair : public MannequinHead, public CharacterModel {
        /* 0x6B0 */ u8 mUnknown6B0[8];
    }; // 0x6B8

    // A character with its models
    struct CharacterPart {
        /* 0x0000 */ u8 mUnknown0000[0x3C];
        /* 0x003C */ SceneCharacter mCharacter;
        /* 0x2B54 */ ModelPair mModels;
    }; // 0x320C

    // The speech player, the character, models and texts, built through a pointer to their start
    struct Part {
        /* 0x00000 */ u8 mUnknown00000[0xC];
        /* 0x0000C */ TtsPlayer mTtsPlayer;
        /* 0x14950 */ u8 mUnknown14950[8];
        /* 0x14958 */ MessageText mTexts[1];
        /* 0x14F48 */ u8 mUnknown14F48[0x14];
        /* 0x14F5C */ CharacterPart mCharacterPart;
        /* 0x18168 */ u8 mUnknown18168[0x2C];
        /* 0x18194 */ MessageText mText;
        /* 0x18784 */ u8 mUnknown18784[0x158];
        /* 0x188DC */ ObjTextCanvas mCanvas1;
        /* 0x18A94 */ ObjTextCanvas mCanvas2;
        /* 0x18C4C */ ObjTextCanvas mCanvas3;
        /* 0x18E04 */ ObjTextCanvas mCanvas4;
        /* 0x18FBC */ u8 mUnknown18FBC[8];
        /* 0x18FC4 */ ModelPair mModels;
        /* 0x1967C */ Model mModel1;
        /* 0x19730 */ Model mModel2;
        /* 0x197E4 */ ParticleSystem mParticles;
        /* 0x197E8 */ HeadIcon mHeadIcons[1];
        /* 0x19828 */ u8 mUnknown19828[0x40];
    }; // 0x19868

    /* 0x00010 */ u8 mUnknown00010[0x158];
    /* 0x00168 */ MessageFile mMessageFiles[4];
    /* 0x001C8 */ Part mPart;
    /* 0x19A30 */ ClothesItem mItems[300];
    /* 0x1B650 */ u8 mUnknown1B650[0x96C];
    /* 0x1BFBC */ SpriteButton mButtons[5];
    /* 0x1C480 */ ShopItemListView mList;
    /* 0x30898 */ ShopWindow mShopWindow;
    /* 0x3BF60 */ MessageWindow mWindow;
    /* 0x46F04 */ SpriteButton mButtons2[2];
    /* 0x470EC */ TextButton mTextButtons[3];
    /* 0x485BC */ TextButton mTextButton;
    /* 0x48CAC */ SpriteButton mButton;
    /* 0x48DA0 */ ObjTextCanvas mCanvas;
}; // 0x48F58

#endif // OV068_SCENE1E_H
