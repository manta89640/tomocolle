#ifndef OV074_ROOMCHARACTER_H
#define OV074_ROOMCHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

#include "Character.h"
#include "FloorPos.h"
#include "HeldItem.h"
#include "Model.h"
#include "Room.h"

class ApartmentRoom;
class RoomCamera;
class RoomCharacter;

// A joint's matrix (MtxFx43) seen as "something with a position at 0x24", which is all that
// RoomCharacter::GetMidpoint reads of it. It exists only because GetMidpoint's name in symbols.txt has it as its
// parameter type: RoomCharacter::GetNode makes one of a joint's matrix.
struct PositionedNode {
    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ VecFx32 mPos;
}; // 0x30

extern "C" {

// --- Main program, not named yet (or named in symbols.txt): functions of the character (0x0208fcf8 to
// 0x0209d898). All take the character as their object. ---

// Sets the face (expression) and restarts its animation
void Character_SetFace(RoomCharacter *self, int face);
// Lip sync: opens the mouth (face 7) while the phoneme is a vowel or 'w'
void func_02090a58(RoomCharacter *self, const char *phoneme, int face);
// Tests of where a point (x, z) is, seen from the character: in front of it, within that width
BOOL func_02091d44(RoomCharacter *self, fx32 x, fx32 z, fx32 width);
// The point is nearer to the character than the square root of that
BOOL func_02091edc(RoomCharacter *self, FloorPos pos, fx32 distanceSq);
// Clamps an angle to +-limit
BOOL func_02090c78(RoomCharacter *self, u16 *angle, int limit);
void func_020910d4(RoomCharacter *self);
// Turns towards mTarget; TRUE when facing it
BOOL Character_TurnToWalkTarget(RoomCharacter *self, int speed, int accel);
// Turns mAngle towards mTargetAngle
BOOL Character_TurnToTargetAngle(RoomCharacter *self, int speed, int accel);
// Moves by mSpeed along mMoveAngle
void Character_MoveAlongAngle(RoomCharacter *self);
// Sets mTarget
void Character_SetWalkTarget(RoomCharacter *self, VecFx32 target);
// Sets mAngle and mMoveAngle
void Character_SetAngle(RoomCharacter *self, u16 angle);
// Sets the walk target away from a point
void func_02091598(RoomCharacter *self, FloorPos pos);
// Turns towards a point
void func_0209171c(RoomCharacter *self, FloorPos pos);
void func_02091968(RoomCharacter *self);
// Starts an animation of the model, at the speed mAnimSpeed (RoomCharacter::SetAnimation calls it)
void func_020919e8(RoomCharacter *self, u32 resType, int id);
void func_02091a6c(RoomCharacter *self);
// Tests of where a point (x, z) is, seen from the character: within that width of the line the character faces
// along (func_02091d44: and in front of it)
BOOL func_02091b80(RoomCharacter *self, fx32 x, fx32 z, fx32 width);
BOOL func_02091c80(RoomCharacter *self, fx32 x, fx32 z);
// The point is nearer than the model's scale times the reach (and than maxReach, if that is positive)
BOOL Character_IsPointWithinReach(RoomCharacter *self, fx32 x, fx32 z, fx32 reach, fx32 maxReach);
// Restarts a slot's progress of Character_PlaySoundAtAnimPhase (mSoundPhase); func_02091fa0 does it for both slots
void Character_ResetSoundPhase(RoomCharacter *self, int slot);
void func_02091fa0(RoomCharacter *self);
// Starts sound `index` of sequence archive `seqArc` once per animation
int func_02091fbc(RoomCharacter *self, int player, int seqArc, int index);
// Starts sound `index` of sequence archive `seqArc` at given quarters (bit mask) of the current animation
int Character_PlaySoundAtAnimPhase(RoomCharacter *self, int slot, int player, int seqArc, int index, int quarters);
// A number below max from a generator seeded by the character (func_020985f8), not the game's generator
int func_02098e88(RoomCharacter *self, int max);
// A value up to 200 from the saved character's 0x4A and 0x4B
int func_02098ec4(RoomCharacter *self);
int func_0209d804(RoomCharacter *self, s8 value);
// Applies the character's voice to the speech engine
void func_0209d898(RoomCharacter *self, TtsPlayer *player);
int func_0207a6c0(RoomCharacter *self);

// --- Main program, not named yet: others ---
GXRgb func_02073514(int color);
BOOL func_02099e34(u8 value);

// --- Data ---
extern MATHRandContext16 gRandom; // The game's random number generator
extern const int data_020fba28[16];   // By mPersonality: the row of the two tables below
extern const s16 data_020fb870[4][5]; // Animation ids, by that row and by mMood: standing
extern const s16 data_020fb898[4][5]; // The same: walking

} // extern "C"

// The overlay's data that the character's functions read, declared and not yet defined here. Measured: the
// compiler puts a file's 4-aligned constants into one section sorted by size and addresses them by the section's
// address plus an offset. In the ROM, 0x0216d404 to 0x0216d484 is sixteen 8-byte objects (gRoomPoints' 13
// places, sButtonSequences, {0, -0x3666} at 0x0216d474 and data_ov074_0216d47c), the start of such a section; so
// "gRoomPoints" is most likely separate constants of that pool, which is why no one declaration suits every user:
// the state functions match only with it not const, StartPairEvent only with it const.
extern FloorPos gRoomPoints[];       // 0x0216d404: places in the room
extern const int gMoodFaces[5];      // data_ov074_0216d544: the face of each mood
extern const s16 gMoodHeadIcons[5];  // data_ov074_0216d484: the head icon of each mood
extern const FloorPos data_ov074_0216d47c;
extern const VecFx32 data_ov074_0216d490;
extern const VecFx32 data_ov074_0216d4a8;
extern const VecFx32 data_ov074_0216d4c0;
extern const VecFx32 data_ov074_0216d4cc;
extern const VecFx32 data_ov074_0216d4d8;
extern const FloorRect data_ov074_0216d4e4;
extern const FloorRect data_ov074_0216d534;

// The SDK's MATH_Rand16 on the game's generator, as a signed number: its results compare signed (bge), which the
// state functions' matches depend on. A u16 gives bcs.
static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

// A character standing in the apartment room: the room's resident, or the visitor. The scene has two (0x2D98 bytes
// each). It is a state machine of 0x118 states: Update dispatches on mState to the Update...States functions, each
// a switch over a range of states.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class RoomCharacter : public CharacterBase {
public:
    // mOverrideState: what is done with the character's model while its own states do not run
    enum OverrideState {
        OVERRIDE_NONE,
        OVERRIDE_UNLOAD,       // Hide everything and unmount the character's archive
        OVERRIDE_LOAD_ARCHIVE, // Mount the archive of the saved character that is in the object now
        OVERRIDE_MAKE_MODEL,   // Make the model from the mounted archive
    };

    // --- 0x02152228 to 0x02152690 (unit RoomEncounter) ---
    RoomCharacter();
    void Init(int slot, int param2, int param3);
    // Loads the head icon of the mood into the scene's particle system. The parameter is a pointer: the scene's
    // Init only matches when it is (with an int and a cast, the particle system's address is computed elsewhere)
    void LoadHeadIcon(ParticleSystem *particles);
    void PlaceHeadIcon();
    void SetState(int state);
    void SetStateWithPartner(int state, int partnerState);
    void CopyName();
    void SetEvent(u8 event);
    void ShowHeadIcon();
    void Update();

    // --- The states, 0x02152690 to 0x02167840 ---
    void UpdateTalkStates();       // 0x01 to 0x1C: the ones with a partner in the room, talking
    void UpdateSoloStates1DTo3F(); // What a character does alone
    void UpdateSoloStates40To67();
    void UpdateSoloStates68To6F();
    void UpdatePairStates70To92(); // What the two do together: slot 0 decides for both
    void UpdatePairStates93ToB2();
    void UpdateStatesB3ToC9();
    void UpdateStatesCAToE0();
    void UpdateStatesE1ToF1();
    void UpdateStatesF2To103();
    void UpdateStates104To105();
    void UpdateStates106To10B();
    void UpdateStates10CTo112();
    void UpdateStates113To116();
    void UpdateState117();

    // --- What the states are built from, 0x02167840 to 0x02168d24 (unit RoomCharacterStatesF2). The Step...
    // functions do one frame's step, by mSubStep, and return TRUE when done. ---
    int StepWalkToRandomPos();
    int StepWalkTo(FloorPos pos);
    int StepTurnWithAnimation(int anim);
    int StepAlternateAnimations(int animA, int animB);
    int StepAnimationFacingPartner(int anim);
    int StepIdleThenAnimation55();
    int StepRandomAnimationFacingPartner();
    int StepAnimationThenFacePartner(int anim);
    void StartAnimationWithIcon(int anim);
    int StepChangeItemsThenAnimation(int anim, int itemA, int itemB);
    int StepChangeHeldItems(int itemA, int itemB);
    void UpdateBodyIfPresent();
    int PickReaction();
    void ResetBody();
    u8 GetFlagOfPartner();
    int GetFaceOfMood(int variant);
    int GetFace();
    void GetMidpoint(PositionedNode *a, PositionedNode *b, VecFx32 *out);
    void SetAnimation(u32 resType, int id);
    BOOL IsOneTwoOrFour(int value);
    BOOL IsState8Or9();
    BOOL IsState10To12();
    BOOL IsAnimOfGroupB(int anim);
    BOOL IsWalkAnim();
    BOOL IsMovingAnim();
    BOOL IsAnimOfGroupA(int anim);
    void GetRandomPosInArea(FloorPos *out);
    void GetRandomPos(FloorPos *out);
    BOOL ClampToRect(FloorPos *pos, const FloorRect *rect);
    BOOL ClampToWalkArea(FloorPos *pos);
    BOOL ClampToRect4E4(FloorPos *pos);
    BOOL ClampToRect534(FloorPos *pos);
    void GetRandomCorner(FloorPos *out);
    void GetRandomEdgePos(FloorPos *out);

    // --- 0x02168d24 to 0x02169dcc (unit RoomObjects): the floor area, head turns, walking steps, held items,
    // loading, sounds and speech texts ---
    // The corner of the walk area on the side of it that the point is on
    void GetWalkAreaCorner(FloorPos pos, FloorPos *corner);
    int GetWalkAreaSide(fx32 x, fx32 z);
    // The nearest multiple of 0x4000
    u16 SnapAngle(u16 angle);
    // An angle to go around a point by; FALSE (and the direction) when there is no need
    BOOL GetAngleAround(u16 *angle, FloorPos from, FloorPos to, int margin);
    BOOL GetAngleAroundPartner(u16 *angle, int margin);
    // angle + mAngle, limited by func_02090c78
    u16 GetHeadYaw(u16 angle, int limit);
    // Sets mHeadPitch, mHeadYaw, mHeadRoll, mHeadTurnFrames and mHeadTurnActive
    void SetHeadTurn(u16 pitch, u16 yaw, u16 roll, u8 frames);
    // The head's yaw towards a point, limited
    u16 GetHeadYawTo(FloorPos pos, int limit);
    // The head's pitch from a point to a point, limited
    u16 GetHeadPitch(VecFx32 from, VecFx32 to, u16 limit);
    // From the head (joint 0) to one of the partner's joints, limited
    u16 GetHeadPitchToPartner(int joint, u16 limit);
    // One step of walking to mTarget, with the step sound (func_02091fbc(1, 0, 0x1a)); TRUE when there. Turns the
    // head by lookAngle less the walking direction when lookLimit is below 0x4000
    BOOL StepWalkLooking(u32 lookLimit, int lookAngle, fx32 reach, int speed, int turnShift);
    // The same with Character_TurnToWalkTarget
    BOOL StepTurnLooking(int speed, int accel, u32 lookLimit);
    // Character_ResetSoundPhase(this, 0) unless IsMovingAnim()
    void RestartStepSound();
    // HeldItem::SetShowState for both held items
    void SetHeldItemsShowState(int state);
    void SetHeldItemsVisible(BOOL visible);
    BOOL AreHeldItemsLoaded();
    // Held item 0 at the midpoint of joints 2 and 1, raised by height times the scale
    void PlaceHeldItemBetweenHands(fx32 height);
    // Held item 1 in the hand (joint 2) and item 0 at joint 1
    void PlaceHeldItemsInHands();
    // Held item 0 at joint 0, turned by the head's yaw
    void PlaceHeldItemAtHead();
    // Held item 1 at (0, 0, 0x3000), both turned by 0x8000
    void PlaceHeldItemsAtStart();
    // By mSubStep: ShowHeadIcon, StepChangeHeldItems, the animation
    BOOL StepPutHeldItemsDown(int anim);
    // Sets mOverrideState and clears its step
    void SetOverrideState(int state);
    void UpdateOverrideState();
    // Sets volume and pitch of mSoundHandle's sound by the character (func_02098ec4)
    void UpdateVoiceOfSound();
    // Sound 3 of archive 0x1e when the animation is of group A or B
    void StartSound3IfAnimOfGroup();
    // Sound 0 of archive 0x1e unless the animation is of group A
    void StartSound0UnlessGroupA();
    // Sound `index` of archive 0x1e unless the animation is of group B
    void StartSoundUnlessGroupB(int index);
    void StartSound0UnlessGroup();
    void RefreshSpeechTexts();
    void ConvertSpeechTexts();
    void ClearSpeechTexts();
    // func_0202c28c(gGameData, mIndex, this) if func_0202c3dc
    void Save();

    // --- Inline helpers. Each is here because the match of some state function depends on its shape: do not
    // "simplify" one without checking the functions that use it. ---

    // Where the character stands, on the floor. Filling a local and returning it is what leaves the dead copies of
    // the point on the callers' stacks.
    FloorPos GetFloorPos() const {
        FloorPos pos;
        pos.x = mModel.mPos.x;
        pos.z = mModel.mPos.z;
        return pos;
    }
    VecFx32 GetPos() const { return mModel.mPos; }
    void SetPos(VecFx32 pos) { mModel.mPos = pos; }
    // The angle of the way from where the character stands to a point
    u16 AngleTo(FloorPos target) {
        FloorPos delta = target - GetFloorPos();
        return FX_Atan2Idx(delta.z, delta.x);
    }
    BOOL IsWithinReach(FloorPos pos, fx32 reach, fx32 maxReach) {
        return Character_IsPointWithinReach(this, pos.x, pos.z, reach, maxReach);
    }
    void SetFloorPos(fx32 x, fx32 z) {
        mModel.mPos.x = x;
        mModel.mPos.z = z;
    }
    // From where the character stands to a point
    FloorPos GetOffsetTo(const FloorPos &to) { return to - GetFloorPos(); }
    BOOL ClampOwnPosToRect4E4() {
        FloorPos pos = GetFloorPos();
        return ClampToRect4E4(&pos);
    }

    // The main program's tests of where another character is, seen from this one. Each takes the other's place
    // into a local first: the scene's encounter functions match only so
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

    // Where it walks to
    VecFx32 GetTarget() { return mTarget; }
    FloorPos GetTargetFloorPos() {
        FloorPos pos;
        pos.x = mTarget.x;
        pos.z = mTarget.z;
        return pos;
    }
    void SetTarget(fx32 x, fx32 y, fx32 z) {
        mTarget.x = x;
        mTarget.y = y;
        mTarget.z = z;
    }
    // Two shapes of setting the target on the floor, both needed: SetTargetXZ leaves y as it is, SetTargetOnFloor
    // clears it
    void SetTargetXZ(fx32 x, fx32 z) {
        mTarget.x = x;
        mTarget.z = z;
    }
    void SetTargetOnFloor(fx32 x, fx32 z) {
        mTarget.x = x;
        mTarget.y = 0;
        mTarget.z = z;
    }
    void SetTargetFloorPos(FloorPos pos) {
        mTarget.x = pos.x;
        mTarget.z = pos.z;
    }
    void SetSavedTarget(VecFx32 target) {
        mSavedTarget.x = target.x;
        mSavedTarget.y = target.y;
        mSavedTarget.z = target.z;
    }

    // The standing and the walking animation of the character's personality and mood
    s16 GetIdleAnim() { return data_020fb870[data_020fba28[mPersonality]][mMood]; }
    s16 GetWalkAnim() { return data_020fb898[data_020fba28[mPersonality]][mMood]; }
    ModelAnimation *GetCurrentAnimation() { return AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId); }
    BOOL IsAnimationFinished() { return GetCurrentAnimation()->mFinished; }
    // A joint's matrix, as GetMidpoint takes it
    PositionedNode *GetNode(int joint) { return (PositionedNode *)&mModel.mJointMatrices[joint]; }

    // GetFlagOfPartner (0x021686f4) as the states have it inline
    u8 GetFlagOfPartnerInline() { return mRelation[mPartner->mIndex]; }

    // UpdateEvent's match depends on the character being the object of this one
    void SetHeadIconMode(int mode) { mHeadIcon.SetMode(mode); }
    // Shows the head icon of an id and sets its mode to 3
    void SetHeadIcon(s16 id) {
        mHeadIcon.Show();
        mHeadIcon.mIconId = id;
        mHeadIcon.Reload();
        mHeadIcon.SetMode(3);
    }
    // Shows the head icon of an id, from its start when that one is already there
    void ShowHeadIconOfId(int id) {
        if (mHeadIcon.mIconId != id) {
            mHeadIcon.Show();
            mHeadIcon.mIconId = id;
            mHeadIcon.Reload();
        } else {
            mHeadIcon.Restart();
        }
    }
    // Icon 9, or icon 3 for the characters whose record has mUnknown0246 set
    void ShowHeadIconOfRecord() {
        if (mUnknown0246 == 0) {
            ShowHeadIconOfId(9);
        } else {
            ShowHeadIconOfId(3);
        }
    }
    // Plays one of the four sounds 10 to 13 of sequence archive 0x22, picked by the character's own generator
    void PlayRandomSoundOfFour() {
        int pick = func_02098e88(this, 4);
        if (pick == 0) {
            gSoundManager.StartSeqArc(1, 0x22, 10);
        } else if (pick == 1) {
            gSoundManager.StartSeqArc(1, 0x22, 11);
        } else if (pick == 2) {
            gSoundManager.StartSeqArc(1, 0x22, 12);
        } else {
            gSoundManager.StartSeqArc(1, 0x22, 13);
        }
    }
    // Turns the head to the camera, up or down by a quarter of the camera's elevation (defined below)
    void LookAtCamera(u8 frames);

    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AA0 */ u8 mUnknown2AA0[0xC];
    /* 0x2AAC */ char mArchiveName[4];     // Name its archive is mounted under (func_02091048 sets both)
    /* 0x2AB0 */ char mArchivePath[0x3C];  // and the archive's path
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknown2AF0;        // func_02091174 multiplies a distance by it
    /* 0x2AF4 */ fx32 mUnknown2AF4;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to (Character_SetWalkTarget)
    /* 0x2B04 */ int mUnknown2B04;
    /* 0x2B08 */ u16 mMoveAngle;           // The way Character_MoveAlongAngle moves it
    /* 0x2B0A */ u16 mAngle;               // The way the character faces (Character_SetAngle sets both)
    /* 0x2B0C */ u16 mTargetAngle;         // Character_TurnToTargetAngle turns mAngle towards it
    /* 0x2B0E */ u16 mWalkAngle;           // The direction to mTarget, as func_02091194 and Character_TurnToWalkTarget
                                           // find it; its negative is the head's yaw while turning
    /* 0x2B10 */ s8 mSoundPhase[2];        // Character_PlaySoundAtAnimPhase's progress through an animation, per slot
    /* 0x2B12 */ u8 mUnknown2B12[2];
    /* 0x2B14 */ fx32 mAnimSpeed;          // func_020919e8 gives it to the model when an animation starts
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;           // SetState keeps the state before
    /* 0x2B20 */ int mOverrideState;       // A second state (SetOverrideState sets it); mState does not run while it is set
    /* 0x2B24 */ int mSlot;                // 0 or 1: which of the scene's two characters this is; also its resource id.
                                           // In the states for two, slot 0 decides for both
    /* 0x2B28 */ int mWait;                // Frames, counted down by Update: the scene chooses again at 0
    /* 0x2B2C */ BOOL mPresent;            // The character is in the room
    /* 0x2B30 */ RoomCharacter *mPartner;  // The other of the two
    /* 0x2B34 */ VecFx32 mSavedTarget;
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ HeldItem *mHeldItems[2];  // What it holds in its hands
    /* 0x2B88 */ ApartmentRoom *mRoom;     // The states of leaving and entering move its door
    /* 0x2B8C */ RoomCamera *mCamera;      // The character turns to it and looks into it
    /* 0x2B90 */ TtsPlayer *mTtsPlayer;
    /* 0x2B94 */ char *mSpeechText[5];     // By mMood: [0] is mSpeechTextDefault, the others mSpeechTextByMood
    /* 0x2BA8 */ void *mSpeechParam[5];    // By mMood: point into the saved character (0x84, 0xA6 + 0x12 * n);
                                           // speech needs a non-null one
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ BOOL mKeepSound;          // SetState stops the sound of mSoundHandle unless this is set, and clears it
    /* 0x2BC4 */ BOOL mSoundArmed;         // The Start...Sound functions only set it the first time they are called
    /* 0x2BC8 */ wchar_t mNameText[11];    // The name as a string, for the scene's messages
    /* 0x2BDE */ s16 mStep;                // The step inside a state: SetState clears it
    /* 0x2BE0 */ s16 mTimer;               // Frames, counted down
    /* 0x2BE2 */ s16 mOverrideStep;
    /* 0x2BE4 */ u16 mCounter;             // Cleared by SetState. By state: animation loops or repeats counted, whose
                                           // turn it is, or the state chosen to enter next
    /* 0x2BE6 */ s16 mTimer2;              // More countdowns; in the states 0x76 to 0x7a these two hold the two
    /* 0x2BE8 */ s16 mTimer3;              // animation ids that alternate
    /* 0x2BEA */ s16 mTimer4;
    /* 0x2BEC */ char mSpeechTextDefault[0x88]; // The saved character's texts in Shift JIS
    /* 0x2C74 */ char mSpeechTextByMood[4][0x48];
    /* 0x2D94 */ u8 mUnknown2D94;
    /* 0x2D95 */ u8 mPrevEvent;            // mEvent before SetEvent
    /* 0x2D96 */ s8 mSubStep;              // Step of the Step... functions that the states call
    /* 0x2D97 */ s8 mSubSubStep;
}; // 0x2D98

// At the end: RoomCamera.h needs RoomCharacter whole, and LookAtCamera needs RoomCamera whole.
#include "RoomCamera.h"

inline void RoomCharacter::LookAtCamera(u8 frames) {
    FloorPos pos;
    pos.x = mCamera->mPosition.x;
    pos.z = mCamera->mPosition.z;
    u16 angle = mCamera->mOrbit.x >> 12;
    SetHeadTurn(-(angle >> 2), GetHeadYawTo(pos, 0x3800), 0, frames);
}

#endif // OV074_ROOMCHARACTER_H
