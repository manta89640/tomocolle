#ifndef OV075_MAINPROGRAM_H
#define OV075_MAINPROGRAM_H

// What overlay 75 uses of the main program that has no shared header yet: declarations private to the overlay,
// which only Scene40.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes Scene40.h).

#include <nitro.h>

#include "SceneMemory.h"
#include "resident/ResidentLife.h"
#include "save/SaveBlocks.h"
#include "Scene40Pool.h"

struct ArchiveLoader;
struct ScreenControl;
struct CharacterRecord;
class GameData;
class ParticleSystem;
class SavedCharacter;

extern "C" {
// The main program's graphics manager, which saves and restores the screens' state
extern u32 gGraphicsManager[];
void GraphicsManager_SaveState(u32 *manager, void *state, int planes);
void GraphicsManager_RestoreState(u32 *manager, void *state, int planes);

void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int level);
void ScreenLoader_SetScrollX(struct ScreenLoader *loader, int plane, int x);
void ScreenLoader_SetScrollY(struct ScreenLoader *loader, int plane, int y);

BOOL GameData_CharacterExists(GameData *game, int index);
BOOL GameData_IsCharacterSlotUsed(GameData *game, int index); // Whether there is a saved character with that index
BOOL func_0202c720(GameData *game, int index);
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
BOOL MiiData_GetSex(CharacterRecord *character);
void MiiData_GetName(CharacterRecord *character, wchar_t *name); // Its name, as a string

extern int gPersonalityKinds[]; // Each personality's kind, 0 to 3: the talk messages and touch animations by it
extern SaveHeader gIslandState;
int ModelResources_SetTouchLine(void *resources, int x, int y);
void ScaleVec(fx32 scale, const VecFx32 *in, VecFx32 *out);
s32 _s32_div_f(s32 a, s32 b); // The line into the scene under a point of the touch screen // Each personality's kind, 0 to 3: the talk messages and touch animations by it
}

// gSceneMemory and gSceneScratch are one block of the main program's .bss (0x0213BA74, 0x3C + 0x70 bytes), which
// src/scene/EmptyScene.cpp defines as two objects. This overlay reads the scratch's first bytes off gSceneMemory's
// address (gSceneMemory + 0x3C) as well as off its own, so its source saw them as part of gSceneMemory.
// The bytes after gSceneMemory's 0x3C: the first of gSceneScratch, which this file reaches through gSceneMemory as
// well (the literal is gSceneMemory's address and the offset is in the instruction)
struct Scene40SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacters[12];
    /* 0x2C */ u16 mUnknown2C[8];
    /* 0x3C */ union { u8 mTalkAnswers[2]; u8 mNextMode; }; // The talk page's answers, for the scene it goes to
}; // 0x3E

static inline Scene40SceneMemory &GetSceneMemory() {
    return (Scene40SceneMemory &)gSceneMemory;
}

// --- The overlay's own, not decompiled yet (other agents' ranges): by address until named ---------------------

class Scene40Character;
class Scene40ListPage;
struct Scene40Context;
struct ScreenRect;
struct WindowPoint;

extern "C" {
int func_ov075_02179290(Scene40Character *character);
void func_ov075_021793fc(int unknown);
void func_ov075_021753c8(Scene40Character *character);
void func_0207b138(void *manager); // Updates a particle system's manager

}

// The scenes after which the visit starts with page entry 23 (with data_ov075_0217b66c and data_ov075_0217ba94)
struct Scene40SceneEntry {
    /* 0x0 */ int mSceneId;
    /* 0x4 */ int mUnknown4;
}; // 0x8
// The scenes after which the visit starts with page entry 24, and what the touch page gives for each
struct Scene40PresentEntry {
    /* 0x0 */ int mSceneId;
    /* 0x4 */ u8 mUnknown4;
    /* 0x5 */ u8 mUnknown5;
    /* 0x6 */ u8 mUnknown6;
    /* 0x7 */ u8 mUnknown7;
    /* 0x8 */ u8 mUnknown8;
    /* 0x9 */ u8 mUnknown9;
    /* 0xA */ u8 mUnknownA[2];
}; // 0xC



#endif // OV075_MAINPROGRAM_H
