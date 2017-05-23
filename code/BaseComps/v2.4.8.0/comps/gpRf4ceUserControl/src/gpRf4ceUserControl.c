/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *   This file contains the implementation of target specific functions of the gpRf4ce User-Control functional block.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceUserControl/src/gpRf4ceUserControl.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG

#include "global.h"

#include "gpRf4ceUserControl.h"
#include "gpRf4ceUserControl_defs.h"

#include "gpSched.h"
#include "gpAssert.h"
#include "gpRf4ce.h"
#include "gpRf4ceDispatcher.h"
#include "gpNvm.h"

#include "gpPd.h" //Packet descriptor support



/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RF4CEUSERCONTROL

#if !defined(GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS) && !defined(GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS)
#error Must handle either indications or requests - both GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS and GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS set to 0 ?
#endif 

#ifndef GP_RF4CE_USER_CONTROL_IND_DECREMENT_STEPS
#define GP_RF4CE_USER_CONTROL_IND_DECREMENT_STEPS   4
#endif

#ifndef GP_RF4CE_USER_REPEAT_INTERVAL_LOOP_OVERHEAD
#define GP_RF4CE_USER_REPEAT_INTERVAL_LOOP_OVERHEAD 5
#endif

//IB access macro's
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#define KEY_REPEAT_WAIT_TIME           MS_TO_US(gpRf4ceUserControl_IB.keyRepeatWaitTime)
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
#define KEY_REPEAT_INTERVAL            MS_TO_US(gpRf4ceUserControl_IB.keyRepeatInterval-GP_RF4CE_USER_REPEAT_INTERVAL_LOOP_OVERHEAD)
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

//pending key access macro's
#define INVALID_KEYID                   -1
#define IS_KEYID_VALID(keyId)           (keyId > INVALID_KEYID)

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
#define REQ_KEY                         gpRf4ceUserControl_Req_Keys
#define REQ_IS_KEY_VALID(keyId)         (REQ_KEY[keyId].AppKeyStatus != gpRf4ceUserControl_KeyStatusIdle)
#define REQ_INVALIDATE_KEY(keyId)       do          \
                                        {           \
                                            REQ_KEY[keyId].AppKeyStatus = gpRf4ceUserControl_KeyStatusIdle;    \
                                            REQ_KEY[keyId].RFKeyStatus = Rf4ceUserControl_RFStatusIdle;     \
                                        }while(false)
                                        
#define REQ_ACTIVE_KEY                  REQ_KEY[gpRf4ceUserControl_Req_ActiveKeyId]
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#define IND_KEY                         gpRf4ceUserControl_Ind_Keys
#define IND_IS_KEY_VALID(keyId)         (IND_KEY[keyId].counter >= 0)
#define IND_INVALIDATE_KEY(keyId)       (IND_KEY[keyId].counter = -1)
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

#define NVM_TAG_USERCONTROL_IB 0

#define USER_CONTROL_GET_VENDOR_ID()      gpRf4ce_GetVendorId()

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
/// @name gpRf4ceUserControl_RFStatus_t
//@{
///  @brief The usercontrol block RF status is idle (no pending transmissions).
#define Rf4ceUserControl_RFStatusIdle                0x00
///  @brief The usercontrol block will send the ZRC Pressed command for the pending key.
#define Rf4ceUserControl_RFStatusPressed             0x01
/// @brief The usercontrol block will send the ZRC Repeated command for the pending key.
#define Rf4ceUserControl_RFStatusRepeated            0x02
/// @brief The usercontrol block will send the ZRC Release command for the pending key.
#define Rf4ceUserControl_RFStatusReleased            0x03
/// @brief The previous RF transmission is handled and the key can be removed from the pending key list.
#define Rf4ceUserControl_RFStatusHandled             0x04
/// @typedef gpRf4ceUserControl_RFStatus_t
/// @brief The gpRf4ceUserControl_RFStatus_t type defines the RF status of a pending key.
typedef UInt8 Rf4ceUserControl_RFStatus_t;
/// @}

typedef struct {
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
    UInt16 keyRepeatWaitTime;
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
    UInt16 keyRepeatInterval;
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
} gpRf4ceUserControl_IB_t;

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
typedef struct {
    UInt8                               bindingId;
    gpRf4ce_ProfileId_t                 profileId;
    gpRf4ceUserControl_KeyStatus_t      AppKeyStatus;
    Rf4ceUserControl_RFStatus_t         RFKeyStatus;
    gpRf4ceUserControl_CommandCode_t    cmdCode;
    UInt16                              txOptions;
    UInt32                              rfRetriesPeriod;
} gpRf4ceUserControl_Req_Key_t;
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
typedef struct {
    Int8                                  counter;
    gpRf4ce_ProfileId_t                   profileId;
    gpRf4ceUserControl_CommandCode_t      cmdCode;
    UInt8                                 bindingId;
} gpRf4ceUserControl_Ind_Key_t;
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static gpRf4ce_VendorId_t Rf4ceUserControl_GetNodeVendorId(void);
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
static UInt8 Rf4ceUserControl_GetNodeAppCapabilities(void);
static Bool Rf4ceUserControl_IsValidProfileId(gpRf4ce_ProfileId_t profileId);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
static Bool Rf4ceUserControl_Req_HasPendingKey(void);
static Int8 Rf4ceUserControl_Req_GetNewKeyId(void);
static Int8 Rf4ceUserControl_Req_GetNextKeyId(Int8 prev);
static void Rf4ceUserControl_Req_SendActiveKey(void);
static void Rf4ceUserControl_Req_UpdateRFStateForActiveKey(void);
static Bool Rf4ceUserControl_Req_HasPendingKeyReleases(void);

static Bool Rf4ceUserControl_Req_PressKey(UInt8                                 bindingId,
                                          gpRf4ce_ProfileId_t                   profileId,
                                          gpRf4ceUserControl_CommandCode_t      cmdCode,
                                          UInt16                                txOptions,
                                          UInt32                                rfRetriesPeriod);
static Bool Rf4ceUserControl_Req_ReleaseKey(UInt8                               bindingId,
                                            gpRf4ce_ProfileId_t                 profileId,
                                            gpRf4ceUserControl_CommandCode_t    cmdCode);
static void Rf4ceUserControl_Req_EarlyRepeatIndication(void);
static void Rf4ceUserControl_Req_UpdatePendingKeys(void);
static void Rf4ceUserControl_Req_Start_SendEngine(void);
static void Rf4ceUserControl_Req_SendEngine(void);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
static Bool Rf4ceUserControl_Ind_HasPendingKey(void);
static Int8 Rf4ceUserControl_Ind_GetNewKeyId(void);
static Bool Rf4ceUserControl_Ind_KeyExists(gpRf4ce_ProfileId_t                   profileId,
                                           gpRf4ceUserControl_CommandCode_t      cmdCode,
                                           UInt8                                 bindingId);
static Bool Rf4ceUserControl_Ind_AddKey(gpRf4ce_ProfileId_t                   profileId,
                                        gpRf4ceUserControl_CommandCode_t      cmdCode,
                                        UInt8                                 bindingId);
static Bool Rf4ceUserControl_Ind_RemoveKey(gpRf4ce_ProfileId_t                   profileId,
                                           gpRf4ceUserControl_CommandCode_t      cmdCode,
                                           UInt8                                 bindingId);
static void Rf4ceUserControl_Ind_TimeoutEngine(void);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

GP_RF4CE_DISPATCHER_DATA_DECLARATION(gpRf4ceUserControl);

//Data callbacks
static void gpRf4ceDispatcher_cbResetIndication(Bool setDefault);
static Bool gpRf4ceDispatcher_cbDEDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);
static void gpRf4ceDispatcher_cbDEDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpPd_Handle_t pdHandle);

GP_RF4CE_DISPATCHER_CONST gpRf4ceDispatcher_DataCallbacks_t ROM gpRf4ceUserControl_DataCallbacks FLASH_PROGMEM = {
        gpRf4ceDispatcher_cbResetIndication,
        gpRf4ceDispatcher_cbDEDataIndication,
        gpRf4ceDispatcher_cbDEDataConfirm,
        NULL,
        NULL
        };



STATIC gpRf4ceUserControl_IB_t   gpRf4ceUserControl_IB;
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
STATIC Bool                         gpRf4ceUserControl_SendEngineBusy;
STATIC Int8                         gpRf4ceUserControl_Req_ActiveKeyId;
STATIC gpRf4ceUserControl_Req_Key_t gpRf4ceUserControl_Req_Keys[GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS];
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
STATIC gpRf4ceUserControl_Ind_Key_t gpRf4ceUserControl_Ind_Keys[GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS];
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

const gpNvm_Tag_t ROM gpRf4ceUserControl_NvmSection[] FLASH_PROGMEM = {
    {(UInt8*)&(gpRf4ceUserControl_IB), sizeof(gpRf4ceUserControl_IB), gpNvm_UpdateFrequencyInitOnly, NULL}
};

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

//COMMON helper functions

gpRf4ce_VendorId_t Rf4ceUserControl_GetNodeVendorId(void)
{
    gpRf4ce_VendorId_t nodeVendorId = USER_CONTROL_GET_VENDOR_ID();
    return nodeVendorId;
}

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
UInt8 Rf4ceUserControl_GetNodeAppCapabilities(void)
{
    UInt8 nodeAppCap;
    MEMCPY_P(&nodeAppCap,&gpRf4ceUserControl_NodeAppCapabilities,sizeof(UInt8));
    return nodeAppCap;
}

Bool Rf4ceUserControl_IsValidProfileId(gpRf4ce_ProfileId_t profileId)
{
    UIntLoop j;

    UInt8               nodeAppCap = Rf4ceUserControl_GetNodeAppCapabilities();
    gpRf4ce_ProfileId_t pNodeProfileIdList[7];
    MEMCPY_P(pNodeProfileIdList, gpRf4ceUserControl_pNodeProfileIdList, GP_RF4CE_APP_CAPABILITY_GET_NBR_PROFILES(nodeAppCap));

    for(j=0;j<GP_RF4CE_APP_CAPABILITY_GET_NBR_PROFILES(nodeAppCap);j++)
    {
        if(profileId == pNodeProfileIdList[j])
        {
            return true;
        }
    }
    return false;
}
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
//REQUEST helper functions

Bool Rf4ceUserControl_Req_HasPendingKey(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {
        if(REQ_IS_KEY_VALID(i)) return true;
    }
    return false;
}

Int8 Rf4ceUserControl_Req_GetNewKeyId(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {
        if(!REQ_IS_KEY_VALID(i)) return i;
    }
    return INVALID_KEYID;
}

Int8 Rf4ceUserControl_Req_GetNextKeyId(Int8 prev)
{
    UIntLoop i;
     for (i = (prev+1) ;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {
        if(REQ_IS_KEY_VALID(i)) return i;
    }
    return INVALID_KEYID;
}

void Rf4ceUserControl_Req_SendActiveKey(void)
{
    GP_ASSERT_DEV_INT(IS_KEYID_VALID(gpRf4ceUserControl_Req_ActiveKeyId));
    GP_ASSERT_DEV_INT(REQ_ACTIVE_KEY.bindingId != 0xFF);
    GP_ASSERT_DEV_INT(REQ_ACTIVE_KEY.RFKeyStatus != Rf4ceUserControl_RFStatusIdle);
    GP_ASSERT_DEV_INT(REQ_ACTIVE_KEY.RFKeyStatus != Rf4ceUserControl_RFStatusHandled);

    {
        UInt8 payload[2];
        UInt8 pdHandle  = gpPd_GetPd();

        payload[0] = REQ_ACTIVE_KEY.RFKeyStatus;
        payload[1] = REQ_ACTIVE_KEY.cmdCode;
        
        if(gpPd_CheckPdValid(pdHandle) != gpPd_ResultValidHandle)
        {
            if(REQ_ACTIVE_KEY.RFKeyStatus == Rf4ceUserControl_RFStatusPressed ||
                REQ_ACTIVE_KEY.RFKeyStatus == Rf4ceUserControl_RFStatusReleased)
            {
                gpRf4ceUserControl_cbUserControlConfirm(REQ_ACTIVE_KEY.bindingId,
                                                        gpRf4ce_ResultTransactionOverflow,
                                                        REQ_ACTIVE_KEY.RFKeyStatus,
                                                        REQ_ACTIVE_KEY.cmdCode,
                                                        REQ_ACTIVE_KEY.txOptions);
            }
            else if(REQ_ACTIVE_KEY.RFKeyStatus == Rf4ceUserControl_RFStatusRepeated )
            {
                gpRf4ceUserControl_cbUserControlRepeatFailureIndication(REQ_ACTIVE_KEY.bindingId,
                                                                        gpRf4ce_ResultTransactionOverflow,
                                                                        REQ_ACTIVE_KEY.RFKeyStatus,
                                                                        REQ_ACTIVE_KEY.cmdCode,
                                                                        REQ_ACTIVE_KEY.txOptions);
            }
            //Remove key - Key will be removed by SendEngine
            REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusHandled;
            //continue with other pending keys
            gpSched_ScheduleEvent(0,Rf4ceUserControl_Req_SendEngine);
            return;
        }

        gpPd_WriteByteStream(pdHandle, 0, sizeof(payload), payload);

        gpRf4ce_DataRequest(
         /*pairingRef*/      REQ_ACTIVE_KEY.bindingId,
         /*profileId*/       REQ_ACTIVE_KEY.profileId,
            
         /*vendorId*/        Rf4ceUserControl_GetNodeVendorId(),
         /*txOptions*/       GP_RF4CE_USER_CONTROL_TXOPTION_GET_RF4CE_TXOPTION(REQ_ACTIVE_KEY.txOptions),
         /*rfRetriesPeriod*/ REQ_ACTIVE_KEY.rfRetriesPeriod,
         /*nsduLength*/      2,
         /*nsduOffset*/      0, 
         /*pdHandle*/        pdHandle);
    }
}

Bool Rf4ceUserControl_Req_PressKey(UInt8                                bindingId,
                                   gpRf4ce_ProfileId_t                  profileId,
                                   gpRf4ceUserControl_CommandCode_t     cmdCode,
                                   UInt16                               txOptions,
                                   UInt32                               rfRetriesPeriod)
{
    UIntLoop i;
    Int8 keyId;
    //Filter double key press
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {   if(!REQ_IS_KEY_VALID(i)) continue;

        if (REQ_KEY[i].cmdCode == cmdCode && 
            REQ_KEY[i].bindingId == bindingId &&
            REQ_KEY[i].profileId == profileId)
        {
            return false;
        }
    }

    keyId = Rf4ceUserControl_Req_GetNewKeyId();

    if (!IS_KEYID_VALID(keyId))
    {
        return false;
    }

    REQ_KEY[keyId].bindingId = bindingId;
    REQ_KEY[keyId].profileId = profileId;
    REQ_KEY[keyId].AppKeyStatus = gpRf4ceUserControl_KeyStatusPressed;
    REQ_KEY[keyId].RFKeyStatus = Rf4ceUserControl_RFStatusIdle;
    REQ_KEY[keyId].cmdCode = cmdCode;
    REQ_KEY[keyId].txOptions = txOptions;
    REQ_KEY[keyId].rfRetriesPeriod = rfRetriesPeriod;

    return true;
}

Bool Rf4ceUserControl_Req_ReleaseKey(UInt8                                 bindingId,
                                     gpRf4ce_ProfileId_t                   profileId,
                                     gpRf4ceUserControl_CommandCode_t      cmdCode)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {   
        if(!REQ_IS_KEY_VALID(i)) continue;

        if (REQ_KEY[i].cmdCode == cmdCode && 
            REQ_KEY[i].bindingId == bindingId &&
            REQ_KEY[i].profileId == profileId)
        {
            REQ_KEY[i].AppKeyStatus = gpRf4ceUserControl_KeyStatusReleased;
            
            if(gpSched_UnscheduleEvent(Rf4ceUserControl_Req_Start_SendEngine))
            {
                gpSched_ScheduleEvent(0, Rf4ceUserControl_Req_Start_SendEngine);
            }
            if(REQ_KEY[i].RFKeyStatus == Rf4ceUserControl_RFStatusHandled) 
            {
                //RFStatus == handled we need to trigger a confirm
                return false;
            }
            else
            {
                return true;
            }
        }
    }
    return false;
}

void Rf4ceUserControl_Req_EarlyRepeatIndication(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {   
        if(!REQ_IS_KEY_VALID(i)) continue;

        if(REQ_KEY[i].AppKeyStatus == gpRf4ceUserControl_KeyStatusPressed)
        {
            if((REQ_KEY[i].RFKeyStatus == Rf4ceUserControl_RFStatusRepeated) ||
                (REQ_KEY[i].RFKeyStatus == Rf4ceUserControl_RFStatusPressed))
            {   
                gpRf4ceUserControl_cbEarlyRepeatIndication();
                return;             
            }
        }
    }
}

void Rf4ceUserControl_Req_UpdatePendingKeys(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {   
        if(!REQ_IS_KEY_VALID(i)) continue;

        if(REQ_KEY[i].RFKeyStatus == Rf4ceUserControl_RFStatusHandled)
        {
            REQ_INVALIDATE_KEY(i);
        }
    }
}

void Rf4ceUserControl_Req_Start_SendEngine(void)
{
    //don't start if there are no pending keys
    if(!Rf4ceUserControl_Req_HasPendingKey())
    {
        return;
    }
    //Check if the sendEngine is not running
    if(!IS_KEYID_VALID(gpRf4ceUserControl_Req_ActiveKeyId))
    { //first call of a new pending key iteration
        if(!gpRf4ceDispatcher_LockRequest(GP_COMPONENT_ID) )
        {
            //could not lock dispatcher, reschedule pending keys engine
            gpSched_ScheduleEvent(KEY_REPEAT_INTERVAL,Rf4ceUserControl_Req_Start_SendEngine);
            //GP_LOG_SYSTEM_PRINTF("S SendEngine in SendEngine 1",0);
            return;
        }
        gpRf4ceUserControl_SendEngineBusy = true;
        gpSched_ScheduleEvent(0, Rf4ceUserControl_Req_SendEngine);
    }
}

void Rf4ceUserControl_Req_SendEngine(void)
{
    if(!IS_KEYID_VALID(gpRf4ceUserControl_Req_ActiveKeyId))
    {
        //first call of a new pending key iteration, Lock and wakeup already done.
        gpRf4ceUserControl_Req_ActiveKeyId = Rf4ceUserControl_Req_GetNextKeyId(INVALID_KEYID);
        GP_ASSERT_DEV_INT(IS_KEYID_VALID(gpRf4ceUserControl_Req_ActiveKeyId)); //Recheck validity of key - shouldn't be in this function if no keys are valid
        Rf4ceUserControl_Req_EarlyRepeatIndication();
    }
    else
    { 
        //continue iteration with next pending key
        gpRf4ceUserControl_Req_ActiveKeyId = Rf4ceUserControl_Req_GetNextKeyId(gpRf4ceUserControl_Req_ActiveKeyId);
    }

    if(!IS_KEYID_VALID(gpRf4ceUserControl_Req_ActiveKeyId))
    {
        //no more pending keys in current iteration
        gpRf4ceDispatcher_ReleaseRequest(GP_COMPONENT_ID);
        gpRf4ceUserControl_SendEngineBusy = false;
        Rf4ceUserControl_Req_UpdatePendingKeys();
        if(Rf4ceUserControl_Req_HasPendingKey())
        { //reschedule if still pending keys pressent
            //GP_LOG_SYSTEM_PRINTF("S SendEngine in SendEngine 2",0);
            if(Rf4ceUserControl_Req_HasPendingKeyReleases())
            {
                gpSched_ScheduleEvent(0,Rf4ceUserControl_Req_Start_SendEngine);
            }
            else
            {
                gpSched_ScheduleEvent(KEY_REPEAT_INTERVAL,Rf4ceUserControl_Req_Start_SendEngine);
            }
        }
    }
    else
    { 
        //Update RF State for next pending key in current iteration
        Rf4ceUserControl_Req_UpdateRFStateForActiveKey();
        //Transmit next pending key in current iteration
        Rf4ceUserControl_Req_SendActiveKey();
    }
}


void Rf4ceUserControl_Req_UpdateRFStateForActiveKey(void)
{
/*
    if RFIdle
        if AppstatusPressed --> RFPressed
        if AppstatusReleased --> RFPressed
    if RFPressed
        if AppstatusPressed --> RFRepeated
        if AppstatusReleased --> RFReleased
    if RF Repeated
        if AppstatusPressed --> RFRepeated
        if AppstatusReleased --> RFReleased
*/
    switch (REQ_ACTIVE_KEY.RFKeyStatus)
    {
        case Rf4ceUserControl_RFStatusIdle:
        {
            GP_ASSERT_DEV_INT(REQ_ACTIVE_KEY.AppKeyStatus != gpRf4ceUserControl_KeyStatusIdle);
            REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusPressed;
            break;
        }
        case Rf4ceUserControl_RFStatusPressed:
        case Rf4ceUserControl_RFStatusRepeated:
        {
            if(REQ_ACTIVE_KEY.AppKeyStatus == gpRf4ceUserControl_KeyStatusPressed)
            {
                REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusRepeated;
            }
            else if(REQ_ACTIVE_KEY.AppKeyStatus == gpRf4ceUserControl_KeyStatusReleased)
            {
                REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusReleased;                
            }
            break;
        }
        default:
        {
            GP_ASSERT_DEV_EXT(false); //Source of RFKeyStatus ?
            break;
        }
    }
}

Bool Rf4ceUserControl_Req_HasPendingKeyReleases(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {
        if(REQ_KEY[i].AppKeyStatus == gpRf4ceUserControl_KeyStatusReleased)
        {
            return true;
        }
    }
    return false;
}
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
//INDICATION helper functions
Bool Rf4ceUserControl_Ind_HasPendingKey(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {
        if(IND_IS_KEY_VALID(i)) return true;
    }
    return false;
}

Int8 Rf4ceUserControl_Ind_GetNewKeyId(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {
        if(!IND_IS_KEY_VALID(i)) return i;
    }
    return INVALID_KEYID;
}

Bool Rf4ceUserControl_Ind_KeyExists(gpRf4ce_ProfileId_t                   profileId,
                                    gpRf4ceUserControl_CommandCode_t      cmdCode,
                                    UInt8                                 bindingId)
{
    UIntLoop i;

    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {   
        if(!IND_IS_KEY_VALID(i)) continue;

        if( (IND_KEY[i].cmdCode == cmdCode) &&
            (IND_KEY[i].profileId == profileId) &&
            (IND_KEY[i].bindingId == bindingId) )
        {
            return true;
        }
    }
    return false;
}

Bool Rf4ceUserControl_Ind_AddKey(gpRf4ce_ProfileId_t                   profileId,
                                 gpRf4ceUserControl_CommandCode_t      cmdCode,
                                 UInt8                                 bindingId)
{
    UIntLoop i;
    Int8 keyId;

    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {   
        if(!IND_IS_KEY_VALID(i)) continue;

        if( (IND_KEY[i].cmdCode == cmdCode) &&
            (IND_KEY[i].profileId == profileId) &&
            (IND_KEY[i].bindingId == bindingId) )
        {
            IND_KEY[i].counter = GP_RF4CE_USER_CONTROL_IND_DECREMENT_STEPS;
            return true;
        }
    }

    keyId = Rf4ceUserControl_Ind_GetNewKeyId();
    if (!IS_KEYID_VALID(keyId))
    {
        return false;
    }

    IND_KEY[keyId].profileId = profileId;
    IND_KEY[keyId].cmdCode = cmdCode;
    IND_KEY[keyId].bindingId = bindingId;
    IND_KEY[keyId].counter = GP_RF4CE_USER_CONTROL_IND_DECREMENT_STEPS;

    if(!gpSched_ExistsEvent(Rf4ceUserControl_Ind_TimeoutEngine))
    {
        gpSched_ScheduleEvent((KEY_REPEAT_WAIT_TIME/GP_RF4CE_USER_CONTROL_IND_DECREMENT_STEPS),
                              Rf4ceUserControl_Ind_TimeoutEngine);
    }

    return true;
}

Bool Rf4ceUserControl_Ind_RemoveKey(gpRf4ce_ProfileId_t                   profileId,
                                    gpRf4ceUserControl_CommandCode_t      cmdCode,
                                    UInt8                                 bindingId)
{
    UIntLoop i;
    Bool result = false;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {   
        if(!IND_IS_KEY_VALID(i)) continue;

        if( (IND_KEY[i].cmdCode == cmdCode) &&
            (IND_KEY[i].profileId == profileId) &&
            (IND_KEY[i].bindingId == bindingId) )
        {
            IND_INVALIDATE_KEY(i);
            result = true;
        }
    }
    if(!Rf4ceUserControl_Ind_HasPendingKey())
    {
        gpSched_UnscheduleEvent(Rf4ceUserControl_Ind_TimeoutEngine);
    }
    return result;
}

void Rf4ceUserControl_Ind_TimeoutEngine(void)
{
    UIntLoop i;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {   
        if(!IND_IS_KEY_VALID(i)) continue;

        //GP_LOG_PRINTF("engine %i %i",4,(UInt16)IND_KEY[i].cmdCode,(UInt16)IND_KEY[i].counter);
        if(IND_KEY[i].counter>0)
        {
            IND_KEY[i].counter--;
        }
        if(IND_KEY[i].counter==0)
        {
            gpRf4ceUserControl_cbUserControlIndication(
                    /*bindingId*/       IND_KEY[i].bindingId,
                    /*profileId*/       IND_KEY[i].profileId,
                    /*vendorId*/        Rf4ceUserControl_GetNodeVendorId(),
                    /*keyStatus*/       gpRf4ceUserControl_KeyStatusReleased,
                    /*commandCode*/     IND_KEY[i].cmdCode,
                    /*signalStrength*/  0xFF,
                    /*linkQuality*/     0xFF,
                    /*rxFlags*/         0xFF
                );
            IND_INVALIDATE_KEY(i);
        }
    }

    if(Rf4ceUserControl_Ind_HasPendingKey())
    {
        gpSched_ScheduleEvent((KEY_REPEAT_WAIT_TIME/GP_RF4CE_USER_CONTROL_IND_DECREMENT_STEPS),
                              Rf4ceUserControl_Ind_TimeoutEngine);
    }
}
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

// ----------------------------- ATTRIBUTES -----------------------------

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
void gpRf4ceUserControl_SetKeyRepeatWaitTime(UInt16 value)
{
    if((value < GP_RF4CE_USER_REPEAT_INTERVAL_LOOP_OVERHEAD) || (value < GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL))
    {
        GP_ASSERT_DEV_EXT(false);
        return;
    }
    //GP_ASSERT_DEV_EXT(value <= 0xFFFF); //Range indication

    gpRf4ceUserControl_IB.keyRepeatWaitTime = value;
    gpNvm_Backup(GP_COMPONENT_ID,NVM_TAG_USERCONTROL_IB, NULL);
}

UInt16 gpRf4ceUserControl_GetKeyRepeatWaitTime(void)
{
    return gpRf4ceUserControl_IB.keyRepeatWaitTime;
}
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
void gpRf4ceUserControl_SetKeyRepeatInterval(UInt16 value)
{
    //GP_ASSERT_DEV_EXT(value >= 0); //Range indication
    if(value >= GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL)
    {
        GP_ASSERT_DEV_EXT(false);
        return;
    }

    gpRf4ceUserControl_IB.keyRepeatInterval = value;
    gpNvm_Backup(GP_COMPONENT_ID,NVM_TAG_USERCONTROL_IB, NULL);
}

UInt16 gpRf4ceUserControl_GetKeyRepeatInterval(void)
{
    return gpRf4ceUserControl_IB.keyRepeatInterval;
}
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

// ----------------------------- USER CONTROL ---------------------------
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
void gpRf4ceUserControl_UserControlRequest(UInt8                                bindingId,
                                           gpRf4ce_ProfileId_t                  profileID,
                                           gpRf4ce_VendorId_t                   vendorID,
                                           gpRf4ceUserControl_KeyStatus_t       keyStatus,
                                           gpRf4ceUserControl_CommandCode_t     cmdCode,
                                           UInt16                               txOptions,
                                           UInt32                               rfRetriesPeriod)
{
    gpRf4ce_Result_t result = 0xFF; //invalid result
    
    GP_ASSERT_DEV_EXT(vendorID == Rf4ceUserControl_GetNodeVendorId());

    if(keyStatus == gpRf4ceUserControl_KeyStatusPressed)
    {
        if(!gpRf4ce_IsPairingTableEntryValid(bindingId))
        {
            result = gpRf4ce_ResultNoPairing;
        }
        else if(!Rf4ceUserControl_Req_PressKey(bindingId,profileID,cmdCode,txOptions,rfRetriesPeriod))
        {
            result = gpRf4ce_ResultTransactionOverflow;
        }
    }
    else if(keyStatus == gpRf4ceUserControl_KeyStatusReleased)
    {
        if(!Rf4ceUserControl_Req_ReleaseKey(bindingId,profileID,cmdCode))
        {
            if(GP_RF4CE_USER_CONTROL_TXOPTION_IS_REPEAT(txOptions))
            {
                result = gpRf4ce_ResultInvalidParameter;
            }
            else
            {
                result = gpRf4ce_ResultSuccess;
            }
        }
    }
    else
    {
        result = gpRf4ce_ResultInvalidParameter;
    }

    if(result != 0xFF)
    {
        gpRf4ceUserControl_cbUserControlConfirm(bindingId, result, keyStatus, cmdCode, txOptions);
        return;
    }

    if(!gpRf4ceUserControl_SendEngineBusy && !gpSched_ExistsEvent(Rf4ceUserControl_Req_Start_SendEngine))
    {
        //GP_LOG_SYSTEM_PRINTF("S SendEngine in UserControlRequest",0);
        gpSched_ScheduleEvent(0,Rf4ceUserControl_Req_Start_SendEngine);
    }
}
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

void gpRf4ceUserControl_Init(void)
{
    gpNvm_RegisterSection(GP_COMPONENT_ID, gpRf4ceUserControl_NvmSection, sizeof(gpRf4ceUserControl_NvmSection)/sizeof(gpNvm_Tag_t),NULL);

}

// ----------------------------- DISPATCHER CALLBACKS ---------------------------

void gpRf4ceDispatcher_cbResetIndication(Bool setDefault)
{
    UIntLoop i;

    //Stop key engines
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
    while( gpSched_UnscheduleEvent(Rf4ceUserControl_Req_Start_SendEngine) ) { }
    while( gpSched_UnscheduleEvent(Rf4ceUserControl_Req_SendEngine) ) { }
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
    while( gpSched_UnscheduleEvent(Rf4ceUserControl_Ind_TimeoutEngine) ) { }
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

    gpNvm_Restore(GP_COMPONENT_ID, NVM_TAG_USERCONTROL_IB,NULL);
    if(setDefault)
    {
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
        gpRf4ceUserControl_SetKeyRepeatWaitTime(GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
        gpRf4ceUserControl_SetKeyRepeatInterval(GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL/2);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
    }

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS    
    for (i = 0;i < GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS;i++)
    {
        IND_INVALIDATE_KEY(i);
    }
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
    gpRf4ceUserControl_Req_ActiveKeyId = INVALID_KEYID;
    for (i = 0;i < GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS;i++)
    {
        REQ_INVALIDATE_KEY(i);
    }
    gpRf4ceUserControl_SendEngineBusy = false;
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
}

Bool gpRf4ceDispatcher_cbDEDataIndication(UInt8 pairingRef, gpRf4ce_ProfileId_t profileId, gpRf4ce_VendorId_t vendorId, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle)
{
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
    UInt8                            frameControl;
    gpRf4ceUserControl_CommandCode_t cmdCode;
    Bool                             indicate = false;

    if((nsduLength != 2) ||
       (!Rf4ceUserControl_IsValidProfileId(profileId)) )
    {
        return false;
    }

    if(GP_RF4CE_RXFLAGS_IS_VENDOR_SPECIFIC(rxFlags) && 
      (Rf4ceUserControl_GetNodeVendorId() != vendorId))
    {
        return false;
    }

    
    frameControl = gpPd_ReadByte(pdHandle, nsduOffset);
    cmdCode      = gpPd_ReadByte(pdHandle, nsduOffset+1);

    switch(frameControl)
    {
        case gpRf4ceUserControl_KeyStatusPressed:
        {
            if(Rf4ceUserControl_Ind_AddKey(profileId,cmdCode,pairingRef))
            {
                indicate = true;
            }
            break;
        }
        case gpRf4ceUserControl_KeyStatusRepeated:
        {
            if(!Rf4ceUserControl_Ind_KeyExists(profileId,cmdCode,pairingRef))
            {
                if(Rf4ceUserControl_Ind_AddKey(profileId,cmdCode,pairingRef))
                {
                    indicate = true;
                    //Override framecontrol - we received a repeat/missed the pressed
                    frameControl = gpRf4ceUserControl_KeyStatusPressed;
                }
            }
            else
            {
                Rf4ceUserControl_Ind_AddKey(profileId,cmdCode,pairingRef);
            }

            Rf4ceUserControl_Ind_AddKey(profileId,cmdCode,pairingRef);
            break;
        }
        case gpRf4ceUserControl_KeyStatusReleased:
        {
            if(Rf4ceUserControl_Ind_RemoveKey(profileId,cmdCode,pairingRef))
            {
                indicate = true;
            }
            break;
        }
        default:
        {
            return false;
        }
    }

    if(indicate)
    {
        gpRf4ceUserControl_cbUserControlIndication(
        /*bindingId*/       pairingRef,
        /*profileId*/       profileId,
        /*vendorId*/        vendorId,
        /*keyStatus*/       frameControl,
        /*commandCode*/     cmdCode,
        /*signalStrength*/  gpPd_GetRssi(pdHandle),
        /*linkQuality*/     gpPd_GetLqi(pdHandle),
        /*rxFlags*/         rxFlags
        );
    }

    gpPd_FreePd(pdHandle);
    return true;
#else
    //Packet not intended for our block
    return false;
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
}

void gpRf4ceDispatcher_cbDEDataConfirm(gpRf4ce_Result_t status, UInt8 pairingRef, gpPd_Handle_t pdHandle)
{
#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
    gpPd_FreePd(pdHandle);
    
    GP_ASSERT_DEV_EXT(IS_KEYID_VALID(gpRf4ceUserControl_Req_ActiveKeyId));
    GP_ASSERT_DEV_EXT(REQ_ACTIVE_KEY.bindingId == pairingRef);
    //continue with other pending keys
    //GP_LOG_SYSTEM_PRINTF("S SendEngine in DataConfirm",0);
    // do this sched function before indicating confirm so possible dispatcher release is done first.
    gpSched_ScheduleEvent(0,Rf4ceUserControl_Req_SendEngine);


    if(REQ_ACTIVE_KEY.RFKeyStatus == Rf4ceUserControl_RFStatusPressed)
    {
        gpRf4ceUserControl_cbUserControlConfirm(REQ_ACTIVE_KEY.bindingId,
                                                status,
                                                gpRf4ceUserControl_KeyStatusPressed,
                                                REQ_ACTIVE_KEY.cmdCode,
                                                REQ_ACTIVE_KEY.txOptions);
        if((!GP_RF4CE_USER_CONTROL_TXOPTION_IS_REPEAT(REQ_ACTIVE_KEY.txOptions)) || status != gpRf4ce_ResultSuccess)
        {
            if(REQ_ACTIVE_KEY.AppKeyStatus == gpRf4ceUserControl_KeyStatusReleased)
            {
                //application already released it so generate confirm
                //i.e. pressed with immediate release case
                REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusReleased;
            }
            else
            {        
                //Key is still pressed by app, so no need to send the confirm for the release yet
                REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusHandled;
            }
        }
    }
    if(REQ_ACTIVE_KEY.RFKeyStatus == Rf4ceUserControl_RFStatusReleased)
    {        
        gpRf4ceUserControl_cbUserControlConfirm(REQ_ACTIVE_KEY.bindingId,
                                                status,
                                                gpRf4ceUserControl_KeyStatusReleased,
                                                REQ_ACTIVE_KEY.cmdCode,
                                                REQ_ACTIVE_KEY.txOptions);
        REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusHandled;
    }

    if(REQ_ACTIVE_KEY.RFKeyStatus == Rf4ceUserControl_RFStatusRepeated  && status != gpRf4ce_ResultSuccess )
    {
        gpRf4ceUserControl_cbUserControlRepeatFailureIndication(REQ_ACTIVE_KEY.bindingId,
                                                                status,
                                                                gpRf4ceUserControl_KeyStatusReleased,
                                                                REQ_ACTIVE_KEY.cmdCode,
                                                                REQ_ACTIVE_KEY.txOptions);
        REQ_ACTIVE_KEY.RFKeyStatus = Rf4ceUserControl_RFStatusHandled;
    }
#else
    //Nothing to handle - shouldn't get the confirm - wrong use of API from external
    GP_ASSERT_DEV_EXT(false);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
}

