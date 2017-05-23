/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 * gpHal_MISC.c
 *
 * This file contains miscellaneous functions
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gphal/k7b/src/gpHal_MISC.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpHal.h"
#include "gpHal_DEFS.h"
#include "gpHal_MISC.h"

//GP hardware dependent register definitions
#include "gpHal_HW.h"
#include "gpHal_reg.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_GPHAL

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

//-------------------------------------------------------------------------------------------------------
//  ADC FUNCTIONS
//-------------------------------------------------------------------------------------------------------


UInt8 gpHal_GetADCValue(gpHal_ADCSource_t source, UInt8 measurementConfig)
{
    return hal_MeasureADC_8(source, measurementConfig);
}


//-------------------------------------------------------------------------------------------------------
//  RANDOM GENERATION FUNCTIONS
//-------------------------------------------------------------------------------------------------------

UInt8 gpHal_GetRandomSeed(void)
{
    UInt8 seed;

    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_OUT_OF_LOCK_INTERRUPT(0);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_CP_OUT_OF_RANGE_INTERRUPT(0);

    //Enable Pseudo random block
    GP_WB_WRITE_PRG_ENABLE_CLK_PRG_BY_UC(1);

    //Toggle radio (on unused channel) to generate I/Q sample for PRG
    GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH5(1);
    HAL_WAIT_US(100);
    GP_WB_WRITE_RIB_RX_ON_WHEN_IDLE_CH5(0);

    seed = GP_WB_READ_PRG_RANDOM_VALUE();

    //Disable Pseudo random block
    GP_WB_WRITE_PRG_ENABLE_CLK_PRG_BY_UC(0);

    GP_WB_TRX_FLL_CP_OUT_OF_RANGE_CLR_INT();
    GP_WB_TRX_FLL_OUT_OF_LOCK_CLR_INT();

    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_OUT_OF_LOCK_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_PHY_FLL_CP_OUT_OF_RANGE_INTERRUPT(1);

    return seed;
}

#define ISCMP(l) ((l)<0)
gpHal_GetAttrResult_t ParseAttr(UInt32 offset, UInt8 expLength, Int8 length, UInt8* pData)
{
    if ISCMP(length)
    {
        UIntLoop i;
        UInt8 loopEnd = -length;

        if (pData[loopEnd-1] == 0) // compare attr can be zero terminating string
        {
            loopEnd--;
        }

        if (loopEnd > expLength)
        {
            return gpHal_GetAttrResultBufferTooBig;
        }

        //Byte-wise comparison
        for(i = 0; i<loopEnd; i++)
        {
            UInt8 cmpData;

            gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_START + offset + i, 1, &cmpData);
            if (cmpData != pData[i])
            {
                return (-1-i);
            }
        }
        return 0;
    }
    else
    {
        if (length < expLength)
        {
            return gpHal_GetAttrResultBufferTooSmall;
        }
        gpHal_FlashReadInf(GP_MM_FLASH_INF_PAGE_START + offset, expLength, pData);

        return expLength;
    }
}

gpHal_GetAttrResult_t gpHal_ParseAttr(UInt8 attrId, Int8 length, UInt8 *pData)
{
    gpHal_GetAttrResult_t result = 0;
    switch(attrId)
    {
        case 0:
        case 1:
        {
            result = ParseAttr(0x100 + 0x0, 10, length, pData);
            break;
        }
        case 2:
        {
            result = ParseAttr(0x100 + 0x50, 16, length, pData);
            break;
        }
        case 3:
        {
#define magicWord (*((UInt32*)buffer))
#define ptr       (buffer[4])
            /* Read the magic word info */
            UInt8 buffer[5];
            gpHal_FlashReadInf(GP_MM_FLASH_TRACKING_START, sizeof(buffer), buffer);

            /* Verify tracking info is written */
            if(magicWord != GP_HAL_TRACKING_INFO_MAGIC_WORD)
            {
                MEMSET(pData, 0, length);
                return gpHal_GetAttrResultDataError;
            }

            /* Only 6 fields available, last one will be overwritten if needed. Clip the pointer to access the correct field */
            if(ptr > 6)
            {
                ptr = 6;
            }

            /* Read the tracking info */
            result = ParseAttr(ptr * 8, length, length, pData);
#undef magicWord
#undef ptr
            break;
        }
        default:
        {
            result = gpHal_GetAttrResultUnkownAttr;
            break;
        }
    }

    return result;
}


