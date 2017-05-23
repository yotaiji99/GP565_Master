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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpPad/inc/gpPad.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPPAD_H_
#define _GPPAD_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include <global.h>

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

#ifndef GP_PAD_NR_OF_HANDLES
#define GP_PAD_NR_OF_HANDLES    3
#endif //GP_PD_NR_OF_HANDLES

#define GP_PAD_INVALID_HANDLE   0xFF

#define GP_PAD_MAX_CHANNELS     3

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

typedef UInt8 gpPad_Handle_t;

#define gpPad_ResultValidHandle        0x0
#define gpPad_ResultInvalidHandle      0x1
#define gpPad_ResultNotInUse           0x2
typedef UInt8 gpPad_Result_t;

typedef struct gpPad_Attributes_s {
    UInt8 channels[GP_PAD_MAX_CHANNELS];
    UInt8 antenna;
    Int8  txPower;
    UInt8 minBE;
    UInt8 maxBE;
    UInt8 maxCsmaBackoffs;
    UInt8 maxFrameRetries;
    UInt8 csma;
    UInt8 cca;
} gpPad_Attributes_t;

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void    gpPad_Init(void);

//Descriptor handling
gpPad_Handle_t gpPad_GetPad(gpPad_Attributes_t* pInitAttributes);
void           gpPad_FreePad(gpPad_Handle_t padHandle);
gpPad_Result_t gpPad_CheckPadValid(gpPad_Handle_t padHandle);

//TxRequest
void gpPad_SetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);
void gpPad_GetAttributes(gpPad_Handle_t padHandle, gpPad_Attributes_t* pAttributes);

void gpPad_SetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
void gpPad_SetTxPower(gpPad_Handle_t padHandle, Int8 txPower);
void gpPad_SetTxAntenna(gpPad_Handle_t padHandle, UInt8 antenna);
void gpPad_SetTxMinBE(gpPad_Handle_t padHandle, UInt8 minBE);
void gpPad_SetTxMaxBE(gpPad_Handle_t padHandle, UInt8 maxBE);
void gpPad_SetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle, UInt8 maxCsmaBackoffs);
void gpPad_SetTxMaxFrameRetries(gpPad_Handle_t padHandle, UInt8 maxFrameRetries);
void gpPad_SetTxCsmaMode(gpPad_Handle_t padHandle, UInt8 csma);
void gpPad_SetCcaMode(gpPad_Handle_t padHandle, UInt8 cca);

void  gpPad_GetTxChannels(gpPad_Handle_t padHandle, UInt8* channels);
Int8  gpPad_GetTxPower(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxAntenna(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMinBE(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMaxBE(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMaxCsmaBackoffs(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxMaxFrameRetries(gpPad_Handle_t padHandle);
UInt8 gpPad_GetTxCsmaMode(gpPad_Handle_t padHandle);
UInt8 gpPad_GetCcaMode(gpPad_Handle_t padHandle);

//Internal helper functions
void gpPad_DataRequest(UInt8 pbmHandle, gpPad_Handle_t padHandle);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // _GPPD_H_


