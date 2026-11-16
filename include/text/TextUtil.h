#ifndef TEXT_TEXTUTIL_H
#define TEXT_TEXTUTIL_H

#include <nitro.h>

// Helpers for the game's wide strings (src/text/TextUtil.cpp): numbers written in full-width digits, and text made
// ready for the speech engine (letters and digits spelt out as it reads them, symbols replaced, hiragana turned to
// katakana). The names are descriptive, not the original.

// Writes a number in full-width digits into a buffer of "size" bytes, right-aligned to "width" characters with
// spaces, or with zeros (and the sign in front of them) when "zeroPad" is set.
wchar_t *FormatNumber(int value, wchar_t *text, u32 size, int width, BOOL zeroPad);

// The same with a comma between each three digits
wchar_t *FormatNumberWithCommas(int value, wchar_t *text, u32 size, int width, BOOL zeroPad);
// The same with a full-width point after the first "pointPosition" digits from the right
wchar_t *FormatDecimal(int value, wchar_t *text, u32 size, int width, BOOL zeroPad, int pointPosition);

// Copies a string, and turns its hiragana into katakana
void CopyToKatakana(wchar_t *dst, const wchar_t *src);
void ToKatakana(wchar_t *text);
// Replaces the symbols the speech engine does not read: '、' and U+2022 become '＿' (a pause), '。' and '…' '.',
// U+FF5E 'ー', the stars and U+E017 '!'
void ReplaceSymbolsForSpeech(wchar_t *text);
// How the speech engine is to read a letter (half or full width, either case) or a digit, NULL for another character
const wchar_t *GetLetterReading(wchar_t c);
const wchar_t *GetDigitReading(wchar_t c);
// Turns a text (a name, mostly) into what the speech engine is to read: katakana, letters and digits spelt out,
// spaces dropped and any other character read as 'ン'; at most 126 characters, written into "size" bytes
void MakeSpeechText(wchar_t *dst, const wchar_t *src, u32 size);
void MakeSpeechTextInPlace(wchar_t *text, u32 size);
// Turns the digits '0' to '9' into full-width ones
void ToFullWidthDigits(wchar_t *text);
BOOL IsFullWidthAlphanumeric(wchar_t c);
// A full-width letter or digit as its half-width one
wchar_t ToHalfWidth(wchar_t c);

// How many syllables a katakana text has
int CountSyllables(const wchar_t *text);
// Removes the pauses and the punctuation that ReplaceSymbolsForSpeech leaves ('＿', '_', '.', '?', '？', '!', '！')
void RemoveSpeechSymbols(wchar_t *text);

#endif // TEXT_TEXTUTIL_H
