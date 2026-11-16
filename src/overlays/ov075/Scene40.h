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

// The goods and clothes the scene shows, two bases: the constructor stores both of this class's tables (this
// file's data, 0x02110B4C with the second at +0x2C) after both base constructors.
class Scene40Items : public InventoryItem, public ClothesItem {}; // 0x2C

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

// What the overlay's 0x02179728 constructs: a date and a time, cleared, and two ints.
class Scene40Unknown30A8C {
public:
    Scene40Unknown30A8C();

    /* 0x00 */ RTCDate mDate; // Exit gives it to the character as the date they last met
    /* 0x10 */ RTCTime mTime;
    /* 0x1C */ int mUnknown1C;
    /* 0x20 */ int mUnknown20;
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

// A character with two models, the goods and clothes it holds and two head icons
class Scene40Character : public Character {
public:
    void Draw(); // The character, then the two objects with a model each at 0x2BE4 and 0x2CDC

    /* 0x2B18 */ u8 mUnknown2B18[0xDC]; // From 0x2BE4, an object of the overlay's that holds mModel
    /* 0x2BF4 */ AnimModel mModel;
    /* 0x2CD4 */ u8 mUnknown2CD4[0xC];  // From 0x2CDC, a second object, which holds mModel2
    /* 0x2CE0 */ AnimModel mModel2;
    /* 0x2DC0 */ u8 mUnknown2DC0[0x4];
    /* 0x2DC4 */ Scene40Items mItems;
    /* 0x2DF0 */ u8 mUnknown2DF0[0xC];
    /* 0x2DFC */ HeadIcon mHeadIcons[2];
    /* 0x2E7C */ u8 mUnknown2E7C[0x27C];
}; // 0x30F8

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
    /* 0x163B0 */ SpeechWindow mSpeechWindow;
    /* 0x169BC */ u8 mUnknown169BC[0x4];
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
    /* 0x30AB0 */ u8 mUnknown30AB0[0xA0];
    /* 0x30B50 */ u8 mUnknown30B50[0x124]; // An object of the overlay's: func_ov075_0214fc00 draws it
    /* 0x30C74 */ Scene40Items mItems;
    /* 0x30CA0 */ u8 mUnknown30CA0[0x50];
    /* 0x30CF0 */ TextButton mTextButtons[4];
    /* 0x328B0 */ u8 mUnknown328B0[0x54];
    /* 0x32904 */ Scene40Items mItems2;
    /* 0x32930 */ u8 mUnknown32930[0x180];
    /* 0x32AB0 */ Scene40Items mItems3;
    /* 0x32ADC */ u8 mUnknown32ADC[0x24];
    /* 0x32B00 */ ClothesList mClothesList2;
    /* 0x34BD4 */ u8 mUnknown34BD4[0x4B4];
    /* 0x35088 */ Scene40ListView mList;
    /* 0x494A8 */ u8 mUnknown494A8[0xC];
    /* 0x494B4 */ SpriteButton mButtons[4];
    /* 0x49884 */ u8 mUnknown49884[0x18];
    /* 0x4989C */ SpriteButton mButtons2[4];
    /* 0x49C6C */ u8 mUnknown49C6C[0x8];
    /* 0x49C74 */ SpriteButton mButtons3[5];
    /* 0x4A138 */ TextButton mTextButton;
    /* 0x4A828 */ TextButton mTextButton2;
    /* 0x4AF18 */ SpriteButton mButton;
    /* 0x4B00C */ MessageText mText2;
    /* 0x4B5FC */ u8 mUnknown4B5FC[0x20];
    /* 0x4B61C */ SoftKeyboard mKeyboard;
    /* 0x51614 */ ObjCharBox mCharBox;
    /* 0x517D0 */ u8 mUnknown517D0[0x58];
    /* 0x51828 */ MessageText mText3;
    /* 0x51E18 */ SpriteButton mButton2;
    /* 0x51F0C */ SpriteButton mButton3;
    /* 0x52000 */ u8 mUnknown52000[0x18];
    /* 0x52018 */ Scene40Items mItems4;
    /* 0x52044 */ u8 mUnknown52044[0x14];
    /* 0x52058 */ MessageText mText4;
    /* 0x52648 */ SpriteButton mButton4;
    /* 0x5273C */ Scene40Button mButtons4[4];
    /* 0x52B1C */ MessageText mTexts2[4];
    /* 0x542DC */ Scene40Button mButtons5[6];
    /* 0x548AC */ MessageText mTexts3[6];
    /* 0x56C4C */ Scene40Button mButtons6[8];
    /* 0x5740C */ MessageText mTexts4[8];
    /* 0x5A38C */ u8 mUnknown5A38C[0xDD8];
    /* 0x5B164 */ MessageText mText5;
    /* 0x5B754 */ u8 mUnknown5B754[0xAC];
    /* 0x5B800 */ MessageText mText6;
    /* 0x5BDF0 */ MessageText mTexts5[3];
    /* 0x5CFC0 */ MessageText mText7;
    /* 0x5D5B0 */ u8 mUnknown5D5B0[0xAC];
    /* 0x5D65C */ ObjTextBox mTextBox;
    /* 0x5D818 */ ObjTextBox mTextBox2;
    /* 0x5D9D4 */ u8 mUnknown5D9D4[0xAC];
    /* 0x5DA80 */ Scene40UnknownA mUnknownA;
    /* 0x5E194 */ ObjTextBox mTextBox3;
    /* 0x5E350 */ Scene40UnknownB mUnknownB;
    /* 0x5EA44 */ Scene40UnknownC mUnknownC;
    /* 0x5F088 */ MessageText mTexts6[9];
    /* 0x625F8 */ ObjTextBox mTextBoxes[11];
    /* 0x6390C */ MessageText mText8;
    /* 0x63EFC */ MessageText mTexts7[11];
    /* 0x6804C */ MessageText mText9;
    /* 0x6863C */ u8 mUnknown6863C[0xC88];
    /* 0x692C4 */ ObjTextCanvas mCanvas;
    /* 0x6947C */ u8 mUnknown6947C[0x38];
    /* 0x694B4 */ Scene40Button mButton5;
    /* 0x695AC */ u8 mUnknown695AC[0xC];
    /* 0x695B8 */ SpriteButton mButtons7[7];
    /* 0x69C64 */ u8 mUnknown69C64[0x1F0];
    /* 0x69E54 */ Scene40ItemModel mItemModels[3];
    /* 0x6A22C */ Scene40Character mCharacter;
    /* 0x6D324 */ CharacterBase mFace;
}; // 0x6F908

#endif // OV075_SCENE40_H
