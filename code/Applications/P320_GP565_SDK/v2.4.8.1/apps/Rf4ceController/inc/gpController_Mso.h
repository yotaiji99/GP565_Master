/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Mso.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_Mso.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

#ifndef _GPCONTROLLER_MSO_H_
#define _GPCONTROLLER_MSO_H_

///@file gpController_Mso.h
///


/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"
#include "gpRf4ce.h"
#include "gpController.h"
#include "gpRf4ceBindValidation.h"
#include "gpRf4ceRib.h"
#include "gpVersion.h"

/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define TX_OPTIONS_MSO_BASE             0x8044      /* MSB:-    // User Control tx options <15-8>
                                                                    Repeat                      = 1 (Repeat Enable)

                                                                    // Rf4ce NSDU tx options <7-0>
                                                                    Vendor specific             = 1 (Enable)
                                                                    Channel designator          = 0 (Not specified)
                                                                    Channel                     = 0 (Multiple)
                                                                    Security                    = 0 (Disabled)
                                                                    Acknowledgement             = 1 (Enabled)
                                                                    Destination Address         = 0 (16 bit Nwk Address)
                                                                    Tx Mode                     = 0 (Unicast)
                                                            */

/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/

/** @name gpController_Mso_MsgId_t */
//@{
/** @brief Request reset of MSO profile layer. */
#define gpController_Mso_MsgId_ResetRequest                 0x01
/** @brief Confirm to the controller the reset has completed. */
#define gpController_Mso_MsgId_cbResetConfirm               0x02
/** @brief Request to the MSO profile layer to start binding. */
#define gpController_Mso_MsgId_BindRequest                  0x03
/** @brief Confirm to the controller the bind has completed. */
#define gpController_Mso_MsgId_cbBindConfirm                0x04
/** @brief Request to the MSO profile layer to start unbinding a specific ID. */
#define gpController_Mso_MsgId_UnbindRequest                0x05
/** @brief Confirm to the controller the unbind has completed. */
#define gpController_Mso_MsgId_cbUnbindConfirm              0x06

/** @brief Indicate to the controller an unbind has occured (triggered by target). */
#define gpController_Mso_MsgId_cbUnbindIndication           0x07
/** @brief Indicate to the controller a validation has started. */
#define gpController_Mso_MsgId_cbStartValidationIndication  0x08
/** @brief Indicate to the controller a validation has failed. */
#define gpController_Mso_MsgId_cbFailValidationIndication   0x09
/** @brief Indicate to the MSO module binding needs to be aborted for this target */
#define gpController_Mso_MsgId_BindAbort                    0x0F
/** @brief Indicate to the MSO module binding needs to be aborted for all targets */
#define gpController_Mso_MsgId_BindAbortFull                0x10
/** @brief Request to the MSO profile layer to send battery status information to a specific target. */
#define gpController_Mso_MsgId_SendBatteryStatusRequest     0x11
/** @brief Confirm to the controller the RIB set request of battery status has completed. */
#define gpController_Mso_MsgId_cbSendBatteryStatusConfirm   0x12
/** @brief Request to the MSO profile layer to send version information to a specific target. */
#define gpController_Mso_MsgId_SendVersionInfoRequest       0x13
/** @brief Confirm to the controller the RIB set request of version info has completed. */
#define gpController_Mso_MsgId_cbSendVersionInfoConfirm     0x14


typedef UInt8 gpController_Mso_MsgId_t;

/** @typedef gpController_Mso_BindConfirm_t
 *  The gpController_Mso_BindConfirm_t type defines paramaters that need to be
 * passed to the controller when a bind is confirmed.
*/
typedef struct gpController_Mso_BindConfirm
{
    /** The binding ID of the confirmed bind. */
    UInt8 bindingId;
    /** The profile ID of the confirmed bind. */
    UInt8 profileId;
    /** The status of the confirmed bind. */
    gpRf4ce_Result_t status;
}gpController_Mso_BindConfirm_t;

typedef struct gpController_Mso_RIBSetBatteryStatusRequest
{
    /** The binding ID of the link to send the battery info to. */
    UInt8 bindingId;
    /** The vendor ID to use in the RIB request. */
    UInt16 vendorId;
    /** The flags used in the RIB Battery Status request message */
    UInt8 flags;
    /** The loaded battery level */
    UInt8 levelLoaded;
    /** The unloaded battery level */
    UInt8 levelUnloaded;
    /** Amount of RF codes sent */
    UInt32 NumberOfRFSent;
    /** Amount of IR codes sent */
    UInt32 NumberOfIRSent;
}gpController_Mso_RIBSetBatteryStatusRequest_t;

typedef struct gpController_Mso_RIBSetVersioningRequest
{
    /** The binding ID of the link to send the battery info to. */
    UInt8 bindingId;
    /** The vendor ID to use in the RIB request. */
    UInt16 vendorId;
    /** The index describing what is being versioned. 0 is SW version, 1 is HW version, 2 is IRDB version.*/
    UInt8 index;
    /** The versionInfo (4 bytes) */
    gpVersion_ReleaseInfo_t version;
}gpController_Mso_RIBSetVersioningRequest_t;

/** @typedef gpController_Mso_RIBSetConfirm_t
 *  The gpController_Mso_RIBSetConfirm_t type defines parameters that need to be
 * passed to the controller when an RIB set action is confirmed.
*/
typedef struct gpController_Mso_RIBSetConfirm
{
    UInt8 bindingId;
    gpRf4ce_Result_t status;
    UInt8 attributeIndex;
}gpController_Mso_RIBSetConfirm_t;

/** @typedef gpController_Mso_UnbindConfirm_t
 *  The gpController_Mso_UnbindConfirm_t type defines parameters that need to be
 * passed to the controller when an unbind is confirmed.
*/
typedef struct gpController_Mso_UnbindConfirm
{
    UInt8 bindingId;
    gpRf4ce_Result_t status;
}gpController_Mso_UnbindConfirm_t;

/** @typedef gpController_Mso_ValidationIndication_t
 *  The gpController_Mso_ValidationIndication_t type defines parameters that
 *  need to be passed to the controller when validation is indicated.
*/
typedef struct gpController_Mso_ValidationIndication
{
    /** The binding ID of the validation. */
    UInt8 bindingId;
    /** The profile ID of the validation. */
    UInt8 profileId;
    /** The status of the indicated validation. */
    gpRf4ce_ResultValidation_t status;

}gpController_Mso_ValidationIndication_t;

/** @typedef gpController_Mso_Msg_t
 *  The gpController_Mso_Msg_t type specifies MSO specific messages to
 *  exchange with the MAIN module.
*/
typedef union {
    gpController_Keys_t keys;
    UInt8 bindingId;
    gpController_Mso_ValidationIndication_t ValidationParams;
    gpController_Mso_BindConfirm_t BindConfirmParams;
    gpController_Mso_UnbindConfirm_t UnbindConfirmParams;
    gpController_Mso_RIBSetBatteryStatusRequest_t BatterStatusUpdate;
    gpController_Mso_RIBSetVersioningRequest_t VersionInfo;
    gpController_Mso_RIBSetConfirm_t RIBSetConfirmParams;
} gpController_Mso_Msg_t;

typedef void (*gpController_Mso_cbMsg_t)(gpController_Mso_MsgId_t msgId, gpController_Mso_Msg_t *pMsg);


/*******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup MSO
 *
 *  This primitive can be used to initialize the MSO module.
  */
GP_API void gpController_Mso_Init(void);

/** @ingroup MSO
 *
 *  This primitive can be used to do a request to the MSO module.
 *
 *  Completion of the request will be indicated by triggering a confirm using
 *  the gpController_MSO_cbMsg callback.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Mso_Msg(gpController_Mso_MsgId_t msgId, gpController_Mso_Msg_t *pMsg);

/** @ingroup MSO
 *
 *  This primitive indicates (to the next higher layer) that an event occured.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Mso_cbMsg(gpController_Mso_MsgId_t msgId, gpController_Mso_Msg_t *pMsg);

#endif /* _GPCONTROLLER_MSO_H_ */
