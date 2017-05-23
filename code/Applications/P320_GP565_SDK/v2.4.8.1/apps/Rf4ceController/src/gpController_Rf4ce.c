/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Rf4ce.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_Rf4ce.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_Rf4ce.h"
#include "gpController.h"
#include "gpBaseComps.h"
#include "gpSched.h"
#include "gpRf4ceDispatcher.h"
#include "gpMacDispatcher.h"
#include "gpNvm.h"
#include "gpReset.h"
#include "gpLog.h"
/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#define APP_TX_DEFAULT_POWER               7 /* dBm */
#define APP_GOTOSLEEP_THRESHOLD         4000

/*******************************************************************************
 *                      Static Defines
 ******************************************************************************/
static UInt8 Rf4ceBindingId = 0xFF;
static UInt8 Rf4ceProfileId = 0xFF;
static UInt8 Rf4ceNodeCapabilities = 0x0C; /* normalization - security - controller */

/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/
static void Rf4ce_StartRequest(void);
static void Rf4ce_cbStartConfirm(void);
static void SendVendorData(UInt8 bindingId, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt16 txOptions, UInt8 *data, UInt8 len);

Bool gpRf4ceDispatcher_cbDEDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);
void gpRf4ceDispatcher_cbDEDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef,gpPd_Handle_t pdHandle);
/******************************************************************************
 *                      Const Data Structure Definitions
 ******************************************************************************/

/* RF4CE Dispatcher Application Callbacks */
GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_DataCallbacks_t ROM
    gpApp_DataCallbacks FLASH_PROGMEM = {
        NULL,
        gpRf4ceDispatcher_cbDEDataIndication,
        gpRf4ceDispatcher_cbDEDataConfirm,
        NULL,
        NULL
};

GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_BindCallbacks_t ROM
    gpApp_BindCallbacks FLASH_PROGMEM = {
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
};

#define NVM_TAG_BINDINGID           0x00
#define NVM_TAG_PROFILEID           NVM_TAG_BINDINGID + 1
#define NVM_TAG_NODECAPABILITIES    NVM_TAG_PROFILEID + 1
#if defined (NVM_TAG_ACTIONMAPPING_BASE)
#define NVM_TAG_VENDORID            NVM_TAG_ACTIONMAPPING_BASE + GP_RF4CEBOUND_MAX_INITIATOR_ACTIONMAPPINGS
#else
#define NVM_TAG_VENDORID            NVM_TAG_NODECAPABILITIES+1
#endif //(NVM_TAG_ACTIONMAPPING_BASE)

GP_NVM_CONST UInt8 ROM Rf4ceNodeCapabilitiesDefault FLASH_PROGMEM = 0x0C;
GP_NVM_CONST UInt8 ROM Rf4ceProfileIdDefault FLASH_PROGMEM = 0xFF;
GP_NVM_CONST UInt8 ROM Rf4ceBindingIdDefault FLASH_PROGMEM = 0xFF;
GP_NVM_CONST UInt16 ROM Rf4ceVendorIdDefault FLASH_PROGMEM = GP_RF4CE_NWKC_VENDOR_IDENTIFIER;

const gpNvm_Tag_t ROM gpController_Rf4ce_NvmSection[] FLASH_PROGMEM = {
        {(UInt8*)&Rf4ceBindingId,        sizeof(Rf4ceBindingId),        gpNvm_UpdateFrequencyLow, (GP_NVM_CONST UInt8*)&Rf4ceBindingIdDefault}
        ,{(UInt8*)&Rf4ceProfileId,        sizeof(Rf4ceProfileId),        gpNvm_UpdateFrequencyLow, (GP_NVM_CONST UInt8*)&Rf4ceProfileIdDefault}
        ,{(UInt8*)&Rf4ceNodeCapabilities, sizeof(Rf4ceNodeCapabilities), gpNvm_UpdateFrequencyLow, (GP_NVM_CONST UInt8*)&Rf4ceNodeCapabilitiesDefault}
        ,{(UInt8*)NULL, sizeof(gpRf4ce_VendorId_t), gpNvm_UpdateFrequencyInitOnly, (GP_NVM_CONST UInt8*)&Rf4ceVendorIdDefault}
    };



/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/
void gpController_Rf4ce_Init(void)
{
    gpBaseComps_StackInit();
    gpNvm_RegisterSection(GP_COMPONENT_ID, gpController_Rf4ce_NvmSection, sizeof(gpController_Rf4ce_NvmSection)/sizeof(gpNvm_Tag_t), NULL);
}

void gpController_Rf4ce_Msg(gpController_Rf4ce_MsgId_t msgId,
                            gpController_Rf4ce_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Rf4ce_MsgId_StartRequest:
        {
            gpSched_ScheduleEvent(0, Rf4ce_StartRequest);
            break;
        }

        case gpController_Rf4ce_MsgId_FactoryResetIndication:
        {
            gpNvm_ClearNvm();
            gpReset_ResetSystem( );
        }
        case gpController_Rf4ce_MsgId_BackupNvm:
        {
            gpNvm_Backup(GP_COMPONENT_ID, NVM_TAG_BINDINGID, NULL);
            gpNvm_Backup(GP_COMPONENT_ID, NVM_TAG_PROFILEID, NULL);
            gpNvm_Backup(GP_COMPONENT_ID, NVM_TAG_NODECAPABILITIES, NULL);
            break;
        }
        case gpController_Rf4ce_MsgId_SendVendorDataRequest:
        {
            gpController_Rf4ce_Msg_t Msg;

            if(gpRf4ceDispatcher_LockedByThisFunctionalBlock(GP_COMPONENT_ID))
            {
                // Dispatcher already locked by this function. That implies previous SendVendorDataRequest is
                // still running. Return transaction overflow.
                Msg.vendorDataConfirm.bindingId = Rf4ceBindingId;
                Msg.vendorDataConfirm.status = gpRf4ce_ResultTransactionOverflow;
                gpController_Rf4ce_cbMsg(gpController_Rf4ce_MsgId_cbSendVendorDataConfirm, &Msg);
            }
            else 
            {
                SendVendorData(Rf4ceBindingId, Rf4ceProfileId, pMsg->vendorData.vendorId, pMsg->vendorData.txOptions, pMsg->vendorData.data, pMsg->vendorData.len);
            }
            break;
        }
        case gpController_Rf4ce_MsgId_IsLinkSecure:
        {
            UInt8 LinkIsSecure = false;
            if(gpRf4ce_IsPairingTableEntryValid(pMsg->bindingId))
            {
                LinkIsSecure = gpRf4ce_IsLinkSecure(pMsg->bindingId);
            }
            pMsg->isLinkSecure = LinkIsSecure;
            break;
        }
        default:
        {
            /* Unknown message ID */
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}

void gpController_Rf4ce_AttrGet(gpController_Rf4ce_MsgId_t msgId,
                                gpController_Rf4ce_Msg_t *pMsg)
{
    switch(msgId)
    {

        case gpController_Rf4ce_MsgId_AttrBindingId:
        {
            pMsg->bindingId = Rf4ceBindingId;
            break;
        }
        case gpController_Rf4ce_MsgId_AttrProfileId:
        {
            pMsg->profileId = Rf4ceProfileId;
            break;
        }
        case gpController_Rf4ce_MsgId_AttrNodeCapabilities:
        {
            pMsg->nodeCapabilities = Rf4ceNodeCapabilities;
            break;
        }
        case gpController_Rf4ce_MsgId_AttrVendorId:
        {
            pMsg->vendorId = gpRf4ce_GetVendorId();
            break;
        }
        case gpController_Rf4ce_MsgId_AttrVendorString:
        {
            gpRf4ce_GetVendorString(&pMsg->vendorString);
            break;
        }
        case gpController_Rf4ce_MsgId_AttrUserString:
        {
            gpRf4ce_GetUserString( pMsg->userString.str );
            break;
        }
        default:
        {
            /* Unknown message ID */
            GP_ASSERT_DEV_EXT(false);
        }
    }
}

void gpController_Rf4ce_AttrSet(gpController_Rf4ce_MsgId_t msgId,
                                gpController_Rf4ce_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Rf4ce_MsgId_AttrBindingId:
        {
            Rf4ceBindingId = pMsg->bindingId;
            break;
        }
        case gpController_Rf4ce_MsgId_AttrProfileId:
        {
            Rf4ceProfileId = pMsg->profileId;
            break;
        }
        case gpController_Rf4ce_MsgId_AttrNodeCapabilities:
        {
            Rf4ceNodeCapabilities = pMsg->nodeCapabilities;
            break;
        }

        case gpController_Rf4ce_MsgId_AttrVendorId:
        {
            gpRf4ce_SetVendorId(pMsg->vendorId);
            gpNvm_Backup(GP_COMPONENT_ID, NVM_TAG_VENDORID, (UInt8*)&pMsg->vendorId);
            break;
        }
        case gpController_Rf4ce_MsgId_AttrVendorString:
        {
            gpRf4ce_SetVendorString(&pMsg->vendorString);
            break;
        }

        case gpController_Rf4ce_MsgId_AttrUserString:
        {
            gpRf4ce_SetUserString( pMsg->userString.str );
            break;
        }
        default:
        {
            /* Unknown message ID */
            GP_ASSERT_DEV_EXT(false);
            break;
        }
    }
}


/*******************************************************************************
 *                      Rf4ce Callbacks
 ******************************************************************************/
void gpRf4ce_cbStartConfirm(gpRf4ce_Result_t status)
{
    Rf4ce_cbStartConfirm();
}

/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static void Rf4ce_cbStartConfirm(void)
{
    gpRf4ce_VendorId_t Rf4ceVendorId;
    /*Update local info*/
    gpNvm_Restore(GP_COMPONENT_ID, NVM_TAG_BINDINGID, NULL);
    gpNvm_Restore(GP_COMPONENT_ID, NVM_TAG_PROFILEID, NULL);
    gpNvm_Restore(GP_COMPONENT_ID, NVM_TAG_NODECAPABILITIES , NULL);
    gpNvm_Restore(GP_COMPONENT_ID, NVM_TAG_VENDORID, (UInt8*)&Rf4ceVendorId);

    /* Inform lower layer about restored VendorID */
    gpRf4ce_SetVendorId(Rf4ceVendorId);

    /*Inform higher layer*/
    gpController_Rf4ce_cbMsg( gpController_Rf4ce_MsgId_cbStartConfirm, NULL);
}
static void Rf4ce_StartRequest(void)
{
     Int8 const defaultTransmitPowers[16] = {
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER,
        APP_TX_DEFAULT_POWER };

    /* set default transmit powers */
    gpMacDispatcher_SetDefaultTransmitPowers((Int8*)defaultTransmitPowers);

    if(gpRf4ce_IsColdStartAdvised(Rf4ceNodeCapabilities))
    {
        gpRf4ce_StartRequest();
    }
    else
    {

        /* warm start */
        Rf4ce_cbStartConfirm();
    }



#ifndef GP_DIVERSITY_XSIF_DEBUG_ENABLED
    gpSched_SetGotoSleepEnable(true);
    gpSched_SetGotoSleepThreshold(APP_GOTOSLEEP_THRESHOLD);
#endif /*GP_DIVERSITY_XSIF_DEBUG_ENABLED*/
}

static void SendVendorData(UInt8 bindingId, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt16 txOptions, UInt8 *data, UInt8 len)
{
    UInt8 pdHandle = 0;

    if(!gpRf4ceDispatcher_LockRequest(GP_COMPONENT_ID))
    {
        gpRf4ceDispatcher_cbDEDataConfirm( gpRf4ce_ResultTransactionOverflow , bindingId , 0xff );
        return;
    }

    pdHandle = gpPd_GetPd();
    if((gpPd_CheckPdValid(pdHandle) != gpPd_ResultValidHandle) || bindingId == 0xFF)
    {
        gpRf4ceDispatcher_cbDEDataConfirm( gpRf4ce_ResultTransactionOverflow , bindingId , 0xff );
        return;
    }

    gpPd_WriteByteStream(pdHandle, 0, len, data);

    gpRf4ce_DataRequest(
            /*pairingRef*/      bindingId,
            /*profileId*/       profileId,
            /*vendorId*/        vendorId,
            /*txOptions*/       (UInt8)txOptions,
            /*rfRetriesPeriod*/ GP_RF4CE_NWKC_MAX_DUTYCYCLE_MS,
            /*nsduLength*/      len,
            /*nsduOffset*/      0,
            /*pbHandle*/        pdHandle
    );
}

void gpRf4ceDispatcher_cbDEDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef,gpPd_Handle_t pdHandle)
{
    gpController_Rf4ce_Msg_t Msg;

    if((gpPd_CheckPdValid(pdHandle) == gpPd_ResultValidHandle))
    {
        gpPd_FreePd(pdHandle);
    }

    if (gpRf4ceDispatcher_LockedByThisFunctionalBlock(GP_COMPONENT_ID))
    {
        /* Release the dispatcher for the functional block */
        gpRf4ceDispatcher_ReleaseRequest( GP_COMPONENT_ID );
    }


    Msg.vendorDataConfirm.bindingId = pairingRef;
    Msg.vendorDataConfirm.status = status;
    gpController_Rf4ce_cbMsg(gpController_Rf4ce_MsgId_cbSendVendorDataConfirm, &Msg);

}

Bool gpRf4ceDispatcher_cbDEDataIndication( UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle )
{
    /* The controller do not expect to receive any packet from the target in ZRC or MSO profile*/
    return false;
    /*Example to parse incoming data:*/

    /*
    if ((profileId == 0xCC) && (nsduLength == (2+2+(28*2))))
    {
        //controller statistics packet
        UInt16 local_tempUInt16;
        UInt16 LatencyBin[28];

        GP_LOG_SYSTEM_PRINTF("== CONTROLLER LATENCY STATISTICS ==",0);
        gpPd_ReadByteStream(pdHandle, nsduOffset,     2, (UInt8*)&local_tempUInt16);
        LITTLE_TO_HOST_UINT16(&local_tempUInt16);
        GP_LOG_SYSTEM_PRINTF("Received: %i",2,(UInt16)local_tempUInt16);
        gpPd_ReadByteStream(pdHandle, nsduOffset + 2, 2, (UInt8*)&local_tempUInt16);
        LITTLE_TO_HOST_UINT16(&local_tempUInt16);
        GP_LOG_SYSTEM_PRINTF("Dropped: %i",2,(UInt16)local_tempUInt16);

        gpPd_ReadByteStream(pdHandle, nsduOffset + 4, 56, (UInt8*)LatencyBin);
        {
            UInt8 i = 0;
            for(i=0;i < NrOfElements(LatencyBin);i++)
            {
                LITTLE_TO_HOST_UINT16(&LatencyBin[i]);
                GP_LOG_SYSTEM_PRINTF("%i: %i",4,(UInt16)i, (UInt16) LatencyBin[i]);
            }
        }
        gpPd_FreePd(pdHandle);
        return true;
    }
    else
    {
        return false;
    }

    */
}

