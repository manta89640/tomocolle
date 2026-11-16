#ifndef OV092_WRITESONGSCENE_H
#define OV092_WRITESONGSCENE_H

#include <nitro.h>

#include "MessageWindow.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "keyboard/SoftKeyboard.h"
#include "room/Room.h"
#include "song/Song.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// Overlay 92, scenes 0x5B, 0x5C and 0xB0 (not decompiled). The scene factory (CreateScene) loads overlay 92 and
// allocates 0x81224 bytes; the constructor is the main program's (0x02044C80, src/scene/SceneManager.cpp), which
// stores the overlay's virtual table (0x02154F54). The members are as the constructor constructs them, by their
// classes; what lies between is not known. The scene mounts "Scn/Scene/SceneWriteSong_LZ.bin",
// "Scn/List/List_WriteSongList_LZ.bin", "Mdl/Misc/write_song_3D_LZ.bin", the singing animations
// "Mdl/Ica/ica_mii_song_*_LZ.bin" and "Msg/TmdSongMsgJP_LZ.bin".
//
// The names are after the scene's archive, "Scn/Scene/SceneWriteSong_LZ.bin", which no other overlay mounts; they are
// not the original: no name of these classes survives in the ROM.

// A model of the scene with a character and an item of its own. Its table (0x02154E64) and D0 are the overlay's,
// which overrides Draw (0x0215021C); its constructor is inline, and its implicit D1 is the main program's
// (0x0204509C, after the scene's constructor), which the table refers to.
class WriteSongModel : public AnimModel {
public:
    virtual void Draw();

    /* 0x00E0 */ u8 mUnknown00E0[0xC];
    /* 0x00EC */ CharacterBase mCharacters[1];
    /* 0x26D0 */ ItemModel mItemModels[1];
}; // 0x27FC

// A part of the scene that the constructor builds from its own address: two texts, the speech player and a song
// player. Constructed inline; the name is descriptive, not the original.
struct WriteSongPart {
    /* 0x0000 */ u8 mUnknown0000[0x40];
    /* 0x0040 */ MessageText mText0040;
    /* 0x0630 */ MessageText mText0630;
    /* 0x0C20 */ u8 mUnknown0C20[0x204];
    /* 0x0E24 */ TtsPlayer mTtsPlayer;
    /* 0x15768 */ u8 mUnknown15768[0x6C];
    /* 0x157D4 */ SongEntry mSong;
    /* 0x15818 */ u8 mUnknown15818[0x4];
}; // 0x1581C

class WriteSongScene : public Scene {
public:
    WriteSongScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x140];
    /* 0x00150 */ SceneCharacter mCharacters[1];
    /* 0x02C68 */ u8 mUnknown02C68[0x1BBC];
    /* 0x04824 */ AnimModel mModel;
    /* 0x04904 */ AnimModel mModel2;
    /* 0x049E4 */ WriteSongModel mSongModel;
    /* 0x071E0 */ u8 mUnknown071E0[0x10];
    /* 0x071F0 */ Room mRoom;
    /* 0x07AF8 */ u8 mUnknown07AF8[0x20];
    /* 0x07B18 */ SpriteButton mButtons[1];
    /* 0x07C0C */ TextButton mTextButtons[8];
    /* 0x0B38C */ TextButton mTextButtons2[32];
    /* 0x1918C */ u8 mUnknown1918C[0x8];
    /* 0x19194 */ MessageWindow mWindow;
    /* 0x24138 */ u8 mUnknown24138[0xC];
    /* 0x24144 */ MessageText mTexts[9];
    /* 0x276B4 */ u8 mUnknown276B4[0x5C];
    /* 0x27710 */ MessageText mTexts2[32];
    /* 0x33510 */ MessageFile mMessageFiles[3];
    /* 0x33558 */ u8 mUnknown33558[0x196FC];
    /* 0x4CC54 */ SpriteButton mButton4CC54;
    /* 0x4CD48 */ SpriteButton mButton4CD48;
    /* 0x4CE3C */ u8 mUnknown4CE3C[0x1D0];
    /* 0x4D00C */ SpriteButton mButtons2[7];
    /* 0x4D6B8 */ ObjTextBox mTextBoxes[7];
    /* 0x4E2DC */ u8 mUnknown4E2DC[0x15E0C];
    /* 0x640E8 */ SpriteButton mButtons3[16];
    /* 0x65028 */ u8 mUnknown65028[0x14];
    /* 0x6503C */ WriteSongPart mPart;
    /* 0x7A858 */ SoftKeyboard mKeyboard;
    /* 0x80850 */ u8 mUnknown80850[0x4];
    /* 0x80854 */ ObjCharBox mCharBox;
    /* 0x80A10 */ u8 mUnknown80A10[0x1C];
    /* 0x80A2C */ SpriteButton mButton80A2C;
    /* 0x80B20 */ MessageText mText;
    /* 0x81110 */ u8 mUnknown81110[0x14];
    /* 0x81124 */ SpriteButton mButtons4[1];
    /* 0x81218 */ u8 mUnknown81218[0xC];
}; // 0x81224

#endif // OV092_WRITESONGSCENE_H
