#include "SceneHelpers.h"

#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"

#include <string.h>
#include <wchar.h>

extern "C" {
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
void MiiData_GetName(SavedCharacter *character, wchar_t *name);
// Turns a text into the speech synthesizer's notation (readings), from src into dst, of size bytes
void func_020372d0(wchar_t *dst, const wchar_t *src, int size);
void func_020372bc(wchar_t *text, int size); // The same in place
wchar_t *func_0203673c(int value, wchar_t *buffer, int size, int digits, BOOL unk); // A number as text
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads a saved character
BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
void func_0202a96c(Light *light);
void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);
extern Light data_021227d8;
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
BOOL func_02031294(SpriteButton *button);
BOOL func_02024004(ScreenControl *screens, int planes, int level);
void func_02023f4c(ScreenControl *screens, int planes, int level);
}

// --- Texts ---

void MakeCharacterSpeechTexts(CharacterSpeechTexts *texts, SavedCharacter *character) {
    MI_CpuFill8(texts->mNameReadingSjis, 0, sizeof(texts->mNameReadingSjis));
    MI_CpuClear16(texts->mName, sizeof(texts->mName));
    MI_CpuClear16(texts->mNameReading, sizeof(texts->mNameReading));
    MI_CpuClear16(texts->mFamilyNameReading, sizeof(texts->mFamilyNameReading));
    MI_CpuClear16(texts->mGivenNameReading, sizeof(texts->mGivenNameReading));
    MI_CpuFill8(texts->mPhraseReadingSjis, 0, sizeof(texts->mPhraseReadingSjis));
    MI_CpuClear16(texts->mPhrase, sizeof(texts->mPhrase));
    MI_CpuClear16(texts->mPhraseReading, sizeof(texts->mPhraseReading));
    func_020372d0(texts->mFamilyNameReading, character->mFamilyName, sizeof(texts->mFamilyNameReading));
    func_020372d0(texts->mGivenNameReading, character->mName, sizeof(texts->mGivenNameReading));
    MiiData_GetName(character, texts->mName);
    MiiData_GetName(character, texts->mNameReading);
    func_020372bc(texts->mNameReading, sizeof(texts->mNameReading));
    int length = sizeof(texts->mNameReadingSjis);
    STD_ConvertStringUnicodeToSjis(texts->mNameReadingSjis, &length, (const u16 *)texts->mNameReading, NULL, NULL);
    if (wcslen(character->mMotto) == 0) {
        wcscpy(texts->mPhrase, L"ミニュウリョク");
        texts->mHasPhrase = FALSE;
    } else {
        wcscpy(texts->mPhrase, character->mMotto);
        texts->mHasPhrase = TRUE;
    }
    func_020372d0(texts->mPhraseReading, texts->mPhrase, sizeof(texts->mPhraseReading));
    length = sizeof(texts->mPhraseReadingSjis);
    STD_ConvertStringUnicodeToSjis(texts->mPhraseReadingSjis, &length, (const u16 *)texts->mPhraseReading, NULL,
                                   NULL);
}

static const char *sThousands[10] = {"", "セン/", "ニセン/", "サ’ンゼン/", "ヨ’ンセン/",
                                     "ゴセン/", "ロクセン/", "ナナ’セン/", "ハッセン/", "キュ’ーセン/"};
static const char *sArchiveNames[SceneCast::MAX] = {"B0I", "B1I", "B2I", "B3I", "B4I",
                                                    "B5I", "B6I", "B7I", "B8I", "B9I"};
static const char *sOnes[10] = {"", "イチ", "ニ", "サン", "ヨ’ン", "ゴ", "ロク", "ナナ", "ハチ", "キュ’ー"};
static const char *sHundreds[10] = {"", "ヒャク/", "ニヒャク/", "サ’ンビャク/", "ヨ’ンヒャク/",
                                    "ゴヒャク/", "ロッピャク/", "ナナ’ヒャク/", "ハッピャク/", "キュ’ーヒャク/"};
static const char *sTens[10] = {"", "ジュウ/", "ニ’ジュウ/", "サ’ンジュー/", "ヨ’ンジュー/",
                                "ゴジュー/", "ロクジュー/", "ナナ’ジュー/", "ハチジュー/", "キュ’ージュー/"};

void MakeNumberReading(wchar_t *text, int number) {
    char reading[0x40];
    MI_CpuClear16(text, 0x40);
    MI_CpuFill8(reading, 0, sizeof(reading));
    int thousands = number / 1000 % 100;
    int hundreds = number / 100 % 10;
    int tens = number / 10 % 10;
    int ones = number % 10;
    if (number == 0) {
        strcat(reading, "レイ");
    } else {
        strcat(reading, sThousands[thousands]);
        strcat(reading, sHundreds[hundreds]);
        strcat(reading, sTens[tens]);
        strcat(reading, sOnes[ones]);
    }
    STD_ConvertStringSjisToUnicode((u16 *)text, NULL, reading, NULL, NULL);
}

void MakeNumberText(wchar_t *text, int number) {
    MI_CpuClear16(text, 10);
    if (number >= 10000) {
        number = 9999;
    }
    func_0203673c(number, text, 10, 0, FALSE);
}

// The directions of FaceCast's two lights
static const VecFx32 sLightDirections[2] = {{0, 0, 0xFA0}, {0, 0, -0xFA0}};
// The sounds of the two buttons (SpriteButton::mSounds)
static const ButtonSounds sReturnButtonSounds = {{12, -1, 0, -1}};
static const ButtonSounds sNewsButtonSounds = {{10, -1, 0, -1}};

// --- SceneCast ---

// Chooses mCount saved characters: those gSceneMemory names first (at most four), the rest at random, all different.
// FALSE if there are not enough.
BOOL SceneCast::Choose(int count) {
    mCount = count;
    mResidentCount = 0;
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
        }
    }
    if (mResidentCount < mCount) {
        return FALSE;
    }
    int n = 0;
    while (n < 4) {
        u32 character = gSceneMemory.mCharacters[(u8)n];
        if (character >= 100 || !func_0202c3dc(&gGameData, character)) {
            break;
        }
        mChosen[n] = character;
        n++;
    }
    for (; n < mCount; n++) {
    retry:
        mChosen[n] = mResidents[MATH_Rand16(&gRandom, mResidentCount)];
        BOOL taken = FALSE;
        for (int i = 0; i < n; i++) {
            if (mChosen[n] == mChosen[i]) {
                taken = TRUE;
            }
        }
        if (taken) {
            goto retry;
        }
    }
    return TRUE;
}

void SceneCast::SetCharacters(Character *characters) {
    for (int i = 0; i < mCount; i++) {
        mCharacters[i] = &characters[i];
    }
    for (int i = 0; i < mCount; i++) {
        func_0202c264(&gGameData, mChosen[i], mCharacters[i]);
        mCharacters[i]->mIndex = mChosen[i];
    }
}

void SceneCast::MountArchives(const char *animationArchive) {
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", animationArchive, 0);
    for (int i = 0; i < mCount; i++) {
        mCharacters[i]->SetArchive(sArchiveNames[i]);
        ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[i]->mArchiveName,
                                       mCharacters[i]->mArchivePath, 0);
    }
}

void SceneCast::InitCharacters() {
    void *animations = func_0202ad04(data_021223e4, 31, "BIC", 0);
    for (int i = 0; i < mCount; i++) {
        ModelResources_SetModel(data_021223e4, i, mCharacters[i]->mModelPath);
        mCharacters[i]->Init(i, 0, 2);
        func_0202c264(&gGameData, mChosen[i], mCharacters[i]);
        mCharacters[i]->MakeFace();
        mCharacters[i]->UpdateBodyScale();
        mCharacters[i]->UpdateAnimSpeed();
        mCharacters[i]->UpdateBodyColors();
        func_02029660((void *)mCharacters[i]->mAnim.mUnknown04, RESOURCE_BCA0, animations);
        mCharacters[i]->SetAnimation(RESOURCE_BCA0, 0);
        mCharacters[i]->SetAngle(0x4000);
        MakeCharacterSpeechTexts(&mSpeechTexts[i], mCharacters[i]);
    }
    mCamera = &data_02122764;
    mSpeaker = -1;
}

void SceneCast::LipSync(const char *phoneme) {
    if (mSpeaker != -1) {
        mCharacters[mSpeaker]->LipSync(phoneme, mSpeakerFace);
    }
}

void SceneCast::UpdateCharacters() {
    for (int i = 0; i < mCount; i++) {
        mCharacters[i]->Update();
    }
}

void SceneCast::DrawCharacters() {
    for (int i = 0; i < mCount; i++) {
        mCharacters[i]->Draw();
    }
}

void SceneCast::Release() {
    for (int i = 0; i < mCount; i++) {
        func_0202aecc(data_021223e4, i);
        ArchiveManager_Unmount(&gArchiveManager, sArchiveNames[i]);
    }
    func_0202aecc(data_021223e4, 31);
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
}

// Loads a character's archive and model again (after its clothes changed)
void SceneCast::ReloadCharacter(int index) {
    func_0202aecc(data_021223e4, index);
    ArchiveManager_Unmount(&gArchiveManager, sArchiveNames[index]);
    mCharacters[index]->SetArchive(sArchiveNames[index]);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[index]->mArchiveName,
                                   mCharacters[index]->mArchivePath, 1);
    ModelResources_SetModel(data_021223e4, index, mCharacters[index]->mModelPath);
    mCharacters[index]->MakeModel(index, 0);
    mCharacters[index]->UpdateBodyColors();
    mCharacters[index]->UpdateBodyScale();
    mCharacters[index]->UpdateAnimSpeed();
}

// Reads a character from the save data again
void SceneCast::RereadCharacter(int index) {
    mCharacters[index]->Init(index, 0, 2);
    func_0202c264(&gGameData, mChosen[index], mCharacters[index]);
    mCharacters[index]->MakeFace();
    MakeCharacterSpeechTexts(&mSpeechTexts[index], mCharacters[index]);
}

// --- FaceCast ---

void FaceCast::StartLoading() {
}

void FaceCast::Init(int capacity) {
    mCount = 0;
    mCapacity = capacity;
    for (u16 i = 0; i < mCapacity; i++) {
        mFaces[i].InitFace(1);
        mFaces[i].mFaceDrawParam.emission = 0;
    }
    for (int i = 0; i < mCapacity; i++) {
        mTransforms[i].mPosition.x = 0;
        mTransforms[i].mPosition.y = 0;
        mTransforms[i].mPosition.z = FX32_ONE;
        mTransforms[i].mRotX = 0;
        mTransforms[i].mRotY = 0;
    }
    mSpeaker = -1;
    mDragX = mDragY = 0;
}

// The camera at its start, and the two lights from the front left and the back
#pragma opt_propagation off
void FaceCast::SetupCamera() {
    Camera *camera = &data_02122764;
    func_0202a640(camera);
    func_0202a93c(camera, 1);
    Light *light = &data_021227cc[0];
    data_021223e4[0x424 / 4] = 0;
    func_0202a96c(light);
    func_0202aa28(light, camera, sLightDirections[0]);
    light = &data_021227d8;
    func_0202a96c(light);
    func_0202aa28(light, camera, sLightDirections[1]);
}

#pragma opt_propagation reset

void FaceCast::UpdateFaces() {
    for (int i = 0; i < mCount; i++) {
        mFaces[i].UpdateFace();
    }
}

static inline ScreenPoint operator-(ScreenPoint a, ScreenPoint b) {
    ScreenPoint result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    return result;
}

// The stylus dragged across the screen turns the first face
void FaceCast::UpdateDrag() {
    if (gTouchInput.mTp.touch && !gTouchInput.mTouched) {
        ScreenPoint last = gTouchInput.mLastPoint;
        ScreenPoint now = gTouchInput.mPoint;
        ScreenPoint drag = now - last;
        int x = mDragX + drag.x;
        if (x > 32) {
            x = 32;
        } else if (x < -32) {
            x = -32;
        }
        mDragX = x;
        int y = mDragY + drag.y;
        if (y > 16) {
            y = 16;
        } else if (y < -16) {
            y = -16;
        }
        mDragY = y;
        mTransforms[0].mRotY = mBaseRotY + (mDragX << 7);
        mTransforms[0].mRotX = mBaseRotX + (mDragY << 7);
    }
    ApplyTransform(0);
}

void FaceCast::DrawFaces() {
    for (u16 i = 0; i < mCount; i++) {
        mFaces[i].Draw();
    }
}

void FaceCast::ResetCamera() {
    func_0202a640(&data_02122764);
    data_021223e4[0x424 / 4] = 1;
}

// Chooses mCount saved characters as SceneCast::Choose does
BOOL FaceCast::Choose() {
    int residents[100];
    int count = 0;
    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            residents[count] = i;
            count++;
        }
    }
    if (count < mCount) {
        return FALSE;
    }
    int n = 0;
    while (n < 4) {
        u32 character = gSceneMemory.mCharacters[(u8)n];
        if (character >= 100 || !func_0202c3dc(&gGameData, character)) {
            break;
        }
        mChosen[n] = character;
        n++;
    }
    for (; n < mCount; n++) {
    retry:
        mChosen[n] = residents[MATH_Rand16(&gRandom, count)];
        BOOL taken = FALSE;
        for (int i = 0; i < n; i++) {
            if (mChosen[n] == mChosen[i]) {
                taken = TRUE;
            }
        }
        if (taken) {
            goto retry;
        }
    }
    return TRUE;
}

void FaceCast::ReadCharacters() {
    for (u16 i = 0; i < mCount; i++) {
        func_0202c264(&gGameData, mChosen[i], &mFaces[i]);
        mFaces[i].MakeFace();
        MakeCharacterSpeechTexts(&mSpeechTexts[i], &mFaces[i]);
    }
}

void FaceCast::SetTransform(int index, VecFx32 position, u16 rotX, u16 rotY, u16 rotZ) {
    mTransforms[index].mPosition.x = position.x;
    mTransforms[index].mPosition.y = position.y;
    mTransforms[index].mPosition.z = position.z;
    mTransforms[index].mRotX = rotX;
    mTransforms[index].mRotY = rotY;
    mTransforms[index].mRotZ = rotZ;
    ApplyTransform(index);
}

// The same from the bytes of a table: the position in 1/32 units, the scale in 1/64, the turns in 1/256 of a circle
void FaceCast::SetTransform(int index, u16 id, int x, s8 y, u8 scale, u8 rotX, u8 rotY, u8 rotZ) {
    mTransforms[index].mId = id;
    mTransforms[index].mPosition.x = x << 7;
    mTransforms[index].mPosition.y = y << 7;
    mTransforms[index].mPosition.z = scale << 6;
    mTransforms[index].mRotX = rotX << 8;
    mTransforms[index].mRotY = rotY << 8;
    mTransforms[index].mRotZ = rotZ << 8;
    ApplyTransform(index);
    if (index == 0) {
        mBaseRotY = mTransforms[0].mRotY;
        mBaseRotX = mTransforms[0].mRotX;
    }
}

// Gives a face the matrices of its transform
void FaceCast::ApplyTransform(int index) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    MtxFx33 turn;
    MTX_Identity33(&rotation);
    MTX_Identity33(&turn);
    MTX_Identity43(&matrix);
    MTX_RotX33(&rotation, FX_SinIdx(mTransforms[index].mRotX), FX_CosIdx(mTransforms[index].mRotX));
    MTX_RotY33(&turn, FX_SinIdx(mTransforms[index].mRotY), FX_CosIdx(mTransforms[index].mRotY));
    MTX_Concat33(&rotation, &turn, &rotation);
    MTX_RotZ33(&turn, FX_SinIdx(mTransforms[index].mRotZ), FX_CosIdx(mTransforms[index].mRotZ));
    MTX_Concat33(&rotation, &turn, &rotation);
    MTX_ScaleApply33(&rotation, &rotation, mTransforms[index].mPosition.z, mTransforms[index].mPosition.z,
                     mTransforms[index].mPosition.z);
    MTX_TransApply43(&matrix, &matrix, mTransforms[index].mPosition.x, mTransforms[index].mPosition.y, 0);
    mFaces[index].SetHeadMatrices(&matrix, &rotation);
}

void FaceCast::SetTransform(int index, FaceTransform transform) {
    SetTransform(index, transform.mPosition, transform.mRotX, transform.mRotY, transform.mRotZ);
}

void FaceCast::LipSync(const char *phoneme) {
    if (mSpeaker != -1) {
        (&mFaces[mSpeaker])->LipSync(phoneme, mSpeakerFace);
    }
}

// --- ReturnButton ---

void ReturnButton::MountArchive() {
    ArchiveManager_MountCompressed(&gArchiveManager, "RET", "Scn/Scene/ReturnBtn_LZ.bin", 0);
}

void ReturnButton::Init(int sceneId, int plane, int resource) {
    mSceneId = sceneId;
    ObjResourceManager_Load(&gObjResourceManager, "RET:/Obj_down.nce", "RET:/Obj_down.ncg", plane, resource, -1);
    func_02027534(&data_021213b8, "RET:/Obj_down.ncl", plane, 0xE0, 0xE0, 0x40);
    Sprite_Init(this, resource, 2, 0x80, 0x60, TRUE);
    func_02031eb0(this, 24, 22);
    func_02031278(this, 2, 3);
    func_02031260(this, 4);
    func_02032158(this, &sReturnButtonSounds);
    SetPositionFromVec(0xF2000, 0xD000);
    func_0203119c(this);
    func_02028280(&data_02121834, plane, 0);
    data_0211d604.Show(plane);
    mHeld = FALSE;
    mPressed = FALSE;
}

void ReturnButton::Update() {
    mHeld = FALSE;
    mPressed = FALSE;
    if (func_0203133c(this)) {
        if (mSceneId != -1) {
            gSceneManager.RequestScene(mSceneId);
        }
        mPressed = TRUE;
    }
    if (func_02031294(this)) {
        mHeld = TRUE;
    }
}

void ReturnButton::Draw() {
    func_02031d7c(this);
}

void ReturnButton::UnmountArchive() {
    ArchiveManager_Unmount(&gArchiveManager, "RET");
}

// --- SceneRoom ---

void SceneRoom::Load(u16 style) {
    mStyle = style;
    func_020a54c4(this, "CO0", 0);
    func_020a56bc(this, "RM", 0);
}

void SceneRoom::Init(int slot, int slot2, BOOL doorTurned) {
    mState = 0;
    mFrame = 0;
    func_020a5514(this, slot);
    func_020a5760(this, slot2);
    func_020a4ab8(&mParts, 0);
    if (doorTurned) {
        mParts.mDoor.mRotY = 0x8000;
    }
}

void SceneRoom::Update() {
    switch (mState) {
    case 0:
        func_020a5960(this);
        if (++mFrame >= 2) {
            mState = 1;
        }
        break;
    case 1:
        break;
    }
    func_020a4a50(&mParts);
}

void SceneRoom::Draw() {
    func_020a5948(this);
}

// Hides parts of the room: 0 those the style gives 0 or less, 1 those it gives 0 or more, 2 all; 3 sets the
// parts' state 2
void SceneRoom::ShowParts(int mode) {
    for (int i = 0; i < 6; i++) {
        if (mParts.mParts[i].mUnknown000 < 0) {
            continue;
        }
        switch (mode) {
        case 0:
            if (data_020f72f8[mStyle].mParts[i].mUnknown8 <= 0) {
                mParts.mParts[i].mModel.mVisible = FALSE;
            }
            break;
        case 1:
            if (data_020f72f8[mStyle].mParts[i].mUnknown8 >= 0) {
                mParts.mParts[i].mModel.mVisible = FALSE;
            }
            break;
        case 2:
            mParts.mParts[i].mModel.mVisible = FALSE;
            break;
        case 3:
            func_020a4ab8(&mParts, 2);
            break;
        }
    }
}

// --- NewsButton ---

void NewsButton::LoadBackground() {
    ArchiveManager_MountCompressed(&gArchiveManager, "NEW", "Scn/Scene/SceneMiiNews_LZ.bin", 1);
    func_02027d7c(&data_02121834, "NEW:/BG_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "NEW:/BG_down.ncg", 0x100, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "NEW:/BG_down.ncl", 0x100, -1, -1, -1);
    func_02027d7c(&data_02121834, "NEW:/TVframeWide.nsc", 8, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "NEW:/TVframe.ncg", 8, 0, 0, 0, -1);
    func_02023f4c(&data_0211d604, 0x17, -16);
    func_020278f8(&data_021213b8, 0, 0x101);
}

void NewsButton::Init(int resource) {
    func_02027290(&gObjResourceManager, "NEW:/Obj_down.nce", 0x1000, resource);
    func_02028f24(&gVramTransferQueue, "NEW:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "NEW:/Obj_down.ncl", 0x1000, -1, -1, -1);
    Sprite_Init(this, 1, 6, 0x80, 0x60, TRUE);
    func_02031eb0(this, 116, 30);
    func_02031278(this, 6, 7);
    func_02031260(this, 4);
    func_02032158(this, &sNewsButtonSounds);
    SetPositionFromVec(0x80000, 0x60000);
    func_0203119c(this);
    func_02028280(&data_02121834, 8, 0);
    func_02028280(&data_02121834, 0x1000, 0);
    func_02028280(&data_02121834, 0x100, 1);
    func_020278f8(&data_021213b8, 0, 0x101);
}

void NewsButton::Update(Scene *scene, BOOL leave) {
    mPressed = FALSE;
    func_02024004(&data_0211d604, 0x17, 1);
    if (func_0203133c(this)) {
        mPressed = TRUE;
        if (leave) {
            scene->mSkipFade = TRUE;
            func_02023f4c(&data_0211d604, 0x3F, -16);
            gSceneManager.RequestScene(0x49);
        }
    }
}

void NewsButton::Draw() {
    func_02031d7c(this);
}

void NewsButton::Unload() {
    ArchiveManager_Unmount(&gArchiveManager, "NEW");
    func_020278f8(&data_021213b8, 0, 0x101);
}
