#ifndef SCENE_G3SPRITETESTSCENE_H
#define SCENE_G3SPRITETESTSCENE_H

#include "MainProgram.h"
#include "G3Sprite.h"
#include "Model.h"
#include "character/Character.h"

// Scene 0x25 (main program, no overlay): a test of the main program's 3D sprites, with the archive
// "Scn/Scene/SceneTestG3Sprite_LZ.bin". A hundred sprites of the texture "Face.tga" (32 by 32 pixels, one of four
// picked at random) stand at random places in front of a character's body model ("Mdl/Body/body000_3D_LZ.bin");
// each frame one of them, in turn, moves to a new place and gets a new random size. The scene factory (main
// SceneManager::CreateScene, case 37) allocates 0x48B4 bytes and calls the constructor, which is in the main program's file of
// scene constructors (func_0204036c).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The classes of the
// rest of the main program are in include/.

// The models' resource table (gModelResources, which Model.h declares as words)
struct ModelResources {
    /* 0x000 */ u8 mUnknown000[0x424];
    /* 0x424 */ int mUnknown424;
};
#define sModelResources (*(ModelResources *)gModelResources)

extern "C" {
void ModelResources_SetModel(ModelResources *resources, int resourceId, const char *modelPath);
void ModelResources_Reset(ModelResources *resources);
}

// The scene.
class G3SpriteTestScene : public Scene {
public:
    G3SpriteTestScene(int sceneId);

    enum { SPRITE_COUNT = 100 };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetRandomPosition(int index);
    void UpdateSprites();

    /* 0x0010 */ Camera *mCamera;
    /* 0x0014 */ G3Sprite mSprites[SPRITE_COUNT];
    /* 0x1DC4 */ int mNextSprite; // The sprite that moves next: counts down from 99
    /* 0x1DC8 */ CharacterBase mCharacter;
    /* 0x43AC */ CharacterModel mModel;
}; // 0x48B4

#endif // SCENE_G3SPRITETESTSCENE_H
