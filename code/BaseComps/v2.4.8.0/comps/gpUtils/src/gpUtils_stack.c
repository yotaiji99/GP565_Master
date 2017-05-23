/*
 * Copyright (c) 2008-2010,2012-2014, GreenPeak Technologies
 *
 *   
 * This file contains the stack features of the Utils component.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_stack.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
     
#define GP_COMPONENT_ID GP_COMPONENT_ID_UTILS

#include "gpUtils.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpWmrk.h" // watermarker monitoring 
/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

UInt16 gpUtils_MaxStackUsage = 0xffff;

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef GP_DIVERSITY_STACK_DBG


#if GP_UTILS_STACK_LOW_TO_HIGH
#define GET_END_OF_STACK(index) ((GP_UTILS_END_OF_STACK-index)[0])
#define GET_STACK_POINTER_IN_RANGE(ptr) (ptr >= GP_UTILS_END_OF_STACK)
#define MOVE_STACK_POINTER_TO_START(ptr) (ptr)-=16
#define STACK_SIZE  (GP_UTILS_END_OF_STACK - GP_UTILS_START_OF_STACK)
#else
#define GET_END_OF_STACK(index) (GP_UTILS_END_OF_STACK[index])
#define GET_STACK_POINTER_IN_RANGE(ptr) (ptr <= GP_UTILS_END_OF_STACK)
#define MOVE_STACK_POINTER_TO_START(ptr) (ptr)+=16
#define STACK_SIZE  (GP_UTILS_START_OF_STACK - GP_UTILS_END_OF_STACK)
#endif

void gpUtils_WmrkSubscribe()
{
}

Bool gpUtils_CheckStackPattern(void)
{
    //Check if stack has reached the last 4 bytes
    if(
        (GET_END_OF_STACK(0) != GP_UTILS_STACK_PATTERN) ||
        (GET_END_OF_STACK(1) != GP_UTILS_STACK_PATTERN) ||
        (GET_END_OF_STACK(2) != GP_UTILS_STACK_PATTERN) ||
        (GET_END_OF_STACK(3) != GP_UTILS_STACK_PATTERN)
    )
    {
        UInt8 * p_this_call = (UInt8 *) &p_this_call; /* Stack pointer before local none-register variables */
        GP_LOG_SYSTEM_PRINTF("stackend %x %x %x %x %x %x",12,p_this_call,GP_UTILS_END_OF_STACK,GET_END_OF_STACK(0),GET_END_OF_STACK(1),GET_END_OF_STACK(2),GET_END_OF_STACK(3));
        return false;
    }
    return true;
}

UInt16 gpUtils_GetMaxStackUsage(void)
{
    UInt16 i =0 ;
    while( GET_END_OF_STACK(i) == GP_UTILS_STACK_PATTERN)
    {
        i++;
    }
    if( i< gpUtils_MaxStackUsage )
    {
        GP_LOG_SYSTEM_PRINTF("Used stack now %i/%i was %i bytes",0,STACK_SIZE-i, STACK_SIZE, STACK_SIZE-gpUtils_MaxStackUsage);
        gpUtils_MaxStackUsage = i;
    }
    return i;
}

Bool gpUtils_CheckStackPointer(void)
{
    UInt8 * p_this_call = (UInt8 *) &p_this_call; /* Stack pointer before local none-register variables */
    if (GET_STACK_POINTER_IN_RANGE(p_this_call))
    {
        GP_LOG_SYSTEM_PRINTF("stackpointer %x %x %x %x %x",10,p_this_call,p_this_call[0],p_this_call[1],p_this_call[2],p_this_call[3]);
        return false;
    }
    return true;
}

UInt8* gpUtils_GetStackPointer(void)
//void gpUtils_DumpStackPointer(void)
{
    UInt8 * p_this_call = (UInt8 *) &p_this_call; /* Stack pointer before local none-register variables */
    return p_this_call;
}

void gpUtils_GetStackPointer2(UInt8** pStack, UInt8** pFilledStack)
//void gpUtils_DumpStackPointer2(UInt16 value)
{
    UInt8 * p_this_call = (UInt8 *) &p_this_call; /* Stack pointer before local none-register variables */
    UInt8*  pStackInUse = GP_UTILS_END_OF_STACK;

    //Start looking for first used stack area
    while (pStackInUse < GP_UTILS_START_OF_STACK)
    {
        if (*pStackInUse != 0x66)
        {
            break;
        }
        MOVE_STACK_POINTER_TO_START(pStackInUse);
    }
    *pStack = p_this_call;
    *pFilledStack = pStackInUse;
}

#ifdef GP_DIVERSITY_LOG
#define GP_UTILS_STACK_TRACK_MAX                  0
#define GP_UTILS_STACK_TRACK_MAX_SLOT             1
#define GP_UTILS_STACK_TRACK_PATTERN_SIZE         2
static UInt8* gpUtils_StackTrackData[GP_UTILS_STACK_TRACK_PATTERN_SIZE] = { (UInt8*)0xFFFF ,
                                                                            (UInt8*)0xFFFF };

void gpUtils_ResetStackTrack(void)
{
    gpUtils_StackTrackData[GP_UTILS_STACK_TRACK_MAX_SLOT]         = (UInt8*)0xFFFF;
}

void gpUtils_StackTrack(void)
{
    UInt8* pStack = gpUtils_GetStackPointer();
    if (pStack < gpUtils_StackTrackData[GP_UTILS_STACK_TRACK_MAX])
        gpUtils_StackTrackData[GP_UTILS_STACK_TRACK_MAX] = pStack;
    if (pStack < gpUtils_StackTrackData[GP_UTILS_STACK_TRACK_MAX_SLOT])
        gpUtils_StackTrackData[GP_UTILS_STACK_TRACK_MAX_SLOT] = pStack;
}

void gpUtils_DumpStackTrack( void )
{
    static UInt8 *pLastFilledStack;
    static UInt8 *LastStackTrackData[GP_UTILS_STACK_TRACK_PATTERN_SIZE];
    UInt8* pStack,*pFilledStack;
    gpUtils_GetStackPointer2(&pStack,&pFilledStack);

    if (pLastFilledStack != pFilledStack ||
        LastStackTrackData[0] != gpUtils_StackTrackData[0] ||
        LastStackTrackData[1] != gpUtils_StackTrackData[1])
        GP_LOG_SYSTEM_PRINTF("stackstat: %x %x %x",6,gpUtils_StackTrackData[0],gpUtils_StackTrackData[1],pFilledStack);

    pLastFilledStack = pFilledStack;
    LastStackTrackData[0] = gpUtils_StackTrackData[0];
    LastStackTrackData[1] = gpUtils_StackTrackData[1];
}
#endif // GP_DIVERSITY_LOG
#endif //GP_DIVERSITY_STACK_DBG


