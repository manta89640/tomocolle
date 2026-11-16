#ifndef SHOP_SHOPWINDOW_H
#define SHOP_SHOPWINDOW_H

#include <nitro.h>

#include "MessageWindow.h"
#include "SpriteButton.h"
#include "item/Item.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"

// The shops' price window (main program 0x020AA464 to 0x020AB09C, src/shop/Shop.cpp; table 0x02115368): a message
// window that asks how many of an item to buy or sell, with a plus and a minus button and three canvases more: the
// count, the total and the money. Its constructor is inline (the window's, then its table, then its members), as the
// shop scenes' constructors show it.
//
// The names are descriptive: no name of these classes survives in the ROM.
class ShopWindow : public MessageWindow {
public:
    // mMode
    enum Mode {
        MODE_BUY,
        MODE_SELL,
    };

    ShopWindow() {}

    virtual void SetVisible(BOOL visible);
    virtual void SetPosition(WindowPoint position);  // Moves its buttons and canvases with the window
    virtual int GetBuyTotal(Item *item, int count);  // The price times count
    virtual int GetSellTotal(Item *item, int count); // The second price times count

    void Init(ScreenRect rect);
    void InitButtons(int resource, int *charOffset, MessageFile *file);
    void Init(const char *screenName, int plane, int resource, int *charOffset, MessageFile *file,
              MessageFile *buttonFile, int *money);
    void Update();
    void Draw();
    void Open(int flags, int mode);
    int GetTotal();
    void DrawCounts();

    /* 0xAFA4 */ int mMode;
    /* 0xAFA8 */ int *mMoney;
    /* 0xAFAC */ Item *mItem;
    /* 0xAFB0 */ int mCount;
    /* 0xAFB4 */ SpriteButton mPlusButton;
    /* 0xB0A8 */ SpriteButton mMinusButton;
    /* 0xB19C */ ObjTextCanvas mTextCanvases[3]; // The count, the total, the money
    /* 0xB6C4 */ MessageFile *mMessageFile; // The window's own messages
}; // 0xB6C8

// The price window that buys at half the price and sells at 11/10 of it (table 0x02115380).
class HalfPriceShopWindow : public ShopWindow {
public:
    HalfPriceShopWindow() {}

    virtual int GetBuyTotal(Item *item, int count);
    virtual int GetSellTotal(Item *item, int count);
}; // 0xB6C8

#endif // SHOP_SHOPWINDOW_H
