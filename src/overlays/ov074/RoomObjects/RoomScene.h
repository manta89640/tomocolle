#ifndef ROOMSCENE_H
#define ROOMSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "text/MessageFile.h"
#include "voice/TtsPlayer.h"

#include "HeldItem.h"
#include "Room.h"
#include "RoomCamera.h"
#include "RoomCharacter.h"

// The apartment scene, as far as its destructor needs it: the unit RoomScene has the whole class
// (src/overlays/ov074/RoomScene/RoomScene.h), and this is the same layout with only the members that have
// destructors given their types. The names are that header's.

// A scene of the game (main program): see the unit RoomScene's header for what its runner does with it.
class Scene {
public:
    virtual void StartLoading() = 0;
    virtual void Init() = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Exit() = 0;
    virtual ~Scene() {}

    /* 0x04 */ int mSceneId;
    /* 0x08 */ int mPhase;
    /* 0x0C */ BOOL mSkipFade;
};

class RoomScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
    // In the game this destructor is implicit (it is emitted after every other function of the overlay, where the
    // implicit HeldItem::~HeldItem also is). It is declared here only so that this unit, which does not have the
    // class's first virtual function, can hold it.
    virtual ~RoomScene();

    /* 0x00010 */ int mInputState;
    /* 0x00014 */ int mEventState;
    /* 0x00018 */ int mFadeState;
    /* 0x0001C */ int mEventTimer;
    /* 0x00020 */ RoomCharacter mCharacters[2];
    /* 0x05B50 */ RoomCharacter *mActor;
    /* 0x05B54 */ RoomCharacter *mActorOther;
    /* 0x05B58 */ HeldItem mItems[4];
    /* 0x05FA8 */ ApartmentRoom mRoom;
    /* 0x068C8 */ RoomCamera mCamera;
    /* 0x0692C */ u8 mMembersWithoutDestructor0692C[0x87D0 - 0x692C];
    /* 0x087D0 */ MessageFile mEtcMessages;
    /* 0x087E8 */ MessageFile mRoomMessages;
    /* 0x08800 */ u8 mMembersWithoutDestructor08800[0x8DFC - 0x8800];
    /* 0x08DFC */ TtsPlayer mTtsPlayer;
    /* 0x1D740 */ u8 mMembersWithoutDestructor1D740[0x1D7C8 - 0x1D740];
}; // 0x1D7C8

#endif // ROOMSCENE_H
