
/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 * 
 * This file defines the pools used by the pool memory allocator
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPoolMem/src/gpPoolMem_data.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
/*****************************************************************************
 *                    Include Definitions
 *****************************************************************************/

#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
#if GP_POOLMEM_CHUNK_AMOUNT1 == 0
#error Minimum need 1 buffer to play with
#endif

#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING GEN_EXTRAM_SECTION
#endif //USE PRAGMA

//Various data
gpPoolMem_Chunks1_t Chunks1[GP_POOLMEM_CHUNK_AMOUNT1] GP_EXTRAM_SECTION_ATTR;
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
gpPoolMem_Chunks2_t Chunks2[GP_POOLMEM_CHUNK_AMOUNT2] GP_EXTRAM_SECTION_ATTR;
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
gpPoolMem_Chunks3_t Chunks3[GP_POOLMEM_CHUNK_AMOUNT3] GP_EXTRAM_SECTION_ATTR;
#endif


#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING
#endif //USE PRAGMA

