/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpLog.c
 *
 * This file contains the implementation of the logging module (printfs and asserts).
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpLog/src/gpLog.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_LOG
#define GP_MODULE_ID GP_MODULE_ID_LOG

#include "hal.h"
#include "hal_WB.h" //Location of Time macro

#include "gpLog.h"
#include "gpCom.h"

#include "gpAssert.h"

#include <stdarg.h>             // va_arg()
#include <string.h>             // memmove()
#include <stdio.h>              // memmove()
#include <stddef.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifdef GP_BIG_ENDIAN
#define LOG_ENDIANESS gpLog_ParameterEndianessBig
#else
#define LOG_ENDIANESS gpLog_ParameterEndianessLittle
#endif

/* log string length can be overruled from application code. */
#ifndef GP_LOG_MAX_LEN
#define GP_LOG_MAX_LEN                  (80+3/*CommandID + ModuleID + formatStringMode*/)
#endif /* GP_LOG_MAX_LEN */
#define GP_LOG_HEADER_LENGTH            (5+4)

//gcc does not like va_arg(short), even if short=int ->promote
//if va_arg(short) is reached, it will abort.
#define va_arg_promoted(args, type)      \
    (sizeof(type) > sizeof(unsigned int) \
     ? va_arg(args, type)                \
     : va_arg(args, unsigned int))

#ifndef GP_LOG_COMMUNICATION_ID
#define GP_LOG_COMMUNICATION_ID                  GP_COM_DEFAULT_COMMUNICATION_ID
#endif

#define COM_DATA_REQUEST(length, pData) GP_COM_DATA_REQUEST(length,pData,GP_LOG_COMMUNICATION_ID)
#define COM_TX_ENABLED()                gpCom_GetTXEnable()
#define COM_FLUSH()                     gpCom_Flush()

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpLog_FormatStringModeString        0
#define gpLog_FormatStringModePointer       1
typedef UInt8 gpLog_FormatStringMode_t;

#define gpLog_ParameterEndianessLittle  0
#define gpLog_ParameterEndianessBig     1
typedef UInt8 gpLog_ParameterEndianess_t;

#define gpLog_CommandIDPrintfIndication         0x02
typedef UInt8 gpLog_CommandID_t;

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static UInt8 gpLog_PrintfTotalCnt = 0;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/
/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpLog_Init(void)
{
    HAL_DISABLE_GLOBAL_INT();
    gpLog_PrintfTotalCnt = 0;
    HAL_ENABLE_GLOBAL_INT();
}



static void Log_SizeErr(UInt8 componentID, FLASH_STRING format_str,Bool progmem)
{
    Char log_strng[21]="ExtSize:";
    if (progmem)
    {
        STRNCPY_P(&log_strng[8], format_str,sizeof(log_strng)-8-1);
    }
    else
    {
        const char * ramfmtstr = (const char *) format_str;
        STRNCPY(&log_strng[8], ramfmtstr,sizeof(log_strng)-8-1);
    }
    log_strng[sizeof(log_strng)-1]='\0';
    gpLog_Printf(componentID,false,(FLASH_STRING)log_strng, 0);
}


void gpLog_Printf(UInt8 componentID, Bool progmem , FLASH_STRING format_str, UInt8 length , ...)
{
    Char  log_strng[GP_LOG_MAX_LEN];
    UInt8  nbr_chars = 0;
    UInt32 time = 0;

    UInt8* pData;
    Char * pFmt;
    UInt8  formatLoopCnt = 0;

    UInt8  format_str_length;

    va_list  args;


    // When gpCom module is disabled, then return directly (avoid build of the printf)
    if (!COM_TX_ENABLED())
        return;

    HAL_TIMER_GET_CURRENT_TIME(time);
    HOST_TO_LITTLE_UINT32(&time);

    //Get length of format string - can be done non-atomic
    if (progmem)
    {
        format_str_length = STRLEN_P(format_str) + 1 /* NULL character */;
    }
    else
    {
        const char * ramfmtstr = (const char *) format_str;
        format_str_length = STRLEN(ramfmtstr) + 1 /* NULL character */;
    }

    //Check if initial header + format string still fits
    if ((GP_LOG_HEADER_LENGTH + format_str_length) > sizeof(log_strng))
        goto overflow;


    //Construction of header
    log_strng[nbr_chars++] = gpLog_CommandIDPrintfIndication;
    log_strng[nbr_chars++] = componentID;
    log_strng[nbr_chars++] = gpLog_FormatStringModeString | (LOG_ENDIANESS << 1);
    log_strng[nbr_chars++] = 0; //Former interval counter
    HAL_DISABLE_GLOBAL_INT();    
    gpLog_PrintfTotalCnt++;
    log_strng[nbr_chars++] = gpLog_PrintfTotalCnt;
    HAL_ENABLE_GLOBAL_INT();    
    MEMCPY(&(log_strng[nbr_chars]),&time,4);
    nbr_chars += 4;

    //Copy format string
    if (progmem)
    {
        MEMCPY_P(&(log_strng[nbr_chars]), format_str, format_str_length);
    }
    else
    {
        const char * ramfmtstr = (const char *) format_str;
        MEMCPY(&(log_strng[nbr_chars]), ramfmtstr, format_str_length);
    }

    //Copy variables to be filled in
    pFmt = (Char*)&(log_strng[nbr_chars]);
    nbr_chars += format_str_length;

    va_start(args, length);
    formatLoopCnt = 0;
    //local pointer pData as UInt8* because casting log_strng[..] to LOG_TYPE_FROM_STACK aligns potential unaligned address
    //copy args to the pData location done via a local variable of type LOG_TYPE_FROM_STACK and a MEMCPY
    pData = (UInt8*)&(log_strng[nbr_chars]);
    while (pFmt[formatLoopCnt])
    {
        if (pFmt[formatLoopCnt] == '%')
        {
            if (pFmt[formatLoopCnt+1] == 'l')
            {
                UInt32 arg32;
                if ((nbr_chars + sizeof(UInt32)) > sizeof(log_strng))
                {
                    goto overflow;
                }
                arg32= va_arg_promoted(args, UInt32);
                MEMCPY(pData,&arg32,sizeof(UInt32));
                pData += sizeof(UInt32);
                nbr_chars += sizeof(UInt32);
            }
            else
            {
                UInt16 arg16;
                if ((nbr_chars + sizeof(UInt16)) > sizeof(log_strng))
                {
                    goto overflow;
                }
                arg16 = va_arg_promoted(args, UInt16);
                MEMCPY(pData,&arg16,sizeof(UInt16));
                pData += sizeof(UInt16);
                nbr_chars += sizeof(UInt16);
            }
        }
        formatLoopCnt++;
    }
    va_end(args);

    //Tranmsit log string
    COM_DATA_REQUEST(nbr_chars,(UInt8*)log_strng);

    return;

overflow:
    Log_SizeErr(componentID, format_str, progmem);

    GP_ASSERT_DEV_INT(false);
    return;
}

void gpLog_Flush(void)
{
    COM_FLUSH();
}

void gpLog_PrintBuffer(UInt8 length, UInt8* pData)
{
    UIntLoop k;
    for (k = 0; k < length; k+=8)
    {
        GP_LOG_SYSTEM_PRINTF("%i: %x %x %x %x",10,(UInt16)k,(UInt16)((UInt16)pData[0+k] << 8)+(UInt16)pData[1+k],
                                                            (UInt16)((UInt16)pData[2+k] << 8)+(UInt16)pData[3+k],
                                                            (UInt16)((UInt16)pData[4+k] << 8)+(UInt16)pData[5+k],
                                                            (UInt16)((UInt16)pData[6+k] << 8)+(UInt16)pData[7+k]);
        gpLog_Flush();
    }
}


