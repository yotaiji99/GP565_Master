/*
 * Copyright (c) 2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpKeyScan/src/gpKeyScan_xap_k7b.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_KEYSCAN

//#define GP_LOCAL_LOG
#include "hal.h"
#include "gpHal.h"
#include "gpHal_Debug.h"

#include "gpAssert.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpKeyScan.h"
#include "gpController_Main.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//#define KEYSCAN_DEBUG_INDICATE_ISR

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static Bool  gpKeyScan_Illegal;
static Bool  gpKeyScan_ScanmodeInterrupt;
static UInt8 gpKeyScan_Stable;
static UInt8 gpKeyScan_NumberOfKeys;
static UInt8 gpKeyScan_Status[8];
static Bool  gpKeyScan_LowVoltage;
static gpKeyScan_cbCheckLowVoltage_t  gpKeyScan_cbCheckLowVoltage;
static gpKeyScan_cbStuckKeyNotification_t  gpKeyScan_cbStuckKeyNotification;
volatile Bool gpKeyScan_StuckKeyHandling;
static gpHal_AbsoluteEventId_t gpKeyScan_EventId;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

//Board specific functions
ALWAYS_INLINE void KeyScan_SetWakeupMode(UInt8 mode);
static        void KeyScan_Init(void);
static        void KeyScan_GenerateScanIndication(UInt8* pStatus);
static        Bool KeyScan_WasIllegal(void);

//Include board specific parts
#include "gpKeyScan_P320_GP565_SDK_40pin_v1_00.c"

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*
 * basic working methode of this keyboard scan:
 * there are two main states: polled and interrupt driven.
 * in interrupt driven, we use the strong pull downs on outputs and the pull ups on the inputs + edge detection + external event.
 * the external event simply triggers a keyboard scan.
 * this is also the sleep state.
 * as soon as a key is processed, this is handled and we switch to polled mode.
 * in polled mode, the pull downs + edge detection are disabled and the keyboard is sampled periodically using an absolute event.
 * if the keyboard scan reveils there is no key pressed, we return to interrupt driven mode
 *
 * the basic keyboard interrupt does not do a lot: it simply disabled the interrupt and schedules a bottom half handler to do the real work
 * this bottom half handler will interpret the keyboard scan result + re-enable the interrupt.
 *
 * the same for the external event handler: it schedules the keyboard scan and disables the external event.
 * both can only be reenabled by the scheduled function, after reading out the result
 *
 * This is to avoid having a new keyboard scan invalidating the registers the bottom half handler is reading out.
 * Great care is taken to detect this (UPDATED property)
 *
 *
 */

#define DEBOUNCE_MS 2
#define SCAN_INTERVAL 25

static inline UInt8 KeyScan_GetKeypadSenses(void)
{
    return GP_WB_READ_KEYPAD_SCAN_KEYPAD_SENSES();
}

void KeyScan_ScheduleScan(UInt16 when)
{
    gpHal_AbsoluteEventDescriptor_t ev;
    gpHal_EventState_t state;

    GP_LOG_PRINTF("sch kbs %i",2,when);
    state = gpHal_UnscheduleAbsoluteEvent(gpKeyScan_EventId);

    if (!(state == gpHal_EventStateDone || state == gpHal_EventStateInvalid))
    {
        GP_LOG_PRINTF("****** ev state %i",2, state);
    }

    //Initialize event
    memset(&ev, '\0', sizeof(ev));
    ev.type = GP_WB_ENUM_EVENT_TYPE_KEYSCAN_START;
    /* we need ex_itl when waking up very fast*/
    ev.executionOptions = GP_ES_EXECUTION_OPTIONS_EXECUTE_IF_TOO_LATE;
    if (when != 0)
    {
        gpHal_GetTime(&ev.exTime);
        ev.exTime+=MS_TO_US(when); //in us
        GP_ES_SET_EVENT_STATE(ev.control, gpHal_EventStateScheduled);
    }
    else
    {
        GP_ES_SET_EVENT_STATE(ev.control, gpHal_EventStateScheduledForImmediate);
    }
    gpHal_ScheduleAbsoluteEvent(&ev, gpKeyScan_EventId);
}

static void KeyScan_Int2Poll(void)
{
    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(false);

    KeyScan_SetWakeupMode(GP_WB_ENUM_WAKEUP_PIN_MODE_NO_EDGE);

    GP_WB_ES_TRIGGER_FAST_SYNC();
    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(true);

    //even if we don't go to deep sleep we don't want to drive the outputs
    GP_WB_WRITE_KEYPAD_SCAN_SENSITIVE_DURING_ACTIVE(false);

    //KeyScan operating in polled mode now
    gpKeyScan_ScanmodeInterrupt = false;
    GP_LOG_PRINTF("int2poll",0);
}

void KeyScan_PollOrInt(UInt8 timeout)
{
    UInt8 keyp_senses;

   /* only this function has a race condition with the external event interrupt. we need to protect it.*/
    HAL_DISABLE_GLOBAL_INT();

    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(false);

    //even if we don't go to deep sleep we do want to drive the keyboard output pins to get an ExtEventInterrupt
    //we have to do it before  to avoid interrupt from falling edge if a key is being pressed
    GP_WB_WRITE_KEYPAD_SCAN_SENSITIVE_DURING_ACTIVE(true);

    KeyScan_SetWakeupMode(GP_WB_ENUM_WAKEUP_PIN_MODE_FALLING_EDGE);

    //put zeros on keyscan outputs
    GP_WB_WRITE_ES_EXTERNAL_EVENT_VALID(true);

    //Keyscan waiting for interrupt again
    gpKeyScan_ScanmodeInterrupt = true;
//    GP_LOG_SYSTEM_PRINTF("KeyScan_PollOrInt",0); /* grn LED on */

    //Check keys before finishing this interrupt mode initialization
    keyp_senses = (gpKeyScan_LowVoltage || gpKeyScan_StuckKeyHandling) ? 0xFF : KeyScan_GetKeypadSenses();

    if (keyp_senses==0xFF)
    {
        //Normal case
        //No keys pressed- interrupts enabled
        HAL_ENABLE_GLOBAL_INT();
        return;
    }

    //Key pressed while setting up interrupt mode, back to polled mode
    GP_LOG_PRINTF("*** senses: %x",2,keyp_senses);
    KeyScan_Int2Poll();
    KeyScan_ScheduleScan(timeout);
    HAL_ENABLE_GLOBAL_INT();
}

static void KeyScan_SetStuckKeyHandling(Bool keyIsStuck)
{
    if(gpKeyScan_StuckKeyHandling != keyIsStuck)
    {
        gpKeyScan_StuckKeyHandling = keyIsStuck;
        if (gpKeyScan_cbStuckKeyNotification != NULL)
        {
            gpKeyScan_cbStuckKeyNotification(gpKeyScan_StuckKeyHandling);
        }
    }
}

static void KeyScan_HandleKeyChange(void)
{
    //we keep the actual scan callbacks at the very end
    //so there is no harm if the callback one of our own functions like Enable/Disable.
    //if callback calls TriggerScan, this will yield trouble.
    if (gpKeyScan_NumberOfKeys == 0)
    {
         KeyScan_PollOrInt(0);
        //now interrupts are (probably) enabled, so a keyscan can happen at any moment under our feet
        //so we can't use keypad registers anymore.
        //let's generate an scan indication right away.
        //maybe it would be easier to swschedule a keyscan from ext event.
        //and poll for result
        gpKeyScan_cbScanIndication(NULL);
    }
    else
    {
        if (gpKeyScan_Stable==1)
        {
            //gpLog_PrintBuffer(8, gpKeyScan_Status);
            //Generate indication with correct keyboard - board specific
            KeyScan_GenerateScanIndication(&(gpKeyScan_Status[0]));
        }
        KeyScan_ScheduleScan(SCAN_INTERVAL);
    }
}

void KeyScan_GetKeyStatus(void)
{
    Bool changed = false;

    if(gpKeyScan_cbCheckLowVoltage != NULL)
    {
        gpKeyScan_LowVoltage = gpKeyScan_cbCheckLowVoltage(); //cb decides
    }
    else
    {
        gpKeyScan_LowVoltage = GP_WB_READ_STANDBY_VLT_STATUS(); //Below 1.8V
    }

    if (gpKeyScan_LowVoltage || gpKeyScan_StuckKeyHandling)
    {
        //simulating empty keyscan
        gpKeyScan_NumberOfKeys = 0;
        MEMSET(&(gpKeyScan_Status[0]),0x0,8);

        //GP_LOG_SYSTEM_PRINTF("enf empty ks %x %x",4,(UInt16)gpKeyScan_LowVoltage,(UInt16)gpKeyScan_StuckKeyHandling);

        //update stable status
        gpKeyScan_Stable = 0;
        gpKeyScan_Illegal = false;

        return;
    }

    do
    {
        GP_WB_KEYPAD_SCAN_CLEAR_RESULTS_UPDATED();

        if (GP_WB_READ_KEYPAD_SCAN_RESULTS_CHANGED())
        {
            GP_WB_KEYPAD_SCAN_CLEAR_RESULTS_CHANGED();
            changed = true;
        }

        gpKeyScan_NumberOfKeys = GP_WB_READ_KEYPAD_SCAN_NUMBER_OF_KEYS();
        GP_HAL_READ_BYTE_STREAM(GP_WB_KEYPAD_SCAN_KEY_VECTOR_0_ADDRESS, &(gpKeyScan_Status[0]), 8);

#if 0
        if (changed) GP_LOG_SYSTEM_PRINTF(" %i %x %x %x %x %x %x %x %x", 18, gpKeyScan_NumberOfKeys, gpKeyScan_Status[0], gpKeyScan_Status[1], gpKeyScan_Status[2], gpKeyScan_Status[3], gpKeyScan_Status[4], gpKeyScan_Status[5], gpKeyScan_Status[6], gpKeyScan_Status[7]);
#endif
    } while(GP_WB_READ_KEYPAD_SCAN_RESULTS_UPDATED());

    //update stable status
    if (changed)
    {
        gpKeyScan_Stable = 0;
    }
    else
    {
        //increment sw stable with saturation at 2
        if (gpKeyScan_Stable < 2)
        {
            gpKeyScan_Stable++;
        }
    }
    gpKeyScan_Illegal = KeyScan_WasIllegal();
}

INTERRUPT_H void handleKeypInt(void)
{
    hal_IntHandlerPrologue();

    //Disable interrupt
    HAL_DISABLE_GLOBAL_INT();
    GP_WB_WRITE_INT_CTRL_MASK_KPS_INT_INTERRUPT(false);
    HAL_ENABLE_GLOBAL_INT();

    //process key interrupt
//    GP_LOG_SYSTEM_PRINTF("process key interrupt",0); /* grn LED on */

    //Clear interrrupt flag
    GP_WB_KEYPAD_SCAN_CLEAR_KEYPAD_INTERRUPT();
    if (GP_WB_READ_KEYPAD_SCAN_RESULTS_UPDATED() == 0)
    {
        GP_LOG_PRINTF("****1",0);
        goto exit;
    }

    KeyScan_GetKeyStatus();

    if (gpKeyScan_Illegal)
    {
        //Scan keypad again, since scan was illegal (for example, because it shares pins with the TWI block, and the latter was active)
        KeyScan_ScheduleScan(SCAN_INTERVAL); //kind of debounce, but with the longer SCAN_INTERVAL
    }
    else if (gpKeyScan_LowVoltage || gpKeyScan_StuckKeyHandling || gpKeyScan_Stable==1 || (gpKeyScan_Stable==2 && gpKeyScan_NumberOfKeys==0))
    {
        //(gpKeyScan_Stable==2 && gpKeyScan_NumberOfKeys==0) is needed to cover the case of an "empty keyboard scan" right after the external event

        //Event should not be scheduled anymore
//        GP_ASSERT_DEV_EXT( !gpSched_UnscheduleEvent(KeyScan_HandleKeyChange));      // not sure ==> DEV_EXT?
        //Schedule interrupt handling
        gpSched_ScheduleEvent(0, KeyScan_HandleKeyChange);
    }
    else if (gpKeyScan_Stable==2)
    {
        KeyScan_ScheduleScan(SCAN_INTERVAL);
    }
    else
    {
        //Scan keypad again
        KeyScan_ScheduleScan(DEBOUNCE_MS);
    }

exit:
    //Re-enable interrupt
    HAL_DISABLE_GLOBAL_INT();
    GP_WB_WRITE_INT_CTRL_MASK_KPS_INT_INTERRUPT(true);
    HAL_ENABLE_GLOBAL_INT();

}

/*****************************************************************************
 *                    gpHal callback
 *****************************************************************************/

void gpHal_cbExternalEvent(void)
{
    if( (gpKeyScan_ScanmodeInterrupt == true) && (KeyScan_GetKeypadSenses() != 0xFF) )
    {
        KeyScan_SetStuckKeyHandling(false);

        //first wakeup after (deep) sleep, some work to do...
        //switch i/o to polling mode
        KeyScan_Int2Poll();

        GP_LOG_PRINTF("ee",0);

        gpHal_ScheduleImmediateEvent(GP_WB_ENUM_EVENT_TYPE_KEYSCAN_START);
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpKeyScan_Init(void)
{
    gpKeyScan_Stable = 0;

    //Perform board specific initialization
    KeyScan_Init();

    gpKeyScan_EventId = gpHal_GetAbsoluteEvent();

    //Common initialization
    gpKeyScan_cbCheckLowVoltage = NULL;
    gpKeyScan_cbStuckKeyNotification = NULL;

    //Keypad settings
    GP_WB_WRITE_KEYPAD_SCAN_ENABLE_CLK_KEYPAD_BY_UC(true);
    //GP_WB_WRITE_KEYPAD_SCAN_ACTIVE_COLUMNS_CHARGE_PERIOD(10);
    GP_WB_WRITE_KEYPAD_SCAN_SENSITIVE_DURING_ACTIVE(true);

    GP_WB_WRITE_KEYPAD_SCAN_KEYPAD_ENABLE(true);

    //Setup external interrupt
    GP_WB_ES_CLR_EXTERNAL_EVENT_INTERRUPT();
    //GP_WB_WRITE_ES_EXTERNAL_EVENT_TYPE_TO_BE_EXECUTED(GP_WB_ENUM_EVENT_TYPE_KEYSCAN_START);
    GP_WB_WRITE_ES_EXTERNAL_EVENT_TYPE_TO_BE_EXECUTED(GP_WB_ENUM_EVENT_TYPE_DUMMY);
    gpHal_RegisterExternalEventCallback(gpHal_cbExternalEvent);
    GP_WB_WRITE_INT_CTRL_MASK_ES_EXTERNAL_EVENT_INTERRUPT(true);
    //Keypad interrupt setup
    GP_WB_WRITE_INT_CTRL_MASK_INT_KPS_INTERRUPT(true);
    GP_WB_WRITE_INT_CTRL_MASK_KPS_INT_INTERRUPT(true);
    GP_WB_WRITE_XAP5_GP_KPS_INT_PRIORITY(0x03); //marked it as a long interrupt (low priority)

    /* this will make us wake up faster from deep sleep...*/
    GP_WB_WRITE_ES_STARTUP_SYMBOL_TIME(0);
    /* this will make us wake up ...*/
    GP_WB_WRITE_ES_LONG_STARTUP_SYMBOL_TIME(0x800);
    GP_WB_ES_APPLY_STARTUP_SYMBOL_TIME();
    GP_WB_WRITE_ES_FAST_SYNC_ON_START(true);

    //Start out in polled situation - no interrupts enabled - no falling edge sensing
    KeyScan_Int2Poll();
}

/*
 * Disclaimer:
 * The public interface functions from below should not be called from interrupt handlers!!
 */
void gpKeyScan_DisableAutomaticKeyScanning(void)
{
    //we leave keypad interrupts enabled,
    //but disable the sources of the keyboard scan: edge detection + polling
    gpHal_UnscheduleAbsoluteEvent(gpKeyScan_EventId);
    KeyScan_Int2Poll();
    //if we leave this function, there is a small chance
    //a keyboard scan is executing in HW
    //or a ScanConfirm is still scheduled (gpSched)
}
void gpKeyScan_EnableAutomaticKeyScanning(void)
{
    //Switch to interrupt mode - sensing on pins enabled
    KeyScan_PollOrInt(10);
}

void gpKeyScan_HandleStuckKey(void)
{
    //Power optimal solution
    KeyScan_SetStuckKeyHandling(true);

    //Power hungry solution
    //gpKeyScan_cbScanIndication(NULL);
}

void gpKeyScan_RegisterCheckLowVoltageCallback(gpKeyScan_cbCheckLowVoltage_t callback)
{
    //GP_LOG_SYSTEM_PRINTF("clv cb: %x",2,(UInt16)callback);
    gpKeyScan_cbCheckLowVoltage = callback;
}

void gpKeyScan_RegisterCheckStuckKeyCallback(gpKeyScan_cbStuckKeyNotification_t callback)
{
    //GP_LOG_SYSTEM_PRINTF("skn cb: %x",2,(UInt16)callback);
    gpKeyScan_cbStuckKeyNotification = callback;
}

