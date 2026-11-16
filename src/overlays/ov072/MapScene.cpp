#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "sound/SoundManager.h"

#include "MapScene.h"

// Overlay 72: the island map (MapScene). It is one source file: its small constants share one pool at the start of
// .rodata that functions throughout the overlay read by offset, its tables of strings share one in .data, and the
// scene's implicit destructor, emitted with the scene's virtual table after the file's own functions, is at the
// very end of .text.

// --- The file's constants ---


// A view of the title mode's camera
struct TitleView {
    /* 0x00 */ VecFx32 mPosition;
    /* 0x0C */ VecFx32 mTarget;
    /* 0x18 */ s8 mTurnY;  // Added to the camera's angles each frame
    /* 0x19 */ s8 mTurnX;
    /* 0x1A */ s8 mSpeed;  // How far the camera moves towards the target each frame
    /* 0x1C */ u16 mFrames;
}; // 0x20

// The fountain's events by the time of day: until mTime (minutes since midnight) the event is mKind
struct FountainTime {
    int mKind;
    u16 mTime;
};

// What features 20 to 24 give the player, and the news each makes
static const int sFeatureItems[5] = { 0xA4, 0xA5, 0xA6, 0xA7, 0xA8 };
// The features of the city hall and of the ranking board: their "new" marks show on the building
static const int sCityHallFeatures[2] = { 5, 12 };
static const VecFx32 sLightDirection1 = { 0, -4000, 1500 };
static const int sFeatureNews[5] = { 0xDE, 0xDF, 0xE0, 0xE1, 0xE2 };
// From the camera to what it looks at
static const VecFx32 sCameraLookOffset = { 409, -5037, -25395 };
static const FountainTime sFountainTimes[14] = {
    { 4, 120 }, { 3, 390 }, { 0, 690 }, { 3, 720 }, { 2, 780 }, { 3, 840 }, { 4, 900 },
    { 1, 960 }, { 3, 990 }, { 4, 1140 }, { 1, 1200 }, { 3, 1260 }, { 2, 1380 }, { 3, 1440 },
};
static const Point sMoneySpritePosition = { 193, 180 };
// The sounds of the surroundings that are the same in every season
static const int sCommonSounds[4] = { 12, 13, 14, 15 };
static const ScreenRect sTitleWindowRect = { 20, 20, 216, 152 };
static const WindowPoint sTutorialButtonPosition = { 128, 149 };
static const Point sBackButtonSize = { 24, 22 };
static const VecFx32 sCameraHomeTarget = { 0, -327, 0 };
static const Point sButtonSizes[3] = { { 80, 22 }, { 80, 22 }, { 80, 22 } };
static const char sWindowDownScreen[] = "S00:/Window_down.nsc";
// From a building to where the camera looks at it from
static const VecFx32 sCameraZoomOffset = { -136, 1979, 8465 };
static const VecFx32 sCameraHome = { -409, 4710, 25395 };
static const int sFeatureMessages[4] = { 0x71, 0xA9, 0xAA, 0xAB };
static const Point sMoneyTextPosition = { 172, 174 };
static const u16 sButtonKinds[3] = { 4, 4, 4 };
// The steps of the music's ramps: volume, pitch, tempo ratio, pan
static const int sMusicRampSteps[4] = { 1, 1, 2, 1 };
static const u16 sButtonMessages[3] = { 0x93, 0x94, 0x95 };
static const ScreenRect sTutorialWindowRect = { 20, 20, 216, 152 };
static const u16 sBackButtonSequences[1] = { 0 };
static const u16 sButtonSequences[3] = { 0x12, 0x12, 0x12 };
// The ground's model by month
static const u8 sMonthGrounds[13] = { 0, 3, 3, 0, 1, 0, 0, 0, 0, 0, 2, 2, 3 };
static const Point sBackButtonPosition = { 242, 13 };
static const int sButtonFonts[3] = { 2, 2, 2 };
static const ScreenRect sWindowTextureRect6 = { 0, 0, 32, 32 };
static const int sTutorialButtonUnknown[4] = { 10, 1, 0, -1 };
static const WindowPoint sTitleTextPosition = { 48, 75 };
static const u16 sClockSequences[MapClockDisplay::SPRITE_COUNT] = { 0, 0, 2, 0, 0, 3, 0, 0, 1, 0, 0 };
static const VecFx32 sLightDirection0 = { 800, -600, -4000 };
static const ButtonSounds sBackButtonSounds = { { -1, -1, 0, -1 } };
static const ScreenRect sWindowTextureRect = { 0, 0, 32, 32 };
static const int sEffectResources[3] = { 1, 2, 0 };
// The sequence archive of the surroundings of each season
static const int sSeasonSoundArchives[5] = { 4, 5, 5, 6, 7 };
static const int sSeasonSoundGroups[5] = { 0x47, 0x48, 0x48, 0x49, 0x4A };
static const char sWindowTitleScreen[] = "S00:/Window_title.nsc";
// The building of each model, and the resource each model is drawn with
static const int sModelBuildings[MapScene::MODEL_COUNT] = { 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
static const Point sButtonPositions[3] = { { 128, 153 }, { 64, 153 }, { 192, 153 } };
static const int sRankingFeatures[7] = { 6, 13, 14, 15, 16, 17, 18 };
// Where the apartment building's mark is, by the island's size
static const Point sApartmentMarkPositions[4] = { { 128, 40 }, { 128, 40 }, { 128, 26 }, { 128, 10 } };
static const ButtonSounds sButtonSounds[3] = { { { 11, -1, 0, -1 } }, { { 11, -1, 0, -1 } }, { { 11, -1, 0, -1 } } };
// The model of each building
static const int sBuildingModels[12] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 };
static const VecFx32 sModelPositions[MapScene::MODEL_COUNT] = {
    { 0, 0, 0 },          { -696, -1146, 6553 }, { -2129, -1146, 4915 }, { 122, -1146, 4505 },
    { -3235, -491, 2580 }, { -983, -368, 2416 },  { 2048, -327, 2048 },   { 3481, -327, 1966 },
    { 0, 0, 0 },          { -2990, 368, 655 },   { 3194, 204, 204 },     { -2498, 1146, -1433 },
    { 2048, 1146, -1761 },
};
// The message of each building's window, when it is built and when it is not
static const int sBuildingMessages[12] = { 0xA5, 0x9C, 0x99, 0xA1, 0xA3, 0xA0, 0x9B, 0xA2, 0x97, 0x9E, 0x96, 0x98 };
static const int sBoardMessages[12] = { 0xAF, 0xAC, 0xAA, 0x96, 0xAE, 0x96, 0xAB, 0x96, 0xA8, 0xAD, 0xA7, 0xA9 };
// The light colours, and the window pictures of the title, of a building that is not built and of the fountain
// before it can be used
static GXRgb sLightColors[2] = { GX_RGB(31, 31, 31), GX_RGB(31, 31, 31) };
static const Point sClockPositions[MapClockDisplay::SPRITE_COUNT] = {
    { 160, 8 }, { 167, 8 }, { 174, 8 }, { 181, 8 }, { 188, 8 }, { 205, 8 },
    { 222, 8 }, { 229, 8 }, { 236, 8 }, { 243, 8 }, { 250, 8 },
};
static const int sModelResources[MapScene::MODEL_COUNT] = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
// The sounds of the surroundings that play long
static const int sLongSounds[4] = { 0, 2, 3, 0 };
static const WindowPoint sTutorialTextPosition = { 38, 33 };
static const WindowPoint sTutorialButtonSize = { 94, 30 };
static const WindowPoint sBoardTextPosition = { 48, 54 };
static const TitleView sTitleViews[10] = {
    { { -23756, 6553, 27033 }, { 0, 0, 0 }, 30, 0, 20, 900 },
    { { 18841, 31948, 10240 }, { -819, -2457, 819 }, -10, 0, 20, 1080 },
    { { 4915, 5734, 12288 }, { 0, 0, 0 }, -10, 0, -30, 900 },
    { { -2457, 0, 19660 }, { 0, 0, 0 }, 8, -5, 0, 900 },
    { { 0, 4915, 38092 }, { 0, 819, 0 }, -5, -2, 28, 900 },
    { { 0, 11468, 16384 }, { 0, 0, 0 }, 0, 5, 0, 1080 },
    { { 0, 1228, 8192 }, { 0, 819, 819 }, 0, -10, -15, 1080 },
    { { 25804, 25804, 12288 }, { -819, -819, 819 }, 5, 0, 15, 1080 },
    { { 12288, 819, 20480 }, { 0, 0, 1228 }, -20, 0, 10, 1080 },
    { { 4096, 1228, 9011 }, { 0, 0, 1228 }, -10, 0, -10, 1080 },
};
// The music's volume, pitch, tempo ratio and pan by model (0: none selected)
static const int sModelMusic[MapScene::MODEL_COUNT][4] = {
    { 70, -96, 192, 0 },  { 127, 64, 256, 0 },  { 120, 48, 240, -8 },  { 120, 48, 240, 8 },  { 120, 16, 224, -16 },
    { 85, -32, 216, -10 }, { 115, 16, 224, 4 }, { 115, 32, 224, 32 },  { 80, -32, 216, 0 },  { 110, 0, 224, -24 },
    { 110, 0, 224, 24 },  { 100, -24, 216, -16 }, { 100, -24, 216, 16 },
};
// The sounds of the surroundings by season and time of day, three each (0xFF: none)
static const u8 sSurroundingSounds[5][12] = {
    { 12, 13, 255, 0, 255, 255, 14, 255, 255, 15, 1, 255 },
    { 12, 13, 255, 0, 255, 255, 14, 0, 1, 15, 2, 3 },
    { 12, 13, 255, 0, 255, 255, 14, 0, 1, 15, 3, 255 },
    { 12, 13, 255, 255, 255, 255, 14, 0, 255, 15, 0, 255 },
    { 12, 13, 0, 255, 255, 255, 14, 1, 255, 15, 255, 255 },
};
// Where each building takes a touch, around its position on the screen
static const ScreenRect sModelTouchRects[MapScene::MODEL_COUNT] = {
    { 0, 0, 0, 0 },       { -12, -24, 20, 24 },  { -28, -36, 56, 36 }, { -16, -32, 32, 32 }, { -16, -32, 32, 32 },
    { -16, -32, 32, 32 }, { -24, -32, 44, 32 },  { -12, -32, 24, 28 }, { -38, -64, 70, 64 }, { -16, -32, 32, 32 },
    { -12, -56, 26, 56 }, { -18, -32, 36, 32 },  { -16, -36, 32, 36 },
};
static const char *sQuestionWindowArchive = "Scn/Map/SceneMap2WndQuestion_LZ.bin";
static const char *sQuestionScreen = "S01:/Fw_question.nsc";
static const char *sBoardPalette = "S01:/Fw_board.ncl";
static const char *sQuestionChars = "S01:/Fw_question.ncg";
static const char *sQuestionPalette = "S01:/Fw_question.ncl";
static const char *sBoardChars = "S01:/Fw_board.ncg";
static const char *sTitlePalette = "S01:/Fw_title.ncl";
static const char *sTitleWindowArchive = "Scn/Map/SceneMap2WndTitle_LZ.bin";
static const char *sBoardWindowArchive = "Scn/Map/SceneMap2WndBoard_LZ.bin";
static const char *sTitleChars = "S01:/Fw_title.ncg";
static const char *sBoardScreen = "S01:/Fw_board.nsc";
// Where the "new" mark of each building is
static const Point sNewMarkPositions[MapScene::MODEL_COUNT] = {
    { 0, 0 }, { 109, 133 }, { 68, 113 }, { 134, 116 }, { 44, 90 }, { 104, 89 }, { 181, 72 },
    { 217, 81 }, { 128, 40 }, { 56, 57 }, { 206, 34 }, { 71, 12 }, { 172, 9 },
};
static const char *sTitleScreen = "S01:/Fw_title.nsc";
// The ground's model by season, and the apartment building's by the island's size
static const char *sGroundModels[4] = {
    "Mdl/Map/map2_ground0_3D_LZ.bin",
    "Mdl/Map/map2_ground1_3D_LZ.bin",
    "Mdl/Map/map2_ground2_3D_LZ.bin",
    "Mdl/Map/map2_ground3_3D_LZ.bin",
};
static const char *sApartmentModels[5] = {
    "Mdl/Map/map2_apart0_3D_LZ.bin", "Mdl/Map/map2_apart1_3D_LZ.bin", "Mdl/Map/map2_apart2_3D_LZ.bin",
    "Mdl/Map/map2_apart3_3D_LZ.bin", "Mdl/Map/map2_apart4_3D_LZ.bin",
};
static const char *sBuildingScreens[12] = {
    "S01:/Fw_machine.nsc",   "S01:/Fw_dome.nsc",      "S01:/Fw_diagnosis.nsc", "S01:/Fw_shop_food.nsc",
    "S01:/Fw_fountain.nsc",  "S01:/Fw_cityhall.nsc",  "S01:/Fw_ranking.nsc",   "S01:/Fw_entrance.nsc",
    "S01:/Fw_shop_item.nsc", "S01:/Fw_news.nsc",      "S01:/Fw_shop_costume.nsc", "S01:/Fw_shop_reform.nsc",
};
static const char *sBuildingWindowArchives[12] = {
    "Scn/Map/SceneMap2WndMachine_LZ.bin",   "Scn/Map/SceneMap2WndDome_LZ.bin",
    "Scn/Map/SceneMap2WndDiagnosis_LZ.bin", "Scn/Map/SceneMap2WndShop_food_LZ.bin",
    "Scn/Map/SceneMap2WndFountain_LZ.bin",  "Scn/Map/SceneMap2WndCityhall_LZ.bin",
    "Scn/Map/SceneMap2WndRanking_LZ.bin",   "Scn/Map/SceneMap2WndEntrance_LZ.bin",
    "Scn/Map/SceneMap2WndShop_item_LZ.bin", "Scn/Map/SceneMap2WndNews_LZ.bin",
    "Scn/Map/SceneMap2WndShop_costume_LZ.bin", "Scn/Map/SceneMap2WndShop_reform_LZ.bin",
};
static const char *sBuildingPalettes[12] = {
    "S01:/Fw_machine.ncl",   "S01:/Fw_dome.ncl",      "S01:/Fw_diagnosis.ncl", "S01:/Fw_shop_food.ncl",
    "S01:/Fw_fountain.ncl",  "S01:/Fw_cityhall.ncl",  "S01:/Fw_ranking.ncl",   "S01:/Fw_entrance.ncl",
    "S01:/Fw_shop_item.ncl", "S01:/Fw_news.ncl",      "S01:/Fw_shop_costume.ncl", "S01:/Fw_shop_reform.ncl",
};
// The pictures and archives of each building's window
static const char *sBuildingChars[12] = {
    "S01:/Fw_machine.ncg",   "S01:/Fw_dome.ncg",      "S01:/Fw_diagnosis.ncg", "S01:/Fw_shop_food.ncg",
    "S01:/Fw_fountain.ncg",  "S01:/Fw_cityhall.ncg",  "S01:/Fw_ranking.ncg",   "S01:/Fw_entrance.ncg",
    "S01:/Fw_shop_item.ncg", "S01:/Fw_news.ncg",      "S01:/Fw_shop_costume.ncg", "S01:/Fw_shop_reform.ncg",
};
static const char *sResourceModels[14] = {
    "M00:/ground.imd",   "M13:/board.imd",     "M01:/machine.imd",   "M02:/dome.imd",    "M03:/diagnosis.imd",
    "M04:/shop_food.imd", "M05:/fountain.imd", "M06:/cityhall.imd", "M07:/ranking.imd", "M08:/apart.imd",
    "M09:/shop_item.imd", "M10:/news.imd",     "M11:/shop_costume.imd", "M12:/shop_interior.imd",
};
static const char *sResourcePaths[14] = {
    "Mdl/Map/map2_ground0_3D_LZ.bin",   "Mdl/Map/map2_board_3D_LZ.bin",
    "Mdl/Map/map2_machine_3D_LZ.bin",   "Mdl/Map/map2_dome_3D_LZ.bin",
    "Mdl/Map/map2_diagnosis_3D_LZ.bin", "Mdl/Map/map2_shop_food_3D_LZ.bin",
    "Mdl/Map/map2_fountain_3D_LZ.bin",  "Mdl/Map/map2_cityhall_3D_LZ.bin",
    "Mdl/Map/map2_ranking_3D_LZ.bin",   "Mdl/Map/map2_apart0_3D_LZ.bin",
    "Mdl/Map/map2_shop_item_3D_LZ.bin", "Mdl/Map/map2_news_3D_LZ.bin",
    "Mdl/Map/map2_shop_costume_3D_LZ.bin", "Mdl/Map/map2_shop_interior_3D_LZ.bin",
};
static const int sTitleFonts[1] = { 0 };
// The archive of each model resource: its name, its path and the model in it
static const char *sResourceArchives[14] = {
    "M00", "M13", "M01", "M02", "M03", "M04", "M05", "M06", "M07", "M08", "M09", "M10", "M11", "M12",
};

static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// What the clock means for the saved characters
static inline void UpdateCharacterTimes() {
    RTCTime time;
    RTCDate date;
    s64 now = func_02035870(data_02123440[0], &date, &time);
    func_020ad568(now);
    func_020ad514(now, &time);
    func_020ad294(now, &time);
}

static inline void ClearSceneCharacters() {
    for (u8 i = 0; i < 12; i++) {
        gSceneMemory.mCharacters[i] = 0xFF;
    }
}

static inline ScreenRect MoveRect(const ScreenRect &rect, int x, int y) {
    ScreenRect result;
    result.x = rect.x + x;
    result.y = rect.y + y;
    result.width = rect.width;
    result.height = rect.height;
    return result;
}

static inline ScreenRect MakeRect(int x, int y, int width, int height) {
    ScreenRect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    return rect;
}

// --- MapCamera ---

void MapCamera::Reset() {
    mGoal = mCamera->mPosition;
    mOffset.x = mGoalOffset.x = mCamera->mTarget.x - mCamera->mPosition.x;
    mOffset.y = mGoalOffset.y = mCamera->mTarget.y - mCamera->mPosition.y;
    mOffset.z = mGoalOffset.z = mCamera->mTarget.z - mCamera->mPosition.z;
    mFrame = 0;
    mMoving = FALSE;
}

void MapCamera::Update() {
    mMoving = FALSE;
    if (mFrame >= MOVE_FRAMES) {
        return;
    }
    mMoving = TRUE;
    fx32 t = (mFrame << FX32_SHIFT) / MOVE_FRAMES;
    t = FX_Mul(t, t);
    t = FX_Mul(t, t);
    mCamera->mPosition.x += FX_Mul(mGoal.x - mCamera->mPosition.x, t);
    mCamera->mPosition.y += FX_Mul(mGoal.y - mCamera->mPosition.y, t);
    mCamera->mPosition.z += FX_Mul(mGoal.z - mCamera->mPosition.z, t);
    mOffset.x += FX_Mul(mGoalOffset.x - mOffset.x, t);
    mOffset.y += FX_Mul(mGoalOffset.y - mOffset.y, t);
    mOffset.z += FX_Mul(mGoalOffset.z - mOffset.z, t);
    mCamera->mTarget.x = mCamera->mPosition.x + mOffset.x;
    mCamera->mTarget.y = mCamera->mPosition.y + mOffset.y;
    mCamera->mTarget.z = mCamera->mPosition.z + mOffset.z;
    mFrame++;
}

void MapCamera::MoveTo(VecFx32 position) {
    MoveTo(position, sCameraLookOffset, 13);
}

void MapCamera::MoveTo(VecFx32 position, VecFx32 offset, int frame) {
    mFrame = frame;
    mGoal.x = position.x;
    mGoal.y = position.y;
    mGoal.z = position.z;
    mGoalOffset.x = offset.x;
    mGoalOffset.y = offset.y;
    mGoalOffset.z = offset.z;
    mMoving = TRUE;
}

BOOL MapCamera::IsAtHome() {
    return mGoal.x == sCameraHome.x && mGoal.y == sCameraHome.y && mGoal.z == sCameraHome.z;
}

// --- MapScene ---

inline void MapScene::ResetCamera() {
    mCamera.mCamera = &data_02122764;
    func_0202a640(mCamera.mCamera);
    mCamera.mCamera->mPosition = sCameraHome;
    mCamera.mCamera->mTarget = sCameraHomeTarget;
    mCamera.mCamera->mAngleX = 0;
    mCamera.mCamera->mAngleY = 0;
    mCamera.mCamera->mAngleZ = 0;
    mCamera.mCamera->mUnknown0C = 4;
    mCamera.Reset();
}

void MapScene::StartLoading() {
    mClock.Update();
    UpdateCharacterTimes();
    switch (mSceneId) {
    case 0x62:
        mIslandStage = 0;
        mWindowCount = GetApartmentStageRooms(0);
        break;
    case 0x63:
        mIslandStage = 1;
        mWindowCount = GetApartmentStageRooms(1);
        break;
    case 0x64:
        mIslandStage = 2;
        mWindowCount = GetApartmentStageRooms(2);
        break;
    case 0x65:
        mIslandStage = 3;
        mWindowCount = GetApartmentStageRooms(3);
        break;
    case 0x66:
        mIslandStage = 4;
        mWindowCount = GetApartmentStageRooms(4);
        break;
    default: {
        SavedCharacter character;
        for (int i = 0; i < RESIDENT_COUNT; i++) {
            if (func_0202c3dc(&gGameData, i)) {
                func_0202c264(&gGameData, (u16)i, &character);
                mResidentRooms[i] = character.mRoom;
            } else {
                mResidentRooms[i] = -1;
            }
        }
        for (int i = 0; i < RESIDENT_COUNT; i++) {
            mRoomResidents[i] = -1;
        }
        for (int i = 0; i < RESIDENT_COUNT; i++) {
            if (mResidentRooms[i] != -1) {
                mRoomResidents[mResidentRooms[i]] = i;
            }
        }
        mFacilities.Init(NULL, NULL);
        mNewBuilding = mFacilities.GetNewFacility(mSceneId == 0x6A);
        if (mNewBuilding != -1) {
            func_02039c0c(&gIsland, mNewBuilding, 2);
        }
        break;
    }
    }
    switch (mSceneId) {
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
        break;
    default:
        mIslandStage = gGameData.mUnknown0C[0x40];
        break;
    }
    mWindowCount = GetApartmentRooms();
    for (int i = 0; i < 14; i++) {
        if (i == 0) {
            ArchiveManager_MountCompressed(&gArchiveManager, sResourceArchives[i],
                                           sGroundModels[sMonthGrounds[mClock.mDate.month]], 0);
        } else if (i == 9) {
            ArchiveManager_MountCompressed(&gArchiveManager, sResourceArchives[i], sApartmentModels[mIslandStage],
                                           0);
        } else {
            ArchiveManager_MountCompressed(&gArchiveManager, sResourceArchives[i], sResourcePaths[i], 0);
        }
    }
    ArchiveManager_MountCompressed(&gArchiveManager, "S00", "Scn/Scene/SceneMap2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "S01", sTitleWindowArchive, 0);
    mLastBuilding = 0;
    ArchiveManager_MountCompressed(&gArchiveManager, "TCO", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TET", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SKY", "Scn/Scene/Sky_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SPL", "Mdl/Spl/SplMapEffect_LZ.bin", 0);
}

// The second light's colour goes through a local: stored straight from the call, the back button's base (this +
// 0x268) is computed one instruction earlier than in the original.
void MapScene::Init() {
    const int unknownE = 0xE;
    const u16 backButtonKind = 4;
    const int titleTextCount = 1;
    const int titleMessage = 0x96;
    mNewsDue = FALSE;
    func_0202b0e8(&data_02122824);
    data_0211d604.SetUnknown44(0);
    data_0211d604.Show(1);
    data_0211d604.SetUnknown48(1);
    func_0202ae38(data_021223e4);
    func_02027290(&gObjResourceManager, "S00:/Obj_down.nce", 0x10, 1);
    func_02028f24(&gVramTransferQueue, "S00:/Obj_down.ncg", 0x10, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "S00:/Obj_down.ncl", 0x10, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "S00:/Obj_up.nce", "S00:/Obj_up.ncg", 0x1000, 0, 0x8000);
    func_02027534(&data_021213b8, "S00:/Obj_up.ncl", 0x1000, -1, -1, -1);
    for (int i = 0; i < MODEL_COUNT; i++) {
        Sprite_Init(&mNewMarks[i], 1, 0x19, 128, 96, TRUE);
        if (i == MODEL_APARTMENT) {
            const Point &mark = sApartmentMarkPositions[mIslandStage];
            mNewMarks[i].SetPositionFromVec(mark.x << FX32_SHIFT, mark.y << FX32_SHIFT);
        } else {
            mNewMarks[i].SetPositionFromVec(sNewMarkPositions[i].x << FX32_SHIFT, sNewMarkPositions[i].y << FX32_SHIFT);
        }
        mNewMarks[i].mVisible = FALSE;
    }
    mClockDisplay.Init(&mClock);
    mClockDisplay.SetState(MapClockDisplay::STATE_SHOWN);
    Sprite_Init(&mTitleSprite, 1, 0x1A, 128, 96, TRUE);
    mTitleSprite.mVisible = FALSE;
    Sprite_Init(&mSprite41BC, 1, 0x1C, 128, 96, TRUE);
    mSprite41BC.mVisible = FALSE;
    func_02028f24(&gVramTransferQueue, "S00:/BG_down.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "S00:/BG_down.ncl", 2, -1, -1, -1);
    func_02027d7c(&data_02121834, "S00:/Window_down.nsc", 2, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, sTitleChars, 0x2F00, 0, 0, 0x6000, -1);
    func_02027d7c(&data_02121834, sTitleScreen, 0x100, 0, 0, 0, 0, -1);
    func_02027534(&data_021213b8, sTitlePalette, 0x100, 0x20, 0x20, 0x40);
    func_02028f24(&gVramTransferQueue, "SKY:/BG_up.ncg", 0x2F00, 0x6000, 0x6000, 0x2000, -1);
    func_02027d7c(&data_02121834, "SKY:/BG_up.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "SKY:/Obj_up.ncg", 0x1000, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "SKY:/Obj_up.nce", 0x1000, 3);
    mSky.Init("SKY", 3, 0x1000, 0x200, 1, 1, 1, 1, 1);
    mSky.Update();
    G3X_SetClearColor(mSky.mBgPalette[1], 31, 0x7FFF, 63, FALSE);
    func_0202b4d8(&data_021232e4);
    func_0202b4dc(&data_021232e4, 0, "S00:/w_live.tga", 3, 32, 32);
    func_0202b4dc(&data_021232e4, 1, "S00:/w_pinch.tga", 3, 32, 32);
    func_0202b4dc(&data_021232e4, 2, "S00:/w_vacant.tga", 3, 32, 32);
    func_0202b4dc(&data_021232e4, 3, "S00:/w_dark.tga", 3, 32, 32);
    func_0202b4dc(&data_021232e4, 4, "S00:/w_depressed.tga", 3, 32, 32);
    func_0202b4dc(&data_021232e4, 5, "S00:/w_love.tga", 3, 32, 32);
    func_0202b4dc(&data_021232e4, 6, "S00:/w_game.tga", 3, 32, 32);
    for (int i = 0; i < mWindowCount; i++) {
        mWindows[i].mVisible = FALSE;
    }
    UpdateWindows();
    for (int i = 0; i < 14; i++) {
        ModelResources_SetModel(data_021223e4, i, sResourceModels[i]);
    }
    ModelResources_SetModel(data_021223e4, 0, "M00:/wait.ita");
    ModelResources_SetModel(data_021223e4, 0, "M00:/wait.ima");
    ModelResources_SetModel(data_021223e4, 11, "M10:/new.itp");
    ModelResources_SetModel(data_021223e4, 6, "M05:/wait.ita");
    func_0202680c(0x4000);
    mParticles = func_0207ab20(func_02026824, 3, 100, 0, 1, 30);
    mParticleResource = ArchiveManager_GetFileByName(&gArchiveManager, "SPL:/SplMapEffect.spa");
    func_0207ac78(mParticles, mParticleResource);
    func_0207b110(mParticles);
    func_0207b124(mParticles);
    for (int i = 0; i < 3; i++) {
        mEmitters[i] = NULL;
    }
    mModels[0].Load(0, 0);
    mModels[0].SetPosition(0, 0, 0);
    mModels[0].SetAnimation(RESOURCE_BTA0, 0);
    mModels[0].SetAnimation(RESOURCE_BMA0, 0);
    for (int i = 1; i < MODEL_COUNT; i++) {
        switch (func_02039c00(&gIsland, sModelBuildings[i])) {
        case 0:
        case 3:
            mModels[i].Load(1, 0);
            break;
        case 2:
            if (mNewBuilding == sModelBuildings[i]) {
                if (i == MODEL_APARTMENT) {
                    if (mFacilities.mResidents == 1) {
                        mModels[i].Load(1, 0);
                        for (int j = 0; j < mWindowCount; j++) {
                            mWindows[j].mVisible = FALSE;
                        }
                    } else {
                        ShowBuilding(i);
                    }
                } else {
                    mModels[i].Load(1, 0);
                }
            } else {
                ShowBuilding(i);
            }
            break;
        case 1:
            ShowBuilding(i);
            break;
        }
        mModels[i].SetPosition(sModelPositions[i].x, sModelPositions[i].y, sModelPositions[i].z);
    }
    ResetCamera();
    mSelected = -1;
    mUnknown384C = -1;
    mLights[0] = &data_021227cc;
    func_0202a96c(mLights[0]);
    mLights[0]->mColor = sLightColors[1];
    mLights[0]->SetDirection(mCamera.mCamera, sLightDirection0);
    mLights[1] = &data_021227d8;
    func_0202a96c(mLights[1]);
    mLights[1]->mColor = sLightColors[0];
    mLights[1]->SetDirection(mCamera.mCamera, sLightDirection1);
    int phase = mSky.GetPhase();
    mLights[0]->mColor = GetSkyLightColor(phase);
    GXRgb color = GetSkyLightColor2(phase);
    mLights[1]->mColor = color;

    for (int i = 0; i < 1; i++) {
        Sprite_Init(&mBackButtons[i], 1, 0, 128, 96, TRUE);
        func_02031eb0(&mBackButtons[i], sBackButtonSize.x, sBackButtonSize.y);
    }
    mBackButtons[0].SetPositionFromVec(sBackButtonPosition.x << FX32_SHIFT, sBackButtonPosition.y << FX32_SHIFT);
    for (int i = 0; i < 1; i++) {
        func_02031260(&mBackButtons[i], backButtonKind);
        func_02031278(&mBackButtons[i], sBackButtonSequences[i], sBackButtonSequences[i] + 1);
        func_02032158(&mBackButtons[i], &sBackButtonSounds);
        func_0203119c(&mBackButtons[i]);
        mBackButtons[i].mVisible = FALSE;
    }
    for (int i = 0; i < 3; i++) {
        Sprite_Init(&mButtons[i], 1, 0, 128, 96, TRUE);
        func_02031eb0(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        mButtons[i].SetPositionFromVec(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT);
        func_02031260(&mButtons[i], sButtonKinds[i]);
        func_02032158(&mButtons[i], &sButtonSounds[i]);
        func_02031278(&mButtons[i], sButtonSequences[i], sButtonSequences[i] + 1);
        func_0203119c(&mButtons[i]);
        mButtons[i].mVisible = FALSE;
    }
    mMessageFiles[0].Open("TCO:tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("TET:tmd_etc_msg_JP.bmg");
    int charOffset = 200;
    int titleCharOffset = 400;
    for (u16 i = 0; i < 3; i++) {
        charOffset += func_02031f58(&mButtons[i], &mMessageFiles[1], sButtonMessages[i], &gFonts[sButtonFonts[i]].mFont,
                                    0x10, charOffset);
    }
    func_02032fb8(&mMessageWindows[0], sTitleWindowRect);
    func_02033174(&mMessageWindows[0], sWindowTitleScreen, 0x100);
    mMessageWindows[0].mUnknownAF9C = 0;
    MessageFile *files = mMessageFiles;
    for (int i = 0; i < titleTextCount; i++) {
        func_02033a58(&mMessageWindows[0], &files[1], titleMessage, &gFonts[sTitleFonts[i]].mFont, 0x1000,
                      sTitleTextPosition, &titleCharOffset);
    }
    func_02032fb8(&mMessageWindows[1], sTutorialWindowRect);
    func_02033174(&mMessageWindows[1], sWindowDownScreen, 2);
    func_020337b8(&mMessageWindows[1], 0, 1, 0x14, 0x15, sTutorialButtonPosition, sTutorialButtonSize,
                  &mMessageFiles[0], 4, &gFonts[2].mFont, 0x10, &charOffset, sTutorialButtonUnknown);
    func_02033a58(&mMessageWindows[1], &mMessageFiles[1], 0x152, &gFonts[0].mFont, 0x10, sTutorialTextPosition,
                  &charOffset);
    func_02034f94(&mMessageWindows[1], 1);
    mMoney = gGameData.mCappedCounter;
    mMoneyDisplay.Init(&mMoney, &charOffset);
    func_02028280(&data_02121834, 0x10, 0);
    func_02028280(&data_02121834, 1, 2);
    func_02028280(&data_02121834, 2, 1);
    func_02028280(&data_02121834, 0x100, 0);
    func_02028280(&data_02121834, 0x1000, 0);
    func_02028280(&data_02121834, 0x200, 3);
    data_0211d604.Set(0x1211);
    switch (mSceneId) {
    case 0x6D:
    case 0x6E:
        mSound.Init(&mClock, MapSound::MODE_UNKNOWN3);
        SetState(STATE_TUTORIAL);
        break;
    case 0x6A:
        mSound.Init(&mClock, MapSound::MODE_RESTART);
        SetState(STATE_TITLE);
        break;
    default:
        mSound.Init(&mClock, MapSound::MODE_START);
        Deselect(FALSE, 1);
        if (mNewBuilding == -1) {
            SetState(STATE_MAP);
        } else {
            SetState(STATE_NEW_BUILDING);
        }
        break;
    }
    NNS_GfdDumpLnkTexVramManager();
    func_02060c1c();
    mIdleFrames = 0;
}

// Not matching yet (0x1148 bytes against 0x1144). With the camera through a pointer in the touch loop, the
// rectangle table's address is loaded in each branch as in the original and the frame is the original's; what
// differs: the registers of &mCamera and of the division's constant in the touch loop, with the order of the
// rectangle arithmetic.
void MapScene::Update() {
    mClock.UpdateIfArchiveIdle();
    mClockDisplay.Update();
    mMoneyDisplay.Update();
    for (int i = 0; i < 2; i++) {
        func_020341d4(&mMessageWindows[i]);
    }
    if (mSky.mChanged) {
        int phase = mSky.GetPhase();
        mLights[0]->mColor = GetSkyLightColor(phase);
        mLights[1]->mColor = GetSkyLightColor2(phase);
    }
    switch (mState) {
    case STATE_TITLE:
        if (gTouchInput.mTouched) {
            SetState(STATE_LEAVE_TITLE);
        }
        mTitleCamera.Update();
        break;
    case STATE_LEAVE_TITLE:
        if (func_02024118(&data_0211d604, 0x3F3F, 1)) {
            mSound.SetMode(&mClock, MapSound::MODE_CONTINUE);
            ResetCamera();
            Deselect(FALSE, 0);
            if (mNewBuilding == -1) {
                SetState(STATE_MAP);
            } else {
                SetState(STATE_NEW_BUILDING);
            }
        }
        break;
    case STATE_TUTORIAL:
        mCamera.Update();
        switch (mTutorialStep) {
        case 1:
        case 3:
        case 4:
        case 6:
        case 8:
        case 11:
        case 15:
        case 18:
        case 21:
        case 22:
        case 23:
            if (mMessageWindows[1].mResult != -1) {
                mMessageWindows[1].mUnknownSetBeforeClose = 15;
                func_020349cc(&mMessageWindows[1], 0x24);
            }
            if (mMessageWindows[1].mTransitionDone) {
                SetTutorialStep(mTutorialStep + 1);
            }
            break;
        case 2:
        case 5:
        case 24:
            if (mMessageWindows[1].mResult != -1) {
                mMessageWindows[1].mUnknownSetBeforeClose = 15;
                func_020349cc(&mMessageWindows[1], 0x24);
            }
            if (mMessageWindows[1].mTransitionDone) {
                func_0203c02c(&gSceneManager, 0x69);
            }
            break;
        case 0:
            if (mTutorialFrames++ > 120) {
                SetTutorialStep(mTutorialStep + 1);
            }
            break;
        case 9:
        case 12:
        case 16:
        case 19:
            if (mTutorialFrames++ > 40) {
                SetTutorialStep(mTutorialStep + 1);
            }
        case 10:
        case 13:
        case 17:
        case 20:
            for (int i = 0; i < 3; i++) {
                if (func_0203133c(&mButtons[i]) && i == 0) {
                    func_0203c02c(&gSceneManager, 0x69);
                }
            }
            break;
        case 7:
            if (++mFrames == 30) {
                StartEffects(&sModelPositions[8]);
            } else if (mFrames == 45) {
                ShowBuilding(MODEL_APARTMENT);
                gSoundManager.StartSeqArc(1, 3, 10);
            } else if (mFrames > 180) {
                SetTutorialStep(mTutorialStep + 1);
            }
            break;
        case 14:
            if (++mFrames == 30) {
                StartEffects(&sModelPositions[4]);
            } else if (mFrames == 45) {
                ShowBuilding(4);
                gSoundManager.StartSeqArc(1, 3, 10);
            } else if (mFrames > 180) {
                SetTutorialStep(mTutorialStep + 1);
            }
            break;
        }
        break;
    case STATE_NEW_BUILDING: {
        func_02024004(&data_0211d604, 0x3F3F, -1);
        mCamera.Update();
        int building = mNewBuilding;
        int news;
        if (building != -1 && building <= 11) {
            if (mFrames == 30) {
                if (building != -1 && building <= 11) {
                    StartEffects(&sModelPositions[sBuildingModels[building]]);
                }
            } else if (mFrames == 45) {
                gSoundManager.StartSeqArc(1, 3, 10);
            } else if (mFrames == 60) {
                for (int i = 1; i < MODEL_COUNT; i++) {
                    if (mNewBuilding == sModelBuildings[i]) {
                        if (i == MODEL_APARTMENT) {
                            int stages = GetApartmentStage(mFacilities.mResidents);
                            if (mIslandStage < stages) {
                                mIslandStage++;
                            }
                            gGameData.mUnknown0C[0x40] = mIslandStage;
                            mWindowCount = GetApartmentStageRooms(mIslandStage);
                            const Point &mark = sApartmentMarkPositions[mIslandStage];
                            mNewMarks[MODEL_APARTMENT].SetPositionFromVec(mark.x << FX32_SHIFT, mark.y << FX32_SHIFT);
                            func_0202aecc(data_021223e4, 9);
                            ArchiveManager_Unmount(&gArchiveManager, sResourceArchives[9]);
                            ArchiveManager_MountCompressed(&gArchiveManager, sResourceArchives[9],
                                                           sApartmentModels[mIslandStage], 1);
                            ModelResources_SetModel(data_021223e4, 9, sResourceModels[9]);
                            ShowBuilding(i);
                            UpdateWindows();
                        } else {
                            ShowBuilding(i);
                        }
                    }
                }
            } else if (mFrames == 120) {
                gSoundManager.StartSeqArc(1, 3, 11);
                mSprite41BC.mVisible = TRUE;
                if (func_02039c00(&gIsland, 9) == 1) {
                    StartEffect2(&sModelPositions[10]);
                }
            } else if (mFrames > 280) {
                switch (building) {
                case 0:
                    news = 0x94;
                    break;
                case 1:
                    news = 0x50;
                    break;
                case 2:
                    news = 0x4F;
                    break;
                case 4:
                    news = 0x9C;
                    break;
                case 5:
                    news = 0x9E;
                    break;
                case 6:
                    news = 0x51;
                    break;
                case 7:
                    switch (mIslandStage) {
                    case 3:
                        news = 0x92;
                        break;
                    case 1:
                        news = 0x70;
                        break;
                    case 2:
                        news = 0x91;
                        break;
                    }
                    break;
                case 8:
                    news = 0x93;
                    break;
                case 9:
                    news = 0x4D;
                    break;
                case 10:
                    news = 0x52;
                    break;
                case 11:
                    news = 0x4E;
                    break;
                }
                ClearSceneCharacters();
                AddNews(news);
                func_0203c02c(&gSceneManager, 0x46);
                gSoundManager.StartSeqArc(2, 1, 11);
            }
        } else {
            if (mFrames == 1) {
                gSoundManager.StartSeqArc(1, 3, 11);
                mSprite41BC.mVisible = TRUE;
                if (func_02039c00(&gIsland, 9) == 1) {
                    StartEffect2(&sModelPositions[10]);
                }
            }
            if (mFrames == 160) {
                switch (mNewBuilding) {
                case 12:
                    news = 0x9B;
                    break;
                case 6:
                    news = 0x51;
                    break;
                case 13:
                    news = 0x95;
                    break;
                case 14:
                    news = 0x96;
                    break;
                case 15:
                    news = 0x97;
                    break;
                case 16:
                    news = 0x98;
                    break;
                case 17:
                    news = 0x99;
                    break;
                case 18:
                    news = 0x9A;
                    break;
                case 20:
                case 21:
                case 22:
                case 23:
                case 24: {
                    Inventory inventory;
                    inventory.Load();
                    inventory.mItems[sFeatureItems[mNewBuilding - 20]].Add(1);
                    inventory.Save();
                    news = sFeatureNews[mNewBuilding - 20];
                    break;
                }
                case 25:
                case 26:
                case 27:
                case 28:
                    news = sFeatureMessages[mNewBuilding - 25];
                    break;
                case 32:
                    if (*(u32 *)&gGameData.mUnknown54[0xB8] < 4) {
                        news = 0x9F;
                    } else {
                        news = 0xA0;
                        for (int i = 0; i < RESIDENT_COUNT; i++) {
                            if (func_0202c3dc(&gGameData, i)) {
                                SavedCharacter character;
                                func_0202c264(&gGameData, i, &character);
                                func_0209c44c(&character, -30);
                                func_0202c28c(&gGameData, i, &character);
                            }
                        }
                    }
                    break;
                }
                ClearSceneCharacters();
                AddNews(news);
                func_0203c02c(&gSceneManager, 0x46);
                gSoundManager.StartSeqArc(2, 1, 11);
            }
        }
        mFrames++;
        break;
    }
    case STATE_MAP: {
        mCamera.Update();
        if (!mCamera.mMoving) {
            if (++mIdleFrames == 20) {
                mNewsDue = IsNewsDue();
                if (func_02039c00(&gIsland, 9) == 1 || func_02039c00(&gIsland, 9) == 2) {
                    if (mNewsDue) {
                        if (func_0202a1f8(&mModels[10], RESOURCE_BTP0, 0)) {
                            mModels[10].SetAnimation(RESOURCE_BTP0, 0);
                        }
                    } else {
                        if (func_0202a1f8(&mModels[10], RESOURCE_BTP0, 0) != 1) {
                            mModels[10].SetAnimation(RESOURCE_BTP0, 1);
                        }
                    }
                }
            } else if (mIdleFrames == 40) {
                // UpdateCharacterTimes written out: the date and time are Update's own locals here (their stack
                // slots are above the touch loop's, which an inline's locals are not), the date first
                RTCDate date;
                RTCTime time;
                s64 now = func_02035870(data_02123440[0], &date, &time);
                func_020ad568(now);
                func_020ad514(now, &time);
                func_020ad294(now, &time);
            } else if (mIdleFrames >= 60) {
                UpdateWindows();
                mIdleFrames = 0;
            }
        }
        if (!func_02024004(&data_0211d604, 0x3F3F, -1)) {
            break;
        }
        if (data_0211f9d4.mTrigger & PAD_BUTTON_START) {
            func_0203c02c(&gSceneManager, 0x2C);
        }
        BOOL held = FALSE;
        for (int i = 0; i < 3; i++) {
            if (func_0203133c(&mButtons[i])) {
                switch (i) {
                case 0:
                    EnterBuilding(mSelected, mNewsDue, 0);
                    break;
                case 1:
                    EnterBuilding(mSelected, mNewsDue, 1);
                    break;
                case 2:
                    EnterBuilding(mSelected, mNewsDue, 2);
                    break;
                }
            }
            if (mButtons[i].mVisible && func_02031294(&mButtons[i])) {
                held = TRUE;
            }
        }
        if (!held) {
            for (int i = 0; i < 1; i++) {
                if (func_0203133c(&mBackButtons[i])) {
                    if (mCamera.IsAtHome()) {
                        gSceneMemory.mCharacters[0] = 0;
                        func_0203c02c(&gSceneManager, 0x11);
                    } else {
                        Deselect(TRUE, 1);
                    }
                }
                if (mBackButtons[i].mVisible && func_02031294(&mBackButtons[i])) {
                    held = TRUE;
                }
            }
        }
        ScreenPoint point = *(gTouchInput.mUnknown4C ? &gTouchInput.mLastPoint : &gTouchInput.mPoint);
        if (!gTouchInput.mTouched || held) {
            break;
        }
        for (int i = 1; i < MODEL_COUNT; i++) {
            if (!mModels[i].mVisible) {
                continue;
            }
            VecFx32 position;
            position.x = sModelPositions[i].x;
            position.y = sModelPositions[i].y;
            position.z = sModelPositions[i].z;
            int x, y;
            NNS_G3dWorldPosToScrPos(&position, &x, &y);
            ScreenRect rect;
            MapCamera *camera = &mCamera;
            if (camera->mMoving || camera->IsAtHome()) {
                rect = MoveRect(sModelTouchRects[i], x, y + 8);
            } else {
                rect = MakeRect(sModelTouchRects[i].x * 27 / 10 + x, sModelTouchRects[i].y * 27 / 10 + (y + 20),
                                sModelTouchRects[i].width * 27 / 10, sModelTouchRects[i].height * 27 / 10);
            }
            if (!func_02028d88(&rect, &point)) {
                continue;
            }
            if (i == mSelected) {
                u8 state = func_02039c00(&gIsland, sModelBuildings[mSelected]);
                if (!(state == 1 || state == 2)) {
                    continue;
                }
                if (mSelected == 3 && !mFacilities.HasTwoOfEachSex()) {
                    continue;
                }
                if (mSelected == 1 && !mFacilities.HasTwoWithBirthdays()) {
                    continue;
                }
                if (mSelected == 2 && !mFacilities.HasFourResidents()) {
                    continue;
                }
                if (mSelected == 10 && !mFacilities.HasSevenResidentsThreeOfEachSex()) {
                    continue;
                }
                EnterBuilding(mSelected, mNewsDue, 0);
                gSoundManager.StartSeqArc(2, 1, 11);
                break;
            }
            gSoundManager.StartSeqArc(1, 3, 1);
            VecFx32 goal = sCameraZoomOffset;
            goal.x += position.x;
            goal.y += position.y;
            goal.z += position.z;
            mCamera.MoveTo(goal);
            SelectBuilding(i);
            break;
        }
        break;
    }
    }
    switch (mWindowState) {
    case WINDOW_OPENING:
        if (mMessageWindows[0].mTransitionDone) {
            OpenWindow();
        }
        break;
    case WINDOW_OPEN:
        break;
    case WINDOW_CLOSED:
        if (mState == STATE_MAP && mWindowFrames++ == 30) {
            UpdateNewMarks();
            mMoneyDisplay.SetVisible(TRUE);
        }
        break;
    }
    mSky.Update();
    if (mSky.mChanged) {
        G3X_SetClearColor(mSky.mBgPalette[1], 31, 0x7FFF, 63, FALSE);
    }
    mSound.Update(&mClock);
}

void MapScene::Draw() {
    for (int i = 0; i < 2; i++) {
        func_020345d8(&mMessageWindows[i]);
    }
    mClockDisplay.Draw();
    mMoneyDisplay.Draw();
    func_02031d7c(&mBackButtons[0]);
    for (int i = 0; i < 3; i++) {
        func_0203206c(&mButtons[i]);
    }
    for (int i = 0; i < mWindowCount; i++) {
        func_0202b4a4(&mWindows[i]);
    }
    for (int i = 0; i < MODEL_COUNT; i++) {
        mModels[i].Draw();
    }
    for (int i = 0; i < MODEL_COUNT; i++) {
        Sprite_Draw(&mNewMarks[i]);
    }
    Sprite_Draw(&mTitleSprite);
    Sprite_Draw(&mSprite41BC);
    mSky.Draw();
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, data_02142438);
    func_0207b138(mParticles);
}

void MapScene::Exit() {
    GXRgb clearColor = mSky.mBgPalette[1];
    // A pointer cast from the byte buffer is kept in a register across the calls; the buffer's address is not.
    GXRgb *colors = (GXRgb *)data_0213bab0;
    int phase = mSky.GetPhase();
    colors[0] = clearColor;
    colors[1] = GetSkyLightColor(phase);
    colors[2] = GetSkyLightColor2(phase);
    for (int i = 0; i < 14; i++) {
        func_0202aecc(data_021223e4, i);
        ArchiveManager_Unmount(&gArchiveManager, sResourceArchives[i]);
    }
    ArchiveManager_Unmount(&gArchiveManager, "S00");
    ArchiveManager_Unmount(&gArchiveManager, "S01");
    ArchiveManager_Unmount(&gArchiveManager, "TCO");
    ArchiveManager_Unmount(&gArchiveManager, "TET");
    ArchiveManager_Unmount(&gArchiveManager, "SKY");
    func_0202683c();
    ArchiveManager_Unmount(&gArchiveManager, "SPL");
    mSound.Stop();
}

void MapScene::SetState(int state) {
    switch (state) {
    case STATE_TITLE:
        mMoneyDisplay.SetVisible(FALSE);
        mClockDisplay.SetVisible(FALSE);
        mTitleSprite.mVisible = TRUE;
        func_02034f38(&mMessageWindows[0], 0, 0xA5);
        func_02028f24(&gVramTransferQueue, sTitleChars, 0x2F00, 0, 0, 0x6000, -1);
        func_02027534(&data_021213b8, sTitlePalette, 0x100, 0x20, 0x20, 0x40);
        func_02033174(&mMessageWindows[0], sTitleScreen, 0x100);
        func_02034720(&mMessageWindows[0], 0x1A, 1);
        NNS_G2dCharCanvasClear(&mMessageWindows[0].mTexts[0].mCanvas, 0);
        func_020341d4(&mMessageWindows[0]);
        mTitleCamera.Start(mCamera.mCamera);
        for (int i = 1; i < MODEL_COUNT; i++) {
            mNewMarks[i].mVisible = FALSE;
        }
        break;
    case STATE_LEAVE_TITLE:
        gSoundManager.StartSeqArc(1, 3, 0);
        if (!func_0202cd54(&gGameData)) {
            func_0203c02c(&gSceneManager, 0xC2);
        }
        break;
    case STATE_TUTORIAL:
        mMoneyDisplay.SetVisible(FALSE);
        mClockDisplay.SetVisible(TRUE);
        switch (gSceneMemory.mTutorialStep) {
        case 2:
            Deselect(FALSE, 1);
            SetTutorialStep(0);
            break;
        case 4:
            Deselect(FALSE, 1);
            SetTutorialStep(3);
            func_02039c0c(&gIsland, 7, 3);
            break;
        case 7:
            Deselect(FALSE, 1);
            SetTutorialStep(6);
            func_02039c0c(&gIsland, 7, 1);
            break;
        case 15: {
            SavedCharacter first;
            SavedCharacter second;
            func_0202c264(&gGameData, 0, &first);
            func_0202c264(&gGameData, 1, &second);
            first.mEvent = 0;
            second.mEvent = 0;
            func_0202c28c(&gGameData, 0, &first);
            func_0202c28c(&gGameData, 1, &second);
            UpdateWindows();
            SetTutorialStep(11);
            func_02039c0c(&gIsland, 3, 3);
            break;
        }
        case 18:
            SetTutorialStep(14);
            func_02039c0c(&gIsland, 3, 1);
            break;
        case 20:
            SetTutorialStep(18);
            break;
        case 23:
            SetTutorialStep(21);
            break;
        }
        break;
    case STATE_NEW_BUILDING:
        mMoneyDisplay.SetVisible(FALSE);
        mClockDisplay.SetVisible(TRUE);
        mTitleSprite.mVisible = FALSE;
        break;
    case STATE_MAP:
        mMoneyDisplay.SetVisible(TRUE);
        mClockDisplay.SetVisible(TRUE);
        mTitleSprite.mVisible = FALSE;
        UpdateCharacterTimes();
        UpdateWindows();
        mIdleFrames = 0;
        break;
    }
    mState = state;
    mFrames = 0;
}

VecFx32 MapScene::GetWindowPosition(int window) {
    WindowCell cell = GetWindowCell(window);
    if (cell.mFloor == 12) {
        cell.mColumn += 2;
    }
    VecFx32 position;
    position.x = (cell.mColumn - 4) * 5 * FX32_CONST(0.01) + 102;
    position.y = cell.mFloor * 5 * FX32_CONST(0.01) + 430;
    position.z = 758;
    return position;
}

void MapScene::SelectBuilding(int model) {
    mWindowState = WINDOW_OPENING;
    mWindowFrames = 0;
    mSelected = model;
    mMessageWindows[0].mUnknownAF9C = 1;
    mMessageWindows[0].mUnknownSetBeforeClose = 5;
    func_020349cc(&mMessageWindows[0], 0x1C);
    ArchiveManager_Unmount(&gArchiveManager, "S01");
    int building = sModelBuildings[model];
    u8 state = func_02039c00(&gIsland, building);
    if (state == 0 || state == 3) {
        ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBoardWindowArchive, 1);
        func_02034f38(&mMessageWindows[0], 0, sBoardMessages[building]);
    } else {
        mHasInfo = TRUE;
        switch (model) {
        case 5:
            if (!IsNewDay()) {
                ArchiveManager_MountCompressed(&gArchiveManager, "S01", sQuestionWindowArchive, 1);
                func_02034f38(&mMessageWindows[0], 0, 0xA4);
                mHasInfo = FALSE;
            } else {
                ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBuildingWindowArchives[building], 1);
                func_02034f38(&mMessageWindows[0], 0, sBuildingMessages[building]);
            }
            break;
        case 3:
            ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBuildingWindowArchives[building], 1);
            if (!mFacilities.HasTwoOfEachSex()) {
                func_02034f38(&mMessageWindows[0], 0, 0x9A);
                mHasInfo = FALSE;
            } else {
                func_02034f38(&mMessageWindows[0], 0, sBuildingMessages[building]);
            }
            break;
        case 1:
            ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBuildingWindowArchives[building], 1);
            if (!mFacilities.HasTwoWithBirthdays()) {
                func_02034f38(&mMessageWindows[0], 0, 0xA6);
                mHasInfo = FALSE;
            } else {
                func_02034f38(&mMessageWindows[0], 0, sBuildingMessages[building]);
            }
            break;
        case 2:
            ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBuildingWindowArchives[building], 1);
            if (!mFacilities.HasFourResidents()) {
                func_02034f38(&mMessageWindows[0], 0, 0x9D);
                mHasInfo = FALSE;
            } else {
                func_02034f38(&mMessageWindows[0], 0, sBuildingMessages[building]);
            }
            break;
        case 10:
            ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBuildingWindowArchives[building], 1);
            if (!mFacilities.HasSevenResidentsThreeOfEachSex()) {
                func_02034f38(&mMessageWindows[0], 0, 0x9F);
                mHasInfo = FALSE;
            } else {
                func_02034f38(&mMessageWindows[0], 0, sBuildingMessages[building]);
            }
            break;
        default:
            ArchiveManager_MountCompressed(&gArchiveManager, "S01", sBuildingWindowArchives[building], 1);
            func_02034f38(&mMessageWindows[0], 0, sBuildingMessages[building]);
            break;
        }
    }
    func_0203119c(&mBackButtons[0]);
    mBackButtons[0].mVisible = TRUE;
    mButtons[0].mVisible = FALSE;
    mButtons[1].mVisible = FALSE;
    mButtons[2].mVisible = FALSE;
    mSound.SetBuilding(model);
    for (int i = 1; i < MODEL_COUNT; i++) {
        mNewMarks[i].mVisible = FALSE;
    }
}

void MapScene::OpenWindow() {
    mMoneyDisplay.SetVisible(FALSE);
    gSoundManager.StartSeqArc(1, 3, 3);
    mWindowState = WINDOW_OPEN;
    mWindowFrames = 0;
    mLastBuilding = mSelected;
    u8 state = func_02039c00(&gIsland, sModelBuildings[mSelected]);
    if (state == 0 || state == 3) {
        func_02028f24(&gVramTransferQueue, sBoardChars, 0x2F00, 0, 0, 0x6000, -1);
        func_02027534(&data_021213b8, sBoardPalette, 0x100, 0x20, 0x20, 0x40);
        func_02033174(&mMessageWindows[0], sBoardScreen, 0x100);
        mMessageWindows[0].SetTextPosition(0, sBoardTextPosition);
        func_02034720(&mMessageWindows[0], 0x1A, 1);
        return;
    }
    if (mHasInfo) {
        func_02028f24(&gVramTransferQueue, sBuildingChars[sModelBuildings[mSelected]], 0x2F00, 0, 0, 0x6000, -1);
        func_02027534(&data_021213b8, sBuildingPalettes[sModelBuildings[mSelected]], 0x100, 0x20, 0x20, 0x40);
        func_02033174(&mMessageWindows[0], sBuildingScreens[sModelBuildings[mSelected]], 0x100);
        mMessageWindows[0].SetTextPosition(0, sTitleTextPosition);
        func_02034720(&mMessageWindows[0], 0x1A, 1);
        switch (mSelected) {
        case 7:
        case 10:
            mButtons[0].mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mButtons[0].mText.mCanvas, 0);
            mButtons[0].mText.mMessage = 0xB4;
            mButtons[0].mText.DrawAll();
            break;
        case 2:
            mButtons[1].mVisible = TRUE;
            mButtons[2].mVisible = TRUE;
            break;
        default:
            mButtons[0].mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mButtons[0].mText.mCanvas, 0);
            mButtons[0].mText.mMessage = 0xB3;
            mButtons[0].mText.DrawAll();
            break;
        }
        return;
    }
    if (mSelected == 5) {
        func_02028f24(&gVramTransferQueue, sQuestionChars, 0x2F00, 0, 0, 0x6000, -1);
        func_02027534(&data_021213b8, sQuestionPalette, 0x100, 0x20, 0x20, 0x40);
        func_02033174(&mMessageWindows[0], sQuestionScreen, 0x100);
        mMessageWindows[0].SetTextPosition(0, sTitleTextPosition);
        func_02034720(&mMessageWindows[0], 0x1A, 1);
        mButtons[0].mVisible = TRUE;
        return;
    }
    func_02028f24(&gVramTransferQueue, sBuildingChars[sModelBuildings[mSelected]], 0x2F00, 0, 0, 0x6000, -1);
    func_02027534(&data_021213b8, sBuildingPalettes[sModelBuildings[mSelected]], 0x100, 0x20, 0x20, 0x40);
    func_02033174(&mMessageWindows[0], sBuildingScreens[sModelBuildings[mSelected]], 0x100);
    mMessageWindows[0].SetTextPosition(0, sTitleTextPosition);
    func_02034720(&mMessageWindows[0], 0x1A, 1);
    mButtons[0].mVisible = FALSE;
}

void MapScene::Deselect(BOOL sound, int unknown) {
    mWindowState = WINDOW_CLOSED;
    mWindowFrames = 0;
    mCamera.MoveTo(sCameraHome);
    mSelected = -1;
    mUnknown384C = -1;
    mBackButtons[0].mVisible = FALSE;
    mButtons[0].mVisible = FALSE;
    mButtons[1].mVisible = FALSE;
    mButtons[2].mVisible = FALSE;
    mMessageWindows[0].mUnknownAF9C = unknown;
    mMessageWindows[0].mUnknownSetBeforeClose = 0;
    func_020349cc(&mMessageWindows[0], 0x1C);
    if (sound) {
        gSoundManager.StartSeqArc(1, 3, 2);
    }
    mSound.SetBuilding(0);
}

void MapScene::SetTutorialStep(int step) {
    switch (step) {
    case 1:
        func_02034f38(&mMessageWindows[1], 0, 0x152);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 2:
        func_02034f38(&mMessageWindows[1], 0, 0x153);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 3: {
        func_02034f38(&mMessageWindows[1], 0, 0x154);
        wchar_t name[8];
        wcscpy(name, data_02123510);
        func_02034a3c(&mMessageWindows[1], 0, 0, name);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    }
    case 4:
        func_02034f38(&mMessageWindows[1], 0, 0x155);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 5:
        func_02034f38(&mMessageWindows[1], 0, 0x156);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 6: {
        SavedCharacter character;
        func_0202c264(&gGameData, 0, &character);
        wchar_t name[11];
        MiiData_GetName(&character, name);
        func_02034a3c(&mMessageWindows[1], 0, 0, name);
        func_02034f38(&mMessageWindows[1], 0, 0x157);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        int room = mResidentRooms[0];
        ScreenRect rect = sWindowTextureRect6;
        func_0202b1f0(&mWindows[room], 3, &rect);
        break;
    }
    case 8: {
        SavedCharacter character;
        func_0202c264(&gGameData, 0, &character);
        wchar_t name[11];
        MiiData_GetName(&character, name);
        func_02034a3c(&mMessageWindows[1], 0, 0, name);
        func_02034f38(&mMessageWindows[1], 0, 0x158);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    }
    case 9:
        gSoundManager.StartSeqArc(1, 3, 1);
        mCamera.MoveTo(sCameraZoomOffset);
        for (int i = 1; i < MODEL_COUNT; i++) {
            mNewMarks[i].mVisible = FALSE;
        }
        break;
    case 10:
        mButtons[0].mVisible = TRUE;
        break;
    case 11:
        func_02034f38(&mMessageWindows[1], 0, 0x16B);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 12:
        gSoundManager.StartSeqArc(1, 3, 1);
        mCamera.MoveTo(sCameraZoomOffset);
        break;
    case 15: {
        SavedCharacter character;
        func_0202c264(&gGameData, 1, &character);
        wchar_t name[11];
        MiiData_GetName(&character, name);
        func_02034a3c(&mMessageWindows[1], 0, 0, name);
        func_02034f38(&mMessageWindows[1], 0, 0x16E);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    }
    case 16: {
        gSoundManager.StartSeqArc(1, 3, 1);
        VecFx32 goal = sCameraZoomOffset;
        goal.x += sModelPositions[4].x;
        goal.y += sModelPositions[4].y;
        goal.z += sModelPositions[4].z;
        mCamera.MoveTo(goal);
        for (int i = 1; i < MODEL_COUNT; i++) {
            mNewMarks[i].mVisible = FALSE;
        }
        break;
    }
    case 13:
    case 17:
    case 20:
        mButtons[0].mVisible = TRUE;
        break;
    case 18: {
        SavedCharacter character;
        func_0202c264(&gGameData, 1, &character);
        wchar_t name[11];
        MiiData_GetName(&character, name);
        func_02034a3c(&mMessageWindows[1], 0, 0, name);
        func_02034f38(&mMessageWindows[1], 0, 0x16F);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    }
    case 19:
        gSoundManager.StartSeqArc(1, 3, 1);
        mCamera.MoveTo(sCameraZoomOffset);
        break;
    case 21: {
        func_02034f38(&mMessageWindows[1], 0, 0x177);
        wchar_t name[8];
        wcscpy(name, data_02123510);
        func_02034a3c(&mMessageWindows[1], 0, 0, name);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        mSound.SetMode(&mClock, MapSound::MODE_TUTORIAL);
        break;
    }
    case 22:
        func_02034f38(&mMessageWindows[1], 0, 0x178);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 23:
        func_02034f38(&mMessageWindows[1], 0, 0x179);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        break;
    case 24:
        func_02034f38(&mMessageWindows[1], 0, 0x17A);
        func_02034720(&mMessageWindows[1], 0x12, 1);
        func_02039c0c(&gIsland, 5, 3);
        break;
    }
    mTutorialStep = step;
    mTutorialFrames = 0;
}

// Whether a news item is due: the newest one is from before the last 7:00 or 19:00, by half a day or more
BOOL MapScene::IsNewsDue() {
    RTCDate date;
    RTCTime time;
    Clock_GetDate(data_02123440[0], &date);
    Clock_GetTime(data_02123440[0], &time);
    NewsItem item;
    func_0202c8a8(&gGameData, 0, &item);
    RTCDate itemDate;
    itemDate.year = item.mYear;
    itemDate.month = item.GetMonth();
    itemDate.day = item.GetDay();
    itemDate.week = RTC_WEEK_SUNDAY;
    BOOL dayBefore = FALSE;
    RTCTime itemTime;
    if (item.GetHour() >= 7 && item.GetHour() < 19) {
        itemTime.hour = 7;
    } else if (item.GetHour() < 7) {
        itemTime.hour = 19;
        dayBefore = TRUE;
    } else {
        itemTime.hour = 19;
    }
    itemTime.minute = 0;
    itemTime.second = 0;
    s64 edition = RTC_ConvertDateTimeToSecond(&itemDate, &itemTime);
    if (dayBefore) {
        edition -= 24 * 60 * 60;
    }
    s64 since = RTC_ConvertDateTimeToSecond(&date, &time) - edition;
    if (since < 0 || since >= 12 * 60 * 60) {
        return TRUE;
    }
    return FALSE;
}

// Not matching yet: the loop that picks a kind of news of the month allocates its registers differently (the
// original materialises the 1 of 1 << monthBit in the loop and keeps x of the inlined MATH_Rand16 apart), and the
// two stores of the other residents are scheduled differently. The rest, both shuffles included, is the original's.
// Makes the day's news: a birthday of a resident when there is one that the last birthday news was not about,
// otherwise a random kind of news of the month
void MapScene::MakeDailyNews() {
    RTCDate date;
    RTCTime time;
    func_02035870(data_02123440[0], &date, &time);
    RTC_ConvertSecondToDateTime(&date, &time, RTC_ConvertDateTimeToSecond(&date, &time) - 7 * 60 * 60);
    date.year += 2000;
    if (date.month == 0) {
        OS_Terminate();
    }
    u8 birthdayCount = 0;
    u8 otherCount = 0;
    int monthBit = date.month - 1;
    u8 birthdays[RESIDENT_COUNT];
    u8 others[RESIDENT_COUNT];
    for (int i = 0; i < RESIDENT_COUNT; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            SavedCharacter character;
            func_0202c264(&gGameData, i, &character);
            if (date.month == func_02098dac(&character, 0x26) && date.day == func_02098dac(&character, 0x27)) {
                birthdays[birthdayCount] = i;
                birthdayCount++;
            } else {
                others[otherCount] = i;
                otherCount++;
            }
        }
    }
    for (int i = 0; i < birthdayCount; i++) {
        int j = Rand(birthdayCount);
        u8 t = birthdays[i];
        birthdays[i] = birthdays[j];
        birthdays[j] = t;
    }
    BOOL found = FALSE;
    u8 chosen;
    if (birthdayCount != 0) {
        int last = -1;
        for (int i = 0; i < 11; i++) {
            NewsItem item;
            func_0202c8a8(&gGameData, i, &item);
            u32 year = item.mYear + 2000;
            if (item.mKind == 0x1D && year == date.year) {
                last = item.mCharacter;
                break;
            }
        }
        if (last != -1) {
            for (int i = 0; i < birthdayCount; i++) {
                chosen = birthdays[i];
                if (last != chosen) {
                    found = TRUE;
                    break;
                }
            }
        } else {
            chosen = birthdays[0];
            found = TRUE;
        }
    }
    ClearSceneCharacters();
    int kind;
    if (found) {
        gSceneMemory.mCharacters[0] = chosen;
        for (int i = 0; i < otherCount; i++) {
            u8 t = others[i];
            int j = Rand(otherCount);
            others[i] = others[j];
            others[j] = t;
        }
        gSceneMemory.mCharacters[1] = others[0];
        gSceneMemory.mCharacters[2] = others[1];
        kind = 0x1D;
    } else {
        for (;;) {
            kind = Rand(0xAC);
            if (data_020fa1d0[kind].mMonths & (1 << monthBit)) {
                break;
            }
        }
    }
    AddNews(kind);
}

void MapScene::AddNews(int kind) {
    NewsItem items[11];
    for (int i = 0; i < 11; i++) {
        func_0202c8a8(&gGameData, i, &items[i]);
    }
    int dropped = 0;
    for (int i = 10; i >= 0; i--) {
        if (!items[i].mUnknown0CBit0 || !items[i].mUnknown0CBit1) {
            dropped = i;
            break;
        }
    }
    NewsItem item;
    int index = 0;
    NewsItem_Make(&item, kind);
    func_0202c8bc(&gGameData, index, &item);
    index++;
    for (int i = 0; i < 11; i++) {
        if (i != dropped) {
            func_0202c8bc(&gGameData, index, &items[i]);
            index++;
        }
    }
}

// Which picture a window of the apartment building shows: of its resident, by what the resident is doing
int MapScene::GetWindowTexture(int resident) {
    if (resident == -1) {
        return 2;
    }
    u8 event = func_0202c468(&gGameData, resident);
    u8 unknown = func_0202c53c(&gGameData, resident);
    int row = func_0202c5b4(&gGameData, resident);
    int column = func_0202c58c(&gGameData, resident);
    int kind = data_02114d78[row][column].mUnknown0;
    if (func_02099c94(event)) {
        return 3;
    }
    if (event == 0x12) {
        return 3;
    }
    if (event == 0x11) {
        return 6;
    }
    if (unknown == 1 || unknown == 2) {
        if (row == 3 && (kind == 5 || kind == 6)) {
            return 5;
        }
        if (row == 4 && kind == 11) {
            return 4;
        }
        return 1;
    }
    return 0;
}

void MapScene::UpdateWindows() {
    for (int i = 0; i < mWindowCount; i++) {
        int texture = GetWindowTexture(mRoomResidents[i]);
        ScreenRect rect = sWindowTextureRect;
        func_0202b1f0(&mWindows[i], texture, &rect);
        mWindows[i].mScaleY = 0xD5;
        mWindows[i].mScaleX = 0xD5;
        func_0202b4c4(&mWindows[i]);
        mWindows[i].mColor = GX_RGB(31, 31, 31);
        mWindows[i].mUnknown40 = 0x294A;
        mWindows[i].mUnknown42 = 0x6739;
        mWindows[i].mUnknown44 = 3;
        VecFx32 position = GetWindowPosition(i);
        mWindows[i].mX = position.x;
        mWindows[i].mY = position.y;
        mWindows[i].mDepth = -position.z / 16;
    }
}

// --- MapSound ---

void MapSound::Init(MapClock *clock, int mode) {
    gSoundManager.StartSeqArcIfNotPlaying(0, 3, 4);
    for (int i = 0; i < 4; i++) {
        mRamps.mRamps[i].mStep = sMusicRampSteps[i] << FX32_SHIFT;
        int value = sModelMusic[0][i] << FX32_SHIFT;
        mRamps.mRamps[i].mValue = value;
        mRamps.mRamps[i].mTarget = value;
        mRamps.mRamps[i].SetMode(ValueRamp::MODE_TO_TARGET);
    }
    gSoundManager.SetSeqArcVolume(0, mRamps.mRamps[0].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPitch(0, 0xFFFF, mRamps.mRamps[1].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTempoRatio(0, mRamps.mRamps[2].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPan(0, 0xFFFF, mRamps.mRamps[3].mValue >> FX32_SHIFT);
    SetMode(clock, mode);
}

void MapSound::SetMode(MapClock *clock, int mode) {
    mDayStart = GetSunrise(clock->mDate.month, 0);
    mNightStart = GetSunset(clock->mDate.month, 0);
    switch (mode) {
    case MODE_START:
    case MODE_CONTINUE:
        if (mode == MODE_START) {
            mSeason = GetSeason(clock->mDate.month);
            gSoundManager.LoadGroup(sSeasonSoundGroups[mSeason]);
            mTimeOfDay = GetTimeOfDay(&clock->mTime);
            StartSurroundings(0, mSeason, mTimeOfDay);
            mSurroundingRamps.mRamps[0].mValue = 0x7E900;
        }
        mDayOrNight = GetDayOrNight(&clock->mTime);
        switch (mDayOrNight) {
        case 0:
            gSoundManager.StartSeqIfNotPlaying(0, 0x1A);
            gSoundManager.MoveSeqVolume(0, 0x7F, 0);
            break;
        case 1:
            gSoundManager.StartSeqIfNotPlaying(1, 0x1B);
            gSoundManager.MoveSeqVolume(1, 0x7F, 0);
            break;
        }
        break;
    case MODE_RESTART:
        mSeason = GetSeason(clock->mDate.month);
        gSoundManager.LoadGroup(sSeasonSoundGroups[mSeason]);
        mTimeOfDay = GetTimeOfDay(&clock->mTime);
        StartSurroundings(0, mSeason, mTimeOfDay);
        mSurroundingRamps.mRamps[0].mValue = 0x7E900;
        break;
    case MODE_TUTORIAL:
        mDayOrNight = GetDayOrNight(&clock->mTime);
        switch (mDayOrNight) {
        case 0:
            gSoundManager.StartSeq(0, 0x1A);
            gSoundManager.MoveSeqVolume(0, 0, 0);
            gSoundManager.MoveSeqVolume(0, 0x7F, 0x78);
            break;
        case 1:
            gSoundManager.StartSeq(1, 0x1B);
            gSoundManager.MoveSeqVolume(1, 0, 0);
            gSoundManager.MoveSeqVolume(1, 0x7F, 0x78);
            break;
        }
        break;
    }
    mMode = mode;
}

void MapSound::Update(MapClock *clock) {
    mRamps.Update();
    gSoundManager.SetSeqArcVolume(0, mRamps.mRamps[0].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPitch(0, 0xFFFF, mRamps.mRamps[1].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTempoRatio(0, mRamps.mRamps[2].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPan(0, 0xFFFF, mRamps.mRamps[3].mValue >> FX32_SHIFT);
    if (!(mMode != MODE_START && mMode != MODE_RESTART && mMode != MODE_CONTINUE)) {
        mSurroundingRamps.Update();
        int volume = mSurroundingRamps.mRamps[0].mValue >> FX32_SHIFT;
        switch (mSurroundingState) {
        case 0:
            gSoundManager.SetSeqArcVolume(5, volume);
            int timeOfDay = GetTimeOfDay(&clock->mTime);
            if (mTimeOfDay != timeOfDay) {
                StartSurroundings(1, mSeason, mTimeOfDay);
            }
            break;
        case 1:
            gSoundManager.SetSeqArcVolume(5, volume);
            if (mLongSound) {
                if (mFrames >= 3600) {
                    mTimeOfDay = GetTimeOfDay(&clock->mTime);
                    StartSurroundings(0, mSeason, mTimeOfDay);
                }
            } else {
                if (mFrames >= 120) {
                    mTimeOfDay = GetTimeOfDay(&clock->mTime);
                    StartSurroundings(0, mSeason, mTimeOfDay);
                }
            }
            mFrames++;
            break;
        }
    }
    if (mMode == MODE_START || mMode == MODE_CONTINUE || mMode == MODE_TUTORIAL) {
        int dayOrNight = GetDayOrNight(&clock->mTime);
        if (mDayOrNight == dayOrNight) {
            return;
        }
        switch (dayOrNight) {
        case 0:
            gSoundManager.StartSeqIfNotPlaying(0, 0x1A);
            gSoundManager.MoveSeqVolume(0, 0, 0);
            gSoundManager.MoveSeqVolume(0, 0x7F, 0xF0);
            gSoundManager.StopSeq(1, 0xF0);
            break;
        case 1:
            gSoundManager.StartSeqIfNotPlaying(1, 0x1B);
            gSoundManager.MoveSeqVolume(1, 0, 0);
            gSoundManager.MoveSeqVolume(1, 0x7F, 0xF0);
            gSoundManager.StopSeq(0, 0xF0);
            break;
        }
        mDayOrNight = dayOrNight;
    }
}

void MapSound::SetBuilding(int model) {
    for (int i = 0; i < 4; i++) {
        mRamps.mRamps[i].mTarget = sModelMusic[model][i] << FX32_SHIFT;
        mRamps.mRamps[i].SetMode(ValueRamp::MODE_TO_TARGET);
    }
}

void MapSound::Stop() {
    gSoundManager.StopSeqArc(0, 30);
}

void MapSound::StartSurroundings(int kind, int season, int timeOfDay) {
    switch (kind) {
    case 0: {
        mSurroundingRamps.mRamps[0].mStep = 0x90;
        mSurroundingRamps.mRamps[0].mValue = 0;
        mSurroundingRamps.mRamps[0].mTarget = 0x7E900;
        mSurroundingRamps.mRamps[0].SetMode(ValueRamp::MODE_TO_TARGET);
        mLongSound = FALSE;
        gSoundManager.StopSeqArc(5, 0);
        gSoundManager.StopSeqArc(4, 0);
        const u8 *sounds = &sSurroundingSounds[season][timeOfDay * 3];
        for (int i = 0; i < 3; i++) {
            StartSurroundingSound(*sounds++);
        }
        gSoundManager.SetSeqArcVolume(5, mSurroundingRamps.mRamps[0].mValue >> FX32_SHIFT);
        break;
    }
    case 1: {
        mSurroundingRamps.mRamps[0].mStep = 0x90;
        mSurroundingRamps.mRamps[0].mValue = 0x7E900;
        mSurroundingRamps.mRamps[0].mTarget = 0;
        mSurroundingRamps.mRamps[0].SetMode(ValueRamp::MODE_TO_TARGET);
        mLongSound = FALSE;
        gSoundManager.SetSeqArcVolume(5, mSurroundingRamps.mRamps[0].mValue >> FX32_SHIFT);
        gSoundManager.StopSeqArc(4, 0x78);
        const u8 *sounds = &sSurroundingSounds[season][timeOfDay * 3];
        for (int i = 0; i < 3; i++) {
            MarkLongSound(*sounds++);
        }
        break;
    }
    }
    mSurroundingState = kind;
    mFrames = 0;
}

void MapSound::StartSurroundingSound(u8 index) {
    if (index == 0xFF) {
        return;
    }
    int seqArc = IsCommonSound(index) ? 3 : sSeasonSoundArchives[mSeason];
    if (IsLongSound(index)) {
        gSoundManager.StartSeqArc(5, seqArc, index);
        mLongSound = TRUE;
    } else {
        gSoundManager.StartSeqArc(4, seqArc, index);
    }
}

void MapSound::MarkLongSound(u8 index) {
    if (index == 0xFF) {
        return;
    }
    if (IsLongSound(index)) {
        mLongSound = TRUE;
    }
}

BOOL MapSound::IsLongSound(int index) {
    for (int i = 0; i < 4; i++) {
        if (index == sLongSounds[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

BOOL MapSound::IsCommonSound(int index) {
    for (int i = 0; i < 4; i++) {
        if (index == sCommonSounds[i]) {
            return TRUE;
        }
    }
    return FALSE;
}

int MapSound::GetSeason(int month) {
    switch (month) {
    case 3:
    case 4:
    case 5:
        return 0;
    case 6:
        return 1;
    case 7:
    case 8:
        return 2;
    case 9:
    case 10:
    case 11:
        return 3;
    case 12:
    case 1:
    case 2:
        return 4;
    }
    return 0;
}

int MapSound::GetTimeOfDay(RTCTime *time) {
    u32 seconds = time->hour * 3600 + time->minute * 60;
    u32 dayStart = mDayStart;
    u32 nightStart = mNightStart;
    u32 morningEnd = dayStart + 2 * 60 * 60;
    u32 eveningEnd = nightStart + 2 * 60 * 60;
    if (seconds >= dayStart - 60 * 60 && seconds < morningEnd) {
        return 0;
    }
    if (seconds >= morningEnd && seconds < nightStart) {
        return 1;
    }
    if (seconds >= nightStart && seconds < eveningEnd) {
        return 2;
    }
    return 3;
}

int MapSound::GetDayOrNight(RTCTime *time) {
    u32 seconds = time->hour * 3600 + time->minute * 60;
    u32 nightEnd = mNightStart + 60 * 60;
    if (seconds >= mDayStart && seconds < nightEnd) {
        return 0;
    }
    return 1;
}

// --- MapScene ---

void MapScene::StartEffects(const VecFx32 *position) {
    for (int i = 0; i < 2; i++) {
        if (mEmitters[i] != NULL) {
            func_0207b364(mParticles, mEmitters[i]);
            mEmitters[i] = NULL;
        }
        mEmitters[i] = func_0207b2fc(mParticles, sEffectResources[i], position);
    }
}

void MapScene::StartEffect2(const VecFx32 *position) {
    for (int i = 2; i < 3; i++) {
        if (mEmitters[i] != NULL) {
            func_0207b364(mParticles, mEmitters[i]);
            mEmitters[i] = NULL;
        }
        mEmitters[i] = func_0207b2fc(mParticles, sEffectResources[i], position);
    }
}

// --- TitleCamera ---

void TitleCamera::Start(Camera *camera) {
    mCamera = camera;
    SetView(Rand(10));
}

void TitleCamera::Update() {
    mCamera->mAngleY += (u16)sTitleViews[mView].mTurnY;
    mCamera->mAngleX += (u16)sTitleViews[mView].mTurnX;
    VecFx32 direction;
    VEC_Subtract(&sTitleViews[mView].mTarget, &sTitleViews[mView].mPosition, &direction);
    VEC_Normalize(&direction, &direction);
    VEC_MultAdd(sTitleViews[mView].mSpeed, &direction, &mCamera->mPosition, &mCamera->mPosition);
    if (mFrame++ > sTitleViews[mView].mFrames) {
        int view;
        do {
            view = Rand(10);
        } while (view == mView);
        SetView(view);
    }
}

void TitleCamera::SetView(int view) {
    mCamera->mAngleY = 0;
    mCamera->mAngleX = 0;
    VEC_Set(&mCamera->mPosition, sTitleViews[view].mPosition.x, sTitleViews[view].mPosition.y,
            sTitleViews[view].mPosition.z);
    VEC_Set(&mCamera->mTarget, sTitleViews[view].mTarget.x, sTitleViews[view].mTarget.y,
            sTitleViews[view].mTarget.z);
    mView = view;
    mFrame = 0;
}

// --- MapScene ---

void MapScene::ShowBuilding(int model) {
    mModels[model].Load(sModelResources[model], 0);
    mModels[model].SetPosition(sModelPositions[model].x, sModelPositions[model].y, sModelPositions[model].z);
    if (model == MODEL_APARTMENT) {
        for (int i = 0; i < mWindowCount; i++) {
            mWindows[i].mVisible = TRUE;
        }
    } else if (model == 10) {
        mNewsDue = IsNewsDue();
        if (mNewsDue) {
            mModels[10].SetAnimation(RESOURCE_BTP0, 0);
        } else {
            mModels[10].SetAnimation(RESOURCE_BTP0, 1);
        }
    } else if (model == 5) {
        mModels[5].SetAnimation(RESOURCE_BTA0, 0);
    }
}

void MapScene::EnterBuilding(int model, BOOL newsDue, int button) {
    switch (model) {
    case 2:
        switch (button) {
        default:
            if (Rand(2) == 0) {
                func_0203c02c(&gSceneManager, 0x23);
            } else {
                func_0203c02c(&gSceneManager, 0x81);
            }
            break;
        case 1:
            func_0203c02c(&gSceneManager, 0x23);
            break;
        case 2:
            func_0203c02c(&gSceneManager, 0x81);
            break;
        }
        break;
    case 9:
        func_0203c02c(&gSceneManager, 0x2F);
        break;
    case 4:
        func_0203c02c(&gSceneManager, 0x68);
        break;
    case 6:
        func_0203c02c(&gSceneManager, 0x5E);
        break;
    case 7:
        func_0203c02c(&gSceneManager, 0x2A);
        break;
    case 3:
        func_0203c02c(&gSceneManager, 0x1F);
        break;
    case 11:
        func_0203c02c(&gSceneManager, 0x1E);
        break;
    case 12:
        func_0203c02c(&gSceneManager, 0x35);
        break;
    case 5:
        if (!IsNewDay()) {
            ClearSceneCharacters();
            if (mFacilities.mResidents < 4) {
                func_0203c02c(&gSceneManager, 0x57);
                break;
            }
            u32 minutes = mClock.mTime.hour * 60 + mClock.mTime.minute;
            int kind = 3;
            for (int i = 0; i < 14; i++) {
                if (minutes < sFountainTimes[i].mTime) {
                    kind = sFountainTimes[i].mKind;
                    break;
                }
            }
            switch (kind) {
            case 0:
                func_0203c02c(&gSceneManager, 0x94);
                break;
            case 1:
                func_0203c02c(&gSceneManager, 0x3F);
                break;
            case 2:
                func_0203c02c(&gSceneManager, 0x95);
                break;
            case 3:
                func_0203c02c(&gSceneManager, 0x57);
                break;
            case 4: {
                u32 tens = (u32)mClock.mTime.minute / 10;
                if (!(tens & 1)) {
                    func_0203c02c(&gSceneManager, tens + 0x9D);
                } else {
                    func_0203c02c(&gSceneManager, 0x57);
                }
                break;
            }
            }
        } else {
            func_0203c02c(&gSceneManager, 0x30);
        }
        break;
    case 1:
        func_0203c02c(&gSceneManager, 0x80);
        break;
    case 10:
        if (newsDue) {
            MakeDailyNews();
        }
        func_0203c02c(&gSceneManager, 0x46);
        break;
    case MODEL_APARTMENT:
        gSceneMemory.mCharacters[0] = 0;
        func_0203c02c(&gSceneManager, 0x11);
        break;
    }
    int building = sModelBuildings[model];
    if (func_02039c00(&gIsland, building) == 2) {
        func_02039c0c(&gIsland, building, 1);
    }
    if (model == 6) {
        for (int i = 0; i < 2; i++) {
            int feature = sCityHallFeatures[i];
            if (func_02039c00(&gIsland, feature) == 2) {
                func_02039c0c(&gIsland, feature, 1);
            }
        }
    }
    if (model == 7) {
        for (int i = 0; i < 7; i++) {
            int feature = sRankingFeatures[i];
            if (func_02039c00(&gIsland, feature) == 2) {
                func_02039c0c(&gIsland, feature, 1);
            }
        }
    }
}

// Whether a new day has begun since the date the game data keeps at +0x6C (none kept counts as one): func_0202cb08
// compares it with today's, a day starting at 5:00, and says FALSE when the clock cannot be read. The fountain's
// window and where entering it leads depend on it.
BOOL MapScene::IsNewDay() {
    return func_0202cb08(&gGameData);
}

// --- MapClock ---

MapClock::MapClock() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

void MapClock::Update() {
    mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
}

void MapClock::UpdateIfArchiveIdle() {
    if (ArchiveManager_IsIdle(&gArchiveManager)) {
        mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
    }
}

// --- MapClockDisplay ---

void MapClockDisplay::Init(MapClock *clock) {
    mClock = clock;
    SetState(STATE_HIDE);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite *sprite = &mSprites[i];
        Sprite_Init(sprite, 0, sClockSequences[i], 128, 96, TRUE);
        mSprites[i].SetPosition(FxVec2(sClockPositions[i].x << FX32_SHIFT, sClockPositions[i].y << FX32_SHIFT));
        sprite->mAnim.animCtrl.bActive = FALSE;
    }
    Refresh(TRUE);
}

void MapClockDisplay::Update() {
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

void MapClockDisplay::Refresh(BOOL all) {
    MapClock *clock = mClock;
    if (mMonth != clock->mDate.month || all) {
        u32 tens = clock->mDate.month / 10;
        Sprite_SetFrame(&mSprites[0], tens);
        Sprite_SetFrame(&mSprites[1], clock->mDate.month - tens * 10);
        mMonth = clock->mDate.month;
    }
    if (mDay != clock->mDate.day || all) {
        u32 tens = clock->mDate.day / 10;
        Sprite_SetFrame(&mSprites[3], tens);
        Sprite_SetFrame(&mSprites[4], clock->mDate.day - tens * 10);
        mDay = clock->mDate.day;
    }
    if (mHour != clock->mTime.hour || all) {
        u32 half = clock->mTime.hour / 12;
        Sprite_SetFrame(&mSprites[5], half);
        u32 hour = clock->mTime.hour - half * 12;
        u32 tens = hour / 10;
        Sprite_SetFrame(&mSprites[6], tens);
        // Subtracted in place: as an argument, the sprite's address is computed before it.
        hour -= tens * 10;
        Sprite_SetFrame(&mSprites[7], hour);
        mHour = clock->mTime.hour;
    }
    if (mMinute != clock->mTime.minute || all) {
        u32 tens = clock->mTime.minute / 10;
        Sprite_SetFrame(&mSprites[9], tens);
        Sprite_SetFrame(&mSprites[10], clock->mTime.minute - tens * 10);
        mMinute = clock->mTime.minute;
    }
}

void MapClockDisplay::Draw() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

void MapClockDisplay::SetState(int state) {
    mState = state;
    mHiddenFrames = 0;
}

void MapClockDisplay::SetVisible(BOOL visible) {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        mSprites[i].mVisible = visible;
    }
}

// --- MapScene ---

void MapScene::UpdateNewMarks() {
    for (int i = 1; i < MODEL_COUNT; i++) {
        if (func_02039c00(&gIsland, sModelBuildings[i]) == 2) {
            mNewMarks[i].mVisible = TRUE;
        } else {
            mNewMarks[i].mVisible = FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        if (func_02039c00(&gIsland, sCityHallFeatures[i]) == 2) {
            mNewMarks[6].mVisible = TRUE;
        }
    }
    for (int i = 0; i < 7; i++) {
        if (func_02039c00(&gIsland, sRankingFeatures[i]) == 2) {
            mNewMarks[7].mVisible = TRUE;
        }
    }
}

// --- MoneyDisplay ---

void MoneyDisplay::Init(int *money, int *charOffset) {
    mState = STATE_NONE;
    mMoney = money;
    mShown = *mMoney;
    Sprite_Init(&mSprite, 1, 0x1B, 128, 96, TRUE);
    mSprite.SetPosition(FxVec2(sMoneySpritePosition.x << FX32_SHIFT, sMoneySpritePosition.y << FX32_SHIFT));
    mSprite.mOverwritePalette = 1;
    mText.mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mText.mTextX = 0x40;
    mText.InitText(&gFonts[0].mFont, 0x10, 8, 2, *charOffset, -2, 1, 0, 0, 15, NULL, 2, NNS_G2D_CHARA_COLORMODE_16);
    mText.mColor = 1;
    NNS_G2dCharCanvasClear(&mText.mCanvas, 0);
    SetTextPosition(sMoneyTextPosition);
    mText.mSprite.mOverwritePalette = 1;
    *charOffset += mText.GetCharCount();
    Refresh();
}

void MoneyDisplay::Update() {
    switch (mState) {
    case STATE_NONE:
        break;
    case STATE_START:
        if (mShown < *mMoney) {
            gSoundManager.StartSeqArc(1, 0x1B, 4);
            mState = STATE_COUNT;
        } else if (mShown > *mMoney) {
            gSoundManager.StartSeqArc(1, 0x1B, 5);
            mState = STATE_COUNT;
        } else {
            mState = STATE_DONE;
        }
        break;
    case STATE_COUNT:
        if (mShown == *mMoney) {
            gSoundManager.StopSeqArc(1, 0);
            mState = STATE_DONE;
        } else if (mShown < *mMoney) {
            mShown += 50;
            if (mShown > *mMoney) {
                mShown = *mMoney;
            }
            Refresh();
        }
        break;
    case STATE_DONE:
        mState = STATE_NONE;
        break;
    }
}

void MoneyDisplay::Draw() {
    mText.Draw();
    Sprite_Draw(&mSprite);
}

void MoneyDisplay::SetVisible(BOOL visible) {
    mText.mSprite.mVisible = visible;
    mSprite.mVisible = visible;
}

void MoneyDisplay::Refresh() {
    wchar_t text[10];
    func_02036874(mShown, text, 20, 0, 0);
    mText.mText = (const NNSG2dChar *)text;
    NNS_G2dCharCanvasClear(&mText.mCanvas, 0);
    mText.DrawText();
}

// --- MapScene ---

MapScene::~MapScene() {}
