#ifndef OV016_SCENE3B_H
#define OV016_SCENE3B_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "room/Room.h"
#include "voice/TtsPlayer.h"

// Overlay 16, scene 0x3B (not decompiled). The scene factory (CreateScene) loads overlay 16 and allocates 0x1B0C4
// bytes; the constructor is the main program's (0x020422EC, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214A500). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/body_ICA_OLD_LZ.bin", "Scn/Scene/SceneBG_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene3B : public Scene {
public:
    Scene3B(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1C48];
    /* 0x04770 */ TtsPlayer mTtsPlayer;
    /* 0x190B4 */ AnimModel mModels[1];
    /* 0x19194 */ u8 mUnknown19194[0x4];
    /* 0x19198 */ InventoryItem mItems[255];
    /* 0x1A584 */ ItemModel mItemModel;
    /* 0x1A6B0 */ u8 mUnknown1A6B0[0x4];
    /* 0x1A6B4 */ SpriteButton mButtons[1];
    /* 0x1A7A8 */ u8 mUnknown1A7A8[0xC];
    /* 0x1A7B4 */ Room mRoom;
    /* 0x1B0BC */ u8 mUnknown1B0BC[0x8];
}; // 0x1B0C4

#endif // OV016_SCENE3B_H
