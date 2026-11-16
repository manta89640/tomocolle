#ifndef OV069_SAVEPLAYSCENE_H
#define OV069_SAVEPLAYSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// The screens: which planes are shown.
struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0x44];
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x14];
    /* 0xEC */ int mUnknownEC;
};

// Graphics resources, loaded by file name into VRAM.
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);

extern ScreenControl data_0211d604;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_020278f8(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}

// The scene manager.
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

// The main program's camera (data_02122764; func_0202a640 resets it). A stand-in with the fields the scene sets.
struct Camera {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ u8 mUnknown08[0xC];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights (data_021227cc is an array of them; func_0202a96c resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

// --- Characters (main program 0x0208eb18 to 0x0209d898). STAND-INS with the fields the scene uses; overlays 74
// and 96 have fuller ones ---

// One saved character (a resident): the 0x2C0 bytes that the save data holds for each.
struct SavedCharacter {
    void Reset();          // Clears the character and gives it the default Mii
    s8 GetPersonality();   // Computes its personality (0 to 15) from its answers

    /* 0x000 */ u8 mUnknown000[0x2A4];
    /* 0x2A4 */ u16 mIndex;        // Which of the saved characters this is
    /* 0x2A6 */ u8 mUnknown2A6[0x16];
    /* 0x2BC */ s8 mPersonality;   // What GetPersonality computes
    /* 0x2BD */ u8 mUnknown2BD[3];
}; // 0x2C0

// The model of a character with its animations and joints (func_0208eb18 constructs it).
struct CharacterModel {
    /* 0x000 */ void *mVtable;
    /* 0x004 */ void *mResourceSet; // What func_02029660 gives the animations of an archive
    /* 0x008 */ u8 mUnknown008[0x49C];
    /* 0x4A4 */ BOOL mUnknown4A4;
    /* 0x4A8 */ u8 mUnknown4A8[0x14];
}; // 0x4BC

// The character with its Mii face (include/character/Character.h has the class).
struct CharacterBase : public SavedCharacter {
    void MakeFace();

    /* 0x02C0 */ u8 mUnknown02C0[0x2324];
}; // 0x25E4

// The main program's character class that the scenes derive theirs from (0x2B18 bytes; include/character/Character.h
// has the class, whose second base, CharacterModel, is mModel here).
struct Character : public CharacterBase {
    void Init(int resourceId, int unk, u32 defaultMii);
    void SetArchive(const char *archiveName);
    void Update();
    void UpdateSpeedScales();
    void Draw();
    void SetAnimation(u32 resType, int id);
    void SetAngle(u16 angle);
    void SetVisible(BOOL visible);
    void UpdateBodyScale();
    void UpdateAnimSpeed();
    void UpdateBodyColors();

    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AA0 */ u8 mUnknown2AA0[0xC];
    /* 0x2AAC */ char mArchiveName[4];    // Name its archive is mounted under (Character::SetArchive sets both)
    /* 0x2AB0 */ char mArchivePath[0x1C]; // and the archive's path
    /* 0x2ACC */ u8 mUnknown2ACC[0x4C];
}; // 0x2B18

extern "C" {
extern SceneManager gSceneManager;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
void func_0204ab04(SceneManager *manager, int unknown);

// --- The game's random number generator: every use is the inline code of the SDK's MATH_Rand16 ---
extern MATHRandContext16 gRandom;

// --- The saved game ---
BOOL func_0202c354(GameData *game, int index); // Whether there is a saved character with that index
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
void func_0202c8f8(GameData *game, int unknown);
BOOL func_0202cd48(GameData *game); // Starts writing the save data; FALSE when it cannot
BOOL func_0202cd54(GameData *game); // The same after func_0202c8f8(game, 1) and func_0202d07c(game, 0)
BOOL func_0202cd74(GameData *game); // The same after func_0202c8f8(game, 0) and func_0202d07c(game, 0)
BOOL func_0202cd38(GameData *game); // Not 0 while the save data is being written

// --- 3D ---
extern Camera data_02122764;
void func_0202a640(Camera *camera);
extern Light data_021227cc[];
// Resets a light: direction straight down, colour white, enabled
void func_0202a96c(Light *light);
// Sets a light's direction: the vector, rotated by the three angles and normalized
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
// The models' resource table (0x1C bytes per resource id)
extern u8 data_021223e4[];
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk); // Binds an archive's files to an id
void func_0202aecc(void *resources, int resourceId); // Releases an id's model and texture
void func_02029660(void *resourceSet, u32 resType, void *archive); // Gives a resource set the animations of an archive
void *func_020295a4(void *resourceSet, u32 resType);

// --- Characters ---
void func_0209decc(Character *character);
}


// --- The scene's objects ----------------------------------------------------------------------------------------

// The scene's character: a resident picked at random, playing one of the animations of "Mdl/Ica/ica_mii_save_LZ.bin"
// in front of the camera. A class of its own with nothing added: its constructor and destructor are the main
// program's copies after the scene's constructor (0x02041054, 0x02041074), as each scene's character class has them.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavePlayCharacter : public Character {
public:
    SavePlayCharacter();
    ~SavePlayCharacter();
}; // 0x2B18

// The scene's camera: it only sets the main program's camera up.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavePlayCamera {
public:
    SavePlayCamera();
    void Init();
    void Update();

    /* 0x0 */ Camera *mCamera;
}; // 0x4

// The scene's two lights, which take the camera's angles.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SavePlayLights {
public:
    void Init(SavePlayCamera *camera);
    void Update();

    /* 0x0 */ Light *mLights[2];
    /* 0x8 */ SavePlayCamera *mCamera;
}; // 0xC

// --- The scene ----------------------------------------------------------------------------------------------------

// Overlay 69, scenes 0x2C and 0x2D: saving. A resident stands on the upper screen and a message window on the lower
// one asks whether to save what has happened so far (etc message 0x17C) and then whether to go on (0x17E): yes goes
// back to the scene the player came from, no writes the data once more and says to turn the power off (0x17F,
// 0x180). Scene 0x2D comes before wireless play: it says the game is saved first (0x181), and goes on to scene 0x29
// once it is (0x182), or to scene 0x5E if the player says no. A failed write asks for scene 0xC2 (overlay 107). The
// scene factory (main CreateScene, cases 44 and 45) loads overlay 69 and allocates 0x6DB84 bytes; the constructor
// is the main program's (func_02040f38).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// archive, "Scn/Scene/SceneSavePlay_LZ.bin".
class SavePlayScene : public Scene {
public:
    // mState
    enum State {
        STATE_NONE,
        STATE_ASK_SAVE,       // "Save what has happened so far?" Yes saves, no leaves
        STATE_SAVING,         // "Saving..."
        STATE_SAVE,
        STATE_ASK_CONTINUE,   // "Saved. Go on playing?"
        STATE_CONTINUE,       // "Save again when you stop"
        STATE_SAVE_CONTINUE,
        STATE_SAVE_QUIT,
        STATE_QUIT,           // "Turn the power off now. Come and play again"
        STATE_LEAVE,          // Ask for mNextSceneId
        STATE_FADE_OUT,       // The music fades out for 124 frames
        STATE_STOPPED,
        // Before wireless play (scene 0x2D)
        STATE_ASK_SAVE_WIRELESS, // "Wireless play first saves what has happened so far. All right?"
        STATE_SAVING_WIRELESS,
        STATE_SAVE_WIRELESS,
        STATE_SAVED_WIRELESS,    // "Saved. Now for wireless play"
        STATE_LEAVE_WIRELESS,
        STATE_17,
    };

    // What Save does
    enum SaveKind {
        SAVE_0, // func_0202c8f8(0), then write
        SAVE_1, // func_0202c8f8(1), then write
        SAVE_2, // func_0202cd54
        SAVE_3, // func_0202cd74
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    void UpdateSaveStates();
    void UpdateWirelessStates();
    BOOL Save(int kind);
    BOOL ShowMessage(u32 message);
    BOOL ShowOk(u32 message);
    BOOL Ask(u32 message, int flags);
    void UpdateObjects();

    /* 0x00010 */ SavePlayCharacter mCharacters[1];
    /* 0x02B28 */ SavePlayCamera mCamera;
    /* 0x02B2C */ SavePlayLights mLights;
    /* 0x02B38 */ MessageWindow mWindow[1];
    /* 0x0DADC */ MessageFile mEtcMessages;
    /* 0x0DAF4 */ MessageFile mCommonMessages;
    /* 0x0DB0C */ int mState;
    /* 0x0DB10 */ int mNextSceneId;
    /* 0x0DB14 */ u8 mUnknownDB14[0x60000]; // Nothing in the overlay or the constructor touches it
    /* 0x6DB14 */ s16 mStep;
    /* 0x6DB16 */ u16 mAnimationCount; // Of the character's animations: one of them is picked at random
    /* 0x6DB18 */ s16 mResidentCount;
    /* 0x6DB1A */ s16 mFrames;         // That Save has waited
    /* 0x6DB1C */ s16 mAnswer;         // The button that answered the last question
    /* 0x6DB1E */ s8 mSubStep;         // Of Save, ShowMessage, ShowOk and Ask
    /* 0x6DB1F */ s8 mUnknown6DB1F;
    /* 0x6DB20 */ u8 mResidents[100];  // The indices of the saved characters that exist
}; // 0x6DB84

#endif // OV069_SAVEPLAYSCENE_H
