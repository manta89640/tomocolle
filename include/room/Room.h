#ifndef ROOM_ROOM_H
#define ROOM_ROOM_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"
#include "item/Item.h"

// A resident's room as the apartment scenes draw it (main program 0x020a3c68 to 0x020a5cc4, src/room/Room.cpp):
// its interior, which is an item the player owns, the model of the room with its door, and up to six pieces of
// furniture. The names are descriptive: no name of these classes survives in the ROM.

// One piece of furniture of an interior, as the interiors table places it
struct InteriorPiece {
    /* 0x0 */ u16 mName; // An index of data_021145a8, the pieces' model names
    /* 0x2 */ s16 mX;
    /* 0x4 */ s16 mY;
    /* 0x6 */ s16 mZ;
    /* 0x8 */ u16 mRotY;
}; // 0xA

// An entry of the interiors table (gInteriorTable, 78 entries)
struct InteriorInfo {
    /* 0x00 */ u32 mPrice : 24;
    /*      */ u32 mSortKey : 8;
    /* 0x04 */ u32 mWeight0 : 4; // The weights for the random choice, by gGameData.mUnknown4E[0]
    /*      */ u32 mWeight1 : 4;
    /*      */ u32 mWeight2 : 4;
    /*      */ u32 mWeight3 : 4;
    /*      */ u32 mTaste3 : 4; // How much each kind of personality likes it, 0 to 15 (GetInteriorLiking)
    /*      */ u32 mTaste1 : 4;
    /*      */ u32 mTaste2 : 4;
    /*      */ u32 mTaste0 : 4;
    /* 0x08 */ GXRgb mLightColor;
    /* 0x0A */ char mModelName[8]; // "rm63": "Mdl/Room/<name>_3D_LZ.bin"
    /* 0x12 */ u8 mPieceCount;
    /* 0x13 */ u8 mKind;
    /* 0x14 */ u8 mUnknown14;
    /* 0x15 */ u8 mSex; // 1: for men, 2: for women, as ClothesInfo::mSex
    /* 0x16 */ InteriorPiece mPieces[6];
}; // 0x54

extern "C" {
extern const InteriorInfo gInteriorTable[];
extern const char *data_021145a8[];
}

// An interior the player owns.
class InteriorItem : public Item {
public:
    InteriorItem();

    virtual int GetPrice() { return gInteriorTable[mId].mPrice; }
    virtual int GetPrice2() { return gInteriorTable[mId].mPrice; }
    virtual const char *GetModelName();
    virtual u8 GetWeight0() { return gInteriorTable[mId].mWeight0; }
    virtual u8 GetWeight1() { return gInteriorTable[mId].mWeight1; }
    virtual u8 GetWeight2() { return gInteriorTable[mId].mWeight2; }
    virtual u8 GetWeight3() { return gInteriorTable[mId].mWeight3; }

    void Settle();
    GXRgb GetLightColor();
    void GetLightRGB(int *r, int *g, int *b);
    void DimLight(int *r, int *g, int *b);
    u8 GetOpenSound();
    u8 GetCloseSound();
    u8 GetDoorSound(int which);
}; // 0x14

#define INTERIOR_COUNT 78

// The interiors the player owns, as the inventory has the goods.
class InteriorList {
public:
    void Load();
    void Save();
    void Sort();
    void SortHeldFirst();
    void GetOwned(Item **list, int *count);
    void GetOwnedKind16(Item **list, int *count);
    void GetOnOffer(Item **list, int *count);
    void GetOwnedFrom8(Item **list, int *count);
    void GetKind13(Item **list, int *count);
    BOOL HasAnyOwned();

    /* 0x000 */ int mCount;
    /* 0x004 */ InteriorItem mItems[INTERIOR_COUNT];
    /* 0x61C */ InteriorItem *mList[INTERIOR_COUNT];
}; // 0x754

extern "C" {
void Model_SetAlpha(Model *model, int alpha); // Sets a model's alpha (0 to 31)
int func_02029d9c(Model *model);
int func_02029dd0(Model *model);
}

// What the room's models have in common: the archive they come from and where they are.
class RoomObject {
public:
    RoomObject();

    void SetPosition(VecFx32 position) {
        mPosition.x = position.x;
        mPosition.y = position.y;
        mPosition.z = position.z;
    }

    void SetArchive(const char *archiveName, const char *modelName);
    void SetArchiveName(const char *archiveName);
    void SetModelName(const char *modelName);
    void Mount(int unk);
    void Unmount();
    void ReleaseModel();
    void Release();

    /* 0x00 */ int mResourceId; // -1 when there is none
    /* 0x04 */ VecFx32 mPosition;
    /* 0x10 */ fx32 mScale;
    /* 0x14 */ char mArchiveName[4];
    /* 0x18 */ char mArchivePath[0x1C]; // "Mdl/Room/<model>_3D_LZ.bin"; empty when it is not mounted
    /* 0x34 */ char mModelPath[0x18];   // "<archive>:/<model>.imd"
}; // 0x4C

// The model of the room itself, with its door. Its node description 0 gives the matrix the furniture is placed
// by, and its node "entrance" is shown while the door is open.
class RoomDoor : public RoomObject {
public:
    enum Motion {
        MOTION_NONE,
        MOTION_OPEN,  // Someone leaves
        MOTION_CLOSE, // Someone enters
    };

    RoomDoor();
    ~RoomDoor() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    void SetArchive(const char *archiveName, const char *modelName);
    void BindModels();
    void InitModel();
    void LoadModel();
    void Reload();
    void Update();
    void UpdateMotion();
    void Draw();
    void SetMotion(int motion);
    void PlayAnimation(u32 resType, int id, BOOL playOnce);
    void SetFlags(int flags);
    void SetCallbacks();
    static void OnNodeDesc(NNSG3dRS *rs);
    static void OnNode(NNSG3dRS *rs);
    static void InitCallbacks(NNSG3dRS *rs);

    /* 0x04C */ AnimModel mModel;
    /* 0x12C */ MtxFx33 mRootRotation; // Of node description 0, as the last drawing had it
    /* 0x150 */ MtxFx43 mRootMatrix;
    /* 0x180 */ BOOL mEntranceVisible;
    /* 0x184 */ int mMotion;
    /* 0x188 */ int mSoundPlayer;
    /* 0x18C */ s16 mOpenSound;  // An entry of sequence archive 0x1D, -1 for none
    /* 0x18E */ s16 mCloseSound;
    /* 0x190 */ s8 mMotionStep;
    /* 0x191 */ u8 mUnknown191[3];
    /* 0x194 */ char mAnimPath[0x10]; // "<archive>:/close.ica"
}; // 0x1A4

// A piece of furniture.
class RoomPart : public RoomObject {
public:
    enum State {
        STATE_IDLE,
        STATE_SHOW,     // 1: shown at once
        STATE_HIDE,     // 2: hidden at once
        STATE_OPAQUE,   // 3
        STATE_CLEAR,    // 4
        STATE_FADE_IN,  // 5
        STATE_FADE_OUT, // 6
    };

    RoomPart();

    void BindModel();
    void InitModel();
    void LoadModel();
    void Reload();
    void Update();
    void Draw(const MtxFx33 *rotation, const MtxFx43 *matrix);
    void SetFlags(int flags);
    void SetModelAlpha(int alpha) { Model_SetAlpha(&mModel, alpha); }
    int GetUnknown() { return func_02029d9c(&mModel); }
    int GetUnknown2() { return func_02029dd0(&mModel); }

    /* 0x04C */ AnimModel mModel;
    /* 0x12C */ int mState;
    /* 0x130 */ s8 mAlpha; // 0 to 31
}; // 0x134

#define ROOM_PART_COUNT 6

// The room and its furniture.
class RoomModels {
public:
    enum State {
        STATE_IDLE,
        STATE_SHOW_ALL,   // Show every piece
        STATE_HIDE_NEAR,  // Hide the pieces between the camera and the room at once
        STATE_FADE_NEAR,  // Fade them
    };

    RoomModels();

    void SetPartPosition(int i, VecFx32 position) {
        mParts[i].mPosition.x = position.x;
        mParts[i].mPosition.y = position.y;
        mParts[i].mPosition.z = position.z;
    }
    void SetPartRotY(int i, u16 rotY) { mParts[i].mModel.mRotY = rotY; }

    void SetArchive(const char *archiveName, const char *modelName);
    void SetModelName(const char *modelName);
    void Release();
    void LoadDoor(int resourceId);
    void ReloadDoor(int resourceId);
    void SetDoorUnknown(int value);
    void SetPartArchiveName(const char *archiveName, int i);
    void SetPartModelName(const char *modelName, int i);
    void LoadPart(int resourceId, int i);
    void ReloadPart(int resourceId, int i);
    void SetPartsUnknown(int value);
    void SetPartScales(fx32 scale);
    void Update();
    void Draw();
    void SetState(int state);
    void SetUnknown(int value);
    void SetAllUnknown(int value);
    void SetUnknown2(int value);
    void SetVisible(BOOL visible);
    void SetFlags(int flags);
    void SetAlpha(int alpha);
    int SumUnknown();
    int SumUnknown2();
    BOOL IsPartNear(fx32 distanceSq, int i);

    /* 0x000 */ RoomDoor mDoor;
    /* 0x1A4 */ RoomPart mParts[ROOM_PART_COUNT];
    /* 0x8DC */ void *mCamera; // The main program's camera, gCamera
    /* 0x8E0 */ int mState;
    /* 0x8E4 */ BOOL mVisible;
    /* 0x8E8 */ fx32 mNearY;
    /* 0x8EC */ fx32 mNearDistanceSq;
    /* 0x8F0 */ s16 mStep;
}; // 0x8F4

// A room of one interior.
class Room : public InteriorItem, public RoomModels {
public:
    Room();

    void SetArchive(const char *archiveName);
    void SetModel();
    void Mount(const char *archiveName, int unk);
    void MountModel(int unk);
    void ReleaseDoor();
    void Load(int resourceId);
    void Reload(int resourceId);
    void SetDoorSounds();
    void SetPartsArchive(const char *archiveName);
    void SetPartModels();
    void MountParts(int unk);
    void MountAll(const char *archiveName, int unk);
    void MountPartModels(int unk);
    void ReleaseParts();
    void LoadParts(int resourceId);
    void ReloadParts(int resourceId);
    void ReleaseAll();
    void Update();
    void SetScale(fx32 scale);
}; // 0x908

u8 ChooseInteriors(u8 *interiors, int count);

#endif // ROOM_ROOM_H
