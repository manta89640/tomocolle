#ifndef OV075_SCENE40CAMERA_H
#define OV075_SCENE40CAMERA_H

#include <nitro.h>

#include "character/FloorPos.h"

struct Camera;
class Scene40Character;

// The scene's camera (scene+0x69DEC): it follows the character, 14 away and at the height 0x41D0, as overlay 96's
// WeddingCamera follows the couple.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40Camera {
public:
    void Init(Scene40Character *character);
    void Reset(BOOL restore);
    void Update();
    void SetState(int state);
    void PlaceAround(VecFx32 *position, VecFx32 *target, VecFx32 *result);
    void ViewFromFront();
    void SetPosition(VecFx32 position);
    void SetTarget(VecFx32 target);
    VecFx32 Approach(VecFx32 *goal, VecFx32 *current, int shift);
    FloorPos GetEyeFloorPos();

    /* 0x00 */ int mState; // 0 still, 1 following the character
    /* 0x04 */ Camera *mCamera;
    /* 0x08 */ Scene40Character *mCharacter;
    /* 0x0C */ VecFx32 mPosition;
    /* 0x18 */ VecFx32 mTarget;
}; // 0x24

#endif // OV075_SCENE40CAMERA_H
