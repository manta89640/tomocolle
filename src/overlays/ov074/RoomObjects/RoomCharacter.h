#ifndef ROOMCHARACTER_H
#define ROOMCHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

#include "HeldItem.h"
#include "Model.h"

class ApartmentRoom;
class RoomCamera;

// A character standing in the apartment room: the room's resident, or the visitor (the class name is group a6's,
// see NOTES.md). This header is group a7's view of it: the layout is group a0's, with the fields and functions of
// 0x02168d24 to 0x02169d90 added. The functions of the other groups are declared as those groups name them.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// A point on the floor of the room: the x and z of a VecFx32. The main program's character functions take and
// return it by value (func_02091edc, func_02091abc), and all of them leave the dead stores of its copies behind.
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;

    FloorPos() {}
    FloorPos(fx32 x_, fx32 z_) {
        x = x_;
        z = z_;
    }
    FloorPos operator-(const FloorPos &other) const { return FloorPos(x - other.x, z - other.z); }
    FloorPos operator+(const FloorPos &other) const { return FloorPos(x + other.x, z + other.z); }
    fx32 LengthSq() const { return FX_Mul(x, x) + FX_Mul(z, z); }
};

// One saved character (a resident): the 0x2C0 bytes that the save data holds for each (func_0202c264 copies one
// out by index, func_0202c28c back in). Main-program class, not decompiled; func_020979c0 is its constructor. The
// scene makes temporary ones on the stack for the residents that are not in the room. Only what the overlay uses is
// named.
struct SavedCharacter {
    SavedCharacter(); // func_020979c0

    /* 0x0000 */ u16 mUnknown0000;
    /* 0x0002 */ wchar_t mName[10];      // func_0207a80c copies it out as a string
    /* 0x0016 */ u8 mUnknown0016[0x34];
    /* 0x004A */ u8 mUnknown004A;        // func_02098ec4 computes a value up to 200 from these two
    /* 0x004B */ u8 mUnknown004B;
    /* 0x004C */ u8 mUnknown004C[3];
    /* 0x004F */ u8 mRoom;               // Number of its room in the building (index of the room list's windows)
    /* 0x0050 */ u8 mUnknown0050[4];
    /* 0x0054 */ u32 mUnknown0054Bits0 : 4;
    /*        */ u32 mInterior : 7;      // Which interior its room has (the room model is told this)
    /* 0x0058 */ u8 mUnknown0058[0x2C];
    // What the character says, as text: RoomCharacter::ConvertSpeechTexts makes Shift JIS of them for the speech
    // synthesizer (RoomCharacter::Init points mSpeechParam[0..4] at them)
    /* 0x0084 */ wchar_t mSpeechDefault[17];
    /* 0x00A6 */ wchar_t mSpeechByMood[4][9];
    /* 0x00EE */ u8 mUnknown00EE[0xE];
    /* 0x00FC */ u8 mAffinity[100];      // For each other character by index: func_02099088 adds to it, 0 to 200
    /* 0x0160 */ u8 mRelation[0x20];     // For each other character by index: func_020990b0 sets it
    /* 0x0180 */ u8 mRelationKind[0x9C]; // For each other character by index: row of data_021135c8
    /* 0x021C */ u32 mEventEndLo;        // A time in seconds (64 bits, as the clock's); 0x7FFFFFFFFFFFFFFF when the
    /* 0x0220 */ u32 mEventEndHi;        // character has none
    /* 0x0224 */ u8 mEvent;              // What the character is doing in its room: 0..18 alone, 0x18.. with a partner
    /* 0x0225 */ u8 mEventPartner;       // mIndex of the character it is doing it with
    /* 0x0226 */ u8 mUnknown0226[0x12];
    /* 0x0238 */ u16 mMoodPartner;       // mIndex of another character, used when mMoodLocked is set
    /* 0x023A */ u8 mMoodLocked;         // func_02098fe0 and the scene leave mMood alone when this is set
    /* 0x023B */ u8 mUnknown023B[0x5A];
    /* 0x0295 */ u8 mMood;               // 0..4 (name is group a2's): index of the head marks and speech texts
    /* 0x0296 */ u8 mUnknown0296[0xE];
    /* 0x02A4 */ u16 mIndex;             // Which of the saved characters this is (the save data is asked by this)
    /* 0x02A6 */ s16 mReaction;          // What the character chose to do next (func_ov074_021686c0, func_0209d5ac)
    /* 0x02A8 */ u8 mUnknown02A8[0x14];
    /* 0x02BC */ s8 mPersonality;        // What func_02098f2c computes (0..15); name is group a6's
}; // 0x2C0

// The character as the main program draws it: the saved character and its model's resources. Main-program class,
// not decompiled; func_0208fcf8 is its constructor.
struct CharacterBase : public SavedCharacter {
    CharacterBase(); // func_0208fcf8

    /* 0x02C0 */ u8 mUnknown02C0[0x2240];
    /* 0x2500 */ int mFace;              // Character_SetFace sets it (name is group a2's)
    /* 0x2504 */ u8 mUnknown2504[0xC];
    /* 0x2510 */ BOOL mHeadTurnActive;   // The head is turning to the angles below
    /* 0x2514 */ u8 mUnknown2514[0xC0];
    /* 0x25D4 */ u16 mUnknown25D4;       // These three and the next three are cleared together when the model is made
    /* 0x25D6 */ u16 mUnknown25D6;       // An angle: added to the head mark's, given to the held item
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u16 mHeadPitch;         // Where the head turns to (names are group a2's)
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
}; // 0x25E4

// The model of a character with its animations. Main-program class (func_0208eb18 constructs it), not decompiled.
// Its destructor is as RoomCharacter's implicit one has it inlined (func_0203eee4).
class CharacterModel : public AnimModel {
public:
    CharacterModel(); // func_0208eb18
    ~CharacterModel() { NNS_G3dRenderObjResetCallBack(&mRenderObj); }

    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x8];
    /* 0x0F4 */ fx32 mScale;
    /* 0x0F8 */ u8 mUnknown0F8[0xC];
    /* 0x104 */ int mAnimId;      // The current 'BCA0' animation (names from here are group a6's)
    /* 0x108 */ int mPrevAnimId;
    /* 0x10C */ int mBlendFrames; // > 0 while the change to the current animation is still running
    /* 0x110 */ int mUnknown110;
}; // 0x114

// The icon shown over a character's head (name is group a6's). Main-program class (func_02081034 constructs it,
// func_020810e4 updates it), not decompiled.
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

// A node of the model with a position at 0x24 (name is group a6's): it is a MtxFx43, see mNodes.
struct PositionedNode {
    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ VecFx32 mPos;
}; // 0x30

class RoomCharacter : public CharacterBase {
public:
    // mOverrideState: what is done with the character's model while its own states do not run
    enum OverrideState {
        OVERRIDE_NONE,
        OVERRIDE_UNLOAD,       // Hide everything and unmount the character's archive
        OVERRIDE_LOAD_ARCHIVE, // Mount the archive of the saved character that is in the object now
        OVERRIDE_MAKE_MODEL,   // Make the model from the mounted archive
    };

    // --- Other groups' functions that these call ---
    void SetState(int state);                                                 // 0x02152370 (a1)
    void CopyName();                                                          // 0x0215243c (a1)
    void ShowHeadIcon();                                                      // 0x02152464 (a1)
    int StepChangeHeldItems(u8 itemA, u8 itemB);                              // 0x021684ac (a6)
    void ResetBody();                                                         // 0x021686dc (a6)
    void GetMidpoint(PositionedNode *a, PositionedNode *b, VecFx32 *out);     // 0x02168734 (a6)
    void SetAnimation(u32 resType, int id);                                   // 0x02168774 (a6)
    BOOL IsAnimOfGroupB(int anim);                                            // 0x02168804 (a6)
    BOOL IsMovingAnim();                                                      // 0x021688d4 (a6)
    BOOL IsAnimOfGroupA(int anim);                                            // 0x02168900 (a6)

    // --- 0x02168d24 to 0x02169d90 ---
    void GetWalkAreaCorner(FloorPos pos, FloorPos *corner);
    int GetWalkAreaSide(fx32 x, fx32 z);
    int SnapAngle(u16 angle);
    BOOL GetAngleAround(u16 *angle, FloorPos from, FloorPos to, int margin);
    void GetAngleAroundPartner(u16 *angle, int margin);
    u16 GetHeadYaw(u16 angle, int limit);
    void SetHeadTurn(u16 pitch, u16 yaw, u16 roll, u8 frames);
    u16 GetHeadYawTo(FloorPos pos, int limit);
    u16 GetHeadPitch(VecFx32 from, VecFx32 to, u16 limit);
    u16 GetHeadPitchToPartner(int node, u16 limit);
    BOOL StepWalkLooking(u32 lookLimit, int lookAngle, fx32 reach, int speed, int turnShift);
    BOOL StepTurnLooking(int unk1, int unk2, u32 lookLimit);
    void RestartStepSound();
    void SetHeldItemsShowState(int state);
    void SetHeldItemsVisible(BOOL visible);
    BOOL AreHeldItemsLoaded();
    void PlaceHeldItemBetweenHands(fx32 height);
    void PlaceHeldItemsInHands();
    void PlaceHeldItemAtHead();
    void PlaceHeldItemsAtStart();
    BOOL StepPutHeldItemsDown(int anim);
    void SetOverrideState(int state);
    void UpdateOverrideState();
    void UpdateVoiceOfSound();
    void StartSound3IfAnimOfGroup();
    void StartSound0UnlessGroupA();
    void StartSoundUnlessGroupB(int index);
    void StartSound0UnlessGroup();
    void RefreshSpeechTexts();
    void ConvertSpeechTexts();
    void ClearSpeechTexts();
    void Save();

    VecFx32 GetPos() const { return mModel.mPos; }
    FloorPos GetFloorPos() const { return FloorPos(mModel.mPos.x, mModel.mPos.z); }

    /* 0x25E4 */ CharacterModel mModel;
    /* 0x26F8 */ MtxFx33 mNodeRotations[10]; // The rotation and the matrix of the model's nodes (2 and 1 are the
    /* 0x2860 */ MtxFx43 mNodes[11];         // hands, 0 the head: the held items are put there)
    /* 0x2A70 */ u8 mUnknown2A70[0x3C];
    /* 0x2AAC */ char mArchiveName[4];     // Name its archive is mounted under (func_02091048 sets both)
    /* 0x2AB0 */ char mArchivePath[0x3C];  // and the archive's path
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;        // func_02091174 multiplies a distance by it
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to
    /* 0x2B04 */ int mUnknown2B04;
    /* 0x2B08 */ u16 mMoveAngle;
    /* 0x2B0A */ u16 mAngle;               // The way the character faces (Character_SetAngle sets both)
    /* 0x2B0C */ u16 mTargetAngle;
    /* 0x2B0E */ u16 mWalkAngle;           // The direction to mTarget, as func_02091194 and Character_TurnToWalkTarget find it
    /* 0x2B10 */ s8 mSoundPhase[2];        // Name is group a2's; Character_ResetSoundPhase clears one, func_02091fa0 both
    /* 0x2B12 */ u8 mUnknown2B12[6];
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ int mOverrideState;
    /* 0x2B24 */ int mSlot;                // 0 or 1: which of the scene's two characters this is; also its resource id
    /* 0x2B28 */ int mWait;
    /* 0x2B2C */ BOOL mPresent;            // The character is in the room
    /* 0x2B30 */ RoomCharacter *mPartner;
    /* 0x2B34 */ VecFx32 mSavedTarget;
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ HeldItem *mHeldItems[2];
    /* 0x2B88 */ ApartmentRoom *mRoom;
    /* 0x2B8C */ RoomCamera *mCamera;
    /* 0x2B90 */ TtsPlayer *mTtsPlayer;
    /* 0x2B94 */ void *mSpeechText[5];     // By mMood: [0] is mSpeechTextDefault, the others mSpeechTextByMood
    /* 0x2BA8 */ void *mSpeechParam[5];
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ BOOL mKeepSound;
    /* 0x2BC4 */ BOOL mSoundArmed;         // The Start...Sound functions only set it the first time they are called
    /* 0x2BC8 */ wchar_t mNameText[11];
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ s16 mOverrideStep;
    /* 0x2BE4 */ u16 mUnknown2BE4;
    /* 0x2BE6 */ u8 mUnknown2BE6[6];
    /* 0x2BEC */ char mSpeechTextDefault[0x88]; // The saved character's texts in Shift JIS
    /* 0x2C74 */ char mSpeechTextByMood[4][0x48];
    /* 0x2D94 */ u8 mUnknown2D94;
    /* 0x2D95 */ u8 mPrevEvent;
    /* 0x2D96 */ s8 mSubStep;
    /* 0x2D97 */ s8 mSubSubStep;
}; // 0x2D98

#endif // ROOMCHARACTER_H
