// The scene manager (gSceneManager) and the scene factory, CreateScene, which constructs each scene by its id.
//
// The constructors of the scenes are in this file, after CreateScene, in the order of the scene ids, each followed
// by the implicit destructors of its members that it needs: this file holds every scene class's constructor,
// including those of the scenes whose other functions are in an overlay. The classes are declared in the headers
// beside the code of each scene (src/overlays/ovNNN/, src/scene/), against the main program's classes in include/.

#include <nitro.h>

// ---- The scene manager and CreateScene (gap38-a) ----------------------------------------------------------------

// ---- Constructors 0x0203de78..0x02040578: scenes up to 0x26 (gap38-a) --------------------------------------------

// ---- Constructors 0x02040578..0x0204830c (gap38-b) ---------------------------------------------------------------

// ---- Constructors and the rest, 0x0204830c..0x0204af20 (gap38-c) -------------------------------------------------
