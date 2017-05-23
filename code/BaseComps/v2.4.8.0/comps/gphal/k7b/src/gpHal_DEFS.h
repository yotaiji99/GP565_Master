/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpHal_DEFS.h
 *
 *  This file contains internal definitions of the GPHAL.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_DEFS.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_GP_DEFS_H_
#define _HAL_GP_DEFS_H_


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gp_global.h"
#include "gpHal_xap.h"
#include "gpHal_kx_SEC.h"
#include "gpHal_kx_MAC.h"
#include "gpHal_kx_ES.h"
#include "gpHal_kx_DP.h"

//debug: by putting this here we have logging macros available everywhere in gphal
#include "gpLog.h"
#include "gpStat.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------

/** @brief Length of time unit of the symbol counter in us */
#define GP_ES_TIME_UNIT                  (1 << GP_ES_TIME_UNIT_SHIFT) /* 2 us */
#define GP_ES_TIME_UNIT_SHIFT            1

//see also gpHal_EventTypeTXPacket
#define GPHAL_ENUM_VQ_TIMED            GP_WB_ENUM_QTA_VQ_SCH0
#define GPHAL_ENUM_VQ_UNTIMED          GP_WB_ENUM_QTA_VQ_UNTIMED

/* gpHalMac_GetFemTxOptions parameter constants */
#define GPHAL_NO_CW_MODE                0
#define GPHAL_TX_OPTIONS_FOR_TRANSMIT   false
#define GPHAL_TX_OPTIONS_FOR_ACK        true

/* Magic word indicating valid tracking info */
#define GP_HAL_TRACKING_INFO_MAGIC_WORD 0x35774B0C

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/** PbmAddress is the base address of a pbm entry
 *  this macro checks if startOfData points to a byte in the FRAME area of the PBM entry.
 */
#define GP_HAL_IS_VALID_PBM_FRAME_PTR(PbmAddress, startOfData) GP_HAL_IS_VALID_PBM_FRAME_OFFSET((startOfData)-((PbmAddress)+GPHAL_REGISTER_PBM_FORMAT_T_FRAME_0))

#define GP_HAL_IS_DATA_CONFIRM_CALLBACK_REGISTERED()                            gpHal_DataConfirmCallback

#define GP_HAL_CB_DATA_CONFIRM(status, pdLoh, lastChannelUsed)                  gpHal_DataConfirmCallback(status, pdLoh, lastChannelUsed)

#define GP_HAL_IS_DATA_INDICATION_CALLBACK_REGISTERED()                         gpHal_DataIndicationCallback

#define GP_HAL_CB_DATA_INDICATION(pdLoh, rxInfo)                                gpHal_DataIndicationCallback(pdLoh, rxInfo)

#define GP_HAL_IS_ED_CONFIRM_CALLBACK_REGISTERED()                              gpHal_EDConfirmCallback

#define GP_HAL_CB_ED_CONFIRM(channelMask, protoED)                              gpHal_EDConfirmCallback(channelMask, protoED)

#define GP_HAL_IS_EXTERNAL_EVENT_CALLBACK_REGISTERED()                          gpHal_ExternalEventCallback

#define GP_HAL_CB_EXTERNAL_EVENT()                                              gpHal_ExternalEventCallback()

#define GP_HAL_IS_BUSY_TX_CALLBACK_REGISTERED()                                 gpHal_BusyTXCallback

#define GP_HAL_CB_BUSY_TX()                                                     gpHal_BusyTXCallback()

#define GP_HAL_IS_EMPTY_QUEUE_CALLBACK_REGISTERED()                             gpHal_EmptyQueueCallback

#define GP_HAL_CB_EMPTY_QUEUE()                                                 gpHal_EmptyQueueCallback()

#define GP_HAL_IS_CMDDATAREQ_CALLBACK_REGISTERED()                              gpHal_CmdDataReqConfirmCallback

#define GP_HAL_CB_CMDDATAREQ()                                                  gpHal_CmdDataReqConfirmCallback()

#define GP_HAL_IS_LOW_BATTERY_CALLBACK_REGISTERED()                             gpHal_LowBatteryCallback

#define GP_HAL_CB_LOW_BATTERY()                                                 gpHal_LowBatteryCallback()

#define GP_HAL_IS_ABSOLUTE_EVENT_CALLBACK_REGISTERED(interruptIndex)            gpHal_AbsoluteEventCallback[interruptIndex]

#define GP_HAL_CB_ABSOLUTE_EVENT(interruptIndex)                                gpHal_AbsoluteEventCallback[interruptIndex]()

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpHal_InitSleep(void);

//Component dependent ISR macro's
void gpHal_PHYInterrupt(void);

void gpHal_STBCInterrupt(void);

#ifndef GP_HAL_PIO_ISR
void gpHal_RCIInterrupt(void);
#define GP_HAL_RCI_ISR()     gpHal_RCIInterrupt()
#endif

#ifndef GP_HAL_MAC_ISR
void gpHal_MACInterrupt(void);
#define GP_HAL_MAC_ISR()     gpHal_MACInterrupt()
#endif

#ifndef GP_HAL_ES_ISR
#if defined(GP_COMP_GPHAL_ES_EXT_EVENT) || defined(GP_COMP_GPHAL_ES_ABS_EVENT) || defined(GP_COMP_GPHAL_ES_REL_EVENT)
void gpHal_ESInterrupt(void);
#define GP_HAL_ES_ISR()      gpHal_ESInterrupt()
#else
#define GP_HAL_ES_ISR()
#endif
#endif


/** Is it safe to access the radio.
 * This is mainly for debugging purposes.
 * Note that even if this function returns false, the radio can be awake/accesible.
 * This is unpredictable however.
 * Check does not factor in RX status of radio (RxOnWhenIdle needs to be checked in application)
 */
#define GP_HAL_IS_RADIO_ACCESSIBLE() ((gpHal_awakeCounter!=0) || GP_HAL_MAC_STATE_GET_RXON())

typedef struct gpHal_PbmTxOptions_s {
    Bool first_boff_is_0;
    Bool csma_ca_enable;
    Bool treat_csma_fail_as_no_ack;
    UInt8 vq_sel;
    Bool skip_cal_tx;
    UInt8 confirm_queue;
    Bool acked_mode;
    UInt8 antenna;
    Bool fcs_insert_dis;
    Bool stop_rx_window_on_full_pbm;
    Bool ed_scan;
    Bool rx_duration_valid;
    Bool start_rx_only_if_pending_bit_set;
    Bool channel_ch0a_en;
    Bool channel_ch0b_en;
    Bool channel_ch0c_en;
    Bool channel_ch1_en;
    UInt8 channel_idx;
    UInt8 tx_power;
    Bool pa_low;
    Bool pa_ultralow;
    UInt8 mode;
    UInt8 antsel_int;
    UInt8 antsel_ext;
    UInt8 min_be;
    UInt8 max_be;
    UInt8 max_csma_backoffs;
    UInt8 max_frame_retries;
    UInt8 channel_ch0a;
    UInt8 channel_ch0b;
    UInt8 channel_ch0c;
    UInt8 channel_ch1;
    UInt8 rx_duration;
    UInt8 state;
    UInt8 return_code;
    UInt8 frame_ptr;
    UInt8 frame_len;
} gpHal_PbmTxOptions_t;

void gpHalMac_GetFemTxOptions(gpHal_TxPower_t txPower, UInt8 channel, gpHal_PbmTxOptions_t* pTxOptions, Bool forAck, UInt8 CWCorrection);

void gpHal_Scan_EDConfirm(UInt8 channel, UInt8 protoRssi);
void gpHal_InitScan(void);
#endif //_HAL_GP_DEFS_H_
