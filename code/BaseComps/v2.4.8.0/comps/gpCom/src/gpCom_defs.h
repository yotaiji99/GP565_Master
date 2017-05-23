/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpCom/src/gpCom_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPCOM_DEFS_H_
#define _GPCOM_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "gpCom.h"
/****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/



#define GP_COM_BLE_HEADER_LENGTH 1

//Shared for UART and USB
#define GP_COM_PACKET_HEADER_LENGTH 6
#define GP_COM_PACKET_FOOTER_LENGTH 2

#define GP_COM_MAXIMAL_COMMAND_PARAMETERS (GP_COM_MAX_PACKET_PAYLOAD_SIZE - sizeof(gpCom_CommandID_t))


/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

#define gpCom_CommandIDTxOverflowIndication             0x02
typedef UInt8 gpCom_CommandID_t;

typedef struct gpCom_SerialPacket
{
    gpCom_CommandID_t        commandID;
    UInt8                    commandParameters[GP_COM_MAXIMAL_COMMAND_PARAMETERS];
} gpCom_SerialPacket_t;

//////////////////
// TxOverflowIndication
//////////////////

typedef struct gpCom_TxOverflowIndicationCommandParameters
{
    UInt16  numberOfMissedMessages;
}gpCom_TxOverflowIndicationCommandParameters_t;

#define gpCom_ProtocolPacketHeader     0
#define gpCom_ProtocolPacketData       1
#define gpCom_ProtocolPacketFooter     2
#define gpCom_ProtocolBleHuntMode      3
typedef UInt8 gpCom_ProtocolPacket_t;

typedef struct gpCom_PacketHeader_s {
    UInt8  length;
    UInt8  moduleID;
    UInt16 crc;
} gpCom_PacketHeader_t;

typedef struct gpCom_Packet_s {
    gpCom_CommunicationId_t commId;
    UInt16 length;
    UInt8  moduleID;
    UInt8  packet[GP_COM_MAX_PACKET_PAYLOAD_SIZE];
} gpCom_Packet_t;

typedef struct {
    gpCom_Packet_t*         pPacket;
    UInt16                  crc;
    gpCom_ProtocolPacket_t  partOfPacket; //Header/data/footer
    UInt16                  counter;
    UInt16                  length;
    UInt8                   moduleID;
    gpCom_CommunicationId_t commId;
} gpCom_ProtocolState_t;


extern gpCom_cbActivateTx_t gpCom_ActivateTxCb;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

//Packet memory handlers
gpCom_Packet_t* Com_GetFreePacket(void);
void            Com_FreePacket(gpCom_Packet_t* pPacket);
//Queue handler
Bool Com_AddPendingPacket(gpCom_Packet_t* pPacket);
void Com_Execute(gpCom_Packet_t * pPacket);

void Com_InitRx(void);
void Com_ConnectionClose(gpCom_CommunicationId_t commId);
void Com_DeInitRx(void);

//Specific parsing
#define gpCom_ProtocolDone       0
#define gpCom_ProtocolContinue   1
#define gpCom_ProtocolError      2
typedef UInt8 gpCom_ProtocolStatus_t;
//Specific parsing
gpCom_ProtocolStatus_t Com_ParseSynProtocol(UInt8 rxbyte, gpCom_ProtocolState_t* state);


#if defined(GP_DIVERSITY_COM_UART)
#define Com_FlushRx() ComUart_FlushRx()
#else
#define Com_FlushRx()
#endif

#if defined( GP_DIVERSITY_COM_UART )
//UART functions 
extern void gpComUart_Init(void);
extern Bool gpComUart_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId);
extern UInt16 gpComUart_GetFreeSpace(gpCom_CommunicationId_t commId);
extern Bool gpComUart_GetTXEnable(void);
extern void gpComUart_HandleTx(void);
extern void gpComUart_Flush(void);
extern void gpComUart_DeInit(void);
extern void ComUart_FlushRx(void);
#endif //defined( GP_DIVERSITY_COM_UART )



#ifdef GP_DIVERSITY_COM_SOCKET
extern void gpComSocket_Init(void);
extern Bool gpComSocket_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId);
extern Bool gpComSocket_GetTXEnable(void);
extern void gpComSocket_HandleTx(void);
extern void gpComSocket_Flush(void);
extern void gpComSocket_DeInit(void);
#endif //GP_DIVERSITY_COM_SOCKET


#endif // _GPCOM_DEFS_H_

