/*
 * Copyright (c) 2010-2013, GreenPeak Technologies
 *
 * gpSched_simple.c
 *   This file contains the data buffers used by the scheduler
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpSched/src/gpSched_data.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SCHED

#include "hal.h"
#include "gpUtils.h"
#include "gpSched.h"
#include "gpSched_defs.h"

/*****************************************************************************
 *                    Precompiler checks
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Size definitions
#ifndef GP_SCHED_EVENT_LIST_SIZE
#define GP_SCHED_EVENT_LIST_SIZE            10
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

GP_UTILS_LL_MEMORY_DECLARATION(gpSched_Event_t, GP_SCHED_EVENT_LIST_SIZE);

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING GEN_EXTRAM_SECTION
#endif
GP_UTILS_LL_MEMORY_ALOCATION(gpSched_Event_t, gpSched_EventArray) GP_EXTRAM_SECTION_ATTR;
#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING
#endif

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

void gpSched_InitExtramData(void)
{
    gpUtils_LLInit(gpSched_EventArray, GP_UTILS_LL_SIZE_OF (gpSched_Event_t) , GP_SCHED_EVENT_LIST_SIZE, gpSched_EventFree);
}


