#ifndef OV073_CHARACTER_H
#define OV073_CHARACTER_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"

// STAND-INS for the main program's character classes, which are not decompiled yet: the saved character, the
// character as it is drawn, its model and the icon over its head. The same file as overlay 74's Character.h, with HeadIcon's functions
// under the names symbols.txt has for them; only what the overlays' code shows is named. The names are descriptive, not the original.

// One saved character (a resident): the 0x2C0 bytes that the save data holds for each (func_0202c264 copies one
// out by index, func_0202c28c back in). func_020979c0 is its constructor. The scene makes temporary ones on the
// stack for the residents that are not in the room.
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
    /* 0x023B */ u8 mUnknown023B[0xB];
    /* 0x0246 */ u8 mUnknown0246;        // Only known use: RoomCharacter::ShowHeadIconOfRecord picks icon 9 (0) or 3
    /* 0x0247 */ u8 mUnknown0247[0x4E];
    /* 0x0295 */ u8 mMood;               // 0..4: column of the animation tables, index of the speech texts, of the
                                         // faces and of the head icons
    /* 0x0296 */ u8 mUnknown0296[0xE];
    /* 0x02A4 */ u16 mIndex;             // Which of the saved characters this is (the save data is asked by this)
    /* 0x02A6 */ s16 mReaction;          // What the character chose to do next (RoomCharacter::PickReaction,
                                         // func_0209d5ac); cleared when the two are done
    /* 0x02A8 */ u8 mUnknown02A8[0x14];
    /* 0x02BC */ s8 mPersonality;        // What func_02098f2c computes (0..15); data_020fba28 turns it into the
                                         // row (0..3) of the animation tables
}; // 0x2C0

// The character as the main program draws it: the saved character and its model's resources. func_0208fcf8 is its
// constructor.
struct CharacterBase : public SavedCharacter {
    CharacterBase(); // func_0208fcf8

    /* 0x02C0 */ u8 mUnknown02C0[0x2240];
    /* 0x2500 */ int mFace;              // Character_SetFace sets it
    /* 0x2504 */ u8 mUnknown2504[0xC];
    /* 0x2510 */ BOOL mHeadTurnActive;   // The head is turning to the angles below (func_ov074_021691d4 sets both)
    /* 0x2514 */ u8 mUnknown2514[0xC0];
    /* 0x25D4 */ u16 mUnknown25D4;       // These three and the next three are cleared together when the model is made
    /* 0x25D6 */ u16 mUnknown25D6;       // An angle: added to the head icon's, given to a held item as its x
                                         // rotation, given back to func_ov074_021691d4 as the head's yaw
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u16 mHeadPitch;         // Where the head turns to
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[2];
    /* 0x25E2 */ u8 mHeadTurnFrames;
    /* 0x25E3 */ u8 mUnknown25E3;
}; // 0x25E4

// The model of a character with its animations and joints (func_0208eb18 constructs it). Its destructor is as
// RoomCharacter's implicit one has it inlined (func_0203eee4).
//
// The joints are inside the model, not beside it in the character: CharacterModel_GetJointPosition takes the model
// and returns the last row of mJointMatrices[joint] (0x2A0 + 0x30 * joint from the model). Where the model ends is
// not known: 0x4BC is where the last thing known to be its own ends.
class CharacterModel : public AnimModel {
public:
    CharacterModel(); // func_0208eb18
    ~CharacterModel() { NNS_G3dRenderObjResetCallBack(&mRenderObj); }

    MtxFx33 *GetJointRotation(int joint) { return &mJointRotations[joint]; }
    MtxFx43 *GetJointMatrix(int joint) { return &mJointMatrices[joint]; }

    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x8];     // func_0208fc34 makes three scale factors from 0xEC and 0xF0
    /* 0x0F4 */ fx32 mScale;             // Character_IsPointWithinReach scales a reach by it
    /* 0x0F8 */ u8 mUnknown0F8[0xC];
    /* 0x104 */ int mAnimId;             // The current 'BCA0' animation
    /* 0x108 */ int mPrevAnimId;         // The one being blended out of
    /* 0x10C */ int mBlendFrames;        // > 0 while the change to the current animation is still running
                                         // (func_0208f050 sets it to 0xC000 on a change)
    /* 0x110 */ int mUnknown110;
    /* 0x114 */ MtxFx33 mJointRotations[10]; // By joint; the held items take their rotation from these
    /* 0x27C */ MtxFx43 mJointMatrices[12];  // By joint: 0 is the head, 1 and 2 are the hands
}; // 0x4BC

// What func_02081024 reads of the icon that is shown.
struct HeadIconFrame {
    /* 0x00 */ u8 mUnknown00[0x26];
    /* 0x26 */ u16 mValue; // The character states 0x5f, 0x69 and 0x6a play a sound in each frame in which it is 8
};
struct HeadIconShown {
    /* 0x00 */ u8 mUnknown00[0x8];
    /* 0x08 */ HeadIconFrame *mFrame;
};

// The icon shown over a character's head (func_02081034 constructs it, func_020810e4 updates it).
struct HeadIcon {
    HeadIcon();                       // func_02081034
    void Show();                      // func_02080fc4: removes the icon that is shown (frees mShown, clears it)
    void Restart();                   // func_02080ff0: clears three flags of the icon that is shown: plays it again
    void Load(int param, int iconId); // func_020810b4: keeps the id and calls func_02080f64 with mPos
    void Reload();                    // func_020810c8: creates the icon of mIconId
    void Update();                    // func_020810e4

    /* 0x00 */ void *mUnknown00;
    /* 0x04 */ HeadIconShown *mShown; // The icon that is shown, NULL when none
    /* 0x08 */ int mMode;             // HeadIcon_SetMode sets it
    /* 0x0C */ VecFx32 mPos;
    /* 0x18 */ MtxFx33 mRotation;
    /* 0x3C */ u16 mAngle;
    /* 0x3E */ s16 mIconId;           // Which icon: func_020810c8 creates the one of this id
}; // 0x40

extern "C" {

// --- The model (0x0208eb18 to 0x0208fcf8) ---
// Set the current animation's two flags (mPlayOnce, mFinished); func_0208ef28 also sets its frame
void func_0208ef00(CharacterModel *model, BOOL playOnce, BOOL finished);
void func_0208ef28(CharacterModel *model, BOOL playOnce, fx32 frame, BOOL finished);
// The position of one of the model's joints
VecFx32 CharacterModel_GetJointPosition(CharacterModel *model, int joint);
// Three scale factors from the model's 0xEC and 0xF0: the states 0x69 to 0x6D give them to a held item
VecFx32 func_0208fc34(CharacterModel *model);

// --- The head icon (0x02080f64 to 0x020810e4) ---
int func_02081024(HeadIcon *icon, HeadIconFrame *frame); // frame ? frame->mValue : 0; does not use the icon
void HeadIcon_SetMode(HeadIcon *icon, int mode); // Sets mMode

} // extern "C"

#endif // OV073_CHARACTER_H
