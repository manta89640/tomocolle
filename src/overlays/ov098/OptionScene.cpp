#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "sound/SoundManager.h"

#include "OptionScene.h"

// Overlay 98: the settings (OptionScene). One source file: its constants are the .rodata (see the comment on
// them), its strings the .data after the scene's virtual table.

// The file's constants, all in one place and in this order, because the order decides where the compiler puts
// them: it keeps a list of the file's data objects (the virtual table last), newest first, and heap-sorts it by size;
// the four-byte-aligned ones are one pool at the start of .rodata, read by offset from its start (see
// docs/notes/ov073.md and docs/notes/ov098.md). This order gives the ROM's order of the objects.
//
// The ROM's pool starts with 36 bytes that no code refers to, here sUnknown0 to sUnknown4 with the ROM's values.
// What they are is not known (see docs/notes/ov098.md): they stand in for them so that the constants after them
// are at the ROM's offsets, which OpenWindow reads. Exit refers to them to keep them, which makes no code.
static const int sUnknown0[1] = {0};
static const WindowPoint sClockTextPosition = {38, 33};
static const WindowPoint sUnknown3 = {2, 2};
static const WindowPoint sUnknown1 = {2, 0};
static const int sWindowButtonMessages[2] = {2, 3}; // "はい" and "いいえ" of the common messages
static const WindowPoint sMessageTextPosition = {38, 33};
static const WindowPoint sOkButtonPosition = {128, 149};
static const WindowPoint sQuestionTextPosition = {38, 33};
static const WindowPoint sSetClockTextPosition = {38, 33};
static const WindowPoint sUnknown2 = {0, 4};
static const WindowPoint sWindowButtonSize = {94, 30};
static const WindowPoint sUnknown4 = {2, 2};
static const u16 sTextButtonSequences[OptionScene::TEXT_BUTTON_COUNT] = {3, 3, 3, 10, 8, 8, 10};
static const u32 sTextMessages[OptionScene::TEXT_COUNT] = {0xDB, 0xDE, 0xDF, 0xE0};
static const int sTextFonts[OptionScene::TEXT_COUNT] = {0, 0, 0, 0};
static const ScreenRect sScreenRect = {0, 0, 256, 192};
static const ScreenRect sAmPmRect = {0, 0, 32, 32};
static const WindowPoint sWindowButtonPositions[2] = {{75, 149}, {181, 149}};
static const ScreenRect sWindowRect = {20, 20, 216, 152};
static const u16 sWindowMessages[10] = {0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0x150, 0x151};
static const char sWindowScreenName[] = "N83:/Window_down.nsc";
static const u32 sTextButtonMessages[OptionScene::TEXT_BUTTON_COUNT] = {0xD7, 0xD8, 0xDA, 0xE8, 0xDC, 0xDD, 0xD9};
static const int sTextButtonKinds[OptionScene::TEXT_BUTTON_COUNT] = {4, 4, 4, 4, 3, 3, 4};
static const int sTextButtonFonts[OptionScene::TEXT_BUTTON_COUNT] = {2, 2, 2, 2, 0, 0, 2};
static const ScreenPoint sTextPositions[OptionScene::TEXT_COUNT] = {{20, 40}, {20, 80}, {20, 120}, {20, 160}};
static const u16 sSpriteSequences[OptionScene::SPRITE_COUNT] = {6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 5, 5, 5};
static const ButtonSounds sWindowButtonSounds[2] = {{{10, 1, 0, -1}}, {{12, 1, 0, -1}}};
static const u16 sButtonSequences[OptionScene::BUTTON_COUNT] = {
    1, 12, 14, 12, 14, 12, 14, 12, 14, 12, 14, 18, 16, 18, 16, 18, 16,
};
static const ScreenPoint sTextButtonPositions[OptionScene::TEXT_BUTTON_COUNT] = {
    {128, 54}, {128, 96}, {128, 138}, {128, 173}, {140, 46}, {208, 46}, {128, 173},
};
static const ScreenPoint sTextButtonSizes[OptionScene::TEXT_BUTTON_COUNT] = {
    {210, 36}, {210, 36}, {210, 36}, {94, 30}, {64, 22}, {64, 22}, {94, 30},
};
static const int sButtonKinds[OptionScene::BUTTON_COUNT] = {4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0};
static const ButtonSounds sTextButtonSounds[OptionScene::TEXT_BUTTON_COUNT] = {
    {{10, -1, 0, -1}}, {{10, -1, 0, -1}}, {{10, -1, 0, -1}}, {{10, -1, 0, -1}},
    {{0, -1, -1, -1}}, {{0, -1, -1, -1}}, {{10, -1, 0, -1}},
};
static const ScreenPoint sSpritePositions[OptionScene::SPRITE_COUNT] = {
    {26, 49},  {46, 49},  {66, 49},  {86, 49},   {134, 49}, {154, 49}, {198, 49}, {218, 49},
    {82, 119}, {102, 119}, {154, 119}, {174, 119}, {52, 120}, {170, 86}, {170, 126}, {170, 166},
};
static const ScreenPoint sButtonSizes[OptionScene::BUTTON_COUNT] = {
    {24, 22}, {18, 13}, {18, 13}, {18, 13}, {18, 13}, {18, 13}, {18, 13}, {18, 13}, {18, 13},
    {18, 13}, {18, 13}, {20, 20}, {20, 20}, {20, 20}, {20, 20}, {20, 20}, {20, 20},
};
static const ScreenPoint sButtonPositions[OptionScene::BUTTON_COUNT] = {
    {242, 13}, {56, 23},  {56, 75},  {144, 23}, {144, 75}, {208, 23}, {208, 75}, {92, 93},  {92, 145},
    {164, 93}, {164, 145}, {126, 86}, {222, 86}, {126, 126}, {222, 126}, {126, 166}, {222, 166},
};
static const ButtonSounds sButtonSounds[OptionScene::BUTTON_COUNT] = {
    {{12, 1, 0, -1}}, {{8, -1, -1, -1}}, {{9, -1, -1, -1}}, {{8, -1, -1, -1}}, {{9, -1, -1, -1}},
    {{8, -1, -1, -1}}, {{9, -1, -1, -1}}, {{8, -1, -1, -1}}, {{9, -1, -1, -1}}, {{8, -1, -1, -1}},
    {{9, -1, -1, -1}}, {{-1, -1, -1, -1}}, {{-1, -1, -1, -1}}, {{-1, -1, -1, -1}}, {{-1, -1, -1, -1}},
    {{-1, -1, -1, -1}}, {{-1, -1, -1, -1}},
};

// Puts a text at a position in pixels
static inline void SetTextPosition(MessageText *text, ScreenPoint position) {
    text->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

// The digit of a number that is worth "unit"
static inline int GetDigit(int value, int unit) {
    return value % (unit * 10) / unit;
}

static inline BOOL IsInRect(ScreenRect rect, ScreenPoint point) {
    return func_02028d88(&rect, &point);
}

// The rectangle around the AM/PM sprite that takes a touch: 32 pixels square around its position (52, 120)
static inline ScreenRect GetAmPmRect() {
    ScreenRect rect = sAmPmRect;
    rect.x = 36;
    rect.y = 104;
    return rect;
}

void OptionScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "N83", "Scn/Scene/SceneOption_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TET", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCO", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    if (mSceneId == 0xAC) {
        gSoundManager.StartSeqIfNotPlaying(0, 0x60);
        gSoundManager.SetSeqTrackMuteOnly(0, 0xF020, TRUE);
    }
    gSoundManager.LoadGroup(0x4B);
    gSoundManager.LoadGroup(0x5C);
}

void OptionScene::Init() {
    int charOffset = 0x2BC;
    mMessageFiles[0].Open("TET:/tmd_etc_msg_JP.bmg");
    mMessageFiles[1].Open("TCO:/tmd_common_msg_JP.bmg");
    mScroll = 0;

    func_02027534(&data_021213b8, "N83:/BG_down.ncl", 0x300, -1, -1, -1);
    func_02027d7c(&data_02121834, "N83:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "N83:/BG_down.ncg", 0x300, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "N83:/BG_up.ncl", 2, -1, -1, -1);
    func_02027d7c(&data_02121834, "N83:/BG_up.nsc", 2, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "N83:/BG_up.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "N83:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "N83:/Obj_down.nce", "N83:/Obj_down.ncg", 0x1000, 1, -1);

    for (int a = 0; a < BUTTON_COUNT; a++) {
        u16 sequence = sButtonSequences[a];
        Sprite_Init(&mButtons[a], 1, sequence, 0x80, 0x60, 1);
        func_02031eb0(&mButtons[a], sButtonSizes[a].x, sButtonSizes[a].y);
        func_02031278(&mButtons[a], sequence, sequence + 1);
        func_02031260(&mButtons[a], sButtonKinds[a]);
        mButtons[a].SetPositionFromVec(sButtonPositions[a].x << FX32_SHIFT, sButtonPositions[a].y << FX32_SHIFT);
        func_02032158(&mButtons[a], &sButtonSounds[a]);
        func_0203119c(&mButtons[a]);
    }
    // The buttons of the date and time repeat while they are held
    for (int b = BUTTON_YEAR_UP; b <= BUTTON_MINUTE_DOWN; b++) {
        func_02031260(&mButtons[b], 1);
        mButtons[b].mUnknownE0 = 6;
    }

    for (int c = 0; c < TEXT_BUTTON_COUNT; c++) {
        u16 sequence = sTextButtonSequences[c];
        Sprite_Init(&mTextButtons[c], 1, sequence, 0x80, 0x60, 1);
        func_02031eb0(&mTextButtons[c], sTextButtonSizes[c].x, sTextButtonSizes[c].y);
        func_02031278(&mTextButtons[c], sequence, sequence + 1);
        func_02031260(&mTextButtons[c], sTextButtonKinds[c]);
        mTextButtons[c].SetPositionFromVec(sTextButtonPositions[c].x << FX32_SHIFT,
                                           sTextButtonPositions[c].y << FX32_SHIFT);
        func_02032158(&mTextButtons[c], &sTextButtonSounds[c]);
        func_0203119c(&mTextButtons[c]);
    }
    for (int d = TEXT_BUTTON_SOUND; d <= TEXT_BUTTON_ERASE; d++) {
        mTextButtons[d].mMoveText = FALSE;
    }
    for (u16 j = 0; j < TEXT_BUTTON_COUNT; j++) {
        charOffset += func_02031f58(&mTextButtons[j], &mMessageFiles[0], sTextButtonMessages[j],
                                    &gFonts[sTextButtonFonts[j]].mFont, 0x1000, charOffset);
    }

    for (int e = 0; e < SPRITE_COUNT; e++) {
        Sprite_Init(&mSprites[e], 1, sSpriteSequences[e], 0x80, 0x60, 1);
        mSprites[e].SetPositionFromVec(sSpritePositions[e].x << FX32_SHIFT, sSpritePositions[e].y << FX32_SHIFT);
        NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mSprites[e].mAnim));
    }

    for (int f = 0; f < TEXT_COUNT; f++) {
        mTexts[f].InitObjForMessage(&mMessageFiles[0], sTextMessages[f], &gFonts[sTextFonts[f]].mFont, 0x1000,
                                    charOffset, 0, 1, 0, 0, 0xF, NULL);
        SetTextPosition(&mTexts[f], sTextPositions[f]);
        mTexts[f].mHAlign = 0;
        mTexts[f].mVAlign = 0;
        mTexts[f].DrawAll();
        charOffset += mTexts[f].GetCharCount();
    }

    func_02028280(&data_02121834, 0x1000, 0);
    func_02028280(&data_02121834, 0x100, 1);
    func_02028280(&data_02121834, 0x200, 2);
    data_0211d604.Set(0x1302);
    mCharOffset = charOffset;
    func_02032fb8(&mWindow, sScreenRect);
    mTtsPlayer.InitEngine();
    mSpeechMessage.mMessageFile = &mMessageFiles[0];
    if (mSceneId == 0xAD) {
        SetState(STATE_FIRST_CLOCK);
        gSoundManager.StartSeqIfNotPlaying(0, 0x61);
    } else {
        SetState(STATE_MENU);
    }
}

// Does not match: the instructions are the ROM's, but the copy of the AM/PM rectangle that GetAmPmRect makes is at
// sp+0x68 and the copy passed to IsInRect at sp+0x58, where the ROM has them the other way round. Tried: the
// rectangle as a local of Update (with and without the inline), passed by value, by pointer and by reference, the
// point by value, by pointer and through a local, and the inlines defined in the other order.
void OptionScene::Update() {
    mTtsPlayer.Update();
    mScroll += 0x4CC;
    func_02028108(&data_02121834, 0x202, mScroll >> FX32_SHIFT);
    func_020341d4(&mWindow);

    switch (mState) {
    case STATE_MENU:
    case STATE_CLOCK:
    case STATE_SOUND:
    case STATE_FIRST_CLOCK_SET: {
        int i;
        for (i = 0; i < BUTTON_COUNT; i++) {
            if (!func_0203133c(&mButtons[i])) {
                continue;
            }
            switch (i) {
            case BUTTON_BACK:
                if (mState == STATE_CLOCK || mState == STATE_SOUND) {
                    SetState(STATE_MENU);
                } else {
                    func_0203c02c(&gSceneManager, 0x5E);
                }
                break;
            case BUTTON_YEAR_UP:
            case BUTTON_YEAR_DOWN:
            case BUTTON_MONTH_UP:
            case BUTTON_MONTH_DOWN:
            case BUTTON_DAY_UP:
            case BUTTON_DAY_DOWN:
            case BUTTON_HOUR_UP:
            case BUTTON_HOUR_DOWN:
            case BUTTON_MINUTE_UP:
            case BUTTON_MINUTE_DOWN: {
                u32 days;
                switch (i) {
                case BUTTON_YEAR_UP:
                    mDate.year++;
                    if (mDate.year > 99) {
                        mDate.year = 0;
                    }
                    ShowYear(mDate.year);
                    days = GetDaysInMonth(mDate.year, mDate.month);
                    if (mDate.day > days) {
                        mDate.day = days;
                        ShowDay(mDate.day);
                    }
                    break;
                case BUTTON_YEAR_DOWN:
                    if (mDate.year != 0) {
                        mDate.year--;
                    } else {
                        mDate.year = 99;
                    }
                    ShowYear(mDate.year);
                    days = GetDaysInMonth(mDate.year, mDate.month);
                    if (mDate.day > days) {
                        mDate.day = days;
                        ShowDay(mDate.day);
                    }
                    break;
                case BUTTON_MONTH_UP:
                    mDate.month++;
                    if (mDate.month > 12) {
                        mDate.month = 1;
                    }
                    ShowMonth(mDate.month);
                    days = GetDaysInMonth(mDate.year, mDate.month);
                    if (mDate.day > days) {
                        mDate.day = days;
                        ShowDay(mDate.day);
                    }
                    break;
                case BUTTON_MONTH_DOWN:
                    if (mDate.month > 1) {
                        mDate.month--;
                    } else {
                        mDate.month = 12;
                    }
                    ShowMonth(mDate.month);
                    days = GetDaysInMonth(mDate.year, mDate.month);
                    if (mDate.day > days) {
                        mDate.day = days;
                        ShowDay(mDate.day);
                    }
                    break;
                case BUTTON_DAY_UP:
                    days = GetDaysInMonth(mDate.year, mDate.month);
                    mDate.day++;
                    if (mDate.day > days) {
                        mDate.day = 1;
                    }
                    ShowDay(mDate.day);
                    break;
                case BUTTON_DAY_DOWN:
                    days = GetDaysInMonth(mDate.year, mDate.month);
                    if (mDate.day <= 1) {
                        mDate.day = days;
                    } else {
                        mDate.day--;
                    }
                    ShowDay(mDate.day);
                    break;
                case BUTTON_HOUR_UP:
                    mTime.hour++;
                    if (mTime.hour >= 24) {
                        mTime.hour = 0;
                    }
                    ShowHour(mTime.hour);
                    break;
                case BUTTON_HOUR_DOWN:
                    if (mTime.hour == 0) {
                        mTime.hour = 23;
                    } else {
                        mTime.hour--;
                    }
                    ShowHour(mTime.hour);
                    break;
                case BUTTON_MINUTE_UP:
                    mTime.minute++;
                    if (mTime.minute >= 60) {
                        mTime.minute = 0;
                    }
                    ShowMinute(mTime.minute);
                    break;
                case BUTTON_MINUTE_DOWN:
                    if (mTime.minute < 1) {
                        mTime.minute = 59;
                    } else {
                        mTime.minute--;
                    }
                    ShowMinute(mTime.minute);
                    break;
                }
                break;
            }
            case BUTTON_MUSIC_UP: {
                u8 volume;
                if (gGameData.mMusicVolume < 3) {
                    volume = gGameData.mMusicVolume + 1;
                    gGameData.mMusicVolume = volume;
                    gSoundManager.StartSeqArc(2, 1, 8);
                    gSoundManager.ApplySeqInitialVolume(0, 0x60);
                } else {
                    volume = 3;
                    gSoundManager.StartSeqArc(2, 8, 7);
                }
                Sprite_SetFrame(&mSprites[SPRITE_MUSIC_VOLUME], volume);
                break;
            }
            case BUTTON_MUSIC_DOWN: {
                u8 volume;
                if (gGameData.mMusicVolume != 0) {
                    volume = gGameData.mMusicVolume - 1;
                    gGameData.mMusicVolume = volume;
                    gSoundManager.StartSeqArc(2, 1, 9);
                    gSoundManager.ApplySeqInitialVolume(0, 0x60);
                } else {
                    volume = 0;
                    gSoundManager.StartSeqArc(2, 8, 8);
                }
                Sprite_SetFrame(&mSprites[SPRITE_MUSIC_VOLUME], volume);
                break;
            }
            case BUTTON_EFFECT_UP: {
                u8 volume;
                if (gGameData.mEffectVolume < 3) {
                    volume = gGameData.mEffectVolume + 1;
                    gGameData.mEffectVolume = volume;
                    gSoundManager.StartSeqArc(2, 1, 8);
                } else {
                    volume = 3;
                    gSoundManager.StartSeqArc(2, 8, 7);
                }
                Sprite_SetFrame(&mSprites[SPRITE_EFFECT_VOLUME], volume);
                break;
            }
            case BUTTON_EFFECT_DOWN: {
                u8 volume;
                if (gGameData.mEffectVolume != 0) {
                    volume = gGameData.mEffectVolume - 1;
                    gGameData.mEffectVolume = volume;
                    gSoundManager.StartSeqArc(2, 1, 9);
                } else {
                    volume = 0;
                    gSoundManager.StartSeqArc(2, 8, 8);
                }
                Sprite_SetFrame(&mSprites[SPRITE_EFFECT_VOLUME], volume);
                break;
            }
            case BUTTON_VOICE_UP: {
                u8 volume;
                if (gGameData.mVoiceVolume < 3) {
                    volume = gGameData.mVoiceVolume + 1;
                    gGameData.mVoiceVolume = volume;
                    gSoundManager.StartSeqArc(2, 1, 8);
                } else {
                    volume = 3;
                    gSoundManager.StartSeqArc(2, 8, 7);
                }
                Sprite_SetFrame(&mSprites[SPRITE_VOICE_VOLUME], volume);
                MI_CpuFill8(mSpeechText, 0, sizeof(mSpeechText));
                mSpeechMessage.mMessage = volume + 0xF1;
                mSpeechMessage.ExpandMessage();
                int length = sizeof(mSpeechText);
                STD_ConvertStringUnicodeToSjis(mSpeechText, &length, (const u16 *)mSpeechMessage.mText, NULL, NULL);
                mTtsPlayer.Speak(mSpeechText, 0);
                break;
            }
            case BUTTON_VOICE_DOWN: {
                u8 volume;
                if (gGameData.mVoiceVolume != 0) {
                    volume = gGameData.mVoiceVolume - 1;
                    gGameData.mVoiceVolume = volume;
                    gSoundManager.StartSeqArc(2, 1, 9);
                } else {
                    volume = 0;
                    gSoundManager.StartSeqArc(2, 8, 8);
                }
                Sprite_SetFrame(&mSprites[SPRITE_VOICE_VOLUME], volume);
                MI_CpuFill8(mSpeechText, 0, sizeof(mSpeechText));
                mSpeechMessage.mMessage = volume + 0xF1;
                mSpeechMessage.ExpandMessage();
                int length = sizeof(mSpeechText);
                STD_ConvertStringUnicodeToSjis(mSpeechText, &length, (const u16 *)mSpeechMessage.mText, NULL, NULL);
                mTtsPlayer.Speak(mSpeechText, 0);
                break;
            }
            }
        }

        for (int j = 0; j < TEXT_BUTTON_COUNT; j++) {
            if (!func_0203133c(&mTextButtons[j])) {
                continue;
            }
            switch (j) {
            case TEXT_BUTTON_SOUND:
                SetState(STATE_SOUND);
                break;
            case TEXT_BUTTON_CLOCK:
                SetState(STATE_CLOCK);
                break;
            case TEXT_BUTTON_ERASE:
                SetState(STATE_ASK_ERASE);
                break;
            case TEXT_BUTTON_SET:
                SetState(STATE_ASK_SAVE_CLOCK);
                break;
            case TEXT_BUTTON_DONE:
                SetState(STATE_ASK_FIRST_CLOCK);
                break;
            case TEXT_BUTTON_STEREO:
                gGameData.mStereo = TRUE;
                gSoundManager.ApplyOutputSetting();
                func_02031e28(&mTextButtons[TEXT_BUTTON_MONO], FALSE);
                gSoundManager.StartSeqIfNotPlaying(0, 0x60);
                break;
            case TEXT_BUTTON_MONO:
                gGameData.mStereo = FALSE;
                gSoundManager.ApplyOutputSetting();
                func_02031e28(&mTextButtons[TEXT_BUTTON_STEREO], FALSE);
                gSoundManager.StartSeqIfNotPlaying(0, 0x60);
                break;
            }
        }

        // Touching the morning or afternoon changes it
        if (mState == STATE_CLOCK && gTouchInput.mTouched) {
            const ScreenPoint *touch = gTouchInput.mUnknown4C ? &gTouchInput.mLastPoint : &gTouchInput.mPoint;
            if (IsInRect(GetAmPmRect(), *touch)) {
                mTime.hour += 12;
                if (mTime.hour >= 24) {
                    mTime.hour -= 24;
                }
                ShowHour(mTime.hour);
            }
        }
        break;
    }
    case STATE_ASK_SAVE_CLOCK:
    case STATE_ASK_ERASE:
    case STATE_ASK_ERASE_AGAIN:
    case STATE_ASK_ERASE_LAST:
    case STATE_FIRST_CLOCK:
    case STATE_ASK_FIRST_CLOCK:
        if (mWindow.mResult != -1) {
            mWindow.mUnknownSetBeforeClose = 0xF;
            func_020349cc(&mWindow, 0x24);
            mWindowResult = mWindow.mResult;
        }
        if (!mWindow.mTransitionDone) {
            break;
        }
        switch (mWindowResult) {
        case 0:
            if (mState == STATE_ASK_FIRST_CLOCK) {
                func_0203c02c(&gSceneManager, 0x69);
            } else {
                SetState(mState + 1);
            }
            break;
        case 1:
            switch (mState) {
            case STATE_ASK_SAVE_CLOCK:
                SetState(STATE_CLOCK);
                break;
            case STATE_ASK_ERASE:
            case STATE_ASK_ERASE_AGAIN:
            case STATE_ASK_ERASE_LAST:
                gSoundManager.MoveSeqVolume(0, 0x7F, 0x3C);
                SetState(STATE_MENU);
                break;
            case STATE_ASK_FIRST_CLOCK:
                SetState(STATE_FIRST_CLOCK_SET);
                break;
            }
            break;
        }
        break;
    case STATE_SAVING_CLOCK:
        if (!func_0202cd38(&gGameData)) {
            mWindow.mUnknownSetBeforeClose = 0xF;
            func_020349cc(&mWindow, 0x24);
        }
        if (mWindow.mTransitionDone) {
            SetState(mState + 1);
        }
        break;
    case STATE_CLOCK_SAVED:
        if (mWindow.mUnknownAF88) {
            gSoundManager.MoveSeqVolume(0, 0, 0x78);
        }
        break;
    case STATE_ERASING:
        if (!func_0202cd38(&gGameData)) {
            mWindow.mUnknownSetBeforeClose = 0xF;
            func_020349cc(&mWindow, 0x24);
        }
        if (mWindow.mTransitionDone) {
            SetState(mState + 1);
        }
        break;
    case STATE_ERASED:
        if (mWindow.mUnknownAF88) {
            gSoundManager.MoveSeqVolume(0, 0, 0x78);
        }
        break;
    }
}

void OptionScene::Draw() {
    func_020345d8(&mWindow);
    for (int i = 0; i < TEXT_COUNT; i++) {
        mTexts[i].Draw();
    }
    for (int j = 0; j < BUTTON_COUNT; j++) {
        func_02031d7c(&mButtons[j]);
    }
    for (int k = 0; k < TEXT_BUTTON_COUNT; k++) {
        func_0203206c(&mTextButtons[k]);
    }
    for (int l = 0; l < SPRITE_COUNT; l++) {
        Sprite_Draw(&mSprites[l]);
    }
}

void OptionScene::Exit() {
    (void)sUnknown0;
    (void)sUnknown1;
    (void)sUnknown2;
    (void)sUnknown3;
    (void)sUnknown4;
    func_02028108(&data_02121834, 1, 0);
    func_020346cc(&mWindow);
    ArchiveManager_Unmount(&gArchiveManager, "N83");
    ArchiveManager_Unmount(&gArchiveManager, "TET");
    ArchiveManager_Unmount(&gArchiveManager, "TCO");
}

// Hides every button, text and sprite
static inline void HideAll(OptionScene *scene) {
    for (int i = 0; i < OptionScene::TEXT_COUNT; i++) {
        scene->mTexts[i].mSprite.mVisible = FALSE;
    }
    for (int j = 0; j < OptionScene::BUTTON_COUNT; j++) {
        scene->mButtons[j].mVisible = FALSE;
    }
    for (int k = 0; k < OptionScene::TEXT_BUTTON_COUNT; k++) {
        scene->mTextButtons[k].mVisible = FALSE;
    }
    for (int l = 0; l < OptionScene::SPRITE_COUNT; l++) {
        scene->mSprites[l].mVisible = FALSE;
    }
}

void OptionScene::SetState(int state) {
    switch (state) {
    case STATE_MENU:
        HideAll(this);
        mButtons[BUTTON_BACK].mVisible = TRUE;
        mTextButtons[TEXT_BUTTON_SOUND].mVisible = TRUE;
        mTextButtons[TEXT_BUTTON_CLOCK].mVisible = TRUE;
        mTextButtons[TEXT_BUTTON_ERASE].mVisible = TRUE;
        data_0211d604.Set(0x1202);
        break;
    case STATE_CLOCK:
    case STATE_FIRST_CLOCK:
        HideAll(this);
        mButtons[BUTTON_BACK].mVisible = TRUE;
        mTextButtons[TEXT_BUTTON_SET].mVisible = TRUE;
        for (int a = BUTTON_YEAR_UP; a <= BUTTON_MINUTE_DOWN; a++) {
            mButtons[a].mVisible = TRUE;
        }
        for (int b = 0; b <= SPRITE_AM_PM; b++) {
            mSprites[b].mVisible = TRUE;
        }
        func_02027d7c(&data_02121834, "N83:/BG_down_tframe.nsc", 0x100, 0, 0, 0, 0, -1);
        data_0211d604.Set(0x1302);
        Clock_GetDate(data_02123440, &mDate);
        Clock_GetTime(data_02123440, &mTime);
        ShowYear(mDate.year);
        ShowMonth(mDate.month);
        ShowDay(mDate.day);
        ShowHour(mTime.hour);
        ShowMinute(mTime.minute);
        if (state == STATE_FIRST_CLOCK) {
            mButtons[BUTTON_BACK].mVisible = FALSE;
            mTextButtons[TEXT_BUTTON_SET].mVisible = FALSE;
            mTextButtons[TEXT_BUTTON_DONE].mVisible = TRUE;
            OpenWindow(WINDOW_SET_CLOCK, mCharOffset);
            func_02034720(&mWindow, 0x12, 1);
        }
        break;
    case STATE_ASK_SAVE_CLOCK:
        OpenWindow(WINDOW_ASK_SAVE_CLOCK, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case STATE_SAVING_CLOCK:
        OpenWindow(WINDOW_SAVING_CLOCK, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        func_02035460(data_02123440, &mDate, &mTime, FALSE);
        func_0202c8f8(&gGameData, 0);
        if (!func_0202cd48(&gGameData)) {
            func_0203c02c(&gSceneManager, 0xC2);
        }
        break;
    case STATE_CLOCK_SAVED:
        OpenWindow(WINDOW_CLOCK_SAVED, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        gSoundManager.StartSeqArc(1, 0x18, 0);
        break;
    case STATE_SOUND:
        HideAll(this);
        mButtons[BUTTON_BACK].mVisible = TRUE;
        for (int c = TEXT_BUTTON_STEREO; c <= TEXT_BUTTON_MONO; c++) {
            mTextButtons[c].mVisible = TRUE;
        }
        for (int d = BUTTON_MUSIC_DOWN; d <= BUTTON_VOICE_UP; d++) {
            mButtons[d].mVisible = TRUE;
        }
        for (int e = SPRITE_MUSIC_VOLUME; e <= SPRITE_VOICE_VOLUME; e++) {
            mSprites[e].mVisible = TRUE;
        }
        for (int f = 0; f <= 3; f++) {
            mTexts[f].mSprite.mVisible = TRUE;
        }
        func_02027d7c(&data_02121834, "N83:/BG_down_sframe.nsc", 0x100, 0, 0, 0, 0, -1);
        data_0211d604.Set(0x1302);
        if (gGameData.mStereo == TRUE) {
            func_02031e28(&mTextButtons[TEXT_BUTTON_STEREO], TRUE);
            func_02031e28(&mTextButtons[TEXT_BUTTON_MONO], FALSE);
        } else {
            func_02031e28(&mTextButtons[TEXT_BUTTON_STEREO], FALSE);
            func_02031e28(&mTextButtons[TEXT_BUTTON_MONO], TRUE);
        }
        Sprite_SetFrame(&mSprites[SPRITE_MUSIC_VOLUME], gGameData.mMusicVolume);
        Sprite_SetFrame(&mSprites[SPRITE_EFFECT_VOLUME], gGameData.mEffectVolume);
        Sprite_SetFrame(&mSprites[SPRITE_VOICE_VOLUME], gGameData.mVoiceVolume);
        break;
    case STATE_ASK_ERASE:
        gSoundManager.MoveSeqVolume(0, 0x40, 0x3C);
        gSoundManager.StartSeqArc(0, 8, 1);
        OpenWindow(WINDOW_ASK_ERASE, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case STATE_ASK_ERASE_AGAIN:
        OpenWindow(WINDOW_ASK_ERASE_AGAIN, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case STATE_ASK_ERASE_LAST:
        OpenWindow(WINDOW_ASK_ERASE_LAST, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case STATE_ERASING:
        OpenWindow(WINDOW_ERASING, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        if (!func_0202cd94(&gGameData)) {
            func_0203c02c(&gSceneManager, 0xC2);
        }
        break;
    case STATE_ERASED:
        OpenWindow(WINDOW_ERASED, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        gSoundManager.StartSeqArc(1, 0x18, 0);
        break;
    case STATE_FIRST_CLOCK_SET:
        break;
    case STATE_ASK_FIRST_CLOCK:
        func_02035668(data_02123440, &mDate, &mTime);
        OpenWindow(WINDOW_ASK_CLOCK, mCharOffset);
        func_02034720(&mWindow, 0x12, 1);
        break;
    }
    mState = state;
    mStep = 0;
}

// Adds the buttons "yes" and "no" to the message window
static inline void AddYesNoButtons(OptionScene *scene, int *charOffset) {
    WindowPoint size = sWindowButtonSize;
    for (int i = 0; i < 2; i++) {
        func_020337b8(&scene->mWindow, i, 1, 10, 0xB, sWindowButtonPositions[i], size, &scene->mMessageFiles[1],
                      sWindowButtonMessages[i], &gFonts[2].mFont, 0x1000, charOffset, &sWindowButtonSounds[i]);
    }
}

// Opens the message window with one of its messages, and its buttons
void OptionScene::OpenWindow(int window, int charOffset) {
    ScreenRect rect = sWindowRect;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, sWindowScreenName, 0x400);
    func_020331bc(&mWindow, 0x3F00, rect);
    int i;
    switch (window) {
    case WINDOW_ASK_SAVE_CLOCK:
    case WINDOW_ASK_ERASE:
    case WINDOW_ASK_ERASE_AGAIN:
    case WINDOW_ASK_ERASE_LAST:
    {
        charOffset = func_02033a58(&mWindow, &mMessageFiles[0], sWindowMessages[window], &gFonts[0].mFont, 0x1000,
                                   sQuestionTextPosition, &charOffset);
        AddYesNoButtons(this, &charOffset);
        break;
    }
    case WINDOW_ASK_CLOCK: {
        int hour = mTime.hour;
        for (int j = 0; j < DATE_TEXT_COUNT; j++) {
            wchar_t *text = mDateTexts[j];
            MI_CpuClear16(text, sizeof(mDateTexts[j]));
            switch (j) {
            case 0:
                func_0203673c(mDate.year + 2000, text, sizeof(mDateTexts[j]), 0, FALSE);
                break;
            case 1:
                func_0203673c(mDate.month, text, sizeof(mDateTexts[j]), 0, FALSE);
                break;
            case 2:
                func_0203673c(mDate.day, text, sizeof(mDateTexts[j]), 0, FALSE);
                break;
            case 3:
                if (hour >= 12) {
                    hour -= 12;
                    wcscpy(text, L"午後");
                } else {
                    wcscpy(text, L"午前");
                }
                break;
            case 4:
                func_0203673c(hour, text, sizeof(mDateTexts[j]), 0, FALSE);
                break;
            case 5:
                func_0203673c(mTime.minute, text, sizeof(mDateTexts[j]), 0, FALSE);
                break;
            }
            func_02034a3c(&mWindow, 0, j, text);
        }
        charOffset = func_02033a58(&mWindow, &mMessageFiles[0], sWindowMessages[window], &gFonts[0].mFont, 0x1000,
                                   sClockTextPosition, &charOffset);
        WindowPoint size = sWindowButtonSize;
        for (i = 0; i < 2; i++) {
            func_020337b8(&mWindow, i, 1, 10, 0xB, sWindowButtonPositions[i], size, &mMessageFiles[1],
                          sWindowButtonMessages[i], &gFonts[2].mFont, 0x1000, &charOffset, &sWindowButtonSounds[i]);
        }
        break;
    }
    case WINDOW_SET_CLOCK: {
        charOffset = func_02033a58(&mWindow, &mMessageFiles[0], sWindowMessages[window], &gFonts[0].mFont, 0x1000,
                                   sSetClockTextPosition, &charOffset);
        func_020337b8(&mWindow, 0, 1, 10, 0xB, sOkButtonPosition, sWindowButtonSize, &mMessageFiles[1], 2,
                      &gFonts[2].mFont, 0x1000, &charOffset, &sWindowButtonSounds[1]);
        break;
    }
    case WINDOW_SAVING_CLOCK:
    case WINDOW_CLOCK_SAVED:
    case WINDOW_ERASING:
    case WINDOW_ERASED:
    {
        charOffset = func_02033a58(&mWindow, &mMessageFiles[0], sWindowMessages[window], &gFonts[0].mFont, 0x1000,
                                   sMessageTextPosition, &charOffset);
        break;
    }
    }
    func_02034f4c(&mWindow, 1);
    func_02034f94(&mWindow, 1);
}

void OptionScene::ShowYear(u32 year) {
    int thousands = GetDigit(year + 2000, 1000);
    int hundreds = GetDigit(year + 2000, 100);
    int tens = GetDigit(year + 2000, 10);
    Sprite_SetFrame(&mSprites[SPRITE_YEAR], thousands);
    Sprite_SetFrame(&mSprites[SPRITE_YEAR + 1], hundreds);
    Sprite_SetFrame(&mSprites[SPRITE_YEAR + 2], tens);
    Sprite_SetFrame(&mSprites[SPRITE_YEAR + 3], GetDigit(year + 2000, 1));
    BOOL visible[4];
    int i;
    for (i = 0; i < 4; i++) {
        visible[i] = TRUE;
    }
    if ((year + 2000) / 10000 == 0 && thousands == 0) {
        visible[0] = FALSE;
        if (hundreds == 0) {
            visible[1] = FALSE;
            if (tens == 0) {
                visible[2] = FALSE;
            }
        }
    }
    for (i = 0; i < 4; i++) {
        mSprites[SPRITE_YEAR + i].mVisible = visible[i];
    }
}

void OptionScene::ShowMonth(u32 month) {
    int tens = GetDigit(month, 10);
    int ones = GetDigit(month, 1);
    Sprite_SetFrame(&mSprites[SPRITE_MONTH], tens);
    Sprite_SetFrame(&mSprites[SPRITE_MONTH + 1], ones);
    BOOL visible[2];
    int i;
    for (i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (month / 100 == 0 && tens == 0) {
        visible[0] = FALSE;
        if (ones == 0) {
            visible[1] = FALSE;
        }
    }
    for (i = 0; i < 2; i++) {
        mSprites[SPRITE_MONTH + i].mVisible = visible[i];
    }
}

void OptionScene::ShowDay(u32 day) {
    int tens = GetDigit(day, 10);
    int ones = GetDigit(day, 1);
    Sprite_SetFrame(&mSprites[SPRITE_DAY], tens);
    Sprite_SetFrame(&mSprites[SPRITE_DAY + 1], ones);
    BOOL visible[2];
    int i;
    for (i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (day / 100 == 0 && tens == 0) {
        visible[0] = FALSE;
        if (ones == 0) {
            visible[1] = FALSE;
        }
    }
    for (i = 0; i < 2; i++) {
        mSprites[SPRITE_DAY + i].mVisible = visible[i];
    }
}

void OptionScene::ShowHour(u32 hour) {
    if (hour >= 12) {
        hour -= 12;
        Sprite_SetFrame(&mSprites[SPRITE_AM_PM], 1);
    } else {
        Sprite_SetFrame(&mSprites[SPRITE_AM_PM], 0);
    }
    int tens = GetDigit(hour, 10);
    Sprite_SetFrame(&mSprites[SPRITE_HOUR], tens);
    Sprite_SetFrame(&mSprites[SPRITE_HOUR + 1], GetDigit(hour, 1));
    BOOL visible[2];
    int i;
    for (i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (hour / 100 == 0 && tens == 0) {
        visible[0] = FALSE;
    }
    for (i = 0; i < 2; i++) {
        mSprites[SPRITE_HOUR + i].mVisible = visible[i];
    }
}

void OptionScene::ShowMinute(u32 minute) {
    int tens = GetDigit(minute, 10);
    Sprite_SetFrame(&mSprites[SPRITE_MINUTE], tens);
    Sprite_SetFrame(&mSprites[SPRITE_MINUTE + 1], GetDigit(minute, 1));
    BOOL visible[2];
    int i;
    for (i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (minute / 100 == 0 && tens == 0) {
        visible[0] = FALSE;
    }
    for (i = 0; i < 2; i++) {
        mSprites[SPRITE_MINUTE + i].mVisible = visible[i];
    }
}

// The year is 0 to 99, of the 2000s
u32 OptionScene::GetDaysInMonth(u32 year, u32 month) {
    switch (month) {
    case 1:
        return 31;
    case 2:
        if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
            return 29;
        }
        return 28;
    case 3:
        return 31;
    case 4:
        return 30;
    case 5:
        return 31;
    case 6:
        return 30;
    case 7:
        return 31;
    case 8:
        return 31;
    case 9:
        return 30;
    case 10:
        return 31;
    case 11:
        return 30;
    case 12:
        return 31;
    }
    return 0;
}

OptionScene::~OptionScene() {}
