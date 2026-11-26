#ifndef OV094_PERSONALITYSCENE_H
#define OV094_PERSONALITYSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "island/IslandProgress.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

#include "../../scene/SceneCast.h"

// Overlay 94 (scenes 0x74 and 0x7E): one character, the first that gSceneMemory names, stands on the upper screen,
// and two panels slide in beside it that show its personality: one its kind (the quarter of the personality chart,
// by a table of the sixteen personalities) and one the personality itself, each with a title and a text and
// coloured by a palette of the personality. A button on the lower screen ends the scene. For scene 0x74 the
// character then starts a long act of state 0 and has its trouble cleared, and the scene asks for the island (0x5F)
// if the apartment building grows or the island has a new facility, and for the apartment (0x11) otherwise; for scene
// 0x7E, a step of the tutorial (binary.md, "TutorialStepScene"), it asks for scene 0x69, the tutorial's next step.
//
// The scene factory (CreateScene) loads overlay 94 and allocates 0x6D78 bytes; the constructor is the main
// program's (0x02045CC0, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214B020). The
// scene's StartLoading mounts "Scn/Scene/SceneCharDistribution_LZ.bin" (as overlay 77 does),
// "Msg/TmdEtcMsgJP_LZ.bin" (twice, as "TBM" and "T03"), "Mdl/Ica/ica_mii_contents_LZ.bin" and
// "Msg/TmdCommonMsgJP_LZ.bin".
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The two panels of the upper screen, which the scene's constructor constructs from the end of the button loop
// before it (a part of the scene of its own). The left panel shows the personality's kind, the right one the
// personality. Each panel is a sprite with a title and a text, which move with it.
struct PersonalityPanels {
    // What a panel is doing (mLeftState, mRightState)
    enum State {
        STATE_SHOWN,
        STATE_SLIDE_IN,
        STATE_SLIDE_OUT,
        STATE_HIDDEN,
    };

    void Init();
    void Update();
    void Draw();
    void SetPersonality(int personality);

    /* 0x0000 */ int mLeftState;
    /* 0x0004 */ int mRightState;
    /* 0x0008 */ s8 mKind;             // The personality's kind, 0 to 3, which the left panel shows
    /* 0x0009 */ s8 mPersonality;      // 0 to 15, which the right panel shows
    /* 0x000C */ ScreenPoint mLeftPosition;  // In pixels, from the screen's centre
    /* 0x0014 */ ScreenPoint mRightPosition;
    /* 0x001C */ Sprite mLeftSprite;
    /* 0x00C8 */ Sprite mRightSprite;
    /* 0x0174 */ MessageFile mMessageFile;
    /* 0x018C */ int mCharOffset;      // Where the next text's characters go in VRAM
    /* 0x0190 */ MessageText mKindTitle;
    /* 0x0780 */ MessageText mPersonalityTitle;
    /* 0x0D70 */ MessageText mKindText;
    /* 0x1360 */ MessageText mPersonalityText;
    /* 0x1950 */ BOOL mKindChanged;    // The left panel has to slide out and in again too
}; // 0x1954

class PersonalityScene : public Scene {
public:
    enum {
        SCENE_PERSONALITY = 0x74,
        SCENE_TUTORIAL = 0x7E, // The tutorial's steps 6 and 11
    };

    PersonalityScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ fx32 mScroll;                  // Of two background planes, which Draw scrolls on
    /* 0x00014 */ SceneCharacter mCharacters[1];
    /* 0x02B2C */ SceneCast mCast;
    /* 0x046E8 */ MessageFile mMessageFiles[2];   // "T02:/tmd_common_msg_JP.bmg", "T03:/tmd_etc_msg_JP.bmg"
    /* 0x04718 */ MessageText mTexts[1];          // The character's name on the upper screen
    /* 0x04D08 */ TextButton mTextButtons[1];     // Ends the scene
    /* 0x053F8 */ PersonalityPanels mPanels;
    /* 0x06D4C */ IslandProgress mIslandProgress;
}; // 0x6D78

#endif // OV094_PERSONALITYSCENE_H
