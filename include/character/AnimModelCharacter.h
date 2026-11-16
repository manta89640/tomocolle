#ifndef CHARACTER_ANIMMODELCHARACTER_H
#define CHARACTER_ANIMMODELCHARACTER_H

#include "character/Character.h"
#include "Model.h"

// A character's face with a plain animated model instead of the character model: what scene 0x02 and scene 0x17
// hold. Its constructor and destructor are the implicit ones, emitted in the scene constructors' file
// (src/scene/SceneManager.cpp: C1 at 0x0203DFCC, D1 at 0x0203E064, which overlay 77 calls too). The name is the
// project's own and says only what it is made of.
class AnimModelCharacter : public CharacterBase {
public:
    ~AnimModelCharacter() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    /* 0x25E4 */ AnimModel mModel;
    /* 0x26C4 */ u8 mUnknown26C4[0x64];
}; // 0x2728

#endif // CHARACTER_ANIMMODELCHARACTER_H
