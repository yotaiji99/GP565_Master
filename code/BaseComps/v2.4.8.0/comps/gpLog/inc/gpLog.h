/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpLog.h
 *
 * This file contains the API definition of the logging module (printfs and asserts).
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpLog/inc/gpLog.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPLOG_H_
#define _GPLOG_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
    
#include "global.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define GP_LOG_PSTR(s) (FLASH_STRING)GEN_STR2FLASH(s)

#define GP_LOG_SPLIT_UINT32(i) (UInt16)(((i)&0xFF00)>>8), (UInt16)((i)&0x00FF)

#if defined(GP_DIVERSITY_LOG) && defined(GP_LOCAL_LOG)
#define GP_LOG_PRINTF(s, ...) gpLog_Printf (GP_COMPONENT_ID, true, GP_LOG_PSTR(s),__VA_ARGS__)
#else //defined(GP_DIVERSITY_LOG) && defined(GP_LOCAL_LOG)
#ifndef HAVE_NO_VARIABLE_LENGTH_MACRO
#define GP_LOG_PRINTF(s, ...)
#else
#define GP_LOG_PRINTF
#endif //HAVE_NO_VARIABLE_LENGTH_MACRO
#endif //defined(GP_DIVERSITY_LOG) && defined(GP_LOCAL_LOG)

#ifdef GP_DIVERSITY_LOG
#define GP_LOG_SYSTEM_PRINTF(s,...) gpLog_Printf(GP_COMPONENT_ID, true, GP_LOG_PSTR(s) , __VA_ARGS__)
#else //GP_COMP_LOG
#ifndef HAVE_NO_VARIABLE_LENGTH_MACRO
#define GP_LOG_SYSTEM_PRINTF(s,...)
#else
#define GP_LOG_SYSTEM_PRINTF
#endif //HAVE_NO_VARIABLE_LENGTH_MACRO
#define gpLog_PrintBuffer(length, pData)
#define gpLog_Flush()
#endif //GP_DIVERSITY_LOG

#define gpLog_PrintfEnable(enable)
/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
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


#ifdef __cplusplus
extern "C" {
#endif

/**
*   Initialises the logging module
*
*   @param    void
*   @return   void
*/
GP_API void gpLog_Init( void );

/**
*   Prints a string with formats through the debug interface.
*
*   @param    char *format_str
*   @return   void
*/
#ifdef __GNUC__
GP_API void gpLog_Printf(UInt8 componentID, Bool progmem , FLASH_STRING format_str, UInt8 length , ...)  __attribute__ ((format (printf, 3, 5)));
#else
GP_API void gpLog_Printf(UInt8 componentID, Bool progmem , FLASH_STRING format_str, UInt8 length , ...);
#endif

#ifdef GP_DIVERSITY_LOG
/**
*   Prints a data buffer in hex format.
*
*   @param    pData    pointer to the buffer
*   @param    length   length to be printed
*   @return   void
*/
GP_API void gpLog_PrintBuffer(UInt8 length, UInt8* pData);

/**
*   Flush any pending logging.
*
*   @param    void
*   @return   void
*/
GP_API void gpLog_Flush(void);


#endif //GP_DIVERSITY_LOG

#ifdef __cplusplus
}
#endif


#endif  // _GPLOG_H_

