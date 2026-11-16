#ifndef OV028_SCENE50_H
#define OV028_SCENE50_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 28, scene 0x50 (not decompiled). The scene factory (CreateScene) loads overlay 28 and allocates 0x1A094
// bytes; the constructor is the main program's (0x02043F98, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AD10). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Msg/TmdItemMsgJP_LZ.bin",
// "Mdl/Misc/cage_3D_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene50 : public Scene {
public:
    Scene50(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ ItemModel mItemModels[1];
    /* 0x04810 */ u8 mUnknown04810[0x10];
    /* 0x04820 */ MessageFile mMessageFiles[2];
    /* 0x04850 */ u8 mUnknown04850[0x98];
    /* 0x048E8 */ TtsPlayer mTtsPlayer;
    /* 0x1922C */ u8 mUnknown1922C[0x4];
    /* 0x19230 */ SpriteButton mButtons[1];
    /* 0x19324 */ u8 mUnknown19324[0x14];
    /* 0x19338 */ MessageText mTexts[1];
    /* 0x19928 */ u8 mUnknown19928[0x98];
    /* 0x199C0 */ MessageText mText;
    /* 0x19FB0 */ AnimModel mModel;
    /* 0x1A090 */ u8 mUnknown1A090[0x4];
}; // 0x1A094

#endif // OV028_SCENE50_H
