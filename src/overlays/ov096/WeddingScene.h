#ifndef OV096_WEDDINGSCENE_H
#define OV096_WEDDINGSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Scene.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

#include <wchar.h>

// Overlay 96: the wedding announcement. Two characters who are to marry stand in front of the camera; one says
// "We are getting married", the other thanks the player, the first promises a happy life, the screen announces the
// marriage, and the second hands the player a wedding album, which goes into the inventory. When the scene comes
// back from the album (scene 0xC0), they say goodbye and leave on their honeymoon.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The main program's
// classes below are STAND-INS (not decompiled): their sizes and the fields that this overlay's code touches. Most of
// them are the same as overlay 74's stand-ins, whose headers say more.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

class WeddingCharacter;
class WeddingScene;

// --- Models (main program 0x02029b04 to 0x0202b008) -----------------------------------------------------------

class AnimModel;
struct ModelAnimation;

extern "C" {
void func_020267f4(void *block); // Frees a block of the main heap
ModelAnimation *AnimModel_FindAnimation(AnimModel *model, u32 resType, int id);
}

// One animation of a model, as AnimModel_FindAnimation finds it.
struct ModelAnimation {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ int mId;
    /* 0x08 */ u32 mResType;
    /* 0x0C */ NNSG3dAnmObj *mAnmObj;
    /* 0x10 */ fx32 mSpeed;    // Frames of the animation per frame of the game
    /* 0x14 */ BOOL mFinished; // Set when a play-once animation reaches its end
    /* 0x18 */ BOOL mPlayOnce;
}; // 0x1C

// A model of the library with its position, rotation and scale.
class Model {
public:
    virtual ~Model() {
        if (mResource != NULL) {
            func_020267f4(mResource);
        }
    }
    virtual void Load(int resourceId, int unk); // func_02029b04
    virtual void UnknownVirtual3();             // func_02029bd4
    virtual void Draw();                        // func_02029df4

    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }

    /* 0x04 */ void *mResourceSet; // What ResourceSet_FindIndexByName and func_02029660 look the animations up in
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ NNSG3dResMdl *mResMdl;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ NNSG3dRenderObj mRenderObj;
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ VecFx32 mScale;
    /* 0x98 */ VecFx32 mPosition;
    /* 0xA4 */ u16 mRotX;
    /* 0xA6 */ u16 mRotY;
    /* 0xA8 */ u16 mRotZ;
    /* 0xAC */ void *mResource; // A block of the main heap that the model owns
    /* 0xB0 */ BOOL mVisible;
}; // 0xB4

// A model with animations.
class AnimModel : public Model {
public:
    virtual ~AnimModel() { RemoveAnims(); }
    virtual void Draw(); // func_0202a0e4

    void RemoveAnims(); // func_0202a464

    /* 0xB4 */ void *mAnims[8];
    /* 0xD4 */ int mAnimCount;
    /* 0xD8 */ fx32 mUnknownD8;
    /* 0xDC */ int mUnknownDC;
}; // 0xE0

extern "C" {
// The models' resource table (0x1C bytes per resource id)
extern u8 data_021223e4[];
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk); // Binds an archive's files to an id
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
void func_0202aecc(void *resources, int resourceId); // Releases an id's model and texture
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
// Gives a resource set the animations of an archive
void func_02029660(void *resourceSet, u32 resType, void *archive);
}

// --- The room (main program 0x020a3c68 to 0x020a5ca0) ---------------------------------------------------------

// The room's door. Not a class derived from the model: its destructor does not set a virtual table before it takes
// the model's render callback away.
class RoomDoorModel {
public:
    ~RoomDoorModel() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    /* 0x000 */ AnimModel mModel;
    /* 0x0E0 */ u8 mUnknownE0[0x78];
}; // 0x158

class RoomPart {
public:
    ~RoomPart();

    /* 0x000 */ u8 mUnknown00[0x4C];
    /* 0x04C */ AnimModel mModel;
    /* 0x12C */ u8 mUnknown12C[8];
}; // 0x134

class RoomParts {
public:
    /* 0x000 */ u8 mUnknown000[0x4C];
    /* 0x04C */ RoomDoorModel mDoor;
    /* 0x1A4 */ RoomPart mParts[6];
    /* 0x8DC */ u8 mUnknown8DC[0x18];
}; // 0x8F4

// The styles a room can have, 0x54 bytes each
struct RoomStyle {
    /* 0x00 */ u8 mUnknown00[8];
    /* 0x08 */ u16 mLightColor;
    /* 0x0A */ u8 mUnknown0A[0x4A];
}; // 0x54

class Room {
public:
    virtual u32 GetStyleUnknown0();

    /* 0x004 */ u16 mStyle; // Index into data_020f72f8
    /* 0x008 */ u8 mUnknown008[0xC];
    /* 0x014 */ RoomParts mParts;
}; // 0x908

extern "C" {
extern const RoomStyle data_020f72f8[];
void func_020a54c4(Room *room, const char *archiveName, int unknown);
void func_020a56bc(Room *room, const char *archiveName, int unknown);
void func_020a5514(Room *room, int slot);
void func_020a5760(Room *room, int slot);
void func_020a4ab8(RoomParts *parts, int state);
void func_020a4a50(RoomParts *parts); // Draws them
void func_020a5948(Room *room);
void func_020a5960(Room *room);
u16 func_020a4d90(Room *room); // The room's light colour now
}

// --- Characters (main program 0x0208eb18 to 0x0209d898) -------------------------------------------------------

// One saved character (a resident): the 0x2C0 bytes that the save data holds for each.
struct SavedCharacter {
    SavedCharacter(); // func_020979c0

    /* 0x0000 */ u8 mUnknown0000[0x54];
    /* 0x0054 */ u32 mUnknown0054Bits0 : 4;
    /*        */ u32 mInterior : 7;   // Which interior its room has
    /* 0x0058 */ u8 mUnknown0058[0x1CE];
    /* 0x0226 */ u8 mUnknown0226;     // With mUnknown0227, an entry of data_02114d78 that says how long the
    /* 0x0227 */ u8 mUnknown0227;     // honeymoon lasts
    /* 0x0228 */ u8 mUnknown0228[8];
    /* 0x0230 */ s64 mAwayUntil;      // A time in seconds, as the clock's: the wedding sets it to when the honeymoon
                                      // ends
    /* 0x0238 */ u8 mUnknown0238[2];
    /* 0x023A */ u8 mMoodLocked;      // Overlay 74's name; the wedding sets it to 3 for the honeymoon
    /* 0x023B */ u8 mUnknown023B[0x69];
    /* 0x02A4 */ u16 mIndex;          // Which of the saved characters this is
    /* 0x02A6 */ u8 mUnknown02A6[0x16];
    /* 0x02BC */ s8 mPersonality;     // What func_02098f2c computes
}; // 0x2C0

// The character as the main program draws it (func_0208fcf8 is its constructor).
struct CharacterBase : public SavedCharacter {
    /* 0x02C0 */ u8 mUnknown02C0[0x2250];
    /* 0x2510 */ BOOL mHeadTurnActive; // The head is turning to mHeadPitch/mHeadYaw
    /* 0x2514 */ u8 mUnknown2514[0xC0];
    /* 0x25D4 */ u16 mHeadPitchNow;    // Where the head is turned now
    /* 0x25D6 */ u16 mHeadYawNow;
    /* 0x25D8 */ u16 mHeadRollNow;
    /* 0x25DA */ u16 mHeadPitch;       // Where it turns to
    /* 0x25DC */ u16 mHeadYaw;
    /* 0x25DE */ u16 mHeadRoll;
    /* 0x25E0 */ u8 mUnknown25E0[4];
}; // 0x25E4

// The model of a character with its animations and joints (func_0208eb18 constructs it).
class CharacterModel : public AnimModel {
public:
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x18];
    /* 0x104 */ int mAnimId; // The current 'BCA0' animation
    /* 0x108 */ u8 mUnknown108[0x354];
    /* 0x45C */ BOOL mUnknown45C; // Set while a character holds the item out
    /* 0x460 */ u8 mUnknown460[0x44];
    /* 0x4A4 */ BOOL mUnknown4A4; // Set once a character's animations are bound
    /* 0x4A8 */ u8 mUnknown4A8[0x14];
}; // 0x4BC

extern "C" {
// The position of one of the model's joints (0 is the head, 1 and 2 the hands)
VecFx32 CharacterModel_GetJointPosition(CharacterModel *model, int joint);
// The matrix of one of the model's joints, and its rotation
void CharacterModel_GetJointMatrices(CharacterModel *model, int joint, MtxFx43 *matrix, MtxFx33 *rotation);
}

// The main program's character class that the scenes derive theirs from (0x2B18 bytes).
class Character : public CharacterBase {
public:
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AA0 */ u8 mUnknown2AA0[0xC];
    /* 0x2AAC */ char mArchiveName[4];  // Name its archive is mounted under (func_02091048 sets both)
    /* 0x2AB0 */ char mArchivePath[0x1C]; // and the archive's path
    /* 0x2ACC */ char mModelPath[0x20];   // The model in the archive
    /* 0x2AEC */ u8 mUnknown2AEC[0x1E];
    /* 0x2B0A */ u16 mAngle;            // The way the character faces
    /* 0x2B0C */ u8 mUnknown2B0C[0xC];
}; // 0x2B18

extern "C" {
void Character_SetFace(Character *character, int face);
void Character_SetAngle(Character *character, u16 angle);
void func_02090a58(Character *character, const char *phoneme, int face); // Lip sync
BOOL func_02090c78(Character *character, u16 *angle, int limit);        // Clamps an angle of the head
void func_02090fdc(Character *character, int slot, int param2, int param3);
void func_02091048(Character *character, const char *archiveName);
void func_020910a8(Character *character);
void func_020910d4(Character *character);
void func_02091968(Character *character); // Draws it
void Character_SetAnimation(Character *character, u32 resType, int id); // Starts an animation
void func_020919e8(Character *character, u32 resType, int id); // Starts an animation
void func_0208fefc(Character *character);
void func_02091a24(Character *character);
void func_02091a6c(Character *character);
void func_02091a88(Character *character);
s8 func_02098f2c(SavedCharacter *character);
void func_0209decc(Character *character);
void func_0209d898(Character *character, TtsPlayer *player); // Gives the speech engine the character's voice
void func_0207a80c(SavedCharacter *character, wchar_t *name); // Its name, as a string
void func_020372d0(wchar_t *dst, const wchar_t *src, int size);
}

// --- Other main-program things --------------------------------------------------------------------------------

// The saved game
struct SavedGame;
extern "C" {
extern SavedGame gGameData;
void func_0202c264(SavedGame *game, int index, SavedCharacter *character); // Copies a saved character out
void func_0202c28c(SavedGame *game, int index, SavedCharacter *character); // and back in
}

// An item: the base class of an item in the inventory and of the item shown by an item model.
class Item {
public:
    virtual void UnknownVirtual0();

    /* 0x4 */ u16 mId;
}; // 0x8

class InventoryItem : public Item {
public:
    BOOL Add(int count); // func_0209ed84

    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
}; // 0x14

// The player's items: what the save data holds of each kind, loaded and saved together.
class Inventory {
public:
    void Load(); // func_0209ee28
    void Save(); // func_0209eef8

    /* 0x0000 */ int mCount;
    /* 0x0004 */ InventoryItem mItems[255];
    /* 0x13F0 */ InventoryItem *mList[255];
}; // 0x17EC

// The 3D model of an item (constructor func_0209f5f0)
class ItemModel : public AnimModel {
public:
    void SetItem(const char *archiveName, int item); // func_0209f700
    void LoadModel(int resourceId);                  // func_0209f770
    void LoadAnimation(int resourceId);              // func_0209f790
    void ResetScale();                               // func_0209f7fc
    void SetScale(fx32 scale);                       // func_0209f84c

    /* 0x0E0 */ Item mItem;
    /* 0x0E8 */ u8 mUnknownE8[0xC];
    /* 0x0F4 */ int mResourceId;
    /* 0x0F8 */ char mArchiveName[4];
    /* 0x0FC */ char mArchivePath[0x1E];
    /* 0x11A */ char mModelPath[0xE];
    /* 0x128 */ fx32 mBaseScale;
}; // 0x12C

// The item the wedding gives.
class WeddingGiftModel : public ItemModel {
public:
    void Load(int resourceId);
    void ChangeItem(int item);
}; // 0x12C

// The main program's camera
struct Camera {
    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }
    void SetTarget(VecFx32 target) {
        mTarget.x = target.x;
        mTarget.y = target.y;
        mTarget.z = target.z;
    }

    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights
struct Light {
    /* 0x0 */ u8 mUnknown0[6];
    /* 0x6 */ u16 mColor;
    /* 0x8 */ u8 mUnknown8[4];
}; // 0xC

// What the scenes remember for each other
struct SceneMemory {
    /* 0x00 */ VecFx32 mCameraPosition;
    /* 0x0C */ VecFx32 mCameraTarget;
    /* 0x18 */ u8 mUnknown18[8];
    /* 0x20 */ u8 mCharacters[2]; // The characters a scene shows: the wedding's come back to it from scene 0xC0
};

struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0x40];
    /* 0x44 */ int mUnknown44;
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x10];
    /* 0xE8 */ BOOL mUnknown44Changed;
    /* 0xEC */ BOOL mUnknown48Changed;
};

struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern Camera data_02122764;
extern Light data_021227cc[];
extern SceneMemory data_0213ba74;
extern ScreenControl data_0211d604;
extern SceneManager gSceneManager;
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
extern void *data_02123440;
// By SavedCharacter::mUnknown0227, a table of these by mUnknown0226
struct EventInfo {
    /* 0x0 */ u32 mUnknown0;
    /* 0x4 */ u32 mUnknown4 : 16;
    /*     */ u32 mMinutes : 12; // How long it lasts
    /*     */ u32 mUnknown4Bits28 : 4;
    /* 0x8 */ u32 mUnknown8;
}; // 0xC
extern const EventInfo *data_02114d78[];

void func_0202a640(Camera *camera);
void func_0202a748(Camera *camera);
void func_0202a96c(Light *light);
void Light_SetDirection(Light *light, int unk, u16 angle, int unk2, VecFx32 direction);
void ScaleVec(fx32 scale, const VecFx32 *in, VecFx32 *out);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void func_0203c02c(SceneManager *manager, int sceneId);
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void func_020278f8(PaletteLoader *loader, int a, int b);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028154(ScreenLoader *loader, int plane, int y);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);
void func_02023f4c(ScreenControl *screens, int planes, int level);
s64 Clock_GetSeconds(void *clock);
}

#define gSceneMemory data_0213ba74

// How long a character's honeymoon lasts
inline u32 GetHoneymoonMinutes(SavedCharacter *character) {
    return data_02114d78[character->mUnknown0227][character->mUnknown0226].mMinutes;
}


// --- Vector helpers -----------------------------------------------------------------------------------------------

// A point on the floor: the x and z of a VecFx32 (as overlay 74's FloorPos: a plain structure, passed by value).
struct FloorPos {
    /* 0x0 */ fx32 x;
    /* 0x4 */ fx32 z;
};

inline FloorPos operator-(FloorPos a, FloorPos b) {
    FloorPos result;
    result.x = a.x - b.x;
    result.z = a.z - b.z;
    return result;
}

static inline fx32 LengthSq(const VecFx32 *v) {
    return FX_Mul(v->x, v->x) + FX_Mul(v->y, v->y) + FX_Mul(v->z, v->z);
}

// The length of (x, z), by the hardware square root
static inline fx32 SqrtXZ(fx32 x, fx32 z) {
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

// (x, z) made one long, as VEC_Normalize does it
static inline FloorPos NormalizeXZ(fx32 x, fx32 z) {
    FloorPos result;
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    s32 sqrt = (s32)CP_GetSqrtResult32();
    t = CP_GetDivResult64();
    t = t * sqrt;
    result.x = (fx32)((t * x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    result.z = (fx32)((t * z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    return result;
}

static inline fx32 Clamp(fx32 value, fx32 min, fx32 max) {
    if (value > max) {
        value = max;
    } else if (value < min) {
        value = min;
    }
    return value;
}

static inline void CopyVec(VecFx32 *dst, const VecFx32 *src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

static inline VecFx32 Shift(const VecFx32 &v, int shift) {
    VecFx32 result;
    result.x = v.x >> shift;
    result.y = v.y >> shift;
    result.z = v.z >> shift;
    return result;
}

// --- The speech window (main program 0x02039660 to 0x020398e4) -------------------------------------------------

class SpeechWindow {
public:
    int Init(int charOffset, MessageFile *file, u32 planes, int screen, TtsPlayer *player, BOOL tall);
    void Show(u32 message, int mode, int frames);
    void Hide();
    void Update();
    void SetStringArg(const wchar_t *text, int index);
    void Draw();

    /* 0x000 */ MessageFile *mMessageFile;
    /* 0x004 */ TtsPlayer *mTtsPlayer;
    /* 0x008 */ MessageText mText;
    /* 0x5F8 */ int mMode;
    /* 0x5FC */ int mFrames;
    /* 0x600 */ int mFrame;
    /* 0x604 */ u32 mPlanes;
    /* 0x608 */ BOOL mShown;
}; // 0x60C

// --- This overlay's classes --------------------------------------------------------------------------------------

// What the scene gives its characters.
struct WeddingContext {
    /* 0x00 */ int mSceneId;
    /* 0x04 */ MessageFile *mCommonMessages;
    /* 0x08 */ MessageFile *mTroubleMessages;
    /* 0x0C */ MessageFile *mItemMessages;
    /* 0x10 */ MessageFile *mEtcMessages;
    /* 0x14 */ SpeechWindow *mWindow;
    /* 0x18 */ TtsPlayer *mTtsPlayer;
    /* 0x1C */ MessageText *mSpeechText;
    /* 0x20 */ class WeddingCamera *mCamera;
    /* 0x24 */ class WeddingLights *mLights;
    /* 0x28 */ WeddingCharacter *mCharacters[2];
    /* 0x30 */ WeddingGiftModel *mGift;
}; // 0x34

class WeddingCharacter : public Character {
public:
    enum State {
        STATE_NONE,
        STATE_ANNOUNCE, // "We are getting married"
        STATE_THANK,    // "Thank you", with a greeting
        STATE_LOOK_AT_PARTNER,
        STATE_PROMISE,  // "We will live happily ever after"
        STATE_GIVE,     // "Please take this as a keepsake", and hands over the album
        STATE_FAREWELL, // "We are off on our honeymoon"
    };

    // What one argument of a message is
    enum ArgType {
        ARG_NONE,
        ARG_CHARACTER, // The name of the saved character with that index
        ARG_ITEM,      // The name of the item with that number
    };

    ~WeddingCharacter(); // func_02048748, the main program's

    void Init(WeddingContext *context);
    void Update();
    void UpdateState();
    void SetState(int state);
    void FaceCamera();
    void Speak(int file, u32 message, int count, int *types, int *values, int mode);
    void Speak(int file, u32 message, int type, int value, int mode);
    void Speak(u32 message, int mode);
    void HoldOutGift();
    void LookAt(VecFx32 *target, int pitchLimit, int yawLimit);
    void TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit);
    void LookAtCamera();
    void TurnHeadToCamera();
    void LookAtPartner();
    u16 GetHeadPitchTo(VecFx32 *target);
    u16 GetHeadYawTo(VecFx32 *target);

    void PlayAnimation(const char *name, BOOL once) {
        func_020919e8(this, RESOURCE_BCA0, ResourceSet_FindIndexByName(mModel.mResourceSet, RESOURCE_BCA0, name));
        AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mPlayOnce = once;
    }
    VecFx32 GetPos() { return mModel.mPos; }
    void SetPos(VecFx32 pos) {
        mModel.mPos.x = pos.x;
        mModel.mPos.y = pos.y;
        mModel.mPos.z = pos.z;
    }
    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mModel.mPos.x;
        pos.z = mModel.mPos.z;
        return pos;
    }
    u16 AngleTo(FloorPos target) {
        FloorPos delta = target - GetFloorPos();
        return FX_Atan2Idx(delta.z, delta.x);
    }
    ModelAnimation *GetAnimation() { return AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId); }

    /* 0x2B18 */ int mCurrentState;
    /* 0x2B1C */ int mState;
    /* 0x2B20 */ BOOL mEntered;   // The state has just been set
    /* 0x2B24 */ BOOL mDone;      // The state has done what it does: the scene goes on
    /* 0x2B28 */ int mFrame;      // Frames in the state, or in the step
    /* 0x2B2C */ int mWait;       // Frames a step waits
    /* 0x2B30 */ int mStep;
    /* 0x2B34 */ BOOL mLipSync;   // The character's mouth follows the speech
    /* 0x2B38 */ TtsPlayer *mTtsPlayer;
    /* 0x2B3C */ WeddingContext *mContext;
    /* 0x2B40 */ WeddingCharacter *mPartner;
}; // 0x2B44

// The camera: it stays in front of the two, between them.
class WeddingCamera {
public:
    void Init(WeddingCharacter *first, WeddingCharacter *second);
    void Reset(BOOL restore);
    void Update();
    void GetStandingPoints(VecFx32 *first, VecFx32 *second);
    VecFx32 Approach(VecFx32 *goal, VecFx32 *current, int shift);

    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }
    void SetTarget(VecFx32 target) {
        mTarget.x = target.x;
        mTarget.y = target.y;
        mTarget.z = target.z;
    }
    void SetDefaultTarget(); // Inline, in WeddingScene.cpp beside the constant
    void Restore() {
        SetPosition(gSceneMemory.mCameraPosition);
        SetTarget(gSceneMemory.mCameraTarget);
        VEC_Subtract(&mPosition, &mTarget, &mPosition);
    }

    /* 0x00 */ int mState;
    /* 0x04 */ Camera *mCamera;
    /* 0x08 */ WeddingCharacter *mFirst;
    /* 0x0C */ WeddingCharacter *mSecond;
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
}; // 0x28

// The two lights, which follow the camera and take the room's colour.
class WeddingLights {
public:
    void Init(Camera *camera, Room *room);
    void Update();
    void SetColor(u16 color);

    /* 0x00 */ int mState;
    /* 0x04 */ Light *mLights[2];
    /* 0x0C */ Camera *mCamera;
    /* 0x10 */ Room *mRoom;
}; // 0x14

class WeddingScene : public Scene {
public:
    enum State {
        STATE_ANNOUNCE,
        STATE_WAIT_ANNOUNCE,
        STATE_WAIT_THANKS,
        STATE_WAIT_PROMISE,
        STATE_SHOW_MARRIED,
        STATE_WAIT_GIFT,
        STATE_FADE_OUT,
        STATE_GIVE_ALBUM,
        STATE_FAREWELL,
        STATE_WAIT_FAREWELL,
        STATE_FADE_OUT_FAREWELL,
        STATE_LEAVE,
    };

    WeddingScene(); // func_02048600, the main program's

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void InitLowerScreen();
    void Init3d();
    u8 GetCharacterIndex(int slot);
    void SetSpeaker(int slot);

    /* 0x00010 */ int mCharOffset;
    /* 0x00014 */ int mUnknown14;
    /* 0x00018 */ int mDrawState;
    /* 0x0001C */ int mDrawFrames;
    /* 0x00020 */ int mState;
    /* 0x00024 */ int mFrame;
    /* 0x00028 */ Inventory mInventory;
    /* 0x01814 */ WeddingContext mContext;
    /* 0x01848 */ MessageFile mMessageFiles[4];
    /* 0x018A8 */ Room mRoom;
    /* 0x021B0 */ TtsPlayer mTtsPlayer;
    /* 0x16AF4 */ MessageText mSpeechText;
    /* 0x170E4 */ SpeechWindow mWindow;
    /* 0x176F0 */ WeddingCamera mCamera;
    /* 0x17718 */ WeddingLights mLights;
    /* 0x1772C */ WeddingCharacter mCharacters[2];
    /* 0x1CDB4 */ WeddingGiftModel mGift;
}; // 0x1CEE0

#endif // OV096_WEDDINGSCENE_H
