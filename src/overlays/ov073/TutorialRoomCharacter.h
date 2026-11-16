#ifndef OV073_TUTORIALROOMCHARACTER_H
#define OV073_TUTORIALROOMCHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "character/Character.h"
#include "HeadIcon.h"
#include "sound/SoundManager.h"

#include "Model.h"
#include "Room.h"

class TutorialRoomCamera;
class TutorialRoomCharacter;

extern "C" {

// --- Main program: functions of the character (0x0208fcf8 to 0x0209d898). All take the character as their
// object. The declarations are overlay 74's. ---


// --- Data ---
extern const int gPersonalityKinds[16];   // By mPersonality: the row of the two tables below
extern const s16 data_020fb870[4][5]; // Animation ids, by that row and by mMood: standing
extern const s16 data_020fb898[4][5]; // The same: walking

} // extern "C"

// The character of the tutorial's apartment room: overlay 74's `RoomCharacter` as the tutorial has it. The scene
// has two (0x2B94 bytes each; overlay 74's are 0x2D98: the scene's constructor, main func_0203efc0, builds them with
// __cxa_vec_ctor(this + 0x18, 2, 0x2B94, ...)). It is the same class up to mPrevState and has the same helper
// functions (many byte for byte: SetAnimation, IsWalkAnim, GetHeadYawTo, ...), but only eighteen states, and no
// held items, no speech, no slot number, no second ("override") state and no sub-steps. The members after
// mPrevState are therefore at other offsets than overlay 74's; their names are overlay 74's.
//
// The name is descriptive, not the original: no name of this class survives in the ROM. The prefix tells it from
// overlay 74's: the program is linked as one, so the two cannot have one name.
class TutorialRoomCharacter : public Character {
public:
    TutorialRoomCharacter();
    void SetState(int state);
    void Update();
    void UpdateStates01To04(); // Standing, and walking to the walk target
    void UpdateStates05To08(); // Walking up to the partner, turning, and two animations with a sound
    void UpdateState09();      // Becoming friends
    void UpdateStates0ATo0F(); // Coming in by the door (overlay 74's states 0x10C to 0x111)
    void UpdateStates10To12(); // Looking at the camera, turning to it and coming up to it (0x113 to 0x115)
    void Draw();
    void CopyName();

    // What the states are built from
    u16 GetHeadYaw(u16 angle, int limit);
    void SetHeadTurn(u16 pitch, u16 yaw, u16 roll, u8 frames);
    u16 GetHeadYawTo(FloorPos pos, int limit);
    u16 GetHeadPitch(VecFx32 from, VecFx32 to, u16 limit);
    u16 GetHeadPitchToPartner(int joint, u16 limit);
    BOOL StepWalkLooking(u32 lookLimit, int lookAngle, fx32 reach, int speed, int turnShift);
    BOOL StepTurnLooking(int speed, int accel, u32 lookLimit);
    void SetAnimation(u32 resType, int id);
    void GetRandomPosInArea(FloorPos *out);
    void LoadHeadIcon(ParticleSystem *particles);
    void PlaceHeadIcon();
    void ShowHeadIcon();
    BOOL IsWalkAnim();
    BOOL IsAnimOfGroupB(int anim);
    BOOL IsAnimOfGroupA(int anim);
    void StartSound3IfAnimOfGroup();
    void StartSound0UnlessGroupA();

    // Where the character stands, on the floor. Filling a local and returning it is what leaves the dead copies of
    // the point on the callers' stacks.
    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mPos.x;
        pos.z = mPos.z;
        return pos;
    }
    VecFx32 GetPos() const { return mPos; }
    void SetPos(VecFx32 pos) { mPos = pos; }
    u8 GetRelationToPartner() { return mRelationRank[mPartner->mIndex]; }
    BOOL IsWithinReach(TutorialRoomCharacter *other, fx32 reach, fx32 maxReach) {
        FloorPos pos = other->GetFloorPos();
        return ((Character *)(this))->IsPointWithinReach(pos.x, pos.z, reach, maxReach);
    }
    void SetWalkTarget(fx32 x, fx32 y, fx32 z) {
        mTarget.x = x;
        mTarget.y = y;
        mTarget.z = z;
    }
    void SetWalkTargetOnFloor(FloorPos pos) {
        mTarget.x = pos.x;
        mTarget.z = pos.z;
    }
    s16 GetIdleAnim() { return data_020fb870[gPersonalityKinds[mPersonality]][mMood]; }
    s16 GetWalkAnim() { return data_020fb898[gPersonalityKinds[mPersonality]][mMood]; }
    // Overlay 74 has this as a function of its own (func_ov074_021694e8)
    void ResetSoundPhaseUnlessWalking() {
        if (!IsWalkAnim()) {
            ((Character *)(this))->ResetSoundPhase(0);
        }
    }
    // Defined in TutorialRoomCamera.h, which needs this class whole
    inline u16 GetHeadYawToCamera();
    inline void TurnHeadToCamera(u8 frames);
    void SetHeadIcon(s16 id) {
        mHeadIcon.Show();
        mHeadIcon.mIconId = id;
        mHeadIcon.Reload();
        mHeadIcon.SetMode(3);
    }

    /* 0x25E4 */ // Character: CharacterModel, then its own fields
    /* 0x2B18 */ int mState;               // 0 when it has finished what SetState told it to do
    /* 0x2B1C */ int mPrevState;           // SetState keeps the state before
    // --- From here on the tutorial's own layout ---
    /* 0x2B20 */ BOOL mPresent;            // The character is in the room: Update and Draw do nothing without it
    /* 0x2B24 */ TutorialRoomCharacter *mPartner; // The other of the two
    /* 0x2B28 */ TutorialApartmentRoom *mApartmentRoom; // The states of entering move its door. Not "mRoom": the
                                           // saved character has its room number under that name
    /* 0x2B2C */ TutorialRoomCamera *mCamera; // The character turns to it and looks into it
    /* 0x2B30 */ RoomHeadIcon mHeadIcon;
    /* 0x2B70 */ int mWait;                // Frames, counted down by Update: the scene chooses what to do next at 0
    /* 0x2B74 */ BOOL mSoundArmed;         // The Start...Sound functions only set it the first time they are called
    /* 0x2B78 */ s16 mStep;                // The step inside a state: SetState clears it
    /* 0x2B7A */ s16 mTimer;               // Frames, counted down
    /* 0x2B7C */ wchar_t mNameText[11];    // The name as a string, for the scene's messages
}; // 0x2B94

#endif // OV073_TUTORIALROOMCHARACTER_H
