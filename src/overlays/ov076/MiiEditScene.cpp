// Overlay 76, the Mii maker (SceneMiiEdit2). Not decompiled yet: see MiiEditScene.h for what is measured.
#include "MiiEditScene.h"

// --- The main program (stand-ins, not decompiled) ---
struct ArchiveLoader;
struct ScreenControl;
struct ScreenLoader;
extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
extern u32 data_021223e4[];
void func_0202aecc(void *resources, int resourceId);
void func_0202683c(void);
extern ScreenControl data_0211d604;
extern ScreenLoader data_02121834;
void func_02024234(ScreenControl *screens, int planes, int a);
void func_02023f4c(ScreenControl *screens, int planes, int a);
void func_02023eac(ScreenControl *screens, int planes);
void func_02028108(ScreenLoader *loader, int plane, int offset);

// The pages' Exit functions (each takes its page, a part of the scene)
void func_ov076_0214c6a4(void *page);
void func_ov076_0214ee78(void *page);
void func_ov076_02153bd4(void *page);
void func_ov076_02156eb4(void *page);
void func_ov076_02158044(void *page);
void func_ov076_0215834c(void *page);
void func_ov076_02158608(void *page);
}

// Where the scene's pages are (measured: Draw and Exit pass these to the pages' functions by mPage)
#define PAGE(offset) ((u8 *)this + (offset))

void MiiEditScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 0x1F);
    ArchiveManager_Unmount(&gArchiveManager, "E00");
    ArchiveManager_Unmount(&gArchiveManager, "B0I");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "SPL");
    func_0202683c();
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "T02");
    func_02024234(&data_0211d604, 0x3F00, 0);
    func_02023f4c(&data_0211d604, 0x3F00, 0);
    func_02023eac(&data_0211d604, 0x3F00);
    func_02028108(&data_02121834, 4, 0);
    switch (mPage) {
    case 0:
        func_ov076_0214c6a4(PAGE(0x1C128));
        break;
    case 1:
        func_ov076_0214ee78(PAGE(0x32080));
        break;
    case 2:
        func_ov076_02153bd4(PAGE(0x354A8));
        break;
    case 3:
        func_ov076_02156eb4(PAGE(0x7DE74));
        break;
    case 4:
        func_ov076_02158044(PAGE(0x81C74));
        break;
    case 6:
        func_ov076_0215834c(PAGE(0x82498));
        break;
    case 5:
        func_ov076_02158608(PAGE(0x8D448));
        break;
    }
}
