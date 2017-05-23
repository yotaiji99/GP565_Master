
/*
 * Copyright (c) 2011-2012,2014, GreenPeak Technologies
 * 
 * This file implements a pool memory allocator
 *
 *                ,               This software is owned by GreenPeak Technologies
 *                g               and protected under applicable copyright law
 *               ]&$              It is delivered under the terms of the license
 *               ;QW              and is intended and supplied for use solely and
 *               G##&             exclusively with products manufactured by
 *               N#&0,            GreenPeak Technologie
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPoolMem/src/gpPoolMem.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

// General includes
#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
 
 /* Init amount of chunks inside memory */
 void gpPoolMem_Init(void)
 {
    PoolMem_Init();
 }
 
  /* Implement malloc function */ 
void *gpPoolMem_Malloc (UInt8 comp_id, UInt8 nbytes)
{
    return PoolMem_Malloc(comp_id,nbytes);
}

/* Implement free function */
void gpPoolMem_Free (void* pData)
/* Check guards and free chunk, if guard error -> assert */
{
    PoolMem_Free(pData);
    // If chunk not found, wrong pointer is given or InUse flag overwritten -> assert
}

