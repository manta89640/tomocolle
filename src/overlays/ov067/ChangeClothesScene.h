#ifndef OV067_CHANGECLOTHESSCENE_H
#define OV067_CHANGECLOTHESSCENE_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "Scene.h"

// The scene of overlay 67 (scene id 0x1D), which mounts Scn/Scene/SceneChangeClothes_LZ.bin: a character, the 300
// clothes items, buttons, message files and texts. The overlay is not decompiled: this is the layout its
// constructor (in SceneManager.cpp) shows; the constructor builds the parts at 0x2C34, 0x50E4 and 0x6020 through
// pointers to their starts, which members of members give. The name is after its archive.
class ChangeClothesScene : public Scene {
public:
    ChangeClothesScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    struct ButtonsPart {
        /* 0x000 */ u8 mUnknown000[0xAC];
        /* 0x0AC */ SpriteButton mButton;
        /* 0x1A0 */ u8 mUnknown1A0[0xAC];
        /* 0x24C */ SpriteButton mButtons[7];
        /* 0x8F8 */ MessageText mText;
        /* 0xEE8 */ u8 mUnknownEE8[0x38];
        /* 0xF20 */ MessageFile mMessageFile;
        /* 0xF38 */ u8 mUnknownF38[4];
    }; // 0xF3C

    struct TextsPart {
        /* 0x0000 */ u8 mUnknown0000[0x10];
        /* 0x0010 */ MessageFile mMessageFile;
        /* 0x0028 */ SpriteButton mButtons[8];
        /* 0x07C8 */ MessageText mTexts[8];
        /* 0x3748 */ ObjTextCanvas mCanvases[8];
        /* 0x4508 */ u8 mUnknown4508[0x2800];
        /* 0x6D08 */ SpriteButton mButton0;
        /* 0x6DFC */ SpriteButton mButton1;
    }; // 0x6EF0

    struct ClothesPart {
        /* 0x0000 */ u8 mUnknown0000[4];
        /* 0x0004 */ ClothesItem mItems[300];
        /* 0x1C24 */ u8 mUnknown1C24[0x3C];
        /* 0x1C60 */ SpriteButton mButtonsA[3];
        /* 0x1F3C */ SpriteButton mButtonsB[5];
        /* 0x2400 */ u8 mUnknown2400[0xB0];
        /* 0x24B0 */ ButtonsPart mButtons;
        /* 0x33EC */ TextsPart mTexts;
    }; // 0xA2DC

    /* 0x0010 */ u8 mUnknown0010[4];
    /* 0x0014 */ SceneCharacter mCharacter;
    /* 0x2B2C */ u8 mUnknown2B2C[0x10];
    /* 0x2B3C */ SpriteButton mButton;
    /* 0x2C30 */ u8 mUnknown2C30[4];
    /* 0x2C34 */ ClothesPart mClothes;
    /* 0xCF10 */ u8 mUnknownCF10[0x160];
}; // 0xD070

#endif // OV067_CHANGECLOTHESSCENE_H
