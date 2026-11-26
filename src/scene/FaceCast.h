#ifndef SCENE_FACECAST_H
#define SCENE_FACECAST_H

#include <nitro.h>

#include "character/Character.h"
#include "character/CharacterSpeechTexts.h"

// The main program's FaceCast (src/scene/SceneHelpers.cpp), in a header of its own so that a scene's header can hold
// one: SceneHelpers.h, which includes it, does not compile beside the other scenes' headers in SceneManager.cpp.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// Where a face is in the camera's plane. A structure of two words of its own, not the first two of a VecFx32:
// SetTransform(int, FaceTransform) passes it in r2 and r3 with one ldm and the scale beside it as a word, where a
// VecFx32 split between the registers and the stack is copied to a temporary first.
struct FacePosition {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 y;
}; // 0x8

// Where one of a FaceCast's faces is, and how it is turned and scaled
struct FaceTransform {
    /* 0x00 */ u16 mId;
    /* 0x04 */ FacePosition mPosition;
    /* 0x0C */ fx32 mScale;
    /* 0x10 */ u16 mRotX;
    /* 0x12 */ u16 mRotY;
    /* 0x14 */ u16 mRotZ;
}; // 0x18

// Up to eleven characters shown as their Mii faces only (CharacterBase), each placed, turned and scaled by a
// FaceTransform; the stylus turns the first.
class FaceCast {
public:
    enum { MAX = 11 };

    void StartLoading(); // Does nothing
    void Init(int capacity);
    void SetupCamera();
    // Every caller passes 0 or 1 (r1); this function does not read it
    void UpdateFaces(BOOL unused);
    void UpdateDrag();
    void DrawFaces();
    void ResetCamera();
    BOOL Choose();
    void ReadCharacters();
    void SetTransform(int index, FacePosition position, fx32 scale, u16 rotX, u16 rotY, u16 rotZ);
    void SetTransform(int index, u16 id, int x, s8 y, u8 scale, u8 rotX, u8 rotY, u8 rotZ);
    void ApplyTransform(int index);
    void SetTransform(int index, FaceTransform transform);
    void LipSync(const char *phoneme);

    /* 0x00000 */ CharacterBase mFaces[MAX];
    /* 0x1A0CC */ CharacterSpeechTexts mSpeechTexts[MAX];
    /* 0x1BD28 */ int mCount;
    /* 0x1BD2C */ int mCapacity;
    /* 0x1BD30 */ int mChosen[MAX];
    /* 0x1BD5C */ FaceTransform mTransforms[MAX];
    /* 0x1BE64 */ int mSpeaker;
    /* 0x1BE68 */ int mSpeakerFace;
    /* 0x1BE6C */ u8 mUnknown1BE6C[0xC];
    /* 0x1BE78 */ u16 mBaseRotY;   // The first face's turn, as SetTransform last set it
    /* 0x1BE7A */ u16 mBaseRotX;
    /* 0x1BE7C */ u8 mUnknown1BE7C[4];
    /* 0x1BE80 */ int mDragX;      // How far the stylus has turned it, -32 to 32
    /* 0x1BE84 */ int mDragY;      // -16 to 16
}; // 0x1BE88 at least

#endif // SCENE_FACECAST_H
