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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/inc/gpHal_kx_MAC.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPHAL_KX_MAC_H_
#define _GPHAL_KX_MAC_H_

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//testspec for IEEE 802.15.4 states that when CCA mode 1 is used, the CCA treshold level should be below -69dBm. (Spec states, -75dBm, but 6dB margin is added)
//So in regular cases treshold will be at -70dBm, worst case it is -72dBm and best case it is at -69dBm.
#ifndef GP_HAL_DIVERSITY_CCA_THRESHOLD_VALUE
#define GPHAL_CCA_THRESHOLD_VALUE       (-69)
#else
#define GPHAL_CCA_THRESHOLD_VALUE GP_HAL_DIVERSITY_CCA_THRESHOLD_VALUE
#endif //GP_HAL_DIVERSITY_CCA_THRESHOLD_VALUE

//  GP constants
#define GP_RX_PBM_DATA_SIZE             GP_MM_RAM_PBM_0_DATA_SIZE

//Ack pbm address
#define GPHAL_ACK_PBM_ADDR              GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(7)

/*rap_pbe2optsbase(pbmEntry)*/
#define GP_HAL_PBM_ENTRY2ADDR_OPT_BASE(pbmEntry)    ((gpHal_Address_t)(GP_MM_RAM_PBM_OPTS_START + ((gpHal_Address_t)(pbmEntry))*GP_MM_RAM_PBM_OPTS_OFFSET))
/*rap_pbe2database(UInt8 pbe)*/
#define GP_HAL_PBM_ENTRY2ADDR_DATA_BASE(pbmEntry)   ((gpHal_Address_t)(GP_MM_RAM_PBM_0_DATA_START + ((gpHal_Address_t)(pbmEntry))*GP_MM_RAM_PBM_0_DATA_SIZE))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

//PA settings k7
typedef struct gpHal_PaSettings_s {
    UInt8 PbmSettingAntselInt;
    Int8 internalDbmSetting;
    Bool  pa_low;
    Bool  pa_ultralow;
} gpHal_PaSettings_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

extern gpHal_BusyTXCallback_t               gpHal_BusyTXCallback;
extern gpHal_EmptyQueueCallback_t           gpHal_EmptyQueueCallback;
extern UInt8 gpHal_MacState;
extern UInt8 gpHal_awakeCounter;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

void gpHal_DataRequest_base(UInt8 pbmHandle);
void gpHalMac_CalibrateChannel(gpHal_SourceIdentifier_t srcId, UInt8 channel);

void gpHal_HandlePioEvent(UInt8 PBMentry, UInt8 EventType);
void gpHal_InitMAC(Bool timedMAC);

void gpHalMac_RecalibrateChannels(void);
void gpHalMac_ConfigurePaSettings(gpHal_TxPower_t txPower, UInt8 channel, gpHal_PaSettings_t* pPaSettings);

#endif //_GPHAL_KX_MAC_H_
