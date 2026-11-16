#include "character/CharacterModel.h"

#include <nnsys/g3d/util.h>

struct ArchiveLoader;

extern "C" {
// STD_ConcatenateString's body, outside std_string.c (whose own copy the library unit links)

extern ArchiveLoader gArchiveManager;
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);

// The main program's model functions (0x02029b04 on), not decompiled yet
void Model_BindResource(AnimModel *model, u32 resType, int resourceId, int unk);   // Makes the model
void Model_QueuePalette(AnimModel *model, const void *pltt, int offset, int size); // Loads palette colours
int func_0202a1b0(AnimModel *model, u32 resType);                            // Counts its animations of a type
void func_0202a25c(AnimModel *model, u32 resType, int id);                   // Adds an animation
void AnimModel_RemoveAnimation(AnimModel *model, u32 resType, int id);                   // Removes an animation
void func_0202a3b0(AnimModel *model, u32 resType);                           // Removes the animations of a type
void AnimModel_SetAnimationFrame(AnimModel *model, fx32 frame, u32 resType, int id); // Sets an animation's frame
fx32 func_0202a5fc(AnimModel *model, u32 resType, int id);                   // An animation's frame
void RotateMtx33ByAngles(MtxFx33 *rotation, u16 x, u16 y, u16 z);                  // Turns a rotation by angles
BOOL LineMeetsSphere(const VecFx32 *origin, const VecFx32 *dir, fx32 radius);  // The line meets the sphere

// A line that HitTestJoint tests the joints against: from gCamera + 0x20, along gTouchLineDirection
extern VecFx32 gTouchLineDirection;
extern u8 gCamera[];
}

#define GET_JNTID(pMdl, pJntID, name) \
    (*(pJntID) = NNS_G3dGetNodeIdxByName(NNS_G3dGetNodeInfo(pMdl), (NNSG3dResName *)(name)))

static const char sRightHand[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "R_hand1";
static const char sLeftHand[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "L_hand1";
static const char sLeftAnkle[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "L_ankle";
static const char sNeck[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "neck";
static const char sBody[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "body";
static const char sLeftElbow[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "L_elbow";
static const char sRightKnee[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "R_knee";
static const char sLeftKnee[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "L_knee";
static const char sRightElbow[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "R_elbow";
static const char sShadowB[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "shadow";
static const char sShadowA[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "shadow";
static const char sRightAnkle[NNS_G3D_UTIL_RESNAME_LEN] ATTRIBUTE_ALIGN(4) = "R_ankle";

CharacterModel::CharacterModel() {
    STD_CopyString(mArchivePath, "Mdl/Body/body000_3D_LZ.bin");
    STD_CopyString(mModelPath, "XXX:/body.imd");
    STD_CopyString(mTexturePath, "XXX:/body_c0.ntfp");
}

void CharacterModel::Load(int resourceId, int unk) {
    mAnim.Load(resourceId, unk);
    mPos.x = 0;
    mPos.y = 0;
    mPos.z = 0;
    mAnim.mPosition.x = mPos.x;
    mAnim.mPosition.y = mPos.y;
    mAnim.mPosition.z = mPos.z;
    mAnim.mScale.x = FX32_ONE;
    mAnim.mScale.y = FX32_ONE;
    mAnim.mScale.z = FX32_ONE;
    mInvScale.x = mAnim.mScale.x;
    mInvScale.y = mAnim.mScale.y;
    mInvScale.z = mAnim.mScale.z;
    mScale.x = mInvScale.x;
    mScale.y = mInvScale.y;
    mScale.z = mInvScale.z;
    mTrackLimbs = FALSE;
    mTurnJoints = FALSE;
    MI_CpuClear16(mJointAngles, sizeof(mJointAngles));
    mAnimId = -1;
    mPrevAnimId = -1;
    mPatternAnimId = -1;
    mBlendFrames = 0;
    mShadowVisible = TRUE;
    mShadowAtHeight = FALSE;
    mShadowHeight = 0xCC;
    NNS_G3dRenderObjSetInitFunc(&mAnim.mRenderObj, InitCallbacks);
    mAnim.mRenderObj.ptrUser = this;
    for (int i = 0; i < JOINT_COUNT; i++) {
        MTX_Identity33(&mJointRotations[i]);
        MTX_Identity43(&mJointMatrices[i]);
    }
}

void CharacterModel::Draw() {
    static const VecFx32 sDrawOffset = {0, 0xCC, 0};

    UpdateBlend();
    VEC_Add(&mPos, &sDrawOffset, &mAnim.mPosition);
    mAnim.Draw();
}

void CharacterModel::Reload(int resourceId, int unk) {
    NNS_G3dRenderObjResetCallBack(&mAnim.mRenderObj);
    mAnim.RemoveAnims();
    Load(resourceId, unk);
    NNS_G3dRenderObjSetInitFunc(&mAnim.mRenderObj, InitCallbacks);
    mAnim.mRenderObj.ptrUser = this;
}

void CharacterModel::MakeBodyArchivePath() {
    STD_CopyString(mArchivePath, "Mdl/Body/body");
    STD_ConcatenateString(mArchivePath, mClothes.GetModelName());
    mTexturePath[11] = '0' + gClothesTable[mClothes.mId].mBodyColor;
}

void CharacterModel::SetArchive(const char *archiveName, int clothes, BOOL female) {
    STD_CopyString(mArchiveName, archiveName);
    mModelPath[0] = mArchiveName[0];
    mModelPath[1] = mArchiveName[1];
    mModelPath[2] = mArchiveName[2];
    MI_CpuCopy8(mArchiveName, mTexturePath, 3);
    SetClothes(clothes, female);
}

void CharacterModel::SetClothes(int clothes, BOOL female) {
    mClothes.mId = clothes;
    MakeBodyArchivePath();
    UseFemaleBody(female);
    STD_ConcatenateString(mArchivePath, "_3D_LZ.bin");
}

void CharacterModel::UseFemaleBody(BOOL female) {
    if (female == TRUE && (gClothesTable[mClothes.mId].mHasFemaleBody & 1)) {
        mArchivePath[16] = 'F';
        mArchivePath[17] = '\0';
    }
}

void CharacterModel::SetAnimFlags(BOOL playOnce, BOOL finished) {
    ModelAnimation *anim = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId);
    anim->mPlayOnce = playOnce;
    anim->mFinished = finished;
}

void CharacterModel::SetAnimFlagsAndFrame(BOOL playOnce, fx32 frame, BOOL finished) {
    ModelAnimation *anim = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId);
    anim->mPlayOnce = playOnce;
    anim->mAnmObj->frame = frame;
    anim->mFinished = finished;
}

void CharacterModel::MakeModel(int resourceId, int unk) {
    fx32 frame = func_0202a5fc(&mAnim, RESOURCE_BCA0, mAnimId);
    NNS_G3dRenderObjResetCallBack(&mAnim.mRenderObj);
    mAnim.RemoveAnims();
    Model_BindResource(&mAnim, RESOURCE_BMD0, resourceId, unk);
    if (mAnimId >= 0) {
        SetAnimation(RESOURCE_BCA0, mAnimId);
    }
    if (mPatternAnimId >= 0) {
        SetAnimation(RESOURCE_BTP0, mPatternAnimId);
    }
    NNS_G3dRenderObjSetInitFunc(&mAnim.mRenderObj, InitCallbacks);
    mAnim.mRenderObj.ptrUser = this;
    AnimModel_SetAnimationFrame(&mAnim, frame, RESOURCE_BCA0, mAnimId);
}

void CharacterModel::SetAnimation(u32 resType, int id) {
    if (resType == RESOURCE_BCA0) {
        if (id != mAnimId) {
            mPrevAnimId = mAnimId;
            mAnimId = id;
        }
        mBlendFrames = 0;
    } else if (resType == RESOURCE_BTP0) {
        mPatternAnimId = id;
    }
    mAnim.SetAnimation(resType, id);
}

void CharacterModel::BlendToAnimation(u32 resType, int id) {
    if (func_0202a1b0(&mAnim, resType) >= 3) {
        SetAnimation(resType, id);
        mBlendFrames = 0;
        return;
    }
    if (resType == RESOURCE_BCA0) {
        if (id == mAnimId) {
            return;
        }
        AnimModel_RemoveAnimation(&mAnim, RESOURCE_BCA0, mPrevAnimId);
        mPrevAnimId = mAnimId;
        mAnimId = id;
        if (mPrevAnimId >= 0) {
            mBlendFrames = FX32_CONST(12);
        } else {
            mBlendFrames = 0;
        }
        func_0202a25c(&mAnim, resType, id);
    } else {
        SetAnimation(resType, id);
    }
}

void CharacterModel::RemoveAnimation(u32 resType) {
    if (resType == RESOURCE_BCA0) {
        mAnimId = -1;
        mPrevAnimId = -1;
        mBlendFrames = 0;
    } else if (resType == RESOURCE_BTP0) {
        mPatternAnimId = -1;
    }
    func_0202a3b0(&mAnim, resType);
}

void CharacterModel::SetPaletteColor1(GXRgb color) {
    NNSG3dResTex *tex = mAnim.mResTex;
    GXRgb *pltt = (GXRgb *)((u8 *)tex + tex->plttInfo.ofsPlttData);
    pltt[1] = color;
    Model_QueuePalette(&mAnim, pltt, -1, 4);
}

void CharacterModel::LoadTexturePalette() {
    Model_QueuePalette(&mAnim, ArchiveManager_GetFileByName(&gArchiveManager, mTexturePath), -1, 0x20);
}

void CharacterModel::SetPaletteColors(GXRgb color0, GXRgb color1) {
    GXRgb *pltt = (GXRgb *)ArchiveManager_GetFileByName(&gArchiveManager, mTexturePath);
    pltt[0] = color0;
    pltt[1] = color1;
    Model_QueuePalette(&mAnim, pltt, -1, 0x20);
}

void CharacterModel::GetJointMatrices(int joint, MtxFx43 *matrix, MtxFx33 *rotation) {
    MI_Copy48B(&mJointMatrices[joint], matrix);
    MI_Copy36B(&mJointRotations[joint], rotation);
}

VecFx32 CharacterModel::GetJointPosition(int joint) {
    VecFx32 pos;
    pos.x = mJointMatrices[joint].m[3][0];
    pos.y = mJointMatrices[joint].m[3][1];
    pos.z = mJointMatrices[joint].m[3][2];
    return pos;
}

BOOL CharacterModel::HitTestJoint(int joint) {
    // The copies of the line are the by-value parameters of an inline
    return HitTestJoint(*(VecFx32 *)(gCamera + 0x20), gTouchLineDirection, joint);
}

BOOL CharacterModel::HitTestJoint(const VecFx32 *origin, const VecFx32 *dir, int joint) {
    fx32 radius;
    fx32 scale = mScale.x;

    switch (joint) {
    case JOINT_BODY:
        radius = 0xA66;
        if (scale > 0x1800) {
            scale = 0x1800;
        } else if (scale < 0xE66) {
            scale = 0xE66;
        }
        break;
    case JOINT_NECK:
        radius = 0x451;
        break;
    case JOINT_L_ELBOW:
    case JOINT_R_ELBOW:
        if (!mTrackLimbs) {
            return FALSE;
        }
        radius = 0x333;
        break;
    case JOINT_L_HAND:
    case JOINT_R_HAND:
        radius = 0x333;
        break;
    case JOINT_L_KNEE:
    case JOINT_R_KNEE:
        if (!mTrackLimbs) {
            return FALSE;
        }
        radius = 0x451;
        break;
    case JOINT_L_ANKLE:
    case JOINT_R_ANKLE:
        if (!mTrackLimbs) {
            return FALSE;
        }
        radius = 0x266;
        break;
    default:
        return FALSE;
    }

    VecFx32 delta;
    VecFx32 pos;
    pos = GetJointPosition(joint);
    VEC_Subtract(origin, &pos, &delta);
    return LineMeetsSphere(&delta, dir, FX_Mul(radius, scale));
}

void CharacterModel::UpdateBlend() {
    if (mBlendFrames <= 0) {
        return;
    }
    mBlendFrames -= MATH_ABS(mAnim.mUnknownD8);
    if (FX_Whole(mBlendFrames) <= 0) {
        mBlendFrames = 0;
    }
    ModelAnimation *prev = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mPrevAnimId);
    ModelAnimation *cur = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId);
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
        AnimModel_RemoveAnimation(&mAnim, RESOURCE_BCA0, mPrevAnimId);
    }
}

void CharacterModel::TurnJoint(NNSG3dRS *rs) {
    CharacterModel *model = (CharacterModel *)NNS_G3dRSGetRenderObj(rs)->ptrUser;
    int id = NNS_G3dRSGetCurrentNodeDescID(rs);
    NNSG3dJntAnmResult *result = rs->pJntAnmResult;
    u16 *angles;

    switch (id) {
    case 2:
        angles = model->mJointAngles[0];
        break;
    case 3:
        angles = model->mJointAngles[1];
        break;
    case 5:
        angles = model->mJointAngles[2];
        break;
    case 6:
        angles = model->mJointAngles[3];
        break;
    case 12:
        angles = model->mJointAngles[4];
        break;
    case 13:
        angles = model->mJointAngles[5];
        break;
    case 14:
        angles = model->mJointAngles[6];
        break;
    case 16:
        angles = model->mJointAngles[7];
        break;
    case 17:
        angles = model->mJointAngles[8];
        break;
    case 19:
        angles = model->mJointAngles[9];
        break;
    default:
        angles = NULL;
        break;
    }
    if (angles != NULL) {
        RotateMtx33ByAngles(&result->rot, angles[0], angles[1], angles[2]);
    }
}

void CharacterModel::OnNodeDesc(NNSG3dRS *rs) {
    CharacterModel *model = (CharacterModel *)NNS_G3dRSGetRenderObj(rs)->ptrUser;
    int id = NNS_G3dRSGetCurrentNodeDescID(rs);
    if (id < 1 || id >= 0x15) {
        return;
    }

    NNSG3dJntAnmResult *result = rs->pJntAnmResult;
    result->flag = (NNSG3dJntAnmResultFlag)(result->flag & ~NNS_G3D_JNTANM_RESULTFLAG_SCALE_ONE);
    result->flag = (NNSG3dJntAnmResultFlag)(result->flag & ~NNS_G3D_JNTANM_RESULTFLAG_SCALEEX0_ONE);
    if (id == 1) {
        VecFx32 offset;
        offset.z = 0;
        offset.y = 0;
        offset.x = FX_Mul(model->mScale.x - FX32_ONE, -result->trans.y);
        MTX_MultVec33(&offset, &result->rot, &offset);
        VEC_Add(&result->trans, &offset, &result->trans);
        VEC_Set(&result->scale, model->mScale.x, model->mScale.y, model->mScale.z);
        VEC_Set(&result->scaleEx0, model->mInvScale.x, model->mInvScale.y, model->mInvScale.z);
    } else if (id == 0x14) {
        result->scale.x = model->mScale.y;
        result->scale.y = model->mScale.x;
        result->scale.z = model->mScale.z;
        result->scaleEx0.x = model->mInvScale.y;
        result->scaleEx0.y = model->mInvScale.x;
        result->scaleEx0.z = model->mInvScale.z;
    } else {
        VEC_Set(&result->scale, model->mScale.x, model->mScale.y, model->mScale.z);
        VEC_Set(&result->scaleEx0, model->mInvScale.x, model->mInvScale.y, model->mInvScale.z);
    }
    if (model->mTurnJoints) {
        model->TurnJoint(rs);
    }
    NNS_G3dRSSetCallBack(rs, OnNodeDescDone, NNS_G3D_SBC_NODEDESC, NNS_G3D_SBC_CALLBACK_TIMING_C);
}

void CharacterModel::OnNodeDescDone(NNSG3dRS *rs) {
    int neck, leftHand, rightHand, body, shadow;
    int leftElbow, rightElbow, leftKnee, rightKnee, leftAnkle, rightAnkle;

    CharacterModel *model = (CharacterModel *)NNS_G3dRSGetRenderObj(rs)->ptrUser;
    int id = NNS_G3dRSGetCurrentNodeDescID(rs);

    GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &neck, sNeck);
    GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &leftHand, sLeftHand);
    GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &rightHand, sRightHand);
    GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &body, sBody);
    GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &shadow, sShadowB);

    if (id == neck) {
        NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_NECK], &model->mJointRotations[JOINT_NECK]);
    } else if (id == leftHand) {
        NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_L_HAND], &model->mJointRotations[JOINT_L_HAND]);
    } else if (id == rightHand) {
        NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_R_HAND], &model->mJointRotations[JOINT_R_HAND]);
    } else if (id == body) {
        NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_BODY], &model->mJointRotations[JOINT_BODY]);
    } else if (id == shadow) {
        if (model->mShadowAtHeight == TRUE) {
            MtxFx33 rotation;
            MtxFx43 matrix;
            NNS_G3dGetCurrentMtx(&matrix, &rotation);
            matrix.m[3][1] = model->mShadowHeight;
            NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
            NNS_G3dGeLoadMtx43((MtxFx43 *)&rotation);
            NNS_G3dGeMtxMode(GX_MTXMODE_POSITION);
            NNS_G3dGeLoadMtx43(&matrix);
            NNS_G3dGeMtxMode(GX_MTXMODE_POSITION_VECTOR);
        }
    } else if (model->mTrackLimbs == TRUE) {
        GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &leftElbow, sLeftElbow);
        GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &rightElbow, sRightElbow);
        GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &leftKnee, sLeftKnee);
        GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &rightKnee, sRightKnee);
        GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &leftAnkle, sLeftAnkle);
        GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &rightAnkle, sRightAnkle);
        if (id == leftKnee) {
            NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_L_KNEE], NULL);
        } else if (id == rightKnee) {
            NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_R_KNEE], NULL);
        } else if (id == leftAnkle) {
            NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_L_ANKLE], NULL);
        } else if (id == rightAnkle) {
            NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_R_ANKLE], NULL);
        } else if (id == leftElbow) {
            NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_L_ELBOW], NULL);
        } else if (id == rightElbow) {
            NNS_G3dGetCurrentMtx(&model->mJointMatrices[JOINT_R_ELBOW], NULL);
        }
    }

    NNS_G3dRSSetCallBack(rs, OnNodeDesc, NNS_G3D_SBC_NODEDESC, NNS_G3D_SBC_CALLBACK_TIMING_B);
    if (id == 0x14) {
        NNS_G3dRSResetCallBack(rs, NNS_G3D_SBC_NODEDESC);
    }
}

void CharacterModel::OnShadowNode(NNSG3dRS *rs) {
    int shadow;
    CharacterModel *model = (CharacterModel *)NNS_G3dRSGetRenderObj(rs)->ptrUser;

    GET_JNTID(NNS_G3dRSGetRenderObj(rs)->resMdl, &shadow, sShadowA);
    if (shadow == NNS_G3dRSGetCurrentNodeDescID(rs)) {
        NNS_G3dRSGetVisAnmResult(rs)->isVisible = model->mShadowVisible;
        NNS_G3dRSResetCallBack(rs, NNS_G3D_SBC_NODE);
    }
}

void CharacterModel::InitCallbacks(NNSG3dRS *rs) {
    NNS_G3dRSSetCallBack(rs, OnNodeDesc, NNS_G3D_SBC_NODEDESC, NNS_G3D_SBC_CALLBACK_TIMING_B);
    NNS_G3dRSSetCallBack(rs, OnShadowNode, NNS_G3D_SBC_NODE, NNS_G3D_SBC_CALLBACK_TIMING_B);
}

void CharacterModel::SetScaleFromBody(u32 height, u32 build) {
    fx32 tall = ((((height * 0x786) >> 7) + 0x666) * build >> 7) + ((height * 0x3AE) >> 7) + 0x666;
    fx32 width = ((height * 0xC51) >> 7) + 0x800;
    mScale.x = width;
    mInvScale.x = FX_Inv(width);
    mScale.y = tall;
    mInvScale.y = FX_Inv(tall);
    mScale.z = tall;
    mInvScale.z = FX_Inv(tall);
}

VecFx32 CharacterModel::GetHeldItemScale() {
    VecFx32 scale;
    fx32 x = mScale.x - FX_Mul(mScale.x, 0x199);
    scale.x = x;
    if (x > FX32_ONE) {
        x = FX32_ONE;
    } else if (x < 0x999) {
        x = 0x999;
    }
    fx32 y = mScale.y + FX_Mul(mScale.y, FX32_HALF);
    scale.x = x;
    scale.y = y;
    if (y > 0x2199) {
        y = 0x2199;
    } else if (y < 0x1199) {
        y = 0x1199;
    }
    scale.y = y;
    scale.z = scale.x;
    return scale;
}
