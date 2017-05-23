/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController_Zrc.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController_Zrc.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

#ifndef _GPCONTROLLER_ZRC_H_
#define _GPCONTROLLER_ZRC_H_

///@file gpController_ZRC.h
///

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"
#include "gpController.h"

#ifdef GP_DIVERSITY_APP_ZRC2_0
#else /*GP_DIVERSITY_APP_ZRC2_0*/
#include "gpRf4ce.h"
#include "gpRf4ceDispatcher.h"
#include "gpRf4ceUserControl.h"
#ifdef GP_DIVERSITY_APP_ZRC1_1
#include "gpRf4ceBindAuto.h"
#include "gpRf4ceKickOutLru.h"
#endif /*GP_DIVERSITY_APP_ZRC1_1*/
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
/*******************************************************************************
 *                      Defines
 ******************************************************************************/
#define TX_OPTIONS_ZRC_BASE             0x8004      /* MSB:-    // User Control tx options <15-8>
                                                                    Repeat                      = 1 (Repeats enabled)

                                                                    // Rf4ce NSDU tx options <7-0>
                                                                    Vendor specific             = 0 (Enable)
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

/** @name gpController_ZRC_MsgId_t */
//@{
/** @brief Request reset of ZRC profile layer. */
#define gpController_Zrc_MsgId_ResetRequest                 0x01
/** @brief Confirm to the controller the reset has completed. */
#define gpController_Zrc_MsgId_cbResetConfirm               0x02
/** @brief Request to the ZRC profile layer to start binding. */
#define gpController_Zrc_MsgId_BindRequest                  0x03
/** @brief Confirm to the controller the bind has completed. */
#define gpController_Zrc_MsgId_cbBindConfirm                0x04
/** @brief Request to the ZRC profile layer to start unbinding a specific ID. */
#define gpController_Zrc_MsgId_UnbindRequest                0x05
/** @brief Confirm to the controller the unbind has completed. */
#define gpController_Zrc_MsgId_cbUnbindConfirm              0x06

/** @brief Indicate to the controller an unbind has occured (triggered by target). */
#define gpController_Zrc_MsgId_cbUnbindIndication               0x07

#ifdef GP_DIVERSITY_APP_ZRC2_0
/** @brief Indicate to the controller a validation has started. */
#define gpController_Zrc_MsgId_cbStartValidationIndication      0x08
#endif /*GP_DIVERSITY_APP_ZRC2_0*/

/** @brief Indicate a key has been pressed and needs to be sent. */
#define gpController_Zrc_MsgId_KeyPressedIndication             0x09
/** @brief Indicate to the controller key was succesfully sent. */
#define gpController_Zrc_MsgId_cbKeyConfirmedIndication         0x0A
/** @brief Indicate to the controller key was not sent. */
#define gpController_Zrc_MsgId_cbKeyFailedIndication            0x0B
/** @brief Indicate to the ZRC module binding needs to be aborted for this target */
#define gpController_Zrc_MsgId_BindAbort                        0x0C
/** @brief Indicate to the ZRC module binding needs to be aborted for all targets */
#define gpController_Zrc_MsgId_BindAbortFull                    0x0D

#ifdef GP_DIVERSITY_APP_ZRC2_0
/** @brief Indicate to the ZRC module which action codes are supported */
#define gpController_Zrc_MsgId_ActionCodesSupportedIndication   0x0E
/** @brief Indicate to the ZRC module validation has failed */
#define gpController_Zrc_MsgId_cbFailValidationIndication       0x0F
/** @brief Request to the ZRC module to send the aplPowerStatus attribute to the recipient */
#define gpController_Zrc_MsgId_SendPowerStatusRequest           0x10
/** @brief Confirm from the ZRC module for the request to send the aplPowerStatus attribute to the recipient */
#define gpController_Zrc_MsgId_cbSendPowerStatusConfirm         0x11
/** @brief Request to push the ActionMappings list to the target. Parameters will contain a list of keys the RC wants to remap. 
           Expect a gpZrc_MsgId_cbOriginatorActionMappingPushConfirm message upon completion.*/
#define gpController_Zrc_MsgId_ActionMappingsPushRequest        0x12
/** @brief Request to pull remappable data for a key from the ActionMappings list from the target. Parameters will contain the index from the list of keys the RC wants to remap.
           Expect a gpZrc_MsgId_cbOriginatorActionMappingPullConfirm message upon completion.*/
#define gpController_Zrc_MsgId_ActionMappingsPullRequest        0x13
/** @brief Confirmation from the ZRC module following the request to push the list of remappable keys.
           When the ZRC module indicates a success, the application can start pulling remappable data.*/
#define gpController_Zrc_MsgId_cbActionMappingPushConfirm       0x14
/** @brief Confirmation from the ZRC module following the request to pull remappable data for 1 key from the list of remappable keys.*/
#define gpController_Zrc_MsgId_cbActionMappingPullConfirm       0x15
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
/** @brief Indicate to the controller early repeat. */
#define gpController_Zrc_MsgId_cbEarlyRepeatIndication          0x17

/** @brief Set/Get the binding ID attribute */
#define gpController_Zrc_MsgId_AttrBindingId                    0x81

/** @typedef gpController_Zrc_MsgId_t
 *  @brief The gpController_Zrc_MsgId_t type defines the ZRC message IDs.
*/
typedef UInt8 gpController_Zrc_MsgId_t;
//@}


/** @typedef gpController_Zrc_BindConfirm_t
 *  The gpController_Zrc_BindConfirm_t type defines that need to be passed to the
 *  controller when a bind is confirmed.
*/
typedef struct gpController_Zrc_BindConfirm
{
    /** The binding ID of the confirmed bind. */
    UInt8 bindingId;
    /** The profile ID of the confirmed bind. */
    UInt8 profileId;
    /** The status of the confirmed bind. */
    gpRf4ce_Result_t status;
}gpController_Zrc_BindConfirm_t;

/** @typedef gpController_Zrc_KeyPressed_t
 *  The gpController_Zrc_KeyPressed_t type defines the parameters that need to
 *  be passed to the ZRC module when a key press needs to be sent to a target.
*/
typedef struct gpController_Zrc_KeyPressed
{
    /* The binding ID of the key press. */
    UInt8 bindingId;
    /* The profile ID of the key press. */
    UInt8 profileId;
    /* The transmit options used to send the key press. */
    UInt16 txOptions;
    /* The vendor ID sent in the key press. */
    gpRf4ce_VendorId_t vendorId;
    /* A structure containing the key codes of the key press. */
    gpController_Keys_t keys;
}gpController_Zrc_KeyPressed_t;

/** @typedef gpController_Zrc_UnbindConfirm_t
 *  The gpController_Zrc_UnbindConfirm_t type defines parameters that need to be
 * passed to the controller when an unbind is confirmed.
*/
typedef struct gpController_Zrc_UnbindConfirm
{
    UInt8 bindingId;
    gpRf4ce_Result_t status;
}gpController_Zrc_UnbindConfirm_t;

/** @typedef gpController_Zrc_ValidationIndication_t
 *  The gpController_Zrc_ValidationIndication_t type defines parameters that
 *  need to be passed to the controller when validation is indicated.
*/

#ifdef GP_DIVERSITY_APP_ZRC2_0
/** @typedef gpController_Zrc_KeyMap_t
 *  The gpController_Zrc_KeyMap_t type defines the parameters that need to
 *  be passed to the ZRC module to set the list of supported action codes .
*/
typedef struct gpController_Zrc_KeyMap
{
    /* The keys. */
    const UInt8 *keys;
    /* Number of keys. */
    UInt8 count;
}gpController_Zrc_KeyMap_t;

typedef struct gpController_Zrc_ValidationIndication
{
    /** The binding ID of the validation. */
    UInt8 bindingId;
    /** The profile ID of the validation. */
    UInt8 profileId;
}gpController_Zrc_ValidationIndication_t;


/** @typedef gpController_Zrc_PowerStatus_t
 *  The gpController_Zrc_PowerStatus_t type defines the parameters that need to be
 * passed to the gpZrc module when it needs to push the aplPowerStatus attribute to the recipient
*/
typedef struct gpController_Zrc_PowerStatus
{
    /** bindingID to push the power status update to. */
    UInt8 bindingId;
    /** powerStatus, formatted as described in the GDP specification - aplPowerStatus attribute.*/
    UInt8 powerStatus;
}gpController_Zrc_PowerStatus_t;

/** @typedef gpController_Zrc_PowerStatusConfirm_t
 *  The gpController_Zrc_PowerStatusConfirm_t type defines parameters that need to be
 * passed to the controller when the powerStatus attribute push is confirmed.
*/
typedef struct gpController_Zrc_PowerStatusConfirm
{
    UInt8 bindingId;
    gpRf4ce_Result_t status;
}gpController_Zrc_PowerStatusConfirm_t;

#endif /*GP_DIVERSITY_APP_ZRC2_0*/

/** @typedef gpController_Zrc_SetKeyRepeatRate_t
 *  The gpController_Zrc_SetKeyRepeatRate_t type defines parameters that are passed from application to the ZRC module
 *  when the application sets the profile specific key repeat rate.
*/
typedef struct gpController_Zrc_SetKeyRepeatInterval
{
    UInt8 bindingId;
    UInt16 keyRepeatInterval;
} gpController_Zrc_SetKeyRepeatInterval_t;

typedef union {
    gpController_Zrc_KeyPressed_t KeyPressedIndication;
    UInt8 bindingId;
    UInt8 profileId;

    gpController_Zrc_BindConfirm_t BindConfirmParams;
    gpController_Zrc_UnbindConfirm_t ActionControlConfirmParams;
    gpController_Zrc_UnbindConfirm_t UnbindConfirmParams;
    gpController_Zrc_SetKeyRepeatInterval_t keyRepeatInterval;
#ifdef GP_DIVERSITY_APP_ZRC2_0
    gpController_Zrc_ValidationIndication_t ValidationParams;
    gpController_Zrc_KeyMap_t keyMap;
    gpController_Zrc_PowerStatus_t PowerStatusUpdate;
    gpController_Zrc_PowerStatusConfirm_t PowerStatusUpdateConfirm;
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
} gpController_Zrc_Msg_t;

/*******************************************************************************
 *                    Public Function Definitions
 ******************************************************************************/
 /** @ingroup ZRC
 *
 *  This primitive can be used to initialize the ZRC module.
  */
GP_API void gpController_Zrc_Init(void);

/** @ingroup ZRC
 *
 *  This primitive can be used to do a request to the ZRC module.
 *
 *  Completion of the request will be indicated by triggering a confirm using
 *  the gpController_ZRC_cbMsg callback.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Zrc_Msg(gpController_Zrc_MsgId_t msgId, gpController_Zrc_Msg_t *pMsg);

/** @ingroup ZRC
 *
 *  This primitive indicates (to the next higher layer) that an event occured.
 *
 *  @param msgId Identifier for the message.
 *  @param pMsg  Data passed in the message (NULL for indication without data).
 */
GP_API void gpController_Zrc_cbMsg(gpController_Zrc_MsgId_t msgId, gpController_Zrc_Msg_t *pMsg);

/** @ingroup ZRC
 *
 *  This primitive can be used to set an attribute for the ZRC module.
 *
 *  @param msgId Identifier for the attribute.
 *  @param pMsg  Data passed to the attribute.
 */
GP_API void gpController_Zrc_AttrGet(gpController_Zrc_MsgId_t msgId, gpController_Zrc_Msg_t *pMsg);

/** @ingroup ZRC
 *
 *  This primitive can be used to get an attribute from the ZRC module.
 *
 *  @param msgId Identifier for the attribute.
 *  @param pMsg  Data passed back to the requesting module.
 */
GP_API void gpController_Zrc_AttrSet(gpController_Zrc_MsgId_t msgId, gpController_Zrc_Msg_t *pMsg);

#ifdef GP_DIVERSITY_APP_ZID
GP_API Bool gpController_Zrc_IsZidProfileSupported(void);
#endif

UInt8 gpController_GetCurrentBindingId(void);

#endif /* _GPCONTROLLER_ZRC_H_ */
