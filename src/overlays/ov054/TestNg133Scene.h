#ifndef OV054_TESTNG133SCENE_H
#define OV054_TESTNG133SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 54, scene 0x92 (not decompiled). The scene factory (CreateScene) loads overlay 54 and allocates 0x26EBC
// bytes; the constructor is the main program's (0x02047974, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B6E0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin", "Scn/Scene/SceneTestNg133_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg133_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg133Scene : public Scene {
public:
    TestNg133Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C58];
    /* 0x04780 */ TtsPlayer mTtsPlayer;
    /* 0x190C4 */ SavedCharacter mSavedCharacter;
    /* 0x19384 */ u8 mUnknown19384[0x294];
    /* 0x19618 */ MessageFile mMessageFiles[1];
    /* 0x19630 */ u8 mUnknown19630[0x8];
    /* 0x19638 */ MessageText mTexts[1];
    /* 0x19C28 */ u8 mUnknown19C28[0x98];
    /* 0x19CC0 */ MessageText mText;
    /* 0x1A2B0 */ Room mRoom;
    /* 0x1ABB8 */ u8 mUnknown1ABB8[0x568];
    /* 0x1B120 */ TextButton mTextButtons[2];
    /* 0x1BF00 */ u8 mUnknown1BF00[0x4];
    /* 0x1BF04 */ MessageWindow mWindow;
    /* 0x26EA8 */ u8 mUnknown26EA8[0x14];
}; // 0x26EBC

#endif // OV054_TESTNG133SCENE_H
