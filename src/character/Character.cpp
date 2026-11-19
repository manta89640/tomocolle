#include "character/Character.h"

#include "sound/SoundManager.h"

extern "C" {
// The Mii library (lib/NFL)
int NFL_ResetFace(int index);
int NFL_RemoveFaceSlot(void *removed);
int NFL_AddFaceSlot(void);
int NFL_AddFace(void *face);
int NFL_FindResetFace(void);
int NFL_MakeFace(int index, const void *mii, u32 partDepth);
int NFL_SetExpression(int index, int kind);
int NFL_GetExpression(int index);
int NFL_CopyDefaultMii(void *mii, u32 index);
int NFL_MakeRandomMii(void *mii, int sex, int b, int c);
int NFL_SetFaceMatrices(int index, const MtxFx43 *mtx43, const MtxFx33 *mtx33);
int NFL_SetFaceDrawParam(int index, const FaceDrawParam *param);
int NFL_DrawFace(int index, BOOL withSecond);
GXRgb NFL_GetFavoriteColor(int index);
}

extern "C" {
// The Mii data (lib/MiiData)
int MiiData_GetSex(CharacterRecord *character);
int MiiData_GetFavoriteColor(CharacterRecord *character);
}

extern "C" {
// The main program's model functions (0x02029b04 on), not decompiled yet
void AnimModel_RemoveAnimation(AnimModel *model, u32 resType, int id);  // Removes an animation
void AnimModel_SetSpeed(AnimModel *model, fx32 speed);           // Sets the speed of its animations
void ModelResources_BeginDraw(u32 *resources);
void RotateMtx33ByAngles(MtxFx33 *rotation, u16 x, u16 y, u16 z); // Turns a rotation by angles
BOOL LineMeetsSphere(const VecFx32 *origin, const VecFx32 *dir, fx32 radius);

extern MATHRandContext16 gRandom;

// A line that HitTestFace tests the face against: from gCamera + 0x20, along gTouchLineDirection
extern VecFx32 gTouchLineDirection;
extern u8 gCamera[];
}

static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// --- CharacterBase ---

CharacterBase::CharacterBase() {
    mFaceId = -1;
}

CharacterBase::~CharacterBase() {
    if (mFaceId >= 0) {
        NFL_ResetFace(mFaceId);
        NFL_RemoveFaceSlot(NULL);
    }
}

void CharacterBase::InitFace(u32 defaultMii) {
    ResetFace();
    if (mFaceId == -1) {
        AddFace();
    }
    SetDefaultMii(defaultMii, TRUE);
}

void CharacterBase::ResetFace() {
    mFaceVisible = TRUE;
    mExpression = 0;
    SetFace(0);
    mHeadTurn = 0;
    mHeadTurnShift = 3;
    mFaceRoll = mHeadPitch = mHeadYaw = mHeadRoll = mHeadPitchNow = mHeadYawNow = mHeadRollNow = 0;
    mFaceOffset.x = 0;
    mFaceOffset.y = 0;
    mFaceOffset.z = 0;
    mFaceScale.x = FX32_ONE;
    mFaceScale.y = FX32_ONE;
    mFaceScale.z = FX32_ONE;
    MTX_Identity33(&mFaceRotation);
    MTX_Identity33(&mHeadRotation);
    MTX_Identity43(&mHeadMatrix);
    MTX_Identity43(&mFaceMatrix);
    mFaceDrawParam.light = 3;
    mFaceDrawParam.polyMode = GX_POLYGONMODE_MODULATE;
    mFaceDrawParam.polygonID = 1;
    mFaceDrawParam.alpha = 31;
    mFaceDrawParam.misc = 0x2000;
    mFaceDrawParam.diffuse = GX_RGB(31, 31, 31);
    mFaceDrawParam.ambient = GX_RGB(8, 8, 8);
    mFaceDrawParam.specular = 0;
    mFaceDrawParam.emission = 0;
}

void CharacterBase::AddFace() {
    NFL_AddFaceSlot();
    NFL_AddFace(mFace);
    mFaceId = NFL_FindResetFace();
}

void CharacterBase::SetMiiParameter(int param, int value, BOOL makeFace) {
    SetParameter(param, value);
    if (makeFace) {
        MakeFace();
    }
}

void CharacterBase::MakeFace() {
    NFL_MakeFace(mFaceId, this, 0);
    NFL_SetExpression(mFaceId, mExpression);
}

void CharacterBase::SetExpression(int expression) {
    mExpression = expression;
    NFL_SetExpression(mFaceId, expression);
}

void CharacterBase::SetDefaultMii(u32 index, BOOL makeFace) {
    NFL_CopyDefaultMii(this, index);
    if (makeFace) {
        MakeFace();
    }
}

void CharacterBase::SetRandomMii(int sex, int unk1, int unk2, BOOL makeFace) {
    int result = NFL_MakeRandomMii(this, sex, unk1, unk2);
    if (makeFace && result == 0) {
        MakeFace();
    }
}

void CharacterBase::SetHeadMatrices(const MtxFx43 *matrix, const MtxFx33 *rotation) {
    MI_Copy48B(matrix, &mHeadMatrix);
    MI_Copy36B(rotation, &mHeadRotation);
}

BOOL CharacterBase::HitTestFace() {
    // The copies of the line are the by-value parameters of an inline
    return HitTestFace(*(VecFx32 *)(gCamera + 0x20), gTouchLineDirection);
}

BOOL CharacterBase::HitTestFace(const VecFx32 *origin, const VecFx32 *dir) {
    static const VecFx32 sFaceCenter = {0, 0, 0};
    VecFx32 delta;
    VecFx32 center;

    MTX_MultVec43(&sFaceCenter, &mFaceMatrix, &center);
    VEC_Subtract(origin, &center, &delta);
    return LineMeetsSphere(&delta, dir, 0xF0A);
}

void CharacterBase::DrawFace() {
    MtxFx33 rotation;
    MtxFx44 clipMatrix;
    MtxFx33 vectorMatrix;

    if (!mFaceVisible) {
        return;
    }
    while (G3X_IsGeometryBusy()) {
    }
    G3_PushMtx();
    G3_Identity();
    {
        fx32 x = mHeadMatrix.m[3][0];
        fx32 y = mHeadMatrix.m[3][1];
        fx32 z = mHeadMatrix.m[3][2];
        G3_Translate(x, y, z);
    }
    G3_MultMtx33(&mHeadRotation);
    rotation = mFaceRotation;
    G3_Scale(mFaceScale.x, mFaceScale.y, mFaceScale.z);
    G3_Translate(mFaceOffset.x, mFaceOffset.y, mFaceOffset.z);
    RotateMtx33ByAngles(&rotation, mHeadPitchNow, mHeadYawNow, (u16)(mHeadRollNow + mFaceRoll));
    G3_MultMtx33(&rotation);
    G3_MtxMode(GX_MTXMODE_PROJECTION);
    G3_PushMtx();
    G3_Identity();
    while (G3X_GetClipMtx(&clipMatrix)) {
    }
    MTX_Copy44To43(&clipMatrix, &mFaceMatrix);
    while (G3X_GetVectorMtx(&vectorMatrix)) {
    }
    NFL_SetFaceMatrices(mFaceId, &mFaceMatrix, &vectorMatrix);
    G3_PopMtx(1);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_PopMtx(1);
    NNS_G3dGlbFlushVP();
    NFL_SetFaceDrawParam(mFaceId, &mFaceDrawParam);
    NFL_DrawFace(mFaceId, TRUE);
}

void CharacterBase::Draw() {
    ModelResources_BeginDraw(gModelResources);
    DrawFace();
}

void CharacterBase::SetFace(int kind) {
    mFaceKind = kind;
    mFaceStep = 0;
    mFaceTimer = 0;
    mFaceTimerStep = FX32_ONE;
}

void CharacterBase::UpdateBlink(int expression) {
    mFaceTimer += mFaceTimerStep;
    s16 step = mFaceStep;
    if (step == 0) {
        mFaceStep = 1;
        SetExpression(expression);
    } else if (step == 1) {
        if (Rand(0) < 300) {
            mFaceTimer = 0;
            mFaceStep++;
            SetExpression(5);
        }
    } else if (step == 2) {
        if (mFaceTimer >= FX32_CONST(6)) {
            mFaceTimer -= FX32_CONST(6);
            SetExpression(expression);
            mFaceStep = 1;
        }
    }
}

void CharacterBase::UpdateBlinkOrWink(int expression, fx32 closedTime) {
    mFaceTimer += mFaceTimerStep;
    s16 step = mFaceStep;
    if (step == 0) {
        mFaceStep = 1;
        SetExpression(0);
    } else if (step == 1) {
        int random = Rand(0);
        if (random < 300) {
            mFaceTimer = 0;
            mFaceStep++;
            SetExpression(expression);
        } else if (random < 500) {
            mFaceTimer = closedTime >> 1;
            mFaceStep++;
            SetExpression(5);
        }
    } else if (step == 2) {
        if (mFaceTimer >= closedTime) {
            mFaceTimer -= closedTime;
            SetExpression(0);
            mFaceStep = 3;
        }
    } else if (step == 3) {
        if (mFaceTimer >= FX32_CONST(60)) {
            mFaceTimer = 0;
            mFaceStep = 1;
        }
    }
}

void CharacterBase::UpdateRandomExpressions(int expression0, int expression1, int expression2, int expression3,
                                            fx32 time1, fx32 time2, int chance) {
    s16 step = mFaceStep;
    if (step == 0) {
        mFaceStep = 1;
        SetExpression(expression0);
    } else if (step == 1) {
        if (Rand(0) < chance) {
            mFaceTimer = time1 + Rand(16) * 0x800;
            mFaceStep++;
            SetExpression(expression1);
        }
    } else if (step == 2) {
        mFaceTimer -= mFaceTimerStep;
        if (mFaceTimer <= 0) {
            mFaceTimer = time2 + Rand(4) * 0x800;
            if (Rand(2) == 0) {
                SetExpression(expression2);
            } else {
                SetExpression(expression3);
            }
            mFaceStep++;
        }
    } else {
        mFaceTimer -= mFaceTimerStep;
        if (mFaceTimer <= 0) {
            mFaceStep = 1;
        }
    }
}

void CharacterBase::UpdateFace() {
    switch (mFaceKind) {
    case 1:
        mFaceKind = 0;
        SetExpression(0);
        break;
    case 2:
        mFaceKind = 0;
        SetExpression(1);
        break;
    case 3:
        mFaceKind = 0;
        SetExpression(2);
        break;
    case 4:
        mFaceKind = 0;
        SetExpression(3);
        break;
    case 5:
        mFaceKind = 0;
        SetExpression(4);
        break;
    case 6:
        mFaceKind = 0;
        SetExpression(5);
        break;
    case 7:
        mFaceKind = 0;
        SetExpression(6);
        break;
    case 8:
        UpdateBlink(0);
        break;
    case 9:
        UpdateBlink(1);
        break;
    case 10:
        UpdateBlinkOrWink(1, FX32_CONST(20));
        break;
    case 11:
        UpdateBlinkOrWink(2, FX32_CONST(20));
        break;
    case 12:
        UpdateBlinkOrWink(3, FX32_CONST(20));
        break;
    case 13:
        UpdateBlink(4);
        break;
    case 14:
        UpdateRandomExpressions(0, 0, 0, 1, FX32_CONST(8), FX32_CONST(2), 5000);
        break;
    case 15:
        UpdateRandomExpressions(0, 0, 0, 2, FX32_CONST(8), FX32_CONST(2), 5000);
        break;
    case 16:
        UpdateRandomExpressions(0, 0, 0, 3, FX32_CONST(8), FX32_CONST(2), 5000);
        break;
    case 17:
        UpdateRandomExpressions(0, 0, 0, 4, FX32_CONST(8), FX32_CONST(2), 5000);
        break;
    case 18:
        UpdateRandomExpressions(0, 0, 0, 5, FX32_CONST(8), FX32_CONST(2), 5000);
        break;
    case 19:
        UpdateRandomExpressions(6, 6, 6, 1, FX32_CONST(8), FX32_CONST(2), 5000);
        break;
    case 20:
        if (mFaceStep == 0) {
            mFaceStep = 1;
            SetExpression(0);
        }
        mFaceTimer += mFaceTimerStep;
        if (mFaceTimer < FX32_CONST(8)) {
            break;
        }
        mFaceTimer -= FX32_CONST(8);
        if (NFL_GetExpression(mFaceId) == 0) {
            SetExpression(6);
        } else {
            SetExpression(0);
        }
        break;
    case 21:
        UpdateRandomExpressions(0, 6, 0, 0, FX32_CONST(6), FX32_CONST(1), 4000);
        break;
    case 22:
        UpdateRandomExpressions(0, 6, 0, 1, FX32_CONST(6), FX32_CONST(3), 4000);
        break;
    case 23:
        UpdateRandomExpressions(0, 6, 0, 2, FX32_CONST(5), FX32_CONST(3), 5000);
        break;
    case 24:
        UpdateRandomExpressions(0, 6, 3, 3, FX32_CONST(6), FX32_CONST(4), 3000);
        break;
    case 25:
        UpdateRandomExpressions(0, 3, 6, 3, FX32_CONST(10), FX32_CONST(1), 8000);
        break;
    case 26:
        UpdateRandomExpressions(0, 6, 0, 4, FX32_CONST(5), FX32_CONST(3), 5000);
        break;
    }
}

void CharacterBase::LipSync(const char *phoneme, int face) {
    BOOL open;
    char c = phoneme[0];
    if (c == 'a') {
        open = TRUE;
    } else if (c == 'i') {
        open = TRUE;
    } else if (c == 'u') {
        open = TRUE;
    } else if (c == 'e') {
        open = TRUE;
    } else if (c == 'o') {
        open = TRUE;
    } else if (c == 'N') {
        open = FALSE;
    } else if (c == 'w') {
        open = TRUE;
    } else {
        open = FALSE;
    }
    mFaceTimer++;
    if (open) {
        SetFace(7);
    } else {
        int closedFrames = mFaceTimer;
        if (closedFrames > 10) {
            SetFace(face);
        } else {
            SetFace(1);
        }
        mFaceTimer = closedFrames;
    }
}

void CharacterBase::UpdateHeadTurn() {
    switch (mHeadTurn) {
    case 1: {
        u16 pitch = StepAngle(mHeadPitchNow, mHeadPitch, mHeadTurnShift);
        u16 yaw = StepAngle(mHeadYawNow, mHeadYaw, mHeadTurnShift);
        u16 roll = StepAngle(mHeadRollNow, mHeadRoll, mHeadTurnShift);
        mHeadPitchNow = pitch;
        mHeadYawNow = yaw;
        mHeadRollNow = roll;
        if (pitch == mHeadPitch && yaw == mHeadYaw && roll == mHeadRoll) {
            mHeadTurn = 0;
        }
        break;
    }
    case 2: {
        u16 pitch = StepAngle(mHeadPitchNow, mHeadPitch, mHeadTurnShift);
        mHeadPitchNow = pitch;
        if (pitch == mHeadPitch) {
            mHeadTurn = 0;
        }
        break;
    }
    case 3: {
        u16 yaw = StepAngle(mHeadYawNow, mHeadYaw, mHeadTurnShift);
        mHeadYawNow = yaw;
        if (yaw == mHeadYaw) {
            mHeadTurn = 0;
        }
        break;
    }
    case 4: {
        u16 roll = StepAngle(mHeadRollNow, mHeadRoll, mHeadTurnShift);
        mHeadRollNow = roll;
        if (roll == mHeadRoll) {
            mHeadTurn = 0;
        }
        break;
    }
    case 5:
    case 6:
    case 7:
        break;
    }
}

BOOL CharacterBase::ClampHeadAngle(u16 *angle, int limit) {
    BOOL inRange = TRUE;
    int value = *angle;
    if (value > 0x8000) {
        value -= 0x10000;
        if (value < -limit) {
            value = -limit;
            inRange = FALSE;
        }
    } else if (value > limit) {
        value = limit;
        inRange = FALSE;
    }
    *angle = value;
    return inRange;
}

u16 CharacterBase::StepAngle(u16 angle, u16 target, int shift) {
    int from = angle;
    int to = target;
    if ((u32)from > (u32)to) {
        if (from - to > 0x8000) {
            from -= 0x10000;
        }
    } else if (to - from > 0x8000) {
        to -= 0x10000;
    }
    int diff = to - from;
    int step = diff >> shift;
    if (MATH_ABS(step) < 0x20) {
        if (diff < 0) {
            step = -0x20;
        } else {
            step = 0x20;
        }
    }
    from += step;
    if (MATH_ABS(to - from) < 0x50) {
        from = to;
    }
    return from;
}

// --- CharacterHead ---

void CharacterHead::Init(int resourceId, u32 defaultMii) {
    InitFace(defaultMii);
    mHead.Load(resourceId, 0);
    mHead.mPosition.x = 0;
    mHead.mPosition.y = 0;
    mHead.mPosition.z = 0;
    mHead.mScale.x = FX32_ONE;
    mHead.mScale.y = FX32_ONE;
    mHead.mScale.z = FX32_ONE;
    mPrevAnimId = mAnimId = -1;
    mBlendFrames = 0;
    mAnimSpeed = FX32_ONE;
    NNS_G3dRenderObjSetInitFunc(&mHead.mRenderObj, InitCallbacks);
    mHead.mRenderObj.ptrUser = this;
    MTX_Identity33(&mRootRotation);
    MTX_Identity43(&mRootMatrix);
}

void CharacterHead::Update() {
    UpdateHeadTurn();
    UpdateFace();
}

void CharacterHead::Draw() {
    UpdateBlend();
    mHead.Draw();
    SetHeadMatrices(&mRootMatrix, &mRootRotation);
    CharacterBase::Draw();
}

fx32 CharacterBase::GetAnimSpeedOf(int liveliness) {
    return liveliness * 0x28 + 0xB54;
}

void CharacterHead::SetAnimSpeed(fx32 speed) {
    mAnimSpeed = speed;
    AnimModel_SetSpeed(&mHead, speed);
}

void CharacterHead::SetAnimation(u32 resType, int id) {
    if (resType == RESOURCE_BCA0) {
        mPrevAnimId = mAnimId;
        mAnimId = id;
    }
    mHead.SetAnimation(resType, id);
    AnimModel_SetSpeed(&mHead, mAnimSpeed);
}

void CharacterHead::UpdateBlend() {
    if (mBlendFrames <= 0) {
        return;
    }
    mBlendFrames -= MATH_ABS(mHead.mUnknownD8);
    if (FX_Whole(mBlendFrames) <= 0) {
        mBlendFrames = 0;
    }
    ModelAnimation *prev = AnimModel_FindAnimation(&mHead, RESOURCE_BCA0, mPrevAnimId);
    ModelAnimation *cur = AnimModel_FindAnimation(&mHead, RESOURCE_BCA0, mAnimId);
    if (prev != NULL && cur != NULL) {
        NNSG3dAnmObj *prevObj = prev->mAnmObj;
        NNSG3dAnmObj *curObj = cur->mAnmObj;
        if (prevObj != NULL && curObj != NULL) {
            fx32 ratio = FX_Whole(mBlendFrames) * 0x155;
            prevObj->ratio = ratio;
            curObj->ratio = FX32_ONE - ratio;
        }
    }
    if (mBlendFrames == 0) {
        AnimModel_RemoveAnimation(&mHead, RESOURCE_BCA0, mPrevAnimId);
    }
}

void CharacterHead::OnNode(NNSG3dRS *rs) {
    CharacterHead *head = (CharacterHead *)NNS_G3dRSGetRenderObj(rs)->ptrUser;
    if (NNS_G3dRSGetCurrentNodeDescID(rs) == 0) {
        NNS_G3dGetCurrentMtx(&head->mRootMatrix, &head->mRootRotation);
        NNS_G3dRSResetCallBack(rs, NNS_G3D_SBC_NODEDESC);
    }
}

void CharacterHead::InitCallbacks(NNSG3dRS *rs) {
    NNS_G3dRSSetCallBack(rs, OnNode, NNS_G3D_SBC_NODE, NNS_G3D_SBC_CALLBACK_TIMING_C);
}

// --- Character ---

void Character::Init(int resourceId, int unk, u32 defaultMii) {
    CharacterModel::Load(resourceId, unk);
    InitFace(defaultMii);
    mFaceRoll = 0xC000;
    mSpeed = 0;
    mWalkSpeedScale = FX32_ONE;
    mTurnSpeedScale = FX32_ONE;
    mMoveAngle = mAngle = mTargetAngle = mWalkAngle = 0;
    mAnimSpeed = FX32_ONE;
    ResetSoundPhases();
}

void Character::SetArchive(const char *archiveName) {
    CharacterModel::SetArchive(archiveName, mClothesId, MiiData_GetSex(this));
}

void Character::UpdateClothes() {
    CharacterModel::SetClothes(mClothesId, MiiData_GetSex(this));
}

void Character::Update() {
    UpdateHeadTurn();
    UpdateFace();
    mAnim.mRotY = 0x4000 - mAngle;
}

void Character::UpdateSpeedScales() {
    UpdateWalkSpeedScale();
    UpdateTurnSpeedScale();
}

void Character::UpdateWalkSpeedScale() {
    int height = GetParameter(0x29);
    mWalkSpeedScale = height * 0x16 + FX32_ONE;
    mWalkSpeedScale = FX_Mul(mWalkSpeedScale, mAnimSpeed);
}

void Character::UpdateTurnSpeedScale() {
    int height = GetParameter(0x29);
    mTurnSpeedScale = height * 3 + 0x1199;
    mTurnSpeedScale = FX_Mul(mTurnSpeedScale, mAnimSpeed);
}

fx32 Character::ScaleBySpeed(fx32 value) {
    return FX_Mul(value, mWalkSpeedScale);
}

BOOL Character::WalkToTarget(fx32 reach, fx32 speed, int turnShift) {
    FloorPos delta = GetTargetFloorPos() - GetFloorPos();
    fx32 distanceSq = LengthSq(delta);
    mWalkAngle = FX_Atan2Idx(delta.z, delta.x);
    fx64 diff = mWalkAngle - mMoveAngle;
    if (diff > 0x8000) {
        diff -= 0x10000;
    } else if (diff < -0x8000) {
        diff += 0x10000;
    }
    diff = (diff * mTurnSpeedScale) >> FX32_SHIFT;
    int step = diff >> turnShift;
    mMoveAngle += (u16)step;
    mAngle += (u16)step;
    mTurnStep = step;
    if (distanceSq < reach) {
        mSpeed = 2;
        return TRUE;
    }
    mSpeed = FX_Mul(speed, mWalkSpeedScale);
    return FALSE;
}

// The direction from the character to a point (the copies of the points it leaves on the stack are the original's)
static inline u16 AngleTo(const Character *character, FloorPos target) {
    FloorPos delta = target - character->GetFloorPos();
    return FX_Atan2Idx(delta.z, delta.x);
}

BOOL Character::TurnToWalkTarget(int turnSpeed, int walkSpeed) {
    FloorPos target = GetTargetFloorPos();
    mWalkAngle = AngleTo(this, target);
    return TurnTo(turnSpeed, walkSpeed, mWalkAngle);
}

BOOL Character::TurnToTargetAngle(int turnSpeed, int walkSpeed) {
    return TurnTo(turnSpeed, walkSpeed, mTargetAngle);
}

BOOL Character::TurnTo(int turnSpeed, int walkSpeed, u16 angle) {
    int diff = angle - mAngle;
    if (diff > 0x8000) {
        diff -= 0x10000;
    } else if (diff < -0x8000) {
        diff += 0x10000;
    }
    int step = FX_Mul(turnSpeed, mTurnSpeedScale);
    if (diff < 0) {
        step = -step;
    }
    mMoveAngle += (u16)step;
    mAngle += (u16)step;
    int rest = diff - step;
    mTurnStep = step;
    if (diff == 0 || rest == 0 || diff == rest || (diff > 0 && rest < 0) || (diff < 0 && rest > 0)) {
        mSpeed = 0;
        if (mMoveAngle == mAngle) {
            SetAngle(angle);
        }
        return TRUE;
    }
    mSpeed = FX_Mul(walkSpeed, mWalkSpeedScale);
    return FALSE;
}

void Character::MoveAlongAngle() {
    u16 angle = mMoveAngle;
    mPos.x += FX_Mul(mSpeed, FX_CosIdx(angle));
    mPos.z += FX_Mul(mSpeed, FX_SinIdx(angle));
}

void Character::SetWalkTarget(VecFx32 target) {
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
}

void Character::SetAngle(u16 angle) {
    mMoveAngle = mAngle = angle;
}

void Character::Turn(u16 delta) {
    mMoveAngle += delta;
    mAngle += delta;
}

// Walks a step away from a point: the target is one unit from where the character stands, away from the point
void Character::SetWalkTargetAwayFrom(FloorPos pos) {
    FloorPos dir = Normalize(pos - GetFloorPos());
    FloorPos target = GetFloorPos() - dir;
    mTarget.x = target.x;
    mTarget.z = target.z;
}

// The same, with the direction to the point turned by its own angle less mMoveAngle first
void Character::SetWalkTargetReflected(FloorPos pos) {
    FloorPos delta = pos - GetFloorPos();
    u16 angle = Angle(delta) - mMoveAngle;
    FloorPos dir = Normalize(Rotate(delta, angle));
    FloorPos target = GetFloorPos() - dir;
    mTarget.x = target.x;
    mTarget.z = target.z;
}

void Character::Draw() {
    CharacterModel::Draw();
    SetHeadMatrices(&mJointMatrices[JOINT_NECK], &mJointRotations[JOINT_NECK]);
    CharacterBase::Draw();
}

void Character::SetAnimSpeed(fx32 speed) {
    mAnimSpeed = speed;
    AnimModel_SetSpeed(&mAnim, speed);
}

void Character::SetAnimation(u32 resType, int id) {
    CharacterModel::SetAnimation(resType, id);
    AnimModel_SetSpeed(&mAnim, mAnimSpeed);
}

void Character::BlendToAnimation(u32 resType, int id) {
    CharacterModel::BlendToAnimation(resType, id);
    AnimModel_SetSpeed(&mAnim, mAnimSpeed);
}

void Character::SetVisible(BOOL visible) {
    mAnim.mVisible = visible;
    mFaceVisible = visible;
}

void Character::UpdateBodyScale() {
    int build = GetParameter(0x2A);
    SetScaleFromBody(GetParameter(0x29), build);
}

fx32 Character::GetAnimSpeed() {
    return GetAnimSpeedOf(GetAnswerScore01());
}

void Character::UpdateAnimSpeed() {
    SetAnimSpeed(GetAnimSpeed());
}

void Character::UpdateBodyColors() {
    GXRgb favorite = NFL_GetFavoriteColor(MiiData_GetFavoriteColor(this));
    SetPaletteColors(favorite, GetFaceColor());
}

void Character::SetLight(int light) {
    mFaceDrawParam.light = light;
    NNS_G3dMdlSetMdlLightEnableFlagAll(mAnim.mResMdl, light);
}

void Character::ResetSoundPhase(int slot) {
    mSoundPhase[slot] = 0;
}

void Character::ResetSoundPhases() {
    MI_CpuFill8(mSoundPhase, 0, sizeof(mSoundPhase));
}

int Character::PlaySoundOnce(int player, int index, int seqArc) {
    return PlaySoundAtAnimPhase(0, player, seqArc, index, 10);
}

// The point at a distance in a direction on the floor
static inline FloorPos Polar(fx32 distance, u16 angle) {
    FloorPos offset;
    offset.x = FX_Mul(distance, FX_CosIdx(angle));
    offset.z = FX_Mul(distance, FX_SinIdx(angle));
    return offset;
}

// The point at a distance in front of the character. `result` is declared first and assigned (not initialised): the
// assignment's temporary and the variable are the two copies of the result at the top of the original's frame.
FloorPos Character::GetPointInFront(fx32 distance) {
    FloorPos result;
    u16 angle = -mAnim.mRotY;
    FloorPos offset = Polar(distance, angle);
    result = GetFloorPos() - offset;
    return result;
}

BOOL Character::IsBeside(fx32 x, fx32 z, fx32 width) {
    FloorPos offset;
    offset.x = x - mPos.x;
    offset.z = z - mPos.z;
    FloorPos rel = Rotate(offset, mAnim.mRotY);
    if (rel.z >= 0 || (rel.z < 0 && MATH_ABS(rel.x) > FX_Mul(MATH_ABS(rel.z), width))) {
        return TRUE;
    }
    return FALSE;
}

BOOL Character::IsInFront(fx32 x, fx32 z) {
    FloorPos offset;
    offset.x = x - mPos.x;
    offset.z = z - mPos.z;
    FloorPos rel = Rotate(offset, mAnim.mRotY);
    return rel.z > 0;
}

BOOL Character::IsAhead(fx32 x, fx32 z, fx32 width) {
    FloorPos offset;
    offset.x = x - mPos.x;
    offset.z = z - mPos.z;
    FloorPos rel = Rotate(offset, mAnim.mRotY);
    if (rel.z > 0 && MATH_ABS(rel.x) < FX_Mul(MATH_ABS(rel.z), width)) {
        return TRUE;
    }
    return FALSE;
}

BOOL Character::IsPointWithinReach(fx32 x, fx32 z, fx32 reach, fx32 maxReach) {
    fx32 scale = mScale.z;
    if (scale < FX32_ONE) {
        scale = FX32_ONE;
    }
    fx32 distance = FX_Mul(reach, scale);
    if (maxReach > 0 && maxReach < distance) {
        distance = maxReach;
    }
    fx32 dz = z - mPos.z;
    fx32 dx = x - mPos.x;
    return FX_Mul(dx, dx) + FX_Mul(dz, dz) < FX_Mul(distance, distance);
}

BOOL Character::IsNearerThan(FloorPos pos, fx32 distanceSq) {
    FloorPos delta = pos - GetFloorPos();
    return LengthSq(delta) < distanceSq;
}

// Plays a sound at the quarters of the animation that `quarters` has bits for, one step per call; returns the step
// passed (-1 for none). The phase is read through a const reference and stepped through the array: the compiler does
// not take a value read through a const lvalue for the same expression as the non-const one, so the step reads the
// byte again, as the original does.
int Character::PlaySoundAtAnimPhase(int slot, int player, int seqArc, int index, int quarters) {
    s8 *phases = mSoundPhase;
    const s8 &phase = phases[slot];
    int result = -1;
    ModelAnimation *anim = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId);
    if (anim == NULL) {
        return result;
    }
    if (anim->mFinished) {
        return result;
    }
    NNSG3dAnmObj *anmObj = anim->mAnmObj;
    BOOL play = FALSE;
    fx32 length = ((NNSG3dResAnmCommon *)anmObj->resAnm)->numFrame << FX32_SHIFT;
    if (phase == 0) {
        if (anmObj->frame >= 0) {
            result = phase;
            if (quarters & 1) {
                play = TRUE;
            }
            phases[slot]++;
        }
    } else if (phase == 1) {
        if (anmObj->frame >= length >> 2) {
            result = phase;
            if (quarters & 2) {
                play = TRUE;
            }
            phases[slot]++;
        }
    } else if (phase == 2) {
        if (anmObj->frame >= length >> 1) {
            result = phase;
            if (quarters & 4) {
                play = TRUE;
            }
            phases[slot]++;
        }
    } else if (phase == 3) {
        if (anmObj->frame >= (length * 3) >> 2) {
            result = phase;
            if (quarters & 8) {
                play = TRUE;
            }
            phases[slot]++;
        }
    } else if (phase == 4) {
        if (anmObj->frame + (fx32)anim->mUnknown10 >= length) {
            result = phase;
            if (quarters & 0x10) {
                play = TRUE;
            }
            phases[slot] = 0;
        }
    } else {
        phases[slot] = 0;
    }
    if (play) {
        gSoundManager.StartSeqArc(player, seqArc, index);
    }
    return result;
}
