#include "WirelessTestScene.h"
#include "MainProgram3d.h"

#include "GameData.h"
#include "SceneManager.h"

#include <string.h>

#include "text/TextUtil.h"

extern "C" {
extern void *data_02123440[];                  // The game's clock
void func_02035460(void *clock, RTCDate *date, RTCTime *time, BOOL unknown); // Sets it (as overlay 98 declares it)
void Clock_GetDate(void *clock, RTCDate *date);
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads a saved character
void func_0202c28c(GameData *game, int index, SavedCharacter *character); // Writes one
BOOL func_0202c354(GameData *game, int index); // Whether there is a saved character with that index
int NFL_GetCharInfo(void *info, const void *mii); // lib/NFL: the Mii's name and fields, 7 for invalid data
}

// What NFL_GetCharInfo fills in
struct MiiCharInfo {
    /* 0x00 */ wchar_t name[11];
    /* 0x16 */ u8 mUnknown16[0x26];
}; // 0x3C

static const wchar_t *sRoleNames[2] = {L"通信モード－親（送信）", L"通信モード－子（受信）"};

static u16 sChunk;          // The packet being sent or received, of four
// (declared in the order that places them in the pool as the ROM has them)
static BOOL sSendReady;     // The last packet has gone out
static int sParentCount;    // Parents found by the scan
static int sShownParentCount;
static BOOL sReceived;      // The four packets are in
static u16 sUserGameInfo[12]; // The owner's nickname, sent with the parent's beacon
static WMBssDesc sParents[WirelessTestScene::PARENT_MAX];
static SavedCharacter sCharacter; // What is sent or received

static const wchar_t *sStateTexts[6][2] = {
    {L"スタートで通信開始", L"スタートで通信開始"},
    {L"接続中", L"接続中  親を選択してスタート"},
    {L"左右でMii選択、スタートで送信", L"親の送信を待っています"},
    {L"送信中", L"　受信中"},
    {L"送信成功(タッチで終了)", L"受信成功(タッチで終了)"},
    {L"エラー発生", L"エラー発生"},
};

// The library's character type is u16, not this compiler's wchar_t (as in overlay 0's TextTestScene)
static inline const NNSG2dChar *ToG2dChars(const wchar_t *text) {
    return (const NNSG2dChar *)text;
}

void WirelessTestScene::StartLoading() {
}

void WirelessTestScene::Init() {
    WH_Initialize();
    G2_SetBG2ControlText(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3800, GX_BG_CHARBASE_0x18000);
    MI_CpuClearFast(G2_GetBG2CharPtr(), 0x20);
    MI_CpuClearFast(G2_GetBG2ScrPtr(), 0x800);
    gScreenControl.Show(4);
    mRole = 0;
    mState = STATE_IDLE;
    mSelectedParent = 0;
    mCharacter = 0;
    sChunk = 0;
    sSendReady = TRUE;
    sReceived = FALSE;
    mCanvas.InitText(&gFonts[2].mFont, 4, 25, 6, 2, 1, 1, 0, 1, 15, NULL, NNS_G2D_TEXT_BG_WIDTH_256,
                     NNS_G2D_CHARA_COLORMODE_16);
    mCanvas.mColor = 1;
    Redraw();
    RTCDate date;
    RTCTime time;
    date.year = 11;
    date.month = 2;
    date.day = 3;
    time.hour = 12;
    time.minute = 13;
    time.second = 14;
    date.week = RTC_WEEK_SUNDAY;
    func_02035460(data_02123440[0], &date, &time, FALSE);
    Clock_GetDate(data_02123440[0], &date);
}

void WirelessTestScene::UpdateIdle() {
    if (data_0211f9d4.mTrigger & PAD_BUTTON_START) {
        WH_SetReceiver(OnReceive);
        mState = STATE_CONNECTING;
        Redraw();
        if (mRole == 0) {
            WH_StartMeasureChannel();
        } else {
            static const u8 sAnyParent[WM_SIZE_BSSID] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            sParentCount = 0;
            sShownParentCount = 0;
            WH_StartScan(OnParentFound, sAnyParent, 0);
        }
    } else if (gTouchInput.mTouched) {
        mRole++;
        mRole %= 2;
        Redraw();
    }
}

void WirelessTestScene::ConnectToParent() {
    WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &sParents[mSelectedParent]);
    mState = STATE_CONNECTED;
    Redraw();
}

void WirelessTestScene::Update() {
    if (sReceived == TRUE) {
        mState = STATE_DONE;
        sReceived = FALSE;
        Redraw();
    }
    if (mState == STATE_DONE) {
        if (gTouchInput.mTouched) {
            gSceneManager.RequestScene(0x11);
        }
        return;
    }
    switch (WH_GetSystemState()) {
    case WH_SYSSTATE_IDLE: {
        OSOwnerInfo info;
        OS_GetOwnerInfo(&info);
        for (int i = 0; i < info.nickNameLength; i++) {
            sUserGameInfo[i] = info.nickName[i];
        }
        WH_SetUserGameInfo(sUserGameInfo, info.nickNameLength * 2);
        if (mState == STATE_CONNECTING) {
            ConnectToParent();
        } else {
            UpdateIdle();
        }
        break;
    }
    case WH_SYSSTATE_MEASURECHANNEL:
        WH_ParentConnect(WH_CONNECTMODE_MP_PARENT, WM_GetNextTgid(), WH_GetMeasureChannel());
        break;
    case WH_SYSSTATE_CONNECTED:
        if (WH_GetBitmap() != 1 && mState == STATE_CONNECTING) {
            mState = STATE_CONNECTED;
            Redraw();
        }
        if (mRole == 0) {
            if (data_0211f9d4.mTrigger & PAD_KEY_RIGHT) {
                mCharacter++;
                if (mCharacter >= 100) {
                    mCharacter = 0;
                }
                Redraw();
            }
            if (data_0211f9d4.mTrigger & PAD_KEY_LEFT) {
                mCharacter += 150;
                mCharacter--;
                mCharacter %= 150;
                Redraw();
            }
            if ((data_0211f9d4.mTrigger & PAD_BUTTON_START) && sChunk == 0) {
                sChunk = 1;
                mState = STATE_SENDING;
                func_0202c264(&gGameData, mCharacter, &sCharacter);
                WH_SendData(&sCharacter, 0x80, OnSent);
                sSendReady = FALSE;
            }
            if (sSendReady == TRUE && mState == STATE_SENDING) {
                WH_SendData((u8 *)&sCharacter + sChunk * 0x80, 0x80, OnSent);
                sChunk++;
                if (sChunk >= 4) {
                    sChunk = 0;
                    mState = STATE_DONE;
                    Redraw();
                }
            }
        }
        break;
    case WH_SYSSTATE_SCANNING:
        if (sParentCount != sShownParentCount) {
            int i;
            for (i = 0; i < sParents[sShownParentCount].gameInfoLength / 2; i++) {
                mParentNames[sShownParentCount][i] = sParents[sShownParentCount].gameInfo.userGameInfo[i];
            }
            mParentNames[sShownParentCount][sParents[sShownParentCount].gameInfoLength / 2] = 0;
            mParentNames[sShownParentCount][sParents[sShownParentCount].gameInfoLength / 2 + 1] = 0;
            Redraw();
            sShownParentCount = sParentCount;
        }
        if (sParentCount > 0) {
            if (data_0211f9d4.mTrigger & PAD_KEY_UP) {
                mSelectedParent = (mSelectedParent + (sParentCount - 1)) % sParentCount;
                Redraw();
            }
            if (data_0211f9d4.mTrigger & PAD_KEY_DOWN) {
                mSelectedParent = (mSelectedParent + (sParentCount + 1)) % sParentCount;
                Redraw();
            }
            if (data_0211f9d4.mTrigger & PAD_BUTTON_START) {
                WH_EndScan();
            }
        }
        break;
    }
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
    }
}

void WirelessTestScene::Draw() {
}

void WirelessTestScene::Exit() {
    NNS_G2dCharCanvasClear(&mCanvas.mCanvas, 0);
    WH_Finalize();
    while (WH_GetSystemState() != WH_SYSSTATE_IDLE) {
    }
    WH_End();
}

// Compiled at optimisation level 2 with propagation off: the loop is not rotated, and the constants stay in registers
#pragma push
#pragma optimization_level 2
#pragma opt_propagation off
void WirelessTestScene::Redraw() {
    NNS_G2dCharCanvasClear(&mCanvas.mCanvas, 0);
    mCanvas.mTextX = 0;
    mCanvas.mTextY = 4;
    mCanvas.mText = ToG2dChars(sRoleNames[mRole]);
    mCanvas.DrawText();
    mCanvas.mTextY = 16;
    mCanvas.mText = ToG2dChars(sStateTexts[mState][mRole]);
    mCanvas.DrawText();
    switch (mState) {
    case STATE_IDLE:
        mCanvas.mTextX = 0;
        mCanvas.mTextY = 28;
        mCanvas.mText = ToG2dChars(L"タッチで送受信切り替え");
        mCanvas.DrawText();
        break;
    case STATE_CONNECTING:
        for (int i = 0; i < sParentCount; i++) {
            mCanvas.mTextY = i * 12 + 28;
            if (i == mSelectedParent) {
                mCanvas.mTextX = 0;
                mCanvas.mText = ToG2dChars(L"★");
                mCanvas.DrawText();
            }
            mCanvas.mTextX = 24;
            mCanvas.mText = (const NNSG2dChar *)mParentNames[i];
            mCanvas.DrawText();
        }
        break;
    case STATE_CONNECTED:
        if (mRole == 0) {
            func_0202c264(&gGameData, mCharacter, &sCharacter);
            wchar_t number[4];
            FormatNumber(mCharacter, number, sizeof(number), 3, TRUE);
            mCanvas.mTextX = 32;
            mCanvas.mTextY = 28;
            mCanvas.mText = ToG2dChars(number);
            mCanvas.DrawText();
            mCanvas.mTextX = 80;
            MiiCharInfo info;
            if (func_0202c354(&gGameData, mCharacter)) {
                NFL_GetCharInfo(&info, &sCharacter);
                mCanvas.mText = ToG2dChars(info.name);
            } else {
                mCanvas.mText = ToG2dChars(L"データがありません");
            }
            mCanvas.DrawText();
        }
        break;
    }
}
#pragma pop

void WirelessTestScene::OnSent(BOOL) {
    sSendReady = TRUE;
}

void WirelessTestScene::OnReceive(u16, u16 *data, u16 size) {
    if (size == 0) {
        return;
    }
    MI_CpuCopy8(data, (u8 *)&sCharacter + sChunk * 0x80, 0x80);
    if (sChunk < 3) {
        sChunk++;
        return;
    }
    for (int i = 0; i < 100; i++) {
        if (!func_0202c354(&gGameData, i)) {
            sCharacter.Register();
            func_0202c28c(&gGameData, i, &sCharacter);
            break;
        }
    }
    sChunk = 0;
    sReceived = TRUE;
}

void WirelessTestScene::OnParentFound(WMBssDesc *bssDesc) {
    for (int i = 0; i < sParentCount; i++) {
        if (memcmp(sParents[i].bssid, bssDesc->bssid, 12) == 0) {
            return;
        }
    }
    WH_PrintBssDesc(bssDesc);
    sParents[sParentCount++] = *bssDesc;
}
