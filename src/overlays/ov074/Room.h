#ifndef OV074_ROOM_H
#define OV074_ROOM_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"

// Shared by the units of overlay 74 (it began as RoomObjects' Room.h).
//
// STAND-INS for the main program's room classes (0x020a3c68 to 0x020a5ca0), which are not decompiled yet: their
// sizes, the fields the apartment scene's code touches, and their destructors as the scene's destructor has them
// inlined. The names are descriptive placeholders.

// A model whose destructor takes its render callback away (the room's door, "XXX:/close.ica").
class RoomDoorModel : public AnimModel {
public:
    ~RoomDoorModel() { NNS_G3dRenderObjResetCallBack(&mRenderObj); }

    /* 0x0E0 */ u8 mUnknownE0[0x58];
    /* 0x138 */ int mMotion; // func_020a4144 sets it (1 and 2 as a character leaves or enters); 0 again when the door is done
    /* 0x13C */ u8 mUnknown13C[0x1C];
}; // 0x158

// One of the room's six parts; func_020a4718 hides those between the camera and the room.
class RoomPart {
public:
    ~RoomPart() {}

    /* 0x000 */ int mUnknown00; // Negative if the part is not used
    /* 0x004 */ VecFx32 mUnknown04;
    /* 0x010 */ u8 mUnknown10[0x3C];
    /* 0x04C */ AnimModel mModel;
    /* 0x12C */ int mUnknown12C;
    /* 0x130 */ int mUnknown130;
}; // 0x134

// The models of a room (constructor func_020a4544).
class RoomParts {
public:
    ~RoomParts() {}

    /* 0x000 */ u8 mUnknown000[0x4C];
    /* 0x04C */ RoomDoorModel mDoor;
    /* 0x1A4 */ RoomPart mParts[6];
    /* 0x8DC */ void *mCamera;   // The main program's camera, data_02122764
    /* 0x8E0 */ int mState;      // func_020a4ab8 sets it and clears mStep
    /* 0x8E4 */ int mUnknown8E4;
    /* 0x8E8 */ fx32 mUnknown8E8;
    /* 0x8EC */ fx32 mUnknown8EC;
    /* 0x8F0 */ s16 mStep;
}; // 0x8F4

// The styles a room can have (data_020f72f8), 0x54 bytes each
struct RoomStyle {
    /* 0x00 */ u8 mUnknown00[8];
    /* 0x08 */ u16 mLightColor;
    /* 0x0A */ u8 mUnknown0A[0x4A];
}; // 0x54

// A room of a given style (constructor func_020a5408, virtual table data_021150c8: seven functions that read
// the style's entry of data_020f72f8).
class Room {
public:
    Room(); // func_020a5408
    ~Room() {}

    virtual u32 GetStyleUnknown0();   // func_020a5ca0
    virtual u32 GetStyleUnknown1();   // func_020a5c7c
    virtual void *GetStyleUnknown2(); // func_020a5484
    virtual u8 GetStyleUnknown3();    // func_020a5c54
    virtual u8 GetStyleUnknown4();    // func_020a5c2c
    virtual u8 GetStyleUnknown5();    // func_020a5c04
    virtual u8 GetStyleUnknown6();    // func_020a5bdc

    /* 0x004 */ u16 mStyle; // Index into data_020f72f8
    /* 0x008 */ u8 mUnknown008[0xC];
    /* 0x014 */ RoomParts mParts;
}; // 0x908

extern "C" {
extern const RoomStyle data_020f72f8[];
// --- The room's functions, as the apartment scene calls them ---
void func_020a4ab8(RoomParts *parts, int state); // Sets the state of the parts
void func_020a4144(RoomParts *parts, int motion); // Starts a motion of the door (RoomDoorModel::mMotion)
void func_020a4b74(RoomParts *parts, int unk);
void func_020a5948(Room *room);
void func_020a5960(Room *room); // Updates the parts (func_020a4718)
void func_020a54e4(Room *room, int unk); // The steps of changing to the style in mStyle
void func_020a56dc(Room *room, int unk);
void func_020a5530(Room *room, int unk);
void func_020a5854(Room *room, int unk);
// The room's current light colour, whole and split
u16 func_020a4d90(Room *room);
void func_020a4dcc(Room *room, int *r, int *g, int *b);
// Writes a number as full-width digits: (value, buffer, buffer size in bytes, digits, BOOL unk)
wchar_t *func_0203673c(int value, wchar_t *buffer, int size, int digits, BOOL unk);
}

// The room of the apartment scene: a Room that can change its style in the background, and that knows its
// number in the building.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class ApartmentRoom : public Room {
public:
    enum State {
        STATE_IDLE,
        STATE_RESET,        // Reset the parts and write the room number
        STATE_CHANGE_STYLE, // Take mNextStyle and load it
        STATE_UNKNOWN3,     // func_020a5530(7), then func_020a5854(8)
    };

    ApartmentRoom();
    void Refresh();
    void SetNextStyle(u16 style);
    void SetStyle(u16 style);
    void UpdateNumberText();
    void Update();
    void SetState(int state);
    void UpdateState();

    /* 0x908 */ int mState;
    /* 0x90C */ u16 mNextStyle;
    /* 0x90E */ u16 mRoomIndex;      // Eight rooms to a floor
    /* 0x910 */ u16 mUnknown910;
    /* 0x912 */ s16 mStep;
    /* 0x914 */ wchar_t mNumberText[6]; // Floor and room, as "101"
}; // 0x920

#endif // OV074_ROOM_H
