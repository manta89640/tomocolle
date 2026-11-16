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
#include "item/Mannequin.h"
#include "keyboard/SoftKeyboard.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

#include "Scene40Context.h"
#include "Scene40Character.h"
#include "Scene40MenuBar.h"
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
    /* 0xF4 */ int mUnknownF4;
}; // 0xF8

// A model of an item with fields of its own. Its constructor (0x020431A0) and D1 (0x020431CC) follow the scene's
// constructor, for __cxa_vec_ctor; its table (0x02110A7C) is this file's data, its D0 (0x0204ADCC) at the file's
// end.
class Scene40ItemModel : public ItemModel {
public:
    /* 0x12C */ u8 mUnknown12C[0x1C];
}; // 0x148

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

    /* 0x13F64 */ u32 mMarkKind;
    /* 0x13F68 */ SavedCharacter *mCharacter; // Whose record the marks are read from
    /* 0x13F6C */ Sprite mMarks[7];           // One beside each row, as the shops' list has them
}; // 0x14420

// The scene's clock (what the overlay's 0x02179728 constructs): the date and time last read and the same moment in
// seconds, as overlay 74's Clock has them in another order.
class Scene40Unknown30A8C {
public:
    Scene40Unknown30A8C();
    void Read();       // 0x02179764: reads the date and time now
    void ReadIfIdle(); // 0x02179790: the same, but only while the archive manager is idle

    /* 0x00 */ RTCDate mDate; // Exit gives it to the character as the date they last met
    /* 0x10 */ RTCTime mTime;
    /* 0x1C */ s64 mSeconds; // The same moment in seconds, as the main program's Clock_GetDateTime returns it
}; // 0x24

// Three classes of the overlay whose constructors only store their tables (0x02182CA0, 0x02182CB8, 0x02182C88:
// four entries each, the first two classes sharing the second and third). What their functions are is not known:
// the virtual functions below stand for the tables' entries, in order, and are not names.
class Scene40UnknownA {
public:
    Scene40UnknownA() {}

    virtual void Virtual0();
    virtual void Virtual1();
    virtual void Virtual2();
    virtual void Virtual3();

    /* 0x004 */ u8 mUnknown004[0x710];
}; // 0x714

class Scene40UnknownB {
public:
    Scene40UnknownB() {}

    virtual void Virtual0();
    virtual void Virtual1();
    virtual void Virtual2();
    virtual void Virtual3();

    /* 0x004 */ u8 mUnknown004[0x6F0];
}; // 0x6F4

class Scene40UnknownC {
public:
    Scene40UnknownC() {}

    virtual void Virtual0();
    virtual void Virtual1();
    virtual void Virtual2();
    virtual void Virtual3();

    /* 0x004 */ u8 mUnknown004[0x640];
}; // 0x644

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

    /* 0x00010 */ u8 mUnknown00010[0x158];
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
    /* 0x2C908 */ u8 mUnknown2C908[0x4C];
    /* 0x2C954 */ ParticleSystem mParticles;
    /* 0x2C958 */ HeadIcon mHeadIcon;
    /* 0x2C998 */ u8 mUnknown2C998[0x8];
    /* 0x2C9A0 */ ClothesList mClothesList;
    /* 0x2EA74 */ Inventory mInventory;
    /* 0x30260 */ InteriorList mInteriors;
    /* 0x309B4 */ int mUnknown309B4;       // Exit gives it to GameData::SetCappedCounter
    /* 0x309B8 */ u8 mUnknown309B8[0x2C];
    /* 0x309E4 */ MessageFile mMessageFiles[7];
    /* 0x30A8C */ Scene40Unknown30A8C mUnknown30A8C;
    /* 0x30AB0 */ Scene40Context mContext; // What the pages are given
    /* 0x30B50 */ Scene40Pages mPages;
    /* 0x69D2C */ u8 mUnknown69D2C[0x128];
    /* 0x69E54 */ Scene40ItemModel mItemModels[3];
    /* 0x6A22C */ Scene40Character mCharacter;
    /* 0x6D324 */ CharacterBase mFace;
}; // 0x6F908

#endif // OV075_SCENE40_H
