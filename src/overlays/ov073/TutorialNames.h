#ifndef OV073_TUTORIALNAMES_H
#define OV073_TUTORIALNAMES_H

// Overlay 73 (the tutorial's apartment) has its own versions of the classes that overlay 74 (the apartment) has:
// most of their functions are byte for byte the same, but several of the classes are smaller here (the character
// has 18 states and no held items, the camera no close-up mode, the buttons no kinds). The two overlays are never
// in memory together, but the program is linked as one, so the two versions cannot have one name. The sources are
// written with the names the classes share, and this header, included first, gives this overlay's versions their
// own.
#define RoomCharacter TutorialRoomCharacter
#define ApartmentRoom TutorialApartmentRoom
#define RoomCamera TutorialRoomCamera
#define RoomLight TutorialRoomLight
#define RoomCursor TutorialRoomCursor
#define BuildingButton TutorialBuildingButton
#define ApartmentFront TutorialApartmentFront
#define SkyBackground TutorialSkyBackground
#define NamePanel TutorialNamePanel
#define TouchSprite TutorialTouchSprite
#define TouchButton TutorialTouchButton
#define ClockDisplay TutorialClockDisplay
#define Clock TutorialClock
#define TimedMessage TutorialTimedMessage
#define HeldItem TutorialHeldItem

#endif // OV073_TUTORIALNAMES_H
