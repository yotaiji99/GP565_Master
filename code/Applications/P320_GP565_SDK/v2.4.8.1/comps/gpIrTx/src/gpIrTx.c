/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *   This file contains the implementation for transmitting all sorts of IR codes
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpIrTx/src/gpIrTx.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_IRTX
#define GP_IR_TX_DIVERSITY_PREDEFINED

#include "hal.h"
#include "gpBsp.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpSched.h"

#include "gpIrTx.h"
#include "gpIrTx_Internal.h"

#include "hal_WB.h"



#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
#include "gpHal_kx_public.h"
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/** @brief  Example NEC device value. Application may use it but is not bound by it. */
#define GP_IRNEC_DEVICE     0x77
/** @brief  Base mask value for the NEC type identifier */
#define IR_TX_NEC_BASE      0x04
/** @brief  Modifier mask value for the NEC type 2 identifier */
#define IR_TX_NEC_TYPE2     0x01
/** @brief  Modifier mask value for the NEC type X identifier */
#define IR_TX_NEC_TYPEx     0x02


/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/
/* Hal expects timing to be expressed in 4us units and be carried in UInt16*/
#define IR_TX_CORRECT_TIMING_PREDEFINED(x) ((UInt16)(x)<<0)	///((UInt16)(x)<<2)

#define IR_TX_PREDEFINED_SEND_IR_SET_CLR(pulseWidth,pauseWidth)    IrTx_SetClr(IR_TX_CORRECT_TIMING_PREDEFINED(pulseWidth), IR_TX_CORRECT_TIMING_PREDEFINED(pauseWidth))


#define NEC_CARRIER_FREQ                38000UL
#define NEC_CARRIER_PERIOD_125NS        IR_TX_CARRIER_FREQ_TO_PERIOD_125NS(NEC_CARRIER_FREQ)
#define NEC_REPEAT_INTERVAL_MS            108U

#define NEC_DATA_PULSE_WIDTH            IR_TX_ROUND_TO_NEAREST_CARRIER_125NS( 560, NEC_CARRIER_PERIOD_125NS)

#define NEC_PAUSE_WIDTH_0               IR_TX_ROUND_TO_NEAREST_CARRIER_125NS( 575, NEC_CARRIER_PERIOD_125NS)
#define NEC_PAUSE_WIDTH_1               IR_TX_ROUND_TO_NEAREST_CARRIER_125NS(1690, NEC_CARRIER_PERIOD_125NS)

#define NEC_START_PULSE_WIDTH           IR_TX_ROUND_TO_NEAREST_CARRIER_125NS(9000, NEC_CARRIER_PERIOD_125NS)
#define NECX_START_PULSE_WIDTH          IR_TX_ROUND_TO_NEAREST_CARRIER_125NS(4500, NEC_CARRIER_PERIOD_125NS)
#define NEC_START_PAUSE_WIDTH           IR_TX_ROUND_TO_NEAREST_CARRIER_125NS(4500, NEC_CARRIER_PERIOD_125NS)

#define NEC_DITTO_START_WIDTH           NEC_START_PULSE_WIDTH
#define NECX_DITTO_START_WIDTH          NECX_START_PULSE_WIDTH
#define NEC_DITTO_PAUSE_WIDTH           IR_TX_ROUND_TO_NEAREST_CARRIER_125NS(2250, NEC_CARRIER_PERIOD_125NS)

#define NEC_END_PULSE_WIDTH             NEC_DATA_PULSE_WIDTH

#define NEC_IS_TYPE_2(x)                ((x) & IR_TX_NEC_TYPE2)
#define NEC_IS_TYPE_X(x)                ((x) & IR_TX_NEC_TYPEx)

const gpIrTx_TransmissionTimingData_t necTimings[] =
{
    /* 0 */ { NEC_DATA_PULSE_WIDTH, NEC_PAUSE_WIDTH_0 },
    /* 1 */ { NEC_DATA_PULSE_WIDTH, NEC_PAUSE_WIDTH_1 },
};

#define IR_TX_PROTOCOL_NEC_DEFINITION  { gpIrTx_IrComponent_Nec, NEC_REPEAT_INTERVAL_MS, NEC_CARRIER_PERIOD_125NS, necTransmit }

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

// config for predefined
typedef struct {
    const IrTx_ProtocolInfo_t* info;
          gpIrTx_Config_t      conf;
} IrTx_ConfigurationPredefined_t;

#ifdef GP_IR_TX_DIVERSITY_PREDEFINED
static IrTx_ConfigurationPredefined_t currentConfig;
#endif //GP_IR_TX_DIVERSITY_PREDEFINED

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
gpIrTx_ConfigMode_t     IrTx_ConfigMode;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifdef GP_IR_TX_DIVERSITY_PREDEFINED
static void IrTx_HandleRepeatPredefined(void);
#endif // GP_IR_TX_DIVERSITY_PREDEFINED

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/
void necTransmit(gpIrTx_Config_t* config, UInt8 irFlags)
{
#define function  config->function
#define type      config->spec.nec.necType
#define device    config->spec.nec.device
#define subdevice config->spec.nec.subDevice

    if ((irFlags & IR_TX_FLAGS_REPEAT) && !NEC_IS_TYPE_2(type))
    {
        IR_TX_PREDEFINED_SEND_IR_SET_CLR(NEC_IS_TYPE_X(type) ? NECX_DITTO_START_WIDTH : NEC_DITTO_START_WIDTH, NEC_DITTO_PAUSE_WIDTH);
    }
    else
    {
        UInt32 packet;
       
        packet  = (UInt16) subdevice << 8 | device;
        packet |= (UInt32) ((UInt16) ~function << 8 | function) << 16;

        IR_TX_PREDEFINED_SEND_IR_SET_CLR(NEC_IS_TYPE_X(type) ? NECX_START_PULSE_WIDTH : NEC_START_PULSE_WIDTH, NEC_START_PAUSE_WIDTH);
        IrTx_SendSequence(necTimings, sizeof(necTimings), 1, 32, packet);
    }
    IR_TX_PREDEFINED_SEND_IR_SET_CLR(NEC_END_PULSE_WIDTH, 0);

#undef subdevice
#undef device
#undef type
#undef function
}


#ifdef GP_IR_TX_DIVERSITY_PREDEFINED
static void IrTx_HandleRepeatPredefined(void)
{
    Bool sendIr = false; //false == IRÃâ·Âstop.

    if(currentConfig.conf.numberMinRepeats)
    {
        sendIr = true;
        currentConfig.conf.numberMinRepeats--;
    }
    else if (currentConfig.conf.repeated != 0 && gpIrTx_cbIsKeyPressed())
    {
         sendIr = true;
    }

    if(sendIr)
    {
        IrTx_PrepareReTransmit(currentConfig.info->carrier_125ns);

        gpSched_ScheduleEvent(MS_TO_US(currentConfig.info->repeat_ms), IrTx_HandleRepeatPredefined);
        currentConfig.info->transmitFun(&currentConfig.conf, IR_TX_FLAGS_REPEAT);

        IrTx_TriggerIR();
        IrTx_DisableIRCarrier();
    }
    else
    {
#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
        gpHal_ReleasePbmMemory();
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
        gpIrTx_cbSendCommandConfirmPredefined(
                        gpIrTx_ResultSuccess,
                        currentConfig.conf.repeated != 0,
                        currentConfig.conf.function
                    );
    }
}
#endif //GP_IR_TX_DIVERSITY_PREDEFINED

/*****************************************************************************
 *                    Additional Static Data Definitions
 *****************************************************************************/

const IrTx_ProtocolInfo_t IrTx_ProtocolInfos[] = {
	IR_TX_PROTOCOL_NEC_DEFINITION,
};

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/


void gpIrTx_Init(void)
{
    IrTx_ConfigMode = gpIrTx_ConfigModeGp;

    IrTx_InitGeneric();
    IrTx_InitDrv();
    IrTx_InitBuffers();

}

void gpIrTx_SetConfigMode(gpIrTx_ConfigMode_t mode)
{
    IrTx_ConfigMode = mode;
}

gpIrTx_ConfigMode_t gpIrTx_GetConfigMode(void)
{
    return IrTx_ConfigMode;
}


#define IR_TX_NUM_PROTOCOLS   (sizeof(IrTx_ProtocolInfos)/sizeof(IrTx_ProtocolInfos[0]))

/*
 * TODO: does not work for bi-phase codes such as RC-5
 */
void IrTx_SendSequence_DrvFull(
                const gpIrTx_TransmissionTimingData_t* pTimingData,
                      UInt8                            numTimingBits,
                      UInt8                            length,
                      UInt32                           bitSequence
            )
{
    IntLoop i;

    GP_ASSERT_DEV_INT(length % numTimingBits == 0);

    for (i = 0; i < length; i += numTimingBits)
    {
        int index = bitSequence & ((1 << numTimingBits) - 1);

        IR_TX_PREDEFINED_SEND_IR_SET_CLR(pTimingData[index].set, pTimingData[index].clr);
        bitSequence >>= numTimingBits;
    }
}

UInt8 IrTx_CalcNibbleChecksum(UInt32 packet)
{
    int sum = 0;

    while (packet != 0)
    {
        sum     += packet & 0xF;
        packet >>= 4;
    }
    return (16 - sum) & 0xF;
}

#ifdef GP_IR_TX_DIVERSITY_PREDEFINED
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

Bool gpIrTx_PredefinedCommandBusy(void)
{
    return gpSched_ExistsEvent(IrTx_HandleRepeatPredefined);
}

void gpIrTx_SendCommandRequestPredefined(
                gpIrTx_IrComponentType_t type,
                gpIrTx_Config_t          config
            )
{
          IntLoop              i;
    const IrTx_ProtocolInfo_t* pInfo = NULL;
          Bool                 noRepeat =
                                          false;

    // Are we idle?
    if (gpIrTx_PredefinedCommandBusy()
     || gpIrTx_GenericCommandBusy()
        )
    {
        gpIrTx_cbSendCommandConfirmPredefined(gpIrTx_ResultBusy, config.repeated != 0, config.function);
        return;
    }
    GP_ASSERT_DEV_EXT(config.numberMinRepeats < 0x80);

    for (i = 0; i < IR_TX_NUM_PROTOCOLS; ++i)
    {
        if (IrTx_ProtocolInfos[i].type == type)
        {
            pInfo = &IrTx_ProtocolInfos[i];
            break;
        }
    }
    if(pInfo == NULL)
    {
        gpIrTx_cbSendCommandConfirmPredefined(gpIrTx_ResultInvalidParameter, config.repeated != 0, config.function);
        return;
    }

#ifdef GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
    if(! gpHal_ClaimPbmMemory())
    {
        gpIrTx_cbSendCommandConfirmPredefined(gpIrTx_ResultBusy, config.repeated != 0, config.function);
    }
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

    IrTx_PrepareTransmit(pInfo->carrier_125ns);

    gpSched_ScheduleEvent(noRepeat ? 0 : MS_TO_US(pInfo->repeat_ms), IrTx_HandleRepeatPredefined);
    pInfo->transmitFun(&config, 0);

    IrTx_TriggerIR();
    IrTx_DisableIRCarrier();

    currentConfig.info = pInfo;
    currentConfig.conf = config;
}

Bool gpIrTx_CancelRequestPredefined(void)
{
    if (gpSched_UnscheduleEvent(IrTx_HandleRepeatPredefined))
    {
        gpSched_ScheduleEvent(0, IrTx_HandleRepeatPredefined);
        return true;
    }
    return false;
}
#endif //GP_IR_TX_DIVERSITY_PREDEFINED

