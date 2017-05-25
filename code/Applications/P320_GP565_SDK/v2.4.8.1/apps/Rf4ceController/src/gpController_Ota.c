/*
 * Copyright (c) 2016, GreenPeak Technologies
 *
 * gpController_Ota.c
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
 *     0!                         $Header: //depot/main/Embedded/Applications/P857_RC_RemoteSolution_Rogers_Eclipse_GP565/vlatest/apps/Rf4ceController/src/gpController_Ota.c#1 $
 *    M'   GreenPeak              $Change: 80002 $
 *   0'         Technologies      $DateTime: 2016/04/30 16:01:19 $
 *  F
 */

//#define GP_LOCAL_LOG

/*******************************************************************************
 *                    Include Files
 ******************************************************************************/
#include "gpAssert.h"
#include "gpLog.h"
#include "gpPoolMem.h"
#include "gpRf4ceOta.h"
#include "gpSched.h"

#include "gpController_Ota.h"

/*******************************************************************************
 *                    Defines
 ******************************************************************************/
#define GP_COMPONENT_ID     GP_COMPONENT_ID_APP

/*******************************************************************************
 *                    Static Data
 ******************************************************************************/
Application_OtaState_t otaState         = Application_OtaState_Idle;
UInt8  currentOtaPairingRef             = 0xFF;
UInt8  currentOtaImageType              = 0xFF;
UInt32 currentOtaImageVersion           = 0xFFFFFFFF;
UInt32 currentOtaImageSize              = 0xFFFFFFFF;
UInt32 currentOtaDownloadFragmentOffset = 0xFFFFFFFF;
UInt8  currentOtaDownloadFragmentLength = 0xFF;
UInt8  currentOtaChecksumType           = 0xFF;
UInt32 currentOtaChecksum               = 0xFFFFFFFF;
UInt32 downloadedOtaImageVersion        = 0xFFFFFFFF;
UInt32 activeImageVersion               = 0xFFFFFFFF;
UInt32 activeHardwareVersion            = 0xFFFFFFFF;
UInt8 *otaImage;

/*******************************************************************************
 *                    Static Function Definitions
 *******************************************************************************/
static void Application_ResetOtaState(void)
{
    otaState = Application_OtaState_Idle;
    currentOtaPairingRef              = 0xFF;
    currentOtaImageType               = 0xFF;
    currentOtaImageVersion            = 0xFFFFFFFF;
    currentOtaImageSize               = 0xFFFFFFFF;
    currentOtaDownloadFragmentOffset  = 0xFFFFFFFF;
    currentOtaDownloadFragmentLength  = 0xFF;
}

/*******************************************************************************
 *                    Public Function Definitions
 *******************************************************************************/
void gpApplication_OtaInit(void)
{
    otaState = Application_OtaState_Idle;
    currentOtaPairingRef              = 0xFF;
    currentOtaImageType               = 0xFF;
    currentOtaImageVersion            = 0xFFFFFFFF;
    currentOtaImageSize               = 0xFFFFFFFF;
    currentOtaDownloadFragmentOffset  = 0xFFFFFFFF;
    currentOtaDownloadFragmentLength  = 0xFF;
    downloadedOtaImageVersion         = 0xFFFFFFFF;
    activeImageVersion                = OTA_DEFAULT_IMAGE_VERSION;
    activeHardwareVersion             = OTA_DEFAULT_HARDWARE_VERSION;
    GP_LOG_PRINTF("OTA Init",0);
}

void gpApplication_OtaImageDownload(void)
{
    UInt8 otaDownloadFragmentLength;
    if(otaState != Application_OtaState_DownloadingImage)
    {
        GP_LOG_SYSTEM_PRINTF("OTA wrong state %x - aborting download",1,otaState);
        Application_ResetOtaState();
        return;
    }
    if(currentOtaDownloadFragmentOffset + currentOtaDownloadFragmentLength < currentOtaImageSize)
    {
        otaDownloadFragmentLength = currentOtaDownloadFragmentLength;
    }
    else if(currentOtaDownloadFragmentOffset < (currentOtaImageSize-1))
    {
        otaDownloadFragmentLength = currentOtaImageSize - currentOtaDownloadFragmentOffset;
    }
    else
    {
        // Full image has been downloaded
        otaState = Application_OtaState_DownloadComplete;
        gpRf4ceOta_ImageDownloadCompleteRequest(currentOtaPairingRef, gpRf4ceOta_Status_DownloadedAndVerified, currentOtaImageType, currentOtaImageVersion);
        return;
    }
    GP_LOG_PRINTF("OTA image download %ld %d %ld %ld",4,currentOtaDownloadFragmentOffset,currentOtaDownloadFragmentLength,currentOtaImageSize,otaDownloadFragmentLength);
    gpRf4ceOta_ImageDownloadBlockRequest(currentOtaPairingRef, currentOtaImageType, currentOtaImageVersion, currentOtaDownloadFragmentOffset, otaDownloadFragmentLength);
}

void gpApplication_OtaImageUpgradeStart(void)
{
    if(otaState != Application_OtaState_DownloadComplete)
    {
        GP_LOG_SYSTEM_PRINTF("OTA wrong state %x - upgrade failed",1,otaState);
        return;
    }
    gpRf4ceOta_ImageUpgradeStartRequest(currentOtaPairingRef, currentOtaImageType, 0x00000000, currentOtaImageVersion, 0xFFFFFFFF);
}

void gpApplication_OtaImageUpgradeComplete(void)
{
    if(otaState != Application_OtaState_UpgradeImage)
    {
        GP_LOG_SYSTEM_PRINTF("OTA wrong state %x - upgrade failed",1,otaState);
        gpRf4ceOta_ImageUpgradeCompleteRequest(currentOtaPairingRef, gpRf4ceOta_Status_UpgradeFailure, currentOtaImageType, currentOtaImageVersion);
    }
    else
    {
        gpRf4ceOta_ImageUpgradeCompleteRequest(currentOtaPairingRef, gpRf4ceOta_Status_Upgraded, currentOtaImageType, currentOtaImageVersion);
    }
}

/*******************************************************************************
 *                    RF4CE OTA Callback Functions
 ******************************************************************************/

void gpRf4ceOta_cbImageDownloadNotifyIndication(UInt8 pairingRef, gpRf4ceOta_ImageDownloadNotificationReason_t Reason, UInt8* ImageTypeBitmap)
{
    currentOtaImageType = gpRf4ceOta_ImageTypeMain;
    otaState            = Application_OtaState_ImageAvailable;
    GP_LOG_PRINTF("OTA %d Download notify (0x%x)",2,pairingRef, Reason);

    gpRf4ceOta_ImageDownloadStartRequest(pairingRef, currentOtaImageType, activeImageVersion , downloadedOtaImageVersion, activeHardwareVersion);

}
void gpRf4ceOta_cbImageDownloadStartConfirm(UInt8 pairingRef, gpRf4ceOta_Status_t Status, UInt8 ImageType, UInt32 NewImageVersion, UInt32 ImageSize, UInt8 ChecksumType, UInt32 Checksum)
{
    GP_LOG_PRINTF("OTA %d Download start response (0x%x)",2,pairingRef,Status);
    if(Status == gpRf4ceOta_Status_ImageAvailable)
    {
        otaState = Application_OtaState_DownloadingImage;
        currentOtaPairingRef              = pairingRef;
        currentOtaImageType               = ImageType;
        currentOtaImageVersion            = NewImageVersion;
        currentOtaImageSize               = ImageSize;
        currentOtaDownloadFragmentOffset  = 0;
        currentOtaDownloadFragmentLength  = OTA_IMAGE_FRAGMENT_LENGTH_DEFAULT;
        currentOtaChecksumType            = ChecksumType;
        currentOtaChecksum                = Checksum;
        otaImage = GP_POOLMEM_MALLOC(ImageSize*sizeof(UInt8));
        gpSched_ScheduleEvent(0,gpApplication_OtaImageDownload);
    }
    else
    {
        GP_LOG_SYSTEM_PRINTF("OTA Abort upate (0x%x)",1,Status);
        Application_ResetOtaState();
    }
}
void gpRf4ceOta_cbImageDownloadBlockConfirm(UInt8 pairingRef, gpRf4ceOta_Status_t Status, UInt8 ImageType, UInt32 NewImageVersion, UInt16 MinimumBlockPeriod, UInt32 ImageFragmentOffset, UInt8 ImageFragmentLength, gpPd_Offset_t nsduOffset, gpPd_Handle_t pdHandle)
{
    switch(Status)
    {
        case gpRf4ceOta_Status_ValidData:
        {
            if((ImageFragmentOffset+ImageFragmentLength)<=currentOtaImageSize)
            {
                gpPd_ReadByteStream(pdHandle, nsduOffset, ImageFragmentLength, &otaImage[ImageFragmentOffset]);
                currentOtaDownloadFragmentOffset += ImageFragmentLength;
                GP_LOG_PRINTF("OTA %d Image Download Block Response (length %d) ",3,pairingRef,ImageFragmentLength);

            }
            else
            {
                GP_LOG_SYSTEM_PRINTF("Overflow %d + %d > %d",3,ImageFragmentOffset, ImageFragmentLength, currentOtaImageSize);
            }
            break;
        }
        case gpRf4ceOta_Status_WaitForData:
        {
            GP_LOG_SYSTEM_PRINTF("OTA %d Image Download Block Response - wait for data",pairingRef);
            break;
        }
        case gpRf4ceOta_Status_AbortDownload:
        {
            GP_LOG_SYSTEM_PRINTF("OTA %d Image Download Block Response - abort download",pairingRef);
            Application_ResetOtaState();
            break;
        }
        default:
        {
            GP_LOG_SYSTEM_PRINTF("OTA %d Invalid status 0x%x",pairingRef,Status);
        }
    }
}
void gpRf4ceOta_cbImageDownloadReadyForNextBlockIndication(UInt8 pairingRef)
{
    gpSched_ScheduleEvent(0,gpApplication_OtaImageDownload);
}
void gpRf4ceOta_cbImageDownloadCompleteConfirm(UInt8 pairingRef, gpRf4ce_Result_t Status, UInt8 ImageType, UInt32 NewImageVersion)
{
    GP_LOG_PRINTF("OTA %d Download Complete (0x%x)",2,pairingRef,Status);
    gpSched_ScheduleEvent(0,gpApplication_OtaImageUpgradeStart);
}
void gpRf4ceOta_cbImageUpgradeNotifyIndication(UInt8 pairingRef, gpRf4ceOta_ImageUpgradeNotificationReason_t Reason, UInt8* ImageTypeBitmap)
{
    GP_LOG_PRINTF("%d Image upgrade notify (reason 0x%x)",2,pairingRef,Reason);
    gpSched_ScheduleEvent(0,gpApplication_OtaImageUpgradeStart);
}
void gpRf4ceOta_cbImageUpgradeStartConfirm(UInt8 pairingRef, UInt8 Status, UInt8 ImageType, UInt32 NewImageVersion)
{
    GP_LOG_PRINTF("%d Start Image Upgrade (0x%x)",2,pairingRef,Status);
    switch(Status)
    {
        case  gpRf4ceOta_Status_UpgradeNow:
        {
            otaState = Application_OtaState_UpgradeImage;
            GP_LOG_SYSTEM_PRINTF("OTA %d Upgrade now ",1,pairingRef);
            GP_LOG_PRINTF("image data: ",0);
            for (UIntLoop i = 0; i < currentOtaImageSize; ++i)
            {
                GP_LOG_PRINTF("0x%x",1,otaImage[i]); gpLog_Flush();
            }
            gpPoolMem_Free(otaImage);
            gpSched_ScheduleEvent(0,gpApplication_OtaImageUpgradeComplete);
            break;
        }
        case gpRf4ceOta_Status_UpgradeWhenQuiet:
        {
            GP_LOG_SYSTEM_PRINTF("OTA %d Upgrade when quiet - not yet supported",1,pairingRef);
            // break;
        }
        case gpRf4ceOta_Status_DropImage:
        {
            Application_ResetOtaState();
            GP_LOG_SYSTEM_PRINTF("OTA %d Drop image",1,pairingRef);
            break;
        }
        case gpRf4ceOta_Status_WaitToUpgrade:
        {
            GP_LOG_SYSTEM_PRINTF("OTA %d Wait to upgrade",1,pairingRef);
            // wait for Image Upgrade Notify
            break;
        }
        default:
        {
            GP_LOG_SYSTEM_PRINTF("OTA %d Status 0x%x invalid",2,pairingRef,Status);
        }
    }

}
void gpRf4ceOta_cbImageUpgradeCompleteConfirm(UInt8 pairingRef, gpRf4ce_Result_t Status, UInt8 ImageType, UInt32 PendingImageVersion)
{
    GP_LOG_PRINTF("OTA %d Image Upgrade Complete Response (0x%x)",2,pairingRef,Status);
    Application_ResetOtaState();
}
