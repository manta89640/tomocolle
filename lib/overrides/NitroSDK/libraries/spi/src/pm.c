/*
 * The game's spi/pm.c (0x0200ECA8 to 0x0200F668) is a later release than the fetched one: the busy wait is a choice of
 * three methods through a pointer, so that power-off works with interrupts disabled; one utility command function
 * takes a parameter and a result buffer; PM_Init no longer sets up the register buffers or a mutex; turning the LCDs off
 * and on sends utility command 0x10; and PM_ForceToPowerOff retries until it succeeds, then stops all DMA and halts.
 * Written from the game's code; see tools/override_functions.py for how this file is used. The names of functions the
 * fetched release does not have are the project's own, for what their code does.
 */

/* override: replace PMi_IsInit */
// In the order that gives the game's .bss pool: each is initialised so that it goes on the list where it is declared
static u16 PMi_IsInit = FALSE;
static u32 PMi_InitVBlankCount = 0;     // The frame of PM_Init: PMi_SetLCDPower waits two frames if it is within two
static PMSleepCallbackInfo * PMi_PostSleepCallbackList = NULL;
static u32 PMi_LCDUtilityParam = 0;     // What PMi_SetLCDPower sends with utility command 0x10 once the LCDs are on
static volatile BOOL PMi_SleepEndFlag = FALSE; // Set by the ARM7's command 0x63
static PMSleepCallbackInfo * PMi_PreSleepCallbackList = NULL;
static u32 PMi_LCDCount = 0;            // The frame the LCDs were last turned off
static PMiWork PMi_Work;

static void PMi_WaitBusy(void);
static void (*PMi_WaitBusyMethod)(void) = PMi_WaitBusy;

void MI_StopAllDma(void);

/* override: remove PMi_Callback */
/* override: remove PMi_Work */
/* override: remove PMi_RegisterBuffer */
/* override: remove PMi_SyncFlag */
/* override: remove PMi_SleepEndFlag */
/* override: remove PMi_Mutex */
/* override: remove PMi_LCDCount */
/* override: remove PMi_PreSleepCallbackList */
/* override: remove PMi_PostSleepCallbackList */
/* override: remove PMi_SetAmp */
/* override: remove sAmpSwitch */

/* override: replace PMi_WaitBusy */
// The default: in an IRQ handler, receive the PXI FIFO by hand, since the reply cannot interrupt
static void PMi_WaitBusy(void)
{
    volatile BOOL * p = &PMi_Work.lock;

    if (PMi_Work.lock) {
        do {
            if (OS_GetProcMode() == OS_PROCMODE_IRQ) {
                PXIi_HandlerRecvFifoNotEmpty();
            }
        } while (*p);
    }
}

// Receives the FIFO by hand whenever IRQs are disabled or the master enable is off
static void PMi_WaitBusyWithInterruptsOff(void)
{
    volatile BOOL * p = &PMi_Work.lock;

    if (PMi_Work.lock) {
        do {
            if (OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE || reg_OS_IME == 0) {
                PXIi_HandlerRecvFifoNotEmpty();
            }
        } while (*p);
    }
}

// Both: whenever an interrupt could not deliver the reply
static void PMi_WaitBusyAnyContext(void)
{
    volatile BOOL * p = &PMi_Work.lock;

    if (PMi_Work.lock) {
        do {
            if (OS_GetCpsrIrq() == OS_INTRMODE_IRQ_DISABLE || reg_OS_IME == 0 || OS_GetProcMode() == OS_PROCMODE_IRQ) {
                PXIi_HandlerRecvFifoNotEmpty();
            }
        } while (*p);
    }
}

/* override: replace PMi_DummyCallback */
static void PMi_DummyCallback(u32 result, void * arg)
{
    if (arg) {
        *(u32 *)arg = result;
    }
}

/* override: replace PMi_CallCallbackAndUnlock */
static void PMi_CallCallbackAndUnlock(u32 result)
{
    PMCallback callback = PMi_Work.callback;
    void * arg = PMi_Work.callbackArg;

    PMi_Work.lock = FALSE;
    if (callback) {
        PMi_Work.callback = NULL;
        callback(result, arg);
    }
}

/* override: replace PM_Init */
void PM_Init(void)
{
    if (PMi_IsInit) {
        return;
    }
    PMi_IsInit = TRUE;
    PMi_Work.lock = FALSE;
    PMi_Work.callback = NULL;

    PXI_Init();
    while (!PXI_IsCallbackReady(PXI_FIFO_TAG_PM, PXI_PROC_ARM7)) {
        SVC_WaitByLoop(100);
    }
    PXI_SetFifoRecvCallback(PXI_FIFO_TAG_PM, PMi_CommonCallback);

    PMi_LCDCount = PMi_InitVBlankCount = OS_GetVBlankCount();
}

/* override: replace PMi_CommonCallback */
void PMi_CommonCallback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag)
    u16 command;
    u16 result;

    command = (u16)((data & 0x7f00) >> 8);
    result = (u16)(data & 0xff);

    if (err) {
        switch (command) {
        case 0x61:
        case 0x62:
            result = PM_RESULT_BUSY;
            break;
        default:
            result = PM_RESULT_ERROR;
            break;
        }
        PMi_CallCallbackAndUnlock(result);
        return;
    }

    switch (command) {
    case 0x61:
        if (PMi_Work.work) {
            *(u16 *)PMi_Work.work = (u16)result;
        }
        result = PM_RESULT_SUCCESS;
        break;
    case 0x60:
        result = PM_RESULT_SUCCESS;
        break;
    case 0x62:
        break;
    case 0x63:
        PMi_SleepEndFlag = TRUE;
        break;
    }
    PMi_CallCallbackAndUnlock(result);
}

/* override: remove PMi_SendSleepStart */

/* override: replace PM_SendUtilityCommandAsync */
// Sends a utility command with a 16-bit parameter; the ARM7's reply, if it has one, goes to *result
static u32 PMi_SendUtilityCommandAsync(u16 command, u16 param, u16 * result, PMCallback callback, void * arg)
{
    if (!PMi_Lock()) {
        return PM_BUSY;
    }
    PMi_Work.callback = callback;
    PMi_Work.callbackArg = arg;
    PMi_Work.work = result;

    PMi_SendPxiData(PMi_MakeData1(SPI_PXI_START_BIT, 0, 0x61, command));
    PMi_SendPxiData(PMi_MakeData2(SPI_PXI_END_BIT, 1, param));
    return PM_SUCCESS;
}

/* override: replace PM_SendUtilityCommand */
static u32 PMi_SendUtilityCommand(u16 command, u16 param, u16 * result)
{
    u32 commandResult;
    u32 sendResult = PMi_SendUtilityCommandAsync(command, param, result, PMi_DummyCallback, &commandResult);

    if (sendResult != PM_SUCCESS) {
        return sendResult;
    }
    PMi_WaitBusyMethod();
    return commandResult;
}

/* override: remove PMi_ReadRegisterAsync */
/* override: remove PMi_ReadRegister */
/* override: remove PMi_WriteRegisterAsync */
/* override: remove PMi_WriteRegister */

/* override: replace PMi_SetLEDAsync */
u32 PMi_SetLEDAsync(PMLEDStatus status, PMCallback callback, void * arg)
{
    u16 command;

    switch (status) {
    case PM_LED_ON:
        command = PM_UTIL_LED_ON;
        break;
    case PM_LED_BLINK_HIGH:
        command = PM_UTIL_LED_BLINK_HIGH_SPEED;
        break;
    case PM_LED_BLINK_LOW:
        command = PM_UTIL_LED_BLINK_LOW_SPEED;
        break;
    default:
        command = 0;
        break;
    }
    if (command == 0) {
        return PM_INVALID_COMMAND;
    }
    return PMi_SendUtilityCommandAsync(command, 0, NULL, callback, arg);
}

/* override: replace PMi_SetLED */
u32 PMi_SetLED(PMLEDStatus status)
{
    u32 commandResult;
    u32 sendResult = PMi_SetLEDAsync(status, PMi_DummyCallback, &commandResult);

    if (sendResult != PM_SUCCESS) {
        return sendResult;
    }
    PMi_WaitBusyMethod();
    return commandResult;
}

/* override: replace PM_SetBackLightAsync */
u32 PM_SetBackLightAsync(PMLCDTarget target, PMBackLightSwitch sw, PMCallback callback, void * arg)
{
    u16 command = 0;

    if (target == PM_LCD_TOP) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD2_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD2_BACKLIGHT_OFF;
        }
    } else if (target == PM_LCD_BOTTOM) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD1_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD1_BACKLIGHT_OFF;
        }
    } else if (target == PM_LCD_ALL) {
        if (sw == PM_BACKLIGHT_ON) {
            command = PM_UTIL_LCD12_BACKLIGHT_ON;
        }
        if (sw == PM_BACKLIGHT_OFF) {
            command = PM_UTIL_LCD12_BACKLIGHT_OFF;
        }
    }
    if (command == 0) {
        return PM_INVALID_COMMAND;
    }
    return PMi_SendUtilityCommandAsync(command, 0, NULL, callback, arg);
}

/* override: replace PM_SetBackLight */
u32 PM_SetBackLight(PMLCDTarget target, PMBackLightSwitch sw)
{
    u32 commandResult;
    u32 sendResult = PM_SetBackLightAsync(target, sw, PMi_DummyCallback, &commandResult);

    if (sendResult != PM_SUCCESS) {
        return sendResult;
    }
    PMi_WaitBusyMethod();
    return commandResult;
}

/* override: remove PMi_SetSoundPowerAsync */
/* override: remove PMi_SetSoundPower */
/* override: remove PMi_SetSoundVolumeAsync */
/* override: remove PMi_SetSoundVolume */

/* override: replace PM_ForceToPowerOffAsync */
static void PMi_PrepareLCDForPowerOff(void);

u32 PM_ForceToPowerOffAsync(PMCallback callback, void * arg)
{
    void (*method)(void) = PMi_WaitBusyMethod;

    PMi_WaitBusyMethod = PMi_WaitBusyAnyContext;
    PMi_PrepareLCDForPowerOff();
    PMi_WaitBusyMethod = method;

    return PMi_SendUtilityCommandAsync(PM_UTIL_FORCE_POWER_OFF, 0, NULL, callback, arg);
}

/* override: replace PM_ForceToPowerOff */
// One attempt: the command, then the wait, with interrupts possibly disabled
static u32 PMi_ForceToPowerOff(void)
{
    u32 commandResult;
    u32 sendResult = PM_ForceToPowerOffAsync(PMi_DummyCallback, &commandResult);

    if (sendResult != PM_SUCCESS) {
        return sendResult;
    }
    (PMi_WaitBusyMethod = PMi_WaitBusyWithInterruptsOff)();
    PMi_WaitBusyMethod = PMi_WaitBusy;
    return commandResult;
}

u32 PM_ForceToPowerOff(void)
{
    while (PMi_ForceToPowerOff() != PM_SUCCESS) {
        OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
    }
    (void)OS_DisableInterrupts();
    MI_StopAllDma();
    while (TRUE) {
        OS_Halt();
    }
    return PM_SUCCESS;
}

// Sends utility command 0x10, which the LCDs' power changes come with, while the LCDs are on
static u32 PMi_SendLCDUtilityCommand(u32 param)
{
    if (!PM_GetLCDPower()) {
        return PM_SUCCESS;
    }
    return PMi_SendUtilityCommand(0x10, (u16)param, NULL);
}

/* override: remove PM_SetAmpAsync */
/* override: remove PM_SetAmp */
/* override: remove PM_SetAmpGainAsync */
/* override: remove PM_SetAmpGain */
/* override: remove PM_GetBattery */
/* override: remove PM_GetBackLight */
/* override: remove PMi_GetSoundPower */
/* override: remove PMi_GetSoundVolume */
/* override: remove PM_GetAmp */
/* override: remove PM_GetAmpGain */
/* override: remove PM_GoSleepMode */

/* override: replace PMi_SetLCDPower */
BOOL PMi_SetLCDPower(PMLCDPower sw, PMLEDStatus led, BOOL skip, BOOL isSync)
{
    switch (sw) {
    case PM_LCD_POWER_ON:
        if (!skip && OS_GetVBlankCount() - PMi_LCDCount <= 7) {
            return FALSE;
        }
        if (led != PM_LED_NONE) {
            if (isSync) {
                while (PMi_SetLED(led) != PM_SUCCESS) {
                    OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
                }
            } else {
                while (PMi_SetLEDAsync(led, NULL, NULL) != PM_SUCCESS) {
                    OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
                }
            }
        }
        reg_GX_POWCNT |= REG_GX_POWCNT_LCD_MASK;
        while (PMi_SendLCDUtilityCommand(PMi_LCDUtilityParam) != PM_SUCCESS) {
            OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
        }
        break;
    case PM_LCD_POWER_OFF:
        while (PMi_SendLCDUtilityCommand(0) != PM_SUCCESS) {
            OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
        }
        if (OS_GetVBlankCount() - PMi_InitVBlankCount <= 2) {
            OS_WaitVBlankIntr();
            OS_WaitVBlankIntr();
        }
        reg_GX_POWCNT &= ~REG_GX_POWCNT_LCD_MASK;
        PMi_LCDCount = OS_GetVBlankCount();
        if (led != PM_LED_NONE) {
            if (isSync) {
                while (PMi_SetLED(led) != PM_SUCCESS) {
                    OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
                }
            } else {
                while (PMi_SetLEDAsync(led, NULL, NULL) != PM_SUCCESS) {
                    OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
                }
            }
        }
        break;
    }
    return TRUE;
}

/* override: remove PMi_GetLCDOffCount */
/* override: remove PMi_SendLEDPatternCommandAsync */
/* override: remove PMi_SendLEDPatternCommand */
/* override: remove PM_GetLEDPatternAsync */
/* override: remove PM_GetLEDPattern */

/* override: replace PMi_DeleteList */
void PMi_DeleteList(PMSleepCallbackInfo ** listp, PMSleepCallbackInfo * info)
{
    OSIntrMode enabled;
    PMSleepCallbackInfo * p;
    PMSleepCallbackInfo * pre;

    if (!listp) {
        return;
    }
    enabled = OS_DisableInterrupts();
    pre = p = *listp;
    while (p) {
        if (p == info) {
            if (p == pre) {
                *listp = p->next;
            } else {
                pre->next = p->next;
            }
            break;
        }
        pre = p;
        p = p->next;
    }
    (void)OS_RestoreInterrupts(enabled);
}

/* override: end */
// Before the power goes off, the LCDs are turned on with their backlights off
static void PMi_PrepareLCDForPowerOff(void)
{
    OS_SpinWait(0x360000);
    if (PM_GetLCDPower() == PM_LCD_POWER_ON) {
        return;
    }
    while (PM_SetBackLight(PM_LCD_ALL, PM_BACKLIGHT_OFF) != PM_SUCCESS) {
        OS_SpinWait(HW_CPU_CLOCK_ARM9 / 100);
    }
    while (!PM_SetLCDPower(PM_LCD_POWER_ON)) {
        OS_SpinWait(10);
    }
}
