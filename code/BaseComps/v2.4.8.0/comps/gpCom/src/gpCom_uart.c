/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 * gpCom.c
 *
 * This file contains the implementation of the serial communication module.
 * It implements the GreenPeak serial protocol.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpCom/src/gpCom_uart.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_COM
#define GP_MODULE_ID GP_MODULE_ID_COM

#include "hal.h"
#include "gpUtils.h"
#include "gpCom.h"
#include "gpCom_defs.h"

#include "gpLog.h"
#include "gpAssert.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#ifndef GP_BSP_UART_COM2
#define GP_COM_NUM_UART 1
#else
#define GP_COM_NUM_UART 2
#endif

#define GP_COM_TX_BUFFER_ACTIVATE_SIZE         (ComTxBufferSize/2)
#define GP_COM_IS_STANDARD_LOGGING_DATA_WAITING(uart)     (gpCom_vlog_upload[uart] != gpCom_vlog[uart])

//GP_COM_MAX_TX_BUFFER_SIZE can be set in hal/bsp if you don't have a lot of ram
#ifndef GP_COM_MAX_TX_BUFFER_SIZE
#define GP_COM_MAX_TX_BUFFER_SIZE 1536
#endif

#ifndef GP_COM_TX_BUFFER_SIZE
#define GP_COM_TX_BUFFER_SIZE GP_COM_MAX_TX_BUFFER_SIZE
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static UInt16     gpCom_vlog[GP_COM_NUM_UART];
static UInt16     gpCom_vlog_upload[GP_COM_NUM_UART];


extern Bool   gpCom_Initialized;
static UInt16 gpComUart_BufferOverflowCounter[GP_COM_NUM_UART];
static Bool   gpComUart_BufferOverflowOccured[GP_COM_NUM_UART];

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/


//Define Tx data buffer here
#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING GEN_EXTRAM_SECTION
#endif
static UInt8 Com_TxBuf[GP_COM_NUM_UART][GP_COM_TX_BUFFER_SIZE] GP_EXTRAM_SECTION_ATTR;
#ifdef USE_PRAGMA
#pragma USER_DATA_MAPPING
#endif

#define ComTxBufferSize GP_COM_TX_BUFFER_SIZE
#define gpCom_Buf(i)    (Com_TxBuf[i])

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

// Static functions
static Int16 Com_cbUartGetTxData(UInt8 uart);
#define Com_cbUartRx NULL
static Bool Com_WriteBlock(UInt16 length , UInt16 *sizeAvailable , UInt16 *sizeBlock , void* pData, UInt8 uart);
static void Com_ActivateTxBuffer(UInt8 uart);
static void Com_CalculateSizes(UInt16* sizeAvailable, UInt16* sizeBlock, UInt8 uart);

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/
static void Com_ActivateTxBuffer(UInt8 uart)
{
    UInt16 retryPackets = 0;
    gpCom_CommunicationId_t commId = (uart == 0 ? GP_COM_COMM_ID_UART1 : GP_COM_COMM_ID_UART2);
    gpComUart_BufferOverflowOccured[uart] = false;

    if (gpCom_ActivateTxCb)
    {
        /* This callback will try to resend packets.
         * If the amount of overflows equals the amount of retries there is no
         * need to send out a "missed packet" notification. */
        retryPackets = gpCom_ActivateTxCb(gpComUart_BufferOverflowCounter[uart], commId);
    }
    GP_ASSERT_DEV_EXT(gpComUart_BufferOverflowCounter[uart] >= retryPackets);

    if ((gpComUart_BufferOverflowCounter[uart]-retryPackets))
    {
        // Remove the following lines, when the TcpListener can interpret the TxOverflowIndication message
        // The printf could in this case be put in the else case of the new serial protocol
        UInt8 txOverflowIndicationLength = sizeof(gpCom_CommandID_t)+sizeof(gpCom_TxOverflowIndicationCommandParameters_t);
        UInt8 txOverflowIndication[sizeof(gpCom_CommandID_t)+sizeof(gpCom_TxOverflowIndicationCommandParameters_t)];

        gpCom_SerialPacket_t* pSerialPacket = (gpCom_SerialPacket_t*)(txOverflowIndication);
        pSerialPacket->commandID            = gpCom_CommandIDTxOverflowIndication;

        HOST_TO_LITTLE_UINT16(&gpComUart_BufferOverflowCounter[uart]);
        MEMCPY(pSerialPacket->commandParameters,&gpComUart_BufferOverflowCounter[uart],2);
        LITTLE_TO_HOST_UINT16(&gpComUart_BufferOverflowCounter[uart]);

        GP_COM_DATA_REQUEST(txOverflowIndicationLength, txOverflowIndication, commId);
        
        //Reset overflow counter
        gpComUart_BufferOverflowCounter[uart] = 0;
    }
}

static void Com_CalculateSizes(UInt16* sizeAvailable, UInt16* sizeBlock, UInt8 uart)
{
    //Calculate avaible size

    //Available size will always be 1 less than the total buffer size.
    //No distinction can be made between completely full or empty (indexes ==)
    if (gpCom_vlog_upload[uart] <= gpCom_vlog[uart])
    {
        *sizeAvailable = ComTxBufferSize - 1 - gpCom_vlog[uart] + gpCom_vlog_upload[uart];
        if(sizeBlock != NULL)
            *sizeBlock = ComTxBufferSize - gpCom_vlog[uart];
    }
    else
    {
        *sizeAvailable = gpCom_vlog_upload[uart] - gpCom_vlog[uart] - 1;
        if(sizeBlock != NULL)
            *sizeBlock = *sizeAvailable;
    }
}

static Bool Com_WriteBlock(UInt16 length , UInt16 *sizeAvailable , UInt16 *sizeBlock , void* pData, UInt8 uart)
{
    GP_UTILS_STACK_TRACK();
    if (length > *sizeAvailable)
    {
        return true;
    }
    else
    {
        if (length > *sizeBlock)
        {
            UInt16 length_2block;
            // Copy in 2 blocks
            MEMCPY(&(gpCom_Buf(uart)[gpCom_vlog[uart]]),pData,*sizeBlock);
            gpCom_vlog[uart] = 0;
            length_2block = length - *sizeBlock;

            MEMCPY(&(gpCom_Buf(uart)[gpCom_vlog[uart]]) ,((UInt8*)pData + *sizeBlock),length_2block);
            gpCom_vlog[uart] = length_2block;

            *sizeAvailable -= *sizeBlock + length_2block;
            *sizeBlock      = *sizeAvailable;
        }
        else
        {
            // Copy in 1 block
            MEMCPY(&(gpCom_Buf(uart)[gpCom_vlog[uart]]),pData,length);
            gpCom_vlog[uart] += length;
            GP_ASSERT_DEV_INT((gpCom_vlog[uart] <= ComTxBufferSize));
            if (gpCom_vlog[uart] >= ComTxBufferSize) gpCom_vlog[uart] = 0;

            *sizeAvailable -= length;
            *sizeBlock     -= length;
        }
    }
    return false;
}

static Int16 Com_cbUartGetTxData(UInt8 uart)
{
    Int16 returnValue = -1;

    HAL_DISABLE_GLOBAL_INT();

    if (GP_COM_IS_STANDARD_LOGGING_DATA_WAITING(uart))
    {
        // Output character
        returnValue = gpCom_Buf(uart)[gpCom_vlog_upload[uart]];
        // Scroll output pointer
        if ((unsigned int) ++gpCom_vlog_upload[uart] >= (unsigned int) ComTxBufferSize) gpCom_vlog_upload[uart] = 0;
    }

    HAL_ENABLE_GLOBAL_INT();

    return returnValue;
}

static Int16 Com_cbUart1GetTxData(void)
{
    return Com_cbUartGetTxData(0);
}
#if GP_COM_NUM_UART == 2
static Int16 Com_cbUart2GetTxData(void)
{
    return Com_cbUartGetTxData(1);
}
#endif




/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

/****************************************************************************
 ****************************************************************************
 **                       Initialization                                   **
 ****************************************************************************
 ****************************************************************************/

void gpComUart_Init(void)
{
    HAL_DISABLE_GLOBAL_INT();

    // init variables
    MEMSET(gpCom_vlog, 0, GP_COM_NUM_UART);
    MEMSET(gpCom_vlog_upload, 0, GP_COM_NUM_UART);

    MEMSET(gpComUart_BufferOverflowOccured, 0, GP_COM_NUM_UART);
    MEMSET(gpComUart_BufferOverflowCounter, 0, GP_COM_NUM_UART);

    // Initialize the UART (serial port)
    HAL_UART_COM_START( Com_cbUartRx , Com_cbUart1GetTxData);

    gpCom_Initialized = true;
    HAL_ENABLE_GLOBAL_INT();
}

void gpComUart_DeInit(void)
{
    HAL_DISABLE_GLOBAL_INT();
    
    // De-Initialize the UART
    HAL_UART_COM_START( NULL , NULL );
    HAL_UART_COM_POWERDOWN();
    

    gpCom_Initialized = false;

    HAL_ENABLE_GLOBAL_INT();
}

UInt16 gpComUart_GetFreeSpace(gpCom_CommunicationId_t commId)
{
    UInt16 freeSpace;
    UInt8  uart;
    UInt16 sizeAvailable, sizeBlock;
    UInt16 overhead = GP_COM_PACKET_HEADER_LENGTH - GP_COM_PACKET_FOOTER_LENGTH;

    if (!gpCom_GetTXEnable())
        return 0;

    uart = (commId == GP_COM_COMM_ID_UART1) ? 0 : 1;
    if (gpComUart_BufferOverflowOccured[uart])
    {
        return 0;
    }

    HAL_DISABLE_GLOBAL_INT ();

    //Calculate available sizes
    Com_CalculateSizes(&sizeAvailable,&sizeBlock, uart);

    freeSpace = overhead < sizeAvailable ? sizeAvailable - overhead : 0;

    HAL_ENABLE_GLOBAL_INT ();

    return freeSpace;
}

Bool gpComUart_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId)
{
    UInt16 checksumNew;
    UInt8  uart;
    Bool   overrun     = false;
    UInt16 sizeAvailable, sizeBlock;
    UInt8  header[GP_COM_PACKET_HEADER_LENGTH];

    if (!gpCom_GetTXEnable())
        return false;

    uart = (commId == GP_COM_COMM_ID_UART1) ? 0 : 1;

    header[0] = 'S';
    header[1] = 'Y';
    header[2] = 'N';
    header[3] = length & 0xFF;
    header[4] =(length >> 8) & 0x0F;
    header[5] = moduleID;

    //calculate CRC before AtomicOn, since it takes a long time...
    checksumNew = 0;
    gpUtils_CalculatePartialCrc(&checksumNew,  header, GP_COM_PACKET_HEADER_LENGTH);
    gpUtils_CalculatePartialCrc(&checksumNew,  pData, length);

    HAL_DISABLE_GLOBAL_INT ();

    //Calculate available sizes
    Com_CalculateSizes(&sizeAvailable,&sizeBlock, uart);

    if ((GP_COM_PACKET_HEADER_LENGTH +
         GP_COM_PACKET_FOOTER_LENGTH +
         length) > sizeAvailable)
    {
        gpComUart_BufferOverflowOccured[uart] = true;
    }

    if (gpComUart_BufferOverflowOccured[uart])
    {
        gpComUart_BufferOverflowCounter[uart]++;
        HAL_ENABLE_GLOBAL_INT ();
        return false;
    }

    // Write new serial protocol header
    overrun = Com_WriteBlock(GP_COM_PACKET_HEADER_LENGTH, &sizeAvailable, &sizeBlock, header, uart);

    // Write data
    if (!overrun)
    {
        overrun = Com_WriteBlock(length, &sizeAvailable, &sizeBlock, pData, uart);
    }

    // Write data
    if (!overrun)
    {
        UInt8 footer[GP_COM_PACKET_FOOTER_LENGTH];

        footer[0] = (UInt8)((checksumNew) >> 0);
        footer[1] = (UInt8)((checksumNew) >> 8);

        // Write additional new serial protocol header
        overrun = Com_WriteBlock(GP_COM_PACKET_FOOTER_LENGTH, &sizeAvailable, &sizeBlock, footer, uart);
    }
    HAL_ENABLE_GLOBAL_INT ();

    HAL_DISABLE_GLOBAL_INT ();
    GP_ASSERT_DEV_INT(!overrun);
    
    // Enable Tx interrupt immediately for continuous logging
#if GP_COM_NUM_UART == 2
    if(uart == 1)
    {
        HAL_UART_COM2_TX_NEW_DATA();
    }
    else
#endif
    {
        HAL_UART_COM_TX_NEW_DATA();
    }

    HAL_ENABLE_GLOBAL_INT ();

    return !overrun;
}

void gpComUart_HandleTx(void)
{        
    UIntLoop i;

    //Tx re-activation after overflow
    for(i = 0; i < GP_COM_NUM_UART; i++)
    {
        if (gpComUart_BufferOverflowOccured[i])
        {
            UInt16 sizeAvailable;
            Com_CalculateSizes(&sizeAvailable, NULL, i);
            if (sizeAvailable > GP_COM_TX_BUFFER_ACTIVATE_SIZE)
            {
                // Activate back the tx buffer, when sufficient empty space available
                Com_ActivateTxBuffer(i);
            }
        }
    }
}

Bool gpComUart_GetTXEnable( void )
{
    return gpCom_Initialized && HAL_UART_COM_TX_ENABLED();
}

void gpComUart_Flush(void)
{
#ifdef HAVE_HAL_UART_FLUSH
    hal_UartComFlush();
#endif
}

void ComUart_FlushRx(void)
{
#ifdef HAVE_HAL_UART_RX_FLUSH
    hal_UartRxComFlush();
#endif
}



