#ifndef OV092_MAINPROGRAM_H
#define OV092_MAINPROGRAM_H

// What overlay 92 uses of the main program that has no shared header yet: declarations private to the overlay,
// which only WriteSongScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes WriteSongScene.h).

#include "WriteSongScene.h"
#include "GameData.h"
#include "sound/SoundManager.h"
#include "../../scene/MainProgram3d.h"
#include "../../scene/SceneCast.h"

extern "C" {
BOOL GameData_CharacterExists(GameData *game, int index);
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character);
void MiiData_GetName(SavedCharacter *character, wchar_t *name);
void ScreenLoader_SetScrollY(ScreenLoader *loader, int plane, int y);

}

#endif // OV092_MAINPROGRAM_H
