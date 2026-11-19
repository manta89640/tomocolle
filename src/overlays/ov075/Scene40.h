#ifndef OV075_SCENE40_H
#define OV075_SCENE40_H

#include <nitro.h>

#include "HeadIcon.h"
#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "item/Item.h"
#include "island/IslandProgress.h"
#include "item/Mannequin.h"
#include "keyboard/SoftKeyboard.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

#include "Scene40Character.h"
#include "Scene40ChoiceList.h"
#include "Scene40ClothesList.h"
#include "Scene40CounterDisplay.h"
#include "Scene40Meter.h"
#include "Scene40ProfilePage.h"
#include "Scene40Camera.h"
#include "Scene40Clock.h"
#include "Scene40ItemModel.h"
#include "Scene40Lights.h"
#include "Scene40Physics.h"
#include "Scene40Touch.h"

#include "Scene40Context.h"
#include "Scene40MenuBar.h"
#include "Scene40MessageSequence.h"
#include "Scene40SpeechWindow.h"

// Overlay 75, scenes 0x40 and 0x6C (not decompiled). The scene factory (CreateScene) loads overlay 75 and allocates
// 0x6F908 bytes; the constructor is the main program's (0x020428E0, src/scene/SceneManager.cpp), which stores the
// overlay's virtual table (0x02182CEC). The members are as the constructor constructs them, by their classes; what
// lies between is not known. The scene mounts "Scn/Scene/SceneMiiTouch_LZ.bin" (which overlays 96 and 101 to 104
// mount too), "Scn/List/List_TouchList_LZ.bin", the touch animations "Mdl/Ica/ica_mii_mii_touch_*_LZ.bin", the
// mannequin's head and the talk messages by personality ("Msg/TmdTalkCoolMsgJP_LZ.bin", ...).
//
// The names are the scene id's: the scene's archive is shared, and no name of these classes survives in the ROM.

// A button of the scene: the main program's SpriteButton (built with its C2, 0x0203115C) and a value of its own
class Scene40Button : public SpriteButton {
public:
    void SetSequences(u16 sequence, u16 pressedSequence, u16 doneSequence);
    void SetDone(BOOL done);

    /* 0xF4 */ u16 mDoneSequence; // Shown when what the button stands for is done (page 9's marks)
    /* 0xF6 */ u8 mUnknownF6[2];
}; // 0xF8

// The scene's list: the shops' ItemListView with the five virtual functions of the overlay's own (0x0215E028 to
// 0x0215E4E0, table 0x02182CD0). Its constructor is inline.
class Scene40ListView : public ItemListView {
public:
    // What the rows' marks show (mMarkKind): whether the character has had the row's item, by the kind of item
    enum MarkKind {
        MARK_NONE = 0,
        MARK_ITEMS = 1,     // The record's bits at 0x1C8
        MARK_CLOTHES = 2,   // mOwnedClothes
        MARK_INTERIORS = 3, // mOwnedInteriors
    };

    Scene40ListView() {}

    virtual void Draw();
    virtual void SetVisible(BOOL visible);
    virtual void Refresh();
    virtual void DrawRow(int row, int item);
    virtual void ClearRow(int row);

    void Init();
    void InitMarks(int resource, int sequence);

    /* 0x13F64 */ u32 mMarkKind;
    /* 0x13F68 */ SavedCharacter *mCharacter; // Whose record the marks are read from
    /* 0x13F6C */ Sprite mMarks[7];           // One beside each row, as the shops' list has them
}; // 0x14420

// The scene's lower-screen pages (the classes above are its members')
#include "Scene40Pages.h"

class Scene40 : public Scene {
public:
    Scene40(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // What the scene's upper screen shows (mScreenState 0 to 3). The names are the files each loads.
    enum Layout {
        LAYOUT_SMT_UP = 0,   // SMT:/BG_up: the scene's own
        LAYOUT_LIST = 1,     // LTL:/BG_list: the list
        LAYOUT_KEYBOARD = 2, // KBD: the keyboard
        LAYOUT_SMT_DOWN = 3, // SMT:/BG_down
    };
    // mScreenState: a layout while it is shown, or one of these while it changes
    enum ScreenState {
        SCREEN_FADE_OUT = 4,
        SCREEN_LOAD = 5,
        SCREEN_FADE_IN = 6,
    };

    void ChooseTouchTalkers(RTCDate *date, RTCTime *time);
    int GetVisitedIndex();
    void InitLowerScreen();
    void Unused0214a884();
    void Init3d();
    int GetLayout(int request);
    int LoadLayout(int request);
    void Start();
    BOOL CameFromSceneOfTables(int sceneId);
    BOOL CameFromSceneOfTable(int sceneId);
    BOOL CameFromSceneOfPresentTable(int sceneId);
    void Unused0214ca80();

    /* 0x00010 */ int mScreenState;    // A layout, or a ScreenState
    /* 0x00014 */ int mLayout;         // The layout shown while the screen changes
    /* 0x00018 */ int mNextLayout;
    /* 0x0001C */ int mRequestedLayout; // What the pages asked for (1 to 4: LoadLayout's argument)
    /* 0x00020 */ int mBrightness;     // 0 to 16 while the screen fades
    /* 0x00024 */ u32 mSavedGraphics[0x50]; // The lower screen's graphics state, as InitLowerScreen saved it
    /* 0x00164 */ u8 mUnknown00164[0x4];
    /* 0x00168 */ int mDrawState;  // 0 while the room and the character are still updated from Draw, 1 after nine frames
    /* 0x0016C */ int mDrawFrames;
    /* 0x00170 */ Mannequin mMannequin;
    /* 0x00828 */ Room mRoom;
    /* 0x01130 */ AnimModel mModels[3];
    /* 0x013D0 */ Sprite mSprite013D0;
    /* 0x0147C */ TtsPlayer mTtsPlayer;
    /* 0x15DC0 */ MessageText mText;
    /* 0x163B0 */ Scene40SpeechWindow mSpeechWindow;
    /* 0x169C0 */ MessageWindow mWindows[2];
    /* 0x2C908 */ Scene40MessageSequence mMessageSequence;
    /* 0x2C954 */ ParticleSystem mParticles;
    /* 0x2C958 */ HeadIcon mHeadIcon;
    // Where the next characters go in the main and the sub engine's OBJ VRAM: Init starts them, and each text,
    // window and page it sets up moves them on (SpeechWindow::Init's charOffset)
    /* 0x2C998 */ int mMainCharOffset;
    /* 0x2C99C */ int mSubCharOffset;
    /* 0x2C9A0 */ ClothesList mClothesList;
    /* 0x2EA74 */ Inventory mInventory;
    /* 0x30260 */ InteriorList mInteriors;
    /* 0x309B4 */ int mMoney;      // gGameData's capped counter: Init reads it, Exit writes it back
    /* 0x309B8 */ IslandProgress mIslandProgress;
    /* 0x309E4 */ MessageFile mMessageFiles[7];
    /* 0x30A8C */ Scene40Clock mClock;
    /* 0x30AB0 */ Scene40Context mContext; // What the pages are given
    /* 0x30B50 */ Scene40Pages mPages;
    /* 0x69D2C */ Scene40Touch mTouch;
    /* 0x69DEC */ Scene40Camera mCamera;
    /* 0x69E10 */ Scene40Lights mLights;
    /* 0x69E24 */ Scene40Physics mPhysics;
    /* 0x69E54 */ Scene40ItemModel mItemModels[3];
    /* 0x6A22C */ Scene40Character mCharacter;
    /* 0x6D324 */ CharacterBase mFace;
}; // 0x6F908

#endif // OV075_SCENE40_H
