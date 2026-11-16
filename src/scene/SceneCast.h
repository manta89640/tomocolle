#ifndef SCENE_SCENECAST_H
#define SCENE_SCENECAST_H

#include <nitro.h>

#include "character/Character.h"
#include "character/CharacterSpeechTexts.h"

// The scenes' cast of characters (src/scene/SceneHelpers.cpp), in a header of its own so that the overlays can take
// it without SceneHelpers.h's stand-ins of the main program (MainProgram3d.h).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.

struct Camera;

class GameData;

// Up to ten characters with their bodies, chosen among the saved ones: those of gSceneMemory.mCharacters first, the
// rest at random. Each has its archive ("B0I" to "B9I"), and the room animations ("BIC") as resource 31.
class SceneCast {
public:
    enum { MAX = 10 };

    BOOL Choose(int count);
    void SetCharacters(Character *characters);
    void MountArchives(const char *animationArchive);
    void InitCharacters();
    void LipSync(const char *phoneme);
    void UpdateCharacters(int unused); // Every caller passes 1, which it does not read
    void DrawCharacters();
    void Release();
    void ReloadCharacter(int index);
    void RereadCharacter(int index);

    CharacterSpeechTexts *GetSpeechTexts(int index) { return &mSpeechTexts[index]; }

    /* 0x0000 */ int mCount;
    /* 0x0004 */ Character *mCharacters[MAX];
    /* 0x002C */ CharacterSpeechTexts mSpeechTexts[MAX];
    /* 0x19F4 */ int mResidents[100];    // The saved characters there are, by index
    /* 0x1B84 */ int mResidentCount;
    /* 0x1B88 */ int mChosen[MAX];       // The saved characters shown, by index
    /* 0x1BB0 */ int mSpeaker;           // The character whose lips follow the speech, -1 for none
    /* 0x1BB4 */ int mSpeakerFace;
    /* 0x1BB8 */ Camera *mCamera;
}; // 0x1BBC

#endif // SCENE_SCENECAST_H
