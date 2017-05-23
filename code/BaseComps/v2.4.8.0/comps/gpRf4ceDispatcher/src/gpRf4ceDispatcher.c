/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *   This file contains the implementation of the gpRf4ceDispatcher.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceDispatcher/src/gpRf4ceDispatcher.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RF4CEDISPATCHER
 
#include "gpSched.h"
#include "gpLog.h"
#include "gpAssert.h"

#include "gpRf4ceDispatcher.h"
 
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

STATIC UInt8 gpRf4ceDispatcher_LockFunctionalBlock;

extern const gpRf4ceDispatcher_BindMapping_t ROM gpRf4ceDispatcher_BindMapping[] FLASH_PROGMEM;
extern const gpRf4ceDispatcher_DataMapping_t ROM gpRf4ceDispatcher_DataMapping[] FLASH_PROGMEM;

STATIC Bool Rf4ceDispatcher_SetDefaultOnReset;


/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/ 

static const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM Rf4ceDispatcher_FindBindBlock(void);
static const ROM gpRf4ceDispatcher_DataCallbacks_t* FLASH_PROGMEM Rf4ceDispatcher_FindDataBlock(void);


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM Rf4ceDispatcher_FindBindBlock()
{
    const ROM gpRf4ceDispatcher_BindMapping_t* FLASH_PROGMEM pBindMapping;
    gpRf4ceDispatcher_BindMapping_t bindMapping;
    pBindMapping = gpRf4ceDispatcher_BindMapping;
    while(true)
    {
        MEMCPY_P(&bindMapping, pBindMapping, sizeof(gpRf4ceDispatcher_BindMapping_t));
        if(bindMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            return NULL;
        }
        else if(bindMapping.functionalBlockID == gpRf4ceDispatcher_LockFunctionalBlock)
        {
            return bindMapping.callbacks;
        }
        pBindMapping++;
    }
}

const ROM gpRf4ceDispatcher_DataCallbacks_t* FLASH_PROGMEM Rf4ceDispatcher_FindDataBlock()
{
    const ROM gpRf4ceDispatcher_DataMapping_t* FLASH_PROGMEM pDataMapping;
    gpRf4ceDispatcher_DataMapping_t dataMapping;
    pDataMapping = gpRf4ceDispatcher_DataMapping;
    while(true)
    {
        MEMCPY_P(&dataMapping, pDataMapping, sizeof(gpRf4ceDispatcher_DataMapping_t));
        if(dataMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            return NULL;
        }
        else if(dataMapping.functionalBlockID == gpRf4ceDispatcher_LockFunctionalBlock)
        {
            return dataMapping.callbacks;
        }
        pDataMapping++;
    }
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
 
void gpRf4ceDispatcher_Init(void)
{
    gpRf4ceDispatcher_LockFunctionalBlock = GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID;
}

Bool gpRf4ceDispatcher_LockRequest(UInt8 fbId)
{
    Bool retval = false;
    
    if(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
    {
        GP_LOG_SYSTEM_PRINTF("lock %x->%x",0,(UInt16)fbId,(UInt16)gpRf4ceDispatcher_LockFunctionalBlock);
        //Lock requested twice by same fb not OK
        GP_ASSERT_DEV_EXT(fbId != gpRf4ceDispatcher_LockFunctionalBlock);
    }
    else
    {
        gpRf4ceDispatcher_LockFunctionalBlock = fbId;
        retval = true;
    }
    
    GP_LOG_PRINTF("Rf4ceDisp LockReq fbId: %x rv: %x ", 2, fbId, retval );
    
    return retval;
}

void gpRf4ceDispatcher_ResetRequest(Bool setDefault , UInt8 nwkNodeCapabilities)
{
    Rf4ceDispatcher_SetDefaultOnReset = setDefault;
    gpRf4ce_ResetRequest(setDefault,nwkNodeCapabilities);
}

void gpRf4ceDispatcher_ReleaseRequest(UInt8 fbId)
{
    if(fbId != gpRf4ceDispatcher_LockFunctionalBlock)
    {
        GP_LOG_SYSTEM_PRINTF("ReleaseRequest %x ", 2, fbId);
    }
    GP_ASSERT_DEV_EXT(fbId == gpRf4ceDispatcher_LockFunctionalBlock);
    gpRf4ceDispatcher_LockFunctionalBlock = GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID;  
}

Bool gpRf4ceDispatcher_LockedByThisFunctionalBlock( UInt8 fbId )
{
    return ( gpRf4ceDispatcher_LockFunctionalBlock == fbId );
}
/*****************************************************************************
 *                    RF4CE Callback Function Definitions
 *****************************************************************************/

void gpRf4ce_cbDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle)
{
    const ROM gpRf4ceDispatcher_DataMapping_t* FLASH_PROGMEM pDataMapping;
    gpRf4ceDispatcher_DataMapping_t dataMapping;
    gpRf4ceDispatcher_cbDEDataIndication_t dataIndicationCb;

    pDataMapping = gpRf4ceDispatcher_DataMapping;
    
    while(true)
    {
        MEMCPY_P(&dataMapping, pDataMapping, sizeof(gpRf4ceDispatcher_DataMapping_t));
        if(dataMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        MEMCPY_P(&dataIndicationCb, (((UIntPtr_P)dataMapping.callbacks) + offsetof(gpRf4ceDispatcher_DataCallbacks_t, dataIndicationCallback)), sizeof(gpRf4ceDispatcher_cbDEDataIndication_t));
        if(dataIndicationCb != NULL)
        {
            if(dataIndicationCb(pairingRef, profileId, vendorId, rxFlags, nsduLength, nsduOffset, pdHandle))
            {
                //dataIndication handled
                return;
            }
        }
        pDataMapping++;
    }
    //dataIndication not handled
    gpPd_FreePd(pdHandle);
}

#ifdef GP_MACCORE_DIVERSITY_POLL_RECIPIENT
void gpRf4ce_cbPollIndication( UInt8 pairingRef )
{
    const ROM gpRf4ceDispatcher_DataMapping_t* FLASH_PROGMEM pDataMapping;
    gpRf4ceDispatcher_DataMapping_t dataMapping;
    gpRf4ceDispatcher_cbPollIndication_t pollIndicationcb;

    pDataMapping = gpRf4ceDispatcher_DataMapping;

    while(true)
    {
        MEMCPY_P(&dataMapping, pDataMapping, sizeof(gpRf4ceDispatcher_DataMapping_t));
        if(dataMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        MEMCPY_P(&pollIndicationcb, (((UIntPtr_P)dataMapping.callbacks) + offsetof(gpRf4ceDispatcher_DataCallbacks_t, pollIndicationCallback)), sizeof(gpRf4ceDispatcher_cbPollIndication_t));
        if(pollIndicationcb != NULL)
        {
            if(pollIndicationcb(pairingRef))
            {
                return;
            }
        }
        pDataMapping++;
    }
}
#endif //GP_MACCORE_DIVERSITY_POLL_RECIPIENT


void gpRf4ce_cbDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, UInt8 pdHandle)
{
    const ROM gpRf4ceDispatcher_DataCallbacks_t* FLASH_PROGMEM  pDataCallbacks;
    gpRf4ceDispatcher_cbDEDataConfirm_t dataConfirmCb;

    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);

    pDataCallbacks = Rf4ceDispatcher_FindDataBlock();
    GP_ASSERT_DEV_EXT(pDataCallbacks != NULL);
    
    MEMCPY_P(&dataConfirmCb , (((UIntPtr_P)pDataCallbacks) + offsetof(gpRf4ceDispatcher_DataCallbacks_t, dataConfirmCallback)), sizeof(gpRf4ceDispatcher_cbDEDataConfirm_t));
    GP_ASSERT_DEV_EXT(dataConfirmCb != NULL);
    
    dataConfirmCb(status, pairingRef, pdHandle);
}

#ifdef GP_MACCORE_DIVERSITY_POLL_ORIGINATOR
void gpRf4ce_cbPollConfirm(gpRf4ce_Result_t status, UInt8 pairingRef )
{
    const ROM gpRf4ceDispatcher_DataCallbacks_t* FLASH_PROGMEM  pDataCallbacks;
    gpRf4ceDispatcher_cbPollConfirm_t pollConfirmCb;
    
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);

    pDataCallbacks = Rf4ceDispatcher_FindDataBlock();
    GP_ASSERT_DEV_EXT(pDataCallbacks != NULL);
    
    MEMCPY_P(&pollConfirmCb , (((UIntPtr_P)pDataCallbacks) + offsetof(gpRf4ceDispatcher_DataCallbacks_t, pollConfirmCallback)), sizeof(gpRf4ceDispatcher_cbPollConfirm_t));
    GP_ASSERT_DEV_EXT(pollConfirmCb != NULL);
    
    pollConfirmCb(status, pairingRef);
}
#endif //GP_MACCORE_DIVERSITY_POLL_ORIGINATOR

#ifdef GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT
void gpRf4ce_cbDiscoveryCommStatusIndication(gpRf4ce_Result_t status, gpRf4ce_PANID_t dstPANId, MACAddress_t* pDstAddr)
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEDiscoveryCommStatusIndication_t discoveryCommStatusIndicationCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&discoveryCommStatusIndicationCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, discoveryCommStatusIndicationCallback)), sizeof(gpRf4ceDispatcher_cbMEDiscoveryCommStatusIndication_t));
    GP_ASSERT_DEV_EXT(discoveryCommStatusIndicationCb);
    discoveryCommStatusIndicationCb(status, dstPANId, pDstAddr);
}
#endif //GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT

#ifdef GP_RF4CE_DIVERSITY_PAIR_RECIPIENT
void gpRf4ce_cbPairCommStatusIndication(gpRf4ce_Result_t status, UInt8 pairingRef, gpRf4ce_PANID_t dstPANId, UInt8 dstAddrMode, gpMacCore_Address_t* pDstAddr)
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEPairCommStatusIndication_t pairCommStatusIndicationCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&pairCommStatusIndicationCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, pairCommStatusIndicationCallback)), sizeof(gpRf4ceDispatcher_cbMEPairCommStatusIndication_t));
    GP_ASSERT_DEV_EXT(pairCommStatusIndicationCb);
    pairCommStatusIndicationCb(status, pairingRef, dstPANId, dstAddrMode, pDstAddr);
}
#endif // GP_RF4CE_DIVERSITY_PAIR_RECIPIENT

#ifdef GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT
void gpRf4ce_cbDiscoveryIndication(gpRf4ce_Result_t status, MACAddress_t *pSrcIEEEAddr, UInt8 orgNodeCapabilities, gpRf4ce_VendorId_t orgVendorId, gpPd_Offset_t orgVendorStringOffset, UInt8 orgAppCapabilities, gpPd_Offset_t orgUserStringOffset, gpPd_Offset_t orgDevTypeListOffset, gpPd_Offset_t orgProfileIdListOffset, gpRf4ce_DeviceType_t searchDevType, gpPd_Handle_t pdHandle)
{    
    const ROM gpRf4ceDispatcher_BindMapping_t* FLASH_PROGMEM pBindMapping;
    gpRf4ceDispatcher_BindMapping_t bindMapping;
    gpRf4ceDispatcher_cbMEDiscoveryIndication_t discoveryIndicationCb;
    pBindMapping = gpRf4ceDispatcher_BindMapping;
    
    while(true)
    {
        MEMCPY_P(&bindMapping, pBindMapping, sizeof(gpRf4ceDispatcher_BindMapping_t));
        if(bindMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        MEMCPY_P(&discoveryIndicationCb, (((UIntPtr_P)bindMapping.callbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, discoveryIndicationCallback)), sizeof(gpRf4ceDispatcher_cbMEDiscoveryIndication_t));
        if(discoveryIndicationCb != NULL)
        {
            if(discoveryIndicationCb(status, pSrcIEEEAddr, orgNodeCapabilities, orgVendorId, orgVendorStringOffset, orgAppCapabilities, orgUserStringOffset, orgDevTypeListOffset, orgProfileIdListOffset, searchDevType, pdHandle))
            {
                GP_LOG_PRINTF("discInd handled by %x", bindMapping.functionalBlockID);
                //discoveryIndication handled
                return;
            }
        }
        pBindMapping++;
    }
    
    GP_LOG_PRINTF("discInd NOT handled",0);
}
#endif // GP_RF4CE_DIVERSITY_DISCOVERY_RECIPIENT

#ifdef GP_RF4CE_DIVERSITY_DISCOVERY_ORIGINATOR
void gpRf4ce_cbDiscoveryConfirm(gpRf4ce_Result_t status, UInt8 numNodes)
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEDiscoveryConfirm_t discoveryConfirmCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&discoveryConfirmCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, discoveryConfirmCallback)), sizeof(gpRf4ceDispatcher_cbMEDiscoveryConfirm_t));
    GP_ASSERT_DEV_EXT(discoveryConfirmCb != NULL);
    discoveryConfirmCb(status, numNodes);    
}

Bool gpRf4ce_cbDiscoveryResponseNotifyIndication(gpRf4ce_NodeDesc_t *pNodeDesc)
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEDiscoveryResponseNotifyIndication_t discoveryResponseNotifyIndicationCb;
    Bool returnValue = false;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&discoveryResponseNotifyIndicationCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, discoveryResponseNotifyIndicationCallback)), sizeof(gpRf4ceDispatcher_cbMEDiscoveryResponseNotifyIndication_t));
    GP_ASSERT_DEV_EXT(discoveryResponseNotifyIndicationCb != NULL);
    discoveryResponseNotifyIndicationCb(pNodeDesc, &returnValue);
    return returnValue;
}
#endif //GP_RF4CE_DIVERSITY_DISCOVERY_ORIGINATOR

#if defined(GP_RF4CE_DIVERSITY_AUTO_DISCOVERY)
void gpRf4ce_cbAutoDiscoveryConfirm(gpRf4ce_Result_t status , MACAddress_t *pSrcIEEEAddr )
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEAutoDiscoveryConfirm_t autoDiscoveryConfirmCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&autoDiscoveryConfirmCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, autoDiscoveryConfirmCallback)), sizeof(gpRf4ceDispatcher_cbMEAutoDiscoveryConfirm_t));
    GP_ASSERT_DEV_EXT(autoDiscoveryConfirmCb != NULL);
    autoDiscoveryConfirmCb(status, pSrcIEEEAddr);  
}
#endif // GP_DIVERSITY_RF4CE_TARGET

#ifdef GP_RF4CE_DIVERSITY_PAIR_RECIPIENT
void gpRf4ce_cbPairIndication(gpRf4ce_Result_t status, gpRf4ce_PANID_t srcPANId, MACAddress_t *pSrcIEEEAddr, UInt8 orgNodeCapabilities, gpRf4ce_VendorId_t orgVendorId, gpPd_Offset_t orgVendorStringOffset, UInt8 orgAppCapabilities, gpPd_Offset_t orgUserStringOffset, gpPd_Offset_t orgDevTypeListOffset, gpPd_Offset_t orgProfileIdListOffset, UInt8 keyExTransferCount, UInt8 provPairingRef, gpPd_Handle_t pdHandle)
{
    const ROM gpRf4ceDispatcher_BindMapping_t* FLASH_PROGMEM pBindMapping;
    gpRf4ceDispatcher_BindMapping_t bindMapping;
    gpRf4ceDispatcher_cbMEPairIndication_t pairIndicationCallbackCb;
    pBindMapping = gpRf4ceDispatcher_BindMapping;
    
    while(true)
    {
        MEMCPY_P(&bindMapping, pBindMapping, sizeof(gpRf4ceDispatcher_BindMapping_t));
        if(bindMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        MEMCPY_P(&pairIndicationCallbackCb, (((UIntPtr_P)bindMapping.callbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, pairIndicationCallback)), sizeof(gpRf4ceDispatcher_cbMEPairIndication_t));
        if(pairIndicationCallbackCb != NULL)
        {
            if(pairIndicationCallbackCb(status, srcPANId, pSrcIEEEAddr, orgNodeCapabilities, orgVendorId, orgVendorStringOffset, orgAppCapabilities, orgUserStringOffset, orgDevTypeListOffset, orgProfileIdListOffset, keyExTransferCount, provPairingRef , pdHandle))
            {
                //pairIndicationCallback handled
                return;
            }
        }
        pBindMapping++;
    }
}
#endif // GP_RF4CE_DIVERSITY_PAIR_RECIPIENT

#ifdef GP_RF4CE_DIVERSITY_PAIR_ORIGINATOR
void gpRf4ce_cbPairConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpRf4ce_VendorId_t recVendorId, gpPd_Offset_t recVendorStringOffset, UInt8 recAppCapabilities, gpPd_Offset_t recUserStringOffset, gpPd_Offset_t recDevTypeListOffset, gpPd_Offset_t recProfileIdListOffset, gpPd_Handle_t pdHandle)
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEPairConfirm_t pairConfirmCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&pairConfirmCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, pairConfirmCallback)), sizeof(gpRf4ceDispatcher_cbMEPairConfirm_t));
    GP_ASSERT_DEV_EXT(pairConfirmCb != NULL);

    pairConfirmCb(status, pairingRef, recVendorId, recVendorStringOffset, recAppCapabilities, recUserStringOffset, recDevTypeListOffset, recProfileIdListOffset, pdHandle);  
}
#endif //GP_RF4CE_DIVERSITY_PAIR_ORIGINATOR

void gpRf4ce_cbUnpairIndication(UInt8 pairingRef)
{
    const ROM gpRf4ceDispatcher_BindMapping_t* FLASH_PROGMEM pBindMapping;
    gpRf4ceDispatcher_BindMapping_t bindMapping;
    gpRf4ceDispatcher_cbMEUnpairIndication_t unPairIndicationCallbackCb;
    pBindMapping = gpRf4ceDispatcher_BindMapping;
    while(true)
    {
        MEMCPY_P(&bindMapping, pBindMapping, sizeof(gpRf4ceDispatcher_BindMapping_t));
        if(bindMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        MEMCPY_P(&unPairIndicationCallbackCb, (((UIntPtr_P)bindMapping.callbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, unpairIndicationCallback)), sizeof(gpRf4ceDispatcher_cbMEUnpairIndication_t));
        if(unPairIndicationCallbackCb != NULL)
        {
            unPairIndicationCallbackCb(pairingRef);
        }
        pBindMapping++;
    }
    if(gpRf4ce_IsPairingTableEntryValid(pairingRef))
    {
        //No-one cared to unpair the reference up to now
        gpRf4ce_UnpairResponse(pairingRef);
    }
}

#ifdef GP_RF4CE_DIVERSITY_UNPAIR_ORIGINATOR
void gpRf4ce_cbUnpairConfirm(gpRf4ce_Result_t status, UInt8 pairingRef)
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbMEUnpairConfirm_t unpairConfirmCallbackCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&unpairConfirmCallbackCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, unpairConfirmCallback)), sizeof(gpRf4ceDispatcher_cbMEUnpairConfirm_t));
    GP_ASSERT_DEV_EXT(unpairConfirmCallbackCb != NULL);
    unpairConfirmCallbackCb(status, pairingRef); 
}
#endif //GP_RF4CE_DIVERSITY_UNPAIR_ORIGINATOR

#ifdef GP_RF4CE_DIVERSITY_GENERIC_CMD_ORIGINATOR
void gpRf4ce_cbGenericCmdConfirm( gpRf4ce_Result_t result , gpRf4ce_CommandId_t genericCmdId )
{
    const ROM gpRf4ceDispatcher_BindCallbacks_t* FLASH_PROGMEM  pBindCallbacks;
    gpRf4ceDispatcher_cbGenericCmdConfirm_t genericCmdConfirmCb;
    GP_ASSERT_DEV_EXT(gpRf4ceDispatcher_LockFunctionalBlock != GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID);
    pBindCallbacks = Rf4ceDispatcher_FindBindBlock();
    GP_ASSERT_DEV_EXT(pBindCallbacks != NULL);
    MEMCPY_P(&genericCmdConfirmCb , (((UIntPtr_P)pBindCallbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, genericCmdConfirmCallback)), sizeof(gpRf4ceDispatcher_cbGenericCmdConfirm_t));
    GP_ASSERT_DEV_EXT(genericCmdConfirmCb != NULL);

    genericCmdConfirmCb(result, genericCmdId );
}
#endif //GP_RF4CE_DIVERSITY_GENERIC_CMD_ORIGINATOR

#ifdef GP_RF4CE_DIVERSITY_GENERIC_CMD_RECIPIENT
void gpRf4ce_cbGenericCmdIndication( UInt8 pairingRef , gpRf4ce_PANID_t srcPANId, MACAddress_t* pSrcAddr , UInt8 rxFlags , gpRf4ce_CommandId_t genericCmdId , UInt8 cmdPayloadLength , UInt8 *pCmdPayload )
{
    const ROM gpRf4ceDispatcher_BindMapping_t* FLASH_PROGMEM pBindMapping;
    gpRf4ceDispatcher_BindMapping_t bindMapping;
    gpRf4ceDispatcher_cbGenericCmdIndication_t genericCmdIndicationCallbackCb;
    pBindMapping = gpRf4ceDispatcher_BindMapping;
    
    while(true)
    {
        MEMCPY_P(&bindMapping, pBindMapping, sizeof(gpRf4ceDispatcher_BindMapping_t));
        if(bindMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        MEMCPY_P(&genericCmdIndicationCallbackCb, (((UIntPtr_P)bindMapping.callbacks) + offsetof(gpRf4ceDispatcher_BindCallbacks_t, genericCmdIndicationCallback)), sizeof(gpRf4ceDispatcher_cbGenericCmdIndication_t));
        if(genericCmdIndicationCallbackCb != NULL)
        {
            if(genericCmdIndicationCallbackCb( pairingRef , srcPANId, pSrcAddr , rxFlags , genericCmdId , cmdPayloadLength , pCmdPayload ))
            {
                //genericCmdIndicationCallback handled
                return;
            }
        }
        pBindMapping++;
    }
}
#endif //GP_RF4CE_DIVERSITY_GENERIC_CMD_RECIPIENT

void gpRf4ce_cbResetConfirm( gpRf4ce_Result_t status )
{
    const ROM gpRf4ceDispatcher_DataMapping_t* FLASH_PROGMEM pDataMapping;
    gpRf4ceDispatcher_DataMapping_t dataMapping;
    gpRf4ceDispatcher_cbResetIndication_t resetIndicationCb;

    GP_ASSERT_DEV_EXT(status == gpRf4ce_ResultSuccess);


    //trigger reset-indication in registered functional blocks
    pDataMapping = gpRf4ceDispatcher_DataMapping;
    while(true)
    {
        MEMCPY_P(&dataMapping, pDataMapping, sizeof(gpRf4ceDispatcher_DataMapping_t));
        if(dataMapping.functionalBlockID == GP_RF4CE_DISPATCHER_INVALID_FUNCTIONAL_BLOCK_ID)
        {
            break;
        }
        /* NOTE:
         * "warning: passing argument 2 of 'memcpy_P' makes pointer from integer without a cast" is ok,
         * Attempt to resolve this warning by adding a (void*) cast gives trouble with PIC related projects. 
        */            
        MEMCPY_P(&resetIndicationCb, (((UIntPtr_P)dataMapping.callbacks) + offsetof(gpRf4ceDispatcher_DataCallbacks_t, resetIndicationCallback)), sizeof(gpRf4ceDispatcher_cbResetIndication_t));
        if(resetIndicationCb != NULL)
        {
            resetIndicationCb(Rf4ceDispatcher_SetDefaultOnReset);
        }
        pDataMapping++;
    }

    gpRf4ceDispatcher_cbResetConfirm(status);

}

