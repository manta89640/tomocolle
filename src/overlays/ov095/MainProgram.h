#ifndef OV095_MAINPROGRAM_H
#define OV095_MAINPROGRAM_H

// What overlay 95 uses of the main program that has no shared header yet: declarations and stand-ins private to the
// overlay, which only AffinityScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes
// AffinityScene.h with the other scenes' headers).

#include "AffinityScene.h"
#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "sound/SoundManager.h"

#include "../../scene/MainProgram3d.h"

extern "C" {
// --- The screens ---
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
BOOL ScreenControl_BlendFadeIn(ScreenControl *screens, int planes, int level);
BOOL ScreenControl_BlendFadeOut(ScreenControl *screens, int planes, int level);
void func_02024234(ScreenControl *screens, int planes, int a);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int a);
void ScreenControl_ClearBlend(ScreenControl *screens, int planes);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int x);

// --- Buttons and windows ---
void SpriteButton_SetDisabled(SpriteButton *button, BOOL disabled); // Draws the button darkened while it is set
// A sprite button of a window, with four sound numbers and a flag after its size
void func_020331f8(MessageWindow *window, int index, int a, u8 sequence, u8 pressedSequence, WindowPoint position,
                   WindowPoint size, int b, int c, int d, int e, int f);
// A text button of a window: MessageWindow_AddTextButtonWithSoundIds's arguments and one more
void func_020334ac(MessageWindow *window, int index, int a, u8 sequence, u8 pressedSequence, WindowPoint position,
                   WindowPoint size, MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset,
                   int b, int c, int d, int e, int f);

// --- Miis ---
int MiiData_GetSex(SavedCharacter *character); // 0 for a man
void MiiData_GetName(SavedCharacter *character, wchar_t *name);

// --- 3D ---
extern Light gLight1;
void ModelResources_Reset(void *resources);
void Light_Reset(Light *light);
void Light_SetDirectionByCamera(Light *light, Camera *camera, VecFx32 direction);
}

// The two tables of 100 pairs of characters that the saved game keeps (src/list/MiiList.cpp's CharacterPair): this
// overlay keeps each pair's affinity in mScore, the tables sorted by it, the same-sex pairs in mFriendPairs and the
// couples in mCouplePairs.
struct AffinityPair {
    /* 0x0 */ u8 mCharacters[2]; // mIndex of the two
    /* 0x2 */ s8 mScore;         // -1 when the entry is empty
};

struct GameDataPairs {
    /* 0x00000 */ u8 mUnknown00000[0x154B8];
    /* 0x154B8 */ AffinityPair mCouplePairs[100];
    /* 0x155E4 */ AffinityPair mFriendPairs[100];
};

#endif // OV095_MAINPROGRAM_H
