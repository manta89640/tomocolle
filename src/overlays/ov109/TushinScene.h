#ifndef OV109_TUSHINSCENE_H
#define OV109_TUSHINSCENE_H

#include <nitro.h>
#include "text/TextUtil.h"
#include <nnsys.h>

#include "GameData.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ScreenControl.h"
#include "Sprite.h"
#include "character/SceneCharacter.h"
#include "island/IslandProgress.h"
#include "item/Item.h"
#include "list/MiiList.h"
#include "song/Song.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "wireless/WirelessSession.h"

#include <wchar.h>

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

// The main program's classes the scene holds, by the names the scene's code gives them
typedef MiiListPanel ListView;
typedef MiiList ResidentList;
typedef MiiListTabs ListPages;

struct Camera;          // The main program's camera (MainProgram.h)
struct ParticleEmitter; // The particle library's (MainProgram.h)
struct ParticleSystem;

// --- The scene ----------------------------------------------------------------------------------------------------

// The layout of one kind of window: which of its four buttons it has
struct WindowLayout {
    /* 0x0 */ BOOL mHasButton[3];
    /* 0xC */ BOOL mHasBackButton;
};

// Overlay 109, scene 0x29: wireless play (通信), the Mii exchange. The scene save screen (overlay 69, scene 0x2D)
// comes here after saving. Its pages (mPage, an index of sPageMessages and sPageLayouts) show etc messages 0xF7 to
// 0x125: what to exchange (Miis, or food, clothes and treasure, which is scene 0xB1, overlay 110), with what (another
// Tomodachi Collection, through WH, or the Wii's Mii Channel, through the main program's Mii transfer code), and on
// a DS whether to recruit a partner (the parent) or to find one (a child). A resident is chosen in the list on the
// lower screen and sent: the 0x2C0-byte character (sTransfer) in 0x80-byte chunks, then the songs of its six slots
// (sSongs); the receiver checks it (IsInvalid, the NG words of HasNgWord, a resident with the same Mii), stores it
// as a new resident or updates the one it has, and answers with a 2-byte code (sCommand). The Wii sends Miis
// only: ReceiveWiiMii adds one from sMiiData. A DS or Wii T-shirt is given the first time (GiveShirt,
// GiveWiiShirt). The scene factory (main CreateScene, case 41) loads overlay 109 and allocates 0x3F0B4 bytes; the
// constructor is the main program's (func_02040644).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// archive, "Scn/Scene/SceneTushin_LZ.bin".
class TushinScene : public Scene {
public:
    TushinScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    SavedCharacter *GetSelectedResident() { return &mResidents.mResidents[mResidents.mSelected]; }

    void SetUpYesNoWindow(int *charOffset, MessageFile *file, u32 message);
    void SetUpWindow(int page);
    void SetUpList(int *charOffset);
    void InitCharacters();
    void InitParticles();
    void UpdateMiiData();
    void ResetExchange();
    void SetUpCharacter(int i);
    int GetStandAnimation(int i);
    void ReloadCharacter(int i);
    void LoadCharacters();
    void ShowNextCharacter();
    void LoadSavedCharacter(int i, int index);
    void LoadCharacter(int i);
    void ReloadCharacterFromResident(int i);
    void ReloadCharacterFromReceived(int i);
    void SendSongs();
    void Send(int kind);
    void UpdateSending();
    void SendFoundIndex();
    void UpdateSaving();
    int FindResident(CharacterRecord *record);
    void RefreshRecord(CharacterRecord *record);
    int ReceiveCharacter();
    int GetMode();
    BOOL IsNgWord(const wchar_t *text);
    BOOL IsBlank(const wchar_t *text);
    BOOL HasNgWord(CharacterRecord *record);
    BOOL HasNgSongReceived();
    BOOL HasNgSong(CharacterRecord *record);
    BOOL IsMiiBroken(CharacterRecord *record);
    BOOL IsNameBlank(CharacterRecord *record);
    BOOL IsNicknameBlank(CharacterRecord *record);
    BOOL IsInvalid(CharacterRecord *record);
    int ReceiveWiiMii();
    void UpdateCharacters();
    void DrawCharacters();
    void UpdateSignal();
    void Disconnect(int kind);
    void Leave();
    void UpdateListButtons();
    void UpdateListWindow();
    void UpdateConnection();
    void UpdateWiiConnection();
    void SetStandAnimation(u8 i);
    void SetOtherStandAnimation();
    void StartSendAnimation();
    void StartReceiveAnimation();
    void UpdateMusic();
    void ResetList();
    void ShowReceived();
    void CloseWindow();
    void StartEmitter(int i);
    void CountFrame();
    void DrawParticles();
    BOOL CannotSend(CharacterRecord *record);
    BOOL GiveItem(int item);
    BOOL GiveShirt();
    BOOL GiveWiiShirt();
    void SetEntry(BOOL enabled);
    void ClearParentNames();
    void Save();
    void SetNeedsSave();
    int NextPageAfterReceive();

    /* 0x00010 */ int mPage;     // What the window shows: an index of sPageMessages and sPageLayouts
    /* 0x00014 */ int mNextPage;
    /* 0x00018 */ int mPageAfterSave;
    /* 0x0001C */ int mBgScroll;
    /* 0x00020 */ int mWait;
    /* 0x00024 */ int mCloseListToReceive;
    /* 0x00028 */ int mLostInList;
    /* 0x0002C */ MessageFile mMessageFiles[2]; // Etc messages, common messages
    /* 0x0005C */ int mUnknown5C;
    /* 0x00060 */ MessageWindow mWindow;
    /* 0x0B004 */ ResidentList mResidents;
    /* 0x1C3AC */ ListView mList;
    /* 0x25824 */ ListPages mPages;
    /* 0x2A97C */ MessageWindow mListWindow;
    /* 0x35920 */ u8 mSortKey;
    /* 0x35924 */ int mUnknown35924;
    /* 0x35928 */ TextButton mButtons[3];
    /* 0x36DF8 */ Sprite mSprite;
    /* 0x36EA4 */ int mCharOffset;
    /* 0x36EA8 */ int mUnknown36EA8;
    /* 0x36EAC */ void *mPalette;
    /* 0x36EB0 */ Camera *mCamera;
    /* 0x36EB4 */ SceneCharacter mCharacters[3];
    /* 0x3EFFC */ u8 mCurrent;
    /* 0x3F000 */ int mCharactersVisible;
    /* 0x3F004 */ ParticleSystem *mParticles;
    /* 0x3F008 */ ParticleEmitter *mEmitters[2];
    /* 0x3F010 */ void *mParticleFile;
    /* 0x3F014 */ VecFx32 mEmitterPositions[2];
    /* 0x3F02C */ int mParticleResource;
    /* 0x3F030 */ int mEmitterStarted;
    /* 0x3F034 */ int mParticleFrames;
    /* 0x3F038 */ wchar_t mParentNames[3][11]; // Of the Wiis (or DSs) found
    /* 0x3F07A */ u16 mParentIds[3];
    /* 0x3F080 */ int mMusicPlaying;
    /* 0x3F084 */ int mSendQueued;
    /* 0x3F088 */ int mPartnerReady;
    /* 0x3F08C */ int mPartnerSending;
    /* 0x3F090 */ int mUnknown3F090;
    /* 0x3F094 */ int mIsReceiveAnimation;
    /* 0x3F098 */ int mNeedsSave;
    /* 0x3F09C */ int mSongStoreFull;
    /* 0x3F0A0 */ int mShirtDue;
    /* 0x3F0A4 */ int mWiiShirtDue;
    /* 0x3F0A8 */ int mReceiveResult;
    /* 0x3F0AC */ int mWindowReady;
    /* 0x3F0B0 */ int mConnectRequested;
}; // 0x3F0B4

#endif // OV109_TUSHINSCENE_H
