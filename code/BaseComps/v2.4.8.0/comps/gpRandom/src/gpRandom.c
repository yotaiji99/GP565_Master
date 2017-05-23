/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 * 
 * This file implements a random number generator
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpRandom/src/gpRandom.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

// General includes
#include "gpRandom.h"
#include "gpSched.h"
#include "gpHal.h"


/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_RANDOM

#ifndef GP_RANDOM_RANDOMNES_STRENGTH_OCTETS
#define GP_RANDOM_RANDOMNES_STRENGTH_OCTETS             16UL
#endif /* GP_RANDOM_RANDOMNES_STRENGTH_OCTETS */
#define GP_RANDOM_RANDOMNESS_STRENGTH_WORDS             ((GP_RANDOM_RANDOMNES_STRENGTH_OCTETS + 3)>>2)
/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
static void Random_MashUpSeed(void); 
static void Random_PseudoAdvance(void);
static UInt32 Random_PseudoRead32(void);
static void Random_GetLastSequence(UInt8* pBuffer, UInt8 nmbrRandomBytes);
/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
static UInt32 Random_Seed[GP_RANDOM_RANDOMNESS_STRENGTH_WORDS];
static UInt8 RandomSeedWordOffset = 0;
static UInt32 Random_m_w = 521288629UL; /* https://groups.google.com/forum/?fromgroups#!topic/sci.crypt/yoaCpGWKEk0[1-25] */
static UInt32 Random_m_z = 362436069UL; /* https://groups.google.com/forum/?fromgroups#!topic/sci.crypt/yoaCpGWKEk0[1-25] */
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
extern UInt8 gpHal_GetRandomSeed(void) ;

 void Random_MashUpSeed(void)
{
    UInt8 seedWordCount;
#ifndef GP_RANDOM_NO_SEED_SHUFFLE
    UInt32 hwTime;
    hwTime=gpSched_GetTime();
    RandomSeedWordOffset = (UInt8)(hwTime % GP_RANDOM_RANDOMNESS_STRENGTH_WORDS);
#else /* !GP_RANDOM_NO_SEED_SHUFFLE */
    RandomSeedWordOffset = (RandomSeedWordOffset++)%GP_RANDOM_RANDOMNESS_STRENGTH_WORDS;
#endif /* GP_RANDOM_NO_SEED_SHUFFLE */
    for(seedWordCount = 0; seedWordCount < GP_RANDOM_RANDOMNESS_STRENGTH_WORDS; seedWordCount++)
    {
        Random_PseudoAdvance();
        Random_Seed[(RandomSeedWordOffset+seedWordCount)%GP_RANDOM_RANDOMNESS_STRENGTH_WORDS] ^= Random_PseudoRead32();
    }
} 

void Random_PseudoAdvance(void) {
    Random_m_z = 36969 * (Random_m_z & 65535) + (Random_m_z >> 16); 
    Random_m_w = 18000 * (Random_m_w & 65535) + (Random_m_w >> 16); 
}

UInt32 Random_PseudoRead32(void) {
    return (Random_m_z << 16) + Random_m_w;
}

void Random_GetLastSequence(UInt8* pBuffer, UInt8 nmbrRandomBytes)
{
    UInt8 randomByteIndex;
    
    for(randomByteIndex=0; randomByteIndex < nmbrRandomBytes; randomByteIndex++)
    {
        pBuffer[randomByteIndex] = ((UInt8*)Random_Seed)[((RandomSeedWordOffset<<2) + randomByteIndex)%(GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2)];
    }
} 

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/
void gpRandom_RandomizeSeed(void)
{
    UInt8 seedByteIndex;

    gpHal_GoToSleepWhenIdle(false);
    for(seedByteIndex = 0; seedByteIndex < (GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2); seedByteIndex++)
    {
        ((UInt8*)Random_Seed)[seedByteIndex] = gpHal_GetRandomSeed();
    }
    gpHal_GoToSleepWhenIdle(true);


}

void gpRandom_GetNewSequence(UInt8 nmbrRandomBytes, UInt8* pBuffer)
{
    UInt8 randomBytesGenerated = 0;
    
    while(randomBytesGenerated < nmbrRandomBytes)
    {
        UInt8 bytesToGenerate = (GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2) < (nmbrRandomBytes - randomBytesGenerated) ? 
                                (GP_RANDOM_RANDOMNESS_STRENGTH_WORDS<<2) : (nmbrRandomBytes - randomBytesGenerated);
        Random_MashUpSeed();
        Random_GetLastSequence(&pBuffer[randomBytesGenerated], bytesToGenerate);
        randomBytesGenerated += bytesToGenerate;
    }
}

void gpRandom_Init(void)
{
}

