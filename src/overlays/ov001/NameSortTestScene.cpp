#include "NameSortTestScene.h"

#include <wchar.h>

// Overlay 1: scene 0x0C, a test of sorting characters by name. A turns a test string to upper case, Y gives the
// eight characters random names and family names and sorts them by name, B gives them random Mii names and sorts
// them on a thread. Everything it would show goes to two print functions that are empty in this build. One source
// file: the scene and its virtual table, with the scene's implicit destructor at the end of .text, and the test
// string after the table in .data.

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The pad: what was pressed this frame
struct PadInput {
    /* 0x00 */ u32 mTrigger;
    /* 0x04 */ u32 mHeld;
};

extern "C" {
extern PadInput data_0211f9d4;
extern MATHRandContext16 gRandom;

// lib/MiiData: the Mii's name, at the start of the character's record
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void MiiData_SetName(CharacterRecord *character, const wchar_t *name);
}

// --- The scene ----------------------------------------------------------------------------------------------------

static void SortThread(void *arg);

void NameSortTestScene::StartLoading() {
}

void NameSortTestScene::Init() {
    mStep = STEP_IDLE;
    mThread.state = OS_THREAD_STATE_TERMINATED; // So that Exit has nothing to destroy before B
    wcscpy(mText, L"かー～ヴヶa　"); // What A turns to upper case

    wchar_t name[11];
    MI_CpuClear16(name, sizeof(name));
    for (int i = 0; i < 8; i++) {
        MiiData_SetName(&mCharacters[i], name);
    }
    // Unused: what is left of it is the CLZ of the SDK's inline assembly, which the compiler keeps
    MATH_QSortStackSize(8);
}

void NameSortTestScene::Update() {
    switch (mStep) {
    case STEP_IDLE:
        if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
            CharacterSort::ToUpperCase(mText);
            PrintText(mText, FALSE);
        } else if (data_0211f9d4.mTrigger & PAD_BUTTON_Y) {
            // Random names and family names, sorted by name
            SavedCharacter *character = mCharacters;
            for (int i = 0; i < 8; i++) {
                wchar_t name[11];
                wchar_t familyName[11];
                MI_CpuClear16(name, sizeof(name));
                MI_CpuClear16(familyName, sizeof(familyName));
                for (int j = 0; j < 10; j++) {
                    name[j] = MATH_Rand16(&gRandom, 0) & 0xFF;
                    familyName[j] = MATH_Rand16(&gRandom, 0) & 0xFF;
                }
                name[0] = L'あ';
                if (i & 1) {
                    name[1] = 0;
                } else {
                    name[1] = L'!';
                }
                wcscpy(character->mName, name);
                PrintName(0, name, 10);
                wcscpy(character->mFamilyName, familyName);
                PrintName(1, familyName, 10);
                character++;
            }
            MATH_QSort(mCharacters, 8, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByName,
                       mSortStack);
            character = mCharacters;
            for (int i = 0; i < 8; i++, character++) {
                PrintName(0, character->mName, 10);
                PrintName(1, character->mFamilyName, 10);
            }
        } else if (data_0211f9d4.mTrigger & PAD_BUTTON_B) {
            // Random Mii names, sorted on the thread
            for (int i = 0; i < 8; i++) {
                wchar_t name[11];
                MI_CpuClear16(name, sizeof(name));
                for (int j = 0; j < 10; j++) {
                    name[j] = MATH_Rand16(&gRandom, 0) & 0xFF;
                }
                name[0] = L'あ';
                if (i & 1) {
                    name[1] = i;
                    name[3] = 0;
                    name[2] = MATH_Rand16(&gRandom, 0) & 0xFF;
                } else {
                    name[1] = L'!';
                }
                MiiData_SetName(&mCharacters[i], name);
                mCharacters[i].mIndex = i;
                MiiData_GetName(&mCharacters[i], name);
                PrintName(1, name, 10);
            }
            mStep = STEP_SORTING;
            OS_CreateThread(&mThread, SortThread, this, &mThread, sizeof(mThreadStack), 26);
            OS_WakeupThreadDirect(&mThread);
        }
        break;
    case STEP_SORTING:
        if (OS_IsThreadTerminated(&mThread)) {
            mStep = STEP_IDLE;
            for (int i = 0; i < 8; i++) {
                wchar_t name[11];
                MiiData_GetName(&mCharacters[i], name);
                PrintName(1, name, 10);
            }
        }
        break;
    }
}

void NameSortTestScene::Draw() {
}

void NameSortTestScene::Exit() {
    if (!OS_IsThreadTerminated(&mThread)) {
        OS_DestroyThread(&mThread);
    }
}

// The thread that B starts: sorts the characters by their Mii's names
static void SortThread(void *arg) {
    NameSortTestScene *scene = (NameSortTestScene *)arg;
    MATH_QSort(scene->mCharacters, 8, sizeof(SavedCharacter), (MATHCompareFunc)CharacterSort::CompareByMiiName, NULL);
}

// Empty in this build, but for the length of the text when the flag is clear: what is left of a debug print
void NameSortTestScene::PrintText(const wchar_t *text, BOOL unknown) {
    if (!unknown) {
        wcslen(text);
    }
}

// Empty in this build: the callers pass a line (0 for names, 1 for family and Mii names), a name and its length, as
// to a debug print
void NameSortTestScene::PrintName(int line, const wchar_t *name, int length) {
}
