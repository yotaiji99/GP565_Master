/*
 *   Copyright (c) 2015, GreenPeak Technologies
 *
 *   Rf4ce Voice Profile
 *   Declarations of the public functions and enumerations of gpRf4ceVoice.
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpRf4ceVoice/inc/gpRf4ceVoice.h#16 $
 *    M'   GreenPeak              $Change: 74459 $
 *   0'         Technologies      $DateTime: 2015/12/09 17:44:24 $
 *  F
 */


#ifndef _GPRF4CEVOICE_H_
#define _GPRF4CEVOICE_H_

/// @file "gpRf4ceVoice.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpRf4ce.h"
#include "gpPd.h"
#include "gpMacCore.h"
#include "gpEncryption.h"



/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpRf4ceVoice_CmdAttribute_t */
//@{
/** @brief Version of Voice profile used. */
#define gpRf4ceVoice_CmdAttributeVoiceProfileVersion           0xA0
/** @brief Recipient status concerning the processing of the last voice command. Matches with gpRf4ceCmd_AttributeBurst_aplBurstBackendStatus. */
#define gpRf4ceVoice_CmdAttributeVoiceCommandStatus            0xA5
/** @brief Voice fragments need to be encrypted. Matches with gpRf4ceCmd_AttributeBurst_aplBurstEncrypted. */
#define gpRf4ceVoice_CmdAttributeVoiceCommandEncryption        0xA6
/** @brief Maximum voice session length in seconds. */
#define gpRf4ceVoice_CmdAttributeVoiceMaxSessionLength         0xE0
/** @brief Supported audio profiles by originator. */
#define gpRf4ceVoice_CmdAttributeVoiceOriginatorAudioProfiles     0xE1
/** @brief Compatible audio profiles of recipient. */
#define gpRf4ceVoice_CmdAttributeCompatibleVoiceRecipientAudioProfile     0xE2
/** @brief Voice statistics kept by originator. */
#define gpRf4ceVoice_CmdAttributeVoiceStatistics               0xE3
/** @brief Period, in seconds, which originator needs to check all voice attributes. */
#define gpRf4ceVoice_CmdAttributeAttributesUpdatedCheckPeriod     0xE4
/** @brief Set if any of the voice attributes where updated by the recipient. */
#define gpRf4ceVoice_CmdAttributeAttributesUpdated             0xE5
/** @typedef gpRf4ceVoice_CmdAttribute_t
    @brief GDP AttributeId list for Voice - range 0xE0 - 0xFF available for derived profile
*/
typedef UInt8                             gpRf4ceVoice_CmdAttribute_t;
//@}

/** @enum gpRf4ceVoice_SessionStatus_t */
//@{
/** @brief Recipient can process a voice command. */
#define gpRf4ceVoice_SessionStatusAvailable                    0x00
/** @brief Recipient encountered a failure during setup of the voice processing. */
#define gpRf4ceVoice_SessionStatusFailure                      0x01
/** @brief Recipient is already processing voice commands. */
#define gpRf4ceVoice_SessionStatusBusy                         0x80
/** @brief The processing backend at the recipient side is not ready to process voice commands. */
#define gpRf4ceVoice_SessionStatusVoiceServerNotReady          0x81
/** @brief Audio format of the originator is not supported by the recipient */
#define gpRf4ceVoice_SessionStatusUnsupportedAudioFormat       0x82
/** @typedef gpRf4ceVoice_SessionStatus_t
    @brief Status reported when session is set-up before sending any fragments.
*/
typedef UInt8                             gpRf4ceVoice_SessionStatus_t;
//@}

/** @enum gpRf4ceVoice_CommandStatus_t */
//@{
/** @brief Command given to backend for processing. Awaiting result. */
#define gpRf4ceVoice_CommandStatusPending                      0x00
/** @brief Processing timed out. */
#define gpRf4ceVoice_CommandStatusTimeout                      0x01
/** @brief Processing backend was offline. */
#define gpRf4ceVoice_CommandStatusOffline                      0x02
/** @brief Processing command succeeded. */
#define gpRf4ceVoice_CommandStatusSuccess                      0x03
/** @brief Processing failed. */
#define gpRf4ceVoice_CommandStatusFail                         0x04
/** @brief No processing ongoing. */
#define gpRf4ceVoice_CommandStatusNoCommandsSent               0x05
/** @typedef gpRf4ceVoice_CommandStatus_t
    @brief Voice command status. Set by the recipient, polled by the originator once the voice transmission has stopped.
*/
typedef UInt8                             gpRf4ceVoice_CommandStatus_t;
//@}

/** @enum gpRf4ceVoice_TimeoutReason_t */
//@{
/** @brief Next ADPCM fragment not received/sent in time. */
#define gpRf4ceVoice_TimeoutReasonADPCMFeed                    0x00
/** @brief Maximum session time reached. */
#define gpRf4ceVoice_TimeoutReasonMaxLengthReached             0x01
/** @typedef gpRf4ceVoice_TimeoutReason_t
    @brief Reason why voice session timed out.
*/
typedef UInt8                             gpRf4ceVoice_TimeoutReason_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_RF4CEVOICE_VOICE_CMD_LENGTH_MAX_SECURED */
/** @brief Default amount of voice sample data sent in bytes. */
#define GP_RF4CEVOICE_VOICE_CMD_LENGTH_MAX_SECURED   88
#define GP_RF4CEVOICE_VOICE_CMD_LENGTH_MAX_UNSECURED 92

/** @macro GP_RF4CEVOICE_MAX_VOICE_LENGTH_DEFAULT */
/** @brief Maximum voice session length in seconds. */
#define GP_RF4CEVOICE_MAX_VOICE_LENGTH_DEFAULT       10
/** @macro GP_RF4CEVOICE_AUDIO_PROFILE_IDX_ADPCM */
/** @brief Audio profile bitmask - ADPCM profile bit index. */
#define GP_RF4CEVOICE_AUDIO_PROFILE_IDX_ADPCM        1

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @struct gpRf4ceVoice_ADPCMData_t
 *  @brief Structure describing the ADPCM sample data
*/
typedef struct {
    /** @brief StepSize index for the ADPCM decompressor */
    UInt8                          stepSizeIndex;
    /** @brief Predicted sample LSB for the ADPCM decompressor */
    UInt8                          predictedSampleLsb;
    /** @brief Predicted sample MSB for the ADPCM decompressor */
    UInt8                          predictedSampleMsb;
    /** @brief Number of 4-bit ADPCM samples sent/received */
    UInt8                          numSamples;
} gpRf4ceVoice_ADPCMData_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
/** @brief Initialize the gpRf4ceVoice component.
*/
void gpRf4ceVoice_Init(void);
/** @brief Request to start a voice session.

*   @param pairingRef                Pairing reference to start a voice session with
*/
void gpRf4ceVoice_SessionRequest(UInt8 pairingRef);
/** @brief Set the voice server ready status. This status must be set to true before incoming Voice Session Request are accepted.

*   @param ready                     Voice processing is ready to accept voice commands.
*/
void gpRf4ceVoice_SetServerReady(Bool ready);
/** @brief Function to send a ADPCM fragment during an established Voice Session

*   @param pAdpcmData                Description of the ADPCM data chunk to be sent.
*   @param pdLoh                     gpPd_Loh_t structure for the ADPCM data. 4-bit samples are written to the Pd in bytes following a [sample_N+1 | sample_N] format per byte.
*   @param pairingRef                Pairing reference where voice session was started with.
*/
void gpRf4ceVoice_ADPCMDataRequest(gpRf4ceVoice_ADPCMData_t* pAdpcmData, gpPd_Loh_t pdLoh, UInt8 pairingRef);
/** @brief Stop a running voice session.

*   @param pairingRef                Pairing reference where voice session was started with.
*/
void gpRf4ceVoice_SessionStopRequest(UInt8 pairingRef);
/** @brief Set the maximum voice session length. Not updated during a running session.

*   @param maxSessionLength          Session length in seconds.
*   @param pairingRef                Pairing reference for which the setting will be used.
*/
void gpRf4ceVoice_SetMaxSessionLength(UInt16 maxSessionLength, UInt8 pairingRef);
/** @brief Set the maximum voice session length.

*   @param pairingRef                Pairing reference for which the setting is used.
*   @return maxSessionLength         Session length in seconds.
*/
UInt16 gpRf4ceVoice_GetMaxSessionLength(UInt8 pairingRef);

Bool gpRf4ceVoice_GetCommandEncryption(UInt8 pairingRef);
void gpRf4ceVoice_SetCommandEncryption(Bool encrypted, UInt8 pairingRef);

/** @brief Set the processing status of the last received voice command. Will be polled by the originator.

*   @param pairingRef                Pairing reference for which the state is to be set.
*   @param cmdStatus                 Processing status of the last received voice command.
*/
void gpRf4ceVoice_SetVoiceCmdStatus(UInt8 pairingRef, gpRf4ceVoice_CommandStatus_t cmdStatus);

//Indications
/** @brief Confirm that the voice session is started. Session and ADPCM feed timeout are started when status is gpRf4ceVoice_SessionStatusAvailable.

*   @param status                    Status of the voice session request returned by the recipient or internally generated
*   @param pairingRef                Pairing reference voice session is attempted
*/
void gpRf4ceVoice_cbSessionConfirm(gpRf4ceVoice_SessionStatus_t status, UInt8 pairingRef);
/** @brief Indication given when a voice session is started

*   @param pairingRef                Pairing reference starting the voice session
*/
void gpRf4ceVoice_cbSessionNotifyIndication(UInt8 pairingRef);
/** @brief Confirm for a sent ADPCM chunk.

*   @param status                    Data request status
*   @param pdHandle                  Handle to data transmitted
*   @param pairingRef                Pairing reference where data was transmitted to.
*/
void gpRf4ceVoice_cbADPCMDataConfirm(gpRf4ce_Result_t status, gpPd_Handle_t pdHandle, UInt8 pairingRef);
/** @brief Indication of ADPCM data of a running voice session

*   @param seqNr                     seq number of ADPCM data chunk
*   @param adpcmData                 Description of the ADPCM data chunk received.
*   @param pdLoh                     gpPd_Loh_t structure for the ADPCM data
*   @param pairingRef                Pairing reference where voice session was started with.
*/
void gpRf4ceVoice_cbADPCMDataIndication(UInt8 seqNr, gpRf4ceVoice_ADPCMData_t* adpcmData, gpPd_Loh_t pdLoh, UInt8 pairingRef);
/** @brief Timeout indication for when next ADPCM chunk was not sent/received in time. Session will be stopped.

*   @param pairingRef                Pairing reference where voice session was started with.
*   @param reason                    Reason why timeout was triggered.
*/
void gpRf4ceVoice_cbSessionTimeoutIndication(UInt8 pairingRef, gpRf4ceVoice_TimeoutReason_t reason);
/** @brief Indication originator stopped the voice session.

*   @param pairingRef                Pairing reference where voice session was started with.
*/
void gpRf4ceVoice_cbSessionStopIndication(UInt8 pairingRef);
/** @brief Confirmation of the stopped voice Session

*   @param voiceCmdStatus            Status of the processed voice fragment from the recipient.
*   @param pairingRef                Pairing reference for the stopped voice session.
*/
void gpRf4ceVoice_cbSessionStopConfirm(gpRf4ceVoice_CommandStatus_t voiceCmdStatus, UInt8 pairingRef);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPRF4CEVOICE_H_

