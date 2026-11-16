#ifndef OV042_TESTNG119SCENE_H
#define OV042_TESTNG119SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 42, scene 0x7C (not decompiled). The scene factory (CreateScene) loads overlay 42 and allocates 0x1BE88
// bytes; the constructor is the main program's (0x02046434, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B460). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg119_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg119_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg119Scene : public Scene {
public:
    TestNg119Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ SavedCharacter mSavedCharacter;
    /* 0x049A4 */ u8 mUnknown049A4[0x94];
    /* 0x04A38 */ TtsPlayer mTtsPlayer;
    /* 0x1937C */ u8 mUnknown1937C[0x560];
    /* 0x198DC */ TextButton mTextButtons[1];
    /* 0x19FCC */ MessageText mTexts[1];
    /* 0x1A5BC */ u8 mUnknown1A5BC[0x224];
    /* 0x1A7E0 */ SpriteButton mButtons[1];
    /* 0x1A8D4 */ u8 mUnknown1A8D4[0xC];
    /* 0x1A8E0 */ MessageFile mMessageFiles[1];
    /* 0x1A8F8 */ u8 mUnknown1A8F8[0x8];
    /* 0x1A900 */ MessageText mTexts1A900[1];
    /* 0x1AEF0 */ u8 mUnknown1AEF0[0x98];
    /* 0x1AF88 */ MessageText mText;
    /* 0x1B578 */ Room mRoom;
    /* 0x1BE80 */ u8 mUnknown1BE80[0x8];
}; // 0x1BE88

#endif // OV042_TESTNG119SCENE_H
