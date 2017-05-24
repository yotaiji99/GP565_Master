/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Mso.c
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/src/gpController_Mso.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "gpController_Mso.h"
#include "gpSched.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpController_BatteryMonitor.h"

#ifdef GP_DIVERSITY_APP_ZRC2_0
extern void Rf4ceBound_clearHeartBeat(UInt8 bindingId);
#endif
/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

#define KEY_REPEAT_INTERVAL_MSO                     120         /* ms*/
#define SHORT_RF_RETRIES_PERIOD                     100000L     /* us*/

/*******************************************************************************
 *                      Static Function Declarations
 ******************************************************************************/
static void gpController_MSO_cbRIBSetConfirm(gpRf4ce_Result_t status , UInt8 bindingRef , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex);

/******************************************************************************
 *                      Static Data Definitions
 ******************************************************************************/
static UInt8 MsoBindingRef = 0xff;

const gpRf4ceBindValidation_OriginatorInfo_t controllerOriginatorInfo = {
     /* orgAppCapabilities    */ (1 << GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_IDX) | (1 << GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_IDX),
     /* pOrgDevTypeList[3]    */ {gpRf4ce_DeviceTypeRemoteControl},
     /* pOrgProfileIdList[7]  */ {(gpRf4ce_ProfileId_t)gpRf4ce_ProfileIdMso},
     /* searchDevType         */ gpRf4ce_DeviceTypeSetTopBox,
     /* discProfileIdListSize */ 1,
     /* pDiscProfileIdList[7] */ {(gpRf4ce_ProfileId_t)gpRf4ce_ProfileIdMso}
    };
/*******************************************************************************
 *                      Public Functions
 ******************************************************************************/
void gpController_Mso_Init(void)
{
    /* Mso initialization here */
}

void gpController_Mso_Msg(gpController_Mso_MsgId_t msgId, gpController_Mso_Msg_t *pMsg)
{
    switch(msgId)
    {
        case gpController_Mso_MsgId_BindRequest:
        {
            gpRf4ceBindValidation_OriginatorBindInfo_t orgBindInfo = {GP_RF4CE_BIND_VALIDATION_ORG_BI_INITIATORINDICATOR_DEDICATEDKEY};
            gpRf4ceBindValidation_BindRequest( gpRf4ce_ProfileIdMso,
                                                &controllerOriginatorInfo,
                                                3 /*KEY_EX_TRANSFER_COUNT*/,
                                                &orgBindInfo,
                                                SHORT_RF_RETRIES_PERIOD);        
            break;
        }
        case gpController_Mso_MsgId_BindAbort:
        {
            gpRf4ceBindValidation_BindAbortRequest(false);
            break;
        }
        case gpController_Mso_MsgId_BindAbortFull:
        {
            gpRf4ceBindValidation_BindAbortRequest(true);
            break;            
        }
        case gpController_Mso_MsgId_UnbindRequest:
        {
            gpRf4ceBindValidation_UnbindRequest((void*)(&pMsg->bindingId));
            break;
        }
        case gpController_Mso_MsgId_SendVersionInfoRequest:
        {           
            GP_LOG_PRINTF("gpController_Mso_MsgId_SendVersionInfoRequest",0);

            gpRf4ceRib_SetRibRequest(
                        pMsg->VersionInfo.bindingId,
                        gpRf4ce_ProfileIdMso,
                        pMsg->VersionInfo.vendorId,
                        gpRf4ceRib_RIBAttributeIdVersioning,
                        pMsg->VersionInfo.index,
                        4,
                        (UInt8*)&pMsg->VersionInfo.version,
                        SHORT_RF_RETRIES_PERIOD,
                        (gpRf4ceRib_cbSetRibConfirm_t)gpController_MSO_cbRIBSetConfirm
                        );
            break;
        }
        case gpController_Mso_MsgId_SendBatteryStatusRequest:
        {
            UInt8 ribData[11];
            
            MEMSET(ribData,0,sizeof(ribData)/sizeof(UInt8));
            ribData[0]  = pMsg->BatterStatusUpdate.flags;
            ribData[1]  = pMsg->BatterStatusUpdate.levelLoaded;
            HOST_TO_LITTLE_UINT32(&pMsg->BatterStatusUpdate.NumberOfRFSent);
            MEMCPY(&ribData[2],&pMsg->BatterStatusUpdate.NumberOfRFSent,4);
            HOST_TO_LITTLE_UINT32(&pMsg->BatterStatusUpdate.NumberOfIRSent);
            MEMCPY(&ribData[6],&pMsg->BatterStatusUpdate.NumberOfIRSent,4);
            ribData[10] = pMsg->BatterStatusUpdate.levelUnloaded;
            
            GP_LOG_PRINTF("gpController_Mso_MsgId_SendBatteryStatusRequest",0);

            gpRf4ceRib_SetRibRequest(
                        pMsg->BatterStatusUpdate.bindingId,
                        gpRf4ce_ProfileIdMso,
                        pMsg->BatterStatusUpdate.vendorId,
                        gpRf4ceRib_RIBAttributeIdBatteryStatus,
                        0,
                        11,
                        ribData,
                        SHORT_RF_RETRIES_PERIOD,
                        (gpRf4ceRib_cbSetRibConfirm_t)gpController_MSO_cbRIBSetConfirm
                        );
            break;
        }
        default:
        {
            break;
        }
    }
}

/*******************************************************************************
 *                      MSO BindValidation Callbacks
 ******************************************************************************/
/*
 * Callback function from the gpRf4ceBindValidation. Confirm MSO bind
 */
void gpRf4ceBindValidation_cbBindConfirm(UInt8 bindingId, gpRf4ce_Result_t status)
{   
    gpController_Mso_Msg_t msg;
    msg.BindConfirmParams.bindingId = bindingId;
    msg.BindConfirmParams.profileId = gpRf4ce_ProfileIdMso;
    msg.BindConfirmParams.status = status;
    gpController_Mso_cbMsg(gpController_Mso_MsgId_cbBindConfirm, &msg);
    MsoBindingRef = bindingId;
    GP_LOG_PRINTF("gpRf4ceBindValidation_cbBindConfirm, Status: 0x%x. BindingId: 0x%x",0,status, bindingId);
#if defined(GP_DIVERSITY_APP_ZRC2_0)
    Rf4ceBound_clearHeartBeat(bindingId); //Stop polling as we are MSO now.
#endif
}

/*
 *Callback function from the gpRf4ceBindValidation. MSO bind validation started
 */
void gpRf4ceBindValidation_cbStartValidationIndication( UInt8 bindingId )
{
    gpController_Mso_Msg_t msg;
    msg.ValidationParams.bindingId = bindingId;
    msg.ValidationParams.profileId = gpRf4ce_ProfileIdMso;
    msg.ValidationParams.status = gpRf4ce_ResultValidationPending;
    gpController_Mso_cbMsg(gpController_Mso_MsgId_cbStartValidationIndication, &msg);
    MsoBindingRef = bindingId;
    
    GP_LOG_PRINTF("gpRf4ceBindValidation_cbStartValidationIndication",0);
}

/*
 * Callback function from the gpRf4ceBindValidation. MSO bind validation failed
 */
void gpRf4ceBindValidation_cbFailValidationIndication( UInt8 bindingId, gpRf4ce_ResultValidation_t status )
{
    gpController_Mso_Msg_t msg;
    msg.ValidationParams.bindingId = bindingId;
    msg.ValidationParams.profileId = gpRf4ce_ProfileIdMso;
    msg.ValidationParams.status = status;
    gpController_Mso_cbMsg(gpController_Mso_MsgId_cbFailValidationIndication, &msg);
    MsoBindingRef = bindingId;
    GP_LOG_PRINTF("gpRf4ceBindValidation_cbFailValidationIndication, Status: 0x%x. BindingId: 0x%x",0,status, bindingId);
}

/*
 * Callback function from the gpRf4ceBindValidation. MSO unbind confirm
 */
void gpRf4ceBindValidation_cbUnbindConfirm( UInt8 bindingId, gpRf4ce_Result_t status )
{
    gpController_Mso_Msg_t msg;
    msg.UnbindConfirmParams.bindingId = bindingId;
    msg.UnbindConfirmParams.status = status;
    gpController_Mso_cbMsg(gpController_Mso_MsgId_cbUnbindConfirm, &msg);
    GP_LOG_PRINTF("gpRf4ceBindValidation_cbUnbindConfirm, Status: 0x%x. BindingId: 0x%x",0,status, bindingId);
}

/*
 * Callback function from the gpRf4ceBindValidation. MSO unbind indication
 */
void gpRf4ceBindValidation_cbUnbindIndication( UInt8 bindingId )
{
    gpController_Mso_Msg_t msg;
    msg.bindingId = bindingId;
    gpController_Mso_cbMsg(gpController_Mso_MsgId_cbUnbindIndication, &msg);
    GP_LOG_PRINTF("gpRf4ceBindValidation_cbUnbindIndication, BindingId: 0x%x",0, bindingId);
}

void gpRf4ceBindValidation_cbRxEnableConfirm( gpRf4ce_Result_t status )
{
}

/*******************************************************************************
 *                      MSO RIB Callbacks
 ******************************************************************************/
static void gpController_MSO_cbRIBSetConfirm(gpRf4ce_Result_t status , UInt8 bindingRef , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex)
{
    gpController_Mso_Msg_t msg;
    msg.RIBSetConfirmParams.bindingId = bindingRef;
    msg.RIBSetConfirmParams.status = status;
    msg.RIBSetConfirmParams.attributeIndex = ribAttributeIndex;
        
	gpController_BatterMonitor_BatteryLevelLoadedUpdated = false;
    if(ribAttribute == gpRf4ceRib_RIBAttributeIdBatteryStatus)
    {
        gpController_Mso_cbMsg(gpController_Mso_MsgId_cbSendBatteryStatusConfirm, &msg);
    }
    else if(ribAttribute == gpRf4ceRib_RIBAttributeIdVersioning)
    {
        gpController_Mso_cbMsg(gpController_Mso_MsgId_cbSendVersionInfoConfirm, &msg);
    }
}
