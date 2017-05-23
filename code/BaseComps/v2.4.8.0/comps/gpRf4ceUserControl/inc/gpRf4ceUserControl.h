/*
 * Copyright (c) 2011-2013, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceUserControl.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceUserControl/inc/gpRf4ceUserControl.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_RF4CE_USER_CONTROL_H_
#define _GP_RF4CE_USER_CONTROL_H_

/// @file gpRf4ceUserControl.h
///
/// @defgroup USRCTRL USERCONTROL primitives
/// This module groups the primitives for communicating user-control button information.
///
/// The process is triggered by a changed state of a user key. The application must call the USERCONTROL request primitive with the propper statuskey in
/// order to transmit this information to connected device. If the corresponding user command frame is transmitted, the application is informed with the
/// USERCONTROL confirm primitive. If a user command frame is received on a device, its application will be informed by the USERCONTROL indication primitive.
/// 
/// The USERCONTROL request primitive specifies which key is pressed or released. It also indicates if the indicated key has single or repeated behavior.
/// For single keys, only the user command frame with keystatus specified as pressed will be transmitted. For repeated keys, the User-Control functional block
/// will automatically transmit the user command frames with keystatus specified as repeat until the functional block is informed the key is released.
///
/// The following picture shows the transmission process when single transmission mode is chosen:
///
/// @image latex USERCONTROL_SINGLE.pdf "USERCONTROL flow overview with single transmission" width=12cm
///
/// When the USERCONTROL request primitive is called with keystatus pressed, the corresponding  user command frame will be transmitted. The status of the pakket
/// transmission will be send to the application with the USERCONTROL confirm primitive. The functional block will take no further actions.
/// 
/// On the receiver side, the reception of the USERCONTROL indication primitive for the pressed key triggers a timeout <em>KeyRepeatWaitTime</em>.  If no repeat
/// message is received,  the target will trigger the USERCONTROL indication primitive for the released key after the timeout. This implementation on the target is
/// identical for the repeated transmission mode (see below).
///
/// When the key is released, the USERCONTROL request primitive can be called optionally with keystatus specified as released. However, in this case the functional
/// block will not transmit any message, it will only confirm the request as successful.
///
/// The following picture shows the transmission process when repeated transmission mode is chosen:
///
/// @image latex USERCONTROL_REPEATED.pdf "USERCONTROL flow overview with repeated transmission" width=12cm
///
/// Pressing a repeated key is simular as the single key situation. However the functional block will internally store that this particular key is pressed.
/// It will also store the corresponding bindingId. As long as there are pending pressed keys, an internal engine will send every <em>KeyRepeatInterval</em>
/// an additional user control command frame with keystatus specified as repeated to the proper device based on the stored bindingId. This will be done for
/// every pending pressed key. These user command frames will trigger no feedback to the application. However an optional early repeat indication callback
/// can be enabled which will be triggered just before the user control command frame with keystatus repeated is transmitted.
///
/// On the receiver side, these addtional user command frames with keystatus repeated will restart the <em>KeyRepeatWaitTime</em> timeout and will trigger the
/// USERCONTROL indication primitive with keystatus repeated.
///
/// If a repeatable key is released, the orginator should call the USERCONTROL request primitive with key state specified as released. The functional block will
/// remove the key from the pending pressed keys list and will transmit the user control frame with keystatus set to released.
///
/// On the receiver side, these user command frames with keystatus released will abort the <em>KeyRepeatWaitTime</em> timeout and will trigger the
/// USERCONTROL indication primitive with keystatus released.
///
/// If however the transmittion of a user control frames fails, the <em>KeyRepeatWaitTime</em> timeout will be triggered and the application will notice this as
/// a USERCONTROL indication call with keystatus specified as released.
///
/// @image latex USERCONTROL_REPEATED_TIMEOUT.pdf "USERCONTROL flow overview with repeated transmission with timeout" width=12cm
///
///
/// @defgroup ATTRIBUTE GET and SET primitives
/// This module groups the primitives to access the attributes of the functional block.
///
/// @image latex ATTR.pdf "GET and SET flow overview" width=12cm
///
/// @remark As these attribute-access functions can immediatly return with a status and in case of a get with the 
///  requested data, there is no need to overload this attribute access with confirm versions of the primitive. The
///  status is returned as return value, the requested data is passed back via a pointer argument.
///
///
/// @defgroup GENERAL General
///
/// It's good practice not to call a request function from a confirm funcion. Instead, update the application state
/// so that the request is called the next time the application state machines is triggered. Nested calls of any request
///  function in the confirm callback function should be avoided because:
/// - the confirm callback is triggered from the interrupt service routine
/// - deep call trees are created which are not supported on all embedded processors
/// - the RF4CE User-Control functional block does not queue requests

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include "gpRf4ce.h"
#include "gpRf4ceUserControl_CommandCodes.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//default compile time configuration
#ifndef GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS
#define GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS  4
#endif
#if GP_RF4CE_USER_CONTROL_REQ_MAX_PENDING_KEYS != 0
#define GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
#endif

#ifndef GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS
#define GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS  4
#endif
#if GP_RF4CE_USER_CONTROL_IND_MAX_PENDING_KEYS != 0
#define GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
#endif

/// The maximum time between consecutive user control repeated command frame transmission.
#define GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL       125UL   /* ms */

#ifndef GP_RF4CE_USER_CONTROL_VENDOR_ID
#ifndef GP_RF4CE_NWKC_VENDOR_IDENTIFIER
#define GP_RF4CE_USER_CONTROL_VENDOR_ID gpRf4ce_VendorIdGreenpeak
#else
#define GP_RF4CE_USER_CONTROL_VENDOR_ID GP_RF4CE_NWKC_VENDOR_IDENTIFIER
#endif
#endif

 
//TxOptions
/** TX options transmission mode bit mask */
#define GP_RF4CE_USER_CONTROL_TXOPTION_TRANS_MODE_BM                0x8000
    
/** Check for single transmission mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_USER_CONTROL_TXOPTION_IS_SINGLE(o)                 (!(o & GP_RF4CE_USER_CONTROL_TXOPTION_TRANS_MODE_BM))
/** Check for repeat transmission mode in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_USER_CONTROL_TXOPTION_IS_REPEAT(o)                 (!!(o & GP_RF4CE_USER_CONTROL_TXOPTION_TRANS_MODE_BM))
 
/** Set single transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_USER_CONTROL_TXOPTION_SET_SINGLE_TRANSMISSION(o)   o &= ~GP_RF4CE_USER_CONTROL_TXOPTION_TRANS_MODE_BM
/** Set repeatable transmission in tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_USER_CONTROL_TXOPTION_SET_REPEAT_TRANSMISSION(o)   o |= GP_RF4CE_USER_CONTROL_TXOPTION_TRANS_MODE_BM

/**  The bitmask defining which bits are used for RF4CE tx options
 */
#define GP_RF4CE_USER_CONTROL_TXOPTION_RF4CE_TXOPTION_BM           0x00FF


/**  Get Rf4ce tx options
 *  @param o  Tx options
 */
#define GP_RF4CE_USER_CONTROL_TXOPTION_GET_RF4CE_TXOPTION(o)           (o&GP_RF4CE_USER_CONTROL_TXOPTION_RF4CE_TXOPTION_BM)


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/// @name gpRf4ceUserControl_Attribute_t
//@{
/// @brief The interval in ms at which user command repeat frames will be transmitted for repeatable keys.
///        [Default: GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL/2 Range: 0 - GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL]
#define gpRf4ceUserControl_AttributeKeyRepeatInterval              0x80
/// @brief The duration that a recipient of a user control repeated command frame waits before terminating
///        a repeated operation.
///        [Default: GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL Range: >= GP_RF4CE_USER_CONTROL_MAX_KEY_REPEAT_INTERVAL]
#define gpRf4ceUserControl_AttributeKeyRepeatWaitTime              0x81
/// @typedef gpRf4ceUserControl_Attribute_t
/// @brief The gpRf4ceUserControl_Attribute_t type defines the attribute type.
typedef UInt8 gpRf4ceUserControl_Attribute_t;
//@}

/// @name gpRf4ceUserControl_KeyStatus_t
//@{
///  @brief Idle state.
#define gpRf4ceUserControl_KeyStatusIdle                0x00
/// @brief The key is pressed.
#define gpRf4ceUserControl_KeyStatusPressed             0x01
/// @brief This status is only valid at the receiver side. It indicates a key repeat packet is received
#define gpRf4ceUserControl_KeyStatusRepeated            0x02
/// @brief The key is released.
#define gpRf4ceUserControl_KeyStatusReleased            0x03
/// @typedef gpRf4ceUserControl_KeyStatus_t
/// @brief The gpRf4ceUserControl_KeyStatus_t type is used to signal a key press/release to the usercontrol block. 
typedef UInt8 gpRf4ceUserControl_KeyStatus_t;
// @}

/*****************************************************************************
 *                    Public Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/// @ingroup USRCTRL
/// This primitive initializes the UserControl block.
GP_API void gpRf4ceUserControl_Init(void);

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS
/// @ingroup ATTRIBUTE
/// This primitive updates the value of @ref gpRf4ceUserControl_Attribute_t <em>KeyRepeatWaitTime</em>.
/// @param  value       The new value for the attribute (in ms).
GP_API void gpRf4ceUserControl_SetKeyRepeatWaitTime(UInt16 value);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of @ref gpRf4ceUserControl_Attribute_t <em>KeyRepeatWaitTime</em>.
/// @return     The value for the attribute (in ms).
GP_API UInt16 gpRf4ceUserControl_GetKeyRepeatWaitTime(void);


/// @ingroup USRCTRL
/// This primitive indicates the reception of user control state change.
/// @param  bindingId       A unique identifier of the device that transmitted this user control request.
/// @param  profileId       The profile with which the user control request was send.
/// @param  vendorId        The vendorID used to tranmsit the packet.
/// @param  keyStatus       The user control status. If this argument is not gpRf4ceUserControl_KeyStatusPressed, the other arguments should be ignored.
/// @param  commandCode     The identifier for the user control.
/// @param  signalStrength  The signal strength of the received the user control packet.
/// @param  linkQuality     The link quality of the received the user control packet.
/// @param  rxFlags         The rxFlags of the received user control.
GP_API void gpRf4ceUserControl_cbUserControlIndication(UInt8                            bindingId,
                                                       gpRf4ce_ProfileId_t              profileId,
                                                       gpRf4ce_VendorId_t               vendorId,
                                                       gpRf4ceUserControl_KeyStatus_t   keyStatus,
                                                       gpRf4ceUserControl_CommandCode_t commandCode,
                                                       Int8                             signalStrength,
                                                       UInt8                            linkQuality,
                                                       UInt8                            rxFlags);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_INDICATIONS

#ifdef GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS
/// @ingroup ATTRIBUTE
/// This primitive updates the value of @ref gpRf4ceUserControl_Attribute_t <em>KeyRepeatInterval</em>.
/// @param  value       The new value for the attribute (in ms).
GP_API void gpRf4ceUserControl_SetKeyRepeatInterval(UInt16 value);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of @ref gpRf4ceUserControl_Attribute_t <em>KeyRepeatInterval</em>.
/// @return     The value for the attribute (in ms).
GP_API UInt16 gpRf4ceUserControl_GetKeyRepeatInterval(void);

/// @ingroup USRCTRL
/// This primitive triggers the transmission of user control state change.
/// @param  bindingId       A unique identifier of the device to which the user control frame should be transmitted.
/// @param  profileId       The profile that uses this UserControlRequest.
/// @param  vendorId        The vendorID used to tranmsit the packet.
/// @param  keyStatus       The user control status. Calls with keystatus argument specified as gpRf4ceUserControl_KeyStatusRepeated
///                         will be ignored (-> gpRf4ceUserControl_cbUserControlConfirm triggered with status argument set to gpRf4ce_ResultInvalidParameter).
/// @param  cmdCode         The identifier for the user control.
/// @param  txOptions       Txoptions field.
/// @param rfRetriesPeriod  RfRetries period used, in \B5s.
void gpRf4ceUserControl_UserControlRequest(UInt8                                bindingId,
                                           gpRf4ce_ProfileId_t                  profileId,
                                           gpRf4ce_VendorId_t                   vendorId,
                                           gpRf4ceUserControl_KeyStatus_t       keyStatus,
                                           gpRf4ceUserControl_CommandCode_t     cmdCode,
                                           UInt16                               txOptions,
                                           UInt32                               rfRetriesPeriod);

/// @ingroup USRCTRL
/// This primitive reports the results of the transmission of user control state change.
/// @param  bindingId       A unique identifier of the device to which the user control frame was transmitted.
/// @param status           The result of the transmission of user control state change.
/// @param keyStatus        The status of the key which was transmitted in the user-control packet.
/// @param commandCode      The key code of the sent user-control packet.
GP_API void gpRf4ceUserControl_cbUserControlConfirm(UInt8                            bindingId,
                                                    gpRf4ce_Result_t                 status,
                                                    gpRf4ceUserControl_KeyStatus_t   keyStatus,
                                                    gpRf4ceUserControl_CommandCode_t commandCode,
                                                    UInt16                           txOptions);

/// @ingroup USRCTRL
/// This primitive indicates the preparation of a new user control repeat message. 
/// The user may prevent unneeded repeat messages by cancelling the transmission in this function.
/// This is an optional primitive which should be enabled with GP_RF4CEUSERCONTROL_DIVERSITY_EARLYREPEATINDICATION.
GP_API void gpRf4ceUserControl_cbEarlyRepeatIndication(void);

/// @ingroup USRCTRL
/// This primitive reports the results of the failed transmission of user control repeat.
/// @param  bindingId       A unique identifier of the device to which the user control frame was transmitted.
/// @param status           The result of the transmission of user control state change.
/// @param keyStatus        The status of the key which was transmitted in the user-control packet.
/// @param commandCode      The key code of the sent user-control packet.
GP_API void gpRf4ceUserControl_cbUserControlRepeatFailureIndication(UInt8                            bindingId,
                                                                    gpRf4ce_Result_t                 status,
                                                                    gpRf4ceUserControl_KeyStatus_t   keyStatus,
                                                                    gpRf4ceUserControl_CommandCode_t commandCode,
                                                                    UInt16                           txOptions);
#endif //GP_RF4CE_USER_CONTROL_HANDLE_REQUESTS

#ifdef __cplusplus
}
#endif

#endif //_GP_RF4CE_USER_CONTROL_H_


