/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gphal_ISR.c
 *
 *  The file contains the interrupt handling code
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_ISR.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpPd.h"
#include "gpHal.h"
#include "gpHal_DEFS.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"          //Containing all uC dependent implementations
#include "gpHal_reg.h"
#include "gpAssert.h"

#include "gpRxArbiter.h"


#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

//-------------------------------------------------------------------------------------------------------

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


//RF  callbacks
static gpHal_DataConfirmCallback_t          gpHal_DataConfirmCallback       = NULL;
static gpHal_DataIndicationCallback_t       gpHal_DataIndicationCallback    = NULL;

gpHal_EDConfirmCallback_t                   gpHal_EDConfirmCallback         = NULL;
//MAC callbacks
gpHal_BusyTXCallback_t                      gpHal_BusyTXCallback            = NULL;
gpHal_EmptyQueueCallback_t                  gpHal_EmptyQueueCallback        = NULL;
static gpHal_CmdDataReqCallback_t           gpHal_CmdDataReqConfirmCallback = NULL;


#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) && (GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > 0)
static gpHal_AbsoluteEventCallback_t        gpHal_AbsoluteEventCallback[GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS] = { NULL, };
#endif

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
static gpHal_ExternalEventCallback_t        gpHal_ExternalEventCallback = NULL;
#endif

static gpHal_LowBatteryCallback_t           gpHal_LowBatteryCallback = NULL;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
void gpHal_DataConfirmInterrupt(UInt8 PBMentry);


void gpHal_RCIInterrupt(void);
void gpHal_MACInterrupt(void);
void gpHal_PHYInterrupt(void);



#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)

void gpHal_ESInterrupt(void);

#endif //GP_COMP_GPHAL_ES


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


//-------------------------------------------------------------------------------------------------------
//  RCI INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------
/* SW workaround for AD-1044 */
UInt8 LastHandledPBM_Entry = GP_PD_INVALID_HANDLE;

void gpHal_RCIInterrupt(void)
{
    UInt8 PBMentry;
    gpHal_Address_t pbmOptAddress;
    UInt8 rciPending;
    gpHal_RxInfo_t rxInfo;


    GP_STAT_SAMPLE_TIME();

    //Using unmasked version - masks are disabled here in internal version
    rciPending = GP_HAL_READ_REG(GP_WB_RIB_UNMASKED_INT_DATA_IND_ADDRESS) & \
                               ( GP_WB_RIB_UNMASKED_INT_DATA_IND_MASK   |  \
                                 GP_WB_RIB_UNMASKED_INT_DATA_CNF_0_MASK /*Data Cfm*/ | \
                                 GP_WB_RIB_UNMASKED_INT_DATA_CNF_1_MASK /*ED scan*/ );
    GP_ASSERT_SYSTEM(rciPending != 0);

    //GP_LOG_SYSTEM_PRINTF("%x",0,rciPending);

    if( rciPending & GP_WB_RIB_UNMASKED_INT_DATA_CNF_0_MASK)
    {
        PBMentry = GP_WB_READ_RIB_PBETE_DATA_CNF_0();
        GP_ASSERT_SYSTEM(GP_HAL_CHECK_PBM_VALID(PBMentry));
        gpHal_DataConfirmInterrupt(PBMentry);
    }
    else if( rciPending & GP_WB_RIB_UNMASKED_INT_DATA_CNF_1_MASK)
    {
        //Get result
        if(GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED())
        {
            UInt8 ed;
            UInt8 scanChannel;
            PBMentry = GP_WB_READ_RIB_PBETE_DATA_CNF_1();
            GP_ASSERT_SYSTEM(GP_HAL_CHECK_PBM_VALID(PBMentry));
            pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

            scanChannel = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0A(pbmOptAddress);
            ed = GP_WB_READ_PBM_FORMAT_T_ED_RESULT(pbmOptAddress);

            /* PBM is freed here. */
            gpHal_Scan_EDConfirm(scanChannel, ed);
        }
        else
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
    else if( rciPending & GP_WB_RIB_UNMASKED_INT_DATA_CNF_2_MASK)
    {
        GP_ASSERT_SYSTEM(false);  //Not used atm
    }
    else if( rciPending & GP_WB_RIB_UNMASKED_INT_DATA_CNF_3_MASK)
    {
        GP_ASSERT_SYSTEM(false);  //Not used atm
    }
    else if( rciPending & GP_WB_RIB_UNMASKED_INT_DATA_IND_MASK)
    {
        if(GP_HAL_IS_DATA_INDICATION_CALLBACK_REGISTERED())
        {
            gpPd_Loh_t pdLoh;
            UInt8 length, offset  = 0;

            PBMentry = GP_WB_READ_RIB_PBETE_DATA_IND();
/* workaround for AD-1044 */
            // Check if SnifferMode is enabled
            if (1 == GP_WB_READ_MACFILT_ENABLE_SNIFFING())
            {
                UInt8 CurrentPBM_InMac = GP_WB_READ_RIB_PBM_PTR_CONTROL();
                
                if ( (PBMentry | 0x08) == CurrentPBM_InMac )
                {
                    // the PBMentry is still in use by the MAC
                    // so this is an invalid irq : probably Rx was interrupted by a timed Tx
                    LastHandledPBM_Entry = GP_PD_INVALID_HANDLE;

                    return;
                }

                if  (LastHandledPBM_Entry == PBMentry)
                {
                  /* This situation may occur when an invalid irq was followd by a valid irq (on the same PBM)
                     if in that case the sw is slow in handling the RCI irq, the MAC may already have claimed another PBM
                     Note that sw may still be processing the PBM (i.e. PBM not freed yet): GP_HAL_IS_PBM_ALLOCATED is a don't care condition
                  */

                    // If we have another pending irq, then that new irq must be for another PBM: 
                    // so, leave LastHandledPBM_Entry as is for checking during processing of the new (now pending) irq
                    // In other words: only set LastHandledPBM_Entry to invalid when there is no pending irq
                    if (!GP_WB_READ_RIB_UNMASKED_INT_DATA_IND())
                    {
                        LastHandledPBM_Entry = GP_PD_INVALID_HANDLE;
                    }
                    return;
                }
            }

            // check if we have a pending data ind irq:
            if (!GP_WB_READ_RIB_UNMASKED_INT_DATA_IND())
            {
                // no irq is pending
                LastHandledPBM_Entry = GP_PD_INVALID_HANDLE;
            }
            else
            {
                LastHandledPBM_Entry = PBMentry;
            }
/* end workaround for AD-1044 */
            GP_ASSERT_SYSTEM(PBMentry <  GPHAL_NUMBER_OF_PBMS_USED);
            pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
            offset = GP_WB_READ_PBM_FORMAT_R_FRAME_PTR(pbmOptAddress);
            length = GP_WB_READ_PBM_FORMAT_R_FRAME_LEN(pbmOptAddress);
            if (length > GP_RX_PBM_DATA_SIZE)
            {
                length = GP_RX_PBM_DATA_SIZE;
            }
            gpPd_DataIndication(PBMentry, offset, length, &pdLoh);
            if (GP_PD_INVALID_HANDLE != pdLoh.handle)
            {
                rxInfo.rxChannel = GP_WB_READ_PBM_FORMAT_R_CHANNEL(pbmOptAddress)+11;
                GP_HAL_CB_DATA_INDICATION(pdLoh, &rxInfo);
            }
        }
        else
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
    else
    {
        GP_ASSERT_SYSTEM(false);
    }
}

void gpHal_DataConfirmInterrupt(UInt8 PBMentry)
{
    UInt8 ackRequest;
    gpPd_Loh_t pdLoh;
    UInt8 retries;

    gpHal_Address_t pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);

    //Fetch Ack from options
    ackRequest = GP_WB_READ_PBM_FORMAT_T_GP_ACKED_MODE(pbmOptAddress);

    //SW or HW multichannel ? - SW only channel A enabled
    retries = GP_WB_READ_PBM_FORMAT_T_MAX_FRAME_RETRIES(pbmOptAddress);
    if(!ackRequest && retries) //More channels remaining
    {
        //A and B to be looped minimal
        UInt8 channelB = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0B(pbmOptAddress);
        //Move to channel A
        GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A(pbmOptAddress,channelB);

        if(retries > 1) //A and B and C to be looped, move C to B
        {
            UInt8 channelC = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_CH0C(pbmOptAddress);
            //Move to channel B
            GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0B(pbmOptAddress,channelC);
        }
        retries--;
        GP_WB_WRITE_PBM_FORMAT_T_MAX_FRAME_RETRIES(pbmOptAddress, retries);
        gpHal_DataRequest_base(PBMentry);
    }
    else
    {
        if(GP_HAL_IS_DATA_CONFIRM_CALLBACK_REGISTERED())
        {
            UInt8 lastChannelUsed;
            UInt8 offset, result, length;
            gpHal_SourceIdentifier_t srcId;

            //Get result of TX
            result = GP_WB_READ_PBM_FORMAT_T_RETURN_CODE(pbmOptAddress);
            length = GP_WB_READ_PBM_FORMAT_T_FRAME_LEN(pbmOptAddress);
            offset = GP_WB_READ_PBM_FORMAT_T_FRAME_PTR(pbmOptAddress);

            //Fetch the channel the packet was sent on
            srcId = GP_WB_READ_PBM_FORMAT_T_GP_CHANNEL_IDX(pbmOptAddress);
            lastChannelUsed = gpHal_GetRxChannel(srcId);

            gpPd_cbDataConfirm(PBMentry, offset, length, &pdLoh);
            GP_HAL_CB_DATA_CONFIRM(result,pdLoh,lastChannelUsed);
        }
        else
        {
            GP_ASSERT_SYSTEM(false);
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//  PHY INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------

void gpHal_PHYInterrupt(void)
{

    //Restore FLL calibration on active channels
    gpHalMac_RecalibrateChannels();
    //TRX
    if(GP_WB_READ_TRX_UNMASKED_FLL_OUT_OF_LOCK())
    {
        //GP_LOG_SYSTEM_PRINTF("outOfLock",0);
        GP_WB_TRX_FLL_OUT_OF_LOCK_CLR_INT();
    }
    if(GP_WB_READ_TRX_UNMASKED_FLL_CP_OUT_OF_RANGE())
    {
        //GP_LOG_SYSTEM_PRINTF("outOfRange",0);
        GP_WB_TRX_FLL_CP_OUT_OF_RANGE_CLR_INT();
    }
}



//-------------------------------------------------------------------------------------------------------
//  STBC INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------

void gpHal_STBCInterrupt(void)
{
    if(GP_WB_READ_STANDBY_UNMASKED_VLT_STATUS_INTERRUPT())
    {
        GP_WB_STANDBY_CLR_VLT_STATUS_INTERRUPT();
        GP_WB_WRITE_INT_CTRL_MASK_STBC_VLT_STATUS_INTERRUPT(0);
        GP_LOG_SYSTEM_PRINTF("vlt",0);
        if(GP_HAL_IS_LOW_BATTERY_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_LOW_BATTERY();
        }
        else
        {
            GP_ASSERT_DEV_INT(false);           // no low battery callback registered
        }
    }
}

//-------------------------------------------------------------------------------------------------------
//  ES INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)

void gpHal_ESInterrupt(void)
{
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT)

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
    if (GP_WB_READ_INT_CTRL_MASKED_ES_EXTERNAL_EVENT_INTERRUPT())
    {
        GP_WB_ES_CLR_EXTERNAL_EVENT_INTERRUPT();   //Clear interrupt
        if(GP_HAL_IS_EXTERNAL_EVENT_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_EXTERNAL_EVENT();
        }
        return;
    }
#endif //GP_COMP_GPHAL_ES_EXT_EVENT

    //No relative event may be pending
    GP_ASSERT_SYSTEM(0 == GP_WB_READ_INT_CTRL_MASKED_ES_RELATIVE_EVENT_INTERRUPT());
#else
    //Check
    GP_ASSERT_SYSTEM(0 == GP_WB_READ_INT_CTRL_MASKED_ES_EXTERNAL_EVENT_INTERRUPT());
#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT)

#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) && (GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > 0)
    {
    UInt16 maskedEventInterrupts      = 0;
    UInt16 maskedEventInterruptsCheck = 0;
    UInt8  interruptIndex             = 0;

    //Look for Absolute interrupts active
    maskedEventInterrupts = GP_WB_READ_ES_UNMASKED_EVENT_INTERRUPTS();

    //Minimum one absolute event should be triggered - relative + external events are no longer a source of the ES interrupt
    GP_ASSERT_SYSTEM(maskedEventInterrupts != 0);

    for (interruptIndex = 0; interruptIndex <  GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS; interruptIndex++)
    {
        maskedEventInterruptsCheck = ((UInt16) 1 << interruptIndex);

        if ((maskedEventInterrupts & maskedEventInterruptsCheck) == 0)
            continue;

        if (gpHal_AbsoluteEventCallback[interruptIndex])
        {
            gpHal_AbsoluteEventCallback[interruptIndex]();
        }
        // Clear interrupt - calculation cost is larger then a direct 16-bit access
        GP_WB_WRITE_ES_CLR_EVENT_INTERRUPTS(maskedEventInterruptsCheck);

        // Stop handling event interrupts to give possible PIO interrupt priority
        break;
    }
    }
#else
    //All the interrupts should be serviced by now.
    //If no absolute events are used, the ES interrupt should be handled by relative+external code.
    GP_ASSERT_SYSTEM(false);
#endif //GP_COMP_GPHAL_ES_ABS_EVENT
}

#endif //defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  REGISTER CALLBACK FUNCTIONS
//-------------------------------------------------------------------------------------------------------



//RF register callbacks
void gpHal_RegisterDataConfirmCallback(gpHal_DataConfirmCallback_t callback)
{
    gpHal_DataConfirmCallback=callback;
}

void gpHal_RegisterDataIndicationCallback(gpHal_DataIndicationCallback_t callback)
{
    gpHal_DataIndicationCallback=callback;
}

void gpHal_RegisterEDConfirmCallback(gpHal_EDConfirmCallback_t callback)
{
    gpHal_EDConfirmCallback=callback;
}

//MAC callbacks
void gpHal_RegisterBusyTXCallback(gpHal_BusyTXCallback_t callback)
{
    gpHal_BusyTXCallback=callback;
}

void gpHal_RegisterEmptyQueueCallback(gpHal_EmptyQueueCallback_t callback)
{
    gpHal_EmptyQueueCallback=callback;
}

void gpHal_RegisterCmdDataReqConfirmCallback(gpHal_CmdDataReqCallback_t callback)
{
    gpHal_CmdDataReqConfirmCallback = callback;
}




//ES register callbacks
#if defined(GP_COMP_GPHAL_ES_ABS_EVENT) && (GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS > 0)
void gpHal_RegisterAbsoluteEventCallback(gpHal_AbsoluteEventCallback_t callback, UInt8 eventNbr)
{
    GP_ASSERT_DEV_EXT(eventNbr < GP_HAL_ES_ABS_EVENT_NMBR_OF_EVENTS);
    gpHal_AbsoluteEventCallback[eventNbr]=callback;
}

#endif //GP_COMP_GPHAL_ES_ABS_EVENT

#ifdef GP_COMP_GPHAL_ES_EXT_EVENT
gpHal_ExternalEventCallback_t gpHal_RegisterExternalEventCallback(gpHal_ExternalEventCallback_t callback)
{
    gpHal_ExternalEventCallback_t oldCb = gpHal_ExternalEventCallback;

    gpHal_ExternalEventCallback = callback;

    return oldCb;
}
#endif //GP_COMP_GPHAL_ES_EXT_EVENT

void gpHal_RegisterLowBatteryEventCallback(gpHal_LowBatteryCallback_t callback)
{
    gpHal_LowBatteryCallback = callback;
}

//-------------------------------------------------------------------------------------------------------
//  MAC INTERRUPT HANDLER
//-------------------------------------------------------------------------------------------------------
void gpHal_MACInterrupt(void)
{
    GP_ASSERT_SYSTEM(false); //Initial copy - SW-2025
#if 0
    //we check queue 0,
    //so check gphal indeed uses VQ_SCHED0 to transmit packets
    UInt8 u8;
    COMPILE_TIME_ASSERT(GPHAL_ENUM_VQ_TIMED == GPHAL_ENUM_VQ_SCHED0);
    u8 = GP_HAL_READ_REG(GPHAL_REGISTER_RIB_MASKED_TRC_INT_0);
    if (GP_HAL_READ_PROPFROM(u8, GPHAL_REGISTER_RIB_MASKED_TRC_INT_0,
                      GPHAL_PROP_RIB_MASKED_SCH0_TRG_TX_BUSY))
    {
        if(GP_HAL_IS_BUSY_TX_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_BUSY_TX();
        }
        GP_WB_WRITE_RIB_CLEAR_SCH0_TRG_TX_BUSY(true);

    }
    if (GP_HAL_READ_PROPFROM(u8, GPHAL_REGISTER_RIB_MASKED_TRC_INT_0,
                      GPHAL_PROP_RIB_MASKED_QTA_SCH0_EMPTY))
    {
        if(GP_HAL_IS_EMPTY_QUEUE_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_EMPTY_QUEUE();
        }
        GP_WB_WRITE_RIB_CLEAR_QTA_SCH0_EMPTY(true);
    }
#endif
}

