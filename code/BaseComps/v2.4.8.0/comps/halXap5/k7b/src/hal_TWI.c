/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *   Hardware Abstraction Layer for the TWI master interface on Xap5 devices.
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
 *     0!                         $Header: //depot/main/Embedded/BaseComps/vlatest/sw/comps/halXap5/k7b/src/hal_TWI.c#4 $
 *    M'   GreenPeak              $Change: 59296 $
 *   0'         Technologies      $DateTime: 2014/12/17 15:19:20 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define COMPLETE_MAP
#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

#include "hal.h"
#include "gpLog.h"
#include "gpHal.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#if HAL_TWI_MAPPING_CLK != HAL_TWI_MAPPING_SDA
# error TWI master: different SDA and SCLK mapping not supported at this time
#endif

#ifndef HAL_TWI_MAPPING_SDA
#define HAL_TWI_MAPPING_SDA         GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif

#ifndef HAL_TWI_MAPPING_CLK
#define HAL_TWI_MAPPING_CLK         GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#endif

#ifndef HAL_DEBUG_MAPPING_TWI_CLK
#define HAL_DEBUG_MAPPING_TWI   GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED
#else
#define HAL_DEBUG_MAPPING_TWI   HAL_DEBUG_MAPPING_TWI_CLK
#endif

#ifndef HAL_TWI_CLK_SPEED
#define HAL_TWI_CLK_SPEED       10000UL
#endif

#ifndef HAL_TWI_MAX_WAIT_FOR_INTERRUPT_IN_US_DIV_255
#define HAL_TWI_MAX_WAIT_FOR_INTERRUPT_IN_US_DIV_255   (1000000UL/HAL_TWI_CLK_SPEED) /*waiting max 255 bit clk cycles*/
#endif

#define TWI_PRESCALER           ((3200000UL/HAL_TWI_CLK_SPEED)-1)
#define TWI_MAPPING             HAL_TWI_MAPPING_SDA     // SDA and SCLK mapping are same.

#define ACK  0
#define NACK 1

//Command options bits
#define TWI_CMD_WRITE   GP_WB_TWI_MS_WRITE_MASK
#define TWI_CMD_READ    GP_WB_TWI_MS_READ_MASK
#define TWI_CMD_STOP    GP_WB_TWI_MS_STOP_MASK
#define TWI_CMD_START   GP_WB_TWI_MS_START_MASK

#define GP_WB_TWI_MS_CTR_REGISTER GP_WB_TWI_MS_WRITE_ADDRESS /* One of the properties */
#define GP_WB_WRITE_TWI_MS_CONTROL_ALL(enable, sda_map, sclk_map, ack, clk_sync_dis)    \
    do {                                                                                \
        GP_WB_WRITE_U8(GP_WB_TWI_MS_ENABLE_ADDRESS,                                     \
                (clk_sync_dis) << GP_WB_TWI_MS_CLK_SYNC_DISABLE_LSB |                   \
                (ack)          << GP_WB_TWI_MS_ACK_LSB              |                   \
                (enable)       << GP_WB_TWI_MS_ENABLE_LSB           |                   \
                (sclk_map)     << GP_WB_TWI_MS_SCLK_PINMAP_LSB      |                   \
                (sda_map)      << GP_WB_TWI_MS_SDA_PINMAP_LSB                           \
            );                                                                          \
        GP_WB_WRITE_TWI_MS_SCLK_PINMAP(sclk_map);                                       \
        GP_WB_WRITE_TWI_MS_SDA_PINMAP(sda_map);                                         \
    } while(false)

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static Bool  halTWI_ActivityDetected = false;
static Bool  halTWI_Active           = false;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static Bool halTWI_ExecuteAndWait(UInt8 cmd)
{
    UInt8 i = 0;

    GP_WB_TWI_MS_CLEAR_DONE_INT();
    GP_WB_TWI_MS_CLEAR_ARB_LOST_INT();

    GP_HAL_WRITE_REG(GP_WB_TWI_MS_CTR_REGISTER, cmd);

    //Wait until interrupt is seen
    //We do need a max loop counter to avoid lock ups in case connectivity problems occur or slave device is broken,
    for (i=0; i<255; i++)
    {
        if (GP_WB_READ_TWI_MS_UNMASKED_DONE_INT())
        {
            return true;
        }
        if (GP_WB_READ_TWI_MS_UNMASKED_ARB_LOST_INT())
        {
            return false;
         }
        HAL_WAIT_US(HAL_TWI_MAX_WAIT_FOR_INTERRUPT_IN_US_DIV_255);
    }
    return false;
}

static Bool halTWI_TxByte(UInt8 cmd, UInt8 txByte)
{
    Bool success;

    //Put byte in Tx register
    GP_WB_WRITE_TWI_MS_TX_DATA(txByte);

    success = halTWI_ExecuteAndWait(cmd | TWI_CMD_WRITE); //Write byte

    //Expect an ack to be received
    success &= (GP_WB_READ_TWI_MS_RX_ACK() == ACK);

    return success;
}

static Bool halTWI_RxByte(Bool bLast, UInt8* pData)
{
    UInt8 cmd=0;
    Bool success;

    if (bLast)
    {
        cmd = TWI_CMD_READ | TWI_CMD_STOP; //Read a byte and stop
        GP_WB_WRITE_TWI_MS_ACK(NACK); //No ack to be sent
    }
    else
    {
        cmd = TWI_CMD_READ; //Read a byte
        GP_WB_WRITE_TWI_MS_ACK(ACK); //Ack to be sent
    }
    success = halTWI_ExecuteAndWait(cmd); //Execute command
    // Expect an nack on blast=true and an ack on blast=false to be received
    success &= (GP_WB_READ_TWI_MS_RX_ACK() == (bLast ? NACK : ACK));

    *pData = GP_WB_READ_TWI_MS_RX_DATA();

    return success;
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_InitTWI(void)
{
    if(TWI_MAPPING == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A)
    {
        //Disable interrupt line for external MCU
        GP_WB_WRITE_IOB_MCU_INTOUTN_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
        GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
        GP_WB_WRITE_IOB_GPIO_1_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else if(TWI_MAPPING == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_B)
    {
        GP_WB_WRITE_IOB_GPIO_20_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
        GP_WB_WRITE_IOB_GPIO_21_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else if(TWI_MAPPING == GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_C)
    {
//        GP_WB_WRITE_IOB_GPIO_16_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
//        GP_WB_WRITE_IOB_GPIO_17_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }

    //Do configuration with disabled core
    GP_WB_WRITE_TWI_MS_ENABLE(false);
    GP_WB_WRITE_TWI_MS_PRESCALER(TWI_PRESCALER);
    GP_WB_WRITE_TWI_MS_CONTROL_ALL(true, TWI_MAPPING, TWI_MAPPING, NACK, 0);

    //Perform a software reset sequence to resolve situation in which the EEPROM is in a corrupt state
    halTWI_ExecuteAndWait(TWI_CMD_START | TWI_CMD_READ);                //Start + Dummy read
    halTWI_ExecuteAndWait(TWI_CMD_START | TWI_CMD_READ | TWI_CMD_STOP); //Start + Stop (+ Dummy read since the hw module requires the read to be done)

    //Disable TWI block + clr mappings
    GP_WB_WRITE_TWI_MS_CONTROL_ALL(false, GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED, GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED, ACK, 0);
}

Bool hal_WriteReadTWI(UInt8 deviceAddress, UInt8 txLength, UInt8* txBuffer, UInt8 rxLength, UInt8* rxBuffer)
{
    UInt8 i;
    UInt8 cmd = 0;
    Bool success;

    halTWI_Active           = true;
    halTWI_ActivityDetected = true;

    //Enable TWI block + set mappings
    GP_WB_WRITE_TWI_MS_CONTROL_ALL(true, TWI_MAPPING, TWI_MAPPING, ACK, 0);

    //GP_LOG_SYSTEM_PRINTF("TX:%u RX:%u TB:%x, RB:%x", 8, (UInt16)txLength, (UInt16)rxLength, txBuffer, rxBuffer); gpLog_Flush();

    //TX
    if(txLength != 0)
    {
        //Write access CMD
        success = halTWI_TxByte(TWI_CMD_START, deviceAddress);
        if(!success)
        {
            goto hal_WriteReadTWIEnd;
        }
    }
    else
    {
        //Only checking for ACK
        success = halTWI_TxByte(TWI_CMD_START | TWI_CMD_STOP, deviceAddress);
        goto hal_WriteReadTWIEnd;
    }

    //Send bytes - address also in these bytes
    for(i=0; i<txLength-1; i++)
    {
        success = halTWI_TxByte(0, txBuffer[i]);
        if(!success) goto hal_WriteReadTWIEnd;
    }

    if((rxLength == 0) || (rxBuffer == NULL))
    {
        cmd = TWI_CMD_STOP;
    }

    //Send last Tx byte
    success = halTWI_TxByte(cmd, txBuffer[txLength-1]);
    if(!success) goto hal_WriteReadTWIEnd;

    //RX

    //Read out wanted bytes
    if(rxLength != 0 && rxBuffer != NULL)
    {
        //Read access CMD
        success = halTWI_TxByte(TWI_CMD_START, deviceAddress | 0x1);
        if(!success) goto hal_WriteReadTWIEnd;

        for(i=0; i<rxLength-1; i++)
        {
            success = halTWI_RxByte(false, &(rxBuffer[i]));
            if(!success) goto hal_WriteReadTWIEnd;
        }
        success = halTWI_RxByte(true, &(rxBuffer[rxLength-1]));
        if(!success) goto hal_WriteReadTWIEnd;
    }

hal_WriteReadTWIEnd:

    //Disable TWI block + clr mappings
    GP_WB_WRITE_TWI_MS_CONTROL_ALL(false, GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED, GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED, ACK, 0);

    halTWI_Active = false;

    return success;
}

Bool hal_WasActiveTWI(void)
{
    if (halTWI_ActivityDetected)
    {
        if (!halTWI_Active) //no longer active
        {
            halTWI_ActivityDetected = false; //destructive read
        }
        return true;
    }
    else
    {
        return false;
    }
}
