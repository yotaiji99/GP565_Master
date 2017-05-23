/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
 *
 * gpMacDispatcher.c
 *   This file contains the implementation of the MAC dispatcher.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacDispatcher/src/gpMacDispatcher_SingleStackStubs.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#include "gpMacDispatcher.h"
#include "gpMacDispatcher_def.h"


/*****************************************************************************
 *                    Application callbacks
 *****************************************************************************/

void gpMacCore_cbDataConfirm(gpMacCore_Result_t status, gpPd_Handle_t pdHandle)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.dataConfirmCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.dataConfirmCallback(status, pdHandle) ;
    }
}

void gpMacCore_cbDataIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 dsn,
                                gpMacCore_Security_t *pSecOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.dataIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.dataIndicationCallback(pSrcAddrInfo, pDstAddrInfo, dsn, pSecOptions, pdLoh) ;
    }
}

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
void gpMacCore_cbPollConfirm( gpMacCore_Result_t status, gpMacCore_AddressInfo_t* pCoordAddrInfo, gpPd_TimeStamp_t txTime  )
{
    if(gpMacDispatcher_StackMapping[0].callbacks.pollConfirmCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.pollConfirmCallback(status, pCoordAddrInfo, txTime) ;
    }
}
#endif

#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
void gpMacCore_cbPollIndication(gpMacCore_AddressInfo_t* pAddrInfo, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.pollIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.pollIndicationCallback(pAddrInfo, rxTime) ;
    }
}
#endif // GP_MACCORE_DIVERSITY_POLL_RECIPIENT


#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
// This function call is the result of an active scan request - invoke only the stack, which is having the dispatcher lock
void gpMacCore_cbBeaconNotifyIndication( UInt8 bsn, gpMacCore_PanDescriptor_t* pPanDescriptor, gpMacCore_StackId_t stackId , UInt8 beaconPayloadLength , UInt8 *pBeaconPayload )
{
    if(gpMacDispatcher_StackMapping[0].callbacks.beaconNotifyIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.beaconNotifyIndicationCallback(bsn, pPanDescriptor, beaconPayloadLength , pBeaconPayload );
    }
}

void gpMacCore_cbScanConfirm(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, UInt32 unscannedChannels, UInt8 resultListSize, UInt8* pEdScanResultList)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.scanConfirmCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.scanConfirmCallback(status, scanType, unscannedChannels, resultListSize, pEdScanResultList) ;
    }
}

#endif // GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
void gpMacCore_cbAssociateConfirm(UInt16 assocShortAddress, gpMacCore_AssocStatus_t status, gpPd_TimeStamp_t txTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.assocConfirmCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.assocConfirmCallback(assocShortAddress, status, txTime) ;
    }
}
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
void gpMacCore_cbAssociateIndication(gpMacCore_Address_t* pDeviceAddress, UInt8 capabilityInformation, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.assocIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.assocIndicationCallback(pDeviceAddress, capabilityInformation, rxTime) ;
    }
}

void gpMacCore_cbAssociateCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.associateCommStatusIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.associateCommStatusIndicationCallback(pSrcAddrInfo, pDstAddrInfo, status, txTime) ;
    }
}
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
void gpMacCore_cbOrphanCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t txTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.orphanCommStatusIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.orphanCommStatusIndicationCallback(pSrcAddrInfo, pDstAddrInfo, status, txTime) ;
    }
}
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT

void gpMacCore_cbOrphanIndication(MACAddress_t* pOrphanAddress, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t rxTime)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.orphanIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.orphanIndicationCallback(pOrphanAddress, rxTime) ;
    }
}

void gpMacCore_cbSecurityFailureCommStatusIndication(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpMacCore_StackId_t stackId, gpPd_TimeStamp_t time)
{
    if(gpMacDispatcher_StackMapping[0].callbacks.securityFailureCommStatusIndicationCallback)
    {
        gpMacDispatcher_StackMapping[0].callbacks.securityFailureCommStatusIndicationCallback(pSrcAddrInfo, pDstAddrInfo, status, time) ;
    }
}

Bool gpMacCore_cbValidStack(UInt8 stackId)
{
    return (stackId != GP_MAC_DISPATCHER_INVALID_STACK_ID);
}

