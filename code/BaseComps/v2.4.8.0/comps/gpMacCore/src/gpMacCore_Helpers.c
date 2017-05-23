/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 * gpMacCore_Helpers.c
 *   This file contains several helper functions to support the MAC protocol.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpMacCore/src/gpMacCore_Helpers.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

// General includes
#include "gpPd.h"

#include "hal.h"
#include "gpHal.h"
#include "gpLog.h"
#include "gpMacCore.h"
#include "gpMacCore_defs.h"

#include "gpBsp.h"
#include "gpSched.h"
#include "gpAssert.h"

#include "gpPoolMem.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_MACCORE

 /*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static INLINE Bool MacCore_isValidAddressMode(UInt8 addressMode) {
    return (addressMode == gpMacCore_AddressModeShortAddress || addressMode == gpMacCore_AddressModeExtendedAddress);
}

 /*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

INLINE void MacCore_ReadStreamAndUpdatePd(UInt8* bytes, UInt8 length, gpPd_Loh_t* p_PdLoh)
{
    gpPd_ReadWithUpdate(p_PdLoh, length, bytes);
#ifdef GP_BIG_ENDIAN
    /* MEMCPY is done to minimize cast align warnings */
    switch(length)
    {
        UInt16 t2;
        UInt32 t4;
        UInt64Struct_t t8;

        case 2:
        {
            t2 = *((UInt16*)bytes);
            LITTLE_TO_HOST_UINT16(&t2);
            MEMCPY(bytes, &t2, length);
            break;
        }
        case 4:
        {
            t4 = *((UInt32*)bytes);
            LITTLE_TO_HOST_UINT32(&t4);
            MEMCPY(bytes, &t4, length);
            break;
        }
        case 8:
        {
            t8 = *((UInt64Struct_t*)bytes);
            LITTLE_TO_HOST_UINT64(&t8);
            MEMCPY(bytes, &t8, length);
            break;
        }
        default:
        {
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
#endif // GP_BIG_ENDIAN
}

INLINE void MacCore_PrependStreamAndUpdatePd(UInt8* bytes, UInt8 length, gpPd_Loh_t* p_PdLoh)
{
#ifdef GP_BIG_ENDIAN
    {
        UInt8 *nbytes=NULL;
        UInt16 t2;
        UInt32 t4;
        UInt64Struct_t t8;

        switch(length)
        {
            case 2:
            {
                t2 = *((UInt16*)bytes);
                nbytes = (UInt8*)&t2;
                HOST_TO_LITTLE_UINT16(nbytes);
                break;
            }
            case 4:
            {
                t4 = *((UInt32*)bytes);
                nbytes = (UInt8*)&t4;
                HOST_TO_LITTLE_UINT32(nbytes);
                break;
            }
            case 8:
            {
                t8 = *((UInt64Struct_t*)bytes);
                nbytes = (UInt8*)&t8;
                HOST_TO_LITTLE_UINT64(nbytes);
                break;
            }
            default:
            {
                GP_ASSERT_DEV_EXT(false);
                break;
            }
        }
        gpPd_PrependWithUpdate(p_PdLoh, length, nbytes);
    }
#else
    gpPd_PrependWithUpdate(p_PdLoh, length, bytes);
#endif // GP_BIG_ENDIAN
}

void MacCore_WriteMacHeaderInPd(gpMacCore_FrameType_t type, gpMacCore_AddressInfo_t* pSrcAddrInfo, const gpMacCore_AddressInfo_t* pDstAddrInfo, UInt8 txOptions, UInt8 securityLevel,  gpPd_Loh_t *pPdLoh , gpMacCore_StackId_t stackId )
{
    UInt16 frameControl = 0x0;
    Bool identicalPans = (pSrcAddrInfo->panId == pDstAddrInfo->panId);

    // add frame type
    MACCORE_FRAMECONTROL_FRAMETYPE_SET(frameControl,type);
    // add security + frame version
    if (securityLevel != gpEncryption_SecLevelNothing )
    {
        MACCORE_FRAMECONTROL_SECURITY_SET(frameControl,0x01);
        MACCORE_FRAMECONTROL_FRAMEVERSION_SET(frameControl,gpMacCore_MacVersion2006);
    }
    else
    {
        if( (pPdLoh->length > GP_MACCORE_MAX_MAC_SAFE_PAYLOAD_SIZE) && (gpMacCore_GetMacVersion(stackId) == gpMacCore_MacVersion2006 ))
        {
            MACCORE_FRAMECONTROL_FRAMEVERSION_SET(frameControl,gpMacCore_MacVersion2006);
        }
    }
    // add ACK-request
    if (txOptions & GP_MACCORE_TX_OPT_ACK_REQ)
    {
        MACCORE_FRAMECONTROL_ACKREQUEST_SET(frameControl, 0x01);
    }
    // add data pending bit
    if (txOptions & GP_MACCORE_TX_OPT_MORE_DATA_PENDING )
    {
        MACCORE_FRAMECONTROL_FRAMEPENDING_SET(frameControl, 0x01);
    }
    // add source addressing mode
    MACCORE_FRAMECONTROL_SRCADDRMODE_SET(frameControl,pSrcAddrInfo->addressMode);
    // add destination addressing mode
    MACCORE_FRAMECONTROL_DSTADDRMODE_SET(frameControl,pDstAddrInfo->addressMode);

    // enable pan compression if possible
    if(MacCore_isValidAddressMode(pSrcAddrInfo->addressMode) && MacCore_isValidAddressMode(pDstAddrInfo->addressMode) && identicalPans)
    {
        MACCORE_FRAMECONTROL_PANCOMPRESSION_SET(frameControl, 0x01);
    }
    // add source address
    if(pSrcAddrInfo->addressMode == gpMacCore_AddressModeShortAddress)
    {
        MacCore_PrependStreamAndUpdatePd((UInt8*)(&pSrcAddrInfo->address.Short), GP_MACCORE_SHORT_ADDR_SIZE, pPdLoh);
    }
    else if(pSrcAddrInfo->addressMode == gpMacCore_AddressModeExtendedAddress)
    {
        MacCore_PrependStreamAndUpdatePd((UInt8*)&pSrcAddrInfo->address.Extended, GP_MACCORE_EXT_ADDR_SIZE, pPdLoh);
    }
    // add source PAN ID
    if((!MacCore_isValidAddressMode(pDstAddrInfo->addressMode) || !identicalPans) && MacCore_isValidAddressMode(pSrcAddrInfo->addressMode))
    {
        MacCore_PrependStreamAndUpdatePd((UInt8*)&pSrcAddrInfo->panId, GP_MACCORE_PANID_SIZE, pPdLoh);
    }
    // add destination address
    if (pDstAddrInfo->addressMode == gpMacCore_AddressModeShortAddress)
    {
        MacCore_PrependStreamAndUpdatePd((UInt8*)&pDstAddrInfo->address.Short, GP_MACCORE_SHORT_ADDR_SIZE, pPdLoh);
    }
    else if (pDstAddrInfo->addressMode == gpMacCore_AddressModeExtendedAddress)
    {
        MacCore_PrependStreamAndUpdatePd((UInt8*)&pDstAddrInfo->address.Extended, GP_MACCORE_EXT_ADDR_SIZE, pPdLoh);
    }
    // add destination PAN ID
    if (pDstAddrInfo->addressMode != gpMacCore_AddressModeNoAddress)
    {
        MacCore_PrependStreamAndUpdatePd((UInt8*)&pDstAddrInfo->panId, GP_MACCORE_PANID_SIZE,pPdLoh);
    }

    // add sequence number (first write it in the header, then increment it)
    {
    UInt8 dsn = gpMacCore_GetDsn(stackId);
    gpPd_PrependWithUpdate(pPdLoh, 1, &dsn);
    gpMacCore_SetDsn(dsn+1, stackId);
    }
    
    // add frame control
    MacCore_PrependStreamAndUpdatePd((UInt8*)&frameControl, 2,pPdLoh);
}

/*
 * in section 7.5.8.2.1 Outgoing frame security procedure:
 * h) insert the auxiliary security header into the frame
 */

void MacCore_InitSrcAddressInfo(gpMacCore_AddressInfo_t *pSrcAddressInfo, gpMacCore_AddressMode_t addrMode, gpMacCore_StackId_t stackId)
{
    GP_ASSERT_DEV_INT(pSrcAddressInfo != NULL);

    pSrcAddressInfo->addressMode = addrMode;
    pSrcAddressInfo->panId = gpMacCore_GetPanId(stackId);
    MEMSET((UInt8*)&pSrcAddressInfo->address, 0, sizeof(pSrcAddressInfo->address));

    switch(addrMode)
    {
        case gpMacCore_AddressModeExtendedAddress:
        {
            gpMacCore_GetExtendedAddress(&pSrcAddressInfo->address.Extended);
            break;
        }
        case gpMacCore_AddressModeShortAddress:
        {
            pSrcAddressInfo->address.Short = gpMacCore_GetShortAddress(stackId);
            break;
        }
        case gpMacCore_AddressModeNoAddress:
        {
            break;
        }
        default:
        {
            GP_ASSERT_DEV_INT(false);
            break;
        }
    }
}

// fixme: needs to work for both security and non security ==> this implementation only works without security.
Bool MacCore_AllocatePdLoh(gpPd_Loh_t* p_PdLoh)
{
    p_PdLoh->handle = gpPd_GetPd();
    if(gpPd_CheckPdValid(p_PdLoh->handle) != gpPd_ResultValidHandle)
    {
        return false;
    }
    p_PdLoh->offset = GP_MACCORE_MAX_MAC_HEADER_SIZE;
    p_PdLoh->length = 0;

    return true;
}

gpMacCore_Result_t MacCore_SendCommand( gpMacCore_AddressInfo_t* pDestAddrInfo, gpMacCore_AddressInfo_t* pSrcAddrInfo, UInt8 txOptions, UInt8 *pData, UInt8 len , gpMacCore_StackId_t stackId, gpHal_MacScenario_t scenario)
{
    gpPd_Loh_t              pdLoh;
    gpHal_DataReqOptions_t  dataReqOptions;
    UInt8                   channels[3];
    gpMacCore_Result_t      result;

    if(!MacCore_AllocatePdLoh(&pdLoh))
    {
        return gpMacCore_ResultTransactionOverflow;
    }

    channels[0] = gpRxArbiter_GetStackChannel(stackId);
    channels[1] = GP_HAL_MULTICHANNEL_INVALID_CHANNEL;
    channels[2] = GP_HAL_MULTICHANNEL_INVALID_CHANNEL;
    dataReqOptions.macScenario = scenario;

    // set transmit attributes for this stack
    gpPad_SetTxChannels(MacCore_GetPad(stackId), channels);

    // Write Data
    gpPd_WriteByteStream(pdLoh.handle, pdLoh.offset, len, pData );
    pdLoh.length += len;

    MacCore_WriteMacHeaderInPd(gpMacCore_FrameTypeCommand, pSrcAddrInfo, pDestAddrInfo, txOptions, 0, &pdLoh , stackId );

    // send command
    result = gpHal_DataRequest(&dataReqOptions, MacCore_GetPad(stackId), pdLoh);
    if( result != gpMacCore_ResultSuccess )
    {
        gpPd_FreePd( pdLoh.handle );
    }
    return result;
}

#if defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)
gpMacCore_Result_t MacCore_SendCommandDataRequest(gpMacCore_AddressInfo_t * pCoordAddrInfo, Bool associationTrigger, gpMacCore_StackId_t stackId)
{
    gpMacCore_AddressMode_t srcAddrMode;
    gpMacCore_AddressInfo_t srcAddrInfo;
    gpMacCore_AddressInfo_t dstAddrInfo;
    UInt8 data = gpMacCore_CommandDataRequest;
    UInt16 coordShortAddr;
    MACAddress_t coordExtendedAddress;

    if(associationTrigger)
    {
        srcAddrMode = gpMacCore_AddressModeExtendedAddress;
        dstAddrInfo.panId = pCoordAddrInfo->panId;
        if(GP_MACCORE_IS_SHORT_ADDR_UNALLOCATED(gpMacCore_GetCoordShortAddress(stackId)))
        {
            dstAddrInfo.addressMode = gpMacCore_AddressModeExtendedAddress;
            gpMacCore_GetCoordExtendedAddress(&(dstAddrInfo.address.Extended), stackId);
        }
        else
        {
            dstAddrInfo.addressMode = gpMacCore_AddressModeShortAddress;
            dstAddrInfo.address.Short = gpMacCore_GetCoordShortAddress(stackId);
        }
    }
    else
    {
        UInt16 shortAddress = gpMacCore_GetShortAddress(stackId);
        dstAddrInfo = *pCoordAddrInfo;

        if(GP_MACCORE_IS_SHORT_ADDR_BROADCAST(shortAddress) || GP_MACCORE_IS_SHORT_ADDR_UNALLOCATED(shortAddress))
        {
            srcAddrMode = gpMacCore_AddressModeExtendedAddress;
        }
        else
        {
            srcAddrMode = gpMacCore_AddressModeShortAddress;
        }
    }

    MacCore_InitSrcAddressInfo(&srcAddrInfo, srcAddrMode, stackId);

    // set coordinator address in HW - so that the receiver stops after receiving a data frame from the coordinator
    if(pCoordAddrInfo->addressMode == gpMacCore_AddressModeShortAddress)
    {
        coordShortAddr = pCoordAddrInfo->address.Short;
        gpMacCore_GetCoordExtendedAddress(&coordExtendedAddress, stackId);
    }
    else
    {
        coordShortAddr = gpMacCore_GetCoordShortAddress(stackId);
        coordExtendedAddress = pCoordAddrInfo->address.Extended;
    }
    gpHal_SetCoordExtendedAddress(&coordExtendedAddress);
    gpHal_SetCoordShortAddress(coordShortAddr);

    return MacCore_SendCommand(&dstAddrInfo, &srcAddrInfo, GP_MACCORE_TX_OPT_ACK_REQ, &data, GP_MACCORE_DATA_REQUEST_CMD_LEN, stackId, gpHal_MacPollReq);
}
#endif //defined(GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR) || defined(GP_MACCORE_DIVERSITY_POLL_ORIGINATOR)


void MacCore_StopRunningRequests(gpMacCore_StackId_t stackId)
{
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    if(gpMacCore_pScanResult && (stackId == gpMacCore_pScanResult->stackId))
    {
#ifdef GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR
        gpSched_UnscheduleEvent(MacCore_DoActiveScan);
#endif //GP_MACCORE_DIVERSITY_SCAN_ACTIVE_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
        gpSched_UnscheduleEvent(MacCore_DoEDScan);
#endif //GP_MACCORE_DIVERSITY_SCAN_ED_ORIGINATOR
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
        gpSched_UnscheduleEvent(MacCore_DoOrphanScan);
#endif //GP_MACCORE_DIVERSITY_SCAN_ORPHAN_ORIGINATOR
#if (GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US != 0)
        gpSched_UnscheduleEvent(MacCore_ScanRxOffWindow);
#endif //(GP_MACCORE_SCAN_RXOFFWINDOW_TIME_US != 0)
        gpPoolMem_Free(gpMacCore_pScanResult);
        gpMacCore_pScanResult = NULL;
    }
#endif //GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
    if(gpMacCore_pAssocRsp && (stackId == gpMacCore_pAssocRsp->stackId))
    {
        gpMacCore_AddressInfo_t dstAddrInfo;

        dstAddrInfo.address.Extended = gpMacCore_pAssocRsp->deviceAddress;
        dstAddrInfo.addressMode = gpMacCore_AddressModeExtendedAddress;
        dstAddrInfo.panId = gpMacCore_GetPanId(stackId);

        GP_LOG_SYSTEM_PRINTF("===B===",0);
        gpSched_UnscheduleEvent(MacCore_AssociateResponseTimeOut);
        gpSched_UnscheduleEvent(MacCore_cbAssocResp);

        //Clear the pending AssociationResponse message
        gpMacCore_DataPending_QueueRemove(&dstAddrInfo, stackId);
        gpPoolMem_Free(gpMacCore_pAssocRsp);
        gpMacCore_pAssocRsp = NULL;
    }
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_RECIPIENT
#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
    if(gpMacCore_pPollReqArgs && stackId == gpMacCore_pPollReqArgs->stackId)
    {
#ifdef GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
        gpSched_UnscheduleEvent(MacCore_AssociateSendCommandDataRequest);
#endif //GP_MACCORE_DIVERSITY_ASSOCIATION_ORIGINATOR
        gpSched_UnscheduleEvent(MacCore_DelayedPollConfirm);
        gpPoolMem_Free(gpMacCore_pPollReqArgs);
        gpMacCore_pPollReqArgs = NULL;
    }
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
}

#if GP_DIVERSITY_NR_OF_STACKS > 1
gpMacCore_StackId_t MacCore_GetStackId( gpMacCore_AddressInfo_t *pAddressInfo )
{
    gpMacCore_StackId_t id;

    if ((pAddressInfo->panId == GP_MACCORE_PANID_BROADCAST) ||
        (pAddressInfo->addressMode == gpMacCore_AddressModeNoAddress))
    {
        return GP_MACCORE_STACK_UNDEFINED;
    }
    
    for(id=0; id < GP_DIVERSITY_NR_OF_STACKS; id++)
    {
        if ( pAddressInfo->panId == gpMacCore_GetPanId(id) )
        {
            if( pAddressInfo->addressMode == gpMacCore_AddressModeShortAddress )
            {
                if( pAddressInfo->address.Short == GP_MACCORE_SHORT_ADDR_BROADCAST  || 
                    pAddressInfo->address.Short == gpMacCore_GetShortAddress(id))
                {
                    return id;
                }
            }
            else if( pAddressInfo->addressMode == gpMacCore_AddressModeExtendedAddress )
            {
                return id;
            }
        }
    }
    return GP_MACCORE_STACK_UNDEFINED;
}
#endif // GP_DIVERSITY_NR_OF_STACKS > 1

// debug functions


