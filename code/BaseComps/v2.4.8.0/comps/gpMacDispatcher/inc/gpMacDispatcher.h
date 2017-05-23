/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 * gpMacDispatcher.h
 *   This file contains the definitions of the public functions and enumerations of the gpMacDispatcher.(based on IEEE802.15.4-2006)
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacDispatcher/inc/gpMacDispatcher.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GP_MACDISPATCHER_H_
#define _GP_MACDISPATCHER_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpMacCore.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @brief The invalid stack ID. */
#define GP_MAC_DISPATCHER_INVALID_STACK_ID       (GP_MACCORE_STACK_UNDEFINED)
#define GP_MAC_DISPATCHER_STRING_ID_LENGTH        5

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @name gpMacDispatcher_Result_t */
//@{
//PHY results
/** @brief The requested operation was completed successfully. For instance if a transmission was requested, this value indicates a successful transmission.*/
#define gpMacDispatcher_ResultSuccess                 gpMacCore_ResultSuccess
//MAC results
/** @brief The frame counter purportedly applied by the originator of the received frame is invalid. */
#define gpMacDispatcher_ResultCounterError            gpMacCore_ResultCounterError
/** @brief The key purportedly applied by the originator of the received frame is not allowed to be used with that frame type according to the key usage policy of the recipient.*/
#define gpMacDispatcher_ResultImproperKeyType         gpMacCore_ResultImproperKeyType
/** @brief The security level purportedly applied by the originator of the received frame does not meet the minimum security level required/expected by the recipient for that frame type.*/
#define gpMacDispatcher_ResultImproperSecurityLevel   gpMacCore_ResultImproperSecurityLevel
/** @brief The received frame was purportedly secured using security based on IEEE Std 802.15.4-2003, and such security is not supported by this standard. */
#define gpMacDispatcher_ResultUnsupportedLegacy       gpMacCore_ResultUnsupportedLegacy
/** @brief The security purportedly applied by the originator of the received frame is not supported. */
#define gpMacDispatcher_ResultUnsupportedSecurity     gpMacCore_ResultUnsupportedSecurity
/** @brief The beacon was lost following a synchronization request. */
#define gpMacDispatcher_ResultBeaconLoss              gpMacCore_ResultBeaconLoss
/** @brief A transmission could not take place due to activity on the channel, i.e., the CSMA-CA mechanism has failed. */
#define gpMacDispatcher_ResultChannelAccessFailure    gpMacCore_ResultChannelAccessFailure
/** @brief The GTS request has been denied by the PAN coordinator. */
#define gpMacDispatcher_ResultDenied                  gpMacCore_ResultDenied
/** @brief The attempt to disable the transceiver has failed. */
#define gpMacDispatcher_ResultDisableTrxFailure       gpMacCore_ResultDisableTrxFailure
/** @brief The received frame induces a failed security check according to the security suite. */
#define gpMacDispatcher_ResultSecurityError           gpMacCore_ResultSecurityError
/** @brief The frame resulting from secure processing has a length that is greater than aMACMaxFrameSize. */
#define gpMacDispatcher_ResultFrameTooLong            gpMacCore_ResultFrameTooLong
/** @brief The requested GTS transmission failed because the specified GTS either did not have a transmit GTS direction or was not defined. */
#define gpMacDispatcher_ResultInvalidGTS              gpMacCore_ResultInvalidGTS
/** @brief A request to purge an MSDU from the transaction queue wasmade using an MSDU handle that was not found in the transaction table. */
#define gpMacDispatcher_ResultInvalidHandle           gpMacCore_ResultInvalidHandle
/** @brief A parameter in the primitive is out of the valid range. */
#define gpMacDispatcher_ResultInvalidParameter        gpMacCore_ResultInvalidParameter
/** @brief No acknowledgment was received after aMaxFrameRetries. */
#define gpMacDispatcher_ResultNoAck                   gpMacCore_ResultNoAck
/** @brief A scan operation failed to find any network beacons. */
#define gpMacDispatcher_ResultNoBeacon                gpMacCore_ResultNoBeacon
/** @brief No response data was available following a request. */
#define gpMacDispatcher_ResultNoData                  gpMacCore_ResultNoData
/** @brief The operation failed because a short address was not allocated. */
#define gpMacDispatcher_ResultNoShortAddress          gpMacCore_ResultNoShortAddress
/** @brief A request to enable the receiver was unsuccessful because it could not be completed within the CAP. */
#define gpMacDispatcher_ResultOutOfCAP                gpMacCore_ResultOutOfCAP
/** @brief A PAN identifier conflict has been detected and communicated to the PAN coordinator. */
#define gpMacDispatcher_ResultPanIdConflict           gpMacCore_ResultPanIdConflict
/** @brief A coordinator realignment command has been received. */
#define gpMacDispatcher_ResultRealignment             gpMacCore_ResultRealignment
/** @brief The transaction has expired and its information is discarded. */
#define gpMacDispatcher_ResultTransactionExpired      gpMacCore_ResultTransactionExpired
/** @brief There is no capacity to store the transaction. */
#define gpMacDispatcher_ResultTransactionOverflow     gpMacCore_ResultTransactionOverflow
/** @brief The transceiver was transmitting when the receiver was requested to be enabled. */
#define gpMacDispatcher_ResultTxActive                gpMacCore_ResultTxActive
/** @brief The appropriate key is not available in the ACL. */
#define gpMacDispatcher_ResultUnavailableKey          gpMacCore_ResultUnavailableKey
/** @brief A SET/GET request was issued with the identifier of a PIBattribute that is not supported. */
#define gpMacDispatcher_ResultUnsupportedAttribute    gpMacCore_ResultUnsupportedAttribute
/** @brief A request to send data was unsuccessful because neither the source address parameters nor the destination address parameters were present.*/
#define gpMacDispatcher_ResultInvalidAddress          gpMacCore_ResultInvalidAddress
/** @brief A receiver enable request was unsuccessful because it specified a number of symbols that was longer than the beacon interval. */
#define gpMacDispatcher_ResultOnTimeTooLong           gpMacCore_ResultOnTimeTooLong
/** @brief A receiver enable request was unsuccessful because it could not be completed within the current superframe and was not permitted to be deferred until the next superframe. */
#define gpMacDispatcher_ResultPastTime                gpMacCore_ResultPastTime
/** @brief The device was instructed to start sending beacons based on the timing of the beacon transmissions of its coordinator, but the device is not currently tracking the beacon of its coordinator.*/
#define gpMacDispatcher_ResultTrackingOff             gpMacCore_ResultTrackingOff
/** @brief An attempt to write to a MAC PIB attribute that is in a table failed because the specified table index was out of range.*/
#define gpMacDispatcher_ResultInvalidIndex            gpMacCore_ResultInvalidIndex
/** @brief A scan operation terminated prematurely because the number of PAN descriptors stored reached an implementationspecified maximum.*/
#define gpMacDispatcher_ResultLimitedReached          gpMacCore_ResultLimitedReached
/** @brief A SET/GET request was issued with the identifier of an attribute that is read only. */
#define gpMacDispatcher_ResultReadOnly                gpMacCore_ResultReadOnly
/** @brief A request to perform a scan operation failed because the MLME was in the process of performing a previously initiated scan operation. */
#define gpMacDispatcher_ResultScanInProgress          gpMacCore_ResultScanInProgress
/** @brief The device was instructed to start sending beacons based on the timing of the beacon transmissions of its coordinator, but the instructed start time overlapped the transmission time of the beacon of its coordinator. */
#define gpMacDispatcher_ResultSuperframeOverlap       gpMacCore_ResultSuperframeOverlap

typedef gpMacCore_Result_t gpMacDispatcher_Result_t;

typedef UInt8 gpMacDispatcher_StackId_t;

typedef struct {
    char str[GP_MAC_DISPATCHER_STRING_ID_LENGTH];
} gpMacDispatcher_StringIdentifier_t;

#if GP_DIVERSITY_NR_OF_STACKS > 1 
#include "gpMacDispatcher_MultiStack.h"
#else
#include "gpMacDispatcher_SingleStack.h"
#endif //(GP_DIVERSITY_NR_OF_STACKS == 1)


/** @ingroup MLME_DATA
 * @typedef gpMacDispatcher_cbDataIndication_t
 * @brief Calls the DataIndication callback function. It is used to inform the next higher layer a data packet is received.
 */
typedef void (*gpMacDispatcher_cbDataIndication_t)(const gpMacCore_AddressInfo_t* pSrcAddrInfo, const gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 dsn, gpMacCore_Security_t *pSecOptions, gpPd_Loh_t pdLoh);
/** @ingroup MLME_DATA
 * @typedef gpMacDispatcher_cbDataConfirm_t
 * @brief Calls the DataConfirm callback function. It is used to inform the next higher layer a data packet is sent.
 */
typedef void (*gpMacDispatcher_cbDataConfirm_t)( gpMacDispatcher_Result_t status, gpPd_Handle_t pdHandle);
/** @ingroup MLME_DATA
 * @typedef gpMacDispatcher_cbPollIndication_t
 * @brief Calls the PollIndication callback function. It is used to inform the next higher layer a Cmd DataRequest is received.
 */
typedef void (*gpMacDispatcher_cbPollIndication_t)( gpMacCore_AddressInfo_t* pAddrInfo, gpPd_TimeStamp_t rxTime);
/** @ingroup MLME_DATA
 * @typedef gpMacDispatcher_cbPollConfirm_t
 * @brief Calls the PollConfirm callback function. It is used to inform the next higher layer a poll request was sent (status).
 */
typedef void (*gpMacDispatcher_cbPollConfirm_t)( gpMacDispatcher_Result_t status, gpMacCore_AddressInfo_t* pAddrInfo, gpPd_TimeStamp_t txTime );

/** @ingroup MLME_DATA
 * @typedef gpMacDispatcher_cbPurgeConfirm_t
 * @brief Calls the PurgeConfirm callback function. It is used to inform the next higher layer a purge request was performed (status).
 */
typedef void (*gpMacDispatcher_cbPurgeConfirm_t)( gpMacDispatcher_Result_t status, gpPd_Handle_t pdHandle );

/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbBeaconNotifyIndication_t
 * @brief Calls the BeaconIndication callback function. It is used to inform the next higher layer a beacon was received.
 */
typedef void (*gpMacDispatcher_cbBeaconNotifyIndication_t)( UInt8 bsn, gpMacCore_PanDescriptor_t* pPanDescriptor,UInt8 beaconPayloadLength , UInt8 *pBeaconPayload );
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbScanConfirm_t
 * @brief Calls the ScanConfirm callback function. It is used to indicate the scan process was finished.
 */
typedef void (*gpMacDispatcher_cbScanConfirm_t)( gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, UInt32 unscannedChannels, UInt8 resultListSize, void* pResultList );
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbAssocConfirm_t
 * @brief Calls the AssocConfirm callback function. It is used to indicate the association process was finished.
 */
typedef void (*gpMacDispatcher_cbAssocConfirm_t)(UInt16 assocShortAddress, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbAssociateIndication_t
 * @brief Calls the AssocIndication callback function. It is used to indicate the association request was received.
 */
typedef void (*gpMacDispatcher_cbAssociateIndication_t)(gpMacCore_Address_t* pDeviceAddress, UInt8 capabilityInformation, gpPd_TimeStamp_t rxTime);
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbOrphanIndication_t
 * @brief Calls the Orphan Indication callback function.
 */
typedef void (*gpMacDispatcher_cbOrphanIndication_t) (MACAddress_t* pOrphanAddress, gpPd_TimeStamp_t rxTime);
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbSecurityFailureCommStatusIndication_t
 * @brief Indicates a security failure communication status to the next higher layer.
 */
typedef void (*gpMacDispatcher_cbSecurityFailureCommStatusIndication_t)(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbAssociateCommStatusIndication_t
 * @brief Indicates a associate response communication status to the next higher layer.
 */
typedef void (*gpMacDispatcher_cbAssociateCommStatusIndication_t)(gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);
/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbOrphanCommStatusIndication_t
 * @brief Indicates an orphan communication status to the next higher layer.
 */
typedef void (*gpMacDispatcher_cbOrphanCommStatusIndication_t)   (gpMacCore_AddressInfo_t* pSrcAddrInfo, gpMacCore_AddressInfo_t* pDstAddrInfo, gpMacCore_Result_t status, gpPd_TimeStamp_t txTime);

/** @ingroup MLME_MISC
 * @typedef gpMacDispatcher_cbOrphanCommStatusIndication_t
 * @brief Indicates an unexpected driver reset to network stacks.
 */
typedef void (*gpMacDispatcher_cbDriverResetIndication_t)   (gpMacCore_Result_t status);

typedef struct gpMacDispatcher_Callbacks_s {
    gpMacDispatcher_cbDataIndication_t          dataIndicationCallback;
    gpMacDispatcher_cbDataConfirm_t             dataConfirmCallback;
    gpMacDispatcher_cbPollIndication_t          pollIndicationCallback;
    gpMacDispatcher_cbPollConfirm_t             pollConfirmCallback;
    gpMacDispatcher_cbPurgeConfirm_t            purgeConfirmCallback;
    gpMacDispatcher_cbBeaconNotifyIndication_t  beaconNotifyIndicationCallback;
    gpMacDispatcher_cbScanConfirm_t             scanConfirmCallback;
    gpMacDispatcher_cbAssociateIndication_t     assocIndicationCallback;
    gpMacDispatcher_cbAssocConfirm_t            assocConfirmCallback;
    gpMacDispatcher_cbOrphanIndication_t        orphanIndicationCallback;
    gpMacDispatcher_cbSecurityFailureCommStatusIndication_t     securityFailureCommStatusIndicationCallback;
    gpMacDispatcher_cbAssociateCommStatusIndication_t           associateCommStatusIndicationCallback;
    gpMacDispatcher_cbOrphanCommStatusIndication_t              orphanCommStatusIndicationCallback;
    gpMacDispatcher_cbDriverResetIndication_t                   driverResetIndicationCallback;
}gpMacDispatcher_Callbacks_t;

/** @brief This function is used to set the callbacks for a specific stack
*/
void gpMacDispatcher_RegisterCallbacks(gpMacDispatcher_StackId_t stackId, gpMacDispatcher_Callbacks_t* pCallbacks);
void gpMacDispatcher_GetCallbacks(gpMacDispatcher_StackId_t stackId, gpMacDispatcher_Callbacks_t* pCallbacks);

void gpMacDispatcher_SetMinInterferenceLevels(UInt8* pInterferenceLevels) ;

#endif // _GP_MACDISPATCHER_H_
