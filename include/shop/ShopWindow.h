#ifndef SHOP_SHOPWINDOW_H
#define SHOP_SHOPWINDOW_H

#include <nitro.h>

#include "MessageWindow.h"
#include "SpriteButton.h"
#include "item/Item.h"
#include "text/TextCanvas.h"

// The shop scenes' price window (main program 0x020AA464 to 0x020AB09C, not decompiled; overlays 56, 68, 83, 84 and
// 85 use it through 0x020aa800, its set-up, 0x020aa9c0, its update, 0x020aaba4, its drawing, and 0x020aac04, its
// opening): a message window with two buttons and three text canvases more. A STAND-IN with what the scenes'
// constructors show: its constructor is inline (the window's, then its table, then its members). Its table is at
// 0x02115368.
//
// The names are descriptive (gap66-c's, whose range has the class's functions): no name of these classes survives
// in the ROM.
class ShopWindow : public MessageWindow {
public:
    // Calls the window's, and sets the two buttons' and the three canvases' visibility too (0x020aaffc)
    virtual void SetVisible(BOOL visible);
    virtual void func_02034bc4(); // 0x020aaea8: draws a text; what it takes is not known
    virtual int GetBuyTotal(Item *item, int count);  // 0x020aab84: the item's first virtual function times count
    virtual int GetSellTotal(Item *item, int count); // 0x020aad10: its second times count

    /* 0xAFA4 */ u8 mUnknownAFA4[0x10];
    /* 0xAFB4 */ SpriteButton mButtonAFB4;
    /* 0xB0A8 */ SpriteButton mButtonB0A8;
    /* 0xB19C */ ObjTextCanvas mTextCanvases[3];
    /* 0xB6C4 */ u8 mUnknownB6C4[4];
}; // 0xB6C8

// The price window that buys at half the price and sells at 11/10 of it (table 0x02115380).
class HalfPriceShopWindow : public ShopWindow {
public:
    virtual int GetBuyTotal(Item *item, int count);  // 0x020ab038: half the first virtual function, times count
    virtual int GetSellTotal(Item *item, int count); // 0x020ab060: 11/10 of the second, times count
}; // 0xB6C8

#endif // SHOP_SHOPWINDOW_H
