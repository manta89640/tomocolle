#ifndef G3SPRITE_H
#define G3SPRITE_H

#include <nitro.h>

#include "Sprite.h"

// A textured square drawn by the 3D engine (main program 0x0202B0E8 to 0x0202B6D0, not decompiled): a rectangle of
// a texture of the sprite texture list (data_021232e4), at a position, with a scale. A STAND-IN with what its
// constructor (_ZN8G3SpriteC1Ev, out of line) sets and the fields that its users touch: the island map shows the
// apartment building's windows with them, scene 0x25 a hundred faces. One definition for every user (the scene
// constructors of src/scene/SceneManager.cpp include several of their scenes' headers at once).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class G3Sprite {
public:
    G3Sprite();
    virtual ~G3Sprite(); // func_0202b6cc, which does nothing

    void SetPosition(VecFx32 position) {
        mX = position.x;
        mY = position.y;
        mDepth = -position.z / 16;
    }

    /* 0x04 */ int mTexture;
    /* 0x08 */ BOOL mVisible;    // TRUE from the constructor: func_0202b4a4 draws nothing while it or mUnknown18 is 0
    /* 0x0C */ int mDepth;       // The map sets it from a position's z; scene 0x25 counts frames in it
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ int mUnknown14;
    /* 0x18 */ u8 mUnknown18;    // 31 from the constructor
    /* 0x19 */ u8 mUnknown19;
    /* 0x1A */ u16 mUnknown1A;
    /* 0x1C */ fx32 mScaleX;     // 1.0 from the constructor
    /* 0x20 */ fx32 mScaleY;
    /* 0x24 */ int mX;
    /* 0x28 */ int mY;
    /* 0x2C */ ScreenRect mRect; // Of the texture: (0, 0, 16, 16) from the constructor
    /* 0x3C */ GXRgb mColor;
    /* 0x3E */ u16 mUnknown3E;
    /* 0x40 */ GXRgb mUnknown40;
    /* 0x42 */ GXRgb mUnknown42; // White from the constructor
    /* 0x44 */ int mUnknown44;
    /* 0x48 */ int mUnknown48;   // 1 from the constructor
}; // 0x4C

// The textures of the 3D sprites
struct G3SpriteTextures;

extern "C" {
extern G3SpriteTextures data_021232e4;
void func_0202b4d8(G3SpriteTextures *textures);
void func_0202b4dc(G3SpriteTextures *textures, int index, const char *path, int format, int width, int height);
void func_0202b1f0(G3Sprite *sprite, int texture, const ScreenRect *rect); // Sets its texture and rectangle
void func_0202b4a4(G3Sprite *sprite);                                         // Draws it
void func_0202b4c4(G3Sprite *sprite);
}

#endif // G3SPRITE_H
