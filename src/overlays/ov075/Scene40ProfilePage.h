#ifndef OV075_SCENE40PROFILEPAGE_H
#define OV075_SCENE40PROFILEPAGE_H

#include <nitro.h>

#include "Sprite.h"
#include "character/Character.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

#include "Scene40Meter.h"

struct Scene40Context;

// A character the visited one has a relation with, as the relations screen lists them
struct Scene40Relation {
    /* 0x00 */ int mSortKey;     // By the rank's place in sRelationRankKeys, then by the meter (GetRelationSortKey)
    /* 0x04 */ wchar_t mName[11];
    /* 0x1A */ u8 mIndex;        // The other character's mIndex
}; // 0x1C

// The visited character's profile (pages + 0x2A49C, scene 0x5AFEC; functions 0x0215f180 to 0x0216146c): its
// name, the satisfaction or trouble meter and the fullness meter, the foods and goods it likes and has had, its
// partner and best friend, and on another screen its relations. L and R switch the screens.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40ProfilePage {
public:
    // mMeterKind: which of the two meters the profile shows
    enum MeterKind {
        METER_SATISFACTION,
        METER_TROUBLE,
    };

    enum Screen {
        SCREEN_PROFILE,
        SCREEN_REDRAW,       // The profile, its texts drawn again in the next frame
        SCREEN_RELATIONS,
        SCREEN_NO_RELATIONS, // The relations screen without any: a message
    };

    void Init(Scene40Context *context, int *charOffset);
    void Update();
    void Draw();
    void ChooseMeterKind();
    void ShowMeterLabel();
    void Refresh();
    void ShowMeters();
    void MoveMeters(BOOL beyondTop);
    void ShowFullness();
    int GetTroubleMeterValue();
    void ShowFavoriteFoods();
    void ShowFavoriteItem();
    void ShowDislikedItem();
    void ShowPartner();
    void ShowBestFriend();
    void ShowHadItemMarks();
    void RedrawTexts();
    void ShowTitle();
    void ShowRelationsTitle();
    void ShowProfile();
    void SetSwitchable(BOOL switchable);
    void ShowProfileScreen();
    void ShowRelationsScreen();
    void UpdateRedraw();
    void UpdateRelations();
    void UpdateNoRelations();
    void SwitchScreen();
    void FindRelations();
    u32 GetRelationMessage(int group, int index);
    static int GetRelationSortKey(int rank, int meter);
    static int GetSortKeyGroup(int key);
    static int GetSortKeyMeter(int key);
    static int GetMeterGrade(int meter);

    /* 0x0000 */ int mScreen;
    /* 0x0004 */ int mScreenIndex;  // Which of the mScreenCount screens L and R show: 0 the profile
    /* 0x0008 */ int mScreenCount;
    /* 0x000C */ BOOL mHasRelations;
    /* 0x0010 */ BOOL mSwitchable;    // L and R switch the screens
    /* 0x0014 */ Scene40Context *mContext;
    /* 0x0018 */ SavedCharacter *mCharacter; // The visited character
    /* 0x001C */ int mMeterKind;
    /* 0x0020 */ Sprite mSprite0020;
    /* 0x00CC */ Sprite mSprite00CC;
    /* 0x0178 */ MessageText mTitle;         // Message 17 or 18 with the character's name
    /* 0x0768 */ Sprite mCompleteMark;     // Shown when the profile is complete: names, creator, birthday, blood type
    /* 0x0814 */ MessageText mFavoriteItem; // The item it likes most, once it has had it
    /* 0x0E04 */ MessageText mFavoriteFoods[3];
    /* 0x1FD4 */ MessageText mDislikedItem;
    /* 0x25C4 */ Sprite mPartnerSexIcon;     // Sequence 22 or 23 by the partner's sex
    /* 0x2670 */ ObjTextBox mPartnerName;
    /* 0x282C */ ObjTextBox mBestFriendName;
    /* 0x29E8 */ Sprite mMeterLabel;         // Sequence 24 or 25 by mMeterKind
    /* 0x2A94 */ Scene40LevelMeter mSatisfactionMeter;
    /* 0x3364 */ Scene40SegmentMeter mTroubleMeter;
    /* 0x3A58 */ Scene40SpriteMeter mFullnessMeter;
    /* 0x3B3C */ Sprite mHadItemMarks[8];  // One for each of sHadItemMarkItems that it has had
    /* 0x409C */ MessageText mGroupTitles[9];     // The relations screen's group titles (five used)
    /* 0x760C */ ObjTextBox mRelationNames[11];  // and its lines: a name
    /* 0x8920 */ MessageText mText8920;     // The relations screen's title
    /* 0x8F10 */ MessageText mRelationTexts[11];  // and what they are to each other
    /* 0xD060 */ MessageText mTextD060;     // Message 19 of the relations screen
    /* 0xD650 */ BOOL mRelationsChanged;      // ShowProfile finds the relations again while it is set (the talk page sets it)
    /* 0xD654 */ Scene40Relation mRelations[100];
    /* 0xE144 */ int mRelationCount;
    /* 0xE148 */ int mFirstGroup;     // The relations are grouped by rank (GetSortKeyGroup of their sort keys)
    /* 0xE14C */ int mPage;           // The relations' page shown
    /* 0xE150 */ int mLastGroup;
    /* 0xE154 */ int mGroups[9];      // The groups there are, in order
    /* 0xE178 */ int mGroupCounts[9]; // By group
    /* 0xE19C */ int mGroupStarts[9]; // By group: its first relation
    /* 0xE1C0 */ int mPageStarts[20]; // The first relation of each page: a page holds 180 pixels of lines, 14 a
                                      // relation and 28 for one that ends a group
    /* 0xE210 */ BOOL mShowUnknown;          // Show "unknown" (message 15 or 12) where there is nothing to show
}; // 0xE214

#endif // OV075_SCENE40PROFILEPAGE_H
