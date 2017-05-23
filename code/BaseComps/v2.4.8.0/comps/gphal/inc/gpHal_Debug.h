/*
 * Copyright (c) 2009-2014, GreenPeak Technologies
 *
 * gpHal_Debug.h
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/inc/gpHal_Debug.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_HAL_DEBUG_H_
#define _GP_HAL_DEBUG_H_

/** @file gpHal_Debug.h
 *  @brief Usefull stuff for debugging and logging.
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_HAL_REGISTER_ACCESS_BUFFER_SIZE
#define GP_HAL_REGISTER_ACCESS_BUFFER_SIZE   50
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gphal_RegisterAccessTypeRead            0
#define gphal_RegisterAccessTypeWrite           1
#define gphal_RegisterAccessTypeReadStream      2
#define gphal_RegisterAccessTypeWriteStream     3
#define gphal_RegisterAccessTypeReadModifyWrite 4
typedef UInt8 gphal_RegisterAccessType_t;


typedef struct gphal_RegisterAccess {
    gphal_RegisterAccessType_t  type;
    UInt8                       dataOrLenght;
    UInt16                      address;
    UInt8                       mask;
} gphal_RegisterAccess_t;


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
#define gphal_DumpBufferAsHex(a,b) do {} while (false)
#define gphal_DumpTxPbmEntry(i) do {} while (false)
#define gphal_DumpPbmStates() do {} while (false)
#define gphal_DumpEventsSummary() do {} while (false)
#define gphal_DumpEvent(event) do {} while (false)
#define gphal_DumIrqConfig() do {} while (false)
#define gphal_DumpExtEvent() do {} while (false)
#define gphal_StartLogRegisterAccesses() do {} while (false)
#define gphal_AddLogRegisterAccess(type, address, dataOrLength, mask) do {} while (false)
#define gphal_StopLogRegisterAccesses() do {} while (false)
#endif //_GP_HAL_DEBUG_H_
