/*
 * Copyright (c) 2008-2013, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpUtils/src/gpUtils_crc.c#1 $
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

#ifdef GP_DIVERSITY_AVR
#include <util/crc16.h>
#endif

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

#ifndef GP_DIVERSITY_AVR
//CRC-16-IBM - x16 + x15 + x2 + 1
//#define GP_UTILS_DIVERSITY_CRC_TABLE
static INLINE UInt16 _crc16_update(UInt16 crc, UInt8 a)
{
    int i;

    crc ^= a;
    for (i = 0; i < 8; ++i)
    {
        if (crc & 1)
            crc = (crc >> 1) ^ 0xA001;
        else
            crc = (crc >> 1);
    }
    
    return crc;
}
#endif //GP_DIVERSITY_AVR

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpUtils_CalculatePartialCrc(UInt16* pCrcValue, UInt8* pData, UInt16 length)
{
    while(length--)
    {
        *pCrcValue = _crc16_update(*pCrcValue, *pData);
        pData++;
    }
}

UInt16 gpUtils_CalculateCrc(UInt8* pData , UInt16 length)
{
    UInt16 crc_value = 0;
    gpUtils_CalculatePartialCrc(&crc_value, pData, length);

    return crc_value;
}

void gpUtils_UpdateCrc(UInt16* pCRCValue, UInt8 byte)
{
    gpUtils_CalculatePartialCrc(pCRCValue, &byte, 1);
}




