/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
 *
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacDispatcher/src/gpMacDispatcher_def.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#ifndef _GP_MACDISPATCHERDEF_H_
#define _GP_MACDISPATCHERDEF_H_

#include "global.h"
#include "gpMacCore.h"
#include "gpMacDispatcher.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef struct gpMacDispatcher_StackMapping_s {
    gpMacDispatcher_StringIdentifier_t  stringId;
    gpMacDispatcher_StackId_t           numericId;
    gpMacDispatcher_Callbacks_t         callbacks;
} gpMacDispatcher_StackMapping_t;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

extern gpMacDispatcher_StackMapping_t gpMacDispatcher_StackMapping[GP_DIVERSITY_NR_OF_STACKS];

#if GP_DIVERSITY_NR_OF_STACKS > 1
gpMacDispatcher_Callbacks_t* MacDispatcher_GetCallbacks(UInt8 stackId);
#else
#define MacDispatcher_GetCallbacks(stackId) (&gpMacDispatcher_StackMapping[0].callbacks)
#endif //GP_DIVERSITY_NR_OF_STACKS > 1

void MacDispatcher_InitStackMapping(void);
void MacDispatcher_UnRegisterCallbacks(UInt8 stackId);

//Data callbacks

//Stack 0
void gpMacDispatcher_cbDataIndication_stack0(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 dsn,
                                            gpMacCore_Security_t *pSecOptions, gpPd_Loh_t pdLoh );
void gpMacDispatcher_cbDataConfirm_stack0(gpMacCore_Result_t status, UInt8 pdHandle);
void gpMacDispatcher_cbPollIndication_stack0( gpMacCore_AddressInfo_t* pAddrInfo, gpPd_TimeStamp_t rxTime);
void gpMacDispatcher_cbPollConfirm_stack0( gpMacCore_Result_t status, gpMacCore_AddressInfo_t* pCoordAddrInfo, gpPd_TimeStamp_t txTime );
void gpMacDispatcher_cbPurgeConfirm_stack0( gpMacCore_Result_t status, gpPd_Handle_t pdHandle);

//Stack 1
void gpMacDispatcher_cbDataIndication_stack1(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 dsn,
                                            gpMacCore_Security_t *pSecOptions, gpPd_Loh_t pdLoh );
void gpMacDispatcher_cbDataConfirm_stack1(gpMacCore_Result_t status, UInt8 pdHandle);
void gpMacDispatcher_cbPollIndication_stack1( gpMacCore_AddressInfo_t* pAddrInfo, gpPd_TimeStamp_t rxTime);
void gpMacDispatcher_cbPollConfirm_stack1( gpMacCore_Result_t status, gpMacCore_AddressInfo_t* pCoordAddrInfo, gpPd_TimeStamp_t txTime );
void gpMacDispatcher_cbPurgeConfirm_stack1( gpMacCore_Result_t status, gpPd_Handle_t pdHandle);

//Misc callbacks

//Stack 0
void gpMacDispatcher_cbBeaconNotifyIndication_stack0(UInt8 bsn, gpMacCore_PanDescriptor_t* pPanDescriptor, UInt8 beaconPayloadLength , UInt8 *pBeaconPayload );
void gpMacDispatcher_cbAssociateIndication_stack0(gpMacCore_Address_t* pDeviceAddress, UInt8 capabilityInformation, gpPd_TimeStamp_t rxTime);
void gpMacDispatcher_cbAssociateConfirm_stack0(UInt16 assocShortAddress, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbScanConfirm_stack0(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, UInt32 unscannedChannels, UInt8 resultListSize, UInt8* pEdScanResultList);
void gpMacDispatcher_cbOrphanIndication_stack0(MACAddress_t* pOrphanAddress, gpPd_TimeStamp_t rxTime);
void gpMacDispatcher_cbSecurityFailureCommStatusIndication_stack0(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbAssociateCommStatusIndication_stack0(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbOrphanCommStatusIndication_stack0   (gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbDriverResetIndication_stack0(gpMacCore_Result_t status);


//Stack 1
void gpMacDispatcher_cbBeaconNotifyIndication_stack1(UInt8 bsn, gpMacCore_PanDescriptor_t* pPanDescriptor, UInt8 beaconPayloadLength , UInt8 *pBeaconPayload );
void gpMacDispatcher_cbAssociateIndication_stack1(gpMacCore_Address_t* pDeviceAddress, UInt8 capabilityInformation, gpPd_TimeStamp_t rxTime);
void gpMacDispatcher_cbAssociateConfirm_stack1(UInt16 assocShortAddress, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbScanConfirm_stack1(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, UInt32 unscannedChannels, UInt8 resultListSize, UInt8* pEdScanResultList);
void gpMacDispatcher_cbOrphanIndication_stack1(MACAddress_t* pOrphanAddress, gpPd_TimeStamp_t rxTime);
void gpMacDispatcher_cbSecurityFailureCommStatusIndication_stack1(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbAssociateCommStatusIndication_stack1(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbOrphanCommStatusIndication_stack1(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
void gpMacDispatcher_cbDriverResetIndication_stack1(gpMacCore_Result_t status);

#endif //_GP_MACDISPATCHERDEF_H_

