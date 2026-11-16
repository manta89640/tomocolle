#ifndef SCENE_MAPMODELSSCENE_H
#define SCENE_MAPMODELSSCENE_H

#include <nitro.h>

#include "Model.h"
#include "Scene.h"

// Scene 0x27, of the main program (functions 0x02051640 to 0x020519b4, virtual table 0x02111C00; not decompiled):
// it mounts three models of the island map, "Mdl/Misc/map_sky_3D_LZ.bin" (M00), "map_vote" (M01) and "map_apart4"
// (M02), and plays an animation of each ("M00:/sky_wait00.ita", "M01:/vote_wait00.itp", "M02:/apart_wait00.iva").
// The scene factory allocates 0x2B0 bytes and loads no overlay; the constructor is in src/scene/SceneManager.cpp.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class MapModelsScene : public Scene {
public:
    MapModelsScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x010 */ AnimModel mSky;
    /* 0x0F0 */ AnimModel mVote;
    /* 0x1D0 */ AnimModel mApart;
}; // 0x2B0

#endif // SCENE_MAPMODELSSCENE_H
