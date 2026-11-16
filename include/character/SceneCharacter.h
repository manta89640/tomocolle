#ifndef CHARACTER_SCENECHARACTER_H
#define CHARACTER_SCENECHARACTER_H

#include "character/Character.h"

// A character as many scenes hold it, with nothing of its own: its complete-object constructor and destructor
// (_ZN14SceneCharacterC1Ev at 0x0203E168, _ZN14SceneCharacterD1Ev at 0x0203E188) are the implicit ones, emitted in
// the file of the scene constructors (src/scene/SceneManager.cpp) and called from about sixty scene constructors
// and fifty overlays. The name is the project's own.
class SceneCharacter : public Character {};

#endif // CHARACTER_SCENECHARACTER_H
