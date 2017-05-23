/*
 * Copyright (c) 2009-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/halXap5/inc/hal_UART.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _HAL_UART_H_
#define _HAL_UART_H_

/*****************************************************************************
 *                    UART
 *****************************************************************************/

//xap has only 4KB of RAM in total, so not a lot of ram available for big buffers.
/** flags */
/* bit 0..1*/
#define HAL_UART_OPT_EVEN_PARITY        GP_WB_ENUM_PARITY_EVEN
#define HAL_UART_OPT_ODD_PARITY         GP_WB_ENUM_PARITY_ODD
#define HAL_UART_OPT_NO_PARITY          GP_WB_ENUM_PARITY_OFF
/* bit 2-3*/
#define HAL_UART_OPT_ONE_STOP_BIT       (1 * BM(2))
#define HAL_UART_OPT_TWO_STOP_BITS      (2 * BM(2))
/* bit 4-7*/
#define HAL_UART_OPT_5_BITS_PER_CHAR    (5 * BM(4))
#define HAL_UART_OPT_6_BITS_PER_CHAR    (6 * BM(4))
#define HAL_UART_OPT_7_BITS_PER_CHAR    (7 * BM(4))
#define HAL_UART_OPT_8_BITS_PER_CHAR    (8 * BM(4))
//#define UART_OPT_9_BITS_PER_CHAR    (9 * BM(4))

typedef Int16 (* hal_cbUartGetTxData_t) (void);
typedef void (* hal_cbUartRx_t) (Int16);
typedef void (* hal_cbUartEot_t) (void);

//Calculate symbol period from baudrate
#define HAL_UART_DIVIDER_SYMBOL_PERIOD(baudrate)   ((16000000L / (UInt32)(baudrate)) - 1)
void hal_UartStart (hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbGetTxData, UInt16 symbolPeriod, UInt16 flags, UInt8 txMapping);

void hal_UartRxEnableDMA(void);
void hal_UartRxDisableDMA(void);
void hal_UartDisable(void);
void hal_UartEnable(void);
void hal_UartTxNewData(void);
void hal_UartWaitEndOfTransmission(void);
void hal_UartRegisterOneShotEndOfTxCb(hal_cbUartEot_t cbEot);

void hal_UartComStart( hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbTx, UInt8 uart);
void hal_UartSComStart( hal_cbUartRx_t cbRx, hal_cbUartGetTxData_t cbTx);
void hal_UartComFlush(void);
Bool hal_UartTxEnabled(void);
Bool hal_UartRxEnabled(void);
void hal_UartRxComFlush(void);

#define HAVE_HAL_UART_FLUSH
#define HAVE_HAL_UART_RX_FLUSH

/*****************************************************************************
 *                    ucHal UART interface
 *****************************************************************************/

#define HAL_UART_COM_START(cbRx, cbTx) do { \
    hal_UartComStart(cbRx , cbTx, 0); \
} while(false)

#define HAL_UART_COM2_START(cbRx, cbTx) do { \
    hal_UartComStart(cbRx , cbTx, 1 ); \
} while(false)


#define HAL_UART_COM_POWERDOWN() hal_UartDisable()
#define HAL_UART_COM_POWERUP()   hal_UartEnable()
#define HAL_UART_COM2_POWERDOWN() hal_UartDisable()
#define HAL_UART_COM2_POWERUP() hal_UartEnable()


#define HAL_UART_COM_TX_ENABLED()             hal_UartTxEnabled()
#define HAL_UART_COM_TX_NEW_DATA()            hal_UartTxNewData()
#define HAL_UART_COM_WAIT_END_OF_TX()         hal_UartWaitEndOfTransmission()
#define HAL_UART_COM_EXPECT_END_OF_TX(cbEOT)  hal_UartRegisterOneShotEndOfTxCb(cbEOT)

#define HAL_UART_COM2_TX_NEW_DATA()            hal_UartTxNewData()


/* SCOM = COM*/
#define HAL_UART_SCOM_START( cbRx , cbTx  ) do{ \
    hal_UartSComStart(cbRx , cbTx); \
}while(false)

#define HAL_UART_SCOM_POWERDOWN()       HAL_UART_COM_POWERDOWN()
#define HAL_UART_SCOM_POWERUP()         HAL_UART_COM_POWERUP()
#define HAL_UART_SCOM_TX_NEW_DATA()     HAL_UART_COM_TX_NEW_DATA()

#endif //_HAL_UART_H_
