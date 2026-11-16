#ifndef OV036_TESTNG107SCENE_H
#define OV036_TESTNG107SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 36, scene 0x59 (not decompiled). The scene factory (CreateScene) loads overlay 36 and allocates 0x26588
// bytes; the constructor is the main program's (0x02044A20, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AE40). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Scn/Scene/SceneTestNg107_LZ.bin",
// "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg107_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class TestNg107Scene : public Scene {
public:
    TestNg107Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C54];
    /* 0x0477C */ TtsPlayer mTtsPlayer;
    /* 0x190C0 */ u8 mUnknown190C0[0x788];
    /* 0x19848 */ TextButton mTextButtons[1];
    /* 0x19F38 */ SpriteButton mButtons[1];
    /* 0x1A02C */ u8 mUnknown1A02C[0x10];
    /* 0x1A03C */ MessageWindow mWindow;
    /* 0x24FE0 */ MessageFile mMessageFiles[1];
    /* 0x24FF8 */ u8 mUnknown24FF8[0x8];
    /* 0x25000 */ MessageText mTexts[1];
    /* 0x255F0 */ u8 mUnknown255F0[0x98];
    /* 0x25688 */ MessageText mText;
    /* 0x25C78 */ Room mRoom;
    /* 0x26580 */ u8 mUnknown26580[0x8];
}; // 0x26588

#endif // OV036_TESTNG107SCENE_H
