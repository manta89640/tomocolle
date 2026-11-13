#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"

#include "ApartmentFront.h"
#include "HeldItem.h"
#include "Clock.h"
#include "Room.h"
#include "RoomCamera.h"
#include "RoomCharacter.h"
#include "RoomLight.h"
#include "SkyBackground.h"
#include "SpeechBalloon.h"
#include "TimedMessage.h"
#include "TouchButton.h"

extern "C" {

// --- The main program's, not decompiled or not named yet ---

// The archive manager (gArchiveManager): mounts archives under a name and loads them in the background
extern u32 gArchiveManager[];
// Whether no archive is being loaded
BOOL ArchiveManager_IsIdle(void *manager);
// Unmounts the archive of that name
void ArchiveManager_Unmount(void *manager, const char *name);
// Starts loading the file at "path" as the archive "name"
void ArchiveManager_MountCompressed(void *manager, const char *name, const char *path, int unk);

// The offset (the plane's number shifted by 11) of a plane's screen in VRAM
int func_020253d4(void *graphicsManager, int plane);
// Sets the scroll of the planes of the mask in the scroll table data_02121834
void func_02028154(void *scroll, int planes, int y);
extern u32 data_02121834[];

// Splits a 15-bit colour
void func_02028b98(int color, int *r, int *g, int *b);
// Resets a light: direction straight down... colour white, enabled
void func_0202a96c(Light *light);
// Sets a light's direction: the vector, rotated by the three angles and normalized
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
extern Light data_021227cc[];

// The saved game: what is known of a character by its index
extern u32 gGameData[];
int func_0202c468(void *settings, int character);
u8 func_0202c53c(void *settings, int character);
int func_0202c5b4(void *settings, int character);
int func_0202c58c(void *settings, int character);
BOOL func_02099c94(int unk);
struct Unknown02114d78Entry {
    u32 mUnknown0 : 8;
    int mUnknown4;
    int mUnknown8;
};
extern const Unknown02114d78Entry *const data_02114d78[];

// The time-of-day tracker
void func_020a612c(DayPhase *phase, RTCDate *date, RTCTime *time);
void func_020a61cc(DayPhase *phase, RTCDate *date, RTCTime *time);
int func_020a6214(DayPhase *phase);

// Queues a character file for transfer to VRAM
void func_02028f24(void *queue, const char *path, int plane, int unk3, int unk4, int unk5, int unk6);
extern u32 gVramTransferQueue[];
// Loads a palette file; sets colours of a loaded palette
void *func_02027534(void *palettes, const char *path, int plane, int unk3, int unk4, int unk5);
void func_020278f8(void *palettes, u16 color, int unk);
extern u32 data_021213b8[];

// The current time with the game's offset applied, as seconds and as date and time
s64 func_02035870(int offset, RTCDate *date, RTCTime *time);
extern int data_02123440[];

// Display state: bit 1 of the first word is set while a TimedMessage is shown
extern u32 data_0211d604[];

// The game's random number generator
extern MATHRandContext16 gRandom;

} // extern "C"

// --- HeldItem ---

// By kind: whether the model has a texture pattern animation, and the digits of its archive
static const u8 sHeldItemHasPattern[12] = { FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE };
static const char *sHeldItemDigits[11] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10" };

// @0x02169dcc
HeldItem::HeldItem() {
    SetShowState(SHOW_IDLE);
    SetLoadState(LOAD_IDLE);
    STD_CopyString(mArchivePath, "Mdl/Goods/gd00_3D_LZ.bin");
    mKind = 0;
}

// @0x02169e4c
void HeldItem::SetArchive(const char *name, int kind) {
    STD_CopyString(mArchiveName, name);
    mKind = kind;
    MI_CpuCopy8(sHeldItemDigits[kind & 0xFF], &mArchivePath[12], 2);
}

// @0x02169e84
void HeldItem::Init(int resourceId) {
    mResourceId = resourceId;
    AnimModel::Load(resourceId, 0);
    StartPattern();
    mVisible = FALSE;
}

// @0x02169eac
void HeldItem::SetShowState(int state) {
    mShowState = state;
    mShowStep = 0;
}

// @0x02169ebc
void HeldItem::Update() {
    UpdateLoad();
    if (mLoadState == LOAD_IDLE) {
        UpdateShow();
    }
}

// @0x02169ee0
void HeldItem::Draw() {
    if (mVisible) {
        AnimModel::Draw();
    }
}

// @0x02169ef8
void HeldItem::UpdateShow() {
    switch (mShowState) {
    case SHOW_IDLE:
        break;
    case SHOW_HIDE:
        SetShowState(SHOW_IDLE);
        mVisible = FALSE;
        break;
    case SHOW_SHOW:
        SetShowState(SHOW_IDLE);
        mVisible = TRUE;
        break;
    }
}

// @0x02169f48
void HeldItem::SetJoint(const MtxFx43 *joint, const MtxFx33 *rotation) {
    VEC_Set(&mPosition, joint->_30, joint->_31, joint->_32);
    MI_Copy36B(rotation, &mRotation);
}

// @0x02169f78
void HeldItem::SetJointOffset(const MtxFx43 *joint, const MtxFx33 *rotation, fx32 x, fx32 y, fx32 z) {
    SetJoint(joint, rotation);
    VecFx32 offset;
    offset.x = x;
    offset.y = y;
    offset.z = z;
    MTX_MultVec33(&offset, rotation, &offset);
    AddPosition(&offset);
}

// @0x02169fc8
void HeldItem::SetLoadState(int state) {
    mLoadState = state;
    mLoadStep = 0;
}

// @0x02169fd8
void HeldItem::SetKind(int kind) {
    if (mKind != kind || mLoadState != LOAD_IDLE) {
        mKind = kind;
        SetLoadState(LOAD_CHANGE);
    } else {
        if (sHeldItemHasPattern[mKind]) {
            RestartAnims();
        }
        mScale.x = FX32_ONE;
        mScale.y = FX32_ONE;
        mScale.z = FX32_ONE;
    }
}

// @0x0216a034
void HeldItem::UpdateLoad() {
    switch (mLoadState) {
    case LOAD_IDLE:
        break;
    case LOAD_UNLOAD:
        SetLoadState(LOAD_IDLE);
        Unload();
        break;
    case LOAD_LOAD_ARCHIVE:
        if (mLoadStep == 0) {
            mLoadStep++;
            MI_CpuCopy8(sHeldItemDigits[mKind], &mArchivePath[12], 2);
            ArchiveManager_MountCompressed(gArchiveManager, mArchiveName, mArchivePath, 0);
        } else if (mLoadStep == 1) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                SetLoadState(LOAD_IDLE);
            }
        }
        break;
    case LOAD_RELOAD:
        SetLoadState(LOAD_IDLE);
        Reload();
        StartPattern();
        break;
    case LOAD_CHANGE:
        if (mLoadStep == 0) {
            mLoadStep++;
            Unload();
        } else if (mLoadStep == 1) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                mLoadStep++;
                MI_CpuCopy8(sHeldItemDigits[mKind], &mArchivePath[12], 2);
                ArchiveManager_MountCompressed(gArchiveManager, mArchiveName, mArchivePath, 0);
            }
        } else if (mLoadStep == 2) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                Reload();
                StartPattern();
                SetLoadState(LOAD_IDLE);
            }
        }
        break;
    }
}

// @0x0216a1ac
void HeldItem::Unload() {
    SetShowState(SHOW_IDLE);
    mVisible = FALSE;
    RemoveAnims();
    func_0202aecc(data_021223e4, mResourceId);
    ArchiveManager_Unmount(gArchiveManager, mArchiveName);
}

// @0x0216a1f0
void HeldItem::Reload() {
    func_0202ad04(data_021223e4, mResourceId, mArchiveName, 0);
    AnimModel::Load(mResourceId, 0);
    mVisible = FALSE;
}

// @0x0216a22c
void HeldItem::StartPattern() {
    BOOL hasPattern = sHeldItemHasPattern[mKind];
    if (hasPattern) {
        func_0202ad04(data_021223e4, mResourceId, mArchiveName, hasPattern);
        SetAnimation(RESOURCE_BTP0, 0);
    }
}

// --- ApartmentRoom ---

// @0x0216a278
ApartmentRoom::ApartmentRoom() {
    SetState(STATE_IDLE);
    func_020a4ab8(&mParts, 3);
    MI_CpuClear16(mNumberText, sizeof(mNumberText));
}

// @0x0216a2c4
void ApartmentRoom::Refresh() {
    UpdateNumberText();
}

// @0x0216a2d0
void ApartmentRoom::SetNextStyle(u16 style) {
    mNextStyle = style;
}

// @0x0216a2dc
void ApartmentRoom::SetStyle(u16 style) {
    mNextStyle = style;
    mStyle = style;
}

// @0x0216a2ec
void ApartmentRoom::UpdateNumberText() {
    int room = (mRoomIndex & 7) + 1;
    int floor = (mRoomIndex >> 3) + 1;
    func_0203673c(floor * 100 + room, mNumberText, sizeof(mNumberText), 4, FALSE);
}

// @0x0216a330
void ApartmentRoom::Update() {
    UpdateState();
    if (mState == STATE_IDLE) {
        func_020a5960(this);
    }
}

// @0x0216a354
void ApartmentRoom::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216a368
void ApartmentRoom::UpdateState() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_RESET:
        SetState(STATE_IDLE);
        func_020a4ab8(&mParts, 0);
        func_020a4144(&mParts, 0);
        func_020a4b74(&mParts, 0);
        func_020a5948(this);
        UpdateNumberText();
        break;
    case STATE_CHANGE_STYLE:
        if (mStep == 0) {
            mStep++;
            mStyle = mNextStyle;
            func_020a54e4(this, 0);
        } else if (mStep == 1) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                mStep++;
                func_020a56dc(this, 0);
            }
        } else if (mStep == 2) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                SetState(STATE_IDLE);
            }
        }
        break;
    case STATE_UNKNOWN3:
        if (mStep == 0) {
            mStep++;
            func_020a5530(this, 7);
        } else if (mStep == 1) {
            SetState(STATE_IDLE);
            func_020a5854(this, 8);
        }
        break;
    }
}

// --- RoomCamera ---

// @0x0216a4a4
RoomCamera::RoomCamera() {
    mCamera = &data_02122764;
}

// @0x0216a4b4
RoomCamera::~RoomCamera() {
    VEC_Set(&data_0213ba74.mPosition, mCamera->mPosition.x, mCamera->mPosition.y, mCamera->mPosition.z);
    VEC_Set(&data_0213ba74.mTarget, mCamera->mTarget.x, mCamera->mTarget.y, mCamera->mTarget.z);
    data_0213ba74.SetOrbit(mOrbit.x >> FX32_SHIFT, mOrbit.y >> FX32_SHIFT, mOrbit.z >> FX32_SHIFT);
}

// @0x0216a514
void RoomCamera::Reset() {
    SetMode(MODE_NONE);
    SetState(STATE_TURN_RIGHT, 0);
    mSnap = TRUE;
    mIgnoreInput = FALSE;
    func_0202a640(mCamera);
    mPosition.x = 0;
    mPosition.y = FX32_CONST(1);
    mPosition.z = FX32_CONST(16);
    mTarget.x = 0;
    mTarget.y = FX32_CONST(1.5);
    mTarget.z = 0;
    mOrbit.x = 0x1200000;
    mOrbit.y = 0x1600000;
    mOrbit.z = 0;
    VecFx32 zero = { 0, 0, 0 };
    mInput = zero;
    mOrbitSpeed = mInput;
    ApplyToCamera();
    mCamera->SetAngles(0, 0, 0);
}

// @0x0216a618
void RoomCamera::SetMode(int mode) {
    mMode = mode;
    mUnknown5C = mFollowFrames = 0;
}

// @0x0216a62c
void RoomCamera::SetState(int state, s16 waitFrames) {
    mState = state;
    mWaitFrames = waitFrames;
}

// Goes to the wanted position and target: at once after a turn, otherwise a part of the way each frame.
// Written out in each of the four places: as an inline function its temporaries come in another order on the stack.
#define MOVE_TO(position, target)                                  \
    if (mSnap) {                                                   \
        mSnap = FALSE;                                             \
        mFollowFrames = 10000;                                     \
    } else {                                                       \
        target = Approach(target, mTarget, 5);                     \
        if (mFollowFrames < 80) {                                  \
            mFollowFrames++;                                       \
            position = Approach(position, mPosition, 5);           \
        } else {                                                   \
            position = Approach(position, mPosition, 4);           \
        }                                                          \
    }                                                              \
    mPosition = position;                                          \
    mTarget = target;                                              \
    ApplyToCamera();

// Not matching yet: 16 instructions of 792 differ, all in the two modes that follow one character (MODE_CHARACTER,
// MODE_CHARACTER_CLOSE): the copy of the character's position is on the stack where the original has it only as
// far as "center = GetPos()" goes; the original then reads it in the order x, z, y and stores radius and target in
// another order. Everything else (both switches, the four MOVE_TO blocks, the stack frame) is identical. Tried:
// center as an initialized local, as a reference to the temporary, as the result of an inline function.
// @0x0216a638
void RoomCamera::Update() {
    switch (mState) {
    case STATE_NONE:
        break;
    case STATE_TURN_RIGHT:
    case STATE_TURN_LEFT:
        if (mState == STATE_TURN_RIGHT) {
            mOrbit.y += (MATH_Rand16(&gRandom, 7) + 1) * 0x2000000;
        } else {
            mOrbit.y -= MATH_Rand16(&gRandom, 2) * 0x2400000;
        }
        SetState(STATE_TURNED, 0);
        mSnap = TRUE;
        VEC_Set(&mInput, 0, 0, 0);
        VEC_Set(&mOrbitSpeed, mInput.x, mInput.y, mInput.z);
        mOrbit.x = (MATH_Rand16(&gRandom, 7) + 4) * 0x200000 + 0xA00000;
        if (mOrbit.x > 0x3000000) {
            mOrbit.x = 0x3000000;
        }
        func_020a4ab8(&mRoom->mParts, mRoom->mParts.mState);
        break;
    case STATE_TURNED:
        SetState(STATE_WAIT, 1140);
        break;
    case STATE_WAIT:
        mWaitFrames--;
        if (mWaitFrames <= 0) {
            if (!mCharacters[0]->mPresent && !mCharacters[1]->mPresent) {
                SetState(STATE_WAIT, 100);
            } else {
                SetState(STATE_TURN_RIGHT, 0);
            }
        }
        break;
    }

    switch (mMode) {
    case MODE_NONE:
        break;
    case MODE_ROOM: {
        UpdateOrbit();
        VecFx32 target;
        target.x = 0;
        target.y = 0x1E66;
        target.z = 0;
        VecFx32 radius;
        radius.x = FX32_CONST(19);
        radius.y = FX32_CONST(9);
        radius.z = FX32_CONST(19);
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 3));
        MOVE_TO(position, target);
        break;
    }
    case MODE_CHARACTER: {
        UpdateOrbit();
        VecFx32 center;
        center = GetPresentCharacter()->GetPos();
        VecFx32 target;
        VecFx32 radius;
        radius.x = center.x;
        radius.y = center.y;
        radius.z = center.z;
        target.x = center.x >> 1;
        target.z = center.z >> 1;
        target.y = 0x1999;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0xA800;
        radius.z = (radius.z * 9 >> 2) + 0xA800;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 4));
        MOVE_TO(position, target);
        break;
    }
    case MODE_BOTH: {
        UpdateOrbit();
        VecFx32 target;
        VEC_Add(&mCharacters[0]->mModel.mPos, &mCharacters[1]->mModel.mPos, &target);
        target.x >>= 1;
        target.z >>= 1;
        target.y = 0x1C00;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        VecFx32 radius;
        VEC_Subtract(&mCharacters[0]->mModel.mPos, &mCharacters[1]->mModel.mPos, &radius);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0x9000;
        radius.z = (radius.z * 9 >> 2) + 0x9000;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 4));
        MOVE_TO(position, target);
        break;
    }
    case MODE_CHARACTER_CLOSE: {
        VecFx32 center;
        center = GetPresentCharacter()->GetPos();
        VecFx32 target;
        VecFx32 radius;
        target.x = center.x;
        target.z = center.z;
        radius.x = center.x;
        radius.y = center.y;
        radius.z = center.z;
        target.y = 0x1999;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0xA800;
        radius.z = (radius.z * 9 >> 2) + 0xA800;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 3));
        MOVE_TO(position, target);
        break;
    }
    }

    if (mState == STATE_TURNED) {
        func_0202a748(mCamera);
    }
}

// @0x0216b298
void RoomCamera::UpdateOrbit() {
    if ((mInput.x > 0 && mOrbit.x > 0x3000000) || (mInput.x < 0 && mOrbit.x < 0xA00000)) {
        mInput.x = 0;
    }
    if (mIgnoreInput) {
        mInput.x = 0;
        mInput.y = 0;
        mInput.z = 0;
    }
    ApplyInput(&mOrbitSpeed.x, mInput.x, 0x4000, 0x40000);
    ApplyInput(&mOrbitSpeed.y, mInput.y, 0x7000, 0x100000);
    VEC_Add(&mOrbit, &mOrbitSpeed, &mOrbit);
    mInput.x = 0;
    mInput.y = 0;
    mInput.z = 0;
}

// @0x0216b34c
void RoomCamera::ApplyInput(fx32 *speed, fx32 input, fx32 friction, fx32 max) {
    *speed += input;
    if (*speed > 0) {
        *speed -= friction;
    } else if (*speed < 0) {
        *speed += friction;
    }
    if (*speed > max) {
        *speed = max;
    } else if (*speed < -max) {
        *speed = -max;
    } else if (MATH_ABS(*speed) <= (friction + 1) >> 1) {
        *speed = 0;
    }
}

// @0x0216b3b0
VecFx32 RoomCamera::GetOrbitPosition(VecFx32 radius, VecFx32 center) {
    u16 elevation = mOrbit.x >> FX32_SHIFT;
    u16 direction = mOrbit.y >> FX32_SHIFT;
    VecFx32 position;
    position.x = center.x + FX_Mul(radius.z, FX_SinIdx(direction));
    position.z = center.z + FX_Mul(radius.x, FX_CosIdx(direction));
    position.y = FX_Mul(radius.y, FX_SinIdx(elevation));
    return position;
}

// @0x0216b488
VecFx32 RoomCamera::Approach(VecFx32 to, VecFx32 from, int shift) {
    VecFx32 step;
    VEC_Subtract(&to, &from, &step);
    step = Shift(step, shift);
    VEC_Add(&step, &from, &step);
    return step;
}

// @0x0216b524
void RoomCamera::Clamp(fx32 *value, fx32 limit) {
    if (*value > limit) {
        *value = limit;
    } else if (*value < -limit) {
        *value = -limit;
    }
}

// --- RoomLight ---

// @0x0216b544
RoomLight::RoomLight() {
    SetState(STATE_IDLE);
}

// @0x0216b55c
void RoomLight::Init(RoomCamera *camera, ApartmentRoom *room) {
    mCamera = camera;
    mRoom = room;
    for (int i = 0; i < 2; i++) {
        mLights[i] = &data_021227cc[i];
        func_0202a96c(mLights[i]);
        mLights[i]->mColor = data_020f72f8[mRoom->mStyle].mLightColor;
    }
}

// @0x0216b5bc
void RoomLight::Update() {
    switch (mState) {
    case STATE_SET_STYLE_COLOR:
        SetColor(data_020f72f8[mRoom->mStyle].mLightColor);
        SetState(STATE_IDLE);
        break;
    case STATE_FADE_STYLE_COLOR:
        if (mStep == 0) {
            mStep++;
            mFade = 0;
        } else {
            int targetR, targetG, targetB;
            int r, g, b;
            u16 color;
            mFade += 0x80;
            func_02028b98(data_020f72f8[mRoom->mStyle].mLightColor, &targetR, &targetG, &targetB);
            BlendColor(&targetR, &targetG, &targetB, &r, &g, &b);
            if (r >= targetR || g >= targetG || b >= targetB) {
                color = data_020f72f8[mRoom->mStyle].mLightColor;
                SetState(STATE_IDLE);
            } else {
                color = r | (g << 5) | (b << 10);
            }
            SetColor(color);
        }
        break;
    case STATE_SET_ROOM_COLOR:
        SetColor(func_020a4d90(mRoom));
        SetState(STATE_IDLE);
        break;
    case STATE_FADE_ROOM_COLOR:
        if (mStep == 0) {
            mStep++;
            mFade = 0;
        } else {
            int targetR, targetG, targetB;
            int r, g, b;
            u16 color;
            mFade += 0x40;
            func_020a4dcc(mRoom, &targetR, &targetG, &targetB);
            BlendColor(&targetR, &targetG, &targetB, &r, &g, &b);
            if (r <= targetR || g <= targetG || b <= targetB) {
                color = targetR | (targetG << 5) | (targetB << 10);
                SetState(STATE_IDLE);
            } else {
                color = r | (g << 5) | (b << 10);
            }
            SetColor(color);
        }
        break;
    }
    func_0202ab2c(mLights[0], mCamera->mOrbit.x >> 12, mCamera->mOrbit.y >> 12, mCamera->mOrbit.z >> 12, 1600,
                  -600, -4000);
    func_0202ab2c(mLights[1], mCamera->mOrbit.x >> 12, mCamera->mOrbit.y >> 12, mCamera->mOrbit.z >> 12, 0,
                  -4000, 1500);
}

// @0x0216b8a8
void RoomLight::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216b8b8
void RoomLight::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// @0x0216b8d4
void RoomLight::BlendColor(int *targetR, int *targetG, int *targetB, int *r, int *g, int *b) {
    func_02028b98(mLights[0]->mColor, r, g, b);
    fx32 weight = FX_Mul(mFade, mFade);
    *r = *r + ((weight * (*targetR - *r)) >> FX32_SHIFT);
    *g = *g + ((weight * (*targetG - *g)) >> FX32_SHIFT);
    *b = *b + ((weight * (*targetB - *b)) >> FX32_SHIFT);
}

// --- RoomCursor ---

// The rectangles around the cursor and the button that take a touch
static const ScreenRect sCursorTouchRect = { -16, -12, 34, 26 };
static const ScreenRect sButtonTouchRect = { -22, -18, 46, 38 };

// @0x0216b970
RoomCursor::RoomCursor() {
    SetState(STATE_IDLE);
}

// @0x0216b988
void RoomCursor::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_MOVE:
        SetState(STATE_IDLE);
        mPosition.x = mTarget.x;
        mPosition.y = mTarget.y;
        break;
    }
}

// @0x0216b9c0
void RoomCursor::SetState(int state) {
    mState = state;
}

// @0x0216b9c8
BOOL RoomCursor::Contains(ScreenPoint point) {
    ScreenRect rect = sCursorTouchRect;
    ScreenPoint position = GetPixelPosition();
    rect.x += position.x;
    rect.y += position.y;
    return func_02028d88(&rect, &point) != 0;
}

// --- BuildingButton ---

// @0x0216ba4c
BuildingButton::BuildingButton() {
    SetState(STATE_IDLE);
}

// @0x0216ba64
void BuildingButton::Init() {
    Sprite_Init(this, 0, 9, 219, 0, TRUE);
}

// @0x0216ba90
void BuildingButton::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_PRESS:
        SetState(STATE_IDLE);
        Sprite_SetSequence(this, 10, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        break;
    case STATE_RELEASE:
        SetState(STATE_IDLE);
        Sprite_SetSequence(this, 9, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        break;
    case STATE_PLAY:
        SetState(STATE_IDLE);
        Sprite_SetPlaying(this, TRUE);
        break;
    }
}

// @0x0216bb18
void BuildingButton::SetState(int state) {
    mState = state;
}

// @0x0216bb20
BOOL BuildingButton::Contains(ScreenPoint point) {
    ScreenRect rect = sButtonTouchRect;
    ScreenPoint position = GetPixelPosition();
    rect.x += position.x;
    rect.y += position.y;
    return func_02028d88(&rect, &point) != 0;
}

// --- ApartmentFront ---

// How many rooms a building of each size has, and how far the view scrolls per floor
static const u8 sRoomCounts[4] = { 24, 48, 72, 100 };
static const fx32 sScrollPerFloor[4] = { 0x16000, 0x16000, 0x19000, 0x1C000 };

// @0x0216bba4
ApartmentFront::ApartmentFront() {
    SetState(STATE_IDLE);
    mScrollTarget = 0;
    mUnknownClearedByConstructor = 0;
    mScroll = 0;
    mSelectedRoom = 0;
    mBuildingSize = 0;
    STD_CopyString(mArchivePath, "Scn/AFwBG/Apart0_LZ.bin");
    MI_CpuFill8(mResidents, 0xFF, sizeof(mResidents));
}

// @0x0216bc08
void ApartmentFront::SetBuildingSize(int size) {
    mBuildingSize = size;
    mArchivePath[15] = size + '0';
}

// @0x0216bc18
void ApartmentFront::Init(int room) {
    for (u8 i = 0; i < sRoomCounts[mBuildingSize]; i++) {
        DrawWindow(i);
    }
    mScreen = (u16 *)(func_020253d4(gGraphicsManager, 0x200) + HW_DB_BG_VRAM);
    mSelectedRoom = room;
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    mScrollTarget = floor * sScrollPerFloor[mBuildingSize];
    ClampScrollTarget();
    mScroll = mScrollTarget;
    UpdateCursorTarget();
    ScreenPoint target = mCursor.GetPixelTarget();
    Sprite_Init(&mCursor, 0, 8, target.x, target.y, TRUE);
    mButton.Init();
    UpdateButtonPosition();
}

// @0x0216bd2c
void ApartmentFront::Touch(ScreenPoint point) {
    SelectRoom(GetRoomAt(point), point.y);
}

// @0x0216bd60
void ApartmentFront::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SCROLL: {
        fx32 target = mScrollTarget;
        fx32 step = (target - mScroll) >> 4;
        if (MATH_ABS(step) >= 0xF5) {
            mScroll += step;
        } else {
            SetState(STATE_IDLE);
            mScroll = target;
        }
        UpdateCursorTarget();
        mCursor.SetState(RoomCursor::STATE_MOVE);
        break;
    }
    }
    int y = 320 - (mScroll >> FX32_SHIFT);
    func_02028154(data_02121834, 0x200, y);
    func_02028154(data_02121834, 0x400, y);
    mCursor.Update();
    mButton.Update();
    UpdateButtonPosition();
}

// @0x0216be1c
void ApartmentFront::Draw() {
    Sprite_Draw(&mCursor);
    Sprite_Draw(&mButton);
}

// @0x0216be34
void ApartmentFront::SetState(int state) {
    mState = state;
}

// @0x0216be3c
void ApartmentFront::SelectRoom(int room, int touchY) {
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    if (mBuildingSize > 3) {
        return;
    }
    if (room < 0) {
        SetState(STATE_SCROLL);
    } else if (room >= sRoomCounts[mBuildingSize]) {
        SetState(STATE_SCROLL);
    } else if (mSelectedRoom != room) {
        mSelectedRoom = room;
        SetState(STATE_SCROLL);
        gSoundManager.StopSeqArc(2, 0);
        switch (GetWindowKind(mSelectedRoom)) {
        case WINDOW_0:
        case WINDOW_1:
        case WINDOW_7:
            gSoundManager.StartSeqArc(2, 9, 3);
            break;
        case WINDOW_2:
            gSoundManager.StartSeqArc(2, 9, 2);
            break;
        case WINDOW_3:
        case WINDOW_5:
        case WINDOW_6:
            gSoundManager.StartSeqArc(2, 9, 4);
            break;
        case WINDOW_EMPTY:
            gSoundManager.StartSeqArc(2, 9, 1);
            break;
        }
    }
    if (touchY >= 40 && touchY <= 144) {
        mScrollTarget = mScroll;
    } else {
        mScrollTarget = floor * sScrollPerFloor[mBuildingSize];
    }
    ClampScrollTarget();
}

// @0x0216bfb8
int ApartmentFront::GetRoomAt(ScreenPoint point) {
    int x = point.x;
    int y = 191 - point.y;
    int floor = (y - 72 + (mScroll >> FX32_SHIFT)) >> 5;
    int column = x >> 5;
    s16 room = column + floor * 8;
    if (floor == 12) {
        if (column <= 1) {
            return room + 4;
        }
        if (column <= 5) {
            return room - 2;
        }
    }
    return room;
}

// @0x0216c014
void ApartmentFront::GetRoomCell(int room, int *column, int *floor) {
    *column = room & 7;
    *floor = room >> 3;
    if (*floor == 12) {
        if (*column <= 3) {
            *column += 2;
        } else if (*column <= 5) {
            *column -= 4;
        }
    }
}

// @0x0216c050
void ApartmentFront::ClampScrollTarget() {
    fx32 max = sScrollPerFloor[mBuildingSize] * (mBuildingSize * 3 + 2);
    if (mScrollTarget < 0) {
        mScrollTarget = 0;
    } else if (mScrollTarget > max) {
        mScrollTarget = max;
    }
}

// Not matching yet: same instructions, another order (11 of 25 differ): the original loads mScroll before the
// column, and stores each of the two locals as soon as it is computed. Tried: plain statements, the scroll in a
// local, compound assignments, the arithmetic in an inline function taking pointers (CellToPosition, kept).
// @0x0216c08c
void ApartmentFront::UpdateCursorTarget() {
    int x;
    int y;
    GetRoomCell(mSelectedRoom, &x, &y);
    CellToPosition(&x, &y, mScroll >> FX32_SHIFT);
    mCursor.SetTarget(x << FX32_SHIFT, y << FX32_SHIFT);
}

// @0x0216c0f0
void ApartmentFront::UpdateButtonPosition() {
    mButton.mPosition.y = ((mScroll >> FX32_SHIFT) + 151) << FX32_SHIFT;
}

// @0x0216c108
int ApartmentFront::GetWindowKind(int room) {
    u8 character = mResidents[room];
    if (character == 0xFF) {
        return WINDOW_EMPTY;
    }
    int unknown1 = func_0202c468(gGameData, character);
    u8 unknown2 = func_0202c53c(gGameData, character);
    int unknown3 = func_0202c5b4(gGameData, character);
    int unknown4 = data_02114d78[unknown3][func_0202c58c(gGameData, character)].mUnknown0;
    if (func_02099c94(unknown1)) {
        return WINDOW_2;
    }
    if (unknown1 == 18) {
        return WINDOW_1;
    }
    if (unknown1 == 17) {
        return WINDOW_7;
    }
    if (unknown2 == 1 || unknown2 == 2) {
        if (unknown3 == 3 && (unknown4 == 5 || unknown4 == 6)) {
            return WINDOW_5;
        }
        if (unknown3 == 4 && unknown4 == 11) {
            return WINDOW_6;
        }
        return WINDOW_3;
    }
    return WINDOW_0;
}

// @0x0216c1f0
void ApartmentFront::DrawWindow(int room) {
    DrawWindowOfKind(GetWindowKind(room), room);
}

// @0x0216c214
void ApartmentFront::DrawWindowOfKind(int kind, int room) {
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    DrawWindowTiles(kind, column, floor);
}

// @0x0216c258
void ApartmentFront::DrawWindowTiles(int kind, u8 column, u8 floor) {
    u16 *screen = mScreen + (column * 4 + 0x660 - floor * 0x80);
    u16 tile;
    int i;
    int palette;
    switch (kind) {
    case WINDOW_0:
        palette = 0x0000;
        tile = 1;
        break;
    case WINDOW_1:
    case WINDOW_2:
        palette = 0x1000;
        tile = 5;
        break;
    case WINDOW_3:
        palette = 0x2000;
        tile = 9;
        break;
    case WINDOW_EMPTY:
        palette = 0x3000;
        tile = 13;
        break;
    case WINDOW_5:
        palette = 0x2000;
        tile = 17;
        break;
    case WINDOW_6:
        palette = 0x2000;
        tile = 21;
        break;
    case WINDOW_7:
        palette = 0x0000;
        tile = 25;
        break;
    default:
        return;
    }
    for (i = 0; i < 4; i++) {
        screen[0] = palette + ((screen[0] & 0xC00) + tile);
        screen[1] = palette + ((screen[1] & 0xC00) + (tile + 1));
        screen[2] = palette + ((screen[2] & 0xC00) + (tile + 2));
        screen[3] = palette + ((screen[3] & 0xC00) + (tile + 3));
        tile += 0x20;
        screen += 0x20;
    }
}

// --- SkyBackground ---

// Which of the seven pictures of the sky each time of day shows
static const s8 sSkyPictures[32] = {
    0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 0,
};

// @0x0216c374
SkyBackground::SkyBackground() {
    STD_CopyString(mArchivePath, "Scn/AFwBG/A0FwFBG0_LZ.bin");
    STD_CopyString(mCharPath, "ASX:/Fw_BG0.ncg");
    STD_CopyString(mPalettePath, "ASX:/Fw_BG0.ncl");
    SetState(STATE_WATCH);
    mPalette = NULL;
    mColors = NULL;
}

// @0x0216c3cc
void SkyBackground::Init(Clock *clock, int buildingSize) {
    mClock = clock;
    func_020a612c(this, &clock->mDate, &clock->mTime);
    mBuildingSize = buildingSize;
    mArchivePath[11] = buildingSize + '0';
    UpdatePaths();
}

// @0x0216c404
void SkyBackground::Refresh() {
    ApplyPalette();
}

// @0x0216c410
void SkyBackground::Update() {
    func_020a61cc(this, &mClock->mDate, &mClock->mTime);
    UpdateState();
}

// @0x0216c434
void SkyBackground::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216c444
void SkyBackground::UpdateState() {
    switch (mState) {
    case STATE_WATCH:
        if (mChanged != 0) {
            SetState(STATE_RELOAD);
        }
        break;
    case STATE_RELOAD:
        if (mStep == 0) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                mStep++;
                UpdatePaths();
                ArchiveManager_Unmount(gArchiveManager, "ASX");
                ArchiveManager_MountCompressed(gArchiveManager, "ASX", mArchivePath, 0);
            }
        } else if (mStep == 1) {
            if (ArchiveManager_IsIdle(gArchiveManager)) {
                SetState(STATE_WATCH);
                func_02028f24(gVramTransferQueue, mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
                mPalette = func_02027534(data_021213b8, mPalettePath, 0x400, 0x80, 0x80, 0xC0);
                ApplyPalette();
            }
        }
        break;
    }
}

// @0x0216c584
void SkyBackground::UpdatePaths() {
    s8 digit = sSkyPictures[func_020a6214(this) & 0x1F] + '0';
    mArchivePath[17] = digit;
    mCharPath[10] = digit;
    mPalettePath[10] = digit;
}

// @0x0216c5bc
void SkyBackground::ApplyPalette() {
    func_020278f8(data_021213b8, mColors[func_020a6214(this)], 0x100);
}

// --- NamePanel ---

// @0x0216c5e8
NamePanel::NamePanel() {
    SetState(STATE_IDLE);
}

// Not matching yet: 17 of 99 instructions differ, between the second InitObjForMessage and the first DrawAll: the
// original computes the second text's character count with other registers and after loading the position's
// constants, and orders the dead stores of SetPosition differently. The calls and their arguments are identical.
// Tried: SetPosition on the sprite and on the canvas, the constants in locals, the count written out.
// @0x0216c618
int NamePanel::Init(int charOffset, MessageFile *file1, MessageFile *file2) {
    Sprite_Init(this, 1, 4, 128, 96, FALSE);
    mText1.InitObjForMessage(file1, 1, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    charOffset += mText1.GetCharCount();
    mText2.InitObjForMessage(file2, 2, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    int count = mText2.GetCharCount();
    mText1.SetPosition(FX32_CONST(38), FX32_CONST(8));
    mText1.DrawAll();
    mText2.SetPosition(FX32_CONST(113), FX32_CONST(8));
    mText2.DrawAll();
    SetState(STATE_SHOWN);
    return charOffset + count;
}

// @0x0216c7a4
void NamePanel::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW_BOTH:
    case STATE_SHOW_FIRST:
        if (mStep == 0) {
            mStep++;
            SetTextVisible(FALSE);
        } else if (mStep == 1) {
            mStep++;
            NNS_G2dCharCanvasClear(&mText1.mCanvas, 0);
            mText1.DrawAll();
        } else {
            NNS_G2dCharCanvasClear(&mText2.mCanvas, 0);
            SetVisible(TRUE);
            if (mState == STATE_SHOW_FIRST) {
                mText2.mSprite.mVisible = FALSE;
            } else {
                mText2.DrawAll();
            }
            SetState(STATE_SHOWN);
        }
        break;
    case STATE_SHOWN:
        if (mStep == 0) {
            mStep++;
            mFrames = 180;
        } else {
            mFrames--;
            if (mFrames <= 0) {
                SetState(STATE_IDLE);
                SetVisible(FALSE);
            }
        }
        break;
    case STATE_HIDE:
        SetVisible(FALSE);
        break;
    }
}

// @0x0216c8f0
void NamePanel::Draw() {
    mText1.Draw();
    mText2.Draw();
    Sprite_Draw(this);
}

// @0x0216c918
void NamePanel::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216c92c
void NamePanel::SetNames(const wchar_t *name1, const wchar_t *name2) {
    mText1.SetStringArg(name1, 0);
    mText2.SetStringArg(name2, 0);
}

// @0x0216c95c
void NamePanel::SetTextVisible(BOOL visible) {
    mText1.mSprite.mVisible = visible;
    mText2.mSprite.mVisible = visible;
}

// @0x0216c968
void NamePanel::SetVisible(BOOL visible) {
    SetTextVisible(visible);
    mVisible = visible;
}

// --- TouchSprite, TouchButton ---

struct ButtonPosition {
    int x;
    int y;
};

struct ButtonSequences {
    u16 mNormal;
    u16 mPressed;
};

// By kind of button
static const ButtonSequences sButtonSequences[3] = { { 2, 3 }, { 0, 1 }, { 0, 0 } };
static const ButtonPosition sButtonPositions[2] = { { 128, 177 }, { 242, 13 } };
static const ScreenRect sButtonRect0 = { -34, -12, 70, 26 };
static const ScreenRect sButtonRect1 = { -16, -16, 34, 34 };
static const ScreenRect sButtonRectOther = { -32, -24, 66, 56 };

// @0x0216c980
TouchSprite::TouchSprite() {
    mEnabled = TRUE;
    mReleased = TRUE;
}

// @0x0216c990
BOOL TouchSprite::Contains(ScreenPoint point) {
    if (!mEnabled) {
        return FALSE;
    }
    ScreenRect rect = *mTouchRect;
    rect.x += mX;
    rect.y += mY;
    return func_02028d88(&rect, &point) != 0;
}

// @0x0216ca14
BOOL TouchSprite::SetReleased(BOOL released) {
    BOOL changed = mReleased != released;
    mReleased = released;
    return changed;
}

// @0x0216ca30
TouchButton::TouchButton() {
    mLabel = NULL;
}

// @0x0216ca4c
void TouchButton::Init(int kind) {
    mKind = kind;
    mX = sButtonPositions[kind].x;
    mY = sButtonPositions[kind].y;
    Sprite_Init(this, 0, sButtonSequences[kind].mNormal, mX, mY, FALSE);
    mState = STATE_IDLE;
    if (kind == 0) {
        mTouchRect = &sButtonRect0;
        MessageText *label = mLabel;
        label->SetPositionFromVec((mX - label->mCanvas.areaWidth * 4) << FX32_SHIFT,
                                   (mY - label->mCanvas.areaHeight * 4) << FX32_SHIFT);
    } else if (kind == 1) {
        mTouchRect = &sButtonRect1;
    } else {
        mTouchRect = &sButtonRectOther;
    }
}

// @0x0216cb34
void TouchButton::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_PRESS:
        mState = STATE_IDLE;
        Sprite_SetSequence(this, sButtonSequences[mKind].mPressed, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        if (mLabel != NULL) {
            mLabel->mSprite.mPosition.y = (mY - mLabel->mCanvas.areaHeight * 4 + 2) << FX32_SHIFT;
        }
        break;
    case STATE_RELEASE:
        mState = STATE_IDLE;
        Sprite_SetSequence(this, sButtonSequences[mKind].mNormal, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        if (mLabel != NULL) {
            mLabel->mSprite.mPosition.y = (mY - mLabel->mCanvas.areaHeight * 4) << FX32_SHIFT;
        }
        break;
    case STATE_PLAY:
        mState = STATE_IDLE;
        Sprite_SetPlaying(this, TRUE);
        break;
    }
}

// @0x0216cc2c
void TouchButton::Draw() {
    Sprite_Draw(this);
}

// @0x0216cc38
BOOL TouchButton::Contains(ScreenPoint point) {
    if (!mVisible) {
        return FALSE;
    }
    return TouchSprite::Contains(point);
}

// --- Clock ---

// @0x0216cc70
Clock::Clock() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

// @0x0216ccac
void Clock::Update() {
    mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
}

// @0x0216ccd4
void Clock::UpdateIfArchiveIdle() {
    if (ArchiveManager_IsIdle(gArchiveManager)) {
        mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
    }
}

// --- ClockDisplay ---

// The animation sequence and the position of each sprite
static const u16 sClockSequences[ClockDisplay::SPRITE_COUNT] = { 0, 0, 2, 0, 0, 3, 0, 0, 1, 0, 0 };
static const ButtonPosition sClockPositions[ClockDisplay::SPRITE_COUNT] = {
    { 160, 8 }, { 167, 8 }, { 174, 8 }, { 181, 8 }, { 188, 8 }, { 205, 8 },
    { 222, 8 }, { 229, 8 }, { 236, 8 }, { 243, 8 }, { 250, 8 },
};

// @0x0216cd10
void ClockDisplay::Init(Clock *clock) {
    mClock = clock;
    SetState(STATE_HIDE);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite *sprite = &mSprites[i];
        Sprite_Init(sprite, 1, sClockSequences[i], 128, 96, TRUE);
        mSprites[i].SetPositionFromVec(sClockPositions[i].x << FX32_SHIFT, sClockPositions[i].y << FX32_SHIFT);
        sprite->mAnim.animCtrl.bActive = FALSE;
    }
    Refresh(TRUE);
}

// @0x0216cdc8
void ClockDisplay::Update() {
    switch (mState) {
    case STATE_SHOWN:
        break;
    case STATE_HIDE:
        SetState(STATE_HIDDEN);
        SetVisible(FALSE);
        break;
    case STATE_HIDDEN:
        mHiddenFrames++;
        if (mHiddenFrames > 190) {
            SetState(STATE_SHOWN);
            SetVisible(TRUE);
        }
        break;
    }
    Refresh(FALSE);
}

// Not matching yet: 2 instructions are swapped in the hour's part ("add r0, r0, #0x400" for &mSprites[7] comes
// after the subtraction in the original, before it here). Everything else is identical.
// @0x0216ce48
void ClockDisplay::Refresh(BOOL all) {
    Clock *clock = mClock;
    if (mMonth != clock->mDate.month || all) {
        u32 tens = clock->mDate.month / 10;
        Sprite_SetFrame(&mSprites[SPRITE_MONTH_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_MONTH_ONES], clock->mDate.month - tens * 10);
        mMonth = clock->mDate.month;
    }
    if (mDay != clock->mDate.day || all) {
        u32 tens = clock->mDate.day / 10;
        Sprite_SetFrame(&mSprites[SPRITE_DAY_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_DAY_ONES], clock->mDate.day - tens * 10);
        mDay = clock->mDate.day;
    }
    if (mHour != clock->mTime.hour || all) {
        u32 half = clock->mTime.hour / 12;
        Sprite_SetFrame(&mSprites[SPRITE_HALF_OF_DAY], half);
        u32 hour = clock->mTime.hour - half * 12;
        u32 tens = hour / 10;
        Sprite_SetFrame(&mSprites[SPRITE_HOUR_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_HOUR_ONES], hour - tens * 10);
        mHour = clock->mTime.hour;
    }
    if (mMinute != clock->mTime.minute || all) {
        u32 tens = clock->mTime.minute / 10;
        Sprite_SetFrame(&mSprites[SPRITE_MINUTE_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_MINUTE_ONES], clock->mTime.minute - tens * 10);
        mMinute = clock->mTime.minute;
    }
}

// @0x0216cfc0
void ClockDisplay::Draw() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

// @0x0216cfe8
void ClockDisplay::SetState(int state) {
    mState = state;
    mHiddenFrames = 0;
}

// @0x0216cffc
void ClockDisplay::SetVisible(BOOL visible) {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        mSprites[i].mVisible = visible;
    }
}

// --- TimedMessage ---

// @0x0216d018
TimedMessage::TimedMessage() {
    SetState(STATE_IDLE);
    mFrames = 0;
}

// @0x0216d070
void TimedMessage::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW:
        NNS_G2dCharCanvasClear(&mCanvas, 0);
        mDrawMode = 1;
        mStarted = FALSE;
        SetVisible(TRUE);
        mFrames = 180;
        SetState(STATE_WAIT);
        break;
    case STATE_SHOW_BY_LINE:
        if (mStep == 0) {
            mStep++;
            NNS_G2dCharCanvasClear(&mCanvas, 0);
            mDrawMode = 3;
            mStarted = FALSE;
        } else if (mDrawMode == 0) {
            SetVisible(TRUE);
            mFrames = 180;
            SetState(STATE_WAIT);
        }
        break;
    case STATE_WAIT:
        if (mFrames > 0) {
            mFrames--;
        } else {
            SetState(STATE_HIDE);
        }
        break;
    case STATE_HIDE:
        SetState(STATE_IDLE);
        SetVisible(FALSE);
        break;
    }
    UpdateDraw();
}

// @0x0216d1a8
void TimedMessage::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216d1bc
void TimedMessage::SetVisible(BOOL visible) {
    mSprite.mVisible = visible;
    if (visible == TRUE) {
        data_0211d604[0] |= 2;
        data_0211d604[0x35] = 1;
    } else {
        data_0211d604[0] &= ~2;
        data_0211d604[0x35] = 1;
    }
}

// --- Emitted by the compiler ---

// HeldItem's destructor is implicit. The copy that only destroys is the main program's func_0203ef44; the
// deleting one is here.
// @0x0216d1fc:D0

// --- NOT WRITTEN YET (34 functions): RoomCharacter's members 0x02168d24 to 0x02169d90, the scene's destructors
// 0x0216d240 (D1) and 0x0216d31c (D0), and the static initialiser 0x0216d67c. They are declared, with the names
// their code suggests, in RoomCharacter.h; what each does:
//   0x02168d24 GetWalkAreaCorner(FloorPos, FloorPos *out): the corner (data_ov074_0216d734) of GetWalkAreaSide
//   0x02168d64 GetWalkAreaSide(x, z): 0..3, which side of the two diagonals the point is on
//   0x02168dc4 SnapAngle(u16): the nearest multiple of 0x4000
//   0x02168dfc GetAngleAround(u16 *angle, FloorPos, FloorPos, int margin): Atan2 of the difference, turned 90
//              degrees to the side that is further from the room's middle, less the margin; FALSE if neither is
//   0x02169144 GetAngleAroundPartner(u16 *angle, int margin): the same for the partner and this; mAngle if FALSE
//   0x021691b0 GetHeadYaw(u16 angle, int limit): angle + mAngle, limited by func_02090c78
//   0x021691d4 SetHeadTurn(u16 pitch, u16 yaw, u16 roll, u8 frames): 0x25da.., 0x25e2, sets mHeadTurnActive
//   0x021691fc GetHeadYawTo(FloorPos, int limit); 0x02169280 GetHeadPitch(VecFx32 from, VecFx32 to, u16 limit)
//   0x02169318 GetHeadPitchToPartner(int node, u16 limit): from CharacterModel_GetJointPosition(model, 0) to the partner's node
//   0x02169388 StepWalkLooking(lookLimit, lookAngle, reach, speed, turnShift): func_02091fbc(1, 0, 0x1a),
//              func_02091194 (TRUE when there), Character_MoveAlongAngle, then the head turn if lookLimit < 0x4000
//   0x0216943c StepTurnLooking(a, b, lookLimit): the same with Character_TurnToWalkTarget
//   0x021694e8 RestartStepSound(): Character_ResetSoundPhase(0) unless IsMovingAnim()
//   0x0216950c SetHeldItemsShowState(int); 0x02169540 SetHeldItemsVisible(BOOL); 0x02169564 AreHeldItemsLoaded()
//   0x02169598 PlaceHeldItemBetweenHands(fx32 height): GetMidpoint of nodes 2 and 1, raised by height * scale
//   0x02169634 PlaceHeldItemsInHands(): item 1 on node 2 with SetJointOffset(0, 0x800, 0), item 0 at node 1
//   0x021696b8 PlaceHeldItemAtHead(): item 0 on node 0 with SetJointOffset(0xbd7, 0, 0)
//   0x02169708 PlaceHeldItemsAtStart(): item 1 at (0, 0, 0x3000), both turned 0x8000
//   0x0216973c StepPutHeldItemsDown(int anim): by mSubStep: ShowHeadIcon, StepChangeHeldItems(2, 3 or 0xff), ...
//   0x02169890 SetOverrideState(int): mOverrideState, clears mOverrideStep
//   0x021698a8 UpdateOverrideState(): 1 unloads the character, 2 mounts its archive, 3 makes its model
//   0x02169a98 UpdateVoiceOfSound(): SetSeqArcVolume and SetSeqArcTrackPitch of mSoundHandle from func_02098ec4
//   0x02169b00, 0x02169b68, 0x02169bb4, 0x02169c04: StartSeqArc(1, 0x1e, 3 / 0 / n / 0) depending on
//              IsAnimOfGroupA/B(mModel.mAnimId); the first call only sets mSoundArmed
//   0x02169c6c RefreshSpeechTexts(): only calls ConvertSpeechTexts()
//   0x02169c78 ConvertSpeechTexts(): the saved character's five texts (0x84, 0xa6..) to Shift JIS at 0x2bec, 0x2c74
//   0x02169d5c ClearSpeechTexts(); 0x02169d90 Save(): func_0202c28c(gGameData, mIndex, this) if func_0202c3dc
//   0x0216d240, 0x0216d31c: the scene's implicit destructor (members with destructors: see NOTES.md)
//   0x0216d67c: fills the walk area's size and four corners (data_ov074_0216d72c, 0x0216d734) from its origin
