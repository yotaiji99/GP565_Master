/*
 * Copyright (c) 2014, GreenPeak Technologies
 *
 *   Hardware Abstraction Layer for the UART on Xap5 devices.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/k7b/src/hal_UART.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
#include "hal.h"
#include "gpBsp.h"
#define COMPLETE_MAP
#include "gpHal.h"
#include "gpAssert.h"

#include "hal_ExtendedPatch.h"

#define GP_COMPONENT_ID GP_COMPONENT_ID_HALXAP5

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#define UART_PIN_MAPPING_A    GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_A     // RX gpio15 | TX gpio14
#define UART_PIN_MAPPING_B    GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_B     // RX gpio11 | TX gpio10
#define UART_PIN_MAPPING_C    GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_C     // RX gpio1  | TX gpio0
#define UART_PIN_MAPPING_D    GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_D     // RX gpio9  | TX gpio8
#define UART_PIN_MAPPING_E    GP_WB_ENUM_GENERIC_SEVEN_PINMAP_MAPPING_E     // RX gpio29 | TX gpio28
#define UART_PIN_DISCONNECTED GP_WB_ENUM_GENERIC_SEVEN_PINMAP_NOT_MAPPED

#ifndef HAL_UART_MAPPING_RX
#define HAL_UART_MAPPING_RX          UART_PIN_DISCONNECTED
#endif

#ifndef HAL_UART_MAPPING_TX
#define HAL_UART_MAPPING_TX          UART_PIN_DISCONNECTED
#endif

#ifndef HAL_DEBUG_MAPPING_UART_RX
#define HAL_DEBUG_MAPPING_UART_RX   UART_PIN_DISCONNECTED
#endif

#ifndef HAL_DEBUG_MAPPING_UART_TX
#define HAL_DEBUG_MAPPING_UART_TX   UART_PIN_DISCONNECTED
#endif

#ifndef GP_BSP_UART_COM_BAUDRATE
#define GP_BSP_UART_COM_BAUDRATE 57600
#endif

#ifndef GP_BSP_UART_SCOM_BAUDRATE
#define GP_BSP_UART_SCOM_BAUDRATE 57600
#endif

//On FPGA always use a mapping
#define UART_MAPPING_TX     ((HAL_DEBUG_IS_UART_ENABLED() ? HAL_DEBUG_MAPPING_UART_TX : HAL_UART_MAPPING_TX))
#define UART_MAPPING_RX     ((HAL_DEBUG_IS_UART_ENABLED() ? HAL_DEBUG_MAPPING_UART_RX : HAL_UART_MAPPING_RX))

#ifdef GP_BSP_UART_COM_BAUDRATE
#define HAL_UART_COM_SYMBOL_PERIOD (16000000L / (8*GP_BSP_UART_COM_BAUDRATE+1))
GP_COMPILE_TIME_VERIFY(HAL_UART_COM_SYMBOL_PERIOD <=  0x0FFF);
#endif

#ifdef GP_BSP_UART_SCOM_BAUDRATE
#define HAL_UART_SCOM_SYMBOL_PERIOD (16000000L / (8*GP_BSP_UART_SCOM_BAUDRATE+1))
GP_COMPILE_TIME_VERIFY(HAL_UART_COM_SYMBOL_PERIOD <=  0x0FFF);
#endif

#define HAL_UART_POLL_BUFFER_SIZE   64U

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifdef HAL_UART_NUM_GET_TX_DATA_CB
//Allowing multiple users for hal TX
static hal_cbUartGetTxData_t hal_cbUartGetTxData[HAL_UART_NUM_GET_TX_DATA_CB] = { NULL, };
static void  hal_UartStoreGetTxData(hal_cbUartGetTxData_t cb);
static Int16 hal_UartHandleGetTxData(void);
#else
static hal_cbUartGetTxData_t   hal_cbUartGetTxData;
#define hal_UartStoreGetTxData(cb) { hal_cbUartGetTxData = cb; }
#define hal_UartHandleGetTxData() ((hal_cbUartGetTxData == NULL)? -1 : hal_cbUartGetTxData())
#endif //HAL_UART_NUM_GET_TX_DATA_CB

static hal_cbUartEot_t  hal_cbUartOneShotEndOfTx;

#ifdef GP_BSP_UART_COM2
static UInt8 halUart_ActiveTxUart;
static void halUart_SwapTxMapping(UInt8 nr);
#endif //GP_BSP_UART_COM2

#ifndef HAL_UART_NO_RX
static UInt8 hal_UartBuffer[HAL_UART_POLL_BUFFER_SIZE];
static UInt8 hal_UartReadPtr = 0;
#ifdef HAL_UART_NUM_RX_CB
//Allowing multiple users for hal RX
static hal_cbUartRx_t hal_cbUartRx[HAL_UART_NUM_RX_CB] = { NULL, };
static void hal_UartStoreRx(hal_cbUartRx_t cb);
static void hal_UartHandleRx(UInt8 data);
#else
static hal_cbUartRx_t  hal_cbUartRx;
#define hal_UartStoreRx(cb)    { hal_cbUartRx = cb; }
#define hal_UartHandleRx(data) hal_cbUartRx(data)
#endif //HAL_UART_NUM_RX_CB
#else
#define hal_UartStoreRx(cb)    NOT_USED(cb)
#endif //HAL_UART_NO_RX

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

#ifndef HAL_UART_NO_RX
void hal_UartRxEnableDMA(void)
{
    UInt32 startAddress = (UIntPtr)hal_UartBuffer;

    // setup dma source buffer
    GP_WB_WRITE_DMA_SRC_START_ADDR(GP_WB_UART_RX_DATA_0_ADDRESS);
    GP_WB_WRITE_DMA_SRC_BUFFER_SIZE(0);
    //GP_WB_WRITE_DMA_WORD_MODE(databits > 8);
    // setup dma destination buffer
    GP_WB_WRITE_DMA_DEST_START_ADDR(startAddress);
    GP_WB_WRITE_DMA_DEST_BUFFER_SIZE(HAL_UART_POLL_BUFFER_SIZE-1); //-(databits > 8));

    GP_WB_DMA_RESET_POINTERS();

    // set up the notification interrupt
    GP_WB_WRITE_DMA_DEST_BUFFER_ALMOST_FULL_THRESHOLD(16-1); // handle as soon as possible
    // connect the dma engine
    GP_WB_WRITE_DMA_CPY_TRIGGER_SRC_SELECT(GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_RX_NOT_EMPTY);

    GP_WB_WRITE_INT_CTRL_MASK_DMA_DEST_ALMOST_FULL_INTERRUPT(true);
    GP_WB_WRITE_INT_CTRL_MASK_INT_DMA_INTERRUPT(true);
    
    hal_UartReadPtr = 0;
}

void hal_UartRxDisableDMA(void)
{
    GP_WB_WRITE_DMA_CPY_TRIGGER_SRC_SELECT(GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_NO_TRIGGER_SRC);
}

static void hal_UartRxCheckErrors(void)
{
    GP_ASSERT_DEV_EXT(hal_cbUartRx != NULL);        // shouldn't we trigger the assert at the moment we register the callback
}

void hal_UartRxHandleDMA(void)
{
    HAL_DISABLE_GLOBAL_INT();
    if(hal_cbUartRx)
    {
        UInt16 writePtr;

        hal_UartRxCheckErrors();

        //Treating buffer as a 'virtual' buffer of 2x the size depending on the wraparound
        writePtr = GP_WB_READ_DMA_INTERNAL_DEST_WRITE_PTR();
        writePtr+= GP_WB_READ_DMA_DEST_WRITE_PTR_WRAP()*HAL_UART_POLL_BUFFER_SIZE; //Take wrap into account

        while (hal_UartReadPtr != writePtr)
        {
            UIntLoop i;
            UInt16 n = 0;

            //Calculate amount of bytes to process
            n = (writePtr - hal_UartReadPtr) % HAL_UART_POLL_BUFFER_SIZE ;

            for (i = 0; i < n; i++)
            {
                hal_UartHandleRx(hal_UartBuffer[(hal_UartReadPtr+i) % HAL_UART_POLL_BUFFER_SIZE]);
            }

            //Update read pointer in 'virtual' buffer
            hal_UartReadPtr += n;
            hal_UartReadPtr %= 2*HAL_UART_POLL_BUFFER_SIZE;

            //Update DMA read pointer
            GP_WB_WRITE_DMA_BUFFER_PTR_VALUE(hal_UartReadPtr % HAL_UART_POLL_BUFFER_SIZE);
            GP_WB_WRITE_DMA_BUFFER_PTR_WRAP_VALUE(hal_UartReadPtr / HAL_UART_POLL_BUFFER_SIZE);
            GP_WB_DMA_SET_DEST_READ_PTR() ;

            writePtr = GP_WB_READ_DMA_INTERNAL_DEST_WRITE_PTR();
            writePtr+= GP_WB_READ_DMA_DEST_WRITE_PTR_WRAP()*HAL_UART_POLL_BUFFER_SIZE;
        }
    }
    HAL_ENABLE_GLOBAL_INT();
}

INTERRUPT_H void handle_dma_int(void)
{
    if(GP_WB_READ_DMA_UNMASKED_DEST_ALMOST_FULL_INTERRUPT())
    {
        hal_UartRxHandleDMA();
    }
}
#endif

void hal_UartRxComFlush(void)
{
#ifndef HAL_UART_NO_RX
    hal_UartRxHandleDMA();
#endif
}

static void handleUartIntTxData(void)
{
    Int16 data_to_tx;

    data_to_tx = hal_UartHandleGetTxData();

    if (data_to_tx < 0)
    {
        //no more data  -> disable interrupt.
        GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_FULL_INTERRUPT(false);
    }
    else
    {
        GP_WB_WRITE_UART_TX_DATA_0(data_to_tx);
    }
}

#ifdef HAL_UART_NUM_GET_TX_DATA_CB
void hal_UartStoreGetTxData(hal_cbUartGetTxData_t cb)
{
    UIntLoop i;
    for(i = 0; i < HAL_UART_NUM_GET_TX_DATA_CB; i++)
    {
        if ((hal_cbUartGetTxData[i] == NULL) || (hal_cbUartGetTxData[i] == cb))
        {
            hal_cbUartGetTxData[i] = cb;
            break;
        }
    }
}

static Int16 hal_UartHandleGetTxData(void)
{
    Int16 ret = -1;
    UIntLoop i;

    for(i = 0; i < HAL_UART_NUM_GET_TX_DATA_CB; i++)
    {
        if (hal_cbUartGetTxData[i] != NULL)
        {
            ret = hal_cbUartGetTxData[i]();
            if (ret >= 0)
            {
#ifdef GP_BSP_UART_COM2
                halUart_SwapTxMapping(i);
#endif //GP_BSP_UART_COM2
                break;
            }
        }
    }

    return ret;
}
#endif //HAL_UART_NUM_GET_TX_DATA_CB

#ifdef GP_BSP_UART_COM2
static void halUart_SwapTxMapping(UInt8 nr)
{
    if (nr != halUart_ActiveTxUart)
    {
        halUart_ActiveTxUart = nr;
        hal_UartWaitEndOfTransmission();
        if (halUart_ActiveTxUart == 0)
        {
            GP_WB_WRITE_UART_TX_MAP(HAL_UART_MAPPING_TX);
        }
        else
        {
            GP_WB_WRITE_UART_TX_MAP(HAL_UART2_MAPPING_TX);
        }
    }
}
#endif //GP_BSP_UART_COM2

#ifndef HAL_UART_NO_RX
#ifdef HAL_UART_NUM_RX_CB
static void hal_UartHandleRx(UInt8 data)
{
    UIntLoop i;

    for(i = 0; i < HAL_UART_NUM_RX_CB; i++)
    {
        if (hal_cbUartRx[i] != NULL)
        {
            hal_cbUartRx[i](data);
        }
    }
}

void hal_UartStoreRx(hal_cbUartRx_t cb)
{
    UIntLoop i;
    for(i = 0; i < HAL_UART_NUM_RX_CB; i++)
    {
        if ((hal_cbUartRx[i] == NULL) || (hal_cbUartRx[i] == cb))
        {
            hal_cbUartRx[i] = cb;
            break;
        }
    }
}
#endif //HAL_UART_NUM_RX_CB
#endif //HAL_UART_NO_RX

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void hal_UartStart(hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbGetTxData, UInt16 symbolPeriod, UInt16 flags, UInt8 uart)
{
    /* bit 2*/
    UInt8 stopbits = (flags >> 2) & 0x03;
    UInt8 parity   = (flags >> 0) & 0x03;
    UInt8 databits = (flags >> 4) & 0x0F;
#ifdef GP_BSP_UART_COM2
    UInt8 txMapping = (uart == 0) ? HAL_UART_MAPPING_TX : HAL_UART2_MAPPING_TX;
#else
    UInt8 txMapping = HAL_UART_MAPPING_TX;
#endif //GP_BSP_UART_COM2
    hal_UartStoreRx(cbRx);
    hal_UartStoreGetTxData(cbGetTxData);
    hal_cbUartOneShotEndOfTx = (hal_cbUartEot_t) NULL;

    if((NULL == cbRx) && (NULL == cbGetTxData))
    {
        hal_UartDisable();
    }

    //Set UART baudrate
    GP_WB_WRITE_UART_BAUD_RATE(symbolPeriod);

    //we only support up to one byte for now.
    //GP_LOG_SYSTEM_PRINTF("bits: %i",2, databits);
    //GP_LOG_SYSTEM_PRINTF("parity: %i",2, parity);
    //GP_LOG_SYSTEM_PRINTF("stop: %i",2, stopbits);

    GP_ASSERT_DEV_EXT(databits <= 8);

    GP_WB_WRITE_UART_DATA_BITS(databits-1);
    GP_WB_WRITE_UART_PARITY(parity);
    GP_WB_WRITE_UART_STOP_BITS(stopbits-1);

#ifdef GP_BSP_UART_COM2
    halUart_ActiveTxUart = uart;
#endif //#GP_BSP_UART_COM2

    // TX pin mapping
    GP_WB_WRITE_UART_TX_MAP(txMapping);
    if (txMapping == UART_PIN_MAPPING_A)
    {
        GP_WB_WRITE_IOB_GPIO_14_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else if(txMapping == UART_PIN_MAPPING_B)
    {
        GP_WB_WRITE_IOB_GPIO_10_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else if(txMapping == UART_PIN_MAPPING_C)
    {
        // Disable interrupt line for external MCU
        GP_WB_WRITE_IOB_MCU_INTOUTN_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
        GP_WB_WRITE_IOB_GPIO_0_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else if(txMapping == UART_PIN_MAPPING_D)
    {
        GP_WB_WRITE_IOB_GPIO_9_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else if(txMapping == UART_PIN_MAPPING_E)
    {
        GP_WB_WRITE_IOB_GPIO_29_CFG(GP_WB_ENUM_GPIO_MODE_FLOAT);
    }
    else
    {
        GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_FULL_INTERRUPT(false);
    }

#ifndef HAL_UART_NO_RX
    //Rx interrupt should be enabled if there is a handler, tx not yet.
    if ((cbRx != NULL) && (UART_MAPPING_RX != UART_PIN_DISCONNECTED))
    {
        if((UART_MAPPING_RX) == UART_PIN_MAPPING_C)
        {
            // disable SPI interface and mapping
            GP_WB_WRITE_MSI_SERIAL_ITF_SELECT(GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE);
            GP_WB_WRITE_SPI_SL_PINMAP(GP_WB_ENUM_GENERIC_SINGLE_PINMAP_NOT_MAPPED);
        }
        //GP_WB_WRITE_INT_CTRL_MASK_UART_RX_NOT_EMPTY_INTERRUPT(true); //Non-DMA mode
        hal_UartRxEnableDMA();
        GP_WB_WRITE_UART_RX_MAP(UART_MAPPING_RX);
        GP_WB_WRITE_UART_RX_ENABLE(true);
    }
    else
#endif //HAL_UART_NO_RX
    {
        GP_WB_WRITE_UART_RX_MAP(UART_PIN_DISCONNECTED);
        GP_WB_WRITE_UART_RX_ENABLE(false);
        GP_WB_WRITE_INT_CTRL_MASK_UART_RX_NOT_EMPTY_INTERRUPT(false);
    }
    //Enable UART block mask
    GP_WB_WRITE_INT_CTRL_MASK_INT_UART_INTERRUPT(true);
}

void hal_UartComStart( hal_cbUartRx_t cbUartRx, hal_cbUartGetTxData_t cbUartGetTxData, UInt8 uart)
{
    hal_UartStart(cbUartRx , cbUartGetTxData , HAL_UART_COM_SYMBOL_PERIOD, \
                   (HAL_UART_OPT_8_BITS_PER_CHAR | HAL_UART_OPT_NO_PARITY | HAL_UART_OPT_ONE_STOP_BIT), uart);
}
void hal_UartSComStart( hal_cbUartRx_t cbUartRx, hal_cbUartGetTxData_t cbUartGetTxData)
{
    hal_UartStart(cbUartRx , cbUartGetTxData , HAL_UART_SCOM_SYMBOL_PERIOD, \
                   (HAL_UART_OPT_8_BITS_PER_CHAR | HAL_UART_OPT_NO_PARITY | HAL_UART_OPT_ONE_STOP_BIT), 0);
}

void hal_UartDisable(void)
{
    GP_WB_WRITE_INT_CTRL_MASK_INT_UART_INTERRUPT(false);

    //Flush remaing bytes
    hal_UartComFlush();

    //Disconnect pins
    GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_FULL_INTERRUPT(false);
    GP_WB_WRITE_UART_TX_MAP(UART_PIN_DISCONNECTED);
#ifndef HAL_UART_NO_RX
    GP_WB_WRITE_INT_CTRL_MASK_UART_RX_NOT_EMPTY_INTERRUPT(false);
    GP_WB_WRITE_UART_RX_ENABLE(false);
    GP_WB_WRITE_UART_RX_MAP(UART_PIN_DISCONNECTED);
#endif //HAL_UART_NO_RX
}

void hal_UartEnable(void)
{
    //(Re-)connect pins
#ifndef HAL_UART_NO_RX
    if(UART_MAPPING_RX != UART_PIN_DISCONNECTED)
    {
        GP_WB_WRITE_UART_RX_MAP(UART_MAPPING_RX);
        GP_WB_WRITE_UART_RX_ENABLE(true);
    }
#endif //HAL_UART_NO_RX
    if(UART_MAPPING_TX != UART_PIN_DISCONNECTED)
    {
#ifdef GP_BSP_UART_COM2
        halUart_ActiveTxUart = 0;
#endif //#GP_BSP_UART_COM2
        GP_WB_WRITE_UART_TX_MAP(UART_MAPPING_TX);
    }

    if((UART_MAPPING_TX != UART_PIN_DISCONNECTED)
#ifndef HAL_UART_NO_RX
    || (UART_MAPPING_RX != UART_PIN_DISCONNECTED)
#endif
    )
    {
        GP_WB_WRITE_INT_CTRL_MASK_INT_UART_INTERRUPT(true);
    }
}

Bool hal_UartTxEnabled(void)
{
    Bool enabled;

    enabled = GP_WB_READ_INT_CTRL_MASK_INT_UART_INTERRUPT() &&
             (GP_WB_READ_UART_TX_MAP() != UART_PIN_DISCONNECTED);

    return enabled;
}

Bool hal_UartRxEnabled(void)
{
    Bool enabled;

    enabled = GP_WB_READ_INT_CTRL_MASK_INT_UART_INTERRUPT() &&
             (GP_WB_READ_UART_RX_MAP() != UART_PIN_DISCONNECTED);

    return enabled;
}


void hal_UartTxNewData(void)
{
    GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_FULL_INTERRUPT(true);
}

void hal_UartWaitEndOfTransmission(void)
{
    while(!GP_WB_READ_UART_UNMASKED_TX_NOT_BUSY_INTERRUPT());
}

void hal_UartRegisterOneShotEndOfTxCb(hal_cbUartEot_t cbEot)
{
    GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_BUSY_INTERRUPT(false);
    hal_cbUartOneShotEndOfTx = cbEot;
    GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_BUSY_INTERRUPT(true);
}

void hal_UartComFlush(void)
{
    HAL_DISABLE_GLOBAL_INT();
    do
    {
        if (GP_WB_READ_UART_UNMASKED_TX_NOT_FULL_INTERRUPT())
        {
            handleUartIntTxData();
        }
    } while (GP_WB_READ_INT_CTRL_MASK_UART_TX_NOT_FULL_INTERRUPT()); //Mask disabled when all data is sent
    hal_UartWaitEndOfTransmission();

    HAL_ENABLE_GLOBAL_INT();
}

INTERRUPT_H void handleUartInt(void)
{
    UInt8 maskedInt;

    maskedInt = GP_HAL_READ_REG(GP_WB_INT_CTRL_MASKED_UART_TX_NOT_BUSY_INTERRUPT_ADDRESS) &
                (GP_WB_INT_CTRL_MASKED_UART_TX_NOT_FULL_INTERRUPT_MASK |
                 GP_WB_INT_CTRL_MASKED_UART_TX_NOT_BUSY_INTERRUPT_MASK);

    //GP_LOG_PRINTF("uart irq %x",2, maskedInt);
    if(HAL_ISR(HAL_ISR_UART_PRE_ID))
    {
        if(maskedInt & GP_WB_INT_CTRL_MASKED_UART_TX_NOT_FULL_INTERRUPT_MASK)
        {
            handleUartIntTxData();
        }
        else if(maskedInt & GP_WB_INT_CTRL_MASKED_UART_TX_NOT_BUSY_INTERRUPT_MASK)
        {
            if ((hal_cbUartEot_t)NULL != hal_cbUartOneShotEndOfTx)
            {
                hal_cbUartOneShotEndOfTx();
            }
            GP_WB_WRITE_INT_CTRL_MASK_UART_TX_NOT_BUSY_INTERRUPT(false);
            hal_cbUartOneShotEndOfTx = (hal_cbUartEot_t)NULL;
        }
        else
        {
#ifndef HAL_UART_NUM_GET_TX_DATA_CB
            GP_ASSERT_DEV_INT(false);       // should not occur
#endif //HAL_UART_NUM_GET_TX_DATA_CB
        }
    }

    if(false == HAL_ISR(HAL_ISR_UART_POST_ID))
    {
        GP_ASSERT_SYSTEM(false);
    }
}

