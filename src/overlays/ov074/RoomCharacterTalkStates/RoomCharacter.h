#ifndef ROOMCHARACTER_A2_H
#define ROOMCHARACTER_A2_H

#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"
#include "voice/TtsPlayer.h"

// A character standing in the apartment room (the class name is group a6's, see NOTES.md). This header declares
// only what the two state functions of group a2 use; the offsets are the evidence, the rest of the object is padding.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// A point on the floor: the x and z of a VecFx32. Passed by value in two registers (0x021691fc, 0x02167930).
struct FloorPos {
    fx32 x;
    fx32 z;

    FloorPos() {}
    FloorPos(fx32 x_, fx32 z_) {
        x = x_;
        z = z_;
    }
    FloorPos operator*(int n) const {
        return FloorPos(x * n, z * n);
    }
};

// One entry of the model's animation table, as AnimModel_FindAnimation returns it.
struct ModelAnimFrame {
    /* 0x00 */ fx32 mFrame; // Current frame
    /* 0x04 */ int mUnknownAt4;
    /* 0x08 */ struct ModelAnimResource *mResource;
};

struct ModelAnimResource {
    /* 0x00 */ u32 mUnknownAt0;
    /* 0x04 */ u16 mFrameCount; // Compared, as fx32, with the current frame
};

struct ModelAnim {
    /* 0x00 */ u8 mUnknownAt0[0xC];
    /* 0x0C */ ModelAnimFrame *mFrame;
    /* 0x10 */ fx32 mSpeed;
    /* 0x14 */ BOOL mFinished; // func_0208ef00 and func_0208ef28 clear it when they (re)start the animation
    /* 0x18 */ BOOL mUnknownSetByStart;
};

// The model and its animations, at RoomCharacter+0x25e4.
struct CharacterModel {
    /* 0x000 */ u8 mUnknownAt0[0xD8];
    /* 0x0D8 */ fx32 mUnknownScaleAtD8; // 0x1000 minus this scales the timers of the eating states
    /* 0x0DC */ int mUnknownAtDC;
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknownAtEC[0x18];
    /* 0x104 */ int mAnimId;      // The current 'BCA0' animation
    /* 0x108 */ int mUnknownAt108;
    /* 0x10C */ int mBlendFrames; // > 0 while the change to the current animation is still running
    /* 0x110 */ int mUnknownAt110;
};

// What a character holds in a hand; RoomCharacter has two (0x2b80, 0x2b84).
struct HeldItem {
    /* 0x00 */ u8 mUnknownAt0[0xA4];
    /* 0xA4 */ s16 mRotX;
    /* 0xA6 */ s16 mRotY;
    /* 0xA8 */ s16 mRotZ;
};

// The icon shown over the head, at RoomCharacter+0x2b40 (a1: 0x02152464 ShowHeadIcon picks its kind by mMood).
// Its functions are the main program's (func_02080fc4, func_020810c8, HeadIcon_SetMode).
struct HeadIcon {
    /* 0x00 */ u8 mUnknownAt0[0x3C];
    /* 0x3C */ s16 mAngle;
    /* 0x3E */ s16 mKind;
};

struct JointWork48 {
    u8 mUnknown[0x48];
};

struct JointWork60 {
    u8 mUnknown[0x60];
};

class RoomCharacter {
public:
    void PlaceHeadIcon();                      // 0x0215232c (a1)
    void SetState(int state);                  // 0x02152370 (a1, a6)
    void ShowHeadIcon();                       // 0x02152464 (a1)
    void UpdateTalkStates();                   // 0x02152690: states 1..0x1c, the ones with a partner in the room
    void UpdateAloneStates();                  // 0x02154608: states 0x1d..0x3f (group a4 has it now)
    void SetAnimation(u32 resType, int id);    // 0x02168774 (a6)

    FloorPos GetFloorPos() const {
        return FloorPos(mModel.mPos.x, mModel.mPos.z);
    }

    /* 0x0000 */ u8 mUnknownAt0[0x295];
    /* 0x0295 */ u8 mMood; // 0..4: column of the animation tables, index of the speech texts, of the default faces and of the head icons
    /* 0x0296 */ u8 mUnknownAt296[0x26];
    /* 0x02BC */ s8 mAnimRowIndex; // Index into data_020fba28, which gives the row of the animation tables; what it is in the saved character is not known
    /* 0x02BD */ u8 mUnknownAt2BD[0x2243];
    /* 0x2500 */ int mFace; // Set by Character_SetFace
    /* 0x2504 */ u8 mUnknownAt2504[0xC];
    /* 0x2510 */ BOOL mHeadTurnActive; // 0x021691d4 sets it with the head angles
    /* 0x2514 */ u8 mUnknownAt2514[0xC6];
    /* 0x25DA */ u16 mHeadPitch;
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknownAt25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknownAt25E3;
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x26F8 */ JointWork48 mUnknownJoints48[5];
    /* 0x2860 */ JointWork60 mUnknownJoints60[6];
    /* 0x2AA0 */ u8 mUnknownAt2AA0[0x4C];
    /* 0x2AEC */ fx32 mSpeed;
    /* 0x2AF0 */ fx32 mUnknownAt2AF0;
    /* 0x2AF4 */ fx32 mUnknownAt2AF4;
    /* 0x2AF8 */ VecFx32 mWalkTarget;
    /* 0x2B04 */ int mUnknownAt2B04;
    /* 0x2B08 */ u16 mMoveAngle;
    /* 0x2B0A */ u16 mAngle;       // The way the character faces
    /* 0x2B0C */ u16 mTargetAngle; // Character_TurnToTargetAngle turns mAngle towards it
    /* 0x2B0E */ u16 mUnknownAngleAt2B0E;
    /* 0x2B10 */ s8 mSoundPhase[2]; // Character_PlaySoundAtAnimPhase's progress through an animation, per slot
    /* 0x2B12 */ u8 mUnknownAt2B12[6];
    /* 0x2B18 */ int mState;
    /* 0x2B1C */ int mPrevState;
    /* 0x2B20 */ u8 mUnknownAt2B20[0x10];
    /* 0x2B30 */ RoomCharacter *mPartner;
    /* 0x2B34 */ VecFx32 mSavedWalkTarget;
    /* 0x2B40 */ HeadIcon mHeadIcon;
    /* 0x2B80 */ HeldItem *mHeldItem[2];
    /* 0x2B88 */ u8 mUnknownAt2B88[8];
    /* 0x2B90 */ TtsPlayer *mTtsPlayer;
    /* 0x2B94 */ const char *mSpeechText[5]; // By mMood
    /* 0x2BA8 */ void *mSavedPhrase[5];       // By mMood; pointers into the saved character (a1), speech needs a non-null one
    /* 0x2BBC */ int mSoundHandle;
    /* 0x2BC0 */ u8 mUnknownAt2BC0[0x1E];
    /* 0x2BDE */ s16 mStep;
    /* 0x2BE0 */ s16 mTimer;
    /* 0x2BE2 */ u8 mUnknownAt2BE2[4];
    /* 0x2BE6 */ s16 mTimer2;
    /* 0x2BE8 */ s16 mTimer3;
    /* 0x2BEA */ u8 mUnknownAt2BEA[0x1AC];
    /* 0x2D96 */ s8 mSubStep;
    /* 0x2D97 */ s8 mSubSubStep;
};

#endif // ROOMCHARACTER_A2_H
