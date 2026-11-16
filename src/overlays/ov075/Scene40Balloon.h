#ifndef OV075_SCENE40BALLOON_H
#define OV075_SCENE40BALLOON_H

#include <nitro.h>

#include "Model.h"

extern "C" {
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
}

class CharacterBase;
class Scene40Character;

// The thought balloon over the character's head ("3HB:/balloon.imd", Mdl/Misc/Tch/mii_tch_balloon, model resource
// 2): it pops up ("appear"), shows what is on the character's mind ("wait_etc", "wait_plain", "wait_dot",
// "wait_dream") and goes ("disappear"). The scene's second Mii face (scene+0x6D324) is drawn in it when the
// character thinks of another character. It is the character's member at 0x2BE4.
//
// The name is descriptive (the model's own): no name of this class survives in the ROM.
class Scene40Balloon {
public:
    enum State {
        STATE_HIDDEN,
        STATE_SHOWN,     // Showing its content, the content's animation looping
        STATE_APPEAR,    // Popping up; shown when the animation ends
        STATE_DISAPPEAR, // Going; hidden when the animation ends
    };

    // What it shows (mContent), as ChooseContent decides it
    enum Content {
        CONTENT_NONE,    // Nothing: it does not appear
        CONTENT_TROUBLE, // In trouble: "wait_etc"
        CONTENT_FACE,    // In trouble of category 4, about another character, whose face it shows: "wait_plain"
        CONTENT_DOTS,    // The record's byte 0x24A is set: "wait_dot"
        CONTENT_DREAM,   // Asleep (long act 18 or 15) and the record's byte 0x246 is set: "wait_dream"
    };

    void Init(int resourceId, int unk, CharacterBase *face, Scene40Character *character);
    void Update();
    void UpdatePosition();
    void Draw();
    void SetState(int state);
    void ChooseContent();
    BOOL IsTouched();

    // The model's animation of a type by its name: plays it, or finds it
    void PlayAnimation(u32 type, const char *name) {
        mModel.SetAnimation(type, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, type, name));
    }
    ModelAnimation *FindAnimation(u32 type, const char *name) {
        return AnimModel_FindAnimation(&mModel, type, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, type, name));
    }

    /* 0x00 */ int mState;
    /* 0x04 */ int mContent;
    /* 0x08 */ BOOL mUnknown08;   // While set the balloon does not appear
    /* 0x0C */ BOOL mCloseQueued; // Asked to go while popping up: it goes as soon as it is shown
    /* 0x10 */ AnimModel mModel;
    /* 0xF0 */ Scene40Character *mCharacter;
    /* 0xF4 */ CharacterBase *mFace; // The face the dream shows
}; // 0xF8

#endif // OV075_SCENE40BALLOON_H
