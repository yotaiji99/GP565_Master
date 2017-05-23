/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Zrc.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_Zrc.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_Zrc.h"

#ifdef GP_DIVERSITY_APP_ZID
#include "gpController_Zid.h"
#endif

#include "gpPoolMem.h"
#include "gpSched.h"
#include "gpLog.h"
#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
#include "gpRf4ceVoice.h"
#endif

#if defined(GP_DIVERSITY_APP_ZRC2_0) && defined(GP_DIVERSITY_APP_ZRC1_1)
#error error: GP_DIVERSITY_APP_ZRC2_0 and GP_DIVERSITY_APP_ZRC1_1 cannot be defined simultaneously
#endif /*defined(GP_DIVERSITY_APP_ZRC2_0) && defined(GP_DIVERSITY_APP_ZRC1_1)*/

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP
#define GP_CONTROLLER_ZRC_NODE_CAPABILITIES     0x0C        /*  normalization -
                                                                security -
                                                                controller */
#define GP_CONTROLLER_MIN_POLLING_TIME_INTERVAL_MS     1000
#define GP_CONTROLLER_MAX_POLLING_TIME_INTERVAL_MS     900000
/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/
static void Zrc_Reset(void);
static void Zrc_ResetRequest(void);
#if defined(GP_DIVERSITY_APP_ZRC2_0) || defined(GP_DIVERSITY_APP_ZRC1_1)
static void Zrc_BindRequest( void );
#endif /*defined(GP_DIVERSITY_APP_ZRC2_0) || defined(GP_DIVERSITY_APP_ZRC1_1)*/
static void Zrc_SendKeys(gpController_Keys_t keys, UInt8 bindingId, gpRf4ce_ProfileId_t profileId, UInt16 TXOptions, gpRf4ce_VendorId_t vendorId);

#ifdef GP_DIVERSITY_APP_ZRC2_0
static void Zrc_KeyCodesToActionCodesSupportedList(const UInt8 *keyCodes, UInt8 keyCount, UInt8 *actionCodes);
#endif /*GP_DIVERSITY_APP_ZRC2_0*/

/******************************************************************************
 *                      Static Data Definitions
 ******************************************************************************/
#ifndef GP_DIVERSITY_APP_ZRC2_0
UInt8 activeKey;
#ifdef GP_DIVERSITY_APP_ZRC1_1
UInt8 KickoutBindingId;
#ifdef GP_DIVERSITY_APP_ZID
gpRf4ceBindAuto_OriginatorInfo_t gpController_appOriginatorInfoZrc = {
     /* orgAppCapabilities    */ 0x23, /* 2 profiles */
     /* pOrgDevTypeList[3]    */ {gpRf4ce_DeviceTypeRemoteControl},
     /* pOrgProfileIdList[7]  */ {gpRf4ce_ProfileIdZrc, gpRf4ce_ProfileIdZid},
     /* searchDevType         */ gpRf4ce_DeviceTypeWildcard,
     /* discProfileIdListSize */ 2,
     /* pDiscProfileIdList[7] */ {(gpRf4ce_ProfileId_t)gpRf4ce_ProfileIdZrc, (gpRf4ce_ProfileId_t)gpRf4ce_ProfileIdZid},
     /* userString            */ {"GP"},
     /* keyExTransferCount    */ 3
    };
#else
gpRf4ceBindAuto_OriginatorInfo_t gpController_appOriginatorInfoZrc = {
     /* orgAppCapabilities    */ (1 << GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_IDX) | (1 << GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_IDX),
     /* pOrgDevTypeList[3]    */ {gpRf4ce_DeviceTypeRemoteControl},
     /* pOrgProfileIdList[7]  */ {gpRf4ce_ProfileIdZrc},
     /* searchDevType         */ gpRf4ce_DeviceTypeWildcard,
     /* discProfileIdListSize */ 1,
     /* pDiscProfileIdList[7] */ {(gpRf4ce_ProfileId_t)gpRf4ce_ProfileIdZrc},
     /* userString            */ {"GP"},
     /* keyExTransferCount    */ 3
    };
#endif
#endif /*GP_DIVERSITY_APP_ZRC1_1*/
#endif /*!GP_DIVERSITY_APP_ZRC2_0*/
/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/

/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/
void gpController_Zrc_Init(void)
{
    /* ZRC initialization here */
#ifndef GP_DIVERSITY_APP_ZRC2_0
    activeKey = 0xff;
#ifdef GP_DIVERSITY_APP_ZRC1_1
    KickoutBindingId=0xff;
#endif /*GP_DIVERSITY_APP_ZRC1_1*/
#endif /*!GP_DIVERSITY_APP_ZRC2_0*/
}

void gpController_Zrc_Msg(gpController_Zrc_MsgId_t msgId, gpController_Zrc_Msg_t *pMsg)
{
#ifdef GP_DIVERSITY_APP_ZRC2_0
    gpZrc_Msg_t msg;
#endif

    switch(msgId)
    {

        case gpController_Zrc_MsgId_ResetRequest:
        {
            Zrc_ResetRequest();
            break;
        }

        case gpController_Zrc_MsgId_KeyPressedIndication:
        {
            if(pMsg->KeyPressedIndication.bindingId != 0xff)
            {
                Zrc_SendKeys(pMsg->KeyPressedIndication.keys, pMsg->KeyPressedIndication.bindingId, pMsg->KeyPressedIndication.profileId, pMsg->KeyPressedIndication.txOptions, pMsg->KeyPressedIndication.vendorId);
            }
            else
            {
                gpController_Zrc_Msg_t *pZrcMsg;
                pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

                pZrcMsg->ActionControlConfirmParams.bindingId = pMsg->KeyPressedIndication.bindingId;
                pZrcMsg->ActionControlConfirmParams.status = gpRf4ce_ResultNoPairing;
                gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbKeyConfirmedIndication, pZrcMsg);

                gpPoolMem_Free(pZrcMsg);
            }
            break;
        }

        case gpController_Zrc_MsgId_BindAbort:
        {
#ifdef GP_DIVERSITY_APP_ZRC2_0
            gpZrc_OriginatorBindAbortRequest_Param_struct_t *abort;

            abort = &msg.OriginatorBindAbortRequest;
            abort->fullAbort = false;

            gpZrc_Msg(gpZrc_MsgId_OriginatorBindAbort, &msg);
#endif  /*GP_DIVERSITY_APP_ZRC2_0*/
#ifdef GP_DIVERSITY_APP_ZRC1_1
            gpRf4ceBindAuto_BindAbort();
#endif  /*GP_DIVERSITY_APP_ZRC1_1*/
            break;
        }

        case gpController_Zrc_MsgId_BindAbortFull:
        {
#ifdef GP_DIVERSITY_APP_ZRC2_0
            gpZrc_OriginatorBindAbortRequest_Param_struct_t *abort;

            abort = &msg.OriginatorBindAbortRequest;
            abort->fullAbort = true;

            gpZrc_Msg(gpZrc_MsgId_OriginatorBindAbort, &msg);
#endif  /*GP_DIVERSITY_APP_ZRC2_0*/
#ifdef GP_DIVERSITY_APP_ZRC1_1
            gpRf4ceBindAuto_BindAbort();
#endif  /*GP_DIVERSITY_APP_ZRC1_1*/
            break;
        }

        case gpController_Zrc_MsgId_UnbindRequest:
        {
#ifdef GP_DIVERSITY_APP_ZRC2_0
            gpZrc_UnbindRequest_Param_struct_t *unbind;

            unbind = &msg.UnbindRequest;
            unbind->bindingId = pMsg->bindingId;

            gpZrc_Msg(gpZrc_MsgId_UnbindRequest, &msg);
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
#ifdef GP_DIVERSITY_APP_ZRC1_1
            gpRf4ceBindAuto_UnbindRequest(pMsg->bindingId);
#endif  /*GP_DIVERSITY_APP_ZRC1_1*/
            break;
        }
#ifdef GP_DIVERSITY_APP_ZRC2_0
        case gpController_Zrc_MsgId_ActionCodesSupportedIndication:
        {
            gpZrc_Result_t result = gpZrc_ResultSuccess;

            result = gpZrc_GetDefault(gpZrc_MsgId_OriginatorSetActionCodesSupportedAttribute, &msg);

            if( result == gpZrc_ResultSuccess)
            {

                gpZrc_OriginatorSetActionCodesSupported_Param_struct_t *actionCodesSupported;
                actionCodesSupported = &msg.OriginatorSetActionCodesSupported;

                actionCodesSupported->bankNumber = 0;
                Zrc_KeyCodesToActionCodesSupportedList( pMsg->keyMap.keys,
                                                        pMsg->keyMap.count,
                                                        actionCodesSupported->ActionCodesSupportedList);

                gpZrc_Set(gpZrc_MsgId_OriginatorSetActionCodesSupportedAttribute, &msg);
            }

            break;
        }
        case gpController_Zrc_MsgId_SendPowerStatusRequest:
        {
            msg.OriginatorPowerStatusRequest.bindingId    = pMsg->PowerStatusUpdate.bindingId;
            msg.OriginatorPowerStatusRequest.powerStatus  = pMsg->PowerStatusUpdate.powerStatus;
            gpZrc_Msg(gpZrc_MsgId_OriginatorPowerStatusRequest, &msg);
            break;
        }
#endif /*GP_DIVERSITY_APP_ZRC2_0*/

#if defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)
        case gpController_Zrc_MsgId_BindRequest:
        {
            gpSched_ScheduleEvent(0, Zrc_BindRequest);
            break;
        }
#endif /*defined(GP_DIVERSITY_APP_ZRC1_1) || defined(GP_DIVERSITY_APP_ZRC2_0)*/
        default:
        {
            GP_LOG_SYSTEM_PRINTF("gpController_Zrc_Msg: Unknown msgId 0x%x",0,msgId);
            break;
        }
    }
}

/*******************************************************************************
 *                      ZRC Callbacks
 ******************************************************************************/
#ifndef GP_DIVERSITY_APP_ZRC2_0
void gpRf4ceDispatcher_cbResetConfirm(gpRf4ce_Result_t status)
#else
void gpZrc_cbResetConfirm( gpRf4ce_Result_t status )
#endif /*!GP_DIVERSITY_APP_ZRC2_0*/
{
#ifdef GP_DIVERSITY_APP_ZRC2_0
    gpZrc_Result_t result = gpZrc_ResultSuccess;
    gpZrc_Msg_t* pZrcMsg;
    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpZrc_Msg_t));

    /* set some stack parameters */
    result = gpZrc_GetDefault(gpZrc_MsgId_OriginatorSetActionBanksSupportedAttribute, pZrcMsg);

    if( result == gpZrc_ResultSuccess)
    {
        gpZrc_OriginatorSetActionBanksSupported_Param_struct_t *actionBanksSupported;
        actionBanksSupported = &pZrcMsg->OriginatorSetActionBanksSupported;

        /* activate the first action bank */
        actionBanksSupported->ActionBanksSupportedList[0] |=
                0x01 << (gpRf4ceCmd_ZrcActionBankLo_HDMI + 0x20 * gpRf4ceCmd_ZrcActionBankHi_HDMI);

        gpZrc_Set(gpZrc_MsgId_OriginatorSetActionBanksSupportedAttribute, pZrcMsg);
    }

    gpPoolMem_Free(pZrcMsg);
#endif /*GP_DIVERSITY_APP_ZRC2_0*/

    /* The Rf4ce NWK has been reset successfully */
    GP_ASSERT_DEV_EXT( status == gpRf4ce_ResultSuccess );
    GP_LOG_SYSTEM_PRINTF("Reset...",0);
    /* confirm to application that reset is done */
    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbResetConfirm, NULL);
}

#ifdef GP_DIVERSITY_APP_ZRC2_0
void gpZrc_cbMsg(gpZrc_MsgId_t MsgId, UInt8 length, gpZrc_Msg_t* pMsg)
{
    gpController_Zrc_Msg_t *pZrcMsg;
    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

    switch(MsgId)
    {
        case gpZrc_MsgId_cbOriginatorBindConfirm:
        {
            UInt8 i;
            pZrcMsg->BindConfirmParams.bindingId = pMsg->cbOriginatorBindConfirm.bindingRef;
            pZrcMsg->BindConfirmParams.status = pMsg->cbOriginatorBindConfirm.result;
            pZrcMsg->BindConfirmParams.profileId = gpRf4ce_ProfileIdZrc;

            if(pMsg->cbOriginatorBindConfirm.result == gpRf4ce_ResultSuccess)
            {
                for(i=0;i<pMsg->cbOriginatorBindConfirm.profileIdListSize;i++)
                {
                    if(pMsg->cbOriginatorBindConfirm.pProfileIdList[i] == gpRf4ce_ProfileIdZrc2_0)
                    {
                        pZrcMsg->BindConfirmParams.profileId = gpRf4ce_ProfileIdZrc2_0;
                        break;
                    }
                }
            }
            else
            {
                pZrcMsg->BindConfirmParams.profileId = 0xff;
            }

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbBindConfirm, pZrcMsg);
            break;
        }
        case gpZrc_MsgId_cbOriginatorStartValidationIndication: /* not handled yet  */
        {
            pZrcMsg->ValidationParams.bindingId = pMsg->cbOriginatorStartValidationIndication.bindingRef;
            /*Validation is a feature of ZRC2.0*/
            pZrcMsg->ValidationParams.profileId = gpRf4ce_ProfileIdZrc2_0;

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbStartValidationIndication, pZrcMsg);
            break;
        }
        case gpZrc_MsgId_cbOriginatorTxActionCodeConfirm:
        {
            pZrcMsg->ActionControlConfirmParams.bindingId = pMsg->cbOriginatorTxActionCodeConfirm.pairingRef;
            pZrcMsg->ActionControlConfirmParams.status = pMsg->cbOriginatorTxActionCodeConfirm.status;

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbKeyConfirmedIndication, pZrcMsg);
            break;
        }
        case gpZrc_MsgId_cbOriginatorTxActionCodeRepeatFailureIndication:
        {
            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbKeyFailedIndication, NULL);
            break;
        }
        case gpZrc_MsgId_cbUnbindConfirm:
        {
            pZrcMsg->UnbindConfirmParams.bindingId = pMsg->cbUnbindConfirm.bindingId;
            pZrcMsg->UnbindConfirmParams.status = pMsg->cbUnbindConfirm.status;

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbUnbindConfirm, pZrcMsg);
            break;
        }
        case gpZrc_MsgId_cbUnbindIndication:
        {
            pZrcMsg->bindingId = pMsg->cbUnbindConfirm.bindingId;

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbUnbindIndication, pZrcMsg);
            break;
        }

        case gpZrc_MsgId_cbOriginatorFailValidationIndication:
        {
            pZrcMsg->ValidationParams.bindingId = pMsg->cbOriginatorFailValidationIndication.bindingRef;
            pZrcMsg->ValidationParams.profileId = gpRf4ce_ProfileIdZrc2_0;

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbFailValidationIndication, pZrcMsg);
            break;
        }

        case gpZrc_MsgId_cbOriginatorPowerStatusConfirm:
        {
            pZrcMsg->PowerStatusUpdateConfirm.bindingId = pMsg->cbOriginatorPowerStatusConfirm.bindingId;
            pZrcMsg->PowerStatusUpdateConfirm.status = pMsg->cbOriginatorPowerStatusConfirm.result;

            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbSendPowerStatusConfirm, pZrcMsg);
            break;
        }


        case gpZrc_MsgId_cbOriginatorGetActionBanksRxRecipientIndication:
        {
            break;
        }
        case gpZrc_MsgId_cbOriginatorEarlyRepeatIndication:
        {
            gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbEarlyRepeatIndication, NULL);
            break;
        }

        default:
        {
            GP_LOG_SYSTEM_PRINTF("Unknown message ID %x", 0, MsgId);
            break;
        }
    }

    gpPoolMem_Free(pZrcMsg);
}
#endif /*GP_DIVERSITY_APP_ZRC2_0*/

#ifdef GP_DIVERSITY_APP_ZRC1_1
void KickoutLru(void)
{
    if(KickoutBindingId != 0xff)
    {
        gpRf4ceBindAuto_UnbindRequest(KickoutBindingId);
    }
}
/* gpRf4ceDispatcher callbacks */
void gpRf4ceBindAuto_cbOriginatorBindConfirm(UInt8 bindingId,gpRf4ce_Result_t status)
{
    if(status == gpRf4ce_ResultSuccess)
    {
        /* Make room for new pairing entries by removing the least recently used entry */
        gpRf4ceKickOutLru_Desc_t removeDesc;
        removeDesc = gpRf4ceKickOutLru_AddDeviceToList(bindingId, gpRf4ce_ProfileIdZrc);

        if( removeDesc.pairingRef != 0xFF )
        {
            KickoutBindingId = removeDesc.pairingRef;
            /* Schedule unpair - extra time to get the ZID configuration fail */
            gpSched_ScheduleEvent(500000UL, KickoutLru);
        }
#ifdef GP_DIVERSITY_APP_ZID
        /* Start ZID configuration */    
        gpController_Zid_Msg_t *pZidMsg;
        pZidMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zid_Msg_t));

        pZidMsg->bindingId = bindingId;
        pZidMsg->profileId = gpRf4ce_ProfileIdZid;
        pZidMsg->status = status;
        
        gpController_Zid_Msg(gpController_Zid_MsgId_ConfigurationRequest, pZidMsg);
        gpPoolMem_Free(pZidMsg);
        return;
#endif
    }
    gpController_Zrc_Msg_t *pZrcMsg;
    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

    pZrcMsg->BindConfirmParams.bindingId = bindingId;
    pZrcMsg->BindConfirmParams.status = status;
    pZrcMsg->BindConfirmParams.profileId = gpRf4ce_ProfileIdZrc;
    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbBindConfirm, pZrcMsg);

    gpPoolMem_Free(pZrcMsg);
}

void gpRf4ceBindAuto_cbUnbindConfirm(UInt8 bindingId, gpRf4ce_Result_t status)
{
    gpController_Zrc_Msg_t *pZrcMsg;
    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

    pZrcMsg->UnbindConfirmParams.bindingId = bindingId;
    pZrcMsg->UnbindConfirmParams.status = status;
    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbUnbindConfirm, pZrcMsg);

    gpPoolMem_Free(pZrcMsg);

    if( (status == gpRf4ce_ResultTransactionOverflow) && (bindingId == KickoutBindingId) )
    {
        /*Device is already removed from kickoutlru so on reset this pairing entry will be removed*/
        GP_ASSERT_SYSTEM(false);
    }
    KickoutBindingId = 0xff;
}

void gpRf4ceBindAuto_cbUnbindIndication( UInt8 bindingId )
{
    gpController_Zrc_Msg_t *pZrcMsg;

    gpRf4ceKickOutLru_RemoveDeviceFromList(bindingId);

    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

    pZrcMsg->bindingId = bindingId;
    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbUnbindIndication, pZrcMsg);

    gpPoolMem_Free(pZrcMsg);
}
#endif /*GP_DIVERSITY_APP_ZRC1_1*/
#ifndef GP_DIVERSITY_APP_ZRC2_0
void gpRf4ceUserControl_cbUserControlConfirm(UInt8                            bindingId,
                                             gpRf4ce_Result_t                 status,
                                             gpRf4ceUserControl_KeyStatus_t   keyStatus,
                                             gpRf4ceUserControl_CommandCode_t commandCode,
                                             UInt16                           txOptions)
{
    gpController_Zrc_Msg_t *pZrcMsg;
    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

    pZrcMsg->ActionControlConfirmParams.bindingId = bindingId;
    pZrcMsg->ActionControlConfirmParams.status = status;
    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbKeyConfirmedIndication, pZrcMsg);

    gpPoolMem_Free(pZrcMsg);
}

void gpRf4ceUserControl_cbUserControlRepeatFailureIndication(UInt8                            bindingId,
                                                             gpRf4ce_Result_t                 status,
                                                             gpRf4ceUserControl_KeyStatus_t   keyStatus,
                                                             gpRf4ceUserControl_CommandCode_t commandCode,
                                                             UInt16                           txOptions)
{
    gpController_Zrc_Msg_t *pZrcMsg;
    pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbKeyFailedIndication, NULL);

    gpPoolMem_Free(pZrcMsg);
}

void gpRf4ceUserControl_cbEarlyRepeatIndication(void)
{
    gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbEarlyRepeatIndication, NULL);
}
#endif /*!GP_DIVERSITY_APP_ZRC2_0*/
/*******************************************************************************
 *                      Static Functions
 ******************************************************************************/
static void Zrc_ResetRequest(void)
{
    gpSched_ScheduleEvent(0, Zrc_Reset);
}

#ifdef GP_DIVERSITY_APP_ZRC2_0
#define ResetRequest gpZrc_ResetRequest
#else
#define ResetRequest gpRf4ceDispatcher_ResetRequest
#endif
static void Zrc_Reset(void)
{
    Bool coldStart = gpRf4ce_IsColdStartAdvised( GP_CONTROLLER_ZRC_NODE_CAPABILITIES );
    ResetRequest(coldStart, GP_CONTROLLER_ZRC_NODE_CAPABILITIES);
}

#if defined(GP_DIVERSITY_APP_ZRC2_0) || defined(GP_DIVERSITY_APP_ZRC1_1)
static void Zrc_BindRequest( void )
{
#ifdef GP_DIVERSITY_APP_ZRC2_0
    gpZrc_Result_t result = gpZrc_ResultSuccess;
    gpZrc_Msg_t msg;

    result = gpZrc_GetDefault(gpZrc_MsgId_OriginatorBindRequest, &msg);

    if( result == gpZrc_ResultSuccess)
    {
        gpZrc_OriginatorBindRequest_Param_struct_t *bindRequest = &msg.OriginatorBindRequest;
#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
        bindRequest->orgAppCapabilities     = 0x25; //daga review
#endif
        bindRequest->keyExTransferCount     = 3;
        bindRequest->vendorIdFilter         = 0xFFFF;
        bindRequest->pOrgDevTypeList[0]     = gpRf4ce_DeviceTypeRemoteControl;
        bindRequest->pOrgProfileIdList[0]     = gpRf4ce_ProfileIdZrc2_0;
#ifdef GP_RF4CEVOICE_DIVERSITY_ORIGINATOR
        bindRequest->pOrgProfileIdList[1]     = gpRf4ce_ProfileIdVoice;
#endif
        bindRequest->searchDevType          = gpRf4ce_DeviceTypeWildcard;

        gpZrc_Msg(gpZrc_MsgId_OriginatorBindRequest, &msg);
    }
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
#ifdef GP_DIVERSITY_APP_ZRC1_1
    MEMSET(gpController_appOriginatorInfoZrc.userString.str,0,GP_RF4CE_MAX_USER_STRING_LENGTH);
    gpRf4ce_GetUserString(gpController_appOriginatorInfoZrc.userString.str);
    gpRf4ceBindAuto_OriginatorBindRequest(&gpController_appOriginatorInfoZrc);
#endif /*GP_DIVERSITY_APP_ZRC1_1*/
}
#endif /*defined(GP_DIVERSITY_APP_ZRC2_0) || defined(GP_DIVERSITY_APP_ZRC1_1)*/

static void Zrc_SendKeys(gpController_Keys_t keys, UInt8 bindingId, gpRf4ce_ProfileId_t profileId, UInt16 TXOptions, gpRf4ce_VendorId_t vendorId)
{
#ifdef GP_DIVERSITY_APP_ZRC2_0
    gpZrc_Msg_t* pMsg;
    pMsg = GP_POOLMEM_MALLOC(sizeof(gpZrc_Msg_t));

    if(gpZrc_GetDefault(gpZrc_MsgId_OriginatorTxActionCodesNoPayloadRequest, pMsg) == gpZrc_ResultSuccess)
    {
        pMsg->OriginatorTxActionCodesNoPayloadRequest.bindingId = bindingId;
        pMsg->OriginatorTxActionCodesNoPayloadRequest.profileId = profileId;
        pMsg->OriginatorTxActionCodesNoPayloadRequest.txOptions = TXOptions;
        pMsg->OriginatorTxActionCodesNoPayloadRequest.numberOfActions = 0;

        if(keys.count <= GP_RF4CECMD_DIVERSITY_CONCURRENT_ACTIONS) /*if more keys are pressed than we have room for, we will ignore all key presses and send key release for all currently pressed keys.*/
        {
            UInt8 i = 0;

            for (i=0; i<keys.count; i++)
            {
                pMsg->OriginatorTxActionCodesNoPayloadRequest.ActionRecordList[i].ActionBank    = 0x00;
                pMsg->OriginatorTxActionCodesNoPayloadRequest.ActionRecordList[i].ActionCode    = keys.codes[i];
                pMsg->OriginatorTxActionCodesNoPayloadRequest.ActionRecordList[i].ActionControl = 0x01;
                pMsg->OriginatorTxActionCodesNoPayloadRequest.ActionRecordList[i].ActionVendor  = vendorId;
                pMsg->OriginatorTxActionCodesNoPayloadRequest.numberOfActions++;
            }
        }

        gpZrc_Msg(gpZrc_MsgId_OriginatorTxActionCodesNoPayloadRequest, pMsg);
    }

    gpPoolMem_Free(pMsg);
#else
    if(keys.count == 1)
    {
        if(activeKey == 0xff)
        {
#ifdef GP_DIVERSITY_APP_ZID
            if(profileId == gpRf4ce_ProfileIdZid && gpController_Zrc_IsZidProfileSupported())
            {
                profileId = gpRf4ce_ProfileIdZrc;
            }
#endif            
                    
            /*no pending key*/
            activeKey = keys.codes[0];
            gpRf4ceUserControl_UserControlRequest(bindingId,
                                                  profileId,
                                                  vendorId,
                                                  gpRf4ceUserControl_KeyStatusPressed,
                                                  keys.codes[0],
                                                  TXOptions,
                                                  GP_RF4CE_NWKC_MAX_DUTYCYCLE_MS);
            return;
        }
    }
    else if(keys.count == 0) /*if more keys are pressed than we have room for, we will ignore all key presses and send key release for all currently pressed keys.*/
    {
        if(activeKey != 0xff)
        {
#ifdef GP_DIVERSITY_APP_ZID
            if(profileId == gpRf4ce_ProfileIdZid && gpController_Zrc_IsZidProfileSupported())
            {
                profileId = gpRf4ce_ProfileIdZrc;
            }
#endif            
               
            gpRf4ceUserControl_UserControlRequest(bindingId,
                                                  profileId,
                                                  vendorId,
                                                  gpRf4ceUserControl_KeyStatusReleased,
                                                  activeKey,
                                                  TXOptions,
                                                  GP_RF4CE_NWKC_MAX_DUTYCYCLE_MS);
            activeKey = 0xff;
            return;
        }
    }

    {
        gpController_Zrc_Msg_t *pZrcMsg;
        pZrcMsg = GP_POOLMEM_MALLOC(sizeof(gpController_Zrc_Msg_t));

        pZrcMsg->ActionControlConfirmParams.bindingId = bindingId;
        pZrcMsg->ActionControlConfirmParams.status = gpRf4ce_ResultTransactionOverflow;
        gpController_Zrc_cbMsg(gpController_Zrc_MsgId_cbKeyConfirmedIndication, pZrcMsg);

        gpPoolMem_Free(pZrcMsg);
    }
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
}

#ifdef GP_DIVERSITY_APP_ZRC2_0
static void Zrc_KeyCodesToActionCodesSupportedList(const UInt8 *keyCodes, UInt8 keyCount, UInt8 *actionCodes)
{
    UInt8 i = 0;

    for(i=0; i<keyCount; i++)
    {
        UInt8 keyCode = keyCodes[i];
        UInt8 index = keyCode/8;
        UInt8 shift = keyCode - ((index) * 8);
        actionCodes[index] |= (1 << shift);
    }


#if 0
    /* Print the action codes set (debug). */
    for(i=0; i<32; i++)
    {
        GP_LOG_SYSTEM_PRINTF("%x", 0, actionCodes[i]);
    }
#endif

}
#endif /*GP_DIVERSITY_APP_ZRC2_0*/


#ifdef GP_DIVERSITY_APP_ZID
Bool gpController_Zrc_IsZidProfileSupported(void)
{
    UInt8 i;
    for(i=0; i<GP_RF4CE_APP_CAPABILITY_GET_NBR_PROFILES(gpController_appOriginatorInfoZrc.orgAppCapabilities);i++)
    {
        if(gpController_appOriginatorInfoZrc.pOrgProfileIdList[i] == gpRf4ce_ProfileIdZrc)
        {
            return true;
        }
    }
    return false;
}
#endif

