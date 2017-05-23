/*
 * Copyright (c) 2011-2012, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpRf4ceBindValidation/src/gpRf4ceBindValidation_configuration.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpRf4ceBindValidation.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_RF4CE_BIND_VALIDATION_USER_STRING
#define GP_RF4CE_BIND_VALIDATION_USER_STRING GP-MSO
#endif

#ifndef GP_RF4CE_BIND_VALIDATION_DEV_TYPE_LIST
#define GP_RF4CE_BIND_VALIDATION_DEV_TYPE_LIST gpRf4ce_DeviceTypeSetTopBox
#endif

#ifndef GP_RF4CE_BIND_VALIDATION_PROFILE_ID_LIST
#define GP_RF4CE_BIND_VALIDATION_PROFILE_ID_LIST 0xC0
#endif



/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

const gpRf4ceBindValidation_UserString_t ROM gpRf4ceBindValidation_NodeUserString FLASH_PROGMEM = {XSTRINGIFY(GP_RF4CE_BIND_VALIDATION_USER_STRING)};

const gpRf4ce_DeviceType_t ROM gpRf4ceBindValidation_pNodeDevTypeList[]   FLASH_PROGMEM = {GP_RF4CE_BIND_VALIDATION_DEV_TYPE_LIST};
const gpRf4ce_ProfileId_t  ROM gpRf4ceBindValidation_pNodeProfileIdList[] FLASH_PROGMEM = {GP_RF4CE_BIND_VALIDATION_PROFILE_ID_LIST};

const UInt8 ROM gpRf4ceBindValidation_NodeAppCapabilities FLASH_PROGMEM =
    (NrOfElements(gpRf4ceBindValidation_pNodeDevTypeList) << GP_RF4CE_APP_CAPABILITY_NBR_DEVICES_IDX) |
    (NrOfElements(gpRf4ceBindValidation_pNodeProfileIdList) << GP_RF4CE_APP_CAPABILITY_NBR_PROFILES_IDX);

