#ifndef OV074_ROOM_H
#define OV074_ROOM_H

#include <nitro.h>
#include <nnsys.h>

#include "room/Room.h"
#include "text/TextUtil.h"

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
        STATE_UNKNOWN3,     // Reload(7), then ReloadParts(8)
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
