#ifndef OV108_MAINPROGRAM_H
#define OV108_MAINPROGRAM_H

#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "TutorialStepScene.h"
#include "character/Character.h"

// What overlay 108's code uses of the main program, which is not decompiled: declarations, which only the overlay's
// source includes (the scene's class, in TutorialStepScene.h, is also included by the main program's scene
// constructors, src/scene/SceneManager.cpp).

extern "C" {
BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
void func_0202c28c(GameData *game, int index, SavedCharacter *character); // Writes one
}

#endif // OV108_MAINPROGRAM_H
