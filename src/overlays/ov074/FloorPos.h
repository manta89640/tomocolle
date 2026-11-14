#ifndef OV074_FLOORPOS_H
#define OV074_FLOORPOS_H

#include <nitro.h>

// A point on the floor of the room: the x and z of a VecFx32. Passed and returned by value, in two registers
// (RoomCharacter::StepWalkTo, the main program's func_02091598, func_0209171c, func_02091edc).
//
// It has to be a plain structure, WITHOUT a constructor: every user of a character's place has dead copies of the
// point on its stack, and those come from accessors that fill a local and return it (RoomCharacter::GetFloorPos).
// Measured: with a constructor, even an unused one, 13 of the state functions no longer match.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;

    // The square of the distance from the middle of the room
    fx32 LengthSq() const { return FX_Mul(x, x) + FX_Mul(z, z); }
    fx32 Dot(const FloorPos &other) const { return FX_Mul(x, other.x) + FX_Mul(z, other.z); }
};

inline FloorPos operator+(FloorPos a, FloorPos b) {
    FloorPos result;
    result.x = a.x + b.x;
    result.z = a.z + b.z;
    return result;
}

inline FloorPos operator-(FloorPos a, FloorPos b) {
    FloorPos result;
    result.x = a.x - b.x;
    result.z = a.z - b.z;
    return result;
}

inline FloorPos Half(FloorPos pos) {
    FloorPos result;
    result.x = pos.x >> 1;
    result.z = pos.z >> 1;
    return result;
}

// The direction of an offset on the floor, as an angle index
inline u16 Angle(FloorPos offset) {
    return FX_Atan2Idx(offset.z, offset.x);
}

inline fx32 LengthSq(const FloorPos &pos) {
    return FX_Mul(pos.x, pos.x) + FX_Mul(pos.z, pos.z);
}

// A rectangle on the floor.
struct FloorRect {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
    /* 0x8 */ fx32 w;
    /* 0xC */ fx32 h;
};

#endif // OV074_FLOORPOS_H
