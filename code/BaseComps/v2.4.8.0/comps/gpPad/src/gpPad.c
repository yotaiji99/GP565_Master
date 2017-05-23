/*
 * Copyright (c) 2013, GreenPeak Technologies
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPad/src/gpPad.c#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

//#define GP_LOCAL_LOG - set LOCAL_LOG in included .c files
#define GP_COMPONENT_ID GP_COMPONENT_ID_PAD

#include "gpPad.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Functional Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    External Data Definition
 *****************************************************************************/

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

/*****************************************************************************
 *                    Source Includes
 *****************************************************************************/

#include "gpPad_ram.c"

/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void gpPad_Init(void)
{
    Pad_Init();
}

gpPad_Handle_t gpPad_GetPad(gpPad_Attributes_t* pInitAttributes)
{
    return Pad_GetPad(pInitAttributes);
}

void gpPad_FreePad(gpPad_Handle_t padHandle)
{
    Pad_FreePad(padHandle);
}

gpPad_Result_t gpPad_CheckPadValid(gpPad_Handle_t padHandle)
{
    return Pad_CheckPadValid(padHandle);
}

void gpPad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    Pad_SetAttributes(padHandle, pAttributes);
}
void gpPad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    Pad_SetTxChannels(padHandle, channels);
}
void gpPad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower)
{
    Pad_SetTxPower(padHandle, txPower);
}
void gpPad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna)
{
    Pad_SetTxAntenna(padHandle, antenna);
}
void gpPad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE)
{
    Pad_SetTxMinBE(padHandle, minBE);
}
void gpPad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE)
{
    Pad_SetTxMaxBE(padHandle, maxBE);
}
void gpPad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs)
{
    Pad_SetTxMaxCsmaBackoffs(padHandle, maxCsmaBackoffs);
}
void gpPad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries)
{
    Pad_SetTxMaxFrameRetries(padHandle, maxFrameRetries);
}
void gpPad_SetTxCsmaMode(gpPad_Handle_t padHandle, UInt8 csma)
{
    Pad_SetTxCsmaMode(padHandle, csma);
}
void gpPad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 ccaMode)
{
    Pad_SetCcaMode(padHandle, ccaMode);
}

void gpPad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes)
{
    Pad_GetAttributes(padHandle, pAttributes);
}
void  gpPad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels)
{
    Pad_GetTxChannels(padHandle, channels);
}
Int8  gpPad_GetTxPower(gpPad_Handle_t padHandle)
{
    return Pad_GetTxPower(padHandle);
}
UInt8 gpPad_GetTxAntenna(gpPad_Handle_t padHandle)
{
    return Pad_GetTxAntenna(padHandle);
}
UInt8 gpPad_GetTxMinBE(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMinBE(padHandle);
}
UInt8 gpPad_GetTxMaxBE(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMaxBE(padHandle);
}
UInt8 gpPad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMaxCsmaBackoffs(padHandle);
}
UInt8 gpPad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle)
{
    return Pad_GetTxMaxFrameRetries(padHandle);
}
UInt8 gpPad_GetTxCsmaMode(gpPad_Handle_t padHandle)
{
    return Pad_GetTxCsmaMode(padHandle);
}
UInt8 gpPad_GetCcaMode(gpPad_Handle_t padHandle)
{
    return Pad_GetCcaMode(padHandle);
}

void gpPad_DataRequest(UInt8 pbmHandle, gpPad_Handle_t padHandle)
{
    Pad_DataRequest(pbmHandle, padHandle);
}

