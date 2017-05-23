
/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPoolMem/src/gpPoolMem_impl.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */
 
#define GP_COMPONENT_ID GP_COMPONENT_ID_POOLMEM
#define GP_MODULE_ID GP_MODULE_ID_POOLMEM

//#define GP_LOCAL_LOG

#include "gpLog.h"
#include "gpAssert.h" 
#include "gpWmrk.h"
#include "gpPoolMem.h"
#include "gpPoolMem_defs.h"
 
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
/* Init chunks, put all InUse flags on false and fill Guard with pattern */
/* InUse flag on GP_POOLMEM_NOTUSED when not used. If used we will place CompId inside */
#define POOLMEM_INIT(size,amount) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        Chunk##size[i].InUse = GP_POOLMEM_NOTUSED; \
        Chunk##size[i].GuardStart = GP_POOLMEM_GUARD;\
        Chunk##size[i].GuardEnd = GP_POOLMEM_GUARD; \
    } \
    } while(false)

/* Look for free chunk and return pointer to data. Check guardStart before checking InUse flag. */
/* Component Id will be used as InUse flag */
#define POOLMEM_MALLOC(size,amount,nbytes,comp_id)    do { \
    UIntLoop i; \
    for (i = 0; i < amount; ++i) \
    { \
        if (Chunk##size[i].InUse == GP_POOLMEM_NOTUSED) \
        { \
            GP_LOG_PRINTF("BA-%u[%u]-CId:%u-S:%u",8,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i,(UInt16)comp_id,(UInt16)nbytes);\
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD);\
            Chunk##size[i].InUse = comp_id; \
            gpWmrk_CntrToLimit(gpPoolMemWmrk##size); \
            gpWmrk_CntrSet(gpPoolMemWmrkSize, nbytes); \
            return &(Chunk##size[i].Data[0]); \
        } \
    } \
    } while(false)

/* Look for chunk to be freed */    
#define POOLMEM_FREE(size,amount) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        if (pData == &(Chunk##size[i].Data[0])) \
        { \
            GP_LOG_PRINTF("BF-%u[%u]:%u",6,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i,(UInt16)Chunk##size[i].InUse);\
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD\
                && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD && Chunk##size[i].InUse != GP_POOLMEM_NOTUSED);\
            Chunk##size[i].InUse = GP_POOLMEM_NOTUSED; \
            gpWmrk_CntrFromLimit(gpPoolMemWmrk##size); \
            return; \
        } \
     }\
    } while(false)
    
#define POOLMEM_FREE_BY_COMPID(size,amount, comp_id) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        if (comp_id == Chunk##size[i].InUse) \
        { \
            GP_LOG_PRINTF("BF-%u[%u]:CId:%u %u",6,(UInt16)sizeof(Chunk##size[i].Data),(UInt16)i, comp_id, (UInt16)Chunk##size[i].InUse);\
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD\
                && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD );\
            Chunk##size[i].InUse = GP_POOLMEM_NOTUSED; \
            gpWmrk_CntrFromLimit(gpPoolMemWmrk##size); \
        } \
     }\
    } while(false)

#define POOLMEM_IN_USE(size,amount,amountInUse) do { \
    UIntLoop i; \
    for (i = 0; i < amount; i++) \
    { \
        if (GP_POOLMEM_NOTUSED != Chunk##size[i].InUse) \
        { \
            GP_ASSERT_SYSTEM(Chunk##size[i].GuardStart == GP_POOLMEM_GUARD\
                && Chunk##size[i].GuardEnd == GP_POOLMEM_GUARD );\
            amountInUse++; \
        } \
     }\
    } while(false)
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#if defined GP_POOLMEM_CHUNK_AMOUNT1 == 0
#error Minimum need 1 buffer to play with
#endif

//Data pools
extern gpPoolMem_Chunks1_t Chunks1[];
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
extern gpPoolMem_Chunks2_t Chunks2[];
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
extern gpPoolMem_Chunks3_t Chunks3[];
#endif


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/* Init of chunks inside memory */
 void PoolMem_Init(void)
 {
    GP_LOG_PRINTF("Init PoolMem",0);
    POOLMEM_INIT(s1,GP_POOLMEM_CHUNK_AMOUNT1);
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_INIT(s2,GP_POOLMEM_CHUNK_AMOUNT2);
#endif // GP_POOLMEM_CHUNK_AMOUNT2 > 0
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_INIT(s3,GP_POOLMEM_CHUNK_AMOUNT3);
#endif
 }
 
void gpPoolMem_Reset(void)
 {
    GP_LOG_PRINTF("Reset PoolMem",0);
    POOLMEM_INIT(s1,GP_POOLMEM_CHUNK_AMOUNT1);

#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_INIT(s2,GP_POOLMEM_CHUNK_AMOUNT2);
#endif // GP_POOLMEM_CHUNK_AMOUNT2 > 0

#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_INIT(s3,GP_POOLMEM_CHUNK_AMOUNT3);
#endif // GP_POOLMEM_CHUNK_AMOUNT3 > 0
 }
 
/* Implementation Malloc function */ 
void *PoolMem_Malloc ( UInt8 comp_id, UInt8 nbytes)
{
    /* Look for free chunk
    Check GuardStart before checking InUse flag */
    if ((GP_POOLMEM_CHUNK_SIZE1 < GP_POOLMEM_PD_SIZE) && (GP_POOLMEM_CHUNK_AMOUNT2 == 0) && (GP_POOLMEM_CHUNK_AMOUNT3 == 0))
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id); 
    }
    else if((GP_POOLMEM_CHUNK_SIZE1 > GP_POOLMEM_PD_SIZE) && (GP_POOLMEM_CHUNK_AMOUNT2 == 0) && (GP_POOLMEM_CHUNK_AMOUNT3 == 0))
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);
    }
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    else if((GP_POOLMEM_CHUNK_AMOUNT2 > 0) && (GP_POOLMEM_CHUNK_AMOUNT3 == 0))
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);
    
        if (GP_POOLMEM_PD_SIZE < GP_POOLMEM_CHUNK_AMOUNT2)
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
        }
        else
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
        }
    }
#endif // GP_POOLMEM_CHUNK_AMOUNT2 > 0
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    else // (GP_POOLMEM_CHUNK_AMOUNT3 > 0) && (GP_POOLMEM_CHUNK_AMOUNT2 > 0) 
    {
        if (nbytes <= GP_POOLMEM_CHUNK_SIZE1) POOLMEM_MALLOC(s1,GP_POOLMEM_CHUNK_AMOUNT1,nbytes,comp_id);
        if (GP_POOLMEM_PD_SIZE < GP_POOLMEM_CHUNK_AMOUNT2)
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE3) POOLMEM_MALLOC(s3,GP_POOLMEM_CHUNK_AMOUNT3,nbytes,comp_id);
        }
        else
        {
            if (nbytes <= GP_POOLMEM_CHUNK_SIZE2) POOLMEM_MALLOC(s2,GP_POOLMEM_CHUNK_AMOUNT2,nbytes,comp_id);
            if (GP_POOLMEM_PD_SIZE < GP_POOLMEM_CHUNK_AMOUNT3)
            {
                if (nbytes <= GP_POOLMEM_CHUNK_SIZE3) POOLMEM_MALLOC(s3,GP_POOLMEM_CHUNK_AMOUNT3,nbytes,comp_id);
            }
            else
            {
                if (nbytes <= GP_POOLMEM_CHUNK_SIZE3) POOLMEM_MALLOC(s3,GP_POOLMEM_CHUNK_AMOUNT3,nbytes,comp_id);
            }
        }
    }
#endif // GP_POOLMEM_CHUNK_AMOUNT3 > 0
    //If no buffer available or buffer sizes too small:
    GP_LOG_PRINTF("Chunk size too small: CId:%u - size:%u",4,(UInt16)comp_id,(UInt16)nbytes);
    GP_ASSERT_DEV_EXT(false);
    return NULL;
}

/* Implementation Free function */
void PoolMem_Free (void* pData)
/* Check guards and free chunk, if guard error -> assert */
{
    // Release chunk
    POOLMEM_FREE(s1,GP_POOLMEM_CHUNK_AMOUNT1);
#if GP_POOLMEM_CHUNK_AMOUNT2 > 0
    POOLMEM_FREE(s2,GP_POOLMEM_CHUNK_AMOUNT2);
#endif
#if GP_POOLMEM_CHUNK_AMOUNT3 > 0
    POOLMEM_FREE(s3,GP_POOLMEM_CHUNK_AMOUNT3);
#endif
    // If chunk not found, wrong pointer is given:
    GP_ASSERT_DEV_EXT(false);
}

