#ifndef TEXT_TEXTOBJRESOURCE_H
#define TEXT_TEXTOBJRESOURCE_H

#include <nitro.h>

// The sprite resources that text drawn on sprites uses: main() loads "MOJ:/Dummy.nce" and "MOJ:/Dummy.ncg" with
// it, once per screen, under the resource numbers (0x47, 0x46) that ObjTextCanvas then makes its sprites from.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class TextObjResource {
public:
    void Load(const char *cellPath, const char *charPath, int screen, int resource, int unk);

    // What the last Load was given
    /* 0x0 */ int mScreen;
    /* 0x4 */ int mResource;
    /* 0x8 */ int mUnknown; // Always 0; passed on to the resource loader
}; // 0xC

extern TextObjResource gTextObjResource;

#endif // TEXT_TEXTOBJRESOURCE_H
