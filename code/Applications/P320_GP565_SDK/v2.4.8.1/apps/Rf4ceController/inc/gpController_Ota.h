/*
 * Copyright (c) 2016, GreenPeak Technologies
 *
 * gpController_Ota.h
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
 *     0!                         $Header: //depot/main/Embedded/Applications/P857_RC_RemoteSolution_Rogers_Eclipse_GP565/vlatest/apps/Rf4ceController/inc/gpController_Ota.h#1 $
 *    M'   GreenPeak              $Change: 80002 $
 *   0'         Technologies      $DateTime: 2016/04/30 16:01:19 $
 *  F
 */


#ifndef _GPCONTROLLER_OTA_H_
#define _GPCONTROLLER_OTA_H_

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"

/*******************************************************************************
 *                    Defines
 ******************************************************************************/
#define OTA_IMAGE_FRAGMENT_LENGTH_DEFAULT     8 
#define OTA_DEFAULT_IMAGE_VERSION             0x00000100 /* v0.0.1.0 */
#define OTA_DEFAULT_HARDWARE_VERSION          0x00000100 /* v0.0.1.0 */

/*******************************************************************************
 *                    Enum Definitions
 ******************************************************************************/
#define Application_OtaState_Idle             0x00
#define Application_OtaState_ImageAvailable   0x01
#define Application_OtaState_DownloadingImage 0x02
#define Application_OtaState_DownloadComplete 0x03
#define Application_OtaState_UpgradeImage     0x04
typedef UInt8 Application_OtaState_t;

/*******************************************************************************
 *                    Public Function Declarations
 *******************************************************************************/
/* Function to initialize the OTA application block */
void gpApplication_OtaInit(void);

/* Function to download image blocks from the target */
void gpApplication_OtaImageDownload(void);

/* Function to trigger the image upgrade */
void gpApplication_OtaImageUpgradeStart(void);

/* Function to indicate to the STB that the upgrade has completed */
void gpApplication_OtaImageUpgradeComplete(void);

#endif /* _GPCONTROLLER_OTA_H_ */
