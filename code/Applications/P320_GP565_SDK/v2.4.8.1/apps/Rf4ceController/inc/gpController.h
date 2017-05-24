/*
 * Copyright (c) 2015, GreenPeak Technologies
 *
 * gpController.h
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/apps/Rf4ceController/inc/gpController.h#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */


#ifndef _GPCONTROLLER_H_
#define _GPCONTROLLER_H_

/**
 * @file gpController.h
 *
 * @defgroup SETUP Reference Setup Module
 * This functional block implements the Remote Control Setup mode. In setup mode
 * the user has access to some special features like triggering remote control
 * binding and unbinding or doing a factory reset.
 * The main controller application can forward key presses to the seup module
 * to trigger special actions. This module can be used as a reference for youw
 * setup code.
 *
 * @defgroup ZRC Reference ZRC Module
 * This functional block implements the ZigBee Remote Control (ZRC) RF4CE
 * profile functionality like triggering binding using the gpRf4ceBindGdp block
 * or sending key actions to the target using the gpRf4ceActionControl block.
 * This module can be used as a reference for youw own ZRC code.
 *
 * @defgroup MSO Reference MSO Module
 * This functional block implements the Multiple Service Operator (MSO) RF4CE
 * profile functionality like trigggering binding using the gpRf4ceBindValidation
 * block or sending key actions to the target using the gpRf4ceUserControl
 * block. This module can be used as a reference for youw own MSO code
 *
 * @defgroup LED Reference LED Module
 * This functional block implements a LED control module that can generate
 * simple LED patterns for a green and red LED on the GreenPeak software
 * development kit. This module can be used as a reference for youw own LED
 * code.
 *
 * @defgroup KEYBOARD Reference Keyboard Module
 * This functional block implements a basic keyboard module that can handle
 * basic key presses/key releases and simple key combinations.
 * This module can be used as a reference for youw own keyboard code
 */

/*******************************************************************************
 *                      Include Files
 ******************************************************************************/
#include "global.h"

#ifdef GP_DIVERSITY_APP_ZRC2_0
#include "gpZrc.h"
#endif /*GP_DIVERSITY_APP_ZRC2_0*/
/*******************************************************************************
 *                    Type Definitions
 ******************************************************************************/

/** @typedef gpController_Keys_t
 *  The gpController_Keys_t type specifies a amount of pressed keys that can be
 *  passed to the ZRC or Setup Module.
*/
typedef struct gpController_Keys
{
    /** number of keys pressed */
    UInt8 count;
    /** key indexes of the pressed keys */
    UInt8 codes[GP_RF4CECMD_DIVERSITY_CONCURRENT_ACTIONS]; 
} gpController_Keys_t;

/** @name gpControllerOperationMode_t */
//@{
/** @brief The default operation mode where keys are sent to the target device */
#define gpController_OperationModeNormal        0x01
/** @brief The setup operation mode where keys are processed as special input
           for controller setup purposes */
#define gpController_OperationModeSetup         0x02
/** @brief The binding operation mode where keys are processed as special input
           for binding purposes */
#define gpController_OperationModeBinding       0x03
/** @brief The  operation mode where IR keys are being transmit*/
#define gpController_OperationModeIR            0x04
/** @brief The  operation mode ZID configuration*/
#define gpController_OperationModeConfiguring   0x05
/** @brief The  operation mode set vendor ID */
#define gpController_OperationModeSetVendorID   0x06
/** @typedef gpControllerOperationMode_t
*   @brief  The gpControllerOperationMode_t type defines several controller
*           operation modes.
*/
#define gpController_OperationModeSpecial       0x07

#define gpController_OperationDoublekey         0x08

#define gpController_OperationSetsequence       0x09

typedef UInt8 gpControllerOperationMode_t;
//@}
#endif /* _GPCONTROLLER_LED_H_ */
