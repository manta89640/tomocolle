#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "sound/SoundManager.h"

#include "InformationScene.h"

// Overlay 99: the mansion's information (InformationScene). One source file: its constants are one pool at the
// start of .rodata, its strings the .data after the scene's virtual table.

#define ABS(x) ((x) < 0 ? -(x) : (x))

struct TextPosition {
    int x;
    int y;
};

// The file's constants. Their order decides where the compiler puts them (see docs/notes/ov073.md).
static const u16 sButtonSequences[1] = {0};
static const TextPosition sButtonSizes[1] = {{24, 22}};
static const TextPosition sButtonPositions[1] = {{242, 13}};
static const u16 sStarSequences[InformationScene::STAR_COUNT] = {0, 0, 0, 0, 0};
static const ButtonSounds sButtonSounds[1] = {{{12, 1, 0, -1}}};
static const TextPosition sStarPositions[InformationScene::STAR_COUNT] = {
    {42, 167}, {56, 167}, {70, 167}, {84, 167}, {98, 167},
};
static const u32 sMessages[InformationScene::TEXT_COUNT] = {
    0x18E, 0x185, 0x186, 0x187, 0x188, 0x1A0, 0x189, 0x18F, 0x18A, 0x18B, 0x18C, 0x18D, 0x1A1,
};
static const int sFonts[InformationScene::TEXT_COUNT] = {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0};
// Where each personality (0 to 15) moves the mansion's type, on two axes
static const int sPersonalityX[16] = {-2, -1, 1, 2, -2, -1, 1, 2, -2, -1, 1, 2, -2, -1, 1, 2};
static const int sPersonalityY[16] = {-2, -2, -2, -2, -1, -1, -1, -1, 1, 1, 1, 1, 2, 2, 2, 2};
static const TextPosition sTextPositions[InformationScene::TEXT_COUNT] = {
    {48, 50},   {31, 99},   {108, 99},  {190, 99},  {45, 143},  {156, 143}, {128, 20},
    {128, 78},  {51, 123},  {128, 123}, {205, 123}, {66, 161},  {186, 167},
};

// Puts a text at a position in pixels
static inline void SetTextPosition(MessageText *text, TextPosition position) {
    text->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

void InformationScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "N83", "Scn/Scene/SceneInformation_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TET", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    gSoundManager.LoadGroup(0x40);
    gSoundManager.StartSeqIfNotPlaying(0, 0x71);
}

void InformationScene::Init() {
    Clock_GetDate(data_02123440, &mDate);
    mDate.year += 2000;
    int charOffset = 100;
    mMessageFiles[0].Open("TET:/tmd_etc_msg_JP.bmg");

    func_02027534(&data_021213b8, "N83:/BG_down.ncl", 0x300, -1, -1, -1);
    func_02027d7c(&data_02121834, "N83:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "N83:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "N83:/BG_up.ncl", 1, -1, -1, -1);
    func_02027d7c(&data_02121834, "N83:/BG_up.nsc", 1, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "N83:/BG_up.ncg", 1, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "N83:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "N83:/Obj_down.nce", "N83:/Obj_down.ncg", 0x1000, 1, -1);
    func_02027534(&data_021213b8, "N83:/Obj_up.ncl", 0x10, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "N83:/Obj_up.nce", "N83:/Obj_up.ncg", 0x10, 0, -1);

    int i;
    for (i = 0; i < STAR_COUNT; i++) {
        Sprite_Init(&mStars[i], 0, sStarSequences[i], 0x80, 0x60, 1);
        mStars[i].SetPositionFromVec(sStarPositions[i].x << FX32_SHIFT, sStarPositions[i].y << FX32_SHIFT);
    }

    for (i = 0; i < 1; i++) {
        u16 sequence = sButtonSequences[i];
        Sprite_Init(&mButtons[i], 1, sequence, 0x80, 0x60, 1);
        func_02031eb0(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        func_02031278(&mButtons[i], sequence, sequence + 1);
        func_02031260(&mButtons[i], 4);
        mButtons[i].SetPositionFromVec(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT);
        func_02032158(&mButtons[i], &sButtonSounds[i]);
        func_0203119c(&mButtons[i]);
    }

    // The statistics over the saved characters
    int month;
    int men = 0;
    int women = 0;
    int ageSum = 0;
    int ageCount = 0;
    int comfortSum = 0;
    int type = 0;
    int typeX = 0;
    int typeY = 0;
    int troubles;
    int residents;
    int averageAge;
    int year;
    u32 typeMessage;
    for (i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            if (func_0202c604(&gGameData, i) == FALSE) {
                men++;
            } else {
                women++;
            }            SavedCharacter character;
            func_0202c264(&gGameData, i, &character);
            year = character.mBirthYear;
            if (year != BIRTH_YEAR_UNKNOWN) {
                month = func_02098dac(&character, PROPERTY_BIRTH_MONTH);
                int day = func_02098dac(&character, PROPERTY_BIRTH_DAY);
                int age = GetAge(year, month, day);
                if (age != -1) {
                    ageSum += age;
                    ageCount++;
                }
            }
            comfortSum += character.mUnknown0FB;
            s8 personality = func_02098f2c(&character);
            typeX += sPersonalityX[personality];
            typeY += sPersonalityY[personality];
        }
    }
    residents = men + women;
    averageAge = ageSum * 10 / ageCount;
    int comfort = GetComfort(comfortSum, residents);
    if (men == 0) {
        men = 0;
        women = 1;
    } else if (women == 0) {
        men = 1;
        women = 0;
    } else {
        int divisor = GetCommonDivisor(men, women);
        men /= divisor;
        women /= divisor;
    }

    // The mansion's type: the direction of the sum of the personalities, in eight
    if (typeX == 0 && typeY == 0) {
        type = 8;
    } else if (typeX == 0) {
        type = typeY > 0 ? 2 : 6;
    } else if (typeY == 0) {
        type = typeX > 0 ? 0 : 4;
    } else if (typeX > 0 && typeY > 0) {
        if (ABS(typeX) >= ABS(typeY)) {
            if (ABS(typeX) / ABS(typeY) >= 2) {
                type = 0;
            } else {
                type = 1;
            }
        } else if (ABS(typeX) < ABS(typeY)) {
            type = ABS(typeY) / ABS(typeX) >= 2 ? 2 : 1;
        }
    } else if (typeX >= 0 && typeY < 0) {
        if (ABS(typeX) >= ABS(typeY)) {
            type = ABS(typeX) / ABS(typeY) >= 2 ? 0 : 7;
        } else if (ABS(typeX) < ABS(typeY)) {
            type = ABS(typeY) / ABS(typeX) >= 2 ? 6 : 7;
        }
    } else if (typeX < 0 && typeY >= 0) {
        if (ABS(typeX) >= ABS(typeY)) {
            type = ABS(typeX) / ABS(typeY) >= 2 ? 4 : 3;
        } else if (ABS(typeX) < ABS(typeY)) {
            type = ABS(typeY) / ABS(typeX) >= 2 ? 2 : 3;
        }
    } else if (typeX < 0 && typeY < 0) {
        if (ABS(typeX) >= ABS(typeY)) {
            type = ABS(typeX) / ABS(typeY) >= 2 ? 4 : 5;
        } else if (ABS(typeX) < ABS(typeY)) {
            type = ABS(typeY) / ABS(typeX) >= 2 ? 6 : 5;
        }
    }
    // and how strongly: each type has two messages
    int strength;
    if (ABS(typeX) >= ABS(typeY)) {
        strength = ABS(typeX);
    } else {
        strength = ABS(typeY);
    }
    if (strength == 0) {
        type = type * 2;
    } else if (residents <= strength * 3 / 2) {
        type = type * 2;
    } else {
        type = type * 2 + 1;
    }

    troubles = gGameData.mTroublesSolved;
    typeMessage = type + 0x18F;
    for (int t = 0; t < TEXT_COUNT; t++) {
        wchar_t numbers[2][11];
        mTexts[t].InitObjForMessage(&mMessageFiles[0], sMessages[t], &gFonts[sFonts[t]].mFont, 0x10, charOffset, 0,
                                    1, 0, 0, 0xF, NULL);
        SetTextPosition(&mTexts[t], sTextPositions[t]);
        if (t == TEXT_COMFORT) {
            mTexts[t].mHAlign = 0;
            mTexts[t].mVAlign = 0;
        } else if (t < TEXT_TITLE) {
            mTexts[t].mHAlign = 0;
            mTexts[t].mVAlign = 0;
        } else {
            mTexts[t].mHAlign = 1;
            mTexts[t].mVAlign = 1;
            MoveSprite(&mTexts[t].mSprite, (-mTexts[t].mCanvas.areaWidth * 4) << FX32_SHIFT,
                       (-mTexts[t].mCanvas.areaHeight * 4) << FX32_SHIFT);
        }
        MI_CpuClear16(numbers[0], sizeof(numbers[0]));
        MI_CpuClear16(numbers[1], sizeof(numbers[1]));
        switch (t) {
        case TEXT_TITLE:
            wcscpy(numbers[0], data_02123510);
            mTexts[t].SetStringArg(numbers[0], 0);
            break;
        case TEXT_TYPE:
            mTexts[t].mMessage = typeMessage;
            break;
        case TEXT_RESIDENTS:
            func_0203673c(residents, numbers[0], sizeof(numbers[0]), 0, FALSE);
            mTexts[t].SetStringArg(numbers[0], 0);
            break;
        case TEXT_AGE:
            func_02036a2c(averageAge, numbers[0], sizeof(numbers[0]), 0, 0, 1);
            mTexts[t].SetStringArg(numbers[0], 0);
            break;
        case TEXT_RATIO:
            func_0203673c(men, numbers[0], sizeof(numbers[0]), 0, FALSE);
            mTexts[t].SetStringArg(numbers[0], 0);
            func_0203673c(women, numbers[1], sizeof(numbers[1]), 0, FALSE);
            mTexts[t].SetStringArg(numbers[1], 1);
            break;
        case TEXT_COMFORT:
            if (comfort < 6) {
                for (int j = 0; j < STAR_COUNT; j++) {
                    if (j < comfort) {
                        mStars[j].mVisible = TRUE;
                    } else {
                        mStars[j].mVisible = FALSE;
                    }
                }
                mTexts[t].mSprite.mVisible = FALSE;
            } else {
                for (int j = 0; j < STAR_COUNT; j++) {
                    if (j == 1) {
                        mStars[j].mVisible = TRUE;
                    } else {
                        mStars[j].mVisible = FALSE;
                    }
                }
                func_0203673c(comfort, numbers[0], sizeof(numbers[0]), 0, FALSE);
                mTexts[t].SetStringArg(numbers[0], 0);
            }
            break;
        case TEXT_TROUBLES:
            func_0203673c(troubles, numbers[0], sizeof(numbers[0]), 0, FALSE);
            mTexts[t].SetStringArg(numbers[0], 0);
            break;
        }
        mTexts[t].DrawAll();
        charOffset += mTexts[t].GetCharCount();
    }

    func_02028280(&data_02121834, 0x1000, 0);
    func_02028280(&data_02121834, 0x100, 1);
    data_0211d604.Set(0x1211);
}

void InformationScene::Update() {
    if (func_0203133c(&mButtons[0])) {
        func_0203c02c(&gSceneManager, 0x11);
    }
}

void InformationScene::Draw() {
    for (int i = 0; i < TEXT_COUNT; i++) {
        mTexts[i].Draw();
    }
    func_02031d7c(&mButtons[0]);
    for (int j = 0; j < STAR_COUNT; j++) {
        Sprite_Draw(&mStars[j]);
    }
}

void InformationScene::Exit() {
    func_02028108(&data_02121834, 1, 0);
    ArchiveManager_Unmount(&gArchiveManager, "N83");
    ArchiveManager_Unmount(&gArchiveManager, "TET");
}

// The age of someone born on that day, as of mDate; -1 if the year is not known
int InformationScene::GetAge(int year, int month, int day) {
    if (year == BIRTH_YEAR_UNKNOWN) {
        return -1;
    }
    int thisYear = mDate.year;
    int thisMonth = mDate.month;
    if (mDate.day < day) {
        thisMonth--;
    }
    if (thisMonth < month) {
        thisYear--;
    }
    int age = thisYear - year;
    if (age < 0) {
        age = -1;
    }
    return age;
}

// The greatest common divisor, by Euclid's algorithm
int InformationScene::GetCommonDivisor(int a, int b) {
    while (a % b != 0) {
        int last = b;
        b = a % b;
        a = last;
    }
    return b;
}

// The number of stars of how comfortable the residents find the mansion
int InformationScene::GetComfort(int sum, int count) {
    int stars = (sum / count + (sum - count) / 2) / 10 + 1;
    if (stars < 0) {
        stars = 0;
    }
    return stars;
}

InformationScene::~InformationScene() {}
