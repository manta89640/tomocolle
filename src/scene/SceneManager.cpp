// The scene manager (gSceneManager) and the scene factory, CreateScene, which constructs each scene by its id.
//
// The constructors of the scenes are in this file, after CreateScene, in the order of the scene ids, each followed
// by the implicit destructors of its members that it needs: this file holds every scene class's constructor,
// including those of the scenes whose other functions are in an overlay. The classes are declared in the headers
// beside the code of each scene (src/overlays/ovNNN/, src/scene/), against the main program's classes in include/.

#include <nitro.h>

#include "Overlay.h"
#include "SceneManager.h"
#include "ScreenControl.h"
#include "LcdPower.h"
#include "sound/SoundManager.h"
#include "wireless/WirelessSession.h"

#include "SceneStandIns.h"

// The palette loader, as far as the scene manager touches it (MainProgram.h declares it, and the scenes' headers)
struct PaletteLoader {
    /* 0x000 */ u8 mUnknown000[0x41C];
    /* 0x41C */ int mUnknown41C;
};

// The main program's test scenes (src/scene/*TestScene.h, EmptyScene.h): constructors 0x0203E320..0x0203EA2C and
// 0x0203F654
#include "DrawManTestScene.h"
#include "HumanTestScene.h"
#include "ParticleTestScene.h"
#include "G3dTestScene.h"
#include "BgTextTestScene.h"
#include "MessageTestScene.h"
#include "KeyboardTestScene.h"
#include "TtsTestScene.h"
#include "EmptyScene.h"
#include "WirelessTestScene.h"

// The scene classes of the constructors 0x0203de78..0x02040578 (gap38-a)
#include "../overlays/ov000/TextTestScene.h"
#include "../overlays/ov105/NintendoLogoScene.h"
#include "../overlays/ov002/FaceTestScene.h"
#include "../overlays/ov003/BodyPaletteTestScene.h"
#include "../overlays/ov004/CharacterBodyTestScene.h"
#include "../overlays/ov005/TestDrawMiiScene.h"
#include "../overlays/ov001/NameSortTestScene.h"
#include "../overlays/ov006/Scene0F.h"
#include "../overlays/ov077/Scene17.h"
#include "../overlays/ov074/RoomScene.h"
#include "../overlays/ov073/TutorialRoomScene.h"
#include "../overlays/ov007/Scene18.h"
#include "../overlays/ov008/Scene19.h"
#include "../overlays/ov009/TestNg20Scene.h"
#include "../overlays/ov078/FriendListScene.h"
#include "../overlays/ov067/ChangeClothesScene.h"
#include "../overlays/ov068/Scene1E.h"
#include "../overlays/ov070/RightJobScene.h"
#include "../overlays/ov071/RightJob2Scene.h"
#include "../overlays/ov080/VoteScene.h"
#include "MiiListMenuScene.h"
#include "G3SpriteTestScene.h"
#include "MiiListScene.h"
// End of gap38-a's scene classes

// The scene classes of the constructors 0x02040578..0x0204830c (gap38-b)
#include "MapModelsScene.h"
#include "../overlays/ov079/RankingScene.h"
#include "../overlays/ov109/TushinScene.h"
#include "../overlays/ov010/Scene2B.h"
#include "../overlays/ov069/SavePlayScene.h"
#include "../overlays/ov083/Scene2F.h"
#include "InteriorViewScene.h"
#include "../overlays/ov081/Scene30.h"
#include "../overlays/ov011/Scene31.h"
#include "../overlays/ov082/Scene33.h"
#include "../overlays/ov012/Scene34.h"
#include "../overlays/ov084/Scene35.h"
#include "../overlays/ov013/TestNg48Scene.h"
#include "../overlays/ov014/TestNg50Scene.h"
#include "../overlays/ov086/ChangeRoomScene.h"
#include "../overlays/ov106/PreStartScene.h"
#include "../overlays/ov015/TestNg55Scene.h"
#include "../overlays/ov016/Scene3B.h"
#include "../overlays/ov017/Scene3C.h"
#include "../overlays/ov018/TestNg62Scene.h"
#include "../overlays/ov019/TestNg63Scene.h"
#include "../overlays/ov020/Scene3F.h"
#include "../overlays/ov075/Scene40.h"
#include "../overlays/ov021/TestNg69Scene.h"
#include "../overlays/ov076/MiiEditScene.h"
#include "../overlays/ov022/Scene45.h"
#include "../overlays/ov087/MiiNewsScene.h"
#include "../overlays/ov023/Scene4B.h"
#include "../overlays/ov024/Scene4C.h"
#include "../overlays/ov025/Scene4D.h"
#include "../overlays/ov026/TestNg87Scene.h"
#include "../overlays/ov027/Scene4F.h"
#include "../overlays/ov028/Scene50.h"
#include "../overlays/ov029/Scene51.h"
#include "../overlays/ov030/TestNg94Scene.h"
#include "../overlays/ov031/TestNg95Scene.h"
#include "../overlays/ov032/Scene54.h"
#include "../overlays/ov033/TestNg100Scene.h"
#include "../overlays/ov089/Scene56.h"
#include "../overlays/ov034/TestNg104Scene.h"
#include "../overlays/ov035/Scene58.h"
#include "../overlays/ov036/TestNg107Scene.h"
#include "../overlays/ov090/RomanceScene.h"
#include "../overlays/ov092/WriteSongScene.h"
#include "../overlays/ov065/Scene5D.h"
#include "../overlays/ov091/CityHallScene.h"
#include "../overlays/ov072/MapScene.h"
#include "../overlays/ov085/Scene68.h"
#include "../overlays/ov108/TutorialStepScene.h"
#include "../overlays/ov088/Scene67.h"
#include "../overlays/ov093/Scene72.h"
#include "../overlays/ov094/Scene74.h"
#include "../overlays/ov037/Scene75.h"
#include "../overlays/ov038/TestNg111Scene.h"
#include "../overlays/ov039/TravelScene.h"
#include "../overlays/ov040/Scene7A.h"
#include "../overlays/ov041/Scene7B.h"
#include "../overlays/ov042/TestNg119Scene.h"
#include "../overlays/ov066/Scene7F.h"
#include "../overlays/ov095/AffinityScene.h"
#include "../overlays/ov043/Scene81.h"
#include "../overlays/ov044/TestNg121Scene.h"
#include "../overlays/ov045/Scene88.h"
#include "../overlays/ov046/Scene89.h"
#include "../overlays/ov047/TestNg125Scene.h"
#include "../overlays/ov048/Scene8B.h"
#include "../overlays/ov049/Scene8C.h"
#include "../overlays/ov050/Scene8D.h"
#include "../overlays/ov051/TestNg129Scene.h"
#include "../overlays/ov052/Scene8F.h"
#include "../overlays/ov053/Scene91.h"
#include "../overlays/ov054/TestNg133Scene.h"
#include "../overlays/ov055/TestNg134Scene.h"
#include "../overlays/ov056/TestNg136Scene.h"
#include "../overlays/ov057/TestNg137Scene.h"
#include "../overlays/ov058/TestNg138Scene.h"
#include "../overlays/ov059/TestNg139Scene.h"
#include "../overlays/ov096/WeddingScene.h"
#include "../overlays/ov098/OptionScene.h"
#include "../overlays/ov099/InformationScene.h"
#include "../overlays/ov101/SceneAF.h"
#include "../overlays/ov110/ItemTradeScene.h"
#include "../overlays/ov102/SceneB2.h"
#include "../overlays/ov103/DivorceScene.h"
#include "../overlays/ov100/AlbumScene.h"
#include "../overlays/ov104/SceneB5.h"
#include "../overlays/ov060/SceneB7.h"
#include "../overlays/ov061/TestNg143Scene.h"
#include "../overlays/ov062/SceneB9.h"
#include "../overlays/ov063/SceneBA.h"
#include "../overlays/ov064/TestNg149Scene.h"
#include "../overlays/ov097/Marriage2Scene.h"
#include "../overlays/ov107/SaveErrorScene.h"
// End of gap38-b's scene classes

extern "C" {
// --- The graphics managers, not decompiled: each records its state into a buffer and restores it from one ---
extern u32 gGraphicsManager[];
struct Unknown02120a10 {
    /* 0x000 */ u8 mUnknown000[0x99C];
    /* 0x99C */ int mUnknown99C;
};
extern Unknown02120a10 data_02120a10;
void GraphicsManager_SaveState(void *manager, void *state, u32 flags);
void GraphicsManager_RestoreState(void *manager, void *state, u32 flags);
void ScreenLoader_SaveState(void *manager, void *state, u32 flags);
void ScreenLoader_RestoreState(void *manager, void *state, u32 flags);
void ScreenControl_SaveState(ScreenControl *screens, void *state, u32 flags);
void ScreenControl_RestoreState(ScreenControl *screens, void *state, u32 flags);
void func_02027270(void *manager);
void PaletteLoader_ClearDefaults(void *manager);
void DestroyMainFrameHeap(void);
// The pad input: +0x14 says whether it is read
struct PadInputState {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ BOOL mEnabled;
};
extern PadInputState gPadInput;
}

// ---- The scene manager and CreateScene (gap38-a) ----------------------------------------------------------------

WirelessSession *gWirelessSession;
SceneManager gSceneManager;

// What the graphics managers record and restore (all of their parts)
#define GRAPHICS_STATE_ALL 0x3F3F

SceneManager::~SceneManager() {}

void SceneManager::Init() {
    GX_ResetBankForBG();
    GX_ResetBankForOBJ();
    GX_ResetBankForBGExtPltt();
    GX_ResetBankForOBJExtPltt();
    GX_ResetBankForTex();
    GX_ResetBankForTexPltt();
    GX_ResetBankForClearImage();
    GX_ResetBankForSubBG();
    GX_ResetBankForSubOBJ();
    GX_ResetBankForSubBGExtPltt();
    GX_ResetBankForSubOBJExtPltt();
    GX_ResetBankForARM7();
    GX_SetBankForBG(GX_VRAM_BG_128_A);
    GX_SetBankForSubBG(GX_VRAM_SUB_BG_48_HI);
    GX_SetBankForOBJ(GX_VRAM_OBJ_64_E);
    GX_SetBankForSubOBJ(GX_VRAM_SUB_OBJ_128_D);
    GX_SetBankForBGExtPltt(GX_VRAM_BGEXTPLTT_NONE);
    GX_SetBankForOBJExtPltt(GX_VRAM_OBJEXTPLTT_NONE);
    GX_SetBankForTex(GX_VRAM_TEX_01_BC);
    GX_SetBankForTexPltt(GX_VRAM_TEXPLTT_01_FG);
    GX_SetBankForSubBGExtPltt(GX_VRAM_SUB_BGEXTPLTT_NONE);
    GX_SetBankForSubOBJExtPltt(GX_VRAM_SUB_OBJEXTPLTT_NONE);
    GX_SetBankForClearImage(GX_VRAM_CLEARIMAGE_NONE);
    GX_SetBankForARM7(GX_VRAM_ARM7_NONE);
    GX_SetBankForLCDC(GX_VRAM_LCDC_NONE);
    GX_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_128K);
    GXS_SetOBJVRamModeChar(GX_OBJVRAMMODE_CHAR_1D_128K);
    GXS_SetOBJVRamModeBmp(GX_OBJVRAMMODE_BMP_2D_W256);
    gSoundManager.LoadGroup(0x44);
    gSoundManager.SaveHeapState(0);
    gSoundManager.SaveHeapState(1);
    gScreenControl.mUnknown44 = 1;
    gScreenControl.mUnknown44Changed = TRUE;
    ScreenLoader_SetPriority(&gScreenLoader, 0x100, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 0x800, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 2);
    ScreenLoader_SetPriority(&gScreenLoader, 0x400, 3);
    ScreenLoader_SetPriority(&gScreenLoader, 0x2, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 0x1, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x8, 2);
    ScreenLoader_SetPriority(&gScreenLoader, 0x4, 3);
    func_02027270(&gObjResourceManager);
    data_02120a10.mUnknown99C = 1;
    PaletteLoader_ClearDefaults(&gPaletteLoader);
    gPaletteLoader.mUnknown41C = 0x1E0;
    gPadInput.mEnabled = TRUE;
    GraphicsManager_SaveState(gGraphicsManager, mGraphicsState, GRAPHICS_STATE_ALL);
    ScreenLoader_SaveState(&gScreenLoader, mScreenLoaderState, GRAPHICS_STATE_ALL);
    ScreenControl_SaveState(&gScreenControl, mScreenControlState, GRAPHICS_STATE_ALL);
    mScene = CreateScene(0xB6);
    mPreviousSceneId = 0;
    mNextSceneId = 0;
}

void SceneManager::Update() {
    if (mScene == NULL) {
        return;
    }
    int phase = mScene->mPhase;
    if (phase == Scene::PHASE_RUN) {
        gLcdPower.Update();
    } else if (phase == Scene::PHASE_DONE) {
        DestroyMainFrameHeap();
        delete mScene;
        UnloadSceneOverlay(mPreviousSceneId);
        mScene = CreateScene(mNextSceneId);
        GraphicsManager_RestoreState(gGraphicsManager, mGraphicsState, GRAPHICS_STATE_ALL);
        ScreenLoader_RestoreState(&gScreenLoader, mScreenLoaderState, GRAPHICS_STATE_ALL);
        ScreenControl_RestoreState(&gScreenControl, mScreenControlState, GRAPHICS_STATE_ALL);
        func_02027270(&gObjResourceManager);
        data_02120a10.mUnknown99C = 1;
        PaletteLoader_ClearDefaults(&gPaletteLoader);
        gPaletteLoader.mUnknown41C = 0x1E0;
    }
    mScene->Run();
}

void SceneManager::RequestScene(int sceneId) {
    if (mScene == NULL) {
        return;
    }
    mScene->mPhase = Scene::PHASE_FADE_OUT;
    mPreviousSceneId = mScene->mSceneId;
    mNextSceneId = sceneId;
}

// Constructs the scene with the id, after loading its overlay (if it has one) and its sound group. The cases are
// in the order of the original's code.
Scene *SceneManager::CreateScene(int sceneId) {
    switch (sceneId) {
    case 0x00:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        return new NintendoLogoScene(sceneId);
    case 0x01:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(0));
        return new TextTestScene(sceneId);
    case 0x02:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(2));
        return new FaceTestScene(sceneId);
    case 0x03:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(3));
        return new BodyPaletteTestScene(sceneId);
    case 0x04:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(4));
        return new CharacterBodyTestScene(sceneId);
    case 0x05:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(5));
        return new TestDrawMiiScene(sceneId);
    case 0x06:
        SetSoundGroup(-1);
        return new DrawManTestScene(sceneId);
    case 0x07:
        SetSoundGroup(-1);
        return new HumanTestScene(sceneId);
    case 0x08:
        SetSoundGroup(-1);
        return new ParticleTestScene(sceneId);
    case 0x10:
        SetSoundGroup(-1);
        return new G3dTestScene(sceneId);
    case 0x09:
        SetSoundGroup(-1);
        return new BgTextTestScene(sceneId);
    case 0x0A:
        SetSoundGroup(-1);
        return new MessageTestScene(sceneId);
    case 0x0B:
        SetSoundGroup(-1);
        return new KeyboardTestScene(sceneId);
    case 0x0C:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(1));
        return new NameSortTestScene(sceneId);
    case 0x0F:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(6));
        return new Scene0F(sceneId);
    case 0x0E:
        SetSoundGroup(-1);
        return new TtsTestScene(sceneId);
    case 0x16:
        SetSoundGroup(-1);
        return new EmptyScene(sceneId);
    case 0x17:
    case 0x7D:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(77));
        return new Scene17(sceneId);
    case 0x18:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(7));
        return new Scene18(sceneId);
    case 0x0D:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        return new NintendoLogoScene(sceneId);
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
        SetSoundGroup(9);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(74));
        return new RoomScene(sceneId);
    case 0x6B:
        SetSoundGroup(9);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(73));
        return new TutorialRoomScene(sceneId);
    case 0x19:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(8));
        return new Scene19(sceneId);
    case 0x1A:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(78));
        return new FriendListScene(sceneId);
    case 0x1B:
        SetSoundGroup(-1);
        return new WirelessTestScene(sceneId);
    case 0x1C:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(9));
        return new TestNg20Scene(sceneId);
    case 0x1D:
        SetSoundGroup(0);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(67));
        return new ChangeClothesScene(sceneId);
    case 0x1E:
        SetSoundGroup(20);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(68));
        return new Scene1E(sceneId);
    case 0x1F:
        SetSoundGroup(46);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(70));
        return new RightJobScene(sceneId);
    case 0x20:
    case 0x21:
    case 0x22:
        SetSoundGroup(46);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(71));
        return new RightJob2Scene(sceneId);
    case 0x23:
        SetSoundGroup(0);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(80));
        return new VoteScene(sceneId);
    case 0x24:
        SetSoundGroup(-1);
        return new MiiListMenuScene(sceneId);
    case 0x25:
        SetSoundGroup(-1);
        return new G3SpriteTestScene(sceneId);
    case 0x26:
        SetSoundGroup(-1);
        return new MiiListScene(sceneId);
    case 0x27:
        SetSoundGroup(-1);
        return new MapModelsScene(sceneId);
    case 0x28:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        return new NintendoLogoScene(sceneId);
    case 0x29:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(109));
        return new TushinScene(sceneId);
    case 0x2A:
        SetSoundGroup(48);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(79));
        return new RankingScene(sceneId);
    case 0x2B:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(10));
        return new Scene2B(sceneId);
    case 0x2C:
    case 0x2D:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(69));
        return new SavePlayScene(sceneId);
    case 0x2E:
        return new InteriorViewScene(sceneId);
    case 0x2F:
        SetSoundGroup(21);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(83));
        return new Scene2F(sceneId);
    case 0x30:
    case 0x70:
        SetSoundGroup(0);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(81));
        return new Scene30(sceneId);
    case 0x31:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(11));
        return new Scene31(sceneId);
    case 0x32:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(11));
        return new Scene31(sceneId);
    case 0x33:
    case 0x73:
        SetSoundGroup(0);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(82));
        return new Scene33(sceneId);
    case 0x34:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(12));
        return new Scene34(sceneId);
    case 0x35:
        SetSoundGroup(22);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(84));
        return new Scene35(sceneId);
    case 0x36:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(13));
        return new TestNg48Scene(sceneId);
    case 0x37:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(14));
        return new TestNg50Scene(sceneId);
    case 0x38:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(86));
        return new ChangeRoomScene(sceneId);
    case 0x39:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(106));
        return new PreStartScene(sceneId);
    case 0x3A:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(15));
        return new TestNg55Scene(sceneId);
    case 0x3B:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(16));
        return new Scene3B(sceneId);
    case 0x3C:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(17));
        return new Scene3C(sceneId);
    case 0x3D:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(18));
        return new TestNg62Scene(sceneId);
    case 0x3E:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(19));
        return new TestNg63Scene(sceneId);
    case 0x3F:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(20));
        return new Scene3F(sceneId);
    case 0x40:
    case 0x6C:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(75));
        return new Scene40(sceneId);
    case 0x41:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(21));
        return new TestNg69Scene(sceneId);
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x6F:
        SetSoundGroup(1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(76));
        return new MiiEditScene(sceneId);
    case 0x45:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(22));
        return new Scene45(sceneId);
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(87));
        return new MiiNewsScene(sceneId);
    case 0x4B:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(23));
        return new Scene4B(sceneId);
    case 0x4C:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(24));
        return new Scene4C(sceneId);
    case 0x4D:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(25));
        return new Scene4D(sceneId);
    case 0x4E:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(26));
        return new TestNg87Scene(sceneId);
    case 0x4F:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(27));
        return new Scene4F(sceneId);
    case 0x50:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(28));
        return new Scene50(sceneId);
    case 0x51:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(29));
        return new Scene51(sceneId);
    case 0x52:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(30));
        return new TestNg94Scene(sceneId);
    case 0x53:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(31));
        return new TestNg95Scene(sceneId);
    case 0x54:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(32));
        return new Scene54(sceneId);
    case 0x55:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(33));
        return new TestNg100Scene(sceneId);
    case 0x56:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(89));
        return new Scene56(sceneId);
    case 0x57:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(34));
        return new TestNg104Scene(sceneId);
    case 0x58:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(35));
        return new Scene58(sceneId);
    case 0x59:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(36));
        return new TestNg107Scene(sceneId);
    case 0x5A:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(90));
        return new RomanceScene(sceneId);
    case 0x5B:
    case 0x5C:
    case 0xB0:
        SetSoundGroup(9);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(92));
        return new WriteSongScene(sceneId);
    case 0x5D:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(65));
        return new Scene5D(sceneId);
    case 0x5E:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(91));
        return new CityHallScene(sceneId);
    case 0x5F:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x6A:
    case 0x6D:
    case 0x6E:
        if (sceneId != 0x6E) {
            SetSoundGroup(0x10);
        }
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(72));
        return new MapScene(sceneId);
    case 0x67:
        SetSoundGroup(0);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(88));
        return new Scene67(sceneId);
    case 0x68:
    case 0x71:
        SetSoundGroup(19);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(85));
        return new Scene68(sceneId);
    case 0x69:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(108));
        return new TutorialStepScene(sceneId);
    case 0x72:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(93));
        return new Scene72(sceneId);
    case 0x74:
    case 0x7E:
        SetSoundGroup(1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(94));
        return new Scene74(sceneId);
    case 0x75:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(37));
        return new Scene75(sceneId);
    case 0x76:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(38));
        return new TestNg111Scene(sceneId);
    case 0x77:
    case 0x78:
    case 0x79:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(39));
        return new TravelScene(sceneId);
    case 0x7A:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(40));
        return new Scene7A(sceneId);
    case 0x7B:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(41));
        return new Scene7B(sceneId);
    case 0x7C:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(42));
        return new TestNg119Scene(sceneId);
    case 0x7F:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(66));
        return new Scene7F(sceneId);
    case 0x80:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(95));
        return new AffinityScene(sceneId);
    case 0x81:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(43));
        return new Scene81(sceneId);
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(44));
        return new TestNg121Scene(sceneId);
    case 0x88:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(45));
        return new Scene88(sceneId);
    case 0x89:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(46));
        return new Scene89(sceneId);
    case 0x8A:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(47));
        return new TestNg125Scene(sceneId);
    case 0x8B:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(48));
        return new Scene8B(sceneId);
    case 0x8C:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(49));
        return new Scene8C(sceneId);
    case 0x8D:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(50));
        return new Scene8D(sceneId);
    case 0x8E:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(51));
        return new TestNg129Scene(sceneId);
    case 0x8F:
    case 0x90:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(52));
        return new Scene8F(sceneId);
    case 0x91:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(53));
        return new Scene91(sceneId);
    case 0x92:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(54));
        return new TestNg133Scene(sceneId);
    case 0x93:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(55));
        return new TestNg134Scene(sceneId);
    case 0x94:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(56));
        return new TestNg136Scene(sceneId);
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9E:
    case 0x9F:
    case 0xA0:
    case 0xA1:
    case 0xA2:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(57));
        return new TestNg137Scene(sceneId);
    case 0xA3:
    case 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
    case 0xA8:
    case 0xA9:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(58));
        return new TestNg138Scene(sceneId);
    case 0xAA:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(59));
        return new TestNg139Scene(sceneId);
    case 0xAB:
        SetSoundGroup(15);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(96));
        return new WeddingScene(sceneId);
    case 0xAC:
    case 0xAD:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(98));
        return new OptionScene(sceneId);
    case 0xAE:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(99));
        return new InformationScene(sceneId);
    case 0xAF:
        SetSoundGroup(9);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(101));
        return new SceneAF(sceneId);
    case 0xB1:
        SetSoundGroup(54);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(110));
        return new ItemTradeScene(sceneId);
    case 0xB2:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(102));
        return new SceneB2(sceneId);
    case 0xB3:
        SetSoundGroup(58);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(103));
        return new DivorceScene(sceneId);
    case 0xB4:
        SetSoundGroup(2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(100));
        return new AlbumScene(sceneId);
    case 0xB5:
        SetSoundGroup(66);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(104));
        return new SceneB5(sceneId);
    case 0xB6:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        return new NintendoLogoScene(sceneId);
    case 0xB7:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(60));
        return new SceneB7(sceneId);
    case 0xB8:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(61));
        return new TestNg143Scene(sceneId);
    case 0xB9:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(62));
        return new SceneB9(sceneId);
    case 0xBA:
    case 0xBB:
    case 0xBC:
    case 0xBD:
    case 0xBE:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(63));
        return new SceneBA(sceneId);
    case 0xBF:
        SetSoundGroup(-2);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(64));
        return new TestNg149Scene(sceneId);
    case 0xC0:
    case 0xC1:
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(97));
        return new Marriage2Scene(sceneId);
    case 0xC2:
        SetSoundGroup(-1);
        FS_LoadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(107));
        return new SaveErrorScene(sceneId);
    }
    return NULL;
}

// ---- Constructors 0x0203de78..0x02040578: scenes up to 0x26 (gap38-a) --------------------------------------------

NintendoLogoScene::NintendoLogoScene(int sceneId) : Scene(sceneId) {}

TextTestScene::TextTestScene(int sceneId) : Scene(sceneId) {}

FaceTestScene::FaceTestScene(int sceneId) : Scene(sceneId) {}

BodyPaletteTestScene::BodyPaletteTestScene(int sceneId) : Scene(sceneId) {}

CharacterBodyTestScene::CharacterBodyTestScene(int sceneId) : Scene(sceneId) {}

TestDrawMiiScene::TestDrawMiiScene(int sceneId) : Scene(sceneId) {}

DrawManTestScene::DrawManTestScene(int sceneId) : Scene(sceneId) {}

HumanTestScene::HumanTestScene(int sceneId) : Scene(sceneId) {}

ParticleTestScene::ParticleTestScene(int sceneId) : Scene(sceneId) {}

G3dTestScene::G3dTestScene(int sceneId) : Scene(sceneId) {}

BgTextTestScene::BgTextTestScene(int sceneId) : Scene(sceneId) {}

MessageTestScene::MessageTestScene(int sceneId) : Scene(sceneId) {}

KeyboardTestScene::KeyboardTestScene(int sceneId) : Scene(sceneId) {}

NameSortTestScene::NameSortTestScene(int sceneId) : Scene(sceneId) {}

Scene0F::Scene0F(int sceneId) : Scene(sceneId) {
    mUnknown62C8[0] = 0;
    mUnknown62C8[1] = 0;
    mUnknown62C8[2] = 0;
    mUnknown62C8[3] = 0;
    mUnknown62C4 = 0;
}

TtsTestScene::TtsTestScene(int sceneId) : Scene(sceneId) {}

EmptyScene::EmptyScene(int sceneId) : Scene(sceneId) {}

Scene17::Scene17(int sceneId) : Scene(sceneId) {}

Scene18::Scene18(int sceneId) : Scene(sceneId) {}

// Scenes 0x11 to 0x15, overlay 74
RoomScene::RoomScene(int sceneId) : Scene(sceneId) {
    mResidentCount = 0;
    mScanIndex = 0;
    mEventTimer = 0;
    mIdleFrames = 0;
    SetFadeState(1);
    mEncounterPending = FALSE;
    mEventFrames = 0;
    MI_CpuFill8(mResidents, 0xFF, sizeof(mResidents));
}

// Scene 0x6B, overlay 73 (its constructor comes here, after RoomScene's: the tutorial's room is built from the
// apartment's classes)
TutorialRoomScene::TutorialRoomScene(int sceneId) : Scene(sceneId) {
    mResidentCount = 0;
    SetFadeState(1);
    mTouchStep = 0;
}

// (RoomScene, TutorialRoomScene)

Scene19::Scene19(int sceneId) : Scene(sceneId) {}

FriendListScene::FriendListScene(int sceneId) : Scene(sceneId) {}

WirelessTestScene::WirelessTestScene(int sceneId) : Scene(sceneId) {}

TestNg20Scene::TestNg20Scene(int sceneId) : Scene(sceneId) {}

ChangeClothesScene::ChangeClothesScene(int sceneId) : Scene(sceneId) {}

Scene1E::Scene1E(int sceneId) : Scene(sceneId) {}

RightJobScene::RightJobScene(int sceneId) : Scene(sceneId) {}

RightJob2Scene::RightJob2Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x23, overlay 80
VoteScene::VoteScene(int sceneId) : Scene(sceneId) {}

// (VoteScene)

MiiListMenuScene::MiiListMenuScene(int sceneId) : Scene(sceneId) {}

G3SpriteTestScene::G3SpriteTestScene(int sceneId) : Scene(sceneId) {}

MiiListScene::MiiListScene(int sceneId) : Scene(sceneId) {}

// ---- Constructors 0x02040578..0x0204830c (gap38-b) ---------------------------------------------------------------

// Scene 0x27
MapModelsScene::MapModelsScene(int sceneId) : Scene(sceneId) {}

// Scene 0x29, overlay 109
TushinScene::TushinScene(int sceneId) : Scene(sceneId) {}

// Scene 0x2A, overlay 79
RankingScene::RankingScene(int sceneId) : Scene(sceneId) {}

// Scene 0x2B, overlay 10
Scene2B::Scene2B(int sceneId) : Scene(sceneId) {}

// Scenes 0x2C and 0x2D, overlay 69
SavePlayScene::SavePlayScene(int sceneId) : Scene(sceneId) {
    mAnimationCount = 0;
    mResidentCount = 0;
}

SavePlayCharacter::SavePlayCharacter() {}

SavePlayCharacter::~SavePlayCharacter() {}

// Scene 0x2E
InteriorViewScene::InteriorViewScene(int sceneId) : Scene(sceneId) {
    mInterior = 0;
    SetState(0);
}

// Scene 0x2F, overlay 83
Scene2F::Scene2F(int sceneId) : Scene(sceneId) {}

// Scenes 0x30 and 0x70, overlay 81
Scene30::Scene30(int sceneId) : Scene(sceneId) {}

// Scenes 0x31 and 0x32, overlay 11
Scene31::Scene31(int sceneId) : Scene(sceneId) {}

// Scenes 0x33 and 0x73, overlay 82
Scene33::Scene33(int sceneId) : Scene(sceneId) {}

// Scene 0x34, overlay 12
Scene34::Scene34(int sceneId) : Scene(sceneId) {}

// Scene 0x35, overlay 84
Scene35::Scene35(int sceneId) : Scene(sceneId) {}

// Scene 0x36, overlay 13
TestNg48Scene::TestNg48Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x37, overlay 14
TestNg50Scene::TestNg50Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x38, overlay 86
ChangeRoomScene::ChangeRoomScene(int sceneId) : Scene(sceneId) {}

// Scene 0x39, overlay 106
PreStartScene::PreStartScene(int sceneId) : Scene(sceneId) {
    mResidentCount = 0;
}

// Scene 0x3A, overlay 15
TestNg55Scene::TestNg55Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x3B, overlay 16
Scene3B::Scene3B(int sceneId) : Scene(sceneId) {}

// Scene 0x3C, overlay 17
Scene3C::Scene3C(int sceneId) : Scene(sceneId) {}

// Scene 0x3D, overlay 18
TestNg62Scene::TestNg62Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x3E, overlay 19
TestNg63Scene::TestNg63Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x3F, overlay 20
Scene3F::Scene3F(int sceneId) : Scene(sceneId) {}

// Scenes 0x40 and 0x6C, overlay 75
Scene40::Scene40(int sceneId) : Scene(sceneId) {}

// Scene 0x41, overlay 21
TestNg69Scene::TestNg69Scene(int sceneId) : Scene(sceneId) {}

// Scenes 0x42, 0x43, 0x44 and 0x6F, overlay 76
MiiEditScene::MiiEditScene(int sceneId) : Scene(sceneId) {}

// Scene 0x45, overlay 22
Scene45::Scene45(int sceneId) : Scene(sceneId) {}

// Scenes 0x46, 0x47, 0x48, 0x49 and 0x4A, overlay 87
MiiNewsScene::MiiNewsScene(int sceneId) : Scene(sceneId) {}

// Scene 0x4B, overlay 23
Scene4B::Scene4B(int sceneId) : Scene(sceneId) {}

// Scene 0x4C, overlay 24
Scene4C::Scene4C(int sceneId) : Scene(sceneId) {}

// Scene 0x4D, overlay 25
Scene4D::Scene4D(int sceneId) : Scene(sceneId) {}

// Scene 0x4E, overlay 26
TestNg87Scene::TestNg87Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x4F, overlay 27
Scene4F::Scene4F(int sceneId) : Scene(sceneId) {}

// Scene 0x50, overlay 28
Scene50::Scene50(int sceneId) : Scene(sceneId) {}

// Scene 0x51, overlay 29
Scene51::Scene51(int sceneId) : Scene(sceneId) {}

// Scene 0x52, overlay 30
TestNg94Scene::TestNg94Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x53, overlay 31
TestNg95Scene::TestNg95Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x54, overlay 32
Scene54::Scene54(int sceneId) : Scene(sceneId) {}

// Scene 0x55, overlay 33
TestNg100Scene::TestNg100Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x56, overlay 89
Scene56::Scene56(int sceneId) : Scene(sceneId) {}

// Scene 0x57, overlay 34
TestNg104Scene::TestNg104Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x58, overlay 35
Scene58::Scene58(int sceneId) : Scene(sceneId) {}

// Scene 0x59, overlay 36
TestNg107Scene::TestNg107Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x5A, overlay 90
RomanceScene::RomanceScene(int sceneId) : Scene(sceneId) {}

// Scenes 0x5B, 0x5C and 0xB0, overlay 92
WriteSongScene::WriteSongScene(int sceneId) : Scene(sceneId) {}

// Scene 0x5D, overlay 65
Scene5D::Scene5D(int sceneId) : Scene(sceneId) {}

// Scene 0x5E, overlay 91
CityHallScene::CityHallScene(int sceneId) : Scene(sceneId) {}

// Scenes 0x5F to 0x66, 0x6A, 0x6D and 0x6E, overlay 72
MapScene::MapScene(int sceneId) : Scene(sceneId) {}

// Scene 0x67, overlay 88
Scene67::Scene67(int sceneId) : Scene(sceneId) {}

// Scenes 0x68 and 0x71, overlay 85
Scene68::Scene68(int sceneId) : Scene(sceneId) {}

// Scene 0x69, overlay 108
TutorialStepScene::TutorialStepScene(int sceneId) : Scene(sceneId) {}

// Scene 0x72, overlay 93
Scene72::Scene72(int sceneId) : Scene(sceneId) {}

// Scenes 0x74 and 0x7E, overlay 94
Scene74::Scene74(int sceneId) : Scene(sceneId) {}

// Scene 0x75, overlay 37
Scene75::Scene75(int sceneId) : Scene(sceneId) {}

// Scene 0x76, overlay 38
TestNg111Scene::TestNg111Scene(int sceneId) : Scene(sceneId) {}

// Scenes 0x77, 0x78 and 0x79, overlay 39
TravelScene::TravelScene(int sceneId) : Scene(sceneId) {}

// Scene 0x7A, overlay 40
Scene7A::Scene7A(int sceneId) : Scene(sceneId) {}

// Scene 0x7B, overlay 41
Scene7B::Scene7B(int sceneId) : Scene(sceneId) {}

// Scene 0x7C, overlay 42
TestNg119Scene::TestNg119Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x7F, overlay 66
Scene7F::Scene7F(int sceneId) : Scene(sceneId) {
    mUnknown22B4[0] = 0;
    mUnknown22B4[1] = 0;
    mUnknown22B0 = 0;
}

// Scene 0x80, overlay 95
AffinityScene::AffinityScene(int sceneId) : Scene(sceneId) {}

// Scene 0x81, overlay 43
Scene81::Scene81(int sceneId) : Scene(sceneId) {}

// Scenes 0x82, 0x83, 0x84, 0x85, 0x86 and 0x87, overlay 44
TestNg121Scene::TestNg121Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x88, overlay 45
Scene88::Scene88(int sceneId) : Scene(sceneId) {}

// Scene 0x89, overlay 46
Scene89::Scene89(int sceneId) : Scene(sceneId) {}

// Scene 0x8A, overlay 47
TestNg125Scene::TestNg125Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x8B, overlay 48
Scene8B::Scene8B(int sceneId) : Scene(sceneId) {}

// Scene 0x8C, overlay 49
Scene8C::Scene8C(int sceneId) : Scene(sceneId) {}

// Scene 0x8D, overlay 50
Scene8D::Scene8D(int sceneId) : Scene(sceneId) {}

// Scene 0x8E, overlay 51
TestNg129Scene::TestNg129Scene(int sceneId) : Scene(sceneId) {}

// Scenes 0x8F and 0x90, overlay 52
Scene8F::Scene8F(int sceneId) : Scene(sceneId) {}

// Scene 0x91, overlay 53
Scene91::Scene91(int sceneId) : Scene(sceneId) {}

// Scene 0x92, overlay 54
TestNg133Scene::TestNg133Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x93, overlay 55
TestNg134Scene::TestNg134Scene(int sceneId) : Scene(sceneId) {}

// Scene 0x94, overlay 56
TestNg136Scene::TestNg136Scene(int sceneId) : Scene(sceneId) {}

// Scenes 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1 and 0xA2, overlay 57
TestNg137Scene::TestNg137Scene(int sceneId) : Scene(sceneId) {}

// ---- Constructors and the rest, 0x0204830c..0x0204af20 (gap38-b, taken over from gap38-c) ---------------------

// Scenes 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8 and 0xA9, overlay 58
TestNg138Scene::TestNg138Scene(int sceneId) : Scene(sceneId) {}

// Scene 0xAA, overlay 59
TestNg139Scene::TestNg139Scene(int sceneId) : Scene(sceneId) {}

// Scene 0xAB, overlay 96
WeddingScene::WeddingScene(int sceneId) : Scene(sceneId) {}

// Scenes 0xAC and 0xAD, overlay 98
OptionScene::OptionScene(int sceneId) : Scene(sceneId) {}

// Scene 0xAE, overlay 99
InformationScene::InformationScene(int sceneId) : Scene(sceneId) {}

// Scene 0xAF, overlay 101
SceneAF::SceneAF(int sceneId) : Scene(sceneId) {}

// Scene 0xB1, overlay 110
ItemTradeScene::ItemTradeScene(int sceneId) : Scene(sceneId) {}

// Scene 0xB2, overlay 102
SceneB2::SceneB2(int sceneId) : Scene(sceneId) {}

// Scene 0xB3, overlay 103
DivorceScene::DivorceScene(int sceneId) : Scene(sceneId) {}

// Scene 0xB4, overlay 100
AlbumScene::AlbumScene(int sceneId) : Scene(sceneId) {}

// Scene 0xB5, overlay 104
SceneB5::SceneB5(int sceneId) : Scene(sceneId) {}

// Scene 0xB7, overlay 60
SceneB7::SceneB7(int sceneId) : Scene(sceneId) {}

// Scene 0xB8, overlay 61
TestNg143Scene::TestNg143Scene(int sceneId) : Scene(sceneId) {}

// Scene 0xB9, overlay 62
SceneB9::SceneB9(int sceneId) : Scene(sceneId) {}

// Scenes 0xBA, 0xBB, 0xBC, 0xBD and 0xBE, overlay 63
SceneBA::SceneBA(int sceneId) : Scene(sceneId) {}

// Scene 0xBF, overlay 64
TestNg149Scene::TestNg149Scene(int sceneId) : Scene(sceneId) {}

// Scenes 0xC0 and 0xC1, overlay 97
Marriage2Scene::Marriage2Scene(int sceneId) : Scene(sceneId) {}

// Scene 0xC2, overlay 107
SaveErrorScene::SaveErrorScene(int sceneId) : Scene(sceneId) {}

// Unloads the overlay of the scene with the id, if it has one. The cases are in the order of the original's code.
void SceneManager::UnloadSceneOverlay(int sceneId) {
    switch (sceneId) {
    case 0x00:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        break;
    case 0x01:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(0));
        break;
    case 0x02:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(2));
        break;
    case 0x03:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(3));
        break;
    case 0x04:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(4));
        break;
    case 0x05:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(5));
        break;
    case 0x0F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(6));
        break;
    case 0x0C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(1));
        break;
    case 0x28:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        break;
    case 0x0D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        break;
    case 0x29:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(109));
        break;
    case 0x5D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(65));
        break;
    case 0x7F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(66));
        break;
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(74));
        break;
    case 0x6B:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(73));
        break;
    case 0x40:
    case 0x6C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(75));
        break;
    case 0x42:
    case 0x43:
    case 0x44:
    case 0x6F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(76));
        break;
    case 0x5F:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x65:
    case 0x66:
    case 0x6A:
    case 0x6D:
    case 0x6E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(72));
        break;
    case 0x1F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(70));
        break;
    case 0x20:
    case 0x21:
    case 0x22:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(71));
        break;
    case 0x1D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(67));
        break;
    case 0x1E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(68));
        break;
    case 0x2C:
    case 0x2D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(69));
        break;
    case 0x17:
    case 0x7D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(77));
        break;
    case 0x1A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(78));
        break;
    case 0x2A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(79));
        break;
    case 0x23:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(80));
        break;
    case 0x30:
    case 0x70:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(81));
        break;
    case 0x33:
    case 0x73:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(82));
        break;
    case 0x2F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(83));
        break;
    case 0x35:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(84));
        break;
    case 0x68:
    case 0x71:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(85));
        break;
    case 0x38:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(86));
        break;
    case 0x46:
    case 0x47:
    case 0x48:
    case 0x49:
    case 0x4A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(87));
        break;
    case 0x67:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(88));
        break;
    case 0x56:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(89));
        break;
    case 0x5A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(90));
        break;
    case 0x5E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(91));
        break;
    case 0x5B:
    case 0x5C:
    case 0xB0:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(92));
        break;
    case 0x69:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(108));
        break;
    case 0x72:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(93));
        break;
    case 0x74:
    case 0x7E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(94));
        break;
    case 0x80:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(95));
        break;
    case 0xAB:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(96));
        break;
    case 0xC0:
    case 0xC1:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(97));
        break;
    case 0xAC:
    case 0xAD:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(98));
        break;
    case 0xAE:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(99));
        break;
    case 0xAF:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(101));
        break;
    case 0xB2:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(102));
        break;
    case 0xB3:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(103));
        break;
    case 0xB1:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(110));
        break;
    case 0xB4:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(100));
        break;
    case 0xB5:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(104));
        break;
    case 0xB6:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(105));
        break;
    case 0x39:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(106));
        break;
    case 0xC2:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(107));
        break;
    case 0x18:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(7));
        break;
    case 0x19:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(8));
        break;
    case 0x1C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(9));
        break;
    case 0x2B:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(10));
        break;
    case 0x31:
    case 0x32:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(11));
        break;
    case 0x34:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(12));
        break;
    case 0x36:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(13));
        break;
    case 0x37:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(14));
        break;
    case 0x3A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(15));
        break;
    case 0x3B:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(16));
        break;
    case 0x3C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(17));
        break;
    case 0x3D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(18));
        break;
    case 0x3E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(19));
        break;
    case 0x3F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(20));
        break;
    case 0x41:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(21));
        break;
    case 0x45:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(22));
        break;
    case 0x4B:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(23));
        break;
    case 0x4C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(24));
        break;
    case 0x4D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(25));
        break;
    case 0x4E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(26));
        break;
    case 0x4F:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(27));
        break;
    case 0x50:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(28));
        break;
    case 0x51:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(29));
        break;
    case 0x52:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(30));
        break;
    case 0x53:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(31));
        break;
    case 0x54:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(32));
        break;
    case 0x55:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(33));
        break;
    case 0x57:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(34));
        break;
    case 0x58:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(35));
        break;
    case 0x59:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(36));
        break;
    case 0x75:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(37));
        break;
    case 0x76:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(38));
        break;
    case 0x77:
    case 0x78:
    case 0x79:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(39));
        break;
    case 0x7A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(40));
        break;
    case 0x7B:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(41));
        break;
    case 0x7C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(42));
        break;
    case 0x81:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(43));
        break;
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(44));
        break;
    case 0x88:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(45));
        break;
    case 0x89:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(46));
        break;
    case 0x8A:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(47));
        break;
    case 0x8B:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(48));
        break;
    case 0x8C:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(49));
        break;
    case 0x8D:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(50));
        break;
    case 0x8E:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(51));
        break;
    case 0x8F:
    case 0x90:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(52));
        break;
    case 0x91:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(53));
        break;
    case 0x92:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(54));
        break;
    case 0x93:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(55));
        break;
    case 0x94:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(56));
        break;
    case 0x95:
    case 0x96:
    case 0x97:
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9E:
    case 0x9F:
    case 0xA0:
    case 0xA1:
    case 0xA2:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(57));
        break;
    case 0xA3:
    case 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
    case 0xA8:
    case 0xA9:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(58));
        break;
    case 0xAA:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(59));
        break;
    case 0xB7:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(60));
        break;
    case 0xB8:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(61));
        break;
    case 0xB9:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(62));
        break;
    case 0xBA:
    case 0xBB:
    case 0xBC:
    case 0xBD:
    case 0xBE:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(63));
        break;
    case 0xBF:
        FS_UnloadOverlay(MI_PROCESSOR_ARM9, OVERLAY_ID(64));
        break;
    }
}

// Loads a sound group into the sound heap, keeping it if it is the one loaded: -1 unloads, -2 keeps what is loaded
void SceneManager::SetSoundGroup(int group) {
    if (group == -1) {
        gSoundManager.StopAllSeq(0);
        gSoundManager.LoadHeapState(0);
        mSoundGroup = group;
        return;
    }
    if (group == mSoundGroup || group == -2) {
        if (gSoundManager.GetHeapLevel() > 1) {
            gSoundManager.LoadHeapState(1);
        }
        return;
    }
    gSoundManager.StopAllSeq(0);
    gSoundManager.LoadHeapState(0);
    mSoundGroup = group;
    gSoundManager.LoadGroup(group);
    gSoundManager.SaveHeapState(1);
}
