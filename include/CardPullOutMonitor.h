#ifndef CARDPULLOUTMONITOR_H
#define CARDPULLOUTMONITOR_H

#include <nitro.h>

// Stops the game when the game card is pulled out: main() calls Update every frame.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class CardPullOutMonitor {
public:
    // Empty, and inline: the file's static initialiser is a lone return (0x020B0604)
    CardPullOutMonitor() {}
    void Init();
    void Update();
    void Terminate();

    static BOOL OnPulledOut();
};

// main() initialises it and updates it every frame
extern CardPullOutMonitor gCardPullOutMonitor;

#endif // CARDPULLOUTMONITOR_H
