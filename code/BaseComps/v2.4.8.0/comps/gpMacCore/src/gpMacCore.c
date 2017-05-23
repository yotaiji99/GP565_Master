/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 * gpMacCore.c
 *   This file contains the implementation of the non beaconed MAC protocol.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacCore/src/gpMacCore.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG
// General includes
#include "hal.h"
#include "gpHal.h"
#include "gpPd.h"
#include "gpLog.h"
#include "gpBsp.h"
#include "gpAssert.h"
#include "gpPoolMem.h"
#include "gpMacCore.h"
#include "gpMacCore_defs.h"
#include "gpMacCore_enumcheck.h"


#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR)
#include "gpSched.h"
#endif

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_MACCORE

#define SHORT_ADDR_SIZE  sizeof(UInt16)
#define EXTENDED_ADDR_SIZE sizeof(MACAddress_t)

//#define MAX_PENDING_ADDRESSES   7
#define MAC_TRANSMISSION_START_CB()     { if (MacCore_TransmissionStartCb)  MacCore_TransmissionStartCb(); }

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

// Hal callbacks
static void MacCore_HalDataIndication(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo);
static void MacCore_HalDataConfirm(gpHal_Result_t status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed);

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
MacCore_PollReqArgs_t *gpMacCore_pPollReqArgs = NULL;
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
static UInt8 MacCore_BeaconRequestStackId;
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT


#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
static gpMacCore_Result_t MacCore_SendCommandCoordinatorRealignment(MACAddress_t* pDestAddr, UInt16 orphanShortAddress, Bool broadCast, gpMacCore_StackId_t stackId);
#endif
/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
STATIC UInt8 MacCore_AnalyseMacHeader(gpPd_Loh_t* p_PdLoh, MacCore_HeaderDescriptor_t* pMacHeaderDecoded);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
STATIC void MacCore_ProcessBeacon(gpPd_Loh_t pdLoh, MacCore_HeaderDescriptor_t* pMdi);
#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
STATIC void MacCore_ConstructBeacon(gpPd_Loh_t* p_PdLoh, UInt8 stackId);
STATIC void MacCore_TransmitBeacon(gpPd_Loh_t* p_PdLoh, gpHal_RxInfo_t *rxInfo);
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT

STATIC Bool MacCore_RxForThisStack(gpMacCore_StackId_t stackId , gpMacCore_AddressMode_t dstAddrMode, UInt8 rxChannel);

extern gpPd_Handle_t MacCore_IndTxEmptyPacketPdHandle;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

//----------------
//Helper functions
//----------------

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
gpMacCore_Result_t MacCore_SendCommandCoordinatorRealignment(MACAddress_t* pDestAddr, UInt16 orphanShortAddress, Bool broadCast, gpMacCore_StackId_t stackId)
{
    UInt16 coordShortAddress;
    gpMacCore_AddressInfo_t dstAddressInfo;
    gpMacCore_AddressInfo_t srcAddrInfo;
    UInt8 payload[GP_MACCORE_COORDINATOR_REALIGNMENT_CMD_LEN];

    MacCore_InitSrcAddressInfo(&srcAddrInfo, gpMacCore_AddressModeExtendedAddress, stackId);
    coordShortAddress = gpMacCore_GetShortAddress(stackId);
    dstAddressInfo.panId = GP_MACCORE_PANID_BROADCAST;

    if(broadCast)
    {
        dstAddressInfo.addressMode   = gpMacCore_AddressModeShortAddress;
        dstAddressInfo.address.Short = GP_MACCORE_SHORT_ADDR_BROADCAST;
    }
    else
    {
        dstAddressInfo.addressMode = gpMacCore_AddressModeExtendedAddress;
        MEMCPY(&dstAddressInfo.address, pDestAddr, sizeof(MACAddress_t));
    }
    // construct the payload
    payload[0] = gpMacCore_CommandCoordinatorRealignment;
    HOST_TO_LITTLE_UINT16(&srcAddrInfo.panId);
    MEMCPY(&payload[1], &srcAddrInfo.panId, GP_MACCORE_PANID_SIZE);
    HOST_TO_LITTLE_UINT16(&coordShortAddress);
    MEMCPY(&payload[3], &coordShortAddress, GP_MACCORE_SHORT_ADDR_SIZE);
    payload[5] = gpRxArbiter_GetStackChannel(stackId);
    HOST_TO_LITTLE_UINT16(&orphanShortAddress);
    MEMCPY(&payload[6], &orphanShortAddress, GP_MACCORE_SHORT_ADDR_SIZE);

    // send the command
    return MacCore_SendCommand(&dstAddressInfo, &srcAddrInfo, GP_MACCORE_TX_OPT_ACK_REQ, payload, GP_MACCORE_COORDINATOR_REALIGNMENT_CMD_LEN, stackId, gpHal_MacDefault);
}
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT

// check that address modes are valid
STATIC INLINE gpMacCore_Result_t MacCore_ValidAddrModes(gpMacCore_AddressMode_t srcAddrMode, gpMacCore_AddressMode_t dstAddrMode)
{

    if(srcAddrMode == gpMacCore_AddressModeNoAddress && dstAddrMode == gpMacCore_AddressModeNoAddress)
    {
        return gpMacCore_ResultInvalidAddress;
    }

    if( (srcAddrMode == gpMacCore_AddressModeReserved || srcAddrMode > gpMacCore_AddressModeExtendedAddress) ||
        (dstAddrMode == gpMacCore_AddressModeReserved || dstAddrMode > gpMacCore_AddressModeExtendedAddress))
    {
        return gpMacCore_ResultInvalidParameter;
    }

    return gpMacCore_ResultSuccess;
}


STATIC gpMacCore_Result_t MacCore_ValidateClearText(MacCore_HeaderDescriptor_t* dataIndicationMacValues)
{
    UInt8 secLevel;

    if(!MACCORE_FRAMECONTROL_SECURITY_GET(dataIndicationMacValues->frameControl))
    {
        secLevel = gpEncryption_SecLevelNothing;
    }
    else
    {
        if(!MACCORE_FRAMECONTROL_FRAMEVERSION_GET(dataIndicationMacValues->frameControl))
        {
            // In section 7.5.8.2.3 Incoming frame security procedure b) security + old frame version ==> legacy
            return gpMacCore_ResultUnsupportedLegacy;
        }
        secLevel = dataIndicationMacValues->secOptions.securityLevel;
    }
    // In section 7.5.8.2.3 Incoming frame security procedure d) return success or unsupported security, depending on the MAC's security setting
    return ( secLevel == gpEncryption_SecLevelNothing ) ? gpMacCore_ResultSuccess : gpMacCore_ResultUnsupportedSecurity;

}

/*
 * This function will read all data in the MAC header of the packet and write it in the pMacHeaderDecoded structure.
 * After this function has been executed, the p_PdLoh points directly after the MAC header.
 * So make sure to call this function only once on each incoming packet!
 *
 * This functions returns the length of the MAC header or 0xFF in case of an invalid header has been detected.
*/
STATIC UInt8 MacCore_AnalyseMacHeader(gpPd_Loh_t* p_PdLoh, MacCore_HeaderDescriptor_t* pMacHeaderDecoded )
{
    UInt8 startMacHdr = p_PdLoh->offset;
    UInt8 packetLength = p_PdLoh->length;

    // read frame control
    MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->frameControl, 2, p_PdLoh);
    // read sequence number
    gpPd_ReadWithUpdate(p_PdLoh, 1, &pMacHeaderDecoded->sequenceNumber);
    // get frametype
    pMacHeaderDecoded->frameType = (gpMacCore_FrameType_t)MACCORE_FRAMECONTROL_FRAMETYPE_GET(pMacHeaderDecoded->frameControl);
    // get destination address mode
    pMacHeaderDecoded->dstAddrInfo.addressMode = (gpMacCore_AddressMode_t)MACCORE_FRAMECONTROL_DSTADDRMODE_GET(pMacHeaderDecoded->frameControl);
    // get source addressing mode
    pMacHeaderDecoded->srcAddrInfo.addressMode = (gpMacCore_AddressMode_t)MACCORE_FRAMECONTROL_SRCADDRMODE_GET(pMacHeaderDecoded->frameControl);

    if( MacCore_ValidAddrModes(pMacHeaderDecoded->srcAddrInfo.addressMode, pMacHeaderDecoded->dstAddrInfo.addressMode) != gpMacCore_ResultSuccess )
    {
        GP_LOG_PRINTF("Invalid header seen",0);
        return GP_MACCORE_INVALID_HEADER_LENGTH;
    }

    if(pMacHeaderDecoded->dstAddrInfo.addressMode >= gpMacCore_AddressModeShortAddress)
    {
        // Get PAN ID
        MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->dstAddrInfo.panId, 2, p_PdLoh);
        // Get address
        if (pMacHeaderDecoded->dstAddrInfo.addressMode == gpMacCore_AddressModeExtendedAddress )
        {
            // Get extended address
            MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->dstAddrInfo.address.Extended, EXTENDED_ADDR_SIZE, p_PdLoh);
        }
        else
        {
            // Get short address
            MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->dstAddrInfo.address.Short, SHORT_ADDR_SIZE, p_PdLoh);
        }
    }
    // Get stackId
    pMacHeaderDecoded->stackId = MacCore_GetStackId( &pMacHeaderDecoded->dstAddrInfo );

    if (pMacHeaderDecoded->srcAddrInfo.addressMode >= gpMacCore_AddressModeShortAddress )
    {
        // Get PAN ID
        if ( (!(MACCORE_FRAMECONTROL_PANCOMPRESSION_GET(pMacHeaderDecoded->frameControl))
             || (pMacHeaderDecoded->dstAddrInfo.addressMode == gpMacCore_AddressModeNoAddress))
             || (pMacHeaderDecoded->frameType == gpMacCore_FrameTypeBeacon))
        {
            MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->srcAddrInfo.panId, 2, p_PdLoh);
        }
        else
        {
            pMacHeaderDecoded->srcAddrInfo.panId = pMacHeaderDecoded->dstAddrInfo.panId;
        }

        // Get address
        if (pMacHeaderDecoded->srcAddrInfo.addressMode == gpMacCore_AddressModeExtendedAddress)
        {
            // Get extended address
            MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->srcAddrInfo.address.Extended, EXTENDED_ADDR_SIZE, p_PdLoh);
        }
        else
        {
            // Get short address
            MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->srcAddrInfo.address.Short, SHORT_ADDR_SIZE, p_PdLoh);
        }
    }

    // Get security properties
    if(MACCORE_FRAMECONTROL_SECURITY_GET(pMacHeaderDecoded->frameControl))
    {
        // get security control
        UInt8 secControl;
        
        gpPd_ReadWithUpdate(p_PdLoh, 1, &secControl);
        pMacHeaderDecoded->secOptions.securityLevel =MACCORE_SECCONTROL_SECLEVEL_GET(secControl);
        pMacHeaderDecoded->secOptions.keyIdMode =MACCORE_SECCONTROL_KEYIDMODE_GET(secControl);
        // get framecounter
        MacCore_ReadStreamAndUpdatePd((UInt8*)&pMacHeaderDecoded->frameCounter, 4, p_PdLoh);
        switch(pMacHeaderDecoded->secOptions.keyIdMode)
        {
            case gpMacCore_KeyIdModeExplicit1Octet:
            {
                break;
            }
            case gpMacCore_KeyIdModeExplicit4Octet:
            {
                // get key source
                MacCore_ReadStreamAndUpdatePd(pMacHeaderDecoded->secOptions.pKeySource.pKeySource4, 4, p_PdLoh);
                break;
            }
            case gpMacCore_KeyIdModeExplicit8Octet:
            {
                // get key source
                MacCore_ReadStreamAndUpdatePd(pMacHeaderDecoded->secOptions.pKeySource.pKeySource8, 8, p_PdLoh);                               
                break;
            }
            case gpMacCore_KeyIdModeImplicit:
            {
                // do nothing
                break;
            }
            default:
            {
                GP_LOG_PRINTF("Invalid header keyIdMode: %x",0,pMacHeaderDecoded->secOptions.keyIdMode);
                return GP_MACCORE_INVALID_HEADER_LENGTH;
            }
        }
        if(pMacHeaderDecoded->secOptions.keyIdMode != gpMacCore_KeyIdModeImplicit)
        {
            // get key index
            gpPd_ReadWithUpdate(p_PdLoh, 1, &pMacHeaderDecoded->secOptions.keyIndex); 
        }
    }
    else
    {
        pMacHeaderDecoded->secOptions.keyIndex = 0;
    }
    if( packetLength < p_PdLoh->offset - startMacHdr )
    {
        GP_LOG_PRINTF("Invalid header length: %i < %i-%i",0,packetLength, p_PdLoh->offset, startMacHdr);
        return GP_MACCORE_INVALID_HEADER_LENGTH;
    }
    return p_PdLoh->offset - startMacHdr;

}

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
// fixme: in case the number of addresses does not match the actual number present in the beacon, we should drop this beacon...
STATIC void MacCore_ProcessBeacon(gpPd_Loh_t pdLoh, MacCore_HeaderDescriptor_t* pMdi)
{
    gpMacCore_PanDescriptor_t panDescriptor;
    UInt8 pendAddrSpec,gtsFields;

    panDescriptor.linkQuality     = gpPd_GetLqi(pdLoh.handle);
    panDescriptor.timeStamp       = gpPd_GetRxTimestamp(pdLoh.handle);

    MEMCPY(&panDescriptor.coordAddressInfo, &pMdi->srcAddrInfo, sizeof(gpMacCore_AddressInfo_t));
    panDescriptor.logicalChannel = gpMacCore_GetCurrentChannel(gpMacCore_pScanResult->stackId);
    MacCore_ReadStreamAndUpdatePd((UInt8*)&panDescriptor.superFrame, 2, &pdLoh);

    // offset points at GTS field
    gpPd_ReadWithUpdate(&pdLoh, 1, &gtsFields);
    gpPd_ReadWithUpdate(&pdLoh, 1, &pendAddrSpec);
    if( gtsFields != 0 || pendAddrSpec !=0 )
    {
        // only analyze a beacon if gts and pending address are both 0.
        // we van't handle this stuff.
        // we do indicate the beacon to the upper layer as we still use it for pan id conlfict.
        pdLoh.length = 0;
    }

    if( pdLoh.length <= GP_MACCORE_MAX_BEACON_PAYLOAD_LENGTH )
    {
        gpMacCore_StackId_t stackId;
        UInt8 *pBeaconPayload=NULL;

        // pending address related fields are not used
        // sduLength and sdu(data) itself is part of the pdLoh, the offset points to Beacon payload (if present)
        for( stackId=0;stackId<GP_DIVERSITY_NR_OF_STACKS;stackId++)
        {
            if( gpMacCore_cbValidStack(stackId) )
            {
                if(     gpMacCore_GetIndicateBeaconNotifications(stackId)
                    || (gpMacCore_pScanResult != NULL && gpMacCore_pScanResult->stackId == stackId ) )
                {
                    if( !pBeaconPayload && pdLoh.length)
                    {
                        pBeaconPayload = GP_POOLMEM_MALLOC(pdLoh.length);
                        if (pBeaconPayload)
                        {
                            gpPd_ReadByteStream(pdLoh.handle,pdLoh.offset,pdLoh.length,pBeaconPayload);
                        }
                    }
                    if (gpMacCore_pScanResult != NULL && gpMacCore_pScanResult->stackId == stackId)
                    {
                        //For active scan, we can use this variable to count the beacons
                        //We only have to count them, no need to store in a list
                        gpMacCore_pScanResult->resultListSize++;
                    }
                    gpMacCore_cbBeaconNotifyIndication( pMdi->sequenceNumber, &panDescriptor , stackId , pBeaconPayload ? pdLoh.length : 0 , pBeaconPayload );
                }
            }
        }
        if(pBeaconPayload)
        {
            gpPoolMem_Free(pBeaconPayload);
        }
    }
}
#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
// format: superframe spec (2) | GTS fields (1) | Pending address fields (1) | Beacon payload (variable)
STATIC void MacCore_ConstructBeacon(gpPd_Loh_t* p_PdLoh, UInt8 stackId)
{
    UInt16 superframeSpecification;
    UInt8 beaconLength;

    // construct the superframe specification field
    // beacon order 0xf =  nonbeacon-enabled personal area network
    // super frame order = 0xf -> hase no meaning if beacon order is 0xf
    // final cap slot = 0xf -> hase no meaning if beacon order is 0xf
    // battery life extension is not supported -> 0
    superframeSpecification = 0x0fff;
    MACCORE_SFS_PAN_COORDINATOR_SET(superframeSpecification, gpMacCore_GetPanCoordinator(stackId));
    MACCORE_SFS_ASSOCIATION_PERMIT_SET(superframeSpecification, gpMacCore_GetAssociationPermit(stackId));
    HOST_TO_LITTLE_UINT16(&superframeSpecification);
    gpPd_WriteByteStream(p_PdLoh->handle, p_PdLoh->offset, 2, (UInt8*)&superframeSpecification);
    p_PdLoh->length += 2;
    // add GTS fields
    gpPd_WriteByte(p_PdLoh->handle, p_PdLoh->offset + 2, GP_MACCORE_GTS_FIELDS);
    p_PdLoh->length++;
    // add pending address fields
    gpPd_WriteByte(p_PdLoh->handle, p_PdLoh->offset + 3, GP_MACCORE_PENDING_ADDRESS_FIELDS);
    p_PdLoh->length++;
    // add beacon payload
    beaconLength = gpMacCore_GetBeaconPayloadLength(stackId);
    if(beaconLength)
    {
        UInt8 beaconPayload[GP_MACCORE_MAX_SUPPORTED_BEACON_PAYLOAD_LENGTH];
        gpMacCore_GetBeaconPayload(beaconPayload, stackId);
        // beacon payload should already be endianess aware !!
        gpPd_WriteByteStream(p_PdLoh->handle, p_PdLoh->offset + 4, beaconLength, beaconPayload);
        p_PdLoh->length += beaconLength;
    }
}

STATIC void MacCore_TransmitBeacon(gpPd_Loh_t* p_PdLoh, gpHal_RxInfo_t *rxInfo)
{
    // this code is executed in response to both a beacon request
    // and a beacon confirm. once a beacon request is received, each
    // beacon confirm triggers the next beacon transmission until
    // stacks are exhausted
    if (MacCore_BeaconRequestStackId >= GP_DIVERSITY_NR_OF_STACKS)
    {
        // memory will be re-allocated as the next beacon request is processed
        gpPd_FreePd(p_PdLoh->handle) ;
        // all beacons were sent out, wait for the next beacon request
    }
    else
    {
        UIntLoop  stackId;
        Bool validStack = false;

        // find the next valid stack; continue where we left off
        // make sure that an invalid stack does not prevent other stacks
        // from sending out beacons
        for (stackId = MacCore_BeaconRequestStackId ; stackId < GP_DIVERSITY_NR_OF_STACKS ; stackId++)
        {
            if(gpMacCore_cbValidStack(stackId) && gpMacCore_GetBeaconStarted(stackId) && MacCore_RxForThisStack(stackId,gpMacCore_AddressModeShortAddress,rxInfo->rxChannel))
            {
                validStack = true ;
                MacCore_BeaconRequestStackId = stackId ;
                break ; // valid stack found - resume
            }
        }

        if (validStack)
        {
            UInt16 shortAddress;
            gpMacCore_AddressMode_t srcAddrMode;
            gpMacCore_AddressInfo_t srcAddressInfo;
            gpMacCore_AddressInfo_t dstAddressInfo;
            gpHal_DataReqOptions_t  dataReqOptions;
            UInt8 channels[3];

            // reuse the same memory fragment, overwriting old data (otherwise new data will be concatenated)
            p_PdLoh->length = 0;

            // construct beacon (and write in PD)
            MacCore_ConstructBeacon(p_PdLoh, MacCore_BeaconRequestStackId);

            // construct mac frame header
            shortAddress = gpMacCore_GetShortAddress(MacCore_BeaconRequestStackId);

            if(GP_MACCORE_IS_SHORT_ADDR_UNALLOCATED(shortAddress))
            {
                srcAddrMode = gpMacCore_AddressModeExtendedAddress;
            }
            else
            {
                srcAddrMode = gpMacCore_AddressModeShortAddress;
            }

            MacCore_InitSrcAddressInfo(&srcAddressInfo, srcAddrMode, MacCore_BeaconRequestStackId);

            dstAddressInfo.addressMode = gpMacCore_AddressModeNoAddress;
            MacCore_WriteMacHeaderInPd(gpMacCore_FrameTypeBeacon, &srcAddressInfo, &dstAddressInfo, 0, 0, p_PdLoh, MacCore_BeaconRequestStackId);

            channels[0] = gpMacCore_GetCurrentChannel(MacCore_BeaconRequestStackId);
            channels[1] = GP_HAL_MULTICHANNEL_INVALID_CHANNEL;
            channels[2] = GP_HAL_MULTICHANNEL_INVALID_CHANNEL;
            dataReqOptions.macScenario = gpHal_MacDefault;

            gpPad_SetTxChannels(MacCore_GetPad(MacCore_BeaconRequestStackId), channels);

            if (gpHal_DataRequest(&dataReqOptions, MacCore_GetPad(MacCore_BeaconRequestStackId), *p_PdLoh) != gpHal_ResultSuccess)
            {
                gpPd_FreePd(p_PdLoh->handle);
                return;
            }

            // once a confirm is received for this request, another beacon will be
            // sent out if needed or reset this counter
            ++MacCore_BeaconRequestStackId;
        }
        else
        {
            // no valid stack found - since we are busy with the last stack, make sure
            // memory is release because there will be no incoming confirm to do
            // it - beacon not sent out.
            gpPd_FreePd(p_PdLoh->handle);
        }
    }
}
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT

STATIC void MacCore_ProcessData(gpPd_Loh_t pdLoh, MacCore_HeaderDescriptor_t* pMdi , UInt8 macHeaderLength, gpHal_RxInfo_t *rxInfo)
{
    UInt8 result;
    result = MacCore_ValidateClearText(pMdi);
    if(result != gpMacCore_ResultSuccess)
    {
        GP_LOG_PRINTF("decryption failed: %x stack %x ",4,result, pMdi->stackId );
        gpMacCore_cbSecurityFailureCommStatusIndication(&pMdi->srcAddrInfo, &pMdi->dstAddrInfo, result, pMdi->stackId, gpPd_GetRxTimestamp(pdLoh.handle));
        gpPd_FreePd(pdLoh.handle);
    }
    else
    {
        if(pdLoh.length > 0)
        {

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
            if(gpMacCore_pPollReqArgs && gpMacCore_pPollReqArgs->assoc == false)
            {
            // We might not know the parent short or extended address, so don't do a check if
            // parent short/extended addr is invalid.
                if( gpMacCore_pPollReqArgs->coordAddrInfo.addressMode == gpMacCore_AddressModeShortAddress )
                {
                    if( gpMacCore_pPollReqArgs->coordAddrInfo.address.Short == pMdi->srcAddrInfo.address.Short
                        || gpMacCore_pPollReqArgs->coordAddrInfo.address.Short == GP_MACCORE_SHORT_ADDR_BROADCAST) //Or not know
                    {
                        gpMacCore_pPollReqArgs->result = gpMacCore_ResultSuccess;
                    }
                }
                else if( gpMacCore_pPollReqArgs->coordAddrInfo.addressMode == gpMacCore_AddressModeExtendedAddress)
                {
                    if( (gpMacCore_pPollReqArgs->coordAddrInfo.address.Extended.LSB == pMdi->srcAddrInfo.address.Extended.LSB
                             && gpMacCore_pPollReqArgs->coordAddrInfo.address.Extended.MSB == pMdi->srcAddrInfo.address.Extended.MSB)
                        || (gpMacCore_pPollReqArgs->coordAddrInfo.address.Extended.LSB == 0xffffffff
                             && gpMacCore_pPollReqArgs->coordAddrInfo.address.Extended.MSB == 0xffffffff ) ) //Or not know
                    {
                        gpMacCore_pPollReqArgs->result = gpMacCore_ResultSuccess;
                    }
                }
            }
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
            if ( pMdi->stackId != GP_MACCORE_STACK_UNDEFINED )
            {
                GP_STAT_SAMPLE_TIME();
                gpMacCore_cbDataIndication(&pMdi->srcAddrInfo, &pMdi->dstAddrInfo, pMdi->sequenceNumber, &pMdi->secOptions, pdLoh, pMdi->stackId);
            }
            else
            {
                UIntLoop count = 0;
                UIntLoop i;
                UInt8 stackList[GP_DIVERSITY_NR_OF_STACKS];
                //count number of stacks with an indication for optimal gpPd reusage
                for(i=0; i<GP_DIVERSITY_NR_OF_STACKS; i++)
                {
                    if(gpMacCore_cbValidStack(i) && MacCore_RxForThisStack(i,pMdi->dstAddrInfo.addressMode,rxInfo->rxChannel))
                    {
                        stackList[count]=i;
                        count++;
                    }
                }
                // indicate to all selected stacks
                if( count )
                {
                    for(i=0; i<count; i++)
                    {
                        gpPd_Loh_t pdLohCopy;
                        GP_LOG_PRINTF("gpMacCore_cbDataIndication",0);
                        pdLohCopy = pdLoh;
                        //don't copy pd object for last dataindication
                        if( i+1 < count )
                        {
                            pdLohCopy.handle = gpPd_CopyPd( pdLoh.handle );
                        }
                        GP_STAT_SAMPLE_TIME();
                        gpMacCore_cbDataIndication(&pMdi->srcAddrInfo, &pMdi->dstAddrInfo, pMdi->sequenceNumber, &pMdi->secOptions, pdLohCopy, stackList[i]);
                    }
                }
                else
                {
                    //dataIndication not handled
                    GP_LOG_PRINTF(" dataIndication not handled %x ",0, __LINE__);
                    gpPd_FreePd(pdLoh.handle);
                }
            }
        }
        else
        {
            gpPd_FreePd( pdLoh.handle );
        }
    }
}

static void MacCore_cbDataConfirm (gpMacCore_Result_t result, gpPd_Handle_t pdHandle)
{
    gpMacCore_cbDataConfirm(result, pdHandle);
}

static void MacCore_HalDataConfirm_dummy(gpHal_Result_t status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed)
{
    NOT_USED(status);
    NOT_USED(pdLoh);
    NOT_USED(lastChannelUsed);
}
static void MacCore_HalDataIndication_dummy(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo)
{
    NOT_USED(pdLoh);
    NOT_USED(rxInfo);
}
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
static void MacCore_cbEDConfirm_dummy(UInt16 channelMask, UInt8 *proto_energy_level_list)
{
    NOT_USED(channelMask);
    NOT_USED(proto_energy_level_list);
}
#endif
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpMacCore_Init(void)
{
    UIntLoop stackId;

    //Register gpHal callback functions
    gpHal_RegisterDataConfirmCallback(MacCore_HalDataConfirm);
    gpHal_RegisterDataIndicationCallback(MacCore_HalDataIndication);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    gpHal_RegisterEDConfirmCallback(gpHal_cbEDConfirm);
#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR


    for( stackId=0;stackId<GP_DIVERSITY_NR_OF_STACKS;stackId++)
    {
        MacCore_InitPad(stackId);
    }

#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
    MacCore_BeaconRequestStackId = 0 ;
#endif // GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT

}

void gpMacCore_DeInit(void)
{
    UIntLoop stackId;


    for( stackId=0;stackId<GP_DIVERSITY_NR_OF_STACKS;stackId++)
    {
        if(gpPad_CheckPadValid(gpMacCore_PIB[stackId].padHandle) == gpPad_ResultValidHandle)
        {
            gpMacCore_StackRemoved(stackId);
        }
    }

    //UnRegister gpHal callback functions
    gpHal_RegisterDataConfirmCallback(MacCore_HalDataConfirm_dummy);
    gpHal_RegisterDataIndicationCallback(MacCore_HalDataIndication_dummy);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    gpHal_RegisterEDConfirmCallback(MacCore_cbEDConfirm_dummy);
#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

}

// fixme: consider of replacing srcAddresMode by a full addressInfo struct (dual stack story where shortaddresses are different for dual pans)
void gpMacCore_DataRequest(gpMacCore_AddressMode_t srcAddrMode, gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, gpMacCore_Security_t *pSecOptions, gpMacCore_MultiChannelOptions_t multiChannelOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId)
{
    gpHal_DataReqOptions_t  dataReqOptions;
    gpMacCore_Result_t      result = gpMacCore_ResultSuccess;
    gpMacCore_AddressInfo_t srcAddressInfo;

    gpPd_Loh_t currentPdLoh;
    currentPdLoh = pdLoh;

    if (GP_MACCORE_INDIRECT_TRANSMISSION_ENABLED(txOptions))
    {
        //Add to queue and return
        GP_LOG_PRINTF("IndTx not supported",0);
        MacCore_cbDataConfirm(gpMacCore_ResultInvalidParameter, currentPdLoh.handle);
        return;

    }

    GP_STAT_SAMPLE_TIME();
    GP_LOG_PRINTF("gpMacCore_DataRequest ",0);

    // check parameters
    if(GP_MACCORE_CHECK_IF_ADDRESSMODE_NOT_NONE(srcAddrMode) && GP_MACCORE_CHECK_IF_ADDRESSMODE_NOT_NONE(pDstAddrInfo->addressMode))
    {
        GP_LOG_PRINTF("Inv addr",0);
        MacCore_cbDataConfirm(gpMacCore_ResultInvalidAddress, currentPdLoh.handle);
        return;
    }

    if(GP_MACCORE_CHECK_IF_ADDRESSMODE_INVALID(srcAddrMode))
    {
        GP_LOG_PRINTF("inv src addr mode",0);
        MacCore_cbDataConfirm(gpMacCore_ResultInvalidParameter, currentPdLoh.handle);
        return;
    }

    if(GP_MACCORE_CHECK_IF_ADDRESSINFO_INVALID(pDstAddrInfo))
    {
        GP_LOG_PRINTF("Inv dst info",0);
        MacCore_cbDataConfirm(gpMacCore_ResultInvalidParameter, currentPdLoh.handle);
        return;
    }

    MacCore_InitSrcAddressInfo(&srcAddressInfo, srcAddrMode, stackId);

    // can also be optimized by converting to assert if
    if(GP_MACCORE_NO_SECURITY_SPECIFIED(pSecOptions))
    {
        // no security ==> only write normal MAC header
        MacCore_WriteMacHeaderInPd(gpMacCore_FrameTypeData, &srcAddressInfo, pDstAddrInfo, txOptions, gpEncryption_SecLevelNothing, &currentPdLoh , stackId );
        if(currentPdLoh.length > GP_MACCORE_MAX_PHY_PACKET_SIZE_NO_FCS)
        {
            // If any parameter in the MCPS-DATA.request primitive is not supported or is out of range, the MAC sublayer will issue the MCPS-DATA.confirm primitive with a status of INVALID_PARAMETER.
            result = gpMacCore_ResultInvalidParameter;
        }
    }

    if(result != gpMacCore_ResultSuccess)
    {
        GP_LOG_PRINTF("MacCore Data req unsuccessful MAC result %d  ",2, result);
        MacCore_cbDataConfirm(result, currentPdLoh.handle);
        return;
    }

    dataReqOptions.macScenario = gpHal_MacDefault;
    if( multiChannelOptions.channel[0] == GP_MACCORE_INVALID_CHANNEL)
    {
        multiChannelOptions.channel[0] = gpMacCore_GetCurrentChannel(stackId);
        multiChannelOptions.channel[1] = GP_MACCORE_INVALID_CHANNEL;
        multiChannelOptions.channel[2] = GP_MACCORE_INVALID_CHANNEL;
    }

    // set channels
    gpPad_SetTxChannels(MacCore_GetPad(stackId), multiChannelOptions.channel);

    result = gpHal_DataRequest(&dataReqOptions, MacCore_GetPad(stackId), currentPdLoh);
    if(result != gpHal_ResultSuccess)
    {
        GP_LOG_PRINTF("MacCore Data req unsuccessful HAL result %d  ",2, result);
        MacCore_cbDataConfirm(result, currentPdLoh.handle);
    }
}

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
void gpMacCore_OrphanResponse(MACAddress_t* pOrphanAddress, UInt16 shortAddress, Bool associatedMember, gpMacCore_StackId_t stackId)
{

    if(associatedMember)//device is associated with coord
    {
        gpMacCore_Result_t result;

        result = MacCore_SendCommandCoordinatorRealignment(pOrphanAddress, shortAddress, false, stackId);

        if( result != gpMacCore_ResultSuccess)
        {
            // fixme: verify if this is correct.
            gpMacCore_AddressInfo_t srcAddr;
            gpMacCore_AddressInfo_t dstAddr;
            // Get SrcAddr attributes
            MacCore_InitSrcAddressInfo(&srcAddr, gpMacCore_AddressModeShortAddress, stackId);
            // Get DstAddr attributes
            dstAddr.addressMode = gpMacCore_AddressModeExtendedAddress;
            MEMCPY(&dstAddr.address.Extended, pOrphanAddress, sizeof(MACAddress_t));
            // derive PanID
            dstAddr.panId = gpMacCore_GetPanId(stackId);
            gpMacCore_cbOrphanCommStatusIndication(&srcAddr, &dstAddr, result, stackId, 0);
        }
    }
}
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
void MacCore_DelayedPollConfirm(void)
{
    if(gpMacCore_pPollReqArgs)
    {
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
        if( gpMacCore_pPollReqArgs->assoc )
        {
            //Skip result on gpMacCore_CommandDataRequest confirm as it's always true for k5
            //Result has been update if Response has received, otherwise we have resultNoData
            gpMacCore_Result_t status = gpMacCore_pPollReqArgs->result;

            if( status  != gpMacCore_ResultSuccess )
            {
                MacCore_HandleAssocConf( );
            }
        }
        else
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
        {
            gpMacCore_AddressInfo_t coordAddrInfo   = gpMacCore_pPollReqArgs->coordAddrInfo;
            gpMacCore_Result_t      result          = gpMacCore_pPollReqArgs->result;
            gpPoolMem_Free(gpMacCore_pPollReqArgs);
            gpMacCore_pPollReqArgs = NULL;

            gpMacCore_cbPollConfirm(result, &coordAddrInfo , 0);
        }
    }
}
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR

Bool MacCore_RxForThisStack(gpMacCore_StackId_t stackId , gpMacCore_AddressMode_t dstAddrMode, UInt8 rxChannel)
{
    UInt8 compareChannel;

    if( dstAddrMode == gpMacCore_AddressModeNoAddress
     && !gpMacCore_GetPanCoordinator(stackId) //Pan coordinator allows ModeNoAddress
     && (gpHal_GetFrameTypeFilterMask() & GPHAL_ENUM_FRAME_TYPE_FILTER_BCN_MASK)) //Shouldn't get ModeNoAddress when beacons are already filtered
    {
        return false;
    }
#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
    // TODO: check logic of this...!
    if( gpMacCore_pPollReqArgs!= NULL )
    {
        //Poll request is waiting for its data
        if( gpMacCore_pPollReqArgs->stackId == stackId )
        {
            return true;
        }
        else
        {
            compareChannel = gpRxArbiter_GetStackChannel(gpMacCore_pPollReqArgs->stackId) ;
        }
    }
    else
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
    {
    compareChannel = rxChannel;
    }
    if( gpRxArbiter_GetStackChannel(stackId) != compareChannel )
    {
        return false;
    }
    if(    gpRxArbiter_GetStackRxOn(stackId)
        || gpRxArbiter_GetDutyCycleEnabled(stackId)
    )
    {
        return true;
    }
    return false;
}

//----------------
//gpHal callback functions
//----------------

void MacCore_HalDataConfirm(gpHal_Result_t status, gpPd_Loh_t pdLoh, UInt8 lastChannelUsed)
{
    MacCore_HeaderDescriptor_t  mdi;

    GP_STAT_SAMPLE_TIME();

    MacCore_AnalyseMacHeader(&pdLoh, &mdi);
    switch(mdi.frameType)
    {
        case gpMacCore_FrameTypeData:
        {
            if(lastChannelUsed != GP_HAL_MULTICHANNEL_INVALID_CHANNEL)
            {
                // it was a gpHal multichannel retransmit
                // => adapt the current stack channel
                gpRxArbiter_SetStackChannel(lastChannelUsed, MacCore_GetStackId( &mdi.srcAddrInfo ) );
            }

            MacCore_cbDataConfirm(status, pdLoh.handle);
            break;
        }
        case gpMacCore_FrameTypeCommand:
        {
            gpMacCore_Command_t command;
#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT)
            gpPd_TimeStamp_t    txTime;
            txTime = gpPd_GetTxTimestamp(pdLoh.handle);
#endif
            mdi.stackId = MacCore_GetStackId( &mdi.srcAddrInfo );
            command = gpPd_ReadByte(pdLoh.handle, pdLoh.offset);

            switch(command)
            {
                case gpMacCore_CommandBeaconRequest:
                {
                    break;
                }
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
                case gpMacCore_CommandOrphanNotification:
                {
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
                case gpMacCore_CommandAssociationResponse:
                {
                    gpMacCore_cbAssociateCommStatusIndication(&mdi.srcAddrInfo, &mdi.dstAddrInfo, status, mdi.stackId, txTime);
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
                case gpMacCore_CommandAssociationRequest:
                {
                    if(gpMacCore_pPollReqArgs)
                    {
                        if(status == gpHal_ResultSuccess)
                        {
                            gpSched_ScheduleEvent((UInt32)((UInt32)GP_MACCORE_RESPONSE_WAIT_TIME * (UInt32)GP_MACCORE_SYMBOL_DURATION), MacCore_AssociateSendCommandDataRequest );
                        }
                        else
                        {
                            gpMacCore_pPollReqArgs->result = status;
                            gpMacCore_pPollReqArgs->txTimestamp = txTime;
                            MacCore_HandleAssocConf();
                        }
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
                case gpMacCore_CommandDataRequest:
                {
                    // FP bit is not correctly read on K5 and status is always successful
                    if(gpMacCore_pPollReqArgs)
                    {
                        /* Do not use the result here from the poll confirm.
                        gpMacCore_pPollReqArgs->result is initially set to no data
                        and is updated by sw if we received correct packet. */
                        if(status == gpHal_ResultNoAck)
                        {
                            gpMacCore_pPollReqArgs->result = gpMacCore_ResultNoAck;
                        }
                        gpMacCore_pPollReqArgs->txTimestamp = txTime;
                        //schedule poll/assoc confirm as we want to make sure all dataindication interrupts are handled.
                        gpSched_ScheduleEvent(0,MacCore_DelayedPollConfirm);
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
                case gpMacCore_CommandCoordinatorRealignment:
                {
                    gpMacCore_cbOrphanCommStatusIndication(&mdi.srcAddrInfo, &mdi.dstAddrInfo, status, mdi.stackId, txTime);
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
                default:
                {
                    break;
                }
            }
            // In all cases free the handle
            gpPd_FreePd(pdLoh.handle);
            break;
        }
#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
        case gpMacCore_FrameTypeBeacon: // Beacon confirmed
        {
            // send out the next beacon if needed - beacon confirmed
            gpHal_RxInfo_t rxInfo;
            rxInfo.rxChannel = lastChannelUsed;
            MacCore_TransmitBeacon(&pdLoh, &rxInfo);
            break;
        }
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
        default:
        {
            //GP_LOG_PRINTF("Free! %i",0,(UInt16)pdLoh.handle);
            gpPd_FreePd(pdLoh.handle);
            break;
        }
    }
}

// MacCore_HalDataIndication - packet reception at MAC layer
void MacCore_HalDataIndication(gpPd_Loh_t pdLoh, gpHal_RxInfo_t *rxInfo)
{
    // FIXME: use rxInfo.rxChannel
    MacCore_HeaderDescriptor_t mdi;
    UInt8 macHeaderLength;
    // analyze the MAC header and check if it contains invalid data in framecontrol
    MEMSET((UInt8*)&mdi, 0, sizeof(MacCore_HeaderDescriptor_t));

    macHeaderLength = MacCore_AnalyseMacHeader(&pdLoh, &mdi);
    if(macHeaderLength == GP_MACCORE_INVALID_HEADER_LENGTH)
    {
        gpPd_FreePd(pdLoh.handle);
        return;
    }
    //fixed for inderect tx,not yet for assoc, when reworking assoc, fix this. (merge gpMacCore_pPollReqArgs and gpMacCore_pPollReqArgs
    // drop packets which are received on another channel than the stackchannel.
    // drop packet if a packet is received even though the receiver for the stack was not on.
    if( mdi.stackId != GP_MACCORE_STACK_UNDEFINED )
    {
        if( !MacCore_RxForThisStack(mdi.stackId,mdi.dstAddrInfo.addressMode,rxInfo->rxChannel) )
        {
            GP_LOG_PRINTF("drop %i %i %i %i",0,mdi.stackId,gpRxArbiter_GetStackRxOn(mdi.stackId) , gpRxArbiter_GetStackChannel(mdi.stackId), gpRxArbiter_GetCurrentRxChannel() );
            gpPd_FreePd(pdLoh.handle);
            return;
        }
    }

    switch(mdi.frameType)
    {
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
        case gpMacCore_FrameTypeBeacon:
        {
            MacCore_ProcessBeacon(pdLoh, &mdi);
            gpPd_FreePd( pdLoh.handle );
            break;
        }
#endif // GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
        case gpMacCore_FrameTypeData:
        {
            MacCore_ProcessData(pdLoh, &mdi,macHeaderLength, rxInfo);
            break;
        }
        case gpMacCore_FrameTypeCommand:
        {
            gpMacCore_Command_t command;
#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_MACCORE_DIVERSITY_POLL_RECIPIENT) || defined(GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT)
            gpPd_TimeStamp_t rxTime = gpPd_GetRxTimestamp(pdLoh.handle);
#endif // defined(GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT) || defined(GP_MACCORE_DIVERSITY_POLL_RECIPIENT)
            command = gpPd_ReadByte(pdLoh.handle, pdLoh.offset);
            pdLoh.offset++;
            switch(command)
            {
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
                case gpMacCore_CommandAssociationRequest:
                {
                    // assoc request is always 2 bytes
                    // mdi.stackId has to be valid here -> stack is based on pan id
                    if(    pdLoh.length == GP_MACCORE_ASSOCIATION_REQUEST_CMD_LEN
                        && mdi.stackId != GP_MACCORE_STACK_UNDEFINED )
                    {
                        // only respond to association request commands when association permit is true
                        if(gpMacCore_GetAssociationPermit(mdi.stackId))
                        {
                            UInt8 capabilityInformation = gpPd_ReadByte(pdLoh.handle, pdLoh.offset);
                            gpMacCore_cbAssociateIndication(&mdi.srcAddrInfo.address, capabilityInformation, mdi.stackId, rxTime);
                        }
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
                case gpMacCore_CommandAssociationResponse:
                {
                    // mdi.stackId not relevant here -> use gpMacCore_pPollReqArgs->stackId
                    // Assoc request pending, if response here, we received data
                    if(     gpMacCore_pPollReqArgs
                         && gpMacCore_pPollReqArgs->assoc == true
                         && pdLoh.length == GP_MACCORE_ASSOCIATION_RESPONSE_CMD_LEN )
                    {
                        gpPd_ReadByteStream( pdLoh.handle, pdLoh.offset, 2, (UInt8*)&gpMacCore_pPollReqArgs->responseShortAddr);
                        pdLoh.offset += 2;
                        LITTLE_TO_HOST_UINT16(&gpMacCore_pPollReqArgs->responseShortAddr);
                        gpMacCore_pPollReqArgs->result = gpPd_ReadByte(pdLoh.handle, pdLoh.offset);
                        if( gpMacCore_pPollReqArgs->result == gpMacCore_ResultSuccess )
                        {
                            gpMacCore_SetCoordExtendedAddress(&mdi.srcAddrInfo.address.Extended, gpMacCore_pPollReqArgs->stackId);
                            gpMacCore_SetShortAddress(gpMacCore_pPollReqArgs->responseShortAddr, gpMacCore_pPollReqArgs->stackId);
                            MacCore_HandleAssocConf();
                        }
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
                case gpMacCore_CommandDataRequest:
                {
                    // mdi.stackId has to be valid here -> if not, drop
                    if(    pdLoh.length == GP_MACCORE_DATA_REQUEST_CMD_LEN
                        && mdi.stackId != GP_MACCORE_STACK_UNDEFINED )
                    {
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
                        if(    gpMacCore_pAssocRsp
                            && mdi.srcAddrInfo.addressMode == gpMacCore_AddressModeExtendedAddress
                            && gpMacCore_pAssocRsp->deviceAddress.LSB == mdi.srcAddrInfo.address.Extended.LSB
                            && gpMacCore_pAssocRsp->deviceAddress.MSB == mdi.srcAddrInfo.address.Extended.MSB
                        )
                        {
                            gpSched_UnscheduleEvent(MacCore_AssociateResponseTimeOut);
                            gpSched_ScheduleEvent(0,MacCore_cbAssocResp);
                        }
                        else
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
                        {
                            MacCore_cbPollIndication(&mdi.srcAddrInfo, mdi.stackId, rxTime);
                        }
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
                case gpMacCore_CommandOrphanNotification:
                {
                    // mdi.stackId not valid here (other side lost his stack) -> ignore
                    //orphan has extended src address
                    if(  mdi.srcAddrInfo.addressMode == gpMacCore_AddressModeExtendedAddress
                      && pdLoh.length == GP_MACCORE_ORPHAN_NOTIFICATION_CMD_LEN )
                    {
                        UInt8 stackId;
                        for( stackId=0;stackId<GP_DIVERSITY_NR_OF_STACKS;stackId++)
                        {
                            if( gpMacCore_cbValidStack(stackId) && MacCore_RxForThisStack( stackId,gpMacCore_AddressModeExtendedAddress,rxInfo->rxChannel) )
                            {
                                gpMacCore_cbOrphanIndication(&mdi.srcAddrInfo.address.Extended, stackId, rxTime);
                            }
                        }
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_RECIPIENT
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
                case gpMacCore_CommandCoordinatorRealignment:
                {
                    // mdi.stackId not relevant here -> use gpMacCore_pScanResult->stackId
                    if(    pdLoh.length == GP_MACCORE_COORDINATOR_REALIGNMENT_CMD_LEN
                        && gpSched_UnscheduleEvent(MacCore_DoOrphanScan) && gpMacCore_pScanResult )
                    {
                        UInt8 temp8;
                        UInt16 temp16;
                        gpPd_ReadByteStream( pdLoh.handle, pdLoh.offset, 2, (UInt8*)&temp16);
                        gpMacCore_SetPanId(temp16,gpMacCore_pScanResult->stackId);
                        pdLoh.offset += 2;

                        gpPd_ReadByteStream( pdLoh.handle, pdLoh.offset, 2, (UInt8*)&temp16);
                        gpMacCore_SetCoordShortAddress(temp16,gpMacCore_pScanResult->stackId);
                        pdLoh.offset += 2;

                        gpPd_ReadByteStream( pdLoh.handle, pdLoh.offset, 1, (UInt8*)&temp8);
                        gpRxArbiter_SetStackChannel(temp8,gpMacCore_pScanResult->stackId);
                        pdLoh.offset += 1;

                        gpPd_ReadByteStream( pdLoh.handle, pdLoh.offset, 2, (UInt8*)&temp16);
                        gpMacCore_SetShortAddress(temp16,gpMacCore_pScanResult->stackId);

                        gpMacCore_SetCoordExtendedAddress(&mdi.srcAddrInfo.address.Extended,gpMacCore_pScanResult->stackId);

                        MacCore_HandleOrphanScanEnd( gpMacCore_ResultSuccess);
                    }
                }
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
                case gpMacCore_CommandBeaconRequest: // Beacon request received
                {
                    // mdi.stackId not valid here (no src addressing) -> ignore
                    // according to the standard, beacon requests must have a broadcast panID address in them
                    if (  mdi.dstAddrInfo.panId == GP_MACCORE_PANID_BROADCAST
                        && pdLoh.length == GP_MACCORE_BEACON_REQUEST_CMD_LEN )
                    {
                        gpPd_Loh_t BeaconPd ;
                        MacCore_BeaconRequestStackId = 0 ;
                        // initialize pd - allocate memory for beacons
                        if(MacCore_AllocatePdLoh(&BeaconPd))
                        {
                            // check whether to transmit is done inside MacCore_TransmitBeacon()
                            // the memory slot is released once all beacons have been transmitted
                            MacCore_TransmitBeacon(&BeaconPd,rxInfo) ;
                        }
                    }
                    break;
                }
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_RECIPIENT
            }
            gpPd_FreePd( pdLoh.handle );
            break;
        }
        default:
        {
            gpPd_FreePd( pdLoh.handle );
            //GP_LOG_SYSTEM_PRINTF("unkn frametype %x", 2, mdi.frameType);
            break;
        }
    }
}


