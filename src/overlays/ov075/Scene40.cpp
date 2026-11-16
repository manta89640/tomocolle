#include "Scene40.h"

extern "C" {
void func_02080f00(ParticleSystem *particles, int unknown); // Draws the particles

// Not decompiled yet
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
void func_ov075_02161ce0(Scene40Character *character, int a); // Updates the character
void func_ov075_02161d6c(Scene40Character *character);        // Draws it
}

// Overlay 75: scenes 0x40 and 0x6C, the scene of touching a Mii ("Scn/Scene/SceneMiiTouch_LZ.bin"). One source
// file by its one static initialiser. Most of it is not decompiled yet.

// --- The scene -----------------------------------------------------------------------------------------------------

// For the first nine frames the room and the character are updated from here as well
void Scene40::Draw() {
    func_020345d8(&mWindows[0]);
    func_020345d8(&mWindows[1]);
    func_ov075_0214fc00(mUnknown30B50);
    Sprite_Draw(&mSprite013D0);

    switch (mDrawState) {
    case 0:
        mRoom.Update();
        func_ov075_02161ce0(&mCharacter, 0);
        mDrawFrames++;
        if (mDrawFrames >= 9) {
            mDrawState = 1;
        }
        break;
    case 1:
        break;
    }

    func_02080f00(&mParticles, 0);
    func_ov075_02161d6c(&mCharacter);
    mFace.Draw();
    mMannequin.Draw();
    mRoom.RoomModels::Draw();
    for (int i = 0; i < 3; i++) {
        mItemModels[i].Draw();
    }
    mSpeechWindow.Draw();
}

// --- The scene's list: the shops' item list with a mark beside each row ------------------------------------------

void Scene40ListView::Draw() {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Draw(&mMarks[i]);
    }
    ItemListView::Draw();
}

void Scene40ListView::SetVisible(BOOL visible) {
    ItemListView::SetVisible(visible);
    for (int i = 0; i < mRowCount; i++) {
        mMarks[i].mVisible = visible;
    }
}

// A row's mark shows that the character has had the row's item: a bit of its record for each
void Scene40ListView::DrawRow(int row, int item) {
    ItemListView::DrawRow(row, item);
    BOOL visible = TRUE;
    switch (mMarkKind) {
    case MARK_NONE:
        visible = FALSE;
        break;
    case MARK_ITEMS:
        visible = mCharacter->mUnknown1C8.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_CLOTHES:
        visible = mCharacter->mOwnedClothes.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_INTERIORS:
        visible = mCharacter->mOwnedInteriors[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    }
    mMarks[row].mVisible = visible;
}

void Scene40ListView::ClearRow(int row) {
    ItemListView::ClearRow(row);
    mMarks[row].mVisible = FALSE;
}

// --- The object at 0x30A8C of the scene --------------------------------------------------------------------------

Scene40Unknown30A8C::Scene40Unknown30A8C() {
    MI_CpuClear32(mUnknown00, sizeof(mUnknown00));
    MI_CpuClear32(mUnknown10, sizeof(mUnknown10));
    mUnknown1C = 0;
    mUnknown20 = 0;
}
