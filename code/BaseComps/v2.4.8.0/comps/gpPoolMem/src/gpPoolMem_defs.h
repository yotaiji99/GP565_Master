/*
 * Copyright (c) 2012,2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPoolMem/src/gpPoolMem_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPPOOLMEM_DEFS_H_
#define _GPPOOLMEM_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include "gpPoolMem.h"

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 
 /** @ingroup DEF_POOLMEM
 *  @brief Size of Chunk size 1
*/
#ifndef GP_POOLMEM_CHUNK_SIZE1
#define GP_POOLMEM_CHUNK_SIZE1 50
#endif //GP_POOLMEM_CHUNK_SIZE1
 /** @ingroup DEF_POOLMEM
 *  @brief Size of Chunk size 2
*/
#ifndef GP_POOLMEM_CHUNK_SIZE2
#define GP_POOLMEM_CHUNK_SIZE2 100
#endif //GP_POOLMEM_CHUNK_SIZE2 
 /** @ingroup DEF_POOLMEM
 *  @brief Size of Chunk size 3
*/
#ifndef GP_POOLMEM_CHUNK_SIZE3
#define GP_POOLMEM_CHUNK_SIZE3 150
#endif //GP_POOLMEM_CHUNK_SIZE3 
  /** @ingroup DEF_POOLMEM
 *  @brief Amount of Chunk Size 1
*/
#ifndef GP_POOLMEM_CHUNK_AMOUNT1
#define GP_POOLMEM_CHUNK_AMOUNT1 1
#endif //GP_POOLMEM_CHUNK_AMOUNT1
  /** @ingroup DEF_POOLMEM
 *  @brief Amount of Chunk Size 2
*/
#ifndef GP_POOLMEM_CHUNK_AMOUNT2
#define GP_POOLMEM_CHUNK_AMOUNT2 1
#endif //GP_POOLMEM_CHUNK_AMOUNT2 
  /** @ingroup DEF_POOLMEM
 *  @brief Amount of Chunk Size 3
*/
#ifndef GP_POOLMEM_CHUNK_AMOUNT3
#define GP_POOLMEM_CHUNK_AMOUNT3 0
#endif //GP_POOLMEM_CHUNK_AMOUNT3 

/** @ingroup DEF_POOLMEM
 *  @brief Amount of PD Size
*/

    #define GP_POOLMEM_PD_AMOUNT         0


#ifdef ALIGNMENT_NEEDED
typedef UInt32 GUARD;
#else
typedef UInt16 GUARD;
#endif
  /** @ingroup DEF_POOLMEM
 *  @brief Guard Pattern
*/
#define GP_POOLMEM_GUARD 0xCA4E //Guard Pattern
  /** @ingroup DEF_POOLMEM
 *  @brief NotUsed pattern
*/
#define GP_POOLMEM_NOTUSED 0xFF 

/* Chunk structure, max 3 different sizes to use */

/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk s1
*/
typedef    struct gpPoolMem_Chunks1 {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_CHUNK_SIZE1];
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunks1_t;


/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk s2
*/
typedef    struct gpPoolMem_Chunks2 {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_CHUNK_SIZE2]; 
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunks2_t;


/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk s3
*/
typedef    struct gpPoolMem_Chunks3 {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_CHUNK_SIZE3]; 
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunks3_t;
 
/** @ingroup DEF_POOLMEM
 *  @brief Structure of Chunk pd
*/
typedef    struct gpPoolMem_Chunkpd {
    GUARD GuardStart;
    UInt8 Data[GP_POOLMEM_PD_SIZE]; 
    GUARD GuardEnd;
    Bool InUse;
}  gpPoolMem_Chunkpd_t;
 
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
 
void PoolMem_Init(void);
void *PoolMem_Malloc (UInt8 comp_id,UInt8 nbytes);
void PoolMem_Free (void* pData);

#endif // _GPPOOLMEM_DEFS_H_

