/*
 * Copyright (c) 2010-2014, GreenPeak Technologies
 *
 * gpSched_defs.h
 *   This file holds the internal defines, typedefs,... of the gpSched component
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpSched/src/gpSched_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GP_SCHED_DEFS_H_
#define _GP_SCHED_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "hal.h"
#include "gpUtils.h"
#include "gpSched.h"

/*****************************************************************************
 *                    Precompiler checks
 *****************************************************************************/

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef union {
    void_func                callback;    // Event to be executed without arguments  SPONG: Make union of 2 funcs
    gpSched_EventCallback_t  callbackarg; // Event to be executed with arguments
} gpSched_func_t;

typedef struct gpSched_Event {
    UInt32                   time;        // sorting key : Absolute in microseconds
    gpSched_func_t           func;
    void*                    arg;
} gpSched_Event_t;

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

extern gpUtils_LinkFree_t gpSched_EventFree[1];
extern gpUtils_LinkList_t gpSched_EventList[1];

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

void gpSched_InitExtramData(void);

#endif //_GP_SCHED_DEFS_H_

