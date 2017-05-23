/*
 * Copyright (c) 2010-2014, GreenPeak Technologies
 *
 * gpAssert.c
 *   This file contains the implementation of the Assert functions.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpAssert/src/gpAssert.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */


/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_ASSERT

#include "gpAssert.h"
#include "hal.h"
#ifdef GP_DIVERSITY_LOG
#include "gpLog.h"
#endif //GP_DIVERSITY_LOG
#ifdef GP_DIVERSITY_ASSERT_ACTION_RESET
#include "gpReset.h"
#endif //GP_DIVERSITY_ASSERT_ACTION_RESET

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
#define GP_ASSERT_DIVERSITY_LED_DEBUG_MAGIC_WORD 0x72
#endif

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
static void Assert_blinkLedAssert(UInt16 number);
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/
#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
gpAssert_cbAssertIndication_t gpAssert_cbAssertIndication = NULL;
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
static Bool gpAssert_DebugEnabled = 0;
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

#ifdef GP_DIVERSITY_LOG
FLASH_STRING gpAssert_filename;
UInt16       gpAssert_line;
#endif //GP_DIVERSITY_LOG

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
#if defined(GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG)
static void Assert_blinkLedAssert(UInt16 number)
{
    UIntLoop i = 5;

    HAL_LED_SET(RED);
    while(i)
    {
        UInt16 subtractor= 0;
        UIntLoop j;
        
        switch(i)
        {
            case 1:
                subtractor = 1;
                break;
            case 2:
                subtractor = 10;
                break;
            case 3:
                subtractor = 100;
                break;
            case 4:
                subtractor = 1000;
                break;
            case 5:
                subtractor = 10000;
                break;
        }

        for(j=0; j< 10; j++)
        {
            if(number >= subtractor)
            {
                HAL_LED_SET(GRN);
                number-=subtractor;
            }
            else
            {
                HAL_LED_CLR(GRN);
            }
            HAL_WAIT_MS(1);
            HAL_LED_CLR(GRN);
            HAL_WAIT_MS(1);
        }

        i--;
    }
    HAL_LED_CLR(RED);
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

// legacy
void gpAssert_Stop(void)
{
    //Performs the end loop with LED blink
    volatile Bool EndLessLoop = true;

    HAL_FORCE_ENABLE_GLOBAL_INT();

    // When the debugger reset the variable EndLessLoop, the CPU can return to the position, where the assert was called
    while (EndLessLoop)
    {
        //toggle green led
        HAL_WDT_RESET();
        HAL_LED_TGL(GRN);
        HAL_WAIT_MS(500);
#ifdef GP_DIVERSITY_LOG
        //Flush out any pending messages
        gpLog_Flush();
#endif //GP_DIVERSITY_LOG
    }
}

#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
void gpAssert_DoAssertHandling_Extended(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line)
{
    GP_ASSERT_REPORT(assertInfo, componentId, filename, line);
    GP_ASSERT_FOLLOW_UP_ACTION();
}
#else
void gpAssert_DoAssertHandling_Basic(UInt8 componentId, FLASH_STRING filename, UInt16 line)
{
    GP_ASSERT_REPORT(0, componentId, filename, line);
    GP_ASSERT_FOLLOW_UP_ACTION();
}
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER

#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
void gpAssert_RegisterCbAssertIndication(gpAssert_cbAssertIndication_t cbAssertIndication)
{
    gpAssert_cbAssertIndication = cbAssertIndication;
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
void gpAssert_EnableDebug(void)
{
    gpAssert_DebugEnabled = GP_ASSERT_DIVERSITY_LED_DEBUG_MAGIC_WORD;
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// reporting
#ifdef GP_DIVERSITY_ASSERT_REPORTING_CALLBACK
void gpAssert_CallCbAssertIndication(gpAssert_AssertInfo_t assertInfo, UInt8 componentId, FLASH_STRING filename, UInt16 line)
{
    if(gpAssert_cbAssertIndication != NULL)
    {
        gpAssert_NotifyInfo_t notifyInfo;
#ifdef GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
        notifyInfo.level = GP_ASSERT_GET_LEVEL(assertInfo);
        notifyInfo.id = GP_ASSERT_GET_ID(assertInfo);
#endif //GP_DIVERSITY_ASSERT_ENABLE_IDENTIFIER
        notifyInfo.componentId = componentId;
        notifyInfo.lineNumber = line;
        notifyInfo.fileName = filename;
        gpAssert_cbAssertIndication(&notifyInfo);
    }
    else
    {
#if defined(GP_DIVERSITY_LOG)
        // print logging when enabled
        gpAssert_PrintLogString(componentId, filename, line);
#endif //GP_DIVERSITY_LOG
    }
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_CALLBACK

#ifdef GP_DIVERSITY_LOG
void gpAssert_PrintLogString(UInt8 componentID, FLASH_STRING filename, UInt16 line)
{
    Char gpAssert_File[41] = "Assert %i!%i";

    HAL_DISABLE_GLOBAL_INT();

    //Store in static variables for debuggers
    gpAssert_filename = filename;
    gpAssert_line = line;

    //Copy filename after '!'
    STRNCPY_P(&(gpAssert_File[10]), filename,30);
    gpLog_Flush();
    gpLog_Printf(componentID, false , (FLASH_STRING)gpAssert_File,2,line);
    gpLog_Flush();

    HAL_ENABLE_GLOBAL_INT();
    return;
}
#endif //#ifdef GP_DIVERSITY_LOG

#ifdef GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG
void gpAssert_LedDebug(UInt16 line)
{
    HAL_FORCE_DISABLE_GLOBAL_INT();
    if(gpAssert_DebugEnabled  == GP_ASSERT_DIVERSITY_LED_DEBUG_MAGIC_WORD)
    {
#ifdef GP_DIVERSITY_LOG
        //Flush out any pending messages
        gpLog_Flush();
#endif //GP_DIVERSITY_LOG
        volatile Bool EndlessLoop = true;
        while(EndlessLoop)
        {
            HAL_WDT_RESET();
            HAL_WAIT_MS(10);
            Assert_blinkLedAssert(line);
        }
    }
}
#endif //GP_DIVERSITY_ASSERT_REPORTING_LED_DEBUG

// action functions
#ifdef GP_DIVERSITY_ASSERT_ACTION_RESET
void gpAssert_ResetSystem(void)
{
    gpReset_ResetSystem();
}
#endif //GP_DIVERSITY_ASSERT_ACTION_RESET

#ifdef GP_DIVERSITY_ASSERT_ACTION_EXIT
void gpAssert_Exit(void)
{
}
#endif //GP_DIVERSITY_ASSERT_ACTION_EXIT

#ifdef GP_DIVERSITY_ASSERT_ACTION_BLINK_LED
void gpAssert_BlinkLed(void)
{
    volatile Bool EndLessLoop = true;

    //Performs the end loop with LED blink
    HAL_FORCE_ENABLE_GLOBAL_INT();

    // When the debugger reset the variable EndLessLoop, the CPU can return to the position, where the assert was called
    while (EndLessLoop)
    {
        //toggle green led
        HAL_WDT_RESET();
        HAL_LED_TGL(GRN);
        HAL_WAIT_MS(500);
#ifdef GP_DIVERSITY_LOG
        //Flush out any pending messages
        gpLog_Flush();
#endif //GP_DIVERSITY_LOG
    }
}
#endif //GP_DIVERSITY_ASSERT_ACTION_BLINK_LED

