#ifndef OV037_SCENE75_H
#define OV037_SCENE75_H

#include <nitro.h>

#include "Model.h"
#include "Scene.h"
#include "MessageWindow.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 37, scene 0x75 (not decompiled). The scene factory (CreateScene) loads overlay 37 and allocates 0x27258
// bytes; the constructor is the main program's (0x02045DBC, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214C7DC). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_room_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
// A model of the scene: a class of overlay 37 derived from AnimModel (its table at 0x0214C7C0), of 0x1AC bytes. Its
// implicit constructor and destructor are the main program's copies after the scene's constructor (0x02045F74,
// 0x02045FC4), emitted for the scene's __cxa_vec_ctor; the overlay's table refers to the destructor.
//
// The name is descriptive, not the original.
class Scene75Model : public AnimModel {
public:
    virtual void Draw(); // 0x0214BB48: the overlay's (its table's last entry)

    /* 0x0E0 */ u8 mUnknown0E0[0xCC];
}; // 0x1AC

class Scene75 : public Scene {
public:
    Scene75(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C5C];
    /* 0x04784 */ TtsPlayer mTtsPlayer;
    /* 0x190C8 */ MessageFile mMessageFiles[2];
    /* 0x190F8 */ u8 mUnknown190F8[0x8];
    /* 0x19100 */ MessageText mTexts[1];
    /* 0x196F0 */ u8 mUnknown196F0[0x98];
    /* 0x19788 */ MessageText mText;
    /* 0x19D78 */ Room mRoom;
    /* 0x1A680 */ u8 mUnknown1A680[0x14];
    /* 0x1A694 */ ItemModel mItemModels[4];
    /* 0x1AB44 */ u8 mUnknown1AB44[0x8];
    /* 0x1AB4C */ Scene75Model mModels1AB4C[4];
    /* 0x1B1FC */ u8 mUnknown1B1FC[0x2B4];
    /* 0x1B4B0 */ TextButton mTextButtons[2];
    /* 0x1C290 */ u8 mUnknown1C290[0x4];
    /* 0x1C294 */ MessageWindow mWindow;
    /* 0x27238 */ u8 mUnknown27238[0x20];
}; // 0x27258

#endif // OV037_SCENE75_H
