#ifndef OV066_SCENE7F_H
#define OV066_SCENE7F_H

#include <nitro.h>

#include "Scene.h"
#include "room/Room.h"
#include "text/TextCanvas.h"

// Overlay 66, scene 0x7F (not decompiled). The scene factory (CreateScene) loads overlay 66 and allocates 0x22B8
// bytes; the constructor is the main program's (0x0204657C, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214B92C) and calls the constructors of three classes of the overlay. The members are as the
// constructor constructs them, by their classes; what lies between is not known. The overlay mounts the room's
// archives ("CO0", "RM") and no scene archive.
//
// The names are the scene id's: no name of these classes survives in the ROM.

// A room of the overlay: the main program's Room with a state and seven halfwords. Its constructor (0x0214AE94)
// builds the Room as its base, stores the overlay's table (0x0214B950, the Room's virtual functions only) and
// clears the halfwords.
class Scene7FRoom : public Room {
public:
    Scene7FRoom();

    /* 0x908 */ int mState;
    /* 0x90C */ u16 mUnknown90C[7];
    /* 0x91A */ s16 mUnknown91A;
}; // 0x91C

// What the overlay's 0x0214B108 constructs: a pointer to the main program's camera (data_02122764), whose fields
// 0x0214B124 sets.
class Scene7FCamera {
public:
    Scene7FCamera();

    /* 0x0 */ void *mCamera;
}; // 0x4

// What the overlay's 0x0214B170 constructs (it clears the halfwords at 0x18 and 0x1A and calls 0x0214B4FC).
class Scene7FUnknown930 {
public:
    Scene7FUnknown930();

    /* 0x00 */ u8 mUnknown00[0x18];
    /* 0x18 */ u16 mUnknown18;
    /* 0x1A */ u16 mUnknown1A;
    /* 0x1C */ u8 mUnknown1C[0x4];
}; // 0x20

// Sprite text canvases of the main program, the same shapes as scene 0x0F's Scene0FTextCanvas and Scene0FTextBox but
// classes of their own (their VTTs, 0x0211087C and 0x0211085C, and tables are other than those): their constructors
// are implicit or inline, their tables are in SceneManager.cpp's data.
class Scene7FTextCanvas : public ObjTextCanvas {}; // 0x1B8

class Scene7FTextBox : public ObjTextCanvas {
public:
    Scene7FTextBox() { mUnknown150 = 0; }

    /* 0x150 */ int mUnknown150;
    /* 0x154 */ u8 mUnknown154[0x18];
}; // 0x1D4

class Scene7F : public Scene {
public:
    Scene7F(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ Scene7FRoom mRoom;
    /* 0x092C */ Scene7FCamera mCamera;
    /* 0x0930 */ Scene7FUnknown930 mUnknown930;
    /* 0x0950 */ ObjTextCanvas mCanvases[4];
    /* 0x1030 */ Scene7FTextBox mTextBoxes[4];
    /* 0x1780 */ Scene7FTextBox mTextBox1;
    /* 0x1954 */ Scene7FTextBox mTextBox2;
    /* 0x1B28 */ Scene7FTextBox mTextBox3;
    /* 0x1CFC */ Scene7FTextBox mTextBox4;
    /* 0x1ED0 */ BgTextCanvas mBgCanvas;
    /* 0x1F3C */ Scene7FTextCanvas mCanvases2[2];
    /* 0x22AC */ u8 mUnknown22AC[0x4];
    /* 0x22B0 */ int mUnknown22B0;
    /* 0x22B4 */ s16 mUnknown22B4[2];
}; // 0x22B8

#endif // OV066_SCENE7F_H
