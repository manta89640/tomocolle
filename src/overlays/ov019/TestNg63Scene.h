#ifndef OV019_TESTNG63SCENE_H
#define OV019_TESTNG63SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 19, scene 0x3E (not decompiled). The scene factory (CreateScene) loads overlay 19 and allocates 0x1CEA8
// bytes; the constructor is the main program's (0x02042634, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AE50). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg63_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin",
// "Mdl/Misc/search_3D_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg63_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
// A model of the scene, of 0x144 bytes: the main program's AnimModel and something after it. Its constructor is the
// overlay's (0x0214A718); its destructor, which only destroys the model, is the main program's (after the scene's
// constructor), emitted for the scene's __cxa_vec_ctor. A member, not a base: the destructor stores only the model's
// tables.
//
// The name is descriptive, not the original.
class TestNg63Model {
public:
    TestNg63Model();

    /* 0x000 */ AnimModel mModel;
    /* 0x0E0 */ u8 mUnknown0E0[0x64];
}; // 0x144

class TestNg63Scene : public Scene {
public:
    TestNg63Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ TestNg63Model mModels46E4[30];
    /* 0x06CDC */ u8 mUnknown06CDC[0x108];
    /* 0x06DE4 */ TtsPlayer mTtsPlayer;
    /* 0x1B728 */ MessageFile mMessageFiles[1];
    /* 0x1B740 */ u8 mUnknown1B740[0x8];
    /* 0x1B748 */ MessageText mTexts[1];
    /* 0x1BD38 */ u8 mUnknown1BD38[0x98];
    /* 0x1BDD0 */ MessageText mText;
    /* 0x1C3C0 */ Room mRoom;
    /* 0x1CCC8 */ u8 mUnknown1CCC8[0x8];
    /* 0x1CCD0 */ SpriteButton mButtons[1];
    /* 0x1CDC4 */ u8 mUnknown1CDC4[0x4];
    /* 0x1CDC8 */ AnimModel mModels1CDC8[1];
}; // 0x1CEA8

#endif // OV019_TESTNG63SCENE_H
