#ifndef OV075_SCENE40COVER_H
#define OV075_SCENE40COVER_H

#include <nitro.h>

#include "Model.h"

extern "C" {
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
}

// What covers the character (model resource 4, archive "3HT"): the bed it sleeps in (goods "Mdl/Goods/gd10", whose
// textures are "bed01" and "bed02"; the scene starts with it when the character's long act is 18) or the curtain it
// changes its clothes behind ("Mdl/Misc/Tch/mii_tch_curtain", animations "appear00" and "wait00": the curtain
// comes down around it, sways, and goes as "appear00" runs backwards). It is the character's member at 0x2CDC.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40Cover {
public:
    enum Kind {
        KIND_BED,
        KIND_CURTAIN,
    };

    void Init(int kind);
    void Draw();
    void SetKind(int kind);
    void SetPosition(VecFx32 pos, u16 angle);
    void SetDiffuse(GXRgb color);
    void Appear();
    void PlayWait();
    void Disappear();
    BOOL HasAppeared();
    BOOL HasDisappeared();

    // The model's animation of a type by its name: plays it, or finds it
    void PlayAnimation(u32 type, const char *name) {
        mModel.SetAnimation(type, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, type, name));
    }
    ModelAnimation *FindAnimation(u32 type, const char *name) {
        return AnimModel_FindAnimation(&mModel, type, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, type, name));
    }

    /* 0x00 */ int mKind;
    /* 0x04 */ AnimModel mModel;
    /* 0xE4 */ BOOL mDisappearing; // The curtain's animation runs backwards
}; // 0xE8

#endif // OV075_SCENE40COVER_H
