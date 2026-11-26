#ifndef SCENE_SCENEHELPERS_H
#define SCENE_SCENEHELPERS_H

#include "MainProgram3d.h"
#include "character/CharacterSpeechTexts.h"
#include "SceneCast.h"
#include "FaceCast.h"

// The main program's helpers that the scene overlays share (src/scene/SceneHelpers.cpp, 0x0204E4B4 to 0x0204FC74):
// the speech texts of a character, numbers read out, the characters a scene shows with their bodies (SceneCast,
// called by the overlays 7 to 63) or as faces (FaceCast), the back button (ReturnButton), the room of a scene
// (SceneRoom) and the Mii news screen's background and button.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// Fills text with the reading of a number from 0 to 9999 in the synthesizer's notation ("レイ" for 0)
void MakeNumberReading(wchar_t *text, int number);
// Fills text with a number as digits (at most 9999)
void MakeNumberText(wchar_t *text, int number);

// --- The characters a scene shows ---

// SceneCast: src/scene/SceneCast.h

// FaceCast: src/scene/FaceCast.h

// --- The back button ---

// The button at the lower screen's top right ("Scn/Scene/ReturnBtn_LZ.bin"), which asks for a scene when pressed.
class ReturnButton : public SpriteButton {
public:
    static void MountArchive();
    void Init(int sceneId, int plane, int resource);
    void Update();
    void Draw();
    static void UnmountArchive();

    /* 0x0F4 */ int mSceneId;  // Asked for when pressed, -1 for none
    /* 0x0F8 */ BOOL mHeld;    // It is being touched
    /* 0x0FC */ BOOL mPressed;
}; // 0x100

// --- The room ---

// STAND-INS for the room classes (main program 0x020A3C68 to 0x020A5CA0, not decompiled): as overlay 96 has them,
// with the fields this file touches.
class RoomPart {
public:
    /* 0x000 */ int mUnknown000; // Negative when the part is not there
    /* 0x004 */ u8 mUnknown004[0x48];
    /* 0x04C */ AnimModel mModel;
    /* 0x12C */ u8 mUnknown12C[8];
}; // 0x134

class RoomModels {
public:
    void Draw();
    void SetState(int state);

    /* 0x000 */ u8 mUnknown000[0x4C];
    /* 0x04C */ AnimModel mDoor;
    /* 0x12C */ u8 mUnknown12C[0x78];
    /* 0x1A4 */ RoomPart mParts[6];
    /* 0x8DC */ u8 mUnknown8DC[0x18];
}; // 0x8F4

// The styles a room can have, 0x54 bytes each (gInteriorTable)
struct RoomStyle {
    /* 0x00 */ u8 mUnknown00[8];
    /* 0x08 */ u16 mLightColor;
    /* 0x0A */ u8 mUnknown0A[0xA];
    /* 0x14 */ struct {
        u8 mUnknown0[8];
        s16 mUnknown8; // The part is hidden by ShowParts(0) when it is 0 or less, by ShowParts(1) when 0 or more
    } mParts[6];
    /* 0x50 */ u8 mUnknown50[4];
}; // 0x54

class Room {
public:
    void Mount(const char *archiveName, int unk);
    void MountAll(const char *archiveName, int unk);
    void Load(int resourceId);
    void LoadParts(int resourceId);
    void ReleaseAll();
    void Update();

    /* 0x000 */ void *mVtable;
    /* 0x004 */ u16 mStyle; // Index into gInteriorTable
    /* 0x008 */ u8 mUnknown008[0xC];
    /* 0x014 */ RoomModels mParts;
}; // 0x908

extern "C" {
extern const RoomStyle gInteriorTable[];
}

// A scene's room: the room with its archives ("CO0", "RM") and a short start.
class SceneRoom : public Room {
public:
    void Load(u16 style);
    void Init(int slot, int slot2, BOOL doorTurned);
    void Update();
    void Draw();
    void ShowParts(int mode);

    /* 0x908 */ int mState;
    /* 0x90C */ int mFrame;
}; // 0x910

// --- The Mii news screen ---

// The background of the lower screen of the Mii news ("Scn/Scene/SceneMiiNews_LZ.bin": a TV frame) and its button.
class NewsButton : public SpriteButton {
public:
    static void LoadBackground();
    void Init(int resource);
    void Update(Scene *scene, BOOL leave);
    void Draw();
    static void Unload();

    /* 0x0F4 */ BOOL mPressed;
}; // 0xF8

#endif // SCENE_SCENEHELPERS_H
