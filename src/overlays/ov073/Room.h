#ifndef OV073_ROOM_H
#define OV073_ROOM_H

#include <nitro.h>
#include "text/TextUtil.h"
#include <nnsys.h>

#include "Model.h"

// Overlay 74's Room.h, with the tutorial's own TutorialApartmentRoom.
//
// STAND-INS for the main program's room classes (0x020a3c68 to 0x020a5ca0), which are not decompiled yet: their
// sizes, the fields the apartment scene's code touches, and their destructors as the scene's destructor has them
// inlined. The names are descriptive placeholders.

// The room's door ("XXX:/close.ica"): a model, and a destructor that takes the model's render callback away. The
// model is a member, not a base: the scene's destructor has the callback reset first and then AnimModel's
// destructor, without a virtual table of the door's own being stored in between (as overlay 74 found).
class RoomDoorModel {
public:
    ~RoomDoorModel() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    /* 0x000 */ AnimModel mModel;

    /* 0x0E0 */ u8 mUnknownE0[0x58];
    /* 0x138 */ int mMotion; // RoomDoor::SetMotion sets it (1 and 2 as a character leaves or enters); 0 again when the door is done
    /* 0x13C */ u8 mUnknown13C[0x1C];
}; // 0x158

// One of the room's six parts; RoomModels::Update hides those between the camera and the room.
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

// The main program's room door (include/room/Room.h): RoomModels starts with it
class RoomDoor {
public:
    void SetMotion(int motion); // 1 and 2 as a character leaves or enters, 0 again when the door is done
};

// The models of a room (include/room/Room.h).
class RoomModels {
public:
    ~RoomModels() {}

    RoomDoor *GetDoor() { return (RoomDoor *)this; }
    void Draw();
    void SetState(int state);
    void SetUnknown(int value);
    void SetVisible(BOOL visible);

    /* 0x000 */ u8 mUnknown000[0x4C];
    /* 0x04C */ RoomDoorModel mDoor;
    /* 0x1A4 */ RoomPart mParts[6];
    /* 0x8DC */ void *mCamera;   // The main program's camera, data_02122764
    /* 0x8E0 */ int mState;      // SetState sets it and clears mStep
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

// A room of a given style (virtual table _ZTV4Room: the seven functions of InteriorItem, which read
// the style's entry of data_020f72f8).
class InteriorItem {
public:
    virtual u32 GetPrice();
    virtual u32 GetPrice2();
    virtual const char *GetModelName();
    virtual u8 GetWeight0();
    virtual u8 GetWeight1();
    virtual u8 GetWeight2();
    virtual u8 GetWeight3();

    GXRgb GetLightColor(); // The room's light colour now, whole and split
    void GetLightRGB(int *r, int *g, int *b);

    /* 0x004 */ u16 mStyle; // Index into data_020f72f8
    /* 0x008 */ u8 mUnknown008[0xC];
}; // 0x14

class Room : public InteriorItem {
public:
    Room();
    ~Room() {}

    void Mount(const char *archiveName, int unk);
    void MountModel(int unk);
    void Load(int resourceId);
    void Reload(int resourceId);
    void MountAll(const char *archiveName, int unk);
    void MountPartModels(int unk);
    void LoadParts(int resourceId);
    void ReloadParts(int resourceId);
    void ReleaseAll();
    void Update();

    /* 0x014 */ RoomModels mParts;
}; // 0x908

extern "C" {
extern const RoomStyle data_020f72f8[];
// --- The room's functions, as the apartment scene calls them ---
// The room's current light colour, whole and split
// Writes a number as full-width digits: (value, buffer, buffer size in bytes, digits, BOOL unk)
}

// The room of the tutorial's apartment scene: a Room that can change its style in the background, and that knows
// its number in the building. Overlay 74's ApartmentRoom in the tutorial's cut: the same layout, but its fourth
// state gives Reload and ReloadParts other values and it has no SetNextStyle.
//
// The name is descriptive, not the original: no name of this class survives in the ROM. The prefix tells it from
// overlay 74's: the program is linked as one, so the two cannot have one name.
class TutorialApartmentRoom : public Room {
public:
    enum State {
        STATE_IDLE,
        STATE_RESET,        // Reset the parts and write the room number
        STATE_CHANGE_STYLE, // Take mNextStyle and load it
        STATE_UNKNOWN3,     // Reload(3), then ReloadParts(4) (overlay 74 gives them 7 and 8)
    };

    TutorialApartmentRoom();
    void Refresh();
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

#endif // OV073_ROOM_H
