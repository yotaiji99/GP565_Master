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



/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
// possible controller modes
#define gpController_ModeZrc                            0x00
#define gpController_ModeMSO                            0x00
#define gpController_ModeIRNec                          0x01
#define gpController_ModeTxTVIR                         0x02
#define gpController_Mode_MAX	                        0x03



/*****************************************************************************
 *                    Public Function Prototypes
 *****************************************************************************/


#endif /* _GPCONTROLLER_MAIN_H_ */

