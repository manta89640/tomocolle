#ifndef OV008_SCENE19_H
#define OV008_SCENE19_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "Model.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 8 (scene id 0x19), which mounts Scn/Scene/NgCaption_LZ.bin (as overlays 9 and 80 do): three
// characters, a model, a speech player, message texts and a button. The overlay is not decompiled: this is the
// layout its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name of this class survives
// in the ROM.
class Scene19 : public Scene {
public:
    Scene19(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[3];
    /* 0x08158 */ u8 mUnknown08158[0x1BBC];
    /* 0x09D14 */ AnimModel mModel;
    /* 0x09DF4 */ TtsPlayer mTtsPlayer;
    /* 0x1E738 */ u8 mUnknown1E738[0x15C];
    /* 0x1E894 */ SpriteButton mButtons[1];
    /* 0x1E988 */ u8 mUnknown1E988[4];
    /* 0x1E98C */ MessageFile mMessageFiles[1];
    /* 0x1E9A4 */ u8 mUnknown1E9A4[8];
    /* 0x1E9AC */ MessageText mTexts[1];
    /* 0x1EF9C */ u8 mUnknown1EF9C[0x21C];
    /* 0x1F1B8 */ MessageText mText;
}; // 0x1F7A8

#endif // OV008_SCENE19_H
