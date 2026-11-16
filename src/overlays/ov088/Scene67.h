#ifndef OV088_SCENE67_H
#define OV088_SCENE67_H

#include <nitro.h>

#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 88, scene 0x67 (not decompiled). The scene factory (CreateScene) loads overlay 88 and allocates 0x3D940
// bytes; the constructor is the main program's (0x02045504, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214C280). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Msg/TmdNewsMsgJP_LZ.bin", "Msg/TmdJobMsgJP_LZ.bin",
// "Msg/TmdItemMsgJP_LZ.bin", "Scn/List/List_NewsList_LZ.bin", "Mdl/Ica/ica_mii_news_LZ.bin" and the news studio's
// models "Mdl/Misc/Nws/news_studio00_3D_LZ.bin" to "news_studio02_3D_LZ.bin".
//
// The name is the scene id: the scene mounts no scene archive of its own, and no name of it survives in the ROM.
class Scene67 : public Scene {
public:
    Scene67(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x370];
    /* 0x00380 */ SpriteButton mButton;
    /* 0x00474 */ SpriteButton mButton474;
    /* 0x00568 */ u8 mUnknown00568[0x1D8];
    /* 0x00740 */ SpriteButton mButtons[7];
    /* 0x00DEC */ ObjTextBox mTextBoxes[7];
    /* 0x01A10 */ u8 mUnknown01A10[0x202B0];
    /* 0x21CC0 */ SavedCharacter mSavedCharacters[5];
    /* 0x22A80 */ u8 mUnknown22A80[0xE0];
    /* 0x22B60 */ MessageFile mMessageFiles[2];
    /* 0x22B90 */ u8 mUnknown22B90[0x1BBC];
    /* 0x2474C */ SceneCharacter mCharacters[1];
    /* 0x27264 */ AnimModel mModel;
    /* 0x27344 */ AnimModel mModel2;
    /* 0x27424 */ SpriteButton mButtons2[1];
    /* 0x27518 */ TextButton mTextButtons[2];
    /* 0x282F8 */ u8 mUnknown282F8[0x84];
    /* 0x2837C */ TtsPlayer mTtsPlayer;
    /* 0x3CCC0 */ u8 mUnknown3CCC0[0x8];
    /* 0x3CCC8 */ MessageText mTexts[1];
    /* 0x3D2B8 */ u8 mUnknown3D2B8[0x98];
    /* 0x3D350 */ MessageText mText;
}; // 0x3D940

#endif // OV088_SCENE67_H
