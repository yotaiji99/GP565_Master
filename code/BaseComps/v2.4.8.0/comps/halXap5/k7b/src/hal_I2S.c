/*
 * Copyright (c) 20146, GreenPeak Technologies
 *
 *   Hardware Abstraction Layer for the I2S interface on Xap5 devices.
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/halXap5/k7b/src/hal_I2S.c#2 $
 *    M'   GreenPeak              $Change: 78203 $
 *   0'         Technologies      $DateTime: 2016/03/10 09:15:29 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

#include "hal.h"
#include "gpHal_reg.h"
#include "gpHal_HW.h"

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/
void hal_ConfigureDmaForI2S(Bool wordMode, UInt8 length, UInt8* pData)
{
    UInt8 lengthCorrection;
    UInt32 startAddress = (UIntPtr)pData;

    lengthCorrection = wordMode ? sizeof(UInt16): sizeof(UInt8);

    /* Configure RxBuffer for DMA */
    // Configure data source for DMA
    GP_WB_WRITE_DMA_SRC_START_ADDR(GP_WB_SPI_SL_RX_DATA_ADDRESS);
    // DMA source interrupt trigger
    GP_WB_WRITE_DMA_CPY_TRIGGER_SRC_SELECT(GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_SL_RX_NOT_EMPTY);
    // If wordMode is true, use 16 bit words, otherwise use 8 bit words
    GP_WB_WRITE_DMA_WORD_MODE(wordMode);
    // Start from fixed address 
    GP_WB_WRITE_DMA_SRC_BUFFER_SIZE(0);
    // Configure data destination for DMA
    GP_WB_WRITE_DMA_DEST_START_ADDR(startAddress);
    GP_WB_WRITE_DMA_DEST_BUFFER_SIZE(length-lengthCorrection);
    // Set DMA threshold level
    GP_WB_WRITE_DMA_DEST_BUFFER_ALMOST_FULL_THRESHOLD((length-lengthCorrection)/2);

    HAL_DISABLE_GLOBAL_INT();

    /* Start in a clean state. Make sure receive buffer is empty and overrun interrupt is cleared */
    GP_WB_DMA_CLR_CPY_ERR_INTERRUPT();
    GP_WB_DMA_CLR_DEST_OVERFLOW_INTERRUPT();
    GP_WB_DMA_CLR_SRC_UNDERRUN_INTERRUPT();
    GP_WB_WRITE_INT_CTRL_MASK_DMA_CPY_ERR_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_DMA_DEST_OVERFLOW_INTERRUPT(1);
    GP_WB_WRITE_INT_CTRL_MASK_DMA_DEST_ALMOST_FULL_INTERRUPT(1);
    /* Reset cyclic buffer ptr */
    GP_WB_DMA_RESET_POINTERS();

    HAL_ENABLE_GLOBAL_INT();
}
