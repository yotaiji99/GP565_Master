/*
 *   Copyright (c) 2015, GreenPeak Technologies
 *
 *   Control over system reset
 *   Declarations of the public functions and enumerations of gpReset.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpReset/inc/gpReset.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPRESET_H_
#define _GPRESET_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Enum Definitions
 *****************************************************************************/

/** @enum gpReset_ResetReason_t */
//@{
#define gpReset_ResetReason_UnSpecified                        0x00
#define gpReset_ResetReason_HW_BrownOutDetected                0x01
#define gpReset_ResetReason_HW_Watchdog                        0x02
#define gpReset_ResetReason_HW_Por                             0x03
#define gpReset_ResetReason_SW_Por                             0x04
typedef UInt8                             gpReset_ResetReason_t;
//@}

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/** @brief Default reset method: maps on sw por */
#define gpReset_ResetSystem()              gpReset_ResetBySwPor()

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

void gpReset_Init(void);

void gpReset_ResetBySwPor(void);

gpReset_ResetReason_t gpReset_GetResetReason(void);

void gpReset_ResetByWatchdog(void);

//Indications

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //_GPRESET_H_

