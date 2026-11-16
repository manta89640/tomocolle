#ifndef OV086_CHANGEROOMSCENE_H
#define OV086_CHANGEROOMSCENE_H

#include <nitro.h>

#include "HeadIcon.h"
#include "Model.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "item/Item.h"
#include "room/Room.h"
#include "shop/Shop.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// Overlay 86, scene 0x38 (not decompiled). The scene factory (CreateScene) loads overlay 86 and allocates 0x17BF8
// bytes; the constructor is the main program's (0x02041F80, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214BAA0). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene mounts "Scn/Scene/SceneChangeRoom_LZ.bin", "Scn/List/List_ChangeRoomList_LZ.bin" and
// "Msg/TmdRoomMsgJP_LZ.bin": it changes a room's interior, from a list of the 78 interiors.
//
// The names are after the scene's archive, "Scn/Scene/SceneChangeRoom_LZ.bin", which no other overlay mounts; they are
// not the original: no name of these classes survives in the ROM.

// The scene's list of interiors: the shops' ItemListView with the five virtual functions of its own (the overlay's
// 0x0214AB4C to 0x0214B17C, table 0x0214BA84). Its constructor is inline: ItemListView's, then its own table.
class ChangeRoomListView : public ItemListView {
public:
    ChangeRoomListView() {}

    virtual void Draw();
    virtual void SetVisible(BOOL visible);
    virtual void Refresh();
    virtual void DrawRow(int row, int item);
    virtual void ClearRow(int row);

    /* 0x13F64 */ u8 mUnknown13F64[0x4C0];
}; // 0x14424

// A part of the scene that the constructor builds from its own address (+0x171E0): a model, the room, particles
// and a head icon. Constructed inline; the name is descriptive, not the original.
struct ChangeRoomPart {
    /* 0x000 */ u8 mUnknown000[0x10];
    /* 0x010 */ Model mModel;
    /* 0x0C4 */ Room mRoom;
    /* 0x9CC */ u8 mUnknown9CC[0x4];
    /* 0x9D0 */ ParticleSystem mParticles;
    /* 0x9D4 */ HeadIcon mHeadIcons[1];
    /* 0xA14 */ u8 mUnknownA14[0x4];
}; // 0xA18

class ChangeRoomScene : public Scene {
public:
    ChangeRoomScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0xC];
    /* 0x0001C */ MessageFile mMessageFiles[3];
    /* 0x00064 */ SavedCharacter mCharacter;
    /* 0x00324 */ u8 mUnknown00324[0x4];
    /* 0x00328 */ InteriorItem mInteriors[78];
    /* 0x00940 */ u8 mUnknown00940[0x13C];
    /* 0x00A7C */ InteriorItem mInteriors2[78];
    /* 0x01094 */ u8 mUnknown01094[0x274];
    /* 0x01308 */ MessageText mText;
    /* 0x018F8 */ TextButton mTextButton;
    /* 0x01FE8 */ TextButton mTextButton2;
    /* 0x026D8 */ SpriteButton mButton;
    /* 0x027CC */ ChangeRoomListView mList;
    /* 0x16BF0 */ MessageText mText2;
    /* 0x171E0 */ ChangeRoomPart mPart;
}; // 0x17BF8

#endif // OV086_CHANGEROOMSCENE_H
