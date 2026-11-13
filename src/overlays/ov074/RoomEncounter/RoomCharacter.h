#ifndef OV074_ROOMCHARACTER_H
#define OV074_ROOMCHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

// A character standing in the apartment room: the room's resident, or the visitor (the class name is group a6's,
// see NOTES.md). This is the part of it that the scene's own functions (0x0214e5f8 to 0x02152690) use.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// A point on the floor of the room: the x and z of a VecFx32 (the name is group a6's). The main program's
// character functions take and return it by value (func_02091edc, func_02091abc). It has to stay a plain structure
// that is copied whole: every user of a character's place has a dead copy of the point next to the one it uses.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;

    fx32 LengthSq() const { return FX_Mul(x, x) + FX_Mul(z, z); }
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

// The character as the main program knows it and saves it: name, personality, and what it feels for each of the
// others. Main-program class, not decompiled; func_0208fcf8 is its constructor. Only what this file uses is named.
struct CharacterBase {
    CharacterBase(); // func_0208fcf8

    /* 0x0000 */ u16 mUnknown0000;
    /* 0x0002 */ wchar_t mName[10];      // func_0207a80c copies it out as a string
    /* 0x0016 */ u8 mUnknown0016[0x34];
    /* 0x004A */ u8 mUnknown004A;        // func_02098ec4 computes a value up to 200 from these two
    /* 0x004B */ u8 mUnknown004B;
    /* 0x004C */ u8 mUnknown004C[3];
    /* 0x004F */ u8 mUnknown004F;        // Given to the scene's object at 0x6948 when the character's event is 18
    /* 0x0050 */ u8 mUnknown0050[0x24];
    /* 0x0074 */ u8 mUnknown0074[0x10];
    /* 0x0084 */ u8 mSpeechDefault[0x22];   // RoomCharacter::Init points mSpeechParam[0] here
    /* 0x00A6 */ u8 mSpeechByMood[4][0x12]; // and mSpeechParam[1..4] here
    /* 0x00EE */ u8 mUnknown00EE[0xE];
    /* 0x00FC */ u8 mAffinity[100];      // For each other character by index: func_02099088 adds to it, 0 to 200
    /* 0x0160 */ u8 mRelation[0xC4];     // For each other character by index: func_020990b0 sets it; row of data_021135c8
    /* 0x0224 */ u8 mEvent;              // What the character is doing in its room: 0..18 alone, 0x18.. with a partner
    /* 0x0225 */ u8 mEventPartner;       // mIndex of the character it is doing it with
    /* 0x0226 */ u8 mUnknown0226[0x14];
    /* 0x023A */ u8 mMoodLocked;         // func_02098fe0 and the scene leave mMood alone when this is set
    /* 0x023B */ u8 mUnknown023B[0x5A];
    /* 0x0295 */ u8 mMood;               // 0..4 (name is group a2's): index of the head marks and speech texts
    /* 0x0296 */ u8 mUnknown0296[0xE];
    /* 0x02A4 */ u16 mIndex;             // Which of the saved characters this is (gGameData is asked for it by this)
    /* 0x02A6 */ s16 mReaction;          // What the character chose to do next (func_ov074_021686c0, func_0209d5ac)
    /* 0x02A8 */ u8 mUnknown02A8[0x232C];
    /* 0x25D4 */ u16 mUnknown25D4;
    /* 0x25D6 */ u16 mUnknown25D6;       // An angle: added to the head mark's
    /* 0x25D8 */ u8 mUnknown25D8[0xC];
}; // 0x25E4

// The model of a character with its animations. Main-program class (func_0208eb18 constructs it), not decompiled.
struct CharacterModel {
    CharacterModel(); // func_0208eb18

    /* 0x000 */ u8 mUnknown000[0xA6];
    /* 0x0A6 */ u16 mUnknown00A6; // func_020910a8 sets it to 0x4000 minus the character's angle
    /* 0x0A8 */ u8 mUnknown00A8[0x38];
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown00EC[0x8];
    /* 0x0F4 */ fx32 mScale;
    /* 0x0F8 */ u8 mUnknown00F8[0x1C];
}; // 0x114

// A node of the model with a position at 0x24 (name is group a6's).
struct PositionedNode {
    VecFx32 GetPos() const { return mPos; }

    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ VecFx32 mPos;
}; // 0x30

// The icon shown over a character's head (name is group a6's). Main-program class (func_02081034 constructs it,
// func_020810e4 updates it), not decompiled.
struct HeadIcon {
    HeadIcon();                 // func_02081034
    void Show();                // func_02080fc4
    void Restart();             // func_02080ff0
    void Load(int param, int iconId); // func_020810b4: keeps the id and calls func_02080f64 with mPos
    void Reload();              // func_020810c8
    void SetMode(int mode);     // HeadIcon_SetMode
    void Update();              // func_020810e4
    void SetPos(fx32 x, fx32 y, fx32 z) {
        mPos.x = x;
        mPos.y = y;
        mPos.z = z;
    }

    /* 0x00 */ void *mUnknown00;
    /* 0x04 */ void *mUnknown04;
    /* 0x08 */ int mMode;
    /* 0x0C */ VecFx32 mPos;
    /* 0x18 */ MtxFx33 mRotation;
    /* 0x3C */ u16 mAngle;
    /* 0x3E */ s16 mIconId;
}; // 0x40

class RoomCharacter;

extern "C" {
// --- Main program, not named yet: tests of where a point (x, z) is, seen from the character ---
// The point is within that width of the line the character faces along (func_02091d44: and in front of it)
BOOL func_02091b80(RoomCharacter *character, fx32 x, fx32 z, fx32 width);
BOOL func_02091c80(RoomCharacter *character, fx32 x, fx32 z);
BOOL func_02091d44(RoomCharacter *character, fx32 x, fx32 z, fx32 width);
// The point is nearer than the character's scale times the reach (and than the maximum, if that is positive)
BOOL Character_IsPointWithinReach(RoomCharacter *character, fx32 x, fx32 z, fx32 reach, fx32 maxReach);
}

extern "C" {
// The point is nearer to the character than the square root of that
BOOL func_02091edc(RoomCharacter *character, FloorPos pos, fx32 distanceSq);
}

class RoomCharacter : public CharacterBase {
public:
    RoomCharacter();
    void Init(int slot, int param2, int param3);
    void LoadHeadIcon(int param);
    void PlaceHeadIcon();
    void SetState(int state);
    void SetStateWithPartner(int state, int partnerState);
    void CopyName();
    void SetEvent(u8 event);
    void ShowHeadIcon();
    void Update();

    // Other units' (0x02168798 to 0x02168c28)
    BOOL IsOneTwoOrFour(int value);
    BOOL IsState8Or9();
    BOOL IsState10To12();
    void GetRandomPosInArea(FloorPos *out);
    void GetRandomPos(FloorPos *out);
    BOOL ClampToWalkArea(FloorPos *pos);
    BOOL ClampToRect4E4(FloorPos *pos);
    BOOL ClampToRect534(FloorPos *pos);
    void GetRandomCorner(FloorPos *out);
    void GetRandomEdgePos(FloorPos *out);

    PositionedNode *GetNode(int i) { return &mNodes[i]; }
    FloorPos GetFloorPos() const {
        FloorPos pos;
        pos.x = mModel.mPos.x;
        pos.z = mModel.mPos.z;
        return pos;
    }
    void SetTarget(fx32 x, fx32 y, fx32 z) {
        mTarget.x = x;
        mTarget.y = y;
        mTarget.z = z;
    }
    void SetTarget(fx32 x, fx32 z) {
        mTarget.x = x;
        mTarget.z = z;
    }
    void SetFloorPos(fx32 x, fx32 z) {
        mModel.mPos.x = x;
        mModel.mPos.z = z;
    }
    void SetSavedTarget(VecFx32 target) {
        mSavedTarget.x = target.x;
        mSavedTarget.y = target.y;
        mSavedTarget.z = target.z;
    }
    BOOL ClampOwnPosToRect4E4() {
        FloorPos pos = GetFloorPos();
        return ClampToRect4E4(&pos);
    }
    // From where the character stands to a point
    FloorPos GetOffsetTo(const FloorPos &to) { return to - GetFloorPos(); }

    // The main program's tests of where another character is, seen from this one
    BOOL IsBeside(const RoomCharacter *other, fx32 width) {
        FloorPos pos = other->GetFloorPos();
        return func_02091b80(this, pos.x, pos.z, width);
    }
    BOOL IsFacing(const RoomCharacter *other) {
        FloorPos pos = other->GetFloorPos();
        return func_02091c80(this, pos.x, pos.z);
    }
    BOOL IsAhead(const RoomCharacter *other, fx32 width) {
        FloorPos pos = other->GetFloorPos();
        return func_02091d44(this, pos.x, pos.z, width);
    }
    BOOL IsNearerThan(const RoomCharacter *other, fx32 distanceSq) {
        FloorPos pos;
        pos.x = other->mModel.mPos.x;
        pos.z = other->mModel.mPos.z;
        return func_02091edc(this, pos, distanceSq);
    }
    BOOL IsWithinReach(const RoomCharacter *other, fx32 reach, fx32 maxReach) {
        FloorPos pos = other->GetFloorPos();
        return Character_IsPointWithinReach(this, pos.x, pos.z, reach, maxReach);
    }

    /* 0x25E4 */ CharacterModel mModel;
    /* 0x26F8 */ u8 mUnknown26F8[0x168];
    /* 0x2860 */ PositionedNode mNodes[12]; // The model's joints (MtxFx43 each, group a5); node 0 is where the head icon is put
    /* 0x2AA0 */ u8 mUnknown2AA0[0x4C];
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;        // func_02091174 multiplies a distance by it
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to
    /* 0x2B04 */ int mUnknown2B04;
    /* 0x2B08 */ u16 mMoveAngle;
    /* 0x2B0A */ u16 mAngle;               // The way the character faces (Character_SetAngle sets both)
    /* 0x2B0C */ u16 mUnknown2B0C;
    /* 0x2B0E */ u16 mUnknown2B0E;
    /* 0x2B10 */ u8 mUnknown2B10[8];
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ int mOverrideState;       // A second state (func_ov074_02169890 sets it); mState does not run while it is set
    /* 0x2B24 */ int mSlot;                // 0 or 1: which of the scene's two characters this is
    /* 0x2B28 */ int mWait;                // Frames, counted down by Update: the scene chooses again at 0
    /* 0x2B2C */ BOOL mPresent;            // The character is in the room
    /* 0x2B30 */ RoomCharacter *mPartner;
    /* 0x2B34 */ VecFx32 mSavedTarget;
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ void *mHeldItems[2];
    /* 0x2B88 */ void *mUnknown2B88[2];
    /* 0x2B90 */ TtsPlayer *mTtsPlayer;
    /* 0x2B94 */ void *mSpeechText[5];     // By mMood: [0] is mSpeechTextDefault, the others mSpeechTextByMood
    /* 0x2BA8 */ void *mSpeechParam[5];    // By mMood: point into the saved character (0x84, 0xA6 + 0x12 * n)
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ BOOL mKeepSound;          // SetState leaves the sound playing once when this is set
    /* 0x2BC4 */ int mUnknownClearedByConstructor;
    /* 0x2BC8 */ wchar_t mNameText[11];    // The name as a string, for the scene's messages
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ u16 mUnknown2BE2;
    /* 0x2BE4 */ u16 mUnknown2BE4;         // Cleared by SetState
    /* 0x2BE6 */ u8 mUnknown2BE6[6];
    /* 0x2BEC */ u8 mSpeechTextDefault[0x88];
    /* 0x2C74 */ u8 mSpeechTextByMood[4][0x48];
    /* 0x2D94 */ u8 mUnknown2D94;
    /* 0x2D95 */ u8 mPrevEvent;            // mEvent before SetEvent
    /* 0x2D96 */ s8 mSubStep;
    /* 0x2D97 */ s8 mSubSubStep;
}; // 0x2D98

#endif // OV074_ROOMCHARACTER_H
