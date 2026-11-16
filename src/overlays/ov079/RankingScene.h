#ifndef OV079_RANKINGSCENE_H
#define OV079_RANKINGSCENE_H

#include <nitro.h>

#include "G3Sprite.h"
#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// Overlay 79, scene 0x2A (not decompiled). The scene factory (CreateScene) loads overlay 79 and allocates 0x2AC10
// bytes; the constructor is the main program's (0x020408A4, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x021508B4). The members are as the constructor constructs them, by their classes; what lies
// between is not known. The scene's StartLoading mounts "Msg/TmdContentsMsgJP_LZ.bin",
// "Scn/Scene/SceneRanking_LZ.bin", "Scn/Scene/Sky_LZ.bin", the models "Mdl/Misc/Rnk/rank_board_3D_LZ.bin",
// "rank_poster", "rank_black" and "rank_cond", and "Mdl/Ica/head_ICA_LZ.bin".
//
// The names are after the scene's archive, "Scn/Scene/SceneRanking_LZ.bin", which no other overlay mounts, and
// descriptive; they are not the original: no name of these classes survives in the ROM.

// The base of the scene's two kinds of piece: a model and eight virtual functions that the base leaves to them (its
// table, at 0x02110994 in the main program, has eight empty entries). Constructed inline: the table, then the model.
class RankingPiece {
public:
    virtual void VirtualUnknown0() = 0;
    virtual void VirtualUnknown1() = 0;
    virtual void VirtualUnknown2() = 0;
    virtual void VirtualUnknown3() = 0;
    virtual void VirtualUnknown4() = 0;
    virtual void VirtualUnknown5() = 0;
    virtual void VirtualUnknown6() = 0;
    virtual void VirtualUnknown7() = 0;

    /* 0x04 */ u8 mUnknown04[0xC];
    /* 0x10 */ AnimModel mModel;
}; // 0xF0

// One kind of piece, five of them (table 0x02150900 of the overlay): two text boxes and two text canvases. Its
// implicit constructor and destructor are the main program's copies after the scene's constructor (0x02040B40,
// 0x02040BFC), emitted for the scene's __cxa_vec_ctor.
class RankingPieceA : public RankingPiece {
public:
    virtual void VirtualUnknown0();
    virtual void VirtualUnknown1();
    virtual void VirtualUnknown2();
    virtual void VirtualUnknown3();
    virtual void VirtualUnknown4();
    virtual void VirtualUnknown5();
    virtual void VirtualUnknown6();
    virtual void VirtualUnknown7();

    /* 0x0F0 */ u8 mUnknown0F0[0xC];
    /* 0x0FC */ ObjTextBox mTextBox0FC;
    /* 0x2B8 */ u8 mUnknown2B8[0x220];
    /* 0x4D8 */ ObjTextCanvas mTextCanvas4D8;
    /* 0x690 */ ObjTextBox mTextBox690;
    /* 0x84C */ ObjTextCanvas mTextCanvas84C;
    /* 0xA04 */ u8 mUnknownA04[0x62C];
}; // 0x1030

// The other kind, three of them (table 0x021508D8 of the overlay): three text boxes and a text canvas. Its implicit
// constructor and destructor are the main program's copies 0x02040C3C and 0x02040D2C.
class RankingPieceB : public RankingPiece {
public:
    virtual void VirtualUnknown0();
    virtual void VirtualUnknown1();
    virtual void VirtualUnknown2();
    virtual void VirtualUnknown3();
    virtual void VirtualUnknown4();
    virtual void VirtualUnknown5();
    virtual void VirtualUnknown6();
    virtual void VirtualUnknown7();

    /* 0x0F0 */ u8 mUnknown0F0[0x10];
    /* 0x100 */ ObjTextBox mTextBoxes[2];
    /* 0x478 */ ObjTextBox mTextBox478;
    /* 0x634 */ u8 mUnknown634[0x220];
    /* 0x854 */ ObjTextCanvas mTextCanvas854;
    /* 0xA0C */ ObjTextBox mTextBoxA0C;
    /* 0xBC8 */ u8 mUnknownBC8[0x900];
}; // 0x14C8

// The scene's characters, six of them: a CharacterHead (gap66-a's) and 0x10 bytes more. Its implicit constructor and
// destructor are the main program's copies 0x02040D6C and 0x02040DC0.
class RankingCharacter : public CharacterHead {
public:
    /* 0x2728 */ u8 mUnknown2728[0x10];
}; // 0x2738

// An object of the overlay's own, constructed by the overlay (0x0214FEFC).
class RankingObject {
public:
    RankingObject();

    /* 0x00 */ u8 mUnknown00[0x24];
}; // 0x24

// A part of the scene with its own members, which the constructor constructs from the part's address (+0x4E9C of
// the scene): the pieces, the characters and their models.
struct RankingPart {
    /* 0x00000 */ u8 mUnknown00000[0x74];
    /* 0x00074 */ RankingPieceA mPiecesA[5];
    /* 0x05164 */ RankingPieceB mPiecesB[3];
    /* 0x08FBC */ u8 mUnknown08FBC[0x14];
    /* 0x08FD0 */ SpriteButton mButton08FD0;
    /* 0x090C4 */ SpriteButton mButton090C4;
    /* 0x091B8 */ u8 mUnknown091B8[0x13C];
    /* 0x092F4 */ RankingCharacter mCharacters[6];
    /* 0x17E44 */ AnimModel mModels[10];
    /* 0x18704 */ AnimModel mModel;
    /* 0x187E4 */ u8 mUnknown187E4[0x4];
}; // 0x187E8

class RankingScene : public Scene {
public:
    RankingScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x8];
    /* 0x00018 */ MessageFile mMessageFile;
    /* 0x00030 */ u8 mUnknown00030[0x4E6C];
    /* 0x04E9C */ RankingPart mPart;
    /* 0x1D684 */ MessageWindow mWindow;
    /* 0x28628 */ SpriteButton mButtons[7];
    /* 0x28CD4 */ SpriteButton mButton28CD4;
    /* 0x28DC8 */ TextButton mTextButton;
    /* 0x294B8 */ u8 mUnknown294B8[0x10];
    /* 0x294C8 */ AnimModel mModel294C8;
    /* 0x295A8 */ AnimModel mModels295A8[7];
    /* 0x29BC8 */ AnimModel mModel29BC8;
    /* 0x29CA8 */ u8 mUnknown29CA8[0xEF8];
    /* 0x2ABA0 */ G3Sprite mG3Sprite;
    /* 0x2ABEC */ RankingObject mObject;
}; // 0x2AC10

#endif // OV079_RANKINGSCENE_H
