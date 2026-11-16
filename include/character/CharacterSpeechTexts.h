#ifndef CHARACTER_CHARACTERSPEECHTEXTS_H
#define CHARACTER_CHARACTERSPEECHTEXTS_H

#include <nitro.h>

class SavedCharacter;

// The texts of a character that the speech synthesizer speaks: the names and the character's own phrase, as they
// are and turned into the synthesizer's notation (func_020372d0), and the readings again in Shift JIS.
// MakeCharacterSpeechTexts (src/scene/SceneHelpers.cpp) fills them from a saved character.
//
// The name is descriptive, not the original: no name of this structure survives in the ROM.
struct CharacterSpeechTexts {
    /* 0x000 */ char mNameReadingSjis[0x52];
    /* 0x052 */ wchar_t mName[11];               // The Mii's name (MiiData_GetName)
    /* 0x068 */ wchar_t mNameReading[41];        // The same, in the synthesizer's notation
    /* 0x0BA */ wchar_t mFamilyNameReading[41];  // CharacterRecord::mFamilyName, in the synthesizer's notation
    /* 0x10C */ wchar_t mGivenNameReading[41];   // CharacterRecord::mName, the same
    /* 0x15E */ char mPhraseReadingSjis[0x82];
    /* 0x1E0 */ wchar_t mPhrase[17];             // CharacterRecord::mMotto, or "ミニュウリョク" when empty
    /* 0x202 */ wchar_t mPhraseReading[65];
    /* 0x284 */ u8 mUnknown284[0xC];
    /* 0x290 */ BOOL mHasPhrase;                 // Whether the character has a phrase of its own
}; // 0x294

void MakeCharacterSpeechTexts(CharacterSpeechTexts *texts, SavedCharacter *character);

#endif // CHARACTER_CHARACTERSPEECHTEXTS_H
