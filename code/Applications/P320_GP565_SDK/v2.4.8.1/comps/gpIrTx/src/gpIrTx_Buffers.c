/*
 * Copyright (c) 2012-2014, GreenPeak Technologies
 *
 *   This file contains the implementation for sending generic IR codes (IR database format)
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
 *     0!                         $Header: //depot/release/Embedded/Applications/P320_GP565_SDK/v2.4.8.1/comps/gpIrTx/src/gpIrTx_Buffers.c#1 $
 *    M'   GreenPeak              $Change: 77946 $
 *   0'         Technologies      $DateTime: 2016/03/03 15:31:03 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/
//#define GP_LOCAL_LOG
#define GP_COMPONENT_ID GP_COMPONENT_ID_IRTX

#include "hal.h"
#include "gpBsp.h"
#include "gpLog.h"
#include "gpAssert.h"
#include "gpSched.h"

#include "gpIrTx.h"
#include "gpIrTx_Internal.h"

#include "hal_WB.h"

#if GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
#include "gpHal.h"
#include "gpHal_kx_public.h"
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                   Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

#ifndef IRTX_NUM_BUFFERS
#define IRTX_NUM_BUFFERS  2
#endif //IRTX_NUM_BUFFERS

#if GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
#define IRTX_NR_PBMS_PER_IR_BUFFER ((IRTX_BUFFER_SIZE_IN_WORDS * 2 / GPHAL_MAX_PBM_LENGTH ) + 1 )
#if GPHAL_NUMBER_OF_PBMS_USED < (IRTX_NUM_BUFFERS * IRTX_NR_PBMS_PER_IR_BUFFER)
#error "not enough PBMs for the IR buffers"
#endif
#endif //GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY

#if GP_DIVERSITY_GPHAL_REUSE_PBM_MEMORY
static UInt16* IrTx_Buffer = (UInt16 *)GP_HAL_PBM_ENTRY2ADDR(0);
#else
static UInt16 IrTx_Buffer[IRTX_BUFFER_SIZE_IN_WORDS * IRTX_NUM_BUFFERS];
#endif
static volatile UInt8  IrTx_BuffersTriggered;
static UInt8  IrTx_NumEntries;
static UInt8  IrTx_CurrentBuffer;

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

static void IrTx_IntCallback(halIr_InterruptId_t id);

/*****************************************************************************
 *                    Function Definitions
 *****************************************************************************/
void IrTx_InitBuffers(void)
{
    GP_LOG_PRINTF("gpIrTx_BuffersInit",0);
    hal_RegisterIrIntCallback(IrTx_IntCallback);
    hal_ConfigureTimeMode(GP_WB_ENUM_IR_TIME_UNIT_TU_1US);
    IrTx_BuffersTriggered = 0;
    IrTx_NumEntries       = 0;
    IrTx_CurrentBuffer    = 0;
}

void IrTx_SetupNextBuffer(void)
{
    GP_LOG_PRINTF("IrTx_SetupNextBuffer",0);
    IrTx_CurrentBuffer = (IrTx_CurrentBuffer+1) % IRTX_NUM_BUFFERS;
    IrTx_NumEntries       = 0;
}

UInt16* IrTx_GetCurrentBuffer(void)
{
    return &(IrTx_Buffer[IRTX_BUFFER_SIZE_IN_WORDS*IrTx_CurrentBuffer]);
}

void IrTx_IntCallback(halIr_InterruptId_t id)
{
    GP_LOG_PRINTF("IrTx_IntCallback %d",0, IrTx_BuffersTriggered);
    IrTx_SetupNextBuffer();

    switch(id)
    {
        case halIr_InterruptIdStart :
            IrTx_cbIrStartOfPattern();
            break;
        case halIr_InterruptIdRepeat:
            IrTx_cbIrStartOfPattern();
            if(IrTx_BuffersTriggered > 1)
            {
                IrTx_BuffersTriggered--;
            }
            break;
        case halIr_InterruptIdDone  :
            IrTx_BuffersTriggered--;
            break;
        default:
            break;
    }
}

void IrTx_TriggerIR(void)
{
    UInt16* pCurrentIrTxBuffer = IrTx_GetCurrentBuffer();

    //New buffer triggered to send
    IrTx_BuffersTriggered++;

    hal_PlaySequenceFromRam(pCurrentIrTxBuffer, IrTx_NumEntries);
}

void IrTx_DisableIRCarrier(void)
{
    while(IrTx_BuffersTriggered)
    {
        //Keep repeat on untill only last buffer is busy
        if(IrTx_BuffersTriggered < 2)
        {
            hal_DisableIRCarrier();
        }
    }
}

void IrTx_WriteEntry(UInt16 entry)
{
    UInt16* pCurrentIrTxBuffer = IrTx_GetCurrentBuffer();
    // Skip first Set/Clr pair if first Set value is 0
    if ((IrTx_NumEntries == 1) && pCurrentIrTxBuffer[0] == 0)
    {
        IrTx_NumEntries = 0;
    }
    else
    {
        pCurrentIrTxBuffer[IrTx_NumEntries] = entry;
        IrTx_NumEntries++;
    }
}

void IrTx_SetClr(UInt32 timeSet, UInt32 timeClr) /*In 1us*/
{
    GP_ASSERT_DEV_EXT((timeSet != 0) || (timeClr != 0));

    if(IrTx_BuffersTriggered >= IRTX_NUM_BUFFERS)
    {
        GP_ASSERT_DEV_EXT(false);
    }

    //Still space for 2 entries more ?
    GP_ASSERT_DEV_EXT(IrTx_NumEntries <= (0xFF - 2));

    //Build up sequence in RAM
    GP_LOG_PRINTF("entry n:%i s:%lu c:%lu",0,IrTx_NumEntries, timeSet, timeClr);
    //Expand timing > 0xFFFF us in separate entries
    while(timeSet > 0xFFFF)
    {
        IrTx_WriteEntry(0xFFFF); //Set
        IrTx_WriteEntry(0x0000); //Clr
        timeSet -= 0xFFFF;
    }
    IrTx_WriteEntry(timeSet & 0xFFFF); //Set

    //Expand timing > 0xFFFF us in separate entries
    while(timeClr > 0xFFFF)
    {
        IrTx_WriteEntry(0xFFFF); //Clr
        IrTx_WriteEntry(0x0000); //Set
        timeClr -= 0xFFFF;
    }
    IrTx_WriteEntry(timeClr & 0xFFFF); //Clr

    //Update length
    GP_ASSERT_DEV_EXT( IrTx_NumEntries <= IRTX_BUFFER_SIZE_IN_WORDS ); //Buffer too small
}
