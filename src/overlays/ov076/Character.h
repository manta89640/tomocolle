#ifndef OV076_CHARACTER_H
#define OV076_CHARACTER_H

#include <nitro.h>
#include <nnsys.h>

// STAND-INS for the main program's model and character classes, which are not decompiled yet, as overlay 76's code
// shows them: the virtual functions in the order of their tables, the constructors and destructors as the scene's
// destructor has them inlined, and the fields it touches. The same classes as overlay 74's Model.h and
// Character.h; the layout of the saved character is overlay 109's. The names are descriptive placeholders.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

extern "C" {
// Frees a block of the main heap
void func_020267f4(void *block);
// Of the Mii library (0x0206CF6C on)
BOOL func_0206dc84(void *face);
}

// A model of the library with its position, rotation and scale.
class Model {
public:
    Model() {
        mUnknown04 = 0;
        mResMdl = NULL;
        mUnknown10 = 0;
        mResource = NULL;
    }
    virtual ~Model() {
        if (mResource != NULL) {
            func_020267f4(mResource);
        }
    }
    virtual void Load(int resourceId, int unk); // func_02029b04
    virtual void UnknownVirtual3();             // func_02029bd4
    virtual void Draw();                        // func_02029df4

    /* 0x04 */ int mUnknown04;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ NNSG3dResMdl *mResMdl;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ NNSG3dRenderObj mRenderObj;
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ VecFx32 mScale;
    /* 0x98 */ VecFx32 mPosition;
    /* 0xA4 */ u16 mRotX;
    /* 0xA6 */ u16 mRotY;
    /* 0xA8 */ u16 mRotZ;
    /* 0xAC */ void *mResource; // A block of the main heap that the model owns
    /* 0xB0 */ BOOL mVisible;
}; // 0xB4

// A model with animations.
class AnimModel : public Model {
public:
    AnimModel() {
        mAnimCount = 0;
        ResetAnims();
    }
    virtual ~AnimModel() { RemoveAnims(); }
    virtual void Draw(); // func_0202a0e4

    void RemoveAnims(); // func_0202a464
    void ResetAnims();  // func_0202a618

    /* 0xB4 */ void *mAnims[8];
    /* 0xD4 */ int mAnimCount;
    /* 0xD8 */ fx32 mUnknownD8;
    /* 0xDC */ int mUnknownDC;
}; // 0xE0

// What the save data holds of a character: the 0x2A4 bytes that func_0202c264 copies out (overlay 109's layout).
struct CharacterRecord {
    /* 0x000 */ u8 mMii[0x4A];        // The Mii's data (0x4A bytes, as the Mii exchange sends it)
    /* 0x04A */ u8 mUnknown04A[5];       // The Mii maker's personality page sets them: 1 to 8, 0 while unset
    /* 0x04F */ u8 mUnknown04F;
    /* 0x050 */ u32 mUnknown050Bit0 : 1;
    /*       */ u32 mUnknown050Bits1 : 16;
    /*       */ u32 mBirthYear : 12; // A number the Mii maker's profile page shows in four digits, 0xFFF for none
    /*       */ u32 mBloodType : 3;  // The profile page shows etc message 0x5A plus it
    /* 0x054 */ u32 mUnknown054Bits0 : 4;
    /*       */ u32 mInterior : 7;        // As overlay 109 has it
    /*       */ u32 mUnknown054Bits11 : 9;
    /*       */ u32 mUnknown054Bits20 : 12;
    /* 0x058 */ wchar_t mName[11];          // As overlay 109 has it from here on: the record's implicit assignment
    /* 0x06E */ wchar_t mFamilyName[11];    // (MiiEditVoicePage::RememberForUndo) copies these members one by one
    /* 0x084 */ wchar_t mSpeechDefault[17];
    /* 0x0A6 */ wchar_t mSpeechByMood[4][9];
    /* 0x0EE */ s16 mVoice[6];              // The voice page's five sliders (func_02098d68 sets them) and its choice of four
    /* 0x0FA */ u8 mUnknown0FA;
    /* 0x0FB */ u8 mUnknown0FB;
    /* 0x0FC */ u8 mAffinity[100];
    /* 0x160 */ u8 mRelation[100];
    /* 0x1C4 */ u32 mUnknown1C4;
    /* 0x1C8 */ struct { u32 m[8]; } mUnknown1C8;
    /* 0x1E8 */ struct { u32 m[10]; } mUnknown1E8;
    /* 0x210 */ u32 mUnknown210[3];   // Flags, a bit each
    /* 0x21C */ s64 mUnknown21C;
    /* 0x224 */ u8 mUnknown224;
    /* 0x225 */ u8 mUnknown225;
    /* 0x226 */ u8 mUnknown226;
    /* 0x227 */ u8 mUnknown227;
    /* 0x228 */ s64 mUnknown228;
    /* 0x230 */ s64 mUnknown230;
    /* 0x238 */ u16 mUnknown238;
    /* 0x23A */ u8 mUnknown23A;
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
    /* 0x295 */ u8 mMood;
    /* 0x296 */ u8 mUnknown296;
    /* 0x297 */ u8 mUnknown297;
    /* 0x298 */ u8 mRelationToPlayer;
    /* 0x299 */ u8 mUnknown299;
    /* 0x29A */ u8 mSongSlots[6];
    /* 0x2A0 */ u8 mUnknown2A0;
    /* 0x2A1 */ u8 mUnknown2A1;
    /* 0x2A2 */ u8 mUnknown2A2;
    /* 0x2A3 */ u8 mUnknown2A3;
}; // 0x2A4

// A saved character and what the game keeps beside it. 0x2C0 bytes.
struct SavedCharacter : public CharacterRecord {
    SavedCharacter(); // _ZN14SavedCharacterC1Ev

    /* 0x2A4 */ u16 mIndex;           // Which of the saved characters this is
    /* 0x2A6 */ u8 mUnknown2A6[0x1A];
}; // 0x2C0

// The character as the main program draws it: the saved character and its model's resources.
struct CharacterBase : public SavedCharacter {
    CharacterBase();  // _ZN13CharacterBaseC1Ev, C2Ev
    ~CharacterBase(); // _ZN13CharacterBaseD1Ev, D2Ev

    /* 0x02C0 */ u8 mUnknown02C0[0x14];
    /* 0x02D4 */ u16 mUnknown02D4;
    /* 0x02D6 */ u16 mUnknown02D6;
    /* 0x02D8 */ u8 mUnknown02D8[0x2220];
    BOOL IsUnknown24F8Clear() { return !func_0206dc84(mUnknown24F8); }

    /* 0x24F8 */ void *mUnknown24F8;     // Given to func_0206dc84, of the Mii library
    /* 0x24FC */ u8 mUnknown24FC[0x18];
    /* 0x2514 */ u8 mUnknown2514[0x24]; // Its address is given to func_0208ff94
    /* 0x2538 */ u8 mUnknown2538[0x30]; // Its address is given to func_0208ff94
    /* 0x2568 */ VecFx32 mUnknown2568;
    /* 0x2574 */ u8 mUnknown2574[0x60];
    /* 0x25D4 */ u16 mUnknown25D4;
    /* 0x25D6 */ u16 mUnknown25D6;
    /* 0x25D8 */ u16 mUnknown25D8;
    /* 0x25DA */ u8 mUnknown25DA[6];
    /* 0x25E0 */ u16 mUnknown25E0;
    /* 0x25E2 */ u8 mUnknown25E2[2];
}; // 0x25E4

// The model of a character (func_0208eb18 constructs it). Where it ends is not known: here, at the end of the
// character.
class CharacterModel : public AnimModel {
public:
    CharacterModel(); // func_0208eb18

    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x3DC];
    /* 0x4C8 */ char mArchiveName[4];    // Name its archive is mounted under
    /* 0x4CC */ char mArchivePath[0x1C]; // and the archive's path
    /* 0x4E8 */ char mUnknown4E8[0x3E];  // A model's path: ModelResources_SetModel is given it
    /* 0x526 */ u16 mUnknown526;         // Its angle? Overlay 76 turns the character until it is 0x4000
    /* 0x528 */ u8 mUnknown528[0xC];
}; // 0x534

// A character with its model.
class SceneCharacter : public CharacterBase {
public:
    // The scene's destructor has it inlined: the model's render object first, then the model's own destructors
    // (which store their tables: the model's own level has nothing to do, and stores none)
    ~SceneCharacter() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    /* 0x25E4 */ CharacterModel mModel;
}; // 0x2B18

#endif // OV076_CHARACTER_H
