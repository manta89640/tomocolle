#ifndef OV007_SCENE18_H
#define OV007_SCENE18_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 7 (scene id 0x18), which mounts Scn/Scene/SceneTestNg13_LZ.bin (as overlays 54 to 59 do): a
// character with its speech player, message texts and a button. The overlay is not decompiled: this is the layout
// its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name of this class survives in the
// ROM.
class Scene18 : public Scene {
public:
    Scene18(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x1BBC];
    /* 0x01BCC */ SceneCharacter mCharacters[1];
    /* 0x046E4 */ u8 mUnknown046E4[0x498];
    /* 0x04B7C */ TtsPlayer mTtsPlayer;
    /* 0x194C0 */ MessageFile mMessageFiles[3];
    /* 0x19508 */ u8 mUnknown19508[8];
    /* 0x19510 */ MessageText mTextsA[1];
    /* 0x19B00 */ u8 mUnknown19B00[0x118];
    /* 0x19C18 */ MessageText mText;
    /* 0x1A208 */ MessageText mTextsB[1];
    /* 0x1A7F8 */ u8 mUnknown1A7F8[0x158];
    /* 0x1A950 */ SpriteButton mButtons[1];
    /* 0x1AA44 */ u8 mUnknown1AA44[0xC];
}; // 0x1AA50

#endif // OV007_SCENE18_H
