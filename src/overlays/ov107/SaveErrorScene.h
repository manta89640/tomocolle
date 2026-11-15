#ifndef OV107_SAVEERRORSCENE_H
#define OV107_SAVEERRORSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "MessageWindow.h"
#include "Scene.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// The screens: which planes are shown.
struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0x44];
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x14];
    /* 0xEC */ int mUnknownEC;
};

// Graphics resources, loaded by file name into VRAM.
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);

extern ScreenControl data_0211d604;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_020278f8(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}

// --- The scene ----------------------------------------------------------------------------------------------------

// Overlay 107, scene 0xC2: the message that the save data could not be written ("データを書き込めませんでした。
// 電源を切って、カードを差込みなおしてください。", message 0x1A6 of the etc messages), in a message window on the
// lower screen, and nothing after it: the player has to turn the console off. The scenes that save (overlays 69 and
// 106) ask for scene 0xC2 when the main program's save function fails. The scene factory (main CreateScene, case
// 194) loads overlay 107, allocates 0xAFEC bytes and calls the constructor, which is in the main program
// (func_02049e40).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class SaveErrorScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    void UpdateWindow();
    BOOL ShowMessage(u32 message);

    /* 0x0010 */ int mState;
    /* 0x0014 */ MessageWindow mWindow[1];
    /* 0xAFB8 */ MessageFile mEtcMessages;
    /* 0xAFD0 */ MessageFile mCommonMessages;
    /* 0xAFE8 */ s8 mSubStep; // Of ShowMessage
    /* 0xAFE9 */ s8 mStep;
}; // 0xAFEC

#endif // OV107_SAVEERRORSCENE_H
