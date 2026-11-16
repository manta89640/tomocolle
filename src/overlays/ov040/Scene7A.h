#ifndef OV040_SCENE7A_H
#define OV040_SCENE7A_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "room/Room.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 40, scene 0x7A (not decompiled). The scene factory (CreateScene) loads overlay 40 and allocates 0x25D90
// bytes; the constructor is the main program's (0x020461D4, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214AC80). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Mdl/Ica/ica_mii_mini_contents_LZ.bin",
// "Scn/Scene/SceneTestNg117_LZ.bin", "Msg/TmdMiniContentsMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is the scene id: nothing names this class, and no name of it survives in the ROM.
class Scene7A : public Scene {
public:
    Scene7A(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[1];
    /* 0x02B28 */ u8 mUnknown02B28[0x1BBC];
    /* 0x046E4 */ SavedCharacter mSavedCharacter;
    /* 0x049A4 */ u8 mUnknown049A4[0x98];
    /* 0x04A3C */ TtsPlayer mTtsPlayer;
    /* 0x19380 */ u8 mUnknown19380[0x4C0];
    /* 0x19840 */ MessageFile mMessageFiles[1];
    /* 0x19858 */ u8 mUnknown19858[0x4];
    /* 0x1985C */ MessageWindow mWindow;
    /* 0x24800 */ u8 mUnknown24800[0x8];
    /* 0x24808 */ MessageText mTexts[1];
    /* 0x24DF8 */ u8 mUnknown24DF8[0x98];
    /* 0x24E90 */ MessageText mText;
    /* 0x25480 */ Room mRoom;
    /* 0x25D88 */ u8 mUnknown25D88[0x8];
}; // 0x25D90

#endif // OV040_SCENE7A_H
