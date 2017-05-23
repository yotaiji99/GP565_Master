/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *   This file contains the implementation of the MAC scan functions
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_Scan.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_MAC.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

//Debug
#include "hal.h"
#include "gpBsp.h"
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#define GP_HAL_SYMBOL_DURATION 16
#define GP_HAL_NUMBER_OF_CHANNELS 16
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
typedef struct gpHal_channelScan {
    UInt16 todoChannels;
    UInt16 scanMask;
    UInt8 pbmEdScan;
    UInt8 protoRssiLst[GP_HAL_NUMBER_OF_CHANNELS];
} gpHal_channelScan_t;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
extern gpHal_EDConfirmCallback_t gpHal_EDConfirmCallback;
static gpHal_channelScan_t gpHal_channelScan;   
static void gpHal_doEmptyEDConfirm(void)
{
    if (GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED())
    {
        GP_HAL_CB_ED_CONFIRM(0, gpHal_channelScan.protoRssiLst);
    }
}

void gpHal_InitScan(void)
{
    gpHal_channelScan.todoChannels = 0;
    gpHal_channelScan.scanMask = 0;
    gpHal_channelScan.pbmEdScan = GP_PBM_INVALID_HANDLE;
}

#if 1
static void rap_format_t_reset_options(UInt16 optsbase)
{
    UInt8 u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_FIRST_BOFF_IS_0_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CSMA_CA_ENABLE_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_TREAT_CSMA_FAIL_AS_NO_ACK_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_VQ_SEL_TO_GP_FLOW_CTRL_0(u8, GP_WB_ENUM_PBM_VQ_UNTIMED);
    GP_WB_SET_PBM_FORMAT_T_GP_SKIP_CAL_TX_TO_GP_FLOW_CTRL_0(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_0(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_CONFIRM_QUEUE_TO_GP_FLOW_CTRL_1(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_ACKED_MODE_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_ANTENNA_TO_GP_FLOW_CTRL_1(u8, GP_WB_READ_RX_ANT_USE());
    GP_WB_SET_PBM_FORMAT_T_GP_FCS_INSERT_DIS_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_STOP_RX_WINDOW_ON_PBM_FULL_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_ED_SCAN_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_RX_DURATION_VALID_TO_GP_FLOW_CTRL_1(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_1(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_IDX_TO_GP_FLOW_CTRL_2(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0B_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH0C_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_CHANNEL_CH1_EN_TO_GP_FLOW_CTRL_2(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FLOW_CTRL_2(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_MODE_CTRL_TO_EXTERNAL_PA_FEM_SETTINGS(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_ANTSEL_INT_TO_EXTERNAL_PA_FEM_SETTINGS(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_ANTSEL_EXT_TO_EXTERNAL_PA_FEM_SETTINGS(u8, 0);
    GP_WB_WRITE_PBM_FORMAT_T_EXTERNAL_PA_FEM_SETTINGS(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_GP_COEX_PACKET_TX_PRIO_TO_GP_COEX_OPTIONS(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_COEX_PACKET_TX_REQ_EN_TO_GP_COEX_OPTIONS(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_COEX_ACK_RX_PRIO_TO_GP_COEX_OPTIONS(u8, 0);
    GP_WB_SET_PBM_FORMAT_T_GP_COEX_ACK_RX_REQ_EN_TO_GP_COEX_OPTIONS(u8, false);
    GP_WB_SET_PBM_FORMAT_T_GP_COEX_GRANT_AWARE_TO_GP_COEX_OPTIONS(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_COEX_OPTIONS(optsbase, u8);
    u8 = 0;
    GP_WB_SET_PBM_FORMAT_T_TX_POWER_TO_PA_POWER_SETTINGS(u8, 0); // +/-0dBm (+1 being added by channel compesation)
    GP_WB_SET_PBM_FORMAT_T_PA_LOW_TO_PA_POWER_SETTINGS(u8, false);
    GP_WB_SET_PBM_FORMAT_T_PA_ULTRALOW_TO_PA_POWER_SETTINGS(u8, false);
    GP_WB_WRITE_PBM_FORMAT_T_PA_POWER_SETTINGS(optsbase, u8);

    GP_WB_WRITE_PBM_FORMAT_T_STATE(optsbase, 7);    //only for test and debug
    GP_WB_WRITE_PBM_FORMAT_T_RETURN_CODE(optsbase, 255);    //only for test and debug
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_PTR(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(optsbase, 0);
}
static void gpHal_EDQueueNext(void)
{
    UInt16 channel;
    gpHal_Address_t pbmOtpAddress;

    /* Get the next channel to be scanned */
    for (channel = 0; channel < GP_HAL_NUMBER_OF_CHANNELS; channel++)
    {
        if(BIT_TST(gpHal_channelScan.todoChannels, channel))
        {
            break;
        }
    }
    pbmOtpAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(gpHal_channelScan.pbmEdScan);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A(pbmOtpAddress, channel);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN(pbmOtpAddress, 1);
    GP_WB_WRITE_QTA_PBEFE_DATA_REQ(gpHal_channelScan.pbmEdScan);
}
void gpHal_Scan_EDConfirm(UInt8 channel, UInt8 protoRssi)
{
    gpHal_channelScan.protoRssiLst[channel] = protoRssi;        // store the result for previously scanned channel
    GP_ASSERT_SYSTEM(gpHal_channelScan.todoChannels);
    BIT_SET(gpHal_channelScan.scanMask, channel); // mark a scanned channel
    BIT_CLR(gpHal_channelScan.todoChannels, channel); // and remove it from the list of channels to do (still)
    if (gpHal_channelScan.todoChannels)
    {
        gpHal_EDQueueNext(); 
    }
    else
    {
        gpHal_FreeHandle(gpHal_channelScan.pbmEdScan);
        gpHal_channelScan.pbmEdScan = GP_PBM_INVALID_HANDLE;
        if (GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED())
        {
            GP_HAL_CB_ED_CONFIRM(gpHal_channelScan.scanMask, gpHal_channelScan.protoRssiLst);
        }
        MEMSET(gpHal_channelScan.protoRssiLst, 0, sizeof(gpHal_channelScan.protoRssiLst));
        gpHal_channelScan.scanMask = 0;
        gpHal_channelScan.pbmEdScan = GP_PBM_INVALID_HANDLE;
    }
}
#endif
//-------------------------------------------------------------------------------------------------------
//  MAC ED REQUEST FUNCTION
//-------------------------------------------------------------------------------------------------------
gpHal_Result_t gpHal_EDRequest(UInt32 time_us , UInt16 channelMask )
{
    gpHal_Address_t pbmOtpAddress;

    if (gpHal_channelScan.todoChannels)   // still busy while new request is made ?
    {
        return gpHal_ResultBusy;
    }

    if (!channelMask) // if 0 channels to scan we schedule empty event, so that the expected flow still applies for the caller
    {
        gpHal_doEmptyEDConfirm();
        return gpHal_ResultSuccess;
    }
    gpHal_channelScan.todoChannels = channelMask;
    gpHal_channelScan.scanMask = 0;
    gpHal_channelScan.pbmEdScan = GP_PBM_INVALID_HANDLE;
    MEMSET(gpHal_channelScan.protoRssiLst, 0, sizeof(gpHal_channelScan.protoRssiLst));
    GP_ASSERT_DEV_INT(!GP_HAL_CHECK_PBM_VALID(gpHal_channelScan.pbmEdScan));
    gpHal_channelScan.pbmEdScan = gpHal_GetHandle();
    if(!GP_HAL_CHECK_PBM_VALID(gpHal_channelScan.pbmEdScan))
    {
        return gpHal_ResultBusy;
    }
    pbmOtpAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(gpHal_channelScan.pbmEdScan);
    rap_format_t_reset_options(pbmOtpAddress);

    // explicitely enable the untimed virtual queue
    GP_WB_WRITE_PBM_FORMAT_T_GP_VQ_SEL(pbmOtpAddress, GP_WB_ENUM_PBM_VQ_UNTIMED);


    GP_WB_WRITE_PBM_FORMAT_T_GP_ED_SCAN(pbmOtpAddress, true);

    if (time_us < 144)
    {
        time_us = 144;
    }
    time_us -= 16;

    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION(pbmOtpAddress,time_us >> 4);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION_VALID(pbmOtpAddress, true);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CONFIRM_QUEUE(pbmOtpAddress,1);         //RCI int Confirm 1

    //Queue the frame
    gpHal_EDQueueNext();          // will start the loop over all channels that must be scanned

    return gpHal_ResultSuccess;
}

UInt8 gpHal_CalculateED( UInt8 protoED)
{
    UInt16 result;
    if(protoED <=  72)
    {
        result = 0;
    }
    else if( protoED<200)
    {
        result = (((UInt16)protoED)<<1)-((UInt16)144);
    }
    else
    {
        result = 0xFF;
    }
    return (UInt8)result;
}

