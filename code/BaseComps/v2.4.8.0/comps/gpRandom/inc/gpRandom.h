
/*
 * Copyright (c) 2012-2013, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRandom/inc/gpRandom.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


#ifndef _GPRANDOM_H_
#define _GPRANDOM_H_

//DOCUMENTATION GENERATION: no @file required as all documented items are refered to a group

/**
 *@defgroup DEF_RANDOM General Random Definitions
 * @brief The general Random Definitions
 *
 * The gpRandom component is intended for generation of the random number sequences. The randomness of
 * the internal random number generator is configurable at compile time and if not changed, the default
 * randomness is 128bits. The randomness is introduced via the call to randomize the generator and the
 * consequent random numbers are produced by mashing up the random seed with 32bit based pseudo random
 * generator. Unless disabled in compile time, the mashing process is modulated with time based low 
 * strength random generator.
 *
 * Note: Randomize function is lengthy and should be avoided to call it from points in code executing
 * under tight timeout.
 * Note: The driver is not made thread safe so care must be taken to avoid reentrancy. The consequence
 * might be that two callers getting random sequences end up with highly correlated random sequences.
 *
 *@defgroup GEN_RANDOM General Random Public Functions
 * @brief The general public Random functionality is implemented in these functions
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/
 
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif
/**@ingroup GEN_RANDOM
 * @brief This function seeds the generator with truly random numbers 
 *
 * Calling this function costs a lot of time being blocked in its execution.
 */
void gpRandom_RandomizeSeed(void);

/**@ingroup GEN_RANDOM
 * @brief This function generates new random number sequence
 *
 * @param pBuffer is refernece to memory location where new random number sequence is to be stored
 * @param nmbrRandomBytes is number of random bytes in sequence to be stored
 */
void gpRandom_GetNewSequence(UInt8 nmbrRandomBytes, UInt8* pBuffer);

void gpRandom_Init(void);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPRANDOM_H_


