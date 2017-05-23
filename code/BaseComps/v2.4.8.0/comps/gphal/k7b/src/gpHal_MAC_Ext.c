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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_MAC_Ext.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */



/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_MAC_Ext.h"
#include "gpSched.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

//Debug
#include "hal.h"
#include "gpBsp.h"
#include "gpAssert.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_HAL_CW_RECALIBRATE_INTERVAL   20000000

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Function Proto types
 *****************************************************************************/

void gpHal_ScheduledReCalibrateChannels( void );

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


static void Hal_DisableContinuousWaveMode(void)
{
    //Unforce PA on
    GP_WB_WRITE_RADIOITF_LOBUF_PUP_PA_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_PA_BIAS_PUP_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_ANTSW_ANT_SW_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_PA_ANT_SW_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_PA_PUP_OVERRULE_ENA(0);         
    
    //Unforce PA power
    GP_WB_WRITE_RADIOITF_PA_PWR_OVERRULE_ENA(0);  
    GP_WB_WRITE_RADIOITF_PA_LOW_OVERRULE_ENA(0);
    GP_WB_WRITE_RADIOITF_PA_ULTRALOW_OVERRULE_ENA(0);   
    
    //Unforce closed loop
    GP_WB_WRITE_PLME_SIG_GEN_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_TO_OFF);
    GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(1);
    GP_WB_PLME_START_RAM_SEQUENCER();
    GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(0);
    
    //Disable continuous modulation
    GP_WB_WRITE_TX_EN_CONT_MOD(false);

}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  MAC TRANSMIT FUNCTIONS
//-------------------------------------------------------------------------------------------------------

gpHal_Result_t gpHal_PurgeRequest(gpPd_Handle_t pdHandle)
{
    GP_ASSERT_SYSTEM(false); //Not working yet - switch to timed mode OK ?
#if 0
    gpHal_Result_t result;
    UInt8 PBMentry;

    //Lookup PBMentry belonging to PdHandle
    //-----------------------------------
    PBMentry = gpPd_PurgeRequest(pdHandle);
    if( GP_PD_INVALID_HANDLE == PBMentry)
    {
        return gpHal_ResultInvalidRequest;
    }

    //Purge PBM entry
    //---------------
    DISABLE_GP_GLOBAL_INT();

    GP_WB_WRITE_QTA_SCH_TX_QTA_PURGE_EN(1); //?

    GP_WB_WRITE_QTA_PURGE_NUMBER(PBMentry);
    
    GP_WB_QTA_PURGE_REQ();
    GP_DO_WHILE_TIMEOUT_ASSERT(!GP_WB_READ_QTA_PURGE_DONE(), GP_HAL_DEFAULT_TIMEOUT);
    
    result = GP_WB_READ_QTA_PURGE_STATUS();

    GP_WB_WRITE_QTA_SCH_TX_QTA_PURGE_EN(0); //?

    ENABLE_GP_GLOBAL_INT();

    gpPd_cbPurgeConfirm(PBMentry);

    return result ? gpHal_ResultBusy : gpHal_ResultSuccess; //Success = 0x0
#endif
    return gpHal_ResultBusy;
}


void gpHal_SetContinuousWaveMode( gpHal_ContinuousWaveMode_t mode, UInt8 channel, Int8 txpower, gpHal_AntennaSelection_t antenna)
{
    UInt32 ChannelCalVal;
    UInt8  TotalCalibrationCorrection;
    static Bool restore_ool_int = false;
    static Bool restore_oor_int = false;
   
    if (mode == gpHal_CW_Off)
    {
        Hal_DisableContinuousWaveMode();

/* APP-3991, during continous wave an effect "pulling" can result in continous triggering of CP_OOR interrupt
   As work arround this interrupt is disabled during continous wave mode. To avoid reenterent effect on recalibration 
   channels on a PHY-interrupt in this mode the FLL-OOL interrupt is also disabled */
        gpSched_UnscheduleEvent(gpHal_ScheduledReCalibrateChannels);
        
        if (restore_ool_int) /* Don't enable if not enabled before. */
        {
            GP_WB_TRX_FLL_OUT_OF_LOCK_CLR_INT();
            GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_OUT_OF_LOCK_INTERRUPT(1);
            restore_ool_int = false;
        }
        if (restore_oor_int) /* Don't enable if not enabled before. */
        {
            GP_WB_TRX_FLL_CP_OUT_OF_RANGE_CLR_INT();
            GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_CP_OUT_OF_RANGE_INTERRUPT(1);
            restore_oor_int = false;
        }
       
    }
    else
    {
/* APP-3991, during continous wave an effect "pulling" can result in continous triggering of CP_OOR interrupt
   As work arround this interrupt is disabled during continous wave mode. To avoid reenterent effect on recalibration 
   channels on a PHY-interrupt in this mode the FLL-OOL interrupt is also disabled */
        if (GP_WB_READ_INT_CTRL_MASK_PHY_FLL_OUT_OF_LOCK_INTERRUPT()) {
            restore_ool_int = true;
            GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_OUT_OF_LOCK_INTERRUPT(0);
        }
        if (GP_WB_READ_INT_CTRL_MASK_PHY_FLL_CP_OUT_OF_RANGE_INTERRUPT())
        {
            restore_oor_int = true;
            GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_CP_OUT_OF_RANGE_INTERRUPT(0);
        }
        gpSched_ScheduleEvent(GP_HAL_CW_RECALIBRATE_INTERVAL, gpHal_ScheduledReCalibrateChannels);
        

        gpHal_PbmTxOptions_t gpHalMacExt_TxOptions;

        // Always disable continuous wave before doing any changes
        Hal_DisableContinuousWaveMode();

        if (antenna == gpHal_AntennaSelection_Auto)
        {
            antenna = GP_HAL_TX_ANTENNA;
        }

        /*Add channel and antenna correction and get all internal pa settings */
        GP_HAL_READ_REGS32(GP_WB_TX_PA_PWR_CORRECTION_CH0_ADDRESS, &ChannelCalVal); 
        TotalCalibrationCorrection = (ChannelCalVal >> ((channel-11)*2)) & 0x00000003;
        TotalCalibrationCorrection += (GP_HAL_READ_REG(GP_WB_TX_PA_PWR_CORRECTION_ANT0_ADDRESS) >> (antenna*2)) & 0x03;

        /*Get all pa settings */
        gpHalMac_GetFemTxOptions(txpower, channel, &gpHalMacExt_TxOptions, GPHAL_TX_OPTIONS_FOR_TRANSMIT, TotalCalibrationCorrection);

        //Calibrate channel
        gpHalMac_CalibrateChannel(gpHal_SourceIdentifier_0, channel);

        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH0(1);
        GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH0(0);

        //Force closed loop
        //FLL to TX freq: will give spectrum @ correct frequency offset but with some extra garbage in unmodulate case
        GP_WB_WRITE_PLME_SIG_GEN_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_TX);
/*
        //FLL to RX freq: will give clean spectrum @ incorrect frequency offset (+2.5MHz)
        //GP_WB_WRITE_PLME_SIG_GEN_TRANSITION(GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_RX);
*/

        GP_WB_WRITE_PLME_SIG_GEN_TRIGGER_FROM_REGMAP(1);
        GP_WB_PLME_START_RAM_SEQUENCER();

        //Enable wanted modulation
        GP_WB_WRITE_TX_EN_CONT_MOD(mode == gpHal_CW_Modulated);

        //Force PA power
        GP_WB_WRITE_RADIOITF_PA_PWR_OVERRULE(gpHalMacExt_TxOptions.tx_power);
        GP_WB_WRITE_RADIOITF_PA_PWR_OVERRULE_ENA(1);  
        GP_WB_WRITE_RADIOITF_PA_LOW_OVERRULE(gpHalMacExt_TxOptions.pa_low);
        GP_WB_WRITE_RADIOITF_PA_LOW_OVERRULE_ENA(1);
        GP_WB_WRITE_RADIOITF_PA_ULTRALOW_OVERRULE(gpHalMacExt_TxOptions.pa_ultralow);
        GP_WB_WRITE_RADIOITF_PA_ULTRALOW_OVERRULE_ENA(1);


        //Force PA on
        GP_WB_WRITE_RADIOITF_LOBUF_PUP_PA_OVERRULE(1);
        GP_WB_WRITE_RADIOITF_LOBUF_PUP_PA_OVERRULE_ENA(1);
        GP_WB_WRITE_RADIOITF_PA_BIAS_PUP_OVERRULE(1);
        GP_WB_WRITE_RADIOITF_PA_BIAS_PUP_OVERRULE_ENA(1);
        GP_WB_WRITE_RADIOITF_PA_PUP_N_OVERRULE(1);
        GP_WB_WRITE_RADIOITF_PA_PUP_N_OVERRULE_ENA(1);
        GP_WB_WRITE_RADIOITF_PA_PUP_P_OVERRULE(1);
        GP_WB_WRITE_RADIOITF_PA_PUP_P_OVERRULE_ENA(1);

        if (GP_WB_ENUM_ANTSEL_INT_USE_PORT_0 == gpHalMacExt_TxOptions.antsel_int)
        {
            GP_WB_WRITE_RADIOITF_ANTSW_ANT_SW_OVERRULE(0);
            GP_WB_WRITE_RADIOITF_PA_ANT_SW_OVERRULE(0);
        }
        else if (GP_WB_ENUM_ANTSEL_INT_USE_PORT_1 == gpHalMacExt_TxOptions.antsel_int)
        {
            GP_WB_WRITE_RADIOITF_ANTSW_ANT_SW_OVERRULE(1);
            GP_WB_WRITE_RADIOITF_PA_ANT_SW_OVERRULE(1);
        }
        else if (GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN == gpHalMacExt_TxOptions.antsel_int)
        {
            GP_WB_WRITE_RADIOITF_ANTSW_ANT_SW_OVERRULE(antenna);
            GP_WB_WRITE_RADIOITF_PA_ANT_SW_OVERRULE(antenna);
        }
        GP_WB_WRITE_RADIOITF_ANTSW_ANT_SW_OVERRULE_ENA(1);
        GP_WB_WRITE_RADIOITF_PA_ANT_SW_OVERRULE_ENA(1);
        GP_WB_WRITE_RADIOITF_PA_PUP_OVERRULE(1);
        GP_WB_WRITE_RADIOITF_PA_PUP_OVERRULE_ENA(1);
    }

}

void gpHal_ScheduledReCalibrateChannels( void )
{
    /* The channels are normally recalibrated after an PHY-interrupt CP-OOR or FLL-OOL
       During continous wave, these interrupts are disabled, but recalibration of channels can still be
       needed. Therefore the channel recalibration is triggered on a slow hartbeat while continous wave is running */
       
    gpHalMac_RecalibrateChannels();

    gpSched_UnscheduleEvent(gpHal_ScheduledReCalibrateChannels);
    gpSched_ScheduleEvent(GP_HAL_CW_RECALIBRATE_INTERVAL, gpHal_ScheduledReCalibrateChannels);
}
