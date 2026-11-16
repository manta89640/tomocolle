#include <nitro.h>

#include "GameData.h"

// The apartment building: how large it is, by the number of residents, and where its rooms are.

// Where a window of the building's front is: its column and its floor from the bottom
struct WindowCell {
    /* 0x0 */ int mColumn;
    /* 0x4 */ int mFloor;
};

// The number of residents from which the building has its second, third and fourth stage
static const u16 sStageResidents[3] = {20, 45, 70};
// The number of rooms of the building in each stage
static const u16 sStageRooms[4] = {24, 48, 72, 100};
// The number of rooms on each floor, from the bottom
static const int sFloorRooms[13] = {8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 4};

int GetApartmentStage(u16 residents) {
    int stage = 0;
    if (residents >= sStageResidents[2]) {
        stage = 3;
    } else if (residents >= sStageResidents[1]) {
        stage = 2;
    } else if (residents >= sStageResidents[0]) {
        stage = 1;
    }
    return stage;
}

u16 GetApartmentStageRooms(int stage);

u16 GetApartmentRooms() {
    return GetApartmentStageRooms(gGameData.mApartmentStage);
}

u16 GetApartmentStageRooms(int stage) {
    return sStageRooms[stage];
}

WindowCell GetWindowCell(int window) {
    WindowCell cell;
    int first = 0;
    for (int floor = 0; floor < 13; floor++) {
        int next = first + sFloorRooms[floor];
        if (window < next) {
            cell.mColumn = window - first;
            cell.mFloor = floor;
            break;
        }
        first = next;
    }
    return cell;
}
