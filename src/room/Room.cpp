#include "room/Room.h"

#include "GameData.h"
#include "sound/SoundManager.h"

// A resident's room (main program 0x020a3c68 to 0x020a5cc4): the room's model with its door, its furniture, the
// interior items and the player's list of them, and the random choice of interiors on offer.

extern "C" {
extern u8 data_02122764[];
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
void ArchiveManager_MountCompressed(void *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(void *loader, const char *name);
extern u8 gArchiveManager[];

void func_02029c20(Model *model, u32 resType, int resourceId, int unk);
void func_02029e94(Model *model);
void func_02029f10(Model *model, int flags);
void func_02029f20(Model *model, int flags);
void func_02029f30(Model *model, int flags);
void func_02029f40(Model *model, int flags);
void func_02028b98(GXRgb color, int *r, int *g, int *b); // Splits a colour into its components

int func_0202c820(GameData *game, int interior); // The interior's count, -1 when it is not owned
void func_0202c834(GameData *game, int interior, s8 count);

extern void *data_02123440;
void Clock_GetDate(void *clock, RTCDate *date);
extern MATHRandContext16 gRandom;
}

#define SORT_BUFFER_SIZE 200

RoomObject::RoomObject() {
    mResourceId = -1;
    mArchiveName[0] = mArchivePath[0] = '\0';
    mPosition.x = 0;
    mPosition.y = 0;
    mPosition.z = 0;
    mScale = FX32_ONE;
}

void RoomObject::SetArchive(const char *archiveName, const char *modelName) {
    STD_CopyString(mArchiveName, archiveName);
    STD_CopyString(mModelPath, archiveName);
    STD_ConcatenateString(mModelPath, ":/");
    SetModelName(modelName);
}

void RoomObject::SetArchiveName(const char *archiveName) {
    STD_CopyString(mArchiveName, archiveName);
    STD_CopyString(mModelPath, archiveName);
    STD_ConcatenateString(mModelPath, ":/");
}

void RoomObject::SetModelName(const char *modelName) {
    STD_CopyString(mArchivePath, "Mdl/Room/");
    STD_ConcatenateString(mArchivePath, modelName);
    STD_ConcatenateString(mArchivePath, "_3D_LZ.bin");
    STD_CopyString(mModelPath + 5, modelName);
    STD_ConcatenateString(mModelPath + 5, ".imd");
}

void RoomObject::Mount(int unk) {
    if (mArchivePath[0] != '\0') {
        ArchiveManager_MountCompressed(gArchiveManager, mArchiveName, mArchivePath, unk);
    }
}

void RoomObject::Unmount() {
    if (mArchiveName[0] != '\0' && mArchivePath[0] != '\0') {
        ArchiveManager_Unmount(gArchiveManager, mArchiveName);
        mArchivePath[0] = '\0';
    }
}

void RoomObject::ReleaseModel() {
    if (mResourceId >= 0) {
        func_0202aecc(data_021223e4, mResourceId);
        mResourceId = -1;
    }
}

void RoomObject::Release() {
    ReleaseModel();
    Unmount();
}

RoomDoor::RoomDoor() {
    mEntranceVisible = FALSE;
    STD_CopyString(mAnimPath, "XXX:/close.ica");
    SetMotion(MOTION_NONE);
    mSoundPlayer = 1;
    mCloseSound = -1;
    mOpenSound = mCloseSound;
}

void RoomDoor::SetArchive(const char *archiveName, const char *modelName) {
    RoomObject::SetArchive(archiveName, modelName);
    MI_CpuCopy8(archiveName, mAnimPath, 3);
}

void RoomDoor::BindModels() {
    int id = mResourceId;
    ModelResources_SetModel(data_021223e4, id, mModelPath);
    ModelResources_SetModel(data_021223e4, id, mAnimPath);
}

void RoomDoor::InitModel() {
    SetCallbacks();
    MTX_Identity33(&mRootRotation);
    MTX_Identity43(&mRootMatrix);
}

void RoomDoor::LoadModel() {
    mModel.Load(mResourceId, 0);
    mModel.SetAnimation(RESOURCE_BCA0, 1);
    AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, 1)->mFinished = TRUE;
}

void RoomDoor::Reload() {
    NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj);
    mModel.RemoveAnims();
    func_02029c20(&mModel, RESOURCE_BMD0, mResourceId, 0);
    mModel.SetAnimation(RESOURCE_BCA0, 1);
    AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, 1)->mFinished = TRUE;
    mEntranceVisible = FALSE;
    SetCallbacks();
}

void RoomDoor::Update() {
    UpdateMotion();
}

void RoomDoor::UpdateMotion() {
    switch (mMotion) {
    case MOTION_NONE:
        return;
    case MOTION_OPEN:
        if (mMotionStep == 0) {
            mMotionStep++;
            PlayAnimation(RESOURCE_BCA0, 1, TRUE);
            mEntranceVisible = TRUE;
            if (mOpenSound >= 0) {
                gSoundManager.StartSeqArc(mSoundPlayer, 0x1D, mOpenSound);
            }
        } else if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, 1)->mFinished) {
            SetMotion(MOTION_NONE);
        }
        break;
    case MOTION_CLOSE:
        if (mMotionStep == 0) {
            mMotionStep++;
            PlayAnimation(RESOURCE_BCA0, 0, TRUE);
            mEntranceVisible = TRUE;
            if (mCloseSound >= 0) {
                gSoundManager.StartSeqArc(mSoundPlayer, 0x1D, mCloseSound);
            }
        } else if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, 0)->mFinished) {
            SetMotion(MOTION_NONE);
            mEntranceVisible = FALSE;
        }
        break;
    }
}

void RoomDoor::Draw() {
    mModel.SetPosition(mPosition.x, mPosition.y, mPosition.z);
    mModel.mScale.x = mModel.mScale.y = mModel.mScale.z = mScale;
    mModel.Draw();
}

void RoomDoor::SetMotion(int motion) {
    mMotion = motion;
    mMotionStep = 0;
}

void RoomDoor::PlayAnimation(u32 resType, int id, BOOL playOnce) {
    mModel.SetAnimation(resType, id);
    AnimModel_FindAnimation(&mModel, resType, id)->mFinished = FALSE;
    AnimModel_FindAnimation(&mModel, resType, id)->mPlayOnce = playOnce;
}

void RoomDoor::SetFlags(int flags) {
    func_02029f10(&mModel, flags);
    func_02029f20(&mModel, flags);
    func_02029f30(&mModel, flags);
    func_02029f40(&mModel, flags);
}

void RoomDoor::SetCallbacks() {
    NNS_G3dRenderObjSetInitFunc(&mModel.mRenderObj, InitCallbacks);
    mModel.mRenderObj.ptrUser = this;
}

void RoomDoor::OnNodeDesc(NNSG3dRS *rs) {
    RoomDoor *door = (RoomDoor *)NNS_G3dRSGetRenderObj(rs)->ptrUser;
    if (NNS_G3dRSGetCurrentNodeDescID(rs) == 0) {
        NNS_G3dGetCurrentMtx(&door->mRootMatrix, &door->mRootRotation);
        NNS_G3dRSResetCallBack(rs, NNS_G3D_SBC_NODEDESC);
    }
}

void RoomDoor::OnNode(NNSG3dRS *rs) {
    int id;
    RoomDoor *door = (RoomDoor *)NNS_G3dRSGetRenderObj(rs)->ptrUser;
    NNS_G3D_GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &id, "entrance");
    if (id == NNS_G3dRSGetCurrentNodeDescID(rs)) {
        NNS_G3dRSGetVisAnmResult(rs)->isVisible = door->mEntranceVisible;
        NNS_G3dRSResetCallBack(rs, NNS_G3D_SBC_NODE);
    }
}

void RoomDoor::InitCallbacks(NNSG3dRS *rs) {
    NNS_G3dRSSetCallBack(rs, OnNode, NNS_G3D_SBC_NODE, NNS_G3D_SBC_CALLBACK_TIMING_B);
    NNS_G3dRSSetCallBack(rs, OnNodeDesc, NNS_G3D_SBC_NODEDESC, NNS_G3D_SBC_CALLBACK_TIMING_C);
}

RoomPart::RoomPart() {
    mState = STATE_IDLE;
    mAlpha = 31;
}

void RoomPart::BindModel() {
    ModelResources_SetModel(data_021223e4, mResourceId, mModelPath);
}

void RoomPart::InitModel() {
}

void RoomPart::LoadModel() {
    mModel.Load(mResourceId, 0);
}

void RoomPart::Reload() {
    func_02029e94(&mModel);
    func_02029c20(&mModel, RESOURCE_BMD0, mResourceId, 0);
    mAlpha = 31;
}

void RoomPart::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW:
        mState = STATE_IDLE;
        mModel.mVisible = TRUE;
        break;
    case STATE_HIDE:
        mState = STATE_IDLE;
        mModel.mVisible = FALSE;
        break;
    case STATE_OPAQUE:
        mState = STATE_IDLE;
        mAlpha = 31;
        func_02029f50(&mModel, mAlpha);
        break;
    case STATE_CLEAR:
        mState = STATE_IDLE;
        mAlpha = 0;
        func_02029f50(&mModel, mAlpha);
        break;
    case STATE_FADE_IN:
        if (mAlpha < 31) {
            mAlpha += 4;
            if (mAlpha > 31) {
                mAlpha = 31;
            }
            func_02029f50(&mModel, mAlpha);
        }
        break;
    case STATE_FADE_OUT:
        if (mAlpha > 0) {
            mAlpha -= 4;
            if (mAlpha < 0) {
                mAlpha = 0;
            }
            func_02029f50(&mModel, mAlpha);
        }
        break;
    }
}

void RoomPart::Draw(const MtxFx33 *rotation, const MtxFx43 *matrix) {
    MTX_MultVec43(&mPosition, matrix, &mModel.mPosition);
    MTX_Copy33(rotation, &mModel.mRotation);
    mModel.mScale.x = mModel.mScale.y = mModel.mScale.z = mScale;
    mModel.Draw();
}

void RoomPart::SetFlags(int flags) {
    func_02029f10(&mModel, flags);
    func_02029f20(&mModel, flags);
    func_02029f30(&mModel, flags);
    func_02029f40(&mModel, flags);
}

RoomModels::RoomModels() {
    mCamera = data_02122764;
    mVisible = TRUE;
    SetState(STATE_IDLE);
    mNearY = 0x2000;
    mNearDistanceSq = 0x4000;
}

void RoomModels::SetArchive(const char *archiveName, const char *modelName) {
    mDoor.SetArchive(archiveName, modelName);
}

void RoomModels::SetModelName(const char *modelName) {
    mDoor.SetModelName(modelName);
}

void RoomModels::Release() {
    mDoor.mModel.RemoveAnims();
    mDoor.Release();
}

void RoomModels::LoadDoor(int resourceId) {
    mDoor.mResourceId = resourceId;
    mDoor.BindModels();
    mDoor.LoadModel();
    mDoor.InitModel();
}

void RoomModels::ReloadDoor(int resourceId) {
    mDoor.mResourceId = resourceId;
    mDoor.BindModels();
    mDoor.Reload();
}

void RoomModels::SetDoorUnknown(int value) {
    NNS_G3dMdlSetMdlPolygonIDAll(mDoor.mModel.mResMdl, value);
}

void RoomModels::SetPartArchiveName(const char *archiveName, int i) {
    mParts[i].SetArchiveName(archiveName);
}

void RoomModels::SetPartModelName(const char *modelName, int i) {
    mParts[i].SetModelName(modelName);
}

void RoomModels::LoadPart(int resourceId, int i) {
    mParts[i].mResourceId = resourceId;
    mParts[i].BindModel();
    mParts[i].LoadModel();
    mParts[i].InitModel();
}

void RoomModels::ReloadPart(int resourceId, int i) {
    mParts[i].mResourceId = resourceId;
    mParts[i].BindModel();
    mParts[i].Reload();
}

void RoomModels::SetPartsUnknown(int value) {
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            NNS_G3dMdlSetMdlPolygonIDAll(mParts[i].mModel.mResMdl, value + i);
        }
    }
}

void RoomModels::SetPartScales(fx32 scale) {
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        mParts[i].mScale = scale;
    }
}

// Not matching yet (98.8%): in STATE_FADE_NEAR the original multiplies z*z and x*x first and rounds x*x first
// (xx in r8, zz in r7, then IsPartNear(xx + zz)), as STATE_HIDE_NEAR does; here zz is rounded between the two
// products and the registers follow. Tried (miilist): the squares in either order, declared apart or together,
// const, as int/u32, through locals for diff.x/z, written out instead of FX_Mul, inside the call, both operand
// orders of the sum, diff and the squares at function scope, mStep read into a local.
void RoomModels::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW_ALL:
        SetState(STATE_IDLE);
        for (int i = 0; i < ROOM_PART_COUNT; i++) {
            if (mParts[i].mResourceId >= 0) {
                mParts[i].mState = RoomPart::STATE_OPAQUE;
            }
        }
        break;
    case STATE_HIDE_NEAR: {
        VecFx32 diff;
        VEC_Subtract((VecFx32 *)((u8 *)mCamera + 0x20), (VecFx32 *)((u8 *)mCamera + 0x2C), &diff);
        fx32 zz = FX_Mul(diff.z, diff.z);
        fx32 xx = FX_Mul(diff.x, diff.x);
        for (int i = 0; i < ROOM_PART_COUNT; i++) {
            if (mParts[i].mResourceId >= 0) {
                fx32 x = mParts[i].mPosition.x;
                fx32 z = mParts[i].mPosition.z;
                if (mParts[i].mPosition.y >= mNearY && FX_Mul(x, x) + FX_Mul(z, z) < mNearDistanceSq) {
                    mParts[i].mState = RoomPart::STATE_SHOW;
                } else if (IsPartNear(xx + zz, i)) {
                    mParts[i].mState = RoomPart::STATE_HIDE;
                } else {
                    mParts[i].mState = RoomPart::STATE_SHOW;
                }
            }
        }
        break;
    }
    case STATE_FADE_NEAR: {
        VecFx32 diff;
        VEC_Subtract((VecFx32 *)((u8 *)mCamera + 0x20), (VecFx32 *)((u8 *)mCamera + 0x2C), &diff);
        fx32 zz = FX_Mul(diff.z, diff.z);
        fx32 xx = FX_Mul(diff.x, diff.x);
        if (mStep < 2) {
            mStep++;
            for (int i = 0; i < ROOM_PART_COUNT; i++) {
                if (mParts[i].mResourceId >= 0) {
                    fx32 x = mParts[i].mPosition.x;
                    fx32 z = mParts[i].mPosition.z;
                    if (mParts[i].mPosition.y >= mNearY && FX_Mul(x, x) + FX_Mul(z, z) < mNearDistanceSq) {
                        mParts[i].mState = RoomPart::STATE_OPAQUE;
                    } else if (IsPartNear(zz + xx, i)) {
                        mParts[i].mState = RoomPart::STATE_CLEAR;
                    } else {
                        mParts[i].mState = RoomPart::STATE_OPAQUE;
                    }
                }
            }
        } else {
            for (int i = 0; i < ROOM_PART_COUNT; i++) {
                if (mParts[i].mResourceId >= 0) {
                    fx32 x = mParts[i].mPosition.x;
                    fx32 z = mParts[i].mPosition.z;
                    if (mParts[i].mPosition.y >= mNearY && FX_Mul(x, x) + FX_Mul(z, z) < mNearDistanceSq) {
                        mParts[i].mState = RoomPart::STATE_OPAQUE;
                    } else if (IsPartNear(zz + xx, i)) {
                        if (mParts[i].mState != RoomPart::STATE_FADE_OUT) {
                            mParts[i].mState = RoomPart::STATE_FADE_OUT;
                        }
                    } else if (mParts[i].mState != RoomPart::STATE_FADE_IN) {
                        mParts[i].mState = RoomPart::STATE_FADE_IN;
                    }
                }
            }
        }
        break;
    }
    }
    mDoor.Update();
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            mParts[i].Update();
        }
    }
}

void RoomModels::Draw() {
    if (!mVisible) {
        return;
    }
    if (mDoor.mResourceId >= 0) {
        mDoor.Draw();
    }
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            mParts[i].Draw(&mDoor.mRootRotation, &mDoor.mRootMatrix);
        }
    }
}

void RoomModels::SetState(int state) {
    mState = state;
    mStep = 0;
}

void RoomModels::SetUnknown(int value) {
    NNS_G3dMdlSetMdlPolygonIDAll(mDoor.mModel.mResMdl, value);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            NNS_G3dMdlSetMdlPolygonIDAll(mParts[i].mModel.mResMdl, value);
        }
    }
}

void RoomModels::SetAllUnknown(int value) {
    SetDoorUnknown(value);
    SetPartsUnknown(value + 1);
}

void RoomModels::SetUnknown2(int value) {
    NNS_G3dMdlSetMdlLightEnableFlagAll(mDoor.mModel.mResMdl, value);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            NNS_G3dMdlSetMdlLightEnableFlagAll(mParts[i].mModel.mResMdl, value);
        }
    }
}

void RoomModels::SetVisible(BOOL visible) {
    mVisible = visible;
}

void RoomModels::SetFlags(int flags) {
    mDoor.SetFlags(flags);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            mParts[i].SetFlags(flags);
        }
    }
}

void RoomModels::SetAlpha(int alpha) {
    func_02029f50(&mDoor.mModel, alpha);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            mParts[i].SetModelAlpha(alpha);
        }
    }
}

int RoomModels::SumUnknown() {
    int sum = func_02029d9c(&mDoor.mModel);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            sum += mParts[i].GetUnknown();
        }
    }
    return sum;
}

int RoomModels::SumUnknown2() {
    int sum = func_02029dd0(&mDoor.mModel);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        if (mParts[i].mResourceId >= 0) {
            sum += mParts[i].GetUnknown2();
        }
    }
    return sum;
}

// Whether a piece is nearer, on the floor, to the camera than the camera's target is
BOOL RoomModels::IsPartNear(fx32 distanceSq, int i) {
    VecFx32 pos = mParts[i].mModel.mPosition;
    fx32 dx = pos.x - ((VecFx32 *)((u8 *)mCamera + 0x20))->x;
    fx32 dz = pos.z - ((VecFx32 *)((u8 *)mCamera + 0x20))->z;
    BOOL near = FALSE;
    if (FX_Mul(dx, dx) + FX_Mul(dz, dz) < distanceSq) {
        near = TRUE;
    }
    return near;
}

InteriorItem::InteriorItem() {
    mId = 0;
}

void InteriorItem::Settle() {
    Item::Settle();
}

GXRgb InteriorItem::GetLightColor() {
    int r, g, b;
    GetLightRGB(&r, &g, &b);
    return GX_RGB(r, g, b);
}

void InteriorItem::GetLightRGB(int *r, int *g, int *b) {
    func_02028b98(data_020f72f8[mId].mLightColor, r, g, b);
    DimLight(r, g, b);
}

void InteriorItem::DimLight(int *r, int *g, int *b) {
    *r = *r * 3 >> 3;
    *g = *g * 3 >> 3;
    *b = *b * 7 >> 4;
}

u8 InteriorItem::GetOpenSound() {
    return GetDoorSound(0);
}

u8 InteriorItem::GetCloseSound() {
    return GetDoorSound(1);
}

u8 InteriorItem::GetDoorSound(int which) {
    // The door's sounds of each kind of room, opening and closing
    static const u8 sDoorSounds[8][2] = {
        {0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}, {14, 15},
    };
    int kind;
    switch (mId) {
    case 0x09:
    case 0x3B:
        kind = 1;
        break;
    case 0x0E:
    case 0x13:
        kind = 2;
        break;
    case 0x19:
    case 0x2D:
        kind = 3;
        break;
    case 0x1B:
        kind = 4;
        break;
    case 0x31:
        kind = 5;
        break;
    case 0x44:
    case 0x47:
        kind = 6;
        break;
    case 0x46:
        kind = 7;
        break;
    default:
        kind = 0;
        break;
    }
    return sDoorSounds[kind][which];
}

void InteriorList::Load() {
    int n = 0;
    mCount = n;
    for (int i = 0; i < INTERIOR_COUNT; i++, n++) {
        mItems[i].Clear();
        mItems[i].mId = i;
        BOOL owned = TRUE;
        int count = func_0202c820(&gGameData, i);
        if (count == -1) {
            owned = FALSE;
            count = 0;
        }
        mItems[i].mOwned = owned;
        mItems[i].mUnknown0C = count;
        mItems[i].mCount = count;
    }
    mCount = n;
    for (int i = 0; i < mCount; i++) {
        mItems[i].Settle();
        mList[i] = &mItems[i];
    }
}

void InteriorList::Save() {
    for (int i = 0; i < mCount; i++) {
        mItems[i].Settle();
        u16 id = mItems[i].mId;
        if (mItems[i].mOwned != FALSE) {
            func_0202c834(&gGameData, id, mItems[i].mCount);
        } else if (mItems[i].mOwned == FALSE) {
            func_0202c834(&gGameData, id, -1);
        }
    }
}

static s32 CompareInteriors(void *a, void *b);
static s32 CompareInteriorsHeldFirst(void *a, void *b);

void InteriorList::Sort() {
    u32 buffer[SORT_BUFFER_SIZE];
    MATH_QSort(mList, mCount, sizeof(InteriorItem *), CompareInteriors, buffer);
}

void InteriorList::SortHeldFirst() {
    u32 buffer[SORT_BUFFER_SIZE];
    MATH_QSort(mList, mCount, sizeof(InteriorItem *), CompareInteriorsHeldFirst, buffer);
}

static s32 CompareInteriors(void *a, void *b) {
    return data_020f72f8[(*(InteriorItem **)a)->mId].mSortKey - data_020f72f8[(*(InteriorItem **)b)->mId].mSortKey;
}

static s32 CompareInteriorsHeldFirst(void *a, void *b) {
    BOOL heldA = FALSE;
    BOOL heldB = FALSE;
    if ((*(InteriorItem **)a)->mCount > 0) {
        heldA = TRUE;
    }
    if ((*(InteriorItem **)b)->mCount > 0) {
        heldB = TRUE;
    }
    if (heldA != heldB) {
        return -(heldA - heldB);
    }
    return CompareInteriors(a, b);
}

void InteriorList::GetOwned(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InteriorItem *item = mList[i];
        if (item->mOwned) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

void InteriorList::GetOwnedKind16(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InteriorItem *item = mList[i];
        if (data_020f72f8[item->mId].mKind == 16) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

void InteriorList::GetOnOffer(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        BOOL ok = TRUE;
        ok &= data_020f72f8[mList[i]->mId].mKind != 0;
        ok &= data_020f72f8[mList[i]->mId].mKind != 13;
        ok &= data_020f72f8[mList[i]->mId].mKind != 16;
        BOOL ok2 = TRUE;
        if (!mList[i]->mOwned && data_020f72f8[mList[i]->mId].mKind != 15) {
            ok2 = FALSE;
        }
        if (ok & ok2) {
            list[n] = mList[i];
            (*count)++;
            n++;
        }
    }
}

void InteriorList::GetOwnedFrom8(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InteriorItem *item = mList[i];
        if (item->mOwned && item->mId > 7) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

void InteriorList::GetKind13(Item **list, int *count) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        InteriorItem *item = mList[i];
        if (data_020f72f8[item->mId].mKind == 13) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

BOOL InteriorList::HasAnyOwned() {
    BOOL result = FALSE;
    for (int i = 0; i < mCount; i++) {
        if (mList[i]->mOwned) {
            result = TRUE;
            break;
        }
    }
    return result;
}

Room::Room() {
}

void Room::SetArchive(const char *archiveName) {
    RoomModels::SetArchive(archiveName, GetModelName());
}

const char *InteriorItem::GetModelName() {
    return data_020f72f8[mId].mModelName;
}

void Room::SetModel() {
    RoomModels::SetModelName(GetModelName());
}

void Room::Mount(const char *archiveName, int unk) {
    SetArchive(archiveName);
    mDoor.Mount(unk);
}

void Room::MountModel(int unk) {
    SetModel();
    mDoor.Mount(unk);
}

void Room::ReleaseDoor() {
    RoomModels::Release();
}

void Room::Load(int resourceId) {
    LoadDoor(resourceId);
    SetDoorSounds();
}

void Room::Reload(int resourceId) {
    ReloadDoor(resourceId);
    SetDoorSounds();
}

void Room::SetDoorSounds() {
    mDoor.mOpenSound = GetOpenSound();
    mDoor.mCloseSound = GetCloseSound();
}

void Room::SetPartsArchive(const char *archiveName) {
    char name[4];
    MI_CpuCopy8(archiveName, name, 4);
    for (int i = 0; i < ROOM_PART_COUNT; i++) {
        name[2] = '0' + i;
        SetPartArchiveName(name, i);
    }
    SetPartModels();
}

void Room::SetPartModels() {
    for (u32 i = 0; i < data_020f72f8[mId].mPieceCount; i++) {
        SetPartModelName(data_021145a8[data_020f72f8[mId].mPieces[i].mName], i);
    }
}

void Room::MountParts(int unk) {
    for (int i = 0; i < data_020f72f8[mId].mPieceCount; i++) {
        mParts[i].Mount(unk);
    }
}

void Room::MountAll(const char *archiveName, int unk) {
    SetPartsArchive(archiveName);
    MountParts(unk);
}

void Room::MountPartModels(int unk) {
    SetPartModels();
    MountParts(unk);
}

void Room::ReleaseParts() {
    for (int i = 0; i < data_020f72f8[mId].mPieceCount; i++) {
        mParts[i].Release();
    }
}

void Room::LoadParts(int resourceId) {
    for (u32 i = 0; i < data_020f72f8[mId].mPieceCount; i++) {
        LoadPart(resourceId, i);
        VecFx32 pos;
        pos.x = data_020f72f8[mId].mPieces[i].mX;
        pos.y = data_020f72f8[mId].mPieces[i].mY;
        pos.z = data_020f72f8[mId].mPieces[i].mZ;
        SetPartPosition(i, pos);
        SetPartRotY(i, data_020f72f8[mId].mPieces[i].mRotY);
        resourceId++;
    }
}

void Room::ReloadParts(int resourceId) {
    for (u32 i = 0; i < data_020f72f8[mId].mPieceCount; i++) {
        ReloadPart(resourceId, i);
        VecFx32 pos;
        pos.x = data_020f72f8[mId].mPieces[i].mX;
        pos.y = data_020f72f8[mId].mPieces[i].mY;
        pos.z = data_020f72f8[mId].mPieces[i].mZ;
        SetPartPosition(i, pos);
        SetPartRotY(i, data_020f72f8[mId].mPieces[i].mRotY);
        resourceId++;
    }
}

void Room::ReleaseAll() {
    ReleaseDoor();
    ReleaseParts();
}

void Room::Update() {
    RoomModels::Update();
}

void Room::SetScale(fx32 scale) {
    mDoor.mScale = scale;
    SetPartScales(scale);
}

// The weight of an interior in the random choice
static int GetInteriorWeight(u16 interior) {
    int weight = 0;
    switch (gGameData.mUnknown4E[0]) {
    case 0:
        weight = data_020f72f8[interior].mWeight0;
        break;
    case 1:
        weight = data_020f72f8[interior].mWeight1;
        break;
    case 2:
        weight = data_020f72f8[interior].mWeight2;
        break;
    case 3:
        weight = data_020f72f8[interior].mWeight3;
        break;
    }
    return weight;
}

static s32 CompareInteriorIds(void *a, void *b);

// Chooses count different interiors of kind 14 at random by their weights, sorts them and returns the first
u8 ChooseInteriors(u8 *interiors, int count) {
    RTCDate date;
    BOOL free[INTERIOR_COUNT];
    BOOL ofKind[INTERIOR_COUNT];
    u32 buffer[SORT_BUFFER_SIZE];
    if (gGameData.mUnknownB2[0] < 1) {
        gGameData.mUnknownB2[0] = 1;
    }
    Clock_GetDate(data_02123440, &date);
    for (int i = 0; i < INTERIOR_COUNT; i++) {
        free[i] = TRUE;
        ofKind[i] = data_020f72f8[i].mKind == 14;
    }
    for (int n = 0; n < count; n++) {
        int total = 0;
        for (int i = 0; i < INTERIOR_COUNT; i++) {
            if (free[i] && ofKind[i]) {
                total += GetInteriorWeight(i);
            }
        }
        u16 choice = MATH_Rand16(&gRandom, total);
        u8 i = 0;
        int sum = i;
        for (; i < INTERIOR_COUNT; i++) {
            if (free[i] && ofKind[i]) {
                sum += GetInteriorWeight(i);
            }
            if (choice < sum) {
                break;
            }
        }
        interiors[n] = i;
        free[i] = FALSE;
    }
    MATH_QSort(interiors, count, sizeof(u8), CompareInteriorIds, buffer);
    return interiors[0];
}

static s32 CompareInteriorIds(void *a, void *b) {
    return *(u8 *)a - *(u8 *)b;
}
