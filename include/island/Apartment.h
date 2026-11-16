#ifndef ISLAND_APARTMENT_H
#define ISLAND_APARTMENT_H

#include <nitro.h>

// The apartment building (src/island/Apartment.cpp): its stage by the number of residents, its rooms, and where a
// room's window is on its front.

// Where a window of the building's front is: its column and its floor from the bottom
struct WindowCell {
    /* 0x0 */ int mColumn;
    /* 0x4 */ int mFloor;
};

int GetApartmentStage(u16 residents);    // 0 to 3: from 20, 45 and 70 residents on
u16 GetApartmentRooms();                 // The rooms of the building's stage in the saved game
u16 GetApartmentStageRooms(int stage);   // 24, 48, 72 or 100
WindowCell GetWindowCell(int window);

#endif // ISLAND_APARTMENT_H
