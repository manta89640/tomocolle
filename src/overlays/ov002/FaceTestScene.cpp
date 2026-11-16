#include "FaceTestScene.h"

// Overlay 2: one source file, FaceTestScene and its virtual table, with the scene's implicit destructor at the end
// of .text. The scene is a test of the Mii face on its own: a bare face (mFace, a CharacterBase without a head
// model), a head model with its face (mHead) and a pair of glasses. With A held the pad moves mFace on its head
// matrix; with B it rolls and pitches it, with Y it turns it; X with up and down picks a face kind that left or right
// gives SetFace, L the same for the way the head turns (mHeadTurn); with R held the pad moves the camera. Touching
// mFace gives it another of the default Miis.

void FaceTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Scn/Scene/SceneTestG3d_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TGH", "Mdl/Ica/head_ICA_LZ.bin", 0);
}

void FaceTestScene::Init() {
    func_0202b0e8(&data_02122824);
    ModelResources_SetModel(data_021223e4, 0, "TG3:/Glass6.imd");
    ModelResources_SetModel(data_021223e4, 1, "TGH:/head.imd");
    ModelResources_SetModel(data_021223e4, 1, "TGH:/wait00.ica");
    data_0211d604.Show(1);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknown48Changed = TRUE;
    data_0211d604.mUnknown44 = 0;
    data_0211d604.mUnknown44Changed = TRUE;
    mGlasses.Load(0, 0);
    mGlasses.mRotY = 0x6000;
    mGlasses.mPosition.y = -0x1199;
    mCamera = &data_02122764;
    func_0202a96c(&data_021227cc[0]);
    func_0202a96c(&data_021227cc[1]);
    func_0208fd90(&mFace, 0);
    mFaceKind = 0;
    mHeadTurn = 0;
    mFace.mHeadRoll = 0x2000;
    mHead.Init(1, 0);
    mHead.mHead.mPosition.x = FX32_ONE;
    mHead.SetAnimation(RESOURCE_BCA0, 0);
    ModelAnimation *anim = AnimModel_FindAnimation(&mHead.mHead, RESOURCE_BCA0, mHead.mAnimId);
    // Start the head's animation at a random frame. Written so: the count of frames goes through an int, shifted to
    // fx32 and back; passed to MATH_Rand16 directly (as FX_Whole(NNS_G3dAnmObjGetNumFrame(...)) too), the shifts fold
    int frames = (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << FX32_SHIFT) >> FX32_SHIFT;
    u16 frame = MATH_Rand16(&gRandom, frames);
    AnimModel_SetAnimationFrame(&mHead.mHead, frame << FX32_SHIFT, RESOURCE_BCA0, mHead.mAnimId);
}

// Sets the line from the camera through the touched point, which CharacterBase::HitTestFace() tests. The point goes
// by value through an inline: that is what leaves the original's dead copy of it on the stack.
static inline int SetTouchLine(ScreenPoint point) {
    return ModelResources_SetTouchLine(data_021223e4, point.x, point.y);
}

void FaceTestScene::Update() {
    if (data_0211f9d4.mHeld & PAD_BUTTON_A) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mFace.mFaceOffset.x += 0x40;
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mFace.mFaceOffset.x -= 0x40;
        }
        if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            mFace.mFaceOffset.y += 0x40;
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            mFace.mFaceOffset.y -= 0x40;
        }
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_B) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mFace.mHeadRollNow += 0xFF50;
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mFace.mHeadRollNow += 0xB0;
        }
        if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            mFace.mHeadPitchNow += 0xFF50;
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            mFace.mHeadPitchNow += 0xB0;
        }
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_Y) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mFace.mHeadYawNow += 0xFF60;
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mFace.mHeadYawNow += 0xA0;
        }
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_X) {
        if (data_0211f9d4.mTrigger & PAD_KEY_UP) {
            if (++mFaceKind >= 27) {
                mFaceKind = 0;
            }
        } else if (data_0211f9d4.mTrigger & PAD_KEY_DOWN) {
            if (--mFaceKind < 0) {
                mFaceKind = 26;
            }
        } else if (data_0211f9d4.mTrigger & PAD_KEY_RIGHT || data_0211f9d4.mTrigger & PAD_KEY_LEFT) {
            Character_SetFace(&mFace, mFaceKind);
        }
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_L) {
        if (data_0211f9d4.mTrigger & PAD_KEY_UP) {
            if (++mHeadTurn >= 8) {
                mHeadTurn = 0;
            }
        } else if (data_0211f9d4.mTrigger & PAD_KEY_DOWN) {
            if (--mHeadTurn < 0) {
                mHeadTurn = 7;
            }
        } else if (data_0211f9d4.mTrigger & PAD_KEY_RIGHT || data_0211f9d4.mTrigger & PAD_KEY_LEFT) {
            mFace.mHeadTurn = mHeadTurn;
        }
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_R) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            Camera_Translate(mCamera, 0, 0, 0x200);
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            Camera_Translate(mCamera, 0, 0, -0x200);
        }
        if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            Camera_Translate(mCamera, 0, 0x200, 0);
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            Camera_Translate(mCamera, 0, -0x200, 0);
        }
    }
    if (gTouchInput.mTouched) {
        SetTouchLine(gTouchInput.mPoint);
        if (mFace.HitTestFace()) {
            func_0208ff44(&mFace, MATH_Rand16(&gRandom, 6), TRUE);
        }
    }
    mFace.UpdateFace();
    mFace.UpdateHeadTurn();
}

void FaceTestScene::Draw() {
    mGlasses.Draw();
    func_02090214(&mFace);
    mHead.Draw();
}

void FaceTestScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    ArchiveManager_Unmount(&gArchiveManager, "TG3");
    ArchiveManager_Unmount(&gArchiveManager, "TGH");
}
