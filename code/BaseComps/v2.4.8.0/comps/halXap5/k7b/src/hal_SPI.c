/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *   Hardware Abstraction Layer for the SPI master interface on Xap5 devices.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/k7b/src/hal_SPI.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define COMPLETE_MAP
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

#include "hal.h"
#include "gpHal_reg.h"
#include "gpHal_HW.h"
#include "gpAssert.h"
#include "gpBsp.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef HAL_SPI_MAPPING_SCLK
#define HAL_SPI_MAPPING_SCLK   GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif //HAL_SPI_MAPPING_CLK

#ifndef HAL_SPI_MAPPING_MOSI
#define HAL_SPI_MAPPING_MOSI  GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif //HAL_SPI_MAPPING_MOSI

#ifndef HAL_SPI_MAPPING_MISO
#define HAL_SPI_MAPPING_MISO  GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif //HAL_SPI_MAPPING_MISO

#define HAL_SPI_MAX_CLK_FREQ  8000000UL

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void halSPI_FlushRx(void)
{
    while (GP_WB_READ_SPI_M_UNMASKED_RX_NOT_EMPTY_INTERRUPT())
    {
        GP_WB_READ_SPI_M_RX_DATA_0();
    }
    GP_WB_SPI_M_CLEAR_RX_OVERRUN_INTERRUPT();
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitSPI(UInt32 frequency, UInt8 mode, Bool lsbFirst)
{
    UInt8 clkSetting;

    //Only factor 2 frequencies available: 8MHz, 4MHz, 2MHz, ... freq will be rounded down to the nearest
    GP_ASSERT_DEV_EXT(frequency <= (HAL_SPI_MAX_CLK_FREQ));
    GP_ASSERT_DEV_EXT(frequency >= (HAL_SPI_MAX_CLK_FREQ >> 7)); //62500 Hz

    GP_WB_WRITE_SPI_M_MOSI_MAP(HAL_SPI_MAPPING_MOSI);
    GP_WB_WRITE_SPI_M_SCK_MAP(HAL_SPI_MAPPING_SCLK);
    GP_WB_WRITE_SPI_M_MISO_MAP(HAL_SPI_MAPPING_MISO);

    //External pull-up expected for MISO line - not enabling any of the internal pull-ups
    if(HAL_SPI_MAPPING_MISO == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A)
    {
        GP_WB_WRITE_IOB_GPIO_1_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    }
    else if(HAL_SPI_MAPPING_MISO == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_B)
    {
        GP_WB_WRITE_IOB_GPIO_6_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    }
    else if(HAL_SPI_MAPPING_MISO == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_C)
    {
        GP_WB_WRITE_IOB_GPIO_10_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    }
    else if(HAL_SPI_MAPPING_MISO == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_D)
    {
        GP_WB_WRITE_IOB_GPIO_15_CFG(GP_WB_ENUM_GPIO_MODE_PULLUP);
    }
    else
    {
        GP_ASSERT_DEV_EXT(false);
    }

    //Only use SW controlled SSN pin
    GP_WB_WRITE_SPI_M_SS_MAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);

    //Set SPI clk speed
    clkSetting = 0;
    while(frequency < HAL_SPI_MAX_CLK_FREQ)
    {
        frequency *= 2;
        clkSetting++;
    }
    GP_WB_WRITE_SPI_M_SCLK_FREQ(clkSetting); //8MHz / 2**sclk_freq

    //Configure SPI settings
    GP_WB_WRITE_SPI_M_DATA_BITS(8-1);
    GP_WB_WRITE_SPI_M_LSB_FIRST(lsbFirst);
    GP_WB_WRITE_SPI_M_MODE(mode);

    //Flush any pending Rx bytes
    halSPI_FlushRx();
}


UInt8 hal_WriteReadSPI(UInt8 byte)
{
    GP_WB_WRITE_SPI_M_TX_DATA_0(byte);
    // Wait until Rx fifo not empty
    while (!GP_WB_READ_SPI_M_UNMASKED_RX_NOT_EMPTY_INTERRUPT());

    // Return Rx fifo content
    return GP_WB_READ_SPI_M_RX_DATA_0();
}

void hal_WriteStreamSPI(UInt8 length, UInt8* pData)
{
    UInt32 startAddress = (UIntPtr)pData;

    //Setup DMA
    GP_WB_WRITE_DMA_SRC_START_ADDR(startAddress);
    GP_WB_WRITE_DMA_SRC_BUFFER_SIZE(length-1);

    GP_WB_WRITE_DMA_DEST_START_ADDR(GP_WB_SPI_M_TX_DATA_0_ADDRESS);
    GP_WB_WRITE_DMA_DEST_BUFFER_SIZE(0);

    GP_WB_DMA_RESET_POINTERS();

    GP_WB_WRITE_DMA_BUFFER_PTR_VALUE(0x0);
    GP_WB_WRITE_DMA_BUFFER_PTR_WRAP_VALUE(1);
    GP_WB_DMA_SET_SRC_WRITE_PTR();

    GP_WB_DMA_CLR_SRC_UNDERRUN_INTERRUPT();
    //Start DMA
    GP_WB_WRITE_DMA_CPY_TRIGGER_SRC_SELECT(GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_M_TX_NOT_FULL);

    //Stop DMA when buffer is done
    while(!GP_WB_READ_DMA_UNMASKED_SRC_UNDERRUN_INTERRUPT());
    GP_WB_WRITE_DMA_CPY_TRIGGER_SRC_SELECT(GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_NO_TRIGGER_SRC);
    GP_WB_DMA_CLR_SRC_UNDERRUN_INTERRUPT();

    //Wait for SPI to finish
    while (!GP_WB_READ_SPI_M_UNMASKED_NOT_BUSY_INTERRUPT());
    halSPI_FlushRx();

}

void hal_EnableFreeRunningSPI(Bool wordMode, UInt8 length, UInt8* pData)
{
    UInt8 lengthCorrection;
    UInt32 startAddress = (UIntPtr)pData;

    lengthCorrection = wordMode ? sizeof(UInt16): sizeof(UInt8);

    //configure RxBuffer for DMA
    GP_WB_WRITE_DMA_SRC_START_ADDR(GP_WB_SPI_M_RX_DATA_0_ADDRESS);
    GP_WB_WRITE_DMA_CPY_TRIGGER_SRC_SELECT(GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_M_RX_NOT_EMPTY);
    GP_WB_WRITE_DMA_WORD_MODE(wordMode); // 16-bit mode

    GP_WB_WRITE_DMA_SRC_BUFFER_SIZE(0);
    GP_WB_WRITE_DMA_DEST_START_ADDR(startAddress);              // RAM_MEMS_BUFFER_START_ADDRESS
    GP_WB_WRITE_DMA_DEST_BUFFER_SIZE(length-lengthCorrection);

    GP_WB_DMA_RESET_POINTERS();
    GP_WB_WRITE_DMA_DEST_BUFFER_ALMOST_FULL_THRESHOLD((length-lengthCorrection)/2);

    /* Configure the MASTER SPI, this code does not affect slave operation, but if using slave could be removed to reduce power */
    hal_InitSPI(1000000, 0, true);    // Initialize SPI @ 1MHz clock
    GP_WB_WRITE_SPI_M_FREE_RUNNING(true);
    GP_WB_WRITE_SPI_M_TX_DATA_0(0);   // 1st write to start clock
    GP_WB_WRITE_SPI_M_DATA_BITS(15);
    GP_WB_WRITE_SPI_M_LSB_FIRST(false);
    // start in a clean state. Make sure receive buffer is empty and overrun interrupt is cleared
    GP_WB_READ_SPI_M_RX_DATA_0();
    GP_WB_SPI_M_CLEAR_RX_OVERRUN_INTERRUPT();

    // kickstart the SPI in free running mode
    GP_WB_WRITE_SPI_M_TX_DATA_0(0) ;

}

void hal_DisableSPI(void)
{
    GP_WB_WRITE_SPI_M_MOSI_MAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_SPI_M_SCK_MAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_SPI_M_MISO_MAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
    GP_WB_WRITE_SPI_M_SS_MAP(GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED);
}

