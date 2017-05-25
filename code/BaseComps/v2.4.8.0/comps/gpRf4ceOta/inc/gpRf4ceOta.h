/*
 *   Copyright (c) 2016, GreenPeak Technologies
 *
 *   RF4CE OTA Profile
 *   Declarations of the public functions and enumerations of gpRf4ceOta.
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/gpRf4ceOta/inc/gpRf4ceOta.h#25 $
 *    M'   GreenPeak              $Change: 79863 $
 *   0'         Technologies      $DateTime: 2016/04/27 16:29:58 $
 *  F
 */


#ifndef _GPRF4CEOTA_H_
#define _GPRF4CEOTA_H_

/// @file "gpRf4ceOta.h"
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpRf4ce.h"
#include "gpPd.h"
#include "gpMacCore.h"
#include "gpEncryption.h"
#include "gpRf4ceCmd.h"




/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpRf4ceOta_ImageDownloadNotificationReason_t */
//@{
#define gpRf4ceOta_ImageDownloadNotificationReason_ClientNotification     0x00
#define gpRf4ceOta_ImageDownloadNotificationReason_PollTimer     0x01
typedef UInt8                             gpRf4ceOta_ImageDownloadNotificationReason_t;
//@}

/** @enum gpRf4ceOta_ImageUpgradeNotificationReason_t */
//@{
#define gpRf4ceOta_ImageUpgradeNotificationReason_ClientNotification     0x00
#define gpRf4ceOta_ImageUpgradeNotificationReason_PollTimer     0x01
typedef UInt8                             gpRf4ceOta_ImageUpgradeNotificationReason_t;
//@}

/** @enum gpRf4ceOta_Status_t */
//@{
/** @brief An image is available */
#define gpRf4ceOta_Status_ImageAvailable                       0x01
/** @brief No image is available. */
#define gpRf4ceOta_Status_NoImageAvailable                     0x02
/** @brief The image fragment contains valid data */
#define gpRf4ceOta_Status_ValidData                            0x03
/** @brief The requested image fragment could not be served yet. The OTA Client should retry, by retransmitting the Image Download Block Request frame at a later moment in time. */
#define gpRf4ceOta_Status_WaitForData                          0x04
/** @brief The Download procedure is aborted. */
#define gpRf4ceOta_Status_AbortDownload                        0x05
/** @brief Download and verification completed successfully. The image is Pending. */
#define gpRf4ceOta_Status_DownloadedAndVerified                0x06
/** @brief The image is rejected. */
#define gpRf4ceOta_Status_Ignore_Image_Download_Forever        0x07
/** @brief The image download is deferred to a later moment in time. */
#define gpRf4ceOta_Status_DeferImageDownload                   0x08
/** @brief The verification method is not supported by the OTA client. */
#define gpRf4ceOta_Status_VerificationMethodNotSupported       0x09
/** @brief The Download procedure is aborted, since the verification failed. */
#define gpRf4ceOta_Status_ChecksumFailure                      0x0A
/** @brief The OTA Client shall apply the Pending Image as soon as possible. */
#define gpRf4ceOta_Status_UpgradeNow                           0x0B
/** @brief The OTA Client is granted the permission to decide on its own behalf when the Pending Image is to be applied. */
#define gpRf4ceOta_Status_UpgradeWhenQuiet                     0x0C
/** @brief The OTA Client shall drop the Pending Image. The image is no longer Pending. */
#define gpRf4ceOta_Status_DropImage                            0x0D
/** @brief The OTA Client shall request the Image Upgrade at a later moment in time. The image is still Pending. */
#define gpRf4ceOta_Status_WaitToUpgrade                        0x0E
/** @brief The OTA Client has successfully applied the Pending Image. */
#define gpRf4ceOta_Status_Upgraded                             0x0F
/** @brief The Pending Image could not be applied. The image is no longer Pending. */
#define gpRf4ceOta_Status_UpgradeFailure                       0x10
/** @brief The image upgrade is rejected. The image is no longer Pending. */
#define gpRf4ceOta_Status_IgnoreImageUpgradeForever            0x11
/** @brief The local application of the OTA Client indicates to defer the image upgrade to a later moment in time. The image is still Pending. */
#define gpRf4ceOta_Status_DeferImageUpgrade                    0x12
typedef gpRf4ce_Result_t                  gpRf4ceOta_Status_t;
//@}

/** @enum gpRf4ceOta_CmdAttribute_t */
//@{
/** @brief Version of the OTA profile used. */
#define gpRf4ceOta_AttributeAplOtaProfileVersion               0xA0
/** @brief Bitmap (256bits) that indicates what checksum types are supported */
#define gpRf4ceOta_AttributeAplSupportedChecksums              0xA1
/** @brief UInt16 period of the image download poll timer expressed in minutes, 0 means disabled */
#define gpRf4ceOta_AttributeAplImageDownloadPollPeriod         0xA2
/** @brief UInt16 period of the image upgrade poll timer expressed in seconds, 0 means disabled */
#define gpRf4ceOta_AttributeAplImageUpgradePollPeriod          0xA3
/** @brief UInt16 minimum period of the image block timer, expresesed in 10ms units */
#define gpRf4ceOta_AttributeAplMinimumBlockPeriod              0xA4
/** @brief UInt16 preferred period of the image block timer, expresesed in 10ms units */
#define gpRf4ceOta_AttributeAplPreferedBlockPeriod             0xA5
/** @typedef gpRf4ceOta_CmdAttribute_t
    @brief GDP AttributeId list for gpRf4ceOta
*/
typedef UInt8                             gpRf4ceOta_CmdAttribute_t;
//@}

/** @enum gpRf4ceOta_ImageType_t */
//@{
/** @brief The image is a main firmware image. */
#define gpRf4ceOta_ImageTypeMain                               0x00
/** @brief Bitmap (256bits) that indicates what checksum types are supported */
#define gpRf4ceOta_ImageTypeAudio                              0x01
/** @typedef gpRf4ceOta_ImageType_t
    @brief Image type list for gpRf4ceOta
*/
typedef UInt8                             gpRf4ceOta_ImageType_t;
//@}

/** @enum gpRf4ceOta_ClientNotification_t */
//@{
#define gpRf4ceOta_ClientNotification_ImageDownload            0x40
#define gpRf4ceOta_ClientNotification_Upgrade                  0x41
typedef UInt8                             gpRf4ceOta_ClientNotification_t;
//@}

/** @enum gpRf4ceOta_ChecksumType_t */
//@{
#define gpRf4ceOta_ChecksumType_Crc                            0x00
#define gpRf4ceOta_ChecksumType_Reserved1                      0x01
typedef UInt8                             gpRf4ceOta_ChecksumType_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/** @macro GP_RF4CE_OTA_APLOTAPROFILEVERSION */
#define GP_RF4CE_OTA_APLOTAPROFILEVERSION            0x100
/** @macro GP_RF4CE_OTA_APLSUPPORTEDCHECKSUMS */
#define GP_RF4CE_OTA_APLSUPPORTEDCHECKSUMS           1
/** @macro GP_RF4CE_OTA_APLCMAXRESPONSEWAITTIME */
#define GP_RF4CE_OTA_APLCMAXRESPONSEWAITTIME         100
/** @macro GP_RF4CE_OTA_APLCOTAPROFILEMAXIMAGEFRAGMENTLENGTH */
#define GP_RF4CE_OTA_APLCOTAPROFILEMAXIMAGEFRAGMENTLENGTH     85
/** @macro GP_RF4CE_OTA_APLIMAGEDOWNLOADPOLLPERIOD_DEFAULT */
#define GP_RF4CE_OTA_APLIMAGEDOWNLOADPOLLPERIOD_DEFAULT     1440
/** @macro GP_RF4CE_OTA_APLIMAGEUPGRADEPOLLPERIOD_DEFAULT */
#define GP_RF4CE_OTA_APLIMAGEUPGRADEPOLLPERIOD_DEFAULT     900
/** @macro GP_RF4CE_OTA_APLMINIMUMBLOCKPERIOD_DEFAULT */
#define GP_RF4CE_OTA_APLMINIMUMBLOCKPERIOD_DEFAULT     0
/** @macro GP_RF4CE_OTA_APLPREFERREDBLOCKPERIOD_DEFAULT */
#define GP_RF4CE_OTA_APLPREFERREDBLOCKPERIOD_DEFAULT     0
/** @macro GP_RF4CE_OTA_IMAGEVERSION_NO_IMAGE_PENDING */
#define GP_RF4CE_OTA_IMAGEVERSION_NO_IMAGE_PENDING     0xFFFFFF
/** @macro GP_RF4CE_OTA_IMAGETYPEBITMAP_LENGTH */
#define GP_RF4CE_OTA_IMAGETYPEBITMAP_LENGTH          32

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

//Requests
void gpRf4ceOta_Init(void);
void gpRf4ceOta_DeInit(void);
Bool gpRf4ceOta_cbGdpClientNotification(UInt8 pairingRef, UInt8 rxFlags, UInt8 nsduLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);
Bool gpRf4ceOta_cbGdpHeartBeatNotifyIndication(UInt8 bindingId, UInt8 heartBeatTrigger);
void gpRf4ceOta_SetPreferredBlockPeriod(UInt16 Period);
UInt16 gpRf4ceOta_GetPreferredBlockPeriod(void);
void gpRf4ceOta_ConfigurationStartInitiator(UInt8 bindingId);
void gpRf4ceOta_SetImageDownloadPollPeriod(UInt16 ImageDownloadPollPeriod);
UInt16 gpRf4ceOta_GetImageDownloadPollPeriod(void);
void gpRf4ceOta_SetImageUpgradePollPeriod(UInt16 ImageUpgradePollPeriod);
UInt16 gpRf4ceOta_GetImageUpgradePollPeriod(void);
void gpRf4ceOta_ImageDownloadStartRequest(UInt8 pairingRef, UInt8 ImageType, UInt32 CurrentImageVersion, UInt32 PendingImageVersion, UInt32 HardwareVersion);
void gpRf4ceOta_ImageDownloadBlockRequest(UInt8 pairingRef, UInt8 ImageType, UInt32 NewImageVersion, UInt32 ImageFragmentOffset, UInt8 ImageFragmentLength);
void gpRf4ceOta_ImageDownloadCompleteRequest(UInt8 pairingRef, gpRf4ceOta_Status_t Status, UInt8 ImageType, UInt32 NewImageVersion);
void gpRf4ceOta_SetPendingImages(UInt8 pairingRef, Bool Enable);
void gpRf4ceOta_ImageUpgradeStartRequest(UInt8 pairingRef, UInt8 ImageType, UInt32 CurrentImageVersion, UInt32 PendingImageVersion, UInt32 HardwareVersion);
void gpRf4ceOta_ImageUpgradeCompleteRequest(UInt8 pairingRef, gpRf4ceOta_Status_t Status, UInt8 ImageType, UInt32 PendingImageVersion);

//Indications
void gpRf4ceOta_cbInitiatorConfigurationConfirm(UInt8 bindingId, gpRf4ceCmd_ResponseCode_t result);
void gpRf4ceOta_cbImageDownloadNotifyIndication(UInt8 pairingRef, gpRf4ceOta_ImageDownloadNotificationReason_t Reason, UInt8* ImageTypeBitmap);
void gpRf4ceOta_cbImageDownloadStartConfirm(UInt8 pairingRef, gpRf4ceOta_Status_t Status, UInt8 ImageType, UInt32 NewImageVersion, UInt32 ImageSize, UInt8 ChecksumType, UInt32 Checksum);
void gpRf4ceOta_cbImageDownloadBlockConfirm(UInt8 pairingRef, gpRf4ceOta_Status_t Status, UInt8 ImageType, UInt32 NewImageVersion, UInt16 MinimumBlockPeriod, UInt32 ImageFragmentOffset, UInt8 ImageFragmentLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle);
void gpRf4ceOta_cbImageDownloadReadyForNextBlockIndication(UInt8 pairingRef);
void gpRf4ceOta_cbImageDownloadCompleteConfirm(UInt8 pairingRef, gpRf4ce_Result_t Status, UInt8 ImageType, UInt32 NewImageVersion);
void gpRf4ceOta_cbImageUpgradeNotifyIndication(UInt8 pairingRef, gpRf4ceOta_ImageUpgradeNotificationReason_t Reason, UInt8* ImageTypeBitmap);
void gpRf4ceOta_cbImageUpgradeStartConfirm(UInt8 pairingRef, UInt8 Status, UInt8 ImageType, UInt32 NewImageVersion);
void gpRf4ceOta_cbImageUpgradeCompleteConfirm(UInt8 pairingRef, gpRf4ce_Result_t Status, UInt8 ImageType, UInt32 PendingImageVersion);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPRF4CEOTA_H_

