/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 *   This file contains the definitions of the public functions and enumerations of the gpRf4ceRib.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRf4ceRib/inc/gpRf4ceRib.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GP_RF4CE_RIB_H_
#define _GP_RF4CE_RIB_H_


/// @file gpRf4ceRib.h
///
///
/// @defgroup RIB_ATTRIBUTE GET-RIB and SET-RIB primitives
/// This module groups the primitives to access the RIB attributes.
///
/// The following picture shows the process of accessing the Remote Information Base (RIB) attributes:
///
/// @image latex RIB.pdf "GET_RIB and SET_RIB flow overview" width=12cm
///
/// For both cases, the process is triggered by the corresponding request primitive on the controller. The request is transferred to
/// the target. The behavior of the read and write functions in the RIB functional block on the target depends on the type of RIB and
/// is out of the scope of this API.
///
/// After accessing (reading or writing), the target transmits the result back to the controller.  On the controller, the corresponding
/// confirm primitive is triggered.
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
/// @defgroup GENERAL General
///
/// It's good practice not to call a request function from a confirm funcion. Instead, update the application state
/// so that the request is called the next time the application state machines is triggered. Nested calls of any request
///  function in the confirm callback function should be avoided because:
/// - the confirm callback is triggered from the interrupt service routine
/// - deep call trees are created which are not supported on all embedded processors
/// - the RF4CE RIB functional block does not queue requests



/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include "gpRf4ce.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/// The maximum length of a RIB element (in bytes).
#define GP_RF4CE_RIB_MAX_RIB_ELEMENT_SIZE   16
/// The maximum size in octets of the elements of the attributes in the RIB. At the same time, the maximum size in octets of the Value field in the set attribute request and get attribute response command frames.
#define GP_RF4CE_RIB_MAX_RIB_ATTRIBUTE_SIZE               89

/// The time (in ms) a device, shall wait after the successful transmission of a request frame, before enabling the receiver to receive a response frame.
#define GP_RF4CE_RIB_RESPONSE_IDLE_TIME                   25

/// The time (in ms) a device shall wait after the GP_RF4CE_RIB_RESPONSE_IDLE_TIME expired, to receive a response frame following a request frame.
#define GP_RF4CE_RIB_RESPONSE_WAIT_TIME                   125

#ifndef GP_RF4CE_RIB_VENDOR_ID
#ifndef GP_RF4CE_NWKC_VENDOR_IDENTIFIER
#define GP_RF4CE_RIB_VENDOR_ID gpRf4ce_VendorIdGreenpeak
#else
#define GP_RF4CE_RIB_VENDOR_ID GP_RF4CE_NWKC_VENDOR_IDENTIFIER
#endif
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/// @name gpRf4ceRib_CommandId_t
//@{
#define gpRf4ceRib_CommandIdSetRIBAttributeRequest         0x22
#define gpRf4ceRib_CommandIdSetRIBAttributeResponse        0x23
#define gpRf4ceRib_CommandIdGetRIBAttributeRequest         0x24
#define gpRf4ceRib_CommandIdGetRIBAttributeResponse        0x25
/// @typedef gpRf4ceRib_CommandId_t
/// @brief The gpRf4ceRib_CommandId_t type defines the command identifiers.
typedef UInt8 gpRf4ceRib_CommandId_t;
//@}

/// @name gpRf4ceRib_RibAttributeId_t
//@{
/// @brief The peripheral Ids RIB attribute.
#define gpRf4ceRib_RIBAttributeIdPeripheralIds          0x00
/// @brief The RF statistics peripheral Ids RIB attribute.
#define gpRf4ceRib_RIBAttributeIdRFStatistics           0x01
/// @brief The Versioning RIB attribute.
#define gpRf4ceRib_RIBAttributeIdVersioning             0x02
/// @brief The Battery status RIB attribute.
#define gpRf4ceRib_RIBAttributeIdBatteryStatus          0x03
/// @brief The maximum time in us a unicast acknowledged multichannel transmission shall be retried.
#define gpRf4ceRib_RIBAttributeIdRfRetriesPeriod        0x04

// gpRf4ceMsoVoice profile attributes
/// @brief The status from VREX of the last voice command
#define gpRf4ceRib_RIBAttributeIdVoiceCommandStatus                 0x10
/// @brief The number of samples included in each voice Data Command
#define gpRf4ceRib_RIBAttributeIdVoiceCommandLength                 0x11
/// @brief The length of time (in ms) that a remote is allowed to send audio data
#define gpRf4ceRib_RIBAttributeIdMaxVoiceUtterLength                0x12
/// @brief Determines if the voice data is encrypted over the rf4ce link
#define gpRf4ceRib_RIBAttributeIdVoiceCommandEncryption             0x13
/// @brief The number of times a single frame will be retransmitted
#define gpRf4ceRib_RIBAttributeIdMaxVoiceDataRetransmissionAttemps   0x14
/// @brief The backoffs used for macMaxCSMABackoffs and nwkMaxFirstAttemptCSMABackoffs for voice data
#define gpRf4ceRib_RIBAttributeIdMaxVoiceDataCsmaBackoffs           0x15
/// @brief The backoff exponent used for macMinBackoffExponent for voice data.
#define gpRf4ceRib_RIBAttributeIdMinVoiceDataBackoffExp             0x16
/// @brief A bitmask of the voice controller’s supported profiles
#define gpRf4ceRib_RIBAttributeIdVoiceControllerAudioProfiles       0x17
/// @brief A bitmask of the compatible audio profiles between the voice target and voice controller
#define gpRf4ceRib_RIBAttributeIdCompatibleVoiceTargetAudioProfile  0x18
/// @brief The number of voice activations, and the length of time voice data is sent
#define gpRf4ceRib_RIBAttributeIdVoiceStatistics                    0x19
/// @brief Informes the voice controller if other RIB entries were updated
#define gpRf4ceRib_RIBAttributeIdRibEntriesUpdated                  0x1a
/// @brief The time, in hours, a voice controller polls the voice target for "RIB Entries Updated"
#define gpRf4ceRib_RIBAttributeIdRibUpdateCheckInterval             0x1b

// gpRf4ceMsoOta profile attributes
/// @brief Array of versions related to updating images and data on the device.
#define gpRf4ceRib_RIBAttributeIdUpdateVersioning                   0x31
/// @brief The 9 bytes used in the MSO Profile User String sent in the Discovery Request that initiated the binding.
#define gpRf4ceRib_RIBAttributeIdProductName                        0x32
/// @brief Specifies the rate at which an image should be downloaded to the controller.
#define gpRf4ceRib_RIBAttributeIdDownloadRate                       0x33
/// @brief The time, in hours, a controller MUST poll the target for an available image update for each supported image type using the Check For Image Request
#define gpRf4ceRib_RIBAttributeIdUpdatePollingPeriod                0x34
/// @brief The minimum time, in milliseconds, the controller MUST wait before attempting a new Image Data Request after the last user activity.
#define gpRf4ceRib_RIBAttributeIdDataRequestWaitTime                0x35

/// @brief The wait time configuration attributes.
#define gpRf4ceRib_RIBAttributeIdValidationProperties   0xDC
/// @brief The general purpose RIB attribute.
#define gpRf4ceRib_RIBAttributeIdGeneralPurpose         0xFF
/// @typedef gpRf4ceRib_RibAttributeId_t
/// @brief The gpRf4ceRib_RibAttributeId_t type defines the RIB attribute type.
typedef UInt8 gpRf4ceRib_RibAttributeId_t;
//@}

/// @name gpRf4ceRib_RIBVersioningIndex_t
//@{
/// @brief The versioning index.
/// @brief The Software Versioning Index
#define gpRf4ceRib_RIBVersioningIndexSw          0x00
/// @brief The Hardware Versioning Index
#define gpRf4ceRib_RIBVersioningIndexHw          0x01
/// @brief The IR-db Versioning Index
#define gpRf4ceRib_RIBVersioningIndexIrdb        0x02
/// @typedef gpRf4ceRib_RIBVersioningIndex_t
/// @brief The gpRf4ceRib_RIBVersioningIndex_t type defines the versioning attribute index.
typedef UInt8 gpRf4ceRib_RIBVersioningIndex_t;
//@}

/// @name gpRf4ceRib_Result_t
//@{
#define gpRf4ceRib_ResultRequestSuccess                 0x00
#define gpRf4ceRib_ResultRequestUnmatched               0x01
// values from Rf4ce
#define gpRf4ceRib_ResultRequestUnsupportedAttribute    gpRf4ce_ResultUnsupportedAttribute
#define gpRf4ceRib_ResultRequestInvalidParameter        gpRf4ce_ResultInvalidParameter
#define gpRf4ceRib_ResultRequestInvalidIndex            gpRf4ce_ResultInvalidIndex
/// @typedef gpRf4ceRib_Result_t
/// @brief The gpRf4ceRib_Result_t type defines the result for the rib callbacks.
typedef UInt8 gpRf4ceRib_Result_t;

typedef gpRf4ceRib_Result_t (* gpRf4ceRib_cbSetRibCommStatusIndication_t) (gpRf4ce_Result_t status, UInt8 bindingRef, UInt8 profileId, gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex);

typedef gpRf4ceRib_Result_t (* gpRf4ceRib_cbGetRibCommStatusIndication_t) (gpRf4ce_Result_t status, UInt8 bindingRef, UInt8 profileId, gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex);

/// @ingroup RIB_ATTRIBUTE
/// This primitive indicates an incoming request to set a RIB attribute value.
/// @param bindingRef           The number of the binding reference. In RF4CE, this is the pairing ref.
/// @param ribAttribute         The identifier of the RIB attribute that was sent.
/// @param ribAttributeIndex    The index within the table or array of the specified RIB attribute to write. This parameter is valid only for RIB attributes that are tables or arrays.
/// @param ribAttributeLength   The length of the data buffer.
/// @param pRibData             A pointer to a buffer containing the attribute data.
/// @param local                Indicates this request is made locally or from the peer device
/// @return                     Returns the result.
/// @remark This primitive is only used for targets.
typedef gpRf4ceRib_Result_t (* gpRf4ceRib_cbSetRibIndication_t) (UInt8 bindingRef, gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8 ribAttributeLength, UInt8* pRibData, Bool local);

/// @ingroup RIB_ATTRIBUTE
/// This primitive indicates an incoming request to get a RIB attribute value.
/// @param bindingRef           The number of the binding reference. In RF4CE, this is the pairing ref.
/// @param ribAttribute         The identifier of the RIB attribute to read.
/// @param ribAttributeIndex    The index within the table or array of the specified RIB attribute to read. This parameter is valid only for RIB attributes that are tables or arrays.
/// @param ribAttributeLength   The length of the data buffer.
/// @param pRibData             A pointer to a buffer containing the attribute data.
/// @param local                Indicates this request is made locally or from the peer device
/// @remark This primitive is only used for controllers.
typedef gpRf4ceRib_Result_t (* gpRf4ceRib_cbGetRibIndication_t)( UInt8 bindingRef , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8* ribAttributeLength , UInt8* pRibData, Bool local);

typedef struct {
    gpRf4ceRib_cbGetRibIndication_t             getCb;
    gpRf4ceRib_cbSetRibIndication_t             setCb;
    gpRf4ceRib_cbGetRibCommStatusIndication_t   getCommStatusCb;
    gpRf4ceRib_cbSetRibCommStatusIndication_t   setCommStatusCb;
} gpRf4ceRib_RibCallbacks_t;

/*****************************************************************************
 *                    Public Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

GP_API void gpRf4ceRib_Init(void);

/// @ingroup RIB_ATTRIBUTE
/// This primitive reports the results of the transmission of the request to get the RIB attribute value.
/// @param status               The status of the get RIB request
/// @param bindingRef           The number of the binding reference. In RF4CE, this is the pairing ref.
/// @param ribAttribute         The identifier of the RIB attribute to read.
/// @param ribAttributeIndex    The index within the table or array of the specified RIB attribute to read. This parameter is valid only for RIB attributes that are tables or arrays.
/// @param ribAttributeLength   The length of the data buffer.
/// @param pRibData             A pointer to a buffer containing the attribute data to be writen.
///                             The buffer needs to be allocated static.
/// @remark This primitive is only used for controllers.
typedef void (* gpRf4ceRib_cbGetRibConfirm_t) (gpRf4ce_Result_t status , UInt8 bindingRef , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8 ribAttributeLength, UInt8* pRibData);


/// @ingroup RIB_ATTRIBUTE
/// This primitive requests to get a RIB attribute value.
/// @param bindingRef           The number of the binding reference. In RF4CE, this is the pairing ref.
/// @param profileID            The profile that uses this GetRibRequest.
/// @param vendorID             The vendorID used to tranmsit the packet.
/// @param ribAttribute         The identifier of the RIB attribute to read.
/// @param ribAttributeIndex    The index within the table or array of the specified RIB attribute to read. This parameter is valid only for RIB attributes that are tables or arrays.
/// @param ribAttributeLength   The length of the data buffer.
/// @param pRibData             A pointer fhe a buffer in which the attribute data should be writen.
///                             The buffer needs to be allocated static.
/// @param rfRetriesPeriod      RfRetries period used, in µs.
/// @param  cbGetRibConfirm     The function that will be called to return the confirm.
/// @remark This primitive is only used for controllers.
GP_API void gpRf4ceRib_GetRibRequest( UInt8 bindingRef , gpRf4ce_ProfileId_t profileID , gpRf4ce_VendorId_t vendorID , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8 ribAttributeLength , UInt8* pRibData  , UInt32 rfRetriesPeriod , gpRf4ceRib_cbGetRibConfirm_t cbGetRibConfirm );

// get local rib attribute
GP_API gpRf4ceRib_Result_t        gpRf4ceRib_GetRibLocal(UInt8 pairingRef, gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8 ribAttributeLength, UInt8* pRibData);
/// @ingroup RIB_ATTRIBUTE
/// This primitive reports the results of the transmission of the request to set the RIB attribute value.
/// @param status               The status of the set RIB request
/// @param bindingRef           The number of the binding reference. In RF4CE, this is the pairing ref.
/// @param ribAttribute        The identifier of the RIB attribute to write.
/// @param ribAttributeIndex   The index within the table or array of the specified RIB attribute to write. This parameter is valid only for RIB attributes that are tables or arrays.
/// @remark This primitive is only used for controllers.
typedef void (* gpRf4ceRib_cbSetRibConfirm_t) (gpRf4ce_Result_t status , UInt8 bindingRef , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex);

/// @ingroup RIB_ATTRIBUTE
/// This primitive requests to set a RIB attribute value.
/// @param bindingRef           The number of the binding reference. In RF4CE, this is the pairing ref.
/// @param profileID            The profile that uses this GetRibRequest.
/// @param vendorID             The vendorID used to tranmsit the packet.
/// @param ribAttribute         The identifier of the RIB attribute to write.
/// @param ribAttributeIndex    The index within the table or array of the specified RIB attribute to write. This parameter is valid only for RIB attributes that are tables or arrays.
/// @param ribAttributeLength   The length of the data buffer.
/// @param pRibData             A pointer to a buffer containing the attribute data.
///                             The pointer was originally passed as argument fo gpRf4ceRib_GetRibRequest().
/// @param rfRetriesPeriod      RfRetries period used, in µs.
/// @param  cbSetRibConfirm     The function that will be called to return the confirm.
/// @remark This primitive is only used for controllers.
GP_API void gpRf4ceRib_SetRibRequest( UInt8 bindingRef , gpRf4ce_ProfileId_t profileID , gpRf4ce_VendorId_t vendorID , gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8 ribAttributeLength , UInt8* pRibData , UInt32 rfRetriesPeriod , gpRf4ceRib_cbSetRibConfirm_t cbSetRibConfirm );

// set local rib attribute
GP_API gpRf4ceRib_Result_t gpRf4ceRib_SetRibLocal(UInt8 bindingRef, gpRf4ceRib_RibAttributeId_t ribAttribute, UInt8 ribAttributeIndex, UInt8 ribAttributeLength, UInt8* pRibData);

GP_API void gpRf4ceRib_RegisterRibCallbacks(gpRf4ceRib_RibCallbacks_t* pCallbacks);

#ifdef __cplusplus
}
#endif

#endif //_GP_RF4CE_USER_CONTROL_H_


