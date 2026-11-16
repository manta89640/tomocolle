#ifndef CHARACTER_FLOORPOS_H
#define CHARACTER_FLOORPOS_H

#include <nitro.h>

// A point on the floor: the x and z of a VecFx32. Passed and returned by value, in two registers
// (RoomCharacter::StepWalkTo in overlay 74, Character::SetWalkTargetAwayFrom, IsNearerThan).
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

// The same as functions, as RoomObjects' code has them
inline FloorPos Add(FloorPos a, FloorPos b) {
    FloorPos sum;
    sum.x = a.x + b.x;
    sum.z = a.z + b.z;
    return sum;
}

inline FloorPos Sub(FloorPos a, FloorPos b) {
    FloorPos difference;
    difference.x = a.x - b.x;
    difference.z = a.z - b.z;
    return difference;
}

// The point turned around the origin by an angle
inline FloorPos Rotate(const FloorPos &a, int angle) {
    FloorPos turned;
    turned.x = FX_Mul(a.x, FX_CosIdx(angle)) - FX_Mul(a.z, FX_SinIdx(angle));
    turned.z = FX_Mul(a.x, FX_SinIdx(angle)) + FX_Mul(a.z, FX_CosIdx(angle));
    return turned;
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

// The direction of an offset, with length 1: VEC_Normalize's computation (NitroSDK fx_vec.c), in two dimensions
inline FloorPos Normalize(FloorPos v) {
    fx64 t;
    s32 sqrt;
    FloorPos result;

    t = (fx64)v.x * v.x;
    t += (fx64)v.z * v.z;
    CP_SetSqrt64((u64)(t << 2));
    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    sqrt = (s32)CP_GetSqrtResult32();
    t = CP_GetDivResult64();
    t = t * sqrt;
    result.x = (fx32)((t * v.x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    result.z = (fx32)((t * v.z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    return result;
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

#endif // CHARACTER_FLOORPOS_H
