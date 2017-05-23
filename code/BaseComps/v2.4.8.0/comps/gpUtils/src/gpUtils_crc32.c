/*
 * Copyright (c) 2011,2013-2014, GreenPeak Technologies
 *
 * gpUtils_crc.c
 *   
 * This file contains the crc features of the Utils component.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_crc32.c#1 $
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

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static INLINE void  _crc32_init(UInt32 crc)
{
    asm volatile ("clu.ddss #(0), %0, %0\n\t" /*(0) - INIT_CRC*/
                  : /*no output */
                  : "r" (crc));
}
static INLINE UInt32 _crc32_update(UInt32 crc, UInt8 a)
{
    UInt32 __updated;    

    asm volatile ("clu.ddss #(4), %0, %1\n\t" /*(4) - NEXT_CRC32_1B_LSB_FIRST*/
                  : "=r" (__updated)
                  : "r" (a)
                  : "cc");

    return __updated;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpUtils_CalculatePartialCrc32 (UInt32* pCrcValue, UInt8* pData, UInt16 length) 
{
    _crc32_init(*pCrcValue);
    while(length--)
    { 
        *pCrcValue = _crc32_update(*pCrcValue, *pData);
        pData++;
    }
} 

UInt32 gpUtils_CalculateCrc32(UInt8* pData , UInt16 length)
{
    UInt32 crc_value = GP_UTILS_CRC32_INITIAL_REMAINDER;
    gpUtils_CalculatePartialCrc32(&crc_value, pData, length);

    return (crc_value ^ GP_UTILS_CRC32_FINAL_XOR_VALUE);
}

void gpUtils_UpdateCrc32(UInt32* pCRCValue, UInt8 byte)
{
    gpUtils_CalculatePartialCrc32(pCRCValue, &byte, 1);
}

