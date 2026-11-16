#ifndef OV055_TESTNG134SCENE_H
#define OV055_TESTNG134SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 55, scene 0x93 (not decompiled). The scene factory (CreateScene) loads overlay 55 and allocates 0x36640
// bytes; the constructor is the main program's (0x02047AE8, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BD98). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents2_LZ.bin",
// "Scn/Scene/SceneTestNg134_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin",
// "Scn/Scene/NgCaption_LZ.bin", "Mdl/Misc/eawase_3D_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg134_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
// A model of the scene: an AnimModel and 0x24 bytes, a class of its own with nothing virtual of its own, whose table
// (0x0211091C) is therefore the scene factory's file's, with its destructors (D1 0x02047D6C after the scene's
// constructor, D0 0x0204AE10 at the file's end). Its implicit constructor is 0x02047D1C.
//
// The name is descriptive, not the original.
class TestNg134Model : public AnimModel {
public:
    /* 0x0E0 */ u8 mUnknown0E0[0x24];
}; // 0x104

// A character of the scene: a CharacterBase and 4 bytes, whose implicit constructor and destructor (0x02047DA8,
// 0x02047DBC) are only CharacterBase's C2 and D2.
//
// The name is descriptive, not the original.
class TestNg134Character : public CharacterBase {
public:
    /* 0x25E4 */ u8 mUnknown25E4[4];
}; // 0x25E8

class TestNg134Scene : public Scene {
public:
    TestNg134Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ SavedCharacter mSavedCharacter;
    /* 0x049A4 */ u8 mUnknown049A4[0xA0];
    /* 0x04A44 */ TtsPlayer mTtsPlayer;
    /* 0x19388 */ u8 mUnknown19388[0x60C];
    /* 0x19994 */ TextButton mTextButtons[1];
    /* 0x1A084 */ SpriteButton mButtons[1];
    /* 0x1A178 */ u8 mUnknown1A178[0xC];
    /* 0x1A184 */ MessageFile mMessageFiles[2];
    /* 0x1A1B4 */ u8 mUnknown1A1B4[0x8];
    /* 0x1A1BC */ MessageText mTexts[1];
    /* 0x1A7AC */ u8 mUnknown1A7AC[0x98];
    /* 0x1A844 */ MessageText mText;
    /* 0x1AE34 */ Room mRoom;
    /* 0x1B73C */ u8 mUnknown1B73C[0x8];
    /* 0x1B744 */ AnimModel mModels[1];
    /* 0x1B824 */ TestNg134Model mModels1B824[12];
    /* 0x1C454 */ TestNg134Character mCharacters1C454[6];
    /* 0x2A7C4 */ u8 mUnknown2A7C4[0x7C4];
    /* 0x2AF88 */ ItemModel mItemModels[6];
    /* 0x2B690 */ u8 mUnknown2B690[0xC];
    /* 0x2B69C */ MessageWindow mWindow;
}; // 0x36640

#endif // OV055_TESTNG134SCENE_H
