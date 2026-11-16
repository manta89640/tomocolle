#ifndef OV026_TESTNG87SCENE_H
#define OV026_TESTNG87SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 26, scene 0x4E (not decompiled). The scene factory (CreateScene) loads overlay 26 and allocates 0x2766C
// bytes; the constructor is the main program's (0x02043D90, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A560). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Scn/Scene/SceneTestNg87_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg87_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg87Scene : public Scene {
public:
    TestNg87Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[6];
    /* 0x102A0 */ u8 mUnknown102A0[0x1C4C];
    /* 0x11EEC */ TtsPlayer mTtsPlayer;
    /* 0x26830 */ u8 mUnknown26830[0xAC];
    /* 0x268DC */ SpriteButton mButtons[1];
    /* 0x269D0 */ u8 mUnknown269D0[0x4];
    /* 0x269D4 */ MessageFile mMessageFiles[1];
    /* 0x269EC */ u8 mUnknown269EC[0x8];
    /* 0x269F4 */ MessageText mTexts[1];
    /* 0x26FE4 */ u8 mUnknown26FE4[0x98];
    /* 0x2707C */ MessageText mText;
}; // 0x2766C

#endif // OV026_TESTNG87SCENE_H
