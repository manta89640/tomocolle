#ifndef OV077_SCENE17_H
#define OV077_SCENE17_H

#include <nitro.h>

#include "character/AnimModelCharacter.h"
#include "MessageWindow.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "Scene.h"

// Two parts of the scene that the constructor builds through a pointer to their start, as members of a member
// (their constructors are implicit). The names are the project's own.
struct Scene17Texts {
    /* 0x0000 */ u8 mUnknown0000[0x178];
    /* 0x0178 */ MessageFile mMessageFile;
    /* 0x0190 */ MessageText mText0;
    /* 0x0780 */ MessageText mText1;
    /* 0x0D70 */ MessageText mText2;
    /* 0x1360 */ MessageText mText3;
    /* 0x1950 */ u8 mUnknown1950[4];
}; // 0x1954

struct Scene17Characters {
    /* 0x0000 */ u8 mUnknown0000[8];
    /* 0x0008 */ AnimModelCharacter mCharactersA[6];
    /* 0xEAF8 */ AnimModelCharacter mCharactersB[4];
    /* 0x18798 */ u8 mUnknown18798[0x1A8];
}; // 0x18940

// The scene of overlay 77 (scene ids 0x17 and 0x7D), which mounts Scn/Scene/SceneCharDistribution_LZ.bin (as
// overlay 94 does): a hundred saved characters, a message window and ten AnimModelCharacters. The overlay is not
// decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The name is the scene id's: no name
// of this class survives in the ROM.
class Scene17 : public Scene {
public:
    Scene17(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SavedCharacter mCharacters[100];
    /* 0x11310 */ u8 mUnknown11310[0x28];
    /* 0x11338 */ SpriteButton mButtons[16];
    /* 0x12278 */ SpriteButton mButton;
    /* 0x1236C */ u8 mUnknown1236C[0x35C];
    /* 0x126C8 */ MessageWindow mWindow;
    /* 0x1D66C */ u8 mUnknown1D66C[0x1C];
    /* 0x1D688 */ MessageFile mMessageFiles[2];
    /* 0x1D6B8 */ u8 mUnknown1D6B8[0x2140];
    /* 0x1F7F8 */ Scene17Texts mTexts;
    /* 0x2114C */ Scene17Characters mModelCharacters;
}; // 0x39A8C

#endif // OV077_SCENE17_H
