#ifndef OV093_SCENE72_H
#define OV093_SCENE72_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "keyboard/SoftKeyboard.h"
#include "sound/ValueRamp.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 93, scene 0x72 (not decompiled). The scene factory (CreateScene) loads overlay 93 and allocates 0x8328
// bytes; the constructor is the main program's (0x02045B6C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B3D4). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Cmn/Kbd/SoftKeyboard_LZ.bin", "Msg/TmdCommonMsgJP_LZ.bin",
// "Msg/TmdEtcMsgJP_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene72 : public Scene {
public:
    Scene72(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SoftKeyboard mKeyboard;
    /* 0x06008 */ u8 mUnknown06008[0x4];
    /* 0x0600C */ ObjTextBox mTextBox;
    /* 0x061C8 */ u8 mUnknown061C8[0x600];
    /* 0x067C8 */ ObjTextBox mTextBox067C8;
    /* 0x06984 */ u8 mUnknown06984[0x600];
    /* 0x06F84 */ ObjCharBox mCharBox;
    /* 0x07140 */ u8 mUnknown07140[0x28];
    /* 0x07168 */ AnimModel mModels[13];
    /* 0x07CC8 */ MessageFile mMessageFiles[2];
    /* 0x07CF8 */ SoundParamRamps mRamps[1]; // An array of one: the constructor builds it in a loop
    /* 0x07D38 */ MessageText mText;
}; // 0x8328

#endif // OV093_SCENE72_H
