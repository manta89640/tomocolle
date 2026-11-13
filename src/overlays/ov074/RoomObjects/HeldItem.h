#ifndef HELDITEM_H
#define HELDITEM_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"

// The model of a thing a character of the apartment room holds in a hand ("Mdl/Goods/gdNN_3D_LZ.bin", NN being
// the kind, 00 to 10). It can change its kind, which loads another archive in the background, and it is placed
// each frame on a joint of the character's model.
//
// The name is group a2's; it is descriptive, not the original: no name of this class survives in the ROM.
class HeldItem : public AnimModel {
public:
    enum ShowState {
        SHOW_IDLE,
        SHOW_HIDE,
        SHOW_SHOW,
    };

    enum LoadState {
        LOAD_IDLE,
        LOAD_UNLOAD,
        LOAD_LOAD_ARCHIVE, // Mount the archive of mKind
        LOAD_RELOAD,       // Make the model from the mounted archive
        LOAD_CHANGE,       // All three, one after the other
    };

    HeldItem();
    virtual void Draw();

    void SetArchive(const char *name, int kind);
    void Init(int resourceId);
    void SetShowState(int state);
    void Update();
    void UpdateShow();
    void SetJoint(const MtxFx43 *joint, const MtxFx33 *rotation);
    void SetJointOffset(const MtxFx43 *joint, const MtxFx33 *rotation, fx32 x, fx32 y, fx32 z);
    void SetLoadState(int state);
    void SetKind(int kind);
    void UpdateLoad();
    void Unload();
    void Reload();
    void StartPattern();

    void SetPosition(VecFx32 position) { mPosition = position; }
    void SetPosition(fx32 x, fx32 y, fx32 z) {
        mPosition.x = x;
        mPosition.y = y;
        mPosition.z = z;
    }

    /* 0x0E0 */ int mResourceId;
    /* 0x0E4 */ int mShowState;
    /* 0x0E8 */ int mLoadState;
    /* 0x0EC */ s16 mShowStep;
    /* 0x0EE */ s16 mLoadStep;
    /* 0x0F0 */ char mArchiveName[4];    // What the archive is mounted as ("HI0" to "HI3")
    /* 0x0F4 */ char mArchivePath[0x1C]; // "Mdl/Goods/gd00_3D_LZ.bin"
    /* 0x110 */ u8 mKind;
}; // 0x114

#endif // HELDITEM_H
