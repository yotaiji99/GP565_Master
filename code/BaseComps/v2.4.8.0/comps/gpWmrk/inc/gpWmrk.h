
/*
 * Copyright (c) 2012, GreenPeak Technologies
 *
 * gpWmrk.h
 * 
 * This file defines the watermarker component api
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpWmrk/inc/gpWmrk.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPWMRK_H_
#define _GPWMRK_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/// @brief The maximum amount tallies to be monitored (can be overruled at compilation time).
#ifndef GP_WMRK_MAX_ENTRIES
#define GP_WMRK_MAX_ENTRIES          6
#endif
/// @brief The interval to generate a log which shows all subscribed tallies (can be overruled at compilation time).
#ifndef GP_WMRK_INTERVAL
#define GP_WMRK_INTERVAL                    60000000
#endif
/// @brief The maximum # chars to define a unique id.
#define GP_WMRK_MAX_UID_LENGTH             3

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/** @typedef gpWmrk_Parameters_t
 *  @brief The gpWmrk_Parameters_t defines the parameters required for the watermarker 
*/
typedef struct gpWmrk_Parameters {
    UInt16  softLimit;
    UInt16  deadLimit;
    UInt16  value;
    char    uid[GP_WMRK_MAX_UID_LENGTH];    // unique id
    Bool    up;                             // up or down counter
} gpWmrk_Parameters_t;

/** @typedef gpWmrk_Id_t
 *  @brief The gpWmrk_Id_t defines the id used by the Watermarker, FAILURE equals 0xFF
 * When a counter needs to be manipulated, the very same Id needs to be passed as argument 
 *
 
*/
typedef UInt8 gpWmrk_Id_t;

/// @brief Value returned in error situations.
#define GP_WMRK_FAILURE 0xFF

/** 
 * @file gpWmrk.h
 *
 *
 * @defgroup GEN_WMRK General Watermarker functions
 * @brief The general Watermarker functionality is implemented in these functions
 *
 * @note When a counter needs to go towards its limit gpWmrk_CntrIncr should be used despite it's direction
 */


#define gpWmrk_Init()
#define gpWmrk_Subscribe(params)         0
#define gpWmrk_CntrToLimit(id)                 
#define gpWmrk_CntrFromLimit(id)
#define gpWmrk_CntrSet(id, val)


#endif // _GPWMRK_H_


