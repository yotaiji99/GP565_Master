
/*
 * Copyright (c) 2011-2012,2014, GreenPeak Technologies
 *
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPoolMem/inc/gpPoolMem.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPPOOLMEM_H_
#define _GPPOOLMEM_H_

//DOCUMENTATION GENERATION: no @file required as all documented items are refered to a group

/**
 *@defgroup DEF_POOLMEM General PoolMem Definitions
 * @brief The general PoolMem Definitions
 *
 * Maximal 3 different sizes of chunks available
 *
 * @image html chunk.png
 * @image latex chunk.png "Single Chunk structure"
 *
 *@defgroup GEN_POOLMEM General PoolMem Public Functions
 * @brief The general public PoolMem functionality is implemented in these functions
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include <global.h>
#include "gpPd.h" // for gpPd_Descriptor_t
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
 
#define GP_POOLMEM_MALLOC(nbytes)    gpPoolMem_Malloc(GP_COMPONENT_ID,(nbytes))

#define GP_POOLMEM_PD_SIZE (sizeof(gpPd_Descriptor_t))

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**@ingroup GEN_POOLMEM
 * @brief This function initialize the chunks, it basically sets all the guards and the InUse flags on false. 
 *
*/
void gpPoolMem_Init(void);

/**@ingroup GEN_POOLMEM
 * @brief This function is almost similar as PoolMem_Init; invoked at warm restart. 
 *
*/
void gpPoolMem_Reset(void);

/**@ingroup GEN_POOLMEM
 * @brief This function allocates a free chunk 
 *
* @param nbytes    Minimal Chunk size
* @return void*    Returns pointer to allocated chunk
* @return Assert   When no chunk free
* @return Assert   When nbytes > size of biggest chunk
* @return Assert   When GuardEnd or GuardStart overwritten of free chunk
*/
void *gpPoolMem_Malloc (UInt8 comp_id, UInt8 nbytes);

/**@ingroup GEN_POOLMEM
 * @brief This function will free an allocated chunk 
 *
* @param pData     Pointer to Chunk
* @return Assert   When wrong pointer given
* @return Assert   When GuardEnd or GuardStart overwritten
*/
void gpPoolMem_Free (void* pData);



#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPPOOLMEM_H_


