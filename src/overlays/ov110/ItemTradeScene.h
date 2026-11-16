#ifndef OV110_ITEMTRADESCENE_H
#define OV110_ITEMTRADESCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 110, scene 0xB1: wireless play, sending and receiving items ("たべもの・服・お宝など", etc message 0xF9).
// Overlay 109 (scene 0x29, the same archive "Scn/Scene/SceneTushin_LZ.bin") sends and receives Miis; this scene is
// the other branch of wireless play. Its classes are named with the prefix "ItemTrade": the program is linked as
// one, and overlay 109's classes have the plain names.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The main program's
// classes below are STAND-INS (not decompiled): their sizes, the fields this overlay's code touches, and what the
// scene's constructor (main, func_02048b78) and destructor show of them. Several are the same as overlay 74's and
// 96's stand-ins, whose headers say more.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

// --- Models (main program 0x02029b04 to 0x0202b008) -----------------------------------------------------------

class AnimModel;

extern "C" {
void func_020267f4(void *block); // Frees a block of the main heap
}

class Model {
public:
    virtual ~Model() {
        if (mResource != NULL) {
            func_020267f4(mResource);
        }
    }
    virtual void Load(int resourceId, int unk); // func_02029b04
    virtual void LoadByName(int, const char *); // 0x02029bd4 (resource id, name): Load() with the index of the model of that name
    virtual void Draw();                        // func_02029df4

    void SetVisible(BOOL visible) { mVisible = visible; }

    /* 0x04 */ void *mResourceSet; // What ResourceSet_FindIndexByName and func_02029660 look the animations up in
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ NNSG3dResMdl *mResMdl;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ NNSG3dRenderObj mRenderObj;
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ VecFx32 mScale;
    /* 0x98 */ VecFx32 mPosition;
    /* 0xA4 */ u16 mRotX;
    /* 0xA6 */ u16 mRotY;
    /* 0xA8 */ u16 mRotZ;
    /* 0xAC */ void *mResource; // A block of the main heap that the model owns
    /* 0xB0 */ BOOL mVisible;
}; // 0xB4

class AnimModel : public Model {
public:
    virtual ~AnimModel() { RemoveAnims(); }
    virtual void Draw(); // func_0202a0e4

    void RemoveAnims(); // func_0202a464

    /* 0xB4 */ void *mAnims[8];
    /* 0xD4 */ int mAnimCount;
    /* 0xD8 */ fx32 mUnknownD8;
    /* 0xDC */ int mUnknownDC;
}; // 0xE0

extern "C" {
// The models' resource table (0x1C bytes per resource id)
extern u8 data_021223e4[];
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk); // Binds an archive's files to an id
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);    // Binds a model file to an id
void func_0202aecc(void *resources, int resourceId); // Releases an id's model and texture
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
// Gives a resource set the animations of an archive
void func_02029660(void *resourceSet, u32 resType, void *archive);
void func_02029f50(AnimModel *model, int alpha); // Sets the model's alpha (0 to 31)
}

// The model of a character with its animations and joints (include/character/CharacterModel.h has the class).
// AnimModel is a member, not a base: the scene's destructor resets the render callback and then runs AnimModel's
// destructor, without a virtual table of the character model's own being stored in between.
class CharacterModel {
public:
    ~CharacterModel() { NNS_G3dRenderObjResetCallBack(&mAnim.mRenderObj); }

    // Names the archive and the body's model for a costume (name of the archive, costume, female)
    void SetArchive(const char *archiveName, int clothes, BOOL female);
    void SetAnimFlags(BOOL playOnce, BOOL finished);
    void MakeModel(int resourceId, int unk);
    void SetPaletteColors(GXRgb color0, GXRgb color1);

    /* 0x000 */ AnimModel mAnim;
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ VecFx32 mScale;    // The scene's Init gives the mannequin's gMannequinBodyScale * 4 / 10
    /* 0x0F8 */ VecFx32 mInvScale; // and its inverses
    /* 0x104 */ u8 mUnknown104[0x3A0];
    /* 0x4A4 */ BOOL mUnknown4A4;
    /* 0x4A8 */ u8 mUnknown4A8[0x14];
}; // 0x4BC


// --- Characters (main program 0x0208eb18 to 0x0209d898) -------------------------------------------------------

// One saved character (a resident): the 0x2C0 bytes that the save data holds for each.
struct SavedCharacter {
    s8 GetPersonality(); // Its personality, 0 to 15

    /* 0x0000 */ u8 mUnknown0000[0x2BC];
    /* 0x02BC */ s8 mPersonality; // What GetPersonality computes
    /* 0x02BD */ u8 mUnknown02BD[3];
}; // 0x2C0

// The character as the main program draws it (_ZN13CharacterBaseC2Ev is its constructor, func_0208fd64 its
// destructor: the scene's destructor calls it after the model's, as a base class's).
class CharacterBase : public SavedCharacter {
public:
    CharacterBase();
    ~CharacterBase();

    /* 0x02C0 */ u8 mUnknown02C0[0x2324];
}; // 0x25E4

// The main program's character class that the scenes derive theirs from (0x2B18 bytes).
class Character : public CharacterBase {
public:
    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AA0 */ u8 mUnknown2AA0[0x78];
}; // 0x2B18

extern "C" {
// The personalities' kinds (0 to 3), by personality
extern const int data_020fba28[];
int MiiData_GetFavoriteColor(SavedCharacter *character);
int MiiData_GetSex(SavedCharacter *character);
GXRgb NFL_GetFavoriteColor(int index); // lib/NFL
}

// The model the scene dresses in a costume to show it: a body ("3MM") with a head ("3MH:/head.imd",
// "Mdl/Misc/headManikin_3D_LZ.bin"). A main-program class (src/item/Mannequin.cpp); the scene's
// constructor builds its members in place.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class Mannequin {
public:
    void Init(int bodyResourceId, int bodyUnk, int headResourceId, int headUnk);
    void Draw();
    void SetAnimation(u32 resType, int id);
    void SetVisible(BOOL visible);
    void SetAlpha(int alpha); // 0 to 31
    void SetAnimation(const char *name);

    /* 0x000 */ AnimModel mHead;
    /* 0x0E0 */ u8 mUnknown0E0[0x60];
    /* 0x140 */ VecFx32 mUnknown140; // The scene's Init gives it gMannequinHeadScale * 4 / 10
    /* 0x14C */ u8 mUnknown14C[0x5C];
    /* 0x1A8 */ CharacterModel mModel;
    /* 0x664 */ u8 mUnknown664[0x10];
    /* 0x674 */ char mArchivePath[0x1C]; // What CharacterModel::SetArchive sets for the costume
    /* 0x690 */ char mModelPath[0x28];
}; // 0x6B8

inline void Mannequin::SetAnimation(const char *name) {
    SetAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName(mModel.mAnim.mResourceSet, RESOURCE_BCA0, name));
}

// --- Items (main program 0x0209e140 to 0x0209f84c, 0x020a4d34 to 0x020a5ca0) ---------------------------------

// An item: the base class of everything the player can own (include/item/Item.h has the main program's classes).
class Item {
public:
    virtual void UnknownVirtual0();

    BOOL Add(int count); // _ZN4Item3AddEi

    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
}; // 0x14

// One kind of goods the player owns, with how many.
class InventoryItem : public Item {}; // 0x14

// A kind of clothes the player owns (func_0209e168 constructs one; the list holds 300).
class ClothesItem : public Item {
public:
    /* 0x14 */ int mUnknown14;
}; // 0x18

// The player's goods: what the save data holds of each kind, loaded and saved together.
class Inventory {
public:
    void Load();
    void Save();
    void Sort();
    void GetOwned(Item **list, int *count, int kind, u32 flags);
    void GetOwnedKind3(Item **list, int *count);
    void GetOwnedKinds4And5Again(Item **list, int *count);
    BOOL HasAnyOwnedKind1();
    BOOL HasAnyOwnedKind3();
    BOOL HasAnyOwnedKinds4And5();

    /* 0x0000 */ int mCount;
    /* 0x0004 */ InventoryItem mItems[255];
    /* 0x13F0 */ InventoryItem *mList[255];
}; // 0x17EC

// The player's clothes, in the same way.
class ClothesList {
public:
    void Load();
    void Save();
    void Sort();
    void GetOwned(Item **list, int *count, int kind);
    BOOL HasAnyOwned();

    /* 0x0000 */ int mCount;
    /* 0x0004 */ ClothesItem mItems[300];
    /* 0x1C24 */ ClothesItem *mList[300];
}; // 0x20D4

// The player's room interiors, in the same way.
class InteriorList {
public:
    void Load();
    void Save();
    void Sort();
    void GetOwned(Item **list, int *count);
    BOOL HasAnyOwned();

    /* 0x000 */ int mCount;
    /* 0x004 */ InventoryItem mItems[78];
    /* 0x61C */ InventoryItem *mList[78];
}; // 0x754

// What the item table (data_020f5f0c, 0x14 bytes an item) says of an item
struct ItemInfo {
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ u32 mUnknown0CBits0 : 4;
    /*      */ u32 mKind : 3; // 1 and 2 are kinds of their own (see ItemTradeSelection::Set)
    /*      */ u32 mUnknown0CBits7 : 25;
    /* 0x10 */ u8 mUnknown10[4];
}; // 0x14

extern "C" {
extern const ItemInfo data_020f5f0c[];
// Each fills a list of the items owned (the list, its count) and returns nothing
// Each returns whether there is anything to show on one of the tabs
// Loading and saving
}

// The 3D model of an item (constructors _ZN9ItemModelC1Ev, _ZN9ItemModelC2Ev)
class ItemModel : public AnimModel {
public:
    void SetItem(const char *archiveName, int item); 
    void LoadModel(int resourceId);                  
    void LoadAnimation(int resourceId);              
    void ResetScale();                               

    /* 0x0E0 */ Item mItem;
    /* 0x0F4 */ int mResourceId;
    /* 0x0F8 */ char mArchiveName[4];
    /* 0x0FC */ char mArchivePath[0x1E];
    /* 0x11A */ char mModelPath[0xE];
    /* 0x128 */ fx32 mBaseScale;
}; // 0x12C

// --- The room (main program 0x020a3c68 to 0x020a5ca0) ---------------------------------------------------------

class RoomDoorModel {
public:
    ~RoomDoorModel() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    /* 0x000 */ AnimModel mModel;
    /* 0x0E0 */ u8 mUnknownE0[0x78];
}; // 0x158

class RoomPart {
public:
    ~RoomPart();

    /* 0x000 */ u8 mUnknown00[0x4C];
    /* 0x04C */ AnimModel mModel;
    /* 0x12C */ u8 mUnknown12C[8];
}; // 0x134

class RoomModels {
public:
    void Draw();
    void SetState(int state);
    void SetVisible(BOOL visible);
    void SetAlpha(int alpha); // 0 to 31

    /* 0x000 */ u8 mUnknown000[4];
    /* 0x004 */ VecFx32 mPosition; // This scene moves the room up and down by its y
    /* 0x010 */ u8 mUnknown010[0x3C];
    /* 0x04C */ RoomDoorModel mDoor;
    /* 0x1A4 */ RoomPart mParts[6];
    /* 0x8DC */ u8 mUnknown8DC[0x18];
}; // 0x8F4

// The styles a room can have (data_020f72f8), 0x54 bytes each
struct RoomStyle {
    /* 0x00 */ u8 mUnknown00[8];
    /* 0x08 */ u16 mLightColor;
    /* 0x0A */ u8 mUnknown0A[0x4A];
}; // 0x54

// The interior a room shows (include/room/Room.h has the main program's classes)
class InteriorItem {
public:
    virtual u32 GetPrice();
    GXRgb GetLightColor(); // The room's light colour now

    /* 0x004 */ u16 mStyle; // Index into data_020f72f8
    /* 0x008 */ u8 mUnknown008[0xC];
}; // 0x14

class Room : public InteriorItem {
public:
    void Mount(const char *archiveName, int unk);
    void MountModel(int unk);
    void Load(int resourceId);
    void Reload(int resourceId);
    void MountAll(const char *archiveName, int unk);
    void MountPartModels(int unk);
    void LoadParts(int resourceId);
    void ReloadParts(int resourceId);
    void ReleaseAll();
    void Update();
    void SetScale(fx32 scale);

    /* 0x014 */ RoomModels mParts;
}; // 0x908

extern "C" {
extern const RoomStyle data_020f72f8[];
}

// --- The main program's touch buttons (include/SpriteButton.h) ------------------------------------------------

extern "C" {
void func_02031e88(SpriteButton *button, BOOL disabled); // Sets mUnknownB8, and draws the button darkened while it is set
void func_02032134(TextButton *button, int palette);     // Sets the overwrite palette of the button and its text
}

// --- The list (main program, 0x020a80f0 to 0x020a9e94) --------------------------------------------------------

// The main program's scrolling list of items on the lower screen ("LTL:/BG_list", "LTL:/Obj_list"; virtual table
// 0x021153BC). A stand-in with the fields this overlay reads.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class ItemListView {
public:
    virtual void Draw();
    virtual void SetVisible(BOOL visible);

    void Init();
    void SetBackground(int plane, int offset);
    void SetUpScrollBar(int a, int b, int c);
    void SetMessageFile(MessageFile *file); // Where the items' names are
    void SetMode(int mode);
    void InitRows(int resource, u16 sequence, u16 pressedSequence, int *charOffset);
    void InitCounts(int *charOffset);
    void InitUpButton(int resource, u16 sequence, u16 pressedSequence);
    void InitDownButton(int resource, u16 sequence, u16 pressedSequence);
    void InitScrollBar(int resource, int sequence, int knobSequence);
    void Update();
    void SetItems(Item **items, int count);
    void Select(int index);
    void ScrollToSelected();
    int FindItem(u16 id); // Where the item with that id is
    void ResetArrowButtons();
    void SetUnknown308(BOOL value);

    /* 0x004 */ u8 mUnknown004[8];
    /* 0x00C */ Item **mItems; // What SetItems gave it
    /* 0x010 */ int mItemCount;
    /* 0x014 */ int mUnknown014;
    /* 0x018 */ BOOL mTouched;           // An item was touched
    /* 0x01C */ int mUnknown01C;
    /* 0x020 */ int mSelected;           // The item touched, or -1
    /* 0x024 */ BOOL mUnknown024;
    /* 0x028 */ u8 mUnknown028[0x13F3C];
}; // 0x13F64


// --- Other main-program things --------------------------------------------------------------------------------

// The main program's camera
struct Camera {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights
struct Light {
    /* 0x0 */ u8 mUnknown0[6];
    /* 0x6 */ u16 mColor;
    /* 0x8 */ u8 mUnknown8[4];
}; // 0xC

// What holds the lights (data_021226e4): this scene sets their colours through it
struct Lighting {
    /* 0x00 */ u8 mUnknown00[0xE8];
    /* 0xE8 */ Light mLights[2];
};

struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
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

struct SceneManager {
    void RequestScene(int sceneId); // Asks for the next scene

    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern Camera data_02122764;
extern Light data_021227cc[];
extern Light data_021227d8[];
extern Lighting data_021226e4;
extern ScreenControl data_0211d604;
extern SceneManager gSceneManager;
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
extern const VecFx32 gMannequinHeadScale;
extern const VecFx32 gMannequinBodyScale;

void func_0202a640(Camera *camera);
void func_0202a748(Camera *camera);
void func_0202a96c(Light *light);
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void func_020278f8(PaletteLoader *loader, u16 color, int unknown);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028154(ScreenLoader *loader, int plane, int y);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);

// --- The saved game ---
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
BOOL func_0202cd48(GameData *game); // Starts writing the save data; FALSE when it cannot
BOOL func_0202cd38(GameData *game); // Not 0 while the save data is being written
}

// --- The message window's functions this scene calls besides MessageWindow.h's ---
extern "C" {
// Adds a button: (window, number, unknown, sequence, sequence when on, position, size, its sounds, unknown)
void func_02033448(MessageWindow *window, int button, int unk1, u8 sequence, u8 onSequence, WindowPoint position,
                   WindowPoint size, const int *sounds, int unk2);
// Adds a button with a text: as func_02033448, with the text's message file and message, font, plane, and
// character offset
void func_02033844(MessageWindow *window, int button, int unk1, u8 sequence, u8 onSequence, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const int *sounds, int unk2);
// Adds the window's message text: (window, file, message, font, plane, area, position, character offset)
void func_020338d0(MessageWindow *window, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   WindowPoint area, WindowPoint position, int *charOffset);
}

// --- Wireless (main program) ------------------------------------------------------------------------------------

// WH, the SDK's wireless sample code, compiled into the main program (lib/WH; its header is not on the game's
// include path, so what this scene uses is declared again here)
extern "C" {
// The SSID of the child that connected (WH_StateOutStartParent copies it in); this scene shows it as the
// partner's name
extern u16 gWHChildSsid[WM_SIZE_CHILD_SSID / sizeof(u16)];

// WH's states (WH_SYSSTATE_*)
enum {
    WH_SYSSTATE_STOP,
    WH_SYSSTATE_IDLE,
    WH_SYSSTATE_SCANNING,
    WH_SYSSTATE_BUSY,
    WH_SYSSTATE_CONNECTED,
    WH_SYSSTATE_DATASHARING,
    WH_SYSSTATE_KEYSHARING,
    WH_SYSSTATE_MEASURECHANNEL,
    WH_SYSSTATE_CONNECT_FAIL,
    WH_SYSSTATE_ERROR,
    WH_SYSSTATE_FATAL,
};
#define WH_BITMAP_EMPTY 1

typedef void (*WHReceiverFunc)(u16 aid, u16 *data, u16 size);
typedef void (*WHSendCallbackFunc)(BOOL result);

int WH_GetLinkLevel(void);
u16 WH_GetBitmap(void);
int WH_GetSystemState(void);
u16 WH_GetMeasureChannel(void);
BOOL WH_Initialize(void);
BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel);
void WH_SetReceiver(WHReceiverFunc proc);
BOOL WH_SendData(const void *data, u16 size, WHSendCallbackFunc callback);
void WH_Reset(void);
void WH_Finalize(void);
}

// The main program's layer over WH that finds and connects the other DS
#include "wireless/WirelessSession.h"

// --- This overlay's classes ----------------------------------------------------------------------------------------

class ItemTradeItemModel;
class ItemTradeScene;

// What the scene gives its list: the player's things, the message files and the scene's objects (0x3C bytes at
// 0x1B1F0 of the scene).
struct ItemTradeContext {
    /* 0x00 */ ClothesList *mCostumes;
    /* 0x04 */ Inventory *mInventory;
    /* 0x08 */ InteriorList *mInteriors;
    /* 0x0C */ MessageFile *mCommonMessages;
    /* 0x10 */ MessageFile *mTroubleMessages;
    /* 0x14 */ MessageFile *mItemMessages;
    /* 0x18 */ MessageFile *mCostumeMessages;
    /* 0x1C */ MessageFile *mRoomMessages;
    /* 0x20 */ MessageFile *mEtcMessages;
    /* 0x24 */ Character *mPlayer;
    /* 0x28 */ ItemTradeItemModel *mItemModel;
    /* 0x2C */ Mannequin *mMannequin;
    /* 0x30 */ Room *mRoom;
    /* 0x34 */ MessageText *mNameText;
    /* 0x38 */ int *mSceneState;
}; // 0x3C

// What is chosen: the kind of thing, its number, and two values for it. Its first parts are an InventoryItem and a
// ClothesItem of classes the main program defines (virtual tables data_02110b0c and data_02110b30, which the
// scene's constructor stores).
class ItemTradeSelection {
public:
    void Set(int kind, int id);

    /* 0x00 */ void *mVtable;
    /* 0x04 */ u16 mId;
    /* 0x06 */ u8 mUnknown06[0x26];
    /* 0x2C */ int mKind; // 0: an item, 1: a costume, 2: a room interior, 3 and 4 not seen
    /* 0x30 */ int mCategory;
    /* 0x34 */ int mValue;
}; // 0x38

// The five tabs over the list: items, costumes, interiors, and two more kinds of items. Each is shown when there
// is something to show on it (mMasks).
class ItemTradeTabs {
public:
    // What mPressed becomes, by tab
    enum {
        TAB_ITEMS = 0x01,
        TAB_COSTUMES = 0x02,
        TAB_INTERIORS = 0x04,
        TAB_ITEMS_2 = 0x08,
        TAB_ITEMS_3 = 0x10,
    };

    void Init(int plane, MessageFile *file, int *charOffset);
    void SetMasks(Inventory *inventory, ClothesList *costumes, InteriorList *interiors);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int tab);
    void SetEnabled(int tabs);
    void OnOpen();

    /* 0x000 */ int mPressed;  // The tab pressed this frame, as TAB_*, or 0
    /* 0x004 */ int mSelected; // Its button
    /* 0x008 */ SpriteButton mButtons[5];
    /* 0x4CC */ int mMasks[5];
}; // 0x4E0

// The four buttons under the items tab, which choose the items it lists.
class ItemTradeItemFilter {
public:
    void Init(int plane);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int button);
    int GetFilter();

    /* 0x000 */ int mPressed; // The button pressed this frame, or -1
    /* 0x004 */ int mSelected;
    /* 0x008 */ SpriteButton mButtons[4];
}; // 0x3D8

// The five buttons under the costumes tab, which choose the costumes it lists.
class ItemTradeCostumeFilter {
public:
    void Init(int plane);
    void Update();
    void Draw();
    void SetVisible(BOOL visible);
    void Select(int button);
    int GetFilter();

    /* 0x000 */ int mPressed; // The button pressed this frame, or -1
    /* 0x004 */ int mSelected;
    /* 0x008 */ SpriteButton mButtons[5];
}; // 0x4CC

// The item model of the scene, which shows the item chosen.
class ItemTradeItemModel : public ItemModel {
public:
    void Load(int resourceId);
    void Reset();
    void ChangeItem(int item);

    /* 0x12C */ int mUnknown12C;
}; // 0x130

// The list of the player's things on the lower screen, with its tabs and buttons, and what it shows of the thing
// chosen on the upper screen: an item's model, a costume on the mannequin, or a room interior.
class ItemTradeList {
public:
    enum State {
        STATE_IDLE,
        STATE_OPEN,
        STATE_RUN,
        STATE_CLOSE_REQUEST,
        STATE_CLOSE,
    };

    // What Run returns
    enum Result {
        RESULT_NONE,
        RESULT_OK,
        RESULT_BACK,
    };

    // The upper screen's views of the thing chosen
    enum Preview {
        PREVIEW_ITEM,
        PREVIEW_COSTUME,
        PREVIEW_ROOM,
    };

    void Init(ItemTradeContext *context, int *charOffset);
    int Update();
    void UpdateListMode();
    void Hide();
    void UpdatePreviews();
    void Open();
    int Run();
    void Close();
    void Draw();
    void ShowCostume(int costume);
    void LoadMannequinHead();
    void Save();
    void RemoveOne();
    void AddOne();
    BOOL Receive(int id);
    void RefreshList();
    void ShowName(int kind, int id);
    void SetRoomMode(int mode);
    void ShowRoom(u16 style);
    void UpdateRoom();
    void SetItemCamera();
    void SetRoomCamera();
    void ResetLights();
    void ResetListButtons();
    void UpdatePreviewStates();
    void ShowPending();

    /* 0x00000 */ int mLastState;
    /* 0x00004 */ int mState;
    /* 0x00008 */ int mMode;       // 0 and 1: what to send, all tabs; 2: costumes only
    /* 0x0000C */ int mChosenId;   // What OK chose
    /* 0x00010 */ BOOL mChosen;
    /* 0x00014 */ int mUnknown14;
    /* 0x00018 */ ItemTradeSelection mSelection;
    /* 0x00050 */ ClothesList *mCostumes;
    /* 0x00054 */ Inventory *mInventory;
    /* 0x00058 */ InteriorList *mInteriors;
    /* 0x0005C */ Item *mItems[300]; // What the list shows
    /* 0x0050C */ int mItemCount;
    /* 0x00510 */ ItemListView mList;
    /* 0x14474 */ ItemTradeTabs mTabs;
    /* 0x14954 */ ItemTradeItemFilter mItemFilter;
    /* 0x14D2C */ ItemTradeCostumeFilter mCostumeFilter;
    /* 0x151F8 */ TextButton mOkButton;
    /* 0x158E8 */ SpriteButton mBackButton;
    /* 0x159DC */ Mannequin *mMannequin;
    /* 0x159E0 */ ItemTradeItemModel *mItemModel;
    /* 0x159E4 */ Room *mRoom;
    /* 0x159E8 */ ItemTradeContext *mContext;
    /* 0x159EC */ u16 mRoomStyle;
    /* 0x159EE */ u16 mRoomStep;
    /* 0x159F0 */ u16 mRoomAngle;
    /* 0x159F4 */ int mRoomMode;
    /* 0x159F8 */ ScreenPoint mPreviewPositions[3];
    /* 0x15A10 */ int mPreviewOffsets[3];
    /* 0x15A1C */ int mPreviewStates[3];
    /* 0x15A28 */ int mPreviewTimers[3];
    /* 0x15A34 */ int mPendingId;    // What to show when the views are free again, or -1
    /* 0x15A38 */ int mPendingIndex; // Its place in the list, or -1
}; // 0x15A3C

// --- The scene -----------------------------------------------------------------------------------------------------

// What the scene shows of the save data's player character.
class ItemTradePlayer : public Character {
}; // 0x2B18

// Overlay 110, scene 0xB1: wireless play with items. One DS looks for a partner and the other is found; once they
// are connected, either can choose a thing from its list and send it. The scene factory (main CreateScene, case
// 177) loads overlay 110 and allocates 0x28E0C bytes; the constructor is the main program's (func_02048b78).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// list archive, "Scn/List/List_TushinItemList_LZ.bin".
class ItemTradeScene : public Scene {
public:
    // mState, by the etc message each shows
    enum State {
        STATE_CHOOSE,           // "Which one?": look for a partner, or be found
        STATE_RECRUITING,       // "Looking for a partner to come..." (the parent)
        STATE_SEARCHING,        // "Searching for a partner..." (the child)
        STATE_CHOOSE_PARTNER,   // "Partners found. Choose one"
        STATE_CONNECTING,       // "Connecting..."
        STATE_CONNECTED_PARENT, // "Connected with ..."
        STATE_CONNECTED_CHILD,
        STATE_LIST,             // The list: choose what to send (no window)
        STATE_ASK_SEND,         // "Send ...? All right?"
        STATE_SENDING,          // "Sending and saving..."
        STATE_RECEIVING,        // "Receiving..."
        STATE_SAVING,           // "Saving..."
        STATE_SENT,             // "Sent"
        STATE_PARTNER_FULL,     // "The partner has too many of it: it could not be sent"
        STATE_RECEIVED,         // "... has come"
        STATE_FULL,             // "There are too many of it: it could not be received"
        STATE_DISCONNECTED,     // "The connection was cut"
        STATE_ASK_DISCONNECT,   // "Disconnect?"
        STATE_WAIT,             // "Please wait..."
        STATE_ERROR,            // "An error occurred"
    };

    // What one DS sends the other besides an item (ItemTradeComm::mCommand)
    enum Command {
        COMMAND_ITEM,     // The item, four bytes
        COMMAND_1,
        COMMAND_SENDING,  // An item is about to come
        COMMAND_READY,    // Ready to receive it
        COMMAND_RECEIVED, // It came
        COMMAND_FULL,     // It came, but there are too many of it already
    };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetupQuestion(int *charOffset, MessageFile *file, u32 message);
    void SetupWindow(int state);
    void Init3d();
    void UpdateWireless();
    void UpdateMusic();
    void UpdateLinkIcon();
    void UpdateSend();
    void InitWirelessWhenIdle();
    void Send(int command);
    void SetEntry(BOOL entry);
    void ClearNames();
    void WriteSave();

    /* 0x00010 */ ItemTradeList mList;
    /* 0x15A4C */ Mannequin mMannequin;
    /* 0x16104 */ ItemTradeItemModel mItemModel;
    /* 0x16234 */ Room mRoom;
    /* 0x16B3C */ MessageText mNameText;
    /* 0x1712C */ int mNameCharOffset;
    /* 0x17130 */ int mListCharOffset;
    /* 0x17134 */ ClothesList mCostumes;
    /* 0x19208 */ Inventory mInventory;
    /* 0x1A9F4 */ InteriorList mInteriors;
    /* 0x1B148 */ MessageFile mMessages[7]; // Common, trouble, item, costume, room, etc and explain messages
    /* 0x1B1F0 */ ItemTradeContext mContext;
    /* 0x1B22C */ ItemTradePlayer mPlayer;
    /* 0x1DD44 */ Camera *mCamera;
    /* 0x1DD48 */ Sprite mLinkIcon;      // The strength of the link
    /* 0x1DDF4 */ MessageWindow mWindow;
    /* 0x28D98 */ int mWindowCharOffset;
    /* 0x28D9C */ int mState;
    /* 0x28DA0 */ int mNextState;
    /* 0x28DA4 */ int mScroll;           // Of the upper screen's background
    /* 0x28DA8 */ int mWait;
    /* 0x28DAC */ wchar_t mNames[3][11]; // Of the partners found
    /* 0x28DF0 */ BOOL mMusicPlaying;
    /* 0x28DF4 */ int mSendStep;      // 2: take the item away and save; 3: wait for the save; 1: send when ready
    /* 0x28DF8 */ BOOL mPartnerReady;  // COMMAND_READY came
    /* 0x28DFC */ BOOL mPartnerSending; // COMMAND_SENDING came
    /* 0x28E00 */ BOOL mIsParent;
    /* 0x28E04 */ BOOL mSaveAfterReturn; // The partner sent the item back: save once it is added again
    /* 0x28E08 */ BOOL mWindowDone;
}; // 0x28E0C

#endif // OV110_ITEMTRADESCENE_H
