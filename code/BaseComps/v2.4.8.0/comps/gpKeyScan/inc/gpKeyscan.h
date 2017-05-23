/*
 * Copyright (c) 2010-2013, GreenPeak Technologies
 *
 * gpKeyScan.h
 *
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpKeyScan/inc/gpKeyScan.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPKEY_SCAN_H_
#define _GPKEY_SCAN_H_
/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"


/** @typedef gpKeyScan_cbCheckLowVoltage_t
 *  @brief   The gpKeyScan_cbCheckLowVoltage_t callback type definition defines the callback prototype of the CheckLowVoltage callback. */
typedef Bool (*gpKeyScan_cbCheckLowVoltage_t)(void);

/** @typedef gpKeyScan_cbStuckKeyNotification_t
 *  @brief   The gpKeyScan_cbStuckKeyNotification_t callback type definition defines the callback prototype of the StuckKey callback. */
typedef void (*gpKeyScan_cbStuckKeyNotification_t)(Bool);

/** @ingroup
 *
 *  This primitive returns a pointer to an array. This array contains all pressed buttons(1). This primitive is called by the gpKeyScan to indicate the application the keyboard has been changed.
 *
 *  @param  pBitmaskMatrix        The matrix with the state of the keyboard.
*/
GP_API void gpKeyScan_cbScanIndication( const UInt8* pBitmaskMatrix);


/** @ingroup
 *
 *  This primitive allows the application to init the gpKeyScan component. The init does not enable key scanning.
 *
*/
GP_API void gpKeyScan_Init( void );

/** @ingroup
 *
 *  This primitive allows the application to enable automaic key scaninng.
 *
*/
GP_API void gpKeyScan_EnableAutomaticKeyScanning(void);

/** @ingroup
 *
 *  This primitive allows the application to disable automaic key scaninng. All Scanning needs to be disabled.
 *
*/
GP_API void gpKeyScan_DisableAutomaticKeyScanning(void);

/** @ingroup
 *
 *  This primitive allows the application to inform the keyscan module that a stuck key scenario is detected.
 *  The keyscan module will mimic a key release (in either a power optimal or power hungry way).
 *
*/
GP_API void gpKeyScan_HandleStuckKey(void);

/** @ingroup
 *
 *  This primitive allows to register a function that monitors the voltage on each keyscan
 *  Note that enabling the voltage monitor can have an impact on the duration of the key scan ISR.
 *
*/
GP_API void gpKeyScan_RegisterCheckLowVoltageCallback(gpKeyScan_cbCheckLowVoltage_t callback);

/** @ingroup
 *
 *  This primitive allows to register a function that will be triggered whenever the key scan
 *  component changes it's internel stuckkey statusflag.
 *
*/
GP_API void gpKeyScan_RegisterCheckStuckKeyCallback(gpKeyScan_cbStuckKeyNotification_t callback);

#endif //_GPKEY_SCAN_H_

