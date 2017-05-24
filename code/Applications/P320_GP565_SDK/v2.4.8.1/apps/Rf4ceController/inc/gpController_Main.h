/*
 * Copyright (c) 2011, GreenPeak Technologies
 *
 * controller.h
 *
 * This file implements the remote setup functionality
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
 *     0!                         $Header: //depot/main/Embedded/Applications/P218_RDRC_CRB36/v1.9.1.0/apps/MsoController/inc/controller.h#1 $
 *    M'   GreenPeak              $Change: 76824 $
 *   0'         Technologies      $DateTime: 2016/02/10 11:53:09 $
 *  F
 */
#ifndef _GPCONTROLLER_MAIN_H_
#define _GPCONTROLLER_MAIN_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "gpKeyboard.h"
#include "gpRf4ce.h"
#include "gpController_Led.h"
 
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define APP_DEFAULT_MODE                                gpController_ModeIRNec



/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
// possible controller modes
#define gpController_ModeZrc                            0x00
#define gpController_ModeMSO                            0x00
#define gpController_ModeIRNec                          0x01
#define gpController_ModeTxTVIR                         0x02
#define gpController_Mode_MAX	                        0x03

typedef UInt8 gpController_Mode_t;

typedef void (*gpController_cbKeyIndication_t)(gpKeyboard_pKeyInfo_t pKey);
#define GP_PROGKEY_MAX_IRCODE_SIZE                      80
#define APP_STATISTICS_TX_KEY_INDEX                     25
#define KEY_POWER                          1	//15
#define KEY_VOL_UP                         2	//6
#define KEY_VOL_DN                         3	//22
#define KEY_MUTE	                       4	//21

#define SET_BLINK_TV_IR_CODE				1
#define SET_BLINK_SW_VERSION				2
#define SET_SEARCH_TV_IR_CODE				3
#define SET_BLINK_BATTERY_STATUS			4

typedef struct controller_TvIrDesc_s
{
    UInt8   code[GP_PROGKEY_MAX_IRCODE_SIZE];
} controller_TvIrDesc_t;

typedef struct controller_KeyDesc_s
{
    UInt8                       keyConfig;
    controller_TvIrDesc_t       tvIrDesc;
} controller_KeyDesc_t;

typedef struct  {
    UInt16 backupActiveIRTableId;
    UInt16 currentSearchIndex;
    UInt16 lastAttemptedTvCode;
} searchTvIrCode_t;

/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/

extern Bool gpController_SelectDeviceInDatabase( UInt16 deviceId );
extern void setup_cbKeyIndicationSearchTvIrCode( gpKeyboard_pKeyInfo_t pKey );
extern Bool gpController_PowerToggle;
extern UInt8 PowerToggle;
extern gpController_Mode_t             gpController_Mode;                  //current controller application mode
extern gpController_cbKeyIndication_t  gpController_cbKeyIndication; //function pointer used to forward the key-information to the mode/setup specific handling
extern UInt32 gpStatus_NumberOfSentIR;
extern UInt32 gpStatus_NumberOfSentRF;
GP_API UInt8 ControllerOperationMode;

#endif /* _GPCONTROLLER_MAIN_H_ */

