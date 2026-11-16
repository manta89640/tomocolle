#ifndef OV075_SCENE40CHARACTER_H
#define OV075_SCENE40CHARACTER_H

#include <nitro.h>

#include "HeadIcon.h"
#include "Model.h"
#include "character/Character.h"
#include "item/Item.h"
#include "voice/TtsPlayer.h"

// Overlay 75's character, the Mii the player visits and touches, and the two objects with a model it carries. The
// names are descriptive: no name of these classes survives in the ROM.

// The goods and clothes the scene shows, two bases: the constructor stores both of this class's tables (this
// file's data, 0x02110B4C with the second at +0x2C) after both base constructors.
class Scene40Items : public InventoryItem, public ClothesItem {}; // 0x2C

// Two objects of the scene's character, each with a model that its Draw draws (0x02178700, 0x02178cf0). What they
// show is not known: the names say only where they are.
class Scene40CharacterPartA {
public:
    void Draw();

    /* 0x00 */ u8 mUnknown00[0x10];
    /* 0x10 */ AnimModel mModel;
    /* 0xF0 */ u8 mUnknownF0[0x8];
}; // 0xF8

class Scene40CharacterPartB {
public:
    void Draw();

    /* 0x00 */ u8 mUnknown00[0x4];
    /* 0x04 */ AnimModel mModel;
}; // 0xE4

// A character with two models, the goods and clothes it holds and two head icons
class Scene40Character : public Character {
public:
    void Draw(); // The character, then the two objects with a model each at 0x2BE4 and 0x2CDC
    void Update(BOOL speaking);

    /* 0x2B18 */ u8 mUnknown2B18[0xC0];
    /* 0x2BD8 */ TtsPlayer *mTtsPlayer; // The scene's, for the lip movement
    /* 0x2BDC */ u8 mUnknown2BDC[0x8];
    /* 0x2BE4 */ Scene40CharacterPartA mPartA;
    /* 0x2CDC */ Scene40CharacterPartB mPartB;
    /* 0x2DC0 */ u8 mUnknown2DC0[0x4];
    /* 0x2DC4 */ Scene40Items mItems;
    /* 0x2DF0 */ u8 mUnknown2DF0[0xC];
    /* 0x2DFC */ HeadIcon mHeadIcons[2];
    /* 0x2E7C */ u8 mUnknown2E7C[0x48];
    /* 0x2EC4 */ BOOL mLipSync;         // Update moves the lips with the speech only while it is set
    /* 0x2EC8 */ u8 mUnknown2EC8[0x230];
}; // 0x30F8

#endif // OV075_SCENE40CHARACTER_H
