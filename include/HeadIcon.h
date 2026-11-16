#ifndef HEADICON_H
#define HEADICON_H

#include <nitro.h>

// The icons shown over the characters' heads, and the particle system they are drawn with (main program
// 0x02080E80 to 0x020812C0, not decompiled): STAND-INS with the fields that the scenes touch, as overlays 73 and 74
// had them. One definition for every scene header (the scene constructors of src/scene/SceneManager.cpp include
// several at once).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The main program's particle system for one effect file (func_02080e98 loads it). Its constructor is emitted as
// C2 (0x02080E80) and C1 (0x02080E8C), byte for byte the same, in that order; the scenes construct plain members
// with C1, and the classes that derive from it (scene 0x76's TestNg111Icon) with C2. HeadIcon's are alike.
struct ParticleSystem {
    ParticleSystem(); // C1 0x02080E8C, C2 0x02080E80

    /* 0x0 */ void *mManager; // func_0207b138 updates it
}; // 0x4

// What func_02081024 reads of the icon that is shown.
struct HeadIconFrame {
    /* 0x00 */ u8 mUnknown00[0x26];
    /* 0x26 */ u16 mValue; // The character states 0x5f, 0x69 and 0x6a play a sound in each frame in which it is 8
};
struct HeadIconShown {
    /* 0x00 */ u8 mUnknown00[0x8];
    /* 0x08 */ HeadIconFrame *mFrame;
};

// The icon shown over a character's head (func_020810e4 updates it).
struct HeadIcon {
    HeadIcon();                                       // C1 0x02081074, C2 0x02081034
    void Show();                                      // 0x02080fc4: removes the icon that is shown (frees mShown)
    void Restart();                                   // 0x02080ff0: clears three flags of the shown icon: plays it again
    void Load(ParticleSystem *particles, int iconId); // 0x020810b4: keeps the id, and creates the icon
    void Reload();                                    // 0x020810c8: creates the icon of mIconId
    void SetMode(int mode);                           // 0x020810dc: sets mMode
    void Update();                                    // 0x020810e4
    void SetPos(fx32 x, fx32 y, fx32 z) {
        mPos.x = x;
        mPos.y = y;
        mPos.z = z;
    }

    /* 0x00 */ void *mUnknown00;
    /* 0x04 */ HeadIconShown *mShown; // The icon that is shown, NULL when none
    /* 0x08 */ int mMode;             // SetMode sets it
    /* 0x0C */ VecFx32 mPos;
    /* 0x18 */ MtxFx33 mRotation;
    /* 0x3C */ u16 mAngle;
    /* 0x3E */ s16 mIconId;           // Which icon: Reload creates the one of this id
}; // 0x40

#endif // HEADICON_H
