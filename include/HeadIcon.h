#ifndef HEADICON_H
#define HEADICON_H

#include <nitro.h>

// The icons shown over the characters' heads, and the particle system they are drawn with (main program
// 0x02080E80 to 0x020812C0, not decompiled): STAND-INS with the fields that the scenes touch, as overlays 73 and 74
// had them. One definition for every scene header (the scene constructors of src/scene/SceneManager.cpp include
// several at once).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The main program's particle system for one effect file (func_02080e98 loads it). The shop scenes construct
// another object of the same kind with a byte-for-byte copy of the constructor (0x02080E8C), with byte-for-byte
// copies of HeadIcon's constructor (0x02081074) for the icons after it; what tells them apart is not known.
struct ParticleSystem {
    ParticleSystem(); // 0x02080E80

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
    HeadIcon();                                       // 0x02081034
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
