#ifndef CARDPULLOUTMONITOR_H
#define CARDPULLOUTMONITOR_H

#include <nitro.h>

// Stops the game when the game card is pulled out: main() calls Update every frame.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class CardPullOutMonitor {
public:
    void Init();
    void Update();
    void Terminate();

    static BOOL OnPulledOut();
};

#endif // CARDPULLOUTMONITOR_H
