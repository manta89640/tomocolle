#ifndef OV002_MAINPROGRAM_H
#define OV002_MAINPROGRAM_H

#include "../../scene/MainProgram3d.h"
#include "FaceTestScene.h"

// What overlay 2's code uses of the main program, which only the overlay's source includes (the scene's class, in
// FaceTestScene.h, is also included by the main program's scene constructors, src/scene/SceneManager.cpp).

// --- What the scene uses of the main program (not decompiled) ---------------------------------------------------

extern "C" {
// Moves the camera, its position and its target alike
void Camera_Translate(Camera *camera, fx32 x, fx32 y, fx32 z);
// Points a light along -z, white, and turns it on
void func_0202a96c(Light *light);
// Sets the frame of a model's animation (found as AnimModel_FindAnimation finds it)
void AnimModel_SetAnimationFrame(AnimModel *model, fx32 frame, u32 resType, int id);
// Sets the line that CharacterBase::HitTestFace() tests against: from the camera's eye (data_02122764 + 0x20)
// through the touched point (data_021223e4 + 0x428), its direction at data_02122818 (data_021223e4 + 0x434).
// Returns NNS_G3dScrPosToWorldLine's result.
int ModelResources_SetTouchLine(void *resources, int x, int y);
}

#endif // OV002_MAINPROGRAM_H
