/*
 * Copyright (c) 2011-2014, GreenPeak Technologies
 *
 * This file contains the internal defines, typedefs,... of the NVM component
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
 *     0!                         $Header: //depot/release/Embedded/BaseComps/v2.4.8.0/sw/comps/gpNvm/src/gpNvm_defs.h#1 $
 *    M'   GreenPeak              $Change: 75614 $
 *   0'         Technologies      $DateTime: 2016/01/14 12:38:05 $
 *  F
 */

#ifndef _GPNVM_DEFS_H_
#define _GPNVM_DEFS_H_

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "global.h"
#include "gpNvm.h"

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

extern const UIntPtr gpNvm_NvmBaseAddr;

/*****************************************************************************
 *                    Public Function Declarations
 *****************************************************************************/

void Nvm_Init(void);
void Nvm_DeInit(void);
UInt16 Nvm_GetMaxSize(void);
void Nvm_Flush(void);
void Nvm_SetI2cDev(UInt16 devAddr, UInt8 pageSize);
void Nvm_RestoreI2cDev(void);
void Nvm_WriteByte(UIntPtr address, UInt8 value, gpNvm_UpdateFrequency_t updateFrequency);
UInt8 Nvm_ReadByte(UIntPtr address, gpNvm_UpdateFrequency_t updateFrequency);
Bool Nvm_WriteBlock(UIntPtr address, UInt16 length, UInt8* txBuffer, gpNvm_UpdateFrequency_t updateFrequency);
void Nvm_ReadBlock(UIntPtr address, UInt16 length, UInt8* rxBuffer, gpNvm_UpdateFrequency_t updateFrequency);
void Nvm_Erase(void);

void Nvm_WriteTag(UInt8 tagId, UInt16 size, UInt8* pData, gpNvm_UpdateFrequency_t updateFrequency);
void Nvm_ReadTag(UInt8 tagId, UInt16 size, UInt8* pData, gpNvm_UpdateFrequency_t updateFrequency);
UInt8 Nvm_GetSectionBaseTag(UInt8 nbrOfTags);
Bool Nvm_CheckAccessible(void);

Bool Nvm_Invalidate(void);


#endif //_GPNVM_DEFS_H_
