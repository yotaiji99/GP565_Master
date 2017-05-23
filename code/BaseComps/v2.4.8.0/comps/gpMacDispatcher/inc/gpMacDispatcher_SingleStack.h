/*
 * Copyright (c) 2013-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacDispatcher/inc/gpMacDispatcher_SingleStack.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_MACDISPATCHER_SINGLESTACK_H_
#define _GP_MACDISPATCHER_SINGLESTACK_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Single stack mappings
 *****************************************************************************/

// general functions
#define gpMacDispatcher_Init()                                  do {} while(false)
#define gpMacDispatcher_DeInit()                                  do {} while(false)
#define gpMacDispatcher_Reset                                   gpMacCore_Reset
#define gpMacDispatcher_LockClaim(id)                           true
#define gpMacDispatcher_LockRelease(id)
#define gpMacDispatcher_LockedByThisStack(id)                   false
#define gpMacDispatcher_Locked(id)                              false
#define gpMacDispatcher_RegisterNetworkStack(id)                (gpMacCore_StackAdded(0), 0)
#define gpMacDispatcher_UnRegisterNetworkStack(id)              (gpMacCore_StackRemoved(0), gpMacCore_ResultSuccess)
#define gpMacDispatcher_DataRequest                             gpMacCore_DataRequest
#define gpMacDispatcher_ScanRequest                             gpMacCore_ScanRequest
#define gpMacDispatcher_Start                                   gpMacCore_Start
#define gpMacDispatcher_AssociateRequest                        gpMacCore_AssociateRequest
#define gpMacDispatcher_AssociateResponse                       gpMacCore_AssociateResponse
#define gpMacDispatcher_PollRequest                             gpMacCore_PollRequest
#define gpMacDispatcher_OrphanResponse                          gpMacCore_OrphanResponse
#define gpMacDispatcher_PurgeRequest(pdHandle, stackId)         gpMacCore_PurgeRequest(pdHandle, stackId)

// getters and setters
#define gpMacDispatcher_SetCurrentChannel(channel, stackId)             gpMacCore_SetCurrentChannel(channel, stackId)
#define gpMacDispatcher_GetCurrentChannel(stackId)                      gpMacCore_GetCurrentChannel(stackId)
#define gpMacDispatcher_SetDefaultTransmitPowers(pDefaultTxPow)         gpMacCore_SetDefaultTransmitPowers(pDefaultTxPow)
#define gpMacDispatcher_SetTransmitPower(transmitPower, stackId)        gpMacCore_SetTransmitPower(transmitPower, stackId)
#define gpMacDispatcher_GetTransmitPower(stackId)                       gpMacCore_GetTransmitPower(stackId)
#define gpMacDispatcher_SetCCAMode(cCAMode, stackId)                    gpMacCore_SetCCAMode(cCAMode, stackId)
#define gpMacDispatcher_GetCCAMode(stackId)                             gpMacCore_GetCCAMode(stackId)
#define gpMacDispatcher_SetCoordExtendedAddress(addr, stackId)          gpMacCore_SetCoordExtendedAddress(addr, stackId)
#define gpMacDispatcher_GetCoordExtendedAddress(addr , stackId)         gpMacCore_GetCoordExtendedAddress(addr , stackId)
#define gpMacDispatcher_SetCoordShortAddress(addr, stackId)             gpMacCore_SetCoordShortAddress(addr, stackId)
#define gpMacDispatcher_GetCoordShortAddress(stackId)                   gpMacCore_GetCoordShortAddress(stackId)
#define gpMacDispatcher_SetPanCoordinator(panCoordinator, stackId)      gpMacCore_SetPanCoordinator(panCoordinator, stackId)
#define gpMacDispatcher_GetPanCoordinator(stackId)                      gpMacCore_GetPanCoordinator(stackId)
#define gpMacDispatcher_SetDsn(dsn, stackId)                            gpMacCore_SetDsn(dsn, stackId)
#define gpMacDispatcher_GetDsn(stackId)                                 gpMacCore_GetDsn(stackId)
#define gpMacDispatcher_SetMaxCsmaBackoffs(maxCsmaBackoffs, stackId)    gpMacCore_SetMaxCsmaBackoffs(maxCsmaBackoffs, stackId)
#define gpMacDispatcher_GetMaxCsmaBackoffs(stackId)                     gpMacCore_GetMaxCsmaBackoffs(stackId)
#define gpMacDispatcher_SetMinBE(minBE, stackId)                        gpMacCore_SetMinBE(minBE, stackId)
#define gpMacDispatcher_GetMinBE(stackId)                               gpMacCore_GetMinBE(stackId)
#define gpMacDispatcher_SetMaxBE(maxBE, stackId)                        gpMacCore_SetMaxBE(maxBE, stackId)
#define gpMacDispatcher_GetMaxBE(stackId)                               gpMacCore_GetMaxBE(stackId)
#define gpMacDispatcher_SetPanId(panId, stackId)                        gpMacCore_SetPanId(panId, stackId)
#define gpMacDispatcher_GetPanId(stackId)                               gpMacCore_GetPanId(stackId)
#define gpMacDispatcher_SetRxOnWhenIdle(rxOnWhenIdle, stackId)          gpMacCore_SetRxOnWhenIdle(rxOnWhenIdle, stackId)
#define gpMacDispatcher_GetRxOnWhenIdle(stackId)                        gpMacCore_GetRxOnWhenIdle(stackId)
#define gpMacDispatcher_SetShortAddress(shortAddress, stackId)          gpMacCore_SetShortAddress(shortAddress, stackId)
#define gpMacDispatcher_GetShortAddress(stackId)                        gpMacCore_GetShortAddress(stackId)
#define gpMacDispatcher_SetAssociationPermit(permit, stackId)           gpMacCore_SetAssociationPermit(permit, stackId)
#define gpMacDispatcher_GetAssociationPermit(stackId)                   gpMacCore_GetAssociationPermit(stackId)
#define gpMacDispatcher_SetBeaconPayload(pBeaconPayload, stackId)       gpMacCore_SetBeaconPayload(pBeaconPayload, stackId)
#define gpMacDispatcher_GetBeaconPayload(pBeaconPayload, stackId)       gpMacCore_GetBeaconPayload(pBeaconPayload, stackId)
#define gpMacDispatcher_SetBeaconPayloadLength(length, stackId)         gpMacCore_SetBeaconPayloadLength(length, stackId)
#define gpMacDispatcher_GetBeaconPayloadLength(stackId)                 gpMacCore_GetBeaconPayloadLength(stackId)
#define gpMacDispatcher_SetPromiscuousMode(promiscuousMode, stackId)    gpMacCore_SetPromiscuousMode(promiscuousMode, stackId)
#define gpMacDispatcher_GetPromiscuousMode(stackId)                     gpMacCore_GetPromiscuousMode(stackId)
#define gpMacDispatcher_SetTransactionPersistenceTime(time, stackId)    gpMacCore_SetTransactionPersistenceTime(time, stackId)
#define gpMacDispatcher_GetTransactionPersistenceTime(stackId)          gpMacCore_GetTransactionPersistenceTime(stackId)
#define gpMacDispatcher_SetExtendedAddress(addr)                        gpMacCore_SetExtendedAddress(addr)
#define gpMacDispatcher_GetExtendedAddress(addr)                        gpMacCore_GetExtendedAddress(addr)
#define gpMacDispatcher_SetNumberOfRetries(numberOfRetries, stackId)    gpMacCore_SetNumberOfRetries(numberOfRetries, stackId)
#define gpMacDispatcher_GetNumberOfRetries(stackId)                     gpMacCore_GetNumberOfRetries(stackId)
#define gpMacDispatcher_SetSecurityEnabled(securityEnabled, stackId)    gpMacCore_SetSecurityEnabled(securityEnabled, stackId)
#define gpMacDispatcher_GetSecurityEnabled(stackId)                     gpMacCore_GetSecurityEnabled(stackId)
#define gpMacDispatcher_SetBeaconStarted(BeaconStarted, stackId)        gpMacCore_SetBeaconStarted(BeaconStarted, stackId)
#define gpMacDispatcher_GetBeaconStarted(stackId)                       gpMacCore_GetBeaconStarted(stackId)
#define gpMacDispatcher_SetTxAntenna(txAntenna, stackId)                gpMacCore_SetTxAntenna(txAntenna, stackId)
#define gpMacDispatcher_GetTxAntenna(stackId)                           gpMacCore_GetTxAntenna(stackId)
#define gpMacDispatcher_SetMacVersion(macVersion, stackId)              gpMacCore_SetMacVersion(macVersion, stackId)
#define gpMacDispatcher_GetMacVersion(stackId)                          gpMacCore_GetMacVersion(stackId)
#define gpMacDispatcher_SetIndicateBeaconNotifications(enable, stackId) gpMacCore_SetIndicateBeaconNotifications(enable,stackId)
#define gpMacDispatcher_GetIndicateBeaconNotifications(stackId)         gpMacCore_GetIndicateBeaconNotifications(stackId)
#define gpMacDispatcher_SetForwardPollIndications(enable, stackId)      gpMacCore_SetForwardPollIndications(enable,stackId)
#define gpMacDispatcher_GetForwardPollIndications(stackId)              gpMacCore_GetForwardPollIndications(stackId)
#define gpMacDispatcher_DataPending_QueueAdd(pAddrInfo, stackId)        gpMacCore_DataPending_QueueAdd(pAddrInfo, stackId)
#define gpMacDispatcher_DataPending_QueueRemove(pAddrInfo, stackId)     gpMacCore_DataPending_QueueRemove(pAddrInfo, stackId)


#define  gpMacDispatcher_SetFrameCounter(frameCounter, stackId)                                gpMacCore_SetFrameCounter(frameCounter)
#define  gpMacDispatcher_GetFrameCounter(stackId)                                              gpMacCore_GetFrameCounter()
#define  gpMacDispatcher_SetKeyDescriptor(pKeyDescriptor, index, stackId)                      gpMacCore_SetKeyDescriptor(pKeyDescriptor, index)
#define  gpMacDispatcher_GetKeyDescriptor(pKeyDescriptor, index, stackId)                      gpMacCore_GetKeyDescriptor(pKeyDescriptor, index)
#define  gpMacDispatcher_SetKeyTableEntries(keyTableEntries, stackId)                          gpMacCore_SetKeyTableEntries(keyTableEntries)
#define  gpMacDispatcher_GetKeyTableEntries(stackId)                                           gpMacCore_GetKeyTableEntries()
#define  gpMacDispatcher_SetDeviceDescriptor(pDeviceDescriptor, index, stackId)                gpMacCore_SetDeviceDescriptor(pDeviceDescriptor, index)
#define  gpMacDispatcher_GetDeviceDescriptor(pDeviceDescriptor , index, stackId)               gpMacCore_GetDeviceDescriptor(pDeviceDescriptor , index)
#define  gpMacDispatcher_SetDeviceTableEntries(deviceTableEntries, stackId)                    gpMacCore_SetDeviceTableEntries(deviceTableEntries)
#define  gpMacDispatcher_GetDeviceTableEntries(stackId)                                        gpMacCore_GetDeviceTableEntries()
#define  gpMacDispatcher_SetSecurityLevelDescriptor(pSecurityLevelDescriptor , index, stackId) gpMacCore_SetSecurityLevelDescriptor(pSecurityLevelDescriptor , index)
#define  gpMacDispatcher_GetSecurityLevelDescriptor(pSecurityLevelDescriptor , index, stackId) gpMacCore_GetSecurityLevelDescriptor(pSecurityLevelDescriptor , index)
#define  gpMacDispatcher_GetSecurityLevelTableEntries(stackId)                                 gpMacCore_GetSecurityLevelTableEntries()
#define  gpMacDispatcher_SetSecurityLevelTableEntries(securityLevelTableEntries, stackId)      gpMacCore_SetSecurityLevelTableEntries(securityLevelTableEntries)
#define  gpMacDispatcher_SetDefaultKeySource(pDefaultKeySource, stackId)                       gpMacCore_SetDefaultKeySource(pDefaultKeySource)
#define  gpMacDispatcher_GetDefaultKeySource(pDefaultKeySource, stackId)                       gpMacCore_GetDefaultKeySource(pDefaultKeySource)
#define  gpMacDispatcher_SetPanCoordExtendedAddress(pPanCoordExtendedAddress, stackId)         gpMacCore_SetPanCoordExtendedAddress(pPanCoordExtendedAddress)
#define  gpMacDispatcher_GetPanCoordExtendedAddress(pPanCoordExtendedAddress, stackId)         gpMacCore_GetPanCoordExtendedAddress(pPanCoordExtendedAddress)
#define  gpMacDispatcher_SetPanCoordShortAddress(PanCoordShortAddress, stackId)                gpMacCore_SetPanCoordShortAddress(PanCoordShortAddress)
#define  gpMacDispatcher_GetPanCoordShortAddress(stackId)                                      gpMacCore_GetPanCoordShortAddress()

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#endif // _GP_MACDISPATCHER_SINGLESTACK_H_
