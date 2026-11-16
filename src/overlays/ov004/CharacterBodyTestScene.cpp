#include <nitro.h>
#include <nnsys.h>

#include "../../scene/MainProgram3d.h"

#include "CharacterBodyTestScene.h"

// The colour of light 0
static GXRgb sLightColor = GX_RGB(30, 0, 0);

// Where the turn test makes the character face
static const FloorPos sTurnTarget = {FX32_ONE, 0};

void CharacterBodyTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "CA3", "Scn/Scene/SceneTestG3d_3_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Mdl/Body/body000_3D_LZ.bin", 0);
}

// Init does not match: the light-0 block (x stored, reloaded and stored again, as in the main program's scenes 6, 7
// and 16 and overlay 3) differs by two stores in the original's order, x's before mEnabled's. Without the pragma the
// registers differ too (0 is loaded late, the colour and the reload go to r2 and ip); with it every register and
// instruction is the original's. Every order of the block's eight statements was tried with and without it, with
// dir.x stored back before mEnabled the allocation flips.
#pragma push
#pragma opt_propagation off
void CharacterBodyTestScene::Init() {
    func_0202b0e8(&data_02122824);
    gScreenControl.Show(1);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = TRUE;
    gScreenControl.mUnknown44 = 0;
    gScreenControl.mUnknown44Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 15, 0x7FFF, 63, FALSE);
    Light *light = &data_021227cc[0];
    u16 color = sLightColor;
    light->mDirection.x = 0xFFF;
    fx16 x = light->mDirection.x;
    light->mColor = color;
    light->mEnabled = TRUE;
    light->mDirection.x = x;
    light->mDirection.y = 0;
    light->mDirection.z = 0;
    ModelResources_SetModel(data_021223e4, 0, "TG3:/body.imd");
    mCharacter.Character::Init(0, 0, 0);
    mCharacter.mTurnJoints = TRUE;
    mCharacter.mTrackLimbs = TRUE;
    mCamera = &data_02122764;
    mCamera->mAngleY = 0x4000;
    mBodyStep = 0;
    mUnknown2B2C = 0;
    mTurnTest = TURN_TEST_IDLE;
    NNS_GfdDumpLnkTexVramManager();
    NNS_GfdDumpLnkPlttVramManager();
}

#pragma pop

// Sets the line from the camera through the touched point. The point goes by value through an inline: that is what
// leaves the original's dead copy of it on the stack.
static inline int SetTouchLine(ScreenPoint point) {
    return ModelResources_SetTouchLine(data_021223e4, point.x, point.y);
}

void CharacterBodyTestScene::Update() {
    if (gPadInput.mTrigger & PAD_BUTTON_B) {
        switch (++mBodyStep % 4) {
        case 0:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "TG3");
            ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Mdl/Body/body000_3D_LZ.bin", 1);
            ModelResources_SetModel(data_021223e4, 0, "TG3:/body.imd");
            mCharacter.CharacterModel::MakeModel(0, 0);
            break;
        case 1:
            func_02029d40(&mCharacter.mAnim, ArchiveManager_GetFileByName(&gArchiveManager, "TG3:/body_c1.ntfp"),
                          -1, -1);
            break;
        case 2:
            func_02029758(data_021223e4, RESOURCE_BMD0);
            ArchiveManager_Unmount(&gArchiveManager, "TG3");
            ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Mdl/Body/body001_3D_LZ.bin", 1);
            ModelResources_SetModel(data_021223e4, 0, "TG3:/body.imd");
            mCharacter.CharacterModel::MakeModel(0, 0);
            break;
        case 3:
            func_02029d40(&mCharacter.mAnim, ArchiveManager_GetFileByName(&gArchiveManager, "TG3:/body_c1.ntfp"),
                          -1, -1);
            break;
        }
    } else if (gPadInput.mHeld & PAD_BUTTON_A) {
        if (gPadInput.mHeld & PAD_KEY_RIGHT) {
            mCharacter.mAnim.mRotZ += 0x80;
        } else if (gPadInput.mHeld & PAD_KEY_LEFT) {
            mCharacter.mAnim.mRotX += 0x80;
        }
        if (gPadInput.mHeld & PAD_KEY_UP) {
            mCharacter.Character::Turn(0x100);
        } else if (gPadInput.mHeld & PAD_KEY_DOWN) {
            mCharacter.Character::Turn(0xFF00);
        }
    } else if (gPadInput.mHeld & PAD_BUTTON_X) {
        if (gPadInput.mHeld & PAD_KEY_RIGHT) {
            mCharacter.mScale.y += 0x40;
            mCharacter.mInvScale.y = FX_Inv(mCharacter.mScale.y);
        } else if (gPadInput.mHeld & PAD_KEY_LEFT) {
            mCharacter.mScale.y -= 0x40;
            mCharacter.mInvScale.y = FX_Inv(mCharacter.mScale.y);
        }
        if (gPadInput.mHeld & PAD_KEY_UP) {
            mCharacter.mScale.z += 0x40;
            mCharacter.mInvScale.z = FX_Inv(mCharacter.mScale.z);
        } else if (gPadInput.mHeld & PAD_KEY_DOWN) {
            mCharacter.mScale.z -= 0x40;
            mCharacter.mInvScale.z = FX_Inv(mCharacter.mScale.z);
        }
    } else if (gPadInput.mHeld & PAD_BUTTON_Y) {
        if (gPadInput.mHeld & PAD_KEY_RIGHT) {
            mCharacter.mPos.x += 0x40;
        } else if (gPadInput.mHeld & PAD_KEY_LEFT) {
            mCharacter.mPos.y += 0x40;
        }
        if (gPadInput.mHeld & PAD_KEY_UP) {
            mCharacter.mScale.x += 0x40;
            mCharacter.mInvScale.x = FX_Inv(mCharacter.mScale.x);
        } else if (gPadInput.mHeld & PAD_KEY_DOWN) {
            mCharacter.mScale.x -= 0x40;
            mCharacter.mInvScale.x = FX_Inv(mCharacter.mScale.x);
        }
    } else if (gPadInput.mHeld & PAD_BUTTON_R) {
        if (gPadInput.mHeld & PAD_KEY_RIGHT) {
            Camera_Translate(mCamera, 0x200, 0, 0);
        } else if (gPadInput.mHeld & PAD_KEY_LEFT) {
            Camera_Translate(mCamera, -0x200, 0, 0);
        }
        if (gPadInput.mHeld & PAD_KEY_UP) {
            Camera_Translate(mCamera, 0, 0x200, 0);
        } else if (gPadInput.mHeld & PAD_KEY_DOWN) {
            Camera_Translate(mCamera, 0, -0x200, 0);
        }
    } else if (gPadInput.mTrigger & PAD_BUTTON_L) {
        mTurnTest = TURN_TEST_START;
    } else {
        if (gPadInput.mHeld & PAD_KEY_RIGHT) {
            mCamera->mAngleY += 0x200;
        } else if (gPadInput.mHeld & PAD_KEY_LEFT) {
            mCamera->mAngleY -= 0x200;
        }
        if (gPadInput.mHeld & PAD_KEY_DOWN) {
            mCamera->mAngleX += 0x200;
        } else if (gPadInput.mHeld & PAD_KEY_UP) {
            mCamera->mAngleX -= 0x200;
        }
    }
    UpdateTurnTest();
    mCharacter.Character::Update();
    if (gTouchInput.mTouched) {
        SetTouchLine(gTouchInput.mPoint);
        mCharacter.HitTestJoint(CharacterModel::JOINT_BODY);
        mCharacter.HitTestJoint(mCamera->mEye, data_02122818, CharacterModel::JOINT_L_KNEE);
    }
}

void CharacterBodyTestScene::Draw() {
    mCharacter.Character::Draw();
}

void CharacterBodyTestScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    ArchiveManager_Unmount(&gArchiveManager, "TG3");
    ArchiveManager_Unmount(&gArchiveManager, "CA3");
}

// The angle of the way from where the character stands to a point, as overlays 74 and 96 have it for their
// characters (RoomCharacter::AngleTo): the inline's copies of the points are the original's dead stores
static inline u16 AngleTo(Character *character, FloorPos target) {
    FloorPos delta = target - character->GetFloorPos();
    return FX_Atan2Idx(delta.z, delta.x);
}

void CharacterBodyTestScene::UpdateTurnTest() {
    switch (mTurnTest) {
    case TURN_TEST_IDLE:
        break;
    case TURN_TEST_START:
        mTurnTest++;
        break;
    case TURN_TEST_AIM:
        mCharacter.mTargetAngle = AngleTo(&mCharacter, sTurnTarget);
        mTurnTest++;
        break;
    case TURN_TEST_TURN: {
        u16 angle = CharacterBase::StepAngle(mCharacter.mAngle, mCharacter.mTargetAngle, 3);
        mCharacter.Character::SetAngle(angle);
        if (angle == mCharacter.mTargetAngle) {
            mTurnTest++;
        }
        break;
    }
    }
}
