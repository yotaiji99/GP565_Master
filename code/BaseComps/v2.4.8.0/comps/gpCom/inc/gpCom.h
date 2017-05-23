/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
 *
 * gpCom.h
 *
 * This file contains the API definition of the serial communication module.
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpCom/inc/gpCom.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPCOM_H_
#define _GPCOM_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>
#include <gpModule.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

//Check for multiple com usage
#ifdef GP_DIVERSITY_COM_UART
#if defined(GP_DIVERSITY_COM_SOCKET)
#define GP_COM_DIVERSITY_MULTIPLE_COM
#endif  //defined(GP_DIVERSITY_COM_SHMEM) || defined(GP_DIVERSITY_COM_USB)
#else
#endif

#define GP_COM_COMM_ID_HW       (0x80000000)
#define GP_COM_COMM_ID_HW_1     (0x10000)
#define GP_COM_COMM_ID_HW_2     (0x20000)
#define GP_COM_COMM_ID_HW_3     (0x40000)
#define GP_COM_COMM_ID_HW_4     (0x80000)
#define GP_COM_COMM_ID_UART1    (GP_COM_COMM_ID_HW | 0x01)
#define GP_COM_COMM_ID_UART2    (GP_COM_COMM_ID_HW | 0x02)
#define GP_COM_COMM_ID_PICUSB         (GP_COM_COMM_ID_HW | 0x0010)
#define GP_COM_COMM_ID_STMUSB         (GP_COM_COMM_ID_UART1)
#define GP_COM_COMM_ID_SHMEM          (GP_COM_COMM_ID_HW | 0x0020)
#define GP_COM_COMM_ID_IOCTL_USER1    (GP_COM_COMM_ID_HW | 0x0040)
#define GP_COM_COMM_ID_IOCTL_USER2    (GP_COM_COMM_ID_HW | 0x0080)
#define GP_COM_COMM_ID_IOCTL_KERN1    (GP_COM_COMM_ID_HW | 0x0100)
#define GP_COM_COMM_ID_IOCTL_KERN2    (GP_COM_COMM_ID_HW | 0x0200)
#ifndef GP_COM_COMM_ID_IOCTL_USER
#define GP_COM_COMM_ID_IOCTL_USER GP_COM_COMM_ID_IOCTL_USER1
#endif
#ifndef GP_COM_COMM_ID_IOCTL_KERN
#define GP_COM_COMM_ID_IOCTL_KERN GP_COM_COMM_ID_IOCTL_KERN1
#endif
#define GP_COM_COMM_ID_SOCKET   0xFFFFFFFF

//Define default COM ID
#ifdef GP_COM_DIVERSITY_MULTIPLE_COM
#ifndef GP_COM_DEFAULT_COMMUNICATION_ID
#error supply default comm ID
#endif
#else
#if   defined(GP_DIVERSITY_COM_SOCKET)
#define GP_COM_DEFAULT_COMMUNICATION_ID  0xFFFFFFFF
#else
//Default case
#ifndef GP_DIVERSITY_COM_UART
#define GP_DIVERSITY_COM_UART
#endif
#define GP_COM_DEFAULT_COMMUNICATION_ID  GP_COM_COMM_ID_UART1
#endif
#endif //GP_COM_DIVERSITY_MULTIPLE_COM

#if defined(GP_COMP_COM)
#define GP_COM_DATA_REQUEST(length,pData, commId)   gpCom_DataRequest(GP_MODULE_ID, (length), (pData), commId)
#if defined(GP_DIVERSITY_COM_UART) 
#define GP_COM_GET_FREE_BUFFER_SPACE(commId)        gpCom_GetFreeBufferSpace(GP_MODULE_ID, commId)
#endif
#define GP_COM_REGISTER_MODULE(handleCallback)      gpCom_RegisterModule(GP_MODULE_ID, (handleCallback))
#define GP_COM_REGISTER_ACTIVATE_TX_CB(activateCallback)      gpCom_RegisterActivateTxCb(GP_MODULE_ID, activateCallback)
#else
#define GP_COM_DATA_REQUEST(length,pData, commId)             do {} while (false)
#define GP_COM_REGISTER_MODULE(handleCallback)                do {} while (false)
#define GP_COM_REGISTER_ACTIVATE_TX_CB(activateCallback)      do {} while (false)
#endif

#ifndef GP_COM_MAX_PACKET_PAYLOAD_SIZE
#define GP_COM_MAX_PACKET_PAYLOAD_SIZE   127
#endif //GP_COM_MAX_PACKET_PAYLOAD_SIZE

//Length expressed in 12-bits - maximum 4096-1 bytes available
#define GP_COM_MAX_PACKET_PAYLOAD_SIZE_PROTOCOL 4095
#if GP_COM_MAX_PACKET_PAYLOAD_SIZE > GP_COM_MAX_PACKET_PAYLOAD_SIZE_PROTOCOL
#error Length maximum 12-bits = 4096-1 bytes
#endif

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt32 gpCom_CommunicationId_t;

typedef void (*gpCom_HandleCallback_t) ( UInt16 length , UInt8* pData, gpCom_CommunicationId_t commId);
typedef UInt16 (*gpCom_cbActivateTx_t) ( UInt16 overFlowCounter, gpCom_CommunicationId_t commId);

#define gpCom_ModeUart   0
#define gpCom_ModeShMem  1
#define gpCom_ModePicUsb 2
#define gpCom_ModeSocket 3
typedef UInt8 gpCom_Mode_t;

#define gpCom_ProtocolSyn       1
#define gpCom_ProtocolBle       2
#define gpCom_ProtocolInvalid   0xff
typedef UInt8 gpCom_Protocol_t;
/*****************************************************************************
 *                    External Function Definitions
 *****************************************************************************/

#if defined( GP_DIVERSITY_COM_UART )
//UART functions 
extern Bool gpComUart_GetTXEnable(void);
extern void gpComUart_HandleTx(void);
extern void gpComUart_Flush(void);
extern void gpComUart_DeInit(void);
#endif //defined( GP_DIVERSITY_COM_UART )



#ifdef GP_DIVERSITY_COM_SOCKET
extern Bool gpComSocket_GetTXEnable(void);
extern void gpComSocket_HandleTx(void);
extern void gpComSocket_Flush(void);
extern void gpComSocket_DeInit(void);
#endif //GP_DIVERSITY_COM_SOCKET


/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void gpCom_Init(void);
void gpCom_DeInit(void);
Bool gpCom_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId);
Bool gpCom_DataRequestAndWaitForAck(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId, Bool *reqAcked, UInt32 timeout, UInt8 ackId);
UInt16 gpCom_GetFreeBufferSpace(UInt8 moduleID, gpCom_CommunicationId_t commId);
Bool gpCom_RegisterModule(UInt8 moduleID, gpCom_HandleCallback_t handleCallback);
Bool gpCom_DeRegisterModule(UInt8 moduleID);
Bool gpCom_RegisterActivateTxCb(UInt8 moduleID, gpCom_cbActivateTx_t cb);
Bool gpCom_IsReceivedPacketPending(void);
void gpCom_HandleRx(void);
void gpCom_HandleRxBlocking(Bool blockingWait, UInt8 cmdId);

#if defined(GP_COM_DIVERSITY_MULTIPLE_COM) 
Bool gpCom_GetTXEnable(void);
void gpCom_HandleTx(void);
void gpCom_HandleRx(void);
void gpCom_Flush(void);
gpCom_Mode_t gpCom_cbSelectMode(void);
#elif defined(GP_DIVERSITY_COM_UART) 
#define gpCom_GetTXEnable()                                 gpComUart_GetTXEnable()
#define gpCom_HandleTx()                                    gpComUart_HandleTx()
#define gpCom_Flush()                                       gpComUart_Flush()
#define gpCom_DeInit()                                      gpComUart_DeInit()
#elif defined(GP_DIVERSITY_COM_SOCKET) 
#define gpCom_GetTXEnable()                                 gpComSocket_GetTXEnable()
#define gpCom_HandleTx()                                    gpComSocket_HandleTx()
#define gpCom_Flush()                                       gpComSocket_Flush()
#define gpCom_DeInit()                                      gpComSocket_DeInit()
#endif //COM_MODE

#if defined(GP_DIVERSITY_COM_SOCKET)
#endif //GP_DIVERSITY_COM_SOCKET


#ifdef __cplusplus
}
#endif

#endif // _GPCOM_H_

