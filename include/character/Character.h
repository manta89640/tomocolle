#ifndef CHARACTER_CHARACTER_H
#define CHARACTER_CHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "character/CharacterModel.h"
#include "character/FloorPos.h"

// The main program's character classes (src/character/), from the record that the save data holds to the character
// that a scene draws:
//
//   CharacterRecord   0x2A4 bytes  what the save data holds of a character (func_0202c264 copies one out)
//   SavedCharacter    0x2C0        + its index and what the game derives from the record
//   CharacterBase     0x25E4       + the Mii's face, drawn with the Mii library (NFL) at the head
//   Character         0x2B18       + the body (CharacterModel, a second base), walking, turning, animations, sounds
//
// The scenes' own characters (SceneCharacter, RoomCharacter, ...) derive from Character. Its constructor and
// destructor are implicit: the scenes' constructors call the base constructors of CharacterBase and of
// CharacterModel (C2, 0x0208eb18: a model that is a member is built with C1, 0x0208eba0, as the G3D test scene's),
// and their destructors CharacterModel's inline destructor and CharacterBase's D2. The names are the project's own.

// What the save data holds of a character: the 0x2A4 bytes that func_0202c264 copies out. Overlay 109's implicit
// copy assignment of it shows its members.
struct CharacterRecord {
    /* 0x000 */ u8 mMii[0x4A];           // The Mii's data (0x4A bytes, lib/MiiData)
    /* 0x04A */ u8 mUnknown04A[5];       // The Mii maker's personality page sets them: 1 to 8, 0 while unset
    /* 0x04F */ u8 mRoomNumber;          // Number of its room in the building
    /* 0x050 */ u32 mUnknown050Bit0 : 1;
    /*       */ u32 mUnknown050Bits1 : 16;
    /*       */ u32 mBirthYear : 12;     // 0xFFF for none
    /*       */ u32 mBloodType : 3;
    /* 0x054 */ u32 mUnknown054Bits0 : 4;
    /*       */ u32 mInterior : 7;       // Which interior its room has
    /*       */ u32 mClothesId : 9;       // The clothes it wears (row of the clothes table)
    /*       */ u32 mUnknown054Bits20 : 12;
    /* 0x058 */ wchar_t mName[11];
    /* 0x06E */ wchar_t mFamilyName[11];
    /* 0x084 */ wchar_t mSpeechDefault[17];
    /* 0x0A6 */ wchar_t mSpeechByMood[4][9];
    /* 0x0EE */ s16 mVoice[6];           // The voice: five parameters of the speech synthesizer and a choice of four
    /* 0x0FA */ u8 mUnknown0FA;
    /* 0x0FB */ u8 mUnknown0FB;
    /* 0x0FC */ u8 mAffinity[100];       // For each other character by index
    /* 0x160 */ u8 mRelation[100];       // For each other character by index
    /* 0x1C4 */ u32 mUnknown1C4;
    /* 0x1C8 */ struct { u32 m[8]; } mUnknown1C8;
    /* 0x1E8 */ struct { u32 m[10]; } mUnknown1E8;
    /* 0x210 */ u32 mUnknown210[3];      // Flags, a bit each
    /* 0x21C */ s64 mEventEnd;           // A time in seconds, as the clock's
    /* 0x224 */ u8 mEvent;               // What the character is doing: 0..18 alone, 0x17.. with a partner
    /* 0x225 */ u8 mEventPartner;        // mIndex of the character it is doing it with
    /* 0x226 */ u8 mUnknown226;
    /* 0x227 */ u8 mUnknown227;
    /* 0x228 */ s64 mUnknown228;
    /* 0x230 */ s64 mAwayUntil;
    /* 0x238 */ u16 mMoodPartner;
    /* 0x23A */ u8 mMoodLocked;
    /* 0x23B */ u8 mUnknown23B;
    /* 0x23C */ s64 mUnknown23C;
    /* 0x244 */ u8 mUnknown244;
    /* 0x245 */ u8 mUnknown245;
    /* 0x246 */ u8 mUnknown246;
    /* 0x247 */ u8 mUnknown247;
    /* 0x248 */ u8 mUnknown248;
    /* 0x249 */ u8 mUnknown249;
    /* 0x24A */ u8 mUnknown24A;
    /* 0x24B */ u8 mUnknown24B;
    /* 0x24C */ u8 mUnknown24C;
    /* 0x24D */ u8 mUnknown24D;
    /* 0x24E */ u8 mUnknown24E;
    /* 0x24F */ u8 mUnknown24F;
    /* 0x250 */ u16 mUnknown250[32];
    /* 0x290 */ u8 mUnknown290;
    /* 0x291 */ u8 mUnknown291;
    /* 0x292 */ u8 mUnknown292[2];
    /* 0x294 */ u8 mUnknown294;
    /* 0x295 */ u8 mMood;                // 0..4
    /* 0x296 */ u8 mUnknown296;
    /* 0x297 */ u8 mUnknown297;
    /* 0x298 */ u8 mRelationToPlayer;
    /* 0x299 */ u8 mUnknown299;
    /* 0x29A */ u8 mSongSlots[6];        // Which of the saved songs each of the character's six is
    /* 0x2A0 */ u8 mUnknown2A0;
    /* 0x2A1 */ u8 mUnknown2A1;
    /* 0x2A2 */ u8 mUnknown2A2;
    /* 0x2A3 */ u8 mUnknown2A3;
}; // 0x2A4

// A saved character and what the game keeps beside it.
class SavedCharacter : public CharacterRecord {
public:
    SavedCharacter();

    void SetParameter(int param, int value);
    int GetParameter(int param);
    GXRgb GetFaceColor();
    int GetAnswerScore01();

    /* 0x2A4 */ u16 mIndex;              // Which of the saved characters this is
    /* 0x2A6 */ s16 mReaction;
    /* 0x2A8 */ u8 mUnknown2A8[0x14];
    /* 0x2BC */ s8 mPersonality;         // 0..15
    /* 0x2BD */ u8 mUnknown2BD[3];
}; // 0x2C0

// What the Mii library's NFL_SetFaceDrawParam takes (lib/NFL's NFLDrawParam, which is C): the polygon attributes and
// material colours of a face.
struct FaceDrawParam {
    /* 0x00 */ int light;
    /* 0x04 */ GXPolygonMode polyMode;
    /* 0x08 */ int polygonID;
    /* 0x0C */ int alpha;
    /* 0x10 */ int misc;
    /* 0x14 */ GXRgb diffuse;
    /* 0x16 */ GXRgb ambient;
    /* 0x18 */ GXRgb specular;
    /* 0x1A */ GXRgb emission;
}; // 0x1C

// The character with its Mii face, which the Mii library (NFL) draws at the head's matrices.
class CharacterBase : public SavedCharacter {
public:
    // mFaceKind: which way the face moves by itself (UpdateFace)
    enum FaceKind {
        FACE_NONE,
        // 1 to 7: show expression 0 to 6 once
        FACE_BLINK = 8,          // 8, 9 and 13: blink, with eyes open in expression 0, 1 or 4
        FACE_LIP_SYNC_OPEN = 7,  // LipSync shows it while the mouth is open
    };

    CharacterBase();
    ~CharacterBase();

    void InitFace(u32 defaultMii);
    void ResetFace();
    void AddFace();
    void SetMiiParameter(int param, int value, BOOL makeFace);
    void MakeFace();
    void SetExpression(int expression);
    void SetDefaultMii(u32 index, BOOL makeFace);
    void SetRandomMii(int sex, int unk1, int unk2, BOOL makeFace);
    void SetHeadMatrices(const MtxFx43 *matrix, const MtxFx33 *rotation);
    BOOL HitTestFace();
    BOOL HitTestFace(const VecFx32 *origin, const VecFx32 *dir);
    BOOL HitTestFace(VecFx32 origin, VecFx32 dir) { return HitTestFace(&origin, &dir); }
    void DrawFace();
    void Draw();
    void SetFace(int kind);
    void UpdateBlink(int expression);
    void UpdateBlinkOrWink(int expression, fx32 closedTime);
    void UpdateRandomExpressions(int expression0, int expression1, int expression2, int expression3, fx32 time1,
                                 fx32 time2, int chance);
    void UpdateFace();
    void LipSync(const char *phoneme, int face);
    void UpdateHeadTurn();
    BOOL ClampHeadAngle(u16 *angle, int limit);
    static u16 StepAngle(u16 angle, u16 target, int shift);
    static fx32 GetAnimSpeedOf(int liveliness);

    /* 0x02C0 */ FaceDrawParam mFaceDrawParam;
    /* 0x02DC */ u8 mFace[0x2218];         // The library's face object (NFL_AddFace)
    /* 0x24F4 */ int mExpression;          // Given to NFL_SetExpression
    /* 0x24F8 */ int mFaceId;              // The face's id in the library, -1 for none
    /* 0x24FC */ BOOL mFaceVisible;
    /* 0x2500 */ int mFaceKind;            // How the face moves by itself (UpdateFace)
    /* 0x2504 */ fx32 mFaceTimerStep;      // What the face timer advances by in a frame
    /* 0x2508 */ fx32 mFaceTimer;
    /* 0x250C */ s16 mFaceStep;            // The step of the face's movement
    /* 0x2510 */ int mHeadTurn;            // Which of the head's angles turn (1: all three, 2, 3, 4: one), 0 none
    /* 0x2514 */ MtxFx33 mHeadRotation;    // The head joint's matrices, as the body last drew it
    /* 0x2538 */ MtxFx43 mHeadMatrix;
    /* 0x2568 */ VecFx32 mFaceOffset;
    /* 0x2574 */ VecFx32 mFaceScale;
    /* 0x2580 */ MtxFx33 mFaceRotation;
    /* 0x25A4 */ MtxFx43 mFaceMatrix;      // Where the face was drawn: the clip matrix
    /* 0x25D4 */ u16 mHeadPitchNow;        // Where the head is turned now
    /* 0x25D6 */ u16 mHeadYawNow;
    /* 0x25D8 */ u16 mHeadRollNow;
    /* 0x25DA */ u16 mHeadPitch;           // Where it turns to
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u16 mFaceRoll;            // Added to the face's roll
    /* 0x25E2 */ u8 mHeadTurnShift;        // The head turns by the distance shifted right by this, at least 0x20
    /* 0x25E3 */ u8 mUnknown25E3;
}; // 0x25E4

// A Mii head without a body: the face on a head model of its own (overlays 2, 77 and 79).
class CharacterHead : public CharacterBase {
public:
    // Implicit constructor; the destructor as the scenes' copies of it have it inlined (0x0203E064, 0x02040DC0)
    ~CharacterHead() { NNS_G3dRenderObjResetCallBack(&mHead.mRenderObj); }

    void Init(int resourceId, u32 defaultMii);
    void Update();
    void Draw();
    void SetAnimSpeed(fx32 speed);
    void SetAnimation(u32 resType, int id);
    void UpdateBlend();
    static void OnNode(NNSG3dRS *rs);
    static void InitCallbacks(NNSG3dRS *rs);

    /* 0x25E4 */ AnimModel mHead;
    /* 0x26C4 */ int mAnimId;              // The current 'BCA0' animation
    /* 0x26C8 */ int mPrevAnimId;
    /* 0x26CC */ fx32 mBlendFrames;
    /* 0x26D0 */ fx32 mAnimSpeed;
    /* 0x26D4 */ MtxFx33 mRootRotation;    // The matrices of the head model's node 0
    /* 0x26F8 */ MtxFx43 mRootMatrix;
}; // 0x2728

// The character with its body.
class Character : public CharacterBase, public CharacterModel {
public:
    void Init(int resourceId, int unk, u32 defaultMii);
    void SetArchive(const char *archiveName);
    void UpdateClothes();
    void Update();
    void UpdateSpeedScales();
    void UpdateWalkSpeedScale();
    void UpdateTurnSpeedScale();
    fx32 ScaleBySpeed(fx32 value);
    BOOL WalkToTarget(fx32 reach, fx32 speed, int turnShift);
    BOOL TurnToWalkTarget(int turnSpeed, int walkSpeed);
    BOOL TurnToTargetAngle(int turnSpeed, int walkSpeed);
    BOOL TurnTo(int turnSpeed, int walkSpeed, u16 angle);
    void MoveAlongAngle();
    void SetWalkTarget(VecFx32 target);
    void SetAngle(u16 angle);
    void Turn(u16 delta);
    void SetWalkTargetAwayFrom(FloorPos pos);
    void SetWalkTargetReflected(FloorPos pos);
    void Draw();
    void SetAnimSpeed(fx32 speed);
    void SetAnimation(u32 resType, int id);
    void BlendToAnimation(u32 resType, int id);
    void SetVisible(BOOL visible);
    void UpdateBodyScale();
    fx32 GetAnimSpeed();
    void UpdateAnimSpeed();
    void UpdateBodyColors();
    FloorPos GetPointInFront(fx32 distance);
    BOOL IsBeside(fx32 x, fx32 z, fx32 width);
    BOOL IsInFront(fx32 x, fx32 z);
    BOOL IsAhead(fx32 x, fx32 z, fx32 width);
    BOOL IsPointWithinReach(fx32 x, fx32 z, fx32 reach, fx32 maxReach);
    BOOL IsNearerThan(FloorPos pos, fx32 distanceSq);
    void SetLight(int light);
    void ResetSoundPhase(int slot);
    void ResetSoundPhases();
    int PlaySoundOnce(int player, int index, int seqArc);
    int PlaySoundAtAnimPhase(int slot, int player, int seqArc, int index, int quarters);

    // Where the character stands, on the floor. Filling a local and returning it is what leaves the dead copies of
    // the point on the callers' stacks.
    FloorPos GetFloorPos() const {
        FloorPos pos;
        pos.x = mPos.x;
        pos.z = mPos.z;
        return pos;
    }
    FloorPos GetTargetFloorPos() const {
        FloorPos pos;
        pos.x = mTarget.x;
        pos.z = mTarget.z;
        return pos;
    }

    /* 0x25E4 */ // CharacterModel
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mWalkSpeedScale;     // From the Mii's parameter 0x29 and mAnimSpeed
    /* 0x2AF4 */ fx32 mTurnSpeedScale;
    /* 0x2AF8 */ VecFx32 mTarget;          // Where it walks to
    /* 0x2B04 */ int mTurnStep;            // How far it turned in the last step
    /* 0x2B08 */ u16 mMoveAngle;           // The way MoveAlongAngle moves it
    /* 0x2B0A */ u16 mAngle;               // The way the character faces
    /* 0x2B0C */ u16 mTargetAngle;         // TurnToTargetAngle turns mAngle towards it
    /* 0x2B0E */ u16 mWalkAngle;           // The direction to mTarget
    /* 0x2B10 */ s8 mSoundPhase[2];        // PlaySoundAtAnimPhase's progress through an animation, per slot
    /* 0x2B12 */ u8 mUnknown2B12[2];
    /* 0x2B14 */ fx32 mAnimSpeed;          // Given to the model when an animation starts
}; // 0x2B18

// The resident list's orders (src/character/CharacterSort.cpp). The name is the project's own.
class CharacterSort {
public:
    static void RemoveSpaces(wchar_t *str);
    static void KatakanaToHiragana(wchar_t *str);
    static void EnlargeSmallKana(wchar_t *str);
    static void RemoveVoicedMarks(wchar_t *str);
    static void ExpandLongVowelMarks(wchar_t *str);
    static void ExpandLigatures(wchar_t *str);
    static void ToUpperCase(wchar_t *str);
    static void ToUpperCaseWithoutAccents(wchar_t *str);
    static void ToCollationKeys(wchar_t *str);
    static int GetCollationKey(wchar_t c);
    static void MakeSortKeys(wchar_t *str);
    static int CompareNames(const SavedCharacter *a, const SavedCharacter *b, int field);
    static int CompareByName(const SavedCharacter *a, const SavedCharacter *b);
    static int CompareByFamilyName(const SavedCharacter *a, const SavedCharacter *b);
    static int CompareByMiiName(const SavedCharacter *a, const SavedCharacter *b);
    static int CompareStrings(const wchar_t *a, const wchar_t *b);
    static int CompareByPersonality(SavedCharacter *a, SavedCharacter *b);
    static int CompareByBirthday(SavedCharacter *a, SavedCharacter *b);
    static int CompareByBirthdayInYear(SavedCharacter *a, SavedCharacter *b);
    static int CompareBySex(SavedCharacter *a, SavedCharacter *b);
    static int CompareByBloodType(SavedCharacter *a, SavedCharacter *b);
    static int GetInitialGroup(wchar_t c);
    static int GetNameInitialGroup(const wchar_t *name);
};

#endif // CHARACTER_CHARACTER_H
