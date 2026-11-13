#ifndef OV073_ROOMCHARACTER_H
#define OV073_ROOMCHARACTER_H

#include <nitro.h>
#include <nnsys.h>

// The character of the tutorial's apartment room (overlay 73): overlay 74's `RoomCharacter` as the tutorial has
// it. It is the same class up to mPrevState and has the same helper functions (byte for byte: SetAnimation,
// IsWalkAnim, ...), but only eighteen states, no held items, no speech and no slot number, so it is 0x2B94 bytes
// where overlay 74's is 0x2D98.
//
// The names are descriptive, not the original: no name of this class survives in the ROM. Those that overlay 74's
// headers have are kept.

// A point on the floor of the room: the x and z of a VecFx32.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
};

// One animation of a model, as AnimModel_FindAnimation finds it by resource type and id.
struct ModelAnimation {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ int mId;
    /* 0x08 */ u32 mResType;
    /* 0x0C */ NNSG3dAnmObj *mAnmObj;
    /* 0x10 */ u32 mUnknown10;
    /* 0x14 */ int mFinished; // Set when a play-once animation reaches its end
    /* 0x18 */ int mPlayOnce;
};

// One saved character (a resident): the 0x2C0 bytes that the save data holds for each (func_0202c264 copies one
// out by index, func_0202c28c back in). Main-program class, not decompiled. Only what this file uses is named.
struct SavedCharacter {
    /* 0x0000 */ u8 mUnknown0000[0x160];
    /* 0x0160 */ u8 mRelation[0xC4];        // For each other character by index; states 12 and 13 greet with
                                            // animation 0x3A when it is 0 for the partner and with 0x39 otherwise
    /* 0x0224 */ u8 mEvent;
    /* 0x0225 */ u8 mUnknown0225[0x70];
    /* 0x0295 */ u8 mMood;                  // 0..4: column of the animation tables, index of the head icons
    /* 0x0296 */ u8 mUnknown0296[0xE];
    /* 0x02A4 */ u16 mIndex;                // Which of the saved characters this is (the save data is asked by this)
    /* 0x02A6 */ s16 mReaction;
    /* 0x02A8 */ u8 mUnknown02A8[0x14];
    /* 0x02BC */ s8 mPersonality;           // Index of data_020fba28, which gives the row of the animation tables
    /* 0x02BD */ u8 mUnknown02BD[3];
}; // 0x2C0

// The character as the main program draws it: the saved character, its face and the turn of its head.
// Main-program class, not decompiled; func_0208fcf8 is its constructor.
struct CharacterBase : public SavedCharacter {
    CharacterBase(); // func_0208fcf8

    /* 0x02C0 */ u8 mUnknown02C0[0x2240];
    /* 0x2500 */ int mFace;
    /* 0x2504 */ u8 mUnknown2504[0xC];
    /* 0x2510 */ int mHeadTurnActive;       // Set by func_ov073_0214db10 with the head turn to blend to
    /* 0x2514 */ u8 mUnknown2514[0xC0];
    /* 0x25D4 */ u16 mUnknown25D4;
    /* 0x25D6 */ u16 mUnknown25D6;          // An angle: added to the head icon's, taken off the angle to turn to
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u16 mHeadPitch;
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknown25E3;
}; // 0x25E4

// The model of a character with its animations. Main-program class (func_0208eb18 constructs it), not decompiled.
struct CharacterModel {
    CharacterModel(); // func_0208eb18

    /* 0x000 */ u8 mUnknown000[0xE0];
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x8];
    /* 0x0F4 */ fx32 mScale;
    /* 0x0F8 */ u8 mUnknown0F8[0xC];
    /* 0x104 */ int mAnimId;      // Current 'BCA0' animation
    /* 0x108 */ int mPrevAnimId;  // The one being blended out of
    /* 0x10C */ int mBlendFrames; // Set on a change of animation; the states wait for it to be <= 0
    /* 0x110 */ u8 mUnknown110[0x4];
}; // 0x114

// The icon shown over a character's head. Main-program class (func_02081034 constructs it), not decompiled.
struct HeadIcon {
    HeadIcon(); // func_02081034

    /* 0x00 */ void *mUnknown00;
    /* 0x04 */ void *mUnknown04;
    /* 0x08 */ int mMode;
    /* 0x0C */ VecFx32 mPos;
    /* 0x18 */ MtxFx33 mRotation;
    /* 0x3C */ u16 mAngle;
    /* 0x3E */ s16 mIconId;
}; // 0x40

// STAND-INS for the two objects of the scene that a character points at; their classes are group b2's.

// The models of a room, its door among them (main-program class, constructor func_020a4544; see overlay 74's
// Room.h).
struct RoomParts {
    /* 0x000 */ u8 mUnknown000[0x8F4];
}; // 0x8F4

// The room (overlay 74's ApartmentRoom, the same class here).
class ApartmentRoom {
public:
    /* 0x000 */ u8 mUnknown000[0x14];
    /* 0x014 */ RoomParts mParts;
    /* 0x908 */ u8 mUnknown908[0x18];
}; // 0x920

// The room's camera (0x0214e41c constructs it). The tutorial's is 0x5C bytes: it has overlay 74's members from
// mPosition on four bytes earlier.
class RoomCamera {
public:
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ VecFx32 mPosition;
    /* 0x18 */ VecFx32 mTarget;
    /* 0x24 */ VecFx32 mOrbit;      // x: elevation, as an angle index times 4096
    /* 0x30 */ u8 mUnknown30[0x18];
    /* 0x48 */ BOOL mIgnoreInput;
    /* 0x4C */ u8 mUnknown4C[0x10];
}; // 0x5C

// The overlay's pool of small constants. In the original these are anonymous: the whole overlay is one translation
// unit, and the compiler puts every initialiser of a small local structure (`FloorPos target = { 0, -0x4800 };`)
// into one object at the start of .rodata, which the code addresses by offset from its start. Their order in it
// follows the compiler's internal numbering of all the file's anonymous objects, so it only comes out as in the
// ROM from the complete original file; until then the pool is declared here, by its address, with the points the
// character walks to on its way in by the door named for where they are (x is 0 for all; the door is at z -9.0).
// The rest of it is used by groups b0 (0x0214a770) and b2 (the touch button, 0x02150348 and 0x02150448).
struct SmallConstants {
    /* 0x00 */ s16 mUnknown00[2];
    /* 0x04 */ FloorPos mPastDoor;   // z -3.4: where state 12 walks to second
    /* 0x0C */ int mUnknown0C[6];
    /* 0x24 */ FloorPos mDoorstep;   // z -6.0: where state 12 walks to first
    /* 0x2C */ FloorPos mInsideDoor; // z -4.5: where state 10 walks to first
    /* 0x34 */ FloorPos mRoomFront;  // z -1.0: where state 10 walks to in the end
};

class RoomCharacter;

extern "C" {
extern const SmallConstants data_ov073_02150ba4;

// --- Main program ---
extern MATHRandContext16 gRandom;
// The row of the two animation tables for each personality, and the tables: standing and walking, by mood
extern const int data_020fba28[16];
extern const s16 data_020fb870[4][5];
extern const s16 data_020fb898[4][5];

void Character_SetFace(RoomCharacter *self, int face);
void Character_ResetSoundPhase(RoomCharacter *self, int slot);
// Starts a sound of a sequence archive at two phases of the current animation
int func_02091fbc(RoomCharacter *self, int player, int index, int seqArcNo);
int Character_PlaySoundAtAnimPhase(RoomCharacter *self, int slot, int player, int seqArcNo, int index, int phaseMask);
BOOL Character_TurnToWalkTarget(RoomCharacter *self, fx32 turnSpeed, int accel);
BOOL Character_TurnToTargetAngle(RoomCharacter *self, fx32 turnSpeed, int accel);
void Character_MoveAlongAngle(RoomCharacter *self);
void Character_SetAngle(RoomCharacter *self, u16 angle);
BOOL Character_IsPointWithinReach(RoomCharacter *self, fx32 x, fx32 z, fx32 reach, fx32 maxReach);
// Moves the character and its model for this frame
void func_020910a8(RoomCharacter *self);
// Draws the character (overlay 74 calls it from its own "if present" wrapper too)
void func_02091968(RoomCharacter *self);
// Copies the character's name out as a string
void func_0207a80c(SavedCharacter *self, wchar_t *name);

// Restart the current animation (from a frame)
void func_0208ef00(CharacterModel *model, int playOnce, int finished);
void func_0208ef28(CharacterModel *model, int playOnce, fx32 frame, int finished);
ModelAnimation *AnimModel_FindAnimation(CharacterModel *model, u32 resType, int id);

void func_02080fc4(HeadIcon *icon);
void func_020810c8(HeadIcon *icon);
void HeadIcon_SetMode(HeadIcon *icon, int mode);
void func_020810e4(HeadIcon *icon); // Updates it

// Starts a motion of the room's door: the character sends 1 before it walks in by it and 2 when it is through
void func_020a4144(RoomParts *parts, int motion);

// --- Overlay 73, group b2's, which have no name yet (each is the tutorial's cut of the overlay 74 function named) ---
// Sets the head turn the character blends to (func_ov074_021691d4)
void func_ov073_0214db10(RoomCharacter *self, u16 pitch, u16 yaw, u16 roll, u8 frames);
// The head's yaw towards a point, limited (func_ov074_021691fc; identical)
u16 func_ov073_0214db3c(RoomCharacter *self, FloorPos pos, fx32 limit);
// The head's pitch towards a joint of the partner, limited (func_ov074_02169318)
u16 func_ov073_0214dc58(RoomCharacter *self, int joint, fx32 limit);
// Walks a step towards the walk target with the head turned to it; TRUE when there (func_ov074_02169388; identical)
BOOL func_ov073_0214dcc8(RoomCharacter *self, u32 headLimit, u16 headYaw, fx32 speed, int seqArcNo, int index);
// Turns a step towards the walk target; TRUE when it faces it (func_ov074_0216943c; identical)
BOOL func_ov073_0214dd7c(RoomCharacter *self, fx32 turnSpeed, int accel, u32 headLimit);
// A random point of the floor
void func_ov073_0214de4c(RoomCharacter *self, FloorPos *out);
// Play a sound of sequence archive 0x1E when the animation changes group (func_ov074_02169b00, func_ov074_02169b68)
void func_ov073_0214e148(RoomCharacter *self);
void func_ov073_0214e1b0(RoomCharacter *self);
}

class RoomCharacter : public CharacterBase {
public:
    RoomCharacter();
    void SetState(int state);
    void Update();
    void UpdateStates01To04();
    void UpdateStates05To08();
    void UpdateState09();
    void UpdateStates0ATo0F();
    void UpdateStates10To12();
    void Draw();
    void CopyName();

    // Group b2's, under overlay 74's names
    void SetAnimation(u32 resType, int id); // 0x0214de28
    void PlaceHeadIcon();                   // 0x0214def8
    void ShowHeadIcon();                    // 0x0214df3c
    BOOL IsWalkAnim();                      // 0x0214dfec

    FloorPos GetFloorPos() {
        FloorPos p;
        p.x = mModel.mPos.x;
        p.z = mModel.mPos.z;
        return p;
    }
    void SetPos(VecFx32 pos) { mModel.mPos = pos; }
    u8 GetRelationToPartner() { return mRelation[mPartner->mIndex]; }
    BOOL IsWithinReach(RoomCharacter *other, fx32 reach, fx32 maxReach) {
        FloorPos pos = other->GetFloorPos();
        return Character_IsPointWithinReach(this, pos.x, pos.z, reach, maxReach);
    }
    void SetWalkTarget(fx32 x, fx32 y, fx32 z) {
        mWalkTarget.x = x;
        mWalkTarget.y = y;
        mWalkTarget.z = z;
    }
    void SetWalkTargetOnFloor(FloorPos pos) {
        mWalkTarget.x = pos.x;
        mWalkTarget.z = pos.z;
    }
    s16 GetIdleAnim() { return data_020fb870[data_020fba28[mPersonality]][mMood]; }
    s16 GetWalkAnim() { return data_020fb898[data_020fba28[mPersonality]][mMood]; }
    // Overlay 74 has this as a function of its own (func_ov074_021694e8)
    void ResetSoundPhaseUnlessWalking() {
        if (!IsWalkAnim()) {
            Character_ResetSoundPhase(this, 0);
        }
    }
    u16 GetHeadYawToCamera() {
        FloorPos pos;
        pos.x = mCamera->mPosition.x;
        pos.z = mCamera->mPosition.z;
        return func_ov073_0214db3c(this, pos, 0x3800);
    }
    void TurnHeadToCamera(u8 frames) {
        FloorPos pos;
        pos.x = mCamera->mPosition.x;
        pos.z = mCamera->mPosition.z;
        u16 angle = mCamera->mOrbit.x >> 12;
        func_ov073_0214db10(this, -(angle >> 2), func_ov073_0214db3c(this, pos, 0x3800), 0, frames);
    }
    void SetHeadIcon(s16 id) {
        func_02080fc4(&mHeadIcon);
        mHeadIcon.mIconId = id;
        func_020810c8(&mHeadIcon);
        HeadIcon_SetMode(&mHeadIcon, 3);
    }

    /* 0x25E4 */ CharacterModel mModel;
    /* 0x26F8 */ u8 mUnknown26F8[0x3F4];
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;   // State 0x12 walks for longer when it is under 1.0
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mWalkTarget;
    /* 0x2B04 */ int mUnknown2B04;
    /* 0x2B08 */ u16 mMoveAngle;
    /* 0x2B0A */ u16 mAngle;
    /* 0x2B0C */ u16 mTargetAngle;
    /* 0x2B0E */ u16 mUnknown2B0E;
    /* 0x2B10 */ s8 mSoundPhase[4];
    /* 0x2B14 */ int mUnknown2B14;
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ BOOL mPresent;       // Update and Draw do nothing without it
    /* 0x2B24 */ RoomCharacter *mPartner;
    /* 0x2B28 */ ApartmentRoom *mApartmentRoom; // Not "mRoom": the saved character has a room number
    /* 0x2B2C */ RoomCamera *mCamera;
    /* 0x2B30 */ HeadIcon mHeadIcon;
    /* 0x2B70 */ int mWait;           // Frames, counted down by Update: the scene chooses what to do next at 0
    /* 0x2B74 */ int mStepSoundArmed; // func_ov073_0214e148 plays no sound the first time, and sets this
    /* 0x2B78 */ s16 mStep;
    /* 0x2B7A */ s16 mTimer;
    /* 0x2B7C */ wchar_t mNameText[11]; // The name as a string, for the scene's messages
}; // 0x2B94

#endif // OV073_ROOMCHARACTER_H
