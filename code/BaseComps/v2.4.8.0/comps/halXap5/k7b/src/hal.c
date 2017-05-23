/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *   Hardware Abstraction Layer for Xap5 devices.
 *
 *
 *                ,               This software is owned by GreenPeak Technologies
 *                g               and protected under applicable copyright laws.
 *               ]&$              It is delivered under the terms of the license
 *               ;QW              and is intended and supplied for use solely and
 *               G##&             exclusively with products manufactured by
 *               N#&0,            GreenPeak Technologies.
 *              +Q*&##
 *              00#Q&&g
 *             ]M8  *&Q           THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 *             #N'   Q0&          CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 *            i0F j%  NN          IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 *           ,&#  ##, "KA         LIMITED TO, IMPLIED WARRANTIES OF
 *           4N  NQ0N  0A         MERCHANTABILITY AND FITNESS FOR A
 *          2W',^^ `48  k#        PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *         40f       ^6 [N        GREENPEAK TECHNOLOGIES B.V. SHALL NOT, IN ANY
 *        jB9         `, 0A       CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 *       ,&?             ]G       INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 *      ,NF               48      FOR ANY REASON WHATSOEVER.
 *      EF                 @
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/k7b/src/hal.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5
#include "hal.h"
#include "hal_WB.h"
#include "gpBsp.h"
#include <machine/xap.h>
#include "gpLog.h"
#include "gpHal.h"
#include "gphal_xap.h"

#include "gpAssert.h"
#include "hal_ExtendedPatch.h"

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define HAL_WDT_TIMEOUT 0xFFFF /*in 16us*/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
static gpHal_AbsoluteEventId_t hal_Xap5_gpSched_EventId;

/*****************************************************************************
 *                    static Function Definitions
 *****************************************************************************/
static void hal_InitSleep(void);
static void hal_AfterSleep(void);

INTERRUPT_H void handle_rci_int(void);
INTERRUPT_H void handleESInt(void);

#define GP_HAL_STANDBYMODEDURINGSLEEP 0xFF
void gpHal_WriteSleepMode(gpHal_SleepMode_t mode);

/*****************************************************************************
 *                    Init
 *****************************************************************************/

void hal_Init(void)
{
    //Clr for debugging purposes
    GP_WB_STANDBY_CLR_PORD_INTERRUPT();

#if defined(GP_COMP_GPHAL_ES_ABS_EVENT)
    /* check for mismatch between configuration and memory map */
    GP_ASSERT_SYSTEM(TOTAL_NR_OF_ABSOLUTE_EVENTS_ALLOCATED >= GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS);
#endif //defined(GP_COMP_GPHAL_ES_ABS_EVENT)

    hal_Xap5_gpSched_EventId = gpHal_GetAbsoluteEvent();

    //Active interrupt triggers every wakeup - unused for now, disable
    GP_WB_WRITE_INT_CTRL_MASK_STBC_ACTIVE_INTERRUPT(0);

    // Enable interrupt when vddb < 1.8V
    GP_WB_WRITE_INT_CTRL_MASK_STBC_VLT_STATUS_INTERRUPT(1);

    HAL_LED_INIT();
    HAL_BTN_INIT();

    //Call macro directly - avoid inclusion of gphal related material in gpBsp.c
    HAL_BSP_INIT();

    //Initialize unused pins
    HAL_UNUSED_INIT();

    //Get and check HW version
    gpHal_InitVersionInfo();

    hal_InitSleep();


    hal_EnableWatchdog(HAL_WDT_TIMEOUT);

    if (HAL_DEBUG_IS_SPI_ENABLED())
    {
        //Enable SPI
        GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_USE_SPI);
        GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
        //Enable pullup on SSN pin
        GP_WB_WRITE_IOB_GPIO_4_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    }
}

/*****************************************************************************
 *                    STACK
 *****************************************************************************/

UInt8* hal_GetStackStartAddress(void)
{
    return ((UInt8*)((UIntPtr)GP_MM_RAM_GEN_WINDOW_VAR_0_START + (UIntPtr)(GP_WB_READ_MM_GEN_WINDOW_VAR_0_LEN()*2) -1));
}
UInt8* hal_GetStackEndAddress(void)
{
    return ((UInt8*)((UIntPtr)GP_MM_RAM_GEN_WINDOW_VAR_0_START));
}

/*****************************************************************************
 *                    DEBUG
 *****************************************************************************/


UInt8 hal_DebugGetInfo(void)
{
    UInt8 debugInfo = 0;
    //Not used during runtime - kept for debugging while still using keypad fi
#if defined(HAL_DEBUG_SPI_ENABLED) || defined(HAL_SPI_ENABLED)
    //Spi enabled   BM(0)
    debugInfo |= BM(0);
#endif //defined(HAL_DEBUG_SPI_ENABLED) || defined(HAL_SPI_ENABLED)
    //UART enabled  BM(1)
    //Sleep enable  BM(2)
    debugInfo |= BM(2);
    //XSIF enable   BM(3)
#if defined(HAL_XSIF_ENABLED)
    debugInfo |= BM(3);
#endif

    return debugInfo;
}

#ifdef HAL_DEBUG_SIF_AVAILABLE
void hal_EnableDebugMode(void)
{
    // Map sif pins
    GP_WB_WRITE_XAP5_GP_SIF_CLK_MAPPING(HAL_DEBUG_MAPPING_SIF_SCLK);
    GP_WB_WRITE_XAP5_GP_SIF_LOADB_MAPPING(HAL_DEBUG_MAPPING_SIF_LOADB);
    GP_WB_WRITE_XAP5_GP_SIF_MISO_MAPPING(HAL_DEBUG_MAPPING_SIF_MISO);
    GP_WB_WRITE_XAP5_GP_SIF_MOSI_MAPPING(HAL_DEBUG_MAPPING_SIF_MOSI);
}

void hal_DisableDebugMode(void)
{
    // Unmap sif pins
    GP_WB_WRITE_XAP5_GP_SIF_CLK_MAPPING(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_XAP5_GP_SIF_LOADB_MAPPING(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_XAP5_GP_SIF_MISO_MAPPING(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_XAP5_GP_SIF_MOSI_MAPPING(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
}
#endif //HAL_DEBUG_SIF_AVAILABLE

/*****************************************************************************
 *                    LED
 *****************************************************************************/

void hal_ledSetClr(UInt8 led, UInt8 fade, Bool setClr)
{
    UInt8 x = !!setClr;

    GP_ASSERT_DEV_EXT(led < 4);

    if (fade != 0)
    {
        x |= 0x2;
        GP_WB_WRITE_LED_SLOPE_TIME_UNIT(fade);
    }
    GP_WB_MWRITE_U8(GP_WB_LED_ENABLE_0_ADDRESS, (0x03 << led*2), (x << led*2));
}

Bool hal_ledGet(UInt8 led)
{
    return GP_WB_READ_U8(GP_WB_LED_ENABLE_0_ADDRESS) & (0x01 << led*2);
}

/*****************************************************************************
 *                    GPIO
 *****************************************************************************/

#define __init(port,idx) { BM(idx), (0x##port-0xA) }
const hal_gpiodsc_t gpios[30] =
{
    [0]  = __init(A,0),
    [1]  = __init(A,1),
    [2]  = __init(A,2),
    [3]  = __init(A,3),
    [4]  = __init(A,4),
    [5]  = __init(A,5),
    [6]  = __init(A,6),
    [7]  = __init(A,7),
    [8]  = __init(B,0),
    [9]  = __init(B,1),
    [10] = __init(B,2),
    [11] = __init(B,3),
    [12] = __init(B,4),
    [13] = __init(B,5),
    [14] = __init(B,6),
    [15] = __init(B,7),
    [16] = __init(C,0),
    [17] = __init(C,1),
    [18] = __init(C,2),
    [19] = __init(C,3),
    [20] = __init(C,4),
    [21] = __init(C,5),
    [22] = __init(C,6),
    [23] = __init(C,7),
    [24] = __init(D,0),
    [25] = __init(D,1),
    [26] = __init(D,2),
    [27] = __init(D,3),
    [28] = __init(D,4),
    [29] = __init(D,5),
};
#undef __init

Bool hal_gpioGet(hal_gpiodsc_t gpio)
{
    UInt8 portValue = GP_HAL_READ_REG(GP_WB_GPIO_GPIO0_INPUT_VALUE_ADDRESS + gpio.regoffset);
    return !!(portValue & gpio.mask);
}

Bool hal_gpioGetSetClr(hal_gpiodsc_t gpio)
{
    UInt8 outputValue = GP_HAL_READ_REG(GP_WB_GPIO_GPIO0_OUTPUT_VALUE_ADDRESS + gpio.regoffset);
    return !!(outputValue & gpio.mask);
}

void hal_gpioSet(hal_gpiodsc_t gpio)
{
    GP_HAL_READMODIFYWRITE_REG(GP_WB_GPIO_GPIO0_OUTPUT_VALUE_ADDRESS + gpio.regoffset, gpio.mask, 0xFF);
}

void hal_gpioClr(hal_gpiodsc_t gpio)
{
    GP_HAL_READMODIFYWRITE_REG(GP_WB_GPIO_GPIO0_OUTPUT_VALUE_ADDRESS + gpio.regoffset, gpio.mask, 0x0);
}

void hal_gpioModePP(hal_gpiodsc_t gpio, Bool enable)
{
    GP_HAL_READMODIFYWRITE_REG(GP_WB_GPIO_GPIO0_DIRECTION_ADDRESS + gpio.regoffset, gpio.mask, (enable ? 0xFF : 0x0));
}

Bool hal_gpioGetModePP(hal_gpiodsc_t gpio)
{
    UInt8 portValue = GP_HAL_READ_REG(GP_WB_GPIO_GPIO0_DIRECTION_ADDRESS + gpio.regoffset);
    return !!(portValue & gpio.mask);
}


void hal_gpioModePU(UInt8 gpio, Bool enable)
{
    UInt8 shift = ((gpio%4)*2);
    UInt8 setting = enable ? (GP_WB_ENUM_GPIO_MODE_PULLUP << shift) : (GP_WB_ENUM_GPIO_MODE_BUSKEEPER << shift);

    //Set IOB to floating or buskeeper
    GP_HAL_READMODIFYWRITE_REG(GP_WB_IOB_GPIO_0_CFG_ADDRESS + (gpio/4), 0x3 << shift, setting);
}

/*****************************************************************************
 *                    Watchdog
 *****************************************************************************/

void hal_EnableWatchdog(UInt16 timeout) /*timeout in 16us*/
{
    if (GP_BSP_USE_WDT_TIMER())
    {
        GP_WB_WRITE_WATCHDOG_KEY(0x55) ;
        //Change window is open - changes can be made
        GP_WB_WRITE_WATCHDOG_TIMEOUT(timeout);
        GP_WB_WRITE_WATCHDOG_WATCHDOG_FUNCTION(GP_WB_ENUM_WATCHDOG_FUNCTION_SOFT_POR);
        GP_WB_WRITE_WATCHDOG_FREEZE_WHEN_UC_ASLEEP(true);
        GP_WB_WRITE_WATCHDOG_FREEZE_WHEN_UC_BREAK_POINT(true);
        GP_WB_WRITE_WATCHDOG_ENABLE(1);
        GP_WB_WRITE_WATCHDOG_KEY(0xAA); //Reset watchdog
        // Wait for change window (to be safe)
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_WATCHDOG_CONTROL_CHANGE_WINDOW_ONGOING(), 1000000UL);
    }
}

void hal_DisableWatchdog(void)
{
    if (GP_BSP_USE_WDT_TIMER())
    {
        GP_WB_WRITE_WATCHDOG_KEY(0x55); //Open change window
        GP_WB_WRITE_WATCHDOG_ENABLE(0);
        // Wait for change window (to be safe)
        GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_WATCHDOG_CONTROL_CHANGE_WINDOW_ONGOING(), 1000000UL);
    }
}

void hal_ResetWatchdog(void)
{
    if (GP_BSP_USE_WDT_TIMER())
    {
        GP_WB_WRITE_WATCHDOG_KEY(0xAA);
    }
}

void hal_TriggerWatchdog(void)
{
    if(GP_WB_READ_WATCHDOG_ENABLE())
    {
        while(1)
        {
            // fixme: might be better to set timeout to 0 and timeout immediately
            // let the watchdog time out
        }
    }
}

/*****************************************************************************
 *                    Reset reason
 *****************************************************************************/

hal_ResetReason_t hal_GetResetReason(void)
{
    UInt8 reason = GP_WB_READ_PMUD_POR_REASON();

    if (reason == GP_WB_ENUM_POR_REASON_HW_POR)
    {
        return hal_ResetReason_HWPor;
    }
    else if (reason == GP_WB_ENUM_POR_REASON_POR_BY_VDDB_CUTOFF)
    {
        return hal_ResetReason_BrownOut;
    }
    else if (reason == GP_WB_ENUM_POR_REASON_SOFT_POR_BY_WATCHDOG)
    {
        return hal_ResetReason_Watchdog;
    }
    else if (reason == GP_WB_ENUM_POR_REASON_SOFT_POR_BY_REGMAP)
    {
        return hal_ResetReason_SWPor;
    }

//Not specified separately:
// GP_WB_ENUM_POR_REASON_SOFT_POR_BY_VRR_BROWNOUT 0x2
// GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ISO_TX 0x3
// GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ES 0x5
// GP_WB_ENUM_POR_REASON_POR_BY_VDDDIG_NOK 0x7

    return hal_ResetReason_Unspecified;
}

/*****************************************************************************
 *                    Timer
 *****************************************************************************/

/** Wait a number of us.
Function should compile to:
    X cycles for calculation 62.5ns cycles (min 1)
    2 cycles for clu.s itself
    1 cycle to fill pipe-line again
Using non-interruptible version of instruction.
 - (0) Not Interruptible, clock gated wait
No interrupts will be serviced during wait (regardless of interrupt enabled/disabled flag)
For reference:
 - (1) Interruptible, clock gated wait
*/
void hal_Waitus(UInt16 us)
{
    UInt16 cycles = us*16 - (1+2+1);

    asm volatile ("clu.s #(0), %0" /*(0) Not Interruptible, clock gated wait*/
                    : /*no output*/
                    : "r" (cycles));
}

/** Wait a number of ms.
Interrupt servicing is still possible - in between 125us blocking parts
For an exact wait time, disable all interrupts manually
 */
void hal_Waitms(UInt16 ms)
{
    UInt32 ms_8;

    ms_8 = ms*8;

    //Split in smaller blocks to allow interupts to come in between blocking waitUs
    while (ms_8--)
    {
        hal_Waitus(125);
    }
}

/*****************************************************************************
 *                    Atomic
 *****************************************************************************/

UInt8 l_n_atomic = 0;

void hal__AtomicOn (void)
{
    if (!l_n_atomic)
    {
        xap_irqd();
    }
    l_n_atomic++;
}

void hal__AtomicOff (void)
{
    //disable interrupts to be sure
    xap_irqd();
    if (l_n_atomic > 1)
    {
        //just decrement the 'atomic nesting level'
        l_n_atomic--;
    }
    else
    {
        l_n_atomic = 0;
        xap_irqe();
    }
}


/*****************************************************************************
 *                    SLEEP
 *****************************************************************************/

void hal_InitSleep(void)
{
    //No callback needed
    gpHal_EnableAbsoluteEventCallbackInterrupt(hal_Xap5_gpSched_EventId, true);

    //Enable deep sleep
    if (HAL_DEBUG_IS_SLEEP_ENABLED())
    {
        GP_WB_WRITE_STANDBY_RTGTS_SW(true);
    }
}

void hal_AfterSleep(void)
{
    UInt8 sleep;

    hal_DisableGotoSleep();

    sleep = GP_WB_READ_PMUD_STBY_MODE();

    //We don't want delay in this function if it is not needed
    if (HAL_DEBUG_IS_UART_ENABLED())
    {
        GP_LOG_PRINTF("WU from %i",2, sleep);
    }
    if (HAL_DEBUG_IS_SPI_ENABLED())
    {
        GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
        /*
        Already save/restored by default (sm restore)
        GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_USE_SPI);
        GP_WB_WRITE_IOB_GPIO_4_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
        */
    }
    if (sleep != GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE)
    {
        gpHal_WriteSleepMode(GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE);
    }
#ifndef HAL_UART_NO_RX
    if (hal_UartRxEnabled())
    {
        hal_UartRxEnableDMA();
    }
#endif
}

//hal_IntHandlerPrologue(void) is the first code executed after wake up, as we always wake up on a interrupt
void hal_IntHandlerPrologue(void)
{
    hal_DisableGotoSleep();

    //Watchdog is still running during ISR handling (fi Keyscan ISR)
    //kick it here as we might not reach the scheduler main loop
    HAL_WDT_RESET();
}


void hal_sleep_uc(UInt32 sleeptime)
{
    Bool bTimedEvent = false;
    UIntPtr spAfterContextPush;
    Bool stackContextOverlapImminent = false;
#define CONTEXT_SAVE_SIZE   44 /* Hardcoded value that has to match the ammount of data pushed on stack by context_backup in crt0.S */

    GP_ASSERT_DEV_EXT(l_n_atomic == 0);

    //Don't try to sleep for VERY short periods
    if (sleeptime < 1000/32)
        return;

    //SW-3888  --  if (sleeptime * 32) > 0x7fffffff then the time is considered as "passed"
    GP_ASSERT_DEV_EXT( (sleeptime == HAL_SLEEP_INDEFINITE_SLEEP_TIME) || (sleeptime <= HAL_SLEEP_MAX_SLEEP_TIME ) );

    //we disable interrupts again, up to just before sleep instruction
    //so we now that all preparation steps have been done or none.
    HAL_DISABLE_GLOBAL_INT();

    /* gpHal_SetSleepMode(GP_WB_ENUM_STANDBY_MODE_RC_MODE); */
    gpHal_WriteSleepMode(GP_HAL_STANDBYMODEDURINGSLEEP);

    if (sleeptime != HAL_SLEEP_INDEFINITE_SLEEP_TIME)
    {
        //Want to wake up in 'sleeptime' * 32us
        //we need to schedule the wakeup event.
        gpHal_AbsoluteEventDescriptor_t ev;

        MEMSET(&ev, 0, sizeof(ev));

        ev.type             = GP_WB_ENUM_EVENT_TYPE_DUMMY;
        /* we need ex_itl when waking up very fast*/
        ev.executionOptions = GP_ES_EXECUTION_OPTIONS_EXECUTE_IF_TOO_LATE;
        ev.interruptOptions = GP_ES_INTERRUPT_OPTIONS_MASK;
        if (HAL_BSP_IO_ACTIVITY_PENDING())
        {
            ev.executionOptions |= GP_ES_EXECUTION_OPTIONS_PROHIBIT_STANDBY;
        }
        gpHal_GetTime(&ev.exTime);
        ev.exTime += sleeptime*32;

        GP_ES_SET_EVENT_STATE(ev.control, gpHal_EventStateScheduled);
        gpHal_ScheduleAbsoluteEvent(&ev, hal_Xap5_gpSched_EventId);

        //Will wake up due to event scheduled - need timing
        bTimedEvent = true;
    }

    // finally, everything is ready for a healthy sleep
    HAL_ENABLE_GLOBAL_INT();


    spAfterContextPush = xap_getAreg(XAP_SP0); //Get current stack pointer

    if(spAfterContextPush < (GP_MM_RAM_GEN_WINDOW_VAR_0_START + GP_MM_RAM_RETENTION_SIZE + CONTEXT_SAVE_SIZE))
    {
        stackContextOverlapImminent = true;
    }
    if(!stackContextOverlapImminent)
    {
#ifndef HAL_UART_NO_RX        
        // SW-4023 - DMA pointers become invalid after a sleep
        hal_UartRxDisableDMA();
        hal_UartRxComFlush();
#endif        

        // Go to sleep
        xap_sleepsif();
    }

    //at this point, any interrupt is executed and IntHandlerPrologue() is executed
    //OR we did not go to sleep at all...
    hal_AfterSleep();
    if(bTimedEvent)
    {
        gpHal_UnscheduleAbsoluteEvent(hal_Xap5_gpSched_EventId);
    }
    if(stackContextOverlapImminent)
    {
        GP_ASSERT_SYSTEM(false);    // indicates problem.
    }

}

void hal_Reset(void)
{
    GP_WB_WRITE_PMUD_SOFT_POR(1);
    while (1);
}

void hal_InitRamRetention(UInt8 ramRetentionSize)
{
    UInt8 ramRetentionBlocks = (ramRetentionSize+3) >> 2; //round to upper 4K block
    UInt8 vddram_sel = 0;
    UInt8 vddram_sel_mask = 0; //bitmask with ramblocks to keep retained

    if (ramRetentionBlocks > 3)
    {
        //full retention required ram
        return;
    }

    if (ramRetentionBlocks > 0)
    {
        //select ram blocks to be retained based on the ram_remapping information
        UIntLoop i;
        UInt8 ram_remap = GP_WB_READ_PMUD_RAM_BLOCK_REMAP_ENTRIES();
        for (i = 0; i < ramRetentionBlocks; ++i)
        {
            vddram_sel_mask |=  (1 << ((ram_remap >> (i*2)) & 0x3)); //ram_remap bitmaks of 4 times 2 bit block selection
        }

        vddram_sel = GP_WB_READ_PMUD_VDDRAM_SEL();
    }

    vddram_sel |= (vddram_sel_mask ^ 0xF);
    GP_WB_WRITE_PMUD_VDDRAM_SEL(vddram_sel);
}

/*****************************************************************************
 *                    Bootloader
 *****************************************************************************/

void hal_GoToBootloader(void)
{
    UInt32* addr = (UInt32*) GP_MM_RAM_CRC_START;

    *addr = 0x424f4f54;
    GP_WB_WRITE_PMUD_SOFT_POR_BOOTLOADER(1);
}

/*****************************************************************************
 *                    INTERRUPT
 *****************************************************************************/

INTERRUPT_H void handle_rci_int(void)
{
    hal_IntHandlerPrologue();
    if (HAL_ISR(HAL_ISR_PIO_LP_PRE_ID))
    {
        /* Disable Mac interrupt - since it is a single store, it is atomic */
        GP_WB_WRITE_INT_CTRL_MASK_INT_RCI_INTERRUPT(0);
    }
    if (false == HAL_ISR(HAL_ISR_PIO_LP_POST_ID))
    {
        GP_ASSERT_SYSTEM(false);
    }
}

INTERRUPT_H void handle_phy_int(void)
{
    {
        hal_IntHandlerPrologue();
        if (HAL_ISR(HAL_ISR_PHY_PRE_ID))
        {
            //disable PHY interrupt
            /* Disable PHY interrupt - since it is a single store, it is atomic */
            GP_WB_WRITE_INT_CTRL_MASK_INT_PHY_INTERRUPT(0);
        }
        if (false == HAL_ISR(HAL_ISR_PHY_POST_ID))
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
}

#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
INTERRUPT_H void handleESInt(void)
{
    hal_IntHandlerPrologue();
    if (HAL_ISR(HAL_ISR_ES_LP_PRE_ID))
    {
        /* Disable ES interrupt - since it is a single store, it is atomic */
        GP_WB_WRITE_INT_CTRL_MASK_INT_ES_INTERRUPT(0);
    }
    if (false == HAL_ISR(HAL_ISR_ES_LP_POST_ID))
    {
        GP_ASSERT_SYSTEM(false);
    }
}
#endif

#ifndef HAL_UART_NO_RX
extern void hal_UartRxHandleDMA(void);
#endif
Bool hal_HandleRadioInterrupt(Bool execute)
{
    Bool pending;

    // check if unmasked RCI, unmasked PHY or unmasked ES is == 1 (radio related) interrupt occured
    pending = (GP_WB_READ_INT_CTRL_UNMASKED_RCI_INTERRUPT() ||
               GP_WB_READ_INT_CTRL_UNMASKED_ES_INTERRUPT()  ||
               GP_WB_READ_INT_CTRL_UNMASKED_PHY_INTERRUPT() );

    if (pending && execute)
    {
        gpHal_Interrupt();

        // enable LP PIO interrupt
        GP_WB_WRITE_INT_CTRL_MASK_INT_RCI_INTERRUPT(1);
        // enable PHY interrupt
        GP_WB_WRITE_INT_CTRL_MASK_INT_PHY_INTERRUPT(1);
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
        // enable LP ES interrupt
        GP_WB_WRITE_INT_CTRL_MASK_INT_ES_INTERRUPT(1);
#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
    }

#ifndef HAL_UART_NO_RX
    hal_UartRxHandleDMA();
#endif
    // GP_LOG_SYSTEM_PRINTF("rint %i",0, pending);
    return pending;
}

