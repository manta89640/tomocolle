#ifndef OV075_SCENE40PAGES_H
#define OV075_SCENE40PAGES_H

// Included from Scene40.h, after the classes it holds.

// The scene's lower screen (scene+0x30B50 to scene+0x69D2C): ten pages, one shown at a time, and what they share.
// Init (0x0214CA84) initialises every page from the scene's context, Update (0x0214CC34) runs the current page,
// Draw (0x0214FC00) draws it. The scene switches the lower screen's graphics when a page asks for another layout
// (mRequestedLayout). Offsets are the object's, then the scene's in brackets. See scratchpad's ov075-design.md.
class Scene40ListPage {
public:
    /* 0x0000 [0x32A94] */ u8 mUnknown32A94[0x1C];
    /* 0x001C [0x32AB0] */ Scene40Items mItems3;
    /* 0x0048 [0x32ADC] */ u8 mUnknown32ADC[0x24];
    /* 0x006C [0x32B00] */ ClothesList mClothesList2;
    /* 0x2140 [0x34BD4] */ u8 mUnknown34BD4[0x4B4];
    /* 0x25F4 [0x35088] */ Scene40ListView mList;
    /* 0x16A14 [0x494A8] */ u8 mUnknown494A8[0xC];
    /* 0x16A20 [0x494B4] */ SpriteButton mButtons[4];
    /* 0x16DF0 [0x49884] */ u8 mUnknown49884[0x18];
    /* 0x16E08 [0x4989C] */ SpriteButton mButtons2[4];
    /* 0x171D8 [0x49C6C] */ u8 mUnknown49C6C[0x8];
    /* 0x171E0 [0x49C74] */ SpriteButton mButtons3[5];
    /* 0x176A4 [0x4A138] */ TextButton mTextButton;
    /* 0x17D94 [0x4A828] */ TextButton mTextButton2;
    /* 0x18484 [0x4AF18] */ SpriteButton mButton;
    /* 0x18578 [0x4B00C] */ MessageText mText2;
    /* 0x18B68 [0x4B5FC] */ u8 mUnknown4B5FC[0xC];
}; // 0x18B74

class Scene40Pages {
public:
    /* 0x00000 [0x30B50] */ u8 mUnknown30B50[0x124]; // An object of the overlay's: func_ov075_0214fc00 draws it
    /* 0x00124 [0x30C74] */ Scene40Items mItems;
    /* 0x00150 [0x30CA0] */ u8 mUnknown30CA0[0x50];
    /* 0x001A0 [0x30CF0] */ TextButton mTextButtons[4];
    /* 0x01D60 [0x328B0] */ u8 mUnknown328B0[0x54];
    /* 0x01DB4 [0x32904] */ Scene40Items mItems2;
    /* 0x01DE0 [0x32930] */ u8 mUnknown32930[0x164];
    /* 0x01F44 [0x32A94] */ Scene40ListPage mListPage;
    /* 0x1AAB8 [0x4B608] */ u8 mUnknown4B608[0x14];
    /* 0x1AACC [0x4B61C] */ SoftKeyboard mKeyboard;
    /* 0x20AC4 [0x51614] */ ObjCharBox mCharBox;
    /* 0x20C80 [0x517D0] */ u8 mUnknown517D0[0x58];
    /* 0x20CD8 [0x51828] */ MessageText mText3;
    /* 0x212C8 [0x51E18] */ SpriteButton mButton2;
    /* 0x213BC [0x51F0C] */ SpriteButton mButton3;
    /* 0x214B0 [0x52000] */ u8 mUnknown52000[0x18];
    /* 0x214C8 [0x52018] */ Scene40Items mItems4;
    /* 0x214F4 [0x52044] */ u8 mUnknown52044[0x14];
    /* 0x21508 [0x52058] */ MessageText mText4;
    /* 0x21AF8 [0x52648] */ SpriteButton mButton4;
    /* 0x21BEC [0x5273C] */ Scene40Button mButtons4[4];
    /* 0x21FCC [0x52B1C] */ MessageText mTexts2[4];
    /* 0x2378C [0x542DC] */ Scene40Button mButtons5[6];
    /* 0x23D5C [0x548AC] */ MessageText mTexts3[6];
    /* 0x260FC [0x56C4C] */ Scene40Button mButtons6[8];
    /* 0x268BC [0x5740C] */ MessageText mTexts4[8];
    /* 0x2983C [0x5A38C] */ u8 mUnknown5A38C[0xDD8];
    /* 0x2A614 [0x5B164] */ MessageText mText5;
    /* 0x2AC04 [0x5B754] */ u8 mUnknown5B754[0xAC];
    /* 0x2ACB0 [0x5B800] */ MessageText mText6;
    /* 0x2B2A0 [0x5BDF0] */ MessageText mTexts5[3];
    /* 0x2C470 [0x5CFC0] */ MessageText mText7;
    /* 0x2CA60 [0x5D5B0] */ u8 mUnknown5D5B0[0xAC];
    /* 0x2CB0C [0x5D65C] */ ObjTextBox mTextBox;
    /* 0x2CCC8 [0x5D818] */ ObjTextBox mTextBox2;
    /* 0x2CE84 [0x5D9D4] */ u8 mUnknown5D9D4[0xAC];
    /* 0x2CF30 [0x5DA80] */ Scene40LevelMeter mSatisfactionMeter;
    /* 0x2D800 [0x5E350] */ Scene40SegmentMeter mTroubleMeter;
    /* 0x2DEF4 [0x5EA44] */ Scene40SpriteMeter mFullnessMeter;
    /* 0x2DFD8 [0x5EB28] */ u8 mUnknown5EB28[0x560];
    /* 0x2E538 [0x5F088] */ MessageText mTexts6[9];
    /* 0x31AA8 [0x625F8] */ ObjTextBox mTextBoxes[11];
    /* 0x32DBC [0x6390C] */ MessageText mText8;
    /* 0x333AC [0x63EFC] */ MessageText mTexts7[11];
    /* 0x374FC [0x6804C] */ MessageText mText9;
    /* 0x37AEC [0x6863C] */ u8 mUnknown6863C[0xBC4];
    /* 0x386B0 [0x69200] */ Scene40CounterDisplay mCounter;
    /* 0x3892C [0x6947C] */ u8 mUnknown6947C[0x38];
    /* 0x38964 [0x694B4] */ Scene40Button mButton5;
    /* 0x38A5C [0x695AC] */ u8 mUnknown695AC[0xC];
    /* 0x38A68 [0x695B8] */ SpriteButton mButtons7[7];
    /* 0x39114 [0x69C64] */ u8 mUnknown69C64[0xC8];
}; // 0x391DC

#endif // OV075_SCENE40PAGES_H
