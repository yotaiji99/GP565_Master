/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpHal_MAC.c
 *   This file contains the implementation of the MAC functions
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_MAC.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */



/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpPd.h"

#define COMPLETE_MAP
#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_MAC.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

//Debug
#include "hal.h"
#include "gpBsp.h"
#include "gpAssert.h"
#include "gpLog.h"


#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

#if defined(GP_BSP_MODE_CTRL_MODE) || defined(GP_BSP_MODE_CTRL_TXEN) || defined(GP_BSP_MODE_CTRL_RXEN)
#define GP_HAL_DIVERSITY_EXT_MODE_SIGNALS
#endif //defined(GP_BSP_MODE_CTRL_MODE) || defined(GP_BSP_MODE_CTRL_TXEN) || defined(GP_BSP_MODE_CTRL_RXEN)

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_HAL_CALCULATE_START_PBM_ADDRESS(absPBMAddress)           ((absPBMAddress) - ((absPBMAddress) % GP_MM_RAM_PBM_0_DATA_SIZE)) /*per 0x80*/
#define GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT(relAddr)           ((relAddr) & 0x7F)

#define GPHAL_DEFAULT_TRANSMIT_POWER   0

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

#define WRITE_PACKET_IN_PBM(address,pPacket,length)                                                  \
    do {                                                                                             \
        GP_HAL_WRITE_PROP_OFFSET(address,  GPHAL_PROP_PBM_FORMAT_T_FRAME_LEN, length);           /* length of payload + header */ \
        GP_HAL_WRITE_DATA_IN_PBM(address, pPacket, length, 0);                                       \
    } while (false)

#define INIT_PBM_TX(address, qta_options, dataOffset)                   \
    do {                                                                \
        GP_HAL_WRITE_REG((address)+GPHAL_PROP_PBM_FORMAT_T_FRAME_PTR_REGISTER,dataOffset); \
        GP_HAL_CHECK_FRAME_PTR_ONLY_ACTIVE_PROP_IN_REG();               \
        GP_HAL_WRITE_REG((address) + GPHAL_REGISTER_PBM_FORMAT_T_GP_QTA_OPTIONS, qta_options);/* Write QTA options */ \
    } while (false)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

//Transmit power can vary for different channels [11-26]
Int8 gpHal_pDefaultTransmitPowerTable[16] = {
    GPHAL_DEFAULT_TRANSMIT_POWER, //Channel 11
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER,
    GPHAL_DEFAULT_TRANSMIT_POWER  //Channel 26
};
#define GET_DEFAULT_TX_POWER(ch) gpHal_pDefaultTransmitPowerTable[ch-11]

//Tx output Min/Max
#define GP_HAL_PA_MIN   (-22) //TODO: To reach this we need to disable one branch of the PA
#define GP_HAL_PA_MAX   (+5)
UInt8 gpHal_MacState=0x0;

//Default set - keeping only for legacy set/gets - to be removed ?
static const ROM gpHal_PbmTxOptions_t gpHalMac_PbmTxOptionsDefault FLASH_PROGMEM =
{
    false,      //first_boff_is_0
    false,      //csma_ca_enable
    true,       //treat_csma_fail_as_no_ack
    GP_WB_ENUM_PBM_VQ_UNTIMED, //vq_sel
    false,      //skip_cal_tx
    0,          //confirm_queue
    false,      //acked_mode
    0,          //antenna;
    false,      //fcs_insert_dis
    false,      //stop_rx_window_on_full_pbm
    false,      //ed_scan
    false,      //rx_duration_valid
    false,      //start_rx_only_if_pending_bit_set
    false,      //channel_ch0a_en
    false,      //channel_ch0b_en
    false,      //channel_ch0c_en
    false,      //channel_ch1_en
    0,          //channel_idx - only 0 used for now
    12,         //tx_power
    false,      //pa_low
    false,      //pa_ultralow
    0x0,        //mode
    GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN, //antsel_int
    GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_0,       //antsel_ext
    0x3,        //min_be
    0x5,        //max_be
    0x4,        //max_csma_backoffs;
    0x3,        //max_frame_retries;
    20-11,         //channel_ch0a;
    20-11,         //channel_ch0b;
    20-11,         //channel_ch0c;
    20-11,         //channel_ch1;
    0,          //rx_duration;
    7,          //state, ONLY DEBUG
    255,        //return_code, ONLY DEBUG
    0,          //frame_ptr
    0,          //frame_len
};

#ifndef GP_BSP_RX_ANTSEL_DEFAULT
#define GP_BSP_RX_ANTSEL_DEFAULT GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN,GP_WB_ENUM_ANTSEL_EXT_USE_PORT_FROM_DESIGN_STRAIGHT
#endif //GP_BSP_INT_RX_ANTSEL_DEFAULT

#ifndef GP_BSP_TX_ANTSEL_DEFAULT
#define GP_BSP_TX_ANTSEL_DEFAULT GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN,GP_WB_ENUM_ANTSEL_EXT_USE_PORT_FROM_DESIGN_STRAIGHT
#endif //GP_BSP_INT_RX_ANTSEL_DEFAULT




/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void gpHalMac_SetTxOptions(UInt8 pbmEntry, gpHal_PbmTxOptions_t* pTxOptions);
static void gpHalMac_SetAckTransmitPower(void);
static void gpHalMac_ApplyAckTransmitPowerSettings(UInt8 channel);


#ifdef GP_HAL_DIVERSITY_EXT_MODE_SIGNALS
static void gpHalMac_InitExternalModeSignal(UInt8 mode_signal);
#endif


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void gpHalMac_SetTxOptions(UInt8 pbmEntry, gpHal_PbmTxOptions_t* pTxOptions)
{
    UInt16 optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmEntry);

    GP_WB_WRITE_PBM_FORMAT_T_GP_FIRST_BOFF_IS_0(optsbase, pTxOptions->first_boff_is_0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CSMA_CA_ENABLE(optsbase, pTxOptions->csma_ca_enable);
    GP_WB_WRITE_PBM_FORMAT_T_GP_TREAT_CSMA_FAIL_AS_NO_ACK(optsbase, pTxOptions->treat_csma_fail_as_no_ack);
    GP_WB_WRITE_PBM_FORMAT_T_GP_VQ_SEL(optsbase, pTxOptions->vq_sel); //GP_WB_ENUM_PBM_VQ_UNTIMED
    GP_WB_WRITE_PBM_FORMAT_T_GP_SKIP_CAL_TX(optsbase, false);

    GP_WB_WRITE_PBM_FORMAT_T_GP_CONFIRM_QUEUE(optsbase, 0);
    //GP_WB_WRITE_PBM_FORMAT_T_GP_ACKED_MODE(optsbase, pTxOptions->acked_mode); - filled in during DataRequest
    GP_WB_WRITE_PBM_FORMAT_T_GP_ANTENNA(optsbase, pTxOptions->antenna);
    GP_WB_WRITE_PBM_FORMAT_T_GP_FCS_INSERT_DIS(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_STOP_RX_WINDOW_ON_PBM_FULL(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_ED_SCAN(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION_VALID(optsbase, false); //Will be overwritten in PollScenario

    GP_WB_WRITE_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET(optsbase, false); //Will be overwritten in PollScenario
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A_EN(optsbase, pTxOptions->channel_ch0a_en);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0B_EN(optsbase, pTxOptions->channel_ch0b_en);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0C_EN(optsbase, pTxOptions->channel_ch0c_en);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH1_EN(optsbase, false);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_IDX(optsbase, 0); //Using 0 as main channel (for Rx/Tx)

    GP_WB_WRITE_PBM_FORMAT_T_TX_POWER(optsbase, pTxOptions->tx_power);
    GP_WB_WRITE_PBM_FORMAT_T_PA_LOW(optsbase, pTxOptions->pa_low);
    GP_WB_WRITE_PBM_FORMAT_T_PA_ULTRALOW(optsbase, pTxOptions->pa_ultralow);

    GP_WB_WRITE_PBM_FORMAT_T_GP_MODE_CTRL(optsbase, pTxOptions->mode);
    GP_WB_WRITE_PBM_FORMAT_T_GP_ANTSEL_INT(optsbase, pTxOptions->antsel_int);
    GP_WB_WRITE_PBM_FORMAT_T_GP_ANTSEL_EXT(optsbase, pTxOptions->antsel_ext);

    //Disable Coex itf
    GP_WB_WRITE_PBM_FORMAT_T_GP_COEX_ACK_RX_PRIO(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_COEX_ACK_RX_REQ_EN(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_COEX_GRANT_AWARE(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_COEX_PACKET_TX_PRIO(optsbase, 0);
    GP_WB_WRITE_PBM_FORMAT_T_GP_COEX_PACKET_TX_REQ_EN(optsbase, 0);

    GP_WB_WRITE_PBM_FORMAT_T_MIN_BE(optsbase,pTxOptions->min_be);
    GP_WB_WRITE_PBM_FORMAT_T_MAX_BE(optsbase,pTxOptions->max_be);
    GP_WB_WRITE_PBM_FORMAT_T_MAX_CSMA_BACKOFFS(optsbase,pTxOptions->max_csma_backoffs);
    GP_WB_WRITE_PBM_FORMAT_T_MAX_FRAME_RETRIES(optsbase,pTxOptions->max_frame_retries);

    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0A(optsbase,pTxOptions->channel_ch0a);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0B(optsbase,pTxOptions->channel_ch0b);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH0C(optsbase,pTxOptions->channel_ch0c);
    GP_WB_WRITE_PBM_FORMAT_T_GP_CHANNEL_CH1(optsbase,pTxOptions->channel_ch1);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION(optsbase,pTxOptions->rx_duration); //Will be overwritten in PollScenario

    GP_WB_WRITE_PBM_FORMAT_T_STATE(optsbase, 7);            //only for test and debug
    GP_WB_WRITE_PBM_FORMAT_T_RETURN_CODE(optsbase, 255);    //only for test and debug
    //GP_WB_WRITE_PBM_FORMAT_T_FRAME_PTR(optsbase, pTxOptions->frame_ptr); //Written in gpHal_DataRequest
    //GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(optsbase, pTxOptions->frame_len); //Written in gpHal_DataRequest

    //GP_WB_WRITE_PBM_FORMAT_T_MISC_DATA(optsbase, val); - unused
}

static void gpHalMac_PreparePollRequestPBM(UInt16 optsbase)
{
    GP_WB_WRITE_PBM_FORMAT_T_GP_START_RX_ONLY_IF_PENDING_BIT_SET(optsbase, true);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION(optsbase, GPHAL_POLL_REQ_MAX_WAIT_TIME);
    GP_WB_WRITE_PBM_FORMAT_T_GP_RX_DURATION_VALID(optsbase, true);
}

void gpHalMac_CalibrateChannel(gpHal_SourceIdentifier_t srcId, UInt8 channel)
{
    //Calibrate channel before enabling receiver
    GP_WB_WRITE_RIB_CHANNEL_IDX(srcId);
    GP_WB_WRITE_RIB_CHANNEL_NR(channel-11);

    GP_WB_RIB_CHANNEL_CHANGE_REQUEST();
    GP_DO_WHILE_TIMEOUT_ASSERT(GP_WB_READ_RIB_CHANNEL_CHANGE_BUSY(), 100000); //100ms - max delay after all RF activity
}

static void gpHalMac_SetRxOnWhenIdle(gpHal_SourceIdentifier_t srcId, Bool flag)
{
    switch (srcId)
    {
    case 0:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH0(flag);
        break;
    case 1:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH1(flag);
        break;
    case 2:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH2(flag);
        break;
    case 3:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH3(flag);
        break;
    case 4:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH4(flag);
        break;
    case 5:
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH5(flag);
        break;
    default:
        GP_ASSERT_SYSTEM(false);
    }
}

UInt8 gpHalMac_GetChannel_LowTXPower(UInt8 *low_channel)
{
    UInt8 tmp_channel;
    gpHal_TxPower_t lowest_txPower = 0;
    gpHal_TxPower_t tmp_txPower = 0;
    Bool notSet = true;
    UInt8 rxOn;

    rxOn = GP_WB_READ_RIB_RX_ON_WHEN_IDLE_CH();
#define CMP_CHANNEL_IDX(idx) \
    if (BIT_TST(rxOn, idx))\
    {\
        tmp_channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_##idx();\
        tmp_txPower = GET_DEFAULT_TX_POWER(tmp_channel);\
        if (notSet || tmp_txPower < lowest_txPower)\
        {\
          lowest_txPower = tmp_txPower;\
          *low_channel=tmp_channel;\
          notSet=false;\
        }\
    }

    CMP_CHANNEL_IDX(0);
    CMP_CHANNEL_IDX(1);
    CMP_CHANNEL_IDX(2);
    CMP_CHANNEL_IDX(3);
    CMP_CHANNEL_IDX(4);
    CMP_CHANNEL_IDX(5);

    return rxOn;
}

void gpHalMac_RecalibrateChannels(void)
{
    UInt8 channel;
    UInt8 rxOn;
    UIntLoop idx;

    rxOn = GP_WB_READ_RIB_RX_ON_WHEN_IDLE_CH();

    for(idx=0; idx < 6; idx++)
    {
        if (BIT_TST(rxOn, idx))
        {
            channel = gpHal_GetRxChannel(idx);
            gpHalMac_CalibrateChannel(idx, channel);
        }
    }
}

#ifdef GP_HAL_DIVERSITY_EXT_MODE_SIGNALS
//---------------------
//External mode signals
//---------------------
void gpHalMac_InitExternalModeSignal(UInt8 mode_signal)
{
    switch(mode_signal)
    {
        case 0:
        {
            GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); //Mode 0 -> gpio 16
            GP_WB_WRITE_TRX_MODE_CTRL_0_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
            break;
        }
        case 1:
        {
            GP_WB_WRITE_IOB_GPIO_15_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); //Mode 1 -> gpio 15
            GP_WB_WRITE_TRX_MODE_CTRL_1_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
            break;
        }
        case 2:
        {
            GP_WB_WRITE_IOB_GPIO_14_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); //Mode 2 -> gpio 14
            GP_WB_WRITE_TRX_MODE_CTRL_2_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_MAPPING_A);
            break;
        }
        case 3:
        {
#ifdef GP_BSP_MAPPING_MODE3
            GP_WB_WRITE_IOB_GPIO_13_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT); //Mode 3 -> gpio 12/13
            GP_WB_WRITE_TRX_MODE_CTRL_3_PINMAP(GP_BSP_MAPPING_MODE3);
#else
            GP_ASSERT_DEV_INT(false);
#endif
            break;
        }
        default:
        {
            GP_ASSERT_DEV_INT(false);
        }
    }
}

UInt8 gpHalMac_FillInModeSignals(UInt8 txEn, UInt8 rxEn, UInt8 mode)
{
    UInt8 mode_ctrl = 0;
    if(0 != txEn)
    {
        BIT_SET(mode_ctrl,GP_BSP_MODE_CTRL_TXEN);
    }
    if(0 != rxEn)
    {
        BIT_SET(mode_ctrl,GP_BSP_MODE_CTRL_RXEN);
    }
    if(0 != mode)
    {
        BIT_SET(mode_ctrl,GP_BSP_MODE_CTRL_MODE);
    }
    return mode_ctrl;
}
#endif //GP_HAL_DIVERSITY_EXT_MODE_SIGNALS



static void gpHalMac_UpdateRxAntennaSelection(UInt8 intAntSel, UInt8 extAntSel)
{
    GP_WB_WRITE_RX_RX_ANTSEL_INT(intAntSel);
    GP_WB_WRITE_RX_RX_ANTSEL_EXT(extAntSel);
}

static void gpHalMac_UpdateTxAntennaSelection(gpHal_PbmTxOptions_t* pTxOptions, UInt8 intAntSel, UInt8 extAntSel)
{
    pTxOptions->antsel_int = intAntSel;
    pTxOptions->antsel_ext = extAntSel;
}







void gpHal_SetCCAThreshold(void)
{
    UInt8 protoEDThreshold = gpHal_CalculateProtoRSSI(GPHAL_CCA_THRESHOLD_VALUE);
    GP_WB_WRITE_PLME_CCA_ED_THRESH(protoEDThreshold);
}


void gpHalMac_GetFemTxOptions(gpHal_TxPower_t txPower, UInt8 channel, gpHal_PbmTxOptions_t* pTxOptions, Bool forAck, UInt8 CWCorrection)
{
    NOT_USED(forAck);


    if (gpHal_DefaultTransmitPower == txPower)
    {
        txPower = GET_DEFAULT_TX_POWER(channel);
    }

    gpHalMac_UpdateTxAntennaSelection(pTxOptions,GP_BSP_TX_ANTSEL_DEFAULT);


    /* In CW-mode the output-power is set using overwrite registers of K7b based chips.
       During overwrite the channel-calibration values that are normaly set py the PA stage
       of the chip are not taken into account. The CWCorrection is used to add the channelcalibration
       value manual to the output power. In normal mode the CWCorrection should be zero.
    */
    txPower += CWCorrection;

    //Results from SW-3140, Add here ticket with corrected power convertion.
    pTxOptions->pa_low = false;
    pTxOptions->pa_ultralow = false;
    pTxOptions->tx_power = txPower + 4;
    if(txPower < -3)
    {
        pTxOptions->tx_power += 9;
        pTxOptions->pa_low = true;
    }
    if(txPower < -12)
    {
        pTxOptions->tx_power += 10;
        pTxOptions->pa_ultralow = true;
    }
}

//-------------------------------------------------------------------------------------------------------
//  TX Power
//-------------------------------------------------------------------------------------------------------

void gpHal_SetDefaultTransmitPowers(Int8* pDefaultTransmitPowerTable )
{
    GP_ASSERT_DEV_EXT(pDefaultTransmitPowerTable != NULL);
    MEMCPY(gpHal_pDefaultTransmitPowerTable,pDefaultTransmitPowerTable,sizeof(gpHal_pDefaultTransmitPowerTable));
    gpHalMac_SetAckTransmitPower();
}

void gpHalMac_SetAckTransmitPower(void)
{
    UInt8 channel;

    /* All RX channels share the same ACK PBM.
     * To be safe we choose the channel with the lowest transmit power.
     */
    if (!gpHalMac_GetChannel_LowTXPower(&channel))
    {
        // Not RX-ing on any channel.
        return;
    }
    channel+=11;
    gpHalMac_ApplyAckTransmitPowerSettings(channel);
}

void gpHalMac_ApplyAckTransmitPowerSettings(UInt8 channel)
{
/*
    All tx power settings here: pa/antsw, ...
*/
    gpHal_PbmTxOptions_t txOptions;

    gpHalMac_GetFemTxOptions(gpHal_DefaultTransmitPower,channel, &txOptions, GPHAL_TX_OPTIONS_FOR_ACK, GPHAL_NO_CW_MODE);

    /* PA settings */
    GP_WB_WRITE_PBM_FORMAT_T_TX_POWER(GPHAL_ACK_PBM_ADDR, txOptions.tx_power);
    GP_WB_WRITE_PBM_FORMAT_T_PA_LOW(GPHAL_ACK_PBM_ADDR, txOptions.pa_low);
    GP_WB_WRITE_PBM_FORMAT_T_PA_ULTRALOW(GPHAL_ACK_PBM_ADDR, txOptions.pa_ultralow);
#ifdef GP_HAL_DIVERSITY_EXT_MODE_SIGNALS
    GP_WB_WRITE_PBM_FORMAT_T_GP_MODE_CTRL(GPHAL_ACK_PBM_ADDR, txOptions.mode);
#endif //GP_HAL_DIVERSITY_EXT_MODE_SIGNALS

    /* Antenna Settings */
    GP_WB_WRITE_PBM_FORMAT_T_GP_ANTSEL_INT(GPHAL_ACK_PBM_ADDR, txOptions.antsel_int);
}

void gpHalMac_InitMacAddress(void)
{
    UInt8 macAddr[8];

    //Load MAC addr
    gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_MAC_ADDR, 8, macAddr);
    gpHal_SetExtendedAddress((MACAddress_t*)&macAddr);
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  INIT
//-------------------------------------------------------------------------------------------------------

void gpHal_InitMAC(Bool timedMAC)
{
    //Start with clean local MAC state
    GP_HAL_MAC_STATE_INIT();

    //Set timedMAC
    //gpHal_SetTimedMode(timedMAC);

    // limit the number of used PBMs to GPHAL_MAX_NUMBER_OF_PBMS (use lower PBMs, as the MAC has already claimed PBM in this range)
#define PBM_MASK BM(GPHAL_NUMBER_OF_PBMS_USED)-1
    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_UC_MASK(PBM_MASK);
    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RXMAC_MASK(PBM_MASK);
#undef PBM_MASK

    gpHalMac_InitMacAddress();

    //GP_WB_WRITE_PLME_CCA_ED_THRESH(GPHAL_CCA_THRESHOLD_VALUE);

#ifdef GP_HAL_DIVERSITY_EXT_MODE_SIGNALS

#ifdef GP_BSP_MODE_CTRL_MODE
    gpHalMac_InitExternalModeSignal(GP_BSP_MODE_CTRL_MODE);
#endif //GP_BSP_MODE_CTRL_MODE

#ifdef GP_BSP_MODE_CTRL_TXEN
    gpHalMac_InitExternalModeSignal(GP_BSP_MODE_CTRL_TXEN);
#endif //GP_BSP_MODE_CTRL_TXEN

#ifdef GP_BSP_MODE_CTRL_RXEN
    gpHalMac_InitExternalModeSignal(GP_BSP_MODE_CTRL_RXEN);
#endif //GP_BSP_MODE_CTRL_RXEN

#ifdef GP_BSP_TRX_OFF_SIGNALS
    /* Define what to do with mode signals when tx and rx off */
    GP_WB_WRITE_TRX_EXT_MODE_CTRL_FOR_TRX_OFF(gpHalMac_FillInModeSignals(GP_BSP_TRX_OFF_SIGNALS));
#endif //GP_BSP_TRX_OFF_SIGNALS

#endif //GP_HAL_DIVERSITY_EXT_MODE_SIGNALS


    gpHalMac_UpdateRxAntennaSelection(GP_BSP_RX_ANTSEL_DEFAULT);
    gpHal_SetCCAThreshold();


#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
    gpHal_SetAntenna(GP_HAL_DIVERSITY_SINGLE_ANTENNA);
#else
    gpHal_SetAntenna(gpHal_AntennaSelection_Auto);
#endif

    //Enable lower level FLL isr masks
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_CP_OUT_OF_RANGE_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_OUT_OF_LOCK_INTERRUPT(1);

}


//-------------------------------------------------------------------------------------------------------
//  MAC FUNCTIONS
//-------------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------------
//  MAC GENERAL FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_SetRxAntennaDiversity( Bool onoff )
{
    GP_WB_WRITE_RX_RX_MODE(onoff ? GP_WB_ENUM_RECEIVER_MODE_NORMAL_AD : GP_WB_ENUM_RECEIVER_MODE_NORMAL);
}

void gpHal_SetTimedMode (Bool timedMode)
{
    GP_ASSERT_SYSTEM(false); //Not working yet ? which registers to be set ?
#if 0
    if(timedMode)
    {
        GP_HAL_MAC_STATE_SET_TIMED();
    }
    else
    {
        GP_HAL_MAC_STATE_CLR_TIMED();
    }
    GP_WB_WRITE_QTA_UNTIMED_VQ_DIS(timedMode); //?
#endif
}

UInt8 gpHal_GetRxChannel(gpHal_SourceIdentifier_t srcId)
{
    UInt8 channel=0xFF-11;
    switch (srcId)
    {
        case 0:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_0();
            break;
        case 1:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_1();
            break;
        case 2:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_2();
            break;
        case 3:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_3();
            break;
        case 4:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_4();
            break;
        case 5:
            channel = GP_WB_READ_PLME_ACTIVE_CHANNEL_5();
            break;
        default:
            GP_ASSERT_SYSTEM(false);
    }
    return channel+11;
}

gpHal_Result_t gpHal_GetRadioState(void)
{
    UInt8 radioState;
    radioState=GP_WB_READ_PLME_TRX_STATE_STATUS();

    switch(radioState)
    {
        case GP_WB_ENUM_PHY_STATE_TRX_OFF:
        {
            radioState = gpHal_ResultTrxOff;
            break;
        }
        case GP_WB_ENUM_PHY_STATE_TX_ON:
        {
            radioState = gpHal_ResultTxOn;
            break;
        }
        case GP_WB_ENUM_PHY_STATE_RX_ON:
        {
            radioState = gpHal_ResultRxOn;
            break;
        }
        default:
        {
            radioState = gpHal_ResultBusy;
            break;
        }
    }

    return radioState;
}

UInt8 gpHal_GetHandle(void)
{
    return GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIM();
}

void gpHal_FreeHandle(UInt8 handle)
{
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(handle) && GP_HAL_IS_PBM_ALLOCATED(handle));
    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RETURN(handle);
}

//Lqi for K7 pbm is invalid, we calcute the lqi from the rssi value
UInt8 gpHal_GetLQI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;
    Int8 rssi;
    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    rssi = gpHal_CalculateRSSI(GP_WB_READ_PBM_FORMAT_R_RSSI(pbmOptAddress));
    return gpHal_CalculateLQIfromRSSI(rssi);
}

Int8 gpHal_GetRSSI(UInt8 PBMentry)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    return (Int8)gpHal_CalculateRSSI(GP_WB_READ_PBM_FORMAT_R_RSSI(pbmOptAddress));
}

void gpHal_GetRxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    *pTimeStamp = GP_WB_READ_PBM_FORMAT_R_TIMESTAMP(pbmOptAddress);
    *pTimeStamp <<= GP_ES_TIME_UNIT_SHIFT;
}

void gpHal_GetTxTimestamp(UInt8 PBMentry, UInt32* pTimeStamp)
{
    gpHal_Address_t pbmOptAddress;

    GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PBMentry) && GP_HAL_IS_PBM_ALLOCATED(PBMentry));
    pbmOptAddress = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentry);
    *pTimeStamp = GP_WB_READ_PBM_FORMAT_T_TIMESTAMP(pbmOptAddress);
    *pTimeStamp <<= GP_ES_TIME_UNIT_SHIFT;
}


//-------------------------------------------------------------------------------------------------------
//  MAC TRANSMIT FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_FillInTxOptions(UInt8 pbmHandle, gpPad_Attributes_t* pOptions)
{
    gpHal_PbmTxOptions_t txOptions;
    gpHal_Address_t optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    GP_LOG_PRINTF("ch:%u,%u,%u a:%u mBE:%u/%u mBO:%u mFR:%u Pow:%i C:%i",0,\
                                        pOptions->channels[0], pOptions->channels[1], pOptions->channels[2], \
                                        pOptions->antenna, \
                                        pOptions->maxBE, pOptions->minBE, \
                                        pOptions->maxCsmaBackoffs, pOptions->maxFrameRetries, \
                                        pOptions->txPower,
                                        pOptions->csma);
    //Set defaults
    MEMCPY_P(&txOptions, &gpHalMac_PbmTxOptionsDefault, sizeof(gpHal_PbmTxOptions_t));

    //Antenna selection
#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
    txOptions.antenna = GP_HAL_DIVERSITY_SINGLE_ANTENNA;
#else
    txOptions.antenna = pOptions->antenna;
#endif

    //CSMA/CA settings
    if(pOptions->csma != gpHal_CollisionAvoidanceModeNoCCA)
    {
        //TxOptions.max_frame_retries = pOptions->maxFrameRetries; //- retries will be set during channel config
        if(pOptions->csma == gpHal_CollisionAvoidanceModeCSMA)
        {
            txOptions.min_be = pOptions->minBE;
            txOptions.max_be = pOptions->maxBE;
            txOptions.max_csma_backoffs = pOptions->maxCsmaBackoffs;
        }
        else if (pOptions->csma == gpHal_CollisionAvoidanceModeCCA)
        {
            txOptions.min_be = 0;
            txOptions.max_be = 0;
            txOptions.max_csma_backoffs = 0;
        }
        txOptions.csma_ca_enable = true;
    }
    else
    {
        txOptions.csma_ca_enable = false;
    }

    //Write channel options
    GP_ASSERT_DEV_INT(pOptions->channels[0] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    txOptions.channel_ch0a = pOptions->channels[0] - 11;

    //Using channel a for Tx channel selection
    txOptions.channel_ch0a_en = (pOptions->channels[0] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    txOptions.channel_ch0b_en = (pOptions->channels[1] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);
    txOptions.channel_ch0c_en = (pOptions->channels[2] != GP_HAL_MULTICHANNEL_INVALID_CHANNEL);

    //Reset possible option
    txOptions.first_boff_is_0 = false;

    //Get acked mode - filled in during DataRequest call
    txOptions.acked_mode = GP_WB_READ_PBM_FORMAT_T_GP_ACKED_MODE(optsbase);
    if(!txOptions.channel_ch0b_en && !txOptions.channel_ch0c_en)
    {
        //Normal no-ack and csma fail results
        txOptions.treat_csma_fail_as_no_ack = false;
        if(txOptions.acked_mode)
        {
            //1 channel Acked Tx - MAC retries can be taken into account
            txOptions.max_frame_retries = pOptions->maxFrameRetries;
        }
        else
        {
            //1 channel Un-acked Tx - no MAC retries - used for multi-channel behavior
            txOptions.max_frame_retries = 0;
        }
    }
    else if(txOptions.channel_ch0b_en && !txOptions.channel_ch0c_en)
    {
        //2 channel Tx - MAC retries ignored - used for multi-channel behavior
        txOptions.max_frame_retries = 1;
        txOptions.channel_ch0b = pOptions->channels[1] - 11; //Using channel info as storage
    }
    else if(txOptions.channel_ch0b_en && txOptions.channel_ch0c_en)
    {
        //3 channel Tx - MAC retries ignored - used for multi-channel behavior
        txOptions.max_frame_retries = 2;
        txOptions.channel_ch0b = pOptions->channels[1] - 11; //Using channel info as storage
        txOptions.channel_ch0c = pOptions->channels[2] - 11; //Using channel info as storage

        // setting FIRST Backoff is 0 gives 1 ms extra margin for a 3-channel mc retansmit.
        // it's only done if there are 3 channels in the list.
        // Measurement indicates that for a short (2byte) RF4CE data frame, the loop
        // decreases from 9.2ms to 8.2ms (as expected)
        txOptions.first_boff_is_0 = true;
    }

    if (txOptions.acked_mode == false)
    {
        //Handle in SW: Non-acked multi-channel
        //Don't need other channels enabled - will loop through their values in SW if needed
        txOptions.channel_ch0b_en = false;
        txOptions.channel_ch0c_en = false;
    }

    /* Get internal/external antenna & Pa settigs:
        txOptions.tx_power
        txOptions.pa_low
        txOptions.pa_ultralow
        txOptions.mode
        txOptions.antsel_ext
        txOptions.antsel_int
    */

    gpHalMac_GetFemTxOptions(pOptions->txPower, pOptions->channels[0], &txOptions, GPHAL_TX_OPTIONS_FOR_TRANSMIT, GPHAL_NO_CW_MODE);

    //Write Tx options
    gpHalMac_SetTxOptions(pbmHandle, &txOptions);
}

void gpHal_DataRequest_base(UInt8 pbmHandle)
{


    GP_STAT_SAMPLE_TIME();
    //Queue the frame
    GP_WB_WRITE_QTA_PBEFE_DATA_REQ(pbmHandle);
}

gpHal_Result_t gpHal_DataRequest(gpHal_DataReqOptions_t *dataReqOptions, gpPad_Handle_t padHandle, gpPd_Loh_t pdLoh)
{
    UInt8 ackRequest;
    UInt8 pbmHandle;
    UInt16 optsbase;

    GP_ASSERT_DEV_EXT(dataReqOptions);

    // make pd offset valid
    pdLoh.offset &= 0x7f;

    if((pdLoh.length > GPHAL_MAX_PBM_LENGTH) || (pdLoh.length == 0))
    {
        GP_ASSERT_DEV_INT(false);
        return gpHal_ResultInvalidParameter;
    }

    //Data to PBM
    pbmHandle = gpPd_DataRequest(&pdLoh);
    if(!GP_HAL_CHECK_PBM_VALID(pbmHandle))
    {
        return gpHal_ResultBusy;
    }
    optsbase = GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmHandle);

    //Check if Ack bit is set
    ackRequest = BIT_TST(gpPd_ReadByte(pdLoh.handle,pdLoh.offset), GPHAL_ACK_REQ_LSB);

    //Set basic settings to pbm
    GP_WB_WRITE_PBM_FORMAT_T_GP_ACKED_MODE(optsbase, ackRequest);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_PTR(optsbase, pdLoh.offset);
    GP_WB_WRITE_PBM_FORMAT_T_FRAME_LEN(optsbase, pdLoh.length);

    //Set all pad settings to a certain PBM
    gpPad_DataRequest(pbmHandle, padHandle);

    //Add any additional settings for a certain scenario
    switch(dataReqOptions->macScenario)
    {
        case gpHal_MacPollReq:
        {
            gpHalMac_PreparePollRequestPBM(optsbase);
            break;
        }
        default:
        {
            break;
        }
    }

    gpHal_DataRequest_base(pbmHandle);
    return gpHal_ResultSuccess;
}

//-------------------------------------------------------------------------------------------------------
//  MAC RECEIVE FUNCTIONS
//-------------------------------------------------------------------------------------------------------

void gpHal_SetRxOnWhenIdle(gpHal_SourceIdentifier_t srcId, Bool flag, UInt8 channel)
{
    if (flag)
    {
        gpHal_GoToSleepWhenIdle( false );
        gpHalMac_CalibrateChannel(srcId, channel);
        gpHalMac_SetRxOnWhenIdle(srcId, flag);
        gpHalMac_SetAckTransmitPower();
        GP_HAL_MAC_STATE_SET_RXON();
    }
    else
    {
        gpHalMac_SetRxOnWhenIdle(srcId, flag);
        gpHalMac_SetAckTransmitPower();
        gpHal_GoToSleepWhenIdle( true);
        GP_HAL_MAC_STATE_CLR_RXON();
    }
}


void gpHal_SetPanId(UInt16 panId, gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        GP_WB_WRITE_MACFILT_PAN_ID(panId);
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        GP_WB_WRITE_RIB_DST_PAN_ID_TABLE_SIZE(6*2-1);
        GP_WB_WRITE_MACFILT_PAN_ID_ALT_A(panId);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
    }
}

UInt16 gpHal_GetPanId(gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        return GP_WB_READ_MACFILT_PAN_ID();
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        return GP_WB_READ_MACFILT_PAN_ID_ALT_A();
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
        return 0xFFFF;
    }
}

void gpHal_SetShortAddress(UInt16 shortAddress, gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        GP_WB_WRITE_MACFILT_SHORT_ADDRESS(shortAddress);
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        //PAN Id needs to be setting takes effect
        GP_WB_WRITE_MACFILT_SHORT_ADDRESS_ALT_A(shortAddress);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
    }
}

UInt16 gpHal_GetShortAddress(gpHal_SourceIdentifier_t srcId)
{
    if(srcId == gpHal_SourceIdentifier_0)
    {
        return GP_WB_READ_MACFILT_SHORT_ADDRESS();
    }
    else if(srcId == gpHal_SourceIdentifier_1)
    {
        return GP_WB_READ_MACFILT_SHORT_ADDRESS_ALT_A();
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);   // unknown src
        return 0xFFFF;
    }
}

void gpHal_SetExtendedAddress(MACAddress_t* pExtendedAddress)
{
    HOST_TO_RF_UINT64(pExtendedAddress);
    //GP_WB_WRITE_MACFILT_MAC_EXTENDED_ADDRESS(pExtendedAddress); //No 64 bit number for check
    GP_HAL_WRITE_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ADDRESS, pExtendedAddress,
                             sizeof(*pExtendedAddress));
    RF_TO_HOST_UINT64(pExtendedAddress);
}

void gpHal_GetExtendedAddress(MACAddress_t* pExtendedAddress)
{
    //GP_WB_READ_MACFILT_MAC_EXTENDED_ADDRESS(pExtendedAddress); //No 64 bit number for check
    GP_HAL_READ_BYTE_STREAM(GP_WB_MACFILT_EXTENDED_ADDRESS_ADDRESS , pExtendedAddress,
                            sizeof(*pExtendedAddress));
    RF_TO_HOST_UINT64(pExtendedAddress);
}

void gpHal_SetCoordExtendedAddress(MACAddress_t* pCoordExtendedAddress)
{
    //GP_HAL_WRITE_REGS64(GPHAL_REGISTER_RIB_MAC_COORD_EXT_ADR_0 , pCoordExtendedAddress);
}

void gpHal_SetCoordShortAddress(UInt16 shortCoordAddress)
{
    //GP_HAL_WRITE_REGS16(GPHAL_REGISTER_RIB_MAC_COORD_SHORT_ADR_0 , &shortCoordAddress );
}

void gpHal_SetPanCoordinator(Bool panCoordinator)
{
    GP_WB_WRITE_MACFILT_PAN_COORDINATOR(panCoordinator);
}

Bool gpHal_GetPanCoordinator(void)
{
    return GP_WB_READ_MACFILT_PAN_COORDINATOR();
}

void gpHal_SetAddressRecognition(Bool enable, Bool panCoordinator)
{
    GP_WB_WRITE_MACFILT_DST_PAN_ID_CHECK_ON(enable);
    GP_WB_WRITE_MACFILT_DST_ADDR_CHECK_ON(enable);
    GP_WB_WRITE_MACFILT_PAN_COORDINATOR(panCoordinator);
}

void gpHal_SetBeaconSrcPanChecking(Bool enable)
{
    GP_WB_WRITE_MACFILT_SRC_PAN_ID_BEACON_CHECK_ON(enable);
}

Bool gpHal_GetBeaconSrcPanChecking(void)
{
    return GP_WB_READ_MACFILT_SRC_PAN_ID_BEACON_CHECK_ON();
}

gpHal_AntennaSelection_t gpHal_GetTxAntenna(void)
{
#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
    return GP_HAL_DIVERSITY_SINGLE_ANTENNA;
#else
    return gpHal_AntennaSelection_Auto;
#endif
}

gpHal_AntennaSelection_t gpHal_GetRxAntenna(void)
{
    switch(GP_WB_READ_RX_RX_ANTSEL_INT())
    {
        case 0: //auto
            if(GP_WB_READ_RX_RX_MODE() != GP_WB_ENUM_RECEIVER_MODE_NORMAL_AD)
            {
                return (GP_WB_READ_RX_ANT_USE() ? gpHal_AntennaSelection_Ant1 : gpHal_AntennaSelection_Ant0);
            }
            return gpHal_AntennaSelection_Auto;
        case 1: //forced to ant0
            return gpHal_AntennaSelection_Ant0;
        case 2: //forced to ant1
            return gpHal_AntennaSelection_Ant1;
        default:
            //all supported cases covered
            GP_ASSERT_DEV_EXT(false);
            break;
    }
    //will never come
    return  gpHal_AntennaSelection_Unknown;
}

void gpHal_SetAntenna(gpHal_AntennaSelection_t rxAntenna)
{
#ifdef GP_HAL_DIVERSITY_SINGLE_ANTENNA
    rxAntenna = GP_HAL_DIVERSITY_SINGLE_ANTENNA;
#endif

    //configure internal antenne switch
    if (rxAntenna == gpHal_AntennaSelection_Auto)
    {
        /* Antenna diversity on */
        GP_WB_WRITE_RX_RX_MODE(GP_WB_ENUM_RECEIVER_MODE_NORMAL_AD);
    }
    else
    {
        GP_WB_WRITE_RX_RX_MODE(GP_WB_ENUM_RECEIVER_MODE_NORMAL);
        GP_WB_WRITE_RX_ANT_USE(rxAntenna);
    }
}

void gpHal_WriteDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length)
{
    UInt8 startWritePBM = GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT((pbmAddr) +(pbmOffset));
    UInt8 splitlength = 0;
    if( ( startWritePBM + (length)) > GP_MM_RAM_PBM_0_DATA_SIZE)
    {// split in in 2
        splitlength = startWritePBM+(length)-128;
        // write second part
        //GP_LOG_SYSTEM_PRINTF("WD_: addr:%lx l:%u-%u o:%u stwr:%u",0,pbmAddr, length, splitlength, pbmOffset, startWritePBM);
        GP_HAL_WRITE_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (UInt8*)((pData)+(length)-splitlength) , splitlength , 0);
    }
    //GP_LOG_SYSTEM_PRINTF("WD: addr:%lx l:%u o:%u stwr:%u pD:%lx",0,pbmAddr, length, pbmOffset, startWritePBM, pData);
    // write first part or full part
    GP_HAL_WRITE_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (pData) , ((length)-splitlength) , startWritePBM);
}

void gpHal_ReadDataInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8* pData, UInt8 length)
{
    UInt8 startWritePBM = GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT((pbmAddr) +(pbmOffset));
    UInt8 splitlength = 0;
    if( ( startWritePBM + (length)) > 128 )
    {// split in in 2
        splitlength = startWritePBM+(length)-128;
        // write second part
        //GP_LOG_SYSTEM_PRINTF("RD_: addr:%lx l:%u-%u o:%u stwr:%u",0,pbmAddr, length, splitlength, pbmOffset, startWritePBM);
        GP_HAL_READ_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (UInt8*)((pData)+(length)-splitlength) , splitlength , 0);
    }
    // write first part or full part
    GP_HAL_READ_DATA_IN_PBM( GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)) , (pData) , ((length)-splitlength) , startWritePBM);
    //GP_LOG_SYSTEM_PRINTF("RD: addr:%lx l:%u o:%u stwr:%u  %x %x %x %x",0,pbmAddr, length, pbmOffset, startWritePBM,
                                                                         //pData[0], pData[1], pData[2], pData[3]);
}


void gpHal_WriteByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset, UInt8 byte)
{
    GP_HAL_WRITE_BYTE_IN_PBM(GP_HAL_CALCULATE_START_PBM_ADDRESS((pbmAddr)),
                             (byte),
                             GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT(pbmAddr + pbmOffset));
}

UInt8 gpHal_ReadByteInPBMCyclic(gpHal_Address_t pbmAddr,UInt8 pbmOffset )
{
    return GP_HAL_READ_BYTE_IN_PBM(GP_HAL_CALCULATE_START_PBM_ADDRESS(pbmAddr),
                                   GP_HAL_REMAP_REL_ADDRESS_TO_DATA_SEGMENT(pbmAddr + pbmOffset));
}

/*****************************************************************************
 *                    k7 Specific
 *****************************************************************************/

void gpHal_MakeBareCopyPBM( UInt8 PBMentryOrig , UInt8 PBMentryDst )
{
    gpHal_Address_t tempCopyAddress;
    UInt8 tempBuffer[32];

    //copy PBM options - GP_MM_RAM_PBM_OPTS_SIZE
    COMPILE_TIME_ASSERT(32 == GP_MM_RAM_PBM_OPTS_SIZE); //Assumption here
    GP_HAL_READ_BYTE_STREAM(GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryOrig), tempBuffer, GP_MM_RAM_PBM_OPTS_SIZE);
/*
    GP_LOG_SYSTEM_PRINTF("CP_: addr:%lx -> %lx : %x %x %x %x",0,GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryOrig),
                                                               GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryDst),
                                                               tempBuffer[0], tempBuffer[1],
                                                               tempBuffer[2], tempBuffer[3]);
*/
    GP_HAL_WRITE_BYTE_STREAM(GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(PBMentryDst), tempBuffer, GP_MM_RAM_PBM_OPTS_SIZE);

    //copy PBM data - GP_MM_RAM_PBM_DATA_SIZE
    COMPILE_TIME_ASSERT((GP_MM_RAM_PBM_0_DATA_SIZE % 32) == 0); //Assumption here
    for(tempCopyAddress=0; tempCopyAddress < GP_MM_RAM_PBM_0_DATA_SIZE; tempCopyAddress+=32)
    {
        GP_HAL_READ_BYTE_STREAM( GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryOrig) + tempCopyAddress, tempBuffer, 32);
/*
        GP_LOG_SYSTEM_PRINTF("CP: addr:%lx -> %lx : %x %x %x %x",0, GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryOrig) + tempCopyAddress,
                                                                    GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryDst)  + tempCopyAddress,
                                                                    tempBuffer[0], tempBuffer[1],
                                                                    tempBuffer[2], tempBuffer[3]);
*/
        GP_HAL_WRITE_BYTE_STREAM(GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(PBMentryDst) + tempCopyAddress, tempBuffer, 32);
    }
}

void gpHal_SetAutoAcknowledge(Bool flag)
{
    GP_WB_WRITE_RIB_ACK_REQUEST_DISABLE(!flag);
}

GP_API Bool gpHal_GetAutoAcknowledge(void)
{
    return !GP_WB_READ_RIB_ACK_REQUEST_DISABLE();
}

Int8 gpHal_CalculateRSSI( UInt8 protoRSSI )
{
    //RSSI (proto) -> RSSI (dBm)
    Int16 temp;

    temp = (UInt16)protoRSSI << 1;
    temp += 1; //for correct rounding of calculation

    if(protoRSSI < 128) //(61/128) * RSSI – 115
    {
        temp *= 61;
        temp >>= 8;
        temp -= 115;
    }
    //Ceil
    else // (75/128) * RSSI - 129
    {
        temp *= 75;
        temp >>= 8;
        temp -= 129;
    }


    return (Int8)temp;
}

UInt8 gpHal_CalculateLQIfromRSSI( Int8 RSSI)
{
    //RSSI -> LQI (0-255)
    UInt16 temp;

    //Floor
    if(RSSI < -87)
    {
        temp = 0;
    }
    //Ceil
    else if(RSSI > -47)
    {
        temp = 255;
    }
    //Range
    else
    {
        if (RSSI < -54)
        {
            //5.25*rssi+495 = (21/4)*rssi+495
            temp = RSSI;
            temp*= 21;
            temp+= 2;   //round + (2/4)
            temp>>= 2;
            temp+= 495;
        }
        else
        {
            //4.25*rssi+445 = (17/4)*rssi+445
            temp = RSSI;
            temp*= 17;
            temp>>= 2;
            temp+= 440;  //445 calculated, but better results with 440
        }
    }

    return (UInt8)temp;
}

UInt8 gpHal_CalculateProtoRSSI(Int8 rssi)
{
    //RSSI (dBm) -> RSSI (proto)
    Int16 protoRSSI = rssi;


    if (rssi < -54)
    {
        protoRSSI += 115;
        protoRSSI <<= 7;
        protoRSSI /= 61;
    }
    else
    {
        protoRSSI += 129;
        protoRSSI <<= 7;
        protoRSSI /= 75;
    }
    if (protoRSSI > 255)
    {
        protoRSSI = 255;
    }

    if (protoRSSI < 0)
    {
        protoRSSI = 0;
    }

    return (UInt8)protoRSSI;
}

Bool gpHal_CheckNoLock(void)
{
    //Out of lock will be corrected by ISR
    //This should only return true in case of consistent out of lock
    return GP_WB_READ_TRX_UNMASKED_FLL_OUT_OF_LOCK();
}

void gpHal_SetPromiscuousMode(Bool flag)
{
    if (flag)
    {
        GP_WB_WRITE_MACFILT_ENABLE_SNIFFING(true);
        GP_WB_WRITE_RIB_ACK_REQUEST_DISABLE(true);
        //Update gpHal state:
        GP_HAL_MAC_STATE_SET_PROMISCUOUS_MODE();
    }
    else
    {
        GP_WB_WRITE_MACFILT_ENABLE_SNIFFING(false);
        GP_WB_WRITE_RIB_ACK_REQUEST_DISABLE(false);
        //Update gpHal state:
        GP_HAL_MAC_STATE_CLR_PROMISCUOUS_MODE();
    }
}

Bool gpHal_GetPromiscuousMode(void)
{
    return GP_HAL_MAC_STATE_GET_PROMISCUOUS_MODE();
}

void gpHal_SetFramePendingAckDefault(Bool enable)
{

}

Bool gpHal_GetFramePendingAckDefault(void)
{
    return false;
}

#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
/* See also on K5: static void gpHalDpi_SetRXMAC_PBMMask(UInt8 mask) */
Bool gpHal_SetRXMAC_PBMMask(UInt8 mask_mac)
{
    UInt8 pbmMac;

    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RXMAC_MASK(mask_mac);

    pbmMac = GP_WB_READ_RIB_RX_PBM_PTR();
    GP_WB_RIB_RX_PBM_PTR_CLR();
    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RETURN(pbmMac);

    return true;
}

Bool gpHal_ClaimPbmMemory(void)
{
    UInt8 PbmHandle=0;
    UInt8 restrictedMacMask = 0x00; // allow no PBMs for HW while we are doing IR.

    GP_LOG_PRINTF("MAC OFF",0);
    if(GP_HAL_MAC_STATE_GET_RXON())
    {
        GP_LOG_PRINTF("RX busy",0);
        return false;
    }

    if(!gpHal_SetRXMAC_PBMMask(restrictedMacMask))
    {
        GP_LOG_PRINTF("change Mac Mask failed",0);
        return false;
    }

    if(GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIMED() != 0)
    {
        GP_LOG_PRINTF("some PBMs are still in use: 0x%x",0, GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIMED());
        return false;
    }

    for(PbmHandle=0;PbmHandle<GPHAL_NUMBER_OF_PBMS_USED;PbmHandle++)
    {
        GP_WB_READ_PBM_ADM_PBM_ENTRY_CLAIM();
    }

    // success
    return true;
}

Bool gpHal_ReleasePbmMemory(void)
{
#define PBM_MASK BM(GPHAL_NUMBER_OF_PBMS_USED)-1

    UInt8 PbmHandle=0;

    GP_LOG_PRINTF("MAC ON",0);


    for(PbmHandle=0;PbmHandle<GPHAL_NUMBER_OF_PBMS_USED;PbmHandle++)
    {
        GP_ASSERT_DEV_EXT(GP_HAL_CHECK_PBM_VALID(PbmHandle) && GP_HAL_IS_PBM_ALLOCATED(PbmHandle));
        GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RETURN(PbmHandle);
    }

    GP_WB_WRITE_PBM_ADM_PBM_ENTRY_RXMAC_MASK(PBM_MASK);
    return true;
}

#endif // GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

