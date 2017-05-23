/*
 * Copyright (c) 2008-2014, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpCom/src/gpCom.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_COM
#define GP_MODULE_ID GP_MODULE_ID_COM

#include "gpCom.h"
#include "gpCom_defs.h"

#include "gpLog.h"
#include "gpAssert.h"

#include "gpSched.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

Bool   gpCom_Initialized = false;

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/
 
gpCom_cbActivateTx_t gpCom_ActivateTxCb = NULL;

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpCom_Init(void)
{

#if defined(GP_DIVERSITY_COM_UART) 
    gpComUart_Init();
#endif //GP_DIVERSITY_COM_UART
#if defined(GP_DIVERSITY_COM_SOCKET)
    gpComSocket_Init();
#endif //GP_DIVERSITY_COM_SOCKET

    gpCom_ActivateTxCb = NULL;
}

Bool gpCom_DataRequest(UInt8 moduleID, UInt16 length, UInt8* pData, gpCom_CommunicationId_t commId)
{
#if defined(GP_DIVERSITY_COM_UART) 
    if((commId == GP_COM_COMM_ID_UART1) || (commId == GP_COM_COMM_ID_UART2) || (commId == GP_COM_COMM_ID_STMUSB))
    {
        return gpComUart_DataRequest(moduleID, length, pData, commId);
    }
    else
#endif
#if defined(GP_DIVERSITY_COM_SOCKET) 
    {
        //Handling all other commId numbers - socketIds are allocated by the socket code
        return gpComSocket_DataRequest(moduleID, length, pData, commId);
    }
#else
    {
        return false;
    }
#endif
}

UInt16 gpCom_GetFreeBufferSpace(UInt8 moduleID, gpCom_CommunicationId_t commId)
{
#if defined(GP_DIVERSITY_COM_UART) 
    if((commId == GP_COM_COMM_ID_UART1) || (commId == GP_COM_COMM_ID_UART2) || (commId == GP_COM_COMM_ID_STMUSB))
    {
        return gpComUart_GetFreeSpace(commId);
    }
    else
#endif
#if defined(GP_DIVERSITY_COM_SOCKET)
    GP_ASSERT_DEV_EXT(false);
#endif
    {
        return 0;
    }
}

#ifdef GP_COM_DIVERSITY_MULTIPLE_COM
//Redirection functions
Bool gpCom_GetTXEnable(void)
{
    Bool enabled = false;
    
#if defined(GP_DIVERSITY_COM_UART) 
    enabled |= gpComUart_GetTXEnable();
#endif
#if defined(GP_DIVERSITY_COM_SOCKET) 
    enabled |= gpComSocket_GetTXEnable();
#endif

    return enabled;
}

void gpCom_Flush(void)
{
#if defined(GP_DIVERSITY_COM_UART) 
    gpComUart_Flush();
#endif
#if defined(GP_DIVERSITY_COM_SOCKET) 
    gpComSocket_Flush();
#endif
}

void gpCom_DeInit(void)
{

#if defined(GP_DIVERSITY_COM_UART) 
    gpComUart_DeInit();
#endif
#if defined(GP_DIVERSITY_COM_SOCKET) 
    gpComSocket_DeInit();
#endif
}

void gpCom_HandleTx(void)
{
#if defined(GP_DIVERSITY_COM_UART) 
    gpComUart_HandleTx();
#endif
#if defined(GP_DIVERSITY_COM_SOCKET) 
    gpComSocket_HandleTx();
#endif
}

#endif //GP_COM_DIVERSITY_MULTIPLE_COM


Bool gpCom_RegisterActivateTxCb(UInt8 moduleID, gpCom_cbActivateTx_t cb)
{
    gpCom_ActivateTxCb = cb;
    return true;
}
