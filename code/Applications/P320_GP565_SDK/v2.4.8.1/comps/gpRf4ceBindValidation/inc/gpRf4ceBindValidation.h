/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceBindValidation.
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpRf4ceBindValidation/inc/gpRf4ceBindValidation.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GP_RF4CE_BIND_VALIDATION_H_
#define _GP_RF4CE_BIND_VALIDATION_H_


/// @file gpRf4ceBindValidation.h
///
/// @defgroup BIND BIND primitives
/// This module groups the primitives for the binding procedure.
///
/// @anchor Successful-Binding
/// <h2>Successful-Binding</h2>
/// The complete process of a successful binding is shown in the next figure:
/// @image latex BIND_SUCCESS.pdf "Successful BIND" width=12cm
///
/// The process is triggered by issuing the BIND request primitive on the controller. The functional block component
/// discovers the targets in range and sorts the detected targets. This classification of the detected targets is based
/// on additional information the targets are passing to the controller as part of the user-string. The content of this
/// additional information is requested from the target's application with the DISCOVERY indication primitive. The application
/// should anwser this call with a call of the DISCOVERY response primitive.
///
/// In the next step, the functional block will pair with the most likely target. If the pair is successful, the STARTVALIDATION
/// indication primitive will be called. The functional block on the controller side will also start transmitting check
/// validation request frames every <em>AutoCheckValidationPeriod</em>. After transmitting this request, the controller
/// should start a listening window.
///
/// On the target side, the link with the controller will be labelled as temporary paired in the BindValidation functional
/// block. The application will be informed of this temporary binding via the BIND indication primitive. The criteria to accept
/// or reject this bind is out of scope of this document. The are independant of the BindValidation functional block itself.
/// If the target accept the temporary bind, it should call the BIND response primitive with argument success. This will
/// label the link as permanent.
///
/// When the functional block at the target side receives a check validation request frame, it should respond by sending
/// a check validation response frame. If the link with the controller is still labelled as temporary, the status in the
/// response should be pending. If the link is labled as permanent, the status in the response should be success.
///
/// When the functional block at the controller side receives a check validation response frame with status success. The
/// regular transmissions of check validation request frames will be disabled and the controller application will be informed
/// of this successful binding with the BIND confirm primitive.
///
/// @anchor Rejected-Binding
/// <h2>Rejected-Binding</h2>
/// The complete process of a rejected binding is shown in the next figure:
/// @image latex BIND_REJECT.pdf "Rejected BIND" width=12cm
///
/// If a target doesn't allow the new initiated binding, it should respond on the BIND indication callback with a call of the
/// BIND response primitive with argument NotPermitted. This will label the link as rejected. When the functional block receives
/// a check validation request frame from the controller, the corresponding check validation response will contain a failure status.
/// When the check validation reponse frame is transmitted successfully, the link will be removed from the pairing table in the underlaying
/// RF4CE layer and the controller will be labled as unpaired.
///
/// When the functional block at the controller side receives a check validation response frame with status failure. The
/// regular transmissions of check validation request frames will be disabled and the controller application will be informed
/// of this failed validation with a trigger of the FAILVALIDATION indication primitive.
///
/// <h2>Multiple Binding Candidates</h2>
/// The binding procedure of the  BindValidation functional block is based on a binding candidate list collected by an initial
/// discovery phase. If a binding attempt is rejected by the target (see @ref Rejected-Binding), the controller will start a new pairing
/// attempt with the with the next most likely target. This is illustrated in the figure below:
///
/// @image latex BIND_SUCCESS_2ND.pdf "BIND with multiple targets" width=12cm
///
/// @remark In the descriptions below, the mechanism to restart all the pairing and validation procedure is not mentioned explicitly.
/// When the FAILVALIDATION indication primitive is mentioned in the explanation below, it is implicitly assumed that the functional
/// block on a controller will keep on trying to bind with another target as long as there are possbile binding candidates.
///
/// @anchor Timeouts
/// <h2>Timeouts</h2>
/// There are some scenarios where the functional block will automatically stop the validation process.
///
/// At the target side, the <em>ValidationWaitTime</em> is a timeout which is started from the moment the BIND indication primitive is
/// called. If this timeout is triggered, the current link will automatically be labelled as rejected. Next to this global timeout, the
/// functional block has a validation watchdog timer which is initilaze with <em>ValidationWatchdogTime</em> at the moment the BIND
/// indication primitive is called. This watchdog should be reseted by the application with the gpRf4ceBindValidation_ResetWatchdogTimer()
/// primitive. If the validation watchdog is triggered, the current link will also be labelled as rejected. These automatic validation
/// rejections or the rejections triggered from the targets application are transparant for the controller. The controller will abort
/// the validation as desicribed in @ref Reject-Binding. Both timeout mechanism are illustrated in the figure below:
///
/// @image latex BIND_TIMEOUT.pdf "BIND with validation timeout" width=12cm
///
/// The functional block on a controller has a <em>LinkLostWaitTime</em> timeout to avoid endless validation attempts when the radio
/// communication between the controller and the target is lost (e.g. out of range situation). This timeout is started whenever a check
/// validation request frame is transmitted and if it is not already pending. If a check validation response frame is received, the timeout
/// is cancelled. If the link-lost timeout is triggered, the validation will stop and the FAILVALIDATION indication callback will be triggered.
/// The validation will also be aborted on the target side based on one of the automatic mechanism described in the paragraph above. The
/// link-lost timeout is illustrated in the figure below:
///
/// @image latex BIND_LINKLOST.pdf "BIND with linklost" width=12cm
///
///
/// @defgroup UNBIND UNBIND primitives
/// This module groups the primitives for the unbinding procedures.
///
/// <h2>Target</h2><br>
/// A target application can force to remove a link with a controller device. It should call the UNBIND request primitive. When the
/// application asks to remove a link which is labelled as permanent, the link will be labeld as unpaired an the UNPAIR request primitive
/// of the underlaying RF4CE layer will be triggered. If the controller receives the unpair request frame (not garanteed nor required),
/// the link will also be labelled as unpaired and the controller application will be informed by a trigger of the UNBIND indication callback.
///
/// @image latex UNBIND_TARGET.pdf "UNBIND at target" width=12cm
///
/// This behavior is exactly the same if the link is still labelled as temporary: mark the link as unpaired and call the UNPAIR request primitive
/// of the RF4CE layer. If the controller however receives an unpair request frame while it is still in validation phase, the autocheck mechanism
/// will be disabled and the controller application will be informed with a call of FAILVALIDATION indication primitive.
///
/// @image latex UNBIND_TARGET_VALIDATION.pdf "UNBIND at target during validation" width=12cm
///
/// It is however more likely that the unpair request frame will not be received by the controller as it will probably not have enabled the receiver.
/// In this case, as the link is removed at the target side, the check validation request frames will not be received by the target and the
/// link-lost timeout will be triggered in the BindValidation functional block on the controller.
///
/// @image latex UNBIND_TARGET_VALIDATION_LINKLOST.pdf "UNBIND at target during validation causing broken link" width=12cm
///
/// <h2>Controller</h2><br>
/// If a controller application wants to remove an existing link wit a target, it should also call the UNBIND request primitive. This will
/// trigger the UNPAIR request primitive of the underlaying RF4CE layer. If the unpair request frame is received by the target (not garanteed
/// nor required), the application will be informed with a call of the UNBIND indication primitive. The link will be labelled as unpaired on
/// both devices.
///
/// @image latex UNBIND_CONTROLLER.pdf "UNBIND at controller" width=12cm
///
/// If the UNBIND request primitive is called on a controller during a validation, additionally to the call of UNPAIR request primitive of the
/// RF4CE layer, the automatic check validation mechansim will be disabled and the FAILVALIDATION incidation callback will be triggered.
///
/// @image latex UNBIND_CONTROLLER_VALIDATION.pdf "UNBIND at controller during validation" width=12cm
///
///
/// @defgroup ATTRIBUTE GET and SET primitives
/// This module groups the primitives to access the attributes of the functional block.
///
/// @image latex ATTR.pdf "Attribute GET and SET flow overview" width=12cm
///
/// @remark As these attribute-access functions can immediatly return with a status and in case of a get with the
/// requested data, there is no need to overload this attribute access with confirm versions of the primitive. The
/// status is returned as return value, the requested data is passed back via a pointer argument.
///
///
/// @defgroup INIT Initialization
///
/// @defgroup MISC Miscellaneous
///
/// @defgroup GENERAL General
///
/// It's good practice not to call a request function from a confirm function. Instead, update the application state
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

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/// @brief The maximum amount of binding candidates considered when binding (can be overruled at compilation time).
#define GP_RF4CE_BIND_VALIDATION_MAX_BINDING_CANDIDATES                     3

/// @brief The time (in ms) a device shall wait after the successful transmission of a request frame, before enabling the receiver to receive a response frame.
#define GP_RF4CE_BIND_VALIDATION_RESPONSE_IDLE_TIME                         50ul

/// @brief The time (in ms) a device shall wait after the GP_RF4CE_BIND_VALIDATION_RESPONSE_IDLE_TIME expired, to receive a response frame following a request frame.
#define GP_RF4CE_BIND_VALIDATION_RESPONSE_WAIT_TIME                         100ul

/// @brief The default time (in ms) a controller device perform a new check validation request.
#define GP_RF4CE_BIND_VALIDATION_DEFAULT_AUTO_CHECK_VALIDATION_PERIOD       500ul

/// @brief The default time (in ms) a controller device will abort if it wasn't able to successfully communicate with the target device.
#define GP_RF4CE_BIND_VALIDATION_DEFAULT_LINK_LOST_WAIT_TIME                10000ul

/// @brief  Maximum length of the MSO user string
#define GP_RF4CE_BIND_VALIDATION_MAX_USER_STRING_LENGTH                     9

#define GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_IDX                  0x00
#define GP_RF4CE_BIND_VALIDATION_AUTO_BIND_FAIL_THRESH_IDX                  0x03
#define GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_COUNTER_IDX          0x06

#define GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_BM                   0x07
#define GP_RF4CE_BIND_VALIDATION_AUTO_BIND_FAIL_THRESH_BM                   0x38
#define GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_COUNTER_BM           0x40

#define RF4CE_BIND_VALIDATION_BM_SET(bf,bm,idx,v)                           bf &= ~bm; bf |= (((UInt16)v) << idx)

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/// @todo JAVE: remove/rename apl/aplc prepend of attributes in FS

/// @name gpRf4ceBindValidation_Attribute_t
//@{
/// @brief The maximum number of binding candidates selected from the discovery response node descriptor list.
///        This is a controller specific attribute.
///        [Default: GP_RF4CE_BIND_VALIDATION_MAX_BINDING_CANDIDATES Range: 0x00 - GP_RF4CE_BIND_VALIDATION_MAX_BINDING_CANDIDATES]
#define gpRf4ceBindValidation_AttributeMaxBindingCandidates                 0x82
/// @brief A flag indication if the controller takes the autovalidate flag in the discovery responses
///        in to account or not.
///        [Default: true]
#define gpRf4ceBindValidation_AttributeAllowAutoValidation                  0x83


/// @brief The repeat period (in ms) on which a controller will transmit check validation request
///        frames during the validation phase.
///        On a controller, this attribute is readonly. It refers to the configured value of the
///        validation property if there was a successful RIB exchange. Otherwise it will refer to
///        the default value. On a target, this attribute refers to the reconfigurable validation
///        properties.
///        [Default: 500ms Range: 1ms - 65535ms]
#define gpRf4ceBindValidation_AttributeAutoCheckValidationPeriod            0x85
/// @brief The maximum time (in ms) a controller is allowed to have non confirmed communication
///        with the target during the validation phase (see @ref Timeouts).
///        On a controller, this attribute is readonly. It refers to the configured value of the
///        validation property if there was a successful RIB exchange. Otherwise it will refer to
///        the default value. On a target, this attribute refers to the reconfigurable validation
///        properties.
///        [Default: 10000ms Range: 1ms - 65535ms]
#define gpRf4ceBindValidation_AttributeLinkLostWaitTime                     0x86


/// @typedef gpRf4ceBindValidation_Attribute_t
/// @brief The gpRf4ceBindValidation_Attribute_t type defines the attribute type.
typedef UInt8 gpRf4ceBindValidation_Attribute_t;
//@}

/// @name gpRf4ceBindValidation_CommandId_t
//@{
#define gpRf4ceBindValidation_CommandIdCheckValidationRequest         0x20
#define gpRf4ceBindValidation_CommandIdCheckValidationResponse        0x21
/// @typedef gpRf4ceBindValidation_CommandId_t
/// @brief The gpRf4ceBindValidation_CommandId_t type defines the command identifiers.
typedef UInt8 gpRf4ceBindValidation_CommandId_t;
//@}

/// @name gpRf4ceBindValidation_CheckValidationControl_t
//@{
/// @brief The default check validation procedure is used.
#define gpRf4ceBindValidation_CheckValidationControlDefault                       0x00
/// @brief Used to indicate the controller request to bind via Auto Validation.
#define gpRf4ceBindValidation_CheckValidationControlRequestAutoValidation         0x01
/// @typedef gpRf4ceBindValidation_CheckValidationControl_t
/// @brief The gpRf4ceBindValidation_CheckValidationControl_t type define the possible values for the Check Validation Control field of the Check Validation Request command frame.
typedef UInt8 gpRf4ceBindValidation_CheckValidationControl_t;
//@}

/// @name gpRf4ce_ResultValidation_t
//@{
/// @brief The validation is still in progress.
#define gpRf4ce_ResultValidationPending                 0xC0
/// @brief The validation timed out, and the binding procedure should continue with other devices in the list.
#define gpRf4ce_ResultValidationTimeout                 0xC1
/// @brief The validation was terminated at the target side, as more than one controller tried to pair.
#define gpRf4ce_ResultValidationCollision               0xC2
/// @brief The validation failed, and the binding procedure should continue with other devices in the list.
#define gpRf4ce_ResultValidationFailure                 0xC3
/// @brief The validation was aborted, and the binding procedure should continue with other devices in the list.
#define gpRf4ce_ResultValidationAbort                   0xC4
/// @brief The validation is aborted, and the binding procedure should NOT continue with other devices in the list.
#define gpRf4ce_ResultValidationFullAbort               0xC5
/// @typedef gpRf4ce_ResultValidation_t
/// @brief The extentions on the gpRf4ce_Result_t type to encode validation results.
typedef int gpRf4ce_ResultValidation_t;
//@}

/// @struct gpRf4ceBindValidation_UserString
/// @brief The gpRf4ceBindValidation_UserString stucture specifies the MSO profile application specific user string.
/// @typedef gpRf4ceBindValidation_UserString_t
/// @brief The gpRf4ceBindValidation_UserString_t type specifies the MSO profile application specific user string.
typedef struct gpRf4ceBindValidation_UserString {
    char str[GP_RF4CE_BIND_VALIDATION_MAX_USER_STRING_LENGTH];
} gpRf4ceBindValidation_UserString_t;

/// @struct gpRf4ceBindValidation_OriginatorInfo
/// @brief The gpRf4ceBindValidation_OriginatorInfo structure specifies the originator information used to initiate a bind request.
/// @typedef gpRf4ceBindValidation_OriginatorInfo_t
/// @brief The gpRf4ceBindValidation_OriginatorInfo_t type specifies the originator information used to initiate a bind request
typedef struct gpRf4ceBindValidation_OriginatorInfo{
    UInt8                   orgAppCapabilities;
    gpRf4ce_DeviceType_t    pOrgDevTypeList[3];
    gpRf4ce_ProfileId_t     pOrgProfileIdList[7];
    gpRf4ce_DeviceType_t    searchDevType;
    UInt8                   discProfileIdListSize;
    gpRf4ce_ProfileId_t     pDiscProfileIdList[7];
} gpRf4ceBindValidation_OriginatorInfo_t;

/// @brief Dedicated-Key-Combo-Bind value for the initiator incidator field of gpRf4ceBindValidation_OriginatorBindInfo_t.
#define GP_RF4CE_BIND_VALIDATION_ORG_BI_INITIATORINDICATOR_DEDICATEDKEY    0x0
/// @brief Any-Button-Bind value for the initiator incidator field of gpRf4ceBindValidation_OriginatorBindInfo_t.
#define GP_RF4CE_BIND_VALIDATION_ORG_BI_INITIATORINDICATOR_UNPAIRED        0x1

/// @struct gpRf4ceBindValidation_OriginatorBindInfo
/// @brief The gpRf4ceBindValidation_OriginatorBindInfo structure specifies the originator bind information used to prepare the classification information in the discovery response.
/// @typedef gpRf4ceBindValidation_OriginatorBindInfo_t
/// @brief The gpRf4ceBindValidation_OriginatorBindInfo_t type specifies the originator bind information used to prepare the classification information in the discovery response.
typedef struct gpRf4ceBindValidation_OriginatorBindInfo{
    UInt8                               initiationIndicator;
    gpRf4ceBindValidation_UserString_t  userString;
} gpRf4ceBindValidation_OriginatorBindInfo_t;

/// @brief Access macro to parse the Basic LQI Treshold information from a discovery node-descriptor
#define GP_RF4CE_BIND_VALIDATION_REC_BI_NODEDESC_BASICLQITRESHOLD(pNodeDesc)                 ((pNodeDesc)->userString.str[GP_RF4CE_MAX_USER_STRING_LENGTH-1])
/// @brief Access macro to parse the Strict LQI Treshold information from a discovery node-descriptor
#define GP_RF4CE_BIND_VALIDATION_REC_BI_NODEDESC_STRICTLQITRESHOLD(pNodeDesc)                ((pNodeDesc)->userString.str[GP_RF4CE_MAX_USER_STRING_LENGTH-2])
/// @brief Access macro to parse the specified Class Descriptor information from a discovery node-descriptor, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_NODEDESC_CLASSDESC(pNodeDesc,classType)              ((pNodeDesc)->userString.str[GP_RF4CE_MAX_USER_STRING_LENGTH-3-(classType)])
/// @brief Access macro to parse the ApplyStrictLQITreshold flag from the specified Class Descriptor information of a discovery node-descriptor, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_NODEDESC_CLASSDESC_STRICTLQI(pNodeDesc,classType)    ((pNodeDesc)->userString.str[GP_RF4CE_MAX_USER_STRING_LENGTH-3-(classType)] & 0x40)

/// @brief Access macro for the Basic LQI Treshold field in gpRf4ceBindValidation_ReceiverBindInfo_t
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_BASICLQITRESHOLD(bindInfo)                 ((bindInfo).basicLqiTreshold)
/// @brief Access macro for the Strict LQI Treshold field in gpRf4ceBindValidation_ReceiverBindInfo_t
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_STRICTLQITRESHOLD(bindInfo)                ((bindInfo).strictLqiTreshold)
/// @brief Access macro for a Class Descriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC(bindInfo,classType)                        ((bindInfo).classDescriptor[(classType)])
/// @brief Access macro to parse the Class Number form a classDescriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_CLASSNBR(bindInfo,classType)               ((bindInfo).classDescriptor[(classType)] & 0x0F)
/// @brief Access macro to parse the Duplicate Class Number Handling from a classDescriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_HANDLING(bindInfo,classType)              (((bindInfo).classDescriptor[(classType)] & 0x30) >> 4)
/// @brief Access macro to parse the ApplyStrictLQITreshold flag from a classDescriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_STRICTLQI(bindInfo,classType)              (((bindInfo).classDescriptor[(classType)] & 0x40) == 0x40)
/// @brief Access macro to parse the EnableRequestAutoValidation flag from a classDescriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_AUTOVALIDATION(bindInfo, classType)        (((bindInfo).classDescriptor[(classType)] & 0x80) == 0x80)
/// @brief Access macro to set a classDescriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_SET_CLASSDESC(bindInfo, classType, classNbr, handling, strictLqi, autoValidation) \
                (bindInfo).classDescriptor[(classType)] = (((autoValidation) & 0x01) << 7) | (((strictLqi) & 0x01) << 6) | (((handling) & 0x03) << 4) | ((classNbr) & 0x0F)
/// @brief Access macro to initialize a classDescriptor in gpRf4ceBindValidation_ReceiverBindInfo_t, classType should be 0,1 or 2 for Primary, Secondary or Tertiary.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_INIT_CLASSDESC(bindInfo, classType)                   GP_RF4CE_BIND_VALIDATION_REC_BI_SET_CLASSDESC((bindInfo), (classType), 15, GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_HANDLING_ABORT, false, false)

/// @brief Access macro for the Auto-bind pair threshold (autoBindProperties field) in gpRf4ceBindValidation_ReceiverBindInfo_t
#define GP_RF4CE_BIND_VALIDATION_REC_BI_SET_AUTO_BIND_PAIR_THRESH(bindInfo, thresh)          RF4CE_BIND_VALIDATION_BM_SET(((bindInfo).autoBindProperties), GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_BM, GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_IDX, thresh)
/// @brief Access macro for the Auto-bind fail threshold (autoBindProperties field) in gpRf4ceBindValidation_ReceiverBindInfo_t
#define GP_RF4CE_BIND_VALIDATION_REC_BI_SET_AUTO_BIND_FAIL_THRESH(bindInfo, thresh)          RF4CE_BIND_VALIDATION_BM_SET(((bindInfo).autoBindProperties), GP_RF4CE_BIND_VALIDATION_AUTO_BIND_FAIL_THRESH_BM, GP_RF4CE_BIND_VALIDATION_AUTO_BIND_FAIL_THRESH_IDX, thresh)
/// @brief Access macro for the Auto-bind pair threshold counter (autoBindProperties field) in gpRf4ceBindValidation_ReceiverBindInfo_t
#define GP_RF4CE_BIND_VALIDATION_REC_BI_SET_AUTO_BIND_PAIR_THRESH_COUNTER(bindInfo, thresh)  RF4CE_BIND_VALIDATION_BM_SET(((bindInfo).autoBindProperties), GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_COUNTER_BM, GP_RF4CE_BIND_VALIDATION_AUTO_BIND_PAIR_THRESH_COUNTER_IDX, thresh)

/// @brief Use-node-descriptor-as-is value for the Duplicate Class Number Handling bits of the classDescriptor field of gpRf4ceBindValidation_ReceiverBindInfo_t.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_HANDLING_ASIS        0x0
/// @brief Remove-node-descriptor value for the Duplicate Class Number Handling bits of the classDescriptor field of gpRf4ceBindValidation_ReceiverBindInfo_t.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_HANDLING_REMOVE      0x1
/// @brief Reclassify-node-descriptor value for the Duplicate Class Number Handling bits of the classDescriptor field of gpRf4ceBindValidation_ReceiverBindInfo_t.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_HANDLING_RECLASS     0x2
/// @brief Abor-binding value for the Duplicate Class Number Handling bits of the classDescriptor field of gpRf4ceBindValidation_ReceiverBindInfo_t.
#define GP_RF4CE_BIND_VALIDATION_REC_BI_CLASSDESC_HANDLING_ABORT       0x3

/// @struct gpRf4ceBindValidation_ReceiverBindInfo
/// @brief The gpRf4ceBindValidation_ReceiverBindInfo structure specifies the reciever bind information used as input for the classification algorithm on the origanator of the bind.
/// @typedef gpRf4ceBindValidation_ReceiverBindInfo_t
/// @brief The gpRf4ceBindValidation_ReceiverBindInfo_t type specifies the reciever bind information used as input for the classification algorithm on the origanator of the bind.
typedef struct gpRf4ceBindValidation_ReceiverBindInfo{
    UInt8                   basicLqiTreshold;
    UInt8                   strictLqiTreshold;
    UInt8                   autoBindProperties;
    UInt8                   classDescriptor[3];
} gpRf4ceBindValidation_ReceiverBindInfo_t;

/*****************************************************************************
 *                    Public Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/// @ingroup INIT
/// This function initializes the RF4CE BindValidation functional block. It should be called before calling any other
/// request function. This primitive is typically called via the gpBaseComps_StackInit() method of the gpBaseComps component.
/// The primitive only initilizes the RF4CE BindValidation functional block. If required the underlying layers need to be
///  initalize explicitly by the application (or gpBaseComps component).
GP_API void gpRf4ceBindValidation_Init(void);

/// @ingroup ATTRIBUTE
/// This primitive updates the value of the <em>DiscoveryLqiTreshold</em> attribute.
/// @param  value       The new value for the attribute.
void gpRf4ceBindValidation_SetDiscoveryLqiTreshold(UInt8 value);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of the <em>DiscoveryLqiTreshold</em> attribute.
/// @return     The value for the attribute.
UInt8 gpRf4ceBindValidation_GetDiscoveryLqiTreshold(void);

/// @ingroup ATTRIBUTE
/// This primitive updates the value of the <em>MaxBindingCandidates</em> attribute.
/// @param  value       The new value for the attribute.
UInt8 gpRf4ceBindValidation_GetMaxBindingCandidates(void);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of the <em>MaxBindingCandidates</em> attribute.
/// @return     The value for the attribute.
void gpRf4ceBindValidation_SetMaxBindingCandidates(UInt8 value);

/// @ingroup ATTRIBUTE
/// This primitive updates the value of the <em>AllowAutoValidate</em> attribute.
/// @param  value       The new value for the attribute.
Bool gpRf4ceBindValidation_GetAllowAutoValidation(void);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of the <em>AllowAutoValidate</em> attribute.
/// @return     The value for the attribute.
void gpRf4ceBindValidation_SetAllowAutoValidation(Bool value);

/// @ingroup ATTRIBUTE
/// This primitive updates the value of the <em>AutoCheckValidationPeriod</em> attribute.
/// @param  value       The new value for the attribute.
void gpRf4ceBindValidation_SetAutoCheckValidationPeriod(UInt16 value);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of the <em>AutoCheckValidationPeriod</em> attribute.
/// @return     The value for the attribute.
UInt16 gpRf4ceBindValidation_GetAutoCheckValidationPeriod(void);

/// @ingroup ATTRIBUTE
/// This primitive updates the value of the <em>LinkLostWaitTime</em> attribute.
/// @param  value       The new value for the attribute.
void gpRf4ceBindValidation_SetLinkLostWaitTime(UInt16 value);
/// @ingroup ATTRIBUTE
/// This primitive reads the value of the <em>LinkLostWaitTime</em> attribute.
/// @return     The value for the attribute.
UInt16 gpRf4ceBindValidation_GetLinkLostWaitTime(void);

/// @ingroup ATTRIBUTE
/// This primitive reads the value of the readonly <em>BindPending</em> attribute.
/// @return     True if the BindValidation functional block has a pending binding, false otherwise.
Bool gpRf4ceBindValidation_IsBindPending(void);

/// @ingroup ATTRIBUTE
/// This primitive checks if device is validated based on the Mac address</em> attribute.
/// @param pMacAddr A pointer to the extended address of the device.
/// @return     True if the BindValidation functional block has a pending binding, false otherwise.
Bool gpRf4ceBindValidation_IsDeviceValidated(MACAddress_t* pMacAddr);



/// @ingroup BIND
/// This primitive allows a controller application to request the RF4CE BindValidation functional
/// block to bind with a target device.
/// @param  orgProfileId        The profile id that will be used during the binding validation process to transmit RF4CE data packets (RequestValidation/RIB).
/// @param  pOrgInfo            Info of the node issuing this primitive.
/// @param  keyExTransferCount  The number of transfers the target should use to exchange the link
///                             key with the binding originator.
/// @param  pOrgBindInfo        Additional binding info of the node issuing this primitive.
/// @param  rfRetryPeriod       The retry period that will be used in µs.
/// @remark This primitive is only used for controllers.
GP_API void gpRf4ceBindValidation_BindRequest(gpRf4ce_ProfileId_t                              orgProfileId,
                                              const gpRf4ceBindValidation_OriginatorInfo_t     *pOrgInfo,
                                              UInt8                                             keyExTransferCount,
                                              const gpRf4ceBindValidation_OriginatorBindInfo_t *pOrgBindInfo,
                                              UInt32                                            rfRetriesPeriod);


/// @ingroup BIND
/// This primitive allows the RF4CE BindValidation functional block to notify the controller application
/// that the bind procedure is finished.
/// @param bindingId            Contains a unique identifier of the target device if status equals
///                             gpRf4ce_ResultSuccess, otherwise 0xFF.
/// @param status               The status of the bind attempt.
/// @remark This primitive is only used for controllers.
GP_API void gpRf4ceBindValidation_cbBindConfirm(UInt8            bindingId,
                                                gpRf4ce_Result_t status);

/// @ingroup BIND
/// This primitive allows a controller application to abort a pending validation.
/// @param fullAbort            If specified as false, only the binding attempt with the current target
///                             will be aborted, if specified as true the complete binding process will
///                             be aborted.
/// @return                     Returns the status of the abort request.
/// @remark This primitive is only used for controllers.
GP_API gpRf4ce_Result_t gpRf4ceBindValidation_BindAbortRequest(Bool fullAbort);

/// @ingroup BIND
/// This primitive allows the RF4CE BindValidation functional block to notify the controller application
/// that new link to a target device has been established and that a validation is started.
/// @param bindingId            Contains a unique identifier of the target.
/// @remark This primitive is only used for controllers.
GP_API void gpRf4ceBindValidation_cbStartValidationIndication(UInt8 bindingId);


/// @ingroup BIND
/// On a controller, this primitive allows the RF4CE BindValidation functional block to notify the controller application
/// that a validation with a target device has failed.
/// On a target, this primitive allows the RF4CE BindValidation functional block to notify the target application
/// that the functional block has internally stopped the validation phase (timeout triggered).
/// @param bindingId            Contains a unique identifier of the device that was in validation state.
/// @param status               Contains a specific reason for failure.
GP_API void gpRf4ceBindValidation_cbFailValidationIndication(UInt8 bindingId, gpRf4ce_ResultValidation_t status);



/// @ingroup UNBIND
/// This primitive removes an existing link between a target and a controller. (see @ref UNBIND).
/// @param bindingId            The identifier of the device from which the link needs to be removed.
GP_API void gpRf4ceBindValidation_UnbindRequest(void* bindingId);

/// @ingroup UNBIND
/// This primitive indicates that a link to a connected device has been removed.
/// @param bindingId            The identifier of the device from which the link has been removed.
GP_API void gpRf4ceBindValidation_cbUnbindIndication(UInt8 bindingId);

/// @ingroup UNBIND
/// This primitive allows the RF4CE BindValidation functional block to notify the application that the
/// unbind has finished.
/// @param bindingId            The identifier of the device from which the link has been removed.
/// @param status               The status of the UnpairRequest frame transmission.
GP_API void gpRf4ceBindValidation_cbUnbindConfirm(UInt8            bindingId,
                                                  gpRf4ce_Result_t status);

/// @ingroup MISC
/// This primitive allows the application to request that the receiver is either enabled (for a finite period or until further notice) or disabled.
/// The RF4CE BindValidation functional block will forward this call to the RF4CE layer if it was able to lock the RF4CE profile dispatcher.
/// @param   rxOnDuration   The number of MAC symbols for which the receiver is to be enabled. A value 0xFFFFFF will enable the receiver until further notice, while 0x0 will disable the receiver.
///                         For power saving mode, this value should correspond to the value of gpRf4ce_AttributeNwkActivePeriod attribute of the RF4CE layer.
GP_API void gpRf4ceBindValidation_RxEnableRequest(Bool enable);

/// @ingroup MISC
/// This primitive reports the results of the attempt to enable or disable the receiver.
/// @param status               The result of the request to enable or disable the receiver.
GP_API void gpRf4ceBindValidation_cbRxEnableConfirm(gpRf4ce_Result_t status);

void gpRf4ceBindValidation_SetForceRollback(Bool forceRollback);

#ifdef __cplusplus
}
#endif

#endif //_GP_RF4CE_BIND_VALIDATION_H_


