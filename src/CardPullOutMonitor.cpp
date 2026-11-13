#include "CardPullOutMonitor.h"

void CardPullOutMonitor::Init() {
    CARD_SetPulledOutCallback(OnPulledOut);
}

void CardPullOutMonitor::Update() {
    if (CARD_IsPulledOut()) {
        Terminate();
    }
}

void CardPullOutMonitor::Terminate() {
    GX_DispOff();
    GXS_DispOff();
    CARD_TerminateForPulledOut();
}

BOOL CardPullOutMonitor::OnPulledOut() {
    return FALSE;
}
